/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.

  The code is released as is under the GNU General Public License (GPL).
---*/

#include "TaylorModel.h"
#include "expression.h"

flowstar::ParseSetting parseSetting;
flowstar::ParseResult parseResult;

using namespace flowstar;


// class Taylor_Model_Computation_Setting

Taylor_Model_Computation_Setting::Taylor_Model_Computation_Setting(const Variables & variables, const std::vector<Interval> & ranges, const Variables & parameters)
{
	vars = variables;
	domain = ranges;
	pars = parameters;
}

Taylor_Model_Computation_Setting::Taylor_Model_Computation_Setting(const Taylor_Model_Computation_Setting & setting)
{
	vars = setting.vars;
	pars = setting.pars;
	cutoff_threshold = setting.cutoff_threshold;
	domain = setting.domain;
}

Taylor_Model_Computation_Setting::~Taylor_Model_Computation_Setting()
{
	domain.clear();
}

Taylor_Model_Computation_Setting & Taylor_Model_Computation_Setting::operator = (const Taylor_Model_Computation_Setting & setting)
{
	if(this == &setting)
		return *this;

	vars = setting.vars;
	pars = setting.pars;
	cutoff_threshold = setting.cutoff_threshold;
	domain = setting.domain;

	return *this;
}

bool Taylor_Model_Computation_Setting::setCutoff(const Interval & cutoff)
{
	if(cutoff.valid())
	{
		cutoff_threshold = cutoff;
		return true;
	}
	else
	{
		printf("Cutoff threshold is not a valid interval.\n");
		return false;
	}
}

bool Taylor_Model_Computation_Setting::setPrecision(const int prec)
{
	if(prec >= 53)
	{
		intervalNumPrecision = prec;
		return true;
	}
	else
	{
//		printf("Precision should be large than 53.\n");
//		return false;

		intervalNumPrecision = 53;
		return true;
	}
}



// class TaylorModel

TaylorModel::TaylorModel()
{
}

TaylorModel::TaylorModel(const Interval & I, const int numVars)
{
	Interval intZero;
	Polynomial polyTemp(I, numVars);
	expansion = polyTemp;
	remainder = intZero;
}

TaylorModel::TaylorModel(const Polynomial & polyExp):expansion(polyExp)
{
}

TaylorModel::TaylorModel(const Polynomial & polyExp, const Interval & I):expansion(polyExp), remainder(I)
{
}

TaylorModel::TaylorModel(const RowVector & coefficients)
{
	Interval intZero;
	Polynomial polyTemp(coefficients);
	expansion = polyTemp;
	remainder = intZero;
}

TaylorModel::TaylorModel(const RowVector & coefficients, const Interval & I)
{
	Polynomial polyTemp(coefficients);
	expansion = polyTemp;
	remainder = I;
}

TaylorModel::TaylorModel(const std::vector<Interval> & coefficients)
{
	Interval intZero;
	Polynomial polyTemp(coefficients);
	expansion = polyTemp;
	remainder = intZero;
}

TaylorModel::TaylorModel(const std::vector<Interval> & coefficients, const Interval & I)
{
	Polynomial polyTemp(coefficients);
	expansion = polyTemp;
	remainder = I;
}

TaylorModel::TaylorModel(const Interval *pcoefficients, const int numVars)
{
	Interval intZero;
	Polynomial polyTemp(pcoefficients, numVars);
	expansion = polyTemp;
	remainder = intZero;
}

TaylorModel::TaylorModel(const TaylorModel & tm):expansion(tm.expansion), remainder(tm.remainder)
{
}

TaylorModel::~TaylorModel()
{
	expansion.clear();
}

TaylorModel::TaylorModel(const std::string & strPolynomial, const Variables & vars)
{
	parsePolynomial.clear();

	std::string prefix(str_prefix_multivariate_polynomial);
	std::string suffix(str_suffix);

	parsePolynomial.strPolynomial = prefix + strPolynomial + suffix;
	parsePolynomial.variables = vars;

	parseMultivariatePolynomial();

	this->expansion = parsePolynomial.result;
}

TaylorModel::TaylorModel(const std::string & strPolynomial, const Interval & rem, const Variables & vars)
{
	parsePolynomial.clear();

	std::string prefix(str_prefix_multivariate_polynomial);
	std::string suffix(str_suffix);

	parsePolynomial.strPolynomial = prefix + strPolynomial + suffix;
	parsePolynomial.variables = vars;

	parseMultivariatePolynomial();

	expansion = parsePolynomial.result;
	remainder = rem;
}

void TaylorModel::clear()
{
	Interval intZero;
	expansion.clear();
	remainder = intZero;
}

void TaylorModel::dump_interval(FILE *fp, std::vector<std::string> const & varNames) const
{
	expansion.dump_interval(fp, varNames);
	fprintf(fp, " + ");
	remainder.dump(fp);
	fprintf(fp, "\n");
}

void TaylorModel::dump_constant(FILE *fp, std::vector<std::string> const & varNames) const
{
	expansion.dump_constant(fp, varNames);
	fprintf(fp, " + ");
	remainder.dump(fp);
	fprintf(fp, "\n");
}

void TaylorModel::output(FILE *fp, const Variables & vars) const
{
	expansion.dump_interval(fp, vars.varNames);
	fprintf(fp, " + ");
	remainder.dump(fp);
	fprintf(fp, "\n");
}

void TaylorModel::constant(Interval & result) const
{
	expansion.constant(result);
}

void TaylorModel::constant(Real & result) const
{
	expansion.constant(result);
}

void TaylorModel::intEval(Interval & result, const std::vector<Interval> & domain) const
{
	expansion.intEval(result, domain);
	result += remainder;
}

void TaylorModel::intEvalNormal(Interval & result, const std::vector<Interval> & step_exp_table) const
{
	expansion.intEvalNormal(result, step_exp_table);
	result += remainder;
}

void TaylorModel::ctrunc(const std::vector<Interval> & domain, const int order)
{
	Interval I;
	expansion.ctrunc(I, domain, order);

	remainder += I;
}

void TaylorModel::nctrunc(const int order)
{
	expansion.nctrunc(order);
}

void TaylorModel::ctrunc_normal(const std::vector<Interval> & step_exp_table, const int order)
{
	Interval I;
	expansion.ctrunc_normal(I, step_exp_table, order);

	remainder += I;
}

void TaylorModel::inv(TaylorModel & result) const
{
	expansion.inv(result.expansion);
	remainder.inv(result.remainder);
}

void TaylorModel::inv_assign()
{
	expansion.inv_assign();
	remainder.inv_assign();
}

void TaylorModel::add(TaylorModel & result, const TaylorModel & tm) const
{
	result.expansion = expansion + tm.expansion;
	result.remainder = remainder + tm.remainder;
}

void TaylorModel::sub(TaylorModel & result, const TaylorModel & tm) const
{
	result.expansion = expansion - tm.expansion;
	result.remainder = remainder - tm.remainder;
}

void TaylorModel::add_assign(const TaylorModel & tm)
{
	expansion += tm.expansion;
	remainder += tm.remainder;
}

void TaylorModel::sub_assign(const TaylorModel & tm)
{
	expansion -= tm.expansion;
	remainder -= tm.remainder;
}

void TaylorModel::mul_ctrunc(TaylorModel & result, const TaylorModel & tm, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const
{
	Interval intZero;
	Polynomial P1xP2;
	Interval P1xI2, P2xI1, I1xI2;

	P1xP2 = expansion * tm.expansion;

	if(!tm.remainder.subseteq(intZero))
	{
		expansion.intEval(P1xI2, domain);
		P1xI2 *= tm.remainder;
	}

	if(!remainder.subseteq(intZero))
	{
		tm.expansion.intEval(P2xI1, domain);
		P2xI1 *= remainder;
	}

	I1xI2 = remainder * tm.remainder;

	result.expansion = P1xP2;

	result.remainder = I1xI2;
	result.remainder += P2xI1;
	result.remainder += P1xI2;

	result.ctrunc(domain, order);

	result.cutoff(domain, cutoff_threshold);
}

void TaylorModel::mul_ctrunc_normal(TaylorModel & result, const TaylorModel & tm, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold) const
{
	Interval intZero;
	Polynomial P1xP2;
	Interval P1xI2, P2xI1, I1xI2;

	P1xP2 = expansion * tm.expansion;

	if(!tm.remainder.subseteq(intZero))
	{
		expansion.intEvalNormal(P1xI2, step_exp_table);
		P1xI2 *= tm.remainder;
	}

	if(!remainder.subseteq(intZero))
	{
		tm.expansion.intEvalNormal(P2xI1, step_exp_table);
		P2xI1 *= remainder;
	}

	I1xI2 = remainder * tm.remainder;

	result.expansion = P1xP2;

	result.remainder = I1xI2;
	result.remainder += P2xI1;
	result.remainder += P1xI2;

	result.ctrunc_normal(step_exp_table, order);

	result.cutoff_normal(step_exp_table, cutoff_threshold);
}

void TaylorModel::mul_no_remainder(TaylorModel & result, const TaylorModel & tm, const int order, const Interval & cutoff_threshold) const
{
	result.expansion = expansion * tm.expansion;
	result.expansion.nctrunc(order);

	result.expansion.cutoff(cutoff_threshold);
}

void TaylorModel::mul_no_remainder_no_cutoff(TaylorModel & result, const TaylorModel & tm, const int order) const
{
	result.expansion = expansion * tm.expansion;
	result.expansion.nctrunc(order);
}

void TaylorModel::mul(TaylorModel & result, const Interval & I) const
{
	expansion.mul(result.expansion, I);
	result.remainder = remainder * I;
}

void TaylorModel::mul_ctrunc_assign(const TaylorModel & tm, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold)
{
	TaylorModel result;
	mul_ctrunc(result, tm, domain, order, cutoff_threshold);
	*this = result;
}

void TaylorModel::mul_ctrunc_normal_assign(const TaylorModel & tm, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold)
{
	TaylorModel result;
	mul_ctrunc_normal(result, tm, step_exp_table, order, cutoff_threshold);
	*this = result;
}

void TaylorModel::mul_no_remainder_assign(const TaylorModel & tm, const int order, const Interval & cutoff_threshold)
{
	TaylorModel result;
	mul_no_remainder(result, tm, order, cutoff_threshold);
	*this = result;
}

void TaylorModel::mul_no_remainder_no_cutoff_assign(const TaylorModel & tm, const int order)
{
	TaylorModel result;
	mul_no_remainder_no_cutoff(result, tm, order);
	*this = result;
}

void TaylorModel::mul_assign(const Interval & I)
{
	TaylorModel result;
	mul(result, I);
	*this = result;
}

void TaylorModel::mul_insert(TaylorModel & result, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold) const
{
	Interval intZero;
	Polynomial P1xP2;
	Interval P1xI2, P2xI1, I1xI2;

	P1xP2 = expansion * tm.expansion;

	if(!tm.remainder.subseteq(intZero))
	{
		expansion.intEval(P1xI2, domain);
		P1xI2 *= tm.remainder;
	}

	if(!remainder.subseteq(intZero))
	{
		P2xI1 = tmPolyRange * remainder;
	}

	I1xI2 = remainder * tm.remainder;

	result.expansion = P1xP2;

	result.remainder = I1xI2;
	result.remainder += P2xI1;
	result.remainder += P1xI2;

	result.cutoff(domain, cutoff_threshold);
}

void TaylorModel::mul_insert_normal(TaylorModel & result, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold) const
{
	Interval intZero;
	Polynomial P1xP2;
	Interval P1xI2, P2xI1, I1xI2;

	P1xP2 = expansion * tm.expansion;

	if(!tm.remainder.subseteq(intZero))
	{
		expansion.intEvalNormal(P1xI2, step_exp_table);
		P1xI2 *= tm.remainder;
	}

	if(!remainder.subseteq(intZero))
	{
		P2xI1 = tmPolyRange * remainder;
	}

	I1xI2 = remainder * tm.remainder;

	result.expansion = P1xP2;

	result.remainder = I1xI2;
	result.remainder += P2xI1;
	result.remainder += P1xI2;

	result.cutoff_normal(step_exp_table, cutoff_threshold);
}


void TaylorModel::mul_insert_ctrunc(TaylorModel & result, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const
{
	Interval intZero;
	Polynomial P1xP2;
	Interval P1xI2, P2xI1, I1xI2;

	P1xP2 = expansion * tm.expansion;

	if(!tm.remainder.subseteq(intZero))
	{
		expansion.intEval(P1xI2, domain);
		P1xI2 *= tm.remainder;
	}

	if(!remainder.subseteq(intZero))
	{
		P2xI1 = tmPolyRange * remainder;
	}

	I1xI2 = remainder * tm.remainder;

	result.expansion = P1xP2;

	result.remainder = I1xI2;
	result.remainder += P2xI1;
	result.remainder += P1xI2;

	result.ctrunc(domain, order);
	result.cutoff(domain, cutoff_threshold);
}

void TaylorModel::mul_insert_ctrunc_normal(TaylorModel & result, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold) const
{
	Interval intZero;
	Polynomial P1xP2;
	Interval P1xI2, P2xI1, I1xI2;

	P1xP2 = expansion * tm.expansion;

	if(!tm.remainder.subseteq(intZero))
	{
		expansion.intEvalNormal(P1xI2, step_exp_table);
		P1xI2 *= tm.remainder;
	}

	if(!remainder.subseteq(intZero))
	{
		P2xI1 = tmPolyRange * remainder;
	}

	I1xI2 = remainder * tm.remainder;

	result.expansion = P1xP2;

	result.remainder = I1xI2;
	result.remainder += P2xI1;
	result.remainder += P1xI2;

	result.ctrunc_normal(step_exp_table, order);
	result.cutoff_normal(step_exp_table, cutoff_threshold);
}

void TaylorModel::mul_insert_ctrunc_normal_no_cutoff(TaylorModel & result, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order) const
{
	Interval intZero;
	Polynomial P1xP2;
	Interval P1xI2, P2xI1, I1xI2;

	P1xP2 = expansion * tm.expansion;

	if(!tm.remainder.subseteq(intZero))
	{
		expansion.intEvalNormal(P1xI2, step_exp_table);
		P1xI2 *= tm.remainder;
	}

	if(!remainder.subseteq(intZero))
	{
		P2xI1 = tmPolyRange * remainder;
	}

	I1xI2 = remainder * tm.remainder;

	result.expansion = P1xP2;

	result.remainder = I1xI2;
	result.remainder += P2xI1;
	result.remainder += P1xI2;

	result.ctrunc_normal(step_exp_table, order);
}

void TaylorModel::mul_insert_ctrunc_normal(TaylorModel & result, Interval & tm1, Interval & intTrunc, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold) const
{
	Polynomial P1xP2;
	Interval P1xI2, P2xI1, I1xI2;

	P1xP2 = expansion * tm.expansion;

	Interval intZero;
	tm1 = intZero;
	intTrunc = intZero;

	if(!tm.remainder.subseteq(intZero))
	{
		expansion.intEvalNormal(P1xI2, step_exp_table);
		tm1 = P1xI2;
		P1xI2 *= tm.remainder;
	}

	if(!remainder.subseteq(intZero))
	{
		P2xI1 = tmPolyRange * remainder;
	}

	I1xI2 = remainder * tm.remainder;

	result.expansion = P1xP2;

	result.remainder = I1xI2;
	result.remainder += P2xI1;
	result.remainder += P1xI2;

	result.expansion.ctrunc_normal(intTrunc, step_exp_table, order);

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);

	intTrunc += intRound;

	result.remainder += intTrunc;
}

void TaylorModel::mul_insert_ctrunc_normal_no_cutoff(TaylorModel & result, Interval & tm1, Interval & intTrunc, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order) const
{
	Polynomial P1xP2;
	Interval P1xI2, P2xI1, I1xI2;

	P1xP2 = expansion * tm.expansion;

	Interval intZero;
	tm1 = intZero;
	intTrunc = intZero;

	if(!tm.remainder.subseteq(intZero))
	{
		expansion.intEvalNormal(P1xI2, step_exp_table);
		tm1 = P1xI2;
		P1xI2 *= tm.remainder;
	}

	if(!remainder.subseteq(intZero))
	{
		P2xI1 = tmPolyRange * remainder;
	}

	I1xI2 = remainder * tm.remainder;

	result.expansion = P1xP2;

	result.remainder = I1xI2;
	result.remainder += P2xI1;
	result.remainder += P1xI2;

	result.expansion.ctrunc_normal(intTrunc, step_exp_table, order);

	result.remainder += intTrunc;
}

void TaylorModel::mul_insert_assign(const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold)
{
	TaylorModel result;
	mul_insert(result, tm, tmPolyRange, domain, cutoff_threshold);
	*this = result;
}

void TaylorModel::mul_insert_normal_assign(const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold)
{
	TaylorModel result;
	mul_insert_normal(result, tm, tmPolyRange, step_exp_table, cutoff_threshold);
	*this = result;
}

void TaylorModel::mul_insert_ctrunc_assign(const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold)
{
	TaylorModel result;
	mul_insert_ctrunc(result, tm, tmPolyRange, domain, order, cutoff_threshold);
	*this = result;
}

void TaylorModel::mul_insert_ctrunc_normal_assign(const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold)
{
	TaylorModel result;
	mul_insert_ctrunc_normal(result, tm, tmPolyRange, step_exp_table, order, cutoff_threshold);
	*this = result;
}

void TaylorModel::mul_insert_ctrunc_normal_no_cutoff_assign(const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order)
{
	TaylorModel result;
	mul_insert_ctrunc_normal_no_cutoff(result, tm, tmPolyRange, step_exp_table, order);
	*this = result;
}

void TaylorModel::mul_insert_ctrunc_normal_assign(Interval & tm1, Interval & intTrunc, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold)
{
	TaylorModel result;
	mul_insert_ctrunc_normal(result, tm1, intTrunc, tm, tmPolyRange, step_exp_table, order, cutoff_threshold);
	*this = result;
}

void TaylorModel::mul_insert_ctrunc_normal_no_cutoff_assign(Interval & tm1, Interval & intTrunc, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order)
{
	TaylorModel result;
	mul_insert_ctrunc_normal_no_cutoff(result, tm1, intTrunc, tm, tmPolyRange, step_exp_table, order);
	*this = result;
}

void TaylorModel::div(TaylorModel & result, const Interval & I) const
{
	expansion.div(result.expansion, I);
	result.remainder = remainder / I;
}

void TaylorModel::div_assign(const Interval & I)
{
	expansion.div_assign(I);
	remainder /= I;
}

void TaylorModel::derivative(TaylorModel & result, const int varIndex) const
{
	Interval intZero;
	result = *this;

	std::list<Monomial>::iterator iter;

	for(iter = result.expansion.monomials.begin(); iter != result.expansion.monomials.end();)
	{
		if(iter->degrees[varIndex] > 0)
		{
			double tmp = iter->degrees[varIndex];
			iter->degrees[varIndex] -= 1;
			iter->d -= 1;
			iter->coefficient.mul_assign(tmp);
			++iter;
		}
		else
		{
			iter = result.expansion.monomials.erase(iter);
		}
	}

	result.remainder = intZero;
}

void TaylorModel::LieDerivative_no_remainder(TaylorModel & result, const TaylorModelVec & f, const int order, const Interval & cutoff_threshold) const
{
	derivative(result, 0);

	int rangeDim = f.tms.size();

	for(int i=0; i<rangeDim; ++i)
	{
		TaylorModel tmTemp;
		derivative(tmTemp, i+1);
		tmTemp.mul_no_remainder_assign(f.tms[i], order, cutoff_threshold);
		result.add_assign(tmTemp);
	}
}

void TaylorModel::integral(TaylorModel & result, const Interval & I) const
{
	result = *this;

	std::list<Monomial>::iterator iter;

	for(iter = result.expansion.monomials.begin(); iter != result.expansion.monomials.end(); ++iter)
	{
		if(iter->degrees[0] > 0)
		{
			iter->degrees[0] += 1;
			iter->d += 1;
			double tmp = iter->degrees[0];
			iter->coefficient.div_assign(tmp);
		}
		else
		{
			iter->degrees[0] += 1;
			iter->d += 1;
		}
	}

	result.remainder *= I;
}

void TaylorModel::integral_no_remainder(TaylorModel & result) const
{
	result = *this;

	std::list<Monomial>::iterator iter;

	for(iter = result.expansion.monomials.begin(); iter != result.expansion.monomials.end(); ++iter)
	{
		if(iter->degrees[0] > 0)
		{
			iter->degrees[0] += 1;
			iter->d += 1;
			double tmp = iter->degrees[0];
			iter->coefficient.div_assign(tmp);
		}
		else
		{
			iter->degrees[0] += 1;
			iter->d += 1;
		}
	}
}

void TaylorModel::linearCoefficients(std::vector<Interval> & result) const
{
	expansion.linearCoefficients(result);
}

void TaylorModel::linearCoefficients(iMatrix & coefficients, const int row) const
{
	expansion.linearCoefficients(coefficients, row);
}

void TaylorModel::linearCoefficients(iMatrix2 & coefficients, const int row) const
{
	expansion.linearCoefficients(coefficients, row);
}

void TaylorModel::toHornerForm(HornerForm & result, Interval & I) const
{
	expansion.toHornerForm(result);
	I = remainder;
}

void TaylorModel::insert(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold) const
{
	if(vars.tms.size() == 0)
	{
		result = *this;

		std::list<Monomial>::iterator iter;

		for(iter = result.expansion.monomials.begin(); iter != result.expansion.monomials.end(); )
		{
			if( ((iter->d) - (iter->degrees[0])) > 0 )
			{
				iter = result.expansion.monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
	else
	{
		HornerForm hf;
		expansion.toHornerForm(hf);

		hf.insert(result, vars, varsPolyRange, domain, cutoff_threshold);
		result.remainder += remainder;
	}
}

void TaylorModel::insert_normal(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const Interval & cutoff_threshold) const
{
	if(vars.tms.size() == 0)
	{
		result = *this;

		std::list<Monomial>::iterator iter;

		for(iter = result.expansion.monomials.begin(); iter != result.expansion.monomials.end(); )
		{
			if( ((iter->d) - (iter->degrees[0])) > 0 )
			{
				iter = result.expansion.monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
	else
	{
		HornerForm hf;
		expansion.toHornerForm(hf);

		hf.insert_normal(result, vars, varsPolyRange, step_exp_table, numVars, cutoff_threshold);
		result.remainder += remainder;
	}
}

void TaylorModel::insert_ctrunc(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const
{
	if(vars.tms.size() == 0)
	{
		result = *this;

		std::list<Monomial>::iterator iter;

		for(iter = result.expansion.monomials.begin(); iter != result.expansion.monomials.end(); )
		{
			if( ((iter->d) - (iter->degrees[0])) > 0 )
			{
				iter = result.expansion.monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
	else
	{
		HornerForm hf;
		expansion.toHornerForm(hf);

		hf.insert_ctrunc(result, vars, varsPolyRange, domain, order, cutoff_threshold);
		result.remainder += remainder;
	}
}

void TaylorModel::insert_no_remainder(TaylorModel & result, const TaylorModelVec & vars, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	if(vars.tms.size() == 0)
	{
		result = *this;

		std::list<Monomial>::iterator iter;

		for(iter = result.expansion.monomials.begin(); iter != result.expansion.monomials.end();)
		{
			if( ((iter->d) - (iter->degrees[0])) > 0 )
			{
				iter = result.expansion.monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
	else
	{
		HornerForm hf;
		expansion.toHornerForm(hf);

		hf.insert_no_remainder(result, vars, numVars, order, cutoff_threshold);
	}
}

void TaylorModel::insert_no_remainder_no_cutoff(TaylorModel & result, const TaylorModelVec & vars, const int numVars, const int order) const
{
	if(vars.tms.size() == 0)
	{
		result = *this;

		std::list<Monomial>::iterator iter;

		for(iter = result.expansion.monomials.begin(); iter != result.expansion.monomials.end();)
		{
			if( ((iter->d) - (iter->degrees[0])) > 0 )
			{
				iter = result.expansion.monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
	else
	{
		HornerForm hf;
		expansion.toHornerForm(hf);

		hf.insert_no_remainder_no_cutoff(result, vars, numVars, order);
	}
}

void TaylorModel::insert_ctrunc_normal(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	if(vars.tms.size() == 0)
	{
		result = *this;

		std::list<Monomial>::iterator iter;

		for(iter = result.expansion.monomials.begin(); iter != result.expansion.monomials.end(); )
		{
			if( ((iter->d) - (iter->degrees[0])) > 0 )
			{
				iter = result.expansion.monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
	else
	{
		HornerForm hf;
		expansion.toHornerForm(hf);

		hf.insert_ctrunc_normal(result, vars, varsPolyRange, step_exp_table, numVars, order, cutoff_threshold);
		result.remainder += remainder;
	}
}

void TaylorModel::insert_ctrunc_normal_no_cutoff(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const
{
	if(vars.tms.size() == 0)
	{
		result = *this;

		std::list<Monomial>::iterator iter;

		for(iter = result.expansion.monomials.begin(); iter != result.expansion.monomials.end(); )
		{
			if( ((iter->d) - (iter->degrees[0])) > 0 )
			{
				iter = result.expansion.monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
	else
	{
		HornerForm hf;
		expansion.toHornerForm(hf);

		hf.insert_ctrunc_normal_no_cutoff(result, vars, varsPolyRange, step_exp_table, numVars, order);
		result.remainder += remainder;
	}
}

void TaylorModel::evaluate_t(TaylorModel & result, const std::vector<Interval> & step_exp_table) const
{
	result.expansion.clear();
	result.remainder = remainder;

	if(expansion.monomials.size() == 0)
		return;

	std::list<Monomial>::const_iterator iter;
	Interval intZero;

	if(step_exp_table[1].subseteq(intZero))		// t = 0
	{
		for(iter = expansion.monomials.begin(); iter != expansion.monomials.end(); ++iter)
		{
			if(iter->degrees[0] == 0)
			{
				result.expansion.add_assign(*iter);
			}
		}
	}
	else
	{
		for(iter = expansion.monomials.begin(); iter != expansion.monomials.end(); ++iter)
		{
			Monomial monoTemp = *iter;
			int tmp = monoTemp.degrees[0];

			if(tmp > 0)
			{
				monoTemp.coefficient *= step_exp_table[tmp];
				monoTemp.d -= tmp;
				monoTemp.degrees[0] = 0;
			}

			result.expansion.add_assign(monoTemp);
		}
	}
}

void TaylorModel::mul(TaylorModel & result, const int varIndex, const int degree) const
{
	result = *this;
	result.mul_assign(varIndex, degree);
}

void TaylorModel::mul_assign(const int varIndex, const int degree)
{
	expansion.mul_assign(varIndex, degree);
}

void TaylorModel::rmConstant()
{
	expansion.rmConstant();
}

void TaylorModel::decompose(TaylorModel & linear, TaylorModel & other) const
{
	Polynomial polylinear, polyother;
	expansion.decompose(polylinear, polyother);

	linear.expansion = polylinear;
	other.expansion = polyother;
}

void TaylorModel::cutoff_normal(const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold)
{
	Interval I;
	expansion.cutoff_normal(I, step_exp_table, cutoff_threshold);

	remainder += I;
}

void TaylorModel::cutoff(const std::vector<Interval> & domain, const Interval & cutoff_threshold)
{
	Interval I;
	expansion.cutoff(I, domain, cutoff_threshold);

	remainder += I;
}

void TaylorModel::cutoff(const Interval & cutoff_threshold)
{
	expansion.cutoff(cutoff_threshold);
}

int TaylorModel::degree() const
{
	return expansion.degree();
}

bool TaylorModel::isZero() const
{
	Interval intZero;

	if(expansion.isZero())
	{
		if(remainder.subseteq(intZero))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void TaylorModel::center_nc()
{
	expansion.center();
}

void TaylorModel::rmZeroTerms(const std::vector<int> & indices)
{
	expansion.rmZeroTerms(indices);
}

void TaylorModel::normalize(std::vector<Interval> & domain)
{
	int domainDim = domain.size();

	// compute the center of the original domain and make it origin-centered
	std::vector<Interval> intVecCenter;
	for(int i=1; i<domainDim; ++i)		// we omit the time dimension
	{
		Interval M;
		domain[i].remove_midpoint(M);
		intVecCenter.push_back(M);
	}

	// compute the scalars
	Interval intZero;
	std::vector<std::vector<Interval> > coefficients;
	std::vector<Interval> row;

	for(int i=0; i<domainDim; ++i)
	{
		row.push_back(intZero);
	}

	for(int i=0; i<domainDim-1; ++i)
	{
		coefficients.push_back(row);
	}

	for(int i=1; i<domainDim; ++i)
	{
		Interval M;
		domain[i].mag(M);
		coefficients[i-1][i] = M;
	}

	TaylorModelVec newVars(coefficients);
	for(int i=0; i<domainDim-1; ++i)
	{
		TaylorModel tmTemp(intVecCenter[i], domainDim);
		newVars.tms[i].add_assign(tmTemp);
	}

	Interval intUnit(-1,1);
	for(int i=1; i<domainDim; ++i)
	{
		domain[i] = intUnit;
	}

	TaylorModel tmTemp;
	insert_no_remainder_no_cutoff(tmTemp, newVars, domainDim, degree());
	expansion = tmTemp.expansion;
}

void TaylorModel::polyRange(Interval & result, const std::vector<Interval> & domain) const
{
	expansion.intEval(result, domain);
}

void TaylorModel::polyRangeNormal(Interval & result, const std::vector<Interval> & step_exp_table) const
{
	expansion.intEvalNormal(result, step_exp_table);
}

void TaylorModel::exp_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	const_part.exp_assign();	// exp(c)

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmExp(const_part, numVars);
		result = tmExp;

		Interval invalid(1,-1);
		ranges.push_back(invalid);

		return;
	}

	ranges.push_back(const_part);			// keep the unchanged part

	Interval I(1);
	TaylorModel tmOne(I, numVars);

	// to compute the expression 1 + F + (1/2!)F^2 + ... + (1/k!)F^k,
	// we evaluate its Horner form (...((1/(k-1))((1/k)*F+1)*F + 1) ... + 1)

	result = tmOne;
	Interval tmFPolyRange;
	tmF.polyRangeNormal(tmFPolyRange, step_exp_table);

	for(int i=order; i>0; --i)
	{
		Interval intFactor(1);
		intFactor.div_assign((double)i);

		result.mul_assign(intFactor);

		Interval tm1Poly, intTrunc;
		result.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

		ranges.push_back(tm1Poly);			// keep the unchanged part
		ranges.push_back(tmFPolyRange);		// keep the unchanged part
		ranges.push_back(intTrunc);			// keep the unchanged part

		result.add_assign(tmOne);
	}

	result.mul_assign(const_part);

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	ranges.push_back(intRound);				// keep the unchanged part
	result.remainder += intRound;

	Interval rem, tmRange;
	ranges.push_back(tmFPolyRange);			// keep the unchanged part
	tmRange = tmFPolyRange + tmF.remainder;
	exp_taylor_remainder(rem, tmRange, order+1);

	result.remainder += const_part * rem;
}

void TaylorModel::rec_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	const_part.rec_assign();	// 1/c

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmRec(const_part, numVars);
		result = tmRec;

		Interval invalid(1,-1);
		ranges.push_back(invalid);

		return;
	}

	Interval I(1);
	TaylorModel tmOne(I, numVars);
	TaylorModel tmF_c;

	ranges.push_back(const_part);			// keep the unchanged part
	tmF.mul(tmF_c, const_part);

	// to compute the expression 1 - F/c + (F/c)^2 - ... + (-1)^k (F/c)^k,
	// we evaluate its Horner form (-1)*(...((-1)*(-F/c + 1)*F/c + 1)...) + 1

	result = tmOne;
	Interval tmF_cPolyRange;
	tmF_c.polyRangeNormal(tmF_cPolyRange, step_exp_table);

	for(int i=order; i>0; --i)
	{
		result.inv_assign();

		Interval tm1Poly, intTrunc;
		result.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF_c, tmF_cPolyRange, step_exp_table, order, cutoff_threshold);

		ranges.push_back(tm1Poly);			// keep the unchanged part
		ranges.push_back(tmF_cPolyRange);	// keep the unchanged part
		ranges.push_back(intTrunc);			// keep the unchanged part

		result.add_assign(tmOne);
	}

	result.mul_assign(const_part);

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	ranges.push_back(intRound);				// keep the unchanged part
	result.remainder += intRound;

	Interval rem, tmF_cRange;
	ranges.push_back(tmF_cPolyRange);		// keep the unchanged part
	tmF_cRange = tmF_cPolyRange + tmF_c.remainder;

	rec_taylor_remainder(rem, tmF_cRange, order+1);

	result.remainder += rem * const_part;
}

void TaylorModel::sin_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	if(tmF.isZero())			// tm = c
	{
		const_part.sin_assign();
		TaylorModel tmSin(const_part, numVars);
		result = tmSin;

		Interval invalid(1,-1);
		ranges.push_back(invalid);

		return;
	}

	ranges.push_back(const_part);				// keep the unchanged part

	Interval sinc, cosc, msinc, mcosc;
	sinc = const_part.sin();
	cosc = const_part.cos();
	sinc.inv(msinc);
	cosc.inv(mcosc);

	TaylorModel tmTemp(sinc, numVars);
	result = tmTemp;

	Interval tmFPolyRange;
	tmF.polyRangeNormal(tmFPolyRange, step_exp_table);

	int k=1;
	Interval I(1);

	TaylorModel tmPowerTmF(I, numVars);

	for(int i=1; i<=order; ++i, ++k)
	{
		k %= 4;

		I.div_assign((double)i);

		switch(k)
		{
		case 0:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			ranges.push_back(tm1Poly);			// keep the unchanged part
			ranges.push_back(tmFPolyRange);		// keep the unchanged part
			ranges.push_back(intTrunc);			// keep the unchanged part

			tmTemp = tmPowerTmF;

			Interval intTemp = I * sinc;
			ranges.push_back(intTemp);			// keep the unchanged part
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 1:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			ranges.push_back(tm1Poly);			// keep the unchanged part
			ranges.push_back(tmFPolyRange);		// keep the unchanged part
			ranges.push_back(intTrunc);			// keep the unchanged part

			tmTemp = tmPowerTmF;

			Interval intTemp = I * cosc;
			ranges.push_back(intTemp);			// keep the unchanged part
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 2:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			ranges.push_back(tm1Poly);			// keep the unchanged part
			ranges.push_back(tmFPolyRange);		// keep the unchanged part
			ranges.push_back(intTrunc);			// keep the unchanged part

			tmTemp = tmPowerTmF;

			Interval intTemp = I * msinc;
			ranges.push_back(intTemp);			// keep the unchanged part
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 3:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			ranges.push_back(tm1Poly);			// keep the unchanged part
			ranges.push_back(tmFPolyRange);		// keep the unchanged part
			ranges.push_back(intTrunc);			// keep the unchanged part

			tmTemp = tmPowerTmF;

			Interval intTemp = I * mcosc;
			ranges.push_back(intTemp);			// keep the unchanged part
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		}
	}

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	ranges.push_back(intRound);					// keep the unchanged part
	result.remainder += intRound;

	// evaluate the remainder
	Interval tmRange, rem;
	tmRange = tmFPolyRange + tmF.remainder;

	ranges.push_back(tmFPolyRange);				// keep the unchanged part
	sin_taylor_remainder(rem, const_part, tmRange, order+1);

	result.remainder += rem;
}

void TaylorModel::cos_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	if(tmF.isZero())			// tm = c
	{
		const_part.cos_assign();
		TaylorModel tmCos(const_part, numVars);
		result = tmCos;

		Interval invalid(1,-1);
		ranges.push_back(invalid);

		return;
	}

	ranges.push_back(const_part);				// keep the unchanged part

	Interval sinc, cosc, msinc, mcosc;
	sinc = const_part.sin();
	cosc = const_part.cos();
	sinc.inv(msinc);
	cosc.inv(mcosc);

	TaylorModel tmTemp(cosc, numVars);
	result = tmTemp;

	Interval tmFPolyRange;
	tmF.polyRangeNormal(tmFPolyRange, step_exp_table);

	int k=1;
	Interval I(1);

	TaylorModel tmPowerTmF(I, numVars);

	for(int i=1; i<=order; ++i, ++k)
	{
		k %= 4;

		I.div_assign((double)i);

		switch(k)
		{
		case 0:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			ranges.push_back(tm1Poly);			// keep the unchanged part
			ranges.push_back(tmFPolyRange);		// keep the unchanged part
			ranges.push_back(intTrunc);			// keep the unchanged part

			tmTemp = tmPowerTmF;

			Interval intTemp = I * cosc;
			ranges.push_back(intTemp);			// keep the unchanged part
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 1:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			ranges.push_back(tm1Poly);			// keep the unchanged part
			ranges.push_back(tmFPolyRange);		// keep the unchanged part
			ranges.push_back(intTrunc);			// keep the unchanged part

			tmTemp = tmPowerTmF;

			Interval intTemp = I * msinc;
			ranges.push_back(intTemp);			// keep the unchanged part
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 2:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			ranges.push_back(tm1Poly);			// keep the unchanged part
			ranges.push_back(tmFPolyRange);		// keep the unchanged part
			ranges.push_back(intTrunc);			// keep the unchanged part

			tmTemp = tmPowerTmF;

			Interval intTemp = I * mcosc;
			ranges.push_back(intTemp);			// keep the unchanged part
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 3:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			ranges.push_back(tm1Poly);			// keep the unchanged part
			ranges.push_back(tmFPolyRange);		// keep the unchanged part
			ranges.push_back(intTrunc);			// keep the unchanged part

			tmTemp = tmPowerTmF;

			Interval intTemp = I * sinc;
			ranges.push_back(intTemp);			// keep the unchanged part
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		}
	}

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	ranges.push_back(intRound);					// keep the unchanged part
	result.remainder += intRound;

	// evaluate the remainder
	Interval tmRange, rem;
	tmRange = tmFPolyRange + tmF.remainder;

	ranges.push_back(tmFPolyRange);				// keep the unchanged part
	cos_taylor_remainder(rem, const_part, tmRange, order+1);

	result.remainder += rem;
}

void TaylorModel::log_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	Interval C = const_part;
	ranges.push_back(const_part);			// keep the unchanged part

	const_part.log_assign();	// log(c)

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmLog(const_part, numVars);
		result = tmLog;

		Interval invalid(1,-1);
		ranges.push_back(invalid);

		return;
	}

	TaylorModel tmF_c;
	tmF.div(tmF_c, C);

	result = tmF_c;

	Interval I((double)order);
	result.div_assign(I);			// F/c * (1/order)

	Interval tmF_cPolyRange;
	tmF_c.polyRangeNormal(tmF_cPolyRange, step_exp_table);

	for(int i=order; i>=2; --i)
	{
		Interval J(1);
		J.div_assign((double)(i-1));
		TaylorModel tmJ(J, numVars);

		result.sub_assign(tmJ);
		result.inv_assign();

		Interval tm1Poly, intTrunc;
		result.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF_c, tmF_cPolyRange, step_exp_table, order, cutoff_threshold);

		ranges.push_back(tm1Poly);			// keep the unchanged part
		ranges.push_back(tmF_cPolyRange);	// keep the unchanged part
		ranges.push_back(intTrunc);			// keep the unchanged part
	}

	TaylorModel const_part_tm(const_part, numVars);
	result.add_assign(const_part_tm);

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	ranges.push_back(intRound);				// keep the unchanged part
	result.remainder += intRound;

	Interval rem, tmF_cRange;
	ranges.push_back(tmF_cPolyRange);		// keep the unchanged part
	tmF_cRange = tmF_cPolyRange + tmF_c.remainder;

	log_taylor_remainder(rem, tmF_cRange, order+1);

	result.remainder += rem;
}

void TaylorModel::sqrt_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	Interval C = const_part;
	ranges.push_back(const_part);			// keep the unchanged part

	const_part.sqrt_assign();	// sqrt(c)

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmSqrt(const_part, numVars);
		result = tmSqrt;

		Interval invalid(1,-1);
		ranges.push_back(invalid);

		return;
	}

	TaylorModel tmF_2c;
	tmF.div(tmF_2c, C);

	Interval intTwo(2);
	tmF_2c.div_assign(intTwo);	// F/2c

	Interval intOne(1);
	TaylorModel tmOne(intOne, numVars);

	result = tmF_2c;

	Interval K(1), J(1);

	Interval tmF_2cPolyRange;
	tmF_2c.polyRangeNormal(tmF_2cPolyRange, step_exp_table);

	for(int i=order, j=2*order-3; i>=2; --i, j-=2)
	{
		// i
		Interval K((double)i);

		// j = 2*i-3
		Interval J((double)j);

		result.inv_assign();
		result.mul_assign( J / K );

		result.add_assign(tmOne);

		Interval tm1Poly, intTrunc;
		result.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF_2c, tmF_2cPolyRange, step_exp_table, order, cutoff_threshold);

		ranges.push_back(tm1Poly);			// keep the unchanged part
		ranges.push_back(tmF_2cPolyRange);	// keep the unchanged part
		ranges.push_back(intTrunc);			// keep the unchanged part
	}

	result.add_assign(tmOne);

	result.mul_assign(const_part);

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	ranges.push_back(intRound);				// keep the unchanged part
	result.remainder += intRound;

	Interval rem, tmF_cRange = tmF_2cPolyRange;
	tmF_cRange.mul_assign(2.0);

	ranges.push_back(tmF_cRange);			// keep the unchanged part

	Interval tmF_c_remainder = tmF_2c.remainder;
	tmF_c_remainder.mul_assign(2.0);
	tmF_cRange += tmF_c_remainder;

	sqrt_taylor_remainder(rem, tmF_cRange, order+1);

	result.remainder += rem * const_part;
}




void TaylorModel::exp_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	const_part.exp_assign();	// exp(c)

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmExp(const_part, numVars);
		result = tmExp;

		Interval invalid(1,-1);

		return;
	}

	Interval I(1);
	TaylorModel tmOne(I, numVars);

	// to compute the expression 1 + F + (1/2!)F^2 + ... + (1/k!)F^k,
	// we evaluate its Horner form (...((1/(k-1))((1/k)*F+1)*F + 1) ... + 1)

	result = tmOne;
	Interval tmFPolyRange;
	tmF.polyRangeNormal(tmFPolyRange, step_exp_table);

	for(int i=order; i>0; --i)
	{
		Interval intFactor(1);
		intFactor.div_assign((double)i);

		result.mul_assign(intFactor);

		Interval tm1Poly, intTrunc;
		result.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

		result.add_assign(tmOne);
	}

	result.mul_assign(const_part);

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	result.remainder += intRound;

	Interval rem, tmRange;
	tmRange = tmFPolyRange + tmF.remainder;
	exp_taylor_remainder(rem, tmRange, order+1);

	result.remainder += const_part * rem;
}

void TaylorModel::rec_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	const_part.rec_assign();	// 1/c

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmRec(const_part, numVars);
		result = tmRec;

		Interval invalid(1,-1);

		return;
	}

	Interval I(1);
	TaylorModel tmOne(I, numVars);
	TaylorModel tmF_c;

	tmF.mul(tmF_c, const_part);

	// to compute the expression 1 - F/c + (F/c)^2 - ... + (-1)^k (F/c)^k,
	// we evaluate its Horner form (-1)*(...((-1)*(-F/c + 1)*F/c + 1)...) + 1

	result = tmOne;
	Interval tmF_cPolyRange;
	tmF_c.polyRangeNormal(tmF_cPolyRange, step_exp_table);

	for(int i=order; i>0; --i)
	{
		result.inv_assign();

		Interval tm1Poly, intTrunc;
		result.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF_c, tmF_cPolyRange, step_exp_table, order, cutoff_threshold);

		result.add_assign(tmOne);
	}

	result.mul_assign(const_part);

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	result.remainder += intRound;

	Interval rem, tmF_cRange;
	tmF_cRange = tmF_cPolyRange + tmF_c.remainder;

	rec_taylor_remainder(rem, tmF_cRange, order+1);

	result.remainder += rem * const_part;
}

void TaylorModel::sin_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	if(tmF.isZero())			// tm = c
	{
		const_part.sin_assign();
		TaylorModel tmSin(const_part, numVars);
		result = tmSin;

		Interval invalid(1,-1);

		return;
	}

	Interval sinc, cosc, msinc, mcosc;
	sinc = const_part.sin();
	cosc = const_part.cos();
	sinc.inv(msinc);
	cosc.inv(mcosc);

	TaylorModel tmTemp(sinc, numVars);
	result = tmTemp;

	Interval tmFPolyRange;
	tmF.polyRangeNormal(tmFPolyRange, step_exp_table);

	int k=1;
	Interval I(1);

	TaylorModel tmPowerTmF(I, numVars);

	for(int i=1; i<=order; ++i, ++k)
	{
		k %= 4;

		I.div_assign((double)i);

		switch(k)
		{
		case 0:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * sinc;

			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 1:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * cosc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 2:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * msinc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 3:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * mcosc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		}
	}

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	result.remainder += intRound;

	// evaluate the remainder
	Interval tmRange, rem;
	tmRange = tmFPolyRange + tmF.remainder;

	sin_taylor_remainder(rem, const_part, tmRange, order+1);

	result.remainder += rem;
}

void TaylorModel::cos_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	if(tmF.isZero())			// tm = c
	{
		const_part.cos_assign();
		TaylorModel tmCos(const_part, numVars);
		result = tmCos;

		Interval invalid(1,-1);

		return;
	}

	Interval sinc, cosc, msinc, mcosc;
	sinc = const_part.sin();
	cosc = const_part.cos();
	sinc.inv(msinc);
	cosc.inv(mcosc);

	TaylorModel tmTemp(cosc, numVars);
	result = tmTemp;

	Interval tmFPolyRange;
	tmF.polyRangeNormal(tmFPolyRange, step_exp_table);

	int k=1;
	Interval I(1);

	TaylorModel tmPowerTmF(I, numVars);

	for(int i=1; i<=order; ++i, ++k)
	{
		k %= 4;

		I.div_assign((double)i);

		switch(k)
		{
		case 0:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * cosc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 1:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * msinc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 2:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * mcosc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 3:
		{
			Interval tm1Poly, intTrunc;
			tmPowerTmF.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF, tmFPolyRange, step_exp_table, order, cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * sinc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		}
	}

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	result.remainder += intRound;

	// evaluate the remainder
	Interval tmRange, rem;
	tmRange = tmFPolyRange + tmF.remainder;

	cos_taylor_remainder(rem, const_part, tmRange, order+1);

	result.remainder += rem;
}

void TaylorModel::log_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	Interval C = const_part;

	const_part.log_assign();	// log(c)

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmLog(const_part, numVars);
		result = tmLog;

		Interval invalid(1,-1);

		return;
	}

	TaylorModel tmF_c;
	tmF.div(tmF_c, C);

	result = tmF_c;

	Interval I((double)order);
	result.div_assign(I);			// F/c * (1/order)

	Interval tmF_cPolyRange;
	tmF_c.polyRangeNormal(tmF_cPolyRange, step_exp_table);

	for(int i=order; i>=2; --i)
	{
		Interval J(1);
		J.div_assign((double)(i-1));
		TaylorModel tmJ(J, numVars);

		result.sub_assign(tmJ);
		result.inv_assign();

		Interval tm1Poly, intTrunc;
		result.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF_c, tmF_cPolyRange, step_exp_table, order, cutoff_threshold);
	}

	TaylorModel const_part_tm(const_part, numVars);
	result.add_assign(const_part_tm);

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	result.remainder += intRound;

	Interval rem, tmF_cRange;
	tmF_cRange = tmF_cPolyRange + tmF_c.remainder;

	log_taylor_remainder(rem, tmF_cRange, order+1);

	result.remainder += rem;
}

void TaylorModel::sqrt_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	Interval C = const_part;

	const_part.sqrt_assign();	// sqrt(c)

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmSqrt(const_part, numVars);
		result = tmSqrt;

		Interval invalid(1,-1);

		return;
	}

	TaylorModel tmF_2c;
	tmF.div(tmF_2c, C);

	Interval intTwo(2);
	tmF_2c.div_assign(intTwo);	// F/2c

	Interval intOne(1);
	TaylorModel tmOne(intOne, numVars);

	result = tmF_2c;

	Interval K(1), J(1);

	Interval tmF_2cPolyRange;
	tmF_2c.polyRangeNormal(tmF_2cPolyRange, step_exp_table);

	for(int i=order, j=2*order-3; i>=2; --i, j-=2)
	{
		// i
		Interval K((double)i);

		// j = 2*i-3
		Interval J((double)j);

		result.inv_assign();
		result.mul_assign( J / K );

		result.add_assign(tmOne);

		Interval tm1Poly, intTrunc;
		result.mul_insert_ctrunc_normal_assign(tm1Poly, intTrunc, tmF_2c, tmF_2cPolyRange, step_exp_table, order, cutoff_threshold);
	}

	result.add_assign(tmOne);

	result.mul_assign(const_part);

	Interval intRound;
	result.expansion.cutoff_normal(intRound, step_exp_table, cutoff_threshold);
	result.remainder += intRound;

	Interval rem, tmF_cRange = tmF_2cPolyRange;
	tmF_cRange.mul_assign(2.0);

	Interval tmF_c_remainder = tmF_2c.remainder;
	tmF_c_remainder.mul_assign(2.0);
	tmF_cRange += tmF_c_remainder;

	sqrt_taylor_remainder(rem, tmF_cRange, order+1);

	result.remainder += rem * const_part;
}







void TaylorModel::exp_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	int numVars = setting.vars.size();

	const_part.exp_assign();	// exp(c)

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmExp(const_part, numVars);
		result = tmExp;

		return;
	}

	Interval I(1);
	TaylorModel tmOne(I, numVars);

	// to compute the expression 1 + F + (1/2!)F^2 + ... + (1/k!)F^k,
	// we evaluate its Horner form (...((1/(k-1))((1/k)*F+1)*F + 1) ... + 1)

	result = tmOne;
	Interval tmFPolyRange;
	tmF.polyRange(tmFPolyRange, setting.domain);

	for(int i=order; i>0; --i)
	{
		Interval intFactor(1);
		intFactor.div_assign((double)i);

		result.mul_assign(intFactor);

		result.mul_insert_ctrunc_assign(tmF, tmFPolyRange, setting.domain, order, setting.cutoff_threshold);

		result.add_assign(tmOne);
	}

	result.mul_assign(const_part);

	Interval rem, tmRange;
	tmRange = tmFPolyRange + tmF.remainder;
	exp_taylor_remainder_external(rem, tmRange, order+1);

	result.remainder += const_part * rem;
}

void TaylorModel::rec_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	int numVars = setting.vars.size();

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	const_part.rec_assign();	// 1/c

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmRec(const_part, numVars);
		result = tmRec;

		return;
	}

	Interval I(1);
	TaylorModel tmOne(I, numVars);
	TaylorModel tmF_c;

	tmF.mul(tmF_c, const_part);

	// to compute the expression 1 - F/c + (F/c)^2 - ... + (-1)^k (F/c)^k,
	// we evaluate its Horner form (-1)*(...((-1)*(-F/c + 1)*F/c + 1)...) + 1

	result = tmOne;
	Interval tmF_cPolyRange;
	tmF_c.polyRange(tmF_cPolyRange, setting.domain);

	for(int i=order; i>0; --i)
	{
		result.inv_assign();

		result.mul_insert_ctrunc_assign(tmF_c, tmF_cPolyRange, setting.domain, order, setting.cutoff_threshold);

		result.add_assign(tmOne);
	}

	result.mul_assign(const_part);

	Interval rem, tmF_cRange;
	tmF_cRange = tmF_cPolyRange + tmF_c.remainder;

	rec_taylor_remainder_external(rem, tmF_cRange, order+1);

	result.remainder += rem * const_part;
}

void TaylorModel::sin_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	int numVars = setting.vars.size();

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	if(tmF.isZero())			// tm = c
	{
		const_part.sin_assign();
		TaylorModel tmSin(const_part, numVars);
		result = tmSin;

		return;
	}

	Interval sinc, cosc, msinc, mcosc;
	sinc = const_part.sin();
	cosc = const_part.cos();
	sinc.inv(msinc);
	cosc.inv(mcosc);

	TaylorModel tmTemp(sinc, numVars);
	result = tmTemp;

	Interval tmFPolyRange;
	tmF.polyRange(tmFPolyRange, setting.domain);

	int k=1;
	Interval I(1);

	TaylorModel tmPowerTmF(I, numVars);

	for(int i=1; i<=order; ++i, ++k)
	{
		k %= 4;

		I.div_assign((double)i);

		switch(k)
		{
		case 0:
		{
			tmPowerTmF.mul_insert_ctrunc_assign(tmF, tmFPolyRange, setting.domain, order, setting.cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * sinc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 1:
		{
			tmPowerTmF.mul_insert_ctrunc_assign(tmF, tmFPolyRange, setting.domain, order, setting.cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * cosc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 2:
		{
			tmPowerTmF.mul_insert_ctrunc_assign(tmF, tmFPolyRange, setting.domain, order, setting.cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * msinc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 3:
		{
			tmPowerTmF.mul_insert_ctrunc_assign(tmF, tmFPolyRange, setting.domain, order, setting.cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * mcosc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		}
	}


	// evaluate the remainder
	Interval tmRange, rem;
	tmRange = tmFPolyRange + tmF.remainder;

	sin_taylor_remainder_external(rem, const_part, tmRange, order+1);

	result.remainder += rem;
}

void TaylorModel::cos_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	int numVars = setting.vars.size();

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	if(tmF.isZero())			// tm = c
	{
		const_part.cos_assign();
		TaylorModel tmCos(const_part, numVars);
		result = tmCos;

		return;
	}

	Interval sinc, cosc, msinc, mcosc;
	sinc = const_part.sin();
	cosc = const_part.cos();
	sinc.inv(msinc);
	cosc.inv(mcosc);

	TaylorModel tmTemp(cosc, numVars);
	result = tmTemp;

	Interval tmFPolyRange;
	tmF.polyRange(tmFPolyRange, setting.domain);

	int k=1;
	Interval I(1);

	TaylorModel tmPowerTmF(I, numVars);

	for(int i=1; i<=order; ++i, ++k)
	{
		k %= 4;

		I.div_assign((double)i);

		switch(k)
		{
		case 0:
		{
			tmPowerTmF.mul_insert_ctrunc_assign(tmF, tmFPolyRange, setting.domain, order, setting.cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * cosc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 1:
		{
			tmPowerTmF.mul_insert_ctrunc_assign(tmF, tmFPolyRange, setting.domain, order, setting.cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * msinc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 2:
		{
			tmPowerTmF.mul_insert_ctrunc_assign(tmF, tmFPolyRange, setting.domain, order, setting.cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * mcosc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		case 3:
		{
			tmPowerTmF.mul_insert_ctrunc_assign(tmF, tmFPolyRange, setting.domain, order, setting.cutoff_threshold);

			tmTemp = tmPowerTmF;

			Interval intTemp = I * sinc;
			tmTemp.mul_assign(intTemp);

			result.add_assign(tmTemp);

			break;
		}
		}
	}

	// evaluate the remainder
	Interval tmRange, rem;
	tmRange = tmFPolyRange + tmF.remainder;

	cos_taylor_remainder_external(rem, const_part, tmRange, order+1);

	result.remainder += rem;
}

void TaylorModel::log_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	int numVars = setting.vars.size();

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	Interval C = const_part;

	const_part.log_assign();	// log(c)

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmLog(const_part, numVars);
		result = tmLog;

		return;
	}

	TaylorModel tmF_c;
	tmF.div(tmF_c, C);

	result = tmF_c;

	Interval I((double)order);
	result.div_assign(I);			// F/c * (1/order)

	Interval tmF_cPolyRange;
	tmF_c.polyRange(tmF_cPolyRange, setting.domain);

	for(int i=order; i>=2; --i)
	{
		Interval J(1);
		J.div_assign((double)(i-1));
		TaylorModel tmJ(J, numVars);

		result.sub_assign(tmJ);
		result.inv_assign();

		result.mul_insert_ctrunc_assign(tmF_c, tmF_cPolyRange, setting.domain, order, setting.cutoff_threshold);
	}

	TaylorModel const_part_tm(const_part, numVars);
	result.add_assign(const_part_tm);

	Interval rem, tmF_cRange;
	tmF_cRange = tmF_cPolyRange + tmF_c.remainder;

	log_taylor_remainder_external(rem, tmF_cRange, order+1);

	result.remainder += rem;
}

void TaylorModel::sqrt_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const
{
	Interval const_part;

	TaylorModel tmF = *this;

	int numVars = setting.vars.size();

	// remove the center point of tm
	tmF.constant(const_part);
	tmF.rmConstant();			// F = tm - c

	Interval C = const_part;

	const_part.sqrt_assign();	// sqrt(c)

	if(tmF.isZero())			// tm = c
	{
		TaylorModel tmSqrt(const_part, numVars);
		result = tmSqrt;

		return;
	}

	TaylorModel tmF_2c;
	tmF.div(tmF_2c, C);

	Interval intTwo(2);
	tmF_2c.div_assign(intTwo);	// F/2c

	Interval intOne(1);
	TaylorModel tmOne(intOne, numVars);

	result = tmF_2c;

	Interval K(1), J(1);

	Interval tmF_2cPolyRange;
	tmF_2c.polyRange(tmF_2cPolyRange, setting.domain);

	for(int i=order, j=2*order-3; i>=2; --i, j-=2)
	{
		// i
		Interval K((double)i);

		// j = 2*i-3
		Interval J((double)j);

		result.inv_assign();
		result.mul_assign( J / K );

		result.add_assign(tmOne);

		result.mul_insert_ctrunc_assign(tmF_2c, tmF_2cPolyRange, setting.domain, order, setting.cutoff_threshold);
	}

	result.add_assign(tmOne);

	result.mul_assign(const_part);

	Interval rem, tmF_cRange = tmF_2cPolyRange;
	tmF_cRange.mul_assign(2.0);

	Interval tmF_c_remainder = tmF_2c.remainder;
	tmF_c_remainder.mul_assign(2.0);
	tmF_cRange += tmF_c_remainder;

	sqrt_taylor_remainder_external(rem, tmF_cRange, order+1);

	result.remainder += rem * const_part;
}

void TaylorModel::mul(TaylorModel & result, const TaylorModel & tm, const int order, const Taylor_Model_Computation_Setting & setting) const
{
	Interval intZero;
	Polynomial P1xP2;
	Interval P1xI2, P2xI1, I1xI2;

	P1xP2 = expansion * tm.expansion;

	if(!tm.remainder.subseteq(intZero))
	{
		expansion.intEval(P1xI2, setting.domain);
		P1xI2 *= tm.remainder;
	}

	if(!remainder.subseteq(intZero))
	{
		tm.expansion.intEval(P2xI1, setting.domain);
		P2xI1 *= remainder;
	}

	I1xI2 = remainder * tm.remainder;

	result.expansion = P1xP2;

	result.remainder = I1xI2;
	result.remainder += P2xI1;
	result.remainder += P1xI2;

	result.ctrunc(setting.domain, order);

	result.cutoff(setting.domain, setting.cutoff_threshold);
}

void TaylorModel::intEval(Interval & result, const Taylor_Model_Computation_Setting & setting) const
{
	expansion.intEval(result, setting.domain);
	result += remainder;
}

Interval TaylorModel::getRemainder() const
{
	return remainder;
}

void TaylorModel::getExpansion(Polynomial & P) const
{
	P = expansion;
}

void TaylorModel::extend(const int num)
{
	expansion.extend(num);
}

void TaylorModel::substitute(TaylorModel & result, const std::vector<int> & varIDs, const std::vector<Interval> & intVals) const
{
	expansion.substitute(result.expansion, varIDs, intVals);
	result.remainder = remainder;
}

void TaylorModel::substitute_with_precond(const std::vector<bool> & substitution, const std::vector<Interval> & step_exp_table)
{
	Interval I;
	expansion.substitute_with_precond(I, substitution, step_exp_table);
	remainder += I;
}

void TaylorModel::substitute_with_precond_no_remainder(const std::vector<bool> & substitution)
{
	expansion.substitute_with_precond_no_remainder(substitution);
}

TaylorModel & TaylorModel::operator = (const TaylorModel & tm)
{
	if(this == &tm)
		return *this;

	expansion = tm.expansion;
	remainder = tm.remainder;
	return *this;
}











































// class TaylorModelVec

TaylorModelVec::TaylorModelVec()
{
}

TaylorModelVec::TaylorModelVec(const std::vector<TaylorModel> & tms_input):tms(tms_input)
{
}

TaylorModelVec::TaylorModelVec(const Matrix & coefficients)
{
	int cols = coefficients.cols();
	RowVector rowVec(cols);

	int rows = coefficients.rows();

	for(int i=0; i<rows; ++i)
	{
		coefficients.row(rowVec, i);
		TaylorModel tmTemp(rowVec);
		tms.push_back(tmTemp);
	}
}

TaylorModelVec::TaylorModelVec(iMatrix & coefficients)
{
	int cols = coefficients.cols();
	int rows = coefficients.rows();

	for(int i=0; i<rows; ++i)
	{
		TaylorModel tmTemp(coefficients[i], cols);
		tms.push_back(tmTemp);
	}
}

TaylorModelVec::TaylorModelVec(const std::vector<Interval> & coefficients)
{
	int numVars = coefficients.size() + 1;
	for(int i=0; i<coefficients.size(); ++i)
	{
		TaylorModel tmTemp(coefficients[i], numVars);
		tmTemp.expansion.mul_assign(i+1, 1);
		tms.push_back(tmTemp);
	}
}

TaylorModelVec::TaylorModelVec(const Matrix & coefficients, const std::vector<Interval> & remainders)
{
	int cols = coefficients.cols();
	RowVector rowVec(cols);

	int rows = coefficients.rows();

	for(int i=0; i<rows; ++i)
	{
		coefficients.row(rowVec, i);
		TaylorModel tmTemp(rowVec, remainders[i]);
		tms.push_back(tmTemp);
	}
}

TaylorModelVec::TaylorModelVec(const std::vector<Interval> & constants, const int numVars)
{
	for(int i=0; i<constants.size(); ++i)
	{
		TaylorModel tmTemp(constants[i], numVars);
		tms.push_back(tmTemp);
	}
}

TaylorModelVec::TaylorModelVec(const std::vector<std::vector<Interval> > & coefficients)
{
	for(int i=0; i<coefficients.size(); ++i)
	{
		TaylorModel tmTemp(coefficients[i]);
		tms.push_back(tmTemp);
	}
}

TaylorModelVec::TaylorModelVec(const std::vector<std::vector<Interval> > & coefficients, const std::vector<Interval> & remainders)
{
	for(int i=0; i<coefficients.size(); ++i)
	{
		TaylorModel tmTemp(coefficients[i], remainders[i]);
		tms.push_back(tmTemp);
	}
}

TaylorModelVec::TaylorModelVec(const std::vector<Interval> & intVec, std::vector<Interval> & domain)
{
	int rangeDim = intVec.size();
	domain = intVec;
	Interval intZero;
	domain.insert(domain.begin(), intZero);

	std::vector<Interval> intVecCenter;
	for(int i=0; i<rangeDim; ++i)		// we omit the time dimension
	{
		double center = intVec[i].midpoint();
		Interval intTemp(center);
		intVecCenter.push_back(intTemp);
		domain[i+1].sub_assign(center);
	}

	// compute the scalars
	Matrix coefficients(rangeDim, rangeDim+1);
	for(int i=0; i<rangeDim; ++i)
	{
		coefficients.set( domain[i+1].sup() , i, i+1);
	}

	TaylorModelVec tmvTemp(coefficients);
	tms = tmvTemp.tms;

	for(int i=0; i<rangeDim; ++i)
	{
		TaylorModel tmTemp(intVecCenter[i], rangeDim+1);
		tms[i].add_assign(tmTemp);
	}

	Interval intUnit(-1,1);
	for(int i=0; i<rangeDim; ++i)
	{
		domain[i+1] = intUnit;
	}
}
/*
TaylorModelVec::TaylorModelVec(Polynomial_matrix & polynomials)
{
	for(int i=0; i<polynomials.rows(); ++i)
	{
		TaylorModel tm(polynomials[i][0]);
		tms.push_back(tm);
	}
}

TaylorModelVec::TaylorModelVec(Polynomial_matrix & polynomials, Interval_matrix & remainders)
{
	for(int i=0; i<polynomials.rows(); ++i)
	{
		TaylorModel tm(polynomials[i][0], remainders[i][0]);
		tms.push_back(tm);
	}
}
*/

TaylorModelVec::TaylorModelVec(const TaylorModelVec & tmv):tms(tmv.tms)
{
}

TaylorModelVec::~TaylorModelVec()
{
	tms.clear();
}

void TaylorModelVec::clear()
{
	tms.clear();
}

void TaylorModelVec::dump_interval(FILE *fp, const std::vector<std::string> & stateVarNames, const std::vector<std::string> & tmVarNames) const
{
	for(int i=0; i<tms.size(); ++i)
	{
		fprintf(fp, "%s = ", stateVarNames[i].c_str());
		tms[i].dump_interval(fp, tmVarNames);
		fprintf(fp, "\n");
	}

	fprintf(fp, "\n");
}

void TaylorModelVec::dump_constant(FILE *fp, const std::vector<std::string> & stateVarNames, const std::vector<std::string> & tmVarNames) const
{
	for(int i=0; i<tms.size(); ++i)
	{
		fprintf(fp, "%s = ", stateVarNames[i].c_str());
		tms[i].dump_constant(fp, tmVarNames);
		fprintf(fp, "\n");
	}

	fprintf(fp, "\n");
}

void TaylorModelVec::constant(std::vector<Interval> & result) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		Interval I;
		tms[i].constant(I);
		result.push_back(I);
	}
}
/*
void TaylorModelVec::constant(iMatrix & result) const
{
	std::vector<Interval> constant_part;
	constant(constant_part);
	result.set(constant_part);
}
*/
void TaylorModelVec::intEval(std::vector<Interval> & result, const std::vector<Interval> & domain) const
{
	result.clear();
	for(int i=0; i<tms.size(); ++i)
	{
		Interval I;
		tms[i].intEval(I, domain);
		result.push_back(I);
	}
}

void TaylorModelVec::intEval(std::vector<Interval> & result, const std::vector<Interval> & domain, const std::vector<int> & varIDs) const
{
	result.clear();
	for(int i=0; i<varIDs.size(); ++i)
	{
		Interval I;
		tms[varIDs[i]].intEval(I, domain);
		result.push_back(I);
	}
}

void TaylorModelVec::intEvalNormal(std::vector<Interval> & result, const std::vector<Interval> & step_exp_table) const
{
	result.clear();
	for(int i=0; i<tms.size(); ++i)
	{
		Interval I;
		tms[i].intEvalNormal(I, step_exp_table);
		result.push_back(I);
	}
}

void TaylorModelVec::intEvalNormal(std::vector<Interval> & result, const std::vector<Interval> & step_exp_table, const std::vector<int> & varIDs) const
{
	result.clear();
	for(int i=0; i<varIDs.size(); ++i)
	{
		Interval I;
		tms[varIDs[i]].intEvalNormal(I, step_exp_table);
		result.push_back(I);
	}
}


void TaylorModelVec::ctrunc(const std::vector<Interval> & domain, const int order)
{
	for(int i=0; i<tms.size(); ++i)
		tms[i].ctrunc(domain, order);
}

void TaylorModelVec::nctrunc(const int order)
{
	for(int i=0; i<tms.size(); ++i)
		tms[i].nctrunc(order);
}

void TaylorModelVec::ctrunc_normal(const std::vector<Interval> & step_exp_table, const int order)
{
	for(int i=0; i<tms.size(); ++i)
		tms[i].ctrunc_normal(step_exp_table, order);
}

void TaylorModelVec::ctrunc(const std::vector<Interval> & domain, const std::vector<int> & orders)
{
	for(int i=0; i<tms.size(); ++i)
		tms[i].ctrunc(domain, orders[i]);
}

void TaylorModelVec::nctrunc(const std::vector<int> & orders)
{
	for(int i=0; i<tms.size(); ++i)
		tms[i].nctrunc(orders[i]);
}

void TaylorModelVec::ctrunc_normal(const std::vector<Interval> & step_exp_table, const std::vector<int> & orders)
{
	for(int i=0; i<tms.size(); ++i)
		tms[i].ctrunc_normal(step_exp_table, orders[i]);
}

void TaylorModelVec::inv(TaylorModelVec & result) const
{
	result.clear();
	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].inv(tmTemp);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::add(TaylorModelVec & result, const TaylorModelVec & tmv) const
{
	result.clear();

	if(tms.size() != tmv.tms.size())
	{
		printf("Dimensions do not coincide.\n");
		return;
	}

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].add(tmTemp, tmv.tms[i]);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::sub(TaylorModelVec & result, const TaylorModelVec & tmv) const
{
	result.clear();

	if(tms.size() != tmv.tms.size())
	{
		printf("Dimensions do not coincide.\n");
		return;
	}

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].sub(tmTemp, tmv.tms[i]);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::add_assign(const TaylorModelVec & tmv)
{
	TaylorModelVec result;
	add(result, tmv);
	*this = result;
}

void TaylorModelVec::sub_assign(const TaylorModelVec & tmv)
{
	TaylorModelVec result;
	sub(result, tmv);
	*this = result;
}

void TaylorModelVec::mul(TaylorModelVec & result, const Interval & I) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].mul(tmTemp, I);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::mul_assign(const Interval & I)
{
	TaylorModelVec result;
	mul(result, I);
	*this = result;
}

void TaylorModelVec::div(TaylorModelVec & result, const Interval & I) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].div(tmTemp, I);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::div_assign(const Interval & I)
{
	TaylorModelVec result;
	div(result, I);
	*this = result;
}

void TaylorModelVec::derivative(TaylorModelVec & result, const int varIndex) const
{
	result.clear();
	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].derivative(tmTemp, varIndex);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::LieDerivative_no_remainder(TaylorModelVec & result, const TaylorModelVec & f, const int order, const Interval & cutoff_threshold) const
{
	result.clear();
	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].LieDerivative_no_remainder(tmTemp, f, order, cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::LieDerivative_no_remainder(TaylorModelVec & result, const TaylorModelVec & f, const std::vector<int> & orders, const Interval & cutoff_threshold) const
{
	result.clear();
	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].LieDerivative_no_remainder(tmTemp, f, orders[i], cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::integral(TaylorModelVec & result, const Interval & I) const
{
	result.clear();
	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].integral(tmTemp, I);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::integral_no_remainder(TaylorModelVec & result) const
{
	result.clear();
	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].integral_no_remainder(tmTemp);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::linearCoefficients(std::vector<std::vector<Interval> > & result) const
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].linearCoefficients(result[i]);
	}
}

void TaylorModelVec::linearCoefficients(iMatrix & coefficients) const
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].linearCoefficients(coefficients, i);
	}
}

void TaylorModelVec::linearCoefficients(iMatrix2 & coefficients) const
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].linearCoefficients(coefficients, i);
	}
}

void TaylorModelVec::rmZeroTerms(const std::vector<int> & indices)
{
	if(indices.size() == 0)
	{
		return;
	}

	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].rmZeroTerms(indices);
	}
}

void TaylorModelVec::insert(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].insert(tmTemp, vars, varsPolyRange, domain, cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::insert_normal(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const Interval & cutoff_threshold) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].insert_normal(tmTemp, vars, varsPolyRange, step_exp_table, numVars, cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::insert_ctrunc(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].insert_ctrunc(tmTemp, vars, varsPolyRange, domain, order, cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::insert_no_remainder(TaylorModelVec & result, const TaylorModelVec & vars, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].insert_no_remainder(tmTemp, vars, numVars, order, cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::insert_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].insert_ctrunc_normal(tmTemp, vars, varsPolyRange, step_exp_table, numVars, order, cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::insert_ctrunc_normal_no_cutoff(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].insert_ctrunc_normal_no_cutoff(tmTemp, vars, varsPolyRange, step_exp_table, numVars, order);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::insert_ctrunc(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const std::vector<int> & orders, const Interval & cutoff_threshold) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].insert_ctrunc(tmTemp, vars, varsPolyRange, domain, orders[i], cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::insert_no_remainder(TaylorModelVec & result, const TaylorModelVec & vars, const int numVars, const std::vector<int> & orders, const Interval & cutoff_threshold) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].insert_no_remainder(tmTemp, vars, numVars, orders[i], cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::insert_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const std::vector<int> & orders, const Interval & cutoff_threshold) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].insert_ctrunc_normal(tmTemp, vars, varsPolyRange, step_exp_table, numVars, orders[i], cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::insert_no_remainder_no_cutoff(TaylorModelVec & result, const TaylorModelVec & vars, const int numVars, const int order) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].insert_no_remainder_no_cutoff(tmTemp, vars, numVars, order);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::evaluate_t(TaylorModelVec & result, const std::vector<Interval> & step_exp_table) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].evaluate_t(tmTemp, step_exp_table);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::mul(TaylorModelVec & result, const int varIndex, const int degree) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].mul(tmTemp, varIndex, degree);
		result.tms.push_back(tmTemp);
	}
}

void TaylorModelVec::mul_assign(const int varIndex, const int degree)
{
	for(int i=0; i<tms.size(); ++i)
		tms[i].mul_assign(varIndex, degree);
}

void TaylorModelVec::linearTrans(TaylorModelVec & result, const Matrix & A) const
{
	result.clear();
	if(tms.size() != A.cols())
	{
		printf("Dimensions do not coincide.\n");
		return;
	}

	int rows = A.rows();
	for(int i=0; i<rows; ++i)
	{
		TaylorModel tm1;

		for(int j=0; j<A.cols(); ++j)
		{
			TaylorModel tm2;
			Interval I( A.get(i,j) );
			tms[j].mul(tm2, I);
			tm1.add_assign(tm2);
		}

		result.tms.push_back(tm1);
	}
}

void TaylorModelVec::linearTrans(TaylorModelVec & result, rMatrix & A) const
{
	result.clear();
	if(tms.size() != A.cols())
	{
		printf("Dimensions do not coincide.\n");
		return;
	}

	int rows = A.rows();
	for(int i=0; i<rows; ++i)
	{
		TaylorModel tm1;

		for(int j=0; j<A.cols(); ++j)
		{
			TaylorModel tm2;
			tms[j].mul(tm2, A[i][j]);
			tm1.add_assign(tm2);
		}

		result.tms.push_back(tm1);
	}
}

void TaylorModelVec::linearTrans_assign(const Matrix & A)
{
	TaylorModelVec result;
	linearTrans(result, A);
	*this = result;
}

void TaylorModelVec::linearTrans_assign(rMatrix & A)
{
	TaylorModelVec result;
	linearTrans(result, A);
	*this = result;
}

void TaylorModelVec::scale(TaylorModelVec & result, const std::vector<Interval> & S)
{
	result = *this;
	result.scale_assign(S);
}

void TaylorModelVec::scale_assign(const std::vector<Interval> & S)
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].mul_assign(S[i]);
	}
}

void TaylorModelVec::rmConstant()
{
	for(int i=0; i<tms.size(); ++i)
		tms[i].rmConstant();
}

void TaylorModelVec::decompose(TaylorModelVec & linear, TaylorModelVec & other) const
{
	linear.tms.clear();
	other.tms.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tm_linear, tm_other;
		tms[i].decompose(tm_linear, tm_other);

		linear.tms.push_back(tm_linear);
		other.tms.push_back(tm_other);
	}
}

void TaylorModelVec::cutoff_normal(const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold)
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].cutoff_normal(step_exp_table, cutoff_threshold);
	}
}

void TaylorModelVec::cutoff(const std::vector<Interval> & domain, const Interval & cutoff_threshold)
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].cutoff(domain, cutoff_threshold);
	}
}


void TaylorModelVec::cutoff(const Interval & cutoff_threshold)
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].cutoff(cutoff_threshold);
	}
}

void TaylorModelVec::cutoff_normal(iMatrix & M, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold)
{
	int d = tms.size();
	iMatrix tmp(d, 1);

	for(int i=0; i<tms.size(); ++i)
	{
		Interval I;
		tms[i].expansion.cutoff_normal(I, step_exp_table, cutoff_threshold);
		tmp[i][0] = I;
		tms[i].remainder += I;
	}

	M = tmp;
}

void TaylorModelVec::center_nc()
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].center_nc();
	}
}

void TaylorModelVec::Expansion(std::vector<Polynomial> & polys) const
{
	polys.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		polys.push_back(tms[i].expansion);
	}
}

void TaylorModelVec::Remainder(iMatrix & rem) const
{
	for(int i=0; i<tms.size(); ++i)
	{
		rem[i][0] = tms[i].remainder;
	}
}

void TaylorModelVec::extend(const int num)
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].extend(num);
	}
}

void TaylorModelVec::get_lti_matrices(iMatrix & A, iMatrix & B) const
{
	Interval intZero;
	int rangeDim = tms.size();
	iMatrix constant(rangeDim, 1), nonconstant(rangeDim, rangeDim);
	A = nonconstant;
	B = constant;

	for(int i=0; i<rangeDim; ++i)
	{
		std::list<Monomial>::const_iterator iter = tms[i].expansion.monomials.begin();

		for(; iter != tms[i].expansion.monomials.end(); ++iter)
		{
			if(iter->d - iter->degrees[0] == 0)
			{
				B[i][0] = iter->coefficient;
			}
			else
			{
				for(int j=1; j<=rangeDim; ++j)
				{
					if(iter->degrees[j] == 1)
					{
						A[i][j-1] = iter->coefficient;
						break;
					}
				}
			}
		}
	}
}

void TaylorModelVec::get_ltv_matrices(upMatrix & A, upMatrix & B) const
{
	Interval intZero;
	int rangeDim = tms.size();
	upMatrix constant(rangeDim, 1), nonconstant(rangeDim, rangeDim);
	A = nonconstant;
	B = constant;

	for(int i=0; i<rangeDim; ++i)
	{
		std::list<Monomial>::const_iterator iter = tms[i].expansion.monomials.begin();

		for(; iter != tms[i].expansion.monomials.end(); ++iter)
		{
			if(iter->d - iter->degrees[0] == 0)
			{
				UnivariatePolynomial upTmp;
				upTmp.coefficients.clear();

				for(int k=0; k<iter->d; ++k)
				{
					upTmp.coefficients.push_back(intZero);
				}

				upTmp.coefficients.push_back(iter->coefficient);

				B[i][0] += upTmp;
			}
			else
			{
				for(int j=1; j<=rangeDim; ++j)
				{
					if(iter->degrees[j] == 1)
					{
						UnivariatePolynomial upTmp;
						upTmp.coefficients.clear();

						for(int k=1; k<iter->d; ++k)
						{
							upTmp.coefficients.push_back(intZero);
						}

						upTmp.coefficients.push_back(iter->coefficient);

						A[i][j-1] += upTmp;

						break;
					}
				}
			}
		}
	}
}

void TaylorModelVec::Picard_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	TaylorModelVec tmvTemp;

	if(order <= 1)
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_no_remainder(tmTemp, *this, numVars, 0, cutoff_threshold);
			tmvTemp.tms.push_back(tmTemp);
		}
	}
	else
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_no_remainder(tmTemp, *this, numVars, order-1, cutoff_threshold);
			tmvTemp.tms.push_back(tmTemp);
		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral_no_remainder(tmvTemp2);

	x0.add(result, tmvTemp2);
}

void TaylorModelVec::Picard_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const std::vector<std::vector<bool> > & substitution, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	TaylorModelVec tmvTemp;
/*
	TaylorModelVec simplified_tms = *this;

	for(int i=0; i<simplified_tms.tms.size(); ++i)
	{
		simplified_tms.tms[i].expansion.simplification_in_decomposition(hybrid_dim);
	}
*/
	if(order <= 1)
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;

			TaylorModelVec sub_tmv = *this;
/*
			for(int j=1; j<substitution[i].size(); ++j)
			{
				if(substitution[i][j] > 0)
				{
					sub_tmv.tms[j-1] = simplified_tms.tms[j-1];
				}
			}
*/

			for(int j=0; j<sub_tmv.tms.size(); ++j)
			{
				sub_tmv.tms[j].expansion.substitute_with_precond_no_remainder(substitution[i]);
			}

			ode[i].insert_no_remainder(tmTemp, sub_tmv, numVars, 0, cutoff_threshold);
			tmvTemp.tms.push_back(tmTemp);
		}
	}
	else
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;

			TaylorModelVec sub_tmv = *this;
/*
			for(int j=1; j<substitution[i].size(); ++j)
			{
				if(substitution[i][j] > 0)
				{
					sub_tmv.tms[j-1] = simplified_tms.tms[j-1];
				}
			}
*/

			for(int j=0; j<sub_tmv.tms.size(); ++j)
			{
				sub_tmv.tms[j].expansion.substitute_with_precond_no_remainder(substitution[i]);
			}

			ode[i].insert_no_remainder(tmTemp, sub_tmv, numVars, order-1, cutoff_threshold);
			tmvTemp.tms.push_back(tmTemp);
		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral_no_remainder(tmvTemp2);

	x0.add(result, tmvTemp2);
}

void TaylorModelVec::Picard_no_remainder_no_cutoff(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const int order) const
{
	TaylorModelVec tmvTemp;

	if(order <= 1)
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_no_remainder_no_cutoff(tmTemp, *this, numVars, 0);
			tmvTemp.tms.push_back(tmTemp);
		}
	}
	else
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_no_remainder_no_cutoff(tmTemp, *this, numVars, order-1);
			tmvTemp.tms.push_back(tmTemp);
		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral_no_remainder(tmvTemp2);

	x0.add(result, tmvTemp2);
}
/*
void TaylorModelVec::Picard_no_remainder_no_cutoff(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const std::vector<TaylorModelVec> & sub_tmvs, const int numVars, const int order) const
{
	TaylorModelVec tmvTemp;

	if(order <= 1)
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_no_remainder_no_cutoff(tmTemp, sub_tmvs[i], numVars, 0);
			tmvTemp.tms.push_back(tmTemp);
		}
	}
	else
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_no_remainder_no_cutoff(tmTemp, sub_tmvs[i], numVars, order-1);
			tmvTemp.tms.push_back(tmTemp);
		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral_no_remainder(tmvTemp2);

	x0.add(result, tmvTemp2);
}
*/
void TaylorModelVec::Picard_no_remainder_assign(const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const int order, const Interval & cutoff_threshold)
{
	TaylorModelVec result;
	Picard_no_remainder(result, x0, ode, numVars, order, cutoff_threshold);
	*this = result;
}

void TaylorModelVec::Picard_no_remainder_assign(const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const std::vector<std::vector<bool> > & substitution, const int numVars, const int order, const Interval & cutoff_threshold)
{
	TaylorModelVec result;
	Picard_no_remainder(result, x0, ode, substitution, numVars, order, cutoff_threshold);
	*this = result;
}

void TaylorModelVec::Picard_no_remainder_no_cutoff_assign(const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const int order)
{
	TaylorModelVec result;
	Picard_no_remainder_no_cutoff(result, x0, ode, numVars, order);
	*this = result;
}
/*
void TaylorModelVec::Picard_no_remainder_no_cutoff_assign(const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const std::vector<TaylorModelVec> & sub_tmv, const int numVars, const int order)
{
	TaylorModelVec result;
	Picard_no_remainder_no_cutoff(result, x0, ode, sub_tmv, numVars, order);
	*this = result;
}
*/
void TaylorModelVec::Picard_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	TaylorModelVec tmvTemp;

	if(order <= 1)
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_ctrunc_normal(tmTemp, *this, polyRange, step_exp_table, numVars, 0, cutoff_threshold);
			tmvTemp.tms.push_back(tmTemp);
		}
	}
	else
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_ctrunc_normal(tmTemp, *this, polyRange, step_exp_table, numVars, order-1, cutoff_threshold);
			tmvTemp.tms.push_back(tmTemp);
		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral(tmvTemp2, step_exp_table[1]);

	x0.add(result, tmvTemp2);
}

void TaylorModelVec::Picard_ctrunc_normal_assign(const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold)
{
	TaylorModelVec result;
	Picard_ctrunc_normal(result, x0, polyRange, ode, step_exp_table, numVars, order, cutoff_threshold);
	*this = result;
}

void TaylorModelVec::Picard_ctrunc_normal(TaylorModelVec & result, std::vector<RangeTree *> & trees, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	TaylorModelVec tmvTemp;

	trees.clear();
	for(int i=0; i<ode.size(); ++i)
	{
		trees.push_back(NULL);
	}
/*
	if(order <= 1)
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_ctrunc_normal(tmTemp, trees[i], *this, polyRange, step_exp_table, numVars, 0, cutoff_threshold);
			tmvTemp.tms.push_back(tmTemp);
		}
	}
	else
	{
*/
		// order must be larger than 1
		for(int i=0; i<ode.size(); ++i)
		{
			if(constant[i])
			{
				TaylorModel tmTemp(ode[i].constant, numVars);
				tmvTemp.tms.push_back(tmTemp);
			}
			else
			{
				TaylorModel tmTemp;
				ode[i].insert_ctrunc_normal(tmTemp, trees[i], *this, polyRange, step_exp_table, numVars, order-1, cutoff_threshold);
				tmvTemp.tms.push_back(tmTemp);
			}
		}
//	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral(tmvTemp2, step_exp_table[1]);

	x0.add(result, tmvTemp2);
}

void TaylorModelVec::Picard_ctrunc_normal(TaylorModelVec & result, std::vector<RangeTree *> & trees, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const std::vector<int> & orders, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	TaylorModelVec tmvTemp;

	trees.clear();
	for(int i=0; i<ode.size(); ++i)
	{
		trees.push_back(NULL);
	}

	for(int i=0; i<ode.size(); ++i)
	{
/*
		if(orders[i] <= 1)
		{
			TaylorModel tmTemp;
			ode[i].insert_ctrunc_normal(tmTemp, trees[i], *this, polyRange, step_exp_table, numVars, 0, cutoff_threshold);
			tmvTemp.tms.push_back(tmTemp);
		}
		else
		{
*/
			if(constant[i])
			{
				TaylorModel tmTemp(ode[i].constant, numVars);
				tmvTemp.tms.push_back(tmTemp);
			}
			else
			{
				TaylorModel tmTemp;
				ode[i].insert_ctrunc_normal(tmTemp, trees[i], *this, polyRange, step_exp_table, numVars, orders[i]-1, cutoff_threshold);
				tmvTemp.tms.push_back(tmTemp);
			}
//		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral(tmvTemp2, step_exp_table[1]);

	x0.add(result, tmvTemp2);
}

void TaylorModelVec::Picard_ctrunc_normal(TaylorModelVec & result, std::vector<RangeTree *> & trees, std::vector<std::vector<Interval> > & trunc_parts, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<std::vector<bool> > & substitution, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	TaylorModelVec tmvTemp;
	Interval intZero;

	trees.clear();
	for(int i=0; i<ode.size(); ++i)
	{
		trees.push_back(NULL);
	}
/*
	TaylorModelVec simplified_tms = *this;
	std::vector<Interval> remainders;

	for(int i=0; i<simplified_tms.tms.size(); ++i)
	{
		Interval I;
		simplified_tms.tms[i].expansion.simplification_in_decomposition(I, hybrid_dim, step_exp_table);
		simplified_tms.tms[i].remainder += I;
		remainders.push_back(I);
	}
*/
	for(int i=0; i<ode.size(); ++i)
	{
		std::vector<Interval> trunc_part;

		if(order <= 1)
		{
			TaylorModel tmTemp;

			TaylorModelVec sub_tmv = *this;
/*
			for(int j=1; j<substitution[i].size(); ++j)
			{
				if(substitution[i][j] > 0)
				{
					sub_tmv.tms[j-1] = simplified_tms.tms[j-1];
					trunc_part.push_back(remainders[j-1]);
				}
				else
				{
					Interval intZero;
					trunc_part.push_back(intZero);
				}
			}
*/




			for(int j=0; j<sub_tmv.tms.size(); ++j)
			{
				Interval I;
				sub_tmv.tms[j].expansion.substitute_with_precond(I, substitution[i], step_exp_table);
				trunc_part.push_back(I);
				sub_tmv.tms[j].remainder += I;
			}

			ode[i].insert_ctrunc_normal(tmTemp, trees[i], sub_tmv, polyRange, step_exp_table, numVars, 0, cutoff_threshold);
			tmvTemp.tms.push_back(tmTemp);
		}
		else
		{
			TaylorModel tmTemp;

			TaylorModelVec sub_tmv = *this;
/*
			for(int j=1; j<substitution[i].size(); ++j)
			{
				if(substitution[i][j] > 0)
				{
					sub_tmv.tms[j-1] = simplified_tms.tms[j-1];
					trunc_part.push_back(remainders[j-1]);
				}
				else
				{
					Interval intZero;
					trunc_part.push_back(intZero);
				}
			}
*/




			for(int j=0; j<sub_tmv.tms.size(); ++j)
			{
				Interval I;
				sub_tmv.tms[j].expansion.substitute_with_precond(I, substitution[i], step_exp_table);
				trunc_part.push_back(I);
				sub_tmv.tms[j].remainder += I;
			}

			ode[i].insert_ctrunc_normal(tmTemp, trees[i], sub_tmv, polyRange, step_exp_table, numVars, order-1, cutoff_threshold);
			tmvTemp.tms.push_back(tmTemp);
		}

		trunc_parts.push_back(trunc_part);
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral(tmvTemp2, step_exp_table[1]);

	x0.add(result, tmvTemp2);
}

void TaylorModelVec::Picard_only_remainder(std::vector<Interval> & result, std::vector<RangeTree *> & trees, std::vector<std::vector<Interval> > & trunc_parts, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const Interval & timeStep) const
{
	result.clear();

	TaylorModelVec tmv = *this;

	for(int i=0; i<ode.size(); ++i)
	{
		for(int j=0; j<trunc_parts[i].size(); ++j)
		{
			tmv.tms[j].remainder += trunc_parts[i][j];
		}

		Interval intTemp;
		ode[i].insert_only_remainder(intTemp, trees[i], tmv, timeStep);
		intTemp *= timeStep;
		result.push_back(intTemp);
	}
}

void TaylorModelVec::Picard_ctrunc_normal_no_cutoff(TaylorModelVec & result, std::vector<RangeTree *> & trees, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const
{
	TaylorModelVec tmvTemp;

	trees.clear();
	for(int i=0; i<ode.size(); ++i)
	{
		trees.push_back(NULL);
	}

	if(order <= 1)
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_ctrunc_normal_no_cutoff(tmTemp, trees[i], *this, polyRange, step_exp_table, numVars, 0);
			tmvTemp.tms.push_back(tmTemp);
		}
	}
	else
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_ctrunc_normal_no_cutoff(tmTemp, trees[i], *this, polyRange, step_exp_table, numVars, order-1);
			tmvTemp.tms.push_back(tmTemp);
		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral(tmvTemp2, step_exp_table[1]);

	x0.add(result, tmvTemp2);
}



/*
void TaylorModelVec::Picard_ctrunc_normal_no_cutoff(TaylorModelVec & result, std::vector<RangeTree *> & trees, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<TaylorModelVec> & sub_tmvs, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const
{
	TaylorModelVec tmvTemp;

	trees.clear();
	for(int i=0; i<ode.size(); ++i)
	{
		trees.push_back(NULL);
	}

	if(order <= 1)
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_ctrunc_normal_no_cutoff(tmTemp, trees[i], sub_tmvs[i], polyRange, step_exp_table, numVars, 0);
			tmvTemp.tms.push_back(tmTemp);
		}
	}
	else
	{
		for(int i=0; i<ode.size(); ++i)
		{
			TaylorModel tmTemp;
			ode[i].insert_ctrunc_normal_no_cutoff(tmTemp, trees[i], sub_tmvs[i], polyRange, step_exp_table, numVars, order-1);
			tmvTemp.tms.push_back(tmTemp);
		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral(tmvTemp2, step_exp_table[1]);

	x0.add(result, tmvTemp2);
}
*/

void TaylorModelVec::Picard_only_remainder(std::vector<Interval> & result, std::vector<RangeTree *> & trees, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const Interval & timeStep, const std::vector<bool> & constant) const
{
	result.clear();

	for(int i=0; i<ode.size(); ++i)
	{
		if(constant[i])
		{
			Interval intZero;
			result.push_back(intZero);
		}
		else
		{
			Interval intTemp;
			ode[i].insert_only_remainder(intTemp, trees[i], *this, timeStep);
			intTemp *= timeStep;
			result.push_back(intTemp);
		}
	}
}

void TaylorModelVec::Picard_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const std::vector<int> & orders, const std::vector<bool> & bIncreased, const Interval & cutoff_threshold) const
{
	result = *this;

	for(int i=0; i<ode.size(); ++i)
	{
		if(bIncreased[i])
		{
			TaylorModel tmTemp;
			if(orders[i] <= 1)
			{
				ode[i].insert_no_remainder(tmTemp, *this, numVars, 0, cutoff_threshold);
			}
			else
			{
				ode[i].insert_no_remainder(tmTemp, *this, numVars, orders[i]-1, cutoff_threshold);
			}

			TaylorModel tmTemp2;
			tmTemp.integral_no_remainder(tmTemp2);
			x0.tms[i].add(result.tms[i], tmTemp2);
		}
	}
}

void TaylorModelVec::Picard_no_remainder_assign(const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const std::vector<int> & orders, const std::vector<bool> & bIncreased, const Interval & cutoff_threshold)
{
	TaylorModelVec result;
	Picard_no_remainder(result, x0, ode, numVars, orders, bIncreased, cutoff_threshold);
	*this = result;
}

void TaylorModelVec::Picard_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const std::vector<int> & orders, const Interval & cutoff_threshold) const
{
	TaylorModelVec tmvTemp;

	for(int i=0; i<ode.size(); ++i)
	{
		TaylorModel tmTemp;
		if(orders[i] <= 1)
		{
			ode[i].insert_ctrunc_normal(tmTemp, *this, polyRange, step_exp_table, numVars, 0, cutoff_threshold);
		}
		else
		{
			ode[i].insert_ctrunc_normal(tmTemp, *this, polyRange, step_exp_table, numVars, orders[i]-1, cutoff_threshold);
		}
		tmvTemp.tms.push_back(tmTemp);
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral(tmvTemp2, step_exp_table[1]);

	x0.add(result, tmvTemp2);
}

void TaylorModelVec::Picard_ctrunc_normal_assign(const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const std::vector<int> & orders, const Interval & cutoff_threshold)
{
	TaylorModelVec result;
	Picard_ctrunc_normal(result, x0, polyRange, ode, step_exp_table, numVars, orders, cutoff_threshold);
	*this = result;
}





// using Taylor approximation
void TaylorModelVec::Picard_non_polynomial_taylor_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const int order, const Interval & cutoff_threshold) const
{
	TaylorModelVec tmvTemp;
	Interval intZero;
	int rangeDim = strOde.size();

	std::string prefix(str_prefix_taylor_polynomial);
	std::string suffix(str_suffix);

	parseSetting.clear();

	parseSetting.cutoff_threshold = cutoff_threshold;
	parseSetting.flowpipe = *this;

	if(order <= 1)
	{
		parseSetting.order = 0;
	}
	else
	{
		parseSetting.order = order-1;
	}

	for(int i=0; i<strOde.size(); ++i)
	{
		parseSetting.strODE = prefix + strOde[i] + suffix;

		parseODE();		// call the parser

		TaylorModel tmTemp(parseResult.expansion, intZero);

		tmvTemp.tms.push_back(tmTemp);
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral_no_remainder(tmvTemp2);

	x0.add(result, tmvTemp2);
}

void TaylorModelVec::Picard_non_polynomial_taylor_no_remainder_assign(const TaylorModelVec & x0, const std::vector<std::string> & strOde, const int order, const Interval & cutoff_threshold)
{
	TaylorModelVec result;
	Picard_non_polynomial_taylor_no_remainder(result, x0, strOde, order, cutoff_threshold);
	*this = result;
}

void TaylorModelVec::Picard_non_polynomial_taylor_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<int> & orders, const std::vector<bool> & bIncreased, const Interval & cutoff_threshold) const
{
	result = *this;

	Interval intZero;
	int rangeDim = strOde.size();

	std::string prefix(str_prefix_taylor_polynomial);
	std::string suffix(str_suffix);

	parseSetting.clear();

	parseSetting.cutoff_threshold = cutoff_threshold;
	parseSetting.flowpipe = *this;

	for(int i=0; i<strOde.size(); ++i)
	{
		if(bIncreased[i])
		{
			if(orders[i] <= 1)
			{
				parseSetting.order = 0;
			}
			else
			{
				parseSetting.order = orders[i]-1;
			}

			parseSetting.strODE = prefix + strOde[i] + suffix;

			parseODE();		// call the parser

			TaylorModel tmTemp(parseResult.expansion, intZero);

			TaylorModel tmTemp2;
			tmTemp.integral_no_remainder(tmTemp2);
			x0.tms[i].add(result.tms[i], tmTemp2);
		}
	}
}

void TaylorModelVec::Picard_non_polynomial_taylor_no_remainder_assign(const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<int> & orders, const std::vector<bool> & bIncreased, const Interval & cutoff_threshold)
{
	TaylorModelVec result;
	Picard_non_polynomial_taylor_no_remainder(result, x0, strOde, orders, bIncreased, cutoff_threshold);
	*this = result;
}

void TaylorModelVec::Picard_non_polynomial_taylor_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	TaylorModelVec tmvTemp;
	Interval intZero;
	int rangeDim = strOde.size();

	std::string prefix(str_prefix_taylor_picard);
	std::string suffix(str_suffix);

	parseSetting.clear();

	parseSetting.flowpipe = *this;
	parseSetting.step_exp_table = step_exp_table;
	parseSetting.cutoff_threshold = cutoff_threshold;
/*
	if(order <= 1)
	{
		parseSetting.order = 0;
	}
	else
	{
		parseSetting.order = order-1;
	}
*/
	parseSetting.order = order - 1;

	for(int i=0; i<strOde.size(); ++i)
	{
		if(constant[i])
		{
			TaylorModel tmTemp(constant_part[i], rangeDim + 1);
			tmvTemp.tms.push_back(tmTemp);
		}
		else
		{
			parseSetting.strODE = prefix + strOde[i] + suffix;
			parseODE();
			TaylorModel tmTemp(parseResult.expansion, parseResult.remainder);

			tmvTemp.tms.push_back(tmTemp);
		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral(tmvTemp2, step_exp_table[1]);

	x0.add(result, tmvTemp2);
}
/*
void TaylorModelVec::Picard_non_polynomial_taylor_ctrunc_normal_assign(const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold)
{
	TaylorModelVec result;
	Picard_non_polynomial_taylor_ctrunc_normal(result, x0, strOde, step_exp_table, order, cutoff_threshold);
	*this = result;
}
*/
void TaylorModelVec::Picard_non_polynomial_taylor_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<Interval> & step_exp_table, const std::vector<int> & orders, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	TaylorModelVec tmvTemp;
	Interval intZero;
	int rangeDim = strOde.size();

	std::string prefix(str_prefix_taylor_picard);
	std::string suffix(str_suffix);

	parseSetting.clear();

	parseSetting.flowpipe = *this;
	parseSetting.step_exp_table = step_exp_table;
	parseSetting.cutoff_threshold = cutoff_threshold;

	for(int i=0; i<strOde.size(); ++i)
	{
/*
		if(orders[i] <= 1)
		{
			parseSetting.order = 0;
		}
		else
		{
			parseSetting.order = orders[i]-1;
		}
*/
		if(constant[i])
		{
			TaylorModel tmTemp(constant_part[i], rangeDim + 1);
			tmvTemp.tms.push_back(tmTemp);
		}
		else
		{
			parseSetting.order = orders[i] - 1;
			parseSetting.strODE = prefix + strOde[i] + suffix;

			parseODE();

			TaylorModel tmTemp(parseResult.expansion, parseResult.remainder);
			tmvTemp.tms.push_back(tmTemp);
		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral(tmvTemp2, step_exp_table[1]);

	x0.add(result, tmvTemp2);
}
/*
void TaylorModelVec::Picard_non_polynomial_taylor_ctrunc_normal_assign(const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<Interval> & step_exp_table, const std::vector<int> & orders, const Interval & cutoff_threshold)
{
	TaylorModelVec result;
	Picard_non_polynomial_taylor_ctrunc_normal(result, x0, strOde, step_exp_table, orders, cutoff_threshold);
	*this = result;
}
*/
void TaylorModelVec::Picard_non_polynomial_taylor_only_remainder(std::vector<Interval> & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const Interval & timeStep, const int order, const std::vector<bool> & constant) const
{
	result.clear();

	std::string prefix(str_prefix_taylor_remainder);
	std::string suffix(str_suffix);

	parseSetting.flowpipe = *this;
	parseSetting.iterRange = parseSetting.ranges.begin();
/*
	if(order <= 1)
	{
		parseSetting.order = 0;
	}
	else
	{
		parseSetting.order = order-1;
	}
*/
	parseSetting.order = order - 1;

	for(int i=0; i<strOde.size(); ++i)
	{
		if(constant[i])
		{
			Interval intZero;
			result.push_back(intZero);
		}
		else
		{
			Interval intTemp;
			parseSetting.strODE = prefix + strOde[i] + suffix;

			parseODE();

			intTemp = parseResult.remainder * timeStep;
			result.push_back(intTemp);
		}
	}
}

void TaylorModelVec::Picard_non_polynomial_taylor_only_remainder(std::vector<Interval> & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const Interval & timeStep, const std::vector<int> & orders, const std::vector<bool> & constant) const
{
	result.clear();

	std::string prefix(str_prefix_taylor_remainder);
	std::string suffix(str_suffix);

	parseSetting.flowpipe = *this;
	parseSetting.iterRange = parseSetting.ranges.begin();

	for(int i=0; i<strOde.size(); ++i)
	{
/*
		if(orders[i] <= 1)
		{
			parseSetting.order = 0;
		}
		else
		{
			parseSetting.order = orders[i]-1;
		}
*/

		if(constant[i])
		{
			Interval intZero;
			result.push_back(intZero);
		}
		else
		{
			parseSetting.order = orders[i] - 1;

			Interval intTemp;

			parseSetting.strODE = prefix + strOde[i] + suffix;
			parseODE();

			intTemp = parseResult.remainder * timeStep;
			result.push_back(intTemp);
		}
	}
}

// functions that use the AST data structure for expressions
void TaylorModelVec::Picard_trunc_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<Expression_AST> & ode, const int numVars, const int order, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	TaylorModelVec tmvTemp;

	for(int i=0; i<ode.size(); ++i)
	{
		if(constant[i])
		{
			TaylorModel tmTemp(constant_part[i], numVars);
			tmvTemp.tms.push_back(tmTemp);
		}
		else
		{
			TaylorModel tmTemp;
			ode[i].evaluate_no_remainder(tmTemp, this->tms, order-1, cutoff_threshold, numVars);
			tmvTemp.tms.push_back(tmTemp);
		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral_no_remainder(tmvTemp2);

	x0.add(result, tmvTemp2);
}

void TaylorModelVec::Picard_trunc_no_remainder_assign(const TaylorModelVec & x0, const std::vector<Expression_AST> & ode, const int numVars, const int order, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part)
{
	TaylorModelVec result;
	Picard_trunc_no_remainder(result, x0, ode, numVars, order, cutoff_threshold, constant, constant_part);
	*this = result;
}

void TaylorModelVec::Picard_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<Expression_AST> & ode, const std::vector<Interval> & step_exp_table, const int order, const int numVars, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part, std::list<Interval> & intermediate_ranges) const
{
	TaylorModelVec tmvTemp;

	for(int i=0; i<ode.size(); ++i)
	{
		if(constant[i])
		{
			TaylorModel tmTemp(constant_part[i], numVars);
			tmvTemp.tms.push_back(tmTemp);
		}
		else
		{
			TaylorModel tmTemp;
			ode[i].evaluate(tmTemp, this->tms, order-1, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);
			tmvTemp.tms.push_back(tmTemp);
		}
	}

	TaylorModelVec tmvTemp2;
	tmvTemp.integral(tmvTemp2, step_exp_table[1]);

	x0.add(result, tmvTemp2);
}

void TaylorModelVec::Picard_ctrunc_normal_remainder(std::vector<Interval> & result, const TaylorModelVec & x0, const std::vector<Expression_AST> & ode, const Interval & timeStep, const int order, const std::vector<bool> & constant, std::list<Interval> & intermediate_ranges) const
{
	std::list<Interval>::iterator iter = intermediate_ranges.begin();

	result.clear();

	for(int i=0; i<ode.size(); ++i)
	{
		if(constant[i])
		{
			Interval intZero;
			result.push_back(intZero);
		}
		else
		{
			Interval intTemp;
			ode[i].evaluate_remainder(intTemp, this->tms, order-1, iter);

			intTemp *= timeStep;
			result.push_back(intTemp);
		}
	}
}




void TaylorModelVec::normalize(std::vector<Interval> & domain)
{
	int domainDim = domain.size();
	int rangeDim = tms.size();

	// compute the center of the original domain and make it origin-centered
	std::vector<Interval> intVecCenter;
	for(int i=1; i<domainDim; ++i)		// we omit the time dimension
	{
		Interval intTemp;
		domain[i].remove_midpoint(intTemp);
		intVecCenter.push_back(intTemp);
	}

	// compute the scalars
	Interval intZero;
	std::vector<std::vector<Interval> > coefficients;
	std::vector<Interval> row;

	for(int i=0; i<domainDim; ++i)
	{
		row.push_back(intZero);
	}

	for(int i=0; i<domainDim-1; ++i)
	{
		coefficients.push_back(row);
	}

	for(int i=1; i<domainDim; ++i)
	{
		Interval M;
		domain[i].mag(M);
		coefficients[i-1][i] = M;
	}

	TaylorModelVec newVars(coefficients);
	for(int i=0; i<domainDim-1; ++i)
	{
		TaylorModel tmTemp(intVecCenter[i], domainDim);
		newVars.tms[i].add_assign(tmTemp);
	}

	Interval intUnit(-1,1);
	for(int i=1; i<domainDim; ++i)
	{
		domain[i] = intUnit;
	}

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tms[i].insert_no_remainder_no_cutoff(tmTemp, newVars, domainDim, tms[i].degree());
		tms[i].expansion = tmTemp.expansion;
	}
}

void TaylorModelVec::polyRange(std::vector<Interval> & result, const std::vector<Interval> & domain) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		Interval intTemp;
		tms[i].polyRange(intTemp, domain);
		result.push_back(intTemp);
	}
}

void TaylorModelVec::polyRangeNormal(std::vector<Interval> & result, const std::vector<Interval> & step_exp_table) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		Interval intTemp;
		tms[i].polyRangeNormal(intTemp, step_exp_table);
		result.push_back(intTemp);
	}
}

void TaylorModelVec::substitute(TaylorModelVec & result, const std::vector<std::vector<int> > & varIDs, const std::vector<std::vector<Interval> > & intVals) const
{
	result.clear();

	for(int i=0; i<tms.size(); ++i)
	{
		TaylorModel tm;
		tms[i].substitute(tm, varIDs[i], intVals[i]);
		result.tms.push_back(tm);
	}
}

void TaylorModelVec::substitute_with_precond(const std::vector<bool> & substitution, const std::vector<Interval> & step_exp_table)
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].substitute_with_precond(substitution, step_exp_table);
	}
}

void TaylorModelVec::substitute_with_precond(const std::vector<std::vector<bool> > & substitution, const std::vector<Interval> & step_exp_table)
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].substitute_with_precond(substitution[i], step_exp_table);
	}
}

void TaylorModelVec::substitute_with_precond_no_remainder(const std::vector<bool> & substitution)
{
	for(int i=0; i<tms.size(); ++i)
	{
		tms[i].substitute_with_precond_no_remainder(substitution);
	}
}

TaylorModelVec & TaylorModelVec::operator = (const TaylorModelVec & tmv)
{
	if(this == &tmv)
		return *this;

	tms = tmv.tms;
	return *this;
}

void TaylorModelVec::get_samples(rMatrix & samples) const
{
	int rangeDim = tms.size();
	int col = 1;

	// samples should be given as a (1 + 2*rangeDim) \times rangeDim real matrix

	// center point
	for(int i=0; i<rangeDim; ++i)
	{
		Real tmp;
		tms[i].constant(tmp);
		samples[i][0] = tmp;
	}

	std::vector<HornerForm> hfs;
	for(int i=0; i<rangeDim; ++i)
	{
		HornerForm hf;
		tms[i].expansion.toHornerForm(hf);
		hfs.push_back(hf);
	}

	Interval intZero, intUp(1), intLo(-1);

	// prepare the domain
	std::vector<Interval> domain;
	domain.push_back(intZero);

	for(int i=0; i<rangeDim; ++i)
	{
		domain.push_back(intZero);
	}

	// center point of every facet of the domain box
	int last_pos = -1;
	for(int i=1; i<=rangeDim; ++i)
	{
		if(last_pos > 0)
		{
			domain[last_pos] = intZero;
		}

		domain[i] = intUp;
		last_pos = i;

		// 0 ... 1 ... 0
		for(int j=0; j<rangeDim; ++j)
		{
			Interval tmp;
			Real sample_j;
			hfs[j].intEval(tmp, domain);
			tmp.sup(sample_j);

			samples[j][col] = sample_j;
		}

		++col;

		// 0 ... -1 ... 0
		domain[i] = intLo;
		for(int j=0; j<rangeDim; ++j)
		{
			Interval tmp;
			Real sample_j;
			hfs[j].intEval(tmp, domain);
			tmp.midpoint(sample_j);

			samples[j][col] = sample_j;
		}

		++col;
	}
}


















ParseSetting::ParseSetting()
{
	iterRange = ranges.begin();
	order = 0;
}

ParseSetting::ParseSetting(const ParseSetting & setting)
{
	strODE				= setting.strODE;
	ranges				= setting.ranges;
	iterRange			= setting.iterRange;
	cutoff_threshold	= setting.cutoff_threshold;
	step_exp_table		= setting.step_exp_table;
	flowpipe			= setting.flowpipe;
	order				= setting.order;
}

ParseSetting::~ParseSetting()
{
	ranges.clear();
	step_exp_table.clear();
}

void ParseSetting::clear()
{
	ranges.clear();
	iterRange = ranges.begin();

	step_exp_table.clear();

	Interval intZero;
	cutoff_threshold = intZero;
}

ParseSetting & ParseSetting::operator = (const ParseSetting & setting)
{
	if(this == &setting)
		return *this;

	strODE				= setting.strODE;
	ranges				= setting.ranges;
	iterRange			= setting.iterRange;
	cutoff_threshold	= setting.cutoff_threshold;
	step_exp_table		= setting.step_exp_table;
	flowpipe			= setting.flowpipe;
	order				= setting.order;

	return *this;
}































ParseResult::ParseResult()
{
}

ParseResult::ParseResult(const ParseResult & result)
{
	expansion		= result.expansion;
	remainder		= result.remainder;
	strExpansion	= result.strExpansion;
	bConstant		= result.bConstant;
	constant		= result.constant;
}

ParseResult::~ParseResult()
{
}

ParseResult & ParseResult::operator = (const ParseResult & result)
{
	if(this == &result)
		return *this;

	expansion		= result.expansion;
	remainder		= result.remainder;
	strExpansion	= result.strExpansion;
	bConstant		= result.bConstant;
	constant		= result.constant;

	return *this;
}






















namespace flowstar
{

void exp_taylor_remainder(Interval & result, const Interval & tmRange, const int order)
{
	Interval intProd = tmRange.pow(order);

	Interval J(0,1);
	J *= tmRange;
	J.exp_assign();

	result = factorial_rec[order] * intProd * J;
}

void rec_taylor_remainder(Interval & result, const Interval & tmRange, const int order)
{
	Interval J(0,1), intOne(1), intMOne(-1);
	J *= tmRange;
	J += intOne;
	J.rec_assign();

	Interval intProd = J;
	intProd *= tmRange;
	intProd *= intMOne;

	result = intProd.pow(order);
	result *= J;
}

void sin_taylor_remainder(Interval & result, const Interval & C, const Interval & tmRange, const int order)
{
	Interval intProd = tmRange.pow(order);

	Interval J(0,1);
	J *= tmRange;
	J += C;

	int k = order % 4;

	switch(k)
	{
	case 0:
		J.sin_assign();
		break;
	case 1:
		J.cos_assign();
		break;
	case 2:
		J.sin_assign();
		J.inv_assign();
		break;
	case 3:
		J.cos_assign();
		J.inv_assign();
		break;
	}

	result = factorial_rec[order] * intProd * J;
}

void cos_taylor_remainder(Interval & result, const Interval & C, const Interval & tmRange, const int order)
{
	Interval intProd = tmRange.pow(order);

	Interval J(0,1);
	J *= tmRange;
	J += C;

	int k = order % 4;

	switch(k)
	{
	case 0:
		J.cos_assign();
		break;
	case 1:
		J.sin_assign();
		J.inv_assign();
		break;
	case 2:
		J.cos_assign();
		J.inv_assign();
		break;
	case 3:
		J.sin_assign();
		break;
	}

	result = factorial_rec[order] * intProd * J;
}

void log_taylor_remainder(Interval & result, const Interval & tmRange, const int order)
{
	Interval J(0,1);
	J *= tmRange;
	J.add_assign(1.0);
	J.rec_assign();

	Interval I = tmRange;
	I *= J;

	result = I.pow(order);

	result.div_assign((double)order);

	if((order+1)%2 == 1)		// order+1 is odd
	{
		result.inv_assign();
	}
}

void sqrt_taylor_remainder(Interval & result, const Interval & tmRange, const int order)
{
	Interval I(0,1);
	I *= tmRange;
	I.add_assign(1.0);
	I.rec_assign();

	Interval intTemp;
	I.sqrt(intTemp);

	I *= tmRange;
	I.div_assign(2.0);

	Interval intProd = I.pow(order-1);

	intProd /= intTemp;
	intProd *= tmRange;
	intProd.div_assign(2.0);

	result = double_factorial[2*order-3] * factorial_rec[order] * intProd;

	if(order % 2 == 0)
	{
		result.inv_assign();
	}
}










void exp_taylor_remainder_external(Interval & result, const Interval & tmRange, const int order)
{
	Interval intProd = tmRange.pow(order);

	Interval J(0,1);
	J *= tmRange;
	J.exp_assign();

	Interval rec_order(1);
	for(int i=2; i<=order; ++i)
	{
		rec_order /= (double)i;
	}

	result = rec_order * intProd * J;
}

void rec_taylor_remainder_external(Interval & result, const Interval & tmRange, const int order)
{
	Interval J(0,1), intOne(1), intMOne(-1);
	J *= tmRange;
	J += intOne;
	J.rec_assign();

	Interval intProd = J;
	intProd *= tmRange;
	intProd *= intMOne;

	result = intProd.pow(order);
	result *= J;
}

void sin_taylor_remainder_external(Interval & result, const Interval & C, const Interval & tmRange, const int order)
{
	Interval intProd = tmRange.pow(order);

	Interval J(0,1);
	J *= tmRange;
	J += C;

	int k = order % 4;

	switch(k)
	{
	case 0:
		J.sin_assign();
		break;
	case 1:
		J.cos_assign();
		break;
	case 2:
		J.sin_assign();
		J.inv_assign();
		break;
	case 3:
		J.cos_assign();
		J.inv_assign();
		break;
	}

	Interval rec_order(1);
	for(int i=2; i<=order; ++i)
	{
		rec_order /= (double)i;
	}

	result = rec_order * intProd * J;
}

void cos_taylor_remainder_external(Interval & result, const Interval & C, const Interval & tmRange, const int order)
{
	Interval intProd = tmRange.pow(order);

	Interval J(0,1);
	J *= tmRange;
	J += C;

	int k = order % 4;

	switch(k)
	{
	case 0:
		J.cos_assign();
		break;
	case 1:
		J.sin_assign();
		J.inv_assign();
		break;
	case 2:
		J.cos_assign();
		J.inv_assign();
		break;
	case 3:
		J.sin_assign();
		break;
	}

	Interval rec_order(1);
	for(int i=2; i<=order; ++i)
	{
		rec_order /= (double)i;
	}

	result = rec_order * intProd * J;
}

void log_taylor_remainder_external(Interval & result, const Interval & tmRange, const int order)
{
	Interval J(0,1);
	J *= tmRange;
	J.add_assign(1.0);
	J.rec_assign();

	Interval I = tmRange;
	I *= J;

	result = I.pow(order);

	result.div_assign((double)order);

	if((order+1)%2 == 1)		// order+1 is odd
	{
		result.inv_assign();
	}
}

void sqrt_taylor_remainder_external(Interval & result, const Interval & tmRange, const int order)
{
	Interval I(0,1);
	I *= tmRange;
	I.add_assign(1.0);
	I.rec_assign();

	Interval intTemp;
	I.sqrt(intTemp);

	I *= tmRange;
	I.div_assign(2.0);

	Interval intProd = I.pow(order-1);

	intProd /= intTemp;
	intProd *= tmRange;
	intProd.div_assign(2.0);

	Interval rec_order(1);
	for(int i=2; i<=order; ++i)
	{
		rec_order /= (double)i;
	}

	Interval double_factorial_factor(1);
	for(int i = 2*order-3; i>0; i-=2)
	{
		double_factorial_factor /= (double)i;
	}

	result = double_factorial_factor * rec_order * intProd;

	if(order % 2 == 0)
	{
		result.inv_assign();
	}
}







void exp_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order)
{
	Interval intZero;
	result = intZero;

	if(!iterRange->valid())
	{
		++iterRange;
		return;
	}

	Interval const_part = *iterRange;
	++iterRange;

	for(int i=order; i>0; --i)
	{
		Interval intFactor(1);
		intFactor.div_assign((double)i);

		result *= intFactor;

		Interval intTemp;
		intTemp = (*iterRange) * remainder;		// P1 x I2
		++iterRange;
		intTemp += (*iterRange) * result;		// P2 x I1
		intTemp += remainder * result;			// I2 x I1
		++iterRange;
		intTemp += (*iterRange);				// truncation
		++iterRange;

		result = intTemp;
	}

	result *= const_part;

	result += (*iterRange);		// cutoff error
	++iterRange;

	Interval tmRange = (*iterRange) + remainder;
	++iterRange;

	Interval rem;
	flowstar::exp_taylor_remainder(rem, tmRange, order+1);
	result += const_part * rem;
}

void rec_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order)
{
	Interval intZero;
	result = intZero;

	if(!iterRange->valid())
	{
		++iterRange;
		return;
	}

	Interval const_part = *iterRange;
	++iterRange;

	Interval tmF_c_remainder = remainder * const_part;

	for(int i=order; i>0; --i)
	{
		result.inv_assign();

		Interval intTemp;
		intTemp = (*iterRange) * tmF_c_remainder;	// P1 x I2
		++iterRange;
		intTemp += (*iterRange) * result;			// P2 x I1
		intTemp += tmF_c_remainder * result;		// I2 x I1
		++iterRange;
		intTemp += (*iterRange);					// truncation
		++iterRange;

		result = intTemp;
	}

	result *= const_part;

	result += (*iterRange);		// cutoff error
	++iterRange;

	Interval rem, tmF_cRange;
	tmF_cRange = (*iterRange) + tmF_c_remainder;
	++iterRange;

	flowstar::rec_taylor_remainder(rem, tmF_cRange, order+1);

	result += rem * const_part;
}

void sin_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order)
{
	Interval intZero;
	result = intZero;

	if(!iterRange->valid())
	{
		++iterRange;
		return;
	}

	Interval const_part = *iterRange;
	++iterRange;

	Interval tmPowerTmF_remainder;

	for(int i=1; i<=order; ++i)
	{
		Interval intTemp;
		intTemp = (*iterRange) * remainder;					// P1 x I2
		++iterRange;
		intTemp += (*iterRange) * tmPowerTmF_remainder;		// P2 x I1
		intTemp += remainder * tmPowerTmF_remainder;		// I2 x I1
		++iterRange;
		intTemp += (*iterRange);							// truncation
		++iterRange;

		tmPowerTmF_remainder = intTemp;

		Interval intTemp2 = tmPowerTmF_remainder;

		intTemp2 *= (*iterRange);
		++iterRange;

		result += intTemp2;
	}

	result += (*iterRange);		// cutoff error
	++iterRange;

	Interval tmRange, rem;
	tmRange = (*iterRange) + remainder;
	++iterRange;

	flowstar::sin_taylor_remainder(rem, const_part, tmRange, order+1);

	result += rem;
}

void cos_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order)
{
	Interval intZero;
	result = intZero;

	if(!iterRange->valid())
	{
		++iterRange;
		return;
	}

	Interval const_part = *iterRange;
	++iterRange;

	int k=1;

	Interval tmPowerTmF_remainder;

	for(int i=1; i<=order; ++i, ++k)
	{
		Interval intTemp;
		intTemp = (*iterRange) * remainder;					// P1 x I2
		++iterRange;
		intTemp += (*iterRange) * tmPowerTmF_remainder;		// P2 x I1
		intTemp += remainder * tmPowerTmF_remainder;		// I2 x I1
		++iterRange;
		intTemp += (*iterRange);							// truncation
		++iterRange;

		tmPowerTmF_remainder = intTemp;

		Interval intTemp2 = tmPowerTmF_remainder;

		intTemp2 *= (*iterRange);
		++iterRange;

		result += intTemp2;
	}

	result += (*iterRange);		// cutoff error
	++iterRange;

	Interval tmRange, rem;
	tmRange = (*iterRange) + remainder;
	++iterRange;

	flowstar::cos_taylor_remainder(rem, const_part, tmRange, order+1);

	result += rem;
}

void log_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order)
{
	Interval intZero;
	result = intZero;

	if(!iterRange->valid())
	{
		++iterRange;
		return;
	}

	Interval C = *iterRange;
	++iterRange;

	Interval const_part = C;

	const_part.log_assign();

	Interval tmF_c_remainder = remainder / C;

	result = tmF_c_remainder;
	result.div_assign((double)order);

	for(int i=order; i>=2; --i)
	{
		result.inv_assign();

		Interval intTemp;
		intTemp = (*iterRange) * tmF_c_remainder;	// P1 x I2
		++iterRange;
		intTemp += (*iterRange) * result;			// P2 x I1
		intTemp += tmF_c_remainder * result;		// I2 x I1
		++iterRange;
		intTemp += (*iterRange);					// truncation
		++iterRange;

		result = intTemp;
	}

	result += (*iterRange);		// cutoff error
	++iterRange;

	Interval rem, tmF_cRange;
	tmF_cRange = (*iterRange) + tmF_c_remainder;
	++iterRange;

	flowstar::log_taylor_remainder(rem, tmF_cRange, order+1);

	result += rem;
}

void sqrt_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order)
{
	Interval intZero;
	result = intZero;

	if(!iterRange->valid())
	{
		++iterRange;
		return;
	}

	Interval C = *iterRange;
	++iterRange;

	Interval const_part = C;
	const_part.sqrt_assign();

	Interval intTwo(2);
	Interval tmF_2c_remainder = (remainder / C) / intTwo;

	result = tmF_2c_remainder;

	Interval K(1), J(1);

	for(int i=order, j=2*order-3; i>=2; --i, j-=2)
	{
		// i
		Interval K((double)i);

		// j = 2*i-3
		Interval J((double)j);

		result.inv_assign();
		result *= J / K;

		Interval intTemp;
		intTemp = (*iterRange) * tmF_2c_remainder;	// P1 x I2
		++iterRange;
		intTemp += (*iterRange) * result;			// P2 x I1
		intTemp += tmF_2c_remainder * result;		// I2 x I1
		++iterRange;
		intTemp += (*iterRange);					// truncation
		++iterRange;

		result = intTemp;
	}

	result *= const_part;

	result += (*iterRange);		// cutoff error
	++iterRange;

	Interval rem, tmF_cRange;
	tmF_cRange = (*iterRange);
	++iterRange;

	tmF_cRange += tmF_2c_remainder * intTwo;

	flowstar::sqrt_taylor_remainder(rem, tmF_cRange, order+1);

	result += rem * const_part;
}

}








