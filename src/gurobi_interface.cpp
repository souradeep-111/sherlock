#include "gurobi_interface.h"
#define node ("n_")    // 1
#define epsilon ("e_") // 2
#define delta ("d_")   // 3
#define con ("c_")     // 4

GRBEnv * env_ptr = NULL;
GRBModel * model_ptr = NULL;
vector< GRBVar > input_neurons;
GRBVar last_neuron;

// static double M = numeric_limits<double> :: max() ;
static double M = sherlock_parameters.MILP_M;
int prove_limit_in_NN(
  vector< vector< datatype > > region_constraints,
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  datatype limit_found,
  vector< datatype > extrema_point,
  int direction
)
/*
1)  Create the neuron variables
2)  Create the epsilon variables
3)  create the delta variables as reals b/w 0 and 1
4)  Create the constraints variables
    : The input constraints
    : The network constraints
    : The output constraints
5)  set the objective function
6)  do optimization
7)  if infeasible,
      return 1
8)  else,
      return 0

*/
{

      vector< unsigned int > network_configuration_buffer;
      vector< unsigned int > total_network_configuration;
      deduce_network_configuration(weights, biases, network_configuration_buffer);
      datatype data;
      unsigned int no_of_inputs, no_of_outputs, i , j , k,
                   no_of_input_constraints, no_of_hidden_layers, sum;


      no_of_inputs = (weights[0][0]).size();
      total_network_configuration.push_back(no_of_inputs);
      i = 0;
      while(i < network_configuration_buffer.size())
      {
        total_network_configuration.push_back(network_configuration_buffer[i]);
        i++;
      }


      no_of_hidden_layers = weights.size() - 1;
      no_of_inputs = (weights[0][0]).size();
      no_of_input_constraints = region_constraints.size();

      if(no_of_inputs != ((region_constraints[0]).size() - 1))
      {
        cout << "No of inputs not matching in find_counter_example_in_NN() " << endl;
        cout << "Exiting .. " << endl;
        exit(0);
      }
      if(!no_of_input_constraints)
      {
        cout << "No constraints received in prove_limit_in_NN() .. " << endl;
        cout << "Exiting .. " << endl;
        exit(0);
      }
      no_of_outputs = (weights[no_of_hidden_layers]).size();
      if(no_of_outputs != 1)
      {
        cout << "No_of_outputs not equal to 1, in find_counter_example_in_NN().. " << endl;
        cout << "Exiting .. "<< endl;
        exit(0);
      }

      // Finding the M values
      vector< vector< datatype > > over_approximated_input_interval(no_of_inputs, vector< datatype>(2));
      vector< int > direction_vector(no_of_inputs);
      vector< vector< datatype > > M_values;

      if(sherlock_parameters.do_dynamic_M_computation)
      {
          i = 0;
          while(i < no_of_inputs)
          {
            // In the negative direction
            fill(direction_vector.begin(), direction_vector.end(), 0);
            direction_vector[i] = -1;
            find_size_of_enclosed_region_in_direction(region_constraints, direction_vector, data);
            over_approximated_input_interval[i][0] = data;
            // In the positive direction
            fill(direction_vector.begin(), direction_vector.end(), 0);
            direction_vector[i] = 1;
            find_size_of_enclosed_region_in_direction(region_constraints, direction_vector, data);
            over_approximated_input_interval[i][1] = data;
            i++;
          }
          compute_M_values_with_interval_propagation(weights, biases, over_approximated_input_interval, M_values);
          // print_2D_vector(M_values);
      }
    // Done with finding the M values

      GRBEnv * env_ptr = new GRBEnv();
      erase_line();
      env_ptr->set(GRB_IntParam_OutputFlag, 0);

      GRBModel * model_ptr = new GRBModel(*env_ptr);

      string const_name = "constant";
      GRBVar const_var = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, const_name);

      // Creating names for all the neurons involved
      vector< vector< string > > neuron_names;
      vector< string > names_vector;
      string name;

      // The names for the input neurons, layer = 0
      j = 0;
      while(j < total_network_configuration[0])
      {
        produce_string_for_variable_index(name, 0, j, 1);
        names_vector.push_back(name);
        j++;
      }
      neuron_names.push_back(names_vector);

      // For the internal neurons

      i = 1;
      while(i < (no_of_hidden_layers+1) )
      {
        names_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          produce_string_for_variable_index(name, i, j, 1);
          names_vector.push_back(name);
          j++;
        }
        neuron_names.push_back(names_vector);
        i++;
      }

      // For the output neurons

      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[no_of_hidden_layers+1])
      {
        produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 1);
        names_vector.push_back(name);
        j++;
      }
      neuron_names.push_back(names_vector);
      // Create the neurons variables
      vector < vector< GRBVar > > neuron_variables;
      vector< GRBVar > var_vector;
      GRBVar var;
      i = 0;
      while(i < (no_of_hidden_layers + 2))
      {
        var_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          var = model_ptr->addVar(-GRB_INFINITY,
                                   GRB_INFINITY,
                                   0.0,
                                   GRB_CONTINUOUS,
                                   neuron_names[i][j]);

          var_vector.push_back(var);
          j++;
        }
        neuron_variables.push_back(var_vector);
        i++;
      }

      vector< vector< string > > epsilon_names;

      // The names for the epsilons, layer = 0

      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[0])
      {
        produce_string_for_variable_index(name, 0, j, 2);
        names_vector.push_back(name);
        j++;
      }
      epsilon_names.push_back(names_vector);

      // For the internal neurons

      i = 1;
      while(i < (no_of_hidden_layers + 1) )
      {
        names_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          produce_string_for_variable_index(name, i, j, 2);
          names_vector.push_back(name);
          j++;
        }
        epsilon_names.push_back(names_vector);
        i++;
      }

      // For the output neurons

      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[no_of_hidden_layers+1])
      {
        produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 2);
        names_vector.push_back(name);
        j++;
      }
      epsilon_names.push_back(names_vector);



      // Create the epsilon variables
      vector < vector< GRBVar > > epsilon_variables;

      i = 0;
      while(i < (no_of_hidden_layers + 2))
      {
        var_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          var = model_ptr->addVar( 0,
                                   GRB_INFINITY,
                                   0.0,
                                   GRB_CONTINUOUS,
                                   epsilon_names[i][j]);

          var_vector.push_back(var);
          j++;
        }
        epsilon_variables.push_back(var_vector);
        i++;
      }

      vector< vector< string > > delta_names;

      // The names for the delta, layer = 0

      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[0])
      {
        produce_string_for_variable_index(name, 0, j, 3);
        names_vector.push_back(name);
        j++;
      }
      delta_names.push_back(names_vector);

      // For the internal neurons

      i = 1;
      while(i < (no_of_hidden_layers + 1) )
      {
        names_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          produce_string_for_variable_index(name, i, j, 3);
          names_vector.push_back(name);
          j++;
        }
        delta_names.push_back(names_vector);
        i++;
      }

      // For the output neurons

      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[no_of_hidden_layers+1])
      {
        produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 3);
        names_vector.push_back(name);
        j++;
      }
      delta_names.push_back(names_vector);

      // Create the delta variables
      vector < vector< GRBVar > > delta_variables;

      i = 0;
      while(i < (no_of_hidden_layers + 2))
      {
        var_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          var = model_ptr->addVar( 0.0,
                                   1.0,
                                   0.0,
                                   GRB_CONTINUOUS,
                                   delta_names[i][j]);

          var_vector.push_back(var);
          j++;
        }
        delta_variables.push_back(var_vector);
        i++;
      }

      // Creating the names for constraint variables

      vector< vector< vector< string > > > constraint_names;
      vector< vector< string > > neuron_constraint_names;
      vector< string > sub_constraint_names;

      // FOR THE CONSTRAINTS ON THE INPUT NEURONS

      neuron_constraint_names.clear();
      j = 0;
      while(j < region_constraints.size())
      {
        sub_constraint_names.clear();

        produce_string_for_variable_index(name, 0, j, 4);
        name += "_a" ;
        sub_constraint_names.push_back(name);
        neuron_constraint_names.push_back(sub_constraint_names);
        j++;
      }

      constraint_names.push_back(neuron_constraint_names);

      // For the internal neurons
      i = 1;
      while(i < (no_of_hidden_layers + 1) )
      {
        neuron_constraint_names.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          sub_constraint_names.clear();

          produce_string_for_variable_index(name, i, j, 4);
          name += "_a";
          sub_constraint_names.push_back(name);
          produce_string_for_variable_index(name, i, j, 4);
          name += "_b";
          sub_constraint_names.push_back(name);
          produce_string_for_variable_index(name, i, j, 4);
          name += "_c";
          sub_constraint_names.push_back(name);
          produce_string_for_variable_index(name, i, j, 4);
          name += "_d";
          sub_constraint_names.push_back(name);

          neuron_constraint_names.push_back(sub_constraint_names);
          j++;
        }
        constraint_names.push_back(neuron_constraint_names);
        i++;
      }

      // For the output neurons

      neuron_constraint_names.clear();
      j = 0;
      while(j < total_network_configuration[no_of_hidden_layers+1])
      {
        sub_constraint_names.clear();

        produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
        name += "_a";
        sub_constraint_names.push_back(name);
        produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
        name += "_b";
        sub_constraint_names.push_back(name);
        produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
        name += "_c";
        sub_constraint_names.push_back(name);
        produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
        name += "_d";
        sub_constraint_names.push_back(name);

        neuron_constraint_names.push_back(sub_constraint_names);
        j++;
      }
      constraint_names.push_back(neuron_constraint_names);

      // Create the constraint variables
      vector < vector< GRBVar > > constraint_variables;
      GRBLinExpr expr_one(1.0);
      GRBLinExpr expr_zero(0.0);
      GRBLinExpr expr_buffer_0(0.0);
      GRBLinExpr expr_buffer_1(0.0);
      GRBLinExpr expr_buffer_2(0.0);
      GRBLinExpr expr_buffer_3(0.0);

      // Putting the constraints imposed by the input region constraints
      i = 0;
      while(i < no_of_input_constraints)
      {
        expr_buffer_0 = expr_zero;
        j = 0;
        while(j < no_of_inputs)
        {
          data = region_constraints[i][j];
          expr_buffer_0.addTerms(& data, & neuron_variables[0][j], 1);
          j++;
        }
        data = region_constraints[i][j];
        expr_buffer_0.addTerms(& data, & const_var, 1);

        model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constraint_names[0][i][0]);
        i++;
      }

      // Putting the constraint imposed by the network connections

      i = 1;
      while(i < (no_of_hidden_layers + 2))
      {
        j = 0;
        while(j < total_network_configuration[i])
        {
          expr_buffer_0 = 0;
          expr_buffer_1 = 0;
          expr_buffer_2 = 0;
          expr_buffer_3 = 0;

          data = 1;
          expr_buffer_0.addTerms(& data, & neuron_variables[i][j], 1);
          data = -1;
          expr_buffer_1.addTerms(& data, & neuron_variables[i][j], 1);
          data = 1;
          expr_buffer_2.addTerms(& data, & neuron_variables[i][j], 1);
          data = -1;
          expr_buffer_3.addTerms(& data, & neuron_variables[i][j], 1);

          k = 0;
          while(k < total_network_configuration[i-1])
          {
            data = -weights[i-1][j][k];   // negative since we are taking it to the left of the eq
            expr_buffer_0.addTerms(& data, & neuron_variables[i-1][k], 1);

            data = weights[i-1][j][k];
            expr_buffer_1.addTerms(& data, & neuron_variables[i-1][k], 1);

            k++;
          }

          data = -biases[i-1][j];   // negative since we are taking it to the left of the eq
          expr_buffer_0.addTerms(& data, & const_var, 1);
          data = 1;
          expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
          model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][0]);

          data = biases[i-1][j];
          expr_buffer_1.addTerms(& data, & const_var, 1);
          data = 1;
          expr_buffer_1.addTerms(& data, & epsilon_variables[i][j], 1);

          if(sherlock_parameters.do_dynamic_M_computation)
          {
            data =  sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
          }
          else
          {
            data = M;
          }
          expr_buffer_1.addTerms(& data, & delta_variables[i][j], 1);
          model_ptr->addConstr(expr_buffer_1, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][1]);

          model_ptr->addConstr(expr_buffer_2, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][2]);

          if(sherlock_parameters.do_dynamic_M_computation)
          {
            data = sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
          }
          else
          {
            data = M;
          }
          expr_buffer_3.addTerms(& data, & const_var, 1);
          if(sherlock_parameters.do_dynamic_M_computation)
          {
            data = -sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
          }
          else
          {
            data = -M;
          }
          expr_buffer_3.addTerms(& data, & delta_variables[i][j], 1);
          model_ptr->addConstr(expr_buffer_3, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][3]);

          j++;
        }
        i++;
      }

      // Putting the constant of '1'
      expr_buffer_0 = expr_zero;
      data = 1;
      expr_buffer_0.addTerms(& data, & const_var, 1);
      model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0, "constant_constraint");

      // Putting in the constraints imposed by the output neuron limits_found
      if(direction == 1)
      {
        expr_buffer_0 = expr_zero;
        data = 1;
        expr_buffer_0.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
        data = -(limit_found + sherlock_parameters.MILP_tolerance + sherlock_parameters.LP_offset);
        expr_buffer_0.addTerms(& data, & const_var, 1);
        model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, "output_constraint");
      }
      else if(direction == (-1))
      {
        expr_buffer_0 = expr_zero;
        data = -1;
        expr_buffer_0.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
        data = (limit_found - sherlock_parameters.MILP_tolerance - sherlock_parameters.LP_offset);
        expr_buffer_0.addTerms(& data, & const_var, 1);
        model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, "output_constraint");
      }
      else
      {
        cout << "Unkown direction input in check_limits() " << endl;
        cout << "Exiting .. " << endl;
        exit(0);
      }

      // Putting the epsilon bounds
      i = 0;
      while(i < (no_of_hidden_layers + 2))
      {
        var_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          expr_buffer_0 = expr_zero;
          data = 1;
          expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
          model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0,"epsilon_lower_con" );
          j++;
        }
        i++;
      }

    // Epsilon sum constraint

    expr_buffer_0 = expr_zero;
    i = 0;
    while(i < (no_of_hidden_layers + 2))
    {
      var_vector.clear();
      j = 0;
      while(j < total_network_configuration[i])
      {
        data = 1;
        expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
        j++;
      }
      i++;
    }
    model_ptr->addConstr(expr_buffer_0, GRB_LESS_EQUAL, sherlock_parameters.MILP_e_tolerance,"epsilon_sum_con" );

    // Putting the delta bounds
    i = 0;
    while(i < (no_of_hidden_layers + 2))
    {
      var_vector.clear();
      j = 0;
      while(j < total_network_configuration[i])
      {
        expr_buffer_0 = expr_zero;
        data = 1;
        expr_buffer_0.addTerms(& data, & delta_variables[i][j], 1);
        model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0,"delta_lower_con" );
        model_ptr->addConstr(expr_buffer_0, GRB_LESS_EQUAL, 1.0,"delta_upper_con" );
        j++;
      }
      i++;
    }

    // Putting the constraint that it cannot be the extrema_point
    // we already found

    // Create the delta variables
    vector< GRBVar > extrema_delta_variables;
    string extrema_delta = "ex_delta" ;

    i = 0;
    while(i < (2 * no_of_inputs))
    {
      var = model_ptr->addVar( 0.0,
                               1.0,
                               0.0,
                               GRB_BINARY,
                               extrema_delta);

      extrema_delta_variables.push_back(var);
      i++;
    }

    unsigned int count = 0;
    i = 0;
    while(i < no_of_inputs)
    {
      // The lower limit
      expr_buffer_0 = 0;
      data = 1;
      expr_buffer_0.addTerms(& data, & neuron_variables[0][i], 1);

      data = M;
      expr_buffer_0.addTerms(& data, & extrema_delta_variables[count], 1);

      data = (-1 * (extrema_point[i] + sherlock_parameters.LP_tolerance_limit));
      expr_buffer_0.addTerms(& data, & const_var, 1);
      model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0 ,"extrema_constr" );
      count++;

      // The upper limit
      expr_buffer_0 = 0;
      data = -1;
      expr_buffer_0.addTerms(& data, & neuron_variables[0][i], 1);

      data = M;
      expr_buffer_0.addTerms(& data, & extrema_delta_variables[count], 1);

      data = (extrema_point[i] - sherlock_parameters.LP_tolerance_limit);
      expr_buffer_0.addTerms(& data, & const_var, 1);
      model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0 ,"extrema_constr" );
      count++;

      i++;
    }

    expr_buffer_0 = 0;
    i = 0;
    while(i < (no_of_inputs))
    {
      expr_buffer_0 = 0;
      data = 1;
      expr_buffer_0.addTerms(& data, & extrema_delta_variables[2 * i], 1);
      data = 1;
      expr_buffer_0.addTerms(& data, & extrema_delta_variables[2 * i + 1], 1);

      model_ptr->addConstr(expr_buffer_0, GRB_LESS_EQUAL, 1 ,"extrema_delta_constr" );

      i++;
    }

    GRBLinExpr objective_expr;

    objective_expr = 0;

    // i = 1;
    // while(i < (no_of_hidden_layers + 2))
    // {
    //     j = 0;
    //     while(j < total_network_configuration[i])
    //     {
    //       data = 1;
    //       objective_expr.addTerms( & data, & epsilon_variables[i][j], 1);
    //       j++;
    //     }
    //     i++;
    //  }
  //   i = 1;
  //   while(i < (no_of_hidden_layers + 2))
  //   {
  //       j = 0;
  //       while(j < total_network_configuration[i])
  //       {
  //         data = 1;
  //         objective_expr.addTerms( & data, & neuron_variables[i][j], 1);
  //         j++;
  //       }
  //       i++;
  //  }

      // j = 0;
      // while(j < total_network_configuration[no_of_hidden_layers+1])
      // {
      //   data = 1;
      //   objective_expr.addTerms( & data, & neuron_variables[no_of_hidden_layers+1][j], 1);
      //   j++;
      // }


    model_ptr->setObjective(objective_expr, GRB_MINIMIZE);
    model_ptr->optimize();

    // model_ptr->update();
    // string s = "check_file_proving_limits.lp";
    // model_ptr->write(s);
    //
    // exit(0);

    if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
    {
      // cout << "\t Here.. feasible " << endl;
      // cout << "\t limit_found = " << limit_found << endl;
      // cout << "\tOutput = " ;
      // cout << neuron_variables[no_of_hidden_layers+1][0].get(GRB_DoubleAttr_X) << endl;
      // cout << "\t Input = ";
      // cout <<  neuron_variables[0][0].get(GRB_DoubleAttr_X) << "  " <<
      // neuron_variables[0][1].get(GRB_DoubleAttr_X) << "  " << endl;
      // neuron_variables[0][2].get(GRB_DoubleAttr_X) << "  " <<
      // neuron_variables[0][3].get(GRB_DoubleAttr_X) << endl ;
      // cout << "\t extreme point = " << extrema_point[0] << "  " <<
      // extrema_point[1] << "  " <<
      // extrema_point[2] << "  " <<
      // extrema_point[3] << endl;
      //
      // cout << "\t extrema delta = " <<
      // extrema_delta_variables[0].get(GRB_DoubleAttr_X) << " " <<
      // extrema_delta_variables[1].get(GRB_DoubleAttr_X) << " " <<
      // extrema_delta_variables[2].get(GRB_DoubleAttr_X) << " " <<
      // extrema_delta_variables[3].get(GRB_DoubleAttr_X) << " " << endl;
      // exit(0);

      // cout << "objective = " << model_ptr->get(GRB_DoubleAttr_ObjVal) << endl;
      delete model_ptr;
      delete env_ptr;
      return 0;
    }
    else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
    {
      // cout << "\t Here.. infeasible " << endl;
      delete model_ptr;
      delete env_ptr;
      return 1;
    }
    else
    {
      // cout << "Unknown error in gurobi implementation ... " << endl;
      // cout << "Status code = " << model_ptr->get(GRB_IntAttr_Status)  << endl;
      return 0;
    }


    delete model_ptr;
    delete env_ptr;



  return 0;
}

int find_counter_example_in_NN(
  vector< vector< datatype > > region_constraints,
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  vector< datatype >& counter_example,
  datatype& limit_found,
  int direction
)
/*
1)  Create the neuron variables
2)  Create the epsilon variables
3)  create the delta variables as reals b/w 0 and 1
4)  Create the constraints variables
    : The input constraints
    : The network constraints
    : The output constraints
5)  set the objective function
6)  do optimization
7)  if feasible,
      load the counter_example
      return 1
8)  else,
      return 0

*/
{

    if(!sherlock_parameters.do_incremental_MILP)
    {

      vector< unsigned int > network_configuration_buffer;
      vector< unsigned int > total_network_configuration;
      deduce_network_configuration(weights, biases, network_configuration_buffer);
      datatype data;
      unsigned int no_of_inputs, no_of_outputs, i , j , k,
                   no_of_input_constraints, no_of_hidden_layers, sum;


      no_of_inputs = (weights[0][0]).size();
      total_network_configuration.push_back(no_of_inputs);
      i = 0;
      while(i < network_configuration_buffer.size())
      {
        total_network_configuration.push_back(network_configuration_buffer[i]);
        i++;
      }


      no_of_hidden_layers = weights.size() - 1;
      no_of_inputs = (weights[0][0]).size();
      no_of_input_constraints = region_constraints.size();

      if(no_of_inputs != ((region_constraints[0]).size() - 1))
      {
        cout << "No of inputs not matching in find_counter_example_in_NN() " << endl;
        cout << "Exiting .. " << endl;
        exit(0);
      }
      if(!no_of_input_constraints)
      {
        cout << "No constraints received in find_counter_example_in_NN() .. " << endl;
        cout << "Exiting .. " << endl;
        exit(0);
      }
      no_of_outputs = (weights[no_of_hidden_layers]).size();
      if(no_of_outputs != 1)
      {
        cout << "No_of_outputs not equal to 1, in find_counter_example_in_NN().. " << endl;
        cout << "Exiting .. "<< endl;
        exit(0);
      }

      // Finding the M values
      vector< vector< datatype > > over_approximated_input_interval(no_of_inputs, vector< datatype>(2));
      vector< int > direction_vector(no_of_inputs);
      vector< vector< datatype > > M_values;

      if(sherlock_parameters.do_dynamic_M_computation)
      {
          i = 0;
          while(i < no_of_inputs)
          {
            // In the negative direction
            fill(direction_vector.begin(), direction_vector.end(), 0);
            direction_vector[i] = -1;
            find_size_of_enclosed_region_in_direction(region_constraints, direction_vector, data);
            over_approximated_input_interval[i][0] = data;
            // In the positive direction
            fill(direction_vector.begin(), direction_vector.end(), 0);
            direction_vector[i] = 1;
            find_size_of_enclosed_region_in_direction(region_constraints, direction_vector, data);
            over_approximated_input_interval[i][1] = data;
            i++;
          }
          compute_M_values_with_interval_propagation(weights, biases, over_approximated_input_interval, M_values);
          // print_2D_vector(M_values);
      }
      // Done with finding the M values

      // GRBEnv * env_ptr = new GRBEnv();
      // erase_line();
      //
      //
      // env_ptr->set(GRB_IntParam_OutputFlag, 0);
      //
      // GRBModel * model_ptr = new GRBModel(*env_ptr);

      env_ptr = new GRBEnv();
      erase_line();
      env_ptr->set(GRB_IntParam_OutputFlag, 0);
      model_ptr = new GRBModel(*env_ptr);


      string const_name = "constant";
      GRBVar const_var = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, const_name);

      // Creating names for all the neurons involved
      vector< vector< string > > neuron_names;
      vector< string > names_vector;
      string name;

      // The names for the input neurons, layer = 0
      j = 0;
      while(j < total_network_configuration[0])
      {
        produce_string_for_variable_index(name, 0, j, 1);
        names_vector.push_back(name);
        j++;
      }
      neuron_names.push_back(names_vector);

      // For the internal neurons

      i = 1;
      while(i < (no_of_hidden_layers+1) )
      {
        names_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          produce_string_for_variable_index(name, i, j, 1);
          names_vector.push_back(name);
          j++;
        }
        neuron_names.push_back(names_vector);
        i++;
      }

      // For the output neurons

      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[no_of_hidden_layers+1])
      {
        produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 1);
        names_vector.push_back(name);
        j++;
      }
      neuron_names.push_back(names_vector);
      // Create the neurons variables
      vector < vector< GRBVar > > neuron_variables;
      vector< GRBVar > var_vector;
      GRBVar var;
      i = 0;
      while(i < (no_of_hidden_layers + 2))
      {
        var_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          var = model_ptr->addVar(-GRB_INFINITY,
                                   GRB_INFINITY,
                                   0.0,
                                   GRB_CONTINUOUS,
                                   neuron_names[i][j]);

          var_vector.push_back(var);
          j++;
        }
        neuron_variables.push_back(var_vector);
        i++;
      }

      vector< vector< string > > epsilon_names;

      // The names for the epsilons, layer = 0

      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[0])
      {
        produce_string_for_variable_index(name, 0, j, 2);
        names_vector.push_back(name);
        j++;
      }
      epsilon_names.push_back(names_vector);

      // For the internal neurons

      i = 1;
      while(i < (no_of_hidden_layers + 1) )
      {
        names_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          produce_string_for_variable_index(name, i, j, 2);
          names_vector.push_back(name);
          j++;
        }
        epsilon_names.push_back(names_vector);
        i++;
      }

      // For the output neurons

      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[no_of_hidden_layers+1])
      {
        produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 2);
        names_vector.push_back(name);
        j++;
      }
      epsilon_names.push_back(names_vector);



      // Create the epsilon variables
      vector < vector< GRBVar > > epsilon_variables;

      i = 0;
      while(i < (no_of_hidden_layers + 2))
      {
        var_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          var = model_ptr->addVar( 0,
                                   GRB_INFINITY,
                                   0.0,
                                   GRB_CONTINUOUS,
                                   epsilon_names[i][j]);

          var_vector.push_back(var);
          j++;
        }
        epsilon_variables.push_back(var_vector);
        i++;
      }

      vector< vector< string > > delta_names;

      // The names for the delta, layer = 0

      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[0])
      {
        produce_string_for_variable_index(name, 0, j, 3);
        names_vector.push_back(name);
        j++;
      }
      delta_names.push_back(names_vector);

      // For the internal neurons

      i = 1;
      while(i < (no_of_hidden_layers + 1) )
      {
        names_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          produce_string_for_variable_index(name, i, j, 3);
          names_vector.push_back(name);
          j++;
        }
        delta_names.push_back(names_vector);
        i++;
      }

      // For the output neurons

      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[no_of_hidden_layers+1])
      {
        produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 3);
        names_vector.push_back(name);
        j++;
      }
      delta_names.push_back(names_vector);

      // Create the delta variables
      vector < vector< GRBVar > > delta_variables;

      i = 0;
      while(i < (no_of_hidden_layers + 2))
      {
        var_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          var = model_ptr->addVar( 0.0,
                                   1.0,
                                   0.0,
                                   GRB_BINARY,
                                   delta_names[i][j]);

          var_vector.push_back(var);
          j++;
        }
        delta_variables.push_back(var_vector);
        i++;
      }

      // Creating the names for constraint variables

      vector< vector< vector< string > > > constraint_names;
      vector< vector< string > > neuron_constraint_names;
      vector< string > sub_constraint_names;

      // FOR THE CONSTRAINTS ON THE INPUT NEURONS

      neuron_constraint_names.clear();
      j = 0;
      while(j < region_constraints.size())
      {
        sub_constraint_names.clear();

        produce_string_for_variable_index(name, 0, j, 4);
        name += "_a" ;
        sub_constraint_names.push_back(name);
        neuron_constraint_names.push_back(sub_constraint_names);
        j++;
      }

      constraint_names.push_back(neuron_constraint_names);

      // For the internal neurons
      i = 1;
      while(i < (no_of_hidden_layers + 1) )
      {
        neuron_constraint_names.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          sub_constraint_names.clear();

          produce_string_for_variable_index(name, i, j, 4);
          name += "_a";
          sub_constraint_names.push_back(name);
          produce_string_for_variable_index(name, i, j, 4);
          name += "_b";
          sub_constraint_names.push_back(name);
          produce_string_for_variable_index(name, i, j, 4);
          name += "_c";
          sub_constraint_names.push_back(name);
          produce_string_for_variable_index(name, i, j, 4);
          name += "_d";
          sub_constraint_names.push_back(name);

          neuron_constraint_names.push_back(sub_constraint_names);
          j++;
        }
        constraint_names.push_back(neuron_constraint_names);
        i++;
      }

      // For the output neurons

      neuron_constraint_names.clear();
      j = 0;
      while(j < total_network_configuration[no_of_hidden_layers+1])
      {
        sub_constraint_names.clear();

        produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
        name += "_a";
        sub_constraint_names.push_back(name);
        produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
        name += "_b";
        sub_constraint_names.push_back(name);
        produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
        name += "_c";
        sub_constraint_names.push_back(name);
        produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
        name += "_d";
        sub_constraint_names.push_back(name);

        neuron_constraint_names.push_back(sub_constraint_names);
        j++;
      }
      constraint_names.push_back(neuron_constraint_names);

      // Create the constraint variables
      vector < vector< GRBVar > > constraint_variables;
      GRBLinExpr expr_one(1.0);
      GRBLinExpr expr_zero(0.0);
      GRBLinExpr expr_buffer_0(0.0);
      GRBLinExpr expr_buffer_1(0.0);
      GRBLinExpr expr_buffer_2(0.0);
      GRBLinExpr expr_buffer_3(0.0);

      // Putting the constraints imposed by the input region constraints
      i = 0;
      while(i < no_of_input_constraints)
      {
        expr_buffer_0 = expr_zero;
        j = 0;
        while(j < no_of_inputs)
        {
          data = region_constraints[i][j];
          expr_buffer_0.addTerms(& data, & neuron_variables[0][j], 1);
          j++;
        }
        data = region_constraints[i][j];
        expr_buffer_0.addTerms(& data, & const_var, 1);

        model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constraint_names[0][i][0]);
        i++;
      }

      // Putting the constraint imposed by the network connections

      i = 1;
      while(i < (no_of_hidden_layers + 2))
      {
        j = 0;
        while(j < total_network_configuration[i])
        {
          expr_buffer_0 = 0;
          expr_buffer_1 = 0;
          expr_buffer_2 = 0;
          expr_buffer_3 = 0;

          data = 1;
          expr_buffer_0.addTerms(& data, & neuron_variables[i][j], 1);
          data = -1;
          expr_buffer_1.addTerms(& data, & neuron_variables[i][j], 1);
          data = 1;
          expr_buffer_2.addTerms(& data, & neuron_variables[i][j], 1);
          data = -1;
          expr_buffer_3.addTerms(& data, & neuron_variables[i][j], 1);

          k = 0;
          while(k < total_network_configuration[i-1])
          {
            data = -weights[i-1][j][k];   // negative since we are taking it to the left of the eq
            expr_buffer_0.addTerms(& data, & neuron_variables[i-1][k], 1);

            data = weights[i-1][j][k];
            expr_buffer_1.addTerms(& data, & neuron_variables[i-1][k], 1);

            k++;
          }

          data = -biases[i-1][j];   // negative since we are taking it to the left of the eq
          expr_buffer_0.addTerms(& data, & const_var, 1);
          data = 1;
          expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
          model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][0]);

          data = biases[i-1][j];
          expr_buffer_1.addTerms(& data, & const_var, 1);
          data = 1;
          expr_buffer_1.addTerms(& data, & epsilon_variables[i][j], 1);

          if(sherlock_parameters.do_dynamic_M_computation)
          {
            data =  sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
          }
          else
          {
            data = M;
          }
          expr_buffer_1.addTerms(& data, & delta_variables[i][j], 1);
          model_ptr->addConstr(expr_buffer_1, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][1]);

          model_ptr->addConstr(expr_buffer_2, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][2]);

          if(sherlock_parameters.do_dynamic_M_computation)
          {
            data = sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
          }
          else
          {
            data = M;
          }
          expr_buffer_3.addTerms(& data, & const_var, 1);
          if(sherlock_parameters.do_dynamic_M_computation)
          {
            data = -sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
          }
          else
          {
            data = -M;
          }
          expr_buffer_3.addTerms(& data, & delta_variables[i][j], 1);
          model_ptr->addConstr(expr_buffer_3, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][3]);

          j++;
        }
        i++;
      }

      // Putting the constant of '1'
      expr_buffer_0 = expr_zero;
      data = 1;
      expr_buffer_0.addTerms(& data, & const_var, 1);
      model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0, "constant_constraint");

      // Putting the epsilon bounds
      i = 0;
      while(i < (no_of_hidden_layers + 2))
      {
        var_vector.clear();
        j = 0;
        while(j < total_network_configuration[i])
        {
          expr_buffer_0 = expr_zero;
          data = 1;
          expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
          model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0,"epsilon_lower_con" );
          j++;
        }
        i++;
      }

      // Epsilon sum constraint

      expr_buffer_0 = expr_zero;
      i = 0;
      while(i < (no_of_hidden_layers + 2))
      {
          var_vector.clear();
          j = 0;
          while(j < total_network_configuration[i])
          {
              data = 1;
              expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
              j++;
          }
          i++;
      }

      model_ptr->addConstr(expr_buffer_0, GRB_LESS_EQUAL, sherlock_parameters.MILP_e_tolerance,"epsilon_sum_con" );

      // The delta output bound
      // expr_buffer_0 = expr_zero;
      // data = 1;
      // expr_buffer_0.addTerms(& data, & delta_variables[no_of_hidden_layers+1][0], 1);
      // model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0,"delta_output_con" );


      // Putting in the constraints imposed by the output neuron limits_found
      if(direction == 1)
      {
        expr_buffer_0 = expr_zero;
        data = 1;
        expr_buffer_0.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
        data = -(limit_found + sherlock_parameters.MILP_tolerance);
        expr_buffer_0.addTerms(& data, & const_var, 1);
        model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, "output_constraint");
      }
      else if(direction == (-1))
      {
        expr_buffer_0 = expr_zero;
        data = -1;
        expr_buffer_0.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
        data = (limit_found - sherlock_parameters.MILP_tolerance);
        expr_buffer_0.addTerms(& data, & const_var, 1);
        model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, "output_constraint");
      }
      else
      {
        cout << "Unkown direction input in check_limits() " << endl;
        cout << "Exiting .. " << endl;
        exit(0);
      }


      GRBLinExpr objective_expr;

      objective_expr = 0;
      // i = 1;
      // while(i < (no_of_hidden_layers + 2))
      // {
      //   j = 0;
      //   while(j < total_network_configuration[i])
      //   {
      //     data = 1;
      //     objective_expr.addTerms( & data, & epsilon_variables[i][j], 1);
      //     j++;
      //   }
      //   i++;
      // }


      model_ptr->setObjective(objective_expr, GRB_MINIMIZE);
      model_ptr->optimize();

      // model_ptr->update();
      // string s = "check_file_find_counter_ex.lp";
      // model_ptr->write(s);


      if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
      {

          counter_example.clear();
          i = 0;
          while(i < no_of_inputs)
          {
            counter_example.push_back((neuron_variables[0][i]).get(GRB_DoubleAttr_X));
            i++;
          }
          limit_found = neuron_variables[no_of_hidden_layers+1][0].get(GRB_DoubleAttr_X);
          cout << "Node count = " << model_ptr->get(GRB_DoubleAttr_NodeCount) << endl;

          delete model_ptr;
          delete env_ptr;
          return 1;
      }
      else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
      {
          cout << "Node count = " << model_ptr->get(GRB_DoubleAttr_NodeCount) << endl;
          delete model_ptr;
          delete env_ptr;
          counter_example.clear();
          return 0;
      }
      else
      {
          // cout << "Unknown error in gurobi implementation ... " << endl;
          // cout << "Status code = " << model_ptr->get(GRB_IntAttr_Status)  << endl;
          return 0;
      }

      delete model_ptr;
      delete env_ptr;

      return 0;
    }

    else
    {
      vector< unsigned int > network_configuration_buffer;
      vector< unsigned int > total_network_configuration;
      deduce_network_configuration(weights, biases, network_configuration_buffer);
      datatype data;
      unsigned int no_of_inputs, no_of_outputs, i , j , k,
                   no_of_input_constraints, no_of_hidden_layers, sum;


      no_of_inputs = (weights[0][0]).size();
      total_network_configuration.push_back(no_of_inputs);
      i = 0;
      while(i < network_configuration_buffer.size())
      {
        total_network_configuration.push_back(network_configuration_buffer[i]);
        i++;
      }


      no_of_hidden_layers = weights.size() - 1;
      no_of_inputs = (weights[0][0]).size();
      no_of_input_constraints = region_constraints.size();

      if(no_of_inputs != ((region_constraints[0]).size() - 1))
      {
        cout << "No of inputs not matching in find_counter_example_in_NN() " << endl;
        cout << "Exiting .. " << endl;
        exit(0);
      }
      if(!no_of_input_constraints)
      {
        cout << "No constraints received in find_counter_example_in_NN() .. " << endl;
        cout << "Exiting .. " << endl;
        exit(0);
      }
      no_of_outputs = (weights[no_of_hidden_layers]).size();
      if(no_of_outputs != 1)
      {
        cout << "No_of_outputs not equal to 1, in find_counter_example_in_NN().. " << endl;
        cout << "Exiting .. "<< endl;
        exit(0);
      }

      // Finding the M values
      vector< vector< datatype > > over_approximated_input_interval(no_of_inputs, vector< datatype>(2));
      vector< int > direction_vector(no_of_inputs);
      vector< vector< datatype > > M_values;

      if(sherlock_parameters.do_dynamic_M_computation)
      {
          i = 0;
          while(i < no_of_inputs)
          {
            // In the negative direction
            fill(direction_vector.begin(), direction_vector.end(), 0);
            direction_vector[i] = -1;
            find_size_of_enclosed_region_in_direction(region_constraints, direction_vector, data);
            over_approximated_input_interval[i][0] = data;
            // In the positive direction
            fill(direction_vector.begin(), direction_vector.end(), 0);
            direction_vector[i] = 1;
            find_size_of_enclosed_region_in_direction(region_constraints, direction_vector, data);
            over_approximated_input_interval[i][1] = data;
            i++;
          }
          compute_M_values_with_interval_propagation(weights, biases, over_approximated_input_interval, M_values);
          // print_2D_vector(M_values);
      }
      // Done with finding the M values

      // GRBEnv * env_ptr = new GRBEnv();
      // erase_line();
      //
      //
      // env_ptr->set(GRB_IntParam_OutputFlag, 0);
      //
      // GRBModel * model_ptr = new GRBModel(*env_ptr);

      if(!env_ptr || !model_ptr)
      {
        env_ptr = new GRBEnv();
        erase_line();
        env_ptr->set(GRB_IntParam_OutputFlag, 0);
        model_ptr = new GRBModel(*env_ptr);

        string const_name = "constant";
        GRBVar const_var = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, const_name);

        // Creating names for all the neurons involved
        vector< vector< string > > neuron_names;
        vector< string > names_vector;
        string name;

        // The names for the input neurons, layer = 0
        j = 0;
        while(j < total_network_configuration[0])
        {
          produce_string_for_variable_index(name, 0, j, 1);
          names_vector.push_back(name);
          j++;
        }
        neuron_names.push_back(names_vector);

        // For the internal neurons

        i = 1;
        while(i < (no_of_hidden_layers+1) )
        {
          names_vector.clear();
          j = 0;
          while(j < total_network_configuration[i])
          {
            produce_string_for_variable_index(name, i, j, 1);
            names_vector.push_back(name);
            j++;
          }
          neuron_names.push_back(names_vector);
          i++;
        }

        // For the output neurons

        names_vector.clear();
        j = 0;
        while(j < total_network_configuration[no_of_hidden_layers+1])
        {
          produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 1);
          names_vector.push_back(name);
          j++;
        }
        neuron_names.push_back(names_vector);

        // Create the neurons variables
        vector < vector< GRBVar > > neuron_variables;
        vector< GRBVar > var_vector;
        GRBVar var;
        i = 0;
        while(i < (no_of_hidden_layers + 2))
        {
          var_vector.clear();
          j = 0;
          while(j < total_network_configuration[i])
          {
            var = model_ptr->addVar(-GRB_INFINITY,
                                     GRB_INFINITY,
                                     0.0,
                                     GRB_CONTINUOUS,
                                     neuron_names[i][j]);

            var_vector.push_back(var);
            j++;
          }
          neuron_variables.push_back(var_vector);
          i++;
        }

        vector< vector< string > > epsilon_names;

        // The names for the epsilons, layer = 0

        names_vector.clear();
        j = 0;
        while(j < total_network_configuration[0])
        {
          produce_string_for_variable_index(name, 0, j, 2);
          names_vector.push_back(name);
          j++;
        }
        epsilon_names.push_back(names_vector);

        // For the internal neurons

        i = 1;
        while(i < (no_of_hidden_layers + 1) )
        {
          names_vector.clear();
          j = 0;
          while(j < total_network_configuration[i])
          {
            produce_string_for_variable_index(name, i, j, 2);
            names_vector.push_back(name);
            j++;
          }
          epsilon_names.push_back(names_vector);
          i++;
        }

        // For the output neurons

        names_vector.clear();
        j = 0;
        while(j < total_network_configuration[no_of_hidden_layers+1])
        {
          produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 2);
          names_vector.push_back(name);
          j++;
        }
        epsilon_names.push_back(names_vector);



        // Create the epsilon variables
        vector < vector< GRBVar > > epsilon_variables;

        i = 0;
        while(i < (no_of_hidden_layers + 2))
        {
          var_vector.clear();
          j = 0;
          while(j < total_network_configuration[i])
          {
            var = model_ptr->addVar( 0,
                                     GRB_INFINITY,
                                     0.0,
                                     GRB_CONTINUOUS,
                                     epsilon_names[i][j]);

            var_vector.push_back(var);
            j++;
          }
          epsilon_variables.push_back(var_vector);
          i++;
        }

        vector< vector< string > > delta_names;

        // The names for the delta, layer = 0

        names_vector.clear();
        j = 0;
        while(j < total_network_configuration[0])
        {
          produce_string_for_variable_index(name, 0, j, 3);
          names_vector.push_back(name);
          j++;
        }
        delta_names.push_back(names_vector);

        // For the internal neurons

        i = 1;
        while(i < (no_of_hidden_layers + 1) )
        {
          names_vector.clear();
          j = 0;
          while(j < total_network_configuration[i])
          {
            produce_string_for_variable_index(name, i, j, 3);
            names_vector.push_back(name);
            j++;
          }
          delta_names.push_back(names_vector);
          i++;
        }

        // For the output neurons

        names_vector.clear();
        j = 0;
        while(j < total_network_configuration[no_of_hidden_layers+1])
        {
          produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 3);
          names_vector.push_back(name);
          j++;
        }
        delta_names.push_back(names_vector);

        // Create the delta variables
        vector < vector< GRBVar > > delta_variables;

        i = 0;
        while(i < (no_of_hidden_layers + 2))
        {
          var_vector.clear();
          j = 0;
          while(j < total_network_configuration[i])
          {
            var = model_ptr->addVar( 0.0,
                                     1.0,
                                     0.0,
                                     GRB_BINARY,
                                     delta_names[i][j]);

            var_vector.push_back(var);
            j++;
          }
          delta_variables.push_back(var_vector);
          i++;
        }

        // Creating the names for constraint variables

        vector< vector< vector< string > > > constraint_names;
        vector< vector< string > > neuron_constraint_names;
        vector< string > sub_constraint_names;

        // FOR THE CONSTRAINTS ON THE INPUT NEURONS

        neuron_constraint_names.clear();
        j = 0;
        while(j < region_constraints.size())
        {
          sub_constraint_names.clear();

          produce_string_for_variable_index(name, 0, j, 4);
          name += "_a" ;
          sub_constraint_names.push_back(name);
          neuron_constraint_names.push_back(sub_constraint_names);
          j++;
        }

        constraint_names.push_back(neuron_constraint_names);

        // For the internal neurons
        i = 1;
        while(i < (no_of_hidden_layers + 1) )
        {
          neuron_constraint_names.clear();
          j = 0;
          while(j < total_network_configuration[i])
          {
            sub_constraint_names.clear();

            produce_string_for_variable_index(name, i, j, 4);
            name += "_a";
            sub_constraint_names.push_back(name);
            produce_string_for_variable_index(name, i, j, 4);
            name += "_b";
            sub_constraint_names.push_back(name);
            produce_string_for_variable_index(name, i, j, 4);
            name += "_c";
            sub_constraint_names.push_back(name);
            produce_string_for_variable_index(name, i, j, 4);
            name += "_d";
            sub_constraint_names.push_back(name);

            neuron_constraint_names.push_back(sub_constraint_names);
            j++;
          }
          constraint_names.push_back(neuron_constraint_names);
          i++;
        }

        // For the output neurons

        neuron_constraint_names.clear();
        j = 0;
        while(j < total_network_configuration[no_of_hidden_layers+1])
        {
          sub_constraint_names.clear();

          produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
          name += "_a";
          sub_constraint_names.push_back(name);
          produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
          name += "_b";
          sub_constraint_names.push_back(name);
          produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
          name += "_c";
          sub_constraint_names.push_back(name);
          produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
          name += "_d";
          sub_constraint_names.push_back(name);

          neuron_constraint_names.push_back(sub_constraint_names);
          j++;
        }
        constraint_names.push_back(neuron_constraint_names);

        // Create the constraint variables
        vector < vector< GRBVar > > constraint_variables;
        GRBLinExpr expr_one(1.0);
        GRBLinExpr expr_zero(0.0);
        GRBLinExpr expr_buffer_0(0.0);
        GRBLinExpr expr_buffer_1(0.0);
        GRBLinExpr expr_buffer_2(0.0);
        GRBLinExpr expr_buffer_3(0.0);

        // Putting the constraints imposed by the input region constraints
        i = 0;
        while(i < no_of_input_constraints)
        {
          expr_buffer_0 = expr_zero;
          j = 0;
          while(j < no_of_inputs)
          {
            data = region_constraints[i][j];
            expr_buffer_0.addTerms(& data, & neuron_variables[0][j], 1);
            j++;
          }
          data = region_constraints[i][j];
          expr_buffer_0.addTerms(& data, & const_var, 1);

          model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constraint_names[0][i][0]);
          i++;
        }

        // Putting the constraint imposed by the network connections

        i = 1;
        while(i < (no_of_hidden_layers + 2))
        {
          j = 0;
          while(j < total_network_configuration[i])
          {
            expr_buffer_0 = 0;
            expr_buffer_1 = 0;
            expr_buffer_2 = 0;
            expr_buffer_3 = 0;

            data = 1;
            expr_buffer_0.addTerms(& data, & neuron_variables[i][j], 1);
            data = -1;
            expr_buffer_1.addTerms(& data, & neuron_variables[i][j], 1);
            data = 1;
            expr_buffer_2.addTerms(& data, & neuron_variables[i][j], 1);
            data = -1;
            expr_buffer_3.addTerms(& data, & neuron_variables[i][j], 1);

            k = 0;
            while(k < total_network_configuration[i-1])
            {
              data = -weights[i-1][j][k];   // negative since we are taking it to the left of the eq
              expr_buffer_0.addTerms(& data, & neuron_variables[i-1][k], 1);

              data = weights[i-1][j][k];
              expr_buffer_1.addTerms(& data, & neuron_variables[i-1][k], 1);

              k++;
            }

            data = -biases[i-1][j];   // negative since we are taking it to the left of the eq
            expr_buffer_0.addTerms(& data, & const_var, 1);
            data = 1;
            expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
            model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][0]);

            data = biases[i-1][j];
            expr_buffer_1.addTerms(& data, & const_var, 1);
            data = 1;
            expr_buffer_1.addTerms(& data, & epsilon_variables[i][j], 1);

            if(sherlock_parameters.do_dynamic_M_computation)
            {
              data =  sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
            }
            else
            {
              data = M;
            }
            expr_buffer_1.addTerms(& data, & delta_variables[i][j], 1);
            model_ptr->addConstr(expr_buffer_1, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][1]);

            model_ptr->addConstr(expr_buffer_2, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][2]);

            if(sherlock_parameters.do_dynamic_M_computation)
            {
              data = sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
            }
            else
            {
              data = M;
            }
            expr_buffer_3.addTerms(& data, & const_var, 1);
            if(sherlock_parameters.do_dynamic_M_computation)
            {
              data = -sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
            }
            else
            {
              data = -M;
            }
            expr_buffer_3.addTerms(& data, & delta_variables[i][j], 1);
            model_ptr->addConstr(expr_buffer_3, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][3]);

            j++;
          }
          i++;
        }

        // Putting the constant of '1'
        expr_buffer_0 = expr_zero;
        data = 1;
        expr_buffer_0.addTerms(& data, & const_var, 1);
        model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0, "constant_constraint");

        // Putting the epsilon bounds
        i = 0;
        while(i < (no_of_hidden_layers + 2))
        {
          var_vector.clear();
          j = 0;
          while(j < total_network_configuration[i])
          {
            expr_buffer_0 = expr_zero;
            data = 1;
            expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
            model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0,"epsilon_lower_con" );
            j++;
          }
          i++;
        }

        // Epsilon sum constraint

        expr_buffer_0 = expr_zero;
        i = 0;
        while(i < (no_of_hidden_layers + 2))
        {
            var_vector.clear();
            j = 0;
            while(j < total_network_configuration[i])
            {
                data = 1;
                expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
                j++;
            }
            i++;
        }

        model_ptr->addConstr(expr_buffer_0, GRB_LESS_EQUAL, sherlock_parameters.MILP_e_tolerance,"epsilon_sum_con" );

        // The delta output bound
        // expr_buffer_0 = expr_zero;
        // data = 1;
        // expr_buffer_0.addTerms(& data, & delta_variables[no_of_hidden_layers+1][0], 1);
        // model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0,"delta_output_con" );


        // Putting in the constraints imposed by the output neuron limits_found
        if(direction == 1)
        {
          // expr_buffer_0 = expr_zero;
          // data = 1;
          // expr_buffer_0.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
          data = -(limit_found + sherlock_parameters.MILP_tolerance);
          // expr_buffer_0.addTerms(& data, & const_var, 1);
          // model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, "output_constraint");

          model_ptr->getEnv().set(GRB_IntParam_SolutionLimit, 1);
          model_ptr->getEnv().set(GRB_DoubleParam_Cutoff, data);

          // Storing the last neuron for using it later:
          last_neuron = neuron_variables[no_of_hidden_layers+1][0];
        }
        else if(direction == (-1))
        {
          // expr_buffer_0 = expr_zero;
          // data = -1;
          // expr_buffer_0.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
          data = (limit_found - sherlock_parameters.MILP_tolerance);
          // expr_buffer_0.addTerms(& data, & const_var, 1);
          // model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, "output_constraint");

          model_ptr->getEnv().set(GRB_IntParam_SolutionLimit, 1);
          model_ptr->getEnv().set(GRB_DoubleParam_Cutoff, data);

          // Storing the last neuron for using it later:
          last_neuron = neuron_variables[no_of_hidden_layers+1][0];
        }
        else
        {
          cout << "Unkown direction input in check_limits() " << endl;
          cout << "Exiting .. " << endl;
          exit(0);
        }


        GRBLinExpr objective_expr;

        objective_expr = 0;
        // i = 1;
        // while(i < (no_of_hidden_layers + 2))
        // {
        //   j = 0;
        //   while(j < total_network_configuration[i])
        //   {
        //     data = 1;
        //     objective_expr.addTerms( & data, & epsilon_variables[i][j], 1);
        //     j++;
        //   }
        //   i++;
        // }

        if(direction == 1)
        {
          data = -1;
          objective_expr.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
        }
        else if(direction == (-1))
        {
          data = 1;
          objective_expr.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
        }
        else
        {
          cout << "Unkown direction input in check_limits() " << endl;
          cout << "Exiting .. " << endl;
          exit(0);
        }

        model_ptr->setObjective(objective_expr, GRB_MINIMIZE);
        model_ptr->optimize();

        // model_ptr->update();
        // string s = "check_file_find_counter_ex.lp";
        // model_ptr->write(s);


        if((model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL) || (model_ptr->get(GRB_IntAttr_Status) == GRB_SOLUTION_LIMIT))
        {

            counter_example.clear();
            input_neurons.clear();
            i = 0;
            while(i < no_of_inputs)
            {
              counter_example.push_back((neuron_variables[0][i]).get(GRB_DoubleAttr_X));
              input_neurons.push_back(neuron_variables[0][i]);
              i++;
            }
            limit_found = neuron_variables[no_of_hidden_layers+1][0].get(GRB_DoubleAttr_X);
            cout << "Node count = " << model_ptr->get(GRB_DoubleAttr_NodeCount) << endl;
            return 1;
        }
        else if((model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE) || (model_ptr->get(GRB_IntAttr_Status) == GRB_CUTOFF))
        {
            cout << "Node count = " << model_ptr->get(GRB_DoubleAttr_NodeCount) << endl;
            delete model_ptr;
            delete env_ptr;
            model_ptr = NULL;
            env_ptr = NULL;
            counter_example.clear();
            return 0;
        }
        else
        {
            // cout << "Unknown error in gurobi implementation ... " << endl;
            // cout << "Status code = 4" << model_ptr->get(GRB_IntAttr_Status)  << endl;
            return 0;
        }

        return 0;

      }
      else
      {

        string const_name = "constant";
        GRBVar const_var = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, const_name);
        GRBLinExpr expr_one(1.0);
        GRBLinExpr expr_zero(0.0);
        GRBLinExpr expr_buffer_0(0.0);
        GRBLinExpr expr_buffer_1(0.0);
        GRBLinExpr expr_buffer_2(0.0);
        GRBLinExpr expr_buffer_3(0.0);



        // Putting in the constraints imposed by the output neuron limits_found
        // Putting in the constraints imposed by the output neuron limits_found
        if(direction == 1)
        {
          // expr_buffer_0 = expr_zero;
          // data = 1;
          // expr_buffer_0.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
          data = -(limit_found + sherlock_parameters.MILP_tolerance);
          // expr_buffer_0.addTerms(& data, & const_var, 1);
          // model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, "output_constraint");

          model_ptr->getEnv().set(GRB_IntParam_SolutionLimit, 1);
          model_ptr->getEnv().set(GRB_DoubleParam_Cutoff, data);

          // Storing the last neuron for using it later:
          // last_neuron = neuron_variables[no_of_hidden_layers+1][0];
        }
        else if(direction == (-1))
        {
          // expr_buffer_0 = expr_zero;
          // data = -1;
          // expr_buffer_0.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
          data = (limit_found - sherlock_parameters.MILP_tolerance);
          // expr_buffer_0.addTerms(& data, & const_var, 1);
          // model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, "output_constraint");
          model_ptr->getEnv().set(GRB_IntParam_SolutionLimit, 1);
          model_ptr->getEnv().set(GRB_DoubleParam_Cutoff, data);
          // Storing the last neuron for using it later:
          // last_neuron = neuron_variables[no_of_hidden_layers+1][0];
        }
        else
        {
          cout << "Unkown direction input in check_limits() " << endl;
          cout << "Exiting .. " << endl;
          exit(0);
        }


        // GRBLinExpr objective_expr;

        // objective_expr = 0;
        // i = 1;
        // while(i < (no_of_hidden_layers + 2))
        // {
        //   j = 0;
        //   while(j < total_network_configuration[i])
        //   {
        //     data = 1;
        //     objective_expr.addTerms( & data, & epsilon_variables[i][j], 1);
        //     j++;
        //   }
        //   i++;
        // }

        // if(direction == 1)
        // {
        //   data = -1;
        //   objective_expr.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
        // }
        // else if(direction == (-1))
        // {
        //   data = 1;
        //   objective_expr.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
        // }
        // else
        // {
        //   cout << "Unkown direction input in check_limits() " << endl;
        //   cout << "Exiting .. " << endl;
        //   exit(0);
        // }

        // model_ptr->setObjective(objective_expr, GRB_MINIMIZE);
        model_ptr->optimize();

        // model_ptr->update();
        // string s = "check_file_find_counter_ex.lp";
        // model_ptr->write(s);


        if((model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL) || (model_ptr->get(GRB_IntAttr_Status) == GRB_SOLUTION_LIMIT))
        {

            counter_example.clear();
            i = 0;
            while(i < no_of_inputs)
            {
              counter_example.push_back((input_neurons[i]).get(GRB_DoubleAttr_X));
              i++;
            }
            limit_found = last_neuron.get(GRB_DoubleAttr_X);
            cout << "Node count = " << model_ptr->get(GRB_DoubleAttr_NodeCount) << endl;
            return 1;
        }
        else if((model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE) || (model_ptr->get(GRB_IntAttr_Status) == GRB_CUTOFF))
        {
            cout << "Node count = " << model_ptr->get(GRB_DoubleAttr_NodeCount) << endl;
            delete model_ptr;
            delete env_ptr;
            model_ptr = NULL;
            env_ptr = NULL;
            counter_example.clear();
            return 0;
        }
        else
        {
            // cout << "Unknown error in gurobi implementation ... " << endl;
            // cout << "Status code = " << model_ptr->get(GRB_IntAttr_Status)  << endl;
            return 0;
        }

        return 0;


      }


    }

    return 0;
}

void do_network_encoding(
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  GRBModel * net_model_ptr,
  GRBEnv * net_env_ptr,
  vector< GRBVar >& input_variables,
  GRBVar & output_variable
)
{
  vector< unsigned int > network_configuration_buffer;
  vector< unsigned int > total_network_configuration;
  deduce_network_configuration(weights, biases, network_configuration_buffer);
  datatype data;
  unsigned int no_of_inputs, no_of_outputs, i , j , k,
               no_of_input_constraints, no_of_hidden_layers, sum;

  vector< vector< datatype > > region_constraints;


  no_of_inputs = (weights[0][0]).size();
  total_network_configuration.push_back(no_of_inputs);
  i = 0;
  while(i < network_configuration_buffer.size())
  {
    total_network_configuration.push_back(network_configuration_buffer[i]);
    i++;
  }

  no_of_hidden_layers = weights.size() - 1;
  no_of_inputs = (weights[0][0]).size();



  no_of_outputs = (weights[no_of_hidden_layers]).size();
  if(no_of_outputs != 1)
  {
    cout << "No_of_outputs not equal to 1, in do_network_encoding().. " << endl;
    cout << "Exiting .. "<< endl;
    exit(0);
  }

  // Finding the M values
  vector< vector< datatype > > over_approximated_input_interval(no_of_inputs, vector< datatype>(2));
  vector< int > direction_vector(no_of_inputs);
  vector< vector< datatype > > M_values;

  // if(sherlock_parameters.do_dynamic_M_computation)
  // {
  //     i = 0;
  //     while(i < no_of_inputs)
  //     {
  //       // In the negative direction
  //       fill(direction_vector.begin(), direction_vector.end(), 0);
  //       direction_vector[i] = -1;
  //       find_size_of_enclosed_region_in_direction(region_constraints, direction_vector, data);
  //       over_approximated_input_interval[i][0] = data;
  //       // In the positive direction
  //       fill(direction_vector.begin(), direction_vector.end(), 0);
  //       direction_vector[i] = 1;
  //       find_size_of_enclosed_region_in_direction(region_constraints, direction_vector, data);
  //       over_approximated_input_interval[i][1] = data;
  //       i++;
  //     }
  //     compute_M_values_with_interval_propagation(weights, biases, over_approximated_input_interval, M_values);
  //     // print_2D_vector(M_values);
  // }
  // Done with finding the M values
  // GRBEnv * env_ptr = new GRBEnv();
  // erase_line();
  //
  //
  // env_ptr->set(GRB_IntParam_OutputFlag, 0);
  //
  // GRBModel * model_ptr = new GRBModel(*env_ptr);

  if((!net_model_ptr) || (!net_env_ptr))
  {
    GRBEnv * env_ptr = new GRBEnv();
    erase_line();
    env_ptr->set(GRB_IntParam_OutputFlag, 0);
    GRBModel * model_ptr = new GRBModel(* env_ptr);

    net_env_ptr = env_ptr;
    net_model_ptr = model_ptr;
  }

  string const_name = "constant";
  GRBVar const_var = net_model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, const_name);

  // Creating names for all the neurons involved
  vector< vector< string > > neuron_names;
  vector< string > names_vector;
  string name;

  // The names for the input neurons, layer = 0
  j = 0;
  while(j < total_network_configuration[0])
  {
    produce_string_for_variable_index(name, 0, j, 1);
    names_vector.push_back(name);
    j++;
  }
  neuron_names.push_back(names_vector);

  // For the internal neurons

  i = 1;
  while(i < (no_of_hidden_layers+1) )
  {
    names_vector.clear();
    j = 0;
    while(j < total_network_configuration[i])
    {
      produce_string_for_variable_index(name, i, j, 1);
      names_vector.push_back(name);
      j++;
    }
    neuron_names.push_back(names_vector);
    i++;
  }

  // For the output neurons

  names_vector.clear();
  j = 0;
  while(j < total_network_configuration[no_of_hidden_layers+1])
  {
    produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 1);
    names_vector.push_back(name);
    j++;
  }
  neuron_names.push_back(names_vector);
  // Create the neurons variables
  vector < vector< GRBVar > > neuron_variables;
  vector< GRBVar > var_vector;
  GRBVar var;
  i = 0;
  while(i < (no_of_hidden_layers + 2))
  {
    var_vector.clear();
    j = 0;
    while(j < total_network_configuration[i])
    {
      var = net_model_ptr->addVar(-GRB_INFINITY,
                               GRB_INFINITY,
                               0.0,
                               GRB_CONTINUOUS,
                               neuron_names[i][j]);

      var_vector.push_back(var);
      j++;
    }
    neuron_variables.push_back(var_vector);
    i++;
  }

  vector< vector< string > > epsilon_names;

  // The names for the epsilons, layer = 0

  names_vector.clear();
  j = 0;
  while(j < total_network_configuration[0])
  {
    produce_string_for_variable_index(name, 0, j, 2);
    names_vector.push_back(name);
    j++;
  }
  epsilon_names.push_back(names_vector);

  // For the internal neurons

  i = 1;
  while(i < (no_of_hidden_layers + 1) )
  {
    names_vector.clear();
    j = 0;
    while(j < total_network_configuration[i])
    {
      produce_string_for_variable_index(name, i, j, 2);
      names_vector.push_back(name);
      j++;
    }
    epsilon_names.push_back(names_vector);
    i++;
  }

  // For the output neurons

  names_vector.clear();
  j = 0;
  while(j < total_network_configuration[no_of_hidden_layers+1])
  {
    produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 2);
    names_vector.push_back(name);
    j++;
  }
  epsilon_names.push_back(names_vector);



  // Create the epsilon variables
  vector < vector< GRBVar > > epsilon_variables;

  i = 0;
  while(i < (no_of_hidden_layers + 2))
  {
    var_vector.clear();
    j = 0;
    while(j < total_network_configuration[i])
    {
      var = net_model_ptr->addVar( 0,
                               GRB_INFINITY,
                               0.0,
                               GRB_CONTINUOUS,
                               epsilon_names[i][j]);

      var_vector.push_back(var);
      j++;
    }
    epsilon_variables.push_back(var_vector);
    i++;
  }

  vector< vector< string > > delta_names;

  // The names for the delta, layer = 0

  names_vector.clear();
  j = 0;
  while(j < total_network_configuration[0])
  {
    produce_string_for_variable_index(name, 0, j, 3);
    names_vector.push_back(name);
    j++;
  }
  delta_names.push_back(names_vector);

  // For the internal neurons

  i = 1;
  while(i < (no_of_hidden_layers + 1) )
  {
    names_vector.clear();
    j = 0;
    while(j < total_network_configuration[i])
    {
      produce_string_for_variable_index(name, i, j, 3);
      names_vector.push_back(name);
      j++;
    }
    delta_names.push_back(names_vector);
    i++;
  }

  // For the output neurons

  names_vector.clear();
  j = 0;
  while(j < total_network_configuration[no_of_hidden_layers+1])
  {
    produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 3);
    names_vector.push_back(name);
    j++;
  }
  delta_names.push_back(names_vector);

  // Create the delta variables
  vector < vector< GRBVar > > delta_variables;

  i = 0;
  while(i < (no_of_hidden_layers + 2))
  {
    var_vector.clear();
    j = 0;
    while(j < total_network_configuration[i])
    {
      var = net_model_ptr->addVar( 0.0,
                               1.0,
                               0.0,
                               GRB_BINARY,
                               delta_names[i][j]);

      var_vector.push_back(var);
      j++;
    }
    delta_variables.push_back(var_vector);
    i++;
  }

  // Creating the names for constraint variables

  vector< vector< vector< string > > > constraint_names;
  vector< vector< string > > neuron_constraint_names;
  vector< string > sub_constraint_names;

  // FOR THE CONSTRAINTS ON THE INPUT NEURONS

  neuron_constraint_names.clear();
  j = 0;
  while(j < region_constraints.size())
  {
    sub_constraint_names.clear();

    produce_string_for_variable_index(name, 0, j, 4);
    name += "_a" ;
    sub_constraint_names.push_back(name);
    neuron_constraint_names.push_back(sub_constraint_names);
    j++;
  }

  constraint_names.push_back(neuron_constraint_names);

  // For the internal neurons
  i = 1;
  while(i < (no_of_hidden_layers + 1) )
  {
    neuron_constraint_names.clear();
    j = 0;
    while(j < total_network_configuration[i])
    {
      sub_constraint_names.clear();

      produce_string_for_variable_index(name, i, j, 4);
      name += "_a";
      sub_constraint_names.push_back(name);
      produce_string_for_variable_index(name, i, j, 4);
      name += "_b";
      sub_constraint_names.push_back(name);
      produce_string_for_variable_index(name, i, j, 4);
      name += "_c";
      sub_constraint_names.push_back(name);
      produce_string_for_variable_index(name, i, j, 4);
      name += "_d";
      sub_constraint_names.push_back(name);

      neuron_constraint_names.push_back(sub_constraint_names);
      j++;
    }
    constraint_names.push_back(neuron_constraint_names);
    i++;
  }

  // For the output neurons

  neuron_constraint_names.clear();
  j = 0;
  while(j < total_network_configuration[no_of_hidden_layers+1])
  {
    sub_constraint_names.clear();

    produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
    name += "_a";
    sub_constraint_names.push_back(name);
    produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
    name += "_b";
    sub_constraint_names.push_back(name);
    produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
    name += "_c";
    sub_constraint_names.push_back(name);
    produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
    name += "_d";
    sub_constraint_names.push_back(name);

    neuron_constraint_names.push_back(sub_constraint_names);
    j++;
  }
  constraint_names.push_back(neuron_constraint_names);

  // Create the constraint variables
  vector < vector< GRBVar > > constraint_variables;
  GRBLinExpr expr_one(1.0);
  GRBLinExpr expr_zero(0.0);
  GRBLinExpr expr_buffer_0(0.0);
  GRBLinExpr expr_buffer_1(0.0);
  GRBLinExpr expr_buffer_2(0.0);
  GRBLinExpr expr_buffer_3(0.0);

  // Putting the constraints imposed by the input region constraints
  i = 0;
  while(i < no_of_input_constraints)
  {

    expr_buffer_0 = expr_zero;
    j = 0;
    while(j < no_of_inputs)
    {
      data = region_constraints[i][j];
      expr_buffer_0.addTerms(& data, & neuron_variables[0][j], 1);
      j++;
    }
    data = region_constraints[i][j];
    expr_buffer_0.addTerms(& data, & const_var, 1);

    net_model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, -GRB_INFINITY, constraint_names[0][i][0]);
    i++;
  }

  // Putting the constraint imposed by the network connections

  i = 1;
  while(i < (no_of_hidden_layers + 2))
  {
    j = 0;
    while(j < total_network_configuration[i])
    {
      expr_buffer_0 = 0;
      expr_buffer_1 = 0;
      expr_buffer_2 = 0;
      expr_buffer_3 = 0;

      data = 1;
      expr_buffer_0.addTerms(& data, & neuron_variables[i][j], 1);
      data = -1;
      expr_buffer_1.addTerms(& data, & neuron_variables[i][j], 1);
      data = 1;
      expr_buffer_2.addTerms(& data, & neuron_variables[i][j], 1);
      data = -1;
      expr_buffer_3.addTerms(& data, & neuron_variables[i][j], 1);

      k = 0;
      while(k < total_network_configuration[i-1])
      {
        data = -weights[i-1][j][k];   // negative since we are taking it to the left of the eq
        expr_buffer_0.addTerms(& data, & neuron_variables[i-1][k], 1);

        data = weights[i-1][j][k];
        expr_buffer_1.addTerms(& data, & neuron_variables[i-1][k], 1);

        k++;
      }

      data = -biases[i-1][j];   // negative since we are taking it to the left of the eq
      expr_buffer_0.addTerms(& data, & const_var, 1);
      data = 1;
      expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
      net_model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][0]);

      data = biases[i-1][j];
      expr_buffer_1.addTerms(& data, & const_var, 1);
      data = 1;
      expr_buffer_1.addTerms(& data, & epsilon_variables[i][j], 1);

      if(sherlock_parameters.do_dynamic_M_computation)
      {
        data =  sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
      }
      else
      {
        data = M;
      }
      expr_buffer_1.addTerms(& data, & delta_variables[i][j], 1);
      net_model_ptr->addConstr(expr_buffer_1, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][1]);

      net_model_ptr->addConstr(expr_buffer_2, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][2]);

      if(sherlock_parameters.do_dynamic_M_computation)
      {
        data = sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
      }
      else
      {
        data = M;
      }
      expr_buffer_3.addTerms(& data, & const_var, 1);
      if(sherlock_parameters.do_dynamic_M_computation)
      {
        data = -sherlock_parameters.scale_factor_for_M * M_values[i-1][j];
      }
      else
      {
        data = -M;
      }
      expr_buffer_3.addTerms(& data, & delta_variables[i][j], 1);
      net_model_ptr->addConstr(expr_buffer_3, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][3]);

      j++;
    }
    i++;
  }

  // Putting the constant of '1'
  expr_buffer_0 = expr_zero;
  data = 1;
  expr_buffer_0.addTerms(& data, & const_var, 1);
  net_model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0, "constant_constraint");

  // Putting the epsilon bounds
  i = 0;
  while(i < (no_of_hidden_layers + 2))
  {
    var_vector.clear();
    j = 0;
    while(j < total_network_configuration[i])
    {
      expr_buffer_0 = expr_zero;
      data = 1;
      expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
      net_model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0,"epsilon_lower_con" );
      j++;
    }
    i++;
  }

  // Epsilon sum constraint

  expr_buffer_0 = expr_zero;
  i = 0;
  while(i < (no_of_hidden_layers + 2))
  {
      var_vector.clear();
      j = 0;
      while(j < total_network_configuration[i])
      {
          data = 1;
          expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
          j++;
      }
      i++;
  }

  net_model_ptr->addConstr(expr_buffer_0, GRB_LESS_EQUAL, sherlock_parameters.MILP_e_tolerance,"epsilon_sum_con" );

  input_variables = neuron_variables[0];
  output_variable = neuron_variables[no_of_hidden_layers + 1][0];



}

datatype do_MILP_optimization(
  vector< vector< datatype > > region_constraints,
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  vector< datatype >& counter_example,
  // datatype& limit_found,
  int direction
)
/*
1)  Create the neuron variables
2)  Create the epsilon variables
3)  create the delta variables as reals b/w 0 and 1
4)  Create the constraints variables
    : The input constraints
    : The network constraints
    : The output constraints
5)  set the objective function
6)  do optimization
7)  if feasible,
      load the counter_example
      return 1
8)  else,
      return 0

*/
{

    vector< unsigned int > network_configuration_buffer;
    vector< unsigned int > total_network_configuration;
    deduce_network_configuration(weights, biases, network_configuration_buffer);
    datatype data;
    unsigned int no_of_inputs, no_of_outputs, i , j , k,
                 no_of_input_constraints, no_of_hidden_layers, sum;


    no_of_inputs = (weights[0][0]).size();
    total_network_configuration.push_back(no_of_inputs);
    i = 0;
    while(i < network_configuration_buffer.size())
    {
      total_network_configuration.push_back(network_configuration_buffer[i]);
      i++;
    }


    no_of_hidden_layers = weights.size() - 1;
    no_of_inputs = (weights[0][0]).size();
    no_of_input_constraints = region_constraints.size();

    if(no_of_inputs != ((region_constraints[0]).size() - 1))
    {
      cout << "No of inputs not matching in find_counter_example_in_NN() " << endl;
      cout << "Exiting .. " << endl;
      exit(0);
    }
    if(!no_of_input_constraints)
    {
      cout << "No constraints received in prove_limit_in_NN() .. " << endl;
      cout << "Exiting .. " << endl;
      exit(0);
    }
    no_of_outputs = (weights[no_of_hidden_layers]).size();
    if(no_of_outputs != 1)
    {
      cout << "No_of_outputs not equal to 1, in find_counter_example_in_NN().. " << endl;
      cout << "Exiting .. "<< endl;
      exit(0);
    }

    GRBEnv * env_ptr = new GRBEnv();
    erase_line();


    env_ptr->set(GRB_IntParam_OutputFlag, 0);

    GRBModel * model_ptr = new GRBModel(*env_ptr);

    string const_name = "constant";
    GRBVar const_var = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, const_name);

    // Creating names for all the neurons involved
    vector< vector< string > > neuron_names;
    vector< string > names_vector;
    string name;

    // The names for the input neurons, layer = 0
    j = 0;
    while(j < total_network_configuration[0])
    {
      produce_string_for_variable_index(name, 0, j, 1);
      names_vector.push_back(name);
      j++;
    }
    neuron_names.push_back(names_vector);

    // For the internal neurons

    i = 1;
    while(i < (no_of_hidden_layers+1) )
    {
      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[i])
      {
        produce_string_for_variable_index(name, i, j, 1);
        names_vector.push_back(name);
        j++;
      }
      neuron_names.push_back(names_vector);
      i++;
    }

    // For the output neurons

    names_vector.clear();
    j = 0;
    while(j < total_network_configuration[no_of_hidden_layers+1])
    {
      produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 1);
      names_vector.push_back(name);
      j++;
    }
    neuron_names.push_back(names_vector);
    // Create the neurons variables
    vector < vector< GRBVar > > neuron_variables;
    vector< GRBVar > var_vector;
    GRBVar var;
    i = 0;
    while(i < (no_of_hidden_layers + 2))
    {
      var_vector.clear();
      j = 0;
      while(j < total_network_configuration[i])
      {
        var = model_ptr->addVar(-GRB_INFINITY,
                                 GRB_INFINITY,
                                 0.0,
                                 GRB_CONTINUOUS,
                                 neuron_names[i][j]);

        var_vector.push_back(var);
        j++;
      }
      neuron_variables.push_back(var_vector);
      i++;
    }

    vector< vector< string > > epsilon_names;

    // The names for the epsilons, layer = 0

    names_vector.clear();
    j = 0;
    while(j < total_network_configuration[0])
    {
      produce_string_for_variable_index(name, 0, j, 2);
      names_vector.push_back(name);
      j++;
    }
    epsilon_names.push_back(names_vector);

    // For the internal neurons

    i = 1;
    while(i < (no_of_hidden_layers + 1) )
    {
      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[i])
      {
        produce_string_for_variable_index(name, i, j, 2);
        names_vector.push_back(name);
        j++;
      }
      epsilon_names.push_back(names_vector);
      i++;
    }

    // For the output neurons

    names_vector.clear();
    j = 0;
    while(j < total_network_configuration[no_of_hidden_layers+1])
    {
      produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 2);
      names_vector.push_back(name);
      j++;
    }
    epsilon_names.push_back(names_vector);



    // Create the epsilon variables
    vector < vector< GRBVar > > epsilon_variables;

    i = 0;
    while(i < (no_of_hidden_layers + 2))
    {
      var_vector.clear();
      j = 0;
      while(j < total_network_configuration[i])
      {
        var = model_ptr->addVar( 0,
                                 GRB_INFINITY,
                                 0.0,
                                 GRB_CONTINUOUS,
                                 epsilon_names[i][j]);

        var_vector.push_back(var);
        j++;
      }
      epsilon_variables.push_back(var_vector);
      i++;
    }

    vector< vector< string > > delta_names;

    // The names for the delta, layer = 0

    names_vector.clear();
    j = 0;
    while(j < total_network_configuration[0])
    {
      produce_string_for_variable_index(name, 0, j, 3);
      names_vector.push_back(name);
      j++;
    }
    delta_names.push_back(names_vector);

    // For the internal neurons

    i = 1;
    while(i < (no_of_hidden_layers + 1) )
    {
      names_vector.clear();
      j = 0;
      while(j < total_network_configuration[i])
      {
        produce_string_for_variable_index(name, i, j, 3);
        names_vector.push_back(name);
        j++;
      }
      delta_names.push_back(names_vector);
      i++;
    }

    // For the output neurons

    names_vector.clear();
    j = 0;
    while(j < total_network_configuration[no_of_hidden_layers+1])
    {
      produce_string_for_variable_index(name, (no_of_hidden_layers + 1), j, 3);
      names_vector.push_back(name);
      j++;
    }
    delta_names.push_back(names_vector);

    // Create the delta variables
    vector < vector< GRBVar > > delta_variables;

    i = 0;
    while(i < (no_of_hidden_layers + 2))
    {
      var_vector.clear();
      j = 0;
      while(j < total_network_configuration[i])
      {
        var = model_ptr->addVar( 0.0,
                                 1.0,
                                 0.0,
                                 GRB_BINARY,
                                 delta_names[i][j]);

        var_vector.push_back(var);
        j++;
      }
      delta_variables.push_back(var_vector);
      i++;
    }

    // Creating the names for constraint variables

    vector< vector< vector< string > > > constraint_names;
    vector< vector< string > > neuron_constraint_names;
    vector< string > sub_constraint_names;

    // FOR THE CONSTRAINTS ON THE INPUT NEURONS

    neuron_constraint_names.clear();
    j = 0;
    while(j < region_constraints.size())
    {
      sub_constraint_names.clear();

      produce_string_for_variable_index(name, 0, j, 4);
      name += "_a" ;
      sub_constraint_names.push_back(name);
      neuron_constraint_names.push_back(sub_constraint_names);
      j++;
    }

    constraint_names.push_back(neuron_constraint_names);

    // For the internal neurons
    i = 1;
    while(i < (no_of_hidden_layers + 1) )
    {
      neuron_constraint_names.clear();
      j = 0;
      while(j < total_network_configuration[i])
      {
        sub_constraint_names.clear();

        produce_string_for_variable_index(name, i, j, 4);
        name += "_a";
        sub_constraint_names.push_back(name);
        produce_string_for_variable_index(name, i, j, 4);
        name += "_b";
        sub_constraint_names.push_back(name);
        produce_string_for_variable_index(name, i, j, 4);
        name += "_c";
        sub_constraint_names.push_back(name);
        produce_string_for_variable_index(name, i, j, 4);
        name += "_d";
        sub_constraint_names.push_back(name);

        neuron_constraint_names.push_back(sub_constraint_names);
        j++;
      }
      constraint_names.push_back(neuron_constraint_names);
      i++;
    }

    // For the output neurons

    neuron_constraint_names.clear();
    j = 0;
    while(j < total_network_configuration[no_of_hidden_layers+1])
    {
      sub_constraint_names.clear();

      produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
      name += "_a";
      sub_constraint_names.push_back(name);
      produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
      name += "_b";
      sub_constraint_names.push_back(name);
      produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
      name += "_c";
      sub_constraint_names.push_back(name);
      produce_string_for_variable_index(name, no_of_hidden_layers+1, j, 4);
      name += "_d";
      sub_constraint_names.push_back(name);

      neuron_constraint_names.push_back(sub_constraint_names);
      j++;
    }
    constraint_names.push_back(neuron_constraint_names);

    // Create the constraint variables
    vector < vector< GRBVar > > constraint_variables;
    GRBLinExpr expr_one(1.0);
    GRBLinExpr expr_zero(0.0);
    GRBLinExpr expr_buffer_0(0.0);
    GRBLinExpr expr_buffer_1(0.0);
    GRBLinExpr expr_buffer_2(0.0);
    GRBLinExpr expr_buffer_3(0.0);

    // Putting the constraints imposed by the input region constraints
    i = 0;
    while(i < no_of_input_constraints)
    {
      expr_buffer_0 = expr_zero;
      j = 0;
      while(j < no_of_inputs)
      {
        data = region_constraints[i][j];
        expr_buffer_0.addTerms(& data, & neuron_variables[0][j], 1);
        j++;
      }
      data = region_constraints[i][j];
      expr_buffer_0.addTerms(& data, & const_var, 1);

      model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constraint_names[0][i][0]);
      i++;
    }

    // Putting the constraint imposed by the network connections

    i = 1;
    while(i < (no_of_hidden_layers + 2))
    {
      j = 0;
      while(j < total_network_configuration[i])
      {
        expr_buffer_0 = 0;
        expr_buffer_1 = 0;
        expr_buffer_2 = 0;
        expr_buffer_3 = 0;

        data = 1;
        expr_buffer_0.addTerms(& data, & neuron_variables[i][j], 1);
        data = -1;
        expr_buffer_1.addTerms(& data, & neuron_variables[i][j], 1);
        data = 1;
        expr_buffer_2.addTerms(& data, & neuron_variables[i][j], 1);
        data = -1;
        expr_buffer_3.addTerms(& data, & neuron_variables[i][j], 1);

        k = 0;
        while(k < total_network_configuration[i-1])
        {
          data = -weights[i-1][j][k];   // negative since we are taking it to the left of the eq
          expr_buffer_0.addTerms(& data, & neuron_variables[i-1][k], 1);

          data = weights[i-1][j][k];
          expr_buffer_1.addTerms(& data, & neuron_variables[i-1][k], 1);

          k++;
        }

        data = -biases[i-1][j];   // negative since we are taking it to the left of the eq
        expr_buffer_0.addTerms(& data, & const_var, 1);
        data = 1;
        expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
        model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][0]);

        data = biases[i-1][j];
        expr_buffer_1.addTerms(& data, & const_var, 1);
        data = 1;
        expr_buffer_1.addTerms(& data, & epsilon_variables[i][j], 1);
        data = M;
        expr_buffer_1.addTerms(& data, & delta_variables[i][j], 1);
        model_ptr->addConstr(expr_buffer_1, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][1]);

        model_ptr->addConstr(expr_buffer_2, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][2]);

        data = M;
        expr_buffer_3.addTerms(& data, & const_var, 1);
        data = -M;
        expr_buffer_3.addTerms(& data, & delta_variables[i][j], 1);
        model_ptr->addConstr(expr_buffer_3, GRB_GREATER_EQUAL, 0.0, constraint_names[i][j][3]);

        j++;
      }
      i++;
    }

    // Putting the constant of '1'
    expr_buffer_0 = expr_zero;
    data = 1;
    expr_buffer_0.addTerms(& data, & const_var, 1);
    model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0, "constant_constraint");

    // Putting in the constraints imposed by the output neuron limits_found
    // if(direction == 1)
    // {
    //   expr_buffer_0 = expr_zero;
    //   data = 1;
    //   expr_buffer_0.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
    //   data = -(limit_found + sherlock_parameters.MILP_tolerance);
    //   expr_buffer_0.addTerms(& data, & const_var, 1);
    //   model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, "output_constraint");
    // }
    // else if(direction == (-1))
    // {
    //   expr_buffer_0 = expr_zero;
    //   data = -1;
    //   expr_buffer_0.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
    //   data = (limit_found - sherlock_parameters.MILP_tolerance);
    //   expr_buffer_0.addTerms(& data, & const_var, 1);
    //   model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, "output_constraint");
    // }
    // else
    // {
    //   cout << "Unkown direction input in check_limits() " << endl;
    //   cout << "Exiting .. " << endl;
    //   exit(0);
    // }

    // Putting the epsilon bounds
    i = 0;
    while(i < (no_of_hidden_layers + 2))
    {
      var_vector.clear();
      j = 0;
      while(j < total_network_configuration[i])
      {
        expr_buffer_0 = expr_zero;
        data = 1;
        expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
        model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0,"epsilon_lower_con" );
        j++;
      }
      i++;
    }

  // Epsilon sum constraint

  expr_buffer_0 = expr_zero;
  i = 0;
  while(i < (no_of_hidden_layers + 2))
  {
    var_vector.clear();
    j = 0;
    while(j < total_network_configuration[i])
    {
      data = 1;
      expr_buffer_0.addTerms(& data, & epsilon_variables[i][j], 1);
      j++;

    }
    i++;
  }
  model_ptr->addConstr(expr_buffer_0, GRB_LESS_EQUAL, sherlock_parameters.MILP_e_tolerance,"epsilon_sum_con" );

  // The delta output bound
    // expr_buffer_0 = expr_zero;
    // data = 1;
    // expr_buffer_0.addTerms(& data, & delta_variables[no_of_hidden_layers+1][0], 1);
    // model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0,"delta_output_con" );


    GRBLinExpr objective_expr;

    objective_expr = 0;
    // i = 1;
    // while(i < (no_of_hidden_layers + 2))
    // {
    //   j = 0;
    //   while(j < total_network_configuration[i])
    //   {
    //     data = 1;
    //     objective_expr.addTerms( & data, & epsilon_variables[i][j], 1);
    //     j++;
    //   }
    //   i++;
    // }

    // objective_expr = 0;


    if(direction == 1)
    {
      data = -1;
      objective_expr.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
    }
    else if(direction == (-1))
    {
      data = 1;
      objective_expr.addTerms(& data, & neuron_variables[no_of_hidden_layers+1][0], 1);
    }
    else
    {
      cout << "Unkown direction input in check_limits() " << endl;
      cout << "Exiting .. " << endl;
      exit(0);
    }


    model_ptr->setObjective(objective_expr, GRB_MINIMIZE);
    model_ptr->optimize();

  // model_ptr->update();
  // string s = "check_file_find_counter_ex.lp";
  // model_ptr->write(s);


  if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
  {

    counter_example.clear();
    i = 0;
    while(i < no_of_inputs)
    {
      counter_example.push_back((neuron_variables[0][i]).get(GRB_DoubleAttr_X));
      i++;
    }
    datatype limit;
    limit = neuron_variables[no_of_hidden_layers+1][0].get(GRB_DoubleAttr_X);
    cout << "Node count = " << model_ptr->get(GRB_DoubleAttr_NodeCount) << endl;

    delete model_ptr;
    delete env_ptr;
    return limit;
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
  {
    cout << "Node count = " << model_ptr->get(GRB_DoubleAttr_NodeCount) << endl;

    delete model_ptr;
    delete env_ptr;
    counter_example.clear();
    cout << "MILP infeasible" << endl;
    return 0;
  }
  else
  {
    cout << "Unknown error in gurobi implementation ... " << endl;
    cout << "Status code = " << model_ptr->get(GRB_IntAttr_Status)  << endl;
    return 0;
  }

  delete model_ptr;
  delete env_ptr;



  return 0;
}

int count_digits(int n)
{
  int buff = 1;
  int i = 0;
  while( buff )
  {
    i++;
    buff = n / pow(10,i);
  }
  return i;
}

void produce_string_for_variable_index(string & return_name,
                                      unsigned int layer_no,
                                      unsigned int var_no,
                                      unsigned int variable_type)
{

  string name;
  if(variable_type == 1)
  {
    name = node;
  }
  else if(variable_type == 2)
  {
    name = epsilon;
  }
  else if(variable_type == 3)
  {
    name = delta;
  }
  else if(variable_type == 4)
  {
    name = con;
  }
  else
  {
    cout << "Unknown variable type in produce_string_for_variable_index() .. " << endl;
    cout << "Exiting... " << endl;
    exit(0);
  }

    unsigned int i,j,k, z;
    i = 0;
    while(i < sherlock_parameters.max_digits_in_var_names)
    {
      name += "0" ;
      i++;
    }
    k = 1;
    while(k <= count_digits(layer_no))
    {
      z = ( layer_no % (int)(pow(10,k)) ) / (pow (10, k-1));
      name[2 + sherlock_parameters.max_digits_in_var_names - 1 - k + 1] = '0' + z;
      k++;
    }

    name += "_";
    i = 0;
    while(i < sherlock_parameters.max_digits_in_var_names)
    {
      name += "0" ;
      i++;
    }

    k = 1;
    while(k <= count_digits(var_no))
    {
      z = ( var_no % (int)(pow(10,k)) ) / (pow (10, k-1));
      name[2 + 2 * sherlock_parameters.max_digits_in_var_names +1 - 1 - k + 1] = '0' + z;
      k++;
    }

    return_name = name;
}

void erase_line()
{
  printf("\r");
  printf("\033[A");
  printf("\033[2K");
}

int find_if_constraint_matters(
  vector< vector< datatype > > constraint_set,
  vector< datatype > constraint,
  vector< vector< datatype > > target_region,
  datatype& degree_of_matter
)
{

  datatype region_amount_with_constraint;
  datatype region_amount_without_constraint;
  datatype region_amount_inside_target;

  unsigned int i, j , k, no_of_inputs, no_of_constraints;
  no_of_constraints = constraint_set.size();
  if(!no_of_constraints)
  {
    cout << "No constraints received in find_if_constraint_matters()" << endl;
    cout << "Exiting.. " << endl;
    exit(0);
  }
  no_of_inputs = (constraint_set[0]).size() - 1;
  vector< int > direction_vector(no_of_inputs, 0);
  vector< datatype > size_vector(no_of_inputs, 0);
  datatype temp_size, buffer_1, buffer_2;

  i = 0;
  while(i < no_of_inputs)
  {
    fill(direction_vector.begin(), direction_vector.end(), 0);
    temp_size = 0;
    // In the increasing direction
    direction_vector[i] = 1;
    find_size_of_enclosed_region_in_direction(constraint_set,
      direction_vector, buffer_1);

    // In the decreasing direction
    direction_vector[i] = -1;
    find_size_of_enclosed_region_in_direction(constraint_set,
      direction_vector, buffer_2);


    temp_size = buffer_1 - buffer_2;
    size_vector[i] = temp_size;

    i++;
  }


  region_amount_without_constraint = sum_vector(size_vector);

  find_size_inside_target(constraint_set,
      target_region, region_amount_inside_target);


  constraint_set.push_back(constraint);

  i = 0;
  while(i < no_of_inputs)
  {
    fill(direction_vector.begin(), direction_vector.end(), 0);
    temp_size = 0;

    // In the increasing direction
    direction_vector[i] = 1;
    find_size_of_enclosed_region_in_direction(constraint_set,
      direction_vector, buffer_1);

    // In the decreasing direction
    direction_vector[i] = -1;
    find_size_of_enclosed_region_in_direction(constraint_set,
      direction_vector, buffer_2);

    temp_size = buffer_1 - buffer_2;
    size_vector[i] = temp_size;

    i++;
  }


  region_amount_with_constraint = sum_vector(size_vector);


  degree_of_matter = 0;
  if(region_amount_with_constraint > sherlock_parameters.num_tolerance)
  {
    // degree_of_matter =
    // (region_amount_without_constraint - region_amount_with_constraint)
    // /
    // (region_amount_without_constraint);
    degree_of_matter =
    (region_amount_inside_target)
    /
    (region_amount_inside_target + region_amount_with_constraint);
    return 1;
  }
  else
  {
    return 0;
  }

}

void find_size_of_enclosed_region_in_direction(
  vector< vector< datatype > > constraint_set,
  vector< int > direction_vector,
  datatype& region_amount
)
{
  unsigned int i, j , k, no_of_inputs;
  unsigned int no_of_constraints;
  datatype data;

  GRBLinExpr expr_one(1.0);
  GRBLinExpr expr_zero(0.0);
  GRBLinExpr expr_buffer_0(0.0);

  GRBEnv * env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  GRBModel * model_ptr = new GRBModel(*env_ptr);

  string const_name = "one";
  GRBVar const_one = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, const_name);

  // Putting the constant of '1'
  expr_buffer_0 = expr_zero;
  data = 1;
  expr_buffer_0.addTerms(& data, & const_one, 1);
  model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0, "constant_1_set");


  no_of_constraints = constraint_set.size();
  if(!no_of_constraints)
  {
    cout << "No constraints received in find_if_constraint_matters()"<< endl;
    cout << "exiting... " << endl;
    exit(0);
  }
  no_of_inputs = ((constraint_set[0]).size() - 1);

  if(direction_vector.size() != no_of_inputs)
  {
    cout << "Direction vector size not equal to the no of inputs expected in " <<
    " find_size_of_enclosed_region_in_direction() " << endl;
    cout << "No of inputs = " << no_of_inputs << endl;
    cout << "Direction vector size = " << direction_vector.size() << endl;
    cout << "Exiting ... "<< endl;
    exit(0);
  }

  string name = "anything";
  string var_name = "var_name";
  string constr_name = "constraint_name";

  // Creating the variables for the equations
  vector< GRBVar > basic_variables;
  GRBVar var;

  i = 0;
  while(i < no_of_inputs)
  {
    var = model_ptr->addVar(-GRB_INFINITY,
                             GRB_INFINITY,
                             0.0,
                             GRB_CONTINUOUS,
                             var_name);
    basic_variables.push_back(var);
    i++;
  }


  i = 0;
  while(i < no_of_constraints)
  {
    expr_buffer_0 = expr_zero;
    j = 0;
    while(j < no_of_inputs)
    {
      data = constraint_set[i][j];
      expr_buffer_0.addTerms(& data, & basic_variables[j], 1);
      j++;
    }
    data = constraint_set[i][j];
    expr_buffer_0.addTerms(& data, & const_one, 1);

    model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constr_name);
    i++;
  }

  int direction = 0;
  GRBLinExpr objective_expr;
  objective_expr = 0;
  i = 0;
  while(i < no_of_inputs)
  {
    if(direction_vector[i] == 1)
    {
      direction = 1;
      data = 1;
      objective_expr.addTerms(& data, & basic_variables[i], 1);
    }
    else if(direction_vector[i] == (-1))
    {
      direction = -1;
      data = -1;
      objective_expr.addTerms(& data, & basic_variables[i], 1);
    }
    i++;
  }

  model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
  model_ptr->optimize();

  // model_ptr->update();
  // string s = "check_file_find_counter_ex.lp";
  // model_ptr->write(s);

  if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
  {
    if(direction == 1)
    {
      region_amount = model_ptr->get(GRB_DoubleAttr_ObjVal);
    }
    else if(direction == (-1))
    {
      region_amount = model_ptr->get(GRB_DoubleAttr_ObjVal);
      region_amount = -region_amount;
    }
    delete model_ptr;
    delete env_ptr;
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
  {
    region_amount = 0;
    delete model_ptr;
    delete env_ptr;
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
  {
    region_amount = 0;
    delete model_ptr;
    delete env_ptr;
  }
  else
  {
    cout << "Unknown error in gurobi implementation ... " << endl;
    cout << "Status code =  " << model_ptr->get(GRB_IntAttr_Status)  << endl;
    delete model_ptr;
    delete env_ptr;
    cout << "Exiting.. " << endl;
    exit(0);
  }



}
int run_optimization(
  vector< vector< datatype > > constraints,
  vector< datatype > objective,
  datatype obj_bias,
  datatype& maximum,
  vector< datatype >& max_point,
  datatype& minimum,
  vector< datatype >& min_point
)
{
  unsigned int no_of_constraints, no_of_inputs,  i , j, k;
  no_of_constraints = constraints.size();
  if(!no_of_constraints)
  {
    cout << "No constraint received in run_optimization()" << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }
  no_of_inputs = (constraints[0]).size() - 1;
  if(no_of_inputs != objective.size())
  {
    cout << "No of elements in objective does not match the " <<
    " the no of inputs in run_optimization() " << endl;
    cout << "Exiting .. " << endl;
    exit(0);
  }

  optimize(constraints, objective, obj_bias, 1, maximum, max_point);
  optimize(constraints, objective, obj_bias, -1, minimum, min_point);
  return 1;
}

void optimize(
  vector< vector< datatype > > constraint_set,
  vector< datatype > objective,
  datatype obj_bias,
  int direction,
  datatype& extrema,
  vector< datatype>& extrema_point
)
{
  unsigned int i, j , k, no_of_inputs;
  unsigned int no_of_constraints;
  datatype data;

  GRBLinExpr expr_one(1.0);
  GRBLinExpr expr_zero(0.0);
  GRBLinExpr expr_buffer_0(0.0);

  GRBEnv * env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  GRBModel * model_ptr = new GRBModel(*env_ptr);

  string const_name = "one";
  GRBVar const_one = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, const_name);

  // Putting the constant of '1'
  expr_buffer_0 = expr_zero;
  data = 1;
  expr_buffer_0.addTerms(& data, & const_one, 1);
  model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0, "constant_1_set");


  no_of_constraints = constraint_set.size();
  if(!no_of_constraints)
  {
    cout << "No constraints received in find_if_constraint_matters()"<< endl;
    cout << "exiting... " << endl;
    exit(0);
  }
  no_of_inputs = ((constraint_set[0]).size() - 1);

  if(!((direction == 1) || (direction == (-1))))
  {
    cout << "Direction received cannot be worked upon in optimization()" << endl;
    cout << "Exiting.. " << endl;
    exit(0);
  }

  string name = "anything";
  string var_name = "var_name";
  string constr_name = "constraint_name";

  // Creating the variables for the equations
  vector< GRBVar > basic_variables;
  GRBVar var;

  i = 0;
  while(i < no_of_inputs)
  {
    var = model_ptr->addVar(-GRB_INFINITY,
                             GRB_INFINITY,
                             0.0,
                             GRB_CONTINUOUS,
                             var_name);
    basic_variables.push_back(var);
    i++;
  }


  i = 0;
  while(i < no_of_constraints)
  {
    expr_buffer_0 = expr_zero;
    j = 0;
    while(j < no_of_inputs)
    {
      data = constraint_set[i][j];
      expr_buffer_0.addTerms(& data, & basic_variables[j], 1);
      j++;
    }
    data = constraint_set[i][j];
    expr_buffer_0.addTerms(& data, & const_one, 1);

    model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constr_name);
    i++;
  }

  GRBLinExpr objective_expr;
  objective_expr = 0;

  i = 0;
  while(i < no_of_inputs)
  {
    data = objective[i];
    objective_expr.addTerms(& data, & basic_variables[i], 1);
    i++;
  }
  data = obj_bias;
  objective_expr.addTerms(& data, & const_one, 1);

  if(direction == 1)
  {
    model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
    model_ptr->optimize();
  }
  else if(direction == (-1))
  {
    model_ptr->setObjective(objective_expr, GRB_MINIMIZE);
    model_ptr->optimize();
  }

  // model_ptr->update();
  // string s = "check_file_find_counter_ex.lp";
  // model_ptr->write(s);

  if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
  {
    extrema = model_ptr->get(GRB_DoubleAttr_ObjVal);
    extrema_point.clear();
    i = 0;
    while(i < no_of_inputs)
    {
      extrema_point.push_back( (basic_variables[i]).get(GRB_DoubleAttr_X) );
      i++;
    }
    delete model_ptr;
    delete env_ptr;
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
  {
    delete model_ptr;
    delete env_ptr;
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
  {
    delete model_ptr;
    delete env_ptr;
  }
  else
  {
    cout << "Unknown error in gurobi implementation ... " << endl;
    cout << "Status code = " << model_ptr->get(GRB_IntAttr_Status)  << endl;
    delete model_ptr;
    delete env_ptr;
    cout << "Exiting.. " << endl;
    exit(0);
  }



}


int find_whether_overlap(
  vector< vector< datatype > > constraint_set_1,
  vector< vector< datatype > > constraint_set_2
)
{
  vector< vector< datatype > > constraint_set;
  unsigned int i, j , k, no_of_inputs;
  unsigned int no_of_constraints;
  datatype data;

  constraint_set = constraint_set_1;
  i = 0;
  while(i < constraint_set_2.size())
  {
    constraint_set.push_back(constraint_set_2[i]);
    i++;
  }


  GRBLinExpr expr_one(1.0);
  GRBLinExpr expr_zero(0.0);
  GRBLinExpr expr_buffer_0(0.0);

  GRBEnv * env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  GRBModel * model_ptr = new GRBModel(*env_ptr);

  string const_name = "one";
  GRBVar const_one = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, const_name);

  // Putting the constant of '1'
  expr_buffer_0 = expr_zero;
  data = 1;
  expr_buffer_0.addTerms(& data, & const_one, 1);
  model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0, "constant_1_set");


  no_of_constraints = constraint_set.size();
  if(!no_of_constraints)
  {
    cout << "No constraints received in find_if_constraint_matters()"<< endl;
    cout << "exiting... " << endl;
    exit(0);
  }
  no_of_inputs = ((constraint_set[0]).size() - 1);
  vector< int > direction_vector(no_of_inputs,0);
  direction_vector[0] = 1;
  if(direction_vector.size() != no_of_inputs)
  {
    cout << "Direction vector size not equal to the no of inputs expected in " <<
    " find_size_of_enclosed_region_in_direction() " << endl;
    cout << "No of inputs = " << no_of_inputs << endl;
    cout << "Direction vector size = " << direction_vector.size() << endl;
    cout << "Exiting ... "<< endl;
    exit(0);
  }

  string name = "anything";
  string var_name = "var_name";
  string constr_name = "constraint_name";

  // Creating the variables for the equations
  vector< GRBVar > basic_variables;
  GRBVar var;

  i = 0;
  while(i < no_of_inputs)
  {
    var = model_ptr->addVar(-GRB_INFINITY,
                             GRB_INFINITY,
                             0.0,
                             GRB_CONTINUOUS,
                             var_name);
    basic_variables.push_back(var);
    i++;
  }


  i = 0;
  while(i < no_of_constraints)
  {
    expr_buffer_0 = expr_zero;
    j = 0;
    while(j < no_of_inputs)
    {
      data = constraint_set[i][j];
      expr_buffer_0.addTerms(& data, & basic_variables[j], 1);
      j++;
    }
    data = constraint_set[i][j];
    expr_buffer_0.addTerms(& data, & const_one, 1);

    model_ptr->addConstr(expr_buffer_0, GRB_GREATER_EQUAL, 0.0, constr_name);
    i++;
  }

  int direction = 0;
  GRBLinExpr objective_expr;
  objective_expr = 0;
  i = 0;
  while(i < no_of_inputs)
  {
    if(direction_vector[i] == 1)
    {
      direction = 1;
      data = 1;
      objective_expr.addTerms(& data, & basic_variables[i], 1);
    }
    else if(direction_vector[i] == (-1))
    {
      direction = -1;
      data = -1;
      objective_expr.addTerms(& data, & basic_variables[i], 1);
    }
    i++;
  }

  model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
  model_ptr->optimize();

  // model_ptr->update();
  // string s = "check_file_find_counter_ex.lp";
  // model_ptr->write(s);

  if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
  {
    delete model_ptr;
    delete env_ptr;
    return 1;
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
  {
    delete model_ptr;
    delete env_ptr;
    return 0;
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
  {
    delete model_ptr;
    delete env_ptr;
    return 0;
  }
  else
  {
    cout << "Unknown error in gurobi implementation ... " << endl;
    cout << "Status code = " << model_ptr->get(GRB_IntAttr_Status)  << endl;
    delete model_ptr;
    delete env_ptr;
    cout << "Exiting.. " << endl;
    exit(0);
  }

  return 0;
}

int find_size_inside_target(
  vector< vector< datatype > > constraint_set,
  vector< vector< datatype > > target_region,
  datatype& size
)
{
  unsigned int i,j,k, no_of_inputs;
  if(constraint_set.empty() || target_region.empty())
  {
    cout <<" No constraints or target received in find_size_inside_target " << endl;
    cout << " Exiting... " << endl;
    exit(0);
  }

  no_of_inputs = (constraint_set[0]).size() - 1;

  k = target_region.size();
  i = 0;
  while(i < k)
  {
    constraint_set.push_back(target_region[i]);
    i++;
  }

  vector< int > direction_vector(no_of_inputs, 0);

  datatype buffer_1, buffer_2, temp_size;
  vector< datatype > size_vector(no_of_inputs);

  i = 0;
  while(i < no_of_inputs)
  {
    fill(direction_vector.begin(), direction_vector.end(), 0);
    temp_size = 0;

    // In the increasing direction
    direction_vector[i] = 1;
    find_size_of_enclosed_region_in_direction(constraint_set,
      direction_vector, buffer_1);

    // In the decreasing direction
    direction_vector[i] = -1;
    find_size_of_enclosed_region_in_direction(constraint_set,
      direction_vector, buffer_2);

    temp_size = buffer_1 - buffer_2;
    size_vector[i] = temp_size;

    i++;
  }

  size = sum_vector(size_vector);

  return size;
}

int find_size(
  vector< vector< datatype > > constraint_set,
  datatype& size
)
{
  unsigned int i,j,k, no_of_inputs;
  if(constraint_set.empty())
  {
    cout <<" No constraints or target received in find_size_inside_target " << endl;
    cout << " Exiting... " << endl;
    exit(0);
  }

  no_of_inputs = (constraint_set[0]).size() - 1;


  vector< int > direction_vector(no_of_inputs, 0);

  datatype buffer_1, buffer_2, temp_size;
  vector< datatype > size_vector(no_of_inputs);

  i = 0;
  while(i < no_of_inputs)
  {
    fill(direction_vector.begin(), direction_vector.end(), 0);
    temp_size = 0;

    // In the increasing direction
    direction_vector[i] = 1;
    find_size_of_enclosed_region_in_direction(constraint_set,
      direction_vector, buffer_1);

    // In the decreasing direction
    direction_vector[i] = -1;
    find_size_of_enclosed_region_in_direction(constraint_set,
      direction_vector, buffer_2);

    temp_size = buffer_1 - buffer_2;
    size_vector[i] = temp_size;

    i++;
  }

  size = sum_vector(size_vector);

  return size;
}

void find_the_non_overlap(
  vector< vector< datatype > > main_region,
  vector< vector< datatype > > region_to_subtract,
  vector< vector< datatype > >& non_overlap
)
{
  datatype original_size, size;

  unsigned int no_of_constraints_to_check;
  no_of_constraints_to_check = region_to_subtract.size();

  vector< vector< datatype > > modified_structure;
  vector< datatype > constraint;
  find_size(main_region, original_size);

  unsigned int i, j , k;
  i = 0;
  while(i < no_of_constraints_to_check)
  {
    modified_structure.clear();
    modified_structure = main_region;
    modified_structure.push_back(region_to_subtract[i]);

    find_size(modified_structure, size);

    if( original_size - size > 1e-3)
    {
      cout << "Orignal size = " << original_size << endl;
      cout << "size = " << size << endl;


      constraint = region_to_subtract[i];
      cout << constraint[0] << " "<< constraint[1] << " "<< constraint[2] << " "<< constraint[3] << " " << endl;
      reverse_a_constraint(constraint);
      main_region.push_back(constraint);
      non_overlap = main_region;
      break;
    }
    i++;
  }
}
