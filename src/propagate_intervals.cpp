#include <cstdlib>
#include "propagate_intervals.h"

network_handler :: network_handler(const char* name)
{
  strncpy(this -> name_of_file, name, 255);
  ifstream file;
  file.open(name);

  // The expcted input format goes something like this,
  // no_of_inputs
  // no_of_outputs
  // no_of_layers
  // configuration no_1 :  no_of_neurons in layer 1
  // configuration no_2 :  no_of_neurons in layer 2
  // configuration no_3 :  no_of_neurons in layer 3
  // configuration no_4 :  no_of_neurons in layer 4
  // ....
  // weight into the first neuron from the 1st input
  // weight into the first neuron from the 2nd input
  // ....
  // bias for the first neuron
  // weight into the 2nd neuron from the 1st input
  // ....
  // ....


  unsigned int i, j , k, size_1, size_2, buffer_integer ;
  file >> no_of_inputs;
  file >> no_of_outputs;
  file >> no_of_hidden_layers;
  network_configuration.clear();
  i = 0;
  while(i < no_of_hidden_layers)
  {
    file >> buffer_integer;
    network_configuration.push_back(buffer_integer);
    i++;
  }

  datatype data;
  // reserving space for the number of hidden layers + 1

  vector<vector< datatype > > input_buffer_mat(network_configuration[0],vector< datatype > (no_of_inputs,0));
  vector< datatype > input_buffer_vec(network_configuration[0]);

  actual_weights.reserve(no_of_hidden_layers + 1);
  actual_biases.reserve(no_of_hidden_layers + 1);

  // reading the input matrix
  for(i = 0 ; i < network_configuration[0] ; i++)
  {
    for( j = 0; j < no_of_inputs; j++)
    {
      file >> data;
      input_buffer_mat[i][j] = data;
    }

    file >> data;
    input_buffer_vec[i] = data;
  }

  actual_weights.push_back(input_buffer_mat);
  actual_biases.push_back(input_buffer_vec);

  vector< datatype > buffer_vec;
  vector<vector< datatype > > buffer_weight_mat;
  vector< datatype > buffer_bias_vec;

  // Reading the inner matrices

  if(no_of_hidden_layers > 1)
  {
    for(i = 1; i < no_of_hidden_layers; i++)
    {
      buffer_weight_mat.clear();
      buffer_bias_vec.clear();

      for(j = 0 ; j < network_configuration[i]; j++)
      {
        buffer_vec.clear();
        for(k = 0; k < network_configuration[i-1] ; k++)
        {
          file >> data;
          buffer_vec.push_back(data);
        }
        file >> data;
        buffer_bias_vec.push_back(data);
        buffer_weight_mat.push_back(buffer_vec);
      }

      actual_weights.push_back(buffer_weight_mat);
      actual_biases.push_back(buffer_bias_vec);
    }

  }

  vector<vector< datatype > > outer_buffer_mat(no_of_outputs,
    vector< datatype > (network_configuration[no_of_hidden_layers-1],0));

  vector< datatype > outer_buffer_vec(no_of_outputs);


  // Reading the  output matrix
  for(i  = 0; i < no_of_outputs ; i++)
  {
    for(j = 0; j < network_configuration[no_of_hidden_layers - 1]; j++)
    {
      file >> data;
      outer_buffer_mat[i][j] = data;
    }
    file >> data;
    outer_buffer_vec[i] = data;
  }

  actual_weights.push_back(outer_buffer_mat);
  actual_biases.push_back(outer_buffer_vec);


  data = -100;
  file >> data;
  if(data != (-100))
  {
    if(sherlock_parameters.verbosity){cout << "Network input file probably has an error ! " << endl;}
  }

  file.close();


}

network_handler :: network_handler( vector< vector< vector< datatype > > > weights,
                 vector< vector < datatype > > biases
                )
{
  actual_weights = weights;
  actual_biases = biases;

  no_of_hidden_layers = weights.size()- 1;
  no_of_outputs = (weights[no_of_hidden_layers]).size();
  no_of_inputs = (weights[0][0]).size();
}
void network_handler :: update_information( vector< vector< vector< datatype > > > weights,
                         vector< vector < datatype > > biases)
{
  actual_weights = weights;
  actual_biases = biases;

  no_of_hidden_layers = weights.size()- 1;
  no_of_outputs = (weights[no_of_hidden_layers]).size();
  no_of_inputs = (weights[0][0]).size();
}

void network_handler :: cast_to_single_output_network(
      vector< vector< vector < datatype > > >& weights,
      vector< vector< datatype > >& biases,
      unsigned int output_number)
{
  if((output_number == 0) || (output_number > no_of_outputs))
  {
    cout << "Output index out of range in the function cast_to_single_output_network() .. " << endl;
  }

  if((weights[0][0]).size() != no_of_inputs)
  {
    cout << "Number of inputs don't match in cast_to_single_output_network()" << endl;
    exit(0);
  }

  if((weights[no_of_hidden_layers].size()) != no_of_outputs)
  {
    cout << "Number of outputs don't match in cast_to_single_output_network() .." << endl;
    exit(0);
  }

  vector< vector< vector< datatype > > > return_weights;
  vector< vector< datatype > > return_biases;

  vector< vector< datatype > > buffer_matrix;
  vector< datatype > buffer_bias;

  unsigned int i, j , k;

  i = 0;
  while(i < no_of_hidden_layers)
  {
    return_weights.push_back(weights[i]);
    return_biases.push_back(biases[i]);
    i++;
  }
  buffer_matrix.clear();
  buffer_matrix.push_back(weights[no_of_hidden_layers][output_number-1]);
  buffer_bias.clear();
  buffer_bias.push_back(biases[no_of_hidden_layers][output_number-1]);

  return_weights.push_back(buffer_matrix);
  return_biases.push_back(buffer_bias);

  if(((return_weights.size()) != (no_of_hidden_layers+1)) || ((return_biases.size()) != (no_of_hidden_layers+1)))
  {
    cout << "Number of weight sets in the returned varaibles does not match in cast_to_single_output_network() " << endl;
    exit(0);
  }
  if(((return_weights[no_of_hidden_layers]).size() != 1) || ((return_biases[no_of_hidden_layers]).size() != 1))
  {
    cout << "Output index in the returning variable does not match  in cast_to_single_output_network() " << endl;
    exit(0);
  }

  weights = return_weights;
  biases = return_biases;
}

vector< datatype > network_handler :: return_gradient(
  vector< datatype > point,
  int direction, vector<vector< datatype > > region_constraints,
  datatype & max_val, vector< datatype >& max_point,
  datatype & min_val, vector< datatype >& min_point )
{

  vector< vector< unsigned int > > active_weights;

  // The guys who store the constraints from time to time
  vector< vector < datatype > > constraint_matrix;
  vector< vector< datatype > > positive_constraint_matrix;
  vector< vector< vector< datatype > > > collection_of_negative_constraint_matrices;
  vector< datatype > objective;
  datatype last_bias, max, min;

  // So it's anding within the different stored matrices, and each of the matrices are to be
  // be negated

  positive_constraint_matrix  = region_constraints;

  vector< datatype > return_gradient(no_of_inputs, 0);

  // Some general purpose data units
  unsigned int no_of_active_layers;
  vector< vector< vector< datatype > > > buffer_weights;
  vector< vector< datatype > > buffer_bias;
  unsigned int i , j , k , l, maximum_penetration, counter , size_1, size_2, size_3;
  datatype network_output;

  vector< unsigned int >  active_neurons;
  vector< unsigned int > active_neurons_first_layer;
  // vector< unsigned int > active_neurons_second_layer(no_of_neurons,0);

  vector< vector< datatype > > input_weight_matrix;
  vector< datatype > input_bias_vector;

  vector< vector< datatype > > output_weight_matrix;
  datatype output_bias;

  vector< vector< datatype > > temp_weight_matrix;
  vector< datatype > temp_bias_vector;

  vector< vector< unsigned int > > useless_input;

  vector< datatype > constraint_vec(no_of_inputs+1,0);

  // Copying the Weights and Biases to a different location, since we will be
  // changing those set of weights and biases on the go

  buffer_weights = weights;
  buffer_bias = biases;

  unsigned int mark = 0;

  uint64_t sample_number = 0;
  counter = 0;

    //  Printng the point


    // propagate the point through the network using the original weights and
    // biases and get the output use this point to generate the trace
    // (or the list of active neurons) and assign it to the vector
    // 'active_Weights' , using weights and biases

  network_output = compute_network_output(point,
                                            weights,
                                            biases,
                                            active_weights);


      // Check if the active weights has some active neurons in all the layers.
      // if the flow stops somewhere, then find that out and store it as 'maximum_penetration'
      // and the output in 'network_output'

  maximum_penetration = find_maximum_penetration(active_weights);

  if( maximum_penetration == no_of_hidden_layers + 1)  // That is the input values activates something
                                                       //  in all the layers  of the 'deep' neural network
  {
    no_of_active_layers = 0;

     while(no_of_active_layers < no_of_hidden_layers)
     {
        if(no_of_active_layers == (no_of_hidden_layers-1) )
        {

                 // create the list of active_neurons
                 active_neurons = active_weights[no_of_active_layers];

                 // create the input_weight_matrix, input_bias_vector, output_weight_matrix, output_bias
                 return_shorter_network(buffer_weights,
                                        buffer_bias,
                                        0,
                                        input_weight_matrix,
                                        input_bias_vector,
                                        output_weight_matrix,
                                        output_bias);


                 // call the function return_constraint_vectors_from_weights()
                 constraint_matrix.clear();
                 constraint_matrix = return_constraint_vectors_from_weights(
                                      input_weight_matrix,
                                      input_bias_vector,
                                      active_neurons);

                 // save the constraint received in the list of positive_constraint_matrix
                 append_matrix_to_matrix(positive_constraint_matrix, constraint_matrix);

                 // Call the function 'return_weights_and_bias_from_a_single_layer_one_output_network()'
                 return_weights_and_bias_from_a_single_layer_one_output_network(
                                input_weight_matrix,
                                input_bias_vector,
                                output_weight_matrix,
                                output_bias,
                                active_neurons,
                                1 ,
                                objective,
                                last_bias);

                // extract the constraint in terms of the output as well
                constraint_matrix.clear();
                constraint_matrix = create_constraint_from_weights_and_bias(
                                    objective,
                                    last_bias);

                //  cout << "Objective = " << objective[0] <<"   " << objective[1] << endl;
                //  cout << "Last bias = " << last_bias << endl;

                // saving the final constraint that comes from the output
                append_matrix_to_matrix(positive_constraint_matrix, constraint_matrix);

                if(!sherlock_parameters.skip_LP_jump)
                {
                  // call the linear programming solver to find the range of the output
                  run_optimization(positive_constraint_matrix, objective, last_bias,
                                 max, max_point, min, min_point);
                }
                else
                {
                  max_point = point;
                  min_point = point;
                  max = compute_network_output(point, weights, biases, useless_input);
                  min = max;
                }
                //  remove_the_last_constraint(positive_constraint_matrix);

                max_val = max;
                min_val = min;
                if(direction == 1)
                {
                     return_gradient = objective;
                }
                else if(direction == (-1) )
                {
                     return_gradient = negate_vector(objective);
                }
                else
                {
                     cout << "Wrong direction input in return_gradient()" << endl;
                }



        }
        else
        {

                // create a vector list for all the active neurons  in first layer
                active_neurons_first_layer = active_weights[no_of_active_layers];

                i = 0;
                while( i < (active_weights[no_of_active_layers + 1]).size() /* you are going through all the neurons in the 2nd layer */)
                {

                   // create the input_weight_matrix, input_bias_vector, output_weight_matrix, output_bias,
                   // for the particular neuron in layer 2

                   return_shorter_network(buffer_weights,
                                          buffer_bias,
                                          i,
                                          input_weight_matrix,
                                          input_bias_vector,
                                          output_weight_matrix,
                                          output_bias);


                   // call the function return_constraint_vectors_from_weights()
                   constraint_matrix.clear();
                   constraint_matrix = return_constraint_vectors_from_weights(
                                        input_weight_matrix,
                                        input_bias_vector,
                                        active_neurons_first_layer);

                   // save the constraint received in the list of positive_constraint_matrix
                   append_matrix_to_matrix(positive_constraint_matrix, constraint_matrix);

                   // Call the function 'return_weights_and_bias_from_a_single_layer_one_output_network()'
                   return_weights_and_bias_from_a_single_layer_one_output_network(
                          input_weight_matrix,
                          input_bias_vector,
                          output_weight_matrix,
                          output_bias,
                          active_neurons_first_layer,
                          1 ,
                          objective,
                          last_bias);

                  // save the weights to the temp_weight_matrix
                  temp_weight_matrix.push_back(objective);
                  // save the bias to the temp_Bias_matrix
                  temp_bias_vector.push_back(last_bias);

                  i++;
                }
                 // replace the 1st and 2nd layer of the network in buffer_weights and buffer_bias,
                 // with the first layer as temp_weight_matrix and temp_bias_matrix

                 replace_layers(buffer_weights, buffer_bias, temp_weight_matrix, temp_bias_vector);
                 temp_weight_matrix.clear();
                 temp_bias_vector.clear();
                 // cout << "Buffer biases size  after = " << (buffer_bias[0]).size() << endl;


        }
        no_of_active_layers ++ ;
     }

  }
  else
  {
    fill(return_gradient.begin(),return_gradient.end(), 0 );
  }


  return return_gradient;

}

void network_handler :: do_gradient_search(
  vector< vector< datatype > > input_constraints,
  vector< datatype > sample_point,
  vector< datatype >& return_val,
  vector< datatype >& extrema_point,
  int direction)
{

 vector< datatype > gradient(no_of_inputs);
 vector< vector< datatype > > region_constraints;
 region_constraints = input_constraints;

 unsigned int  i , j ,k , size_1, size_2;
 vector< datatype > max_point;
 vector< datatype > min_point;
 datatype max_val, min_val;
 vector< datatype > old_point;
 vector< datatype > optima_point;
 datatype new_optima_attained, old_optima_attained;
 if(direction == 1)
 {
   new_optima_attained = -1e20;
   old_optima_attained = -1e20;
 }
 else if (direction == -1)
 {
   new_optima_attained = 1e20;
   old_optima_attained = 1e20;
 }

 vector< datatype > current_point(no_of_inputs);
 vector< datatype > next_point(no_of_inputs);

 // The structure of the code here has been written down in comments
 vector< vector< unsigned int > > bogus_input;
 current_point = sample_point;
 old_point = current_point;
 if(sherlock_parameters.verbosity)
 {
   cout << "Doing gradient search ... " << endl;
   cout << "Starting at " << compute_network_output(current_point, weights, biases, bogus_input ) << endl;
 }

 datatype current_rate = sherlock_parameters.gradient_rate * sherlock_parameters.grad_scaling_factor;
 unsigned int switch_count = 0;
 vector< datatype > previous_point;
 unsigned int switch_flag = sherlock_parameters.switch_to_modified_gradient_search;
 datatype change_amount = 100;

 unsigned int gradient_search_time_out = sherlock_parameters.grad_switch_count;
 unsigned int grad_steps = 0 ;

 while(  change_amount > sherlock_parameters.grad_termination_limit )
 {

   gradient = return_gradient(current_point, direction , region_constraints,
      max_val, max_point, min_val, min_point);

  normalize_vector(gradient);

   i = 0;
   while(i < no_of_inputs)
   {
     if(isnan(gradient[i]) || (fabs(gradient[i]) < sherlock_parameters.tool_zero))
     {
       vector< datatype > counter_ex;
       datatype val;
       val = do_MILP_optimization(
         input_constraints, weights, biases, counter_ex, direction);
       extrema_point = counter_ex;
       return_val.clear();
       return_val.push_back(val);
       return_val.push_back(val);
       return;
     }
     i++;
   }

   if(!sherlock_parameters.skip_LP_jump)
   {
     if(direction == 1)
     {
       current_point = max_point;
     }
     else if(direction == (-1))
     {
       current_point = min_point;
     }
     else
     {
       cout << "Unknown direction in do gradient search " << endl;
       cout << "Exiting.. " << endl;
       exit(0);
     }
   }

    if(sherlock_parameters.grad_search_point_verbosity)
    {
      cout << endl << "Current point = " ;
      i = 0;
      while(i < no_of_inputs)
      {
        cout << current_point[i] << " ";
        i++;
      }
      cout << endl;
      cout << "Max val from gradient compute step = " << max_val << endl;
      cout << "Min val from gradient compute step = " << min_val << endl;

    }
    if(direction == 1)
    {
      new_optima_attained = max_val;
      change_amount = new_optima_attained - old_optima_attained;
      if(new_optima_attained > old_optima_attained)
      {
        old_optima_attained = max_val;
        optima_point = max_point;
      }
    }
    else if(direction == -1)
    {
      new_optima_attained = min_val;
      change_amount = old_optima_attained - new_optima_attained;

      if(new_optima_attained < old_optima_attained)
      {
        old_optima_attained = min_val;
        optima_point = min_point;
      }
    }

    if(change_amount < 0)
    {
      if(
          check_inflection_point(old_point, weights, biases, direction, region_constraints)
        )
        {
          // cout << "Breaks out here 1" << endl;
          current_point = old_point;
          break;
        }
    }


    if(switch_flag)
    {
      gradient = scale_vector(gradient, current_rate);
      if(sherlock_parameters.grad_search_point_verbosity)
      {
        cout << "Gradient = ";
        i = 0;
        while(i < no_of_inputs)
        {
          cout << gradient[i] << " ";
          i++;
        }
        cout << endl;
      }
      old_point = current_point;
      if(!propagate_point(current_point, gradient, region_constraints))
      {
        current_rate = sherlock_parameters.gradient_rate;
        switch_flag = 0;
        current_point = old_point;
        change_amount = 1e20;
      }
      switch_count++;
      if(switch_count > sherlock_parameters.grad_switch_count)
      {
        current_rate = sherlock_parameters.gradient_rate;
        switch_flag = 0;
        change_amount = 1e20;
      }
    }
    else
    {
      old_point = current_point;
      gradient = scale_vector(gradient, current_rate);
      if(sherlock_parameters.grad_search_point_verbosity)
      {
        cout << "Gradient = ";
        i = 0;
        while(i < no_of_inputs)
        {
          cout << gradient[i] << " ";
          i++;
        }
        cout << endl;
      }
      if(!propagate_point(current_point, gradient, region_constraints))
      {
        // cout << "Breaks out here 2" << endl;
        break;
      }
    }

    if(grad_steps > gradient_search_time_out)
    {
      // cout << "Breaks out here 3" << endl;
      break;
    }

    grad_steps++;

 }


 if(sherlock_parameters.verbosity)
 {
   cout << "Gradient search ends ... " << endl;
   cout << "Ending at " << compute_network_output(current_point, weights, biases, bogus_input ) << endl;
 }

 sample_point = current_point;



 if(direction == 1)
 {
   extrema_point = max_point ;
   return_val[0] = compute_network_output(current_point, weights, biases, bogus_input ) + sherlock_parameters.num_shift;
   return_val[1] = compute_network_output(current_point, weights, biases, bogus_input ) + sherlock_parameters.num_shift;
 }
 else if(direction == (-1))
 {
   extrema_point = min_point;
   return_val[0] = compute_network_output(current_point, weights, biases, bogus_input ) - sherlock_parameters.num_shift;
   return_val[1] = compute_network_output(current_point, weights, biases, bogus_input ) - sherlock_parameters.num_shift;
 }
 else
 {
   cout << "Wrong direction input in do_gradient_search() " << endl;
   cout << "Exiting ... " << endl;
   exit(0);
 }

}

void network_handler :: return_interval_output(
  vector< vector< datatype > > input_constraints,
  vector< datatype>& return_val,
  unsigned int output_number
)
{

  weights = actual_weights;
  biases  = actual_biases;
  cast_to_single_output_network(weights, biases, output_number);

  vector< vector< datatype > > region_constraints;
  region_constraints = input_constraints;

  vector< datatype > sample_point(no_of_inputs);
  vector< datatype > counter_example(no_of_inputs);
  find_random_sample(region_constraints, sample_point);

  unsigned int i , j , k, found_limit;
  vector< datatype > current_point(no_of_inputs,0);
  vector< datatype > interval(no_of_inputs);

  datatype max, min, current_max, current_min;
  vector< datatype > extrema_point;

  clock_t begin, end;

  found_limit = 0;
  current_point = sample_point;

           if(sherlock_parameters.verbosity)
           {
   cout << "Beginning point for finding the minima : " << endl;
   cout << "[ " ;
   i = 0;
   while( i < no_of_inputs)
   {
     cout << "  "<< current_point[i] ;
     i++;
   }
   cout << "] "  << endl;
  }


  // Find the minima
  found_limit = 0;
  current_point = sample_point;
  while(!found_limit)
  {

    if(sherlock_parameters.time_verbosity)
    {
      begin = clock();
    }

    do_gradient_search(region_constraints, current_point, interval, extrema_point, -1 );

    if(sherlock_parameters.time_verbosity)
    {
      end = clock();
      printf("time cost for gradient search: %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
    }

    current_min = interval[0];
    if(current_min < sherlock_parameters.tool_zero)
    {
      current_min = (datatype)0;
    }

    if(sherlock_parameters.time_verbosity)
    {
      begin = clock();
    }

    if(check_limits(weights, biases,
      current_min, extrema_point, region_constraints, -1, counter_example))
    {
     found_limit = 1;
   }
    else
    {
     found_limit = 0;
     current_point = counter_example;
   }

    if(sherlock_parameters.time_verbosity)
    {
      end = clock();
      printf("time cost for check_limits : %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
    }

  }

  min = current_min;

          if(sherlock_parameters.verbosity)
          {
   cout << "Min found  = "  << min << endl;
  }

  found_limit = 0;
  current_point = sample_point;

        if(sherlock_parameters.verbosity)
        {
   cout << "Beginning point for finding the maxima: " << endl;
   cout << "[ " ;
   i = 0;
   while( i < no_of_inputs)
   {
     cout << "  "<< current_point[i] ;
     i++;
   }
   cout << "] "  << endl;
  }

  // Find the maxima
  while(!found_limit)
  {
        if(sherlock_parameters.time_verbosity)
        {
      begin = clock();
    }

   do_gradient_search(region_constraints, current_point, interval, extrema_point, 1 );

        if(sherlock_parameters.time_verbosity)
        {
     end = clock();
     printf("time cost for gradient search: %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
   }

   current_max = interval[1];

        if(sherlock_parameters.verbosity)
        {
     cout << "Checking limits start ... " << endl ;
   }
        if(sherlock_parameters.time_verbosity)
        {
     begin = clock();
   }

   if(check_limits(weights, biases,
     current_max, extrema_point, region_constraints, 1, counter_example))
   {
     found_limit = 1;
   }
   else
   {
     found_limit = 0;
     current_point = counter_example;
   }

        if(sherlock_parameters.time_verbosity)
        {
     end = clock();
     printf("time cost for check_limits : %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
   }
        if(sherlock_parameters.verbosity)
        {
     cout << "Checking limits ends ... " << endl ;
   }

  }

  max = current_max;
  if(sherlock_parameters.verbosity)
  {
   cout << "Max found  "<< max << endl;
  }


 return_val.clear();

 return_val.push_back(min);
 return_val.push_back(max);

}

void network_handler :: return_network_information(
  vector < vector < vector < datatype > > >& buffer_for_weights,
  vector < vector < datatype > >& buffer_for_biases
)
{
  buffer_for_weights.clear();
  buffer_for_biases.clear();
  buffer_for_weights = actual_weights;
  buffer_for_biases = actual_biases;
}

void network_handler :: return_GUROBI_handle_of_network(
                                      GRBModel * milp_model,
                                      GRBEnv * milp_env,
                                      vector< GRBVar >& input_variables,
                                      GRBVar & output_variables
)
{
  vector< vector< vector< datatype > > > weights;
  vector< vector< datatype > > biases;

  return_network_information(weights, biases);

  do_network_encoding(weights, biases, milp_model, milp_env, input_variables, output_variables);

  if((milp_model == NULL) || (milp_env == NULL))
  {
    cout << "No network was encoded into GRB model pointer " << endl;
    cout << "Exiting.. " << endl;
    exit(0);
  }
}


void merge_networks(datatype network_offset,
                    datatype scaling_factor,
                    char * output_file,
                    char * main_network,
                    char * sub_network_1,
                    char * sub_network_2 ,
                    char * sub_network_3 ,
                    char * sub_network_4 )
{
  if(!output_file)
  {
    if(sherlock_parameters.verbosity)
    {
      cout << "No output file name received while merging, naming it as : output_network " << endl;
    }
    char output_file[] = "output_network";
  }
  if( (!main_network) || (!sub_network_1) )
  {
    cout << "Either No main network received while merging or no sub network received, quitting... " << endl;
     exit(0);
   }
  unsigned int no_of_inputs, no_of_inputs_to_be_patched, i , j , k, no_of_neurons, sum;
  unsigned int max_number_of_weight_sets, no_of_neurons_in_main_net;

  no_of_inputs_to_be_patched = 0;


  vector< vector< vector < datatype > > > return_weights;
  vector< vector < datatype > > return_biases;

  vector< vector< vector < datatype > > > main_network_weights;
  vector< vector < datatype > > main_network_biases;
  network_handler main_network_handler(main_network);
  main_network_handler.return_network_information(main_network_weights, main_network_biases);
  no_of_neurons_in_main_net = (main_network_weights[0]).size();

  vector< vector< vector < datatype > > > sub_network_1_weights;
  vector< vector < datatype > > sub_network_1_biases;
  network_handler sub_network_1_handler(sub_network_1);
  sub_network_1_handler.return_network_information(sub_network_1_weights, sub_network_1_biases);
  max_number_of_weight_sets = sub_network_1_weights.size();
  no_of_inputs_to_be_patched++;



  vector< vector< vector < datatype > > > sub_network_2_weights;
  vector< vector < datatype > > sub_network_2_biases;
  vector< vector< vector < datatype > > > sub_network_3_weights;
  vector< vector < datatype > > sub_network_3_biases;
  vector< vector< vector < datatype > > > sub_network_4_weights;
  vector< vector < datatype > > sub_network_4_biases;


  if(sub_network_2)
  {
    network_handler sub_network_2_handler(sub_network_2);
    sub_network_2_handler.return_network_information(sub_network_2_weights, sub_network_2_biases);
    if(max_number_of_weight_sets < sub_network_2_weights.size())
    {
      max_number_of_weight_sets = sub_network_2_weights.size();
    }
    no_of_inputs_to_be_patched++;
  }

  if(sub_network_3)
  {
    network_handler sub_network_3_handler(sub_network_3);
    sub_network_3_handler.return_network_information(sub_network_3_weights, sub_network_3_biases);
    if(max_number_of_weight_sets < sub_network_3_weights.size())
    {
      max_number_of_weight_sets = sub_network_3_weights.size();
    }
    no_of_inputs_to_be_patched++;

  }

  if(sub_network_4)
  {
    network_handler sub_network_4_handler(sub_network_4);
    sub_network_4_handler.return_network_information(sub_network_4_weights, sub_network_4_biases);
    if(max_number_of_weight_sets < sub_network_4_weights.size())
    {
        max_number_of_weight_sets = sub_network_4_weights.size();
    }
    no_of_inputs_to_be_patched++;
  }

  no_of_inputs = ((main_network_weights[0][0]).size()) - no_of_inputs_to_be_patched;
  if(no_of_inputs != (sub_network_1_weights[0][0]).size())
  {
    cout << " Sub network 1 and main network inputs count doesnot match " << endl;
  }

  vector< vector< datatype > > adhesive_weights;
  vector< datatype > adhesive_biases;
  adhesive_weights = main_network_weights[0];
  adhesive_biases = main_network_biases[0];

  datatype extra_bias = -network_offset * 2;

  adjust_offset_in_weights(adhesive_weights, adhesive_biases, extra_bias,
                          network_offset, scaling_factor, no_of_inputs_to_be_patched);
  vector< vector< vector< datatype > > > top_weights;
  vector< vector< datatype > > top_biases;

  create_fake_network(top_weights, top_biases, max_number_of_weight_sets - 1,
                      no_of_inputs, extra_bias);

  patch_networks_vertically(top_weights, top_biases, sub_network_1_weights, sub_network_1_biases);


  if(sub_network_2)
  {
    while(sub_network_2_weights.size() < max_number_of_weight_sets)
    {
      add_fake_layer_to_right(sub_network_2_weights, sub_network_2_biases);
    }
    patch_networks_vertically(top_weights, top_biases, sub_network_2_weights, sub_network_2_biases);
  }
  if(sub_network_3)
  {
    while(sub_network_3_weights.size() < max_number_of_weight_sets)
    {
      add_fake_layer_to_right(sub_network_3_weights, sub_network_3_biases);
    }
    patch_networks_vertically(top_weights, top_biases, sub_network_3_weights, sub_network_3_biases);
  }
  if(sub_network_4)
  {
    while(sub_network_4_weights.size() < max_number_of_weight_sets)
    {
      add_fake_layer_to_right(sub_network_4_weights, sub_network_4_biases);
    }
    patch_networks_vertically(top_weights, top_biases, sub_network_4_weights, sub_network_4_biases);
  }


  // Buidling the network to be returned :

  i = 0;
  while( i < max_number_of_weight_sets)
  {
    return_weights.push_back( top_weights[i] );
    return_biases.push_back( top_biases[i] );
    i++;
  }

  return_weights.push_back(adhesive_weights);
  return_biases.push_back(adhesive_biases);



  i = 1;
  while(i < main_network_weights.size())
  {
    return_weights.push_back( main_network_weights[i] );
    return_biases.push_back( main_network_biases[i] );
    i++;
  }


  write_network_to_file( return_weights, return_biases, output_file) ;


}

int split_set(set_info current_set, set_info stable_box,
               vector< set_info >& group_of_sets
)
{
  unsigned int i, j ,k, split_happens, current_time_stamp, all_in;
  unsigned int no_of_constraints, no_of_inputs, no_of_stable_region_constraints;
  vector< vector< datatype > > region_constraints,stable_region_constraints;
  datatype degree_of_matter;
  set_info buffer_set;

  current_time_stamp = current_set.time_stamp;

  region_constraints = current_set.region_constr;
  no_of_constraints = region_constraints.size();
  if(!no_of_constraints)
  {
    cout << "No constraint received in split_set()" << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }

  no_of_inputs = (region_constraints[0]).size() - 1;
  stable_region_constraints = stable_box.region_constr;
  no_of_stable_region_constraints = stable_region_constraints.size();
  vector< datatype > buffer_for_constr;

  if( no_of_stable_region_constraints != (2 * no_of_inputs) )
  {
    cout << "The stable region constraints probably does not form a box.. "<< endl;
    cout << "exiting... " << endl;
    exit(0);
  }


  group_of_sets.clear();

  all_in = 0;
  split_happens = 0;
  i = 0;
  while(i < no_of_stable_region_constraints)
  {
    buffer_for_constr = stable_region_constraints[i];
    reverse_a_constraint(buffer_for_constr);

    if(find_if_constraint_matters(region_constraints, buffer_for_constr, stable_region_constraints, degree_of_matter))
    {
      cout << "Degree of matter = " << degree_of_matter << endl;
      if(degree_of_matter > sherlock_parameters.split_threshold)
      {
        split_happens = 1;
        buffer_set.region_constr = region_constraints;
        (buffer_set.region_constr).push_back(buffer_for_constr);
        buffer_set.time_stamp = current_time_stamp;
        group_of_sets.push_back(buffer_set);

        // The remaining part as well
        buffer_set.region_constr = region_constraints;
        (buffer_set.region_constr).push_back(stable_region_constraints[i]);
        buffer_set.time_stamp = current_time_stamp;
        group_of_sets.push_back(buffer_set);
      }

    }
    else
    {
      all_in++;
    }
    i++;
  }

  if(split_happens)
  {
    cout << "Split happens" << endl;
    return 1;
  }
  else if(all_in == (no_of_stable_region_constraints))
  {
    return 0;
  }
  else
  {
    group_of_sets.push_back(current_set);
    return 0;
  }

}
int split_set(set_info current_set, set_info stable_box,
               queue< set_info >& queue_of_sets
)
{
  unsigned int i, j ,k, split_happens, current_time_stamp, all_in;
  unsigned int no_of_constraints, no_of_inputs, no_of_stable_region_constraints;
  vector< vector< datatype > > region_constraints,stable_region_constraints;
  datatype degree_of_matter;
  set_info buffer_set;

  current_time_stamp = current_set.time_stamp;

  region_constraints = current_set.region_constr;
  no_of_constraints = region_constraints.size();
  if(!no_of_constraints)
  {
    cout << "No constraint received in split_set()" << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }

  no_of_inputs = (region_constraints[0]).size() - 1;
  stable_region_constraints = stable_box.region_constr;
  no_of_stable_region_constraints = stable_region_constraints.size();
  vector< datatype > buffer_for_constr;

  if(!find_whether_overlap(region_constraints, stable_region_constraints))
  {
    queue_of_sets.push(current_set);
    return 0;
  }

  if( no_of_stable_region_constraints != (2 * no_of_inputs) )
  {
    cout << "The stable region constraints probably does not form a box.. "<< endl;
    cout << "exiting... " << endl;
    exit(0);
  }

  datatype internal_size, size_inside_target = 0;
  vector< vector < datatype > > excess;
  all_in = 0;
  split_happens = 0;
  i = 0;
  while(i < no_of_stable_region_constraints)
  {

    buffer_for_constr = stable_region_constraints[i];
    reverse_a_constraint(buffer_for_constr);

    if(find_if_constraint_matters(region_constraints, buffer_for_constr, stable_region_constraints, degree_of_matter))
    {
      cout << "Degree of matter = " << degree_of_matter << " for i = " << i << endl;
      if(degree_of_matter > sherlock_parameters.split_threshold)
      {
        split_happens = 1;
        buffer_set.region_constr = region_constraints;
        (buffer_set.region_constr).push_back(buffer_for_constr);
        buffer_set.time_stamp = current_time_stamp;
        queue_of_sets.push(buffer_set);


        // The remaining part as well
        // buffer_set.region_constr = region_constraints;
        // (buffer_set.region_constr).push_back(stable_region_constraints[i]);
        // find_size_inside_target(buffer_set.region_constr, stable_region_constraints, size_inside_target);
        // find_size(region_constraints, internal_size);
        //
        // if(internal_size - size_inside_target > num_tolerance)
        // {
        //   cout << "THe other one matters .. " << endl;
        //   find_the_non_overlap(buffer_set.region_constr, stable_region_constraints,excess );
        //   buffer_set.region_constr = excess;
        //   buffer_set.time_stamp = current_time_stamp;
        //   queue_of_sets.push(buffer_set);
        //   cout << "Excess added = " << endl;
        //   print_region(excess);
        // }


      }

    }
    else
    {
      all_in++;
    }
    i++;
  }


  if(split_happens)
  {
    cout << "Split happens" << endl;
    return 1;
  }
  else if(all_in == (no_of_stable_region_constraints))
  {
    return 0;
  }
  else
  {
    queue_of_sets.push(current_set);
    return 0;
  }

}

void simulate_accelerated(
  network_handler system_network,
  unsigned int acceleration_number,
  vector<unsigned int> important_outputs,
  vector< datatype > scaling_factor,
  vector< datatype > offset_already,
  vector< vector< datatype > > input_constraints,
  vector< vector< datatype > >& output_bias_terms
)
{

      unsigned int dim = system_network.no_of_inputs;
      unsigned int no_of_sys_outputs = input_constraints.size()/2;

      output_bias_terms.clear();

      unsigned int i, j , k;
      vector< datatype > zero(2,0);
      i = 0;
      while(i < no_of_sys_outputs)
      {

        output_bias_terms.push_back(zero);
        i++;
      }

      vector< vector< vector< datatype > > > return_weights, weights;
      vector< vector< datatype > > return_biases, biases;

      system_network.return_network_information(weights, biases);


      if(acceleration_number > 1)
      {
        patch_networks_horizontally(weights, biases, scaling_factor, offset_already, important_outputs,
                                    weights, biases, return_weights, return_biases);

        acceleration_number -= 2;
        unsigned int h = 0;
        while(h < acceleration_number)
       {
          patch_networks_horizontally(return_weights, return_biases, scaling_factor, offset_already, important_outputs,
                                    weights, biases, return_weights, return_biases);
          h++;
       }
        weights = return_weights;
        biases = return_biases;

      }

      system_network.update_information(weights, biases);

      i = 0;
      while(i < no_of_sys_outputs)
      {
        system_network.return_interval_output(input_constraints, output_bias_terms[i], i+1);
        i++;
      }



}

void find_limits_using_reluplex(
  vector< vector< datatype > > input_interval,
  vector< datatype >& output_range
)
{
  unsigned int i,j ,k, no_of_inputs;
  datatype max_val, min_val;
  if(!input_interval.size())
  {
    cout << "Received input interval is empty in " <<
    " find_limits_using_reluplex() " << endl;
    cout << "Exiting.. " << endl;
    exit(0);
  }

  if((input_interval[0].size()!=2) )
  {
    cout << "Input interval received in find_limits_using_reluplex() " <<
    " is not in a hyper rectangle format ! " << endl;
    cout << "Exiting.. " << endl;
    exit(0);
  }

  no_of_inputs = input_interval.size();
  output_range.clear();

  datatype threshold_size = 1e-2;
  vector< datatype > limits(2);
  datatype init_upper = 20;
  datatype init_lower = 0;
  limits[0] = init_lower;
  limits[1] = init_upper;

  vector< datatype > buffer_limits_low(2);
  vector< datatype > buffer_limits_up(2);

  vector< datatype > point;
  vector< vector< vector< datatype > > > weights;
  vector< vector< datatype > > biases;
  vector< vector< unsigned int > > active_weights;
  char name[] = "./network_files/neural_network_information";
  network_handler network(name);
  network.return_network_information(weights, biases);

  // Let us find the maximum first
  while((limits[1] - limits[0]) > threshold_size)
  {
    buffer_limits_low[0] = limits[0];
    buffer_limits_low[1] = (limits[0] + limits[1])/2;

    buffer_limits_up[0] = (limits[0] + limits[1])/2;
    buffer_limits_up[1] = limits[1];

    if( check_limits_using_reluplex(buffer_limits_up, input_interval, 1, point) )
    {
      limits = buffer_limits_up;

    }
    else if(check_limits_using_reluplex(buffer_limits_low, input_interval, 1, point))
    {
      limits = buffer_limits_low;

    }
    else
    {
      limits[0] =  2 * init_lower;
      limits[1] =  2 * init_upper;
    }
    cout << "limits = [" << limits[0] << " , " << limits[1] <<" ]" << endl;
  }
  max_val = compute_network_output(point, weights, biases, active_weights);

  cout << "Max found = " << max_val << endl;
  // Let us find the minimum now
  limits[0] = init_lower;
  limits[1] = init_upper;

  while((limits[1] - limits[0]) > threshold_size)
  {
    buffer_limits_low[0] = limits[0];
    buffer_limits_low[1] = (limits[0] + limits[1])/2;

    buffer_limits_up[0] = (limits[0] + limits[1])/2;
    buffer_limits_up[1] = limits[1];

    if( check_limits_using_reluplex(buffer_limits_low, input_interval, 1, point) )
    {
      limits = buffer_limits_low;
    }
    else if(check_limits_using_reluplex(buffer_limits_up, input_interval, 1, point))
    {
      limits = buffer_limits_up;
    }
    else
    {
      limits[0] =  2 * init_lower;
      limits[1] =  2 * init_upper;
    }
    cout << "limits = [" << limits[0] << " , " << limits[1] <<" ]" << endl;
  }

  min_val = compute_network_output(point, weights, biases, active_weights);
  cout << "Min found = " << min_val;

  output_range.push_back(min_val);
  output_range.push_back(max_val);

}
