#ifndef config_h
#define datatype double
#define no_of_sub_divisions (10)  // Number of sub_divisions along a given interval, for
                                 // multi threading

#define gradient_rate (1e-3) // 0.005
#define grad_scaling_factor (2e2)
#define grad_switch_count (1e3)
#define grad_termination_limit (1e-7)
#define switch_to_modified_gradient_search (1)

#define num_tolerance (1e-6)         // By how much different a point should be to be considered different,
                                   // used in 'check_limits' for exanding a given limit, and similarity checking
#define num_shift (0)
#define num_similar (1e-5)
#define delta_inflection (2e-3)    // For checking the inflection point

#define MILP_M (1e10) // 1e10
#define MILP_tolerance (5e-2) // 1e-3
#define MILP_e_tolerance (1e-25)  // 1e-6
#define epsilon_degeneracy (1e-10)
#define do_dynamic_M_computation (1)
#define scale_factor_for_M (1)


#define verbosity (1)
#define time_verbosity (0)
#define grad_search_point_verbosity (0)


#define max_digits_in_var_names (2)
#define tool_zero (1e-30)
#define constr_comb_offset ((-4 * 100))
#define split_threshold (0.5)

#define LP_tolerance_limit (1e-4)
#define int_bias_for_RK (1e2)


#define skip_LP_jump (0)
#define LP_offset (0)
#define do_incremental_MILP (1)
#endif

/* Pending improvements
-> Using the input constraint to clear off a
  large part of the network, by just doing simple interval/
  affine arithmetic based range propagation

*/

/*
Things to do :

merge_networks()

THe LP infeasibility Things

*/

/*
What the code cannot do :
-> handle degeneracies in directions other than the principal
directions
*/
