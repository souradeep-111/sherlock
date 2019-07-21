#ifndef computation_graph_h
#define computation_graph_h

#include <string>
#include <iostream>
#include <vector>
#include "nodes.h"
#include <mutex>
#include <thread>
#include <map>
#include <time.h>
#include <set>


class computation_graph
{
  private:
    map< uint32_t, node > all_nodes;
    vector< uint32_t > input_nodes;
    vector< uint32_t > output_nodes;
    uint32_t no_of_input_nodes, no_of_output_nodes;

  public:
    computation_graph();
    void clear();

    void add_new_node(uint32_t node_id, node & node_to_add);

    void mark_node_as_input(uint32_t input_node_number);
    void mark_node_as_output(uint32_t output_node_number);

    string return_node_position(uint32_t node_index);

    void connect_node1_to_node2_with_weight(uint32_t node_1_index, uint32_t node_2_index,
                                            datatype weight);
    void set_bias_of_node(uint32_t node_id, datatype bias);

    friend void evaluate_node(computation_graph & c_graph, uint32_t node_id , map< uint32_t , double > & table,
                              int& threads_available , double & ret_val, int thread_id);

    void evaluate_graph(map < uint32_t , double > input_node_and_value ,
                        map < uint32_t, double > & output_node_and_value);

    map< uint32_t, datatype > return_gradient_wrt_inputs(uint32_t node_id,
                                                         map < uint32_t, double > & input_node_and_value );

    friend void compute_gradient_wrt_inputs( computation_graph & c_graph,
                                             uint32_t node_id,
                                             map< uint32_t, double > & input_node_and_value,
                                             map< uint32_t, map< uint32_t,  double > > & memoized_table,
                                             int & available_threads,
                                             map< uint32_t, double > & result , int thread_id);


    map< uint32_t, node > & return_ref_to_all_nodes();
    void return_id_of_all_nodes(vector< uint32_t >& node_indices);
    void return_id_of_input_output_nodes(vector< uint32_t > & in_nodes , vector< uint32_t > & op_nodes );
    void return_id_of_nodes_at_depth_one_from_set( vector<uint32_t> current_set ,
                                                   set<uint32_t>& set_at_depth_one);
     void return_id_of_nodes_at_depth_one_from_set( set < uint32_t > current_set ,
                                                    set < uint32_t >& set_at_depth_one);
};



// Here is the plan for generating constraints from the computation graph you have.
// First, the terminal condition is the following :
// if at node which has assertions on it, then you just include it.
// Else, it includes the constraints which relates the inputs to the output of itself,
//  and then simply recurse on the inputs to that node . Plan to do that in a breadth first fashion.

#endif
