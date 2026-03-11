
//===- SchedulingIO.cpp - Support for input/output files managing  -----------*-
// C++
//-*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares support for input/output files managing for static
// scheduling
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Transforms/StaticScheduling/SchedulingIO.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"

namespace dynamatic {
// Define the method to print scheduled graph in dot format
void writeScheduledDOTPerBlock(
    const llvm::MapVector<Operation *, unsigned> &schedule, ModuleOp moduleOp,
    std::string &filename, const StaticTimingConfig &timingConfig) {

  std::error_code ec;
  llvm::raw_fd_ostream fileStream(filename, ec);
  if (ec) {
    llvm::errs() << "Error opening file " << filename
                 << " for writing: " << ec.message() << "\n";
    return;
  }
  mlir::raw_indented_ostream os(fileStream);
  // Replace non-alphanumeric characters with underscores so that
  // operation names are valid DOT node identifiers.
  auto dotEscape = [](StringRef name) -> std::string {
    std::string s = name.str();
    for (char &c : s)
      if (!isalnum(c))
        c = '_';
    return s;
  };
  // DOT node identifier for a function input argument: blockarg_<argNum>.
  // Block arguments only exist in BB0.
  auto getBlockArgNodeName = [&](unsigned argNum) -> std::string {
    return dotEscape("blockarg_" + std::to_string(argNum));
  };

  // Orange highlights boolean edges so control flow decisions are
  // visually traceable from comparison to branch.
  // Purple highlights start signal edges. The i0 type identifies the
  // start signal after SSA maximization threads it through phis and
  // branches, which is why the check is on type rather than on
  // cfx::StartOp.
  auto getEdgeColor = [](Operation *defOp, Value operand) -> std::string {
    if (isa<mlir::arith::CmpIOp, mlir::arith::CmpFOp>(defOp))
      return "orange";
    if (operand.getType().isInteger(0))
      return "purple";
    return "";
  };
  os << "digraph ScheduledGraph {\n";
  os.indent();
  // newrank=true tells Graphviz to honor rank constraints inside subgraph
  // clusters (the default mode ignores them).
  os << "newrank=true;\n";
  // Horizontal spacing between nodes (inches)
  os << "nodesep=0.5;\n";
  // Vertical spacing between ranks (inches)
  os << "ranksep=0.75;\n";
  os << "node [shape=box];\n";
  // Double the default edge thickness for visibility at typical zoom levels
  os << "edge [penwidth=2];\n\n";
  // For each function in the module
  for (cfx::FuncOp funcOp : moduleOp.getOps<cfx::FuncOp>()) {
    llvm::SetVector<std::pair<Operation *, Operation *>> backEdges =
        funcOp.getSelfLoopBackEdgesInCDFG();
    auto cfxBlocks = funcOp.getCFXBasicBlocks();

    // A cross-BB edge routed through blue dot nodes.
    // Forward edges (3 DOT edges):
    //   src -> bottomDot -> topDot -> dst
    //   bottomDot in source BB (rank=max), topDot in dest BB (rank=min).
    // Back edges (4 DOT edges):
    //   src -> bottomDot, outerDot -> bottomDot,
    //   topDot -> outerDot, topDot -> dst
    //   bottomDot in BB (rank=max), topDot in BB (rank=min).
    //   outerDot is outside all subgraphs (invisible).
    struct CrossBBEdge {
      std::string srcName;
      std::string dstName;
      std::string topDotName;
      std::string bottomDotName;
      std::string outerDotName; // invisible, outside all subgraphs
      int64_t srcBB;
      int64_t dstBB;
      bool isBackEdge;
    };

    // The visualizer assumes every operation in every CFX basic block has
    // been scheduled. Verify this upfront so that later code can use
    // schedule.lookup without checking.
    for (const auto &block : cfxBlocks) {
      for (Operation *op : block) {
        if (!schedule.contains(op))
          llvm::report_fatal_error(
              "writeScheduledDOTPerBlock: operation '" + getUniqueName(op) +
              "' in BB" + std::to_string(block.getID()) +
              " is not in the schedule map. "
              "The visualizer assumes all operations are scheduled.");
      }
    }

    // Pass 1: Identify cross-BB edges and allocate routing dot nodes.
    // This must happen before pass 2 because dot node declarations must
    // appear inside the correct subgraph before edges can reference them.
    // Forward and back edges stored separately because they use
    // different routing: forward edges have 3 DOT edges, back edges have 4.
    SmallVector<CrossBBEdge> forwardEdges;
    SmallVector<CrossBBEdge> backEdgeCrossings;

    // Dot names grouped by BB so that pass 2 can declare and
    // rank-group dots by iterating the list for the current BB.
    // Top dots live in the destination BB's subgraph, bottom dots
    // live in the source BB's subgraph.
    DenseMap<int64_t, SmallVector<std::string>> forwardTopDots;
    DenseMap<int64_t, SmallVector<std::string>> forwardBotDots;
    DenseMap<int64_t, SmallVector<std::string>> backTopDots;
    DenseMap<int64_t, SmallVector<std::string>> backBotDots;
    // Routing dots are named dot_0, dot_1, ... with a globally unique
    // counter. Each forward cross-BB edge allocates 2 dots (top and
    // bottom), each back edge allocates 3 (top, bottom, and outer).
    unsigned dotCounter = 0;

    for (const auto &block : cfxBlocks) {
      int64_t bbID = block.getID();

      // For each operand whose defining op is in a different BB,
      // create a forward cross-BB edge with two routing dots.
      for (Operation *op : block) {
        std::string dst = dotEscape(getUniqueName(op));
        for (Value operand : op->getOperands()) {
          if (auto *defOp = operand.getDefiningOp()) {
            if (funcOp.getBB(defOp) != bbID) {
              std::string src = dotEscape(getUniqueName(defOp));
              std::string topDot = "dot_" + std::to_string(dotCounter++);
              std::string bottomDot = "dot_" + std::to_string(dotCounter++);
              int64_t srcBBID = funcOp.getBB(defOp);
              forwardEdges.push_back(
                  {src, dst, topDot, bottomDot, "", srcBBID, bbID, false});
              // We group dot names by BB so pass 2 can declare each dot
              // inside the correct subgraph.
              // Top dot goes in the destination BB, bottom dot in the
              // source BB, because that is where they appear visually.
              forwardTopDots[bbID].push_back(topDot);
              forwardBotDots[srcBBID].push_back(bottomDot);
            }
          }
        }
      }
    }

    // Back edges (from getSelfLoopBackEdgesInCDFG) are self-loops: a branch
    // feeding a phi in the same BB. Each gets three routing dots: top,
    // bottom, and an invisible outer dot that sits outside all subgraphs
    // so the edge visually exits and re-enters the cluster boundary.
    for (auto &edge : backEdges) {
      Operation *srcOp = edge.first;
      Operation *dstOp = edge.second;
      int64_t bbID = funcOp.getBB(srcOp);
      std::string src = dotEscape(getUniqueName(srcOp));
      std::string dst = dotEscape(getUniqueName(dstOp));
      std::string topDot = "dot_" + std::to_string(dotCounter++);
      std::string bottomDot = "dot_" + std::to_string(dotCounter++);
      std::string outerDot = "dot_" + std::to_string(dotCounter++);
      backEdgeCrossings.push_back(
          {src, dst, topDot, bottomDot, outerDot, bbID, bbID, true});
      // We group dot names by BB so pass 2 can declare each dot
      // inside the correct subgraph.
      // Both top and bottom are in the same BB because back edges
      // are self-loops.
      // The outer dot is not in a per-BB list because it is declared
      // outside all subgraphs.
      backTopDots[bbID].push_back(topDot);
      backBotDots[bbID].push_back(bottomDot);
    }

    // Pass 2: Emit each BB as a subgraph with its nodes, routing dots,
    // rank groupings, and intra-BB edges.
    for (auto &block : cfxBlocks) {
      int64_t bbID = block.getID();
      // Start subgraph for the basic block
      os << "subgraph cluster_" << funcOp.getName() << "_bb" << bbID << " {\n";
      // Check if the block has an II attribute for pipelined scheduling
      std::optional<unsigned> ii = funcOp.getInitiationIntervalForBlock(bbID);
      os.indent();
      if (ii.has_value() && ii.value() > 0) {
        os << "label=\"" << funcOp.getName() << " :: BB#" << bbID
           << " (II=" << ii.value() << ")\";\n";
      } else {
        os << "label=\"" << funcOp.getName() << " :: BB#" << bbID << "\";\n";
      }
      os << "style=rounded;\n\n";

      if (bbID == 0) {
        // Draw non-MemRef function input arguments as nodes in BB0 at
        // time 0. MemRef arguments are skipped because they represent
        // pointer-typed memory connections, not scheduled data values.
        for (BlockArgument &arg : funcOp.getBody().front().getArguments()) {
          if (isa<MemRefType>(arg.getType()))
            continue;
          std::string nodeName = getBlockArgNodeName(arg.getArgNumber());
          os << nodeName << " [label=\"" << nodeName << "\\n@0\"];\n";
        }
        os << "\n";
      }

      // Node names that need rank constraints to control vertical placement.
      // We fill these lists when writing the nodes out, and then later use
      // them to build rank groupings.
      SmallVector<std::string> phiNodeNames;
      SmallVector<std::string> branchesUpperRank;
      SmallVector<std::string> branchesLowerRank;
      // Any branch name from each rank, used for invisible ordering
      // edges that connect stores to branches and branches to bottom
      // dots. Which specific branch does not matter, we just need one
      // from each rank to anchor the ordering.
      std::optional<std::string> anyUpperBranch;
      std::optional<std::string> anyLowerBranch;
      // Write operation nodes inside the BB's subgraph so Graphviz
      // displays them in the correct cluster.
      for (Operation *op : block) {
        std::string nodeName = dotEscape(getUniqueName(op));
        auto latency = timingConfig.getLatency(op);
        double lat = latency.value_or(0.0);
        os << nodeName << " [label=\"" << getUniqueName(op) << "\\n@"
           << schedule.lookup(op) << " (lat="
           << static_cast<unsigned>(lat) << ")\"];\n";
        if (isa<cfx::PhiOp>(op))
          phiNodeNames.push_back(nodeName);
        // We split branches into two ranks so that when_true and when_false
        // branches sit on separate horizontal rows in the visualization.
        // Unconditional branches go in the upper rank with when_false because
        // they have no polarity and need to be grouped somewhere.
        if (auto condBr = dyn_cast<cfx::CondBranchOp>(op)) {
          if (condBr.getPolarity() == cfx::BranchPolarity::when_true) {
            branchesLowerRank.push_back(nodeName);
            // Record a representative from this rank for ordering edges.
            anyLowerBranch = nodeName;
          } else {
            branchesUpperRank.push_back(nodeName);
            // Record a representative from this rank for ordering edges.
            anyUpperBranch = nodeName;
          }
        }
        if (isa<cfx::BranchOp>(op)) {
          branchesUpperRank.push_back(nodeName);
          // Record a representative from this rank for ordering edges.
          anyUpperBranch = nodeName;
        }
      }

      // Rank grouping controls vertical placement within the cluster.
      // Top to bottom: forward top dots (rank=min), back edge top dots,
      // phis, computation ops, upper branch rank (when_false +
      // unconditional), lower branch rank (when_true), back edge bottom
      // dots, forward bottom dots (rank=max).
      // Invisible edges with minlen=2 enforce separation between groups.

      // Forward top dots: entry points for edges arriving from other BBs.
      // {rank=min; ...} tells Graphviz to place these nodes at the
      // topmost rank in this subgraph.
      if (!forwardTopDots[bbID].empty()) {
        os << "{rank=min;";
        for (const auto &dot : forwardTopDots[bbID])
          os << " " << dot << " [shape=point, width=0.08, color=blue];";
        os << "}\n";
      }
      // Back edge top dots: entry points for self-loop back edges.
      // {rank=same; ...} groups them on one horizontal row, placed
      // below the forward top dots by the invisible ordering edge.
      if (!backTopDots[bbID].empty()) {
        os << "{rank=same;";
        for (const auto &dot : backTopDots[bbID])
          os << " " << dot << " [shape=point, width=0.08, color=blue];";
        os << "}\n";
        // Invisible edge pushes back edge top dots below forward top dots.
        if (!forwardTopDots[bbID].empty()) {
          os << forwardTopDots[bbID][0] << " -> " << backTopDots[bbID][0]
             << " [style=invis, minlen=2];\n";
        }
      }
      // Phi nodes: placed on one row below the back edge top dots.
      if (!phiNodeNames.empty()) {
        os << "{rank=same;";
        for (const auto &name : phiNodeNames)
          os << " " << name << ";";
        os << "}\n";
        // Phis already receive edges from back edge top dots, but
        // minlen=2 enforces vertical spacing between the ranks.
        if (!backTopDots[bbID].empty()) {
          os << backTopDots[bbID][0] << " -> " << phiNodeNames[0]
             << " [style=invis, minlen=2];\n";
        }
      }
      // Upper branch rank: when_false conditional branches and
      // unconditional branches, grouped on one row.
      if (!branchesUpperRank.empty()) {
        os << "{rank=same;";
        for (const auto &name : branchesUpperRank)
          os << " " << name << ";";
        os << "}\n";
      }
      // Lower branch rank: when_true conditional branches,
      // placed on their own row below the upper branch rank.
      if (!branchesLowerRank.empty()) {
        os << "{rank=same;";
        for (const auto &name : branchesLowerRank)
          os << " " << name << ";";
        os << "}\n";
      }
      // Stores have no data-flow users in the same BB, so without this
      // edge Graphviz would place them at any rank. We add an invisible
      // edge from each store to a branch to push branches below stores.
      // We prefer the upper rank since it sits closest to the stores
      // above, falling back to lower if there is no upper rank.
      {
        auto &topBranch = anyUpperBranch ? anyUpperBranch : anyLowerBranch;
        if (topBranch) {
          for (Operation *op : block) {
            if (isa<mlir::memref::StoreOp>(op)) {
              os << dotEscape(getUniqueName(op)) << " -> " << *topBranch
                 << " [style=invis, minlen=3];\n";
            }
          }
        }
      }
      // Invisible edge to order upper branch rank above lower branch rank.
      if (anyUpperBranch && anyLowerBranch) {
        os << *anyUpperBranch << " -> " << *anyLowerBranch
           << " [style=invis];\n";
      }
      // Invisible edge to push back edge bottom dots below branches.
      // We prefer the lower rank since it sits closest to the bottom
      // dots, falling back to upper if there is no lower rank.
      {
        auto &bottomBranch = anyLowerBranch ? anyLowerBranch : anyUpperBranch;
        if (bottomBranch && !backBotDots[bbID].empty()) {
          os << *bottomBranch << " -> " << backBotDots[bbID][0]
             << " [style=invis, minlen=2];\n";
        }
      }
      // Bottom dots: exit points for edges leaving this BB.
      // Back edge bottom dots sit on their own rank above the forward
      // bottom dots. Forward bottom dots use rank=max to anchor them
      // at the very bottom of the cluster.

      // Back edge bottom dots: grouped on one row above forward bottom dots.
      if (!backBotDots[bbID].empty()) {
        os << "{rank=same;";
        for (const auto &dot : backBotDots[bbID])
          os << " " << dot << " [shape=point, width=0.08, color=blue];";
        os << "}\n";
      }
      // Forward bottom dots: rank=max anchors them at the very bottom.
      // {rank=max; ...} tells Graphviz to place these nodes at the
      // bottommost rank in this subgraph.
      if (!forwardBotDots[bbID].empty()) {
        os << "{rank=max;";
        for (const auto &dot : forwardBotDots[bbID])
          os << " " << dot << " [shape=point, width=0.08, color=blue];";
        os << "}\n";
        // Invisible edge pushes back edge bottom dots above forward
        // bottom dots.
        if (!backBotDots[bbID].empty()) {
          os << backBotDots[bbID][0] << " -> " << forwardBotDots[bbID][0]
             << " [style=invis, minlen=2];\n";
        }
      }

      os.unindent();
      os << "\n}\n\n";

      // Emit intra-BB data-dependency edges with color coding by type.
      // Cross-BB edges are drawn separately through blue routing dots.
      for (Operation *op : block) {
        std::string dst = dotEscape(getUniqueName(op));
        for (Value operand : op->getOperands()) {
          if (auto *defOp = operand.getDefiningOp()) {
            // Back edges (from getSelfLoopBackEdgesInCDFG) are emitted
            // separately with dashed blue lines through routing dots.
            if (backEdges.contains({defOp, op}))
              continue;
            // Only draw edges within the same BB here
            if (funcOp.getBB(defOp) == bbID) {
              std::string src = dotEscape(getUniqueName(defOp));
              std::string color = getEdgeColor(defOp, operand);
              if (color.empty())
                os << src << " -> " << dst << ";\n";
              else
                os << src << " -> " << dst << " [color=" << color << "];\n";
            }
          } else if (auto blockArg = operand.dyn_cast<BlockArgument>()) {
            // Block arguments are function inputs declared as nodes
            // in BB0. We draw an edge from the argument node to this
            // op, skipping memrefs because they are pointer-typed
            // memory connections, not data edges.
            if (isa<MemRefType>(blockArg.getType()))
              continue;
            std::string src = getBlockArgNodeName(blockArg.getArgNumber());
            os << src << " -> " << dst << ";\n";
          }
        }
      }

      // Emit intra-BB memory-dependency edges with color coding by type.
      for (auto [src, dst, distance] : block.getMemoryDependencies()) {
        std::string srcName = dotEscape(getUniqueName(src));
        std::string dstName = dotEscape(getUniqueName(dst));
        // Add an edge between src and dst labeled with the distance.
        os << srcName << " -> " << dstName << " [label=\"memdep<" << distance
           << ">\", style=dashed];\n";
      }

      os << "\n";
    }

    // Declare outer routing dots for back edges. These must be outside all
    // subgraph clusters so Graphviz places them between cluster boundaries,
    // making the back edge visually exit and re-enter the cluster.
    for (const auto &e : backEdgeCrossings) {
      os << e.outerDotName
         << " [shape=point, width=0, height=0, style=invis];\n";
    }

    // Emit forward cross-BB edges through blue routing dots.
    // 3 DOT edges: src -> bottomDot -> topDot -> dst
    for (const auto &e : forwardEdges) {
      // DOT edge 1: source op down to bottom dot in source BB
      os << e.srcName << " -> " << e.bottomDotName << " [color=blue];\n";
      // DOT edge 2: bottom dot to top dot across BBs (minlen=4 for spacing)
      os << e.bottomDotName << " -> " << e.topDotName
         << " [color=blue, minlen=4];\n";
      // DOT edge 3: top dot down to destination op in destination BB
      os << e.topDotName << " -> " << e.dstName << " [color=blue];\n";
    }

    // Emit back edges through blue routing dots with dashed lines.
    // 4 DOT edges: src -> bottomDot, outerDot -> bottomDot,
    //   topDot -> outerDot, topDot -> dst
    // All DOT edges point higher-to-lower rank so Graphviz does not
    // rearrange the layout. arrowhead=none on intermediate DOT edges
    // prevents double arrows.
    for (const auto &e : backEdgeCrossings) {
      // DOT edge 1: source op down to bottom dot inside the cluster
      os << e.srcName << " -> " << e.bottomDotName
         << " [color=blue, style=dashed];\n";
      // DOT edge 2: outer dot down to bottom dot (exits the cluster)
      os << e.outerDotName << " -> " << e.bottomDotName
         << " [color=blue, style=dashed, arrowhead=none];\n";
      // DOT edge 3: top dot down to outer dot (enters the cluster)
      os << e.topDotName << " -> " << e.outerDotName
         << " [color=blue, style=dashed, arrowhead=none];\n";
      // DOT edge 4: top dot down to destination phi
      os << e.topDotName << " -> " << e.dstName
         << " [color=blue, style=dashed];\n";
    }
    os << "\n";
  }
  os.unindent();
  os << "}\n";
  os.flush();
}

// Define the method to print the scheduling results in a graphical format
// Example output with two basic blocks where the second block is pipelined
// with II = 7:
// === Scheduling Result ===
// Function: fir
//   Basic Block #0
//    -----------+---
//      Operation| 0
//    -----------+---
//    arg_0      | -
//    arg_1      | -
//    constant0  | -
//    br0        | -
//    -----------+---
//
//   Basic Block #0
//     (Initiation Interval: 7)
//    -------------+---+---+---+---+---+---+---
//        Operation| 0 | 1 | 2 | 3 | 4 | 5 | 6
//    -------------+---+---+---+---+---+---+---
//    arg_0        | - |   |   |   |   |   |
//    arg_1        | - |   |   |   |   |   |
//    constant4    | - |   |   |   |   |   |
//    constant5    | - |   |   |   |   |   |
//    muli0        | X | X | X | X |   |   |
//    addi0        |   |   |   |   | X |   |
//    addi1        |   |   |   |   |   | X |
//    -------------+---+---+---+---+---+---+---
// === End of Scheduling Result ===
//
void printSchedulingResult(mlir::raw_indented_ostream &os,
                           llvm::MapVector<Operation *, unsigned> &schedule,
                           ModuleOp moduleOp, const StaticTimingConfig &timingConfig) {

  os << "\n=== Scheduling Result ===\n";
  for (cfx::FuncOp funcOp : moduleOp.getOps<cfx::FuncOp>()) {
    os << "Function: " << funcOp.getName() << "\n";
    for (const auto &block : funcOp.getCFXBasicBlocks()) {
      int64_t bbID = block.getID();
      os << "  Basic Block #" << bbID << "\n";
      // Check if the block has an II attribute for pipelined scheduling
      std::optional<unsigned> ii = funcOp.getInitiationIntervalForBlock(bbID);
      if (ii.has_value() && ii.value() > 0) {
        os << "    (Initiation Interval: " << ii.value() << ")\n";
      }
      // Collect scheduled operations in this block
      SmallVector<Operation *> ops;
      unsigned maxTime = 0;
      size_t maxNameWidth = 0;
      for (Operation *op : block) {
        auto *it = schedule.find(op);
        if (it == schedule.end())
          continue;
        // Find the maximum scheduling time and maximum name width
        ops.push_back(op);
        maxTime = std::max(maxTime, it->second);
        maxNameWidth = std::max(maxNameWidth, getUniqueName(op).size());
      }
      if (ops.empty()) {
        os << "    (no scheduled operations)\n\n";
        continue;
      }
      // Print scheduling table
      const unsigned nameColWidth = maxNameWidth + 2;
      const unsigned timeColWidth = 3;

      // Print separator line for a chart with the given time range
      auto printSeparator = [&](unsigned colWidth, unsigned chartMaxTime) {
        os << "    " << std::string(colWidth, '-');
        for (unsigned t = 0; t <= chartMaxTime; ++t)
          os << "+" << std::string(timeColWidth, '-');
        os << "+\n";
      };

      // Print column header row for a chart with the given time range
      auto printHeader = [&](unsigned colWidth, unsigned chartMaxTime) {
        printSeparator(colWidth, chartMaxTime);
        os << "    " << llvm::right_justify("Operation", colWidth);
        for (unsigned t = 0; t <= chartMaxTime; ++t)
          os << "|" << llvm::center_justify(std::to_string(t), timeColWidth);
        os << "|\n";
        printSeparator(colWidth, chartMaxTime);
      };

      // Print an iteration separator label (e.g. "--- Iteration 1 (offset +2) ---")
      auto printIterationLabel = [&](unsigned colWidth, unsigned chartMaxTime,
                                     unsigned iter, unsigned offset) {
        std::string label = "--- Iteration " + std::to_string(iter) +
                            " (offset +" + std::to_string(offset) + ") ";
        unsigned totalWidth =
            colWidth + (chartMaxTime + 1) * (timeColWidth + 1) + 1;
        if (label.size() < totalWidth)
          label += std::string(totalWidth - label.size(), '-');
        os << "    " << label << "\n";
      };

      // Print block argument and operation rows with a time offset
      auto printRows = [&](unsigned colWidth, unsigned chartMaxTime,
                           unsigned timeOffset) {
        if (bbID == 0) {
          for (BlockArgument &arg : funcOp.getBody().front().getArguments()) {
            os << "    "
               << llvm::left_justify(
                      "arg_" + std::to_string(arg.getArgNumber()), colWidth);
            for (unsigned t = 0; t <= chartMaxTime; ++t) {
              std::string mark = (t == timeOffset) ? "-" : "";
              os << "|" << llvm::center_justify(mark, timeColWidth);
            }
            os << "|\n";
          }
        }
        for (Operation *op : ops) {
          unsigned time = schedule.lookup(op) + timeOffset;
          auto latencyOpt = timingConfig.getLatency(op);
          double latency = latencyOpt.value_or(0.0);
          bool isImmediate = (latency == 0.0);
          os << "    " << llvm::left_justify(getUniqueName(op), colWidth);
          for (unsigned t = 0; t <= chartMaxTime; ++t) {
            std::string mark;
            if (isImmediate) {
              mark = (t == time) ? "-" : "";
            } else {
              mark = (t >= time && t < time + static_cast<unsigned>(latency))
                         ? "X"
                         : "";
            }
            os << "|" << llvm::center_justify(mark, timeColWidth);
          }
          os << "|\n";
        }
      };

      // Original single-iteration chart
      printHeader(nameColWidth, maxTime);
      printRows(nameColWidth, maxTime, 0);
      printSeparator(nameColWidth, maxTime);
      os << "\n";

      // Pipelined view: show 3 overlapping iterations
      if (ii.has_value() && ii.value() > 0) {
        unsigned iiVal = ii.value();
        unsigned pipeMaxTime = maxTime + 2 * iiVal;

        os << "    Pipelined view (3 iterations, II=" << iiVal << ")\n";
        printHeader(nameColWidth, pipeMaxTime);

        for (unsigned iter = 0; iter < 3; ++iter) {
          unsigned offset = iter * iiVal;
          if (iter > 0)
            printIterationLabel(nameColWidth, pipeMaxTime, iter, offset);
          printRows(nameColWidth, pipeMaxTime, offset);
        }
        printSeparator(nameColWidth, pipeMaxTime);
        os << "\n";
      }
    }
    os << "=== End of Scheduling Result ===\n\n";
  }
}

} // namespace dynamatic
