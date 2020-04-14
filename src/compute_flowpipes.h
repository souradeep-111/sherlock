#ifndef compute_flowpipes_h
#define compute_flowpipes_h

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include "sherlock.h"
#include "Continuous.h"
#include "polynomial_computations.h"



using namespace std;
using namespace flowstar;

void compute_flowpipes_for_n_steps(
  vector< Interval > initial_interval_vector,
  int no_of_steps,
  int no_of_flowpipes, // remember this decides the sampling rate
  int polynomial_degree,
  ODE plant,
  Continuous_Reachability_Setting crs,
  computation_graph control_graph,
  vector< uint32_t > input_indices, vector< uint32_t > output_indices,
  list< Flowpipe > & result, string filename_to_save,
  pair < int, int > variable_indices, // -1 is for time
  map< string, double > & timing_information // Should have 5 elements in the following order:
  //  total_execution_time, time_in_regression, time_in_PWL_construction, time_in_sherlock, time_in_flowstar
);

void compute_taylor_models_for_neural_network(
  map< uint32_t, pair< double, double > > input_interval,
  Flowpipe & initial_set,
  Continuous_Reachability_Setting crs,
  computation_graph controller_graph,
  vector< uint32_t > input_indices, uint32_t output_index,
  int domainDim, int polynomial_degree,
  TaylorModel & tm_u, double & time_in_pwl,
  double & time_in_regression, double & time_in_sherlock,
  int & max_linear_pieces, double & max_difference
);

void plot_in_matlab(
  map<double, vector< pair <double, double > > > & sets,
  string filename_to_save,
  double step,
  pair < int, int > variable_indices
);

void generate_polynomial_for_NN(
  computation_graph controller_graph,
  uint32_t output_index,
  int max_degree,
  map< uint32_t, pair< double, double > > input_interval,
  vector< vector< unsigned int > >& monomial_terms,
  vector< datatype >& coefficients
);

#endif
