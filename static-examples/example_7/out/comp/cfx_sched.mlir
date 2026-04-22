module {
  cfx.func @example_7(%arg0: memref<1000xi32>, %arg1: memref<1000xi32>, %arg2: memref<1000xi32>, %arg3: memref<1000xi32>, %arg4: memref<1000xi32>, %arg5: memref<1000xi32>, %arg6: memref<1000xi32>, %arg7: memref<1000xi32>) attributes {handshake.name = "func0"} {
    %0 = cfx.start {cfx.bb = 0 : i64, handshake.name = "start0", sched_var = #cfx.sched_var<0>}
    %1 = cfx.constant %0 0 : i32 {cfx.bb = 0 : i64, handshake.name = "constant1", sched_var = #cfx.sched_var<0>} : i32
    %2 = cfx.br %1 {cfx.bb = 0 : i64, handshake.name = "br1", sched_var = #cfx.sched_var<0>} : i32
    %3 = cfx.br %0 {cfx.bb = 0 : i64, handshake.name = "br2", sched_var = #cfx.sched_var<0>} : i0
    %4 = cfx.phi %66, %2 {cfx.bb = 1 : i64, handshake.name = "phi0", sched_var = #cfx.sched_var<0>} : i32, i32 -> i32
    %5 = cfx.phi %67, %3 {cfx.bb = 1 : i64, handshake.name = "phi1", sched_var = #cfx.sched_var<0>} : i0, i0 -> i0
    %6 = cfx.constant %5 1000 : i32 {cfx.bb = 1 : i64, handshake.name = "constant2", sched_var = #cfx.sched_var<0>} : i32
    %7 = cfx.constant %5 1 : i32 {cfx.bb = 1 : i64, handshake.name = "constant5", sched_var = #cfx.sched_var<0>} : i32
    %8 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui0", sched_var = #cfx.sched_var<0>} : i32 to i64
    %9 = arith.index_cast %8 {cfx.bb = 1 : i64, handshake.name = "index_cast0", sched_var = #cfx.sched_var<0>} : i64 to index
    %10 = memref.load %arg7[%9] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load0", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %11 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui1", sched_var = #cfx.sched_var<0>} : i32 to i64
    %12 = arith.index_cast %11 {cfx.bb = 1 : i64, handshake.name = "index_cast1", sched_var = #cfx.sched_var<0>} : i64 to index
    %13 = memref.load %arg0[%12] {cfx.bb = 1 : i64, handshake.deps = #handshake<deps[["store2", 1, 0]]>, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "load1", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %14 = arith.muli %13, %10 {cfx.bb = 1 : i64, handshake.name = "muli0", sched_var = #cfx.sched_var<1>} : i32
    %15 = arith.index_cast %11 {cfx.bb = 1 : i64, handshake.name = "index_cast2", sched_var = #cfx.sched_var<0>} : i64 to index
    memref.store %14, %arg0[%15] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store2", sched_var = #cfx.sched_var<5>} : memref<1000xi32>
    %16 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui2", sched_var = #cfx.sched_var<0>} : i32 to i64
    %17 = arith.index_cast %16 {cfx.bb = 1 : i64, handshake.name = "index_cast3", sched_var = #cfx.sched_var<0>} : i64 to index
    %18 = memref.load %arg7[%17] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load3", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %19 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui3", sched_var = #cfx.sched_var<0>} : i32 to i64
    %20 = arith.index_cast %19 {cfx.bb = 1 : i64, handshake.name = "index_cast4", sched_var = #cfx.sched_var<0>} : i64 to index
    %21 = memref.load %arg1[%20] {cfx.bb = 1 : i64, handshake.deps = #handshake<deps[["store5", 1, 0]]>, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "load4", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %22 = arith.muli %21, %18 {cfx.bb = 1 : i64, handshake.name = "muli1", sched_var = #cfx.sched_var<1>} : i32
    %23 = arith.index_cast %19 {cfx.bb = 1 : i64, handshake.name = "index_cast5", sched_var = #cfx.sched_var<0>} : i64 to index
    memref.store %22, %arg1[%23] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store5", sched_var = #cfx.sched_var<5>} : memref<1000xi32>
    %24 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui4", sched_var = #cfx.sched_var<0>} : i32 to i64
    %25 = arith.index_cast %24 {cfx.bb = 1 : i64, handshake.name = "index_cast6", sched_var = #cfx.sched_var<0>} : i64 to index
    %26 = memref.load %arg7[%25] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load6", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %27 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui5", sched_var = #cfx.sched_var<0>} : i32 to i64
    %28 = arith.index_cast %27 {cfx.bb = 1 : i64, handshake.name = "index_cast7", sched_var = #cfx.sched_var<0>} : i64 to index
    %29 = memref.load %arg2[%28] {cfx.bb = 1 : i64, handshake.deps = #handshake<deps[["store8", 1, 0]]>, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "load7", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %30 = arith.muli %29, %26 {cfx.bb = 1 : i64, handshake.name = "muli2", sched_var = #cfx.sched_var<1>} : i32
    %31 = arith.index_cast %27 {cfx.bb = 1 : i64, handshake.name = "index_cast8", sched_var = #cfx.sched_var<0>} : i64 to index
    memref.store %30, %arg2[%31] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store8", sched_var = #cfx.sched_var<5>} : memref<1000xi32>
    %32 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui6", sched_var = #cfx.sched_var<0>} : i32 to i64
    %33 = arith.index_cast %32 {cfx.bb = 1 : i64, handshake.name = "index_cast9", sched_var = #cfx.sched_var<0>} : i64 to index
    %34 = memref.load %arg7[%33] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load9", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %35 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui7", sched_var = #cfx.sched_var<0>} : i32 to i64
    %36 = arith.index_cast %35 {cfx.bb = 1 : i64, handshake.name = "index_cast10", sched_var = #cfx.sched_var<0>} : i64 to index
    %37 = memref.load %arg3[%36] {cfx.bb = 1 : i64, handshake.deps = #handshake<deps[["store11", 1, 0]]>, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "load10", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %38 = arith.muli %37, %34 {cfx.bb = 1 : i64, handshake.name = "muli3", sched_var = #cfx.sched_var<1>} : i32
    %39 = arith.index_cast %35 {cfx.bb = 1 : i64, handshake.name = "index_cast11", sched_var = #cfx.sched_var<0>} : i64 to index
    memref.store %38, %arg3[%39] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store11", sched_var = #cfx.sched_var<5>} : memref<1000xi32>
    %40 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui8", sched_var = #cfx.sched_var<0>} : i32 to i64
    %41 = arith.index_cast %40 {cfx.bb = 1 : i64, handshake.name = "index_cast12", sched_var = #cfx.sched_var<0>} : i64 to index
    %42 = memref.load %arg7[%41] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load12", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %43 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui9", sched_var = #cfx.sched_var<0>} : i32 to i64
    %44 = arith.index_cast %43 {cfx.bb = 1 : i64, handshake.name = "index_cast13", sched_var = #cfx.sched_var<0>} : i64 to index
    %45 = memref.load %arg4[%44] {cfx.bb = 1 : i64, handshake.deps = #handshake<deps[["store14", 1, 0]]>, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "load13", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %46 = arith.muli %45, %42 {cfx.bb = 1 : i64, handshake.name = "muli4", sched_var = #cfx.sched_var<1>} : i32
    %47 = arith.index_cast %43 {cfx.bb = 1 : i64, handshake.name = "index_cast14", sched_var = #cfx.sched_var<0>} : i64 to index
    memref.store %46, %arg4[%47] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store14", sched_var = #cfx.sched_var<5>} : memref<1000xi32>
    %48 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui10", sched_var = #cfx.sched_var<0>} : i32 to i64
    %49 = arith.index_cast %48 {cfx.bb = 1 : i64, handshake.name = "index_cast15", sched_var = #cfx.sched_var<0>} : i64 to index
    %50 = memref.load %arg7[%49] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load15", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %51 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui11", sched_var = #cfx.sched_var<0>} : i32 to i64
    %52 = arith.index_cast %51 {cfx.bb = 1 : i64, handshake.name = "index_cast16", sched_var = #cfx.sched_var<0>} : i64 to index
    %53 = memref.load %arg5[%52] {cfx.bb = 1 : i64, handshake.deps = #handshake<deps[["store17", 1, 0]]>, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "load16", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %54 = arith.muli %53, %50 {cfx.bb = 1 : i64, handshake.name = "muli5", sched_var = #cfx.sched_var<1>} : i32
    %55 = arith.index_cast %51 {cfx.bb = 1 : i64, handshake.name = "index_cast17", sched_var = #cfx.sched_var<0>} : i64 to index
    memref.store %54, %arg5[%55] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store17", sched_var = #cfx.sched_var<5>} : memref<1000xi32>
    %56 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui12", sched_var = #cfx.sched_var<0>} : i32 to i64
    %57 = arith.index_cast %56 {cfx.bb = 1 : i64, handshake.name = "index_cast18", sched_var = #cfx.sched_var<0>} : i64 to index
    %58 = memref.load %arg7[%57] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<MC>, handshake.name = "load18", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %59 = arith.extui %4 {cfx.bb = 1 : i64, handshake.name = "extui13", sched_var = #cfx.sched_var<0>} : i32 to i64
    %60 = arith.index_cast %59 {cfx.bb = 1 : i64, handshake.name = "index_cast19", sched_var = #cfx.sched_var<0>} : i64 to index
    %61 = memref.load %arg6[%60] {cfx.bb = 1 : i64, handshake.deps = #handshake<deps[["store20", 1, 0]]>, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "load19", sched_var = #cfx.sched_var<0>} : memref<1000xi32>
    %62 = arith.muli %61, %58 {cfx.bb = 1 : i64, handshake.name = "muli6", sched_var = #cfx.sched_var<1>} : i32
    %63 = arith.index_cast %59 {cfx.bb = 1 : i64, handshake.name = "index_cast20", sched_var = #cfx.sched_var<0>} : i64 to index
    memref.store %62, %arg6[%63] {cfx.bb = 1 : i64, handshake.mem_interface = #handshake.mem_interface<LSQ: 0>, handshake.name = "store20", sched_var = #cfx.sched_var<5>} : memref<1000xi32>
    %64 = arith.addi %4, %7 {cfx.bb = 1 : i64, handshake.name = "addi0", sched_var = #cfx.sched_var<0>} : i32
    %65 = arith.cmpi ult, %64, %6 {cfx.bb = 1 : i64, handshake.name = "cmpi0", sched_var = #cfx.sched_var<1>} : i32
    %66 = cfx.cond_br cond = %65[when_true], input = %64 {cfx.bb = 1 : i64, handshake.name = "cond_br1", sched_var = #cfx.sched_var<1>} : i1, i32 -> i32
    %67 = cfx.cond_br cond = %65[when_true], input = %5 {cfx.bb = 1 : i64, handshake.name = "cond_br2", sched_var = #cfx.sched_var<1>} : i1, i0 -> i0
    %68 = cfx.cond_br cond = %65[when_false], input = %5 {cfx.bb = 1 : i64, handshake.name = "cond_br3", sched_var = #cfx.sched_var<1>} : i1, i0 -> i0
    cfx.return start = %68,  {cfx.bb = 2 : i64, handshake.name = "return1", sched_var = #cfx.sched_var<1>}
  }
}

