#ifndef region_constraints_h
#define region_constraints_h

#include "gurobi_c++.h"
#include <iostream>
#include "configuration.h"

class linear_inequality
{
private:
  uint32_t dimension;
  map < uint32_t, double > linear_inequality;
public:
  linear_inequality();
  linear_inequality(int dim);
  linear_inequality(map< uint32_t,  double > & lin_ineq);
  void update(map< uint32_t,  double > & lin_ineq)
  void add_this_constraint_to_MILP_model(map< uint32_t, GRBVar > grb_variables, GRBModel * grb_model );

};


class region_constraints
{
private:
  uint32_t dimension;
  vector< linear_inequality > polytope;
public:
  region_constraints();
  region_constraints(int dim);
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


};


#endif
