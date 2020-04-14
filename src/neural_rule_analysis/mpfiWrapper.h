#ifndef D__MPFI_LIB__HH__
#define D__MPFI_LIB__HH__

#include <gmp.h>
#define __gmp_const const
#include <mpfi.h>
#include <mpfi_io.h>
#include <string>
using namespace std;

namespace NeuralRuleAnalysis {
    typedef mp_prec_t PrecisionType;

    class MpfiWrapper {
    protected:
        mpfi_t r;
        int flag;
        static mp_prec_t curPrec;
    public:
        MpfiWrapper();

        MpfiWrapper(double d, mp_prec_t prec = curPrec);

        MpfiWrapper(double l, double u, mp_prec_t prec = curPrec);

        MpfiWrapper(MpfiWrapper const &w, mp_prec_t prec = curPrec);

        MpfiWrapper(int n, mp_prec_t prec = curPrec);


        ~MpfiWrapper();

        MpfiWrapper &operator=(const MpfiWrapper &r);

      /*  MpfiWrapper &copy(const MpfiWrapper &r,
                          mp_prec_t prec = curPrec);
        */

        void set(double d);
        void set(MpfiWrapper const & r);
        void set(double l, double u);

        double lower() const;

        double upper() const;

        friend bool zero_in(MpfiWrapper const &m);

        // Precision
        static void setDefaultPrecision(mp_prec_t newprec);


        // Arithmetic operators
        // Philosophy: are members only the operations between MpfiWrapper

        MpfiWrapper operator+(const MpfiWrapper &r) const;

        friend MpfiWrapper operator+(const MpfiWrapper &r1, const double r2);

        friend MpfiWrapper operator+(const double r1, const MpfiWrapper &r2);


        MpfiWrapper operator-(const MpfiWrapper &r) const;

        friend MpfiWrapper operator-(const MpfiWrapper &r1, const double r2);

        friend MpfiWrapper operator-(const double r1, const MpfiWrapper &r2);

        MpfiWrapper operator-() const;

        MpfiWrapper operator*(const MpfiWrapper &r) const;

        friend MpfiWrapper operator*(const MpfiWrapper &r1, const double r2);

        friend MpfiWrapper operator*(const double r1, const MpfiWrapper &r2);

        MpfiWrapper operator/(const MpfiWrapper &r) const;

        friend MpfiWrapper operator/(const MpfiWrapper &r1, const double r2);

        friend MpfiWrapper operator/(const double r1, const MpfiWrapper &r2);

        friend double median(const MpfiWrapper &r);

        friend MpfiWrapper pow(const MpfiWrapper &r, int n);

        friend MpfiWrapper inverse(const MpfiWrapper &r);

        friend MpfiWrapper sqrt(const MpfiWrapper &r);

        friend MpfiWrapper square(const MpfiWrapper &r);

        friend MpfiWrapper exp(const MpfiWrapper &r);

        friend MpfiWrapper sin(const MpfiWrapper &r);

        friend MpfiWrapper cos(const MpfiWrapper &r);

        friend MpfiWrapper tan(const MpfiWrapper &r);

        friend MpfiWrapper intersect(const MpfiWrapper &r1, const MpfiWrapper &r2);

        friend ostream & operator<< (ostream & os, MpfiWrapper const & r);

    };

};

#endif
