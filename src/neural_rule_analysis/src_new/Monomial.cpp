//
// Created by Mac User on 1/10/18.
//
#include "Monomial.h"
#include "mpfiWrapper.h"

namespace NeuralRuleAnalysis {

    void Monomial::add_used_variables_to_set(std::set<int> & s) const {
        for (auto const & kv: _vars_to_powers){
            assert(kv.second > 0);
            s.insert(kv.first);
        }
        return;
    }

    void Monomial::interval_evaluation(Box const &b, MpfiWrapper &result) const {
        MpfiWrapper mon_range(_c); // Initialize a new interval
        for (auto const & kv: _vars_to_powers){
            int var = kv.first;
            int p = kv.second;
            MpfiWrapper const & xi = b.get_bounds_for_input_var(var);
            MpfiWrapper tmp = pow(xi, p);
            MpfiWrapper tmp2 = mon_range * tmp;
            mon_range.set(tmp2);
        }
        MpfiWrapper tmp3 = result + mon_range;
        result.set(tmp3);
        return;
    }


    double Monomial::evaluate_at_point(std::map<int, double> const & x) const{
        MpfiWrapper res_intvl(_c);
        for (auto const & kv:this -> _vars_to_powers) {
            int var = kv.first;
            int p = kv.second;
            auto it = x.find(var);
            if (it == x.end()) {
                // Coordinate i is not part of the map, then
                return 0;
            } else {
                MpfiWrapper tmp (it -> second);
                if (p >= 2)
                    tmp = pow(tmp, p);
                res_intvl = res_intvl * tmp;
            }
        }

        return median(res_intvl);
    }

    double Monomial::evaluate_partial_derivative_at(int xj, std::map<int, double> const & x) const{
        // If the variable to differentiate is not part of the monomial, then let us bail.
        if (_vars_to_powers.find(xj) == _vars_to_powers.end()){
            return 0.0;
        }
        MpfiWrapper res_intvl(_c);
        for (auto const & kv:this -> _vars_to_powers) {
            int var = kv.first;
            int p = kv.second;
            auto it = x.find(var);
            if (xj == var){
                assert(p >= 1 );
                if (p == 1) continue;
                res_intvl = res_intvl * MpfiWrapper(p); // perform differentiation
                if (it == x.end()) {
                    // Coordinate i is not part of the map, then
                    return 0.0;
                } else {// res_intvl = res_intvl * xj^{p-1}
                    MpfiWrapper tmp (it -> second);
                    if (p >= 3)
                        tmp = pow(tmp, p-1);
                    res_intvl = res_intvl * tmp;
                }
            } else {
                if (it == x.end()) {
                    // Coordinate i is not part of the map, then
                    return 0.0;
                } else {
                    MpfiWrapper tmp (it -> second);
                    if (p >= 2)
                        tmp = pow(tmp, p);
                    res_intvl = res_intvl * tmp;
                }
            }
        }
        return median(res_intvl);
    }

    AffineArithmeticExpression
    Monomial::affine_arithmetic_evaluate(AffineArithmeticNoiseSymbols &env,
                                         std::map<int, AffineArithmeticExpression> const & var_id_to_noise_symbol) const {

        AffineArithmeticExpression ret_expr(env);
        ret_expr.set_constant(_c);
        for (auto const & kv: _vars_to_powers){
            int var_id = kv.first;
            int pow = kv.second;
            auto it = var_id_to_noise_symbol.find(var_id);
            assert( it != var_id_to_noise_symbol.end());
            AffineArithmeticExpression var_expr(it -> second);
            // compute var_expr ^ pow
            // multiply it to ret_expr
            var_expr.pow_assign(pow);
            ret_expr.multiply_assign(var_expr);
        }
        return ret_expr;
    }

};