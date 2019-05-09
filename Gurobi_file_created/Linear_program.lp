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
 n_5_binary_sum_constraint_: n_5_delta_1_ + n_5_delta_2_ + n_5_delta_3_
   = 1
 n_5_output_upper_: n_5 <= 1
 n_5_output_lower_: n_5 >= 0
 n_5_left_input_: n_5_ip_sum + 1e+06 n_5_delta_1_
   - 999997.6999999 const_name <= 0
 n_5_left_output_: n_5 + 1e+06 n_5_delta_1_ - 1.0000001e+06 const_name
   <= 0
 n_5_middle_input_a_: n_5_ip_sum + 1e+06 n_5_delta_2_
   - 1.0000022999999e+06 const_name <= 0
 n_5_middle_input_b_: n_5_ip_sum - 1e+06 n_5_delta_2_
   + 1.0000022999999e+06 const_name >= 0
 n_5_middle_output_a_: n_5 - 0.2 n_5_ip_sum + 1e+06 n_5_delta_2_
   - 1.00000055e+06 const_name <= 0
 n_5_middle_output_b_: n_5 - 0.2 n_5_ip_sum - 1e+06 n_5_delta_2_
   + 999999.55 const_name >= 0
 n_5_right_input_: n_5_ip_sum - 1e+06 n_5_delta_3_
   + 999997.6999999 const_name >= 0
 n_5_right_output_: n_5 - 1e+06 n_5_delta_3_ + 999999.1 const_name >= 0
 n_3_sum_constr_: - 10 n_1 - 10 n_2 - n_3_ip_sum = 0
 n_3_binary_sum_constraint_: n_3_delta_1_ + n_3_delta_2_ + n_3_delta_3_
   = 1
 n_3_output_upper_: n_3 <= 1
 n_3_output_lower_: n_3 >= 0
 n_3_left_input_: n_3_ip_sum + 1e+06 n_3_delta_1_
   - 999997.6999999 const_name <= 0
 n_3_left_output_: n_3 + 1e+06 n_3_delta_1_ - 1.0000001e+06 const_name
   <= 0
 n_3_middle_input_a_: n_3_ip_sum + 1e+06 n_3_delta_2_
   - 1.0000022999999e+06 const_name <= 0
 n_3_middle_input_b_: n_3_ip_sum - 1e+06 n_3_delta_2_
   + 1.0000022999999e+06 const_name >= 0
 n_3_middle_output_a_: n_3 - 0.2 n_3_ip_sum + 1e+06 n_3_delta_2_
   - 1.00000055e+06 const_name <= 0
 n_3_middle_output_b_: n_3 - 0.2 n_3_ip_sum - 1e+06 n_3_delta_2_
   + 999999.55 const_name >= 0
 n_3_right_input_: n_3_ip_sum - 1e+06 n_3_delta_3_
   + 999997.6999999 const_name >= 0
 n_3_right_output_: n_3 - 1e+06 n_3_delta_3_ + 999999.1 const_name >= 0
 n_4_sum_constr_: 10 n_1 + 0.5 n_2 - n_4_ip_sum = 0
 n_4_binary_sum_constraint_: n_4_delta_1_ + n_4_delta_2_ + n_4_delta_3_
   = 1
 n_4_output_upper_: n_4 <= 1
 n_4_output_lower_: n_4 >= 0
 n_4_left_input_: n_4_ip_sum + 1e+06 n_4_delta_1_
   - 999997.6999999 const_name <= 0
 n_4_left_output_: n_4 + 1e+06 n_4_delta_1_ - 1.0000001e+06 const_name
   <= 0
 n_4_middle_input_a_: n_4_ip_sum + 1e+06 n_4_delta_2_
   - 1.0000022999999e+06 const_name <= 0
 n_4_middle_input_b_: n_4_ip_sum - 1e+06 n_4_delta_2_
   + 1.0000022999999e+06 const_name >= 0
 n_4_middle_output_a_: n_4 - 0.2 n_4_ip_sum + 1e+06 n_4_delta_2_
   - 1.00000055e+06 const_name <= 0
 n_4_middle_output_b_: n_4 - 0.2 n_4_ip_sum - 1e+06 n_4_delta_2_
   + 999999.55 const_name >= 0
 n_4_right_input_: n_4_ip_sum - 1e+06 n_4_delta_3_
   + 999997.6999999 const_name >= 0
 n_4_right_output_: n_4 - 1e+06 n_4_delta_3_ + 999999.1 const_name >= 0
 n_6_sum_constr_: n_4 - n_6_ip_sum = 0
 n_6_binary_sum_constraint_: n_6_delta_1_ + n_6_delta_2_ + n_6_delta_3_
   = 1
 n_6_output_upper_: n_6 <= 1
 n_6_output_lower_: n_6 >= 0
 n_6_left_input_: n_6_ip_sum + 1e+06 n_6_delta_1_
   - 999997.6999999 const_name <= 0
 n_6_left_output_: n_6 + 1e+06 n_6_delta_1_ - 1.0000001e+06 const_name
   <= 0
 n_6_middle_input_a_: n_6_ip_sum + 1e+06 n_6_delta_2_
   - 1.0000022999999e+06 const_name <= 0
 n_6_middle_input_b_: n_6_ip_sum - 1e+06 n_6_delta_2_
   + 1.0000022999999e+06 const_name >= 0
 n_6_middle_output_a_: n_6 - 0.2 n_6_ip_sum + 1e+06 n_6_delta_2_
   - 1.00000055e+06 const_name <= 0
 n_6_middle_output_b_: n_6 - 0.2 n_6_ip_sum - 1e+06 n_6_delta_2_
   + 999999.55 const_name >= 0
 n_6_right_input_: n_6_ip_sum - 1e+06 n_6_delta_3_
   + 999997.6999999 const_name >= 0
 n_6_right_output_: n_6 - 1e+06 n_6_delta_3_ + 999999.1 const_name >= 0
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
 n_5_delta_1_ n_5_delta_2_ n_5_delta_3_ n_3_delta_1_ n_3_delta_2_
 n_3_delta_3_ n_4_delta_1_ n_4_delta_2_ n_4_delta_3_ n_6_delta_1_
 n_6_delta_2_ n_6_delta_3_
End
