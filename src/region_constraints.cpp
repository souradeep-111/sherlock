#include "region_constraints.h"

using namespace std;

linear_inequality :: linear_inequality()
{
  dimension = 0;
  linear_inequality.clear();
}

linear_inequality :: linear_inequality(int dim)
{
  assert(dim > 0);
  dimension = dim;
}

linear_inequality :: linear_inequality(map< int,  double > & lin_ineq)
{
  assert(!lin_ineq.empty());
  dimension = lin_ineq.size();
  linear_inequality = lin_ineq;
}

void linear_inequality :: update(map< int,  double > & lin_ineq)
{
  assert(!lin_ineq.empty());
  dimension = lin_ineq.size();

  linear_inequality = lin_ineq;
}

void linear_inequality :: add_this_constraint_to_MILP_model(map< int, GRBVar >& grb_variables, GRBModel * grb_model)
{
  assert(!grb_moddel);
  assert(!grb_variables.empty());

  GRBVar gurobi_one = grb_model->addVar(1.0, 1.0, 0.0, GRB_CONTINUOUS, "grb_one_input");

  GRBLinExpr expression(0.0);
  double data;

  for(auto & some_var : grb_variables)
  {
    if(some_var.first < 0)
      continue;
    data = linear_inequality[some_var.first];
    expression.addTerms(& data, & grb_variables[some_var.first], 1);
  }

  data = linear_inequality[-1];
  expression.addTerms(& data, & gurobi_one, 1);

  grb_model->addConstr(expression, GRB_GREATER_EQUAL, 0.0, "_some_input_constraint_");

}

bool linear_inequality :: if_true(map< uint32_t, double > & point )
{
  assert(!linear_inequality.empty());
  auto sum = 0.0;
  for(auto term : linear_inequality)
  {
    sum += (point[term.first] * term.second) ;
  }
  sum += linear_inequality[-1];
  return ((sum > 0) ? (true) : (false));
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
  assert(region_ineq[0].size() == dimension + 1);
  polytope = region_ineq;
}

bool region_constraints :: check(map< uint32_t, double > & point )
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

void region_constraints :: add_this_region_to_MILP_model(map< uint32_t, GRBVar > & grb_variables,
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
  vector< vector< datatype > > limits_in_axes_directions(input_size, vector< datatype >(2,0));

  int direction, dimension;


  limits_in_axes_directions.clear();
  overapproximate_polyhedron_as_rectangle(*this, limits_in_axes_directions);

  point.clear();

  k = 0;
  while( k < no_of_tries)
  {
    srand(k + seed);

    i = 0;
    while(i < input_size)
    {
      point[i] = limits_in_axes_directions[i][0] +
       (double)(
                   (
                      (double) (rand() % scale) * (datatype)(limits_in_axes_directions[i][1] - limits_in_axes_directions[i][0])
                   )
                    /
                  ((double)scale)
                 ) ;
      i++;
    }

    if( check(point) )
    {
      return true;
    }
    k++;
  }

  return false;

}


void overapproximate_polyhedron_as_rectangle(
  region_constraint & region,
  vector< vector< double > >& interval
)
{

  int no_of_inputs = region.get_space_dimension(); // -1 for the constant term involved
  vector< int > direction_vector(no_of_inputs, 0);

  interval.clear();
  vector< double > range(2);

  for(int i = 0; i < no_of_inputs; i++)
  {

    // Get the lower limit
    fill(direction_vector.begin(), direction_vector.end(), 0);
    direction_vector[i] = -1;
    optimize_in_direction(direction_vector, region, range[0]);

    // Get the upper limit
    fill(direction_vector.begin(), direction_vector.end(), 0);
    direction_vector[i] = 1;
    optimize_in_direction(direction_vector, region, range[1]);

    interval.push_back(range);
  }

}


bool optimize_in_direction(
  vector< int > direction_vector,
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

  i = 0;
  while(i < dimension)
  {
    var = model_ptr->addVar(-GRB_INFINITY,
                             GRB_INFINITY,
                             0.0,
                             GRB_CONTINUOUS,
                             var_name);
    basic_variables.insert( make_pair < uint32_t, GRBVar > (i, var) );
    i++;
  }

  // Adding the region constraints
  region.add_this_region_to_MILP_model(basic_variables, model_ptr);


  // Setting the objective
  int direction = 0;
  GRBLinExpr objective_expr;
  objective_expr = 0;
  i = 0;
  while(i < dimenson)
  {
    data = direction_vector[i];
    objective_expr.addTerms(& data, & basic_variables[i], 1);
    i++;
  }

  model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
  model_ptr->optimize();

  // model_ptr->update();
  // string s = "check_file_find_counter_ex.lp";
  // model_ptr->write(s);

  if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
  {
    if(direction == 1)
    {
      value = model_ptr->get(GRB_DoubleAttr_ObjVal);
    }
    else if(direction == (-1))
    {
      value = - model_ptr->get(GRB_DoubleAttr_ObjVal);
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
    exit(0);
    return false;
  }


  return false;
}
