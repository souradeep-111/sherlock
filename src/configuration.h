/*

Contributors to the tool : 
Souradeep Dutta

email : souradeep.dutta@colorado.edu

LICENSE : Please see the license file, in the main directory

*/

#ifndef config_h
#define config_h
#define datatype double

struct parameter_values
{
  int no_of_sub_divisions;
  double gradient_rate; 
  double grad_scaling_factor; 
  unsigned int grad_switch_count; 
  double grad_termination_limit; 
  bool switch_to_modified_gradient_search; 

  double num_tolerance;  
  double num_shift; 
  double num_similar;
  double delta_inflection;    // For checking the inflection point

  double MILP_M ;
  double MILP_tolerance ;
  double MILP_e_tolerance;
  double epsilon_degeneracy; 
  bool do_dynamic_M_computation;
  double scale_factor_for_M ; 


  bool verbosity;  
  bool time_verbosity; 
  bool grad_search_point_verbosity; 


  int max_digits_in_var_names; 
  double tool_zero; 
  double constr_comb_offset; 
  double split_threshold; 

  double LP_tolerance_limit; 
  int int_bias_for_RK ; 


  bool skip_LP_jump; 
  double LP_offset;
  bool do_incremental_MILP; 
  bool do_LP_certificate;

  parameter_values()
  {
    no_of_sub_divisions = 10;
    gradient_rate = 1e-3;
    grad_scaling_factor = 2e2;
    grad_switch_count = 1e3;
    grad_termination_limit = 1e-7;
    switch_to_modified_gradient_search = true;

    num_tolerance = 1e-6;
    num_shift = 0.0;
    num_similar = 1e-5;
    delta_inflection = 2e-3;

    MILP_M = 1e10;
    MILP_tolerance = 5e-2;
    MILP_e_tolerance = 1e-25;
    epsilon_degeneracy = 1e-10;
    do_dynamic_M_computation = true;
    scale_factor_for_M = 1.0;


    verbosity = false;
    time_verbosity = false;
    grad_search_point_verbosity = false;


    max_digits_in_var_names = 2;
    tool_zero = 1e-30;
    constr_comb_offset = -400;
    split_threshold = 0.5;

    LP_tolerance_limit = 1e-4;
    int_bias_for_RK = 1e2;


    skip_LP_jump = false;
    LP_offset = 0.0;
    do_incremental_MILP = true;

    do_LP_certificate = false;
  }

};


#endif
