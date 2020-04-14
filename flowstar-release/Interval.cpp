/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.
  
  The code is released as is under the GNU General Public License (GPL).
---*/

/*
 * MPFR_RNDU: round toward plus infinity (roundTowardPositive in IEEE 754-2008),
 * MPFR_RNDD: round toward minus infinity (roundTowardNegative in IEEE 754-2008),
 */

#include "Interval.h"

using namespace flowstar;

mpfr_prec_t intervalNumPrecision = normal_precision;


Real::Real()
{
	mpfr_init2(value, intervalNumPrecision);
	mpfr_set_ui(value, 0L, MPFR_RNDD);
}

Real::Real(const double v)
{
	mpfr_init2(value, intervalNumPrecision);
	mpfr_set_d(value, v, MPFR_RNDN);
}

Real::Real(const Real & real)
{
	mpfr_init2(value, intervalNumPrecision);
	mpfr_set(value, real.value, MPFR_RNDN);
}

Real::~Real()
{
	mpfr_clear(value);
}

bool Real::isZero() const
{
	return mpfr_cmp_ui(value, 0L);
}

double Real::getValue_RNDD() const
{
	return mpfr_get_d(value, MPFR_RNDD);
}

double Real::getValue_RNDU() const
{
	return mpfr_get_d(value, MPFR_RNDU);
}

void Real::abs(Real & real) const
{
	mpfr_abs(real.value, value, MPFR_RNDU);
}

void Real::abs_assign()
{
	mpfr_abs(value, value, MPFR_RNDU);
}

void Real::to_sym_int(Interval & I) const
{
	mpfr_abs(I.up, value, MPFR_RNDU);
	mpfr_neg(I.lo, I.up, MPFR_RNDD);
}

void Real::exp_RNDU(Real & result) const
{
	mpfr_exp(result.value, value, MPFR_RNDU);
}

void Real::exp_assign_RNDU()
{
	mpfr_exp(value, value, MPFR_RNDU);
}

void Real::pow_assign_RNDU(const int n)
{
	mpfr_pow_ui(value, value, n, MPFR_RNDU);
}

void Real::pow_assign(const int n)
{
	mpfr_pow_ui(value, value, n, MPFR_RNDN);
}

void Real::rec(Real & result) const
{
	mpfr_ui_div(result.value, 1L, value, MPFR_RNDN);
}

void Real::add_RNDD(Real & result, const Real & real) const
{
	mpfr_add(result.value, value, real.value, MPFR_RNDD);
}

void Real::add_assign_RNDD(const Real & real)
{
	mpfr_add(value, value, real.value, MPFR_RNDD);
}

void Real::add_RNDU(Real & result, const Real & real) const
{
	mpfr_add(result.value, value, real.value, MPFR_RNDU);
}

void Real::add_assign_RNDU(const Real & real)
{
	mpfr_add(value, value, real.value, MPFR_RNDU);
}

void Real::add_RNDN(Real & result, const Real & real) const
{
	mpfr_add(result.value, value, real.value, MPFR_RNDN);
}

void Real::add_assign_RNDN(const Real & real)
{
	mpfr_add(value, value, real.value, MPFR_RNDN);
}

void Real::sub_RNDD(Real & result, const Real & real) const
{
	mpfr_sub(result.value, value, real.value, MPFR_RNDD);
}

void Real::sub_assign_RNDD(const Real & real)
{
	mpfr_sub(value, value, real.value, MPFR_RNDD);
}

void Real::sub_RNDU(Real & result, const Real & real) const
{
	mpfr_sub(result.value, value, real.value, MPFR_RNDU);
}

void Real::sub_assign_RNDU(const Real & real)
{
	mpfr_sub(value, value, real.value, MPFR_RNDU);
}

void Real::mul_RNDD(Real & result, const Real & real) const
{
	mpfr_mul(result.value, value, real.value, MPFR_RNDD);
}

void Real::mul_assign_RNDD(const Real & real)
{
	mpfr_mul(value, value, real.value, MPFR_RNDD);
}

void Real::mul_RNDU(Real & result, const Real & real) const
{
	mpfr_mul(result.value, value, real.value, MPFR_RNDU);
}

void Real::mul_assign_RNDU(const Real & real)
{
	mpfr_mul(value, value, real.value, MPFR_RNDU);
}

void Real::mul_RNDD(Real & result, const int n) const
{
	mpfr_mul_si(result.value, value, n, MPFR_RNDD);
}

void Real::mul_assign_RNDD(const int n)
{
	mpfr_mul_si(value, value, n, MPFR_RNDD);
}

void Real::mul_RNDU(Real & result, const int n) const
{
	mpfr_mul_si(result.value, value, n, MPFR_RNDU);
}

void Real::mul_assign_RNDU(const int n)
{
	mpfr_mul_si(value, value, n, MPFR_RNDU);
}

void Real::div_RNDD(Real & result, const Real & real) const
{
	mpfr_div(result.value, value, real.value, MPFR_RNDD);
}

void Real::div_assign_RNDD(const Real & real)
{
	mpfr_div(value, value, real.value, MPFR_RNDD);
}

void Real::div_RNDU(Real & result, const Real & real) const
{
	mpfr_div(result.value, value, real.value, MPFR_RNDU);
}

void Real::div_assign_RNDU(const Real & real)
{
	mpfr_div(value, value, real.value, MPFR_RNDU);
}

void Real::div_RNDD(Real & result, const int n) const
{
	mpfr_div_si(result.value, value, n, MPFR_RNDD);
}

void Real::div_assign_RNDD(const int n)
{
	mpfr_div_si(value, value, n, MPFR_RNDD);
}

void Real::div_RNDU(Real & result, const int n) const
{
	mpfr_div_si(result.value, value, n, MPFR_RNDU);
}

void Real::div_assign_RNDU(const int n)
{
	mpfr_div_si(value, value, n, MPFR_RNDU);
}

void Real::output(FILE *fp) const
{
	mpfr_out_str(fp, 10, PN, value, MPFR_RNDN);
}

void Real::sin_assign()
{
	mpfr_sin(value, value, MPFR_RNDN);
}

void Real::cos_assign()
{
	mpfr_cos(value, value, MPFR_RNDN);
}

void Real::exp_assign()
{
	mpfr_exp(value, value, MPFR_RNDN);
}

void Real::log_assign()
{
	mpfr_log(value, value, MPFR_RNDN);
}

void Real::sqrt_assign()
{
	mpfr_sqrt(value, value, MPFR_RNDN);
}

Real & Real::operator += (const Real & r)
{
	mpfr_add(value, value, r.value, MPFR_RNDN);
	return *this;
}

Real & Real::operator -= (const Real & r)
{
	mpfr_sub(value, value, r.value, MPFR_RNDN);
	return *this;
}

Real & Real::operator *= (const Real & r)
{
	mpfr_mul(value, value, r.value, MPFR_RNDN);
	return *this;
}

Real & Real::operator /= (const Real & r)
{
	mpfr_div(value, value, r.value, MPFR_RNDN);
	return *this;
}

Real & Real::operator += (const double d)
{
	mpfr_add_d(value, value, d, MPFR_RNDN);
	return *this;
}

Real & Real::operator -= (const double d)
{
	mpfr_sub_d(value, value, d, MPFR_RNDN);
	return *this;
}

Real & Real::operator *= (const double d)
{
	mpfr_mul_d(value, value, d, MPFR_RNDN);
	return *this;
}

Real & Real::operator /= (const double d)
{
	mpfr_div_d(value, value, d, MPFR_RNDN);
	return *this;
}

Real Real::operator + (const Real & r) const
{
	Real result = *this;
	result += r;
	return result;
}

Real Real::operator - (const Real & r) const
{
	Real result = *this;
	result -= r;
	return result;
}

Real Real::operator * (const Real & r) const
{
	Real result = *this;
	result *= r;
	return result;
}

Real Real::operator / (const Real & r) const
{
	Real result = *this;
	result /= r;
	return result;
}

bool Real::operator == (const Real & r) const
{
	if(mpfr_cmp(value, r.value) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Real::operator != (const Real & r) const
{
	if(mpfr_cmp(value, r.value) == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Real::operator >= (const Real & r) const
{
	if(mpfr_cmp(value, r.value) >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Real::operator > (const Real & r) const
{
	if(mpfr_cmp(value, r.value) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Real & Real::operator = (const Real & r)
{
	if(this == &r)
		return *this;

	mpfr_set(value, r.value, MPFR_RNDN);
	return *this;
}









Interval::Interval()
{
	mpfr_inits2(intervalNumPrecision, lo, up, (mpfr_ptr) 0);

	mpfr_set_d(lo, 0.0, MPFR_RNDD);
	mpfr_set_d(up, 0.0, MPFR_RNDU);
}

Interval::Interval(const double c)
{
	mpfr_inits2(intervalNumPrecision, lo, up, (mpfr_ptr) 0);

	mpfr_set_d(lo, c, MPFR_RNDD);
	mpfr_set_d(up, c, MPFR_RNDU);
}

Interval::Interval(const Real & r)
{
	mpfr_inits2(intervalNumPrecision, lo, up, (mpfr_ptr) 0);

	mpfr_set(lo, r.value, MPFR_RNDD);
	mpfr_set(up, r.value, MPFR_RNDU);
}

Interval::Interval(const double l, const double u)
{
	mpfr_inits2(intervalNumPrecision, lo, up, (mpfr_ptr) 0);

	mpfr_set_d(lo, l, MPFR_RNDD);
	mpfr_set_d(up, u, MPFR_RNDU);
}

Interval::Interval(const Real & c, const Real & r)
{
	mpfr_inits2(intervalNumPrecision, lo, up, (mpfr_ptr) 0);

	mpfr_add(up, c.value, r.value, MPFR_RNDU);
	mpfr_sub(lo, c.value, r.value, MPFR_RNDD);
}

Interval::Interval(const Real & l, const Real & u, const int n)
{
	mpfr_inits2(intervalNumPrecision, lo, up, (mpfr_ptr) 0);

	mpfr_set(up, u.value, MPFR_RNDU);
	mpfr_set(lo, l.value, MPFR_RNDD);
}

Interval::Interval(const char *strLo, const char *strUp)
{
	mpfr_inits2(intervalNumPrecision, lo, up, (mpfr_ptr) 0);

	mpfr_set_str(lo, strLo, 10, MPFR_RNDD);
	mpfr_set_str(up, strUp, 10, MPFR_RNDU);
}

Interval::Interval(const Interval & I)
{
	mpfr_inits2(intervalNumPrecision, lo, up, (mpfr_ptr) 0);

	mpfr_set(lo, I.lo, MPFR_RNDD);
	mpfr_set(up, I.up, MPFR_RNDU);
}

Interval::~Interval()
{
	mpfr_clear(lo);
	mpfr_clear(up);
}

bool Interval::isZero() const
{
	Interval intZero;

	if(this->subseteq(intZero))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Interval::set(const double l, const double u)
{
	mpfr_set_d(lo, l, MPFR_RNDD);
	mpfr_set_d(up, u, MPFR_RNDU);
}

void Interval::set(const double c)
{
	mpfr_set_d(lo, c, MPFR_RNDD);
	mpfr_set_d(up, c, MPFR_RNDU);
}

void Interval::set(const Real & r)
{
	mpfr_set(lo, r.value, MPFR_RNDD);
	mpfr_set(up, r.value, MPFR_RNDU);
}

void Interval::setInf(const double l)
{
	mpfr_set_d(lo, l, MPFR_RNDD);
}

void Interval::setInf(const Interval & I)
{
	mpfr_set(lo, I.lo, MPFR_RNDD);
}

void Interval::setSup(const double u)
{
	mpfr_set_d(up, u, MPFR_RNDU);
}

void Interval::setSup(const Interval & S)
{
	mpfr_set(up, S.up, MPFR_RNDU);
}

void Interval::split(Interval & left, Interval & right) const
{
	mpfr_t tmp;
	mpfr_inits2(intervalNumPrecision, tmp, (mpfr_ptr) 0);

	mpfr_set(left.lo, lo, MPFR_RNDD);
	mpfr_add(tmp, lo, up, MPFR_RNDU);
	mpfr_div_d(left.up, tmp, 2.0, MPFR_RNDU);

	mpfr_set(right.up, up, MPFR_RNDU);
	mpfr_add(tmp, lo, up, MPFR_RNDD);
	mpfr_div_d(right.lo, tmp, 2.0, MPFR_RNDD);

	mpfr_clear(tmp);
}

void Interval::split(std::list<Interval> & result, const int n) const
{
	mpfr_t inc, w, newup, newlo;
	mpfr_inits2(intervalNumPrecision, inc, w, newup, newlo, (mpfr_ptr) 0);

	mpfr_sub(w, up, lo, MPFR_RNDU);
	mpfr_div_si(inc, w, (long)n, MPFR_RNDU);

	Interval grid;
	mpfr_set(grid.lo, lo, MPFR_RNDD);
	mpfr_add(grid.up, lo, inc, MPFR_RNDU);
	result.push_back(grid);

	for(int i=1; i<n; ++i)
	{
		mpfr_add(grid.lo, grid.lo, inc, MPFR_RNDD);
		mpfr_add(grid.up, grid.up, inc, MPFR_RNDU);
		result.push_back(grid);
	}
}

void Interval::set_inf()
{
	mpfr_set_inf(lo, -1);
	mpfr_set_inf(up, 1);
}

double Interval::sup() const
{
	return mpfr_get_d(up, MPFR_RNDU);
}

double Interval::inf() const
{
	return mpfr_get_d(lo, MPFR_RNDD);
}

void Interval::sup(Interval & S) const
{
	mpfr_set(S.up, up, MPFR_RNDU);
	mpfr_set(S.lo, up, MPFR_RNDD);
}

void Interval::inf(Interval & I) const
{
	mpfr_set(I.up, lo, MPFR_RNDU);
	mpfr_set(I.lo, lo, MPFR_RNDD);
}

void Interval::sup(Real & u) const
{
	mpfr_set(u.value, up, MPFR_RNDU);
}

void Interval::inf(Real & l) const
{
	mpfr_set(l.value, lo, MPFR_RNDD);
}

double Interval::midpoint() const
{
	mpfr_t tmp;
	mpfr_inits2(intervalNumPrecision, tmp, (mpfr_ptr) 0);

	mpfr_add(tmp, lo, up, MPFR_RNDN);
	mpfr_div_d(tmp, tmp, 2.0, MPFR_RNDN);

	double dMidpoint = mpfr_get_d(tmp, MPFR_RNDN);
	mpfr_clear(tmp);

	return dMidpoint;
}

void Interval::midpoint(Interval & M) const
{
	mpfr_t tmp;
	mpfr_inits2(intervalNumPrecision, tmp, (mpfr_ptr) 0);

	mpfr_add(tmp, lo, up, MPFR_RNDU);
	mpfr_div_d(tmp, tmp, 2.0, MPFR_RNDU);

	mpfr_set(M.up, tmp, MPFR_RNDU);

	mpfr_add(tmp, lo, up, MPFR_RNDD);
	mpfr_div_d(tmp, tmp, 2.0, MPFR_RNDD);

	mpfr_set(M.lo, tmp, MPFR_RNDD);

	mpfr_clear(tmp);
}

void Interval::midpoint(Real & mid) const
{
	mpfr_t tmp;

	mpfr_inits2(intervalNumPrecision, tmp, (mpfr_ptr) 0);

	mpfr_add(tmp, lo, up, MPFR_RNDN);
	mpfr_div_d(mid.value, tmp, 2, MPFR_RNDN);

	mpfr_clear(tmp);
}

void Interval::toCenterForm(Real & center, Real & radius) const
{
	mpfr_t tmp1;

	mpfr_inits2(intervalNumPrecision, tmp1, (mpfr_ptr) 0);

	mpfr_add(tmp1, lo, up, MPFR_RNDN);
	mpfr_div_d(center.value, tmp1, 2, MPFR_RNDN);

	mpfr_sub(tmp1, up, lo, MPFR_RNDU);
	mpfr_div_d(radius.value, tmp1, 2, MPFR_RNDU);

	mpfr_clear(tmp1);
}

void Interval::remove_midpoint(Interval & M)
{
	mpfr_t tmp;
	mpfr_inits2(intervalNumPrecision, tmp, (mpfr_ptr) 0);

	mpfr_add(tmp, lo, up, MPFR_RNDU);
	mpfr_div_d(tmp, tmp, 2.0, MPFR_RNDU);

	mpfr_set(M.up, tmp, MPFR_RNDU);

	mpfr_add(tmp, lo, up, MPFR_RNDD);
	mpfr_div_d(tmp, tmp, 2.0, MPFR_RNDD);

	mpfr_set(M.lo, tmp, MPFR_RNDD);

	mpfr_sub(lo, lo, M.up, MPFR_RNDD);
	mpfr_sub(up, up, M.lo, MPFR_RNDU);

	mpfr_clear(tmp);
}

void Interval::remove_midpoint()
{
	mpfr_t tmp1, tmp2;
	mpfr_inits2(intervalNumPrecision, tmp1, tmp2, (mpfr_ptr) 0);

	mpfr_add(tmp1, lo, up, MPFR_RNDU);
	mpfr_div_d(tmp1, tmp1, 2.0, MPFR_RNDU);

	mpfr_add(tmp2, lo, up, MPFR_RNDD);
	mpfr_div_d(tmp2, tmp2, 2.0, MPFR_RNDD);

	mpfr_sub(lo, lo, tmp1, MPFR_RNDD);
	mpfr_sub(up, up, tmp2, MPFR_RNDU);

	mpfr_clear(tmp1);
	mpfr_clear(tmp2);
}

Interval Interval::intersect(const Interval & I) const
{
	Interval result;

	if(mpfr_cmp(lo, I.lo) > 0)
	{
		mpfr_set(result.lo, lo, MPFR_RNDD);
	}
	else
	{
		mpfr_set(result.lo, I.lo, MPFR_RNDD);
	}

	if(mpfr_cmp(up, I.up) > 0)
	{
		mpfr_set(result.up, I.up, MPFR_RNDU);
	}
	else
	{
		mpfr_set(result.up, up, MPFR_RNDU);
	}

	return result;
}

void Interval::intersect_assign(const Interval & I)
{
	if(mpfr_cmp(lo, I.lo) < 0)
	{
		mpfr_set(lo, I.lo, MPFR_RNDD);
	}

	if(mpfr_cmp(up, I.up) > 0)
	{
		mpfr_set(up, I.up, MPFR_RNDU);
	}
}

void Interval::bloat(const double e)
{
	mpfr_sub_d(lo, lo, e, MPFR_RNDD);
	mpfr_add_d(up, up, e, MPFR_RNDU);
}

void Interval::bloat(const Real & e)
{
	mpfr_sub(lo, lo, e.value, MPFR_RNDD);
	mpfr_add(up, up, e.value, MPFR_RNDU);
}

bool Interval::within(const Interval & I, const double e) const
{
	mpfr_t tmp;
	mpfr_inits2(intervalNumPrecision, tmp, (mpfr_ptr) 0);

	if(mpfr_cmp(up, I.up) >= 0)
	{
		mpfr_sub(tmp, up, I.up, MPFR_RNDU);
	}
	else
	{
		mpfr_sub(tmp, up, I.up, MPFR_RNDD);
	}

	mpfr_abs(tmp, tmp, MPFR_RNDU);
	double d = mpfr_get_d(tmp, MPFR_RNDU);

	if(d > e)
	{
		mpfr_clear(tmp);
		return false;
	}

	if(mpfr_cmp(lo, I.lo) >= 0)
	{
		mpfr_sub(tmp, lo, I.lo, MPFR_RNDU);
	}
	else
	{
		mpfr_sub(tmp, lo, I.lo, MPFR_RNDD);
	}

	mpfr_abs(tmp, tmp, MPFR_RNDU);
	d = mpfr_get_d(tmp, MPFR_RNDU);

	if(d > e)
	{
		mpfr_clear(tmp);
		return false;
	}
	else
	{
		mpfr_clear(tmp);
		return true;
	}
}

double Interval::width() const
{
	mpfr_t tmp;
	mpfr_inits2(intervalNumPrecision, tmp, (mpfr_ptr) 0);

	mpfr_sub(tmp, up, lo, MPFR_RNDU);

	double dWidth = mpfr_get_d(tmp, MPFR_RNDU);
	mpfr_clear(tmp);

	return dWidth;
}

void Interval::width(Interval & W) const
{
	mpfr_t tmp;
	mpfr_inits2(intervalNumPrecision, tmp, (mpfr_ptr) 0);

	mpfr_sub(tmp, up, lo, MPFR_RNDU);

	mpfr_set(W.lo, tmp, MPFR_RNDD);
	mpfr_set(W.up, tmp, MPFR_RNDU);

	mpfr_clear(tmp);
}

double Interval::mag() const
{
	double inf = mpfr_get_d(lo, MPFR_RNDD);
	double sup = mpfr_get_d(up, MPFR_RNDU);

	inf = fabs(inf);
	sup = fabs(sup);

	return inf < sup ? sup : inf;
}

void Interval::mag(Real & m) const
{
	mpfr_t tmp1, tmp2;
	mpfr_inits2(intervalNumPrecision, tmp1, tmp2, (mpfr_ptr) 0);

	mpfr_abs(tmp1, lo, MPFR_RNDU);
	mpfr_abs(tmp2, up, MPFR_RNDU);

	if(mpfr_cmp(tmp1, tmp2) > 0)
	{
		mpfr_set(m.value, tmp1, MPFR_RNDU);
	}
	else
	{
		mpfr_set(m.value, tmp2, MPFR_RNDU);
	}

	mpfr_clear(tmp1);
	mpfr_clear(tmp2);
}

void Interval::mag(Interval & M) const
{
	mpfr_t tmp1, tmp2;
	mpfr_inits2(intervalNumPrecision, tmp1, tmp2, (mpfr_ptr) 0);

	mpfr_abs(tmp1, lo, MPFR_RNDU);
	mpfr_abs(tmp2, up, MPFR_RNDU);

	if(mpfr_cmp(tmp1, tmp2) > 0)
	{
		mpfr_set(M.lo, tmp1, MPFR_RNDD);
		mpfr_set(M.up, tmp1, MPFR_RNDU);
	}
	else
	{
		mpfr_set(M.lo, tmp2, MPFR_RNDD);
		mpfr_set(M.up, tmp2, MPFR_RNDU);
	}

	mpfr_clear(tmp1);
	mpfr_clear(tmp2);
}

void Interval::abs(Interval & result) const
{
	mpfr_t tmp1, tmp2;
	mpfr_inits2(intervalNumPrecision, tmp1, tmp2, (mpfr_ptr) 0);

	mpfr_abs(tmp1, lo, MPFR_RNDD);
	mpfr_abs(tmp2, up, MPFR_RNDD);

	if(mpfr_cmp(tmp1, tmp2) > 0)
	{
		mpfr_set(result.lo, tmp2, MPFR_RNDD);
	}
	else
	{
		mpfr_set(result.lo, tmp1, MPFR_RNDD);
	}

	mpfr_abs(tmp1, lo, MPFR_RNDU);
	mpfr_abs(tmp2, up, MPFR_RNDU);

	if(mpfr_cmp(tmp1, tmp2) > 0)
	{
		mpfr_set(result.up, tmp1, MPFR_RNDU);
	}
	else
	{
		mpfr_set(result.up, tmp2, MPFR_RNDU);
	}

	mpfr_clear(tmp1);
	mpfr_clear(tmp2);
}

void Interval::abs_assign()
{
	mpfr_t tmp1, tmp2, newLo, newUp;
	mpfr_inits2(intervalNumPrecision, tmp1, tmp2, newLo, newUp, (mpfr_ptr) 0);

	mpfr_abs(tmp1, lo, MPFR_RNDD);
	mpfr_abs(tmp2, up, MPFR_RNDD);

	if(mpfr_cmp(tmp1, tmp2) > 0)
	{
		mpfr_set(newLo, tmp2, MPFR_RNDD);
	}
	else
	{
		mpfr_set(newLo, tmp1, MPFR_RNDD);
	}

	mpfr_abs(tmp1, lo, MPFR_RNDU);
	mpfr_abs(tmp2, up, MPFR_RNDU);

	if(mpfr_cmp(tmp1, tmp2) > 0)
	{
		mpfr_set(newUp, tmp1, MPFR_RNDU);
	}
	else
	{
		mpfr_set(newUp, tmp2, MPFR_RNDU);
	}

	mpfr_set(lo, newLo, MPFR_RNDD);
	mpfr_set(up, newUp, MPFR_RNDU);

	mpfr_clear(tmp1);
	mpfr_clear(tmp2);
	mpfr_clear(newLo);
	mpfr_clear(newUp);
}

bool Interval::subseteq(const Interval & I) const
{
	if( (mpfr_cmp(I.lo, lo) <= 0) && (mpfr_cmp(I.up, up) >= 0) )
		return true;
	else
		return false;
}

bool Interval::supseteq(const Interval & I) const
{
	if( (mpfr_cmp(lo, I.lo) <= 0) && (mpfr_cmp(up, I.up) >= 0) )
		return true;
	else
		return false;
}

bool Interval::valid() const
{
	if(mpfr_cmp(up, lo) >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Interval::operator == (const Interval & I) const
{
	return ( (mpfr_cmp(lo, I.lo) == 0) && (mpfr_cmp(up, I.up) == 0) );
}

bool Interval::operator != (const Interval & I) const
{
	return ( (mpfr_cmp(lo, I.lo) != 0) || (mpfr_cmp(up, I.up) != 0) );
}

bool Interval::operator > (const Interval & I) const
{
	return ( (mpfr_cmp(lo, I.up) > 0) );
}

bool Interval::operator < (const Interval & I) const
{
	return ( (mpfr_cmp(up, I.lo) < 0) );
}

bool Interval::operator <= (const Interval & I) const
{
	return ( (mpfr_cmp(lo, I.lo) < 0) );
}

bool Interval::operator >= (const Interval & I) const
{
	return ( (mpfr_cmp(up, I.up) > 0) );
}

bool Interval::smallereq(const Interval & I) const
{
	return ( (mpfr_cmp(up, I.lo) <= 0) );
}

Interval & Interval::operator = (const Interval & I)
{
	if(this == &I)
		return *this;	// check for self assignment

	mpfr_set(lo, I.lo, MPFR_RNDD);
	mpfr_set(up, I.up, MPFR_RNDU);

	return *this;
}

Interval & Interval::operator = (const Real & r)
{
	mpfr_set(lo, r.value, MPFR_RNDD);
	mpfr_set(up, r.value, MPFR_RNDU);

	return *this;
}

Interval & Interval::operator = (const double d)
{
	mpfr_set_d(lo, d, MPFR_RNDD);
	mpfr_set_d(up, d, MPFR_RNDU);

	return *this;
}

Interval & Interval::operator += (const Interval & I)
{
	mpfr_add(lo, lo, I.lo, MPFR_RNDD);
	mpfr_add(up, up, I.up, MPFR_RNDU);

	return *this;
}

Interval & Interval::operator += (const double c)
{
	mpfr_add_d(lo, lo, c, MPFR_RNDD);
	mpfr_add_d(up, up, c, MPFR_RNDU);

	return *this;
}

Interval & Interval::operator -= (const Interval & I)
{
	mpfr_sub(lo, lo, I.up, MPFR_RNDD);
	mpfr_sub(up, up, I.lo, MPFR_RNDU);

	return *this;
}

Interval & Interval::operator -= (const double c)
{
	mpfr_sub_d(lo, lo, c, MPFR_RNDD);
	mpfr_sub_d(up, up, c, MPFR_RNDU);

	return *this;
}

Interval & Interval::operator *= (const Interval & I)
{
	mpfr_t result_lo, result_up, tmp1, tmp2;
	mpfr_inits2(intervalNumPrecision, result_lo, result_up, tmp1, tmp2, (mpfr_ptr) 0);

	if(mpfr_cmp_ui(lo, 0L) >= 0)
	{
		if(mpfr_cmp_ui(I.lo, 0L) >= 0)
		{
			mpfr_mul(result_lo, lo, I.lo, MPFR_RNDD);
			mpfr_mul(result_up, up, I.up, MPFR_RNDU);
		}
		else if(mpfr_cmp_ui(I.up, 0L) <= 0)
		{
			mpfr_mul(result_lo, up, I.lo, MPFR_RNDD);
			mpfr_mul(result_up, lo, I.up, MPFR_RNDU);
		}
		else
		{
			mpfr_mul(result_lo, up, I.lo, MPFR_RNDD);
			mpfr_mul(result_up, up, I.up, MPFR_RNDU);
		}
	}
	else if(mpfr_cmp_ui(up, 0L) <= 0)
	{
		if(mpfr_cmp_ui(I.lo, 0L) >= 0)
		{
			mpfr_mul(result_lo, lo, I.up, MPFR_RNDD);
			mpfr_mul(result_up, up, I.lo, MPFR_RNDU);
		}
		else if(mpfr_cmp_ui(I.up, 0L) <= 0)
		{
			mpfr_mul(result_lo, up, I.up, MPFR_RNDD);
			mpfr_mul(result_up, lo, I.lo, MPFR_RNDU);
		}
		else
		{
			mpfr_mul(result_lo, lo, I.up, MPFR_RNDD);
			mpfr_mul(result_up, lo, I.lo, MPFR_RNDU);
		}
	}
	else
	{
		if(mpfr_cmp_ui(I.lo, 0L) >= 0)
		{
			mpfr_mul(result_lo, lo, I.up, MPFR_RNDD);
			mpfr_mul(result_up, up, I.up, MPFR_RNDU);
		}
		else if(mpfr_cmp_ui(I.up, 0L) <= 0)
		{
			mpfr_mul(result_lo, up, I.lo, MPFR_RNDD);
			mpfr_mul(result_up, lo, I.lo, MPFR_RNDU);
		}
		else
		{
			mpfr_mul(tmp1, lo, I.up, MPFR_RNDD);
			mpfr_mul(tmp2, up, I.lo, MPFR_RNDD);

			if(mpfr_cmp(tmp1, tmp2) > 0)
			{
				mpfr_set(result_lo, tmp2, MPFR_RNDD);
			}
			else
			{
				mpfr_set(result_lo, tmp1, MPFR_RNDD);
			}

			mpfr_mul(tmp1, lo, I.lo, MPFR_RNDU);
			mpfr_mul(tmp2, up, I.up, MPFR_RNDU);

			if(mpfr_cmp(tmp1, tmp2) > 0)
			{
				mpfr_set(result_up, tmp1, MPFR_RNDU);
			}
			else
			{
				mpfr_set(result_up, tmp2, MPFR_RNDU);
			}
		}
	}

	mpfr_set(lo, result_lo, MPFR_RNDD);
	mpfr_set(up, result_up, MPFR_RNDU);

	mpfr_clears(result_lo, result_up, tmp1, tmp2, (mpfr_ptr) 0);

	return *this;
}

Interval & Interval::operator *= (const double c)
{
	Interval result;

	if(c > 0)
	{
		mpfr_mul_d(result.lo, lo, c, MPFR_RNDD);
		mpfr_mul_d(result.up, up, c, MPFR_RNDU);
	}
	else
	{
		mpfr_mul_d(result.lo, up, c, MPFR_RNDD);
		mpfr_mul_d(result.up, lo, c, MPFR_RNDU);
	}

	*this = result;

	return *this;
}

Interval & Interval::operator /= (const Interval & I)
{
	Interval tmp;

	I.rec(tmp);
	*this *= tmp;

	return *this;
}

Interval & Interval::operator /= (const double c)
{
	Interval result;

	if(c > 0)
	{
		mpfr_div_d(result.lo, lo, c, MPFR_RNDD);
		mpfr_div_d(result.up, up, c, MPFR_RNDU);
	}
	else
	{
		mpfr_div_d(result.lo, up, c, MPFR_RNDD);
		mpfr_div_d(result.up, lo, c, MPFR_RNDU);
	}

	*this = result;
	return *this;
}

Interval & Interval::operator ++ ()
{
	mpfr_add_ui(lo, lo, 1L, MPFR_RNDD);
	mpfr_add_ui(up, up, 1L, MPFR_RNDU);

	return *this;
}

Interval & Interval::operator -- ()
{
	mpfr_sub_ui(lo, lo, 1L, MPFR_RNDD);
	mpfr_sub_ui(up, up, 1L, MPFR_RNDU);

	return *this;
}

const Interval Interval::operator + (const Interval & I) const
{
	Interval result = *this;
	result += I;
	return result;
}

const Interval Interval::operator + (const double c) const
{
	Interval result = *this;
	result += c;
	return result;
}

const Interval Interval::operator - (const Interval & I) const
{
	Interval result = *this;
	result -= I;
	return result;
}

const Interval Interval::operator - (const double c) const
{
	Interval result = *this;
	result -= c;
	return result;
}

const Interval Interval::operator * (const Interval & I) const
{
	Interval result = *this;
	result *= I;
	return result;
}

const Interval Interval::operator * (const double c) const
{
	Interval result;

	if(c > 0)
	{
		mpfr_mul_d(result.lo, lo, c, MPFR_RNDD);
		mpfr_mul_d(result.up, up, c, MPFR_RNDU);
	}
	else
	{
		mpfr_mul_d(result.lo, up, c, MPFR_RNDD);
		mpfr_mul_d(result.up, lo, c, MPFR_RNDU);
	}

	return result;
}

const Interval Interval::operator / (const Interval & I) const
{
	Interval result = *this;
	result /= I;
	return result;
}

const Interval Interval::operator / (const double c) const
{
	Interval result;

	if(c > 0)
	{
		mpfr_div_d(result.lo, lo, c, MPFR_RNDD);
		mpfr_div_d(result.up, up, c, MPFR_RNDU);
	}
	else
	{
		mpfr_div_d(result.lo, up, c, MPFR_RNDD);
		mpfr_div_d(result.up, lo, c, MPFR_RNDU);
	}

	return result;
}

void Interval::sqrt(Interval & result) const
{
	if(mpfr_sgn(lo) < 0)
	{
		printf("Exception: Square root of a negative number.\n");
		exit(1);
	}

	mpfr_sqrt(result.lo, lo, MPFR_RNDD);
	mpfr_sqrt(result.up, up, MPFR_RNDU);
}

void Interval::inv(Interval & result) const
{
	mpfr_mul_si(result.lo, up, -1L, MPFR_RNDD);
	mpfr_mul_si(result.up, lo, -1L, MPFR_RNDU);
}

void Interval::rec(Interval & result) const
{
	if (mpfr_sgn(lo) <= 0 && mpfr_sgn(up) >= 0)
	{
		printf("Exception: Divided by 0.\n");
		exit(1);
	}
	else
	{
		mpfr_t tmp;
		mpfr_inits2(intervalNumPrecision, tmp, (mpfr_ptr) 0);
		mpfr_set(tmp, lo, MPFR_RNDD);

		mpfr_si_div(result.lo, 1L, up, MPFR_RNDD);
		mpfr_si_div(result.up, 1L, tmp, MPFR_RNDU);

		mpfr_clear(tmp);
	}
}

void Interval::sqrt_assign()
{
	if(mpfr_sgn(lo) < 0)
	{
		printf("Exception: Square root of a negative number.\n");
		exit(1);
	}

	mpfr_sqrt(lo, lo, MPFR_RNDD);
	mpfr_sqrt(up, up, MPFR_RNDU);
}

void Interval::inv_assign()
{
	Interval result;
	this->inv(result);
	*this = result;
}

void Interval::rec_assign()
{
	Interval result;
	this->rec(result);
	*this = result;
}

void Interval::add_assign(const double c)
{
	mpfr_add_d(lo, lo, c, MPFR_RNDD);
	mpfr_add_d(up, up, c, MPFR_RNDU);
}

void Interval::sub_assign(const double c)
{
	mpfr_sub_d(lo, lo, c, MPFR_RNDD);
	mpfr_sub_d(up, up, c, MPFR_RNDU);
}

void Interval::mul_assign(const double c)
{
	Interval result;

	if(c > 0)
	{
		mpfr_mul_d(result.lo, lo, c, MPFR_RNDD);
		mpfr_mul_d(result.up, up, c, MPFR_RNDU);
	}
	else
	{
		mpfr_mul_d(result.lo, up, c, MPFR_RNDD);
		mpfr_mul_d(result.up, lo, c, MPFR_RNDU);
	}

	*this = result;
}

void Interval::div_assign(const double c)
{
	Interval result;

	if(c > 0)
	{
		mpfr_div_d(result.lo, lo, c, MPFR_RNDD);
		mpfr_div_d(result.up, up, c, MPFR_RNDU);
	}
	else
	{
		mpfr_div_d(result.lo, up, c, MPFR_RNDD);
		mpfr_div_d(result.up, lo, c, MPFR_RNDU);
	}

	*this = result;
}

void Interval::mul_add(Interval *result, const Interval *intVec, const int size)
{
	mpfr_t tmp1, tmp2, tmp_up, tmp_lo;
	mpfr_inits2(intervalNumPrecision, tmp_up, tmp_lo, tmp1, tmp2, (mpfr_ptr) 0);

	if(mpfr_cmp_ui(lo, 0L) >= 0)
	{
		for(int i=0; i<size; ++i)
		{
			if(mpfr_cmp_ui(intVec[i].lo, 0L) >= 0)
			{
				mpfr_mul(tmp_lo, lo, intVec[i].lo, MPFR_RNDD);
				mpfr_mul(tmp_up, up, intVec[i].up, MPFR_RNDU);
			}
			else if(mpfr_cmp_ui(intVec[i].up, 0L) <= 0)
			{
				mpfr_mul(tmp_lo, up, intVec[i].lo, MPFR_RNDD);
				mpfr_mul(tmp_up, lo, intVec[i].up, MPFR_RNDU);
			}
			else
			{
				mpfr_mul(tmp_lo, up, intVec[i].lo, MPFR_RNDD);
				mpfr_mul(tmp_up, up, intVec[i].up, MPFR_RNDU);
			}

			mpfr_add(result[i].lo, result[i].lo, tmp_lo, MPFR_RNDD);
			mpfr_add(result[i].up, result[i].up, tmp_up, MPFR_RNDU);
		}
	}
	else if(mpfr_cmp_ui(up, 0L) <= 0)
	{
		for(int i=0; i<size; ++i)
		{
			if(mpfr_cmp_ui(intVec[i].lo, 0L) >= 0)
			{
				mpfr_mul(tmp_lo, lo, intVec[i].up, MPFR_RNDD);
				mpfr_mul(tmp_up, up, intVec[i].lo, MPFR_RNDU);
			}
			else if(mpfr_cmp_ui(intVec[i].up, 0L) <= 0)
			{
				mpfr_mul(tmp_lo, up, intVec[i].up, MPFR_RNDD);
				mpfr_mul(tmp_up, lo, intVec[i].lo, MPFR_RNDU);
			}
			else
			{
				mpfr_mul(tmp_lo, lo, intVec[i].up, MPFR_RNDD);
				mpfr_mul(tmp_up, lo, intVec[i].lo, MPFR_RNDU);
			}

			mpfr_add(result[i].lo, result[i].lo, tmp_lo, MPFR_RNDD);
			mpfr_add(result[i].up, result[i].up, tmp_up, MPFR_RNDU);
		}
	}
	else
	{
		for(int i=0; i<size; ++i)
		{
			if(mpfr_cmp_ui(intVec[i].lo, 0L) >= 0)
			{
				mpfr_mul(tmp_lo, lo, intVec[i].up, MPFR_RNDD);
				mpfr_mul(tmp_up, up, intVec[i].up, MPFR_RNDU);
			}
			else if(mpfr_cmp_ui(intVec[i].up, 0L) <= 0)
			{
				mpfr_mul(tmp_lo, up, intVec[i].lo, MPFR_RNDD);
				mpfr_mul(tmp_up, lo, intVec[i].lo, MPFR_RNDU);
			}
			else
			{
				mpfr_mul(tmp1, lo, intVec[i].up, MPFR_RNDD);
				mpfr_mul(tmp2, up, intVec[i].lo, MPFR_RNDD);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(tmp_lo, tmp2, MPFR_RNDD);
				}
				else
				{
					mpfr_set(tmp_lo, tmp1, MPFR_RNDD);
				}

				mpfr_mul(tmp1, lo, intVec[i].lo, MPFR_RNDU);
				mpfr_mul(tmp2, up, intVec[i].up, MPFR_RNDU);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(tmp_up, tmp1, MPFR_RNDU);
				}
				else
				{
					mpfr_set(tmp_up, tmp2, MPFR_RNDU);
				}
			}

			mpfr_add(result[i].lo, result[i].lo, tmp_lo, MPFR_RNDD);
			mpfr_add(result[i].up, result[i].up, tmp_up, MPFR_RNDU);
		}
	}

	mpfr_clears(tmp_lo, tmp_up, tmp1, tmp2, (mpfr_ptr) 0);
}

Interval Interval::pow(const int n) const
{
	Interval result;

	if(n % 2 == 1)		// n is odd
	{
		mpfr_pow_ui(result.lo, lo, n, MPFR_RNDD);		// a = lo^n
		mpfr_pow_ui(result.up, up, n, MPFR_RNDU);		// b = up^n
	}
	else				// n is even
	{
		if(mpfr_cmp_si(lo, 0L) >= 0)			// 0 <= lo <= up
		{
			mpfr_pow_ui(result.lo, lo, n, MPFR_RNDD);		// a = lo^n
			mpfr_pow_ui(result.up, up, n, MPFR_RNDU);		// b = up^n
		}
		else if(mpfr_cmp_si(up, 0L) <= 0)		// lo <= up <= 0
		{
			mpfr_pow_ui(result.lo, up, n, MPFR_RNDD);		// a = up^n
			mpfr_pow_ui(result.up, lo, n, MPFR_RNDU);		// b = lo^n
		}
		else									// lo < 0 < up
		{
			mpfr_t tmp1, tmp2;
			mpfr_inits2(intervalNumPrecision, tmp1, tmp2, (mpfr_ptr) 0);

			mpfr_pow_ui(tmp1, lo, n, MPFR_RNDU);
			mpfr_pow_ui(tmp2, up, n, MPFR_RNDU);

			// set b = max ( lo^n , up^n )
			if(mpfr_cmp(tmp1, tmp2) >= 0)
			{
				mpfr_set(result.up, tmp1, MPFR_RNDU);
			}
			else
			{
				mpfr_set(result.up, tmp2, MPFR_RNDU);
			}

			mpfr_clear(tmp1);
			mpfr_clear(tmp2);
		}
	}

	// return [a,b]
	return result;
}

Interval Interval::exp() const
{
	Interval result;

	mpfr_exp(result.lo, lo, MPFR_RNDD);
	mpfr_exp(result.up, up, MPFR_RNDU);

	return result;
}

Interval Interval::sin() const
{
	mpfr_t pi_up, pi_lo, tmp_up, tmp_lo;
	mpfr_inits2(intervalNumPrecision, pi_up, pi_lo, tmp_up, tmp_lo, (mpfr_ptr) 0);
	mpfr_set_str(pi_up, str_pi_up, 10, MPFR_RNDU);
	mpfr_set_str(pi_lo, str_pi_lo, 10, MPFR_RNDD);

	mpfr_div(tmp_up, up, pi_lo, MPFR_RNDU);
	mpfr_div(tmp_lo, lo, pi_up, MPFR_RNDD);

	mpfr_mul_si(tmp_up, tmp_up, 2, MPFR_RNDU);
	mpfr_mul_si(tmp_lo, tmp_lo, 2, MPFR_RNDD);

	mpfr_floor(tmp_up, tmp_up);
	mpfr_floor(tmp_lo, tmp_lo);

	int iUp = (int) mpfr_get_si(tmp_up, MPFR_RNDN);
	int iLo = (int) mpfr_get_si(tmp_lo, MPFR_RNDN);

	int iPeriod = iUp - iLo;

	if(iPeriod >= 4)
	{
		Interval result(-1,1);
		return result;
	}
	else
	{
		int modUp = iUp % 4;
		if(modUp < 0)
			modUp += 4;

		int modLo = iLo % 4;
		if(modLo < 0)
			modLo += 4;

		Interval result;
		mpfr_t tmp1, tmp2;
		mpfr_inits2(intervalNumPrecision, tmp1, tmp2, (mpfr_ptr) 0);

		switch(modLo)
		{
		case 0:
			switch(modUp)
			{
			case 0:
				if(iPeriod == 0)
				{
					mpfr_sin(result.lo, lo, MPFR_RNDD);
					mpfr_sin(result.up, up, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 1:
				mpfr_set_d(result.up, 1, MPFR_RNDU);

				mpfr_sin(tmp1, lo, MPFR_RNDD);
				mpfr_sin(tmp2, up, MPFR_RNDD);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.lo, tmp2, MPFR_RNDD);
				}
				else
				{
					mpfr_set(result.lo, tmp1, MPFR_RNDD);
				}
				break;
			case 2:
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				mpfr_sin(result.lo, up, MPFR_RNDD);
				break;
			case 3:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				break;
			}
			break;
		case 1:
			switch(modUp)
			{
			case 0:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);

				mpfr_sin(tmp1, lo, MPFR_RNDU);
				mpfr_sin(tmp2, up, MPFR_RNDU);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.up, tmp1, MPFR_RNDU);
				}
				else
				{
					mpfr_set(result.up, tmp2, MPFR_RNDU);
				}
				break;
			case 1:
				if(iPeriod == 0)
				{
					mpfr_sin(result.lo, up, MPFR_RNDD);
					mpfr_sin(result.up, lo, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 2:
				mpfr_sin(result.lo, up, MPFR_RNDD);
				mpfr_sin(result.up, lo, MPFR_RNDU);
				break;
			case 3:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_sin(result.up, lo, MPFR_RNDU);
				break;
			}
			break;
		case 2:
			switch(modUp)
			{
			case 0:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_sin(result.up, up, MPFR_RNDU);
				break;
			case 1:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				break;
			case 2:
				if(iPeriod == 0)
				{
					mpfr_sin(result.lo, up, MPFR_RNDD);
					mpfr_sin(result.up, lo, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 3:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);

				mpfr_sin(tmp1, lo, MPFR_RNDU);
				mpfr_sin(tmp2, up, MPFR_RNDU);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.up, tmp1, MPFR_RNDU);
				}
				else
				{
					mpfr_set(result.up, tmp2, MPFR_RNDU);
				}
				break;
			}
			break;
		case 3:
			switch(modUp)
			{
			case 0:
				mpfr_sin(result.lo, lo, MPFR_RNDD);
				mpfr_sin(result.up, up, MPFR_RNDU);
				break;
			case 1:
				mpfr_sin(result.lo, lo, MPFR_RNDD);
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				break;
			case 2:
				mpfr_set_d(result.up, 1, MPFR_RNDU);

				mpfr_sin(tmp1, lo, MPFR_RNDD);
				mpfr_sin(tmp2, up, MPFR_RNDD);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.lo, tmp2, MPFR_RNDD);
				}
				else
				{
					mpfr_set(result.lo, tmp1, MPFR_RNDD);
				}
				break;
			case 3:
				if(iPeriod == 0)
				{
					mpfr_sin(result.lo, lo, MPFR_RNDD);
					mpfr_sin(result.up, up, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			}
			break;
		}

		mpfr_clear(tmp1);
		mpfr_clear(tmp2);
		return result;
	}
}

Interval Interval::cos() const
{
	mpfr_t pi_up, pi_lo, tmp_up, tmp_lo;
	mpfr_inits2(intervalNumPrecision, pi_up, pi_lo, tmp_up, tmp_lo, (mpfr_ptr) 0);
	mpfr_set_str(pi_up, str_pi_up, 10, MPFR_RNDU);
	mpfr_set_str(pi_lo, str_pi_lo, 10, MPFR_RNDD);

	mpfr_div(tmp_up, up, pi_lo, MPFR_RNDU);
	mpfr_div(tmp_lo, lo, pi_up, MPFR_RNDD);

	mpfr_mul_si(tmp_up, tmp_up, 2, MPFR_RNDU);
	mpfr_mul_si(tmp_lo, tmp_lo, 2, MPFR_RNDD);

	mpfr_floor(tmp_up, tmp_up);
	mpfr_floor(tmp_lo, tmp_lo);

	int iUp = (int) mpfr_get_si(tmp_up, MPFR_RNDN);
	int iLo = (int) mpfr_get_si(tmp_lo, MPFR_RNDN);

	int iPeriod = iUp - iLo;

	if(iPeriod >= 4)
	{
		Interval result(-1,1);
		return result;
	}
	else
	{
		int modUp = iUp % 4;
		if(modUp < 0)
			modUp += 4;

		int modLo = iLo % 4;
		if(modLo < 0)
			modLo += 4;

		Interval result;
		mpfr_t tmp1, tmp2;
		mpfr_inits2(intervalNumPrecision, tmp1, tmp2, (mpfr_ptr) 0);

		switch(modLo)
		{
		case 0:
			switch(modUp)
			{
			case 0:
				if(iPeriod == 0)
				{
					mpfr_cos(result.lo, up, MPFR_RNDD);
					mpfr_cos(result.up, lo, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 1:
				mpfr_cos(result.lo, up, MPFR_RNDD);
				mpfr_cos(result.up, lo, MPFR_RNDU);
				break;
			case 2:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_cos(result.up, lo, MPFR_RNDU);
				break;
			case 3:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);

				mpfr_cos(tmp1, lo, MPFR_RNDU);
				mpfr_cos(tmp2, up, MPFR_RNDU);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.up, tmp1, MPFR_RNDU);
				}
				else
				{
					mpfr_set(result.up, tmp2, MPFR_RNDU);
				}
				break;
			}
			break;
		case 1:
			switch(modUp)
			{
			case 0:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				break;
			case 1:
				if(iPeriod == 0)
				{
					mpfr_cos(result.lo, up, MPFR_RNDD);
					mpfr_cos(result.up, lo, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 2:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);

				mpfr_cos(tmp1, lo, MPFR_RNDU);
				mpfr_cos(tmp2, up, MPFR_RNDU);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.up, tmp1, MPFR_RNDU);
				}
				else
				{
					mpfr_set(result.up, tmp2, MPFR_RNDU);
				}
				break;
			case 3:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_cos(result.up, up, MPFR_RNDU);
				break;
			}
			break;
		case 2:
			switch(modUp)
			{
			case 0:
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				mpfr_cos(result.lo, lo, MPFR_RNDD);
				break;
			case 1:
				mpfr_set_d(result.up, 1, MPFR_RNDU);

				mpfr_cos(tmp1, lo, MPFR_RNDD);
				mpfr_cos(tmp2, up, MPFR_RNDD);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.lo, tmp2, MPFR_RNDD);
				}
				else
				{
					mpfr_set(result.lo, tmp1, MPFR_RNDD);
				}
				break;
			case 2:
				if(iPeriod == 0)
				{
					mpfr_cos(result.lo, lo, MPFR_RNDD);
					mpfr_cos(result.up, up, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 3:
				mpfr_cos(result.lo, lo, MPFR_RNDD);
				mpfr_cos(result.up, up, MPFR_RNDU);
				break;
			}
			break;
		case 3:
			switch(modUp)
			{
			case 0:
				mpfr_set_d(result.up, 1, MPFR_RNDU);

				mpfr_cos(tmp1, lo, MPFR_RNDD);
				mpfr_cos(tmp2, up, MPFR_RNDD);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.lo, tmp2, MPFR_RNDD);
				}
				else
				{
					mpfr_set(result.lo, tmp1, MPFR_RNDD);
				}
				break;
			case 1:
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				mpfr_cos(result.lo, up, MPFR_RNDD);
				break;
			case 2:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				break;
			case 3:
				if(iPeriod == 0)
				{
					mpfr_cos(result.lo, lo, MPFR_RNDD);
					mpfr_cos(result.up, up, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			}
			break;
		}

		mpfr_clear(tmp1);
		mpfr_clear(tmp2);
		return result;
	}
}

Interval Interval::log() const
{
	if(mpfr_sgn(lo) <= 0)
	{
		printf("Exception: Logarithm of a non-positive number.\n");
		exit(1);
	}
	else
	{
		Interval result;
		mpfr_log(result.lo, lo, MPFR_RNDD);
		mpfr_log(result.up, up, MPFR_RNDU);
		return result;
	}
}

void Interval::pow_assign(const int n)
{
	mpfr_t tmp1, tmp2;
	mpfr_inits2(intervalNumPrecision, tmp1, tmp2, (mpfr_ptr) 0);

	if(n % 2 == 1)		// n is odd
	{
		mpfr_pow_ui(lo, lo, n, MPFR_RNDD);		// a = lo^n
		mpfr_pow_ui(up, up, n, MPFR_RNDU);		// b = up^n
	}
	else				// n is even
	{
		if(mpfr_cmp_si(lo, 0L) >= 0)			// 0 <= lo <= up
		{
			mpfr_pow_ui(lo, lo, n, MPFR_RNDD);		// a = lo^n
			mpfr_pow_ui(up, up, n, MPFR_RNDU);		// b = up^n
		}
		else if(mpfr_cmp_si(up, 0L) <= 0)		// lo <= up <= 0
		{
			mpfr_pow_ui(tmp1, up, n, MPFR_RNDD);		// a = up^n
			mpfr_pow_ui(tmp2, lo, n, MPFR_RNDU);		// b = lo^n

			mpfr_set(lo, tmp1, MPFR_RNDD);
			mpfr_set(up, tmp2, MPFR_RNDU);
		}
		else									// lo < 0 < up
		{
			mpfr_pow_ui(tmp1, lo, n, MPFR_RNDU);
			mpfr_pow_ui(tmp2, up, n, MPFR_RNDU);

			// set b = max ( lo^n , up^n )
			if(mpfr_cmp(tmp1, tmp2) >= 0)
			{
				mpfr_set(up, tmp1, MPFR_RNDU);
			}
			else
			{
				mpfr_set(up, tmp2, MPFR_RNDU);
			}

			mpfr_set_si(lo, 0L, MPFR_RNDD);
		}
	}

	mpfr_clear(tmp1);
	mpfr_clear(tmp2);
}

void Interval::exp_assign()
{
	mpfr_exp(lo, lo, MPFR_RNDD);
	mpfr_exp(up, up, MPFR_RNDU);
}

void Interval::sin_assign()
{
	mpfr_t pi_up, pi_lo, tmp_up, tmp_lo;
	mpfr_inits2(intervalNumPrecision, pi_up, pi_lo, tmp_up, tmp_lo, (mpfr_ptr) 0);
	mpfr_set_str(pi_up, str_pi_up, 10, MPFR_RNDU);
	mpfr_set_str(pi_lo, str_pi_lo, 10, MPFR_RNDD);

	mpfr_div(tmp_up, up, pi_lo, MPFR_RNDU);
	mpfr_div(tmp_lo, lo, pi_up, MPFR_RNDD);

	mpfr_mul_si(tmp_up, tmp_up, 2, MPFR_RNDU);
	mpfr_mul_si(tmp_lo, tmp_lo, 2, MPFR_RNDD);

	mpfr_floor(tmp_up, tmp_up);
	mpfr_floor(tmp_lo, tmp_lo);

	int iUp = (int) mpfr_get_si(tmp_up, MPFR_RNDN);
	int iLo = (int) mpfr_get_si(tmp_lo, MPFR_RNDN);

	int iPeriod = iUp - iLo;

	if(iPeriod >= 4)
	{
		mpfr_set_d(lo, -1, MPFR_RNDD);
		mpfr_set_d(up, 1, MPFR_RNDU);
	}
	else
	{
		int modUp = iUp % 4;
		if(modUp < 0)
			modUp += 4;

		int modLo = iLo % 4;
		if(modLo < 0)
			modLo += 4;

		Interval result;
		mpfr_t tmp1, tmp2;
		mpfr_inits2(intervalNumPrecision, tmp1, tmp2, (mpfr_ptr) 0);

		switch(modLo)
		{
		case 0:
			switch(modUp)
			{
			case 0:
				if(iPeriod == 0)
				{
					mpfr_sin(result.lo, lo, MPFR_RNDD);
					mpfr_sin(result.up, up, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 1:
				mpfr_set_d(result.up, 1, MPFR_RNDU);

				mpfr_sin(tmp1, lo, MPFR_RNDD);
				mpfr_sin(tmp2, up, MPFR_RNDD);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.lo, tmp2, MPFR_RNDD);
				}
				else
				{
					mpfr_set(result.lo, tmp1, MPFR_RNDD);
				}
				break;
			case 2:
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				mpfr_sin(result.lo, up, MPFR_RNDD);
				break;
			case 3:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				break;
			}
			break;
		case 1:
			switch(modUp)
			{
			case 0:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);

				mpfr_sin(tmp1, lo, MPFR_RNDU);
				mpfr_sin(tmp2, up, MPFR_RNDU);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.up, tmp1, MPFR_RNDU);
				}
				else
				{
					mpfr_set(result.up, tmp2, MPFR_RNDU);
				}
				break;
			case 1:
				if(iPeriod == 0)
				{
					mpfr_sin(result.lo, up, MPFR_RNDD);
					mpfr_sin(result.up, lo, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 2:
				mpfr_sin(result.lo, up, MPFR_RNDD);
				mpfr_sin(result.up, lo, MPFR_RNDU);
				break;
			case 3:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_sin(result.up, lo, MPFR_RNDU);
				break;
			}
			break;
		case 2:
			switch(modUp)
			{
			case 0:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_sin(result.up, up, MPFR_RNDU);
				break;
			case 1:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				break;
			case 2:
				if(iPeriod == 0)
				{
					mpfr_sin(result.lo, up, MPFR_RNDD);
					mpfr_sin(result.up, lo, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 3:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);

				mpfr_sin(tmp1, lo, MPFR_RNDU);
				mpfr_sin(tmp2, up, MPFR_RNDU);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.up, tmp1, MPFR_RNDU);
				}
				else
				{
					mpfr_set(result.up, tmp2, MPFR_RNDU);
				}
				break;
			}
			break;
		case 3:
			switch(modUp)
			{
			case 0:
				mpfr_sin(result.lo, lo, MPFR_RNDD);
				mpfr_sin(result.up, up, MPFR_RNDU);
				break;
			case 1:
				mpfr_sin(result.lo, lo, MPFR_RNDD);
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				break;
			case 2:
				mpfr_set_d(result.up, 1, MPFR_RNDU);

				mpfr_sin(tmp1, lo, MPFR_RNDD);
				mpfr_sin(tmp2, up, MPFR_RNDD);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.lo, tmp2, MPFR_RNDD);
				}
				else
				{
					mpfr_set(result.lo, tmp1, MPFR_RNDD);
				}
				break;
			case 3:
				if(iPeriod == 0)
				{
					mpfr_sin(result.lo, lo, MPFR_RNDD);
					mpfr_sin(result.up, up, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			}
			break;
		}

		mpfr_clear(tmp1);
		mpfr_clear(tmp2);
		*this = result;
	}
}

void Interval::cos_assign()
{
	mpfr_t pi_up, pi_lo, tmp_up, tmp_lo;
	mpfr_inits2(intervalNumPrecision, pi_up, pi_lo, tmp_up, tmp_lo, (mpfr_ptr) 0);
	mpfr_set_str(pi_up, str_pi_up, 10, MPFR_RNDU);
	mpfr_set_str(pi_lo, str_pi_lo, 10, MPFR_RNDD);

	mpfr_div(tmp_up, up, pi_lo, MPFR_RNDU);
	mpfr_div(tmp_lo, lo, pi_up, MPFR_RNDD);

	mpfr_mul_si(tmp_up, tmp_up, 2, MPFR_RNDU);
	mpfr_mul_si(tmp_lo, tmp_lo, 2, MPFR_RNDD);

	mpfr_floor(tmp_up, tmp_up);
	mpfr_floor(tmp_lo, tmp_lo);

	int iUp = (int) mpfr_get_si(tmp_up, MPFR_RNDN);
	int iLo = (int) mpfr_get_si(tmp_lo, MPFR_RNDN);

	int iPeriod = iUp - iLo;

	if(iPeriod >= 4)
	{
		mpfr_set_d(lo, -1, MPFR_RNDD);
		mpfr_set_d(up, 1, MPFR_RNDU);
	}
	else
	{
		int modUp = iUp % 4;
		if(modUp < 0)
			modUp += 4;

		int modLo = iLo % 4;
		if(modLo < 0)
			modLo += 4;

		Interval result;
		mpfr_t tmp1, tmp2;
		mpfr_inits2(intervalNumPrecision, tmp1, tmp2, (mpfr_ptr) 0);

		switch(modLo)
		{
		case 0:
			switch(modUp)
			{
			case 0:
				if(iPeriod == 0)
				{
					mpfr_cos(result.lo, up, MPFR_RNDD);
					mpfr_cos(result.up, lo, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 1:
				mpfr_cos(result.lo, up, MPFR_RNDD);
				mpfr_cos(result.up, lo, MPFR_RNDU);
				break;
			case 2:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_cos(result.up, lo, MPFR_RNDU);
				break;
			case 3:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);

				mpfr_cos(tmp1, lo, MPFR_RNDU);
				mpfr_cos(tmp2, up, MPFR_RNDU);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.up, tmp1, MPFR_RNDU);
				}
				else
				{
					mpfr_set(result.up, tmp2, MPFR_RNDU);
				}
				break;
			}
			break;
		case 1:
			switch(modUp)
			{
			case 0:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				break;
			case 1:
				if(iPeriod == 0)
				{
					mpfr_cos(result.lo, up, MPFR_RNDD);
					mpfr_cos(result.up, lo, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 2:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);

				mpfr_cos(tmp1, lo, MPFR_RNDU);
				mpfr_cos(tmp2, up, MPFR_RNDU);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.up, tmp1, MPFR_RNDU);
				}
				else
				{
					mpfr_set(result.up, tmp2, MPFR_RNDU);
				}
				break;
			case 3:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_cos(result.up, up, MPFR_RNDU);
				break;
			}
			break;
		case 2:
			switch(modUp)
			{
			case 0:
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				mpfr_cos(result.lo, lo, MPFR_RNDD);
				break;
			case 1:
				mpfr_set_d(result.up, 1, MPFR_RNDU);

				mpfr_cos(tmp1, lo, MPFR_RNDD);
				mpfr_cos(tmp2, up, MPFR_RNDD);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.lo, tmp2, MPFR_RNDD);
				}
				else
				{
					mpfr_set(result.lo, tmp1, MPFR_RNDD);
				}
				break;
			case 2:
				if(iPeriod == 0)
				{
					mpfr_cos(result.lo, lo, MPFR_RNDD);
					mpfr_cos(result.up, up, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			case 3:
				mpfr_cos(result.lo, lo, MPFR_RNDD);
				mpfr_cos(result.up, up, MPFR_RNDU);
				break;
			}
			break;
		case 3:
			switch(modUp)
			{
			case 0:
				mpfr_set_d(result.up, 1, MPFR_RNDU);

				mpfr_cos(tmp1, lo, MPFR_RNDD);
				mpfr_cos(tmp2, up, MPFR_RNDD);

				if(mpfr_cmp(tmp1, tmp2) > 0)
				{
					mpfr_set(result.lo, tmp2, MPFR_RNDD);
				}
				else
				{
					mpfr_set(result.lo, tmp1, MPFR_RNDD);
				}
				break;
			case 1:
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				mpfr_cos(result.lo, up, MPFR_RNDD);
				break;
			case 2:
				mpfr_set_d(result.lo, -1, MPFR_RNDD);
				mpfr_set_d(result.up, 1, MPFR_RNDU);
				break;
			case 3:
				if(iPeriod == 0)
				{
					mpfr_cos(result.lo, lo, MPFR_RNDD);
					mpfr_cos(result.up, up, MPFR_RNDU);
				}
				else
				{
					mpfr_set_d(result.lo, -1, MPFR_RNDD);
					mpfr_set_d(result.up, 1, MPFR_RNDU);
				}
				break;
			}
			break;
		}

		mpfr_clear(tmp1);
		mpfr_clear(tmp2);
		*this = result;
	}
}

void Interval::log_assign()
{
	if(mpfr_sgn(lo) <= 0)
	{
		printf("Exception: Logarithm of a non-positive number.\n");
		exit(1);
	}
	else
	{
		mpfr_log(lo, lo, MPFR_RNDD);
		mpfr_log(up, up, MPFR_RNDU);
	}
}

double Interval::widthRatio(const Interval & I) const
{
	mpfr_t width1, width2, ratio;
	mpfr_inits2(intervalNumPrecision, width1, width2, ratio, (mpfr_ptr) 0);

	mpfr_sub(width1, up, lo, MPFR_RNDU);
	mpfr_sub(width2, I.up, I.lo, MPFR_RNDU);

	mpfr_div(ratio, width2, width1, MPFR_RNDU);		// we assume that width1 >= width2

	double result = mpfr_get_d(ratio, MPFR_RNDU);

	mpfr_clear(width1);
	mpfr_clear(width2);
	mpfr_clear(ratio);

	return result;
}

void Interval::toString(std::string & result) const
{
	char strTemp[30];

	std::string strInt;
	strInt += '[';

	mpfr_sprintf(strTemp, "%.10RDe", lo);
	std::string strLo(strTemp);

	strInt += strLo;
	strInt += ' ';
	strInt += ',';
	strInt += ' ';

	mpfr_sprintf(strTemp, "%.10RUe", up);
	std::string strUp(strTemp);
	strInt += strUp;
	strInt += ']';

	result = strInt;
}

void Interval::dump(FILE *fp) const
{
	fprintf (fp, "[");
	mpfr_out_str(fp, 10, PN, lo, MPFR_RNDD);
	fprintf(fp, " , ");
	mpfr_out_str(fp, 10, PN, up, MPFR_RNDU);
	fprintf(fp, "]");
}

void Interval::output(FILE * fp, const char * msg, const char * msg2) const
{
	fprintf (fp, "%s [ ", msg);
	mpfr_out_str(fp, 10, PN, lo, MPFR_RNDD);
	fprintf(fp, " , ");
	mpfr_out_str(fp, 10, PN, up, MPFR_RNDU);
	fprintf(fp, " ] %s", msg2);
}













