#ifndef propagate_intervals_h
#define propagate_intervals_h

#include "configuration.h"


#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <thread>
#include <mutex>
#include <ctime>
#include <ratio>
#include <chrono>
#include "network_computation.h"
#include "gurobi_interface.h"
// #include "SMT_interface.h"


using namespace std;
extern parameter_values sherlock_parameters;

class network_handler
{
  private :
  // string for the file which has the information
  // of the weights and biases and input range
  char name_of_file[256];

  // data structures for the neural net information

  vector< vector< vector< datatype > > > weights;
  vector< vector< datatype > > biases;

  vector < vector < vector < datatype > > > actual_weights;
 // contains the information of all the weights in the network

  vector < vector < datatype > > actual_biases;
  // contains all the biases, the size of biases is one more than
  // the number of sets of weights

  // unsigned int no_of_inputs,no_of_outputs,no_of_hidden_layers;

  vector< unsigned int > network_configuration;

  public :
  unsigned int no_of_inputs,no_of_outputs,no_of_hidden_layers;
  network_handler();

  network_handler( vector< vector< vector< datatype > > > weights,
                   vector< vector < datatype > > biases);

  network_handler(const char* name);
  // the constructor which takes in the information file

  void update_information( vector< vector< vector< datatype > > > weights,
                           vector< vector < datatype > > biases);

  void cast_to_single_output_network( vector< vector< vector < datatype > > >& weights,
                                      vector< vector< datatype > >& biases,
                                      unsigned int output_number);
  vector< datatype > return_gradient( vector< datatype > point,
                                      int direction, vector<vector< datatype > > region_constraints,
                                      datatype & max_val, vector< datatype >& max_point,
                                      datatype & min_val, vector< datatype >& min_point);

  void do_gradient_search(vector< vector< datatype > > input_interval,
                                             vector< datatype > sample_point,
                                             vector< datatype>& return_val,
                                             vector< datatype >& extrema_point,
                                             int direction);

  void return_interval_output(vector< vector< datatype > > input_interval,
                              vector< datatype>& return_val,
                              unsigned int output_number);

  void return_network_information( vector < vector < vector < datatype > > >& buffer_for_weights,
                              vector < vector < datatype > >& buffer_for_biases
                            );
  void return_GUROBI_handle_of_network( GRBModel * milp_model,
                                        GRBEnv * milp_env,
                                        vector< GRBVar >& input_variables,
                                        GRBVar & output_variables);
};

void merge_networks (datatype network_offset = 0,
                     datatype scaling_factor = 1,
                     char * output_file = NULL,
                     char * main_network = NULL,
                     char * sub_network_1 = NULL,
                     char * sub_network_2 = NULL,
                     char * sub_network_3 = NULL,
                     char * sub_network_4 = NULL);
// In this function I have assumed that you always connect the control
// network to the last few nodes in a chronological order

// struct set_info{
//   vector< vector< datatype > > region_constr;
//   unsigned int time_stamp;
// };

int split_set(set_info current_set, set_info stable_box,
               vector< set_info >& group_of_sets
);
int split_set(set_info current_set, set_info stable_box,
               queue< set_info >& group_of_sets
);
void find_limits_using_reluplex(
  vector< vector< datatype > > input_interval,
  vector< datatype >& output_range
);
void simulate_accelerated(
  network_handler system_network,
  unsigned int acceleration_number,
  vector<unsigned int> important_outputs,
  vector< datatype > scaling_factor,
  vector< datatype > offset_already,
  vector< vector< datatype > > input_constraints,
  vector< vector< datatype > >& output_bias_terms
);

#endif
