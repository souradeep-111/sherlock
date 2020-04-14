#ifndef sherlock_poly_h
#define sherlock_poly_h

#include "sherlock.h"
typedef map<uint32_t, double > _point_;

class polyhedral_abstraction
{
public:
  void optimize( region_constraints & input_polyhedron,
                 linear_inequality & optimal_direction,
                 computation_graph & neural_network,
                 _point_& contact_point, double & result);

  void sample_vertex( region_constraints & region,
                      vector< _point_ > & contact_points,
                      set< _point_ > & vertices);

  uint32_t pick_dimension_to_increment( vector< linear_inequality > & lines_sat,
                                        set < uint32_t > & determined_dimensions,
                                        set < uint32_t > & undetermined_dimensions,
                                        _point_ & current_point, int seed);

  void increment_point_in_direction( _point_ & starting_value,
                          map< uint32_t, double > & direction);

  void estimate_max_penetration_depth( region_constraints & input_region,
                                       region_constraints & output_region,
                                       computation_graph & neural_network,
                                       linear_inequality & separator_axis);

  _point_ find_intersection_points( vector< linear_inequality > & lines);
  linear_inequality find_direction( vector < _point_ >& contact_points,
                                    _point_ & intersection_point);
   void find_the_undetermined_values(
                          vector< linear_inequality > & equations_involved,
                          _point_& partial_values);

  void tighten_polyhedron( region_constraints & input_polyhedron,
                           region_constraints & starting_polyhedron,
                           computation_graph & current_graph,
                           region_constraints & output_region);

  void split_polyhedron(  const region_constraints & input_polyhedron,
                          const region_constraints & output_polyhedron,
    pair< region_constraints, region_constraints > & result_polyhedrons);

  void compute_polyhedrons( vector < region_constraints > & input_polyhedrons,
                            computation_graph & neural_network,
                            set < uint32_t > & current_output_neurons,
                            vector < region_constraints > & all_polyhedrons);

  void propagate_polyhedrons( computation_graph & neural_network,
                              region_constraints & input_polyhedron,
                              region_constraints & output_polyhedron,
                              set< uint32_t >& output_indices);
};
bool check_if_point_in_set(_point_& , set< _point_ > & );
bool _test_equal_ (_point_ & , _point_ & );


void test_poly_abstr_simple(computation_graph & CG);

void drone_example(computation_graph & CG);

bool check_subset(set < uint32_t > & left_set,
                  set < uint32_t > & right_set);

bool check_subset( vector < uint32_t > & left_set,
                   set < uint32_t > & right_set);

void convert_vector_to_set( vector < uint32_t > & vector_in,
                            set < uint32_t > & set_out);


bool check_subset(
  set < uint32_t > & left_set,
  set < uint32_t > & right_set
);


bool check_subset(
  set < uint32_t > & left_set,
  vector < uint32_t > & right_set
);

void return_random_vector(
  int seed,
  set< uint32_t > & dimensions,
  map < uint32_t, double > & random_vector
);

int equations_satisfied(
  _point_ sample_point,
  vector< linear_inequality > & lines
);
int equations_satisfied(
  _point_ sample_point,
  vector< linear_inequality > & lines,
  vector < linear_inequality > & lines_sat
);

void add_vertices(
  region_constraints & region,
  set< _point_ > & vertices
);

#endif
