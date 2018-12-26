#include "computation_graph.h"

computation_graph :: computation_graph()
{
  all_nodes.clear();
  input_nodes.clear();
  output_nodes.clear();
  no_of_input_nodes = 0;
  no_of_output_nodes = 0;
}


void computation_graph :: add_new_node(uint32_t node_id, node & node_to_add)
{
  if(all_nodes.find(node_id) == all_nodes.end())
  {
    all_nodes.push_back( make_pair(node_id, node_to_add) );
  }
  else
  {
    cout << "Node id to be added already present ! " << endl;
    assert(false);
  }

}

void computation_graph :: mark_node_as_input(uint32_t node_id)
{
  // assertion to check that the node is already there in the list
  assert( all_nodes.find(node_id) != all_nodes.end());
  input_nodes.push_back( node_id );
  no_of_input_nodes++;
}

void computation_graph :: mark_node_as_output(uint32_t node_id)
{
  // assertion to check that the node is already there in the list
  assert( all_nodes.find(node_id) != all_nodes.end());
  output_nodes.push_back( node_id );
  no_of_output_nodes++;
}

void computation_graph :: connect_node1_to_node2_with_weight(
  uint32_t node_1_index, uint32_t node_2_index, datatype weight)
{
  // assert that these nodes are already there in the list
  assert( (all_nodes.find(node_1_index) != all_nodes.end())
  && (all_nodes.find(node_2_index) != all_nodes.end()) );

  auto node_1, node_2;
  node_1 = all_nodes[node_1_index];
  node_2 = all_nodes[node_2_index];

  node_1.add_forward_connection(& node_2 , weight);
  node_2.add_backward_connection(& node_1 , weight);

}

void computation_graph :: set_bias_of_node(node node_id, datatype bias)
{
  // assert that these nodes are already there in the list
  assert( all_nodes.find(node_id) != all_nodes.end() );
  node_1 = all_nodes[node_id];
  node_1.set_bias(bias);
}


void computation_graph :: evaluate_graph(map < uint32_t, double > input_node_and_value,
                                         map < uint32_t, double > & output_node_and_value )
{
  // Making sure you received some non empty input map
  assert(!(input_node_and_value.empty()));
  // Making sure that the counts are consistent
  assert(no_of_input_nodes == input_node_and_value.size());

  map< uint32_t, double > memoized_table;

  // Set the value of the input nodes and make sure those are constant nodes

  for(auto input_node : input_node_and_value)
  {
    assert(all_nodes[input_node.first].return_node_type == "constant");
    input_node.set_node_val(input_node.second);
  }

  for(auto output_values : output_node_and_value)
  {
    output_value.second = evaluate_node(output_value.first, memoized_table);
  }


}

datatype computation_graph :: evaluate_node( uint32_t node_id , map< uint32_t , double > & table )
{

  // the value of the output node :
  // compute the value of inputs to the node
  // compute the output once the inputs are ready.
  auto current_node = all_nodes[node_id];

  map< uint32_t , pair< node * , datatype > > backward_connections;
  current_node.get_backward_connections(backward_connections);
  map< uint32_t, double > inputs_to_the_node;

  // for each input node do the following :

    // if current node is a constant return that node id
    // if not, check if the value is already in that list of memoized table
    // else make a recursive call on this node's inputs

  for(auto some_connection : backward_connections)
  {
    auto input_node_ptr = some_connection.second.first;

    if(input_node_ptr->return_node_type() == "constant") // If a constant node then just get the value and store in the table
    {
      pair< uint32_t , double > node_and_value = make_pair(some_connection.first, input_node_ptr->return_current_output()) ;
      inputs_to_the_node.push_back( node_and_value ) ;
      table.push_back(node_and_value);
    }
    else if( table.find(input_node_ptr->get_node_number())  != table.end() ) // check if the value is already in the table
    {
      pair< uint32_t , double > node_and_value = make_pair(some_connection.first, table[input_node_ptr->get_node_number] ) ;
      inputs_to_the_node[some_connection.first] = node_and_value;
    }
    else // make  a recursive call to the inputs, get the value and compute
    {
      inputs_to_the_node[some_connection.first] = evaluate_node(some_connection.first, table);
    }

  }
  current_node.set_inputs(inputs_to_the_node);
  datatype result = current_node.return_current_output();

  table.push_back( make_pair ( node_id , result ) );

  return result;

}

map< uint32_t, datatype > computation_graph :: return_gradient_wrt_inputs(uint32_t node_id,  map < uint32_t, double > & input_node_and_value )
{

  map < uint32_t, double >  output_node_and_value;
  // Making sure you received some non empty input map
  assert(!(input_node_and_value.empty()));
  // Making sure that the counts are consistent
  assert(no_of_input_nodes == input_node_and_value.size());

  map< uint32_t, double > node_output_table;

  // Set the value of the input nodes and make sure those are constant nodes

  for(auto input_node : input_node_and_value)
  {
    assert(all_nodes[input_node.first].return_node_type == "constant");
    input_node.set_node_val(input_node.second);
  }

  for(auto output_value : output_node_and_value)
  {
    output_value.second = evaluate_node(output_value.first, node_output_table);
  }

  map< uint32_t, map< uint32_t, double > > node_derivative_wrt_inputs;
  map< uint32_t, double > output_derivative_wrt_inputs;

  output_derivative_wrt_inputs = compute_gradient_wrt_inputs(node_id, input_node_and_value, node_derivative_wrt_inputs);

  return output_derivative_wrt_inputs;

}

map< uint32_t, datatype > computation_graph :: compute_gradient_wrt_inputs(uint32_t node_id,
                                                      map< uint32_t, double > & input_node_and_value,
                                                      map< uint32_t, map< uint32_t, double > > & memoized_table)
{
  // Gradient with respect to inputs is computed as the following :
  // If the node is itself the input and a constant, then the gradient is 1
  // If the node is alrady in memory, look it up and report
  // If the node is neither of the above 2 conditions , then do the following :
    // Get gradients with the respect to the node's inputs, and then take a dot product of it
    //  with the gradient of the inputs of the node, with respect to the inputs of the network
    // This last step needs recursion and memoization

  if( ( find(input_nodes.begin(), input_nodes.end(), node_id) != input_nodes.end() ) &&
     ( all_nodes[node_id].return_node_type() == "constant" ) )
    {
      map < uint32_t, double > gradient_wrt_inputs_to_the_graph;
      gradient_wrt_inputs_to_the_graph.push_back(make_pair(node_id, 1.0));
      for(auto input_node : input_node_and_value)
      {
        if(input_node.first != node_id)
        {
          gradient_wrt_inputs_to_the_graph.push_back(make_pair(input_node.first, 0.0));
        }
      }
      return gradient_wrt_inputs_to_the_graph;
    }
  else if( memoized_table.find(node_id) != memoized_table.end() )
  {
    return memoized_table[node_id];
  }
  else
    {
        auto current_node = all_nodes[node_id];
        map< uint32_t, double > gradient_wrt_inputs_to_the_node = current_node.return_gradient();
        map< uint32_t, pair< node * , double > > input_nodes_to_the_current_node;
        current_node.get_backward_connections( input_nodes_to_the_current_node );

        map < uint32_t, map< uint32_t, double > > gradient_matrix;
        for(auto each_input_to_the_node : input_nodes_to_the_current_node )
        {
          auto input_node_index = each_input_to_the_node.first;
          auto grad_of_an_input_to_the_node_wrt_graph_inputs = compute_gradient_wrt_inputs(input_node_index, input_node_and_value, memoized_table);
          gradient_matrix.push_back(make_pair( input_node_index, grad_of_an_input_to_the_node_wrt_graph_inputs));
        }

        // Compute the gradient wrt inputs
        map < uint32_t, double > gradient_wrt_inputs_to_the_graph;
        for(auto each_input_to_the_comp_graph : input_node_and_value)
        {
          auto current_network_input_node_index  = each_input_to_the_comp_graph.first;
          double grad_buff = 0.0;
          for(auto each_input_to_the_node : input_nodes_to_the_current_node)
          {
            auto current_node_input_index = each_input_to_the_node.first;
            grad_buff += (gradient_matrix[current_network_input_node_index].second[current_network_input_node_index] *
                         gradient_wrt_inputs_to_the_node[current_node_input_index] ) ;
          }
          gradient_wrt_inputs_to_the_graph.push_back(make_pair( current_network_input_node_index, grad_buff ))
        }

        memoized_table.push_back(make_pair(node_id, gradient_wrt_inputs_to_the_graph));
        return gradient_wrt_inputs_to_the_graph;
    }

}
