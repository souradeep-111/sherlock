//
// Created by Mac User on 1/19/18.
//

#include "AffineArithmeticNoiseSymbols.h"
namespace  NeuralRuleAnalysis {
    int AffineArithmeticNoiseSymbols::add_noise_symbol(MpfiWrapper rng) {
        int id = num_symbols();
        NoiseSymbol s;
        s.id = id;
        s.range = rng;
        _syms.push_back(s);
        return id;
    }
};

