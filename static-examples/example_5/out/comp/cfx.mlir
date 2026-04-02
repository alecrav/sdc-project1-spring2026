module {
  cfx.func @example_5(%arg0: memref<1000xi32>, %arg1: memref<1000xi32>, %arg2: memref<1000xi32>, %arg3: memref<1000xi32>, %arg4: memref<1000xi32>, %arg5: memref<1000xi32>, %arg6: memref<1000xi32>, %arg7: memref<1000xi32>) attributes {handshake.name = "func0"} {
    %0 = cfx.start {cfx.bb = 0 : i64, handshake.name = "start0"}
    %1 = cfx.constant %0 0 : i32 {cfx.bb = 0 : i64, handshake.name = "constant1"} : i32
    %2 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br1"} : i32
    %3 = cfx.br %0 {cfx.bb = 0 : i64, handshake.name = "br2"} : i0
    %4 = cfx.phi %36, %2 {cfx.bb = 1 : i64, handshake.name = "phi0"} : i32, i32 -> i32
    %5 = cfx.phi %37, %3 {cfx.bb = 1 : i64, handshake.name = "phi1"} : i0, i0 -> i0
    %6 = cfx.constant %5 1000 : i32 {cfx.bb = 1 : i64, handshake.name = "constant2"} : i32
    %7 = cfx.constant %5 1 : i32 {cfx.bb = 1 : i64, handshake.name = "constant5"} : i32
    %8 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui0"} : i32 to i64
    %9 = arith.index_cast %8 {cfx.bb = 1 : i64, handshake.name = "index_cast0"} : i64 to index
    %10 = memref.load %arg2[%9] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load0"} : memref<1000xi32>
    %11 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui1"} : i32 to i64
    %12 = arith.index_cast %11 {cfx.bb = 1 : i64, handshake.name = "index_cast1"} : i64 to index
    %13 = memref.load %arg1[%12] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load1"} : memref<1000xi32>
    %14 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui2"} : i32 to i64
    %15 = arith.index_cast %14 {cfx.bb = 1 : i64, handshake.name = "index_cast2"} : i64 to index
    %16 = memref.load %arg0[%15] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load2"} : memref<1000xi32>
    %17 = arith.muli %13, %16 {cfx.bb = 1 : i64, handshake.name = "muli0"} : i32
    %18 = arith.addi %10, %17 {cfx.bb = 1 : i64, handshake.name = "addi0"} : i32
    %19 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui3"} : i32 to i64
    %20 = arith.index_cast %19 {cfx.bb = 1 : i64, handshake.name = "index_cast3"} : i64 to index
    memref.store %18, %arg3[%20] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "store3"} : memref<1000xi32>
    %21 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui4"} : i32 to i64
    %22 = arith.index_cast %21 {cfx.bb = 1 : i64, handshake.name = "index_cast4"} : i64 to index
    %23 = memref.load %arg6[%22] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load4"} : memref<1000xi32>
    %24 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui5"} : i32 to i64
    %25 = arith.index_cast %24 {cfx.bb = 1 : i64, handshake.name = "index_cast5"} : i64 to index
    %26 = memref.load %arg5[%25] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load5"} : memref<1000xi32>
    %27 = arith.muli %23, %26 {cfx.bb = 1 : i64, handshake.name = "muli1"} : i32
    %28 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui6"} : i32 to i64
    %29 = arith.index_cast %28 {cfx.bb = 1 : i64, handshake.name = "index_cast6"} : i64 to index
    %30 = memref.load %arg4[%29] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load6"} : memref<1000xi32>
    %31 = arith.muli %27, %30 {cfx.bb = 1 : i64, handshake.name = "muli2"} : i32
    %32 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui7"} : i32 to i64
    %33 = arith.index_cast %32 {cfx.bb = 1 : i64, handshake.name = "index_cast7"} : i64 to index
    memref.store %31, %arg7[%33] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "store7"} : memref<1000xi32>
    %34 = arith.addi %4, %7 {cfx.bb = 1 : i64, handshake.name = "addi1"} : i32
    %35 = arith.cmpi ult, %34, %6 {cfx.bb = 1 : i64, handshake.name = "cmpi0"} : i32
    %36 = cfx.cond_br cond = %35[when_true], input = %34 {cfx.bb = 1 : i64, handshake.name = "cond_br1"} : i1, i32 -> i32
    %37 = cfx.cond_br cond = %35[when_true], input = %5 {cfx.bb = 1 : i64, handshake.name = "cond_br2"} : i1, i0 -> i0
    %38 = cfx.cond_br cond = %35[when_false], input = %5 {cfx.bb = 1 : i64, handshake.name = "cond_br3"} : i1, i0 -> i0
    cfx.return start = %38,  {cfx.bb = 2 : i64, handshake.name = "return1"}
  }
}

