//
// Created by Mac User on 1/19/18.
//

#ifndef NEURAL_RULE_ANALYSIS_AFFINEARITHMETICNOISESYMBOLS_H
#define NEURAL_RULE_ANALYSIS_AFFINEARITHMETICNOISESYMBOLS_H
#include <cassert>
#include <vector>
#include <map>
#include "mpfiWrapper.h"


namespace NeuralRuleAnalysis {
    struct NoiseSymbol {
        int id;
        MpfiWrapper range;
    };

    class AffineArithmeticNoiseSymbols{
    protected:
        std::vector<NoiseSymbol> _syms;

    public:

        AffineArithmeticNoiseSymbols()  = default;

        AffineArithmeticNoiseSymbols( AffineArithmeticNoiseSymbols const & what ) = default;

        int add_noise_symbol(MpfiWrapper rng);

        MpfiWrapper get_symbol_range(int id) const {
            assert(id >= 0);
            assert(id < num_symbols());
            return _syms[id].range;
        }

        int num_symbols() const { return (int) _syms.size(); };

        NoiseSymbol const & get_symbol(int id) const {
            assert(id >= 0);
            assert(id < num_symbols());
            return _syms[id];
        };

        NoiseSymbol & get_symbol(int id) {
            assert(id >= 0);
            assert(id < num_symbols());
            return _syms[id];
        };
    };
};


#endif //NEURAL_RULE_ANALYSIS_AFFINEARITHMETICNOISESYMBOLS_H
