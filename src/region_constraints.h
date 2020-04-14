#ifndef region_constraints_h
#define region_constraints_h

#include "gurobi_c++.h"
#include <iostream>
#include "configuration.h"
#include <algorithm>
#include <map>
#include <vector>
#include <ctime>
#include <assert.h>
#include "network_computation.h"

using namespace std;
typedef map<uint32_t, double > _point_;


class linear_inequality
{
  private:
    uint32_t dimension;

    // The  stuff is mapped in the following fashion :
    // -x_0 + 2x_1 + x_2 + 3 > 0
    // <=> lin_ineq[-1] = 3, lin_ineq[0] = -1, lin_ineq[1] = 2, lin_ineq[2] = 1

    map< int, double > inequality;
  public:
    linear_inequality();
    linear_inequality(int dim);
    uint32_t get_dim();
    linear_inequality(map< int,  double > & lin_ineq);
    map<int, double> get_content();
    void update_bias(double bias_val);
    void update(map< int,  double > & lin_ineq);
    void add_this_constraint_to_MILP_model(map< uint32_t, GRBVar >& grb_variables,
                                           GRBModel * grb_model );
    void add_equality_constraint_to_MILP_model(map< uint32_t, GRBVar >& grb_variables,
                                               GRBModel * grb_model );
    bool if_true(_point_&);
    bool empty();
    uint32_t size();
    void print();
    bool same_direction(linear_inequality & rhs);
    double evaluate(_point_ &);
    void normalize();
    void negate();
    void get_the_dimension_indices(vector< uint32_t >&);
    friend class region_constraints;
};


class region_constraints
{
  private:
    uint32_t dimension;
    vector< linear_inequality > polytope;
    map< uint32_t, pair< double, double > > limits_in_axes_directions;
    vector< _point_ > contact_points;

  public:
    region_constraints();
    region_constraints(int dim);

    void set_dimension(int dim);
    // some function using which you can add linear inequalities to it one by one,
    // which also checks if things are making sense
    void add(linear_inequality & some_ineq);

    // some function which allows you to add constraints in a bunch
    // some function
    void add(vector< linear_inequality > & region_ineq);

    void update(vector< linear_inequality > & region_ineq);

    bool check(_point_ );

    void clear();

    // Some function which given the input variables to the network, imposes constraints
    // using it to the gurobi model pointer

    void add_this_region_to_MILP_model(map< uint32_t, GRBVar > & grb_variables, GRBModel * grb_model);

    int get_space_dimension();
    int get_number_of_constraints();

    bool return_sample( _point_&, int);

    void create_region_from_interval(map< uint32_t, pair < double, double > > interval);
    void create_region_from_interval(vector< vector < double > > interval);
    void create_region_from_interval(map< uint32_t, pair< double, double > >& interval,
                                     map< uint32_t, pair< _point_, _point_ > >& limit_points);

    void overapproximate_polyhedron_as_rectangle( map< uint32_t, pair< double, double > >& interval);
    void print();
    vector<int> get_input_indices();
    bool has(map<uint32_t, double >& direction_vector, double & bias_term);

    void set_contact_points(vector< _point_ > & _contact_points_);
    void get_contact_points(vector< _point_ > & _contact_points_);
    void add_direction_and_contact_point( linear_inequality & buffer_ineq ,
                                          _point_ & _contact_point_);
    void pick_random_directions(int count, vector< linear_inequality > & lines,
                                int input_seed);
    void get_content(vector< linear_inequality > & );
};


// void overapproximate_polyhedron_as_rectangle(
//   region_constraints & region,
//   map< uint32_t, pair< double, double > >& interval
// );

extern uint32_t generate_random_int(uint32_t);
extern uint32_t generate_random_int(uint32_t, uint32_t seed);
extern uint32_t generate_random_int_from_set(set< uint32_t > & input_set,
                                             uint32_t seed);

bool optimize_in_direction(
  map< uint32_t, double > direction_vector,
  region_constraints & region,
  double & value
);
void print_polyhedrons_in_desmos_format(
  vector < region_constraints > & all_polyhedrons,
  string filename
);



#endif
