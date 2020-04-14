#ifndef sherlock_h
#define sherlock_h

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <queue>
#include <math.h>
#include <string>
#include <limits>
#include <set>
#include "computation_graph.h"
#include "generate_constraints.h"
#include "configuration.h"
#include "region_constraints.h"
#include "network_computation.h"
#include "selective_binarization.h"
#include "parsing_onnx.h"
#include "image_handler.h"

using namespace std;

class sherlock
{
private:
  computation_graph neural_network;
  constraints_stack network_constraints;
public:
  uint32_t nodes_explored;
  sherlock();
  sherlock(computation_graph & CG);
  void clear();
  void set_computation_graph(computation_graph & CG);
  void optimize_node(uint32_t node_index, bool direction,
                      region_constraints & input_region,
                      double & optima_achieved);

  void optimize_node_with_witness(uint32_t node_index,
                      bool direction, region_constraints & input_region,
                      double & optima_achieved, _point_& optima_point);

  void maximize_in_direction(linear_inequality & direction,
                             region_constraints & input_region,
                             double & result, _point_& optima_point);

  void optimize_constrained(uint32_t node_index, bool direction,
                                 region_constraints & input_region,
                                 vector< linear_inequality > & inequalities,
                                 double & optima_achieved);

  void optimize_node(uint32_t node_index, bool direction,
                     region_constraints & input_region,
                     double & optima_achieved, _point_ & optima_point);

  void gradient_driven_optimization(uint32_t node_index,
                                    region_constraints & input_region,
                                    bool direction, double & optima);

  void gradient_driven_optimization(uint32_t node_index,
                          region_constraints & input_region, bool direction,
                          double & optima, _point_& final_point);

  void optimize_using_gradient(uint32_t node_index,
                      region_constraints & input_region, bool direction,
                      double & optima);
  void optimize_using_gradient(uint32_t node_index,
                      region_constraints & input_region, bool direction,
                      double & optima, _point_ & final_point);
  bool gradient_driven_target(uint32_t node_index,
                              region_constraints & input_region, bool direction,
                              double target, _point_& final_point);

  void compute_output_range(uint32_t node_index,
                            region_constraints & input_region,
                            pair < double, double >& output_range);
  void compute_output_region(region_constraints & input_region,
                             region_constraints & output_region);

  void perform_gradient_search(uint32_t node_index, bool direction,
                                region_constraints & region,
                                map< uint32_t, double > & starting_point,
                                double & val);

  void perform_gradient_search_with_random_restarts(uint32_t node_index,
                               bool direction, region_constraints & region,
                               map< uint32_t, double > & starting_point,
                               double & val);

  void compute_output_range_by_sampling(region_constraints & input_region,
                                        uint32_t output_node_index,
                                        pair < double , double > & output_range,
                                        uint32_t sample_count);

  void increment_point_in_direction(map<uint32_t, double >& current_values,
                                        map<uint32_t, double > direction,
                                        region_constraints& region);

  void increment_point_in_direction(map<uint32_t, double >& current_values,
                                    map<uint32_t, double > direction);

  bool increment_point_in_direction(map<uint32_t, double >& current_values,
                                    double step_size,
                                    map<uint32_t, double > direction,
                                    region_constraints& region);

  bool return_best_effort_random_counter_example(bool direction,
                                    map< uint32_t , double >& current_point,
                                    double& val_curr, uint32_t node_index,
                                    region_constraints & region);

  void add_constraint(linear_inequality & lin_ineq);

  bool prove_bounds(uint32_t node_index, double bound, bool direction,
                    region_constraints& input_region,
                    set< uint32_t >& binarized_neurons);

  bool check_satisfaction(region_constraints & input_region,
                          map< uint32_t, double >& output_assignment,
                          map< uint32_t, double >& input_assignment);

  void return_interval_difference_wrt_PWL(
    map< uint32_t, pair< double, double > > input_interval,
    vector< datatype>& return_val, uint32_t output_number,
    vector<PolynomialApproximator> const & decomposed_pwls,
    vector<double> lower_bounds, vector<double> upper_bounds);

};


void create_computation_graph_from_file(string filename,
                                        computation_graph & CG,
                                        bool has_output_relu,
                                        vector<uint32_t>& input_node_indices,
                                        vector<uint32_t>& output_node_indices);

void read_controller_graph(string filename, computation_graph & CG,
                            bool has_output_relu,
                            vector < double > scale, vector < double > offset,
                            vector < uint32_t >& input_node_indices,
                            vector < uint32_t >& output_node_indices);

void print_polyhedron_using_python(
  region_constraints & input_polyhedron,
  computation_graph & neural_network,
  region_constraints & output_polyhedron,
  string filename
);

void test_network_1(computation_graph & CG);
void test_network_2(computation_graph & CG);
void test_network_sigmoid(computation_graph & CG);
void test_network_tanh(computation_graph & CG);
#endif
