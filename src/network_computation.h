#ifndef network_computation_h
#define network_computation_h
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <string>
#include <math.h>
#include <stdint.h>
#include "gurobi_interface.h"
#include "configuration.h"

using namespace std;
extern parameter_values sherlock_parameters;


int __check_if_the_weights_and_biases_make_sense__(
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases
);
void deduce_network_configuration(
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  vector< unsigned int >& network_configuration
);
// Here the count includes everything but the input layer

vector< datatype > compute_activation  (vector< datatype >& inputs,
                                        vector< vector< datatype > >& weights,
                                        vector<datatype>& bias);

// This computes the network output for a single layer

vector< datatype > compute_activation_no_relu (
  vector< datatype >& inputs,
  vector< vector< datatype > >& weights,
  vector<datatype>& bias);

vector< unsigned int > find_positives (vector < datatype >&);

vector< datatype > do_relu(vector<datatype>& input);

datatype compute_network_output(
    vector< datatype > inputs,
    vector < vector < vector< datatype > > > weights,
    vector< vector < datatype > > biases,
    vector< vector < unsigned int > >& active_weights
);


vector< vector< datatype > > return_constraint_vectors_from_weights (
  vector< vector<datatype> > weights,
  vector< datatype > biases,
  vector< unsigned int > active_neurons
);

void return_weights_and_bias_from_a_single_layer_one_output_network(
  vector< vector<datatype> > input_weight_matrix,
  vector< datatype > input_bias,
  vector < vector< datatype > > output_weight_matrix,
  datatype output_bias,
  vector<unsigned int> active_neurons,
  unsigned int output_status,
  vector < datatype >& return_weight,
  datatype& return_bias
);

int return_shorter_network(
    vector< vector< vector < datatype > > > weights,
    vector< vector < datatype > > biases,
    unsigned int neuron_no_in_layer_2,
    vector< vector< datatype > > & input_weight_matrix,
    vector< datatype > & input_bias,
    vector< vector< datatype > > & output_weight_matrix,
    datatype & output_bias
);

int find_maximum_penetration (
  vector< vector< unsigned int > > active_weights);

void append_matrix_to_matrix(
    vector< vector< datatype > >& target,
    vector< vector< datatype > > source
 );
vector< vector< datatype> > create_constraint_from_weights_and_bias(
    vector< datatype > weights,
    datatype bias
  );

void delete_the_first_n_constraints(
  vector< vector< datatype > >& constraint_matrix,
  unsigned int number
);

void remove_the_last_constraint(
  vector< vector< datatype > >& constraint_matrix
);

void replace_layers(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >& biases,
  vector< vector< datatype > > new_weights,
  vector< datatype > new_bias
);

void reverse_a_constraint(
  vector< datatype >& constraint
);

int check_limits_using_reluplex(
  vector< datatype > limits,
  vector< vector< datatype > > region,
  int direction,
  vector< datatype >& counter_example
);

void create_constraint_from_interval (
  vector< vector< datatype > >&  constraint_matrix,
  vector< vector< datatype > > interval
);

void find_the_real_range(
  vector< vector< datatype > >& input,
  vector< datatype >& output
);
int similar(
  vector< datatype > vector_1,
  vector< datatype >  vector_2
);
int detect_degeneracy(
  vector< vector < datatype > > input_interval
);

int check_counter_example(
  vector< vector< datatype > >& positive_constraint_matrix,
  vector< vector< vector< datatype > > >& collection_of_negative_constraint_matrices,
  vector< datatype >& counter_example
);

datatype parse_string(
  const char * string
);

int find_random_sample(
  vector< vector< datatype > > positive_constraint,
  vector< datatype >& counter_example
);
int find_random_sample_with_seed(
  vector< vector< datatype > > positive_constraint,
  vector< datatype >& counter_example,
  int seed
);
int find_uniform_counter_example(
  vector< vector< datatype > > positive_constraint,
  vector< vector< vector< datatype > > > list_of_negative_constraints,
  vector< datatype >& counter_example, uint64_t& sample_number
);

void create_sub_boxes(
  vector< vector< datatype > >& input_interval,
  vector< vector< vector < datatype > > >& output_collection
);

vector< datatype > scale_vector(
  vector< datatype > input_vector,
  datatype factor
);

vector< datatype > negate_vector(
  vector< datatype > input_vector
);

int propagate_point(
  vector< datatype >& point,
  vector< datatype > direction,
  vector< vector< datatype > > region
);

int check_inflection_point(
  vector< datatype > point,
  vector < vector < vector< datatype > > >& weights,
  vector< vector < datatype > >& biases,
  int direction,
  vector< vector< datatype > > region
);

int check_limits(
  vector< vector < vector< datatype > > > weights,
  vector< vector < datatype > > biases,
  datatype limit_found,
  vector< datatype >& extrema_point,
  vector< vector< datatype > > region,
  int direction,
  vector< datatype >& counter_example
);
void expand_output_to_full_width(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >& biases
);
void expand_width_of_inner_layers(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >& biases,
  unsigned int expansion_number
);
// Does not expand the input information.
void create_fake_network(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >& biases,
  unsigned int expansion_number,
  unsigned int no_of_inputs,
  datatype network_bias
);
void add_fake_layer_to_right(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >& biases
);
void expand_last_few_input(
  vector< vector< datatype > >& weights,
  vector< datatype >& biases,
  vector< unsigned int > expansion_vector
);
// So the above function basically follows up the inputs with a few dummy
// inputs, expansion_vector contains the exact number of new neurons
void patch_networks_vertically(
  vector< vector< vector< datatype > > >& main_weights,
  vector< vector< datatype > >& main_biases,
  vector< vector< vector< datatype > > > sub_weights,
  vector< vector< datatype > > sub_biases
);
void write_network_to_file(
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  char * output_file_name
);
void print_network_weights(
  vector< vector< vector< datatype > > > weights
);
void print_network_biases(
  vector< vector< datatype > > biases
);
void adjust_offset_in_weights(
  vector< vector< datatype > >& weights,
  vector< datatype >& biases,
  datatype top_offset,
  datatype offset,
  datatype scaling_factor,
  unsigned int no_of_last_few_inputs
);
void adjust_offset(
  vector< datatype >& range,
  datatype offset
);
void adjust_offset(
  datatype& output,
  datatype offset
);
void adjust_offset(
  vector< vector< datatype > > & vec,
  vector< datatype > offset_amount
);
void add_directions_to_output(
  vector< vector< vector< datatype > > >& weights,
  vector< vector< datatype > >&  biases,
  vector< vector< datatype > > directions,
  datatype constr_offset,
  vector< datatype > offset_already
);
void convert_direction_biases_to_constraints(
  vector< vector< datatype > > directions,
  vector< vector< datatype > > biases,
  vector< vector< datatype > >& constraints
);
void print_constraints(
  vector< vector< datatype > > constraints
);
void normalize_vector(
  vector< datatype >& input_vector
);
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
);

void print_region(
  vector< vector< datatype > > region_constraints
);

datatype sum_vector(vector < datatype > temp);

void scale_vector(
  vector< datatype >& vector_input,
  vector< datatype > scale_factor
);
void scale_vector(
  vector< vector< datatype > >& vector_input,
  vector< datatype > scale_factor
);

void create_RK_network_from_differential_eq(
  vector< string > file_names,
  vector< datatype > scaling_factor,
  vector< datatype > output_biases,
  string return_file_name,
  datatype network_offset,
  datatype sampling_period
);

void patch_all_networks_vertically(
  vector< vector< vector< vector< datatype > > > > all_weights,
  vector< vector< vector< datatype > > > all_biases,
  vector< vector< vector< datatype > > >& return_weights,
  vector< vector < datatype > >& return_biases
);
// void increase_network_width_till_output_to_tap_off_nth_input(
//   vector< vector < vector< datatype > > >& weights,
//   vector< vector< datatype > >& biases,
//   unsigned int input_number,
//   datatype bias_to_be_added
// );
// create_first_adhesive_layer_with_n_sets_at_the_bottom(
//  unsigned int dimension,
//  unsigned no_of_sets,
//  vector< datatype > tap_off_neuron_offset,
//  vector< datatype > network_output_offset,
//  datatype factor,
//  vector< vector< datatype > >& weight_matrix,
//  vector< datatype >& bias_vector,
//  datatype bias_to_set
// );

void compute_M_values_with_interval_propagation(
  vector< vector< vector< datatype > > > weights,
  vector< vector< datatype > > biases,
  vector< vector< datatype > > input_interval,
  vector< vector< datatype > >& M_values
);

void compute_interval_for_linear_input_combination(
  vector< datatype > weights_vector,
  datatype bias_term,
  vector< vector< datatype > > interval_input,
  vector< datatype >& interval_output
);

void store_pointer_in_a_file(
  void* model_pointer,
  void* env_pointer,
  char * name
);

int read_pointer_from_file(
  void* & model_pointer,
  void* & env_pointer,
   char * name
);

void delete_pointer_stored_in_file(
  char * name
);

void print_2D_vector(
  vector< vector< datatype > > data
);
void save_2D_vector_to_file(
  vector< vector< datatype > > data,
  char * file_name
);
void read_2D_vector_from_file(
  char * file_name,
  vector< vector < datatype > >& data
);
datatype compute_max_abs_in_a_vector(
  vector< datatype > input_vector
);
void save_1D_vector_to_file(
  vector< datatype > data,
  char * file_name
);
struct set_info{
  vector< vector< datatype > > region_constr;
  unsigned int time_stamp;
};
void form_interval_from_region_constraints(
  vector< vector< datatype > > region_constr,
  vector< vector< datatype > >& interval
);
void collect_all_reach_sets_for_the_time_stamp(
  unsigned int time_stamp,
  vector< set_info > time_stamped_reach_sets,
  vector< vector< vector< datatype > > >& returned_reach_sets
);

class plotting_data{
public:
  int reach_set_time_range;
  int system_trace_time_range;
  int no_of_system_traces;

  unsigned int dimension;

  vector< vector< vector< datatype > > > system_traces;
  vector< vector< vector< datatype > > > reach_sets;

  plotting_data(unsigned int dim);
  void add_reach_set(vector< vector< datatype > > current_reach_set);
  void add_system_trace(vector< vector< datatype > > system_trace);
  void collect_and_merge_reach_sets(vector< set_info > time_stamped_reach_sets);
  void plot(int mode);  // 1 for just the trace , 2 for just the reach sets, 3 for overlaying the plots
};

#endif

// reminders
