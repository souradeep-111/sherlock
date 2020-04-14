//
// Created by Mac User on 1/19/18.
//

#include "AffineArithmeticExpression.h"

namespace NeuralRuleAnalysis {

    void AffineArithmeticExpression::add_to_coefficient(int sym_id, MpfiWrapper what){
        MpfiWrapper c = get_coefficient(sym_id);
        set_coefficient(sym_id, c+what);
    }

    void AffineArithmeticExpression::scale_and_add_assign( MpfiWrapper const & scale,
                                                          const AffineArithmeticExpression & what_else) {
        if (scale.upper() <= 0.0 && scale.lower() >= 0.0){
            return; // Nothing to do.
        }
        // Iterate
        for (auto kv: what_else.expr){
            add_to_coefficient(kv.first, kv.second * scale );
        }
        constant_coeff = constant_coeff + scale * what_else.constant_coeff;
    }

    void AffineArithmeticExpression::scale_assign( MpfiWrapper const & scale){
        constant_coeff = scale * constant_coeff;
        if (scale.lower() >= 0.0 && scale.upper() <= 0.0){
            expr.clear();
            return;
        }
        for (auto & kv: expr){
            kv.second = scale * kv.second;
        }
    }

    void AffineArithmeticExpression::multiply_assign(AffineArithmeticExpression const &what_else) {
        // multiply what_else by my constant
        // scale me by what_else constant
        AffineArithmeticExpression ret_expr(*this);

        ret_expr.scale_assign(what_else.constant_coeff);
        ret_expr.scale_and_add_assign(constant_coeff, what_else);

        ret_expr.set_constant(constant_coeff * what_else.constant_coeff);
        // Create a new noise symbol for the rest.
        MpfiWrapper noise_range = get_noise_range_for_multiplication(what_else);
        if (noise_range.upper() <= 0.0 && noise_range.lower() >= 0.0){
            this -> assign(ret_expr);
            return;
        }

        int new_sym_id = aaEnv.add_noise_symbol(noise_range);
        ret_expr.set_coefficient(new_sym_id, MpfiWrapper(1.0));
        this -> assign(ret_expr);
    }

    MpfiWrapper AffineArithmeticExpression::get_noise_range_for_multiplication(const AffineArithmeticExpression &what_else) const {
        MpfiWrapper noise_range(0.0);
        for (auto const & kv0: expr){
            int sym_id0 = kv0.first;
            MpfiWrapper sym_id0_range = aaEnv.get_symbol_range(sym_id0);
            MpfiWrapper coeff0 = kv0.second;
            for (auto const & kv1: what_else.expr){
                int sym_id1 = kv1.first;
                MpfiWrapper sym_id1_range = aaEnv.get_symbol_range(sym_id1);
                MpfiWrapper coeff1 = kv1.second;
                if (sym_id0 == sym_id1){
                    MpfiWrapper tmp = pow(sym_id0_range, 2);
                    tmp = tmp * coeff0 * coeff1;
                    noise_range = noise_range + tmp;
                } else {
                    MpfiWrapper tmp = sym_id0_range * sym_id1_range * coeff0 * coeff1;
                    noise_range = noise_range + tmp;
                }
            }

        }
        return noise_range;
    }

    void AffineArithmeticExpression::assign(AffineArithmeticExpression const &what) {
        constant_coeff = what.constant_coeff;
        expr.clear();
        for (auto const & kv: what.expr){
            expr.insert(kv);
        }
    }

    MpfiWrapper AffineArithmeticExpression::get_range() {
        MpfiWrapper ret(constant_coeff);
        for (auto const & kv: expr){
            MpfiWrapper tmp = aaEnv.get_symbol_range(kv.first);
            tmp  = tmp * kv.second;
            ret = ret + tmp;
        }
        return ret;
    }

    void AffineArithmeticExpression::square_assign(){
        AffineArithmeticExpression ret_expr(*this);
        MpfiWrapper scale_fact = 2.0 * constant_coeff;
        ret_expr.scale_assign(scale_fact);
        ret_expr.set_constant(pow(constant_coeff, 2));
        // Create a new noise symbol for the rest.
        MpfiWrapper noise_range = get_noise_range_for_multiplication(*this);
        int new_sym_id = aaEnv.add_noise_symbol(noise_range);
        ret_expr.set_coefficient(new_sym_id, MpfiWrapper(1.0));
        this -> assign(ret_expr);
    }

    void AffineArithmeticExpression::pow_assign(int k) {
        AffineArithmeticExpression ret_expr(this -> aaEnv);
        ret_expr.set_constant(MpfiWrapper(1.0));
        AffineArithmeticExpression tmp_expr(*this);
        int k0 = k;
        int l = 0;
        int m = 1;
        while (k > 0){
            if (k % 2 == 1){
                ret_expr.multiply_assign(tmp_expr);
                l = l + m;
            }
            if (k > 1) {
                tmp_expr.square_assign();
                m = m * 2;
            }
            k = k / 2;
        }
        assert( l == k0);
        assign(ret_expr);
    }

    std::ostream & operator<< (std::ostream & out, AffineArithmeticExpression const & to_print){
        out << to_print.constant_coeff;
        for(auto const & kv: to_print.expr){
            int sym_id = kv.first;
            MpfiWrapper sym_range = to_print.aaEnv.get_symbol_range(sym_id);
            out << " + " << kv.second << "*" << "{N" << sym_id << ":" << sym_range <<"}";
        }
        return out;
    }



};
