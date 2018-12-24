#ifndef nodes_h
#define nodes_h

#include "configuration.h"
#include <string>
#include <iostream>
#include <queue>
#include <fstream>
#include <thread>
#include <mutex>
#include <ctime>
#include <ratio>
#include <chrono>
#include <assert.h>
#include "network_computation.h"
#include "gurobi_interface.h"

class node{
  private:
    enum type {constant, _tanh_, _sigmoid_, _relu_ };
    type node_type;

    uint32_t node_id;
    string node_name;
    //  Computation specific values
    vector< datatype > current_inputs;
    datatype current_outputs;
    vector< datatype > current_gradient;
    datatype bias;

    // Pointers backward and forward
    map < int, pair< node * , datatype > > forward_nodes;
    map < int, pair< node * , datatype > > backward_nodes;

  public:
    node();
    node(int node_index,string type_name); // type_name = {"constant_node", "computation_node"}

    void add_forward_connection(node * forward_node_ptr, datatype weight );
    void add_backward_connection(node * backward_node_ptr, datatype weight );
    void set_bias(datatype bias_value);

    void set_inputs(vector< datatype > inputs);
    datatype return_current_output(void);
    vector< datatype > return_gradient(void);

}


// Here is a list of capabilities I want from the objects here :
// There are two types of nodes, constant nodes, and computation nodes
// This is for the computation type nodes :
// internally it stores the input, output, and gradient
//  whenever it's queried it should return the outputs, and/or gradient when required
// and has pointers to the previous nodes it receives it's inputs from, and
// another set of forward pointers, Not required, but just in case it's needed.



#endif
