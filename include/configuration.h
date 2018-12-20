#ifndef config_h
#define config_h

typedef  double datatype;
#define reach_set_color (" \'silver\' ")

struct parameter_values
{
  int no_of_sub_divisions; // 10
  double gradient_rate; // (1e-3) // 0.005
  double grad_scaling_factor; // (2e2)
  unsigned int grad_switch_count; // (1e3)
  double grad_termination_limit; // (1e-7)
  bool switch_to_modified_gradient_search; // (1)

  double num_tolerance;  // (1e-6)         // By how much different a point should be to be considered different,
                                     // used in 'check_limits' for exanding a given limit, and similarity checking
  double num_shift; // (0)
  double num_similar; // (1e-5)
  double delta_inflection; // (2e-3)    // For checking the inflection point

  double MILP_M ; // 1e10
  double MILP_tolerance ; // 5e-2 // 1e-3
  double MILP_e_tolerance; //(1e-25)  // 1e-6
  double epsilon_degeneracy; // (1e-10)
  bool do_dynamic_M_computation; // (1)
  double scale_factor_for_M ;  // (1)


  bool verbosity;  // (1)
  bool time_verbosity; // (0)
  bool grad_search_point_verbosity; // (0)


  int max_digits_in_var_names; // (2)
  double tool_zero; //(1e-30)
  double constr_comb_offset; // ((-4 * 100))
  double split_threshold; // (0.5)

  double LP_tolerance_limit; // (1e-4)
  int int_bias_for_RK ; // (1e2)


  bool skip_LP_jump; // (0)
  double LP_offset;// (0)
  bool do_incremental_MILP; // (1)
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
    split_threshold = 0.9;

    LP_tolerance_limit = 1e-4;
    int_bias_for_RK = 1e2;


    skip_LP_jump = false;
    LP_offset = 0.0;
    do_incremental_MILP = true;

    do_LP_certificate = false;
  }

  // void print_parameter_values()
  // {
  //   cout << "No of sub divisons = " << no_of_sub_divisions << endl;
  //   cout << "Gradient rate = " << gradient_rate << endl;
  //   cout << "Grad scaling factor = " << grad_scaling_factor << endl;
  //   cout << "grad switch count = " << grad_switch_count << endl;
  //   cout << "grad termination limit = " << grad_termination_limit << endl;
  //   cout << "switch to modified gradient search = " << switch_to_modified_gradient_search << endl;
  //   cout << "num tolerance = " << num_tolerance << endl;
  //   cout << "num shift = " << num_shift << endl;
  //   cout << "num_similar = " << num_similar << endl;
  //   cout << "delta_inflection = " << delta_inflection << endl;
  //   cout << "MILP M = " << MILP_M << endl;
  //   cout << "MILP tolerance = " << MILP_tolerance << endl;
  //   cout << "MILP e tolerance = " << MILP_e_tolerance << endl;
  //   cout << "epsilon degeneracy = " << epsilon_degeneracy << endl;
  //   cout << "do_dynamic_M_computation = " << do_dynamic_M_computation << endl;
  //   cout << "scale factor for M = " << scale_factor_for_M << endl;
  //   cout << "verbosity = " << verbosity << endl;
  //   cout << "time_verbosity = " << time_verbosity << endl;
  //   cout << "grad search point verbosity = " << grad_search_point_verbosity << endl;
  //   cout << "max_digits_in_var_names = " << max_digits_in_var_names << endl;
  //   cout << "tool zero = " << tool_zero << endl;
  //   cout << "constr comb offset = " << constr_comb_offset << endl;
  //   cout << "split threshold = " << split_threshold << endl;
  //   cout << "LP_tolerance_limit = " << LP_tolerance_limit << endl;
  //   cout << "int_bias_for_RK = " << int_bias_for_RK << endl;
  //   cout << "skip LP jump = " << skip_LP_jump << endl;
  //   cout << "LP offset = " << LP_offset << endl;
  //   cout << "do incremental MILP = " << do_incremental_MILP << endl;
  // }

};


#endif
