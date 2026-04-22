module {
  cfx.func @example_12(%arg0: memref<1000xf32>, %arg1: memref<1000xf32>) -> f32 attributes {handshake.name = "func0"} {
    %0 = cfx.start {cfx.bb = 0 : i64, handshake.name = "start0", sched_var = #cfx.sched_var<0>}
    %1 = cfx.constant %0 0 : i32 {cfx.bb = 0 : i64, handshake.name = "constant2", sched_var = #cfx.sched_var<0>} : i32
    %2 = cfx.constant %0 1.000000e+00 : f32 {cfx.bb = 0 : i64, handshake.name = "constant3", sched_var = #cfx.sched_var<0>} : f32
    %3 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br1", sched_var = #cfx.sched_var<0>} : i32
    %4 = cfx.br %2 {cfx.bb = 0 : i64, handshake.name = "br2", sched_var = #cfx.sched_var<0>} : f32
    %5 = cfx.br %0 {cfx.bb = 0 : i64, handshake.name = "br3", sched_var = #cfx.sched_var<0>} : i0
    %6 = cfx.phi %24, %3 {cfx.bb = 1 : i64, handshake.name = "phi0", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %7 = cfx.phi %25, %4 {cfx.bb = 1 : i64, handshake.name = "phi1", sched_var = #cfx.sched_var<0>} : f32, f32 -> f32
    %8 = cfx.phi %26, %5 {cfx.bb = 1 : i64, handshake.name = "phi2", sched_var = #cfx.sched_var<0>} : i0, i0 -> i0
    %9 = cfx.constant %8 1000 : i32 {cfx.bb = 1 : i64, handshake.name = "constant4", sched_var = #cfx.sched_var<0>} : i32
    %10 = cfx.constant %8 1.000000e+03 : f32 {cfx.bb = 1 : i64, handshake.name = "constant8", sched_var = #cfx.sched_var<0>} : f32
    %11 = cfx.constant %8 1 : i32 {cfx.bb = 1 : i64, handshake.name = "constant9", sched_var = #cfx.sched_var<0>} : i32
    %12 = arith.extui %6 {cfx.bb = 1 : i64, handshake.name = "extui0", sched_var = #cfx.sched_var<0>} : i32 to i64
    %13 = arith.index_cast %12 {cfx.bb = 1 : i64, handshake.name = "index_cast0", sched_var = #cfx.sched_var<0>} : i64 to index
    %14 = memref.load %arg0[%13] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load0", sched_var = #cfx.sched_var<0>} : memref<1000xf32>
    %15 = arith.extui %6 {cfx.bb = 1 : i64, handshake.name = "extui1", sched_var = #cfx.sched_var<0>} : i32 to i64
    %16 = arith.index_cast %15 {cfx.bb = 1 : i64, handshake.name = "index_cast1", sched_var = #cfx.sched_var<0>} : i64 to index
    %17 = memref.load %arg1[%16] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load1", sched_var = #cfx.sched_var<0>} : memref<1000xf32>
    %18 = arith.mulf %14, %17 {cfx.bb = 1 : i64, handshake.name = "mulf0", sched_var = #cfx.sched_var<1>} : f32
    %19 = arith.mulf %7, %18 {cfx.bb = 1 : i64, handshake.name = "mulf1", sched_var = #cfx.sched_var<6>} : f32
    %20 = arith.addi %6, %11 {cfx.bb = 1 : i64, handshake.name = "addi0", sched_var = #cfx.sched_var<0>} : i32
    %21 = arith.cmpf olt, %19, %10 {cfx.bb = 1 : i64, handshake.name = "cmpf0", sched_var = #cfx.sched_var<11>} : f32
    %22 = arith.cmpi ult, %20, %9 {cfx.bb = 1 : i64, handshake.name = "cmpi0", sched_var = #cfx.sched_var<1>} : i32
    %23 = arith.andi %21, %22 {cfx.bb = 1 : i64, handshake.name = "andi0", sched_var = #cfx.sched_var<12>} : i1
    %24 = cfx.cond_br cond = %23[when_true], input = %20 {cfx.bb = 1 : i64, handshake.name = "cond_br1", sched_var = #cfx.sched_var<12>} : i1, i32 -> i32
    %25 = cfx.cond_br cond = %23[when_true], input = %19 {cfx.bb = 1 : i64, handshake.name = "cond_br2", sched_var = #cfx.sched_var<12>} : i1, f32 -> f32
    %26 = cfx.cond_br cond = %23[when_true], input = %8 {cfx.bb = 1 : i64, handshake.name = "cond_br3", sched_var = #cfx.sched_var<12>} : i1, i0 -> i0
    %27 = cfx.cond_br cond = %23[when_false], input = %19 {cfx.bb = 1 : i64, handshake.name = "cond_br4", sched_var = #cfx.sched_var<12>} : i1, f32 -> f32
    %28 = cfx.cond_br cond = %23[when_false], input = %8 {cfx.bb = 1 : i64, handshake.name = "cond_br5", sched_var = #cfx.sched_var<12>} : i1, i0 -> i0
    cfx.return start = %28, %27 {cfx.bb = 2 : i64, handshake.name = "return1", sched_var = #cfx.sched_var<12>} : f32
  }
}

