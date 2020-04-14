//
// Created by Sriram Sankaranarayanan
//

#ifndef NEURAL_RULE_ANALYSIS_MONOMIAL_H
#define NEURAL_RULE_ANALYSIS_MONOMIAL_H
#include <vector>
#include <set>
#include <map>
#include <gmpxx.h>
#include <gmp.h>
#include <ostream>
#include <cassert>
#include "mpfiWrapper.h"
#include "Box.h"
#include "AffineArithmeticNoiseSymbols.h"
#include "AffineArithmeticExpression.h"

using std::ostream;

namespace NeuralRuleAnalysis{
    class Monomial {
    protected:
        std::map<int, int> _vars_to_powers;
        MpfiWrapper _c;
    public:
        // Constructors
        Monomial() {};

        Monomial(Monomial const & m):_vars_to_powers(m._vars_to_powers), _c(m._c)
        {};

        Monomial(MpfiWrapper const & c): _c(c)
        {};

        Monomial(int var): _c(1.0,1.0){
            _vars_to_powers[var] = 1;
            return;
        }
        //Destructor
        ~Monomial(){};

        //Functions for getting and setting

        void set_power(int var, int pow){
            assert( pow >= 0);
            if (pow == 0){
                _vars_to_powers.erase(var);
                return;
            }
            _vars_to_powers[var] = pow;
        }

        int get_pow(int var) const{
            auto it = _vars_to_powers.find(var);
            if (it != _vars_to_powers.end())
                return it -> second;
            return 0;
        }

        void set_coefficient(double f){
            _c.set(f);
        }

        void set_coefficient(MpfiWrapper const & f) {
            _c.set(f);
        }

        void set_coefficient(double lb, double ub){
            _c.set(lb, ub);
        }

        MpfiWrapper & get_coefficient(){
            return _c;
        }

        MpfiWrapper const & get_coefficient() const{
            return _c;
        }

        void add_used_variables_to_set(std::set<int> & s) const;

        friend ostream & operator<<(ostream & o, Monomial const & m){
            o << m._c ;
            for (auto & kv: m._vars_to_powers){
                assert(kv.second != 0);
                if (kv.second >= 2) {
                    o << "* x_" << kv.first << "^" << kv.second;
                } else {
                    o << "* x_"<< kv.first;
                }
            }
            return o;
        }

        void interval_evaluation(Box const & b, MpfiWrapper & result) const;

        double evaluate_at_point(std::map<int, double> const & x) const;

        double evaluate_partial_derivative_at(int var, std::map<int, double> const & x) const;

        AffineArithmeticExpression affine_arithmetic_evaluate(AffineArithmeticNoiseSymbols & env,
                                             std::map<int, AffineArithmeticExpression> const & var_id_to_noise_symbol) const;
    };

};

#endif //NEURAL_RULE_ANALYSIS_MONOMIAL_H
