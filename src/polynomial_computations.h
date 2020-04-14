#ifndef polynomial_computations_h
#define polynomial_computations_h

#include "network_computation.h"

std::vector <my_monomial_t> create_polynomial_from_monomials_and_coeffs(
          vector< vector< unsigned int > > monomial_terms,
          vector< double > coefficients
);

void compute_monomials_for_the_input(
  vector< unsigned int > monomial_powers,
  vector< datatype > values,
  datatype & result
);

datatype compute_prediction_for_linear_regression(
  Eigen::VectorXf coeff_vector,
  vector< datatype > var_values
);

void generate_monomials(
  int min_degree,
  int max_degree,
  int no_of_vars,
  vector< vector< unsigned int > >& monomials
);

void create_PWL_approximation(
  std::vector< my_monomial_t> input_polynomial,
  std::vector< std::vector < double > > input_region,
  double tolerance_limit,
  std::vector< std::vector< std::vector< std::vector< double > > > >& region_descriptions,
  std::vector< std::vector< std::vector< double > > >& linear_mapping
);

void create_PWL_approximation(std::vector< my_monomial_t> input_polynomial,
  map< uint32_t, pair<double, double> > input_interval,
  double tolerance_limit,
  std::vector< std::vector< std::vector< std::vector< double > > > >& region_descriptions,
  std::vector< std::vector< std::vector< double > > >& linear_mapping,
  vector<PolynomialApproximator> & decomposed_pwls,
  vector<double>  &lb,
  vector<double>  &ub,
  int& linear_piece_count
);

#endif
