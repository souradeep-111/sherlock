#include "selective_binarization.h"

bool debug_this = true;
selective_binarization :: selective_binarization()
{
  env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  model_ptr = new GRBModel(*env_ptr);
  model_ptr->set(GRB_DoubleParam_IntFeasTol, sherlock_parameters.int_tolerance);
  neurons.clear();
  binaries.clear();

  neuron_bounds.clear();
  skip_activation_encoding_for_index.clear();
  binaries.clear();
}


bool selective_binarization :: optimize_binarization(uint32_t node_index, bool direction,
                                   map< uint32_t, double >& neuron_value,
                                   map<uint32_t, double > & binary_values,
                                   double & result)
{
  neuron_value.clear();
  GRBLinExpr objective_expr;
  objective_expr = 0;
  double data = 1.0;

  objective_expr.addTerms(& data, & neurons[node_index] , 1);


   if(direction)
   {
     model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
   }
   else
   {
     model_ptr->setObjective(objective_expr, GRB_MINIMIZE);
   }


   model_ptr->optimize();
   model_ptr->update();



   string s = "./Gurobi_file_created/Linear_program.lp";
   model_ptr->write(s);


   if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
   {
       neuron_value.clear();
       for(auto & some_neuron : neurons)
       {
         neuron_value[some_neuron.first] = some_neuron.second.get(GRB_DoubleAttr_X);
       }

       binary_values.clear();
       for(auto & some_binary_value : binaries)
       {
         binary_values[some_binary_value.first]
         = some_binary_value.second.get(GRB_DoubleAttr_X);
       }

       result = neuron_value[node_index];
       nodes_explored_last_optimization = model_ptr->get(GRB_DoubleAttr_NodeCount);
       return true;
   }
   else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
   {
       neuron_value.clear();
       nodes_explored_last_optimization = model_ptr->get(GRB_DoubleAttr_NodeCount);
       return false;
   }
   else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
   {
     model_ptr->set(GRB_IntParam_DualReductions, 0);
     model_ptr->update();
     model_ptr->optimize();
     if( model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL )
     {
         neuron_value.clear();
         for(auto & some_neuron : neurons)
         {
           neuron_value[some_neuron.first] = some_neuron.second.get(GRB_DoubleAttr_X);
         }

         binary_values.clear();
         for(auto & some_binary_value : binaries)
         {
           binary_values[some_binary_value.first] = some_binary_value.second.get(GRB_DoubleAttr_X);
         }

         result = neuron_value[node_index];
         nodes_explored_last_optimization = model_ptr->get(GRB_DoubleAttr_NodeCount);
         return true;
     }
     else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
     {
         neuron_value.clear();
         nodes_explored_last_optimization = model_ptr->get(GRB_DoubleAttr_NodeCount);
         return false;
     }
   }
   else
   {
       cout << "Some unkown Gurobi flag !" << endl;
       cout << "Flag returned - " << model_ptr->get(GRB_IntAttr_Status) << endl;
       assert(false);
       return false;
   }

   return false;


}


void selective_binarization :: set_binary_variables_excluding(set< uint32_t > & do_not_touch)
{
  for(auto & each_neuron : binaries)
  {
    if(do_not_touch.find(each_neuron.first) == do_not_touch.end())
    {
        if(binarize.find(each_neuron.first) != binarize.end())
          each_neuron.second.set(GRB_CharAttr_VType, 'B');
        else
          each_neuron.second.set(GRB_CharAttr_VType, 'C');
    }
    else
    {
      each_neuron.second.set(GRB_CharAttr_VType, 'C');
    }
  }
   // model_ptr->update();

}

bool selective_binarization ::  perform_binarization(uint32_t node_index, double bound,
                                     bool direction, region_constraints & input_region,
                                                     computation_graph & neural_network,
                                                     set<uint32_t> & binarized_neurons)
{
  uint32_t max_iterations = 10;
  uint32_t samples_count = 50;
  double current_optima;
  binarized_neurons.clear();
  set< uint32_t > variables_already_binarized;
  map< uint32_t, double > neuron_values, current_boolean_state;
  set< uint32_t > on_neurons, off_neurons;
  set< uint32_t > binarize_all;

  vector< uint32_t > node_indices;
  neural_network.return_id_of_all_nodes(node_indices);

  if(debug_this)
  cout << "Total neurons = " << node_indices.size() << endl;

  if(debug_this)
  cout << "Reaches here 1" << endl;

  if(sherlock_parameters.skip_invariant_guarantees_in_binarization)
  {
    network_signatures network_signature;
    network_signature.create_signature_for_graph(neural_network, input_region, samples_count);
    network_signature.learn_constant_neurons( on_neurons, off_neurons);
  }
  else
  {
    find_invariants(neural_network, input_region, on_neurons, off_neurons);
  }

  if(debug_this)
  cout << "Reaches here 2" << endl;

  delete_and_reinitialize();
  create_the_input_overapproximation_for_each_neuron(neural_network, input_region);

  generate_graph_constraints(input_region, neural_network, node_index);

  if(debug_this)
  cout << "Reaches here 3" << endl;

  dont_touch = getUnion < uint32_t > (on_neurons, off_neurons);

  // cout << "Constant neurons = [ ";
  // for(auto each_id : dont_touch)
  //   cout << each_id << "  ";
  // cout << " ] " << endl;

  int counter = 0;
  bool first_pass = true;
  variables_already_binarized.clear();
  binarize.clear();

  int strategy_index = 1;


  while(counter < max_iterations)
  {
    cout << "At iteration count - " << counter << " size of learnt neurons -  " <<
    dont_touch.size() << endl;
    if(!first_pass)
    {
      if(strategy_index == 0)
      add_variables_to_binarize(neuron_values, current_boolean_state, binarize);

      if(strategy_index == 1)
      use_layered_binarization_strategy(neural_network,
                                        current_boolean_state, binarize);

    }
    else
      first_pass = false;


    set_binary_variables_excluding(dont_touch);

    if(!sherlock_parameters.skip_invariant_guarantees_in_binarization)
      add_constant_neurons(on_neurons, off_neurons);

    if(optimize_binarization(node_index, direction, neuron_values,
                            current_boolean_state, current_optima))
    {
      if( (direction && (current_optima < bound))
              ||
          ((!direction) && (current_optima > bound))
         )
      {
        if(debug_this)
        {
          cout << "Bound received : " << bound << endl;
          cout << "Optima computed : " << current_optima << endl;
        }
        for(auto each_neuron : binarize)
        {
          binarized_neurons.insert(each_neuron);
        }
        return true;
      }
      else
      {
        if(debug_this)
        {
          cout << "Bound received : " << bound << endl;
          cout << "Optima computed : " << current_optima << endl;
        }

      }

    }
    else
    {

      binarized_neurons.clear();
      return false;
    }

    counter++;
  }

  binarized_neurons.clear();
  return false;
}

void selective_binarization :: add_variables_to_binarize(
                                              map< uint32_t, double > current_assignment,
                                              map< uint32_t, double > current_boolean_state,
                                              set < uint32_t >& binarize_variables)
{
  int counter = 0;
  // binarize_variables.clear();
  cout << "Current Continous State : " ;
  for(auto bool_val : current_boolean_state)
  {
    if((dont_touch.find(bool_val.first) == dont_touch.end())
      &&
      (binarize_variables.find(bool_val.first) == binarize_variables.end() )
    )
    {

      cout << " [ " << bool_val.first << " = " << bool_val.second << " ] , " ;
      if ( (bool_val.second > 1e-3) && (bool_val.second < (1 - 1e-3) ) )
      {
        binarize_variables.insert(bool_val.first);
      }

    }
  }
  cout << endl;
  cout << "Binary Variables chosen ------ " << binarize_variables.size() << endl;
}

void selective_binarization :: use_layered_binarization_strategy(
                                              computation_graph & neural_network,
                                              map< uint32_t, double > current_boolean_state,
                                              set< uint32_t > & binarize_variables)
{

  vector< uint32_t > input_nodes, output_nodes;
  neural_network.return_id_of_input_output_nodes( input_nodes, output_nodes );

  set< uint32_t > nodes_set, next_nodes_set, last_nodes_set;
  neural_network.return_id_of_nodes_at_depth_one_from_set(input_nodes, nodes_set);

  binarize_variables.clear();

  int depth_counter = 1;
  while(nodes_set.size())
  {
    last_nodes_set = nodes_set;
    cout << "Size of nodes at depth : " << depth_counter << " is " << nodes_set.size() << endl;
    if(depth_counter == 1)
    {
      binarize_variables = getUnion <uint32_t> (binarize_variables, last_nodes_set);
      break;
    }
    neural_network.return_id_of_nodes_at_depth_one_from_set(nodes_set, next_nodes_set);
    nodes_set = next_nodes_set;
    depth_counter++;
  }

  cout << "Binary Variables chosen ------ " << binarize_variables.size() << endl;
}
