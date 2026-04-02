module {
  cfx.func @example_3(%arg0: memref<1000xi32>, %arg1: memref<1000xi32>) attributes {handshake.name = "func0"} {
    %0 = cfx.start {cfx.bb = 0 : i64, handshake.name = "start0", sched_var = #cfx.sched_var<0>}
    %1 = cfx.constant %0 0 : i32 {cfx.bb = 0 : i64, handshake.name = "constant1", sched_var = #cfx.sched_var<0>} : i32
    %2 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br1", sched_var = #cfx.sched_var<0>} : i32
    %3 = cfx.br %0 {cfx.bb = 0 : i64, handshake.name = "br2", sched_var = #cfx.sched_var<0>} : i0
    %4 = cfx.phi %21, %2 {cfx.bb = 1 : i64, handshake.name = "phi0", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %5 = cfx.phi %22, %3 {cfx.bb = 1 : i64, handshake.name = "phi1", sched_var = #cfx.sched_var<0>} : i0, i0 -> i0
    %6 = cfx.constant %5 1000 : i32 {cfx.bb = 1 : i64, handshake.name = "constant2", sched_var = #cfx.sched_var<0>} : i32
    %7 = cfx.constant %5 1 : i32 {cfx.bb = 1 : i64, handshake.name = "constant5", sched_var = #cfx.sched_var<0>} : i32
    %8 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui0", sched_var = #cfx.sched_var<0>} : i32 to i64
    %9 = arith.index_cast %8 {cfx.bb = 1 : i64, handshake.name = "index_cast0", sched_var = #cfx.sched_var<0>} : i64 to index
    %10 = memref.load %arg1[%9] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load0", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %11 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui1", sched_var = #cfx.sched_var<0>} : i32 to i64
    %12 = arith.index_cast %11 {cfx.bb = 1 : i64, handshake.name = "index_cast1", sched_var = #cfx.sched_var<0>} : i64 to index
    %13 = memref.load %arg1[%12] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load1", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %14 = arith.muli %10, %13 {cfx.bb = 1 : i64, handshake.name = "muli0", sched_var = #cfx.sched_var<0>} : i32
    %15 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui2", sched_var = #cfx.sched_var<0>} : i32 to i64
    %16 = arith.index_cast %15 {cfx.bb = 1 : i64, handshake.name = "index_cast2", sched_var = #cfx.sched_var<0>} : i64 to index
    memref.store %14, %arg0[%16] {cfx.bb = 1 : i64, handshake.deps = #handshake<deps[["store3", 1, 0]]>, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store2", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %17 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui3", sched_var = #cfx.sched_var<0>} : i32 to i64
    %18 = arith.index_cast %17 {cfx.bb = 1 : i64, handshake.name = "index_cast3", sched_var = #cfx.sched_var<0>} : i64 to index
    memref.store %4, %arg0[%18] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store3", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %19 = arith.addi %4, %7 {cfx.bb = 1 : i64, handshake.name = "addi0", sched_var = #cfx.sched_var<0>} : i32
    %20 = arith.cmpi ult, %19, %6 {cfx.bb = 1 : i64, handshake.name = "cmpi0", sched_var = #cfx.sched_var<0>} : i32
    %21 = cfx.cond_br cond = %20[when_true], input = %19 {cfx.bb = 1 : i64, handshake.name = "cond_br1", sched_var = #cfx.sched_var<0>} : i1, i32 -> i32
    %22 = cfx.cond_br cond = %20[when_true], input = %5 {cfx.bb = 1 : i64, handshake.name = "cond_br2", sched_var = #cfx.sched_var<0>} : i1, i0 -> i0
    %23 = cfx.cond_br cond = %20[when_false], input = %5 {cfx.bb = 1 : i64, handshake.name = "cond_br3", sched_var = #cfx.sched_var<0>} : i1, i0 -> i0
    cfx.return start = %23,  {cfx.bb = 2 : i64, handshake.name = "return1", sched_var = #cfx.sched_var<0>}
  }
}

