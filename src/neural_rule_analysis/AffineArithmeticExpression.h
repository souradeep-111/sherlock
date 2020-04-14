//
// Created by Mac User on 1/19/18.
//

#ifndef NEURAL_RULE_ANALYSIS_AFFINEARITHMETICEXPRESSION_H
#define NEURAL_RULE_ANALYSIS_AFFINEARITHMETICEXPRESSION_H

#include <map>
#include <vector>
#include <iostream>
#include "AffineArithmeticNoiseSymbols.h"

namespace NeuralRuleAnalysis {
    class AffineArithmeticExpression {

    protected:
        AffineArithmeticNoiseSymbols & aaEnv;
        std::map<int, MpfiWrapper> expr;
        MpfiWrapper constant_coeff;

        MpfiWrapper get_noise_range_for_multiplication(const AffineArithmeticExpression &what_else) const;


    public:

        explicit AffineArithmeticExpression(AffineArithmeticNoiseSymbols & env):aaEnv(env){};

        AffineArithmeticExpression(AffineArithmeticExpression const & what) = default;

        // Getters and setters
        int num_noise_symbols() const { return aaEnv.num_symbols(); };

        MpfiWrapper get_coefficient(int symbol_id) const{
            assert(symbol_id >= 0);
            assert(symbol_id < aaEnv.num_symbols());

            auto it = expr.find(symbol_id);
            if (it != expr.end())
                return it -> second;
            return MpfiWrapper(0.0);
        }

        void set_coefficient(int symbol_id, MpfiWrapper what){
            assert(symbol_id >= 0);
            assert(symbol_id < aaEnv.num_symbols());
            expr[symbol_id] = what;
        }

        void add_to_coefficient(int sym_id, MpfiWrapper what);
        // this = scale1 * this + scale2 * what_else
        void scale_and_add_assign(MpfiWrapper const & scale,
                                  AffineArithmeticExpression const & what_else);

        void add_assign(AffineArithmeticExpression const & what_else){
            scale_and_add_assign(MpfiWrapper(1.0), what_else);
        }

        void subtract_assign(AffineArithmeticExpression const & what_else){
            scale_and_add_assign(MpfiWrapper(-1.0), what_else);
        }

        void multiply_assign(AffineArithmeticExpression const & what_else);

        void scale_assign(MpfiWrapper const & what);

        void set_constant(MpfiWrapper const & what){
            constant_coeff = what;
        }

        void assign(AffineArithmeticExpression const & what);

        MpfiWrapper get_range();

        void square_assign();

        void pow_assign(int k);

        friend std::ostream & operator<< (std::ostream & what, AffineArithmeticExpression const & to_print);
    };
};


#endif //NEURAL_RULE_ANALYSIS_AFFINEARITHMETICEXPRESSION_H
