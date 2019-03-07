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
                                        computation_graph & CG)
{
  CG.clear();
  ifstream file;
  file.open(filename.c_str(), ios::open);

  int no_of_inputs, no_of_outputs, no_of_hidden_layers;
  int i, j, k, node_index;
  vector < uint32_t > indices_of_previous_layer_nodes;
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

  // Creating the input nodes, and adding them to the computation graph
  i = 0;
  while(i < no_of_inputs )
  {
    node node_x(node_index, "constant");
    CG.add_new_node(node_index, node_x);
    CG.mark_node_as_input(node_index);

    node_index ++;
    i++:
  }
  node_index = 0;
  // Reading the hidden neurons if any
  for( i = 0; i < no_of_hidden_layers; i++)
  {

    for( j = 0 ; j < network_configuration[i] ; j++ )
    {
      node node_x(node_index, "relu");
      CG.add_new_node(node_index, node_x);
      node_index ++;
      // create a neuron node
      // read all the weights leading into it
      // read the bias term for this one,
    }
  }

  // Reading the output mapping

}
