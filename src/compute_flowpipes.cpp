#include "compute_flowpipes.h"

bool debug_compute_flowpipes = true;
// Here is the assumption while doing the reachability analysis :
// the 0th variable is 't', followed by all the state space variables ,
// then the control inputs, in the right order

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
)
{
  map<double, vector< pair <double, double > > > sets;
  map< uint32_t, pair< double, double > > input_interval;

  result.clear();
  // Preparing the initial flowpipe from which the computation needs to start
  // depending on the initial interval received :
  int max_linear_pieces = 0;
  double max_difference = 0;
  Interval init_u, intZero;
  Flowpipe initial_set(initial_interval_vector, intZero);
  // the flowpipe that keeps the overapproximation at the end of a time horizon
	Flowpipe fp_last;
  // the symbolic remainder
  Symbolic_Remainder symb_rem(initial_set);


  int control_dim = output_indices.size();
  int problem_dim = initial_interval_vector.size() - control_dim; // since it has controls


  std :: chrono :: steady_clock::time_point t1 = std :: chrono :: steady_clock::now();
  std :: chrono :: steady_clock::time_point start_time;
  std :: chrono :: steady_clock::time_point end_time;

  std:: chrono::duration< double > time_span;
  double time_in_regression, time_in_pwl_construction, time_in_sherlock, time_in_flowstar;
  double total_time_in_regression, total_time_in_PWL_construction, total_time_in_sherlock, total_time_in_flowstar;
  total_time_in_regression = 0;
  total_time_in_PWL_construction = 0;
  total_time_in_sherlock = 0;
  total_time_in_flowstar = 0;

  vector< pair<double, double > > hyper_rectangle;
  region_constraints current_region;

  for(int k = 0; k < no_of_steps; ++k)
  {
    std::vector<Interval> NN_input;
    initial_set.intEvalNormal(NN_input, crs.step_end_exp_table, crs.cutoff_threshold);

    hyper_rectangle.clear();
    input_interval.clear();
    assert(problem_dim == input_indices.size());

    for(int j = 0; j < input_indices.size(); j++)
    {
      input_interval[input_indices[j]] = make_pair(NN_input[j].inf(), NN_input[j].sup());
      cout << " [" << input_interval[input_indices[j]].first << " , "
      << input_interval[input_indices[j]].second << " ] ";
      hyper_rectangle.push_back(make_pair(input_interval[input_indices[j]].first,
         input_interval[input_indices[j]].second));
    }
    sets[k * crs.step * no_of_flowpipes] = hyper_rectangle;
    cout << endl;

    current_region.create_region_from_interval(input_interval);

    // Get the taylor model for each control input

    for(int index  = 0; index < output_indices.size(); index++)
    {
      uint32_t output_index = output_indices[index];
      TaylorModel tm_u; // Sorry, for creating a new Taylor model everytime, this was not required.
                         // But, I don't trust what Flow* does internally, feel free to take this outside the loop
                         // if you are confident with Flow* more than I am !


      compute_taylor_models_for_neural_network(input_interval, initial_set, crs, control_graph,
      input_indices, output_index, input_indices.size(), polynomial_degree, tm_u, time_in_pwl_construction,
      time_in_regression, time_in_sherlock, max_linear_pieces, max_difference);

      total_time_in_regression += time_in_regression;
      total_time_in_PWL_construction += time_in_pwl_construction;
      total_time_in_sherlock += time_in_sherlock;
      initial_set.tmvPre.tms[problem_dim + index] = tm_u;

    }

    printf("Step %d\n", k);
  	start_time = std :: chrono :: steady_clock::now();
    bool res = plant.reach_symbolic_remainder(result, fp_last, symb_rem, crs, initial_set, no_of_flowpipes, 200);

    if(res)
    {
      initial_set = fp_last;
    }
    else
    {
      printf("Terminated due to too large overestimation.\n");
      break;
    }

    end_time = std :: chrono :: steady_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
    total_time_in_flowstar += time_span.count();

  }

  std :: chrono :: steady_clock::time_point t2 = std::chrono::steady_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

  timing_information.clear();
  timing_information["total_execution_time"] = time_span.count();
  timing_information["time_in_regression"] = total_time_in_regression/time_span.count();
  timing_information["time_in_pwl_construction"] = total_time_in_PWL_construction/time_span.count();
  timing_information["time_in_sherlock"] = total_time_in_sherlock/time_span.count();
  timing_information["time_in_flowstar"] = total_time_in_flowstar/time_span.count();
  timing_information["max_linear_pieces"] = max_linear_pieces;
  timing_information["max_error_encountered"] = max_difference;

  plot_in_matlab(sets, filename_to_save, crs.step * no_of_flowpipes, variable_indices);
}

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
)
{

  // cout << "Domain dim = " << domainDim << endl; exit(0);


  std :: chrono :: steady_clock::time_point start_time;
  std :: chrono :: steady_clock::time_point end_time;
  std:: chrono::duration< double > time_span;

  vector< vector< unsigned int > > monomial_terms;
  vector< datatype > coefficients;

  unsigned int degree = polynomial_degree;
  int linear_piece_count;

  start_time = std :: chrono :: steady_clock::now();

  generate_polynomial_for_NN(controller_graph, output_index, degree,
    input_interval, monomial_terms, coefficients);
  int i,j,l;

  vector<my_monomial_t> polynomial;
  polynomial = create_polynomial_from_monomials_and_coeffs(monomial_terms, coefficients);

  if(debug_compute_flowpipes)
  {
    cout << " ---- Polynomial is the following ---- " << endl;
    int ind = 0;
    for(auto each_monomial : monomial_terms)
    {
      cout << "For monomial -- [ ";
      for(auto each_power : each_monomial)
        cout << each_power << "  ";
      cout << "]   ,   coefficient is " << coefficients[ind] << endl;
      ind++;
    }

  }

  end_time = std :: chrono :: steady_clock::now();

  time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
  time_in_regression = time_span.count();

  vector< vector< vector< vector< datatype > > > > region_descriptions;
  vector< vector < vector< datatype > > > linear_mapping;

  vector< PolynomialApproximator > decomposed_pwls;
  vector< double > lower_bounds;
  vector< double > upper_bounds;

  double tolerance = 0.01; //1e-5
  start_time = std :: chrono :: steady_clock::now();


  create_PWL_approximation(polynomial, input_interval, tolerance, region_descriptions,
    linear_mapping, decomposed_pwls, lower_bounds, upper_bounds, linear_piece_count);

  cout << "No of linear pieces computed - " << linear_piece_count << endl;

  if(linear_piece_count > max_linear_pieces)
  {
    max_linear_pieces = linear_piece_count;
  }

  end_time = std :: chrono :: steady_clock::now();
  time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
  time_in_pwl = time_span.count();

  start_time = std :: chrono :: steady_clock::now();
  vector< datatype > difference;
  sherlock sherlock_handler(controller_graph);


  sherlock_handler.return_interval_difference_wrt_PWL(input_interval, difference, output_index,
    decomposed_pwls, lower_bounds, upper_bounds);


  end_time = std :: chrono :: steady_clock::now();
  time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
  time_in_sherlock = time_span.count();


  datatype optima = compute_max_abs_in_a_vector(difference);
  if(optima > max_difference)
  {
    max_difference = optima;
  }
  cout << " Difference is : " << optima << endl;


  start_time = std :: chrono :: steady_clock::now();
  Polynomial poly_u;

  for(int i=0; i < monomial_terms.size(); ++i)
  {
    monomial_terms[i].insert(monomial_terms[i].begin(), 0);
    monomial_terms[i].push_back(0);

    Monomial monomial(coefficients[i], *((vector<int> *) &monomial_terms[i]));

    poly_u.monomials.push_back(monomial);
  }

  poly_u.reorder();

  TaylorModel tm_u_buffer;

  vector<Interval> polyRange_initial_set;
  initial_set.tmvPre.polyRangeNormal(polyRange_initial_set, crs.step_end_exp_table);

  poly_u.insert_normal(tm_u_buffer, initial_set.tmvPre, polyRange_initial_set,
    crs.step_end_exp_table, domainDim, crs.cutoff_threshold);

  Interval intError(-optima, optima);
  tm_u_buffer.remainder += intError;

  tm_u = tm_u_buffer;


}

void plot_in_matlab(
  map<double, vector< pair <double, double > > > & sets,
  string filename_to_save,
  double step,
  pair < int, int > variable_indices
)
{
  string buffer, x_coord, y_coord, x_dist, y_dist;
  double min, max;
  double min_x_val, min_y_val, max_x_val, max_y_val;
  min_x_val = 1e30;
  min_y_val = 1e30;
  max_x_val = -1e30;
  max_y_val = -1e30;
  int dimension_index = 0;

  ofstream file;
  file.open(filename_to_save.c_str());

  map< double, vector< pair < double, double > > > :: iterator it = sets.begin();
  int i = 0;
  while(i < sets.size())
  {
    buffer.clear();
    // Plot the first box
    buffer = "rectangle(\'Position\',[" ;
    if(variable_indices.first < 0)
    {
      min = it->first;
      max = min + step;
      x_coord = to_string(min);
      x_dist = to_string( step );
    }
    else
    {
      dimension_index = variable_indices.first;
      min = it->second[dimension_index].first ;
      max = it->second[dimension_index].second ;
      x_coord = to_string(min);
      assert( (max - min) > 0);
      x_dist = to_string(max-min);
    }

    if(min < min_x_val)
    {
      min_x_val = min;
    }
    if(max > max_x_val)
    {
      max_x_val = max;
    }

    dimension_index = variable_indices.second;
    y_coord = to_string(it->second[dimension_index].first);
    assert(((it->second[dimension_index].second) - (it->second[dimension_index].first)) > 0);
    y_dist = to_string((it->second[dimension_index].second) - (it->second[dimension_index].first));

    min = it->second[dimension_index].first;
    max = it->second[dimension_index].second;

    buffer = buffer + " " + x_coord + " " + y_coord + " " + x_dist + " " + y_dist + " ";
    buffer += " ]);\n";

    file << buffer;

    if(min < min_y_val)
    {
      min_y_val = min;
    }
    if(max > max_y_val)
    {
      max_y_val = max;
    }

    i++;
    it++;
  }

  buffer.clear();
  buffer = "axis([";
  buffer += (to_string(min_x_val) + " " + to_string(max_x_val) + " " + to_string(min_y_val - 1.0) + " " + to_string(max_y_val + 1.0) );
  buffer += "]);\n";
  file << buffer;

  file.close();
}



void generate_polynomial_for_NN(
  computation_graph controller_graph,
  uint32_t output_index,
  int max_degree,
  map< uint32_t, pair< double, double > > input_interval,
  vector< vector< unsigned int > >& monomial_terms,
  vector< datatype >& coefficients
)
{
  region_constraints input_constraints;
  input_constraints.create_region_from_interval(input_interval);

  monomial_terms.clear();
  coefficients.clear();

  if(!max_degree)
  {
    cout << " You kidding me ?  Asking for a 'constant' as an approximation to a neural network !! " << endl;
    cout << "Change the max degree and try again ... exiting.. " << endl;
    exit(0);
  }

  unsigned int i, j, k, no_of_vars;

  vector< uint32_t > input_nodes, output_nodes;
  controller_graph.return_id_of_input_output_nodes(input_nodes, output_nodes);
  no_of_vars = input_nodes.size();

  vector< unsigned int > monomial_term;

  vector< vector< unsigned int > > monomial_degrees_linear;

  generate_monomials(-1, 1, no_of_vars, monomial_degrees_linear);

  unsigned int no_of_monomial_degrees_linear = monomial_degrees_linear.size();

  map < uint32_t, double > sample_input, sample_output;
  vector< double > values;

  datatype result_term;

  datatype monomial_val;

  vector< vector < datatype > > data_matrix;
  vector< datatype > desired_right_side_vector;
  vector< datatype > row_of_a_matrix;

  data_matrix.clear();
  desired_right_side_vector.clear();
  i = 0;
  while(i < (no_of_vars * sherlock_parameters.mult_fac_for_regression))
  {
    assert(input_constraints.return_sample(sample_input, i));
    controller_graph.evaluate_graph(sample_input, sample_output);
    result_term = sample_output[output_index];

    row_of_a_matrix.clear();

    // Compute the monomials
    j = 0 ;
    while(j < no_of_monomial_degrees_linear)
    {
      monomial_term = monomial_degrees_linear[j] ;

      values.clear();
      for(auto value : sample_input)
        values.push_back(value.second);

      compute_monomials_for_the_input(monomial_term, values, monomial_val);
      row_of_a_matrix.push_back(monomial_val);
      j++;
    }

    data_matrix.push_back(row_of_a_matrix);
    desired_right_side_vector.push_back(result_term);
    i++;
  }


  MatrixXf A = MatrixXf::Random((int)(no_of_vars * sherlock_parameters.mult_fac_for_regression),
                                 no_of_monomial_degrees_linear);
  VectorXf b = VectorXf::Random((int)(no_of_vars * sherlock_parameters.mult_fac_for_regression));

  i = 0;
  while(i < (no_of_vars * sherlock_parameters.mult_fac_for_regression))
  {
    j = 0;
    while(j < no_of_monomial_degrees_linear)
    {
      A(i,j) = data_matrix[i][j];
      j++;
    }
    b(i) = desired_right_side_vector[i];
    i++;
  }


  // Do the  linear regression

  VectorXf coeff_vector_linear =  VectorXf::Random((int)no_of_monomial_degrees_linear);
  coeff_vector_linear = A.jacobiSvd(ComputeThinU | ComputeThinV).solve(b) ;

  vector< vector< unsigned int > > monomial_degrees_non_linear;
  generate_monomials(1, max_degree, no_of_vars, monomial_degrees_non_linear);
  unsigned int no_of_monomial_degrees_non_linear = monomial_degrees_non_linear.size();

  // Generate the difference betweeen the linear regression and the actual functions
  if(max_degree > 1)
  {
    datatype linear_prediction;
    datatype difference;

    data_matrix.clear();
    desired_right_side_vector.clear();

    i = 0;
    while(i < (no_of_vars * sherlock_parameters.mult_fac_for_regression))
    {
      assert(input_constraints.return_sample(sample_input, i));
      controller_graph.evaluate_graph(sample_input, sample_output);
      result_term = sample_output[output_index];


      values.clear();
      for(auto value : sample_input)
        values.push_back(value.second);

      linear_prediction = compute_prediction_for_linear_regression(coeff_vector_linear, values);
      difference = result_term - linear_prediction;

      // cout << "difference = " << difference << endl;

      desired_right_side_vector.push_back(difference);

      row_of_a_matrix.clear();
      // Compute the monomials
      j = 0 ;
      while(j < no_of_monomial_degrees_non_linear)
      {
        monomial_term = monomial_degrees_non_linear[j] ;

        values.clear();
        for(auto value : sample_input)
          values.push_back(value.second);
        compute_monomials_for_the_input(monomial_term, values, monomial_val);

        row_of_a_matrix.push_back(monomial_val);
        j++;
      }

      data_matrix.push_back(row_of_a_matrix);
      i++;
    }

    MatrixXf A_for_non_linear = MatrixXf::Random((int)(no_of_vars * sherlock_parameters.mult_fac_for_regression),
                                no_of_monomial_degrees_non_linear);
    VectorXf b_for_non_linear = VectorXf::Random((int)(no_of_vars * sherlock_parameters.mult_fac_for_regression));

    i = 0;
    while(i < (no_of_vars * sherlock_parameters.mult_fac_for_regression))
    {
      j = 0;
      while(j < no_of_monomial_degrees_non_linear)
      {
        A_for_non_linear(i,j) = data_matrix[i][j];
        j++;
      }
      b_for_non_linear(i) = desired_right_side_vector[i];
      i++;
    }


    // Do the  non-linear regression

    VectorXf coeff_vector_non_linear =  VectorXf::Random((int)no_of_monomial_degrees_non_linear);
    coeff_vector_non_linear = A_for_non_linear.jacobiSvd(ComputeThinU | ComputeThinV).solve(b_for_non_linear) ;

    auto x = ridge(A_for_non_linear, b_for_non_linear, 1e-1);
    coeff_vector_non_linear = x;

    coefficients.clear();
    i = 0;
    while(i < no_of_monomial_degrees_linear)
    {
      coefficients.push_back(coeff_vector_linear(i));
      i++;
    }

    i = 0;
    while(i < no_of_monomial_degrees_non_linear)
    {
      coefficients.push_back(coeff_vector_non_linear(i));
      i++;
    }

    monomial_terms.clear();
    i = 0;
    while(i < no_of_monomial_degrees_linear)
    {
      monomial_terms.push_back(monomial_degrees_linear[i]);
      i++;
    }
    i = 0;
    while(i < no_of_monomial_degrees_non_linear)
    {
      monomial_terms.push_back(monomial_degrees_non_linear[i]);
      i++;
    }

    return;
  }


  coefficients.clear();
  i = 0;
  while(i < no_of_monomial_degrees_linear)
  {
    coefficients.push_back(coeff_vector_linear(i));
    i++;
  }


  monomial_terms.clear();
  i = 0;
  while(i < no_of_monomial_degrees_linear)
  {
    monomial_terms.push_back(monomial_degrees_linear[i]);
    i++;
  }

}
