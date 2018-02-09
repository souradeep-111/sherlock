#ifndef gurobi_interface_h
#define gurobi_interface_h

#include "gurobi_c++.h"
#include "network_computation.h"
#include <iostream>
#include <math.h>
#include <limits>
#include "configuration.h"
using namespace std;
extern parameter_values sherlock_parameters;

int prove_limit_in_NN(
  vector< vector< datatype > > region_constraints,
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  datatype limit_found,
  vector< datatype > extrema_point,
  int direction
);
int find_counter_example_in_NN(
  vector< vector< datatype > > region_constraints,
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  vector< datatype >& counter_example,
  datatype& limit_found,
  int direction
);
void do_network_encoding(
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  GRBModel * net_model_ptr,
  GRBEnv * net_env_ptr,
  vector< GRBVar >& input_variables,
  GRBVar & output_variable
);
datatype do_MILP_optimization(
  vector< vector< datatype > > region_constraints,
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  vector< datatype >& counter_example,
  // datatype& limit_found,
  int direction
);
int count_digits(int n);
void produce_string_for_variable_index(
  string & return_name,
  unsigned int layer_no,
  unsigned int var_no,
  unsigned int variable_type
);
void erase_line();
int find_if_constraint_matters(
  vector< vector< datatype > > constraint_set,
  vector< datatype > constraint,
  vector< vector< datatype > > target_region,
  datatype& degree_of_matter
);
void find_size_of_enclosed_region_in_direction(
  vector< vector< datatype > > constraint_set,
  vector< int > direction_vector,
  datatype& region_amount
);
int run_optimization(
  vector< vector< datatype > > constraints,
  vector< datatype > objective,
  datatype obj_bias,
  datatype& maximum,
  vector< datatype >& max_point,
  datatype& minimum,
  vector< datatype >& min_point
);
void optimize(
  vector< vector< datatype > > constraints,
  vector< datatype > objective,
  datatype obj_bias,
  int direction,
  datatype& extrema,
  vector< datatype>& extrema_point
);
int find_whether_overlap(
  vector< vector< datatype > > constraint_set_1,
  vector< vector< datatype > > constraint_set_2
);
int find_size_inside_target(
  vector< vector< datatype > > constraint_set,
  vector< vector< datatype > > target_region,
  datatype& size
);
int find_size(
  vector< vector< datatype > > constraint_set,
  datatype& size
);
void find_the_non_overlap(
  vector< vector< datatype > > main_region,
  vector< vector< datatype > > region_to_subtract,
  vector< vector< datatype > >& non_overlap
);
#endif
