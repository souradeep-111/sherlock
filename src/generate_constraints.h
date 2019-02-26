#ifndef generate_constraints_h
#define generate_constraints_h

#include "gurobi_c++.h"
#include <iostream>
#include <math.h>
#include <limits>
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
  map< uint32_t, pair< double , double > > input_ranges;

public:

  uint32_t nodes_explored_last_optimization;
  constraints_stack();
  void feed_computation_graph(computation_graph & CG);
  void create_the_input_overapproximation();
  void generate_graph_constraints();
  void generate_node_constraints();
  void relate_input_output(type node_type, GRBVar input_var, GRBVar output_var);
  void delete_and_reinitialize();
  void add_invariants();

  void optimize(uint32_t node_number, bool direction);
  void optimize_enough(uint32_t node_number, double level);

  friend void add_constraints_for_node();
};

#endif
