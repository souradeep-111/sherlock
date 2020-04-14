#include "sherlock_poly.h"

double improv_delta = 1e-2;
double trial_count = 1e2;
double equal_thresh = 1e-4;
double scaling_factor = 1e-2;
int attempts_for_vertex = 10;

bool debug_sherlock_poly = true;
bool verbose_poly = true;

void polyhedral_abstraction :: optimize(
  region_constraints & input_polyhedron,
  linear_inequality & optimal_direction,
  computation_graph & neural_network,
  _point_& contact_point, double & result
)
{
  assert(!optimal_direction.empty());

  _point_ input_point;
  sherlock sherlock_handler(neural_network);

  sherlock_handler.maximize_in_direction(optimal_direction, input_polyhedron,
                        result, input_point);
  neural_network.evaluate_graph(input_point, contact_point);

  sherlock_handler.clear();

}

void polyhedral_abstraction :: sample_vertex(
  region_constraints & region,
  vector< _point_ > & _contact_points,
  set< _point_ > & vertices
)
{
  vector < linear_inequality > lines;
  region.get_content(lines);
  vector< _point_ > contact_points = _contact_points; // Late change in decision, sorry !!
  assert(! lines.empty());
  assert(! contact_points.empty());
  assert(lines.size() == contact_points.size());
  vertices.clear();



  if(verbose_poly)
  {
    cout << "\tEnters sample vertex " << endl;
    cout << "\tContact points received - " << contact_points.size() << endl;
  }

  add_vertices(region, vertices);
  if(verbose_poly)
    cout << "Leaves sample vertex" << endl;
  return;

  vector < linear_inequality > lines_sat, current_eqs_satisfied;
  set < uint32_t > determined_dimensions, undetermined_dimensions;
  map< uint32_t, double > random_vector;
  set < _point_ > init_points_explored;

  vector< uint32_t > dimensions_involved;
  lines[0].get_the_dimension_indices(dimensions_involved);

  int space_dim = dimensions_involved.size();

  if(space_dim == 1)
  {
    vertices.clear();
    for(auto each_point : contact_points)
      vertices.insert(each_point);

    return;
  }

  _point_ current_point;
  int contact_point_index, attempt_index;
  uint32_t dim_picked, count_of_eqs_satisfied;

  // Prune the  contact points for repetitions
  set< _point_ > filtered_contact_points;
  for(auto each_point : contact_points)
    if(!check_if_point_in_set(each_point, filtered_contact_points))
      filtered_contact_points.insert(each_point);

  contact_points.clear();
  for(auto each_point : filtered_contact_points)
    contact_points.push_back(each_point);

  // If size is too small then fill it up with some internal samples
    // This is an arbitrary choice of predicates though
  if(contact_points.size() < (space_dim / 10))
  {
    if(verbose_poly)
    {
      cout << "\t No of contact points received not enough, " <<
       "had to fill with random internal points " << endl;
    }
    _point_ internal_sample;
    region_constraints internal_region;
    internal_region.update(lines);
    for(int counter = 0; counter < 10; counter ++)
    {
      if(!internal_region.return_sample(internal_sample, 13 * (counter + 1)))
        assert(false);
      contact_points.push_back(internal_sample);
    }
  }


  contact_point_index = 0;
  while(contact_point_index < contact_points.size() )
  {
    // Initializations
    current_point = contact_points[contact_point_index];


    if(equations_satisfied(current_point, lines, lines_sat) == space_dim)
    {
      // SAMPLE SOME CORNER POINTS
      if(verbose_poly)
        cout << "\t Is a corner point, so sampling an internal point " << endl;

      region_constraints internal_region;
      internal_region.update(lines);
      if(!internal_region.return_sample(current_point, 13 * (contact_point_index + 1)))
        assert(false);
    }

    if(equations_satisfied(current_point, lines, lines_sat) > 1)
    {
      contact_point_index++;
      continue;
    }

    if(verbose_poly)
      cout << "\t Index of contact point being played with : " << contact_point_index << endl;

    determined_dimensions.clear();
    undetermined_dimensions.clear();
    convert_vector_to_set(dimensions_involved, determined_dimensions);
    attempt_index = 0;
    random_vector.clear();


    // MEMO :
    // Determined dimensions picked up by the random vector
    // Undetermined dimensions computed as a result of the equations involved

    while(attempt_index < attempts_for_vertex)
    {
      while((equations_satisfied(current_point, lines, lines_sat) < space_dim))
      {

        assert(equations_satisfied(current_point, lines, lines_sat) < space_dim);

        // Randomly pick a dimension to be determined
        if(lines_sat.empty())
        {
          undetermined_dimensions.clear();
          convert_vector_to_set(dimensions_involved, determined_dimensions);
        }
        else
        {
          dim_picked = pick_dimension_to_increment(lines_sat, determined_dimensions,
            undetermined_dimensions, current_point, attempt_index + contact_point_index + 1);
            if(dim_picked == 0)
            {
              // Trigerring exit
              attempt_index = attempts_for_vertex;
              break;
            }

          determined_dimensions.erase(dim_picked);
          undetermined_dimensions.insert(dim_picked);
        }



        assert(!determined_dimensions.empty());
        // Choose random direction for the other vectors
        return_random_vector(attempt_index + contact_point_index + 1, determined_dimensions, random_vector);
        assert(! random_vector.empty());

        for(auto & each_term : random_vector)
          each_term.second *= scaling_factor;

        count_of_eqs_satisfied = equations_satisfied(current_point, lines, lines_sat);
        // assert(! lines_sat.empty());

        while(count_of_eqs_satisfied == equations_satisfied(current_point, lines))
        {
          increment_point_in_direction(current_point, random_vector);

          for(auto dim : undetermined_dimensions)
            if(current_point.find(dim) != current_point.end())
              current_point.erase(dim);

          // Find the determined dimensions
          if(!lines_sat.empty())
            find_the_undetermined_values(lines_sat, current_point);
        }
        assert(equations_satisfied(current_point, lines, lines_sat)
               == (count_of_eqs_satisfied + 1));

      }
      attempt_index++;
    }
    // Corner point picked pushed into vertices data structure.
    if(equations_satisfied(current_point, lines, lines_sat) >= space_dim)
      vertices.insert(current_point);

    contact_point_index ++;
  }
  if(vertices.size() < 2*space_dim)
  {
    add_vertices(region, vertices);
  }

  if(verbose_poly)
    cout << "Leaves sample vertex" << endl;

}

void polyhedral_abstraction :: increment_point_in_direction(
  _point_ & starting_value,
  map< uint32_t, double > & direction
)
{
  assert(! starting_value.empty());
  assert( direction.size() <= starting_value.size());

  for(auto & each_term : direction)
  {
    assert(starting_value.find(each_term.first) != starting_value.end());
    starting_value[each_term.first] += each_term.second;
  }

}

void polyhedral_abstraction :: estimate_max_penetration_depth(
  region_constraints & input_region,
  region_constraints & output_region,
  computation_graph & neural_network,
  linear_inequality & separator_axis
)
{
  // vector< _point_ > contact_points;
  // output_region.get_contact_points(contact_points);
  // assert(! contact_points.empty());
  //
  //
  // vector < tuple < double, _point_, _point_ > > penetration_data_per_face,
  //                                               penetration_data;
  //
  // int trial_index = 0;
  // // < length of the vector > , < starting point >, < ending point >
  //
  // for(/* each <contact point AND equality > in  all
  //       < contact points and inequalities > */)
  // {
  //   penetration_data_per_face.clear();
  //   trial_index = 0;
  //   while( trial index < trial count / 2 )
  //   {
  //     // choose a random dimension to be determined
  //     // choose a random unit direction for the remaining dimensions
  //     // randomly choose a lambda multiplying factor
  //     // Keep going along the direction of the vector perpendicular to
  //     // the plane to estimate the depth
  //     // add it to the  penetration data
  //   }
  //
  //
  //   trial_index = 0;
  //   while( /* For each starting point in the penetration data */)
  //   {
  //      // Pick a starting point
  //
  //      // choose a random dimension to be determined
  //
  //      // choose a random unit direction for the remaining dimensions
  //      // randomly choose a lambda multiplying factor
  //
  //      // Keep going along the direction of the vector perpendicular to
  //      // the plane to estimate the depth
  //
  //      // add it to the  penetration data
  //   }
  //
  //   // penetration_data = (penetration_data) Union (penetration data per face)
  // }


  // Find the penetration depth with the maximum value
}

_point_ polyhedral_abstraction :: find_intersection_points(
  vector< linear_inequality > & lines
)
{
  assert(! lines.empty());
  _point_ return_val;

  // Declare all the Gurobi variables
  GRBEnv * env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  GRBModel * model_ptr = new GRBModel(*env_ptr);
  model_ptr->set(GRB_DoubleParam_IntFeasTol, sherlock_parameters.int_tolerance);

  map< uint32_t, GRBVar > gurobi_variables;
  vector< uint32_t > dimension_indices;
  lines[0].get_the_dimension_indices(dimension_indices);

  for(auto each_index : dimension_indices)
  {
    GRBVar gurobi_var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY,
      0.0, GRB_CONTINUOUS, to_string(each_index));
    gurobi_variables[each_index] = gurobi_var;
  }

  for(auto & each_line : lines)
    each_line.add_equality_constraint_to_MILP_model(gurobi_variables, model_ptr);

  // Set Optimization direction to nothing
  GRBLinExpr objective_expr;
  objective_expr = 0;
  model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
  model_ptr->optimize();
  model_ptr->update();



  // Get a feasible solution
  if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
  {
      return_val.clear();
      for(auto & var : gurobi_variables)
        return_val[var.first] = var.second.get(GRB_DoubleAttr_X);

  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
  {
      return_val.clear();
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
  {
    model_ptr->set(GRB_IntParam_DualReductions, 0);
    model_ptr->update();
    model_ptr->optimize();
    if( model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL )
    {
      return_val.clear();
      for(auto & var : gurobi_variables)
        return_val[var.first] = var.second.get(GRB_DoubleAttr_X);
    }
    else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
    {
      return_val.clear();
    }
  }
  else
  {
      cout << "Some unkown Gurobi flag !" << endl;
      cout << "Flag returned - " << model_ptr->get(GRB_IntAttr_Status) << endl;
      assert(false);
  }


  // Delete the Gurobi stuff
  if(model_ptr)
    delete model_ptr;
  if(env_ptr)
    delete env_ptr;

  return return_val;
}

linear_inequality polyhedral_abstraction :: find_direction(
  vector < _point_ >& contact_points,
  _point_ & intersection_point
)
{



  // Build the Gurobi model and environment
  // Declare the variables involved in this thing
  // Set the constraints that contact points have to be on one side
  // Set the constraint that the intersection points have to be on another side
  // Get the solution, for the the linear inequality and return it

  assert(! contact_points.empty());
  assert(! intersection_point.empty());

  // Making sure that at least for the 1st contact point, the dimensions match
  for(auto each_pair : contact_points[0])
    assert(intersection_point.find(each_pair.first) != intersection_point.end());

  GRBEnv * env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  GRBModel * model_ptr = new GRBModel(*env_ptr);
  model_ptr->set(GRB_DoubleParam_IntFeasTol, sherlock_parameters.int_tolerance);

  double data;
  map< uint32_t, GRBVar > gurobi_variables;

  // For the constant part of the inequality
  GRBVar gurobi_var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY,
    0.0, GRB_CONTINUOUS, "c");
  gurobi_variables[-1] = gurobi_var;

  // For all the other dimensions involved
  for(auto each_pair : intersection_point)
  {
    GRBVar gurobi_var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY,
      0.0, GRB_CONTINUOUS, "c_" + to_string(each_pair.first));
    gurobi_variables[each_pair.first] = gurobi_var;
  }


  GRBLinExpr expr;
  // Constraint saying all the contact points have to be on one side
  for(auto each_point : contact_points)
  {
    // Basically the linear inequality has to be negative on the contact points
    expr = 0.0;

    for(auto value : each_point)
    {
      data = value.second;
      expr.addTerms(& data, & gurobi_variables[value.first], 1);
    }
    data = 1.0;
    expr.addTerms(& data, & gurobi_variables[-1], 1);

    model_ptr->addConstr(expr, GRB_LESS_EQUAL, 0.0, "contact_point_constr");
  }
  // Constraint saying the intersection point has to be on the other side
  expr = 0.0;
  GRBLinExpr objective_expr = 0.0;
  for(auto value : intersection_point)
  {
    data = value.second;
    expr.addTerms(& data, & gurobi_variables[value.first], 1);
    objective_expr.addTerms(& data, & gurobi_variables[value.first], 1);

    model_ptr->addConstr(gurobi_variables[value.first], GRB_LESS_EQUAL, 1.0);
    model_ptr->addConstr(gurobi_variables[value.first], GRB_GREATER_EQUAL, -1.0);

  }
  data = 1.0;
  expr.addTerms(& data, & gurobi_variables[-1], 1);
  objective_expr.addTerms(& data, & gurobi_variables[-1], 1);
  model_ptr->addConstr(expr, GRB_GREATER_EQUAL, (improv_delta * 1e-4), "intersection_point_constr");





  model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
  model_ptr->optimize();
  model_ptr->update();

  string s = "./Gurobi_file_created/Integer_Linear_program.lp";
  model_ptr->write(s);

  map < int, double > linear_exp;

  // Get a feasible solution
  if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
  {

      linear_exp.clear();
      for(auto & var : gurobi_variables)
        linear_exp[var.first] = var.second.get(GRB_DoubleAttr_X);
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
  {
      linear_exp.clear();
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
  {

    model_ptr->set(GRB_IntParam_DualReductions, 0);
    model_ptr->update();
    model_ptr->optimize();
    if( model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL )
    {

      linear_exp.clear();
      for(auto & var : gurobi_variables)
        linear_exp[var.first] = var.second.get(GRB_DoubleAttr_X);

    }
    else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
    {
      linear_exp.clear();
    }
    else
    {
        // cout << "Some unkown Gurobi flag in Find new direction !" << endl;
        // cout << "Flag returned - " << model_ptr->get(GRB_IntAttr_Status) << endl;
        linear_exp.clear();
    }

  }
  else
  {
      // cout << "Some unkown Gurobi flag in Find new direction !" << endl;
      // cout << "Flag returned - " << model_ptr->get(GRB_IntAttr_Status) << endl;
      linear_exp.clear();
  }


  // Delete the Gurobi stuff
  if(model_ptr)
    delete model_ptr;
  if(env_ptr)
    delete env_ptr;


  linear_inequality l(linear_exp);
  return l;

}

void polyhedral_abstraction :: find_the_undetermined_values(
  vector< linear_inequality > & equations_involved,
  _point_& partial_values
)
{
  assert(! equations_involved.empty());
  assert(partial_values.size() > 0);

  GRBEnv * env_ptr = new GRBEnv();
  erase_line();
  env_ptr->set(GRB_IntParam_OutputFlag, 0);
  GRBModel * model_ptr = new GRBModel(*env_ptr);
  model_ptr->set(GRB_DoubleParam_IntFeasTol, sherlock_parameters.int_tolerance);

  vector< uint32_t > dimension_indices;
  linear_inequality lq= *(equations_involved.begin());
  lq.get_the_dimension_indices(dimension_indices);


  assert(partial_values.size() < dimension_indices.size());
  assert((partial_values.size() + equations_involved.size()) == dimension_indices.size());

  map< uint32_t, GRBVar > gurobi_variables;
  for(auto each_dim : dimension_indices)
  {
    GRBVar gurobi_var = model_ptr->addVar(-GRB_INFINITY, GRB_INFINITY,
      0.0, GRB_CONTINUOUS, "n_" + to_string(each_dim));
    gurobi_variables[each_dim] = gurobi_var;
  }

  // Adding the constraint involved with each equation
  for(auto equation : equations_involved)
    equation.add_equality_constraint_to_MILP_model(gurobi_variables, model_ptr);

  // Adding the constraint for each value involved
  for(auto each_val : partial_values)
    model_ptr->addConstr(gurobi_variables[each_val.first], GRB_EQUAL, each_val.second,
                         "value assignment constraint");

  GRBLinExpr objective_expr = 0;
  model_ptr->setObjective(objective_expr, GRB_MAXIMIZE);
  model_ptr->optimize();
  model_ptr->update();

  string s = "./Gurobi_file_created/Finding_undetermined_values.lp";
  model_ptr->write(s);

  // Get a feasible solution
  if(model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
  {
    partial_values.clear();
    for(auto & var : gurobi_variables)
      partial_values[var.first] = var.second.get(GRB_DoubleAttr_X);
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
  {
      partial_values.clear();
  }
  else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
  {
    model_ptr->set(GRB_IntParam_DualReductions, 0);
    model_ptr->update();
    model_ptr->optimize();
    if( model_ptr->get(GRB_IntAttr_Status) == GRB_OPTIMAL )
    {
      partial_values.clear();
      for(auto & var : gurobi_variables)
        partial_values[var.first] = var.second.get(GRB_DoubleAttr_X);
    }
    else if(model_ptr->get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
      partial_values.clear();
    else
      partial_values.clear();
  }
  else
      partial_values.clear();


  // Delete the Gurobi stuff
  if(model_ptr)
    delete model_ptr;
  if(env_ptr)
    delete env_ptr;

  assert( (partial_values.size() == dimension_indices.size())
           || (partial_values.size() == 0) );

}

void polyhedral_abstraction :: tighten_polyhedron(
  region_constraints & input_polyhedron,
  region_constraints & starting_polyhedron,
  computation_graph & current_graph,
  region_constraints & output_region
)
{
  if(verbose_poly)
    cout << "\t Enters tightening of polyhedron " << endl;
  // Using the linear inequality data class, as a way to store equality stuff
  // as well.

  output_region.clear();
  output_region = starting_polyhedron;

  uint32_t index, vertex_sample_index;

  linear_inequality optimal_direction;
  vector< linear_inequality > lines;

  _point_ new_contact_point, intersection_point, input_assignment;
  vector< _point_ > contact_points, vectorized_vertices;
  set< _point_ > vertices;

  double optimal_value, score;
  int space_dim = starting_polyhedron.get_space_dimension();

  sherlock sherlock_instance(current_graph);
  set<_point_> intersection_point_collection;

  output_region.get_contact_points(contact_points);
  output_region.get_content(lines);

  sample_vertex(output_region, contact_points, vertices);
  vectorized_vertices.clear();
  for(auto each_vertex : vertices)
    vectorized_vertices.push_back(each_vertex);

  index = 1;
  while((index <= trial_count) &&
       (index <= vectorized_vertices.size()))
  {

    // vertex_sample_index = generate_random_int(vectorized_vertices.size(),index) - 1;
    vertex_sample_index = index - 1;
    intersection_point = vectorized_vertices[vertex_sample_index];

    if(check_if_point_in_set(intersection_point, intersection_point_collection))
    {
        index++;
        continue;
    }
    else
      intersection_point_collection.insert(intersection_point);

    if(verbose_poly)
      cout << "\t Starting to explore an intersection point, current index - "
      << vertex_sample_index << endl;

    if(output_region.check(intersection_point) &&
       !sherlock_instance.check_satisfaction(input_polyhedron, intersection_point,
                                          input_assignment))
    {

      output_region.get_contact_points(contact_points);
      optimal_direction = find_direction(contact_points, intersection_point);

      if(verbose_poly)
        cout << "\t The intersection point is legit. Current index - "
        << vertex_sample_index << endl;

      if(!optimal_direction.empty())
      {
        if(verbose_poly)
          cout << "\t Found an optimal direction Current index - "
            << vertex_sample_index << endl;

        optimal_direction.normalize();
        optimize(input_polyhedron, optimal_direction, current_graph,
                new_contact_point, optimal_value);
        optimal_direction.negate();
        optimal_direction.update_bias(optimal_value);
        score = -optimal_direction.evaluate(intersection_point);
        if(score > improv_delta)
        {
          if(verbose_poly)
            cout << "\t Score - " << score << " . Current index - "
            << vertex_sample_index << endl;
          output_region.add_direction_and_contact_point(optimal_direction,
             new_contact_point);
        }

      }
    }
    else
    {
      if(!output_region.check(intersection_point))
      {
        if(verbose_poly)
          cout << "\t Vertex point rejected because it's not in the output set " << endl;
      }
      else if(sherlock_instance.check_satisfaction(input_polyhedron, intersection_point,
                                        input_assignment))
       {
         if(verbose_poly)
          cout << "\t Vertex point rejected because it's not a bad point " << endl;
       }
    }
    index++;
  }

}

void polyhedral_abstraction :: split_polyhedron(
  const region_constraints & input_polyhedron,
  const region_constraints & output_polyhedron,
  pair< region_constraints, region_constraints > & result_polyhedrons
)
{


}

void polyhedral_abstraction :: compute_polyhedrons(
  vector < region_constraints > & input_polyhedrons,
  computation_graph & neural_network,
  set < uint32_t > & current_output_neurons,
  vector < region_constraints > & all_polyhedrons
)
{

  all_polyhedrons.clear();
  // Checking if the input is too small
  for(auto each_polyhedron : input_polyhedrons)
  {
    double max_width = -1e30, limit_val_1, limit_val_2;
    map< uint32_t , pair< double, double > > rectangle_size;
    each_polyhedron.overapproximate_polyhedron_as_rectangle(rectangle_size);

    for(auto each_limit : rectangle_size)
    {
      limit_val_2 = each_limit.second.second;
      limit_val_1 = each_limit.second.first;
      if(max_width < abs(limit_val_2 - limit_val_1))
        max_width = abs(limit_val_2 - limit_val_1);
    }

    if(max_width < sherlock_parameters.MILP_tolerance)
    {
      assert(input_polyhedrons.size() == 1);
      region_constraints result_poly;
      _point_ sample, output;
      each_polyhedron.return_sample(sample, 19);
      neural_network.evaluate_graph(sample, output);
      rectangle_size.clear();
      for(auto each_val : output)
        rectangle_size[each_val.first] = make_pair(
          output[each_val.first], output[each_val.first]);

      result_poly.create_region_from_interval(rectangle_size);
      all_polyhedrons.push_back(result_poly);
      return;
    }
  }

  // To be removed when you have multiple ones
  assert(input_polyhedrons.size() == 1);

  region_constraints current_polyhedron, result_polyhedron;
  // input_poly = input_polyhedrons[0];

  // Building the region for axis parallel directions
  double max, min, candidate_max, candidate_min;
  map< uint32_t, pair< _point_, _point_> > contact_points;
  _point_ max_point, min_point, max_output, min_output;
  _point_ candidate_max_point, candidate_min_point;

  map< uint32_t , pair< double, double > > interval_limits;
  sherlock sherlock_instance(neural_network);

  max = -1e30; min = 1e30;

  for(auto each_output_neuron : current_output_neurons)
  {
    for(auto input_poly : input_polyhedrons)
    {
      sherlock_instance.optimize_node_with_witness(
        each_output_neuron, true, input_poly, candidate_max, candidate_max_point);
      sherlock_instance.optimize_node_with_witness(
        each_output_neuron, false, input_poly, candidate_min, candidate_min_point);

      if(candidate_max > max)
      {
        max = candidate_max;
        max_point = candidate_max_point;
      }
      if(candidate_min < min)
      {
        min = candidate_min;
        min_point = candidate_min_point;
      }
    }


    interval_limits[each_output_neuron] = make_pair(min, max);

    neural_network.evaluate_graph(min_point, min_output);
    neural_network.evaluate_graph(max_point, max_output);

    contact_points[each_output_neuron] = make_pair(min_output, max_output);

  }

  current_polyhedron.clear();
  current_polyhedron.create_region_from_interval(interval_limits, contact_points);


  if(sherlock_parameters.find_extra_directions)
  {
    tighten_polyhedron(input_polyhedrons[0], current_polyhedron, neural_network, result_polyhedron);
  }
  else
  {
    result_polyhedron = current_polyhedron;
  }

  all_polyhedrons.clear();
  all_polyhedrons.push_back(result_polyhedron);

}

void polyhedral_abstraction :: propagate_polyhedrons(
  computation_graph & neural_network,
  region_constraints & input_polyhedron,
  region_constraints & output_polyhedron,
  set< uint32_t >& output_indices
)
{

  set < uint32_t > current_input_neurons, current_output_neurons, sub_graph_inputs,
                    input_indices;
  vector< uint32_t > input_neurons, output_neurons;

  vector<int> vec_indices = input_polyhedron.get_input_indices();
  for(auto index : vec_indices)
    input_indices.insert(index);

  current_input_neurons = input_indices;
  current_output_neurons = input_indices;

  vector < region_constraints > current_input_polyhedrons, current_output_polyhedrons;
  computation_graph sub_graph;

  current_input_polyhedrons.push_back(input_polyhedron);

  int layer_number = 1;
  while(!check_subset(output_indices, current_output_neurons) )
  {
    if(verbose_poly)
      cout << "At layer number - " << layer_number << endl;

    neural_network.return_next_layer_from_set(current_input_neurons,
                    current_output_neurons);

    // So basically there is nothing interesting left in the network
    // we just burn our way through the rest of it
    if(current_output_neurons.empty())
      current_output_neurons = output_indices;

    if(verbose_poly)
      cout << "Number of output neurons - " << current_output_neurons.size() << endl;

    neural_network.extract_graph(current_output_neurons, current_input_neurons, sub_graph);

    compute_polyhedrons(current_input_polyhedrons, sub_graph, current_output_neurons,
                        current_output_polyhedrons);

    if(current_output_polyhedrons[0].get_space_dimension() == 2)
    print_polyhedron_using_python(current_input_polyhedrons[0], sub_graph,
        current_output_polyhedrons[0], "./Plots/play_poly_" + to_string(layer_number)+".py");


    current_input_polyhedrons = current_output_polyhedrons;
    current_input_neurons = current_output_neurons;

    layer_number++;
  }

  // For single polyhedron case
  output_polyhedron = current_output_polyhedrons[0];
  return;
}

uint32_t polyhedral_abstraction :: pick_dimension_to_increment(
  vector< linear_inequality > & lines_sat,
  set < uint32_t > & determined_dimensions,
  set < uint32_t > & undetermined_dimensions,
  _point_ & current_point,
  int seed
)
{
  seed += 1;

  assert(!lines_sat.empty());
  assert(!determined_dimensions.empty());
  assert(!current_point.empty());
  uint32_t trial_index, return_val = 0, dim_index_picked;

  set < uint32_t > copy_det_dim, copy_of_undet_dim;
  map < uint32_t, double > random_vector;
  int trial_count = lines_sat[0].get_dim();
  trial_index = 0;
  _point_ partial_values, copy_of_init_point;

  for(;trial_index < (2 * trial_count); trial_index++)
  {
    copy_det_dim = determined_dimensions;
    copy_of_undet_dim = undetermined_dimensions;
    copy_of_init_point = current_point;

    // Randomly drop something from the determined dimensions
    dim_index_picked = generate_random_int_from_set(copy_det_dim, seed + trial_index);
    copy_det_dim.erase(dim_index_picked);
    copy_of_undet_dim.insert(dim_index_picked);


    // Pick a vector for the remaining dimensions in determined dimensions
    return_random_vector(seed + trial_index, copy_det_dim, random_vector);
    for(auto & each_term : random_vector)
    each_term.second *= scaling_factor;


    // Increment in the direction picked
    increment_point_in_direction(copy_of_init_point, random_vector);
    for(auto dim : copy_of_undet_dim)
    if(copy_of_init_point.find(dim) != copy_of_init_point.end())
    copy_of_init_point.erase(dim);

    // See if the undetermined dimensions can be computed
    find_the_undetermined_values(lines_sat, copy_of_init_point);

    if(copy_of_init_point.size() ==
      (determined_dimensions.size() + undetermined_dimensions.size()))
    {
        return_val = dim_index_picked;
        break;
    }
  }

  if(return_val <= 0)
  {
    return 0;
  }

  return return_val;

}

bool check_subset(
  set < uint32_t > & left_set,
  set < uint32_t > & right_set
)
{
  assert(!right_set.empty());


  for(auto each_number : left_set)
  {
    if(right_set.find(each_number) == right_set.end())
      return false;
  }

  return true;
}

bool check_subset(
  vector < uint32_t > & left_set,
  set < uint32_t > & right_set
)
{
  assert(!right_set.empty());
  assert(left_set.size() <= right_set.size());
  for(auto each_number : left_set)
  {
    if(right_set.find(each_number) == right_set.end())
      return false;
  }
  return true;
}

void convert_vector_to_set(
  vector < uint32_t > & vector_in,
  set < uint32_t > & set_out
)
{
  set_out.clear();

  for(auto each_element : vector_in)
    set_out.insert(each_element);

}

bool check_if_point_in_set(_point_& candidate, set< _point_ > & collection)
{
  for(_point_ each_point : collection)
  {
    if(_test_equal_(candidate, each_point))
      return true;
  }

  return false;
}

bool _test_equal_ (_point_ & p1, _point_ & p2)
{
  for(auto coordinate : p1)
  {
    if(p2.find(coordinate.first) != p2.end())
    {
      if( abs(p2[coordinate.first] - coordinate.second) > equal_thresh)
        return false;
    }
  }

  return true;
}

void add_vertices(region_constraints & region,
                     set< _point_ > & vertices)
{
  map< uint32_t, pair< double, double> > interval;
  region.overapproximate_polyhedron_as_rectangle(interval);

  int dimension = region.get_space_dimension();
  _point_ candidate_point;

  vector< linear_inequality > lines;
  region.get_content(lines);
  vector< uint32_t > dimensions_involved;
  lines[0].get_the_dimension_indices(dimensions_involved);
  // set< _point_ > dimension_indices;
  // convert_vector_to_set(dimensions_involved, dimension_indices);

  // Return dimension number of vertex points
  int index = 0;
  while(index < dimension )
  {

    candidate_point.clear();
    for(auto each_dim : dimensions_involved)
    {
      if(generate_random_int(2, each_dim + index + 19) == 1) // lower limit
      {
        candidate_point[each_dim] = interval[each_dim].first;
      }
      else
      {
        candidate_point[each_dim] = interval[each_dim].second;
      }

    }

    if(!check_if_point_in_set(candidate_point, vertices))
      vertices.insert(candidate_point);
    index++;
  }

}

void test_poly_abstr_simple(computation_graph & CG)
{
  map<uint32_t , double > inputs, outputs;
  inputs[1] = 1.0;
  inputs[2] = 6.0;
  uint32_t output_index = 38;

  set < uint32_t > input_indices, output_indices;
  input_indices.insert(1);
  input_indices.insert(2);
  output_indices.insert(output_index);

  CG.evaluate_graph(inputs, outputs);
  cout << "Output - " << outputs[output_index] << endl;

  map< uint32_t, pair< double, double > > input_interval;
  input_interval[1] = make_pair(0,5);
  input_interval[2] = make_pair(0,5);
  region_constraints input_polyhedron, output_polyhedron;
  input_polyhedron.create_region_from_interval(input_interval);

  polyhedral_abstraction sherlock_poly;
  sherlock_poly.propagate_polyhedrons(CG, input_polyhedron, output_polyhedron,
    output_indices);

  cout << " ------ Output polyhedron computed ----- " << endl;
  output_polyhedron.print();

  sherlock sherlock_instance(CG);
  pair< double, double > output_range;
  sherlock_instance.compute_output_range(output_index, input_polyhedron, output_range);
  cout << "Computed output range by Sherlock = [" <<
	output_range.first << " , " << output_range.second << " ] " << endl;

  sherlock_instance.compute_output_range_by_sampling(input_polyhedron, output_index, output_range, 1000);
	cout << "Computed output range from random sampling = [" <<
	output_range.first << " , " << output_range.second << " ] " << endl;

}
void drone_example(computation_graph & CG)
{
  map<uint32_t , double > inputs, outputs;
  inputs[1] = 0.0;
  inputs[2] = -3.2342;
  inputs[3] = 0.0;
  inputs[4] = -0.1815;
  inputs[5] = -0.2004;
  inputs[6] = 0.6001;
  inputs[7] = -0.4803;
  inputs[8] = -0.1372;

  CG.evaluate_graph(inputs, outputs);
  // Expected: 0.6991
  cout << "Output - " << outputs[1009] << endl;
  // Expected: -0.0676
  cout << "Output - " << outputs[1010] << endl;


  inputs[1] = 0.0;
  inputs[2] = 2.3855;
  inputs[3] = 0.0;
  inputs[4] = -2.5104;
  inputs[5] = -0.7279;
  inputs[6] = 0.1594;
  inputs[7] = 0.7386;
  inputs[8] = 0.0997;

  CG.evaluate_graph(inputs, outputs);
  // Expected : 0.9333
  cout << "Output - " << outputs[1009] << endl;
  // Expected : -0.8953
  cout << "Output - " << outputs[1010] << endl;


  inputs[1] = 0.0;
  inputs[2] = -0.3683;
  inputs[3] = 0.0;
  inputs[4] = 3.5940;
  inputs[5] = 0.0265;
  inputs[6] = -0.8481;
  inputs[7] = -0.1964;
  inputs[8] = -0.5202;

  CG.evaluate_graph(inputs, outputs);
  // Expected : -0.3540
  cout << "Output - " << outputs[1009] << endl;
  // Expected : 0.9458
  cout << "Output - " << outputs[1010] << endl;


  inputs[1] = 0.0;
  inputs[2] = -3.9443;
  inputs[3] = 0.0;
  inputs[4] = 2.0550;
  inputs[5] = 0.8896;
  inputs[6] = -0.0215;
  inputs[7] = -0.0183;
  inputs[8] = -0.3246;

  CG.evaluate_graph(inputs, outputs);
  // Expected: -1.3783
  cout << "Output - " << outputs[1009] << endl;
  // Expected: 0.5207
  cout << "Output - " << outputs[1010] << endl;


  inputs[1] = 0.0;
  inputs[2] = 4.1208;
  inputs[3] = 0.0;
  inputs[4] = -1.8342;
  inputs[5] = -0.2205;
  inputs[6] = -0.1922;
  inputs[7] = -0.5166;
  inputs[8] = -0.8071;

  CG.evaluate_graph(inputs, outputs);
  // Expected: 1.1930
  cout << "Output - " << outputs[1009] << endl;
  // Expected: -0.6009
  cout << "Output - " << outputs[1010] << endl;

}

void return_random_vector(
  int seed,
  set< uint32_t > & dimensions,
  map < uint32_t, double > & random_vector
)
{
  assert(! dimensions.empty());
  random_vector.clear();
  uint32_t random_int, length = 100;
  float random_0_1_float;
  map< int, double > r_int_vector;

  for(auto each_dim : dimensions)
  {
    random_int = generate_random_int(length, each_dim + 13 * seed);
    r_int_vector[each_dim] = 0.01 +
     (double)random_int - ((double)length/2.0);
  }

  linear_inequality random_ineq;
  random_ineq.update(r_int_vector);

  random_ineq.normalize();
  r_int_vector = random_ineq.get_content();
  for(auto each_term : r_int_vector)
    random_vector[each_term.first] = each_term.second;

}

int equations_satisfied(
  _point_ sample_point,
  vector< linear_inequality > & lines,
  vector < linear_inequality > & lines_sat
)
{
  int count = 0;
  lines_sat.clear();

  for(auto each_line : lines)
  {

    if(abs(each_line.evaluate(sample_point)) < (2 * scaling_factor))
    {
      lines_sat.push_back(each_line);
      count++;
    }
  }

  return count;
}

int equations_satisfied(
  _point_ sample_point,
  vector< linear_inequality > & lines
)
{

  int count = 0;
  for(auto each_line : lines)
    if(abs(each_line.evaluate(sample_point)) < (2 * scaling_factor))
      count++;

  return count;
}
