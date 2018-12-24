#include "nodes.h"

using namespace std;

node :: node()
{
    node_type = constant;
    node_id = 0;
    string name = "node";
    node_name = name;
    forward_nodes.clear();
    backward_nodes.clear();
}

node :: node(int node_index, string type_name)
{
  assert(node_index > 0);
  assert(strlen(type_name) > 0);

  node_id = node_index;
  node_name = type_name;

  forward_nodes.clear();
  backward_nodes.clear();
}


void node :: add_forward_connection(node * forward_node_ptr, datatype weight)
{
  index_of_new_node = forward_nodes.size();
  pair< node * , datatype > connection = makepair( forward_node_ptr, weight );
  forward_nodes.push_back( makepair( index_of_new_node, connection) );
}

void node :: add_backward_connection(node * backward_node_ptr, datatype weight)
{
  index_of_new_node = backward_nodes.size();
  pair< node * , datatype > connection = makepair( backward_node_ptr, weight );
  backward_nodes.push_back( makepair( index_of_new_node, connection) );
}

void node :: set_bias(datatype bias_value)
{
  bias = bias_value;
}

void node :: set_inputs( vector < datatype > inputs )
{
  assert(!(inputs.empty()))
  assert(backward_nodes.size() == inputs.size());
  current_inputs = inputs;
}
