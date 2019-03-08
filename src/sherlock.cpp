#include "sherlock.h"

using namespace std;

sherlock :: sherlock()
{
  // Pretty much does nothing
}

sherlock :: sherlock(computation_graph & CG)
{
  neural_network = CG;
  network_constraints.feed_computation_graph(CG);
}

void sherlock :: optimize_node(uint32_t node_index, double & optima_achived)
{
  uint32_t node_index;
  map< uint32_t, double > neuron_values;
  network_constraints.delete_and_reinitialize();
  network_constraints.create_the_input_overapproximation();
  network_constraints.generate_graph_constraints();

  if( network_constraints.optimize(node_index, direction, neuron_values, optima_achived) )
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

void sherlock :: compute_output_range(uint32_t node_index, region_constraints & input_region, pair < double, double >& output_range )
{

  // Maximizing
  gradient_driven_optimization(node_index, input_region, true, output_range.second);

  // Minimizing :
  gradient_driven_optimization(node_index, input_region, false, output_range.first);

}

void sherlock :: gradient_driven_optimization(uint32_t node_index, region_constraints & input_region, bool direction, double & optima)
{
  vector< uint32_t > indices_of_input_nodes, indices_of_output_nodes;
  neural_network.return_id_of_input_output_nodes(indices_of_input_nodes, indices_of_output_nodes);
  double current_optima;
  map<uint32_t, double > neuron_values, search_point;

  assert(indices_of_output_nodes.size() == 1);
  network_constraints.delete_and_reinitialize();
  network_constraints.feed_computation_graph(neural_network);
  network_constraints.create_the_input_overapproximation(input_region);
  network_constraints.generate_graph_constraints();

  if(!return_random_sample(input_region, search_point))
  {
    cout << "Failed to compute random sample, input region might be infeasible " << endl;
    assert(false);
  }

  current_optima = ((direction) ? (-1e30) : (1e30)) ;

  do{
    if(sherlock_parameters.do_random_restarts)
    {
      perform_gradient_search_with_random_restarts(node_index, direction, input_region, search_point, current_optima);
    }
    else
    {
      perform_gradient_search(node_index, direction, input_region, search_point, current_optima);
    }
    bool res = network_constraints.optimize_enough(node_index, current_optima, direction, neuron_values);
    if(!neuron_values.empty())
    {
      for(auto input_index : indices_of_input_nodes)
      {
        search_point[input_index] = neuron_values[input_index];
      }
    }
    else if(res)
    {
      cout << "Gurobi counter example generation feasible, but didn't return the value " << endl;
      assert(false);
    }

  }while(res)

  optima = current_optima;


}


void sherlock :: compute_output_region(region_constraints & input_region, region_constraints & output_region)
{


}



void sherlock :: perform_gradient_search(uint32_t node_index, bool direction, region_constraints & region,
                                         map< uint32_t, double > & starting_point, double & val)
{
  double improvement = 1e30;
  auto network_gradient;
  map<uint32_t, double > current_point, next_point, network_output_value_1, network_output_value_2;

  while(improvement > sherlock_parameters.grad_termination_limit)
  {
    network_gradient = neural_network.return_gradient_wrt_inputs(node_index, current_point);
    handle_bad_gradients(network_gradient);

    next_point = current_point;
    increment_point_in_direction(next_point, network_gradient, region);

    neural_network.evaluate_graph(current_point, network_output_value_1);
    auto value_prev = network_output_value_1[node_index];
    neural_network.evaluate_graph(next_point, network_output_value_2)
    auto value_curr = network_output_value_2[node_index];

    improvement = value_curr - value_prev;
    improvement = (direction) ? (improvement) : (-improvement);
  }

  val = value_curr;
  return;


}

void sherlock :: perform_gradient_search_with_random_restarts(uint32_t node_index, bool direction, region_constraints & region,
                                                              map< uint32_t, double > & starting_point, double & val )
{
  double improvement = 1e30;
  auto network_gradient;
  map<uint32_t, double > current_point, next_point, network_output_value_1, network_output_value_2, trial_point;
  auto trial_val;

  int restart_count = -1;
  while(restart_count < sherlock_parameters.no_of_random_restarts)
  {
    while(improvement > sherlock_parameters.grad_termination_limit)
    {
      network_gradient = neural_network.return_gradient_wrt_inputs(node_index, current_point);
      handle_bad_gradients(network_gradient);

      next_point = current_point;
      increment_point_in_direction(next_point, network_gradient, region);

      neural_network.evaluate_graph(current_point, network_output_value_1);
      auto value_prev = network_output_value_1[node_index];
      neural_network.evaluate_graph(next_point, network_output_value_2)
      auto value_curr = network_output_value_2[node_index];

      improvement = value_curr - value_prev;
      improvement = (direction) ? (improvement) : (-improvement);
    }

    return_random_sample(region, trial_point, restart_count * 17 + 100);
    neural_network.evaluate_graph(trial_point, trial_val);
    if((direction && (trial_val > value_curr)) || (!direction) && (trial_val < value_curr))
    {
      current_point = trial_point;
    }
    restart_count ++;
  }

  val = value_curr;
  return;

}

void sherlock :: increment_point_in_direction(map<uint32_t, double >& current_values, map<uint32_t, double > direction, region_constraints& region)
{
  map< uint32_t, bool> directions_map;
  if(sherlock_parameters.do_signed_gradient)
  {
    for(auto gradient_in_some_direction : direction )
    {
      bool b = (gradient_in_some_direction.second > 0.0) ? (true) : (false)
      directions_map.insert(make_pair< uint32_t, bool > ( gradient_in_some_direction.first , b ));
    }

    for(auto some_direction_bool : directions_map)
    {
      double movement_amount = (some_direction_bool.second) ? (gradient_rate) : (-1.0 * gradient_rate) ;
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

    for(auto some_direction : directions)
    {
      double movement_amount = some_direction.second * gradient_rate ;
      current_values[some_direction.first] += (movement_amount) ;
      if(!region.check(current_values))
      {
        current_values[some_direction.first] -= (movement_amount) ;
      }
    }

    return;

  }


}

void sherlock :: increment_point_in_direction(map<uint32_t, double >& current_values, map<uint32_t, double > direction)
{
  map< uint32_t, bool> directions_map;
  if(sherlock_parameters.do_signed_gradient)
  {
    for(auto gradient_in_some_direction : direction )
    {
      bool b = (gradient_in_some_direction.second > 0.0) ? (true) : (false)
      directions_map.insert(make_pair< uint32_t, bool > ( gradient_in_some_direction.first , b ));
    }

    for(auto some_direction_bool : directions_map)
    {
      double movement_amount = (some_direction_bool.second) ? (gradient_rate) : (-1.0 * gradient_rate) ;
      current_values[some_direction_bool.first] += (movement_amount) ;
    }
    return;
  }
  else
  {

    for(auto some_direction : directions)
    {
      double movement_amount = some_direction.second * gradient_rate ;
      current_values[some_direction.first] += (movement_amount) ;
    }

    return;

  }


}


void create_computation_graph_from_file(string filename,
                                        computation_graph & CG,
                                        bool has_output_relu)
{
  CG.clear();
  ifstream file;
  file.open(filename.c_str(), ios::open);

  int no_of_inputs, no_of_outputs, no_of_hidden_layers;
  int i, j, k, node_index, no_of_neurons_in_previous_layer;
  vector < uint32_t > indices_of_previous_layer_nodes, indices_of_current_layer_nodes;

  double buffer;
  file >> buffer; no_of_inputs = (int) buffer;
  file >> buffer; no_of_outpus = (int) buffer;
  file >> buffer; no_of_hidden_layers = (int) buffer;

  vector< uint32_t > network_configuration;
  i = 0;
  while(i < no_of_hidden_layers)
  {
    file >> buffer;
    network_configuration.push_back((uint32_t) buffer);
    i++;
  }

  node_index = 0;

  // Creating the input nodes, and adding them to the computation graph
  indices_of_previous_layer_nodes.clear();
  i = 0;
  while(i < no_of_inputs )
  {
    node node_x(node_index, "constant");
    CG.add_new_node(node_index, node_x);
    CG.mark_node_as_input(node_index);
    indices_of_previous_layer_nodes.push_back(node_index);

    node_index ++;
    i++:
  }
  // Reading the hidden neurons if any
  for( i = 0; i < no_of_hidden_layers; i++)
  {
    indices_of_current_layer_nodes.clear();
    for( j = 0 ; j < network_configuration[i] ; j++ )
    {
      node node_x(node_index, "relu");
      CG.add_new_node(node_index, node_x);
      indices_of_current_layer_nodes.push_back();

      no_of_neurons_in_previous_layer = (i == 0) ? (no_of_inputs):(network_configuration[i-1]) ;
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

  i = 0;
  while(i < no_of_outputs )
  {
    node node_x(node_index, "relu");
    CG.add_new_node(node_index, node_x);
    CG.mark_node_as_output(node_index);
    indices_of_previous_layer_nodes.push_back(node_index);
    node_index ++;
    i++:
  }

  // Reading the output mapping
  for( j = 0 ; j < no_of_outputs ; j++ )
  {
    string type = (has_output_relu) ? ("relu") : ("none");

    node node_x(node_index, type);
    CG.add_new_node(node_index, node_x);

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
  CG.connect_node1_to_node2_with_weight(1,3,1.0);
  CG.connect_node1_to_node2_with_weight(1,4,1.0);
  CG.connect_node1_to_node2_with_weight(2,3,1.0);
  CG.connect_node1_to_node2_with_weight(2,4,-0.5);
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
