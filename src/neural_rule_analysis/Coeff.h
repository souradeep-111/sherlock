//
// Created by Mac User on 1/10/18.
//

#ifndef NEURAL_RULE_ANALYSIS_COEFF_H
#define NEURAL_RULE_ANALYSIS_COEFF_H

#include <gmpxx.h>
#include <cassert>

namespace NeuralRuleAnalysis{
    class Coeff {
    protected:
        mpq_class q;
    public:

        Coeff(){};

        Coeff(double f): q(f)
        {};

        Coeff(int num, int den): q(num, den)
        {};

        Coeff(Coeff const & c0): q(c0.q)
        {};

        mpq_class const & get_mpq() const {
            return q;
        }

        mpq_class & get_mpq() {
            return q;
        }

        bool fits_long() const {
            mpz_class num = q.get_num();
            mpz_class den = q.get_den();
            return num.fits_sint_p() && den.fits_sint_p();
        };

        void set(Coeff const & c) {
            q = c.q;
        }

        long get_numerator() const{
            mpz_class num = q.get_num();
            assert(num.fits_sint_p());
            return num.get_si();
        };

        long get_denominator() const{
            mpz_class den = q.get_den();
            assert(den.fits_sint_p());
            return den.get_si();
        };

        double get_d() const{
            return q.get_d();
        };

        friend ostream  & operator<< (ostream & os, Coeff const & c){
            os << c.q;
            return os;
        };
    };
};

#endif //NEURAL_RULE_ANALYSIS_COEFF_H
