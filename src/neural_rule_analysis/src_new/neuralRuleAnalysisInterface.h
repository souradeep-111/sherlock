//
// Created by Mac User on 1/19/18.
//

#ifndef NEURAL_RULE_ANALYSIS_NEURALRULEANALYSISINTERFACE_H
#define NEURAL_RULE_ANALYSIS_NEURALRULEANALYSISINTERFACE_H

#include <vector>
#include <map>
#include <set>
#include "PolynomialApproximator.h"
#include <gurobi_c++.h>

using NeuralRuleAnalysis::PolynomialApproximator;
using NeuralRuleAnalysis::LinearPiece;

/*--
 * Polynomial Approximation Method
 * Inputs:
 *    int n -- number of variables in the system
 *    std::vector< std::pair< double, std::vector<int> > polynomial --- the polynomial you would like to approx.
 *    std::vector<double> lower_bounds -- lower bounds on the variables
 *    std::vector<double> upper_bounds -- upper bounds on the variables
 *    std::vector<int> num_subdivs -- how many subdivisions along each dimension
 *    double tol -- positive number that indicates total error tolerance
 *
 * Outputs:
 *    vector<PolynomialApproximator> result;
 *
 *    Each entry corresponds to a different set of terms,
 *    so that the overall polynomial is approximated
 *    by the sum of each part.
 *    Note that the result vector is passed along as an output argument.
 */

typedef std::pair<double, std::vector<int> > my_monomial_t;

void piecewise_approximate_polynomial(int n, std::vector< my_monomial_t > & poly, std::vector< double > const & lower_bounds,
                                      std::vector< double > const & upper_bounds, double tol, std::vector<int> const & num_subdivs,
                                      std::vector<PolynomialApproximator> & result);

/* Function : encode_pwl_models_in_milp
 *  Given the Gurobi model and the PWL variables,
 *  the function creates a way to encode it using Gurobi
 */
GRBVar encode_pwl_models_in_milp(int n, std::vector<PolynomialApproximator> const & decomposed_pwls,
                               std::vector<double> const &lower_bounds,
                               std::vector<double> const &upper_bounds,
                               GRBModel & model, std::vector<GRBVar> const & input_variables);




#endif //NEURAL_RULE_ANALYSIS_NEURALRULEANALYSISINTERFACE_H
