#include "generate_constraints.h"

constraints_stack :: constraints_stack()
{
  env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  model_ptr = new GRBModel(*env_ptr);

  neurons.clear();
  binaries.clear();
  input_ranges.clear();
}

void constraints_stack :: feed_computation_graph(computation_graph & CG)
{
  all_nodes = CG.return_ref_to_all_nodes();
}

void constraints_stack :: create_the_input_overapproximation()
{
  // Need to write this :
  // Takes in as input some description of the input region and does an
  // analysis to give over and under approximtion of the input ranges to each neuron
  for(auto & some_input_range : input_ranges)
  {
    some_input_range.second.first = -sherlock_parameters.MILP_M;
    some_input_range.second.second = sherlock_parameters.MILP_M;
  }
}

void constraints_stack :: generate_graph_constraints(region_constraints & region, computation_graph & CG, uint32_t output_node_id)
{
  // Basically encode the whole network here

  // Create a map for explored nodes in the graph
  vector < uint32_t > explored_nodes;

  // Declaring the neurons for the input nodes of the computation graph
  vector< uint32_t > & input_node_indices, output_node_indices;
  CG.return_id_of_input_output_nodes(input_node_indices, output_node_indices);
  for(auto & input_node_index : input_node_indices)
  {
    GRBVar var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,  all_nodes[input_node_index].get_node_name() );
    neurons.insert( make_pair( input_node_index , var ) );
  }
  /* NOTE : Don't change 'neurons'  here, it is supposed to be a list of input neurons to the computation graph here */
  // impose constraints on the input nodes of the computation graph
  region.add_this_region_to_MILP_model(neurons, model_ptr);

  // A table which keeps track of the nodes who's constraints have already been added ,
  // we initialize it with the inputs of the computation graph to begin with
  vector < uint32_t > explored_nodes;
  for(auto each_input_index : input_node_indices)
  {
    explored_nodes.push_back(each_input_index);
  }
  // Call generate_node_constraints on the output of the graph
  generate_node_constraints(explored_nodes ,output_node_id);
}

void constraints_stack :: generate_node_constraints( computation_graph & CG
                                                     vector< uint32_t > explored nodes,
                                                     uint32_t output_node_id,
                                                     GRBModel * model_ptr )
{
  // START CODING HERE
  // Basically we are doing a b-f-s traversal of the computation graph in a backward fashion
  // and adding the constraints to the Gurobi model file, as we discover it.

  // NOTE: ALGORITHM--
  // Create a queue for unexplored nodes in the graph
  // Until the queue is empty
      // pop a node
      // remove it from the list of unexplored nodes
      // add all the backedged nodes to the queue
      // call create_sum_of_inputs_and_return_var
      // call relate_input_output function
      // if threads are available and there is still stuff in the queue
          // pop a node , ..... (basically everything that you did in the main loop )
      // wait for all the threads you started to end

  queue < uint32_t > unexplored_nodes;
  map< uint32_t, GRBVar > input_nodes_to_the_current_node;

  GRBVar output_var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,  all_nodes[output_node_id].get_node_name());
  neurons.insert(make_pair(output_node_id, output_var));


  unexplored_nodes.push(output_node_id);
  uint32_t current_node;

  while(!unexplored_nodes.empty())
  {
    current_node = unexplored_nodes.pop();

    auto list_of_backward_nodes;
    all_nodes[current_node].get_backward_connections(list_of_backward_nodes);

    GRBVar ip_sum = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,  all_nodes[current_node].get_node_name() + "_ip_sum" );
    input_nodes_to_the_current_node.clear();

    for(auto backward_node : list_of_backward_nodes)
    {
      string check_string("input_node");
      if( ((all_nodes[backward_node.first]).return_node_type()).compare(check_string) != 0)
      {
        // Push it into the list of nodes to be explored in the future
        unexplored_nodes.push(backward_node.first);

        // Create a  gurobi variable which will be used for adding constraints using that backward
        // nodes' output.
        GRBVar gurobi_var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,
                                               all_nodes[backward_node.first].get_node_name());
        neurons.insert(make_pair(backward_node.first, gurobi_var));
        input_nodes_to_the_current_node.insert(make_pair(backward_node.first, gurobi_var));
      }
    }
    create_sum_of_inputs_and_return_var(input_nodes_to_the_current_node, current_node, model_ptr);
    relate_input_output(current_node.get_node_type(), neurons[current_node.get_node_number()], model_ptr);
    if(available_threads > 1)
    {
      // START CODING HERE
      // REPEAT THE ABOVE CODE AGAIN
    }
  }

}

void constraints_stack :: create_sum_of_inputs_and_return_var(map< uint32_t, GRBVar > & inputs_to_node, GRBVar & current_node, GRBModel * model_ptr)
{
  // Given a Gurobi Var vector , sum  them up and return a reference to the return val
}
void constraints_stack :: relate_input_output(type node_type, GRBVar input_var, GRBVar output_var, GRBModel * model_ptr)
{
  // Basically input some switch-case type implementation
  // if node_type is a _none_, then just say input = output
  // if node type is _relu_, then add one binary variable, and use the big M from the input ranges
  // computed and add the constraint
  // if node type is sigmoid/tanh/etc , get the upper bound and lower bound constraints
  // add them, and assert that the output is indeed included there

}

void constraints_stack :: add_invariants()
{
  // Make a call to the right function in generate invariants and do the implementation

}
