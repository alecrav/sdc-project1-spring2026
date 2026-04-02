module {
  cfx.func @example_4(%arg0: memref<1000xi32>, %arg1: memref<1000xi32>, %arg2: memref<1000xi32>) -> i32 attributes {handshake.name = "func0"} {
    %0 = cfx.start {cfx.bb = 0 : i64, handshake.name = "start0"}
    %1 = cfx.constant %0 0 : i32 {cfx.bb = 0 : i64, handshake.name = "constant1"} : i32
    %2 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br1"} : i32
    %3 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br2"} : i32
    %4 = cfx.br %0 {cfx.bb = 0 : i64, handshake.name = "br3"} : i0
    %5 = cfx.phi %37, %2 {cfx.bb = 1 : i64, handshake.name = "phi0"} : i32, i32 -> i32
    %6 = cfx.phi %38, %3 {cfx.bb = 1 : i64, handshake.name = "phi1"} : i32, i32 -> i32
    %7 = cfx.phi %39, %4 {cfx.bb = 1 : i64, handshake.name = "phi2"} : i0, i0 -> i0
    %8 = cfx.constant %7 1000 : i32 {cfx.bb = 1 : i64, handshake.name = "constant2"} : i32
    %9 = cfx.constant %7 1 : i32 {cfx.bb = 1 : i64, handshake.name = "constant5"} : i32
    %10 = arith.extui %5 {cfx.bb = 1 : i64, handshake.name = "extui0"} : i32 to i64
    %11 = arith.index_cast %10 {cfx.bb = 1 : i64, handshake.name = "index_cast0"} : i64 to index
    %12 = memref.load %arg1[%11] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load0"} : memref<1000xi32>
    %13 = arith.extui %5 {cfx.bb = 1 : i64, handshake.name = "extui1"} : i32 to i64
    %14 = arith.index_cast %13 {cfx.bb = 1 : i64, handshake.name = "index_cast1"} : i64 to index
    %15 = memref.load %arg0[%14] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load1"} : memref<1000xi32>
    %16 = arith.extui %5 {cfx.bb = 1 : i64, handshake.name = "extui2"} : i32 to i64
    %17 = arith.index_cast %16 {cfx.bb = 1 : i64, handshake.name = "index_cast2"} : i64 to index
    %18 = memref.load %arg1[%17] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load2"} : memref<1000xi32>
    %19 = arith.extui %5 {cfx.bb = 1 : i64, handshake.name = "extui3"} : i32 to i64
    %20 = arith.index_cast %19 {cfx.bb = 1 : i64, handshake.name = "index_cast3"} : i64 to index
    %21 = memref.load %arg0[%20] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load3"} : memref<1000xi32>
    %22 = arith.extui %5 {cfx.bb = 1 : i64, handshake.name = "extui4"} : i32 to i64
    %23 = arith.index_cast %22 {cfx.bb = 1 : i64, handshake.name = "index_cast4"} : i64 to index
    %24 = memref.load %arg1[%23] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load4"} : memref<1000xi32>
    %25 = arith.extui %5 {cfx.bb = 1 : i64, handshake.name = "extui5"} : i32 to i64
    %26 = arith.index_cast %25 {cfx.bb = 1 : i64, handshake.name = "index_cast5"} : i64 to index
    %27 = memref.load %arg1[%26] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load5"} : memref<1000xi32>
    %28 = arith.muli %24, %27 {cfx.bb = 1 : i64, handshake.name = "muli0"} : i32
    %29 = arith.addi %21, %28 {cfx.bb = 1 : i64, handshake.name = "addi0"} : i32
    %30 = arith.extui %5 {cfx.bb = 1 : i64, handshake.name = "extui6"} : i32 to i64
    %31 = arith.index_cast %30 {cfx.bb = 1 : i64, handshake.name = "index_cast6"} : i64 to index
    memref.store %29, %arg2[%31] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "store6"} : memref<1000xi32>
    %32 = arith.muli %15, %18 {cfx.bb = 1 : i64, handshake.name = "muli1"} : i32
    %33 = arith.addi %12, %32 {cfx.bb = 1 : i64, handshake.name = "addi1"} : i32
    %34 = arith.addi %6, %33 {cfx.bb = 1 : i64, handshake.name = "addi2"} : i32
    %35 = arith.addi %5, %9 {cfx.bb = 1 : i64, handshake.name = "addi3"} : i32
    %36 = arith.cmpi ult, %35, %8 {cfx.bb = 1 : i64, handshake.name = "cmpi0"} : i32
    %37 = cfx.cond_br cond = %36[when_true], input = %35 {cfx.bb = 1 : i64, handshake.name = "cond_br1"} : i1, i32 -> i32
    %38 = cfx.cond_br cond = %36[when_true], input = %34 {cfx.bb = 1 : i64, handshake.name = "cond_br2"} : i1, i32 -> i32
    %39 = cfx.cond_br cond = %36[when_true], input = %7 {cfx.bb = 1 : i64, handshake.name = "cond_br3"} : i1, i0 -> i0
    %40 = cfx.cond_br cond = %36[when_false], input = %34 {cfx.bb = 1 : i64, handshake.name = "cond_br4"} : i1, i32 -> i32
    %41 = cfx.cond_br cond = %36[when_false], input = %7 {cfx.bb = 1 : i64, handshake.name = "cond_br5"} : i1, i0 -> i0
    cfx.return start = %41, %40 {cfx.bb = 2 : i64, handshake.name = "return1"} : i32
  }
}

