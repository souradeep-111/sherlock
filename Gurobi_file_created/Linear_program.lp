\ LP format - for model browsing. Use MPS format to capture full model detail.
Minimize
  0 grb_one_input + 0 grb_one_input + n_7 + 0 const_name + 0 const_name
   + 0 const_name + 0 const_name + 0 const_name
Subject To
 _some_input_constraint_: - n_1 + 5 grb_one_input >= 0
 _some_input_constraint_: n_1 >= 0
 _some_input_constraint_: - n_2 + 5 grb_one_input >= 0
 _some_input_constraint_: n_2 >= 0
 n_7_sum_constr_: - n_7_ip_sum + 0.5 n_5 + 0.5 n_6 = 0
 n_7_ip_op_constr_: - n_7 + n_7_ip_sum = 0
 n_5_sum_constr_: - n_5_ip_sum + n_3 = 0
 n_5_ip_op_constr_a_: n_5 - n_5_ip_sum - 1e+06 n_5_delta_ <= 0
 n_5_ip_op_constr_b_: n_5 - n_5_ip_sum >= 0
 n_5_ip_op_constr_c_: n_5 >= 0
 n_5_ip_op_constr_d_: n_5 + 1e+06 n_5_delta_ - 1e+06 const_name <= 0
 n_5_trick_lower: n_5_ip_sum + 1e+06 n_5_delta_ - 1e-07 const_name >= 0
 n_5_trick_upper: n_5_ip_sum + 1e+06 n_5_delta_ - 999999.9999999 const_name
   <= 0
 n_3_sum_constr_: - 10 n_1 - 10 n_2 - n_3_ip_sum = 0
 n_3_ip_op_constr_a_: n_3 - n_3_ip_sum - 1e+06 n_3_delta_ <= 0
 n_3_ip_op_constr_b_: n_3 - n_3_ip_sum >= 0
 n_3_ip_op_constr_c_: n_3 >= 0
 n_3_ip_op_constr_d_: n_3 + 1e+06 n_3_delta_ - 1e+06 const_name <= 0
 n_3_trick_lower: n_3_ip_sum + 1e+06 n_3_delta_ - 1e-07 const_name >= 0
 n_3_trick_upper: n_3_ip_sum + 1e+06 n_3_delta_ - 999999.9999999 const_name
   <= 0
 n_4_sum_constr_: 10 n_1 + 0.5 n_2 - n_4_ip_sum = 0
 n_4_ip_op_constr_a_: n_4 - n_4_ip_sum - 1e+06 n_4_delta_ <= 0
 n_4_ip_op_constr_b_: n_4 - n_4_ip_sum >= 0
 n_4_ip_op_constr_c_: n_4 >= 0
 n_4_ip_op_constr_d_: n_4 + 1e+06 n_4_delta_ - 1e+06 const_name <= 0
 n_4_trick_lower: n_4_ip_sum + 1e+06 n_4_delta_ - 1e-07 const_name >= 0
 n_4_trick_upper: n_4_ip_sum + 1e+06 n_4_delta_ - 999999.9999999 const_name
   <= 0
 n_6_sum_constr_: n_4 - n_6_ip_sum = 0
 n_6_ip_op_constr_a_: n_6 - n_6_ip_sum - 1e+06 n_6_delta_ <= 0
 n_6_ip_op_constr_b_: n_6 - n_6_ip_sum >= 0
 n_6_ip_op_constr_c_: n_6 >= 0
 n_6_ip_op_constr_d_: n_6 + 1e+06 n_6_delta_ - 1e+06 const_name <= 0
 n_6_trick_lower: n_6_ip_sum + 1e+06 n_6_delta_ - 1e-07 const_name >= 0
 n_6_trick_upper: n_6_ip_sum + 1e+06 n_6_delta_ - 999999.9999999 const_name
   <= 0
Bounds
 n_1 free
 n_2 free
 grb_one_input = 1
 grb_one_input = 1
 grb_one_input = 1
 grb_one_input = 1
 n_7 free
 n_7_ip_sum free
 n_5 free
 n_6 free
 const_name = 1
 n_5_ip_sum free
 n_3 free
 n_4 free
 const_name = 1
 const_name = 1
 n_3_ip_sum free
 const_name = 1
 const_name = 1
 n_4_ip_sum free
 const_name = 1
 const_name = 1
 n_6_ip_sum free
 const_name = 1
 const_name = 1
Binaries
 n_5_delta_ n_3_delta_ n_4_delta_ n_6_delta_
End
