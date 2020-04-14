#include "nodes.h"

using namespace std;

map< uint32_t, bool > last_signature;

bool debug_nodes = true;
node :: node()
{
    node_type = constant;
    node_id = 0;
    string name = "node";
    node_name = name;
    forward_nodes.clear();
    backward_nodes.clear();
}

node :: node(uint32_t node_index, string type_name)
{
  assert(node_index > 0);
  assert(type_name.length() > 0);

  node_id = node_index;

  string buffer_name = "n_" + to_string(node_index);
  node_name = buffer_name;

  if(type_name == "constant")
  node_type = constant;
  else if(type_name == "tanh")
  node_type = _tanh_;
  else if(type_name == "sigmoid")
  node_type = _sigmoid_;
  else if(type_name == "relu")
  node_type = _relu_;
  else if(type_name == "none")
  node_type = _none_;
  else assert(false);


  forward_nodes.clear();
  backward_nodes.clear();
}

node :: node(uint32_t node_index)
{
  node_id = node_index;
  string buffer_name = "n_" + to_string(node_index);
  node_name = buffer_name;

  forward_nodes.clear();
  backward_nodes.clear();
}

int node :: get_node_number()
{
  return node_id;
}

string node :: get_node_name()
{
  assert(!node_name.empty());
  return node_name;
}

void node :: set_node_type(string type_name)
{
  assert(!type_name.empty());

  if(type_name == "constant")
  node_type = constant;
  else if(type_name == "tanh")
  node_type = _tanh_;
  else if(type_name == "sigmoid")
  node_type = _sigmoid_;
  else if(type_name == "relu")
  node_type = _relu_;
  else if(type_name == "none")
  node_type = _none_;
  else assert(false);
}

type node :: get_node_type()
{
  return node_type;
}

string node :: return_node_type()
{
  string return_name;

  if(node_type == constant)
  return_name = "constant";
  else if(node_type == _tanh_)
  return_name = "tanh";
  else if(node_type == _sigmoid_)
  return_name = "sigmoid";
  else if(node_type == _relu_)
  return_name = "relu";
  else if(node_type == _none_)
  return_name = "none";
  else assert(false);

  return return_name;
}

void node :: set_node_val(datatype value)
{
  assert(node_type == constant);
  constant_val = value;
}


void node :: add_forward_connection(node * forward_node_ptr, datatype weight)
{
  auto index_of_new_node = forward_node_ptr->get_node_number();
  pair< node * , datatype > connection = make_pair( forward_node_ptr, weight );
  forward_nodes.insert( make_pair( index_of_new_node, connection) );
}

void node :: add_backward_connection(node * backward_node_ptr, datatype weight)
{
  auto index_of_new_node = backward_node_ptr->get_node_number();
  pair< node * , datatype > connection = make_pair( backward_node_ptr, weight );
  backward_nodes.insert( make_pair( index_of_new_node, connection) );


}

void node :: set_bias(datatype bias_value)
{
  bias = bias_value;
}

void node :: get_bias(datatype & bias_value)
{
  bias_value = bias;
}

void node :: get_forward_connections(map< uint32_t , pair< node * , datatype > > & forward_nodes_container)
{
  if(forward_nodes.empty())
  {
    forward_nodes_container.clear();
    return;
  }

  forward_nodes_container = forward_nodes;
}
void node :: get_backward_connections(map< uint32_t , pair< node * , datatype > > & backward_nodes_container)
{

  assert(!backward_nodes.empty());
  if(backward_nodes.empty())
  {
    backward_nodes_container.clear();
    return;
  }
  backward_nodes_container = backward_nodes;
}


void node :: set_inputs( map < uint32_t, datatype > & inputs )
{
  assert(!(inputs.empty()));
  assert(backward_nodes.size() == inputs.size());
  current_inputs = inputs;
}

datatype node :: return_current_output(void)
{
  if(node_type == constant)
  {
    return constant_val;
  }

  datatype argument = bias;
  for(auto input_node : current_inputs)
  {
    argument += (backward_nodes[input_node.first].second * input_node.second ) ;
  }

  if(node_type == _tanh_ )
  {
    return tanh(argument);
  }
  else if(node_type == _sigmoid_ )
  {

    return (1.0/(1.0 + exp(-argument)));
  }
  else if (node_type == _relu_)
  {
    last_signature[node_id] = ((argument > 0) ? true : false) ;

    return (argument > 0) ? argument : 0 ;
  }
  else if(node_type == _none_)
  {
    return argument;
  }
  else
  {
    cout << "Node type not included in the list of evaluation functions ! Exiting....  " << endl;
    exit(0);
  }

}

map< uint32_t, datatype > node :: return_gradient(void)
{

  map < uint32_t, datatype > gradient_info;


  if(node_type == constant) // If constant then set the derivatives to zero
  {
    for(auto some_backward_node : backward_nodes)
    {
        gradient_info.insert( make_pair(some_backward_node.first, 0.0) );
    }
    return gradient_info;
  }

  datatype argument = bias;
  for( auto some_backward_node : backward_nodes ) // compute the weighted sum of the inputs
  {
    argument += ( ( some_backward_node.second.second ) * ( current_inputs[some_backward_node.first] )  ); // weights * input_value
  }




  if(node_type == _tanh_ )
  {
    for(auto some_node : backward_nodes)
    {
      gradient_info.insert( make_pair( some_node.first, some_node.second.second * (1 - pow(tanh(argument), 2)) ) );
    }

    return gradient_info;
  }
  else if(node_type == _sigmoid_ )
  {
    for(auto some_node : backward_nodes)
    {
      datatype buffer = (1.0/(1.0 + exp(-argument))) * (1.0 - (1.0/(1.0 + exp(-argument)))) ;
      gradient_info.insert( make_pair( some_node.first, some_node.second.second * buffer) );
    }

    return gradient_info;

  }
  else if (node_type == _relu_)
  {
    for(auto some_node : backward_nodes)
    {
      datatype buffer = ((argument > 0) ? 1.0 : 0.0 ) ;
      gradient_info.insert( make_pair( some_node.first, some_node.second.second * buffer) );
    }

    return gradient_info;
  }
  else if (node_type == _none_)
  {
    for(auto some_node : backward_nodes)
    {
      gradient_info.insert( make_pair( some_node.first, some_node.second.second ));
    }
    return gradient_info;
  }
  else
  {
    cout << "Node type not included in the list of evaluation functions for derivative ! Exiting....  " << endl;
    exit(0);
  }
}

node & node :: operator= (const node & rhs)
{
  this->node_type = rhs.node_type;
  this->node_id = rhs.node_id;
  this->node_name = rhs.node_name;
  this->current_inputs = rhs.current_inputs;
  this->current_outputs = rhs.current_outputs;
  this->current_gradient = rhs.current_gradient;
  this->bias = rhs.bias;
  this->constant_val = rhs.constant_val;

  this->forward_nodes = rhs.forward_nodes;
  this->backward_nodes = rhs.backward_nodes;

  return *this;
}


void print_map(map< uint32_t, double > some_map )
{
  cout << "[ " ;
  for(auto map_element : some_map)
  {
      cout <<  map_element.first  << " : " << map_element.second << " , ";
  }
  cout << " ] " << endl;
}
