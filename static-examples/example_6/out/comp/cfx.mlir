module {
  cfx.func @example_6(%arg0: memref<1000xi32>, %arg1: memref<1000xi32>, %arg2: memref<1000xi32>, %arg3: memref<1000xi32>, %arg4: memref<1000xi32>, %arg5: memref<1000xi32>, %arg6: memref<1000xi32>, %arg7: memref<1000xi32>) attributes {handshake.name = "func0"} {
    %0 = cfx.start {cfx.bb = 0 : i64, handshake.name = "start0"}
    %1 = cfx.constant %0 0 : i32 {cfx.bb = 0 : i64, handshake.name = "constant1"} : i32
    %2 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br1"} : i32
    %3 = cfx.br %0 {cfx.bb = 0 : i64, handshake.name = "br2"} : i0
    %4 = cfx.phi %40, %2 {cfx.bb = 1 : i64, handshake.name = "phi0"} : i32, i32 -> i32
    %5 = cfx.phi %41, %3 {cfx.bb = 1 : i64, handshake.name = "phi1"} : i0, i0 -> i0
    %6 = cfx.constant %5 1000 : i32 {cfx.bb = 1 : i64, handshake.name = "constant2"} : i32
    %7 = cfx.constant %5 1 : i32 {cfx.bb = 1 : i64, handshake.name = "constant5"} : i32
    %8 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui0"} : i32 to i64
    %9 = arith.index_cast %8 {cfx.bb = 1 : i64, handshake.name = "index_cast0"} : i64 to index
    %10 = memref.load %arg1[%9] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load0"} : memref<1000xi32>
    %11 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui1"} : i32 to i64
    %12 = arith.index_cast %11 {cfx.bb = 1 : i64, handshake.name = "index_cast1"} : i64 to index
    %13 = memref.load %arg0[%12] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load1"} : memref<1000xi32>
    %14 = arith.muli %10, %13 {cfx.bb = 1 : i64, handshake.name = "muli0"} : i32
    %15 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui2"} : i32 to i64
    %16 = arith.index_cast %15 {cfx.bb = 1 : i64, handshake.name = "index_cast2"} : i64 to index
    memref.store %14, %arg2[%16] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "store2"} : memref<1000xi32>
    %17 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui3"} : i32 to i64
    %18 = arith.index_cast %17 {cfx.bb = 1 : i64, handshake.name = "index_cast3"} : i64 to index
    %19 = memref.load %arg6[%18] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load3"} : memref<1000xi32>
    %20 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui4"} : i32 to i64
    %21 = arith.index_cast %20 {cfx.bb = 1 : i64, handshake.name = "index_cast4"} : i64 to index
    %22 = memref.load %arg5[%21] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load4"} : memref<1000xi32>
    %23 = arith.muli %19, %22 {cfx.bb = 1 : i64, handshake.name = "muli1"} : i32
    %24 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui5"} : i32 to i64
    %25 = arith.index_cast %24 {cfx.bb = 1 : i64, handshake.name = "index_cast5"} : i64 to index
    %26 = memref.load %arg4[%25] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load5"} : memref<1000xi32>
    %27 = arith.muli %23, %26 {cfx.bb = 1 : i64, handshake.name = "muli2"} : i32
    %28 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui6"} : i32 to i64
    %29 = arith.index_cast %28 {cfx.bb = 1 : i64, handshake.name = "index_cast6"} : i64 to index
    %30 = memref.load %arg3[%29] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load6"} : memref<1000xi32>
    %31 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui7"} : i32 to i64
    %32 = arith.index_cast %31 {cfx.bb = 1 : i64, handshake.name = "index_cast7"} : i64 to index
    %33 = memref.load %arg1[%32] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load7"} : memref<1000xi32>
    %34 = arith.addi %30, %33 {cfx.bb = 1 : i64, handshake.name = "addi0"} : i32
    %35 = arith.muli %27, %34 {cfx.bb = 1 : i64, handshake.name = "muli3"} : i32
    %36 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui8"} : i32 to i64
    %37 = arith.index_cast %36 {cfx.bb = 1 : i64, handshake.name = "index_cast8"} : i64 to index
    memref.store %35, %arg7[%37] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "store8"} : memref<1000xi32>
    %38 = arith.addi %4, %7 {cfx.bb = 1 : i64, handshake.name = "addi1"} : i32
    %39 = arith.cmpi ult, %38, %6 {cfx.bb = 1 : i64, handshake.name = "cmpi0"} : i32
    %40 = cfx.cond_br cond = %39[when_true], input = %38 {cfx.bb = 1 : i64, handshake.name = "cond_br1"} : i1, i32 -> i32
    %41 = cfx.cond_br cond = %39[when_true], input = %5 {cfx.bb = 1 : i64, handshake.name = "cond_br2"} : i1, i0 -> i0
    %42 = cfx.cond_br cond = %39[when_false], input = %5 {cfx.bb = 1 : i64, handshake.name = "cond_br3"} : i1, i0 -> i0
    cfx.return start = %42,  {cfx.bb = 2 : i64, handshake.name = "return1"}
  }
}

