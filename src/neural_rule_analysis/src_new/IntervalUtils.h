//
// Created by Mac User on 1/11/18.
//

#ifndef NEURAL_RULE_ANALYSIS_INTERVALUTILS_H
#define NEURAL_RULE_ANALYSIS_INTERVALUTILS_H
#include <map>
#include <gmp.h>
#include <gmpxx.h>

namespace NeuralRuleAnalysis{
    std::pair<mpq_class, mpq_class> compute_power(std::pair<mpq_class, mpq_class> & intvl, int k);
    void multiply_assign(mpq_class & lb1, mpq_class & ub1,
                         mpq_class const & lb2, mpq_class const & ub2);


};
#endif //NEURAL_RULE_ANALYSIS_INTERVALUTILS_H
