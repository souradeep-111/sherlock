#include "generate_constraints.h"

mutex mtx_gen_cons;
constraints_stack :: constraints_stack()
{
  env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  model_ptr = new GRBModel(*env_ptr);

  neurons.clear();
  binaries.clear();

  neuron_bounds.clear();
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

  queue < uint32_t > unexplored_nodes;

  GRBVar output_var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,  all_nodes[output_node_id].get_node_name());
  neurons.insert(make_pair(output_node_id, output_var));

  vector< thread > threads_currently_running;
  set< uint32_t > nodes_to_explore;

  unexplored_nodes.push(output_node_id);
  uint32_t current_node_id;

  int available_threads = sherlock_parameters.thread_count_for_constraint_generation;
  while(!unexplored_nodes.empty())
  {
    current_node_id = unexplored_nodes.front();
    unexplored_nodes.pop();
    add_constraints_for_node( *this, current_node_id, CG, model_ptr, nodes_to_explore);
    // Adding all the new nodes to explore  to the list
    for(set<uint32_t> ::iterator it = nodes_to_explore.begin(); it != nodes_to_explore.end(); it ++)
      unexplored_nodes.push(*it);


    explored_nodes.push_back(current_node_id);
    available_threads = sherlock_parameters.thread_count_for_constraint_generation;
    threads_currently_running.clear();
    // Launch all the threads here
    while( (available_threads > 1) && (!unexplored_nodes.empty()) )
    {
      current_node_id = unexplored_nodes.front();
      unexplored_nodes.pop();
      available_threads--;
      thread current_thread( add_constraints_for_node,
                             ref(*this),
                             current_node_id,
                             ref(CG),
                             model_ptr,
                             ref(nodes_to_explore)
                           );
      explored_nodes.push_back(current_node_id);
      threads_currently_running.push_back(move(current_thread));
    }
    // Wait for all threads to complete
    for(thread & some_thread : threads_currently_running)
    {
      if(some_thread.joinable())
      {
        some_thread.join();
        available_threads ++;
      }
    }

    // Adding all the new nodes to explore  to the list
    for(set<uint32_t> ::iterator it = nodes_to_explore.begin(); it != nodes_to_explore.end(); it ++)
      unexplored_nodes.push(*it);

  }

}

void add_constraints_for_node(constraints_stack & CS, uint32_t current_node_id,
                             computation_graph & CG, GRBModel * model_ptr,
                             set < uint32_t >& nodes_to_explore )
{
  map< uint32_t, GRBVar > input_nodes_to_the_current_node;
  map< uint32_t, pair< node * , double > > list_of_backward_nodes;

  // mtx_gen_cons.lock();
  map< uint32_t, node >& all_nodes = CG.return_ref_to_all_nodes();
  all_nodes[current_node_id].get_backward_connections(list_of_backward_nodes);
  // mtx_gen_cons.unlock();

  mtx_gen_cons.lock();
  GRBVar ip_sum = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,
                                     all_nodes[current_node_id].get_node_name() + "_ip_sum" );

  mtx_gen_cons.unlock();

  input_nodes_to_the_current_node.clear();

  for(auto backward_node : list_of_backward_nodes)
  {
    string check_string("input_node");

    mtx_gen_cons.lock();
    auto is_an_input_node = ((all_nodes[backward_node.first]).return_node_type()).compare(check_string);
    mtx_gen_cons.unlock();

    if( is_an_input_node != 0)
    {
      // Push it into the list of nodes to be explored in the future
      mtx_gen_cons.lock();
      nodes_to_explore.insert(backward_node.first);

      // Create a  gurobi variable which will be used for adding constraints using that backward
      // nodes' output.
      GRBVar gurobi_var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,
                                             all_nodes[backward_node.first].get_node_name());

      CS.neurons.insert(make_pair(backward_node.first, gurobi_var));
      mtx_gen_cons.unlock();

      input_nodes_to_the_current_node.insert(make_pair(backward_node.first, gurobi_var));

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

  mtx_gen_cons.lock();
  GRBVar gurobi_one = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, "const_name");
  mtx_gen_cons.unlock();

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

  mtx_gen_cons.lock();
  model_ptr->addConstr(expression_1, GRB_EQUAL, sum_variable, current_node.get_node_name() + "_sum_constr_");
  mtx_gen_cons.unlock();

}

double constraints_stack :: get_M_val_for_node(uint32_t node_index)
{
  assert(neuron_bounds[node_index].first < neuron_bounds[node_index].second);
  double max;
  if(abs(neuron_bounds[node_index].first) > abs(neuron_bounds[node_index].second))
  {
    max = abs(neuron_bounds[node_index].first);
  }
  else
  {
    max = abs(neuron_bounds[node_index].second);
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
  if(node_type == _none_)
  {
      mtx_gen_cons.lock();
      model_ptr->addConstr(input_var, GRB_EQUAL, output_var, current_node.get_node_name() + "_ip_op_constr_" );
      mtx_gen_cons.unlock();
  }
  else if(node_type == _relu_)
  {
      mtx_gen_cons.lock();
      GRBVar current_node_binary_var = model_ptr->addVar( 0.0, 1.0, 0.0, GRB_BINARY, current_node.get_node_name() + "_delta_" );
      GRBVar gurobi_one = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, "const_name");
      mtx_gen_cons.unlock();

      GRBLinExpr expression(0.0);
      double data = 1.0;
      expression.addTerms(& data, & input_var, 1);
      data = get_M_val_for_node(current_node.get_node_number());
      expression.addTerms(& data, & current_node_binary_var, 1);

      mtx_gen_cons.lock();
      model_ptr->addConstr(output_var, GRB_LESS_EQUAL, expression, current_node.get_node_name() + "_ip_op_constr_a_");
      model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, expression, current_node.get_node_name() + "_ip_op_constr_b_");
      model_ptr->addConstr(output_var, GRB_GREATER_EQUAL, 0.0, current_node.get_node_name() + "_ip_op_constr_c_");
      mtx_gen_cons.unlock();

      GRBLinExpr expression_0(0.0);
      data = get_M_val_for_node(current_node.get_node_number());
      expression_0.addTerms(& data, & gurobi_one, 1);
      data = -get_M_val_for_node(current_node.get_node_number());
      expression_0.addTerms( & data, & current_node_binary_var, 1);

      mtx_gen_cons.lock();
      model_ptr->addConstr(output_var, GRB_LESS_EQUAL, expression_0, current_node.get_node_name() + "ip_op_constr_d_");
      mtx_gen_cons.unlock();
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

void constraints_stack :: add_invariants()
{
  // Make a call to the right function in generate invariants and do the implementation

}

bool constraints_stack :: optimize(uint32_t node_index, bool direction,
                                   map< uint32_t, double >& neuron_value,
                                   double & result)
{
  GRBLinExpr objective_expr;
  objective_expr = 0;
  double data = 1.0;

  objective_expr.addTerms(& data, & neurons[node_index] , 1);

   if(direction)
   {
     model_ptr->setObjective(objective_expr, GRB_MINIMIZE);
   }
   else
   {
     model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
   }


   model_ptr->optimize();
   model_ptr->update();


   string s = "./Gurobi_file_created/Linear_program_formed_as_txt";
   model_ptr->write(s);

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
                                          double current_optima, bool direction,
                                          map< uint32_t, double > neuron_value)
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

     model_ptr->setObjective(objective_expr, GRB_MINIMIZE);
   }
   else
   {
     data = current_optima - sherlock_parameters.MILP_tolerance;
     model_ptr->getEnv().set(GRB_IntParam_SolutionLimit, 1);
     model_ptr->getEnv().set(GRB_DoubleParam_Cutoff, data);

     model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
   }


   model_ptr->update();
   model_ptr->optimize();


   string s = "./Gurobi_file_created/Linear_program.txt";
   model_ptr->write(s);

   if((model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL) || (model_ptr->get(GRB_IntAttr_Status) == GRB_SOLUTION_LIMIT) )
   {
       neuron_value.clear();
       for(auto & some_neuron : neurons)
       {
         neuron_value[some_neuron.first] = some_neuron.second.get(GRB_DoubleAttr_X);
       }
       nodes_explored_last_optimization = model_ptr->get(GRB_DoubleAttr_NodeCount);
       return true;
   }
   else if((model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE) || (model_ptr->get(GRB_IntAttr_Status) == GRB_CUTOFF) )
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
