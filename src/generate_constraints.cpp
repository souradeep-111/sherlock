#include "generate_constraints.h"

uint32_t trial_count_for_constraint_generation = 1000;

mutex mtx_gen_cons;
bool debug_gen_constr = true;
constraints_stack :: constraints_stack()
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
}

void constraints_stack :: create_the_input_overapproximation_for_each_neuron(
                          computation_graph & CG,
                          region_constraints & input
)
{
  // Need to write this :
  // Takes in as input some description of the input region and does an
  // analysis to give over and under approximtion of the input ranges to each neuron

  input_region = input;
  map< uint32_t, node >& all_nodes = CG.return_ref_to_all_nodes();
  vector< uint32_t > input_indices, output_indices;
  CG.return_id_of_input_output_nodes(input_indices, output_indices);


  for(auto each_node : all_nodes)
  {
     auto flag  = find(input_indices.begin(), input_indices.end(), each_node.first);
     if(flag == input_indices.end())
     {
       neuron_bounds[each_node.first] = make_pair(-sherlock_parameters.MILP_M, sherlock_parameters.MILP_M);
     }


  }
}

void constraints_stack :: generate_graph_constraints(region_constraints & region,
                                                     computation_graph & CG,
                                                     uint32_t output_node_id)
{
  // Basically encode the whole network here

  // Getting a reference to all the nodes in the computation graph
  map< uint32_t, node >& all_nodes = CG.return_ref_to_all_nodes();

  // Declaring the neurons for the input nodes of the computation graph
  vector< uint32_t > input_node_indices, output_node_indices;
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
  generate_node_constraints(CG, explored_nodes ,output_node_id);

}

void constraints_stack :: generate_node_constraints( computation_graph & CG,
                                                     vector< uint32_t > explored_nodes,
                                                     uint32_t output_node_id)
{
  // NOTE: ALGORITHM--
  // Create a queue for unexplored nodes in the graph
  // Until the queue is empty
      // pop a node
      // Call the function : add constraint for node

      // if threads are available and there is still stuff in the queue
          // pop a node , ..... (basically everything that you did in the main loop )
      // wait for all the threads you started to end

  map< uint32_t, node >& all_nodes = CG.return_ref_to_all_nodes();

  set < uint32_t > unexplored_nodes;

  GRBVar output_var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,  all_nodes[output_node_id].get_node_name());
  neurons.insert(make_pair(output_node_id, output_var));

  vector< thread > threads_currently_running;
  set< uint32_t > nodes_to_explore;

  unexplored_nodes.insert(output_node_id);
  uint32_t current_node_id;

  while(!unexplored_nodes.empty())
  {

      current_node_id = *(unexplored_nodes.begin());
      unexplored_nodes.erase(unexplored_nodes.begin());

      // cout << "Adding constraints for " << current_node_id << endl;
      add_constraints_for_node(*this, current_node_id, CG, model_ptr, nodes_to_explore);
      explored_nodes.push_back(current_node_id);

    // Adding all the new nodes to explore  to the list
    for(set<uint32_t> ::iterator it = nodes_to_explore.begin(); it != nodes_to_explore.end(); it ++)
    {
      unexplored_nodes.insert(*it);
    }

    // cout << "Unexplored nodes : [ " ;
    // for(set<uint32_t> ::iterator it = unexplored_nodes.begin(); it != unexplored_nodes.end(); it ++)
    // {
    //   cout << *it << " ,  ";
    // }
    // cout << " ] " << endl;
  }


}

void add_constraints_for_node(constraints_stack & CS, uint32_t current_node_id,
                             computation_graph & CG, GRBModel * model_ptr,
                             set < uint32_t >& nodes_to_explore )
{

  map< uint32_t, GRBVar > input_nodes_to_the_current_node;
  map< uint32_t, pair< node * , double > > list_of_backward_nodes;

  nodes_to_explore.erase(current_node_id);
  map< uint32_t, node >& all_nodes = CG.return_ref_to_all_nodes();
  all_nodes[current_node_id].get_backward_connections(list_of_backward_nodes);

  GRBVar ip_sum = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,
                                     all_nodes[current_node_id].get_node_name() + "_ip_sum" );

  input_nodes_to_the_current_node.clear();


  for(auto backward_node : list_of_backward_nodes)
  {
    string check_string("input_node");
    auto is_an_input_node = ( CG.return_node_position(backward_node.first)).compare(check_string);

    if(( is_an_input_node != 0) && (CS.neurons.find(backward_node.first) == CS.neurons.end()))
    {
        // Create a  gurobi variable which will be used for adding constraints using that backward
        // nodes' output.
        GRBVar gurobi_var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,
                                               all_nodes[backward_node.first].get_node_name());
        CS.neurons.insert(make_pair(backward_node.first, gurobi_var));
        nodes_to_explore.insert(backward_node.first);
        input_nodes_to_the_current_node.insert(make_pair(backward_node.first, gurobi_var));
    }
    else
    {
      input_nodes_to_the_current_node.insert(make_pair(backward_node.first, CS.neurons[backward_node.first]));
    }

  }


  CS.create_sum_of_inputs_and_return_var(input_nodes_to_the_current_node, all_nodes[current_node_id], ip_sum, model_ptr);
  CS.relate_input_output(all_nodes[current_node_id], ip_sum, CS.neurons[current_node_id], model_ptr);


}

// Remember this function might be implemented inside several threads, need to keep in mind
// how the different threads behave
void constraints_stack :: create_sum_of_inputs_and_return_var(map< uint32_t, GRBVar > & inputs_to_node,
                                                              node current_node,
                                                              GRBVar & sum_variable,
                                                              GRBModel * model_ptr)
{
  // Given a Gurobi Var vector , sum  them up and return a reference to the return val

  GRBVar gurobi_one = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, "const_name");

  map< uint32_t, pair< node* , double > > backward_nodes;
  current_node.get_backward_connections(backward_nodes);
  double data, weight_val, bias_val;
  uint32_t node_index;

  GRBLinExpr expression_1, expression_2;
  GRBLinExpr expression_zero(0.0);
  expression_1 = expression_zero;

  current_node.get_bias(bias_val);

  for(auto some_backward_node : backward_nodes)
  {
    node_index = some_backward_node.first;
    weight_val = some_backward_node.second.second;

    data = weight_val;
    expression_1.addTerms(& data, & inputs_to_node[node_index], 1);
  }

  data = bias_val;
  expression_1.addTerms(& data, & gurobi_one, 1);

  model_ptr->addConstr(expression_1, GRB_EQUAL, sum_variable, current_node.get_node_name() + "_sum_constr_");

}

double constraints_stack :: get_M_val_for_node(uint32_t node_index)
{
  assert(neuron_bounds[node_index].first < neuron_bounds[node_index].second);
  double max;
  if(fabs(neuron_bounds[node_index].first) > fabs(neuron_bounds[node_index].second))
  {
    max = fabs(neuron_bounds[node_index].first);
  }
  else
  {
    max = fabs(neuron_bounds[node_index].second);
  }

  return max;
}
// Remember this function might be implemented inside several threads, need to keep in mind
// how the different threads behave
void constraints_stack :: relate_input_output(node current_node,
                                              GRBVar input_var, GRBVar output_var,
                                              GRBModel * model_ptr)
{
  // Basically input some switch-case type implementation
  // if node_type is a _none_, then just say input = output
  // if node type is _relu_, then add one binary variable, and use the big M from the input ranges
  // computed and add the constraint
  // if node type is sigmoid/tanh/etc , get the upper bound and lower bound constraints
  // add them, and assert that the output is indeed included there
  type node_type = current_node.get_node_type();
  bool skip_all_binary_encoding = false;
  if( !skip_activation_encoding_for_index.empty() )
  {
    skip_all_binary_encoding = true;
  }

    if(find(skip_activation_encoding_for_index.begin(), skip_activation_encoding_for_index.end(), current_node.get_node_number())
      != skip_activation_encoding_for_index.end() )
  {
    node_type = _none_;
  }

  if(node_type == _none_)
  {
      model_ptr->addConstr(input_var, GRB_EQUAL, output_var, current_node.get_node_name() + "_ip_op_constr_" );
  }
  else if(node_type == _relu_)
  {
    if(!sherlock_parameters.encode_relu_new)
    {
      GRBVar current_node_binary_var;
      if(skip_all_binary_encoding)
      {
        current_node_binary_var = model_ptr->addVar( 0.0, 1.0, 0.0, GRB_CONTINUOUS, current_node.get_node_name() + "_delta_" );
      }
      else
      {
        current_node_binary_var = model_ptr->addVar( 0.0, 1.0, 0.0, GRB_BINARY, current_node.get_node_name() + "_delta_" );
      }
      GRBVar gurobi_one = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, "const_name");
      binaries[current_node.get_node_number()] = current_node_binary_var;

      GRBLinExpr expression(0.0);
      double data = 1.0;
      expression.addTerms(& data, & input_var, 1);
      data = get_M_val_for_node(current_node.get_node_number());
      expression.addTerms(& data, & current_node_binary_var, 1);
      data = (sherlock_parameters.int_tolerance * get_M_val_for_node(current_node.get_node_number()));
      expression.addTerms(& data, & gurobi_one, 1);


      model_ptr->addConstr(output_var, GRB_LESS_EQUAL, expression, current_node.get_node_name() + "_ip_op_constr_a_");
      model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, input_var, current_node.get_node_name() + "_ip_op_constr_b_");
      model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, 0.0, current_node.get_node_name() + "_ip_op_constr_c_");

      GRBLinExpr expression_0(0.0);
      data = get_M_val_for_node(current_node.get_node_number());
      expression_0.addTerms(& data, & gurobi_one, 1);
      data = -get_M_val_for_node(current_node.get_node_number());
      expression_0.addTerms( & data, & current_node_binary_var, 1);
      data = get_M_val_for_node(current_node.get_node_number()) * (-sherlock_parameters.int_tolerance);
      expression_0.addTerms(& data, & gurobi_one, 1);


      model_ptr->addConstr(output_var, GRB_LESS_EQUAL, expression_0, current_node.get_node_name() + "_ip_op_constr_d_");


      GRBLinExpr trick_expression(0.0);
      data = sherlock_parameters.epsilon;
      trick_expression.addTerms(& data, & gurobi_one, 1);
      data = (-get_M_val_for_node(current_node.get_node_number()));
      trick_expression.addTerms(& data, & current_node_binary_var, 1);

      model_ptr->addConstr(input_var, GRB_GREATER_EQUAL, trick_expression, current_node.get_node_name() + "_trick_lower");

      GRBLinExpr trick_expression_(0.0);

      data = (-sherlock_parameters.epsilon);
      trick_expression_.addTerms(& data, & gurobi_one, 1);

      data = get_M_val_for_node(current_node.get_node_number());
      trick_expression_.addTerms(&data, & gurobi_one, 1);

      data = (-get_M_val_for_node(current_node.get_node_number()));
      trick_expression_.addTerms(& data, & current_node_binary_var, 1);

      model_ptr->addConstr(input_var, GRB_LESS_EQUAL, trick_expression_, current_node.get_node_name() + "_trick_upper");

    }
    else
    {
      GRBVar current_node_binary_var;
      if(skip_all_binary_encoding)
      {
        current_node_binary_var = model_ptr->addVar( 0.0, 1.0, 0.0, GRB_CONTINUOUS, current_node.get_node_name() + "_delta_" );
      }
      else
      {
        current_node_binary_var = model_ptr->addVar( 0.0, 1.0, 0.0, GRB_BINARY, current_node.get_node_name() + "_delta_" );
      }
      GRBVar gurobi_one = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, "const_name");
      binaries[current_node.get_node_number()] = current_node_binary_var;


      GRBLinExpr expression(0.0);
      double data = 1.0;
      expression.addTerms(& data, & input_var, 1);
      data = get_M_val_for_node(current_node.get_node_number());
      expression.addTerms(& data, & current_node_binary_var, 1);

      model_ptr->addConstr(output_var, GRB_LESS_EQUAL, expression, current_node.get_node_name() + "_ip_op_constr_a_");
      model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, input_var, current_node.get_node_name() + "_ip_op_constr_b_");
      model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, 0.0, current_node.get_node_name() + "_ip_op_constr_c_");

      GRBLinExpr expression_0(0.0);
      data = get_M_val_for_node(current_node.get_node_number());
      expression_0.addTerms(& data, & gurobi_one, 1);

      data = -get_M_val_for_node(current_node.get_node_number());
      expression_0.addTerms(& data, & current_node_binary_var, 1);


      model_ptr->addConstr(output_var, GRB_LESS_EQUAL, expression_0, current_node.get_node_name() + "_ip_op_constr_d_");


      GRBLinExpr trick_expression(0.0);
      data = sherlock_parameters.epsilon;
      trick_expression.addTerms(& data, & gurobi_one, 1);
      data = (-get_M_val_for_node(current_node.get_node_number()));
      trick_expression.addTerms(& data, & current_node_binary_var, 1);

      model_ptr->addConstr(input_var, GRB_GREATER_EQUAL, trick_expression, current_node.get_node_name() + "_trick_lower");

      GRBLinExpr trick_expression_(0.0);

      data = (-sherlock_parameters.epsilon);
      trick_expression_.addTerms(& data, & gurobi_one, 1);

      data = get_M_val_for_node(current_node.get_node_number());
      trick_expression_.addTerms(&data, & gurobi_one, 1);

      data = (-get_M_val_for_node(current_node.get_node_number()));
      trick_expression_.addTerms(& data, & current_node_binary_var, 1);

      model_ptr->addConstr(input_var, GRB_LESS_EQUAL, trick_expression_, current_node.get_node_name() + "_trick_upper");

    }

  }
  else if(node_type == _sigmoid_)
  {
    GRBVar binary_1 = model_ptr->addVar(0.0, 1.0, 0.0, GRB_BINARY, current_node.get_node_name() + "_delta_1_");
    GRBVar binary_2 = model_ptr->addVar(0.0, 1.0, 0.0, GRB_BINARY, current_node.get_node_name() + "_delta_2_");
    GRBVar binary_3 = model_ptr->addVar(0.0, 1.0, 0.0, GRB_BINARY, current_node.get_node_name() + "_delta_3_");
    GRBVar gurobi_one = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, "const_name");

    GRBLinExpr expression(0.0);
    double data = 1.0;
    expression.addTerms(& data, & binary_1, 1);
    data = 1.0;
    expression.addTerms(& data, & binary_2, 1);
    data = 1.0;
    expression.addTerms(& data, & binary_3, 1);
    model_ptr->addConstr(expression, GRB_EQUAL, 1.0, current_node.get_node_name() + "_binary_sum_constraint_");

    model_ptr->addConstr(output_var, GRB_LESS_EQUAL, 1.0, current_node.get_node_name() + "_output_upper_" );
    model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, 0.0, current_node.get_node_name() + "_output_lower_" );

    // Linear Piece #1
    expression = 0.0;
    data = -2.3-sherlock_parameters.epsilon;
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_1, 1);
    model_ptr->addConstr(input_var, GRB_LESS_EQUAL, expression, current_node.get_node_name() + "_left_input_" );

    expression = 0.0;
    data = 0.1;
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_1, 1);
    model_ptr->addConstr(output_var, GRB_LESS_EQUAL, expression, current_node.get_node_name() + "_left_output_");


    // Linear Piece #2
    expression = 0.0;
    data = 2.3 - sherlock_parameters.epsilon;
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_2, 1);
    model_ptr->addConstr(input_var, GRB_LESS_EQUAL, expression, current_node.get_node_name() + "_middle_input_a_" );

    expression = 0.0;
    data = -2.3 + sherlock_parameters.epsilon;
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_2, 1);
    model_ptr->addConstr(input_var, GRB_GREATER_EQUAL, expression, current_node.get_node_name() + "_middle_input_b_" );


    expression = 0.0;
    data = 0.2;
    expression.addTerms(& data, & input_var , 1);
    data = 0.55;
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_2, 1);
    model_ptr->addConstr(output_var, GRB_LESS_EQUAL, expression, current_node.get_node_name() + "_middle_output_a_");

    expression = 0.0;
    data = 0.2;
    expression.addTerms(& data, & input_var , 1);
    data = 0.45;
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_2, 1);
    model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, expression, current_node.get_node_name() + "_middle_output_b_");



    // Linear Piece #3
    expression = 0.0;
    data = 2.3 + sherlock_parameters.epsilon;
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_3, 1);
    model_ptr->addConstr(input_var, GRB_GREATER_EQUAL, expression, current_node.get_node_name() + "_right_input_" );

    expression = 0.0;
    data = 0.9;
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_3, 1);
    model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, expression, current_node.get_node_name() + "_right_output_");

  }
  else if(node_type == _tanh_)
  {
    GRBVar binary_1 = model_ptr->addVar(0.0, 1.0, 0.0, GRB_BINARY, current_node.get_node_name() + "_delta_1_");
    GRBVar binary_2 = model_ptr->addVar(0.0, 1.0, 0.0, GRB_BINARY, current_node.get_node_name() + "_delta_2_");
    GRBVar binary_3 = model_ptr->addVar(0.0, 1.0, 0.0, GRB_BINARY, current_node.get_node_name() + "_delta_3_");
    GRBVar gurobi_one = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, "const_name");

    GRBLinExpr expression(0.0);
    double data = 1.0;
    expression.addTerms(& data, & binary_1, 1);
    data = 1.0;
    expression.addTerms(& data, & binary_2, 1);
    data = 1.0;
    expression.addTerms(& data, & binary_3, 1);
    model_ptr->addConstr(expression, GRB_EQUAL, 1.0, current_node.get_node_name() + "_binary_sum_constraint_");

    model_ptr->addConstr(output_var, GRB_LESS_EQUAL, 1.0, current_node.get_node_name() + "_output_upper_" );
    model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, -1.0, current_node.get_node_name() + "_output_lower_" );

    // Linear Piece #1
    expression = 0.0;
    data = -1.5-sherlock_parameters.epsilon;
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_1, 1);
    model_ptr->addConstr(input_var, GRB_LESS_EQUAL, expression, current_node.get_node_name() + "_left_input_" );

    expression = 0.0;
    data = -0.9;
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_1, 1);
    model_ptr->addConstr(output_var, GRB_LESS_EQUAL, expression, current_node.get_node_name() + "_left_output_");


    // Linear Piece #2
    expression = 0.0;
    data = 1.5 - sherlock_parameters.epsilon;
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_2, 1);
    model_ptr->addConstr(input_var, GRB_LESS_EQUAL, expression, current_node.get_node_name() + "_middle_input_a_" );

    expression = 0.0;
    data = -1.5 + sherlock_parameters.epsilon;
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_2, 1);
    model_ptr->addConstr(input_var, GRB_GREATER_EQUAL, expression, current_node.get_node_name() + "_middle_input_b_" );


    expression = 0.0;
    data = 0.7;
    expression.addTerms(& data, & input_var , 1);
    data = 0.15;
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_2, 1);
    model_ptr->addConstr(output_var, GRB_LESS_EQUAL, expression, current_node.get_node_name() + "_middle_output_a_");

    expression = 0.0;
    data = 0.7;
    expression.addTerms(& data, & input_var , 1);
    data = -0.14;
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_2, 1);
    model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, expression, current_node.get_node_name() + "_middle_output_b_");



    // Linear Piece #3
    expression = 0.0;
    data = 1.5 + sherlock_parameters.epsilon;
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_3, 1);
    model_ptr->addConstr(input_var, GRB_GREATER_EQUAL, expression, current_node.get_node_name() + "_right_input_" );

    expression = 0.0;
    data = 0.9;
    expression.addTerms(& data, & gurobi_one, 1);
    data = -get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & gurobi_one, 1);
    data = get_M_val_for_node(current_node.get_node_number());
    expression.addTerms(& data, & binary_3, 1);
    model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, expression, current_node.get_node_name() + "_right_output_");



  }
  else
  {
    cout << "Node type relation missing from the code " << endl;
    assert(false);
  }
}

void constraints_stack :: delete_and_reinitialize()
{
  if(model_ptr)
    delete model_ptr;
  if(env_ptr)
    delete env_ptr;

  env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  model_ptr = new GRBModel(*env_ptr);

  neurons.clear();
  binaries.clear();
  neuron_bounds.clear();

}

void constraints_stack :: _delete_()
{
  if(model_ptr)
    delete model_ptr;
  if(env_ptr)
    delete env_ptr;

}

void constraints_stack :: add_invariants(
                            computation_graph & neural_network,
                            region_constraints & input_region)
{

  // Make a call to the right function in generate invariants and do the implementation
  network_signatures network_signature;
  network_signature.create_signature_for_graph(neural_network, input_region,
                    trial_count_for_constraint_generation);
  if(network_signature.empty())
  {
    return;
  }
  // So there are 3 types of invariants about the network that is being attempted here

  // Facts about constant neurons
  set< uint32_t > always_on, always_off;

  network_signature.learn_constant_neurons( always_on, always_off);
  check_constant_neurons(neural_network, input_region, always_on, always_off);



  if( (!always_on.empty()) || (!always_off.empty()))
  {
    add_constant_neurons(always_on, always_off);
  }

  /*
  // Facts about same sense neurons
  set< pair< uint32_t, uint32_t > > same_sense_nodes, opposite_sense_nodes;
  network_signatures.learn_pairwise_relationship(trial_count_for_constraint_generation,same_sense_nodes, opposite_sense_nodes);
  check_pairwise_relationship(same_sense_nodes, opposite_sense_nodes);
  if(!(same_sense_nodes.empty() && opposite_sense_nodes.empty()))
  {
    add_pairwise_neurons(same_sense_nodes, opposite_sense_nodes);
  }

  */

  if(debug_gen_constr)
  {
    cout << "Done with adding constant neurons " << endl;
    cout << "Starting to learn implies relation " << endl;
  }

  // Facts about implication relationship about neurons
  set< pair< uint32_t, uint32_t > > true_implication, false_implication;
  network_signature.learn_implies_relationship(trial_count_for_constraint_generation, true_implication, false_implication);

  if(debug_gen_constr)
  {
    cout << "Number of true implications learnt : " << true_implication.size() << endl;
    cout << "Number of false implications learnt : " << false_implication.size() << endl;
  }

  check_implies_relationship(neural_network, input_region ,true_implication, false_implication);

  if(debug_gen_constr)
  {
    cout << "Done with learning implies relation " << endl;
    cout << "Adding implies relation " << endl;
  }

  if( (!true_implication.empty()) || (!false_implication.empty()) )
  {
    add_implication_neurons(true_implication, false_implication);
  }

  if(debug_gen_constr)
  {
    cout << "Done with adding implies relation " << endl;
    cout << "Actually true implications learnt : " << true_implication.size() << endl;
    cout << "Actually false implications learnt : " << false_implication.size() << endl;
  }
}

bool constraints_stack :: optimize(uint32_t node_index, bool direction,
                                   map< uint32_t, double >& neuron_value,
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

   //
   // cout << "Status = " << model_ptr->get(GRB_IntAttr_Status) << endl;
   // cout << "Objective = " << model_ptr->get(GRB_DoubleAttr_ObjVal) << endl;
   // cout << "Objective in the neurons list = " << neurons[node_index].get(GRB_DoubleAttr_X) << endl;
   //
   // GRBVar *vars = 0;
   // int numvars = model_ptr->get(GRB_IntAttr_NumVars);
   // vars = model_ptr->getVars();
   //
   // cout << "Number of variables = " << numvars << endl;
   // for (int j = 0; j < numvars; j++)
   // {
   //    GRBVar v = vars[j];
   //    cout << "For var name = " << v.get(GRB_StringAttr_VarName) ;
   //    cout << "  ------- Value = " << v.get(GRB_DoubleAttr_X)  << endl;
   // }
   //    cout << endl;
   //



   if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
   {
       neuron_value.clear();
       for(auto & some_neuron : neurons)
       {
         neuron_value[some_neuron.first] = some_neuron.second.get(GRB_DoubleAttr_X);
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
   else
   {
       cout << "Some unkown Gurobi flag !" << endl;
       cout << "Flag returned - " << model_ptr->get(GRB_IntAttr_Status) << endl;
       assert(false);
       return false;
   }

   return false;


}

bool constraints_stack :: optimize_enough(uint32_t node_index,
                                          double& current_optima, bool direction,
                                          map< uint32_t, double >& neuron_value)
{
  GRBLinExpr objective_expr;
  objective_expr = 0;
  double data = 1.0;

  objective_expr.addTerms(& data, & neurons[node_index] , 1);

   if(direction)
   {
     data = current_optima + sherlock_parameters.MILP_tolerance;
     model_ptr->getEnv().set(GRB_IntParam_SolutionLimit, 1);
     model_ptr->getEnv().set(GRB_DoubleParam_Cutoff, data);

     model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
   }
   else
   {
     data = current_optima - sherlock_parameters.MILP_tolerance;
     model_ptr->getEnv().set(GRB_IntParam_SolutionLimit, 1);
     model_ptr->getEnv().set(GRB_DoubleParam_Cutoff, data);

     model_ptr->setObjective(objective_expr, GRB_MINIMIZE);
   }

   // Initializing the input vars
   // First, open all plants
    // for (p = 0; p < nPlants; ++p)
    // {
    //   open[p].set(GRB_DoubleAttr_Start, 1.0);
    // }

    vector< int > input_indices = input_region.get_input_indices();

    for(auto index : input_indices)
    {
      neurons[index].set(GRB_DoubleAttr_Start, neuron_value[index]);
    }

   model_ptr->update();
   model_ptr->optimize();


   string s = "./Gurobi_file_created/Linear_program.lp";
   model_ptr->write(s);


   if((model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL) || (model_ptr->get(GRB_IntAttr_Status) == GRB_SOLUTION_LIMIT) )
   {
       neuron_value.clear();
       for(auto & some_neuron : neurons)
       {
         neuron_value[some_neuron.first] = some_neuron.second.get(GRB_DoubleAttr_X);
       }
       current_optima = neuron_value[node_index];
       nodes_explored_last_optimization = model_ptr->get(GRB_DoubleAttr_NodeCount);
       return true;
   }
   else if((model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE) || (model_ptr->get(GRB_IntAttr_Status) == GRB_CUTOFF) )
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
     if((model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL) || (model_ptr->get(GRB_IntAttr_Status) == GRB_SOLUTION_LIMIT) )
     {
         neuron_value.clear();
         for(auto & some_neuron : neurons)
         {
           neuron_value[some_neuron.first] = some_neuron.second.get(GRB_DoubleAttr_X);
         }
         current_optima = neuron_value[node_index];
         nodes_explored_last_optimization = model_ptr->get(GRB_DoubleAttr_NodeCount);
         return true;
     }
     else if((model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE) || (model_ptr->get(GRB_IntAttr_Status) == GRB_CUTOFF) )
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

void constraints_stack :: add_constant_neurons(set<uint32_t>& always_on, set<uint32_t>& always_off)
{
  GRBLinExpr current_constraint;
  double data;


  for(auto some_on_neuron : always_on)
  {
    if(binaries.find(some_on_neuron) == binaries.end())
      continue;

    current_constraint = 0.0;
    current_constraint.clear();
    data = 1;
    current_constraint.addTerms(& data, & binaries[some_on_neuron], 1);
    model_ptr->addConstr(current_constraint, GRB_EQUAL, 0.0, "_valid_ineq_const_node_" + to_string(some_on_neuron) );

  }


  for(auto some_off_neuron : always_off)
  {
    if(binaries.find(some_off_neuron) == binaries.end())
      continue;

    current_constraint = 0.0;
    data = 1;
    current_constraint.addTerms(&data, & binaries[some_off_neuron], 1);
    model_ptr->addConstr(current_constraint, GRB_EQUAL, 1.0, "_valid_inequality_constant_node_" + to_string(some_off_neuron) );
  }

}

void constraints_stack :: check_constant_neurons(computation_graph & neural_network,
                                                 region_constraints & input_region,
                                                 set< uint32_t > & always_on,
                                                 set< uint32_t > & always_off)
{
  if(always_on.empty() && always_off.empty())
    return;

  relaxed_constraints_stack lp_constraints_for_this_node;
  map< uint32_t, double > neuron_and_value;
  double result;
  // lp_constraints_for_this_node.skip_activation_encoding_for_index.push_back(-1);

  set< uint32_t > filtered_always_on, filtered_always_off;

  while(!always_on.empty())
  // For each neuron in the always_on stack check if the minimum is more
  // than 0
  {
      lp_constraints_for_this_node.delete_and_reinitialize();

      uint32_t current_on_neuron_index;
      set< uint32_t > current_set;

      current_on_neuron_index = *( always_on.begin() );
      current_set.insert(current_on_neuron_index);

      lp_constraints_for_this_node.skip_activation_encoding_for_index.clear();
      lp_constraints_for_this_node.skip_activation_encoding_for_index.push_back(current_on_neuron_index);
      // Create the relaxed set of constraints
      lp_constraints_for_this_node.create_the_input_overapproximation_for_each_neuron(neural_network, input_region);
      lp_constraints_for_this_node.generate_graph_constraints(input_region, neural_network, current_set);

      // Check if the input to the neuron is always more than 0
      if(
          lp_constraints_for_this_node.optimize(current_on_neuron_index, false, neuron_and_value, result)
        )
      {

        if(result > 0)
        {
          filtered_always_on.insert(current_on_neuron_index);
        }

      }
      always_on.erase(always_on.begin());
      lp_constraints_for_this_node.skip_activation_encoding_for_index.clear();

  }

  always_on = filtered_always_on;



  while(!always_off.empty())
  // For each neuron in the always_on stack check if the minimum is more
  // than 0
  {
      lp_constraints_for_this_node.delete_and_reinitialize();

      uint32_t current_off_neuron_index;
      set< uint32_t > current_set;

      current_off_neuron_index = *( always_off.begin() );
      current_set.insert(current_off_neuron_index);

      lp_constraints_for_this_node.skip_activation_encoding_for_index.clear();
      lp_constraints_for_this_node.skip_activation_encoding_for_index.push_back(current_off_neuron_index);

      // Create the relaxed set of constraints
      lp_constraints_for_this_node.create_the_input_overapproximation_for_each_neuron(neural_network, input_region);
      lp_constraints_for_this_node.generate_graph_constraints(input_region, neural_network, current_set);

      // Check if the input to the neuron is always less than 0
      if(
          lp_constraints_for_this_node.optimize(current_off_neuron_index, true, neuron_and_value, result)
        )
      {
        if(result < 0)
        {
          filtered_always_off.insert(current_off_neuron_index);
        }

      }


      always_off.erase(always_off.begin());
  }

  always_off = filtered_always_off;

  if(debug_gen_constr)
  {
    cout << "No of actually always on neurons : " << always_on.size() << endl;
    cout << "No of actually always off neurons : " << always_off.size() << endl;
  }
}

void constraints_stack :: add_pairwise_neurons(set< pair< uint32_t, uint32_t > > & same_sense_nodes,
                                               set< pair< uint32_t, uint32_t > > & opposite_sense_nodes)
{
  GRBLinExpr lhs, rhs;
  double data;

  for(auto some_pair : same_sense_nodes)
  {
    lhs = 0.0;
    rhs = 0.0;
    data = 1;
    lhs.addTerms(& data, & binaries[some_pair.first], 1);
    rhs.addTerms(& data, & binaries[some_pair.second], 1);

    model_ptr->addConstr(lhs, GRB_EQUAL, rhs, "_same_sense_node_" + to_string(some_pair.first) + "_" + to_string(some_pair.second) );
  }

  GRBLinExpr current_constraint;
  for(auto some_pair : opposite_sense_nodes)
  {
    current_constraint = 0.0;
    data = 1;
    current_constraint.addTerms(& data, & binaries[some_pair.first], 1);
    current_constraint.addTerms(& data, & binaries[some_pair.second], 1);

    model_ptr->addConstr(current_constraint, GRB_EQUAL, 1.0, "_opposite_sense_node_" + to_string(some_pair.first) + "_" + to_string(some_pair.second) );
  }

}

void constraints_stack :: check_pairwise_relationship(set< pair< uint32_t, uint32_t > > & same_sense_nodes,
                                                      set< pair< uint32_t, uint32_t > > & opposite_sense_nodes )
{
  cout << "This function should not be called, sorry to crash the program !" << endl;
  assert(false);

  // Create the LP relaxed constraints stack

  // For the binary variables, made into LP just assert that,
}

void constraints_stack :: add_implication_neurons(set< pair< uint32_t, uint32_t > > & true_sense_nodes,
                                               set< pair< uint32_t, uint32_t > > & false_sense_nodes)

{
  GRBLinExpr lhs, rhs;
  double data;

  for(auto some_pair : true_sense_nodes)
  {
    if((binaries.find(some_pair.first) == binaries.end()) || (binaries.find(some_pair.second) == binaries.end()))
      continue;
    lhs = 0.0;
    rhs = 0.0;
    data = 1;
    lhs.addTerms(& data, & binaries[some_pair.first], 1);
    rhs.addTerms(& data, & binaries[some_pair.second], 1);

    model_ptr->addConstr(rhs, GRB_LESS_EQUAL, lhs, to_string(some_pair.first) + "_implies_true_" + to_string(some_pair.second) );
  }


  for(auto some_pair : false_sense_nodes)
  {
    if((binaries.find(some_pair.first) == binaries.end()) || (binaries.find(some_pair.second) == binaries.end()))
      continue;

    lhs = 0.0;
    rhs = 0.0;
    data = 1;
    lhs.addTerms(& data, & binaries[some_pair.first], 1);
    rhs.addTerms(& data, & binaries[some_pair.second], 1);

    model_ptr->addConstr(lhs, GRB_LESS_EQUAL, rhs, to_string(some_pair.first) + "_implies_false_" + to_string(some_pair.second) );
  }

}

void constraints_stack :: check_implies_relationship(
                                                    computation_graph & neural_network,
                                                    region_constraints & input_region,
                                                    set< pair< uint32_t, uint32_t > > & true_implication,
                                                    set< pair< uint32_t, uint32_t > > & false_implication)
{
  if(true_implication.empty() && false_implication.empty())
    return;

  relaxed_constraints_stack lp_constraints_for_this_node;
  map< uint32_t, double > neuron_and_value;
  double result;
  lp_constraints_for_this_node.skip_activation_encoding_for_index.push_back(-1);

  set < pair< uint32_t, uint32_t > > filtered_true_implication;
  while(!true_implication.empty())
  // For each neuron activation check if one of them is more than 0, then is the
  // other one also more than 0
  {
    lp_constraints_for_this_node.delete_and_reinitialize();

    pair< uint32_t, uint32_t > current_pair;
    current_pair = *(true_implication.begin());
    lp_constraints_for_this_node.skip_activation_encoding_for_index.clear();
    lp_constraints_for_this_node.skip_activation_encoding_for_index.push_back(current_pair.first);
    lp_constraints_for_this_node.skip_activation_encoding_for_index.push_back(current_pair.second);
    lp_constraints_for_this_node.create_the_input_overapproximation_for_each_neuron(neural_network, input_region);
    set< uint32_t > output_nodes;
    output_nodes.insert(current_pair.first);
    output_nodes.insert(current_pair.second);

    lp_constraints_for_this_node.generate_graph_constraints(input_region, neural_network, output_nodes);



    // Check if the input to the neuron is always more than 0
    if(
        lp_constraints_for_this_node.check_implies_relation(true, current_pair.first, current_pair.second)
      )
    {
        filtered_true_implication.insert(current_pair);
    }

    true_implication.erase(true_implication.begin());
  }

  true_implication = filtered_true_implication;

  set < pair< uint32_t, uint32_t > > filtered_false_implication;
  while(!false_implication.empty())
  // For each neuron activation check if one of them is more than 0, then is the
  // other one also more than 0
  {
    lp_constraints_for_this_node.delete_and_reinitialize();

    pair< uint32_t, uint32_t > current_pair;
    current_pair = *(false_implication.begin());
    lp_constraints_for_this_node.skip_activation_encoding_for_index.clear();
    lp_constraints_for_this_node.skip_activation_encoding_for_index.push_back(current_pair.first);
    lp_constraints_for_this_node.skip_activation_encoding_for_index.push_back(current_pair.second);
    lp_constraints_for_this_node.create_the_input_overapproximation_for_each_neuron(neural_network, input_region);
    set< uint32_t > output_nodes;
    output_nodes.insert(current_pair.first);
    output_nodes.insert(current_pair.second);
    lp_constraints_for_this_node.generate_graph_constraints(input_region, neural_network, output_nodes);


    // Check if the input to the neuron is always more than 0
    if(
        lp_constraints_for_this_node.check_implies_relation(false, current_pair.first, current_pair.second)
      )
    {
        filtered_false_implication.insert(current_pair);
    }

    false_implication.erase(false_implication.begin());
  }

  false_implication = filtered_false_implication;


}

void constraints_stack :: add_linear_constraint(linear_inequality & lin_ineq)
{
  lin_ineq.add_this_constraint_to_MILP_model(neurons, model_ptr);
}


bool relaxed_constraints_stack :: check_implies_relation(bool sense,
                                  uint32_t node_1_index, uint32_t node_2_index)
{

  // For the binary variables, made into LP just assert that,
  // for true implication (Node_1 'on' implies Node_2 is 'on')  : check if min ( (sum of inputs to node_2) - (sum of inputs to node_1) ) > 0
  // for false implication : (Node-1 'off' implies Node_2 'off' ) : check if max ( (sum of inputs to node_2) - (sum of inputs to node_1) )  < 0

  GRBLinExpr objective_expr;
  objective_expr = 0;

  if(sense)
  {

    double data = 1.0;
    objective_expr.addTerms(& data, & neurons[node_2_index] , 1);
    data = -1.0;
    objective_expr.addTerms(& data, & neurons[node_1_index] , 1);
    model_ptr->setObjective(objective_expr, GRB_MINIMIZE);
  }
  else
  {
    double data = 1.0;
    objective_expr.addTerms(& data, & neurons[node_2_index] , 1);
    data = -1.0;
    objective_expr.addTerms(& data, & neurons[node_1_index] , 1);
    model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);

  }


   model_ptr->optimize();
   model_ptr->update();



   string s = "./Gurobi_file_created/Linear_program.lp";
   model_ptr->write(s);

   //
   // cout << "Status = " << model_ptr->get(GRB_IntAttr_Status) << endl;
   // cout << "Objective = " << model_ptr->get(GRB_DoubleAttr_ObjVal) << endl;
   // cout << "Objective in the neurons list = " << neurons[node_index].get(GRB_DoubleAttr_X) << endl;
   //
   // GRBVar *vars = 0;
   // int numvars = model_ptr->get(GRB_IntAttr_NumVars);
   // vars = model_ptr->getVars();
   //
   // cout << "Number of variables = " << numvars << endl;
   // for (int j = 0; j < numvars; j++)
   // {
   //    GRBVar v = vars[j];
   //    cout << "For var name = " << v.get(GRB_StringAttr_VarName) ;
   //    cout << "  ------- Value = " << v.get(GRB_DoubleAttr_X)  << endl;
   // }
   //    cout << endl;
   //




   if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
   {
     if(sense)
     {
       if(model_ptr->get(GRB_DoubleAttr_ObjVal) > 0)
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
       if(model_ptr->get(GRB_DoubleAttr_ObjVal) < 0)
       {
         return true;
       }
       else
       {
         return false;
       }

     }

   }
   else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
   {
       return false;
   }
   else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
   {
     model_ptr->set(GRB_IntParam_DualReductions, 0);
     model_ptr->update();
     model_ptr->optimize();
     if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
     {
       if(sense)
       {
         if(model_ptr->get(GRB_DoubleAttr_ObjVal) > 0)
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
         if(model_ptr->get(GRB_DoubleAttr_ObjVal) < 0)
         {
           return true;
         }
         else
         {
           return false;
         }

       }

     }
     else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
     {
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


void relaxed_constraints_stack :: generate_graph_constraints(
                         region_constraints &  region,
                         computation_graph & CG,
                         set< uint32_t > output_nodes)
{
  // Basically encode the whole network here

  // Getting a reference to all the nodes in the computation graph
  map< uint32_t, node >& all_nodes = CG.return_ref_to_all_nodes();

  // Declaring the neurons for the input nodes of the computation graph
  vector< uint32_t > input_node_indices, output_node_indices;
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
  generate_node_constraints(CG, explored_nodes ,output_nodes);
}


void relaxed_constraints_stack :: generate_node_constraints(
                          computation_graph & CG,
                          vector< uint32_t > explored_nodes,
                          set< uint32_t > output_nodes)

{
  // NOTE: ALGORITHM--
  // Create a queue for unexplored nodes in the graph
  // Until the queue is empty
      // pop a node
      // Call the function : add constraint for node

      // if threads are available and there is still stuff in the queue
          // pop a node , ..... (basically everything that you did in the main loop )
      // wait for all the threads you started to end

  map< uint32_t, node >& all_nodes = CG.return_ref_to_all_nodes();

  set < uint32_t > unexplored_nodes;

  for(set< uint32_t> :: iterator it = output_nodes.begin(); it != output_nodes.end(); it++)
  {
    GRBVar output_var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,  all_nodes[*it].get_node_name());
    neurons.insert(make_pair(*it, output_var));
  }

  vector< thread > threads_currently_running;
  set< uint32_t > nodes_to_explore;

  // unexplored_nodes.insert(output_node_id);
  unexplored_nodes = output_nodes;
  uint32_t current_node_id;

  while(!unexplored_nodes.empty())
  {

      current_node_id = *(unexplored_nodes.begin());
      unexplored_nodes.erase(unexplored_nodes.begin());

      // cout << "Adding constraints for " << current_node_id << endl;
      add_constraints_for_node(*this, current_node_id, CG, model_ptr, nodes_to_explore);
      explored_nodes.push_back(current_node_id);

    // Adding all the new nodes to explore  to the list
    for(set<uint32_t> ::iterator it = nodes_to_explore.begin(); it != nodes_to_explore.end(); it ++)
    {
      unexplored_nodes.insert(*it);
    }
  }

}


void relaxed_constraints_stack :: search_constant_nodes_incrementally()
{
  // NOTE : Algorithm ---
  // Depth counter = 1
  // While( you don't discover any new constnt node )
    // Starting from the input nodes, find the nodes at depth = counter

    // Out of the nodes found, try to mine which ones, form constants,
    // and verify it.

    // Increment counter by 1
    // Set the nodes discovered as constant to the values found


  uint32_t depth_counter = 1;

  bool discovered_new_constant_nodes = true;

  set< uint32_t > constant_nodes;
  while(discovered_new_constant_nodes)
  {
    discovered_new_constant_nodes = false;
    
  }
}
