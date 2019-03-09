#ifndef region_constraints_h
#define region_constraints_h

#include "gurobi_c++.h"
#include <iostream>
#include "configuration.h"
#include <algorithm>



class linear_inequality
{
private:
  uint32_t dimension;

  // The  stuff is mapped in the following fashion :
  // -x_0 + 2x_1 + x_2 + 3 > 0
  // <=> lin_ineq[-1] = 3, lin_ineq[0] = -1, lin_ineq[1] = 2, lin_ineq[2] = 1

  map < int, double > linear_inequality;
public:
  linear_inequality();
  linear_inequality(int dim);
  linear_inequality(map< int,  double > & lin_ineq);
  void update(map< int,  double > & lin_ineq)
  void add_this_constraint_to_MILP_model(map< int, GRBVar >& grb_variables, GRBModel * grb_model );
  bool if_true(map<uint32_t, double > & point);

};


class region_constraints
{
private:
  uint32_t dimension;
  vector< linear_inequality > polytope;
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

  bool check(map<uint32_t, double>& point);

  void clear();

  // Some function which given the input variables to the network, imposes constraints
  // using it to the gurobi model pointer

  void add_this_region_to_MILP_model(map< uint32_t, GRBVar > & grb_variables, GRBModel * grb_model);

  int get_space_dimension();
  int get_number_of_constraints();

  bool return_sample(map< uint32_t, double > & point, int seed);

};

void overapproximate_polyhedron_as_rectangle(
  region_constraint & region,
  vector< vector< double > >& interval
);


bool optimize_in_direction(
  vector< int > direction_vector,
  region_constraints & region,
  double & value
);

#endif
