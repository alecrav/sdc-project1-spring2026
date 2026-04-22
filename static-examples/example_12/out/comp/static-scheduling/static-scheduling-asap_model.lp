Maximize
- sv_addi0 - sv_andi0 - sv_br1 - sv_br2 - sv_br3 - sv_cmpf0 - sv_cmpi0 - sv_cond_br1 - sv_cond_br2 - sv_cond_br3 - sv_cond_br4 - sv_cond_br5 - sv_constant2 - sv_constant3 - sv_constant4 - sv_constant8 - sv_constant9 - sv_extui0 - sv_extui1 - sv_index_cast0 - sv_index_cast1 - sv_load0 - sv_load1 - sv_mulf0 - sv_mulf1 - sv_phi0 - sv_phi1 - sv_phi2 - sv_return1 - sv_start0
Subject to
- sv_constant2 + sv_start0 <= 0.000000
- sv_constant3 + sv_start0 <= 0.000000
- sv_br1 + sv_constant2 <= 0.000000
- sv_br2 + sv_constant3 <= 0.000000
- sv_br3 + sv_start0 <= 0.000000
- sv_constant4 + sv_phi2 <= 0.000000
- sv_constant8 + sv_phi2 <= 0.000000
- sv_constant9 + sv_phi2 <= 0.000000
- sv_extui0 + sv_phi0 <= 0.000000
sv_extui0 - sv_index_cast0 <= 0.000000
sv_index_cast0 - sv_load0 <= 0.000000
- sv_extui1 + sv_phi0 <= 0.000000
sv_extui1 - sv_index_cast1 <= 0.000000
sv_index_cast1 - sv_load1 <= 0.000000
sv_load0 - sv_mulf0 <=  -1.000000
sv_load1 - sv_mulf0 <=  -1.000000
- sv_mulf1 + sv_phi1 <= 0.000000
sv_mulf0 - sv_mulf1 <=  -5.000000
- sv_addi0 + sv_phi0 <= 0.000000
- sv_addi0 + sv_constant9 <= 0.000000
- sv_cmpf0 + sv_mulf1 <=  -5.000000
- sv_cmpf0 + sv_constant8 <= 0.000000
sv_addi0 - sv_cmpi0 <=  -1.000000
- sv_cmpi0 + sv_constant4 <= 0.000000
- sv_andi0 + sv_cmpf0 <=  -1.000000
- sv_andi0 + sv_cmpi0 <= 0.000000
sv_andi0 - sv_cond_br1 <= 0.000000
sv_addi0 - sv_cond_br1 <=  -1.000000
sv_andi0 - sv_cond_br2 <= 0.000000
- sv_cond_br2 + sv_mulf1 <=  -5.000000
sv_andi0 - sv_cond_br3 <= 0.000000
- sv_cond_br3 + sv_phi2 <= 0.000000
sv_andi0 - sv_cond_br4 <= 0.000000
- sv_cond_br4 + sv_mulf1 <=  -5.000000
sv_andi0 - sv_cond_br5 <= 0.000000
- sv_cond_br5 + sv_phi2 <= 0.000000
sv_cond_br5 - sv_return1 <= 0.000000
sv_cond_br4 - sv_return1 <= 0.000000


sv_addi0 >= 0.000000
sv_andi0 >= 0.000000
sv_br1 >= 0.000000
sv_br2 >= 0.000000
sv_br3 >= 0.000000
sv_cmpf0 >= 0.000000
sv_cmpi0 >= 0.000000
sv_cond_br1 >= 0.000000
sv_cond_br2 >= 0.000000
sv_cond_br3 >= 0.000000
sv_cond_br4 >= 0.000000
sv_cond_br5 >= 0.000000
sv_constant2 >= 0.000000
sv_constant3 >= 0.000000
sv_constant4 >= 0.000000
sv_constant8 >= 0.000000
sv_constant9 >= 0.000000
sv_extui0 >= 0.000000
sv_extui1 >= 0.000000
sv_index_cast0 >= 0.000000
sv_index_cast1 >= 0.000000
sv_load0 >= 0.000000
sv_load1 >= 0.000000
sv_mulf0 >= 0.000000
sv_mulf1 >= 0.000000
sv_phi0 >= 0.000000
sv_phi1 >= 0.000000
sv_phi2 >= 0.000000
sv_return1 >= 0.000000
sv_start0 >= 0.000000
General
sv_addi0 sv_andi0 sv_br1 sv_br2 sv_br3 sv_cmpf0 sv_cmpi0 sv_cond_br1 sv_cond_br2 sv_cond_br3 sv_cond_br4 sv_cond_br5 sv_constant2 sv_constant3 sv_constant4 sv_constant8 sv_constant9 sv_extui0 sv_extui1 sv_index_cast0 sv_index_cast1 sv_load0 sv_load1 sv_mulf0 sv_mulf1 sv_phi0 sv_phi1 sv_phi2 sv_return1 sv_start0 

End
