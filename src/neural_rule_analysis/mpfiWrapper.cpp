#include<iostream>
#include <cassert>
#include <mpfr.h>
#include "mpfiWrapper.h"



namespace NeuralRuleAnalysis {
    PrecisionType MpfiWrapper::curPrec = 128L;

    MpfiWrapper::MpfiWrapper() {
        mpfi_init2(r, curPrec);
        mpfi_set_d(r, 0.0);
        flag = 0;

    }

    MpfiWrapper::MpfiWrapper(MpfiWrapper const &w, mp_prec_t prec) {
        mpfi_init2(r, curPrec);
        mpfi_set(r, w.r);
        return;
    }


    MpfiWrapper::MpfiWrapper(double d,
                             PrecisionType prec) {
        mpfi_init2(r, prec);
        flag = mpfi_set_d(r, d);
    }

    void MpfiWrapper::set(double d){
        mpfi_set_d(r,d);
    }
    void MpfiWrapper::set(MpfiWrapper const & what){
        mpfi_set(r, what.r);
        return;
    }

    void MpfiWrapper::set(double l, double u){
        flag = mpfi_interv_d(r,l, u);
    }

    MpfiWrapper::MpfiWrapper(double l, double u, mp_prec_t prec) {
        mpfi_init2(r, prec);
        flag = mpfi_interv_d(r, l, u);

    }

    MpfiWrapper::MpfiWrapper(int n, PrecisionType prec) {
        mpfi_init2(r, prec);
        flag = mpfi_set_si(r, n);
    }

    MpfiWrapper::~MpfiWrapper() {
        mpfi_clear(r);

    }

    MpfiWrapper &MpfiWrapper::operator=(const MpfiWrapper &op) {
        flag |= mpfi_set(this->r, op.r);
        return *this;
    }

    void MpfiWrapper::setDefaultPrecision(mp_prec_t newPrec) {
        mpfr_set_default_prec(newPrec);
    }

    MpfiWrapper MpfiWrapper::operator+(const MpfiWrapper &op) const {
        MpfiWrapper res;
        res.flag |= mpfi_add(res.r, this->r, op.r);
        return res;
    }


    MpfiWrapper operator+(const MpfiWrapper &op1, const double op2) {
        MpfiWrapper res;
        res.flag |= mpfi_add_d(res.r, op1.r, op2);
        return res;
    }


    MpfiWrapper operator+(const double op2, const MpfiWrapper &op1) {
        MpfiWrapper res;
        res.flag |= mpfi_add_d(res.r, op1.r, op2);
        return res;
    }


    MpfiWrapper MpfiWrapper::operator-(const MpfiWrapper &op) const {
        MpfiWrapper res;
        res.flag |= mpfi_sub(res.r, this->r, op.r);
        return res;
    }

    MpfiWrapper operator-(const MpfiWrapper &r1, const double r2) {
        MpfiWrapper res;
        res.flag |= mpfi_sub_d(res.r, r1.r, r2);
        return res;
    }

    MpfiWrapper operator-(const double r1, const MpfiWrapper &r2) {
        MpfiWrapper res;
        res.flag |= mpfi_d_sub(res.r, r1, r2.r);
        return res;
    }

    MpfiWrapper MpfiWrapper::operator-() const {
        MpfiWrapper res;
        res.flag |= mpfi_neg(res.r, this->r);
        return res;
    }


    MpfiWrapper MpfiWrapper::operator*(const MpfiWrapper &op) const {
        MpfiWrapper res;
        res.flag |= mpfi_mul(res.r, this->r, op.r);
        return res;
    }

    MpfiWrapper operator*(const MpfiWrapper &r1, const double r2) {
        MpfiWrapper res;
        res.flag |= mpfi_mul_d(res.r, r1.r, r2);
        return res;
    }

    MpfiWrapper operator*(const double r1, const MpfiWrapper &r2) {
        return r2 * r1;
    }


    MpfiWrapper MpfiWrapper::operator/(const MpfiWrapper &op) const {
        MpfiWrapper res;
        res.flag |= mpfi_div(res.r, this->r, op.r);
        return res;
    }

    MpfiWrapper operator/(const MpfiWrapper &r1, const double r2) {
        MpfiWrapper res;
        res.flag |= mpfi_div_d(res.r, r1.r, r2);
        return res;
    }

    MpfiWrapper operator/(const double r1, const MpfiWrapper &r2) {
        MpfiWrapper res;
        res.flag |= mpfi_d_div(res.r, r1, r2.r);
        return res;

    }

    double median(const MpfiWrapper &r1) {
        mpfr_t rVal;
        mpfr_init(rVal);
        mpfi_mid(rVal, r1.r);
        double d = mpfr_get_d(rVal, MPFR_RNDN);
        mpfr_clear(rVal);
        return d;
    }

    bool zero_in(MpfiWrapper const &r1) {
        return (mpfi_has_zero(r1.r) != 0);
    }

    double MpfiWrapper::upper() const {
        mpfr_t rVal;
        mpfr_init(rVal);
        mpfi_get_right(rVal, r);
        double d = mpfr_get_d(rVal, MPFR_RNDU);
        mpfr_clear(rVal);
        return d;
    }


    double MpfiWrapper::lower() const {

        mpfr_t rVal;
        mpfr_init(rVal);
        mpfi_get_left(rVal, r);
        double d = mpfr_get_d(rVal, MPFR_RNDU);
        mpfr_clear(rVal);
        return d;
    }

    MpfiWrapper pow(const MpfiWrapper &op, int n) {


        if (n == 0) {
            return MpfiWrapper(1.0, MpfiWrapper::curPrec);
        }
        int m;
        MpfiWrapper retVal(1.0);
        mpfi_t tmp;
        mpfi_init2(tmp, MpfiWrapper::curPrec);
        if (n < 0) {
            mpfi_inv(tmp, op.r);
            // tmp  =r^-1
            m = -n;
        } else {
            mpfi_set(tmp, op.r); // tmp = r
            m = n;
        }
        assert(m > 0);

        while (m > 0) {
            if (m % 2 == 1) {
                retVal.flag |= mpfi_mul(retVal.r, retVal.r, tmp); //r = r * tmp
            }
            retVal.flag |= mpfi_sqr(tmp, tmp); // tmp = tmp^2
            m = m / 2;
        }

        return retVal;


    }

    MpfiWrapper inverse(const MpfiWrapper &op) {
        MpfiWrapper retVal;
        retVal.flag |= mpfi_inv(retVal.r, op.r);
        return retVal;
    }

    MpfiWrapper sqrt(const MpfiWrapper &op) {
        MpfiWrapper retVal;
        retVal.flag |= mpfi_sqrt(retVal.r, op.r);
        return retVal;

    }


    MpfiWrapper square(const MpfiWrapper &op) {
        MpfiWrapper retVal;
        retVal.flag |= mpfi_sqr(retVal.r, op.r);
        return retVal;

    }


    MpfiWrapper exp(const MpfiWrapper &op) {
        MpfiWrapper retVal;
        retVal.flag |= mpfi_exp(retVal.r, op.r);
        return retVal;

    }

    MpfiWrapper sin(const MpfiWrapper &op) {
        MpfiWrapper retVal;
        retVal.flag |= mpfi_sin(retVal.r, op.r);
        return retVal;
    }


    MpfiWrapper cos(const MpfiWrapper &op) {
        MpfiWrapper retVal;
        retVal.flag |= mpfi_cos(retVal.r, op.r);
        return retVal;
    }


    MpfiWrapper tan(const MpfiWrapper &op) {
        MpfiWrapper retVal;
        retVal.flag |= mpfi_tan(retVal.r, op.r);
        return retVal;
    }

    MpfiWrapper intersect(const MpfiWrapper &r1, const MpfiWrapper &r2) {
        MpfiWrapper retVal;
        retVal.flag |= mpfi_intersect(retVal.r, r1.r, r2.r);
        return retVal;
    }

    ostream & operator<< (ostream & os, MpfiWrapper const & r){
        os << "[" <<r.lower() << "," << r.upper()<<"]";
        return os;
    }


};