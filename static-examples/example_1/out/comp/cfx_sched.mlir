module {
  cfx.func @example_1(%arg0: memref<1000xi32>, %arg1: memref<1000xi32>, %arg2: memref<1000xi32>, %arg3: memref<1000xi32>) -> i32 attributes {handshake.name = "func0"} {
    %0 = cfx.start {cfx.bb = 0 : i64, handshake.name = "start0", sched_var = #cfx.sched_var<0>}
    %1 = cfx.constant %0 0 : i32 {cfx.bb = 0 : i64, handshake.name = "constant1", sched_var = #cfx.sched_var<0>} : i32
    %2 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br1", sched_var = #cfx.sched_var<0>} : i32
    %3 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br2", sched_var = #cfx.sched_var<0>} : i32
    %4 = cfx.br %0 {cfx.bb = 0 : i64, handshake.name = "br3", sched_var = #cfx.sched_var<0>} : i0
    %5 = cfx.phi %28, %2 {cfx.bb = 1 : i64, handshake.name = "phi0", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %6 = cfx.phi %29, %3 {cfx.bb = 1 : i64, handshake.name = "phi1", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %7 = cfx.phi %30, %4 {cfx.bb = 1 : i64, handshake.name = "phi2", sched_var = #cfx.sched_var<0>} : i0, i0 -> i0
    %8 = cfx.constant %7 1000 : i32 {cfx.bb = 1 : i64, handshake.name = "constant2", sched_var = #cfx.sched_var<0>} : i32
    %9 = cfx.constant %7 1 : i32 {cfx.bb = 1 : i64, handshake.name = "constant5", sched_var = #cfx.sched_var<0>} : i32
    %10 = arith.extui %5 {cfx.bb = 1 : i64, handshake.name = "extui0", sched_var = #cfx.sched_var<0>} : i32 to i64
    %11 = arith.index_cast %10 {cfx.bb = 1 : i64, handshake.name = "index_cast0", sched_var = #cfx.sched_var<0>} : i64 to index
    %12 = memref.load %arg0[%11] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load0", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %13 = arith.extui %5 {cfx.bb = 1 : i64, handshake.name = "extui1", sched_var = #cfx.sched_var<0>} : i32 to i64
    %14 = arith.index_cast %13 {cfx.bb = 1 : i64, handshake.name = "index_cast1", sched_var = #cfx.sched_var<0>} : i64 to index
    %15 = memref.load %arg1[%14] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load1", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %16 = arith.muli %12, %15 {cfx.bb = 1 : i64, handshake.name = "muli0", sched_var = #cfx.sched_var<1>} : i32
    %17 = arith.extui %5 {cfx.bb = 1 : i64, handshake.name = "extui2", sched_var = #cfx.sched_var<0>} : i32 to i64
    %18 = arith.index_cast %17 {cfx.bb = 1 : i64, handshake.name = "index_cast2", sched_var = #cfx.sched_var<0>} : i64 to index
    %19 = memref.load %arg2[%18] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load2", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %20 = arith.extui %5 {cfx.bb = 1 : i64, handshake.name = "extui3", sched_var = #cfx.sched_var<0>} : i32 to i64
    %21 = arith.index_cast %20 {cfx.bb = 1 : i64, handshake.name = "index_cast3", sched_var = #cfx.sched_var<0>} : i64 to index
    %22 = memref.load %arg3[%21] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load3", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %23 = arith.muli %19, %22 {cfx.bb = 1 : i64, handshake.name = "muli1", sched_var = #cfx.sched_var<1>} : i32
    %24 = arith.addi %16, %23 {cfx.bb = 1 : i64, handshake.name = "addi0", sched_var = #cfx.sched_var<5>} : i32
    %25 = arith.addi %6, %24 {cfx.bb = 1 : i64, handshake.name = "addi1", sched_var = #cfx.sched_var<6>} : i32
    %26 = arith.addi %5, %9 {cfx.bb = 1 : i64, handshake.name = "addi2", sched_var = #cfx.sched_var<0>} : i32
    %27 = arith.cmpi ult, %26, %8 {cfx.bb = 1 : i64, handshake.name = "cmpi0", sched_var = #cfx.sched_var<1>} : i32
    %28 = cfx.cond_br cond = %27[when_true], input = %26 {cfx.bb = 1 : i64, handshake.name = "cond_br1", sched_var = #cfx.sched_var<1>} : i1, i32 -> i32
    %29 = cfx.cond_br cond = %27[when_true], input = %25 {cfx.bb = 1 : i64, handshake.name = "cond_br2", sched_var = #cfx.sched_var<7>} : i1, i32 -> i32
    %30 = cfx.cond_br cond = %27[when_true], input = %7 {cfx.bb = 1 : i64, handshake.name = "cond_br3", sched_var = #cfx.sched_var<1>} : i1, i0 -> i0
    %31 = cfx.cond_br cond = %27[when_false], input = %25 {cfx.bb = 1 : i64, handshake.name = "cond_br4", sched_var = #cfx.sched_var<7>} : i1, i32 -> i32
    %32 = cfx.cond_br cond = %27[when_false], input = %7 {cfx.bb = 1 : i64, handshake.name = "cond_br5", sched_var = #cfx.sched_var<1>} : i1, i0 -> i0
    cfx.return start = %32, %31 {cfx.bb = 2 : i64, handshake.name = "return1", sched_var = #cfx.sched_var<7>} : i32
  }
}

