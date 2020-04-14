#include "region_constraints.h"

using namespace std;

bool debug_reg = true;

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
  // assert(!lin_ineq.empty());
  dimension = lin_ineq.size() - 1;
  inequality = lin_ineq;
}

uint32_t linear_inequality :: get_dim()
{
  return dimension;
}

map<int, double > linear_inequality :: get_content()
{
  assert(! inequality.empty());
  return inequality;
}

void linear_inequality :: update_bias(double bias)
{
  assert(!inequality.empty());
  inequality[-1] = bias;
}

void linear_inequality :: update(map< int,  double > & lin_ineq)
{
  assert(!lin_ineq.empty());
  dimension = lin_ineq.size();

  inequality = lin_ineq;
}

void linear_inequality :: add_this_constraint_to_MILP_model(
                                map< uint32_t, GRBVar >& grb_variables,
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

void linear_inequality :: add_equality_constraint_to_MILP_model(
                                map< uint32_t, GRBVar >& grb_variables,
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

  grb_model->addConstr(expression, GRB_EQUAL, 0.0, "_equality_constraint_");

}

bool linear_inequality :: if_true( _point_& sample_point)
{
  assert(!inequality.empty());
  auto sum = 0.0;
  for(auto term : inequality)
  {
    sum += (sample_point[term.first] * term.second) ;
  }
  sum += inequality[-1];
  return ((sum >= 0.0) ? (true) : (false));
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

void linear_inequality :: normalize()
{
  double squared_sum = 0.0;
  double normalizing_term;
  for(auto each_term : inequality)
  {
    if(each_term.first > 0)
    {
      squared_sum += (each_term.second * each_term.second);
    }
  }
  normalizing_term = sqrt(squared_sum);
  assert(normalizing_term > 0.0);
  for(auto & each_term : inequality)
  {
    each_term.second /= normalizing_term;
  }

}

void linear_inequality :: negate()
{
  for(auto & each_term : inequality)
    each_term.second = -each_term.second;

}
bool linear_inequality :: same_direction(linear_inequality & rhs)
{

  this->normalize();
  rhs.normalize();

  double left_coeff, right_coeff;
  for(auto each_pair : this->inequality)
  {
    if(each_pair.first > 0)
    {
      assert(rhs.inequality.find(each_pair.first) != rhs.inequality.end());
      left_coeff = each_pair.second;
      right_coeff = rhs.inequality[each_pair.first];
      if( fabs(left_coeff - right_coeff) > 1e-3)
      {
        return false;
      }
    }

  }

  return true;
}

double linear_inequality :: evaluate(_point_& evaluation_point)
{
  assert(!inequality.empty());
  auto sum = 0.0;
  for(auto term : inequality)
  {
    if(term.first > 0)
    {
      assert(evaluation_point.find(term.first) != evaluation_point.end());
      sum += (evaluation_point[term.first] * term.second) ;
    }
  }

  sum += inequality[-1];
  return sum;
}

void linear_inequality :: get_the_dimension_indices(
  vector< uint32_t > & indices_involved
)
{
  assert(!inequality.empty());
  indices_involved.clear();
  for(auto each_term : inequality)
  {
    if(each_term.first > 0)
      indices_involved.push_back(each_term.first);
  }

}

region_constraints :: region_constraints()
{
  dimension = 0;
  polytope.clear();
  limits_in_axes_directions.clear();
}

region_constraints :: region_constraints(int dim)
{
  assert(dim > 0);
  dimension = dim;
  polytope.clear();
  limits_in_axes_directions.clear();
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
  vector< uint32_t > dimension_indices;
  region_ineq[0].get_the_dimension_indices(dimension_indices);
  dimension = dimension_indices.size();

  polytope = region_ineq;

}

bool region_constraints :: check(_point_ p)
{
  for(auto some_linear_inequality : polytope)
  {
    if(!some_linear_inequality.if_true(p))
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

bool region_constraints :: return_sample(_point_ & point, int seed)
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


  int direction, dimension;

  if(limits_in_axes_directions.empty())
  {
    overapproximate_polyhedron_as_rectangle( limits_in_axes_directions);
  }


  point.clear();

  k = 0;
  while( k < no_of_tries)
  {
    // srand(k + seed);

    point.clear();

    for(auto axis_limits : limits_in_axes_directions)
    {
      if((axis_limits.second.second - axis_limits.second.first) > sherlock_parameters.tool_zero)
      {
        double factor = ((double) generate_random_int(scale, axis_limits.first + k + seed + 13)
        / (double) scale);
        double shift = factor * (axis_limits.second.second - axis_limits.second.first);
        point[axis_limits.first] = axis_limits.second.first + shift;
      }
      else
      {
        point[axis_limits.first] = axis_limits.second.first;
      } 
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

  limits_in_axes_directions = interval;


  return;
}

void region_constraints :: create_region_from_interval(
                            vector< vector < double > > interval)
{
  assert(! interval.empty());
  uint32_t dimension_index;
  map < uint32_t, pair< double, double > > buffer;
  buffer.clear();
  pair< double, double > limits;
  dimension_index = 0;
  while(dimension_index < interval.size())
  {
    assert(!interval[dimension_index].empty());
    limits = make_pair(interval[dimension_index][0], interval[dimension_index][1]);
    buffer[dimension_index] = limits;
    dimension_index++;
  }

  create_region_from_interval(buffer);

}

void region_constraints :: create_region_from_interval(
  map< uint32_t, pair< double, double > > & interval,
  map< uint32_t, pair< _point_, _point_ > > & limit_points
)
{
  assert(!interval.empty());
  assert(!limit_points.empty());

  dimension = interval.size();
  polytope.clear();
  contact_points.clear();
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
    contact_points.push_back((limit_points[node_range_1.first]).second);

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
    contact_points.push_back((limit_points[node_range_1.first]).first);
  }

  limits_in_axes_directions = interval;

}

void region_constraints :: overapproximate_polyhedron_as_rectangle(
                           map< uint32_t, pair< double, double > >& interval
)
{
  if(!limits_in_axes_directions.empty())
  {
    interval = limits_in_axes_directions;
    return;
  }

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

      // direction vector belongs to a region constraint
      // direction return the constant term

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

  return_vector.clear();
  for(auto each_term : linear_ineq.inequality)
  {
    if(each_term.first > 0)
    {
      return_vector.push_back(each_term.first);
    }
  }

  // assert(return_vector.size() > 0);

  return return_vector;

}

bool region_constraints :: has(map<uint32_t, double >& direction_vector,
                                      double & bias_term)
{
  map<int, double> buffer_dir;
  for(auto each_term : direction_vector)
  {
    buffer_dir[each_term.first] = each_term.second;
  }
  buffer_dir[-1] = 0;
  linear_inequality test_expr(buffer_dir);

  for(auto each_expr : polytope)
  {
    if(each_expr.same_direction(test_expr))
    {
      bias_term = each_expr.inequality[-1];
      return true;
    }
  }
  return false;
}

void region_constraints :: set_contact_points(vector< _point_ >& _contact_points_)
{
  assert(!_contact_points_.empty());
  assert(polytope.size() == _contact_points_.size());
  contact_points = _contact_points_;
}

void region_constraints :: get_contact_points(vector< _point_ > & _contact_points_)
{
  assert(!contact_points.empty());
  _contact_points_ = contact_points;
}

void region_constraints :: add_direction_and_contact_point(
  linear_inequality & buffer_ineq,
  _point_ & _contact_point_
)
{
  assert(!buffer_ineq.empty());
  assert(!_contact_point_.empty());
  assert(_contact_point_.size() == (buffer_ineq.inequality.size() - 1));

  double val = buffer_ineq.evaluate(_contact_point_);
  // assert((val < -sherlock_parameters.tool_zero) || (val > sherlock_parameters.tool_zero)) ;

  polytope.push_back(buffer_ineq);
  contact_points.push_back(_contact_point_);
}

void region_constraints :: pick_random_directions(int count,
                            vector< linear_inequality > & lines,
                            int input_seed)
{

  assert(count > 0);
  assert(polytope.size() > 0);
  assert(count < polytope.size());

  lines.clear();
  linear_inequality candidate_line, negation;
  uint32_t random_index;


  int seed = 0;
  while(lines.size() < count)
  {
    seed += 1;
    random_index = generate_random_int(polytope.size(), seed * input_seed);
    random_index--;

    candidate_line = polytope[random_index];
    negation = candidate_line;
    negation.negate();

    bool is_parallel = false;
    for(auto each_line : lines)
    {
      if((each_line.same_direction(candidate_line)) ||
        (each_line.same_direction(negation)))
      {
        is_parallel = true;
        break;
      }

    }
    if(!is_parallel)
      lines.push_back(candidate_line);

  }

}

void region_constraints :: get_content(vector< linear_inequality > & contents)
{
  assert(! polytope.empty());
  contents = polytope;
}


void print_polyhedrons_in_desmos_format(
  vector < region_constraints > & all_polyhedrons,
  string filename
)
{
  setprecision(4);
  ofstream file;
  file.open(filename.c_str());
  vector< linear_inequality > all_halfspaces;
  map< int, double > inequation;

  int index = 0, dim = 0;
  for(auto each_polyhedron : all_polyhedrons)
  {
    file << "Polyhedron -- " << index << "\n";
    each_polyhedron.get_content(all_halfspaces);
    file << "\n";
    file << "x < " << sherlock_parameters.MILP_M;
    for(auto each_inequality : all_halfspaces)
    {
      inequation = each_inequality.get_content();
      assert(inequation.size() == 3);
      // Print each inequality to file
      //  \left \{ 2x+3y + 4 >0 \right \}

      file << " \\{";
      dim = 0;
      for(auto each_term : inequation)
      {
        if(each_term.first > 0)
        {
          if(dim == 0)
            file << each_term.second << "x + ";
          else if(dim == 1)
            file << each_term.second << "y + ";

          dim ++;
        }
      }
      file << inequation[-1] << " > 0" << " \\}";
    }

    file << "\n";
    index++;
  }

  file.close();

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

  GRBEnv * env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  GRBModel * model_ptr = new GRBModel(*env_ptr);


  GRBLinExpr expr_one(1.0);
  GRBLinExpr expr_zero(0.0);
  GRBLinExpr expr_buffer_0(0.0);

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
  // GRBVar var;

  for(auto direction : direction_vector)
  {
    GRBVar var = model_ptr->addVar(-GRB_INFINITY,
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
  model_ptr->update();

  // string s = "./Gurobi_file_created/interval_finding.lp";
  // model_ptr->write(s);

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
