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
#include "network_computation.h"
#include "gurobi_interface.h"

class node{
private:
  enum type {constant, computation};
  type node_type;

  //  Computation specific values
  datatype current_inputs;
  datatype current_outputs;
  datatype current_gradient;

  // Pointers backward and forward
  map < int, pair< node * , datatype > > forward_pointers;

  vector < node * > backward_pointers;

public:
  node();
  node(string type_name); // type_name = {"constant_node", "computation_node"}


}


// Here is a list of capabilities I want from the objects here :
// There are two types of nodes, constant nodes, and computation nodes
// This is for the computation type nodes :
// internally it stores the input, output, and gradient
//  whenever it's queried it should return the outputs, and/or gradient when required
// and has pointers to the previous nodes it receives it's inputs from, and
// another set of forward pointers, Not required, but just in case it's needed.



#endif
