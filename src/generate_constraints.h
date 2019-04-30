#ifndef generate_constraints_h
#define generate_constraints_h

#include "gurobi_c++.h"
#include <iostream>
#include <math.h>
#include <limits>
#include <algorithm>
#include "configuration.h"
#include <map>
#include "computation_graph.h"
#include "region_constraints.h"
#include <mutex>
#include <thread>
#include <set>
using namespace std;

extern parameter_values sherlock_parameters;

class constraints_stack
{
private:
  GRBEnv * env_ptr;
  GRBModel * model_ptr;
  map< uint32_t, GRBVar > neurons;
  vector< GRBVar > binaries;
  map< uint32_t, node > & all_nodes;
  vector< uint32_t > & input_indices, output_indices;

  map< uint32_t, pair< double , double > > neuron_bounds;
  region_constraints & input_region;

public:

  uint32_t nodes_explored_last_optimization;
  constraints_stack();
  void feed_computation_graph(computation_graph & CG);
  void create_the_input_overapproximation_for_each_neuron(
                              region_constraints & input_region);
  void generate_graph_constraints();
  void generate_node_constraints();
  void relate_input_output(type node_type, GRBVar input_var,
                           GRBVar output_var);
  void delete_and_reinitialize();
  void add_invariants();

  bool optimize(uint32_t node_index, bool direction,
                map< uint32_t, double >& neuron_value,
                double & result);

  bool optimize_enough(uint32_t node_index,
                       double current_optima, bool direction,
                       map< uint32_t, double > neuron_value);

  double get_M_val_for_node(uint32_t node_index);
  void create_sum_of_inputs_and_return_var(map< uint32_t, GRBVar > & inputs_to_node,
                                           node current_node,
                                           GRBVar & sum_variable,
                                           GRBModel * model_ptr);

  friend void add_constraints_for_node();
};

#endif
