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
#include <string>
#include <ratio>
#include <chrono>
#include <assert.h>
#include <math.h>
#include <algorithm>
#include "network_computation.h"
#include "gurobi_interface.h"
#include <map>

enum type {constant, _tanh_, _sigmoid_, _relu_, _none_ };
const string const_string("constant");
const string tanh_string("tanh");
const string sigmoid_string("sigmoid");
const string relu_string("relu");
const string none_string("none");


class node{
  private:

    // Node identifiers
    type node_type;
    uint32_t node_id;
    string node_name;


    //  Computation specific values
    map< uint32_t, datatype > current_inputs;
    datatype current_outputs;
    map< uint32_t, datatype > current_gradient;
    datatype bias;
    datatype constant_val;

    // Pointers backward and forward
    map < uint32_t, pair< node * , datatype > > forward_nodes;
    map < uint32_t, pair< node * , datatype > > backward_nodes;

  public:
    node();
    node(uint32_t node_index,string type_name); // type_name = {"constant_node", computation_node}
    node(uint32_t node_index);
    int get_node_number();
    string get_node_name();
    void set_node_type(string type_name);
    string return_node_type();
    type get_node_type();

    void set_node_val(datatype value);

    void add_forward_connection(node * forward_node_ptr, datatype weight );
    void add_backward_connection(node * backward_node_ptr, datatype weight );
    void set_bias(datatype bias_value);
    void get_bias(datatype & bias_value);
    void get_forward_connections(map< uint32_t , pair< node * , datatype > > & forward_nodes_container);
    void get_backward_connections(map< uint32_t , pair< node * , datatype > > & backward_nodes_container);

    void set_inputs( map < uint32_t, datatype > & inputs);
    datatype return_current_output(void);
    map< uint32_t, datatype > return_gradient(void);
    node & operator= (const node & rhs);
};

void print_map(map< uint32_t, double > some_map );

// Here is a list of capabilities I want from the objects here :
// There are two types of nodes, constant nodes, and computation nodes
// This is for the computation type nodes :
// internally it stores the input, output, and gradient
//  whenever it's queried it should return the outputs, and/or gradient when required
// and has pointers to the previous nodes it receives it's inputs from, and
// another set of forward pointers, Not required, but just in case it's needed.



#endif
