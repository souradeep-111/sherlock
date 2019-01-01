#ifndef generate_constraints_h
#define generate_constraints_h

#include "gurobi_c++.h"
#include <iostream>
#include <math.h>
#include <limits>
#include "configuration.h"
#include <map>
using namespace std;

class constraints_stack
{
private:
  // some gurobi object
  // some variable list for the real variables involved in the encoding
  // some variaable list for the binary variables involved in the encoding
  // some reference to all the nodes in the computation graph
  // something which keeps track of the big - M for every node
public:

  // some constructor
  // some function which adds all the nodes in the graph to an instance of this class
  // something which does a b-f-s traversal over the graph and creates the big-M things
  // something which does a b-f-s traversal over the graph and generate the constraints
  // something which relates the inputs and outputs of a node depending on which type of node it is
  // and given the variables for the inputs and outputs, and also add the extra binary variables required
};

#endif
