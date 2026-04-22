module {
  cfx.func @example_8(%arg0: memref<1000xi32>, %arg1: i32) attributes {handshake.name = "func0"} {
    %0 = cfx.start {cfx.bb = 0 : i64, handshake.name = "start0", sched_var = #cfx.sched_var<0>}
    %1 = cfx.constant %0 0 : i32 {cfx.bb = 0 : i64, handshake.name = "constant1", sched_var = #cfx.sched_var<0>} : i32
    %2 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br1", sched_var = #cfx.sched_var<0>} : i32
    %3 = cfx.br %arg1 {cfx.bb = 0 : i64, handshake.name = "br2", sched_var = #cfx.sched_var<0>} : i32
    %4 = cfx.br %0 {cfx.bb = 0 : i64, handshake.name = "br3", sched_var = #cfx.sched_var<0>} : i0
    %5 = cfx.phi %30, %2 {cfx.bb = 1 : i64, handshake.name = "phi0", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %6 = cfx.phi %31, %3 {cfx.bb = 1 : i64, handshake.name = "phi1", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %7 = cfx.phi %32, %4 {cfx.bb = 1 : i64, handshake.name = "phi2", sched_var = #cfx.sched_var<0>} : i0, i0 -> i0
    %8 = cfx.constant %7 500 : i32 {cfx.bb = 1 : i64, handshake.name = "constant2", sched_var = #cfx.sched_var<0>} : i32
    %9 = cfx.constant %7 1 : i32 {cfx.bb = 1 : i64, handshake.name = "constant5", sched_var = #cfx.sched_var<0>} : i32
    %10 = arith.shli %5, %9 {cfx.bb = 1 : i64, handshake.name = "shli0", sched_var = #cfx.sched_var<0>} : i32
    %11 = arith.extui %10 {cfx.bb = 1 : i64, handshake.name = "extui0", sched_var = #cfx.sched_var<0>} : i32 to i64
    %12 = arith.index_cast %11 {cfx.bb = 1 : i64, handshake.name = "index_cast0", sched_var = #cfx.sched_var<0>} : i64 to index
    %13 = memref.load %arg0[%12] {cfx.bb = 1 : i64, handshake.deps = #handshake<deps[["store1", 1, 0]]>, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "load0", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %14 = arith.muli %13, %6 {cfx.bb = 1 : i64, handshake.name = "muli0", sched_var = #cfx.sched_var<1>} : i32
    %15 = arith.shli %5, %9 {cfx.bb = 1 : i64, handshake.name = "shli1", sched_var = #cfx.sched_var<0>} : i32
    %16 = arith.extui %15 {cfx.bb = 1 : i64, handshake.name = "extui1", sched_var = #cfx.sched_var<0>} : i32 to i64
    %17 = arith.index_cast %16 {cfx.bb = 1 : i64, handshake.name = "index_cast1", sched_var = #cfx.sched_var<0>} : i64 to index
    memref.store %14, %arg0[%17] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store1", sched_var = #cfx.sched_var<5>} : memref<1000xi32>
    %18 = arith.shli %5, %9 {cfx.bb = 1 : i64, handshake.name = "shli2", sched_var = #cfx.sched_var<0>} : i32
    %19 = arith.ori %18, %9 {cfx.bb = 1 : i64, handshake.name = "ori0", sched_var = #cfx.sched_var<0>} : i32
    %20 = arith.extui %19 {cfx.bb = 1 : i64, handshake.name = "extui2", sched_var = #cfx.sched_var<0>} : i32 to i64
    %21 = arith.index_cast %20 {cfx.bb = 1 : i64, handshake.name = "index_cast2", sched_var = #cfx.sched_var<0>} : i64 to index
    %22 = memref.load %arg0[%21] {cfx.bb = 1 : i64, handshake.deps = #handshake<deps[["store3", 1, 0]]>, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "load2", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %23 = arith.muli %22, %6 {cfx.bb = 1 : i64, handshake.name = "muli1", sched_var = #cfx.sched_var<1>} : i32
    %24 = arith.shli %5, %9 {cfx.bb = 1 : i64, handshake.name = "shli3", sched_var = #cfx.sched_var<0>} : i32
    %25 = arith.ori %24, %9 {cfx.bb = 1 : i64, handshake.name = "ori1", sched_var = #cfx.sched_var<0>} : i32
    %26 = arith.extui %25 {cfx.bb = 1 : i64, handshake.name = "extui3", sched_var = #cfx.sched_var<0>} : i32 to i64
    %27 = arith.index_cast %26 {cfx.bb = 1 : i64, handshake.name = "index_cast3", sched_var = #cfx.sched_var<0>} : i64 to index
    memref.store %23, %arg0[%27] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store3", sched_var = #cfx.sched_var<5>} : memref<1000xi32>
    %28 = arith.addi %5, %9 {cfx.bb = 1 : i64, handshake.name = "addi0", sched_var = #cfx.sched_var<0>} : i32
    %29 = arith.cmpi ult, %28, %8 {cfx.bb = 1 : i64, handshake.name = "cmpi0", sched_var = #cfx.sched_var<1>} : i32
    %30 = cfx.cond_br cond = %29[when_true], input = %28 {cfx.bb = 1 : i64, handshake.name = "cond_br1", sched_var = #cfx.sched_var<1>} : i1, i32 -> i32
    %31 = cfx.cond_br cond = %29[when_true], input = %6 {cfx.bb = 1 : i64, handshake.name = "cond_br2", sched_var = #cfx.sched_var<1>} : i1, i32 -> i32
    %32 = cfx.cond_br cond = %29[when_true], input = %7 {cfx.bb = 1 : i64, handshake.name = "cond_br3", sched_var = #cfx.sched_var<1>} : i1, i0 -> i0
    %33 = cfx.cond_br cond = %29[when_false], input = %7 {cfx.bb = 1 : i64, handshake.name = "cond_br4", sched_var = #cfx.sched_var<1>} : i1, i0 -> i0
    cfx.return start = %33,  {cfx.bb = 2 : i64, handshake.name = "return1", sched_var = #cfx.sched_var<1>}
  }
}

