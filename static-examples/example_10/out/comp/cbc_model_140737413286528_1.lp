Maximize
- sv_addi0 - sv_addi1 - sv_br1 - sv_br2 - sv_cmpi0 - sv_cond_br1 - sv_cond_br2 - sv_cond_br3 - sv_constant1 - sv_constant2 - sv_constant6 - sv_constant7 - sv_extui0 - sv_extui1 - sv_extui2 - sv_index_cast0 - sv_index_cast1 - sv_index_cast2 - sv_load0 - sv_load1 - sv_muli0 - sv_phi0 - sv_phi1 - sv_return1 - sv_start0 - sv_store2
Subject to
- sv_constant1 + sv_start0 <= 0.000000
- sv_br1 + sv_constant1 <= 0.000000
- sv_br2 + sv_start0 <= 0.000000
- sv_constant2 + sv_phi1 <= 0.000000
- sv_constant6 + sv_phi1 <= 0.000000
- sv_constant7 + sv_phi1 <= 0.000000
- sv_addi0 + sv_phi0 <= 0.000000
- sv_addi0 + sv_constant6 <= 0.000000
sv_addi0 - sv_extui0 <=  -1.000000
sv_extui0 - sv_index_cast0 <= 0.000000
sv_index_cast0 - sv_load0 <= 0.000000
- sv_extui1 + sv_phi0 <= 0.000000
sv_extui1 - sv_index_cast1 <= 0.000000
sv_index_cast1 - sv_load1 <= 0.000000
sv_load0 - sv_muli0 <=  -1.000000
sv_load1 - sv_muli0 <=  -1.000000
- sv_extui2 + sv_phi0 <= 0.000000
sv_extui2 - sv_index_cast2 <= 0.000000
sv_muli0 - sv_store2 <=  -4.000000
sv_index_cast2 - sv_store2 <= 0.000000
- sv_addi1 + sv_phi0 <= 0.000000
- sv_addi1 + sv_constant7 <= 0.000000
sv_addi1 - sv_cmpi0 <=  -1.000000
- sv_cmpi0 + sv_constant2 <= 0.000000
sv_cmpi0 - sv_cond_br1 <= 0.000000
sv_addi1 - sv_cond_br1 <=  -1.000000
sv_cmpi0 - sv_cond_br2 <= 0.000000
- sv_cond_br2 + sv_phi1 <= 0.000000
sv_cmpi0 - sv_cond_br3 <= 0.000000
- sv_cond_br3 + sv_phi1 <= 0.000000
sv_cond_br3 - sv_return1 <= 0.000000
sv_cond_br1 - sv_phi0 <= 2.000000
sv_cond_br2 - sv_phi1 <= 2.000000
- sv_load0 + sv_store2 <= 1.000000


sv_addi0 >= 0.000000
sv_addi1 >= 0.000000
sv_br1 >= 0.000000
sv_br2 >= 0.000000
sv_cmpi0 >= 0.000000
sv_cond_br1 >= 0.000000
sv_cond_br2 >= 0.000000
sv_cond_br3 >= 0.000000
sv_constant1 >= 0.000000
sv_constant2 >= 0.000000
sv_constant6 >= 0.000000
sv_constant7 >= 0.000000
sv_extui0 >= 0.000000
sv_extui1 >= 0.000000
sv_extui2 >= 0.000000
sv_index_cast0 >= 0.000000
sv_index_cast1 >= 0.000000
sv_index_cast2 >= 0.000000
sv_load0 >= 0.000000
sv_load1 >= 0.000000
sv_muli0 >= 0.000000
sv_phi0 >= 0.000000
sv_phi1 >= 0.000000
sv_return1 >= 0.000000
sv_start0 >= 0.000000
sv_store2 >= 0.000000
General
sv_addi0 sv_addi1 sv_br1 sv_br2 sv_cmpi0 sv_cond_br1 sv_cond_br2 sv_cond_br3 sv_constant1 sv_constant2 sv_constant6 sv_constant7 sv_extui0 sv_extui1 sv_extui2 sv_index_cast0 sv_index_cast1 sv_index_cast2 sv_load0 sv_load1 sv_muli0 sv_phi0 sv_phi1 sv_return1 sv_start0 sv_store2 

End
