#include "region_constraints.h"

using namespace std;

linear_inequality :: linear_inequality()
{
  dimension = 0;
  inequality.clear();
}

linear_inequality :: linear_inequality(int dim)
{
  assert(dim > 0);
  dimension = dim;
}

linear_inequality :: linear_inequality(map< int,  double > & lin_ineq)
{
  assert(!lin_ineq.empty());
  dimension = lin_ineq.size() - 1;
  inequality = lin_ineq;
}

uint32_t linear_inequality :: get_dim()
{
  return dimension;
}
void linear_inequality :: update(map< int,  double > & lin_ineq)
{
  assert(!lin_ineq.empty());
  dimension = lin_ineq.size();

  inequality = lin_ineq;
}

void linear_inequality :: add_this_constraint_to_MILP_model(map< uint32_t, GRBVar >& grb_variables,
                                GRBModel * grb_model)
{
  assert(grb_model);
  assert(!grb_variables.empty());

  GRBVar gurobi_one = grb_model->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, "grb_one_input");

  GRBLinExpr expression(0.0);
  double data;

  for(auto & some_var : grb_variables)
  {
    if((some_var.first < 0) || (inequality.find(some_var.first) == inequality.end()))
    {
      continue;
    }
    data = inequality[some_var.first];
    expression.addTerms(& data, & grb_variables[some_var.first], 1);
  }

  data = inequality[-1];
  expression.addTerms(& data, & gurobi_one, 1);

  grb_model->addConstr(expression, GRB_GREATER_EQUAL, 0.0, "_some_input_constraint_");

}

bool linear_inequality :: if_true(map< uint32_t, double > & point )
{
  assert(!inequality.empty());
  auto sum = 0.0;
  for(auto term : inequality)
  {
    sum += (point[term.first] * term.second) ;
  }
  sum += inequality[-1];
  return ((sum > 0) ? (true) : (false));
}

bool linear_inequality :: empty()
{
  return inequality.empty();
}

uint32_t linear_inequality :: size()
{
  return inequality.size();
}

void linear_inequality :: print()
{
  cout << " [ ";
  for(auto each_term : inequality)
  {
    cout << each_term.first << " : " << each_term.second  << " , ";
  }
  cout << " ] ";
}


region_constraints :: region_constraints()
{
  dimension = 0;
  polytope.clear();
}

region_constraints :: region_constraints(int dim)
{
  assert(dim > 0);
  dimension = dim;
  polytope.clear();
}

void region_constraints :: set_dimension(int dim)
{
  assert(dim > 0);
  dimension = dim;
}


void region_constraints :: add(linear_inequality & some_input)
{
  assert(!some_input.empty());
  assert(some_input.size() >= 2);
  polytope.push_back(some_input);

}

void region_constraints :: add(vector< linear_inequality > & region_ineq)
{
  assert(!region_ineq.empty());
  assert(region_ineq[0].size() >= 2);

  for(auto & some_ineq : region_ineq)
  {
    polytope.push_back(some_ineq);
  }

}

void region_constraints :: update( vector< linear_inequality > & region_ineq )
{
  assert(!region_ineq.empty());
  polytope = region_ineq;
}

bool region_constraints :: check(map< uint32_t, double >  point )
{
  for(auto some_linear_inequality : polytope)
  {
    if(!some_linear_inequality.if_true(point))
    {
      return false;
    }
  }
  return true;
}

void region_constraints :: clear()
{
  polytope.clear();
}

void region_constraints :: add_this_region_to_MILP_model(
                                 map< uint32_t, GRBVar > & grb_variables,
                                 GRBModel * grb_model)
{
  assert(!grb_variables.empty());
  assert(grb_model);
  assert(grb_variables.size() == dimension);

  for(auto & lin_ineq : polytope)
  {
    lin_ineq.add_this_constraint_to_MILP_model(grb_variables, grb_model);
  }
}


int region_constraints :: get_space_dimension()
{
  return dimension;
}

int region_constraints :: get_number_of_constraints()
{
  return polytope.size();
}

bool region_constraints :: return_sample(map< uint32_t, double > & point, int seed)
{
  // so, the way it is done here is very simple.
  // First we are finding the centre of the poyhedral from the
  // directions parallel to the axes. Then perturb  that point
  // randomly to find an interior point

  unsigned int i , j , k, input_size, no_of_constraints;

  no_of_constraints = polytope.size();
  input_size = get_space_dimension();

  unsigned int scale = 1e4;
  unsigned int no_of_tries = 1e5;


  // Finding the constraints which actually refer to the main principal directions
  map< uint32_t, pair< double, double > > limits_in_axes_directions;

  int direction, dimension;


  limits_in_axes_directions.clear();
  overapproximate_polyhedron_as_rectangle( limits_in_axes_directions);

  point.clear();

  k = 0;
  while( k < no_of_tries)
  {
    srand(k + seed);

    point.clear();

    for(auto axis_limits : limits_in_axes_directions)
    {
      point[axis_limits.first] = limits_in_axes_directions[axis_limits.first].first +
       (double)(
                   (
                      (double) (rand() % scale) * (datatype)(limits_in_axes_directions[axis_limits.first].second - limits_in_axes_directions[axis_limits.first].first)
                   )
                    /
                  ((double)scale)
                 ) ;
    }
    if( check(point) )
    {
      return true;
    }
    k++;
  }

  return false;

}

void region_constraints :: create_region_from_interval(
                            map< uint32_t, pair < double, double > > interval)
{
  assert(!interval.empty());
  dimension = interval.size();
  polytope.clear();
  linear_inequality lin_ineq(dimension);

  map< int, double > coeffs;
  for(auto node_range_1 : interval)
  {
    // add the upper limit
    coeffs.clear();

    for(auto node_range_2 : interval)
    {
      coeffs[node_range_2.first] = 0.0;
      if(node_range_1.first == node_range_2.first)
      {
        coeffs[node_range_2.first] = -1.0;
        coeffs[-1] = interval[node_range_2.first].second;
      }
    }
    lin_ineq.update(coeffs);
    polytope.push_back(lin_ineq);

    // add the lower limit
    coeffs.clear();
    for(auto node_range_2 : interval)
    {
      coeffs[node_range_2.first] = 0.0;
      if(node_range_1.first == node_range_2.first)
      {
        coeffs[node_range_2.first] = 1.0;
        coeffs[-1] = -interval[node_range_2.first].first;
      }
    }
    lin_ineq.update(coeffs);
    polytope.push_back(lin_ineq);


  }

  return;
}


void region_constraints :: overapproximate_polyhedron_as_rectangle(
                           map< uint32_t, pair< double, double > >& interval
)
{

  interval.clear();
  assert(!polytope.empty());

  map< uint32_t, double > direction_vector;

  interval.clear();
  pair< double, double > range;


  for(auto each_term_1 : polytope[0].inequality)
  {
    if(!(each_term_1.first < 0))
    {

      // Get the lower limit
      direction_vector.clear();
      for(auto each_term_2 : polytope[0].inequality )
      {
        if(! (each_term_2.first < 0))
        {
          direction_vector[each_term_2.first] = 0.0;
        }
      }

      direction_vector[each_term_1.first] = -1.0;
      optimize_in_direction(direction_vector, *this, range.first);

      // Get the upper limit
      direction_vector.clear();
      for(auto each_term_2 : polytope[0].inequality )
      {
        if(!(each_term_2.first < 0))
        {
          direction_vector[each_term_2.first] = 0.0;
        }
      }
      direction_vector[each_term_1.first] = 1.0;
      optimize_in_direction(direction_vector, *this, range.second);

      interval[each_term_1.first] = range;
    }
  }

}

void region_constraints :: print()
{
  cout << "Region is bounded by the following inequalities : [ -1 is for the constant ;-) ] " << endl;

  for(auto each_inequality : polytope)
  {
    cout << "\t ----------- ";
    each_inequality.print();
    cout << endl;
  }
}


vector<int> region_constraints :: get_input_indices()
{
  assert(!polytope.empty());
  linear_inequality linear_ineq = polytope[0];
  vector< int > return_vector;

  for(auto each_term : linear_ineq.inequality)
  {
    if(! each_term.first < 0)
    {
      return_vector.push_back(each_term.first);
    }
  }

  return return_vector;

}


bool optimize_in_direction(
  map< uint32_t, double > direction_vector,
  region_constraints & region,
  double & value
)
{

  unsigned int i, j , k, no_of_inputs;
  unsigned int no_of_constraints;
  double data;
  unsigned int dimension = region.get_space_dimension();

  GRBLinExpr expr_one(1.0);
  GRBLinExpr expr_zero(0.0);
  GRBLinExpr expr_buffer_0(0.0);

  GRBEnv * env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  GRBModel * model_ptr = new GRBModel(*env_ptr);

  string const_name = "one";
  GRBVar const_one = model_ptr->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, const_name);

  // Putting the constant of '1'
  expr_buffer_0 = expr_zero;
  data = 1;
  expr_buffer_0.addTerms(& data, & const_one, 1);
  model_ptr->addConstr(expr_buffer_0, GRB_EQUAL, 1.0, "constant_1_set");

  assert(direction_vector.size() == region.get_space_dimension() ) ;


  string name = "anything";
  string var_name = "var_name";
  string constr_name = "constraint_name";

  // Creating the variables for the equations
  map< uint32_t, GRBVar > basic_variables;
  GRBVar var;

  for(auto direction : direction_vector)
  {
    var = model_ptr->addVar(-GRB_INFINITY,
                             GRB_INFINITY,
                             0.0,
                             GRB_CONTINUOUS,
                             var_name);
    basic_variables.insert( make_pair (direction.first, var) );
  }

  // Adding the region constraints

  region.add_this_region_to_MILP_model(basic_variables, model_ptr);

  // Setting the objective
  int direction = 0;
  GRBLinExpr objective_expr;
  objective_expr = 0;
  int maximizing = -1;

  for(auto some_direction : direction_vector)
  {
    data = some_direction.second;
    objective_expr.addTerms(& data, & basic_variables[some_direction.first], 1);
    if(data == 1.0)
    {
      maximizing = 1.0;
    }

    if (data == -1.0)
    {
      maximizing = 0.0;
    }
  }

  model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
  model_ptr->optimize();

  if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
  {
    value = model_ptr->get(GRB_DoubleAttr_ObjVal);
    if(maximizing == 0.0)
    {
      value = -value;
    }

    delete model_ptr;
    delete env_ptr;

    return true;
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
  {
    value = -1e30;
    delete model_ptr;
    delete env_ptr;
    return false;
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
  {
    value = 1e30;
    delete model_ptr;
    delete env_ptr;
    return false;
  }
  else
  {
    cout << "Unknown error in gurobi implementation ... " << endl;
    cout << "Status code =  " << model_ptr->get(GRB_IntAttr_Status)  << endl;
    delete model_ptr;
    delete env_ptr;
    cout << "Exiting.. " << endl;
    assert(false);
    return false;
  }


  return false;
}
