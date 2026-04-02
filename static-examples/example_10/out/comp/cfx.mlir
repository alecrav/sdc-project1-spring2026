module {
  cfx.func @example_10(%arg0: memref<1000xi32>, %arg1: memref<1000xi32>) attributes {handshake.name = "func0"} {
    %0 = cfx.start {cfx.bb = 0 : i64, handshake.name = "start0"}
    %1 = cfx.constant %0 1 : i32 {cfx.bb = 0 : i64, handshake.name = "constant1"} : i32
    %2 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br1"} : i32
    %3 = cfx.br %0 {cfx.bb = 0 : i64, handshake.name = "br2"} : i0
    %4 = cfx.phi %21, %2 {cfx.bb = 1 : i64, handshake.name = "phi0"} : i32, i32 -> i32
    %5 = cfx.phi %22, %3 {cfx.bb = 1 : i64, handshake.name = "phi1"} : i0, i0 -> i0
    %6 = cfx.constant %5 1000 : i32 {cfx.bb = 1 : i64, handshake.name = "constant2"} : i32
    %7 = cfx.constant %5 -1 : i32 {cfx.bb = 1 : i64, handshake.name = "constant6"} : i32
    %8 = cfx.constant %5 1 : i32 {cfx.bb = 1 : i64, handshake.name = "constant7"} : i32
    %9 = arith.addi %4, %7 {cfx.bb = 1 : i64, handshake.name = "addi0"} : i32
    %10 = arith.extui %9 {cfx.bb = 1 : i64, handshake.name = "extui0"} : i32 to i64
    %11 = arith.index_cast %10 {cfx.bb = 1 : i64, handshake.name = "index_cast0"} : i64 to index
    %12 = memref.load %arg0[%11] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "load0"} : memref<1000xi32>
    %13 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui1"} : i32 to i64
    %14 = arith.index_cast %13 {cfx.bb = 1 : i64, handshake.name = "index_cast1"} : i64 to index
    %15 = memref.load %arg1[%14] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load1"} : memref<1000xi32>
    %16 = arith.muli %12, %15 {cfx.bb = 1 : i64, handshake.name = "muli0"} : i32
    %17 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui2"} : i32 to i64
    %18 = arith.index_cast %17 {cfx.bb = 1 : i64, handshake.name = "index_cast2"} : i64 to index
    memref.store %16, %arg0[%18] {cfx.bb = 1 : i64, handshake.deps = #handshake<deps[["load0", 1, 1]]>, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store2"} : memref<1000xi32>
    %19 = arith.addi %4, %8 {cfx.bb = 1 : i64, handshake.name = "addi1"} : i32
    %20 = arith.cmpi ult, %19, %6 {cfx.bb = 1 : i64, handshake.name = "cmpi0"} : i32
    %21 = cfx.cond_br cond = %20[when_true], input = %19 {cfx.bb = 1 : i64, handshake.name = "cond_br1"} : i1, i32 -> i32
    %22 = cfx.cond_br cond = %20[when_true], input = %5 {cfx.bb = 1 : i64, handshake.name = "cond_br2"} : i1, i0 -> i0
    %23 = cfx.cond_br cond = %20[when_false], input = %5 {cfx.bb = 1 : i64, handshake.name = "cond_br3"} : i1, i0 -> i0
    cfx.return start = %23,  {cfx.bb = 2 : i64, handshake.name = "return1"}
  }
}

