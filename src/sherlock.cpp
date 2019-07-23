#include "sherlock.h"

using namespace std;

bool debug_sherlock = true;

sherlock :: sherlock()
{
  // Pretty much does nothing
}

sherlock :: sherlock(computation_graph & CG)
{
  neural_network = CG;
}

void sherlock :: clear()
{
  neural_network.clear();
  network_constraints.delete_and_reinitialize();
}

void sherlock :: set_computation_graph(computation_graph & CG)
{
  neural_network = CG;
}

void sherlock :: optimize_node(uint32_t node_index, bool direction,
                               region_constraints & input_region,
                               double & optima_achieved)
{
  map< uint32_t, double > neuron_values;
  network_constraints.delete_and_reinitialize();
  network_constraints.create_the_input_overapproximation_for_each_neuron(
                      neural_network, input_region);
  network_constraints.generate_graph_constraints(
                      input_region, neural_network, node_index);

  if( network_constraints.optimize(node_index, direction, neuron_values, optima_achieved) )
  {
    // if(debug_sherlock)
    // {
    //   cout << "Point of " << (direction?"maxima":"minima") << " found at " << endl;
    //   vector< uint32_t> input_nodes, output_nodes;
    //   neural_network.return_id_of_input_output_nodes(input_nodes, output_nodes);
    //   map< uint32_t, double > optima_point;
    //   for(auto index:input_nodes)
    //   {
    //     optima_point[index] = neuron_values[index];
    //   }
    //   print_point(optima_point);
    // }
    return;
  }
  else
  {
    cout << "Optimization failed , terminating itself ! " << endl;
    assert(false);
  }

  return;

}

void sherlock :: optimize_constrained(uint32_t node_index, bool direction,
                               region_constraints & input_region,
                               vector< linear_inequality > & inequalities,
                               double & optima_achieved)
{
  map< uint32_t, double > neuron_values;
  network_constraints.delete_and_reinitialize();
  network_constraints.create_the_input_overapproximation_for_each_neuron(
                      neural_network, input_region);
  network_constraints.generate_graph_constraints(
                      input_region, neural_network, node_index);

  for(auto & each_inequality : inequalities)
  {
    network_constraints.add_linear_constraint(each_inequality);
  }

  if( network_constraints.optimize(node_index, direction, neuron_values, optima_achieved) )
  {
    return;
  }
  else
  {
    cout << "Optimization failed , terminating itself ! " << endl;
    assert(false);
  }

  return;
}

void sherlock :: compute_output_range(uint32_t node_index,
                                      region_constraints & input_region,
                                      pair < double, double >& output_range )
{

  network_constraints.delete_and_reinitialize();
  network_constraints.create_the_input_overapproximation_for_each_neuron(neural_network, input_region);
  network_constraints.generate_graph_constraints(input_region, neural_network, node_index);
  network_constraints.add_invariants(neural_network, input_region);

  // Maximizing
  gradient_driven_optimization(node_index, input_region, true, output_range.second);
  nodes_explored = network_constraints.nodes_explored_last_optimization;


  // Minimizing :
  gradient_driven_optimization(node_index, input_region, false, output_range.first);
  nodes_explored += network_constraints.nodes_explored_last_optimization;

}

void sherlock :: gradient_driven_optimization(uint32_t node_index,
                                      region_constraints & input_region, bool direction,
                                      double & optima)
{
  vector< uint32_t > indices_of_input_nodes, indices_of_output_nodes;
  neural_network.return_id_of_input_output_nodes(indices_of_input_nodes, indices_of_output_nodes);
  double current_optima;
  map<uint32_t, double > neuron_values, search_point;

  // assert(indices_of_output_nodes.size() == 1);
  // network_constraints.delete_and_reinitialize();
  // network_constraints.create_the_input_overapproximation_for_each_neuron(neural_network, input_region);
  // network_constraints.generate_graph_constraints(input_region, neural_network, node_index);
  // network_constraints.add_invariants(neural_network, input_region);

  bool res = true;

  if(!input_region.return_sample(search_point, 19))
  {
    cout << "Failed to compute random sample, input region might be infeasible " << endl;
    assert(false);
  }


  current_optima = ((direction) ? (-1e30) : (1e30)) ;

  do{
    if(sherlock_parameters.verbosity)
    {
      cout << "Gradient search starts at : ";
      print_point(search_point);
      cout << endl;
    }
    if(sherlock_parameters.do_random_restarts)
    {
      perform_gradient_search_with_random_restarts(node_index, direction, input_region,
                                                   search_point, current_optima);
    }
    else
    {
      perform_gradient_search(node_index, direction, input_region, search_point, current_optima);
    }

    if(sherlock_parameters.verbosity)
    {
      cout << "Gradient search ends at : " << current_optima << "  ";
      print_point(search_point);
      cout << endl;
    }

    neural_network.evaluate_graph(search_point, neuron_values);

    res = network_constraints.optimize_enough(node_index, current_optima, direction, neuron_values);
    if(!neuron_values.empty())
    {
      for(auto input_index : indices_of_input_nodes)
      {
        search_point[input_index] = neuron_values[input_index];
      }

      neural_network.evaluate_graph(search_point, neuron_values);
      double actual_val = neuron_values[node_index];
      if(fabs(actual_val - current_optima) > sherlock_parameters.MILP_tolerance)
      {
        optimize_node(node_index, direction, input_region, optima);
        return;
      }
    }
    else if(res)
    {
      cout << "Gurobi counter example generation feasible, but didn't return the value " << endl;
      assert(false);
    }
    else
    {
      if(sherlock_parameters.grad_search_point_verbosity)
      {
        cout << " No Counter example found in Gurobi " << endl;
      }
    }

    if(sherlock_parameters.verbosity)
    {
      cout << "Nodes explored = " << network_constraints.nodes_explored_last_optimization << endl;
      cout << "This phase ends at : ";
      print_point(search_point);
      cout << endl;
    }



  }while(res);

  optima = current_optima;

  if(sherlock_parameters.verbosity)
  {
    cout << "The whole search ends at : ";
    print_point(search_point);
    cout << endl;
    cout << "At value = " << optima << endl;
  }


}


void sherlock :: compute_output_region(region_constraints & input_region,
                                      region_constraints & output_region)
{

  // Does nothing at this point
}

void sherlock :: perform_gradient_search(uint32_t node_index, bool direction,
                                       region_constraints & region,
                                       map< uint32_t, double > & starting_point, double & val)
{

  double improvement = 1e30;
  map< uint32_t, double > network_gradient;
  map<uint32_t, double > current_point, next_point, network_output_value_1, network_output_value_2;
  double value_prev, value_curr;
  double step_size = sherlock_parameters.gradient_rate;
  int trigger_counter = 0;
  bool steps_in_region = false;
  int exponent_number = 0;
  int gradient_step = 0;
  current_point = starting_point;

  while(improvement > sherlock_parameters.grad_termination_limit)
  {

    /* Do gradient compuutation */
    network_gradient = neural_network.return_gradient_wrt_inputs(node_index, current_point);
    if(bad_gradients(network_gradient))
    {
      neural_network.evaluate_graph(current_point, network_output_value_1);
      val = network_output_value_1[node_index];
      starting_point = current_point;
      return;
    }

    /* Compute improvement */
    next_point = current_point;
    steps_in_region = increment_point_in_direction(next_point, step_size,  network_gradient, region);

    neural_network.evaluate_graph(current_point, network_output_value_1);
    value_prev = network_output_value_1[node_index];

    neural_network.evaluate_graph(next_point, network_output_value_2);
    value_curr = network_output_value_2[node_index];


    improvement = value_curr - value_prev;
    improvement = (direction ) ? (improvement) : (-improvement);


    if((improvement > 0.0) && (steps_in_region))
    {
      trigger_counter++;
      if(trigger_counter == sherlock_parameters.triggering_limit)
      {
        trigger_counter = 0;
        if(exponent_number < sherlock_parameters.exponential_limit_upper)
        {
          step_size *= 2.0;
          exponent_number++;
        }
        else
        {
          break;
        }
      }
    }
    else
    {
      next_point = current_point;
      if(exponent_number > sherlock_parameters.exponential_limit_lower)
      {
        step_size /= 2.0;
        exponent_number--;
      }
      else
      {
        break;
      }
    }

    gradient_step++;
    if(sherlock_parameters.grad_search_point_verbosity)
    {
      cout << "At step = " << gradient_step << " "  << "  value = " << value_curr ;
      print_point(next_point) ;
    }

    current_point = next_point;

  }



  if(improvement > 0.0)
  {
    starting_point = current_point;
    val = value_curr;
  }

  return;


}

void sherlock :: perform_gradient_search_with_random_restarts(uint32_t node_index,
                                      bool direction, region_constraints & region,
                                      map< uint32_t, double > & starting_point, double & val )
{
  double improvement = 1e30;
  map<uint32_t, double > current_point, next_point, network_output_value_1,
                         network_output_value_2, trial_point, network_gradient;

  double trial_val, val_curr, val_prev, trial_count;

  neural_network.evaluate_graph(starting_point, network_output_value_1);
  val_curr = network_output_value_1[node_index];
  val = val_curr;

  current_point = starting_point;
  int restart_count = -1;
  while(restart_count < sherlock_parameters.no_of_random_restarts)
  {

    perform_gradient_search(node_index, direction, region, current_point, val);

    if(return_best_effort_random_counter_example(direction, current_point, val, node_index, region))
    {
      if(sherlock_parameters.grad_search_point_verbosity)
      {
        cout << "Random counter example found = " ;
        print_point(current_point);
        neural_network.evaluate_graph(current_point, network_output_value_1);
        cout << " Which has value : " << network_output_value_1[node_index] << endl;
      }


    }
    else
    {
      val_curr = val;
      break;
    }

    restart_count ++;
  }

  starting_point = current_point;


  return;

}

bool sherlock :: increment_point_in_direction(map<uint32_t, double >& current_values, double step_size,
                                      map<uint32_t, double > direction, region_constraints& region)
{
  map< uint32_t, bool> directions_map;
  if(sherlock_parameters.do_signed_gradient)
  {
    for(auto gradient_in_some_direction : direction )
    {
      bool b = (gradient_in_some_direction.second > 0.0) ? (true) : (false);
      directions_map.insert(make_pair( gradient_in_some_direction.first , b ));
    }

    for(auto some_direction_bool : directions_map)
    {
      double movement_amount = (some_direction_bool.second) ? (step_size) : (-1.0 * step_size) ;
      current_values[some_direction_bool.first] += (movement_amount) ;
    }
    if(region.check(current_values))
    {
        return true;
    }
    else
    {
      return false;
    }
  }
  else
  {

    for(auto some_direction : direction)
    {
      double movement_amount = some_direction.second * step_size ;
      current_values[some_direction.first] += (movement_amount) ;

    }
    if(region.check(current_values))
    {
      return true;
    }
    else
    {
      return false;
    }
  }


}

void sherlock :: increment_point_in_direction(map<uint32_t, double >& current_values,
                                      map<uint32_t, double > direction, region_constraints& region)
{
  map< uint32_t, bool> directions_map;
  if(sherlock_parameters.do_signed_gradient)
  {
    for(auto gradient_in_some_direction : direction )
    {
      bool b = (gradient_in_some_direction.second > 0.0) ? (true) : (false);
      directions_map.insert(make_pair( gradient_in_some_direction.first , b ));
    }

    for(auto some_direction_bool : directions_map)
    {
      double movement_amount = (some_direction_bool.second) ? (sherlock_parameters.gradient_rate) : (-1.0 * sherlock_parameters.gradient_rate) ;
      current_values[some_direction_bool.first] += (movement_amount) ;
      if(!region.check(current_values))
      {
        current_values[some_direction_bool.first] -= (movement_amount) ;
      }
    }
    return;
  }
  else
  {

    for(auto some_direction : direction)
    {
      double movement_amount = some_direction.second * sherlock_parameters.gradient_rate ;
      current_values[some_direction.first] += (movement_amount) ;
      if(!region.check(current_values))
      {
        current_values[some_direction.first] -= (movement_amount) ;
      }
    }

    return;

  }


}

void sherlock :: increment_point_in_direction(map<uint32_t, double >& current_values,
                                      map<uint32_t, double > direction)
{
  map< uint32_t, bool> directions_map;
  if(sherlock_parameters.do_signed_gradient)
  {
    for(auto gradient_in_some_direction : direction )
    {
      bool b = (gradient_in_some_direction.second > 0.0) ? (true) : (false);
      directions_map.insert(make_pair ( gradient_in_some_direction.first , b ));
    }

    for(auto some_direction_bool : directions_map)
    {
      double movement_amount = (some_direction_bool.second) ?
                               (sherlock_parameters.gradient_rate) :
                               (-1.0 * sherlock_parameters.gradient_rate) ;
      current_values[some_direction_bool.first] += (movement_amount) ;
    }
    return;
  }
  else
  {

    for(auto some_direction : direction)
    {
      double movement_amount = some_direction.second * sherlock_parameters.gradient_rate ;
      current_values[some_direction.first] += (movement_amount) ;
    }

    return;

  }


}

void sherlock :: compute_output_range_by_sampling(region_constraints & input_region,
                                                  uint32_t output_node_index,
                                                  pair < double , double > & output_range,
                                                  uint32_t sample_count)
{

  // assert(sample_count > 10);
  map < uint32_t, double > input_point;
  map < uint32_t, double > output_val;
  output_range.first = 1e30;
  output_range.second = -1e30;

  for(int i = 0; i < sample_count; i++)
  {
    input_region.return_sample(input_point, i);
    neural_network.evaluate_graph(input_point, output_val);
    if(output_val[output_node_index] < output_range.first)
    {
      output_range.first = output_val[output_node_index];
    }
    if(output_val[output_node_index] > output_range.second)
    {
      output_range.second = output_val[output_node_index];
    }
  }
  return;
}

bool sherlock :: return_best_effort_random_counter_example(bool direction,
                                      map< uint32_t , double >& current_point,
                                      double& val_curr, uint32_t node_index,
                                      region_constraints & region)
{
  double trial_val;
  int trial_count;
  map< uint32_t, double > trial_point, network_output_value;

  trial_val =  (direction) ? -sherlock_parameters.tool_high : sherlock_parameters.tool_high ;
  trial_count = 0;
  while( trial_count < sherlock_parameters.random_counter_example_count )
  {
    region.return_sample(trial_point, ((int) val_curr)*17 + 100*trial_count);
    trial_count ++;

    neural_network.evaluate_graph(trial_point, network_output_value);
    trial_val = network_output_value[node_index];
    if( (direction && (trial_val > (val_curr + sherlock_parameters.grad_termination_limit)))
                          ||
        ((!direction) && (trial_val < (val_curr - sherlock_parameters.grad_termination_limit))) )
    {
      current_point = trial_point;
      val_curr = trial_val;
      return true;
    }
  }
  return false;
}

void sherlock :: add_constraint(linear_inequality & lin_ineq)
{
  network_constraints.add_linear_constraint(lin_ineq);
}



void create_computation_graph_from_file(string filename,
                                        computation_graph & CG,
                                        bool has_output_relu,
                                        vector<uint32_t>& input_node_indices,
                                        vector<uint32_t>& output_node_indices)
{
  input_node_indices.clear();
  output_node_indices.clear();
  CG.clear();
  ifstream file;
  file.open(filename.c_str(), ios::in);

  int no_of_inputs, no_of_outputs, no_of_hidden_layers;
  int i, j, k, node_index, no_of_neurons_in_previous_layer;
  vector < uint32_t > indices_of_previous_layer_nodes, indices_of_current_layer_nodes;

  double buffer;
  file >> buffer; no_of_inputs = (int) buffer;
  file >> buffer; no_of_outputs = (int) buffer;
  file >> buffer; no_of_hidden_layers = (int) buffer;

  vector< uint32_t > network_configuration;
  i = 0;
  while(i < no_of_hidden_layers)
  {
    file >> buffer;
    network_configuration.push_back((uint32_t) buffer);
    i++;
  }


  node_index = 1;

  // Creating the input nodes, and adding them to the computation graph
  indices_of_previous_layer_nodes.clear();
  i = 0;
  while(i < no_of_inputs )
  {
    node node_x(node_index, "constant");
    CG.add_new_node(node_index, node_x);
    CG.mark_node_as_input(node_index);
    indices_of_previous_layer_nodes.push_back(node_index);
    input_node_indices.push_back(node_index);
    node_index ++;
    i++;
  }
  // Reading the hidden neurons if any
  for( i = 0; i < no_of_hidden_layers; i++)
  {
    indices_of_current_layer_nodes.clear();
    for( j = 0 ; j < network_configuration[i] ; j++ )
    {
      node node_x(node_index, "relu");
      CG.add_new_node(node_index, node_x);
      indices_of_current_layer_nodes.push_back(node_index);

      no_of_neurons_in_previous_layer = ((i == 0) ? (no_of_inputs):(network_configuration[i-1])) ;
      assert(no_of_neurons_in_previous_layer == indices_of_previous_layer_nodes.size());
      // Reading the weights
      for(k = 0; k < no_of_neurons_in_previous_layer; k++)
      {
        file >> buffer;
        CG.connect_node1_to_node2_with_weight(indices_of_previous_layer_nodes[k], node_index, buffer);
      }
      // Reading the bias
      file >> buffer;
      CG.set_bias_of_node(node_index, buffer);
      node_index ++;
    }
    indices_of_previous_layer_nodes = indices_of_current_layer_nodes;
  }
  //
  // i = 0;
  // while(i < no_of_outputs )
  // {
  //   node node_x(node_index, "relu");
  //   CG.add_new_node(node_index, node_x);
  //   CG.mark_node_as_output(node_index);
  //   indices_of_previous_layer_nodes.push_back(node_index);
  //   node_index ++;
  //   i++;
  // }

  // Reading the output mapping
  for( j = 0 ; j < no_of_outputs ; j++ )
  {
    string node_type = (has_output_relu) ? ("relu") : ("none");

    node node_x(node_index, node_type);
    CG.add_new_node(node_index, node_x);
    CG.mark_node_as_output(node_index);

    no_of_neurons_in_previous_layer = network_configuration[no_of_hidden_layers - 1] ;
    // Reading the weights
    for(k = 0; k < no_of_neurons_in_previous_layer; k++)
    {
      file >> buffer;
      CG.connect_node1_to_node2_with_weight(indices_of_previous_layer_nodes[k], node_index, buffer);
    }
    // Reading the bias
    file >> buffer;
    CG.set_bias_of_node(node_index, buffer);
    output_node_indices.push_back(node_index);
    node_index ++;
  }

}




void test_network_1(computation_graph & CG)
{
  CG.clear();
  // The two input nodes to the graph declared as constants
  node node_1_b(1, "constant");
  CG.add_new_node(1, node_1_b);
  node node_2_b(2, "constant");
  CG.add_new_node(2, node_2_b);

  // The internal nodes
  node node_3_b(3, "relu");
  CG.add_new_node(3, node_3_b);
  node node_4_b(4, "relu");
  CG.add_new_node(4, node_4_b);
  node node_5_b(5, "relu");
  CG.add_new_node(5, node_5_b);
  node node_6_b(6, "relu");
  CG.add_new_node(6, node_6_b);
  node node_7_b(7, "relu");
  CG.add_new_node(7, node_7_b);
  node node_8_b(8, "relu");
  CG.add_new_node(8, node_8_b);
  node node_9_b(9, "relu");
  CG.add_new_node(9, node_9_b);

  // The output node
  node node_10_b(10, "none");
  CG.add_new_node(10, node_10_b);


  // First let's mark some of the nodes as inputs and outputs
  CG.mark_node_as_input(1);
  CG.mark_node_as_input(2);
  CG.mark_node_as_output(10);

  // Now let's create the connections:

  // first layer connections and bias
  CG.connect_node1_to_node2_with_weight(1,3,1.0);
  CG.connect_node1_to_node2_with_weight(2,3,1.0);
  CG.set_bias_of_node(3, 0.0);

  CG.connect_node1_to_node2_with_weight(1,4,1.0);
  CG.connect_node1_to_node2_with_weight(2,4,1.0);
  CG.set_bias_of_node(4, 0.0);

  CG.connect_node1_to_node2_with_weight(3,5,1.0);
  CG.connect_node1_to_node2_with_weight(4,5,0.0);
  CG.set_bias_of_node(5, 0.0);

  CG.connect_node1_to_node2_with_weight(3,6,1.0);
  CG.connect_node1_to_node2_with_weight(4,6,0.0);
  CG.set_bias_of_node(6, 0.0);

  CG.connect_node1_to_node2_with_weight(3,7,1.0);
  CG.connect_node1_to_node2_with_weight(4,7,1.0);
  CG.set_bias_of_node(7, 0.0);

  CG.connect_node1_to_node2_with_weight(3,8,0.0);
  CG.connect_node1_to_node2_with_weight(4,8,1.0);
  CG.set_bias_of_node(8, 0.0);

  CG.connect_node1_to_node2_with_weight(3,9,0.0);
  CG.connect_node1_to_node2_with_weight(4,9,1.0);
  CG.set_bias_of_node(9, 0.0);

  CG.connect_node1_to_node2_with_weight(5,10,1.0);
  CG.connect_node1_to_node2_with_weight(6,10,1.0);
  CG.connect_node1_to_node2_with_weight(7,10,1.0);
  CG.connect_node1_to_node2_with_weight(8,10,1.0);
  CG.connect_node1_to_node2_with_weight(9,10,1.0);
  CG.set_bias_of_node(10, 0.0);


}

void test_network_2(computation_graph & CG)
{
  CG.clear();
  // The two input nodes to the graph declared as constants
  node node_1(1, "constant");
  CG.add_new_node(1, node_1);
  node node_2(2, "constant");
  CG.add_new_node(2, node_2);

  // The internal nodes
  node node_3(3, "relu");
  CG.add_new_node(3, node_3);
  node node_4(4, "relu");
  CG.add_new_node(4, node_4);
  node node_5(5, "relu");
  CG.add_new_node(5, node_5);
  node node_6(6, "relu");
  CG.add_new_node(6, node_6);

  // The output node
  node node_7(7, "none");
  CG.add_new_node(7, node_7);


  // First let's mark some of the nodes as inputs and outputs
  CG.mark_node_as_input(1);
  CG.mark_node_as_input(2);
  CG.mark_node_as_output(7);

  // Now let's create the connections:

  // first layer connections and bias
  CG.connect_node1_to_node2_with_weight(1,3,-10.0);
  CG.connect_node1_to_node2_with_weight(1,4,10.0);
  CG.connect_node1_to_node2_with_weight(2,3,-10.0);
  CG.connect_node1_to_node2_with_weight(2,4,0.5);
  CG.set_bias_of_node(3, 0.0);
  CG.set_bias_of_node(4, 0.0);

  CG.connect_node1_to_node2_with_weight(3,5,1.0);
  CG.connect_node1_to_node2_with_weight(3,6,0.0);
  CG.connect_node1_to_node2_with_weight(4,5,0.0);
  CG.connect_node1_to_node2_with_weight(4,6,1.0);
  CG.set_bias_of_node(5,0.0);
  CG.set_bias_of_node(6,0.0);

  CG.connect_node1_to_node2_with_weight(5,7,0.5);
  CG.connect_node1_to_node2_with_weight(6,7,0.5);
  CG.set_bias_of_node(7, 0.0);

}

void test_network_sigmoid(computation_graph & CG)
{
  CG.clear();
  // The two input nodes to the graph declared as constants
  node node_1(1, "constant");
  CG.add_new_node(1, node_1);
  node node_2(2, "constant");
  CG.add_new_node(2, node_2);

  // The internal nodes
  node node_3(3, "sigmoid");
  CG.add_new_node(3, node_3);
  node node_4(4, "sigmoid");
  CG.add_new_node(4, node_4);
  node node_5(5, "sigmoid");
  CG.add_new_node(5, node_5);
  node node_6(6, "sigmoid");
  CG.add_new_node(6, node_6);

  // The output node
  node node_7(7, "none");
  CG.add_new_node(7, node_7);


  // First let's mark some of the nodes as inputs and outputs
  CG.mark_node_as_input(1);
  CG.mark_node_as_input(2);
  CG.mark_node_as_output(7);

  // Now let's create the connections:

  // first layer connections and bias
  CG.connect_node1_to_node2_with_weight(1,3,-10.0);
  CG.connect_node1_to_node2_with_weight(1,4,10.0);
  CG.connect_node1_to_node2_with_weight(2,3,-10.0);
  CG.connect_node1_to_node2_with_weight(2,4,0.5);
  CG.set_bias_of_node(3, 0.0);
  CG.set_bias_of_node(4, 0.0);

  CG.connect_node1_to_node2_with_weight(3,5,1.0);
  CG.connect_node1_to_node2_with_weight(3,6,0.0);
  CG.connect_node1_to_node2_with_weight(4,5,0.0);
  CG.connect_node1_to_node2_with_weight(4,6,1.0);
  CG.set_bias_of_node(5,0.0);
  CG.set_bias_of_node(6,0.0);

  CG.connect_node1_to_node2_with_weight(5,7,0.5);
  CG.connect_node1_to_node2_with_weight(6,7,0.5);
  CG.set_bias_of_node(7, 0.0);

}

void test_network_tanh(computation_graph & CG)
{
  CG.clear();
  // The two input nodes to the graph declared as constants
  node node_1(1, "constant");
  CG.add_new_node(1, node_1);
  node node_2(2, "constant");
  CG.add_new_node(2, node_2);

  // The internal nodes
  node node_3(3, "tanh");
  CG.add_new_node(3, node_3);
  node node_4(4, "tanh");
  CG.add_new_node(4, node_4);
  node node_5(5, "tanh");
  CG.add_new_node(5, node_5);
  node node_6(6, "tanh");
  CG.add_new_node(6, node_6);

  // The output node
  node node_7(7, "none");
  CG.add_new_node(7, node_7);


  // First let's mark some of the nodes as inputs and outputs
  CG.mark_node_as_input(1);
  CG.mark_node_as_input(2);
  CG.mark_node_as_output(7);

  // Now let's create the connections:

  // first layer connections and bias
  CG.connect_node1_to_node2_with_weight(1,3,-10.0);
  CG.connect_node1_to_node2_with_weight(1,4,10.0);
  CG.connect_node1_to_node2_with_weight(2,3,-10.0);
  CG.connect_node1_to_node2_with_weight(2,4,0.5);
  CG.set_bias_of_node(3, 0.0);
  CG.set_bias_of_node(4, 0.0);

  CG.connect_node1_to_node2_with_weight(3,5,1.0);
  CG.connect_node1_to_node2_with_weight(3,6,0.0);
  CG.connect_node1_to_node2_with_weight(4,5,0.0);
  CG.connect_node1_to_node2_with_weight(4,6,1.0);
  CG.set_bias_of_node(5,0.0);
  CG.set_bias_of_node(6,0.0);

  CG.connect_node1_to_node2_with_weight(5,7,0.5);
  CG.connect_node1_to_node2_with_weight(6,7,0.5);
  CG.set_bias_of_node(7, 0.0);

}
