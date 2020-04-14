//
// Created by Mac User on 1/11/18.
//

#ifndef NEURAL_RULE_ANALYSIS_POLYNOMIAL_H
#define NEURAL_RULE_ANALYSIS_POLYNOMIAL_H

#include <map>
#include <set>
#include <vector>
#include "Monomial.h"
#include "Box.h"




namespace NeuralRuleAnalysis {
    class Polynomial {
    protected:
        // A vector of monomials
        std::vector<Monomial> _mons;
        // A set of variables in the polynomial
        std::set<int> _used_vars;
        double evaluate_at_point(std::map<int, double> const & value) const;
        double evaluate_partial_derivative_at_point(int var, std::map<int, double> const & value) const;

    public:

        // Create an empty polynomial.
        Polynomial() = default;

        // Copy constructor
        Polynomial(Polynomial const & p):_used_vars(p._used_vars){
            for (auto const & m : p._mons){
                _mons.push_back(Monomial(m));
            }
        }
        // Add a monomial to the polynomial
        void add_monomial(Monomial const & m){
            _mons.push_back(m);
            m.add_used_variables_to_set(_used_vars);
        }

        std::vector<int> get_used_vars() const {
            std::vector<int> ret_vec;
            for(int i: _used_vars){
                ret_vec.push_back(i);
            }
            return ret_vec;
        }
        // Pretty print
        friend ostream& operator<< (ostream & o, Polynomial const & p);

        // Linearize around a point
        // Input: maps system variables to their values.
        // Output: a linear expression, maps system variables to their coefficients in the polynomial
        //         + constant term
        std::pair< std::map<int, double>, double>
        linearize_around_point(std::map<int, double> const & value);

        // Interval evaluation
        MpfiWrapper  interval_evaluation(Box const & b) const;

        // Find maximal box for linearization bound around a point
        //  void compute_maximal_box_for_guaranteed_bound(Box & b, std::map<int, double> const & value,
        //                                              mpq_class delta, mpq_class lb, mpq_class ub);

        void subtract_linear_expr(const map<int, double> &map);

        AffineArithmeticExpression affine_arithmetic_evaluate(AffineArithmeticNoiseSymbols & env, Box const & b ) const;

        MpfiWrapper interval_evaluation_using_affine_arithmetic(Box const & b) const;
    };
};


#endif //NEURAL_RULE_ANALYSIS_POLYNOMIAL_H
