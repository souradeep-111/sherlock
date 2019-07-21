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
#include "network_signatures.h"
#include <mutex>
#include <thread>
#include <set>
using namespace std;

extern parameter_values sherlock_parameters;

class constraints_stack
{
  protected:
    GRBEnv * env_ptr;
    GRBModel * model_ptr;
    map< uint32_t, GRBVar > neurons;
    map< uint32_t, GRBVar > binaries;
    // vector< uint32_t > indices_of_all_nodes;
    // vector< uint32_t > input_indices, output_indices;

    map< uint32_t, pair< double , double > > neuron_bounds;
    region_constraints input_region;

  public:

    vector< int > skip_activation_encoding_for_index;
    map<uint32_t, bool> binarize;
    uint32_t nodes_explored_last_optimization;

    constraints_stack();
    void create_the_input_overapproximation_for_each_neuron(
                                computation_graph & CG,
                                region_constraints & input_region);
    void generate_graph_constraints(region_constraints & region,
                                    computation_graph & CG,
                                    uint32_t output_node_id);

    void generate_node_constraints(computation_graph & CG,
                                   vector< uint32_t > explored_nodes,
                                   uint32_t output_node_id);

    void relate_input_output(node current_node,
                             GRBVar input_var, GRBVar output_var,
                             GRBModel * model_ptr);

    void _delete_();
    void delete_and_reinitialize();
    void add_invariants(computation_graph & neural_network,
                        region_constraints & input_region);

    bool optimize(uint32_t node_index, bool direction,
                  map< uint32_t, double >& neuron_value,
                  double & result);

    bool optimize_enough(uint32_t node_index,
                         double& current_optima, bool direction,
                         map< uint32_t, double >& neuron_value);

    double get_M_val_for_node(uint32_t node_index);
    void create_sum_of_inputs_and_return_var(map< uint32_t, GRBVar > & inputs_to_node,
                                             node current_node,
                                             GRBVar & sum_variable,
                                             GRBModel * model_ptr);

     void check_constant_neurons(computation_graph & neural_network,
                                 region_constraints & input_region,
                                 set< uint32_t > & always_on,
                                 set< uint32_t > & always_off);
     void check_constant_neurons(computation_graph & neural_network,
                                  region_constraints & input_region,
                                  set< uint32_t > & pre_set_to_on,
                                  set< uint32_t > & pre_set_to_off,
                                  set< uint32_t > & always_on,
                                  set< uint32_t > & always_off);

     void add_pairwise_neurons(set< pair< uint32_t, uint32_t > > & same_sense_nodes,
                               set< pair< uint32_t, uint32_t > > & opposite_sense_nodes);


    void check_pairwise_relationship(set< pair< uint32_t, uint32_t > > & same_sense_nodes,
                                     set< pair< uint32_t, uint32_t > > & opposite_sense_nodes );

    void add_implication_neurons(set< pair< uint32_t, uint32_t > > & same_sense_nodes,
                                 set< pair< uint32_t, uint32_t > > & opposite_sense_nodes);

    void check_implies_relationship(computation_graph & neural_network,
                                    region_constraints & input_region,
                                    set< pair< uint32_t, uint32_t > > & true_implication,
                                    set< pair< uint32_t, uint32_t > > & false_implication);


    void add_constant_neurons(set<uint32_t>& always_on, set<uint32_t>& always_off);

    void add_linear_constraint(linear_inequality & lin_ineq);

    friend void add_constraints_for_node(constraints_stack & CS,
                                         uint32_t current_node_id,
                                         computation_graph & CG,
                                         GRBModel * model_ptr,
                                         set < uint32_t >& nodes_to_explore );

};

class relaxed_constraints_stack : public constraints_stack
{

  public:

    // using constraints_stack :: relate_input_output;
    // void relate_input_output_generalized(node current_node,
    //                          GRBVar input_var, GRBVar output_var,
    //                          GRBModel * model_ptr);

    using constraints_stack :: generate_graph_constraints;
    void generate_graph_constraints(
                             region_constraints &  region,
                             computation_graph & CG,
                             set< uint32_t > output_nodes);

    using constraints_stack :: generate_node_constraints;
    void generate_node_constraints(
                             computation_graph & CG,
                             vector< uint32_t > explored_nodes,
                             set< uint32_t > output_nodes);

    bool check_implies_relation(bool sense, uint32_t node_1_index,
                                uint32_t node_2_index);

    void search_constant_nodes_incrementally(computation_graph & CG,
                                              region_constraints input_region,
                                              set< uint32_t > & on_neurons,
                                              set< uint32_t > & off_neurons);


};

class selective_binarization : public constraints_stack
{
public:
//
};

template <typename T>
set<T> getUnion(const set<T>& a, const set<T>& b);


#endif
