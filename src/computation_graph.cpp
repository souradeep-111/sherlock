#include "computation_graph.h"
bool debug_eval = false;
bool debug_deriv = false;
std::mutex mtx;

using namespace std;
computation_graph :: computation_graph()
{
  all_nodes.clear();
  input_nodes.clear();
  output_nodes.clear();
  no_of_input_nodes = 0;
  no_of_output_nodes = 0;
}

void computation_graph :: clear()
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
    all_nodes.insert( make_pair(node_id, node_to_add) );
  }
  else
  {
    // cout << "Node id to be added already present ! " << endl;
    // assert(false);
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

void computation_graph :: clear_output_nodes()
{
  output_nodes.clear();
}

string computation_graph :: return_node_position(uint32_t node_index)
{
  if(find(input_nodes.begin(), input_nodes.end(), node_index) != input_nodes.end())
  {
    string return_string("input_node");
    return return_string;
  }
  else if(find(output_nodes.begin(), output_nodes.end(), node_index) != output_nodes.end())
  {
    string return_string("output_node");
    return return_string;
  }
  else
  {
    string return_string("internal_node");
    return return_string;
  }

  string return_string("none");
  return return_string;
}


void computation_graph :: connect_node1_to_node2_with_weight(
  uint32_t node_1_index, uint32_t node_2_index, datatype weight)
{
  // assert that these nodes are already there in the list
  assert( (all_nodes.find(node_1_index) != all_nodes.end())
  && (all_nodes.find(node_2_index) != all_nodes.end()) );

  all_nodes[node_1_index].add_forward_connection(& all_nodes[node_2_index] , weight);
  all_nodes[node_2_index].add_backward_connection(& all_nodes[node_1_index] , weight);

}

void computation_graph :: set_bias_of_node(uint32_t node_id, datatype bias)
{
  // assert that these nodes are already there in the list
  assert( all_nodes.find(node_id) != all_nodes.end() );

  all_nodes[node_id].set_bias(bias);
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
    assert( all_nodes[input_node.first].return_node_type() == const_string );
    all_nodes[input_node.first].set_node_val(input_node.second);
    memoized_table[input_node.first] = input_node.second;
    if(debug_eval)
    {
      cout << "Setting node val of node id " << all_nodes[input_node.first].get_node_number() << " as " << all_nodes[input_node.first].return_current_output() << endl;
    }
  }

  // Creating the space for the outputs of the network
  output_node_and_value.clear();
  for(auto output_node : output_nodes)
  {
    output_node_and_value.insert(make_pair(output_node, 0.0));
  }


  for(auto & output_values : output_node_and_value)
  {
    evaluate_node(*this, output_values.first, memoized_table, sherlock_parameters.thread_count, output_values.second, output_values.first);
  }


}


void evaluate_node(computation_graph & c_graph, uint32_t node_id ,
                   map< uint32_t , double > & table,
                   int & available_threads, double & ret_val , int thread_id)
{


    while(!mtx.try_lock());
    auto & current_node = c_graph.all_nodes[node_id];
    map< uint32_t , pair< node * , datatype > > backward_connections_, backward_connections;
    current_node.get_backward_connections(backward_connections_);
    backward_connections = backward_connections_;
    mtx.unlock();


    vector< thread > vector_of_threads_created;


    bool direction;
    if(thread_id % 2)
    {
      direction = true;
    }
    else
    {
      direction = false;
    }

    // for(auto some_connection : backward_connections)
    if(direction == true)
    {

      for(auto it = backward_connections.begin(); it != backward_connections.end(); it ++)
      {
        // auto input_node_ptr = some_connection.second.first;
        auto input_node_ptr = it->second.first;

        if(  input_node_ptr->return_node_type() == const_string) // If a constant node then just get the value and store in the table
        {
          // pair< uint32_t , double > node_and_value = make_pair(some_connection.first, input_node_ptr->return_current_output()) ;
          pair< uint32_t , double > node_and_value = make_pair(it->first, input_node_ptr->return_current_output()) ;
          while(!mtx.try_lock());
          table.insert(node_and_value);
          mtx.unlock();
          continue;
        }

        // check if the value is already in the table
        while(!mtx.try_lock());
        bool value_in_the_table = ( ( table.find(input_node_ptr->get_node_number())  == table.end() ) ? (false) : (true) ) ;
        mtx.unlock();
        if( value_in_the_table )
        {
          while(!mtx.try_lock());
          // pair< uint32_t , double > node_and_value = make_pair(some_connection.first, table[input_node_ptr->get_node_number()] ) ;
          pair< uint32_t , double > node_and_value = make_pair(it->first, table[input_node_ptr->get_node_number()] ) ;
          mtx.unlock();
        }
        else // make  a recursive call to the inputs, get the value and compute
        {
          if(available_threads == 1)
          {
            double buffer;
            // evaluate_node(c_graph, some_connection.first, table, available_threads, buffer, thread_id);
            evaluate_node(c_graph, it->first, table, available_threads, buffer, thread_id);
            // pair< uint32_t, double > node_and_value = make_pair(some_connection.first, buffer);
            pair< uint32_t, double > node_and_value = make_pair(it->first, buffer);
            while(!mtx.try_lock());
            table.insert(node_and_value);
            mtx.unlock();
          }
          else
          {

            while(!mtx.try_lock());
            available_threads--;
            mtx.unlock();
            if(debug_eval)
            {
              while(!mtx.try_lock());
              // cout << "Starting a thread from node number = " << some_connection.first << endl;
              cout << "Starting a thread with node number = " << it->first << " from thread " << thread_id << endl;
              cout << "Available threads = " << available_threads << endl;
              mtx.unlock();
            }

            double buffer;
            thread current_thread(evaluate_node,
                                  ref(c_graph),
                                  // some_connection.first,
                                  it->first,
                                  ref(table),
                                  ref(available_threads),
                                  ref(buffer),
                                  // some_connection.first) ;
                                  it->first) ;

            vector_of_threads_created.push_back(move(current_thread));
          }

        }

      }


    }
    else
    {
      for(auto it = backward_connections.rbegin(); it != backward_connections.rend(); it ++)
      {
        // auto input_node_ptr = some_connection.second.first;
        auto input_node_ptr = it->second.first;

        if(  input_node_ptr->return_node_type() == const_string) // If a constant node then just get the value and store in the table
        {
          // pair< uint32_t , double > node_and_value = make_pair(some_connection.first, input_node_ptr->return_current_output()) ;
          pair< uint32_t , double > node_and_value = make_pair(it->first, input_node_ptr->return_current_output()) ;
          while(!mtx.try_lock());
          table.insert(node_and_value);
          mtx.unlock();
          continue;
        }

        // check if the value is already in the table
        while(!mtx.try_lock());
        bool value_in_the_table = ( ( table.find(input_node_ptr->get_node_number())  == table.end() ) ? (false) : (true) ) ;
        mtx.unlock();
        if( value_in_the_table )
        {
          while(!mtx.try_lock());
          // pair< uint32_t , double > node_and_value = make_pair(some_connection.first, table[input_node_ptr->get_node_number()] ) ;
          pair< uint32_t , double > node_and_value = make_pair(it->first, table[input_node_ptr->get_node_number()] ) ;
          mtx.unlock();
        }
        else // make  a recursive call to the inputs, get the value and compute
        {
          if(available_threads == 1)
          {
            double buffer;
            // evaluate_node(c_graph, some_connection.first, table, available_threads, buffer, thread_id);
            evaluate_node(c_graph, it->first, table, available_threads, buffer, thread_id);
            // pair< uint32_t, double > node_and_value = make_pair(some_connection.first, buffer);
            pair< uint32_t, double > node_and_value = make_pair(it->first, buffer);
            while(!mtx.try_lock());
            table.insert(node_and_value);
            mtx.unlock();
          }
          else
          {
            while(!mtx.try_lock());
            available_threads--;
            mtx.unlock();
            if(debug_eval)
            {
              while(!mtx.try_lock());
              // cout << "Starting a thread from node number = " << some_connection.first << endl;
              cout << "Starting a thread with node number = " << it->first << " from thread " << thread_id << endl;
              cout << "Available threads = " << available_threads << endl;
              mtx.unlock();
            }

            double buffer;
            thread current_thread(evaluate_node,
                                  ref(c_graph),
                                  // some_connection.first,
                                  it->first,
                                  ref(table),
                                  ref(available_threads),
                                  ref(buffer),
                                  // some_connection.first) ;
                                  it->first) ;

            vector_of_threads_created.push_back(move(current_thread));
          }

        }

      }

    }



    int threads_counter = 0;

    for (thread & some_thread : vector_of_threads_created)
    {
    	if (some_thread.joinable())
      {
        some_thread.join();
        while(!mtx.try_lock());
        available_threads++;
        mtx.unlock();
        if(debug_eval)
        {
          while(!mtx.try_lock());
          cout << "Some thread ended" << endl;
          cout << "Available threads = " << available_threads << endl;
          mtx.unlock();
        }
      }

    }

    // Since the input to all the nodes is ready, compute the output now

    map< uint32_t, double > inputs_to_the_node;
    for(auto some_connection : backward_connections)
    {
      auto input_node_ptr = some_connection.second.first;

      while(!mtx.try_lock());
      bool value_in_the_table = ( ( table.find(input_node_ptr->get_node_number())  == table.end() ) ? (false) : (true) ) ;
      assert( value_in_the_table );
      pair< uint32_t , double > node_and_value = make_pair(some_connection.first, table[input_node_ptr->get_node_number()] ) ;
      mtx.unlock();

      inputs_to_the_node.insert(node_and_value);
    }



    while(!mtx.try_lock());

    current_node.set_inputs(inputs_to_the_node);
    double result = current_node.return_current_output();
    table.insert( make_pair ( node_id , result ) );
    if(debug_eval)
    {
      cout << "Computed value of node_id : " << node_id << " as " << result << " in thread id = " << thread_id <<  endl;
      cout << "Current table : " << " [ " ;
      for(auto each_entry : table)
      {
        cout << each_entry.first << " --- " << each_entry.second << " , ";
      }
      cout << " ] " << endl;

    }
    mtx.unlock();

    ret_val = result;

    return;

}

map< uint32_t, datatype > computation_graph :: return_gradient_wrt_inputs(
                uint32_t node_id,  map < uint32_t, double > & input_node_and_value )
{
  if(debug_deriv)
  {
    cout << "Starting overall gradient computation " << endl;
  }

  map < uint32_t, double >  output_node_and_value;
  // Making sure you received some non empty input map
  assert(!(input_node_and_value.empty()));
  // Making sure that the counts are consistent
  assert(no_of_input_nodes == input_node_and_value.size());

  map< uint32_t, double > node_output_table;

  // Set the value of the input nodes and make sure those are constant nodes


  for(auto input_node : input_node_and_value)
  {
    assert(all_nodes[input_node.first].return_node_type() == const_string);
    (all_nodes[input_node.first]).set_node_val(input_node.second);
  }



  for(auto output_node : output_nodes)
  {
    output_node_and_value.insert(make_pair( output_node, 0.0 ));
  }

  evaluate_graph(input_node_and_value, output_node_and_value);

  map< uint32_t, map< uint32_t, double > > node_derivative_wrt_inputs;
  map< uint32_t, double > output_derivative_wrt_inputs;

  compute_gradient_wrt_inputs(*this, node_id, input_node_and_value, node_derivative_wrt_inputs, sherlock_parameters.thread_count,
                              output_derivative_wrt_inputs, node_id);

  return output_derivative_wrt_inputs;

}

void compute_gradient_wrt_inputs(computation_graph & c_graph,
                                        uint32_t node_id,
                                        map< uint32_t, double > & input_node_and_value,
                                        map< uint32_t, map< uint32_t, double > > & memoized_table,
                                        int & available_threads,
                                        map< uint32_t, double > & result,
                                        int thread_id )
{
  // Gradient with respect to inputs is computed as the following :
  // If the node is itself the input and of constant type, then the gradient is 1
  // If the node is already in memory, look it up and report
  // If the node is neither of the above 2 conditions , then do the following :
    // Get gradients with the respect to the node's inputs, and then take a dot product of it
    //  with the gradient of the inputs of the node, with respect to the inputs of the network
    // This last step needs recursion and memoization

  if(debug_deriv)
  {
    while(!mtx.try_lock());
    cout << "Started gradient computation for node = " << node_id << " in thread = " << thread_id << endl;
    mtx.unlock();
  }

  while(!mtx.try_lock());
  bool check_if_input = ( ( find(c_graph.input_nodes.begin(), c_graph.input_nodes.end(), node_id) == c_graph.input_nodes.end() ) ?
                          (false) : (true)  );
  mtx.unlock();

  if( check_if_input )
  {
      map < uint32_t, double > gradient_wrt_inputs_to_the_graph;
      gradient_wrt_inputs_to_the_graph.insert(make_pair(node_id, 1.0));
      for(auto input_node : input_node_and_value)
      {
        if(input_node.first != node_id)
        {
          gradient_wrt_inputs_to_the_graph.insert(make_pair(input_node.first, 0.0));
        }
      }

      if(debug_deriv)
      {
        while(!mtx.try_lock());
        cout << "Gradient for node " << node_id << endl;
        print_map(gradient_wrt_inputs_to_the_graph);
        cout << "Done with gradient computation for node = " << node_id  << " in thread " << thread_id << endl;
        mtx.unlock();
      }
      result = gradient_wrt_inputs_to_the_graph;
      while(!mtx.try_lock());
      memoized_table.insert(make_pair(node_id, gradient_wrt_inputs_to_the_graph));
      mtx.unlock();

      return;

  }

  while(!mtx.try_lock());
  bool check_if_already_in_the_table = ( ( memoized_table.find(node_id) == memoized_table.end() ) ?
                          (false) : (true)  );
  mtx.unlock();



  if( check_if_already_in_the_table)
  {

    if(debug_deriv)
    {
      while(!mtx.try_lock());
      cout << "Picking up gradient computed from memory for node " << node_id << endl;
      mtx.unlock();
    }
    while(!mtx.try_lock());
    result = memoized_table[node_id];
    mtx.unlock();
    return;
  }


  while(!mtx.try_lock());
  auto & current_node = c_graph.all_nodes[node_id];
  mtx.unlock();
  vector< thread > vector_of_threads_created;


  map< uint32_t, double > grad_of_an_input_to_the_node_wrt_graph_inputs;

  map< uint32_t, double > gradient_wrt_inputs_to_the_node = current_node.return_gradient();
  map< uint32_t, pair< node * , double > > input_nodes_to_the_current_node;
  current_node.get_backward_connections( input_nodes_to_the_current_node );

  map < uint32_t, map< uint32_t, double > > gradient_matrix;
  // for(auto each_input_to_the_node : input_nodes_to_the_current_node )

  // Some way to randomize direction
  bool direction;
  if( thread_id % 2)
  {
    direction = true;
  }
  else
  {
    direction = false;
  }

  if(direction == true)
  {

    for(auto it = input_nodes_to_the_current_node.begin(); it != input_nodes_to_the_current_node.end(); it ++)
    {

      // auto input_node_index = each_input_to_the_node.first;
      auto input_node_index = it->first;

      if(available_threads == 1)
      {
        compute_gradient_wrt_inputs(c_graph, input_node_index, input_node_and_value, memoized_table,
                                    available_threads, grad_of_an_input_to_the_node_wrt_graph_inputs, thread_id);
        gradient_matrix.insert(make_pair( input_node_index, grad_of_an_input_to_the_node_wrt_graph_inputs));
      }
      else
      {
        available_threads--;
        if(debug_deriv)
        {
          while(!mtx.try_lock());
          cout << "Starting a thread from node number = " << input_node_index << endl;
          cout << "Available threads = " << available_threads << endl;
          mtx.unlock();
        }

        gradient_matrix.insert(make_pair(input_node_index, grad_of_an_input_to_the_node_wrt_graph_inputs));

        thread current_thread(compute_gradient_wrt_inputs,
                              ref(c_graph),
                              input_node_index,
                              ref(input_node_and_value),
                              ref(memoized_table),
                              ref(available_threads),
                              ref( gradient_matrix[input_node_index] ),
                              input_node_index);

        vector_of_threads_created.push_back(move(current_thread));
      }
      // grad_of_an_input_to_the_node_wrt_graph_inputs = compute_gradient_wrt_inputs(input_node_index, input_node_and_value, memoized_table);
      if(debug_deriv)
      {
        while(!mtx.try_lock());
        cout << "For node : " << node_id << " trying to get gradient for input number " << input_node_index << endl;
        mtx.unlock();
      }

    }

    for (thread & some_thread : vector_of_threads_created)
    {

      if (some_thread.joinable())
      {
        some_thread.join();
        available_threads++;
        if(debug_deriv)
        {
          while(!mtx.try_lock());
          cout << "Some thread ended" << endl;
          cout << "Available threads = " << available_threads << endl;
          mtx.unlock();
        }
      }

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
        grad_buff += (   ((gradient_matrix[current_node_input_index])[current_network_input_node_index]) *
                     gradient_wrt_inputs_to_the_node[current_node_input_index] ) ;
      }
      gradient_wrt_inputs_to_the_graph.insert(make_pair( current_network_input_node_index, grad_buff ));
    }

    while(!mtx.try_lock());
    memoized_table.insert(make_pair(node_id, gradient_wrt_inputs_to_the_graph));
    mtx.unlock();

    if(debug_deriv)
    {
      while(!mtx.try_lock());
      cout << "Gradient for node " << node_id << endl;
      print_map(gradient_wrt_inputs_to_the_graph);
      cout << "Done with gradient computation for node = " << node_id << " in thread " << thread_id << endl;
      mtx.unlock();
    }

    result = gradient_wrt_inputs_to_the_graph;
    return;
  }
  else
  {

    for(auto it = input_nodes_to_the_current_node.rbegin(); it != input_nodes_to_the_current_node.rend(); it ++)
    {

      // auto input_node_index = each_input_to_the_node.first;
      auto input_node_index = it->first;

      if(available_threads == 1)
      {
        compute_gradient_wrt_inputs(c_graph, input_node_index, input_node_and_value, memoized_table,
                                    available_threads, grad_of_an_input_to_the_node_wrt_graph_inputs, thread_id);
        gradient_matrix.insert(make_pair( input_node_index, grad_of_an_input_to_the_node_wrt_graph_inputs));
      }
      else
      {
        available_threads--;
        if(debug_deriv)
        {
          while(!mtx.try_lock());
          cout << "Starting a thread from node number = " << input_node_index << endl;
          cout << "Available threads = " << available_threads << endl;
          mtx.unlock();
        }

        gradient_matrix.insert(make_pair(input_node_index, grad_of_an_input_to_the_node_wrt_graph_inputs));

        thread current_thread(compute_gradient_wrt_inputs,
                              ref(c_graph),
                              input_node_index,
                              ref(input_node_and_value),
                              ref(memoized_table),
                              ref(available_threads),
                              ref( gradient_matrix[input_node_index] ),
                              input_node_index);

        vector_of_threads_created.push_back(move(current_thread));
      }
      // grad_of_an_input_to_the_node_wrt_graph_inputs = compute_gradient_wrt_inputs(input_node_index, input_node_and_value, memoized_table);
      if(debug_deriv)
      {
        while(!mtx.try_lock());
        cout << "For node : " << node_id << " trying to get gradient for input number " << input_node_index << endl;
        mtx.unlock();
      }

    }

    for (thread & some_thread : vector_of_threads_created)
    {

      if (some_thread.joinable())
      {
        some_thread.join();
        available_threads++;
        if(debug_deriv)
        {
          while(!mtx.try_lock());
          cout << "Some thread ended" << endl;
          cout << "Available threads = " << available_threads << endl;
          mtx.unlock();
        }
      }

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
        grad_buff += (   ((gradient_matrix[current_node_input_index])[current_network_input_node_index]) *
                     gradient_wrt_inputs_to_the_node[current_node_input_index] ) ;
      }
      gradient_wrt_inputs_to_the_graph.insert(make_pair( current_network_input_node_index, grad_buff ));
    }

    while(!mtx.try_lock());
    memoized_table.insert(make_pair(node_id, gradient_wrt_inputs_to_the_graph));
    mtx.unlock();

    if(debug_deriv)
    {
      while(!mtx.try_lock());
      cout << "Gradient for node " << node_id << endl;
      print_map(gradient_wrt_inputs_to_the_graph);
      cout << "Done with gradient computation for node = " << node_id << " in thread " << thread_id << endl;
      mtx.unlock();
    }

    result = gradient_wrt_inputs_to_the_graph;
    return;
  }


}

map< uint32_t , node > & computation_graph :: return_ref_to_all_nodes()
{
  return all_nodes;
}

void computation_graph :: return_id_of_all_nodes(vector< uint32_t >& node_indices)
{
  node_indices.clear();
  for(auto node : all_nodes)
  {
    node_indices.push_back(node.first);
  }
}


void computation_graph :: return_id_of_input_output_nodes(vector< uint32_t > & in_nodes ,
                                                          vector< uint32_t > & op_nodes )
{
  in_nodes = input_nodes;
  op_nodes = output_nodes;
}
void computation_graph :: return_id_of_nodes_at_depth_one_from_set(
                                                  vector<uint32_t> current_vector ,
                                                  set <uint32_t> & set_at_depth_one)
{


  set_at_depth_one.clear();
  node current_node;
  uint32_t current_node_index;
  bool relu_detected;
  map< uint32_t, pair< node * , double > > forward_nodes_container;

  // Make a queue out of the current set
  queue < uint32_t > unexplored_nodes;
  for(auto each_node : current_vector)
    unexplored_nodes.push(each_node);


  relu_detected = false;
  while(!unexplored_nodes.empty())
  {
    current_node_index = unexplored_nodes.front();
    unexplored_nodes.pop();

    current_node = all_nodes[current_node_index];

    current_node.get_forward_connections(forward_nodes_container);
    for(auto each_forward_connection : forward_nodes_container)
    {
      if(each_forward_connection.second.first->get_node_type() == _relu_)
      {
        set_at_depth_one.insert(each_forward_connection.first);
        relu_detected = true;
      }
      else
      {
        if(!relu_detected)
          unexplored_nodes.push(each_forward_connection.first);
      }
    }
  }

  return;
}

void computation_graph :: return_id_of_nodes_at_depth_one_from_set(
                                                  set < uint32_t > current_set ,
                                                  set < uint32_t > & set_at_depth_one)
{

  set_at_depth_one.clear();
  node current_node;
  uint32_t current_node_index;
  bool relu_detected;
  map< uint32_t, pair< node * , double > > forward_nodes_container;

  // Make a queue out of the current set
  queue < uint32_t > unexplored_nodes;
  for(auto each_node : current_set)
    unexplored_nodes.push(each_node);

  relu_detected = false;
  while(!unexplored_nodes.empty())
  {
    current_node_index = unexplored_nodes.front();
    unexplored_nodes.pop();

    current_node = all_nodes[current_node_index];

    current_node.get_forward_connections(forward_nodes_container);
    for(auto each_forward_connection : forward_nodes_container)
    {
      if(each_forward_connection.second.first->get_node_type() == _relu_)
      {
        set_at_depth_one.insert(each_forward_connection.first);
        relu_detected = true;
      }
      else
      {
        if(!relu_detected)
          unexplored_nodes.push(each_forward_connection.first);
      }
    }
  }

  return;
}

void computation_graph :: return_next_layer_from_set(
                          vector < uint32_t > & current_layer,
                          set < uint32_t > & set_at_depth_one
)
{
  // Algorithm for this function :
  // Do a depth first search on the tree
  // Each time you find a Relu unit, don't explore further
  // While exploring, don't add the non computable nodes to the stack
  // A node is computable when all it's inputs are computable as well

  set< uint32_t > computable_set;
  stack< uint32_t > current_stack;

  while(!current_stack.empty()){current_stack.pop();}

  for(auto each_index : current_layer)
  {
    current_stack.push(each_index);
    computable_set.insert(each_index);
  }
  map< uint32_t , pair< node*, double > > forward_connections;
  uint32_t current_node_id, child_id;
  node current_node, child_node;

  set_at_depth_one.clear();


  while(!current_stack.empty())
  {
    current_node_id = current_stack.top();
    current_stack.pop();
    current_node = all_nodes[current_node_id];
    current_node.get_forward_connections(forward_connections);

    for(auto each_connection : forward_connections)
    {
      child_id = each_connection.first;
      if(is_computable(child_id, computable_set))
      {
        computable_set.insert(child_id);
        child_node = all_nodes[child_id];

        // These are things which basically DON'T have a future
        if((child_node.get_node_type() == _tanh_) ||
           (child_node.get_node_type() == _sigmoid_) ||
           (child_node.get_node_type() == _relu_))
         {
           set_at_depth_one.insert(child_id);
         }
         else
         {
            current_stack.push(child_id);
         }
      }

    }
  }

}

void computation_graph :: return_next_layer_from_set(
                          set < uint32_t > & current_set,
                          set < uint32_t > & set_at_depth_one
)
{
  vector < uint32_t > current_vector;

  current_vector.clear();
  for(set< uint32_t > :: iterator it = current_set.begin();
      it != current_set.end(); it++)
    current_vector.push_back(*it);

  return_next_layer_from_set(current_vector, set_at_depth_one);
}

bool computation_graph :: is_computable(uint32_t node_id,
                   set< uint32_t > & current_computable_set)
{
  assert(!current_computable_set.empty());

  set< uint32_t > :: iterator it = current_computable_set.find(node_id);
  if(it != current_computable_set.end())
  {
    return true;
  }


  map< uint32_t, pair< node *, double > > backward_connections;
  node current_node;
  current_node = all_nodes[node_id];

  current_node.get_backward_connections(backward_connections);

  for(auto each_connection : backward_connections)
  {
    if(current_computable_set.find(each_connection.first) == current_computable_set.end())
      return false;
  }

  return true;

}


void computation_graph :: extract_graph(const set < uint32_t > & output_node_set,
                                        const set < uint32_t > & input_node_set,
                                        computation_graph & sub_graph
)
{
  // Algorithm for the function here
  // Start with the output node set
  // Do a breadth first search, and keep building the network in a backward fashion
  // Stop when the backward nodes are exactly same as the received input nodes

  assert(!output_node_set.empty());
  assert(!input_node_set.empty());
  sub_graph.clear();

  // Some node is unexplored if it's backward connections have not been added yet
  queue < uint32_t > current_nodes_slice, unexplored_nodes;
  set< uint32_t > current_backward_nodes, present_set;
  map < uint32_t, pair< node*, double > > backward_connections;
  node current_node;
  uint32_t node_id;

  // all_nodes = return_reference_to_all_nodes();

  for(auto each_index : output_node_set)
    current_nodes_slice.push(each_index);

  while(!current_nodes_slice.empty())
  {
    // For each node, add them to the sub-graph,
    // get the backward nodes add them to the graph, along with the connections


    // Collect all the backward nodes collected and add them back to the backward
    // nodes after filtration.
    unexplored_nodes = current_nodes_slice;
    current_backward_nodes.clear();

    while(!unexplored_nodes.empty())
    {
      node_id = unexplored_nodes.front();
      unexplored_nodes.pop();

      present_set.insert(node_id);
      current_node = all_nodes[node_id];
      sub_graph.add_new_node(current_node.get_node_number(), current_node);
      current_node.get_backward_connections(backward_connections);
      for(auto each_pair : backward_connections)
        current_backward_nodes.insert(each_pair.first);


    }


    // Filter the backward nodes, in terms of which ones needs to be
    // explored further.
    while(!current_nodes_slice.empty()) current_nodes_slice.pop();

    for(auto node_id : current_backward_nodes)
    {
      node node_x;
      node_x = all_nodes[node_id];

      if(input_node_set.find(node_id) == input_node_set.end())
      {
        if(node_x.get_node_type() != constant)
          current_nodes_slice.push(node_id);

        sub_graph.add_new_node(node_id, node_x);
      }
      else
      {
        node_x.set_node_type("constant");
        sub_graph.add_new_node(node_id, node_x);
      }
    }


  }

  // Mark all the input node set as inputs
  for(auto each_id : input_node_set)
  {
    assert(sub_graph.all_nodes[each_id].return_node_type() == "constant");
    sub_graph.mark_node_as_input(each_id);
  }

  // Mark all the output node set as outputs
  for(auto each_id : output_node_set)
    sub_graph.mark_node_as_output(each_id);



}


void print_stack(stack< uint32_t > my_stack)
{
  cout << "Stack -- " ;
  while(!my_stack.empty())
  {
    cout << my_stack.top() << ",";
    my_stack.pop();
  }
  cout << endl;
}

void print_set(set< uint32_t > my_set)
{
  cout << "Set -- " ;
  for(auto it = my_set.begin(); it != my_set.end(); it++)
    cout << *it << ",";
  cout << endl;
}
