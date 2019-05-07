#ifndef config_h
#define config_h

typedef  double datatype;
#define reach_set_color (" \'silver\' ")

struct parameter_values
{
  int exponential_limit_upper;
  int exponential_limit_lower;
  int triggering_limit;
  double gradient_rate;
  unsigned int grad_switch_count;
  double grad_termination_limit;
  bool switch_to_modified_gradient_search;

  double num_tolerance;
  double num_similar;
  double delta_inflection;

  double epsilon;
  double int_tolerance;
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
  double tool_high;
  double constr_comb_offset; // ((-4 * 100))
  double split_threshold; // (0.5)

  double LP_tolerance_limit; // (1e-4)
  int int_bias_for_RK ; // (1e2)


  bool skip_LP_jump; // (0)
  double LP_offset;// (0)
  bool do_incremental_MILP; // (1)
  bool do_LP_certificate;


  int thread_count;
  int thread_count_for_constraint_generation;
  bool do_signed_gradient;
  bool do_random_restarts;
  int no_of_random_restarts;

  int random_counter_example_count;
  bool encode_relu_new;





  parameter_values()
  {
    triggering_limit = 5;
    exponential_limit_upper = 10;
    exponential_limit_lower = -10;
    gradient_rate = 1e-3;
    grad_termination_limit = 1e-7;
    switch_to_modified_gradient_search = true;

    num_tolerance = 1e-6;
    num_similar = 1e-5;
    delta_inflection = 2e-3;

    // 0 >> M \times tolerance
    // epsilon < M \times \tolerance
    epsilon = 1e-7;
    int_tolerance = 1e-9;
    MILP_M = 1e6;
    MILP_tolerance = 5e-2;
    MILP_e_tolerance = 1e-25;
    epsilon_degeneracy = 1e-10;
    do_dynamic_M_computation = true;
    scale_factor_for_M = 1.0;


    verbosity = true;
    time_verbosity = false;
    grad_search_point_verbosity = true;


    max_digits_in_var_names = 2;
    tool_zero = 1e-30;
    tool_high = 1e30;
    constr_comb_offset = -400;
    split_threshold = 0.9;

    LP_tolerance_limit = 1e-4;
    int_bias_for_RK = 1e2;


    skip_LP_jump = false;
    LP_offset = 0.0;
    do_incremental_MILP = true;

    do_LP_certificate = false;

    thread_count = 4;
    thread_count_for_constraint_generation = 5;

    do_signed_gradient = true;
    do_random_restarts = true;
    no_of_random_restarts = 10;
    random_counter_example_count = 1e2;

    encode_relu_new = true;


  }


};


#endif
