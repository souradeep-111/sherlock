#include "network_computation.h"


int __check_if_the_weights_and_biases_make_sense__(
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases
)
{
  unsigned int no_of_inputs, no_of_outputs, no_of_hidden_layers;
  unsigned int i, j , k;
  vector< unsigned int > network_configuration;
  deduce_network_configuration(weights, biases, network_configuration);
  no_of_hidden_layers = weights.size() - 1;
  no_of_inputs = (weights[0][0]).size();
  no_of_outputs = (weights[no_of_hidden_layers][0]).size();

  unsigned int  no_of_neurons_prev, no_of_neurons_current, no_of_neurons_next;
  // Should have checked whether information in the weights and biases
  // tallied against each other, but that is already being done by the
  // deduce_network_configuration()

  no_of_neurons_next = (weights[0]).size() ;

  i = 1;
  while(i < (no_of_hidden_layers + 1))
  {
    no_of_neurons_prev = (weights[i][0]).size();

    if(no_of_neurons_prev != no_of_neurons_next)
    {
      return 0;
    }
    no_of_neurons_next = (weights[i]).size();
    i++;
  }


  return 1;
}
void deduce_network_configuration(
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  vector< unsigned int >& network_configuration
)
{
  unsigned int no_of_inputs, no_of_outputs, no_of_layers,
               i , j , k, no_of_neurons;
  no_of_layers = weights.size();
  no_of_inputs = (weights[0][0]).size();
  network_configuration.clear();
  i = 0;
  while(i < no_of_layers)
  {
    no_of_neurons = (weights[i]).size();
    if(no_of_neurons != (biases[i]).size())
    {
      cout << "Mismatch in the number of biases and weights during deduce_network_configuration() .. " << endl;
    }

    network_configuration.push_back(no_of_neurons);
    i++;
  }

  if(network_configuration.size() != no_of_layers)
  {
    cout << "Wrong layer count in deduce_network_configuration() .. " << endl;
    cout << "Exiting.... " << endl;
    exit(0);
  }

}

datatype compute_network_output(
    vector< datatype > inputs,
    vector < vector < vector< datatype > > > weights,
    vector< vector < datatype > > biases,
    vector< vector < unsigned int > >& active_weights
)
{
  unsigned int size_1,size_2,size_3, i , j , k, no_of_neurons;
  size_1 = weights.size(); // The number of sets of weights we have
  // there should be #hidden_layers + 1 number of such sets of weights
  vector< unsigned int > network_configuration;
  deduce_network_configuration(weights, biases, network_configuration);


  size_3 = biases.size();

  if(size_1 != size_3)
  {
    cout << "Number of biases and number of sets of weight does not match in compute_network_output()! " << endl;
    cout << "Exiting .. " << endl;
    exit(0);
  }


  if(((weights[0][0]).size()) != inputs.size())
  {
    cout << "Input dimension does not match the inputs weights dimension in compute_network_output() " << endl;
    cout << "Input size = " << inputs.size() << endl;
    cout  << "Weights expect an input size of : " << (weights[0][0]).size() << endl;
    cout << "Exiting.. " << endl;
    exit(0);
  }

  size_3 = (weights[0]).size(); // # rows in level 1 of the matrix
  size_2 = (weights[0][0]).size(); // # columns in the first row of the matrix


  active_weights.clear();
  vector< unsigned int > buff_vec(size_3,0);


  vector< datatype > buffer_vector(size_3,(datatype)0);
  vector <vector< datatype > > input_weight_matrix(size_3, vector<datatype> (size_2,0));

  for(i = 0;i < size_3; i++)
  {
    for(j = 0; j < size_2; j++)
    {
      input_weight_matrix[i][j] = weights[0][i][j];
    }
  }
  vector <vector< datatype > > inner_weight_matrix;
  vector< vector< datatype > > output_weight_matrix;
  output_weight_matrix = weights[size_1 - 1];


  // The actual computation of the neural network
  i = 0;
  while(i < size_1)
  {
    if(i == 0)
    {
      buffer_vector = compute_activation(inputs, input_weight_matrix, biases[0]);
      buff_vec = find_positives(buffer_vector);
      active_weights.push_back(buff_vec);
    }
    else if(i == (size_1 -1) )
    {
      buffer_vector = compute_activation(buffer_vector, output_weight_matrix, biases[i]);
      buff_vec = find_positives(buffer_vector);
      active_weights.push_back(buff_vec);
    }
    else
    {
        inner_weight_matrix = weights[i];
        buffer_vector = compute_activation(buffer_vector, inner_weight_matrix, biases[i]);
        buff_vec = find_positives(buffer_vector);
        active_weights.push_back(buff_vec);
    }
    i++;
  }

  datatype return_val;
  if(buffer_vector.size()!=1)
  {
    cout << "Size of the output buffer vector is wrong in compute_network_output() !! " << endl;
    exit(0);
  }

  i = 0;
  while(i < size_1)
  {
    if( (active_weights[i]).size() != network_configuration[i] )
    {
      cout << "active_weights and network configuration does not match in "
      << " compute_network_output()" << endl;
      cout << "Exiting...." << endl;
      exit(0);
    }
    i++;
  }

  return_val = buffer_vector[0];
  return return_val;
}


vector < unsigned int > find_positives(vector<datatype >& input )
{
  unsigned int size,i,j,k;
  size = input.size();
  vector < unsigned int > output(size,(unsigned int)0);
  i = 0;
  while(i < size )
  {
    if(input[i] > 0)
    {
      output[i] = 1;
    }
    i++;
  }
  return output;
}

vector< datatype > compute_activation (
    vector< datatype >& inputs,
    vector< vector< datatype > >& weights,
    vector<datatype>& bias)
{
  unsigned int no_of_inputs, input_dimension, output_dimension, no_of_bias_terms;
  no_of_inputs = inputs.size();
  if(!weights.empty())
  {
    input_dimension = (weights[0]).size();
    output_dimension = weights.size();
  }
  else
  {
    cout << "Weights matrix received while computing the output of a layer in the network is empty !! " << endl;
    cout << "Exiting... " << endl;
    exit(0);
  }

  no_of_bias_terms = bias.size();

  if(input_dimension != no_of_inputs)
  {
    cout << "Input dimension does not match the number of inputs the weight matrix expects in compute_activation !! " << endl;
    cout << "Exiting... " << endl;
    cout << "Input dimension = " << input_dimension << endl;
    cout << "No of inputs = " << no_of_inputs << endl;
    exit(0);
  }
  if(output_dimension != no_of_bias_terms)
  {
    cout << "Output dimension of the weight matrix received does not match the number of bias terms available in the received bias vector " << endl;
    cout << "Exiting... " << endl;
    exit(0);
  }

  unsigned int i , j , k , l;
  vector< datatype > output_mat_mul(output_dimension,(datatype) 0);
  datatype sum;

  // Multiplying the weights
   i = 0;
   while( i < output_dimension)
   {
     sum = 0;
     j = 0;
     while(j < input_dimension)
     {
       sum +=  (weights[i][j] * inputs[j]);
       j++;
     }
     output_mat_mul[i] = sum;
     i++;
   }

  //  Adding the biases
  i = 0;
  while(i < output_dimension)
  {
    output_mat_mul[i] +=  bias[i];
    i++;
  }

  // Doing the Relu thresholding
  output_mat_mul = do_relu(output_mat_mul);

  return output_mat_mul;
}

vector< datatype > compute_activation_no_relu (
    vector< datatype >& inputs,
    vector< vector< datatype > >& weights,
    vector<datatype>& bias)
{
  unsigned int no_of_inputs, input_dimension, output_dimension, no_of_bias_terms;
  no_of_inputs = inputs.size();
  if(!weights.empty())
  {
    input_dimension = (weights[0]).size();
    output_dimension = weights.size();
  }
  else
  {
    cout << "Weights matrix received while computing the output of a layer in the netwrok is empty !! " << endl;
    cout << "Exiting... " << endl;
    exit(0);
  }


  no_of_bias_terms = bias.size();

  if(input_dimension != no_of_inputs)
  {
    cout << "Input dimension does not match the number of inputs the weight matrix expects in compute_activation_no_relu !! " << endl;
    cout << "Exiting.. " << endl;
    exit(0);
  }
  if(output_dimension != no_of_bias_terms)
  {
    cout << "Output dimension of the weight matrix received does not match the number of bias terms available in the received bias vector " << endl;
    cout << "Exiting .. " << endl;
    exit(0);
  }

  unsigned int i , j , k , l;
  vector< datatype > output_mat_mul(output_dimension,(datatype) 0);
  datatype sum;

  // Multiplying the weights
   i = 0;
   while( i < output_dimension)
   {
     sum = 0;
     j = 0;
     while(j < input_dimension)
     {
       sum +=  weights[i][j] * inputs[j];
       j++;
     }
     output_mat_mul[i] = sum;
     i++;
   }

  //  Adding the biases
  i = 0;
  while(i < output_dimension)
  {
    output_mat_mul[i] +=  bias[i];
    i++;
  }

  return output_mat_mul;
}

vector< datatype > do_relu(vector<datatype>& input)
{
  unsigned int size = input.size();
  vector< datatype > output(size,(datatype)0);

  unsigned int i;
  i = 0;
  while(i  < size)
  {
    if( input[i] > (datatype)0 )
    {
      output[i] = input[i];
    }
    i++;
  }
  return output;
}

vector< vector< datatype > > return_constraint_vectors_from_weights (
  vector< vector<datatype> > weights,
  vector< datatype > biases,
  vector< unsigned int > active_neurons
)
{
  unsigned int no_of_outputs = weights.size();
  unsigned int no_of_inputs = (weights[0]).size();

  if(active_neurons.size() != no_of_outputs )
  {
    cout << " Size of the vector for no_of_active_neurons not equal to number of outputs !" << endl;
  }
  vector < vector< datatype > > constraint_collection;
  vector< datatype > constraint(no_of_inputs+1);
  unsigned int i, j , k;
  i = 0;
  while(i < no_of_outputs)
  {
    if(active_neurons[i])
    {
      j = 0;
      while(j < no_of_inputs)
      {
        constraint[j] = weights[i][j];
        j++;
      }
      constraint[j] = biases[i];
    }
    else
    {
      j = 0;
      while(j < no_of_inputs)
      {
        constraint[j] = -weights[i][j];
        j++;
      }
      constraint[j] = -biases[i];
    }
    constraint_collection.push_back(constraint);
    i++;
  }

  return constraint_collection;

}
void return_weights_and_bias_from_a_single_layer_one_output_network(
  vector< vector<datatype> > input_weight_matrix,
  vector< datatype > input_bias,
  vector< vector< datatype > > output_weight_matrix,
  datatype output_bias,
  vector<unsigned int> active_neurons,
  unsigned int output_status,
  vector < datatype >& return_weight,
  datatype& return_bias
)
{
  unsigned int no_of_neurons, no_of_inputs, i , j , k;
  no_of_neurons = input_weight_matrix.size();
  no_of_inputs = (input_weight_matrix[0]).size();


  vector< datatype > buff_vector_1(no_of_neurons);
  vector< datatype > buff_vector_2(no_of_inputs);

  vector< datatype > buff_vector_3(1);
  vector< datatype > buff_vector_4(1);

  vector< vector< datatype > > filtered_input_weight_matrix_1;
  vector< datatype > filtered_input_bias_terms_1;

  vector< vector< datatype > > filtered_output_weight_matrix_1(1);

  vector< datatype > input_vector(no_of_inputs,0);

  // Filtering for the working neurons here for the input
  i = 0 ;
  while(i < no_of_neurons)
  {
    if(active_neurons[i])
    {
      buff_vector_2 = input_weight_matrix[i];
      filtered_input_weight_matrix_1.push_back(buff_vector_2);
      filtered_input_bias_terms_1.push_back(input_bias[i]);
    }
    i++;
  }
  buff_vector_1 = compute_activation_no_relu(input_vector,
                                           filtered_input_weight_matrix_1,
                                           filtered_input_bias_terms_1);

  // Filtering for the working neurons here for the output
   i = 0 ;
   while(i < no_of_neurons)
   {
     if(active_neurons[i])
     {
       (filtered_output_weight_matrix_1[0]).push_back(output_weight_matrix[0][i]);
     }
    i++;
   }

  buff_vector_4[0] = output_bias;
  buff_vector_3 = compute_activation_no_relu(buff_vector_1,
                                             filtered_output_weight_matrix_1,
                                             buff_vector_4);

  return_bias = buff_vector_3[0];



  return_weight.clear();
  j = 0;
  while(j < no_of_inputs)
  {
    // Clearing out all the inputs to the net and just turning high one of them
    fill(input_vector.begin(), input_vector.end(), 0);
    input_vector[j] = 1;
    buff_vector_1 = compute_activation_no_relu(input_vector,
                                             filtered_input_weight_matrix_1,
                                             filtered_input_bias_terms_1);


    buff_vector_3 = compute_activation_no_relu(buff_vector_1,
                                              filtered_output_weight_matrix_1,
                                              buff_vector_4);


    //  Taking off the bias terms which were anyway added to the network output
    buff_vector_3[0] -= return_bias;

    // cout << "For input vector = " << input_vector[0]<< input_vector[1] << endl;
    // cout << "Output = " << buff_vector_3[0] << endl;

    if(output_status)
    {
      return_weight.push_back(buff_vector_3[0]);
    }
    else
    {
      return_weight.push_back(-buff_vector_3[0]);
    }

    j++;
  }
  if(!output_status)
  {
    return_bias *= -1;
  }


}

int return_shorter_network(
    vector< vector< vector < datatype > > > weights,
    vector< vector < datatype > > biases,
    unsigned int neuron_no_in_layer_2,
    vector< vector< datatype > > & input_weight_matrix,
    vector< datatype > & input_bias,
    vector< vector< datatype > > & output_weight_matrix,
    datatype & output_bias
)
{
  unsigned int no_of_layers; // the actual number of hidden layers
  unsigned int no_of_neurons, i, j , k ;
  vector< datatype > weights_vector;

  // clearing the received data structures
  input_weight_matrix.clear();
  input_bias.clear();
  output_weight_matrix.clear();

  no_of_layers = weights.size();

  if(no_of_layers == 1)
  {
    cout << "Weights matrix has no inner layer in return_shorter_network.. " << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }

  no_of_neurons = (weights[0]).size();


  if(no_of_layers == 2)
  {
    i = 0;
    while(i < no_of_neurons)
    {
      input_weight_matrix.push_back(weights[0][i]);
      input_bias.push_back(biases[0][i]);
      i++;
    }

    (output_weight_matrix).push_back(weights[1][0]);
    output_bias = biases[1][0];
    return 1;
  }

  // Setting the input weights
  i = 0;
  while(i < no_of_neurons)
  {
    input_weight_matrix.push_back(weights[0][i]);
    input_bias.push_back(biases[0][i]);
    i++;
  }
  // setting the output weights
  weights_vector.clear();
  i = 0;
  while(i < no_of_neurons)
  {
    weights_vector.push_back(weights[1][neuron_no_in_layer_2][i]);
    i++;
  }

  output_weight_matrix.push_back(weights_vector);
  output_bias = biases[1][neuron_no_in_layer_2] ;

  return 1;
}


int find_maximum_penetration(vector< vector< unsigned int > > active_weights)
{
  unsigned int no_of_neurons, no_of_layers,i , j, mark;
  // rememeber the number of layers includes the output neurons as well
  // the structure of the active weights matrix goes like this, it
  // has the layers arranged according to the rows in the matrix

  no_of_layers = active_weights.size();
  // no_of_neurons = (active_weights[0]).size();

  if(no_of_layers == 0)
  {
    cout << "Need at least one hidden layer to do the proper computation in find_maximum_penetration() " << endl;
    cout << "Exiting .. " << endl;
    exit(0);
  }

  int maximum_penetration = 0;

  i = 0;
  while(i < no_of_layers)
  {
    if( i == (no_of_layers - 1) )
    {
      if(active_weights[i][0] == 1)
      {
        maximum_penetration = no_of_layers;
        return maximum_penetration;
      }
    }

    mark = 0;
    j = 0 ;
    while(j <  (active_weights[i]).size() )
    {
      if(active_weights[i][j])
      {
        mark = 1;
      }
      j++;
    }

    if(mark)
    {
      maximum_penetration = i + 1;
    }
    else
    {
      return maximum_penetration;
    }
    i++;
  }

  return 0;
}

void append_matrix_to_matrix(
    vector< vector< datatype > >& target,
    vector< vector< datatype > > source
 )
{
   unsigned int size, i , j , k;
   size = source.size();
   if(!size)
   {
     cout << "Source matrix received is empty in append_matrix_to_matrix() !! " << endl;
     cout << "Exiting ... " << endl;
     exit(0);
   }

   i = 0;
   while(i < size)
   {
     target.push_back(source[i]);
     i++;
   }

}

vector< vector< datatype > > create_constraint_from_weights_and_bias(
     vector< datatype > weights,
     datatype bias
)
{
    unsigned int no_of_inputs, i, j , k , size;
    no_of_inputs = weights.size();
    if(!no_of_inputs)
    {
      cout << "Weights received is empty " << endl;
    }

    vector< vector< datatype > > constraint_mat(1,vector<datatype> (no_of_inputs + 1,0));
    i = 0;
    while(i < no_of_inputs)
    {
      constraint_mat[0][i] = weights[i];
      i++;
    }
    constraint_mat[0][i] = bias;

    return constraint_mat;
}

void delete_the_first_n_constraints(
  vector< vector< datatype > >& constraint_matrix,
  unsigned int number
)
{
  unsigned int i, j , k, size_1, size_2;
  size_1 = constraint_matrix.size();
  size_2 = (constraint_matrix[0]).size();

  vector< vector< datatype > > copied_matrix(size_1 - number, vector< datatype > (size_2,0));

  i = number;
  while(i < size_1)
  {
    copied_matrix[i-number] = constraint_matrix[i];
    i++;
  }
  constraint_matrix.clear();
  constraint_matrix = copied_matrix;
  // cout << " Size after deletion = " << constraint_matrix.size() << endl;

}
void remove_the_last_constraint(
  vector< vector< datatype > >& constraint_matrix
)
{
  unsigned int size;
  size = constraint_matrix.size();
  constraint_matrix.erase(constraint_matrix.begin() + size - 1);
}

void replace_layers(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >& biases,
  vector< vector< datatype > > new_weights,
  vector< datatype > new_bias
)
{

  unsigned int no_of_layers,no_of_neurons, i, j,k , no_of_inputs;
  no_of_layers = weights.size();
  // no_of_neurons = (weights[0]).size();
  no_of_inputs = (weights[0][0]).size();

  if(biases.size() != no_of_layers)
  {
    cout << "THe number of bias sets and weight sets do not match in replace_layers() ! " << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }

  if( (biases[0]).size() != (weights[0]).size() )
  {
    cout << " Number of neurons in the biases vector and weights do not match " << endl;
    cout <<  "  biases size = " << (biases[0]).size() << endl;
    cout <<  "  no of neurons = " << (weights[0]).size() << endl;
    cout << "Exiting .. " << endl;
    exit(0);
  }

  if(no_of_inputs != (new_weights[0]).size() )
  {
    cout << " Number of inputs in the new weights and the main weights matrix do not match " << endl;
    cout <<  "  No of inputs from main weights matrix = " << no_of_inputs << endl;
    cout <<  "  no of inputs in the new weights matrix = " << (weights[0]).size() << endl;

  }



  if(no_of_layers != 1)
  {
    // replace the first layer's weights and biases as the new_weights and
    // new_bias

    weights[0] = new_weights;
    biases[0]  = new_bias;

    // delete the second layer
    weights.erase(weights.begin() + 1);
    biases.erase(biases.begin() + 1);

  }

  if(!__check_if_the_weights_and_biases_make_sense__(weights, biases) )
  {
    cout << "Weights and biases matrix does not make any sense in replace_layers().. " << endl;
    cout << "Exiting.. " << endl;
    exit(0);
  }

}

int check_limits_using_reluplex(
  vector< datatype > limits,
  vector< vector< datatype > > region,
  int direction,
  vector< datatype >& counter_example
)
{
  ofstream output_file;
  output_file.open("./headers/query_interval");

  unsigned int i , j , k, size_1, size_2, input_size;
  input_size = region.size();
  datatype data, counter_val;


  i = 0;
  while(i < input_size)
  {
    data = region[i][0];
    output_file << data << "\n";
    data = region[i][1];
    output_file << data << "\n";
    i++;
  }

  // if(direction == 1)
  // {
  //   data = limit_found + sherlock_parameters.num_tolerance;
  //   output_file << data << "\n";
  //   data = upper_lim;
  //   output_file << data << "\n";
  // }
  // if(direction == (-1))
  // {
  //   data = -upper_lim;
  //   output_file << data << "\n";
  //   data = limit_found - sherlock_parameters.num_tolerance;
  //   output_file << data << "\n";
  // }

    data = limits[0];
    output_file << data << "\n";
    data = limits[1];
    output_file << data << "\n";

  vector< vector < vector< datatype > > > bogus;
  vector< vector < datatype > > constraint_collection;
  create_constraint_from_interval(constraint_collection, region);


  output_file.close();

  cout << "Started reluplex " << endl;
  system("./Reluplex/reluplex/reluplex.elf>dump");
  system("rm dump");
  cout << "Reluplex ends " << endl;

  ifstream input_file;
  input_file.open("./headers/result");

  input_file >> data;
  if(data == 0)
  {
    input_file.close();
    return 0;
  }

  counter_example.clear();
  if(data == 1)
  {
    i = 0;
    while(i < input_size)
    {
      input_file >> data;
      counter_example.push_back(data);
      i++;
    }

    // if(!check_counter_example(constraint_collection,bogus,
    //                             counter_example))
    // {
    //   return 1;
    // }

    input_file >> data;
    counter_val = data;
    // cout << "Counter val = " << counter_val << endl;
    // cout << "Limit found = " << limit_found << endl;
    input_file.close();

    // if(direction == 1)
    // {
    //   if(fabs(counter_val - limit_found) < reluplex_tolerance)
    //   {
    //     return 1;
    //   }
    //   else
    //   {
    //     return 0;
    //   }
    // }
    // if(direction == (-1))
    // {
    //   if(fabs( limit_found - counter_val ) < reluplex_tolerance)
    //   {
    //     return 1;
    //   }
    //   else
    //   {
    //     return 0;
    //   }
    // }

    return 1;
  }

  return 0;


}

void reverse_a_constraint(
  vector< datatype >& constraint
)
{
  unsigned int i, size;
  size = constraint.size();
  i = 0;
  while(i < size)
  {
    constraint[i] = -constraint[i];
    i++;
  }
}


void create_constraint_from_interval (
  vector< vector< datatype > >&  constraint_matrix,
  vector< vector< datatype > > interval
)
{
  unsigned int no_of_inputs, i ,j ,k;
  no_of_inputs = interval.size();
  vector< datatype > constraint_vector(no_of_inputs + 1);
  constraint_matrix.clear();

  i = 0;
  while(i  < no_of_inputs)
  {
    fill(constraint_vector.begin(), constraint_vector.end(), 0);

    constraint_vector[i] = 1;
    constraint_vector[no_of_inputs] = -interval[i][0];
    constraint_matrix.push_back(constraint_vector);
    constraint_vector[i] = -1;
    constraint_vector[no_of_inputs] = interval[i][1];
    constraint_matrix.push_back(constraint_vector);

    i++;
  }

}

void find_the_real_range(
  vector< vector< datatype > >& input,
  vector< datatype >& output
)
{
  unsigned int size , covered_min, covered_max, no_of_covers, i , j ,k;
  size = input.size();
  datatype temp_max, temp_min , abs_max, abs_min;
  abs_max = input[0][1];
  abs_min = input[0][0];
  datatype epsilon = ((datatype)1)/((datatype)10000000);


  no_of_covers = 0;
  i = 0;
  while(i < size)
  {
   //  Checking the maximum
       temp_max = input[i][1];
       covered_max = 0;
       j = 0;
       while(j < size)
       {
         if(i != j)
         {
           if(
               ( (input[j][0] - epsilon) < temp_max ) &&
               ( (input[j][1] + epsilon) > temp_max )
             )
             {
               covered_max = 1;
               break;
             }
         }
         else
         {
           covered_max = 1;
         }
         j++;
       }

   //  Checking the minimum
      temp_min = input[i][0];
      covered_min = 0;
      j = 0;
      while(j < size)
      {
        if(i != j)
        {
          if(
              ( (input[j][0] - epsilon) < temp_min ) &&
              ( (input[j][1] + epsilon) > temp_min )
            )
            {
               covered_min = 1;
               break;
            }
        }
        else
        {
          covered_min = 1;
        }
        j++;
      }

   if(covered_max && covered_min)
   {
     no_of_covers ++;
   }

   if(abs_max < temp_max)
   {abs_max = temp_max;}
   if(abs_min > temp_min)
   {abs_min = temp_min;}

   i++;
  }

  if(no_of_covers == size)
  {
    output[0] = abs_min;
    output[1] = abs_max;
  }

}

int similar(
  vector< datatype > vector_1,
  vector< datatype > vector_2
)
{
  datatype epsilon = sherlock_parameters.num_similar;
  unsigned int i, j , size;
  size = vector_1.size();
  j = 1;
  i = 0;
  while(i < size)
  {
    if(
      (vector_1[i] > (vector_2[i] -  epsilon)) &&
      (vector_1[i] < (vector_2[i] +  epsilon))
      )
      {
        j *= 1;
      }
      else
      {
        j = 0;
      }
    i++;
  }
  if(j)
  {
    return 1;
  }
  else
  {
    return 0;
  }

}

int detect_degeneracy(
  vector< vector < datatype > > input_interval
)
{
  unsigned int i , j , k, size;
  datatype epsilon = sherlock_parameters.epsilon_degeneracy;
  size = input_interval.size();
  i= 0 ;
  while(i < size)
  {
    if(
        ( (input_interval[i][0] + epsilon) > input_interval[i][1]) &&
        ( (input_interval[i][1] - epsilon) < input_interval[i][0])
    )
    {
      return 1;
    }

    i++;
  }
  return 0;
}

int check_counter_example(
  vector< vector< datatype > >& positive_constraint_matrix,
  vector< vector< vector< datatype > > >& collection_of_negative_constraint_matrices,
  vector< datatype >& counter_example
)
{
  unsigned int i, j , k, size_1, size_2, size_3, input_size, count;
  size_1 = positive_constraint_matrix.size();
  input_size = counter_example.size();

  datatype buff;

  // check positive constraints
  i = 0;
  while(i < size_1)
  {
    buff = 0;
    j = 0;
    while(j < input_size)
    {
      buff += positive_constraint_matrix[i][j] * counter_example[j];
      j++;
    }
    buff += positive_constraint_matrix[i][j];
   if(buff < (0-sherlock_parameters.num_tolerance))
    {
      // cout << "Wrong counter example postive constraints .." << endl;
      return 0;
    }
    i++;
  }

  // Check negative
  size_2 = collection_of_negative_constraint_matrices.size();
  i = 0;
  while(i < size_2)
  {
    size_1 = (collection_of_negative_constraint_matrices[i]).size();
    count = 0;
    j = 0;
    while(j < size_1)
    {
      buff = 0;
      k = 0;
      while(k < input_size)
      {
        buff += collection_of_negative_constraint_matrices[i][j][k] * counter_example[k];
        k++;
      }
      buff += collection_of_negative_constraint_matrices[i][j][k];
      if(buff < 0)
      {
        count++;
        break;
      }
      j++;
    }
    if(count == 0)
    {
      // cout << "Wrong counter example negative constraints .." << endl;
      // cout << "Counter example is " << counter_example[0] << "   "<< counter_example[1] << endl;
      // cout << "In matrix number = " << i << endl;
      return 0;
    }

    i++;
  }

  return 1;

}

datatype parse_string(
  const char * string
)
{
  int size, i , j, k, pos;
  datatype return_val ;
  long double den, num;
  pos = 0;
  // Let's search for the divided symbol
  size = strlen(string);
  i = 0;
  while( i < size)
  {
    if(string[i] == 47) // ASCII code for the division symbol
    {
      pos = i;
    }
    i++;
  }
  if(!pos)
  {
    return_val = 0;
    i = size - 2 ;
    while(i >=  0)
    {
      j = string[size - 2 - i] - 48 ;
      return_val += ( j * pow(10,i) );
      i--;
    }
    return return_val;
  }

  num = 0;
  i = 0 ;
  while(i <  pos)
  {
    j = string[i] - 48 ;
    k = pos - i - 1;
    num += ( j * pow(10,k) );
    i++;
  }

  den = 0;
  i = pos + 1 ;
  while(i <  size)
  {
    j = string[i] - 48 ;
    k = size - 1 - i;
    den += ( j * pow(10,k) );
    i++;
  }
  return_val = num / den;

  return return_val;
}

int find_random_sample(
  vector< vector< datatype > > positive_constraint,
  vector< datatype >& counter_example
)
{
  // so, the way it is done here is very simple.
  // First we are finding the centre of the poyhedral from the
  // directions parallel to the axes. Then perturb  that point
  // randomly to find an interior point

  unsigned int i , j , k, size_1, size_2, input_size, no_of_constraints;
  no_of_constraints = positive_constraint.size();
  input_size = (positive_constraint[0]).size() - 1;
  vector< vector< vector< datatype > > > list_of_negative_constraints;
  unsigned int scale = 1e4;
  unsigned int no_of_tries = 1e5;

  vector< datatype > mid_point(input_size);

  // Finding the constraints which actually refer to the main principal directions
  vector< vector< datatype > > limits_in_axes_directions(input_size, vector< datatype >(2,0));

  int direction, dimension;

  i = 0;
  while(i < no_of_constraints)
  {
    size_1 = 0; // Keeps a count of no_of 1's
    size_2 = 0; // Keeps a count of no_of 0's
    j = 0;
    while(j < (input_size) )
    {
      if(
        (  (positive_constraint[i][j] < (1 + sherlock_parameters.num_similar)) &&
            (positive_constraint[i][j] > (1 - sherlock_parameters.num_similar))
        ) ||
        (  (positive_constraint[i][j] < (-1 + sherlock_parameters.num_similar)) &&
            (positive_constraint[i][j] > (-1 - sherlock_parameters.num_similar))
        )
        )
          {
            size_1 += 1;
            dimension = j;
            if(positive_constraint[i][j] > 0)
            {
              direction = 1;
            }
            else
            {
              direction = -1;
            }
          }
      else if(
          (positive_constraint[i][j] < (0 + sherlock_parameters.num_similar)) &&
          (positive_constraint[i][j] > (0 - sherlock_parameters.num_similar))
          )
          {
            size_2 += 1;
          }

      j++;
    }

    if((size_1 == 1) && (size_2 == (input_size -1) ))
    {
      if(direction > 0)
      {
        limits_in_axes_directions[dimension][0] = -(positive_constraint[i][input_size]);
        // Since for x > a , it is [1, 0 , ...,-a ] > 0
      }
      else if (direction < 0)
      {
        limits_in_axes_directions[dimension][1] = (positive_constraint[i][input_size]);
        // Since for x < b , it is [-1, 0 , ..., b] > 0
      }
    }
    i++;
  }


  i = 0;
  while(i < input_size)
  {
    mid_point[i] = (limits_in_axes_directions[i][0] + limits_in_axes_directions[i][1]) * 0.5;
    i++;
  }
  counter_example = mid_point;


  k = 0;
  while( k < no_of_tries)
  {
    srand(k);

    i = 0;
    while(i < input_size)
    {
      counter_example[i] = limits_in_axes_directions[i][0] +
       (datatype)(
                   (
                      (datatype)(rand() % scale) * (datatype)(limits_in_axes_directions[i][1] - limits_in_axes_directions[i][0])
                   )
                    /
                  ((datatype)scale)
                 ) ;
      i++;
    }

    if(
      check_counter_example(positive_constraint,
                            list_of_negative_constraints,
                            counter_example
                            )
    )
    {

      return 1;
    }
    k++;
  }
  return 0;



}

int find_random_sample_with_seed(
  vector< vector< datatype > > positive_constraint,
  vector< datatype >& counter_example,
  int seed
)
{
  // so, the way it is done here is very simple.
  // First we are finding the centre of the poyhedral from the
  // directions parallel to the axes. Then perturb  that point
  // randomly to find an interior point

  unsigned int i , j , k, size_1, size_2, input_size, no_of_constraints;
  no_of_constraints = positive_constraint.size();
  input_size = (positive_constraint[0]).size() - 1;
  vector< vector< vector< datatype > > > list_of_negative_constraints;
  unsigned int scale = 1e4;
  unsigned int no_of_tries = 1e5;

  vector< datatype > mid_point(input_size);

  // Finding the constraints which actually refer to the main principal directions
  vector< vector< datatype > > limits_in_axes_directions(input_size, vector< datatype >(2,0));

  int direction, dimension;

  i = 0;
  while(i < no_of_constraints)
  {
    size_1 = 0; // Keeps a count of no_of 1's
    size_2 = 0; // Keeps a count of no_of 0's
    j = 0;
    while(j < (input_size) )
    {
      if(
        (  (positive_constraint[i][j] < (1 + sherlock_parameters.num_similar)) &&
            (positive_constraint[i][j] > (1 - sherlock_parameters.num_similar))
        ) ||
        (  (positive_constraint[i][j] < (-1 + sherlock_parameters.num_similar)) &&
            (positive_constraint[i][j] > (-1 - sherlock_parameters.num_similar))
        )
        )
          {
            size_1 += 1;
            dimension = j;
            if(positive_constraint[i][j] > 0)
            {
              direction = 1;
            }
            else
            {
              direction = -1;
            }
          }
      else if(
          (positive_constraint[i][j] < (0 + sherlock_parameters.num_similar)) &&
          (positive_constraint[i][j] > (0 - sherlock_parameters.num_similar))
          )
          {
            size_2 += 1;
          }

      j++;
    }

    if((size_1 == 1) && (size_2 == (input_size -1) ))
    {
      if(direction > 0)
      {
        limits_in_axes_directions[dimension][0] = -(positive_constraint[i][input_size]);
        // Since for x > a , it is [1, 0 , ...,-a ] > 0
      }
      else if (direction < 0)
      {
        limits_in_axes_directions[dimension][1] = (positive_constraint[i][input_size]);
        // Since for x < b , it is [-1, 0 , ..., b] > 0
      }
    }
    i++;
  }


  i = 0;
  while(i < input_size)
  {
    mid_point[i] = (limits_in_axes_directions[i][0] + limits_in_axes_directions[i][1]) * 0.5;
    i++;
  }
  counter_example = mid_point;


  k = 0;
  while( k < no_of_tries)
  {
    srand(k + seed);

    i = 0;
    while(i < input_size)
    {
      counter_example[i] = limits_in_axes_directions[i][0] +
       (datatype)(
                   (
                      (datatype)(rand() % scale) * (datatype)(limits_in_axes_directions[i][1] - limits_in_axes_directions[i][0])
                   )
                    /
                  ((datatype)scale)
                 ) ;
      i++;
    }

    if(
      check_counter_example(positive_constraint,
                            list_of_negative_constraints,
                            counter_example
                            )
    )
    {

      return 1;
    }
    k++;
  }
  return 0;



}

int find_uniform_counter_example(
  vector< vector< datatype > > positive_constraint,
  vector< vector< vector< datatype > > > list_of_negative_constraints,
  vector< datatype >& counter_example, uint64_t& sample_number
)
{
  int i, j , k, input_size;
  input_size = (positive_constraint[0]).size() - 1;
  vector < uint64_t > grid_point(input_size,0);

  uint64_t count;
  uint64_t no_of_points = 100;
  uint64_t no_of_tries = 1;
  i = 0;
  while(i < input_size)
  {
    no_of_tries *= no_of_points;
    i++;
  }


  // Finding the range from the positive constraint
  vector< vector < datatype >  > limits(input_size, vector< datatype >(2,0));
  i = 0;
  while(i < input_size)
  {
    limits[i][0] = -positive_constraint[i][input_size];
    limits[i][1] = positive_constraint[2 * i + 1][input_size];
    i++;
  }


  while(sample_number < no_of_tries)
  {
    count = 0;
    while(count < sample_number+1)
    {

      // Find the grid co-ordinate
      grid_point[0]++;

      i = 0;
      while(i < input_size-1)
      {
        if(grid_point[i] > (no_of_points - 1))
        {
          grid_point[i] = 0;
          grid_point[i+1]++;
        }
        i++;
      }
      if(grid_point[input_size-1] == no_of_points)
      {return 0;}
      count++;

    }


    i = 0;
    while(i < input_size)
    {
      counter_example[i] = limits[i][0] +
      ( grid_point[i] *
        ( (datatype)limits[i][1] - (datatype)limits[i][0] ) )
        /(datatype)no_of_points ;
      i++;
    }
    if(
      check_counter_example(positive_constraint,
                            list_of_negative_constraints,
                            counter_example
                            )
    )
    {
      sample_number++;
      // cout << "Found uniform counter example" << endl;
      return 1;
    }

    sample_number++;
  }


  return 0;


}

void create_sub_boxes(
  vector< vector< datatype > >& input_interval,
  vector< vector< vector < datatype > > >& output_collection
)
{
  vector< vector< datatype > > interval_received;
  vector< datatype > return_val(2,0);
  interval_received  = input_interval;
  int i , j ,k;
  unsigned int no_of_inputs, size_1, size_2, no_of_sub_boxes;
  vector< unsigned int > grid_point(no_of_inputs,0);

  no_of_inputs = input_interval.size();
  vector< vector< datatype > > temp_box(no_of_inputs,vector< datatype >(2,0));


  vector< vector< vector< datatype > > > collection_of_sub_intervals;

  // Finding the size of the sub boxes
  vector< datatype > unit_size(no_of_inputs);
  j = 0;
  while(j < no_of_inputs)
  {
    unit_size[j] = (input_interval[j][1] - input_interval[j][0] )/ sherlock_parameters.no_of_sub_divisions ;
    j++;
  }

  // Making the sub_boxes
  no_of_sub_boxes = (unsigned int)(pow(sherlock_parameters.no_of_sub_divisions,no_of_inputs));

  i = 0;
  while(i < no_of_sub_boxes)
  {
    j = 0;
    while(j < no_of_inputs)
    {
      temp_box[j][0] = interval_received[j][0] + grid_point[j] * unit_size[j];
      temp_box[j][1] = interval_received[j][0] + (grid_point[j] + 1) * unit_size[j];
      j++;
    }

    output_collection.push_back(temp_box);
    // Making the grid point
      grid_point[0]++;
      j = 0;
      while(j < no_of_inputs-1)
      {
        if(grid_point[j] >= sherlock_parameters.no_of_sub_divisions)
        {
          grid_point[j] = 0;
          grid_point[j+1]++;
        }
        j++;
      }

    i++;
  }


}

vector< datatype > scale_vector(
  vector< datatype > input_vector,
  datatype factor
)
{
  if(fabs(factor) < sherlock_parameters.num_tolerance )
  {
    cout << "Scaling by a factor of almost zero .. " << endl;
    cout << "exiting.. " << endl;
    exit(0);
  }
  unsigned int i, size;
  size = input_vector.size();
  vector< datatype > output_vector(size);
  i = 0;
  while(i < size)
  {
    output_vector[i] = factor * input_vector[i];
    i++;
  }
  return output_vector;
}

vector< datatype > negate_vector(
  vector< datatype > input_vector
)
{
  unsigned int i, size;
  size = input_vector.size();
  vector< datatype > output_vector(size);
  i = 0;
  while(i < size)
  {
    output_vector[i] = -input_vector[i];
    i++;
  }
  return output_vector;
}

int propagate_point(
  vector< datatype >& point,
  vector< datatype > direction,
  vector< vector< datatype > > region
)
{
  unsigned int i, j , k, size_1, size_2, input_size;
  input_size = point.size();
  if(direction.size() != input_size)
  {
    cout << " Input size and direction vector size does not match in propagate_point().. " << endl;
    cout << "Input size = " << input_size << endl;
    cout << "direction_vector size = " << direction.size() << endl;
    exit(0);
  }

  vector< unsigned int > grid_point(input_size,0);
  vector< vector< vector< datatype > > > bogus_input;

  vector< datatype > next_point(input_size);
  i = 0;
  while(i < input_size)
  {
    next_point[i] = point[i] + direction[i]; /* * gradient_rate*/
    i++;
  }

  if(check_counter_example(region, bogus_input, next_point))
  {
    point =  next_point;
    return 1;
  }
  else
  {
    next_point = point;
    i = 0;
    while(i < input_size)
    {

      next_point[i] = point[i] + direction[i];
      if(check_counter_example(region, bogus_input, next_point))
      {
        point = next_point;
        return 1;
      }
      else // restore whatever changes you did
      {
         next_point[i] = point[i] ;
      }

      i++;
    }

  }


  return 0;
}



int check_inflection_point(
  vector< datatype > point,
  vector < vector < vector< datatype > > >& weights,
  vector< vector < datatype > >& biases,
  int direction,
  vector< vector< datatype > > region
)
{
  unsigned int i, j , k, size_1, size_2, mark, input_size;
  vector< vector< unsigned int > > bogus_input;
  input_size = point.size();
  vector< unsigned int > grid_point(input_size,0);
  vector< datatype > test_point(input_size);
  datatype orig_eval, new_eval;

  vector< vector< vector< datatype > > > bogus_input_2;

  // constraint_collection = region;
  orig_eval = compute_network_output(point, weights, biases, bogus_input);

  // cout << "Original Evaluation = " << orig_eval << endl;
  size_1 = pow(2, input_size);
  size_2 = 0;
  mark = 0;
  i = 0;
  while(i < input_size)
  {
    test_point = point;
    test_point[i] += sherlock_parameters.delta_inflection;

    new_eval = compute_network_output(test_point, weights, biases, bogus_input);

    if(check_counter_example(region, bogus_input_2, test_point))
    {

      if( (direction == 1) && (new_eval < orig_eval)  )
      {
        mark++;
      }
      else if( (direction == (-1) ) && (new_eval > orig_eval) )
      {
        mark++;
      }
      size_2 ++;
    }

    test_point = point;
    test_point[i] -= sherlock_parameters.delta_inflection;

    new_eval = compute_network_output(test_point, weights, biases, bogus_input);

    if(check_counter_example(region, bogus_input_2, test_point))
    {
      if( (direction == 1) && (new_eval < orig_eval)  )
      {
        mark++;
      }
      else if( (direction == (-1) ) && (new_eval > orig_eval) )
      {
        mark++;
      }
      size_2 ++;
    }

    i++;

  }

    if( (mark == size_2) && (size_2) )
    {
      // cout << "Returns 1 " << endl;
      // cout << "Mark = " << mark << "Size _2 = " << size_2 << endl;
      return 1;
    }

  return 0;



}


int check_limits(
  vector< vector < vector< datatype > > > weights,
  vector< vector < datatype > > biases,
  datatype limit_found,
  vector< datatype >& extrema_point,
  vector< vector< datatype > > region,
  int direction,
  vector< datatype >& counter_example
)
{
  vector< vector< unsigned int > > active_weights;
  if(sherlock_parameters.grad_search_point_verbosity)
  {
    cout << "Limit being send to  the solvers = " << limit_found << endl;
  }
  if( (sherlock_parameters.do_LP_certificate) && (prove_limit_in_NN(region, weights, biases, limit_found,
                        extrema_point, direction)))
  {
    if(sherlock_parameters.grad_search_point_verbosity)
    {
      cout << "prove limits works " << endl;
      cout << "limit found = " << limit_found << endl;
      cout << "extrema point = " << extrema_point[0] << " " << extrema_point[1] << endl;
      cout << "direction = " << direction << endl;
    }
    return 1;
  }
  else if(find_counter_example_in_NN(region, weights, biases,
            counter_example, limit_found, direction))
  {
    if(sherlock_parameters.grad_search_point_verbosity)
    {
      cout << "For direction = " << direction << endl;
      cout << "New limit found = " << limit_found << endl;
      cout << "Network Output = " << compute_network_output(counter_example, weights, biases, active_weights) << endl;
      cout << "Counter example = " << counter_example[0] << " " << counter_example[1] << endl << endl;
    }

    return 0;
  }
  else
  {
    if(sherlock_parameters.grad_search_point_verbosity)
    {
      cout << "Gets MILP certificate" << endl;
    }
    return 1;
  }

}

void expand_output_to_full_width(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >& biases
)
{
  unsigned int no_of_inputs, no_of_hidden_layers, no_of_neurons, i , j, k;
  no_of_hidden_layers = weights.size() - 1 ;
  no_of_neurons = (weights[0]).size();
  no_of_inputs = (weights[0][0]).size();

  vector< datatype > buffer_vec(no_of_neurons,0);

  i = 1;
  while(i < no_of_neurons)
  {
    (weights[no_of_hidden_layers]).push_back(buffer_vec);
    (biases[no_of_hidden_layers]).push_back(0);
    i++;
  }

  if(! ( (weights[no_of_hidden_layers]).size() == no_of_neurons) )
  {
    cout << "Expansion incorrect due to improper weights in expand_output_to_full_width() !! " << endl;
  }

  if(! ( (biases[no_of_hidden_layers]).size() == no_of_neurons) )
  {
    cout << "Expansion incorrect due to improper biases in expand_output_to_full_width() !! " << endl;
  }


}

void expand_width_of_inner_layers(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >& biases,
  unsigned int expansion_number
)
{
  vector< vector< vector < datatype > > > temp_weights;
  vector< vector< datatype > > temp_biases;

  unsigned int no_of_inputs, no_of_neurons_old, no_of_neurons_new, no_of_hidden_layers, i , j , k;
  no_of_inputs = (weights[0][0]).size();
  no_of_neurons_old = (weights[0]).size();
  no_of_neurons_new = no_of_neurons_old + expansion_number;
  no_of_hidden_layers = weights.size() - 1;

  vector< datatype > buffer_vec_1(no_of_inputs, (datatype)0);

  vector< vector< datatype > > buffer_matrix(no_of_neurons_new, vector< datatype >(no_of_neurons_new));
  vector< datatype > buffer_vec_2(no_of_neurons_new, (datatype)0);
  vector< datatype > buffer_vec_3(no_of_neurons_new, (datatype)0);
  vector< vector< datatype > > buffer_input_matrix(no_of_neurons_new, vector< datatype >(no_of_inputs,0));

  // for the input layers
  i = 0 ;
  while(i < no_of_neurons_new)
  {
    if(i < no_of_neurons_old)
    {
      fill(buffer_vec_1.begin(), buffer_vec_1.end(), 0);
      k = 0;
      while(k < no_of_inputs)
      {
        buffer_vec_1[k] = weights[0][i][k];
        k++;
      }
    }
    else
    {
      fill(buffer_vec_1.begin(), buffer_vec_1.end(), 0);
    }

    buffer_input_matrix[i] = buffer_vec_1;
    if(i < no_of_neurons_old)
    {
      buffer_vec_3[i] = biases[0][i];
    }
    else
    {
      buffer_vec_3[i] = 0;
    }
    i++;
  }
  temp_weights.push_back(buffer_input_matrix) ;
  temp_biases.push_back(buffer_vec_3);

  // for the inner layers

  i = 1;
  while(i < no_of_hidden_layers)
  {

    j = 0;
    while(j <  no_of_neurons_new)
    {

      if(j < no_of_neurons_old)
      {
        fill(buffer_vec_2.begin(), buffer_vec_2.end(), 0);
        k = 0;
        while(k < no_of_neurons_new)
        {
          if(k < no_of_neurons_old)
          {
            buffer_vec_2[k] = weights[i][j][k];
          }
          else
          {
            buffer_vec_2[k] = 0;
          }
          k++;
        }
      }
      else
      {
        fill(buffer_vec_2.begin(), buffer_vec_2.end(), 0);
      }

      buffer_matrix[j] = buffer_vec_2;
      if(j < no_of_neurons_old)
      {
        buffer_vec_3[j] = biases[i][j];
      }
      else
      {
        buffer_vec_3[j] = 0;
      }
      j++;
    }
    temp_weights.push_back(buffer_matrix);
    temp_biases.push_back(buffer_vec_3);
    i++;
  }

  vector< vector< datatype > > buffer_matrix_1(1, vector< datatype >(no_of_neurons_new,0));
  vector< datatype > bias_vec(1);

  // For the output layer
  k = 0;
  while(k < no_of_neurons_new)
  {
    if(k < no_of_neurons_old)
    {
      buffer_matrix_1[0][k] = weights[no_of_hidden_layers][0][k];
    }
    else
    {
      buffer_matrix_1[0][k] = 0;
    }
    k++;
  }
  bias_vec[0] = biases[no_of_hidden_layers][0];

  (temp_weights).push_back(buffer_matrix_1);
  (temp_biases).push_back(bias_vec);


  weights = temp_weights;
  biases = temp_biases;

  if(! ( (weights[0]).size() == no_of_neurons_new))
  {
    cout << " Wrong number of neurons in expand_width_of_inner_layers() .. "<< endl;
  }

}
void create_fake_network(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >& biases,
  unsigned int expansion_number,
  unsigned int no_of_inputs,
  datatype network_bias
)
{
  if(expansion_number > 0)
  {
    unsigned int no_of_hidden_layers, i , j , k;
    no_of_hidden_layers = expansion_number;

    vector< vector < vector< datatype > > > temp_weights;
    vector< vector < datatype > > temp_biases;
    vector< vector< datatype > > buffer_weight_matrix;
    vector< datatype > buffer_bias_vector;
    vector< datatype > temp_vec(no_of_inputs);

    vector< vector< vector< datatype > > > return_weights;
    vector< vector< datatype > > return_biases;

    i = 0 ;
    while(i < expansion_number  + 1)
    {
      buffer_weight_matrix.clear();
      buffer_bias_vector.clear();

      j = 0;
      while(j < no_of_inputs )
      {
        fill(temp_vec.begin(),temp_vec.end(),0);
        temp_vec[j] = 1;
        buffer_weight_matrix.push_back(temp_vec);
        if(!i)
        {
          // Biasing the first one only
          buffer_bias_vector.push_back(network_bias);
        }
        else
        {
          buffer_bias_vector.push_back(0);
        }
        j++;
      }
      temp_weights.push_back(buffer_weight_matrix);
      temp_biases.push_back(buffer_bias_vector);
      i++;
    }


    i = 0;
    while(i < expansion_number + 1)
    {
      return_weights.push_back(temp_weights[i]);
      return_biases.push_back(temp_biases[i]);
      i++;
    }

    weights = return_weights;
    biases = return_biases;

    if(weights.size() != expansion_number + 1)
    {
      cout << "Weights size = " << weights.size() << endl;
      cout << "Expansion number = " << expansion_number << endl;
      cout << " Wrong number of weights matrices in create_fake_network()...  " << endl;
      exit(0);
    }

  }
}

void add_fake_layer_to_right(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >& biases
)
{
  unsigned int no_of_inputs, no_of_outputs, no_of_layers_old, no_of_layers_new, no_of_neurons;
  vector<unsigned int> network_configuration;
  deduce_network_configuration(weights, biases, network_configuration);
  unsigned int i , j , k;
  no_of_layers_old = weights.size();
  no_of_layers_new = no_of_layers_old + 1;
  no_of_neurons = (weights[no_of_layers_old -1]).size();

  vector< vector< datatype > > buffer_weight_matrix(no_of_neurons, vector< datatype >(no_of_neurons,0));
  vector< datatype > buffer_bias_vector(no_of_neurons,0);
  i = 0;
  while(i < no_of_neurons)
  {
    buffer_weight_matrix[i][i] = 1;
    i++;
  }

  weights.push_back(buffer_weight_matrix);
  biases.push_back(buffer_bias_vector);



  if( (weights.size()) != no_of_layers_new )
  {
    cout << "Mismatch of sizes in add_fake_layers_to_right( ) ... " << endl;
    exit(0);
  }
}

void expand_last_few_input(
  vector< vector< datatype > >& weights,
  vector< datatype >& biases,
  vector< unsigned int > expansion_vector
)
{
  unsigned int no_of_untouched_inputs,
               inputs_to_be_expanded,
               no_of_left_neurons_new,
               no_of_left_neurons_old,
               no_of_right_neurons, i , j , k, sum, current_sum, current_index;

  no_of_right_neurons = weights.size();
  no_of_left_neurons_old = (weights[0]).size();

  inputs_to_be_expanded = expansion_vector.size();
  no_of_untouched_inputs = no_of_left_neurons_old - inputs_to_be_expanded;

  sum = 0;
  i = 0;
  while(i < inputs_to_be_expanded)
  {
    sum += expansion_vector[i];
    i++;
  }
  no_of_left_neurons_new = no_of_untouched_inputs + sum;

  if( (no_of_untouched_inputs + sum) != ( no_of_right_neurons))
  {
    if(sherlock_parameters.verbosity)
    {
      cout << "Mistake in the count, for function expand_last_few_input()" << endl;
    }
  }

  vector< vector< datatype > > return_weights;
  vector< datatype > return_biases;
  vector< datatype > buffer_vector_1(no_of_untouched_inputs + sum, 0);
  vector< datatype > zero_vector(no_of_untouched_inputs + sum, 0);
  vector< unsigned int > indices_to_switch_on(no_of_untouched_inputs + sum , 0);

  current_sum = 0;
  current_index = 0;
  i = 0;
  while(i < (no_of_untouched_inputs + sum) )
  {
    if(i < no_of_untouched_inputs)
    {
      indices_to_switch_on[i] = 1;
    }
    else
    {
      if( i == (current_sum + no_of_untouched_inputs))
      {
        indices_to_switch_on[i] = 1;
        current_sum += expansion_vector[current_index];
        current_index++;
      }
    }
    i++;
  }

  // cout << "Indices to switch on " << endl;
  // i = 0;
  // while(i < indices_to_switch_on.size())
  // {
  //   cout << indices_to_switch_on[i] << endl;
  //   i++;
  // }
  //

  i = 0;
  while(i < no_of_right_neurons)
  {
    current_sum = 0;
    current_index = 0;
    fill(buffer_vector_1.begin(), buffer_vector_1.end(), 0);
    j = 0;
    while(j < (no_of_untouched_inputs + sum) )
    {
      if(indices_to_switch_on[j])
      {
        if( j >= no_of_untouched_inputs)
        {
          buffer_vector_1[j] = weights[i][j - current_sum + current_index];
          current_sum += expansion_vector[current_index];
          j += expansion_vector[current_index];
          current_index++;
          continue;

        }
        else
        {
          buffer_vector_1[j] = weights[i][j];
        }
      }
      j++;
    }

    return_weights.push_back(buffer_vector_1);
    return_biases.push_back(biases[i]);
    i++;
  }

  weights = return_weights;
  biases = return_biases;

}
void patch_networks_vertically(
  vector< vector< vector< datatype > > >& main_weights,
  vector< vector< datatype > >& main_biases,
  vector< vector< vector< datatype > > > sub_network_weights,
  vector< vector< datatype > > sub_network_biases
)
{
  vector< vector< vector< datatype > > > result_network;
  vector< vector< datatype > > result_biases;

  unsigned int layers_in_sub_network , layers_in_main_network;
  unsigned int width_of_main_network, width_of_sub_network, no_of_inputs;
  unsigned int no_of_outputs_main_network, no_of_outputs_sub_network;

  layers_in_main_network = (main_weights).size() - 1;
  layers_in_sub_network = sub_network_weights.size() - 1;

  no_of_inputs = (main_weights[0][0]).size();
  no_of_outputs_main_network = (main_weights[layers_in_main_network]).size();
  no_of_outputs_sub_network = (sub_network_weights[layers_in_sub_network]).size();

  vector< unsigned int > main_network_configuration;
  deduce_network_configuration(main_weights, main_biases, main_network_configuration);
  vector< unsigned int > sub_network_configuration;
  deduce_network_configuration(sub_network_weights, sub_network_biases, sub_network_configuration);

  // Let's do some initial sanity checks on the inputs received

  if( layers_in_sub_network != layers_in_main_network )
  {
    cout << "Mismatch in no of layers in the function patch_networks_vertically() .. " << endl;
  }
  // Sanity check ends here


  unsigned int new_width, new_layers_count, new_output_count;
  new_layers_count = layers_in_main_network;

  unsigned int i , j , k;


  vector< datatype > buffer_vector_1(no_of_inputs);
  vector< vector< datatype > > buffer_weight_matrix;
  vector< datatype > buffer_bias_vector;

  // taking care of the input layer first
  buffer_weight_matrix.clear();
  buffer_bias_vector.clear();
  i = 0;
  while(i < (main_network_configuration[0] + sub_network_configuration[0]))
  {
    if(i < main_network_configuration[0])
    {
      buffer_vector_1 = (main_weights[0][i]);
      buffer_weight_matrix.push_back(buffer_vector_1);
      buffer_bias_vector.push_back(main_biases[0][i]);
    }
    else
    {
      buffer_vector_1 = (sub_network_weights[0][i - main_network_configuration[0]]);
      buffer_weight_matrix.push_back(buffer_vector_1);
      buffer_bias_vector.push_back(sub_network_biases[0][i - main_network_configuration[0]]);
    }
    i++;
  }

  result_network.push_back(buffer_weight_matrix);
  result_biases.push_back(buffer_bias_vector);


  vector< datatype > buffer_vector_2;
 // That is you need to look into inner layers if there are any...

    i = 1;
    while(i < (new_layers_count) )
    {
      buffer_weight_matrix.clear();
      buffer_bias_vector.clear();

      j = 0;
      while(j < (main_network_configuration[i] + sub_network_configuration[i]))
      {
        buffer_vector_2.clear();
        k = 0;
        while(k < (main_network_configuration[i-1] + sub_network_configuration[i-1]) )
        {
          if( (k < main_network_configuration[i-1]) && (j < main_network_configuration[i]) )
          {
            buffer_vector_2.push_back(main_weights[i][j][k]);
          }
          else if( (k >= main_network_configuration[i-1]) && (j >= main_network_configuration[i]) )
          {
            buffer_vector_2.push_back(sub_network_weights[i]
                                                         [j - main_network_configuration[i]]
                                                         [k - main_network_configuration[i-1]]
                                      );
          }
          else
          {
            buffer_vector_2.push_back(0);
          }
          k++;
        }

        if(j < main_network_configuration[i])
        {
          buffer_bias_vector.push_back(main_biases[i][j]);
        }
        else
        {
          buffer_bias_vector.push_back(sub_network_biases[i][j - main_network_configuration[i]]);
        }

        buffer_weight_matrix.push_back(buffer_vector_2);
        j++;
      }

      result_network.push_back(buffer_weight_matrix);
      result_biases.push_back(buffer_bias_vector);
      i++;
    }


    // The output, could have done this on the upper loop but just for the sake of clarity
    i = new_layers_count;

    buffer_weight_matrix.clear();
    buffer_bias_vector.clear();

    j = 0;
    while(j < (main_network_configuration[i] + sub_network_configuration[i]))
    {
      buffer_vector_2.clear();
      k = 0;
      while(k < (main_network_configuration[i-1] + sub_network_configuration[i-1]) )
      {
        if( (k < (main_network_configuration[i-1]) ) && (j < (main_network_configuration[i])) )
        {
          buffer_vector_2.push_back(main_weights[i][j][k]);
        }
        else if( (k >= main_network_configuration[i-1]) && (j >= main_network_configuration[i]) )
        {
          buffer_vector_2.push_back( sub_network_weights[i]
                                                        [j - main_network_configuration[i]]
                                                        [k - main_network_configuration[i-1]]
                                    );
        }
        else
        {
          buffer_vector_2.push_back(0);
        }
        k++;
      }

      if(j < main_network_configuration[i])
      {
        buffer_bias_vector.push_back(main_biases[i][j]);
      }
      else
      {
        buffer_bias_vector.push_back(sub_network_biases[i][j - main_network_configuration[i]]);
      }

      buffer_weight_matrix.push_back(buffer_vector_2);
      j++;
    }

    result_network.push_back(buffer_weight_matrix);
    result_biases.push_back(buffer_bias_vector);


  main_weights = result_network;
  main_biases = result_biases;

}

void write_network_to_file(
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  char * output_file_name
)
{
  unsigned int no_of_inputs, no_of_outputs, no_of_layers, no_of_neurons;
  unsigned int  i , j , k ;
  datatype data;
  no_of_layers = weights.size() - 1;
  no_of_neurons = (weights[0]).size();
  no_of_inputs = (weights[0][0]).size();
  no_of_outputs = (weights[no_of_layers]).size();

  vector< unsigned int > network_configuration;
  deduce_network_configuration(weights, biases, network_configuration);

  if(network_configuration[no_of_layers] != no_of_outputs)
  {
    cout << "No of outputs does not match with the info given in the weights" << endl;
  }

  ofstream output_file;
  output_file.open(output_file_name);

  output_file << no_of_inputs << "\n";
  output_file << no_of_outputs << "\n";
  output_file << no_of_layers << "\n";
  i = 0;
  while(i < no_of_layers)
  {
    no_of_neurons = network_configuration[i];
    output_file << no_of_neurons << "\n";
    i++;
  }



  // The input layer
  i = 0;
  while(i < network_configuration[0])
  {
    j = 0;
    while(j < no_of_inputs)
    {
      data = weights[0][i][j];
      output_file << data <<"\n" ;
      j++;
    }
    data = biases[0][i];
    output_file << data <<"\n" ;
    i++;
  }

   i = 1;
   while(i < no_of_layers)
   {
     j = 0;
     while(j < network_configuration[i])
     {
        k = 0;
        while(k < network_configuration[i-1])
        {
          data = weights[i][j][k] ;
          output_file << data << "\n";
          k++;
        }
        data = biases[i][j] ;
        output_file << data << "\n" ;
       j++;
     }
     i++;
   }


   i = 0;
   while(i < no_of_outputs)
   {
     j = 0;
     while(j < network_configuration[no_of_layers-1])
     {
       data = weights[no_of_layers][i][j] ;
       output_file << data << "\n" ;
       j++;
     }
     data = biases[no_of_layers][i];
     output_file << data << "\n" ;
     i++;
   }




   output_file.close();



}

void print_network_weights(
  vector< vector< vector< datatype > > > weights
)
{
  unsigned int i, j , k;
  cout << "Printing the network weights data : " << endl;
  i = 0;
  while(i < weights.size())
  {
    cout << "Layer number  = " << i << endl;
    j = 0;
    while(j < (weights[i]).size())
    {
      k = 0;
      while(k < (weights[i][j]).size())
      {
        cout << weights[i][j][k] << " " ;
        k++;
      }
      cout << endl;
      j++;
    }
    cout << endl << endl;
    i++;
  }
}
void print_network_biases(
  vector< vector< datatype > > biases
)
{
  unsigned int i, j , k;
  cout << "Printing the network biases data : " << endl;
  i = 0;
  while(i < biases.size())
  {
    cout << "Layer number  = " << i << endl;
    j = 0;
    while(j < (biases[i]).size())
    {
      cout << biases[i][j] << " " ;
      j++;
    }
    cout << endl << endl;
    i++;
  }
}
void adjust_offset_in_weights(
  vector< vector< datatype > >& weights,
  vector< datatype >& biases,
  datatype top_biases,
  datatype offset,
  datatype scaling_factor,
  unsigned int no_of_last_few_inputs
)
{
  unsigned int no_of_left_neurons, no_of_right_neurons;
  no_of_right_neurons = weights.size();
  no_of_left_neurons = (weights[0]).size();

  if( no_of_left_neurons < no_of_last_few_inputs  )
  {
    cout << "No of left neurons is less than the number " <<
     "of inputs to be adjusted for in  adjust_offset_in_weights()" << endl;
    exit(0);
  }

  unsigned int sum, i , j , k, no_of_untouched_inputs,
              current_sum, current_index;

  no_of_untouched_inputs = no_of_left_neurons - no_of_last_few_inputs;

  vector< unsigned int > indices_to_pay_attention(no_of_left_neurons,0);

  i = no_of_untouched_inputs;
  while(i < no_of_left_neurons)
  {
    indices_to_pay_attention[i] = 1;
    i++;
  }

  i = 0 ;
  while(i < no_of_right_neurons)
  {
    j = 0;
    while(j < no_of_left_neurons)
    {
      if(indices_to_pay_attention[j])
      {
        weights[i][j] *=  scaling_factor;
      }
      j++;
    }
    i++;
  }

  datatype adjust;
  i = 0 ;
  while(i < no_of_right_neurons)
  {
    adjust = 0;
    j = 0;
    while(j < no_of_left_neurons)
    {
      if(indices_to_pay_attention[j])
      {
        adjust += (weights[i][j] * offset);
      }
      else
      {
        adjust -= (weights[i][j] * top_biases);
      }
      j++;
    }

    biases[i] += adjust;
    i++;
  }



}

void adjust_offset(
  vector< datatype >& range,
  datatype offset
)
{
  unsigned int i, size;
  i = 0 ;
  size = range.size();
  while(i < size)
  {
    range[i] += offset;
    i++;
  }
}
void adjust_offset(
  datatype& output,
  datatype offset
)
{
  output += offset;
}

void adjust_offset(
  vector< vector< datatype > >& vec,
  vector< datatype> offset_amount
)
{
  unsigned int i, j , k, size;
  size = vec.size();
  if(size != offset_amount.size())
  {
    cout << "Size of vector and offset amount does not match " << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }

  i = 0;
  while(i < size)
  {
    vec[i][0] += offset_amount[i] ;
    vec[i][1] += offset_amount[i] ;
    i++;
  }


}

void add_directions_to_output(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >&  biases,
  vector< vector< datatype > > directions,
  datatype constr_offset,
  vector< datatype > offset_already
)
{
  unsigned int no_of_directions, i, j , k, no_of_inputs,
               no_of_layers, no_of_outputs;

  no_of_layers = weights.size();
  no_of_inputs = (weights[0][0]).size();
  no_of_directions = directions.size() + no_of_inputs;
  no_of_outputs = (weights[no_of_layers - 1]).size();

  if(no_of_inputs != no_of_outputs)
  {
    cout << "No of inputs not equal to no of outputs in the weights received, in add_directions_to_output()" << endl;
    cout << "Exiting .. "<< endl;
    exit(0);
  }
  if(!directions.size())
  {
    cout << "Directions vector is empty !! " << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }

  if((directions[0]).size() != no_of_inputs)
  {
    cout << "No of inputs in weights and directions do not match ,.." << endl;
    cout << "Exiting... " << endl;
    exit(0);
  }

  vector< vector< datatype > > buffer_weights_matrix;
  vector< datatype > buffer_weights_vector(no_of_outputs);
  vector< datatype > buffer_bias_vector;
  datatype bias;

  datatype adjust;
  // Adding the principal directions first
  i = 0;
  while(i < no_of_outputs)
  {
    fill(buffer_weights_vector.begin(), buffer_weights_vector.end(), 0);
    buffer_weights_vector[i] = 1;
    bias = 0;

    buffer_weights_matrix.push_back(buffer_weights_vector);
    buffer_bias_vector.push_back(bias);
    i++;
  }

  i = 0;
  while(i < directions.size())
  {

    adjust = 0;
    j = 0;
    while(j < no_of_outputs)
    {
      buffer_weights_vector[j] = directions[i][j];
      adjust += (directions[i][j] * offset_already[j]);
      j++;
    }
    bias = 0 + adjust - constr_offset;

    // cout << "adjust = " << adjust << endl;
    // cout << "constr offset = " << constr_offset << endl;
    // cout << "bias = " << bias << endl;

    buffer_weights_matrix.push_back(buffer_weights_vector);
    buffer_bias_vector.push_back(bias);

    i++;
  }

  weights.push_back(buffer_weights_matrix);
  biases.push_back(buffer_bias_vector);


  if((weights.size() != (no_of_layers + 1)) || (biases.size() != (no_of_layers + 1)))
  {
    cout << "Incorrect implementation in function add_directions_to_output()" << endl;
    cout << "Exiting ..." << endl;
    exit(0);
  }

  if((weights[no_of_layers]).size() != no_of_directions)
  {
    cout << "No of outputs does not match in add_directions_to_output() " << endl;
    cout << "Exiting ..." << endl;
    exit(0) ;
  }

}

void convert_direction_biases_to_constraints(
  vector< vector< datatype > > directions,
  vector< vector< datatype > > biases,
  vector< vector< datatype > >& constraints
)
{
  unsigned int no_of_directions, no_of_inputs, i , j ,k;

  no_of_directions = directions.size();
  if(no_of_directions != biases.size())
  {
    cout << "No of directions and biases do not match in " <<
    " convert_direction_biases_to_constraints()" << endl;
    cout << "No of directions = " << no_of_directions << endl;
    cout << "No of biases = " << biases.size() << endl;
    cout << "Exiting.... " << endl;
    exit(0);
  }

  if((biases[0]).size() != 2)
  {
    cout << "Biases do not follow the expected pattern " <<
    " convert_direction_biases_to_constraints()" <<  endl;
    cout << "Exiting.. " << endl;
    exit(0);
  }
  no_of_inputs = (directions[0]).size();
  constraints.clear();

  vector< datatype > constraint_vector(no_of_inputs + 1);

  i = 0;
  while(i < no_of_directions)
  {
    // Doing it for the lower limit
    j = 0;
    while(j < no_of_inputs)
    {
      constraint_vector[j] = directions[i][j];
      j++;
    }
    constraint_vector[j] = -biases[i][0]; // Since this is the lower limit
    constraints.push_back(constraint_vector);

    // Doing it for the upper limit
    j = 0;
    while(j < no_of_inputs)
    {
      constraint_vector[j] = -directions[i][j];
      j++;
    }
    constraint_vector[j] = biases[i][1]; // Since this is the upper limit
    constraints.push_back(constraint_vector);

    i++;
  }

}

void print_constraints(
  vector< vector< datatype > > constraints
)
{
  unsigned int no_of_constraints, i , j, no_of_inputs;
  no_of_inputs = (constraints[0]).size() - 1;
  no_of_constraints = constraints.size();
  i = 0;
  while(i < no_of_constraints)
  {
    j = 0;
    while(j < no_of_inputs)
    {
      cout << constraints[i][j] << " " ;
      j++;
    }
    cout << constraints[i][j] << " " ;
    cout << endl;
    i++;
  }

}

void normalize_vector(
  vector< datatype >& input_vector
)
{
  unsigned int i,j,k, size;
  size = input_vector.size();
  datatype sum = 0;
  i = 0;
  while(i < size)
  {
    sum += (input_vector[i] * input_vector[i]);
    i++;
  }

  sum = pow(sum, 0.5);
  i = 0;
  while(i < size)
  {
    input_vector[i] = (input_vector[i] / sum );
    i++;
  }



}

void patch_networks_horizontally(
  vector< vector< vector< datatype > > > network_1_weights,
  vector< vector< datatype > > network_1_biases,
  vector< datatype > network_1_scaling_factor,
  vector< datatype > network_1_offset,
  vector< unsigned int > important_outputs,
  vector< vector< vector< datatype > > > network_2_weights,
  vector< vector< datatype > > network_2_biases,
  vector< vector< vector< datatype > > >& network_3_weights,
  vector< vector< datatype > >& network_3_biases
)
{

  unsigned int network_1_outputs, network_2_inputs, i, j ,k,
               no_of_important_outputs, no_of_layers, index, network_2_outputs;

  no_of_layers = network_1_weights.size();
  network_1_outputs = (network_1_weights[no_of_layers - 1]).size();
  network_2_outputs = (network_2_weights[network_2_weights.size()-1]).size();
  if(important_outputs.size() != network_1_outputs)
  {
    cout <<"No of important outputs and no of elements in network_1_outputs" <<
     " do not match in patch_networks_horizontally() " << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }

  no_of_important_outputs = 0;
  i = 0;
  while(i < important_outputs.size())
  {
    if(important_outputs[i] == 1)
    {
      no_of_important_outputs++;
    }
    i++;
  }

  network_2_inputs = (network_2_weights[0][0]).size();
  if(network_2_inputs != no_of_important_outputs)
  {
    cout << "No of important outputs and no of inputs in the " <<
    " second weight layer do not match " << endl;
    cout << "No of imp output = " << no_of_important_outputs << endl;
    cout << "No of inputs in 2nd layer = " << network_2_inputs << endl;
    cout << "Exiting .. " << endl;
    exit(0);
  }

  vector< vector< datatype > > adhesive_weights;
  vector< datatype > adhesive_biases;
  vector< datatype > buffer_weights_vector(network_1_outputs,0);
  datatype bias, adjust;

  unsigned int no_of_neurons_network_2_first_hidden_layer;
  no_of_neurons_network_2_first_hidden_layer = (network_2_weights[0]).size();

    // Adjusting the scaling factor
  i = 0;
  while(i < no_of_neurons_network_2_first_hidden_layer)
  {
    j = 0;
    while(j < no_of_important_outputs)
    {
        network_2_weights[0][i][j] *= network_1_scaling_factor[j];
        j++;
    }
    i++;
  }

  index = 0;
  i = 0;
  while(i < no_of_neurons_network_2_first_hidden_layer)
  {
    adjust = 0;
    index = 0;
    j = 0;
    while(j < network_1_outputs)
    {
      if(important_outputs[j] == 1)
      {
        buffer_weights_vector[j] = network_2_weights[0][i][index];
        adjust += (buffer_weights_vector[j] * network_1_offset[j]);
        index++;
      }
      else
      {
        buffer_weights_vector[j] = 0;
      }
      j++;
    }
    bias = network_2_biases[0][i] + adjust;

    adhesive_weights.push_back(buffer_weights_vector);
    adhesive_biases.push_back(bias);
    i++;
  }

  if( (adhesive_weights.size() != no_of_neurons_network_2_first_hidden_layer)
          ||
      (adhesive_biases.size() != no_of_neurons_network_2_first_hidden_layer)

  )
  {
    cout << "Some problem 1 in execution in patch_networks_horizontally()" << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }

  // Creating the return network
  network_3_weights.clear();
  network_3_biases.clear();

  i = 0;
  while(i < network_1_weights.size())
  {
    network_3_weights.push_back(network_1_weights[i]);
    network_3_biases.push_back(network_1_biases[i]);
    i++;
  }

  network_3_weights.push_back(adhesive_weights);


  network_3_biases.push_back(adhesive_biases);



  i = 1;
  while(i < network_2_weights.size())
  {
    network_3_weights.push_back(network_2_weights[i]);
    network_3_biases.push_back(network_2_biases[i]);
    i++;
  }

  if(
    (network_3_weights.size() != (network_1_weights.size() + network_2_weights.size()))
    ||
    (network_3_biases.size() != (network_1_biases.size() + network_2_biases.size()))
    )
  {
    cout << "Some problem 2 in execution in patch_networks_horizontally()" << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }


}

void print_region(
  vector< vector< datatype > > region_constraints
)
{
  unsigned int size, no_of_inputs, i, j;
  size = region_constraints.size();
  if(!size)
  {
    cout << "No constraints received while printing " ;
    cout << "exiting... " << endl;
    exit(0);
  }
  no_of_inputs = (region_constraints[0]).size() - 1;
  cout << "Constraint for the region are : " << endl;
  cout << "Dimension = " << no_of_inputs << endl;

  i = 0;
  while(i < size)
  {
    cout << "\t" << i << " ----- ";
    j = 0;
    while(j < no_of_inputs)
    {
      cout << region_constraints[i][j] << " ";
      j++;
    }
    cout << region_constraints[i][j] << " ";
    cout << endl;
    i++;
  }

}

datatype sum_vector(vector < datatype > temp)
{
  unsigned int i;
  datatype sum = 0;
  i = 0;
  while(i < temp.size())
  {
    sum += temp[i];
    i++;
  }

  return sum;
}

void scale_vector(
  vector< datatype >& vector_input,
  vector< datatype > scale_factor
)
{
  unsigned int no_of_elements, i , j;
  no_of_elements = vector_input.size();
  if(!no_of_elements)
  {
    cout << "Input vector is empty, in scale_vector()" << endl;
    cout << "Exiting.. " << endl;
    exit(0);
  }
  if(no_of_elements != scale_factor.size())
  {
    cout << "Size of input vector and scale factor does not match " << endl;
    cout << "Scale factor size = " << scale_factor.size() << endl;
    cout << "Input vector size = " << vector_input.size() << endl;
    cout << "exiting.. " << endl;
    exit(0);
  }

  i = 0;
  while(i < no_of_elements)
  {
    vector_input[i] *= scale_factor[i];
    i++;
  }

}

void scale_vector(
  vector< vector< datatype > >& vector_input,
  vector< datatype > scale_factor
)
{
  unsigned int no_of_elements, i , j;
  no_of_elements = vector_input.size();
  if(!no_of_elements)
  {
    cout << "Input vector is empty, in scale_vector()" << endl;
    cout << "Exiting.. " << endl;
    exit(0);
  }
  if(no_of_elements != scale_factor.size())
  {
    cout << "Size of input vector and scale factor does not match " << endl;
    cout << "Scale factor size = " << scale_factor.size() << endl;
    cout << "Input vector size = " << vector_input.size() << endl;
    cout << "exiting.. " << endl;
    exit(0);
  }

  i = 0;
  while(i < no_of_elements)
  {
    vector_input[i][0] *= scale_factor[i];
    vector_input[i][1] *= scale_factor[i];
    i++;
  }

}
//
// void create_RK_network_from_differential_eq(
//   vector< string > file_names,
//   vector< datatype > scaling_factor,
//   vector< datatype > output_biases,
//   string return_file_name,
//   datatype network_offset,
//   datatype sampling_period
// )
// {
//   unsigned int dimension, i , j , k;
//   dimension = file_names.size();
//   if(!dimension)
//   {
//     cout << "No input file received IN create_RK_network_from_differential_eq().. " << endl;
//     cout << "Exiting ... " << endl;
//     exit(0);
//   }
//
//   datatype internal_bias = sherlock_parameters.int_bias_for_RK;
//
//   vector< vector< vector < vector < datatype > > > > all_weights;
//   vector< vector< vector< datatype > > > all_biases;
//
//   vector< vector< vector< datatype > > > weights;
//   vector< vector< datatype > > biases;
//
//   vector< vector< datatype > > weight_matrix;
//   vector< datatype > bias_vector;
//
//   i = 0;
//   while(i < dimesion)
//   {
//     char * c = new char[(file_names[i]).length() + 1];
//     strcpy(c, (file_names[i]).c_str());
//
//     network_handler network(c);
//     network.return_network_information(weights, biases);
//
//     all_weights.push_back(weights);
//     all_biases.push_back(biases);
//     i++;
//   }
//
//   if(all weights.size() != dimension)
//   {
//     cout << "Weights collection and input dimension does not match in" <<
//     " create_RK_network_from_differential_eq() .. " << endl;
//     cout << "Exiting ... " << endl;
//     exit(0);
//   }
//
//   vector< vector< vector< datatype > > > return_weights;
//   vector< vector< datatype > > return_biases;
//   vector< vector< vector < datatype > > > buffer_weights;
//   vector< vector< datatype > > buffer_biases;
//
//   // For the tapping of thing what we do is patch the network below
//   // with either the inputs or the different compositions
//
//   // Patch all the networks vertically using the function
//   patch_all_networks_vertically(all_weights, all_biases,
//                                 buffer_weights, buffer_biases);
//
//   // Add the extra layer at the bottom to tap off the inputs and add the bias
//   i = 0;
//   while(i < dimension)
//   {
//     increase_network_width_till_output_to_tap_off_nth_input(all_weights, all_biases,
//                                                             i, internal_bias);
//     i++;
//   }
//
//   // Create the adhesive layer and do all the offsets and scaling
//   // required
//   create_first_adhesive_layer_with_n_sets_at_the_bottom( dimension, 1, (0.5 * sampling_period) ,
//                                                          weight_matrix, bias_vector);
//
//
//   // Also add neurons to copy the last 'dimension' number of
//   // inputs from the previous layers and take care of the biases that were
//   // put in
//
//   // Now tap of the first dimension number of inputs from the previous
//   // layer and add it to the bottom of the adhesive layer , take care of the
//   // bias and add the bias in each of the neurons
//
//   // ******** First input layer complete ****************** //
//
//   // Add the adhesive layer to the network that is being built.
//
//   // Patch all the networks vertically using the function take care of the input
//   // layer of the main networks for adjusting the scaling and offsets added
//
//   // Add some extra layers at the bottom, for tapping of the
//   // last (2 * dimension) number of neurons
//
//   // Add this new network to the main network that you are building incrementatlly
//
//   // Create the adhesive layer and take care of all the offsets and scaling
//   // required
//
//   // Also add neurons to copy the last '2 * dimension' number of
//   // inputs from the previous layers
//
//   // Now tap of the first dimension number of inputs from the previous
//   // layer and add it to the bottom of the adhesive layer and add the offset
//
//   // ************************ 2nd layer complete **********************/
//
//   // Patch all the networks vertically, and take care of the input layers for
//   // adjusting the scaling and offsets involved.
//
//   // Add some extra layers at the bottom for tapping of the last (3 * dimension)
//   // number of neuron outputs.
//
//   // Create the adhesive layer and do all the offsets and scaling
//   // required
//
//   // Also add neurons to copy the last '3 * dimension' number of
//   // inputs from the previous layers
//
//   // Now tap of the first dimension number of inputs from the previous
//   // layer and add it to the bottom of the adhesive layer and giving appropriate
//   // offset.
//
//   // ********************** 3rd Layers complete ***********************/
//
//   // Patch all the networks vertically, and take care of the input layers for
//   // adjusting the scaling and offsets involved.
//
//   // Add some extra layers at the bottom for tapping of the last (4 * dimension)
//   // number of neuron outputs.
//
//   // Add this new network to the main network that you are building incrementatlly
//
//
//
//
//   // Create the adhesive layer by taking a scaled input from the appropriate neurons
//
//   // Also add neurons for tapping of the inputs
//
//   // In the last layers do the final computation
//
//
//
// }
//
// void patch_all_networks_vertically(
//   vector< vector< vector< vector< datatype > > > > all_weights,
//   vector< vector< vector< biases > > > all_biases,
//   vector< vector< vector< datatype > > >& return_weights,
//   vector< vectror < datatype > >& return_biases
// )
// {
//   return_weights.clear();
//   return_biases.clear();
//
//   unsigned int i, no_of_weight_sets;
//   no_of_weight_sets = all_weights.size();
//
//   if(all_weights.size() != all_biases.size())
//   {
//     cout << "No of weight sets and No of biases sets does not match " << endl;
//     cout << "No of weights sets  = " << all_weights.size() << endl;
//     cout << "No of biases sets = " << all_biases.size() << endl;
//     cout << "Exiting ... " << endl;
//     exit(0);
//   }
//
//   return_weights = all_weights[0];
//   return_biases = all_biases[0];
//   i = 1;
//   while(i < no_of_weight_sets)
//   {
//     patch_networks_vertically(return_weights,
//                               return_biases,
//                               all_weights[i],
//                               all_biases[i]);
//
//     i++;
//   }
//
// }
//
// void increase_network_width_till_output_to_tap_off_nth_input(
//   vector< vector < vector< datatype > > >& weights,
//   vector< vector< datatype > >& biases,
//   unsigned int input_number,
//   datatype bias_to_be_added
// )
// {
//   unsigned int  i, j , k, no_of_inputs;
//   vector< unsigned int > network_structure;
//   if((!weights.size()) || (!biases.size()))
//   {
//     cout << "Weights or biases matrix is empty()" << endl;
//     cout << "Size of weights matrix = " << weights.size() << endl;
//     cout << "Size of biases matrix = " << biases.size() << endl;
//     cout << "Exiting ... " << endl;
//     exit(0);
//   }
//
//   no_of_inputs = (weights[0][0]).size();
//   deduce_network_configuration(weights, biases, network_structure);
//   if(input_number > no_of_inputs)
//   {
//     cout << "Input number received is more than the number of inputs the network " <<
//     "expects in increase_network_width_till_output_to_tap_off_nth_input()" << endl;
//     cout << "Input number = " << input_number << endl;
//     cout << "No of inputs = " << no_of_inputs << endl;
//     cout << "Exiting .. " << endl;
//     exit(0);
//   }
//   vector< datatype > buffer_weight_vector;
//   vector< vector< datatype > > buffer_weight_matrix;
//   datatype buffer_bias;
//   datatype buffer_bias_vector;
//
//   i = 0;
//   while(i < weights.size())
//   {
//     buffer_weight_matrix = weights[i];
//     buffer_bias_vector = biases[i];
//
//     if(!i)
//     {
//         buffer_weight_vector.clear();
//         k = 0;
//         while(k < no_of_inputs)
//         {
//           buffer_weight_vector.push_back(0.0);
//           k++;
//         }
//         buffer_weight_vector[input_number] = 1.0;
//         buffer_bias = bias_to_be_added;
//
//         buffer_weight_matrix.push_back(buffer_weight_vector);
//         buffer_bias_vector.push_back(buffer_bias);
//     }
//     else
//     {
//       buffer_weight_vector.clear();
//       k = 0;
//       while(k < network_structure[i-1])
//       {
//         buffer_weight_vector.push_back(0.0);
//         k++;
//       }
//       buffer_weight_vector.push_back(1.0);
//       buffer_bias = 0.0;
//
//       buffer_weight_matrix.push_back(buffer_weight_vector);
//       buffer_bias_vector.push_back(buffer_bias);
//     }
//
//     weights[i] = buffer_weight_matrix;
//     biases[i] = buffer_bias_vector;
//     i++;
//   }
//
//
// }
//
//   // START CODING HERE CHECK THE FOLLOWING CODE
//
//
// create_first_adhesive_layer_with_n_sets_at_the_bottom(
//  unsigned int dimension,
//  unsigned no_of_sets,
//  vector< datatype > tap_off_neuron_offset,
//  vector< datatype > network_output_offset,
//  datatype factor,
//  vector< vector< datatype > >& weight_matrix,
//  vector< datatype >& bias_vector,
//  datatype bias_to_set
// )
// {
//   weight_matrix.clear();
//   bias_vector.clear();
//
//   if((tap_off_neuron_bias.size() != dimension) || (network_output_bias.size() != dimension))
//   {
//     cout << "Size mismatch in create_first_adhesive_layer_with_n_sets_at_the_bottom()" << endl;
//     cout << "No_of_tap_of_neuron_bias = " << tap_off_neuron_bias.size() << endl;
//     cout << "No of network_output_bias = " << network_output_offset.size() << endl;
//     cout << " Dimension received  = " << dimension << endl;
//     cout << "Exiting ... " << endl;
//     exit(0);
//   }
//
//   unsigned int i, j, k, count;
//   vector< datatype > buffer_weight_vector(dimension * (no_of_sets) + dimension);
//   datatype bias, adjust;
//   unsigned int no_of_output_neurons = dimension * (1 + no_of_sets) + dimension;
//   unsigned int no_of_input_neurons = (dimension * (no_of_sets) + dimension);
//
//   i = 0;
//   while(i < no_of_output_neurons)
//   {
//     adjust = 0;
//     if(i < (dimension * (1 + no_of_sets)) )
//     {
//       count = 0;
//       j = 0 ;
//       while(j < dimension)
//       {
//         if(i == j)
//         {
//           buffer_weight_vector[count] = factor;
//           adjust += (network_output_offset[j] * factor) ;
//         }
//         else
//         {
//           buffer_weight_vector[count] = 0.0;
//         }
//
//         count++;
//         j++;
//       }
//
//       k = 0;
//       while(k < no_of_sets)
//       {
//         j = 0 ;
//         while(j < dimension)
//         {
//           if(i == j)
//           {
//             buffer_weight_vector[count] = 1.0;
//             adjust += (tap_off_neuron_offset[j] * 1) ;
//           }
//           else
//           {
//             buffer_weight_vector[count] = 0.0;
//           }
//           count++;
//           j++;
//         }
//
//         k++;
//       }
//     }
//     else
//     {
//       count = 0;
//       j = 0 ;
//       while(j < dimension)
//       {
//         if((i - (dimension * (1+no_of_sets)) ) == j)
//         {
//           buffer_weight_vector[count] = 1.0;
//           adjust += (network_output_offset[j] * 1) ;
//         }
//         else
//         {
//           buffer_weight_vector[count] = 0.0;
//         }
//         count++;
//         j++;
//       }
//
//       k = 0;
//       while(k < no_of_sets)
//       {
//         j = 0 ;
//         while(j < dimension)
//         {
//           buffer_weight_vector[count] = 0.0;
//           count++;
//           j++;
//         }
//         k++;
//       }
//
//     }
//     bias = (-adjust + bias_to_set) ;
//     weight_matrix.push_back(buffer_weight_vector);
//     bias_vector.push_back(bias);
//     i++;
//   }
//
// }


void compute_M_values_with_interval_propagation(
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  vector< vector< datatype > > input_interval,
  vector< vector< datatype > >& M_values
)
{
  unsigned int i, j , k, no_of_inputs, no_of_hidden_layers;
  no_of_inputs = weights[0][0].size();
  vector < unsigned int > network_configuration;
  vector< unsigned int > buffer;
  vector< datatype > weights_vector;
  datatype bias;
  vector < vector< datatype > > last_layer_input_interval;
  vector < vector< datatype > > next_layer_input_interval;

  deduce_network_configuration(weights, biases, buffer);
  network_configuration.push_back(no_of_inputs);
  i = 0;
  while(i < buffer.size())
  {
    network_configuration.push_back(buffer[i]);
    i++;
  }

  // So the network configuration now includes the input layer as well, MIND IT !!

  M_values.clear();
  no_of_hidden_layers = network_configuration.size() - 2;
  vector< datatype > M_values_for_a_layer;
  vector< datatype > M_values_for_a_neuron;

  i = 1;
  while(i < no_of_hidden_layers + 2)
  {
    M_values_for_a_layer.clear();
    next_layer_input_interval.clear();
    j = 0;
    while(j < network_configuration[i])
    {
      weights_vector.clear();
      weights_vector = weights[i-1][j];
      bias = biases[i-1][j];

      if( i == 1)
      {
        last_layer_input_interval = input_interval;
        compute_interval_for_linear_input_combination(weights_vector, bias,
          last_layer_input_interval, M_values_for_a_neuron);
      }
      else
      {
        compute_interval_for_linear_input_combination(weights_vector, bias,
          last_layer_input_interval, M_values_for_a_neuron);
      }
      next_layer_input_interval.push_back(M_values_for_a_neuron);
      M_values_for_a_layer.push_back(compute_max_abs_in_a_vector(M_values_for_a_neuron));
      j++;
    }
    last_layer_input_interval = next_layer_input_interval;
    M_values.push_back(M_values_for_a_layer);
    i++;
  }



}

void compute_interval_for_linear_input_combination(
  vector< datatype > weights_vector,
  datatype bias_term,
  vector< vector< datatype > > interval_input,
  vector< datatype >& interval_output
)
{
    unsigned int i,j,k, no_of_inputs;
    no_of_inputs = weights_vector.size();
    if(interval_input.size() != no_of_inputs)
    {
      cout << "Weights vector size and interval input size does not match in " <<
       "compute_interval_for_linear_input_combination()" << endl;
       cout << "Weights vector size = " << no_of_inputs << endl;
       cout << "interval input size = " << interval_input.size() << endl;
       cout << "Exiting... " << endl;
       exit(0);
    }

    if(interval_input[0].size()!= 2)
    {
      cout << "Interval input is in wrong shape !"  << endl;
      cout << "EXITING ... " << endl;
      exit(0);
    }

    interval_output.clear();
    datatype max, min;
    max = 0.0;
    min = 0.0;


    i = 0 ;
    while(i < no_of_inputs)
    {
      if(weights_vector[i] > 0)
      {
          max += (interval_input[i][1] * weights_vector[i]);
          min += (interval_input[i][0] * weights_vector[i]);
      }
      else if (weights_vector[i] < 0)
      {
        max += (interval_input[i][0] * weights_vector[i]);
        min += (interval_input[i][1] * weights_vector[i]);
      }
      i++;
    }

    max += bias_term;
    min += bias_term;

    interval_output.push_back(min);
    interval_output.push_back(max);

}

void print_2D_vector(
  vector< vector< datatype > > data
)
{
  unsigned int i, j , k, no_of_rows, no_of_columns;
  no_of_rows = data.size();

  i = 0;
  while(i < no_of_rows)
  {
    cout << "Row " << i << " = " ;
    j = 0;
    no_of_columns = data[i].size();
    while(j < no_of_columns)
    {
      cout << data[i][j] << "  " ;
      j++;
    }
    cout << endl;
    i++;
  }
}

void save_2D_vector_to_file(
  vector< vector< datatype > > data,
  char * file_name
)
{
  unsigned int i, j , k, no_of_rows, no_of_columns;
  no_of_rows = data.size();

  ofstream file;
  file.open(file_name);

  file << no_of_rows << "\n" ;
  i = 0;
  while(i < no_of_rows)
  {
    no_of_columns = data[i].size();
    file << no_of_columns << "\n";
    i++;
  }


  i = 0;
  while(i < no_of_rows)
  {
    j = 0;
    no_of_columns = data[i].size();
    while(j < no_of_columns)
    {
      file << data[i][j] << "\n" ;
      j++;
    }
    i++;
  }

  file.close();
}

void read_2D_vector_from_file(
  char * file_name,
  vector< vector < datatype > >& data
)
{
  unsigned int i, j , k, no_of_layers;
  vector< unsigned int > no_of_neurons_each_layer;

  ifstream file;
  file.open(file_name);

  datatype buffer;
  file >> buffer;
  no_of_layers = (unsigned int)buffer;

  i = 0;
  while(i < no_of_layers)
  {
    file >> buffer;
    no_of_neurons_each_layer.push_back((unsigned int)buffer);
    i++;
  }

  data.clear();
  vector< datatype > data_buffer_vector;

  i = 0;
  while(i < no_of_layers)
  {
    data_buffer_vector.clear();
    j = 0;
    while(j < no_of_neurons_each_layer[i])
    {
      file >> buffer;
      data_buffer_vector.push_back(buffer);
      j++;
    }
    data.push_back(data_buffer_vector);
    i++;
  }

  file.close();
}
datatype compute_max_abs_in_a_vector(
  vector< datatype > input_vector
)
{
  datatype max = 0; // Because of ReLU
  unsigned int i;
  i = 0;
  while(i < input_vector.size())
  {
    if(max < fabs(input_vector[i]) )
    {
      max = fabs(input_vector[i]);
    }
    i++;
  }
  return max;
}

void save_1D_vector_to_file(
  vector< datatype > data,
  char * file_name
)
{
  unsigned int i, no_of_elements;
  no_of_elements = data.size();

  ofstream file;
  file.open(file_name);

  i = 0;
  while(i < no_of_elements)
  {
    file << data[i] << "\n" ;
    i++;
  }

  file.close();
}
void form_interval_from_region_constraints(
  vector< vector< datatype > > region_constr,
  vector< vector< datatype > >& interval
)
{
  if(region_constr.size() == 0)
  {
    cout << "No region constrain received, exiting... " << endl;
    exit(0);
  }
  unsigned int no_of_inputs;
  no_of_inputs = region_constr[0].size() - 1;

  vector< int > direction(no_of_inputs);
  interval.clear();

  unsigned int i, j , k;
  vector< datatype > max(no_of_inputs,0.0);
  vector< datatype > min(no_of_inputs,0.0);

  i = 0;
  while(i < no_of_inputs)
  {
    fill(direction.begin(), direction.end(), 0);

    direction[i] = 1;
    find_size_of_enclosed_region_in_direction(region_constr, direction, max[i]);

    direction[i] = -1;
    find_size_of_enclosed_region_in_direction(region_constr, direction, min[i]);

    i++;
  }

  interval.push_back(max);
  interval.push_back(min);

}
void collect_all_reach_sets_for_the_time_stamp(
  unsigned int time_stamp,
  vector< set_info > time_stamped_reach_sets,
  vector< vector< vector< datatype > > >& returned_reach_sets
)
{
  returned_reach_sets.clear();
  unsigned int i, size;
  size = time_stamped_reach_sets.size();
  i = 0;
  while(i < size)
  {
    if( time_stamped_reach_sets[i].time_stamp == time_stamp)
    {
      returned_reach_sets.push_back(time_stamped_reach_sets[i].region_constr);
    }
    i++;
  }
}
plotting_data :: plotting_data (unsigned int dim)
{
  dimension = dim;
  reach_set_time_range = 0;
  system_trace_time_range = -1;
  no_of_system_traces = 0;
  reach_sets.clear();
  system_traces.clear();
}

void plotting_data :: add_reach_set(vector< vector< datatype > > current_reach_set)
{
  int received_set_dim = current_reach_set[0].size();
  if(received_set_dim != dimension)
  {
    cout << "In the plotting function the initial dimension set and the dimension of the reach set " <<
    "received does not match .. " << endl;
    cout << "Initial dimension set = " << dimension << endl;
    cout << "Dimension of the reach set received  = " << received_set_dim << endl;
    cout << "Exiting .. Sorry ! " << endl;
    exit(0);
  }

  if(current_reach_set.size() != 2)
  {
    cout << "More than the 2 elements for the 0th dimension in one of the reach sets .. " << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }

  reach_sets.push_back(current_reach_set);
  reach_set_time_range++;
}
void plotting_data :: add_system_trace(vector< vector< datatype > > system_trace)
{
  int received_point_dim = system_trace[0].size();
  if(received_point_dim != dimension)
  {
    cout << "In the plotting function the initial dimension set and the dimension of the reach set " <<
    "received does not match .. " << endl;
    cout << "Initial dimension set = " << dimension << endl;
    cout << "Dimension of the reach set received  = " << received_point_dim << endl;
    cout << "Exiting .. Sorry ! " << endl;
    exit(0);
  }

  system_traces.push_back(system_trace);
  system_trace_time_range = system_trace.size();


  no_of_system_traces++;
}

void plotting_data :: collect_and_merge_reach_sets(vector< set_info > time_stamped_reach_sets)
{
  // Finding the maximum time stamps
  unsigned int max_time_stamp, no_of_reach_sets_received;
  unsigned int i,j,k, no_of_inputs;
  no_of_inputs = time_stamped_reach_sets[0].region_constr[0].size()-1;
  vector< datatype > abs_max_vector(no_of_inputs), abs_min_vector(no_of_inputs);
  vector< datatype > current_max_vector(no_of_inputs), current_min_vector(no_of_inputs);
  vector< vector< datatype > > box;

  no_of_reach_sets_received = time_stamped_reach_sets.size();

  // Finding the maximum time stamp in the set
  max_time_stamp = 0;
  i = 0;
  while(i < no_of_reach_sets_received)
  {
    if(time_stamped_reach_sets[i].time_stamp > max_time_stamp)
    {
      max_time_stamp = time_stamped_reach_sets[i].time_stamp;
    }
    i++;
  }

  reach_set_time_range = max_time_stamp;

  vector< vector< vector< datatype > > > reach_sets_for_time_stamp;
  reach_sets.clear();
  i = 0;
  while(i <= max_time_stamp)
  {
    collect_all_reach_sets_for_the_time_stamp(i, time_stamped_reach_sets, reach_sets_for_time_stamp);

    // Compute the interval for each of those sets
    // Merge all those reach sets, by doing a simple max over the maxes and
    //  doing a simple min over all the mins

    fill(abs_max_vector.begin(), abs_max_vector.end(), -1e30);
    fill(abs_min_vector.begin(), abs_min_vector.end(), 1e30);
    j = 0;
    while(j < reach_sets_for_time_stamp.size())
    {
      form_interval_from_region_constraints(reach_sets_for_time_stamp[j], box);
      current_max_vector = box[0];
      current_min_vector = box[1];

      k = 0;
      while(k < no_of_inputs)
      {
        if(current_max_vector[k] > abs_max_vector[k])
        {
          abs_max_vector[k] = current_max_vector[k];
        }
        if(current_min_vector[k] < abs_min_vector[k])
        {
          abs_min_vector[k] = current_min_vector[k];
        }
        k++;
      }
      j++;
    }

    // Save the reach sets in the data structure 'reach_sets'
    box[0] = abs_max_vector;
    box[1] = abs_min_vector;

    reach_sets.push_back(box);
    i++;
  }

}


void plotting_data :: plot(int mode)
/* 1 for just the trace ,
   2 for just the reach sets,
   3 for overlaying the plots
*/
{
  string trace_plots_color = " \'black\' ";
  string reach_sets_color = " \'cyan\' ";
  string font_size = "10";
  string font_style = " \'serif\' ";
  string trace_plots_name = " \'Sherlock_System_Traces\' ";
  int Max_No_Y_ticks = 4;


  if(mode == 1)
  {
    char filename[] = "./headers/system_trace_data.py";
    ofstream file;
    file.open(filename);

    string buff, buff_1;
    string line ;
    line = "import matplotlib.pyplot as plt";
    file << line << "\n";
    line = "from matplotlib.ticker import MaxNLocator";
    file << line << "\n";

    line = "time_scale = []";
    file << line << "\n";
    int i, j, k;
    // Generating the time stamps :
    i = 0 ;
    while(i < system_trace_time_range)
    {
      string line_1 = "time_scale.append(";
      string line_2 = to_string(i);
      string line_3 = ")";
      line = line_1 + line_2 + line_3;
      file << line << "\n";
      i++;
    }



    // Generating the plots for each system trace
    unsigned int trace_index;
    trace_index = 0;
    while(trace_index < no_of_system_traces)
    {
      vector< vector< datatype > > single_trace = system_traces[trace_index];
      // Preparing for a single trace
      string plot_name = "y_";
      string trace_no = to_string(trace_index);
      plot_name = plot_name + trace_no;

      j = 0;
      while(j < dimension)
      {
        string plot_buff = plot_name;
        string dim_tag = to_string(j);
        buff = "_";
        plot_buff = plot_name + buff + dim_tag;
        buff  = " = []";
        line  = plot_buff + buff;
        file << line << "\n";
        k = 0;
        while(k < system_trace_time_range)
        {
          buff = ".append(";
          line = plot_buff + buff;
          buff = to_string(single_trace[k][j]);
          line = line + buff;
          buff = ")";
          line = line + buff;
          file << line << "\n";
          k++;
        }
        j++;
      }
      trace_index++;
    }


    // Preparing to plot

    line = "";
    buff = "fig, (";
    line = line + buff;
    j = 0;
    while (j < dimension)
    {
      buff_1 = to_string(j);
      buff = "ax_";
      buff = buff + buff_1;
      if(j != (dimension - 1))
      {
        buff_1 = ",";
        buff = buff + buff_1;
      }
      line = line + buff;
      j++;
    }

    buff = ") = plt.subplots( ";
    line = line + buff;
    buff = to_string(dimension);
    line = line + buff;
    buff = " , 1 , sharex = True)" ;
    line = line + buff;

    file << line << "\n";

    j = 0;
    while(j < dimension)
    {
      k = 0;
      while(k < no_of_system_traces)
      {
        line = "";
        buff = "ax_";
        line = line + buff;
        buff = to_string(j);
        line = line + buff;
        buff = ".plot(time_scale, ";
        line = line + buff;
        buff = "y_";
        line = line + buff;
        buff = to_string(k);
        line = line + buff;
        buff = "_";
        line = line + buff;
        buff = to_string(j);
        line = line + buff;
        buff = " , color = ";
        line = line + buff;
        buff = trace_plots_color;
        line = line + buff;
        buff = " ) ";
        line = line + buff;
        file << line << "\n";
        k++;
      }

      // Setting y_label
      line = "";
      buff = "ax_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ".set_ylabel( \' $ x_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = " $";
      line = line + buff;
      buff = " \' ";
      line = line + buff;
      buff = ", family = ";
      buff = buff + font_style;
      line = line + buff;
      buff ="  , fontsize = ";
      line = line + buff;
      line = line + font_size;
      buff = " + 10 )";
      line = line + buff;
      file << line << "\n";

      // Setting x_label
      line = "";
      buff = "ax_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ".set_xlabel('Time Steps' ";
      line = line + buff;
      buff = ", family = ";
      buff = buff + font_style;
      line = line + buff;
      buff = " , fontsize = ";
      line = line + buff;
      line = line + font_size;
      buff = ")";
      line = line + buff;
      file << line << "\n";

      // Setting max number of y_ticks
      // ax_0.yaxis.set_major_locator(MaxNLocator(4))
      line = "";
      buff = "ax_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ".yaxis.set_major_locator(MaxNLocator(";
      line = line + buff;
      buff = to_string(Max_No_Y_ticks);
      line = line + buff;
      buff = "))";
      line = line + buff;
      file << line << "\n";

      j++;
    }

    line = "";
    buff = "plt.savefig( ";
    line = line + buff;
    buff = trace_plots_name;
    line = line + buff;
    buff = ")";
    line = line + buff;

    file << line << "\n";

    file.close();

    system("python ./headers/system_trace_data.py");
    system("rm ./headers/system_trace_data.py");
  }
  else if (mode == 2)
  {
    char filename[] = "./headers/system_sets_data.py";
    ofstream file;
    file.open(filename);

    string buff, buff_1;
    string line ;
    line = "import matplotlib.pyplot as plt";
    file << line << "\n";
    line = "from matplotlib.ticker import MaxNLocator";
    file << line << "\n";

    line = "time_scale = []";
    file << line << "\n";
    int i, j, k;
    // Generating the time stamps :
    i = 0 ;
    while(i <= reach_set_time_range)
    {
      string line_1 = "time_scale.append(";
      string line_2 = to_string(i);
      string line_3 = ")";
      line = line_1 + line_2 + line_3;
      file << line << "\n";
      i++;
    }


    // Generating the plots for the limits

    vector< vector< datatype > > system_reach_set ;

    // Preparing for just the upper limit

    string plot_name = "y_upper";
    j = 0;
    while(j < dimension)
    {
      string plot_buff = plot_name;
      string dim_tag = to_string(j);
      buff = "_";
      plot_buff = plot_name + buff + dim_tag;
      buff  = " = []";
      line  = plot_buff + buff;
      file << line << "\n";

      k = 0;
      while(k <= reach_set_time_range)
      {
        system_reach_set = reach_sets[k];
        datatype upper_lim = system_reach_set[0][j];

        buff = ".append(";
        line = plot_buff + buff;
        buff = to_string(upper_lim);
        line = line + buff;
        buff = ")";
        line = line + buff;
        file << line << "\n";
        k++;
      }
      j++;
    }

    // Preparing for just the lower limit

    plot_name = "y_lower";
    j = 0;
    while(j < dimension)
    {
      string plot_buff = plot_name;
      string dim_tag = to_string(j);
      buff = "_";
      plot_buff = plot_name + buff + dim_tag;
      buff  = " = []";
      line  = plot_buff + buff;
      file << line << "\n";

      k = 0;
      while(k <= reach_set_time_range)
      {
        system_reach_set = reach_sets[k];
        datatype lower_lim = system_reach_set[1][j];

        buff = ".append(";
        line = plot_buff + buff;
        buff = to_string(lower_lim);
        line = line + buff;
        buff = ")";
        line = line + buff;
        file << line << "\n";
        k++;
      }
      j++;
    }

    // Preparing to plot

    line = "";
    buff = "fig, (";
    line = line + buff;
    j = 0;
    while (j < dimension)
    {
      buff_1 = to_string(j);
      buff = "ax_";
      buff = buff + buff_1;
      if(j != (dimension - 1))
      {
        buff_1 = ",";
        buff = buff + buff_1;
      }
      line = line + buff;
      j++;
    }

    buff = ") = plt.subplots( ";
    line = line + buff;
    buff = to_string(dimension);
    line = line + buff;
    buff = " , 1 , sharex = True)" ;
    line = line + buff;

    file << line << "\n";

    j = 0;
    while(j < dimension)
    {

      line = "";
      buff = "ax_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ".fill_between(time_scale, ";
      line = line + buff;
      buff = "y_lower_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ", y_upper_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = " , color = ";
      line = line + buff;
      buff = reach_sets_color;
      line = line + buff;
      buff = " ) ";
      line = line + buff;
      file << line << "\n";


      // Setting y_label
      line = "";
      buff = "ax_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ".set_ylabel( \' $ x_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = " $";
      line = line + buff;
      buff = " \' ";
      line = line + buff;
      buff = ", family = ";
      buff = buff + font_style;
      line = line + buff;
      buff ="  , fontsize = ";
      line = line + buff;
      line = line + font_size;
      buff = " + 10 )";
      line = line + buff;
      file << line << "\n";

      // Setting x_label
      line = "";
      buff = "ax_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ".set_xlabel('Time Steps' ";
      line = line + buff;
      buff = ", family = ";
      buff = buff + font_style;
      line = line + buff;
      buff = " , fontsize = ";
      line = line + buff;
      line = line + font_size;
      buff = ")";
      line = line + buff;
      file << line << "\n";

      // Setting max number of y_ticks
      // ax_0.yaxis.set_major_locator(MaxNLocator(4))
      line = "";
      buff = "ax_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ".yaxis.set_major_locator(MaxNLocator(";
      line = line + buff;
      buff = to_string(Max_No_Y_ticks);
      line = line + buff;
      buff = "))";
      line = line + buff;
      file << line << "\n";

      j++;
    }

    line = "";
    buff = "plt.savefig( ";
    line = line + buff;
    buff = trace_plots_name;
    line = line + buff;
    buff = ")";
    line = line + buff;

    file << line << "\n";

    file.close();

    system("python ./headers/system_sets_data.py");
    system("rm ./headers/system_sets_data.py");

  }
  else if(mode == 3)
  {

    char filename[] = "./headers/system_sets_data.py";
    ofstream file;
    file.open(filename);

    string buff, buff_1;
    string line ;
    line = "import matplotlib.pyplot as plt";
    file << line << "\n";
    line = "from matplotlib.ticker import MaxNLocator";
    file << line << "\n";

    line = "time_scale = []";
    file << line << "\n";
    int i, j, k;
    // Generating the time stamps :
    i = 0 ;
    while(i <= reach_set_time_range)
    {
      string line_1 = "time_scale.append(";
      string line_2 = to_string(i);
      string line_3 = ")";
      line = line_1 + line_2 + line_3;
      file << line << "\n";
      i++;
    }

    ///////////////// Generating the data for the traces ///////////

    // Generating the plots for each system trace
    unsigned int trace_index;
    trace_index = 0;
    while(trace_index < no_of_system_traces)
    {
      vector< vector< datatype > > single_trace = system_traces[trace_index];
      // Preparing for a single trace
      string plot_name = "y_";
      string trace_no = to_string(trace_index);
      plot_name = plot_name + trace_no;

      j = 0;
      while(j < dimension)
      {
        string plot_buff = plot_name;
        string dim_tag = to_string(j);
        buff = "_";
        plot_buff = plot_name + buff + dim_tag;
        buff  = " = []";
        line  = plot_buff + buff;
        file << line << "\n";
        k = 0;
        while(k < system_trace_time_range)
        {
          buff = ".append(";
          line = plot_buff + buff;
          buff = to_string(single_trace[k][j]);
          line = line + buff;
          buff = ")";
          line = line + buff;
          file << line << "\n";
          k++;
        }
        j++;
      }
      trace_index++;
    }

    /////////////// End of generation of data for the traces ////

    // Generating the plots for the limits

    vector< vector< datatype > > system_reach_set ;

    // Preparing for just the upper limit

    string plot_name = "y_upper";
    j = 0;
    while(j < dimension)
    {
      string plot_buff = plot_name;
      string dim_tag = to_string(j);
      buff = "_";
      plot_buff = plot_name + buff + dim_tag;
      buff  = " = []";
      line  = plot_buff + buff;
      file << line << "\n";

      k = 0;
      while(k <= reach_set_time_range)
      {
        system_reach_set = reach_sets[k];
        datatype upper_lim = system_reach_set[0][j];

        buff = ".append(";
        line = plot_buff + buff;
        buff = to_string(upper_lim);
        line = line + buff;
        buff = ")";
        line = line + buff;
        file << line << "\n";
        k++;
      }
      j++;
    }

    // Preparing for just the lower limit

    plot_name = "y_lower";
    j = 0;
    while(j < dimension)
    {
      string plot_buff = plot_name;
      string dim_tag = to_string(j);
      buff = "_";
      plot_buff = plot_name + buff + dim_tag;
      buff  = " = []";
      line  = plot_buff + buff;
      file << line << "\n";

      k = 0;
      while(k <= reach_set_time_range)
      {
        system_reach_set = reach_sets[k];
        datatype lower_lim = system_reach_set[1][j];

        buff = ".append(";
        line = plot_buff + buff;
        buff = to_string(lower_lim);
        line = line + buff;
        buff = ")";
        line = line + buff;
        file << line << "\n";
        k++;
      }
      j++;
    }

    // Preparing to plot

    line = "";
    buff = "fig, (";
    line = line + buff;
    j = 0;
    while (j < dimension)
    {
      buff_1 = to_string(j);
      buff = "ax_";
      buff = buff + buff_1;
      if(j != (dimension - 1))
      {
        buff_1 = ",";
        buff = buff + buff_1;
      }
      line = line + buff;
      j++;
    }

    buff = ") = plt.subplots( ";
    line = line + buff;
    buff = to_string(dimension);
    line = line + buff;
    buff = " , 1 , sharex = True)" ;
    line = line + buff;

    file << line << "\n";

    j = 0;
    while(j < dimension)
    {

      ////////  System Traces Part starts here ///////////////
      k = 0;
      while(k < no_of_system_traces)
      {
        line = "";
        buff = "ax_";
        line = line + buff;
        buff = to_string(j);
        line = line + buff;
        buff = ".plot(time_scale, ";
        line = line + buff;
        buff = "y_";
        line = line + buff;
        buff = to_string(k);
        line = line + buff;
        buff = "_";
        line = line + buff;
        buff = to_string(j);
        line = line + buff;
        buff = " , color = ";
        line = line + buff;
        buff = trace_plots_color;
        line = line + buff;
        buff = " ) ";
        line = line + buff;
        file << line << "\n";
        k++;
      }

      //////// System Traces Part ends here ////////////////

      line = "";
      buff = "ax_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ".fill_between(time_scale, ";
      line = line + buff;
      buff = "y_lower_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ", y_upper_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = " , color = ";
      line = line + buff;
      buff = reach_sets_color;
      line = line + buff;
      buff = " ) ";
      line = line + buff;
      file << line << "\n";


      // Setting y_label
      line = "";
      buff = "ax_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ".set_ylabel( \' $ x_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = " $";
      line = line + buff;
      buff = " \' ";
      line = line + buff;
      buff = ", family = ";
      buff = buff + font_style;
      line = line + buff;
      buff ="  , fontsize = ";
      line = line + buff;
      line = line + font_size;
      buff = " + 10 )";
      line = line + buff;
      file << line << "\n";

      // Setting x_label
      line = "";
      buff = "ax_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ".set_xlabel('Time Steps' ";
      line = line + buff;
      buff = ", family = ";
      buff = buff + font_style;
      line = line + buff;
      buff = " , fontsize = ";
      line = line + buff;
      line = line + font_size;
      buff = ")";
      line = line + buff;
      file << line << "\n";

      // Setting max number of y_ticks
      // ax_0.yaxis.set_major_locator(MaxNLocator(4))
      line = "";
      buff = "ax_";
      line = line + buff;
      buff = to_string(j);
      line = line + buff;
      buff = ".yaxis.set_major_locator(MaxNLocator(";
      line = line + buff;
      buff = to_string(Max_No_Y_ticks);
      line = line + buff;
      buff = "))";
      line = line + buff;
      file << line << "\n";

      j++;
    }

    line = "";
    buff = "plt.savefig( ";
    line = line + buff;
    buff = trace_plots_name;
    line = line + buff;
    buff = ")";
    line = line + buff;

    file << line << "\n";

    file.close();

    system("python ./headers/system_sets_data.py");
    // system("rm ./headers/system_sets_data.py");


  }
  else{
    cout << "Unrecognised mode in the plotting function " << endl;
    cout << "Exiting ... " << endl;
    exit(0);
  }
}
