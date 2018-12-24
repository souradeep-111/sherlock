#ifndef computation_graph_h
#define computation_graph_h

#include <string>
#include <iostream>
#include <vector>
#include "node.h"

class computation_graph
{
  private:
    vector< nodes > all_nodes;
  public:
    computation_graph();
    void mark_node_as_input(int input_node_number);
    void mark_node_as_output(int output_node_number);
    void add_new_node(int node_number);
    void add_new_node(string node_name);
    void connect_node1_to_node2_with_weight(int node_1_index, int node_2_index);


    void evaluate_graph(map < int , double > input_node_and_value );
    vector< datatype > return_gradient_wrt_inputs(int node_index,  map < int, double > input_node_and_value );


};

// Here is the plan for generating constraints from the computation graph you have.
// First, the terminal condition is the following :
// if at node which has assertions on it, then you just include it.
// Else, it includes the constraints which relates the inputs to the output of itself,
//  and then simply recurse on the inputs to that node . Plan to do that in a breadth first fashion.

#endif
