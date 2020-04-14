//
// Created by Mac User on 1/11/18.
//

#include "Polynomial.h"

namespace NeuralRuleAnalysis {

    extern bool debug;

    MpfiWrapper Polynomial::interval_evaluation(Box const & b) const {
        MpfiWrapper result(0.0);
        for(Monomial const & m: _mons){
            m.interval_evaluation(b, result);
        }
        return result;
    };

    ostream & operator << (ostream & out, Polynomial const & what){
        bool first = false;
        for (Monomial const & m: what._mons){
            if (first)
                out << " + ";
            out << m ;
            first = true;
        }
        return out;
    }

    double Polynomial::evaluate_at_point(std::map<int, double> const & value) const {
        double result = 0.0;
        for(Monomial const & m: _mons){
            result += m.evaluate_at_point(value);
        }
        return result;
    }


    double Polynomial::evaluate_partial_derivative_at_point(int var, std::map<int, double> const & value) const {
        double result = 0.0;
        for(Monomial const & m: _mons){
            result += m.evaluate_partial_derivative_at(var, value);
        }
        return result;
    }

    std::pair< std::map<int, double>, double>
    Polynomial::linearize_around_point(std::map<int, double> const & value){
        // For each variable xi that occurs in the polynomial, evaluate the partial
        // derivative of the polynomial w.r.t xi wrt the provided point.
        // Constant term is simply the value of the polynomial at the point.
        std::map<int, double> ret_expr;
        double ret_const = evaluate_at_point(value);
        for (int var: _used_vars){
            double ci = evaluate_partial_derivative_at_point(var, value);
            ret_expr[var] = ci;
            auto it = value.find(var);
            if (it != value.end()){
                ret_const -= it-> second * ci;
            }
        }
        return std::make_pair(ret_expr, ret_const);
    }

    void Polynomial::subtract_linear_expr(const map<int, double> &map) {
        for( auto const & kv: map){
            int var = kv.first;
            double c = kv.second;
            // Create a new monomial term
            Monomial m;
            m.set_coefficient(-c);
            m.set_power(var, 1);
            add_monomial(m);
        }
    }

    MpfiWrapper Polynomial::interval_evaluation_using_affine_arithmetic(Box const &b) const {
        AffineArithmeticNoiseSymbols env;
        AffineArithmeticExpression expr = affine_arithmetic_evaluate(env, b);
        return expr.get_range();
    }

    AffineArithmeticExpression
    Polynomial::affine_arithmetic_evaluate(AffineArithmeticNoiseSymbols &env, Box const &b) const {
        // Create noise symbols for the variables
        std::map<int, AffineArithmeticExpression> var_map;
        for (int i: _used_vars){
            MpfiWrapper w = b.get_bounds_for_input_var(i);
            // Make an affine arithmetic expression out of w
            double c = median(w);
            w = w - c;
            int sym_id = env.add_noise_symbol(w);
            AffineArithmeticExpression expr(env);
            expr.set_constant(MpfiWrapper(c));
            expr.set_coefficient(sym_id, MpfiWrapper(1.0));
            var_map.insert(make_pair(i, expr));
           /* if (debug){
                std::cout << "<VariableMap> x_"<< i;
                std::cout << expr << "</VariableMap>" << std::endl;
            }*/
        }
        // Now evaluate the expression for each monomial
        AffineArithmeticExpression ret_expr(env);
        for (Monomial const & m: _mons){
            AffineArithmeticExpression m_expr = m.affine_arithmetic_evaluate(env, var_map);
            /*if (debug){
                std::cout << "<Monomial>" << m << std::endl;
                std::cout << m_expr << "</Monomial>" << std::endl;
            }*/
            ret_expr.add_assign(m_expr);
        }

        return ret_expr;
    }
};
