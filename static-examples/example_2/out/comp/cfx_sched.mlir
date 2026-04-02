module {
  cfx.func @example_2(%arg0: memref<1000xi32>, %arg1: memref<1000xi32>, %arg2: memref<1000xi32>, %arg3: memref<1000xi32>, %arg4: i32) -> i32 attributes {handshake.name = "func0"} {
    %0 = cfx.start {cfx.bb = 0 : i64, handshake.name = "start0", sched_var = #cfx.sched_var<0>}
    %1 = cfx.constant %0 0 : i32 {cfx.bb = 0 : i64, handshake.name = "constant1", sched_var = #cfx.sched_var<0>} : i32
    %2 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br1", sched_var = #cfx.sched_var<0>} : i32
    %3 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br2", sched_var = #cfx.sched_var<0>} : i32
    %4 = cfx.br %arg4 {cfx.bb = 0 : i64, handshake.name = "br3", sched_var = #cfx.sched_var<0>} : i32
    %5 = cfx.br %0 {cfx.bb = 0 : i64, handshake.name = "br4", sched_var = #cfx.sched_var<0>} : i0
    %6 = cfx.phi %30, %2 {cfx.bb = 1 : i64, handshake.name = "phi0", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %7 = cfx.phi %31, %3 {cfx.bb = 1 : i64, handshake.name = "phi1", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %8 = cfx.phi %32, %4 {cfx.bb = 1 : i64, handshake.name = "phi2", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %9 = cfx.phi %33, %5 {cfx.bb = 1 : i64, handshake.name = "phi3", sched_var = #cfx.sched_var<0>} : i0, i0 -> i0
    %10 = cfx.constant %9 1000 : i32 {cfx.bb = 1 : i64, handshake.name = "constant2", sched_var = #cfx.sched_var<0>} : i32
    %11 = cfx.constant %9 1 : i32 {cfx.bb = 1 : i64, handshake.name = "constant6", sched_var = #cfx.sched_var<0>} : i32
    %12 = arith.extui %6 {cfx.bb = 1 : i64, handshake.name = "extui0", sched_var = #cfx.sched_var<0>} : i32 to i64
    %13 = arith.index_cast %12 {cfx.bb = 1 : i64, handshake.name = "index_cast0", sched_var = #cfx.sched_var<0>} : i64 to index
    %14 = memref.load %arg0[%13] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load0", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %15 = arith.extui %6 {cfx.bb = 1 : i64, handshake.name = "extui1", sched_var = #cfx.sched_var<0>} : i32 to i64
    %16 = arith.index_cast %15 {cfx.bb = 1 : i64, handshake.name = "index_cast1", sched_var = #cfx.sched_var<0>} : i64 to index
    %17 = memref.load %arg1[%16] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load1", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %18 = arith.muli %14, %17 {cfx.bb = 1 : i64, handshake.name = "muli0", sched_var = #cfx.sched_var<0>} : i32
    %19 = arith.extui %6 {cfx.bb = 1 : i64, handshake.name = "extui2", sched_var = #cfx.sched_var<0>} : i32 to i64
    %20 = arith.index_cast %19 {cfx.bb = 1 : i64, handshake.name = "index_cast2", sched_var = #cfx.sched_var<0>} : i64 to index
    %21 = memref.load %arg2[%20] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load2", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %22 = arith.extui %6 {cfx.bb = 1 : i64, handshake.name = "extui3", sched_var = #cfx.sched_var<0>} : i32 to i64
    %23 = arith.index_cast %22 {cfx.bb = 1 : i64, handshake.name = "index_cast3", sched_var = #cfx.sched_var<0>} : i64 to index
    %24 = memref.load %arg3[%23] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load3", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %25 = arith.muli %21, %24 {cfx.bb = 1 : i64, handshake.name = "muli1", sched_var = #cfx.sched_var<0>} : i32
    %26 = arith.addi %18, %25 {cfx.bb = 1 : i64, handshake.name = "addi0", sched_var = #cfx.sched_var<0>} : i32
    %27 = arith.addi %7, %26 {cfx.bb = 1 : i64, handshake.name = "addi1", sched_var = #cfx.sched_var<0>} : i32
    %28 = arith.addi %6, %11 {cfx.bb = 1 : i64, handshake.name = "addi2", sched_var = #cfx.sched_var<0>} : i32
    %29 = arith.cmpi ult, %28, %10 {cfx.bb = 1 : i64, handshake.name = "cmpi0", sched_var = #cfx.sched_var<0>} : i32
    %30 = cfx.cond_br cond = %29[when_true], input = %28 {cfx.bb = 1 : i64, handshake.name = "cond_br2", sched_var = #cfx.sched_var<0>} : i1, i32 -> i32
    %31 = cfx.cond_br cond = %29[when_true], input = %27 {cfx.bb = 1 : i64, handshake.name = "cond_br3", sched_var = #cfx.sched_var<0>} : i1, i32 -> i32
    %32 = cfx.cond_br cond = %29[when_true], input = %8 {cfx.bb = 1 : i64, handshake.name = "cond_br4", sched_var = #cfx.sched_var<0>} : i1, i32 -> i32
    %33 = cfx.cond_br cond = %29[when_true], input = %9 {cfx.bb = 1 : i64, handshake.name = "cond_br5", sched_var = #cfx.sched_var<0>} : i1, i0 -> i0
    %34 = cfx.cond_br cond = %29[when_false], input = %11 {cfx.bb = 1 : i64, handshake.name = "cond_br6", sched_var = #cfx.sched_var<0>} : i1, i32 -> i32
    %35 = cfx.cond_br cond = %29[when_false], input = %27 {cfx.bb = 1 : i64, handshake.name = "cond_br7", sched_var = #cfx.sched_var<0>} : i1, i32 -> i32
    %36 = cfx.cond_br cond = %29[when_false], input = %8 {cfx.bb = 1 : i64, handshake.name = "cond_br8", sched_var = #cfx.sched_var<0>} : i1, i32 -> i32
    %37 = cfx.cond_br cond = %29[when_false], input = %9 {cfx.bb = 1 : i64, handshake.name = "cond_br9", sched_var = #cfx.sched_var<0>} : i1, i0 -> i0
    %38 = cfx.phi %61, %34 {cfx.bb = 2 : i64, handshake.name = "phi4", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %39 = cfx.phi %62, %35 {cfx.bb = 2 : i64, handshake.name = "phi5", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %40 = cfx.phi %63, %36 {cfx.bb = 2 : i64, handshake.name = "phi6", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %41 = cfx.phi %64, %37 {cfx.bb = 2 : i64, handshake.name = "phi7", sched_var = #cfx.sched_var<0>} : i0, i0 -> i0
    %42 = cfx.constant %41 1000 : i32 {cfx.bb = 2 : i64, handshake.name = "constant7", sched_var = #cfx.sched_var<0>} : i32
    %43 = arith.extui %38 {cfx.bb = 2 : i64, handshake.name = "extui4", sched_var = #cfx.sched_var<0>} : i32 to i64
    %44 = arith.index_cast %43 {cfx.bb = 2 : i64, handshake.name = "index_cast4", sched_var = #cfx.sched_var<0>} : i64 to index
    %45 = memref.load %arg0[%44] {cfx.bb = 2 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load4", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %46 = arith.extui %38 {cfx.bb = 2 : i64, handshake.name = "extui5", sched_var = #cfx.sched_var<0>} : i32 to i64
    %47 = arith.index_cast %46 {cfx.bb = 2 : i64, handshake.name = "index_cast5", sched_var = #cfx.sched_var<0>} : i64 to index
    %48 = memref.load %arg1[%47] {cfx.bb = 2 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load5", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %49 = arith.muli %45, %48 {cfx.bb = 2 : i64, handshake.name = "muli2", sched_var = #cfx.sched_var<0>} : i32
    %50 = arith.extui %38 {cfx.bb = 2 : i64, handshake.name = "extui6", sched_var = #cfx.sched_var<0>} : i32 to i64
    %51 = arith.index_cast %50 {cfx.bb = 2 : i64, handshake.name = "index_cast6", sched_var = #cfx.sched_var<0>} : i64 to index
    %52 = memref.load %arg2[%51] {cfx.bb = 2 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load6", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %53 = arith.extui %38 {cfx.bb = 2 : i64, handshake.name = "extui7", sched_var = #cfx.sched_var<0>} : i32 to i64
    %54 = arith.index_cast %53 {cfx.bb = 2 : i64, handshake.name = "index_cast7", sched_var = #cfx.sched_var<0>} : i64 to index
    %55 = memref.load %arg3[%54] {cfx.bb = 2 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load7", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %56 = arith.muli %52, %55 {cfx.bb = 2 : i64, handshake.name = "muli3", sched_var = #cfx.sched_var<0>} : i32
    %57 = arith.addi %49, %56 {cfx.bb = 2 : i64, handshake.name = "addi3", sched_var = #cfx.sched_var<0>} : i32
    %58 = arith.addi %39, %57 {cfx.bb = 2 : i64, handshake.name = "addi4", sched_var = #cfx.sched_var<0>} : i32
    %59 = arith.muli %38, %40 {cfx.bb = 2 : i64, handshake.name = "muli4", sched_var = #cfx.sched_var<0>} : i32
    %60 = arith.cmpi ult, %59, %42 {cfx.bb = 2 : i64, handshake.name = "cmpi1", sched_var = #cfx.sched_var<0>} : i32
    %61 = cfx.cond_br cond = %60[when_true], input = %59 {cfx.bb = 2 : i64, handshake.name = "cond_br10", sched_var = #cfx.sched_var<0>} : i1, i32 -> i32
    %62 = cfx.cond_br cond = %60[when_true], input = %58 {cfx.bb = 2 : i64, handshake.name = "cond_br11", sched_var = #cfx.sched_var<0>} : i1, i32 -> i32
    %63 = cfx.cond_br cond = %60[when_true], input = %40 {cfx.bb = 2 : i64, handshake.name = "cond_br12", sched_var = #cfx.sched_var<0>} : i1, i32 -> i32
    %64 = cfx.cond_br cond = %60[when_true], input = %41 {cfx.bb = 2 : i64, handshake.name = "cond_br13", sched_var = #cfx.sched_var<0>} : i1, i0 -> i0
    %65 = cfx.cond_br cond = %60[when_false], input = %58 {cfx.bb = 2 : i64, handshake.name = "cond_br14", sched_var = #cfx.sched_var<0>} : i1, i32 -> i32
    %66 = cfx.cond_br cond = %60[when_false], input = %41 {cfx.bb = 2 : i64, handshake.name = "cond_br15", sched_var = #cfx.sched_var<0>} : i1, i0 -> i0
    cfx.return start = %66, %65 {cfx.bb = 3 : i64, handshake.name = "return1", sched_var = #cfx.sched_var<0>} : i32
  }
}

