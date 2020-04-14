#include "polynomial_computations.h"

std::vector <my_monomial_t> create_polynomial_from_monomials_and_coeffs(
          vector< vector< unsigned int > > monomial_terms,
          vector< double > coefficients
)
{
  int no_of_vars = monomial_terms[0].size();
  std::vector< int > powers(no_of_vars);
  double coeffs;

  if(coefficients.size() != monomial_terms.size())
  {
    cout << "No of monomials and no of coefficients do not match ! " << endl;
    cout << "Exiting... " << endl;
    exit(0);

  }

  int no_of_monomial_terms = monomial_terms.size();

  int i,j;
  std::vector< my_monomial_t > return_poly;
  std::pair<double, vector<int> > single_term;

  i = 0;
  while(i < no_of_monomial_terms)
  {
    coeffs = coefficients[i];
    j = 0;
    while(j < no_of_vars)
    {
      powers[j] = monomial_terms[i][j];
      j++;
    }

    single_term = std::make_pair(coeffs, powers);
    return_poly.push_back(single_term);

    i++;
  }

  return return_poly;
}

void compute_monomials_for_the_input(
  vector< unsigned int > monomial_powers,
  vector< datatype > values,
  datatype & result
)
{
  if((!monomial_powers.size()) || (monomial_powers.size() != values.size()))
  {
    cout << "Monomial powers received is empty.. Exiting .. " << endl;
    exit(0);
  }
  unsigned int i, j , k;
  result = 1.0;
  i = 0;
  while(i < monomial_powers.size())
  {
    result *= pow(values[i], monomial_powers[i]);
    i++;
  }

}

void generate_monomials(
  int min_degree,
  int max_degree,
  int no_of_vars,
  vector< vector< unsigned int > >& monomials
)
{


  vector< vector< unsigned int > > monomial_degrees;
  vector< unsigned int > monomial_term;

  vector< unsigned int > iterators(no_of_vars,0);
  unsigned int i, j , k;

  // Max limit -- definitely an over approximation is being computed
  int max_limit = 1;
  i = 0;
  while(i < no_of_vars)
  {
    max_limit *= (max_degree+1);
    i++;
  }

  int sum = 0;
  // Generate monomial degrees and store the result
  i = 0;
  while(i < max_limit)
  {
    // Compute the sum of the iterators
    sum = 0;
    j = 0;
    while(j < no_of_vars)
    {
      sum += iterators[j];
      j++;
    }

    if((sum <= max_degree) && (sum > min_degree))
    {
      monomial_degrees.push_back(iterators);
    }

    iterators[0]++;
    j = 0;
    while(j < no_of_vars-1)
    {
      if(iterators[j] == (max_degree+1))
      {
        iterators[j] = 0;
        iterators[j+1]++;
      }
      j++;
    }
    i++;
  }

  monomials = monomial_degrees;

}


void create_PWL_approximation(std::vector< my_monomial_t> input_polynomial,
                              std::vector< std::vector < double > > input_region,
                              double tolerance_limit,
                              std::vector< std::vector< std::vector< std::vector< double > > > >& region_descriptions,
                              std::vector< std::vector< std::vector< double > > >& linear_mapping)

{

  if(input_region[0].size() != 2)
  {
    std::cout << "Input region expects interval bases descriptions for the input region.. " << std::endl;
    std::cout << "Exiting .. " << std::endl;
    exit(0);
  }
  int no_of_vars = input_region.size();

  std::vector < double > lower_bounds(no_of_vars);
  std::vector < double > upper_bounds(no_of_vars);
  std::vector<int> nsubs(no_of_vars, 10); // Choose 10 subdivisions along each axis

  region_descriptions.clear();
  linear_mapping.clear();

  vector<int> skip_list;
  vector<int> present_list;

  int i,j,k;
  i = 0;
  while(i < no_of_vars)
  {
    lower_bounds[i] = input_region[i][0];
    upper_bounds[i] = input_region[i][1];
    i++;
  }

  vector<PolynomialApproximator> res; // This is the vector that will store ther results
  vector< double > linear_map(no_of_vars+1);
  vector< vector< double > > region_bounds(no_of_vars);
	vector< vector < vector < double > > > region_bounds_for_a_monomial;
	vector< vector < double > > linear_mapping_for_a_monomial;

  double tol=0.0;
	region_descriptions.clear();

  piecewise_approximate_polynomial(no_of_vars, input_polynomial,lower_bounds, upper_bounds, tolerance_limit, nsubs, res);

  cout << "Size of res = " << res.size() << endl;
  for (PolynomialApproximator & pa: res)
	{
      // std::cout << "------ Polynomial Approximation ------ " << std::endl;
      // std::cout << "Polynomial is : " << pa.get_polynomial() << std::endl;
      // pa.pretty_print_linearization();
      std::vector< LinearPiece > current_linear_pieces = pa.get_linear_pieces();

			region_bounds_for_a_monomial.clear();
			linear_mapping_for_a_monomial.clear();

      cout << "Size of current_linear_pieces = " << current_linear_pieces.size() << endl;
      i = 0;
      while(i < current_linear_pieces.size())
      {

        // cout << " i = " << i << endl;
        NeuralRuleAnalysis :: LinearPiece _lin_piece = current_linear_pieces[i];

        // Upper limit encoding
        linear_map.clear();
        region_bounds.clear();

        skip_list.clear();
        present_list.clear();

        linear_map.resize(no_of_vars+1);
        region_bounds.resize(no_of_vars);
        j = 0;
        while(j < no_of_vars)
        {
          region_bounds[j].push_back(lower_bounds[j]);
          region_bounds[j].push_back(upper_bounds[j]);
          j++;
        }
        // std::cout << "<LinearExpr>" << std::endl;
        // std::cout << "\t" << (_lin_piece.tol_intvl+ _lin_piece.const_term);
        NeuralRuleAnalysis::MpfiWrapper tmp = _lin_piece.tol_intvl;
        double w = max(tmp.upper(), - tmp.lower());
        if (w >= tol){ tol = w; }
        linear_map[no_of_vars] = _lin_piece.const_term + tmp.upper();

        for (auto kv: _lin_piece.linear_expr)
				{
            // std::cout << "+ (" << kv.second << "* x_"<<kv.first << ")";
            present_list.push_back(kv.first);
            linear_map[kv.first] = kv.second;
        }
				// cout << "  " <<  _lin_piece.const_term << " ";
        // std::cout << std::endl;
        // std::cout << "</LinearExpr>" << std::endl;

        // std::cout << "<Box>" << std::endl;
        NeuralRuleAnalysis :: Box current_box = _lin_piece.b;
        std::map<int, double> center = current_box.get_center();
        // cout << current_box << endl;
        j = 0;
        while(j < present_list.size())
        {
          NeuralRuleAnalysis::MpfiWrapper bounds = current_box.get_bounds_for_input_var(present_list[j]);
          // cout << "For dimension = " << present_list[j] << " upper = " << bounds.upper() << " lower = " << bounds.lower() << endl;
          // cout << "And the center is " << center[present_list[j]] << endl;

          region_bounds[present_list[j]][0] = bounds.lower();
          region_bounds[present_list[j]][1] = bounds.upper();
          j++;
        }
        // cout << "Printing the center of the box : " << endl;
        // for (auto& x : center)
        // {
        //   cout << "For dimension = " << x.first << " mid point is " << x.second << endl;
        // }
        // std::cout << "</Box>" << std::endl;

        region_bounds_for_a_monomial.push_back(region_bounds);

        linear_mapping_for_a_monomial.push_back(linear_map);

        i++;
      }
      // std::cout << "TOLERANCE FOR MONOMIAL is " << tol << std::endl;

		region_descriptions.push_back(region_bounds_for_a_monomial);
		linear_mapping.push_back(linear_mapping_for_a_monomial);
  }

}

void create_PWL_approximation(std::vector< my_monomial_t> input_polynomial,
                              map< uint32_t, pair<double, double> > input_interval,
                              double tolerance_limit,
                              std::vector< std::vector< std::vector< std::vector< double > > > >& region_descriptions,
                              std::vector< std::vector< std::vector< double > > >& linear_mapping,
                              vector<PolynomialApproximator> & decomposed_pwls,
                              vector<double>  &lb,
                              vector<double>  &ub,
                              int& linear_piece_count
)
{

  vector< vector < double > > input_region;
  for(pair<uint32_t, pair< double, double > > _pairs_ : input_interval)
  {
    vector< double > _ranges_;
    _ranges_.push_back(_pairs_.second.first);
    _ranges_.push_back(_pairs_.second.second);

    input_region.push_back(_ranges_);
  }
  int no_of_vars = input_region.size();

  if(input_region[0].size() != 2)
  {
    std::cout << "Input region expects interval bases descriptions for the input region.. " << std::endl;
    std::cout << "Exiting .. " << std::endl;
    exit(0);
  }

  std::vector < double > lower_bounds(no_of_vars);
  std::vector < double > upper_bounds(no_of_vars);
  std::vector<int> nsubs(no_of_vars, 5); // Choose 10 subdivisions along each axis

  region_descriptions.clear();
  linear_mapping.clear();

  vector<int> skip_list;
  vector<int> present_list;

  lb.clear();
  ub.clear();
  int i,j,k;
  i = 0;
  while(i < no_of_vars)
  {
    lb.push_back(input_region[i][0]);
    ub.push_back(input_region[i][1]);
    i++;
  }

  vector<PolynomialApproximator> res; // This is the vector that will store ther results
  vector< double > linear_map(no_of_vars+1);
  vector< vector< double > > region_bounds(no_of_vars);
	vector< vector < vector < double > > > region_bounds_for_a_monomial;
	vector< vector < double > > linear_mapping_for_a_monomial;

  double tol= 0.0;
	region_descriptions.clear();

  piecewise_approximate_polynomial(no_of_vars, input_polynomial, lb, ub, tolerance_limit, nsubs, decomposed_pwls);


  // decomposed_pwls = res;
  // lb = lower_bounds;
  // ub = upper_bounds;

  linear_piece_count = 0;
  // cout << "Size of res = " << res.size() << endl;
  for (PolynomialApproximator & pa: decomposed_pwls)
	{
      // std::cout << "------ Polynomial Approximation ------ " << std::endl;
      // std::cout << "Polynomial is : " << pa.get_polynomial() << std::endl;
      // pa.pretty_print_linearization();
      std::vector< LinearPiece > current_linear_pieces = pa.get_linear_pieces();

			region_bounds_for_a_monomial.clear();
			linear_mapping_for_a_monomial.clear();

      // cout << "Number of linear pieces for current polynomial = " << current_linear_pieces.size() << endl;
      linear_piece_count += current_linear_pieces.size();
      i = 0;
      while(i < current_linear_pieces.size())
      {

        // cout << " i = " << i << endl;
        NeuralRuleAnalysis :: LinearPiece _lin_piece = current_linear_pieces[i];

        // Upper limit encoding
        linear_map.clear();
        region_bounds.clear();

        skip_list.clear();
        present_list.clear();

        linear_map.resize(no_of_vars+1);
        region_bounds.resize(no_of_vars);
        j = 0;
        while(j < no_of_vars)
        {
          region_bounds[j].push_back(lower_bounds[j]);
          region_bounds[j].push_back(upper_bounds[j]);
          j++;
        }
        // std::cout << "<LinearExpr>" << std::endl;
        // std::cout << "\t" << (_lin_piece.tol_intvl+ _lin_piece.const_term);
        NeuralRuleAnalysis::MpfiWrapper tmp = _lin_piece.tol_intvl;
        double w = max(tmp.upper(), - tmp.lower());
        if (w >= tol){ tol = w; }
        linear_map[no_of_vars] = _lin_piece.const_term + tmp.upper();

        for (auto kv: _lin_piece.linear_expr)
				{
            // std::cout << "+ (" << kv.second << "* x_"<<kv.first << ")";
            present_list.push_back(kv.first);
            linear_map[kv.first] = kv.second;
        }
				// cout << "  " <<  _lin_piece.const_term << " ";
        // std::cout << std::endl;
        // std::cout << "</LinearExpr>" << std::endl;

        // std::cout << "<Box>" << std::endl;
        NeuralRuleAnalysis :: Box current_box = _lin_piece.b;
        std::map<int, double> center = current_box.get_center();
        // cout << current_box << endl;
        j = 0;
        while(j < present_list.size())
        {
          NeuralRuleAnalysis::MpfiWrapper bounds = current_box.get_bounds_for_input_var(present_list[j]);
          // cout << "For dimension = " << present_list[j] << " upper = " << bounds.upper() << " lower = " << bounds.lower() << endl;
          // cout << "And the center is " << center[present_list[j]] << endl;

          region_bounds[present_list[j]][0] = bounds.lower();
          region_bounds[present_list[j]][1] = bounds.upper();
          j++;
        }
        // cout << "Printing the center of the box : " << endl;
        // for (auto& x : center)
        // {
        //   cout << "For dimension = " << x.first << " mid point is " << x.second << endl;
        // }
        // std::cout << "</Box>" << std::endl;

        region_bounds_for_a_monomial.push_back(region_bounds);

        linear_mapping_for_a_monomial.push_back(linear_map);

        i++;
      }
      // std::cout << "TOLERANCE FOR MONOMIAL is " << tol << std::endl;

		region_descriptions.push_back(region_bounds_for_a_monomial);
		linear_mapping.push_back(linear_mapping_for_a_monomial);
  }

}
