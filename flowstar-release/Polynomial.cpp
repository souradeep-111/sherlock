/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.

  The code is released as is under the GNU General Public License (GPL).
---*/

#include "Polynomial.h"
#include "TaylorModel.h"

using namespace flowstar;

namespace flowstar
{
std::vector<Interval> factorial_rec;
std::vector<Interval> power_4;
std::vector<Interval> double_factorial;
UnivariatePolynomial up_parseresult;
ParsePolynomial parsePolynomial;
}


Variables::Variables()
{
}

Variables::~Variables()
{
	varTab.clear();
	varNames.clear();
}

Variables::Variables(const Variables & variables)
{
	varTab		= variables.varTab;
	varNames	= variables.varNames;
}

Variables & Variables::operator = (const Variables & variables)
{
	if(this == &variables)
		return *this;

	varTab		= variables.varTab;
	varNames	= variables.varNames;

	return *this;
}

bool Variables::declareVar(const std::string & vName)
{
	std::map<std::string,int>::const_iterator iter;

	if((iter = varTab.find(vName)) == varTab.end())
	{
		varTab[vName] = varNames.size();
		varNames.push_back(vName);
		return true;
	}
	else
	{
		return false;
	}
}

int Variables::getIDForVar(const std::string & vName) const
{
	std::map<std::string,int>::const_iterator iter;
	if((iter = varTab.find(vName)) == varTab.end())
	{
		return -1;
	}

	return iter->second;
}

bool Variables::getVarName(std::string & vName, const int id) const
{
	if(id >= 0 && id < varNames.size())
	{
		vName = varNames[id];
		return true;
	}
	else
	{
		return false;
	}
}

int Variables::size() const
{
	return varNames.size();
}

void Variables::clear()
{
	varTab.clear();
	varNames.clear();
}















RangeTree::RangeTree()
{
}

RangeTree::RangeTree(const std::list<Interval> & ranges_input, const std::list<RangeTree *> & children_input)
{
	ranges = ranges_input;
	children = children_input;
}

RangeTree::RangeTree(const RangeTree & tree)
{
	ranges = tree.ranges;
	children = tree.children;
}

RangeTree::~RangeTree()
{
	std::list<RangeTree *>::iterator iter = children.begin();

	for(; iter!=children.end(); ++iter)
	{
		delete *iter;
	}

	ranges.clear();
	children.clear();
}

RangeTree & RangeTree::operator = (const RangeTree & tree)
{
	if(this == &tree)
		return *this;

	ranges = tree.ranges;
	children = tree.children;

	return *this;
}





























// class HornerForm

HornerForm::HornerForm()
{
}

HornerForm::HornerForm(const Interval & I):constant(I)
{
}

HornerForm::HornerForm(const Interval & I, const std::vector<HornerForm> & hfs):constant(I), hornerForms(hfs)
{
}

HornerForm::HornerForm(const HornerForm & hf):constant(hf.constant), hornerForms(hf.hornerForms)
{
}

HornerForm::~HornerForm()
{
	hornerForms.clear();
}

void HornerForm::clear()
{
	constant.set(0,0);
	hornerForms.clear();
}

void HornerForm::intEval(Interval & result, const std::vector<Interval> & domain) const
{
	result = constant;

	for(int i=0; i<hornerForms.size(); ++i)
	{
		Interval intHF;
		hornerForms[i].intEval(intHF, domain);
		intHF *= domain[i];
		result += intHF;
	}
}

void HornerForm::insert(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold) const
{
	Interval intZero;
	int numVars = domain.size();

	result.clear();

	if(!constant.subseteq(intZero))
	{
		TaylorModel tmConstant(constant, numVars);
		result = tmConstant;
	}

	if(hornerForms.size() > 0)						// the first variable is t
	{
		TaylorModel tmTemp;
		hornerForms[0].insert(tmTemp, vars, varsPolyRange, domain, cutoff_threshold);

		tmTemp.expansion.mul_assign(0,1);			// multiplied by t
		tmTemp.remainder *= domain[0];
		result.add_assign(tmTemp);

		for(int i=1; i<hornerForms.size(); ++i)
		{
			hornerForms[i].insert(tmTemp, vars, varsPolyRange, domain, cutoff_threshold);	// recursive call
			tmTemp.mul_insert_assign(vars.tms[i-1], varsPolyRange[i-1], domain, cutoff_threshold);
			result.add_assign(tmTemp);
		}
	}
}

void HornerForm::insert_normal(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const Interval & cutoff_threshold) const
{
	Interval intZero;

	result.clear();

	if(!constant.subseteq(intZero))
	{
		TaylorModel tmConstant(constant, numVars);
		result = tmConstant;
	}

	if(hornerForms.size() > 0)						// the first variable is t
	{
		TaylorModel tmTemp;
		hornerForms[0].insert_normal(tmTemp, vars, varsPolyRange, step_exp_table, numVars, cutoff_threshold);

		tmTemp.expansion.mul_assign(0,1);			// multiplied by t
		tmTemp.remainder *= step_exp_table[1];
		result.add_assign(tmTemp);

		for(int i=1; i<hornerForms.size(); ++i)
		{
			hornerForms[i].insert_normal(tmTemp, vars, varsPolyRange, step_exp_table, numVars, cutoff_threshold);	// recursive call
			tmTemp.mul_insert_normal_assign(vars.tms[i-1], varsPolyRange[i-1], step_exp_table, cutoff_threshold);
			result.add_assign(tmTemp);
		}
	}
}

void HornerForm::insert_ctrunc(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const
{
	Interval intZero;
	int numVars = domain.size();

	result.clear();

	if(!constant.subseteq(intZero))
	{
		TaylorModel tmConstant(constant, numVars);
		result = tmConstant;
	}

	if(hornerForms.size() > 0)						// the first variable is t
	{
		TaylorModel tmTemp;
		hornerForms[0].insert_ctrunc(tmTemp, vars, varsPolyRange, domain, order, cutoff_threshold);

		tmTemp.expansion.mul_assign(0,1);			// multiplied by t
		tmTemp.remainder *= domain[0];

		tmTemp.ctrunc(domain, order);
		result.add_assign(tmTemp);

		for(int i=1; i<hornerForms.size(); ++i)
		{
			hornerForms[i].insert_ctrunc(tmTemp, vars, varsPolyRange, domain, order, cutoff_threshold);	// recursive call
			tmTemp.mul_insert_ctrunc_assign(vars.tms[i-1], varsPolyRange[i-1], domain, order, cutoff_threshold);
			result.add_assign(tmTemp);
		}
	}
}

void HornerForm::insert_no_remainder(TaylorModel & result, const TaylorModelVec & vars, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval intZero;
	result.clear();

	if(!constant.subseteq(intZero))
	{
		TaylorModel tmConstant(constant, numVars);
		result = tmConstant;
	}

	if(hornerForms.size() > 0)						// the first variable is t
	{
		TaylorModel tmTemp;
		hornerForms[0].insert_no_remainder(tmTemp, vars, numVars, order, cutoff_threshold);

		tmTemp.expansion.mul_assign(0,1);			// multiplied by t
		tmTemp.nctrunc(order);
		result.add_assign(tmTemp);

		for(int i=1; i<hornerForms.size(); ++i)
		{
			hornerForms[i].insert_no_remainder(tmTemp, vars, numVars, order, cutoff_threshold);	// recursive call
			tmTemp.mul_no_remainder_assign(vars.tms[i-1], order, cutoff_threshold);
			result.add_assign(tmTemp);
		}
	}
}

void HornerForm::insert_no_remainder_no_cutoff(TaylorModel & result, const TaylorModelVec & vars, const int numVars, const int order) const
{
	Interval intZero;
	result.clear();

	if(!constant.subseteq(intZero))
	{
		TaylorModel tmConstant(constant, numVars);
		result = tmConstant;
	}

	if(hornerForms.size() > 0)						// the first variable is t
	{
		TaylorModel tmTemp;
		hornerForms[0].insert_no_remainder_no_cutoff(tmTemp, vars, numVars, order);

		tmTemp.expansion.mul_assign(0,1);			// multiplied by t
		tmTemp.nctrunc(order);
		result.add_assign(tmTemp);

		for(int i=1; i<hornerForms.size(); ++i)
		{
			hornerForms[i].insert_no_remainder_no_cutoff(tmTemp, vars, numVars, order);	// recursive call
			tmTemp.mul_no_remainder_no_cutoff_assign(vars.tms[i-1], order);
			result.add_assign(tmTemp);
		}
	}
}

void HornerForm::insert_no_remainder_no_cutoff(Polynomial & result, const TaylorModelVec & vars, const int numVars) const
{
	Interval intZero;
	result.clear();

	if(!constant.subseteq(intZero))
	{
		Polynomial polyConstant(constant, numVars);
		result = polyConstant;
	}

	if(hornerForms.size() > 0)						// the first variable is t
	{
		Polynomial polyTemp;
		hornerForms[0].insert_no_remainder_no_cutoff(polyTemp, vars, numVars);

		polyTemp.mul_assign(0,1);			// multiplied by t
		result += polyTemp;

		for(int i=1; i<hornerForms.size(); ++i)
		{
			hornerForms[i].insert_no_remainder_no_cutoff(polyTemp, vars, numVars);	// recursive call
			polyTemp *= vars.tms[i-1].expansion;
			result += polyTemp;
		}
	}
}

void HornerForm::insert_no_remainder_no_cutoff(Polynomial & result, const std::vector<Polynomial> & vars, const int numVars) const
{
	Interval intZero;
	result.clear();

	if(!constant.subseteq(intZero))
	{
		Polynomial polyConstant(constant, numVars);
		result = polyConstant;
	}

	if(hornerForms.size() > 0)						// the first variable is t
	{
		Polynomial polyTemp;
		hornerForms[0].insert_no_remainder_no_cutoff(polyTemp, vars, numVars);

		polyTemp.mul_assign(0,1);			// multiplied by t
		result += polyTemp;

		for(int i=1; i<hornerForms.size(); ++i)
		{
			hornerForms[i].insert_no_remainder_no_cutoff(polyTemp, vars, numVars);	// recursive call
			polyTemp *= vars[i-1];
			result += polyTemp;
		}
	}
}

void HornerForm::insert_ctrunc_normal(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval intZero;
	result.clear();

	if(!constant.subseteq(intZero))
	{
		TaylorModel tmConstant(constant, numVars);
		result = tmConstant;
	}

	if(hornerForms.size() > 0)						// the first variable is t
	{
		TaylorModel tmTemp;
		hornerForms[0].insert_ctrunc_normal(tmTemp, vars, varsPolyRange, step_exp_table, numVars, order, cutoff_threshold);

		tmTemp.expansion.mul_assign(0,1);			// multiplied by t
		tmTemp.remainder *= step_exp_table[1];

		tmTemp.ctrunc_normal(step_exp_table, order);
		result.add_assign(tmTemp);

		for(int i=1; i<hornerForms.size(); ++i)
		{
			hornerForms[i].insert_ctrunc_normal(tmTemp, vars, varsPolyRange, step_exp_table, numVars, order, cutoff_threshold);	// recursive call

			tmTemp.mul_insert_ctrunc_normal_assign(vars.tms[i-1], varsPolyRange[i-1], step_exp_table, order, cutoff_threshold);

			result.add_assign(tmTemp);
		}
	}
}

void HornerForm::insert_ctrunc_normal_no_cutoff(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const
{
	Interval intZero;
	result.clear();

	if(!constant.subseteq(intZero))
	{
		TaylorModel tmConstant(constant, numVars);
		result = tmConstant;
	}

	if(hornerForms.size() > 0)						// the first variable is t
	{
		TaylorModel tmTemp;
		hornerForms[0].insert_ctrunc_normal_no_cutoff(tmTemp, vars, varsPolyRange, step_exp_table, numVars, order);

		tmTemp.expansion.mul_assign(0,1);			// multiplied by t
		tmTemp.remainder *= step_exp_table[1];

		tmTemp.ctrunc_normal(step_exp_table, order);
		result.add_assign(tmTemp);

		for(int i=1; i<hornerForms.size(); ++i)
		{
			hornerForms[i].insert_ctrunc_normal_no_cutoff(tmTemp, vars, varsPolyRange, step_exp_table, numVars, order);	// recursive call

			tmTemp.mul_insert_ctrunc_normal_no_cutoff_assign(vars.tms[i-1], varsPolyRange[i-1], step_exp_table, order);

			result.add_assign(tmTemp);
		}
	}
}

void HornerForm::insert_ctrunc_normal(TaylorModel & result, RangeTree * & tree, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval intZero;
	result.clear();

	if(!constant.subseteq(intZero))
	{
		TaylorModel tmConstant(constant, numVars);
		result = tmConstant;
	}

	RangeTree *pnode = new RangeTree;

	if(hornerForms.size() > 0)						// the first variable is t
	{
		TaylorModel tmTemp;
		RangeTree *child;

		hornerForms[0].insert_ctrunc_normal(tmTemp, child, vars, varsPolyRange, step_exp_table, numVars, order, cutoff_threshold);

		tmTemp.expansion.mul_assign(0,1);			// multiplied by t
		tmTemp.remainder *= step_exp_table[1];

		Interval intTrunc;
		tmTemp.expansion.ctrunc_normal(intTrunc, step_exp_table, order);
		tmTemp.remainder += intTrunc;

		pnode->ranges.push_back(intTrunc);
		pnode->children.push_back(child);

		result.add_assign(tmTemp);

		for(int i=1; i<hornerForms.size(); ++i)
		{
			TaylorModel tmTemp;
			RangeTree *child;

			hornerForms[i].insert_ctrunc_normal(tmTemp, child, vars, varsPolyRange, step_exp_table, numVars, order, cutoff_threshold);	// recursive call

			Interval tm1, intTrunc2;
			tmTemp.mul_insert_ctrunc_normal_assign(tm1, intTrunc2, vars.tms[i-1], varsPolyRange[i-1], step_exp_table, order, cutoff_threshold); 	// here coefficient_range = tm1

			pnode->ranges.push_back(tm1);
			pnode->ranges.push_back(varsPolyRange[i-1]);
			pnode->ranges.push_back(intTrunc2);
			pnode->children.push_back(child);

			result.add_assign(tmTemp);
		}
	}

	tree = pnode;
}

void HornerForm::insert_ctrunc_normal_no_cutoff(TaylorModel & result, RangeTree * & tree, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const
{
	Interval intZero;
	result.clear();

	if(!constant.subseteq(intZero))
	{
		TaylorModel tmConstant(constant, numVars);
		result = tmConstant;
	}

	RangeTree *pnode = new RangeTree;

	if(hornerForms.size() > 0)						// the first variable is t
	{
		TaylorModel tmTemp;
		RangeTree *child;

		hornerForms[0].insert_ctrunc_normal_no_cutoff(tmTemp, child, vars, varsPolyRange, step_exp_table, numVars, order);

		tmTemp.expansion.mul_assign(0,1);			// multiplied by t
		tmTemp.remainder *= step_exp_table[1];

		Interval intTrunc;
		tmTemp.expansion.ctrunc_normal(intTrunc, step_exp_table, order);
		tmTemp.remainder += intTrunc;

		pnode->ranges.push_back(intTrunc);
		pnode->children.push_back(child);

		result.add_assign(tmTemp);

		for(int i=1; i<hornerForms.size(); ++i)
		{
			TaylorModel tmTemp;
			RangeTree *child;

			hornerForms[i].insert_ctrunc_normal_no_cutoff(tmTemp, child, vars, varsPolyRange, step_exp_table, numVars, order);	// recursive call

			Interval tm1, intTrunc2;
			tmTemp.mul_insert_ctrunc_normal_no_cutoff_assign(tm1, intTrunc2, vars.tms[i-1], varsPolyRange[i-1], step_exp_table, order); 	// here coefficient_range = tm1

			pnode->ranges.push_back(tm1);
			pnode->ranges.push_back(varsPolyRange[i-1]);
			pnode->ranges.push_back(intTrunc2);
			pnode->children.push_back(child);

			result.add_assign(tmTemp);
		}
	}

	tree = pnode;
}

void HornerForm::insert_only_remainder(Interval & result, RangeTree *tree, const TaylorModelVec & vars, const Interval & timeStep) const
{
	Interval intZero;

	result = intZero;
	std::list<Interval>::const_iterator iter = tree->ranges.begin();
	std::list<RangeTree *>::const_iterator child = tree->children.begin();

	if(hornerForms.size() > 0)						// the first variable is t
	{
		Interval intTemp;
		hornerForms[0].insert_only_remainder(intTemp, *child, vars, timeStep);
		intTemp *= timeStep;

		intTemp += (*iter);
		result += intTemp;

		++iter;
		++child;

		for(int i=1; i<hornerForms.size(); ++i,++child)
		{
			Interval intTemp2;
			hornerForms[i].insert_only_remainder(intTemp2, *child, vars, timeStep);

			Interval newRemainder = (*iter) * vars.tms[i-1].remainder;
			++iter;
			newRemainder += (*iter) * intTemp2;
			newRemainder += vars.tms[i-1].remainder * intTemp2;
			++iter;
			newRemainder += (*iter);

			result += newRemainder;
			++iter;
		}
	}
}

void HornerForm::dump(FILE *fp, const std::vector<std::string> & varNames) const
{
	int numVars = hornerForms.size();

	Interval intZero;
	bool bPlus = false;

	fprintf(fp, " ( ");
	if(!constant.subseteq(intZero))
	{
		bPlus = true;
		constant.dump(fp);
	}

	if(numVars == 0)
	{
		fprintf(fp, " ) ");
		return;
	}

	for(int i=0; i<numVars; ++i)
	{
		if(hornerForms[i].hornerForms.size() != 0 || !hornerForms[i].constant.subseteq(intZero))
		{
			if(bPlus)		// only used to print the "+" symbol
				fprintf(fp, " + ");
			else
				bPlus = true;

			hornerForms[i].dump(fp, varNames);
			fprintf(fp, "* %s", varNames[i].c_str());
		}
	}

	fprintf(fp, " ) ");
}

HornerForm & HornerForm::operator = (const HornerForm & hf)
{
	if(this == &hf)
		return *this;

	constant = hf.constant;
	hornerForms = hf.hornerForms;
	return *this;
}



































// class Polynomial

Polynomial::Polynomial()
{
}

Polynomial::Polynomial(const Interval & constant, const int numVars)
{
	Interval intZero;

	if(!constant.subseteq(intZero))
	{
		Monomial monomial(constant, numVars);
		monomials.push_back(monomial);
	}
}

Polynomial::Polynomial(const RowVector & coefficients)
{
	int numVars = coefficients.size();

	for(int i=0; i<numVars; ++i)
	{
		double dTemp = coefficients.get(i);
		if(dTemp <= THRESHOLD_LOW && dTemp >= -THRESHOLD_LOW)		// dTemp is zero
			continue;

		Interval intTemp(dTemp);
		Monomial monoTemp(intTemp, numVars);
		monoTemp.degrees[i] = 1;
		monoTemp.d = 1;
		monomials.push_back(monoTemp);
	}

	reorder();
}

Polynomial::Polynomial(const std::vector<Interval> & coefficients)
{
	int numVars = coefficients.size();
	Interval intZero;

	for(int i=0; i<numVars; ++i)
	{
		if(coefficients[i].subseteq(intZero))		// the coefficient is zero
			continue;

		Monomial monoTemp(coefficients[i], numVars);
		monoTemp.degrees[i] = 1;
		monoTemp.d = 1;
		monomials.push_back(monoTemp);
	}

	reorder();
}

Polynomial::Polynomial(const Interval *pcoefficients, const int numVars)
{
	Interval intZero;

	for(int i=0; i<numVars; ++i)
	{
		if((pcoefficients + i)->subseteq(intZero))		// the coefficient is zero
		{
			continue;
		}

		Monomial monoTemp(*(pcoefficients + i), numVars);
		monoTemp.degrees[i] = 1;
		monoTemp.d = 1;
		monomials.push_back(monoTemp);
	}

	reorder();
}

Polynomial::Polynomial(const Monomial & monomial)
{
	monomials.push_back(monomial);
}

Polynomial::Polynomial(const std::list<Monomial> & monos):monomials(monos)
{
	reorder();
}

Polynomial::Polynomial(const int varID, const int degree, const int numVars)
{
	Interval intOne(1);
	Monomial monomial(intOne, numVars);
	monomial.degrees[varID] = degree;
	monomial.d = degree;
	monomials.push_back(monomial);
}

Polynomial::Polynomial(const UnivariatePolynomial & up, const int numVars)
{
	Interval intZero;
	std::vector<int> degrees;
	for(int i=0; i<numVars; ++i)
	{
		degrees.push_back(0);
	}

	for(int i=0; i<up.coefficients.size(); ++i)
	{
		if(!up.coefficients[i].subseteq(intZero))
		{
			Monomial monomial(up.coefficients[i], degrees);
			monomial.degrees[0] = i;
			monomial.d = i;

			monomials.push_back(monomial);
		}
	}
}

Polynomial::Polynomial(const Polynomial & polynomial):monomials(polynomial.monomials)
{
}

Polynomial::~Polynomial()
{
	monomials.clear();
}

Polynomial::Polynomial(const std::string & strPolynomial, const Variables & vars)
{
	parsePolynomial.clear();

	std::string prefix(str_prefix_multivariate_polynomial);
	std::string suffix(str_suffix);

	parsePolynomial.strPolynomial = prefix + strPolynomial + suffix;
	parsePolynomial.variables = vars;

	parseMultivariatePolynomial();

	*this = parsePolynomial.result;
}

void Polynomial::reorder()
{
	monomials.sort();
}

void Polynomial::clear()
{
	monomials.clear();
}

void Polynomial::dump_interval(FILE *fp, const std::vector<std::string> & varNames) const
{
	if(monomials.size() == 0)
	{
		fprintf(fp, "[0,0]");
		return;
	}

	std::list<Monomial>::const_iterator iter, iter_last;
	iter_last = monomials.end();
	--iter_last;

	for(iter = monomials.begin(); iter != iter_last; ++iter)
	{
		iter->dump_interval(fp, varNames);
		fprintf(fp, " + ");
	}

	monomials.back().dump_interval(fp, varNames);
}

void Polynomial::dump_constant(FILE *fp, const std::vector<std::string> & varNames) const
{
	if(monomials.size() == 0)
	{
		fprintf(fp, "[0,0]");
		return;
	}

	std::list<Monomial>::const_iterator iter, iter_last;
	iter_last = monomials.end();
	--iter_last;

	for(iter = monomials.begin(); iter != iter_last; ++iter)
	{
		iter->dump_constant(fp, varNames);
		fprintf(fp, " + ");
	}

	monomials.back().dump_constant(fp, varNames);
}

void Polynomial::constant(Interval & result) const
{
	Interval intZero;

	if(monomials.size() > 0 && (monomials.begin())->d == 0)
	{
		result = (monomials.begin())->coefficient;
	}
	else
	{
		result = intZero;
	}
}

void Polynomial::constant(Real & result) const
{
	Real zero;

	if(monomials.size() > 0 && (monomials.begin())->d == 0)
	{
		(monomials.begin())->coefficient.midpoint(result);
	}
	else
	{
		result = zero;
	}
}

void Polynomial::intEval(Interval & result, const std::vector<Interval> & domain) const
{
	HornerForm hf;
	toHornerForm(hf);
	hf.intEval(result, domain);
}

void Polynomial::intEvalNormal(Interval & result, const std::vector<Interval> & step_exp_table) const
{
	Interval intZero;
	result = intZero;

	std::list<Monomial>::const_iterator iter;

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		Interval intTemp;
		iter->intEvalNormal(intTemp, step_exp_table);

		result += intTemp;
	}
}

void Polynomial::inv(Polynomial & result) const
{
	result = *this;
	result.inv_assign();
}

void Polynomial::inv_assign()
{
	std::list<Monomial>::iterator iter;

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		iter->coefficient.inv_assign();
	}
}

void Polynomial::pow(Polynomial & result, const int degree) const
{
	Polynomial temp = *this;
	result = *this;

	for(int d = degree - 1; d > 0;)
	{
		if(d & 1)
		{
			result *= temp;
		}

		d >>= 1;

		if(d > 0)
		{
			temp *= temp;
		}
	}
}

void Polynomial::pow_assign(const int degree)
{
	Polynomial temp = *this;
	Polynomial result = *this;

	for(int d = degree - 1; d > 0;)
	{
		if(d & 1)
		{
			result *= temp;
		}

		d >>= 1;

		if(d > 0)
		{
			temp *= temp;
		}
	}

	*this = result;
}

void Polynomial::pow(Polynomial & result, const int degree, const int order) const
{
	Polynomial p = *this;
	p.nctrunc(order);

	Polynomial temp = p;
	result = p;

	for(int d = degree - 1; d > 0;)
	{
		if(d & 1)
		{
			result *= temp;
			result.nctrunc(order);
		}

		d >>= 1;

		if(d > 0)
		{
			temp *= temp;
			temp.nctrunc(order);
		}
	}
}

void Polynomial::pow_assign(const int degree, const int order)
{
	Polynomial p = *this;
	p.nctrunc(order);

	Polynomial temp = p;
	Polynomial result = p;

	for(int d = degree - 1; d > 0;)
	{
		if(d & 1)
		{
			result *= temp;
			result.nctrunc(order);
		}

		d >>= 1;

		if(d > 0)
		{
			temp *= temp;
			temp.nctrunc(order);
		}
	}

	*this = result;
}

void Polynomial::center()
{
	std::list<Monomial>::iterator iter;
	for(iter = monomials.begin(); iter != monomials.end(); )
	{
		bool bvalid = iter->center();

		if(!bvalid)
		{
			iter = monomials.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void Polynomial::add_assign(const Monomial & monomial)
{
	bool bAdded = false;

	std::list<Monomial>::iterator iter;

	Interval intZero;

	if(monomial.coefficient.subseteq(intZero))
	{
		return;
	}

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		if(monomial < *iter)
		{
			monomials.insert(iter, monomial);
			bAdded = true;
			break;
		}
		else if(monomial == *iter)
		{
			(*iter) += monomial;

			bAdded = true;
			break;
		}
	}

	if(!bAdded)
	{
		monomials.push_back(monomial);
	}
}

void Polynomial::sub_assign(const Monomial & monomial)
{
	Monomial monoTemp;
	monomial.inv(monoTemp);
	add_assign(monoTemp);
}

void Polynomial::mul_assign(const Monomial & monomial)
{
	Interval intZero;

	if(monomial.coefficient.subseteq(intZero))	// the monomial is zero
	{
		clear();
	}
	else
	{
		std::list<Monomial>::iterator iter;
		for(iter = monomials.begin(); iter != monomials.end(); )
		{
			(*iter) *= monomial;
			++iter;
		}
	}
}

void Polynomial::mul_assign(const Interval & I)
{
	Interval intZero;

	if(I.subseteq(intZero))	// the interval is zero
	{
		clear();
	}
	else
	{
		std::list<Monomial>::iterator iter;
		for(iter = monomials.begin(); iter != monomials.end(); )
		{
			iter->coefficient *= I;
			if(iter->coefficient.subseteq(intZero))
			{
				iter = monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
}

void Polynomial::div_assign(const Interval & I)
{
	std::list<Monomial>::iterator iter;
	for(iter = monomials.begin(); iter != monomials.end(); )
	{
		iter->coefficient /= I;
		++iter;
	}
}

void Polynomial::mul(Polynomial & result, const Interval & I) const
{
	result = *this;
	result.mul_assign(I);
}

void Polynomial::div(Polynomial & result, const Interval & I) const
{
	result = *this;
	result.div_assign(I);
}

void Polynomial::mul_assign(const int varIndex, const int degree)
{
	std::list<Monomial>::iterator iter;

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		iter->degrees[varIndex] += degree;
		iter->d += degree;
	}
}

void Polynomial::mul(Polynomial result, const int varIndex, const int degree) const
{
	result = *this;
	result.mul_assign(varIndex, degree);
}

Polynomial & Polynomial::operator = (const Polynomial & polynomial)
{
	if(this == &polynomial)
		return *this;

	monomials = polynomial.monomials;
	return *this;
}

Polynomial & Polynomial::operator += (const Polynomial & polynomial)
{
	Polynomial result;

	std::list<Monomial>::const_iterator iterA;	// polynomial A
	std::list<Monomial>::const_iterator iterB;	// polynomial B

	for(iterA = monomials.begin(), iterB = polynomial.monomials.begin(); ; )
	{
		if(iterA == monomials.end() || iterB == polynomial.monomials.end())
			break;

		if((*iterA) < (*iterB))
	    {
			result.monomials.push_back(*iterA);
			++iterA;
	    }
		else if((*iterB) < (*iterA))
	    {
			result.monomials.push_back(*iterB);
			++iterB;
	    }
		else
		{
			Interval intTemp;
			intTemp = iterA->coefficient + iterB->coefficient;

			Monomial monoTemp(*iterA);
			monoTemp.coefficient = intTemp;
			result.monomials.push_back(monoTemp);

			++iterA;
			++iterB;
		}
	}

	if(iterA == monomials.end() && iterB != polynomial.monomials.end())
	{
		for(; iterB != polynomial.monomials.end(); ++iterB)
			result.monomials.push_back(*iterB);
	}
	else if(iterA != monomials.end() && iterB == polynomial.monomials.end())
	{
		for(; iterA != monomials.end(); ++iterA)
			result.monomials.push_back(*iterA);
	}

	*this = result;
	return *this;
}

Polynomial & Polynomial::operator -= (const Polynomial & polynomial)
{
	Polynomial polyTemp = polynomial;
	polyTemp.inv_assign();
	*this += polyTemp;

	return *this;
}

Polynomial & Polynomial::operator *= (const Polynomial & polynomial)
{
	Polynomial result;

	if((monomials.size() == 0) || (polynomial.monomials.size() == 0))
	{
		this->clear();
		return *this;
	}

	std::list<Monomial>::const_iterator iterB;	// polynomial B

	for(iterB = polynomial.monomials.begin(); iterB != polynomial.monomials.end(); ++iterB)
	{
		Polynomial polyTemp = *this;
		polyTemp.mul_assign(*iterB);
		result += polyTemp;
	}

	*this = result;
	return *this;
}

Polynomial & Polynomial::operator *= (const Interval & I)
{
	Interval intZero;

	if(I.subseteq(intZero))	// the interval is zero
	{
		clear();
	}
	else
	{
		std::list<Monomial>::iterator iter;
		for(iter = monomials.begin(); iter != monomials.end(); )
		{
			iter->coefficient *= I;
			if(iter->coefficient.subseteq(intZero))
			{
				iter = monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	return *this;
}

Polynomial Polynomial::operator + (const Polynomial & polynomial) const
{
	Polynomial result = *this;
	result += polynomial;
	return result;
}

Polynomial Polynomial::operator - (const Polynomial & polynomial) const
{
	Polynomial result = *this;
	result -= polynomial;
	return result;
}

Polynomial Polynomial::operator * (const Polynomial & polynomial) const
{
	Polynomial result = *this;
	result *= polynomial;
	return result;
}

Polynomial Polynomial::operator * (const Interval & I) const
{
	Polynomial result = *this;
	result *= I;
	return result;
}

void Polynomial::ctrunc(Interval & remainder, const std::vector<Interval> & domain, const int order)
{
	Polynomial polyTemp;
	Monomial monoTemp;

	for(; monomials.size() > 0;)
	{
		monoTemp = monomials.back();

		if(monoTemp.d > order)
		{
			polyTemp.monomials.insert(polyTemp.monomials.begin(), monoTemp);
			monomials.pop_back();
		}
		else
		{
			break;
		}
	}

	polyTemp.intEval(remainder, domain);
}

void Polynomial::nctrunc(const int order)
{
	Monomial monoTemp;

	for(; monomials.size() > 0;)
	{
		monoTemp = monomials.back();

		if(monoTemp.d > order)
		{
			monomials.pop_back();
		}
		else
		{
			break;
		}
	}
}

void Polynomial::ctrunc_normal(Interval & remainder, const std::vector<Interval> & step_exp_table, const int order)
{
	Polynomial polyTemp;
	Monomial monoTemp;

	for(; monomials.size() > 0;)
	{
		monoTemp = monomials.back();

		if(monoTemp.d > order)
		{
			polyTemp.monomials.insert(polyTemp.monomials.begin(), monoTemp);
			monomials.pop_back();
		}
		else
		{
			break;
		}
	}

	polyTemp.intEvalNormal(remainder, step_exp_table);
}

void Polynomial::linearCoefficients(std::vector<Interval> & result) const
{
	// initially, the result should be filled with 0

	std::list<Monomial>::const_iterator iter;

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		int i;

		if(iter->d > 1)
			break;

		if(iter->isLinear(i))
		{
			result[i] = iter->coefficient;
		}
	}
}

void Polynomial::linearCoefficients(RowVector & result) const
{
	// initially, the result should be filled with 0

	std::list<Monomial>::const_iterator iter;

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		int i;

		if(iter->d > 1)
			break;

		if(iter->isLinear(i))
		{
			result.set(iter->coefficient.sup(), i);
		}
	}
}

void Polynomial::linearCoefficients(iMatrix & coefficients, const int row) const
{
	// initially, the result should be filled with 0

	std::list<Monomial>::const_iterator iter;

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		int i;

		if(iter->d > 1)
			break;

		if(iter->isLinear(i))
		{
			if(i != 0)		// variable t is not considered
			{
				coefficients[row][i-1] = iter->coefficient;
			}
		}
	}
}

void Polynomial::linearCoefficients(iMatrix2 & coefficients, const int row) const
{
	// initially, the result should be filled with 0

	std::list<Monomial>::const_iterator iter;

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		int i;

		if(iter->d > 1)
			break;

		if(iter->isLinear(i))
		{
			if(i != 0)		// variable t is not considered
			{
				Real c, r;
				iter->coefficient.toCenterForm(c, r);
				coefficients.center[row][i-1] = c;
				coefficients.radius[row][i-1] = r;
			}
		}
	}
}

void Polynomial::constraintCoefficients(RowVector & result) const
{
	// initially, the result should be filled with 0

	std::list<Monomial>::const_iterator iter;

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		int i;

		if(iter->d > 1)
			break;

		if(iter->isLinear(i))
		{
			if(i > 0)
			{
				result.set(iter->coefficient.sup(), i-1);
			}
		}
	}
}

void Polynomial::constraintCoefficients(std::vector<Interval> & result) const
{
	// initially, the result should be filled with 0

	std::list<Monomial>::const_iterator iter;

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		int i;

		if(iter->d > 1)
			break;

		if(iter->isLinear(i))
		{
			if(i > 0)
			{
				result[i-1] = iter->coefficient;
			}
		}
	}
}

void Polynomial::toHornerForm(HornerForm & result) const
{
	result.clear();

	if(monomials.size() == 0)
		return;

	int numVars = (monomials.begin())->degrees.size();

	std::list<Monomial> lstMono = monomials;
	std::list<Monomial>::iterator iter = lstMono.begin();

	if(iter->d == 0)
	{
		result.constant = iter->coefficient;
		iter = lstMono.erase(iter);

		if(lstMono.size() == 0)
			return;
	}

	std::vector<std::list<Monomial> > vlMono;

	for(int i=0; i<numVars; ++i)
	{
		std::list<Monomial> lst_ith;

		for(iter = lstMono.begin(); iter != lstMono.end();)
		{
			if(iter->degrees[i] > 0)
			{
				iter->degrees[i] -= 1;
				iter->d -= 1;
				lst_ith.push_back(*iter);
				iter = lstMono.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		vlMono.push_back(lst_ith);
	}

	for(int i=0; i<numVars; ++i)
	{
		Polynomial polyTemp(vlMono[i]);
		HornerForm hf;
		polyTemp.toHornerForm(hf);
		result.hornerForms.push_back(hf);
	}
}

void Polynomial::rmConstant()
{
	if(monomials.size() > 0 && (monomials.begin())->d == 0)
	{
		monomials.erase( monomials.begin() );
	}
}

void Polynomial::decompose(Polynomial & linear, Polynomial & other) const
{
	std::list<Monomial>::const_iterator iter;
	linear.monomials.clear();
	other.monomials.clear();

	for(iter=monomials.begin(); iter!=monomials.end(); ++iter)
	{
		if(iter->d != 1)
		{
			other.monomials.push_back(*iter);
		}
		else
		{
			linear.monomials.push_back(*iter);
		}
	}
}

int Polynomial::degree() const
{
	if(monomials.size() > 0)
	{
		std::list<Monomial>::const_iterator iter = monomials.end();
		--iter;
		return iter->d;
	}
	else
	{
		return 0;
	}
}

int Polynomial::degree_wo_t() const
{
	std::list<Monomial>::const_iterator iter = monomials.begin();

	int degree = 0;

	for(; iter!=monomials.end(); ++iter)
	{
		int tmp = iter->d - iter->degrees[0];
		if(degree < tmp)
		{
			degree = tmp;
		}
	}

	return degree;
}

bool Polynomial::isLinear_wo_t() const
{
	std::list<Monomial>::const_iterator iter = monomials.begin();

	for(; iter!=monomials.end(); ++iter)
	{
		if(iter->d - iter->degrees[0] > 1)
		{
			return false;
		}
	}

	return true;
}

bool Polynomial::isZero() const
{
	if(monomials.size() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Polynomial::rmZeroTerms(const std::vector<int> & indices)
{
	if(indices.size() == 0)
	{
		return;
	}

	std::list<Monomial>::iterator iter = monomials.begin();

	for(; iter != monomials.end();)
	{
		bool bDeleted = false;

		for(int i=0; i<indices.size(); ++i)
		{
			if(iter->degrees[indices[i]] > 0)
			{
				iter = monomials.erase(iter);
				bDeleted = true;
				break;
			}
		}

		if(bDeleted == false)
		{
			++iter;
		}
	}
}

void Polynomial::integral_t()
{
	std::list<Monomial>::iterator iter = monomials.begin();

	for(; iter != monomials.end(); ++iter)
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



/*
void Polynomial::cutoff_normal(Interval & intRem, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold)
{
	Polynomial polyTemp;

	std::list<Monomial>::iterator iter;
	for(iter = monomials.begin(); iter != monomials.end(); )
	{
		double width = iter->coefficient.width();
		if(iter->d == 0)
		{
			if(width >= CUTOFF_WIDTH)
			{
				Interval M;
				iter->coefficient.remove_midpoint(M);
				polyTemp.monomials.push_back(*iter);
				iter->coefficient = M;
			}
			else
			{
				++iter;
			}
		}
		else
		{
			if(iter->coefficient.subseteq(cutoff_threshold) || width >= CUTOFF_WIDTH)
			{
				polyTemp.monomials.push_back(*iter);
				iter = monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	polyTemp.intEvalNormal(intRem, step_exp_table);
}

void Polynomial::cutoff(Interval & intRem, const std::vector<Interval> & domain, const Interval & cutoff_threshold)
{
	Polynomial polyTemp;

	std::list<Monomial>::iterator iter;
	for(iter = monomials.begin(); iter != monomials.end(); )
	{
		double width = iter->coefficient.width();
		if(iter->d == 0)
		{
			if(width >= CUTOFF_WIDTH)
			{
				Interval M;
				iter->coefficient.remove_midpoint(M);
				polyTemp.monomials.push_back(*iter);
				iter->coefficient = M;
			}
			else
			{
				++iter;
			}
		}
		else
		{
			if(iter->coefficient.subseteq(cutoff_threshold) || width >= CUTOFF_WIDTH)
			{
				polyTemp.monomials.push_back(*iter);
				iter = monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	polyTemp.intEval(intRem, domain);
}

void Polynomial::cutoff(const Interval & cutoff_threshold)
{
	std::list<Monomial>::iterator iter;
	for(iter = monomials.begin(); iter != monomials.end(); )
	{
		double width = iter->coefficient.width();
		if(iter->d == 0)
		{
			if(width >= CUTOFF_WIDTH)
			{
				Interval M;
				iter->coefficient.midpoint(M);
				iter->coefficient = M;
			}
			else
			{
				++iter;
			}
		}
		else
		{
			if(iter->coefficient.subseteq(cutoff_threshold) || width >= CUTOFF_WIDTH)
			{
				iter = monomials.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}
}
*/


void Polynomial::cutoff_normal(Interval & intRem, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold)
{
	Polynomial polyTemp;

	std::list<Monomial>::iterator iter;
	for(iter = monomials.begin(); iter != monomials.end(); )
	{
		Monomial monoTemp;
		int res = iter->cutoff(monoTemp, cutoff_threshold);

		switch(res)
		{
		case 0:
			++iter;
			break;
		case 1:
			polyTemp.monomials.push_back(monoTemp);
			++iter;
			break;
		case 2:
			polyTemp.monomials.push_back(*iter);
			iter = monomials.erase(iter);
			break;
		}
	}

	polyTemp.intEvalNormal(intRem, step_exp_table);
}

void Polynomial::cutoff(Interval & intRem, const std::vector<Interval> & domain, const Interval & cutoff_threshold)
{
	Polynomial polyTemp;

	std::list<Monomial>::iterator iter;
	for(iter = monomials.begin(); iter != monomials.end(); )
	{
		Monomial monoTemp;
		int res = iter->cutoff(monoTemp, cutoff_threshold);

		switch(res)
		{
		case 0:
			++iter;
			break;
		case 1:
			polyTemp.monomials.push_back(monoTemp);
			++iter;
			break;
		case 2:
			polyTemp.monomials.push_back(*iter);
			iter = monomials.erase(iter);
			break;
		}
	}

	polyTemp.intEval(intRem, domain);
}

void Polynomial::cutoff(const Interval & cutoff_threshold)
{
	std::list<Monomial>::iterator iter;
	for(iter = monomials.begin(); iter != monomials.end(); )
	{
		int res = iter->cutoff(cutoff_threshold);

		switch(res)
		{
		case 0:
			++iter;
			break;
		case 1:
			++iter;
			break;
		case 2:
			iter = monomials.erase(iter);
			break;
		}
	}
}

void Polynomial::derivative(Polynomial & result, const int varIndex) const
{
	result = *this;

	std::list<Monomial>::iterator iter;

	for(iter = result.monomials.begin(); iter != result.monomials.end(); )
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
			iter = result.monomials.erase(iter);
		}
	}
}

void Polynomial::LieDerivative(Polynomial & result, const std::vector<Polynomial> & f) const
{
	derivative(result, 0);

	int rangeDim = f.size();

	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial P;
		derivative(P, i+1);
		P *= f[i];
		result += P;
	}
}

void Polynomial::sub(Polynomial & result, const Polynomial & P, const int order) const
{
	std::list<Monomial> monomials1, monomials2;
	std::list<Monomial>::const_iterator iter;

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		if(iter->d == order)
		{
			monomials1.push_back(*iter);
		}
	}

	for(iter = P.monomials.begin(); iter != P.monomials.end(); ++iter)
	{
		if(iter->d == order)
		{
			monomials2.push_back(*iter);
		}
	}

	Polynomial P1(monomials1), P2(monomials2);
	result = P1 - P2;
}

void Polynomial::exp_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	Polynomial F = *this;

	// remove the center point of tm
	F.constant(const_part);
	F.rmConstant();				// F = tm - c

	const_part.exp_assign();	// exp(c)

	if(F.isZero())				// tm = c
	{
		Polynomial polyExp(const_part, numVars);
		result = polyExp;
		return;
	}

	Interval I(1);
	Polynomial polyOne(I, numVars);

	// to compute the expression 1 + F + (1/2!)F^2 + ... + (1/k!)F^k,
	// we evaluate its Horner form (...((1/(k-1))((1/k)*F+1)*F + 1) ... + 1)

	result = polyOne;

	for(int i=order; i>0; --i)
	{
		Interval intFactor(1);
		intFactor.div_assign((double)i);

		result.mul_assign(intFactor);

		result *= F;
		result.nctrunc(order);
		result.cutoff(cutoff_threshold);

		result += polyOne;
	}

	result.mul_assign(const_part);
}

void Polynomial::rec_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	Polynomial F = *this;

	// remove the center point of tm
	F.constant(const_part);
	F.rmConstant();				// F = tm - c

	const_part.rec_assign();	// 1/c

	if(F.isZero())				// tm = c
	{
		Polynomial polyExp(const_part, numVars);
		result = polyExp;
		return;
	}

	Interval I(1);
	Polynomial polyOne(I, numVars);
	Polynomial F_c;
	F.mul(F_c, const_part);

	// to compute the expression 1 - F/c + (F/c)^2 - ... + (-1)^k (F/c)^k,
	// we evaluate its Horner form (-1)*(...((-1)*(-F/c + 1)*F/c + 1)...) + 1

	result = polyOne;

	for(int i=order; i>0; --i)
	{
		result.inv_assign();

		result *= F_c;
		result.nctrunc(order);
		result.cutoff(cutoff_threshold);

		result += polyOne;
	}

	result.mul_assign(const_part);
}

void Polynomial::sin_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	Polynomial F = *this;

	// remove the center point of tm
	F.constant(const_part);
	F.rmConstant();			// F = tm - c

	if(F.isZero())			// tm = c
	{
		const_part.sin_assign();
		Polynomial polyExp(const_part, numVars);
		result = polyExp;
		return;
	}

	Interval sinc, cosc, msinc, mcosc;
	sinc = const_part.sin();
	cosc = const_part.cos();
	sinc.inv(msinc);
	cosc.inv(mcosc);

	Polynomial polyTemp(sinc, numVars);
	result = polyTemp;

	int k=1;
	Interval I(1);

	Polynomial polyPowerF(I, numVars);

	for(int i=1; i<=order; ++i, ++k)
	{
		k %= 4;

		I.div_assign((double)i);

		switch(k)
		{
		case 0:
		{
			polyPowerF *= F;
			polyPowerF.nctrunc(order);
			polyPowerF.cutoff(cutoff_threshold);

			polyTemp = polyPowerF;
			Interval intTemp = I * sinc;
			polyTemp.mul_assign(intTemp);

			result += polyTemp;

			break;
		}
		case 1:
		{
			polyPowerF *= F;
			polyPowerF.nctrunc(order);
			polyPowerF.cutoff(cutoff_threshold);

			polyTemp = polyPowerF;
			Interval intTemp = I * cosc;
			polyTemp.mul_assign(intTemp);

			result += polyTemp;

			break;
		}
		case 2:
		{
			polyPowerF *= F;
			polyPowerF.nctrunc(order);
			polyPowerF.cutoff(cutoff_threshold);

			polyTemp = polyPowerF;
			Interval intTemp = I * msinc;
			polyTemp.mul_assign(intTemp);

			result += polyTemp;

			break;
		}
		case 3:
		{
			polyPowerF *= F;
			polyPowerF.nctrunc(order);
			polyPowerF.cutoff(cutoff_threshold);

			polyTemp = polyPowerF;
			Interval intTemp = I * mcosc;
			polyTemp.mul_assign(intTemp);

			result += polyTemp;

			break;
		}
		}
	}

	result.cutoff(cutoff_threshold);
}

void Polynomial::cos_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	Polynomial F = *this;

	// remove the center point of tm
	F.constant(const_part);
	F.rmConstant();			// F = tm - c

	if(F.isZero())			// tm = c
	{
		const_part.cos_assign();
		Polynomial polyExp(const_part, numVars);
		result = polyExp;
		return;
	}

	Interval sinc, cosc, msinc, mcosc;
	sinc = const_part.sin();
	cosc = const_part.cos();
	sinc.inv(msinc);
	cosc.inv(mcosc);

	Polynomial polyTemp(cosc, numVars);
	result = polyTemp;

	int k=1;
	Interval I(1);

	Polynomial polyPowerF(I, numVars);

	for(int i=1; i<=order; ++i, ++k)
	{
		k %= 4;

		I.div_assign((double)i);

		switch(k)
		{
		case 0:
		{
			polyPowerF *= F;
			polyPowerF.nctrunc(order);
			polyPowerF.cutoff(cutoff_threshold);

			polyTemp = polyPowerF;
			Interval intTemp = I * cosc;
			polyTemp.mul_assign(intTemp);

			result += polyTemp;

			break;
		}
		case 1:
		{
			polyPowerF *= F;
			polyPowerF.nctrunc(order);
			polyPowerF.cutoff(cutoff_threshold);

			polyTemp = polyPowerF;
			Interval intTemp = I * msinc;
			polyTemp.mul_assign(intTemp);

			result += polyTemp;

			break;
		}
		case 2:
		{
			polyPowerF *= F;
			polyPowerF.nctrunc(order);
			polyPowerF.cutoff(cutoff_threshold);

			polyTemp = polyPowerF;
			Interval intTemp = I * mcosc;
			polyTemp.mul_assign(intTemp);

			result += polyTemp;

			break;
		}
		case 3:
		{
			polyPowerF *= F;
			polyPowerF.nctrunc(order);
			polyPowerF.cutoff(cutoff_threshold);

			polyTemp = polyPowerF;
			Interval intTemp = I * sinc;
			polyTemp.mul_assign(intTemp);

			result += polyTemp;

			break;
		}
		}
	}

	result.cutoff(cutoff_threshold);
}

void Polynomial::log_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	Polynomial F = *this;

	// remove the center point of tm
	F.constant(const_part);
	F.rmConstant();			// F = tm - c

	Interval C = const_part;

	const_part.log_assign();	// log(c)

	if(F.isZero())			// tm = c
	{
		Polynomial polyLog(const_part, numVars);
		result = polyLog;

		return;
	}

	Polynomial F_c;
	F.div(F_c, C);

	result = F_c;

	Interval I((double)order);
	result.div_assign(I);			// F/c * (1/order)

	for(int i=order; i>=2; --i)
	{
		Interval J(1);
		J.div_assign((double)(i-1));
		Polynomial polyJ(J, numVars);

		result -= polyJ;
		result.inv_assign();

		result *= F_c;
		result.nctrunc(order);
		result.cutoff(cutoff_threshold);
	}

	Polynomial const_part_poly(const_part, numVars);
	result += const_part_poly;
}

void Polynomial::sqrt_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const
{
	Interval const_part;

	Polynomial F = *this;

	// remove the center point of tm
	F.constant(const_part);
	F.rmConstant();			// F = tm - c

	Interval C = const_part;
	const_part.sqrt_assign();	// sqrt(c)

	if(F.isZero())			// tm = c
	{
		Polynomial polySqrt(const_part, numVars);
		result = polySqrt;

		return;
	}

	Polynomial F_2c;
	F.div(F_2c, C);

	Interval intTwo(2);
	F_2c.div_assign(intTwo);	// F/2c

	Interval intOne(1);
	Polynomial polyOne(intOne, numVars);

	result = F_2c;

	Interval K(1), J(1);

	for(int i=order, j=2*order-3; i>=2; --i, j-=2)
	{
		// i
		Interval K((double)i);

		// j = 2*i-3
		Interval J((double)j);

		result.inv_assign();
		result.mul_assign( J / K );

		result += polyOne;
		result *= F_2c;
		result.nctrunc(order);
		result.cutoff(cutoff_threshold);
	}

	result += polyOne;

	result.mul_assign(const_part);
}

void Polynomial::toString(std::string & result, const std::vector<std::string> & varNames) const
{
	std::string strPoly;

	if(monomials.size() == 0)
	{
		strPoly = "(0)";
		return;
	}

	std::list<Monomial>::const_iterator iter, iter_last;
	iter_last = monomials.end();
	--iter_last;

	strPoly += '(';

	for(iter = monomials.begin(); iter != iter_last; ++iter)
	{
		std::string strTemp;
		iter->toString(strTemp, varNames);

		strPoly += strTemp;
		strPoly += ' ';
		strPoly += '+';
		strPoly += ' ';
	}

	std::string strTemp2;
	monomials.back().toString(strTemp2, varNames);
	strPoly += strTemp2;
	strPoly += ')';

	result = strPoly;
}

void Polynomial::substitute(const int varID, const Interval & intVal)
{
	Polynomial result;

	std::list<Monomial>::iterator iter;
	for(iter=monomials.begin(); iter!=monomials.end(); ++iter)
	{
		iter->substitute(varID, intVal);
		result.add_assign(*iter);
	}

	monomials = result.monomials;
}

void Polynomial::substitute(const std::vector<int> & varIDs, const std::vector<Interval> & intVals)
{
	Polynomial result;

	std::list<Monomial>::iterator iter;
	for(iter=monomials.begin(); iter!=monomials.end(); ++iter)
	{
		iter->substitute(varIDs, intVals);
		result.add_assign(*iter);
	}

	monomials = result.monomials;
}

void Polynomial::substitute_with_precond(Interval & intRem, const std::vector<bool> & substitution, const std::vector<Interval> & step_exp_table)
{
	Polynomial polyRem;

	std::list<Monomial>::iterator iter;
	for(iter=monomials.begin(); iter!=monomials.end(); )
	{
		if(iter->substitute_with_precond(substitution))
		{
			polyRem.monomials.push_back(*iter);
			iter = monomials.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	polyRem.intEvalNormal(intRem, step_exp_table);
}

void Polynomial::substitute_with_precond_no_remainder(const std::vector<bool> & substitution)
{
	Polynomial result;

	std::list<Monomial>::iterator iter;
	for(iter=monomials.begin(); iter!=monomials.end(); )
	{
		if(iter->substitute_with_precond(substitution))
		{
			iter = monomials.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void Polynomial::simplification_in_decomposition(const std::vector<bool> & substitution)
{
	Interval even(0,1), odd(-1,1);

	Polynomial result;

	std::list<Monomial>::iterator iter;
	for(iter=monomials.begin(); iter!=monomials.end(); ++iter)
	{
		for(int i=1; i<substitution.size(); ++i)
		{
			if(substitution[i])
			{
				iter->d -= iter->degrees[i];

				if(iter->degrees[i]%2 == 0)
				{
					iter->coefficient *= even;
				}
				else
				{
					iter->coefficient *= odd;
				}

				iter->degrees[i] = 0;
			}
		}

		result.add_assign(*iter);
	}

	monomials = result.monomials;
}

void Polynomial::substitute(Polynomial & result, const int varID, const Interval & intVal) const
{
	result.clear();

	std::list<Monomial>::const_iterator iter;
	for(iter=monomials.begin(); iter!=monomials.end(); ++iter)
	{
		Monomial monomial = *iter;
		monomial.substitute(varID, intVal);
		result.add_assign(monomial);
	}
}

void Polynomial::substitute(Polynomial & result, const std::vector<int> & varIDs, const std::vector<Interval> & intVals) const
{
	result.clear();

	std::list<Monomial>::const_iterator iter;
	for(iter=monomials.begin(); iter!=monomials.end(); ++iter)
	{
		Monomial monomial = *iter;
		monomial.substitute(varIDs, intVals);
		result.add_assign(monomial);
	}
}

void Polynomial::evaluate_t(Polynomial & result, const std::vector<Interval> & step_exp_table) const
{
	result.clear();

	if(monomials.size() == 0)
		return;

	std::list<Monomial>::const_iterator iter;
	Interval intZero;

	if(step_exp_table[1].subseteq(intZero))		// t = 0
	{
		for(iter = monomials.begin(); iter != monomials.end(); ++iter)
		{
			if(iter->degrees[0] == 0)
			{
				result.add_assign(*iter);
			}
		}
	}
	else
	{
		for(iter = monomials.begin(); iter != monomials.end(); ++iter)
		{
			Monomial monoTemp = *iter;
			int tmp = monoTemp.degrees[0];

			if(tmp > 0)
			{
				monoTemp.coefficient *= step_exp_table[tmp];
				monoTemp.d -= tmp;
				monoTemp.degrees[0] = 0;
			}

			result.add_assign(monoTemp);
		}
	}
}

void Polynomial::extend(const int num)
{
	std::list<Monomial>::iterator iter;

	for(iter = monomials.begin(); iter != monomials.end(); ++iter)
	{
		iter->extend(num);
	}
}

void Polynomial::insert(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold) const
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
		toHornerForm(hf);

		hf.insert(result, vars, varsPolyRange, domain, cutoff_threshold);
	}
}

void Polynomial::insert_normal(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const Interval & cutoff_threshold) const
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
		toHornerForm(hf);

		hf.insert_normal(result, vars, varsPolyRange, step_exp_table, numVars, cutoff_threshold);
	}
}














// class for univariate polynomials
UnivariatePolynomial::UnivariatePolynomial()
{
	Interval intZero;
	coefficients.push_back(intZero);
}

UnivariatePolynomial::UnivariatePolynomial(const std::vector<Interval> & coeffs)
{
	coefficients = coeffs;
}

UnivariatePolynomial::UnivariatePolynomial(const UnivariatePolynomial & polynomial)
{
	coefficients = polynomial.coefficients;
}

UnivariatePolynomial::UnivariatePolynomial(const Interval & I)
{
	coefficients.push_back(I);
}

UnivariatePolynomial::UnivariatePolynomial(const double c)
{
	Interval I(c);
	coefficients.push_back(I);
}

UnivariatePolynomial::UnivariatePolynomial(const double c, const int d)
{
	Interval I(c), intZero;

	for(int i=0; i<d; ++i)
	{
		coefficients.push_back(intZero);
	}

	coefficients.push_back(I);
}

UnivariatePolynomial::UnivariatePolynomial(const Interval & I, const int d)
{
	Interval intZero;

	for(int i=0; i<d; ++i)
	{
		coefficients.push_back(intZero);
	}

	coefficients.push_back(I);
}

UnivariatePolynomial::UnivariatePolynomial(const std::string & strPolynomial)
{
	std::string prefix(str_prefix_univariate_polynomial);
	std::string suffix(str_suffix);

	std::string input = prefix + strPolynomial + suffix;

	parseUnivariatePolynomial(input);		// call the parser

	*this = up_parseresult;
}

UnivariatePolynomial::~UnivariatePolynomial()
{
	coefficients.clear();
}

void UnivariatePolynomial::set2zero()
{
	coefficients.clear();

	Interval intZero;
	coefficients.push_back(intZero);
}

int UnivariatePolynomial::degree() const
{
	return (int)(coefficients.size() - 1);
}

bool UnivariatePolynomial::isZero() const
{
	Interval intZero;

	int n = coefficients.size();

	if(n == 0)
	{
		return true;
	}
	else
	{
		bool bzero = true;
		for(int i=0; i<n; ++i)
		{
			if(!coefficients[i].isZero())
			{
				bzero = false;
				break;
			}
		}

		return bzero;
	}
}

Interval UnivariatePolynomial::intEval(const std::vector<Interval> & val_exp_table) const
{
	Interval result = coefficients[0];

	for(int i=1; i<coefficients.size(); ++i)
	{
		result += coefficients[i] * val_exp_table[i];
	}

	return result;
}

Interval UnivariatePolynomial::intEval(const Interval & val) const
{
	Interval result = coefficients[coefficients.size()-1];

	for(int i=coefficients.size()-2; i>=0; --i)
	{
		result = result * val + coefficients[i];
	}

	return result;
}

void UnivariatePolynomial::intEval(Real & c, Real & r, const std::vector<Interval> & val_exp_table) const
{
	Interval result = coefficients[0];

	for(int i=1; i<coefficients.size(); ++i)
	{
		result += coefficients[i] * val_exp_table[i];
	}

	result.toCenterForm(c, r);
}

void UnivariatePolynomial::intEval(Real & c, Real & r, const Interval & val) const
{
	Interval I = coefficients[coefficients.size()-1];

	for(int i=coefficients.size()-2; i>=0; --i)
	{
		I = I * val + coefficients[i];
	}

	I.toCenterForm(c, r);
}

void UnivariatePolynomial::integral()
{
	Interval intZero;
	coefficients.push_back(intZero);

	for(int i=coefficients.size()-2; i>=0; --i)
	{
		Interval factor(i+1);
		coefficients[i+1] = coefficients[i] / factor;
	}

	coefficients[0] = intZero;
}

void UnivariatePolynomial::times_x(const int order)
{
	Interval intZero;

	if(order == 0 || coefficients.size() == 0)
	{
		return;
	}

	if(coefficients.size() == 1)
	{
		if(coefficients[0].subseteq(intZero))
			return;
	}

	for(int i=0; i<order; ++i)
	{
		coefficients.push_back(intZero);
	}

	for(int i=coefficients.size()-order-1; i>=0; --i)
	{
		coefficients[i+order] = coefficients[i];
	}

	for(int i=0; i<order; ++i)
	{
		coefficients[i] = intZero;
	}
}

void UnivariatePolynomial::pow(UnivariatePolynomial & result, const int degree) const
{
	UnivariatePolynomial temp = *this;
	result = *this;

	for(int d = degree - 1; d > 0;)
	{
		if(d & 1)
		{
			result *= temp;
		}

		d >>= 1;

		if(d > 0)
		{
			temp *= temp;
		}
	}
}

void UnivariatePolynomial::decompose(UnivariatePolynomial & pos, UnivariatePolynomial & neg, Interval & rem) const
{
	pos.set2zero();
	neg.set2zero();

	Interval I = coefficients[0], M, intZero;
	I.remove_midpoint(M);

	rem = I;

	if(M > intZero)
	{
		pos.coefficients[0] = M;
	}
	else
	{
		neg.coefficients[0] = M;
	}


	for(int i=1; i<coefficients.size(); ++i)
	{
		if(coefficients[i] > intZero)
		{
			pos.coefficients.push_back(coefficients[i]);
			neg.coefficients.push_back(intZero);
		}
		else
		{
			neg.coefficients.push_back(coefficients[i]);
			pos.coefficients.push_back(intZero);
		}
	}
/*
	printf("=====================================\n");
	this->output(stdout);printf("\n\n");
	pos.output(stdout);printf("\n\n");
	neg.output(stdout);printf("\n\n");
	rem.dump(stdout);printf("\n");
	printf("=====================================\n");
*/
}

void UnivariatePolynomial::ctrunc(Interval & rem, const int order, const std::vector<Interval> & val_exp_table)
{
	UnivariatePolynomial trunc_part;

	for(int i=order+1; i<coefficients.size(); ++i)
	{
		trunc_part.coefficients.push_back(coefficients[i]);
	}

	for(int i=coefficients.size()-1; i>order; --i)
	{
		coefficients.pop_back();
	}

	rem = trunc_part.intEval(val_exp_table);

	rem *= val_exp_table[order+1];
}

void UnivariatePolynomial::ctrunc(Interval & rem, const int order, const Interval & val)
{
	UnivariatePolynomial trunc_part;

	for(int i=order+1; i<coefficients.size(); ++i)
	{
		trunc_part.coefficients.push_back(coefficients[i]);
	}

	for(int i=coefficients.size()-1; i>order; --i)
	{
		coefficients.pop_back();
	}

	rem = trunc_part.intEval(val);

	for(int i=0; i<=order; ++i)
	{
		rem *= val;
	}
}

void UnivariatePolynomial::ctrunc(Interval & rem1, Interval & rem2, const int order, const std::vector<Interval> & val1_exp_table, const std::vector<Interval> & val2_exp_table)
{
	UnivariatePolynomial trunc_part;

	for(int i=order+1; i<coefficients.size(); ++i)
	{
		trunc_part.coefficients.push_back(coefficients[i]);
	}

	for(int i=coefficients.size()-1; i>order; --i)
	{
		coefficients.pop_back();
	}

	rem1 = trunc_part.intEval(val1_exp_table);
	rem2 = trunc_part.intEval(val2_exp_table);

	rem1 *= val1_exp_table[order+1];
	rem2 *= val2_exp_table[order+1];
}

void UnivariatePolynomial::ctrunc(Interval & rem1, Interval & rem2, const int order, const Interval & val1, const Interval & val2)
{
	UnivariatePolynomial trunc_part;

	for(int i=order+1; i<coefficients.size(); ++i)
	{
		trunc_part.coefficients.push_back(coefficients[i]);
	}

	for(int i=coefficients.size()-1; i>order; --i)
	{
		coefficients.pop_back();
	}

	rem1 = trunc_part.intEval(val1);
	rem2 = trunc_part.intEval(val2);

	for(int i=0; i<=order; ++i)
	{
		rem1 *= val1;
		rem2 *= val2;
	}
}

void UnivariatePolynomial::ctrunc(const int order, const std::vector<Interval> & val_exp_table)
{
	UnivariatePolynomial trunc_part;

	for(int i=order+1; i<coefficients.size(); ++i)
	{
		trunc_part.coefficients.push_back(coefficients[i]);
	}

	for(int i=coefficients.size()-1; i>order; --i)
	{
		coefficients.pop_back();
	}

	Interval rem = trunc_part.intEval(val_exp_table);

	rem *= val_exp_table[order+1];

	coefficients[0] += rem;
}

void UnivariatePolynomial::ctrunc(const int order, const Interval & val)
{
	UnivariatePolynomial trunc_part;

	for(int i=order+1; i<coefficients.size(); ++i)
	{
		trunc_part.coefficients.push_back(coefficients[i]);
	}

	for(int i=coefficients.size()-1; i>=order; --i)
	{
		coefficients.pop_back();
	}

	Interval rem = trunc_part.intEval(val);

	for(int i=0; i<=order; ++i)
	{
		rem *= val;
	}

	coefficients[0] += rem;
}

void UnivariatePolynomial::nctrunc(const int order)
{
	for(int i=coefficients.size()-1; i>=order; --i)
	{
		coefficients.pop_back();
	}
}

void UnivariatePolynomial::substitute(UnivariatePolynomial & result, const std::vector<UnivariatePolynomial> & t_exp_table) const
{
	result.set2zero();

	result += coefficients[0];

	for(int i=1; i<coefficients.size(); ++i)
	{
		result += t_exp_table[i] * coefficients[i];
	}
}

void UnivariatePolynomial::substitute(UnivariatePolynomial & result, const UnivariatePolynomial & t) const
{
	result.coefficients.clear();
	result.coefficients.push_back(coefficients[coefficients.size()-1]);

	for(int i=coefficients.size()-2; i>=0; --i)
	{
		result = result * t + coefficients[i];
	}
}

void UnivariatePolynomial::output(FILE *fp) const
{
	Interval intZero;
	bool bfirst = true;

	if(!coefficients[0].subseteq(intZero))
	{
		coefficients[0].dump(stdout);
		bfirst = false;
	}

	for(int i=1; i<coefficients.size(); ++i)
	{
		if(!coefficients[i].subseteq(intZero))
		{
			if(!bfirst)
			{
				fprintf(fp, " + ");
			}
			else
			{
				bfirst = false;
			}

			coefficients[i].dump(stdout);
			fprintf(fp, "*t^%d", i);
		}
	}

	if(bfirst)
	{
		fprintf(fp, "0");
	}
}

UnivariatePolynomial & UnivariatePolynomial::operator = (const UnivariatePolynomial & polynomial)
{
	if(this == &polynomial)
		return *this;

	coefficients = polynomial.coefficients;
	return *this;
}

UnivariatePolynomial & UnivariatePolynomial::operator = (const Interval & I)
{
	coefficients.clear();
	coefficients.push_back(I);

	return *this;
}

UnivariatePolynomial & UnivariatePolynomial::operator += (const UnivariatePolynomial & polynomial)
{
	int n = coefficients.size();
	int m = polynomial.coefficients.size();

	if(n <= m)
	{
		for(int i=0; i<n; ++i)
		{
			coefficients[i] += polynomial.coefficients[i];
		}

		for(int i=n; i<m; ++i)
		{
			coefficients.push_back(polynomial.coefficients[i]);
		}
	}
	else
	{
		for(int i=0; i<m; ++i)
		{
			coefficients[i] += polynomial.coefficients[i];
		}
	}

	return *this;
}

UnivariatePolynomial & UnivariatePolynomial::operator -= (const UnivariatePolynomial & polynomial)
{
	int n = coefficients.size();
	int m = polynomial.coefficients.size();

	if(n <= m)
	{
		for(int i=0; i<n; ++i)
		{
			coefficients[i] -= polynomial.coefficients[i];
		}

		for(int i=n; i<m; ++i)
		{
			coefficients.push_back(polynomial.coefficients[i]);
			coefficients[i].inv_assign();
		}
	}
	else
	{
		for(int i=0; i<m; ++i)
		{
			coefficients[i] -= polynomial.coefficients[i];
		}
	}

	return *this;
}

UnivariatePolynomial & UnivariatePolynomial::operator *= (const UnivariatePolynomial & polynomial)
{
	if(this->isZero())
	{
		return *this;
	}

	if(polynomial.isZero())
	{
		this->set2zero();
		return *this;
	}

	int n = coefficients.size();
	int m = polynomial.coefficients.size();

	std::vector<Interval> result;

	for(int i=0; i<n; ++i)
	{
		for(int j=0; j<m; ++j)
		{
			int degree = i + j;
			if(result.size() <= degree)
			{
				result.push_back(coefficients[i] * polynomial.coefficients[j]);
			}
			else
			{
				result[degree] += coefficients[i] * polynomial.coefficients[j];
			}
		}
	}

	coefficients = result;

	return *this;
}

UnivariatePolynomial & UnivariatePolynomial::operator += (const Interval & I)
{
	coefficients[0] += I;
	return *this;
}

UnivariatePolynomial & UnivariatePolynomial::operator -= (const Interval & I)
{
	coefficients[0] -= I;
	return *this;
}

UnivariatePolynomial & UnivariatePolynomial::operator *= (const Interval & I)
{
	if(this->isZero())
	{
		return *this;
	}

	if(I.isZero())
	{
		this->set2zero();
		return *this;
	}

	for(int i=0; i<coefficients.size(); ++i)
	{
		coefficients[i] *= I;
	}

	return *this;
}

UnivariatePolynomial & UnivariatePolynomial::operator /= (const Interval & I)
{
	for(int i=0; i<coefficients.size(); ++i)
	{
		coefficients[i] /= I;
	}

	return *this;
}

UnivariatePolynomial UnivariatePolynomial::operator + (const UnivariatePolynomial & polynomial) const
{
	int n = coefficients.size();
	int m = polynomial.coefficients.size();

	UnivariatePolynomial result;
	result.coefficients.clear();

	if(n <= m)
	{
		for(int i=0; i<n; ++i)
		{
			result.coefficients.push_back(coefficients[i] + polynomial.coefficients[i]);
		}

		for(int i=n; i<m; ++i)
		{
			result.coefficients.push_back(polynomial.coefficients[i]);
		}
	}
	else
	{
		for(int i=0; i<m; ++i)
		{
			result.coefficients.push_back(coefficients[i] + polynomial.coefficients[i]);
		}

		for(int i=m; i<n; ++i)
		{
			result.coefficients.push_back(coefficients[i]);
		}
	}

	return result;
}

UnivariatePolynomial UnivariatePolynomial::operator - (const UnivariatePolynomial & polynomial) const
{
	int n = coefficients.size();
	int m = polynomial.coefficients.size();

	UnivariatePolynomial result;
	result.coefficients.clear();

	if(n <= m)
	{
		for(int i=0; i<n; ++i)
		{
			result.coefficients.push_back(coefficients[i] - polynomial.coefficients[i]);
		}

		for(int i=n; i<m; ++i)
		{
			result.coefficients.push_back(polynomial.coefficients[i]);
			result.coefficients[i].inv_assign();
		}
	}
	else
	{
		for(int i=0; i<m; ++i)
		{
			result.coefficients.push_back(coefficients[i] - polynomial.coefficients[i]);
		}

		for(int i=m; i<n; ++i)
		{
			result.coefficients.push_back(coefficients[i]);
		}
	}

	return result;
}

UnivariatePolynomial UnivariatePolynomial::operator * (const UnivariatePolynomial & polynomial) const
{
	UnivariatePolynomial result;

	if(this->isZero() || polynomial.isZero())
	{
		return result;
	}

	int n = coefficients.size();
	int m = polynomial.coefficients.size();

	for(int i=0; i<n; ++i)
	{
		for(int j=0; j<m; ++j)
		{
			int degree = i + j;
			if(result.coefficients.size() <= degree)
			{
				result.coefficients.push_back(coefficients[i] * polynomial.coefficients[j]);
			}
			else
			{
				result.coefficients[degree] += coefficients[i] * polynomial.coefficients[j];
			}
		}
	}

	return result;
}

Polynomial UnivariatePolynomial::operator * (const Polynomial & polynomial) const
{
	Polynomial result;

	for(int i=0; i<coefficients.size(); ++i)
	{
		std::list<Monomial>::const_iterator iter = polynomial.monomials.begin();
		Polynomial tmp;

		for(; iter != polynomial.monomials.end(); ++iter)
		{
			Monomial monomial = *iter;
			monomial.coefficient *= coefficients[i];
			monomial.degrees[0] += i;
			monomial.d += i;

			tmp.add_assign(monomial);
		}

		result += tmp;
	}

	return result;
}

UnivariatePolynomial UnivariatePolynomial::operator + (const Interval & I) const
{
	UnivariatePolynomial result = *this;
	result.coefficients[0] += I;

	return result;
}

UnivariatePolynomial UnivariatePolynomial::operator - (const Interval & I) const
{
	UnivariatePolynomial result = *this;
	result.coefficients[0] -= I;

	return result;
}

UnivariatePolynomial UnivariatePolynomial::operator * (const Interval & I) const
{
	UnivariatePolynomial result;

	if(this->isZero() || I.isZero())
	{
		return result;
	}

	result.coefficients[0] = coefficients[0] * I;
	for(int i=1; i<coefficients.size(); ++i)
	{
		result.coefficients.push_back(coefficients[i] * I);
	}

	return result;
}

UnivariatePolynomial UnivariatePolynomial::operator / (const Interval & I) const
{
	UnivariatePolynomial result;

	result.coefficients[0] = coefficients[0] / I;
	for(int i=1; i<coefficients.size(); ++i)
	{
		result.coefficients.push_back(coefficients[i] / I);
	}

	return result;
}







ParsePolynomial::ParsePolynomial()
{
}

ParsePolynomial::ParsePolynomial(const ParsePolynomial & setting)
{
	strPolynomial = setting.strPolynomial;
	variables = setting.variables;
	result = setting.result;
}

ParsePolynomial::~ParsePolynomial()
{
	variables.clear();
}

ParsePolynomial & ParsePolynomial::operator = (const ParsePolynomial & setting)
{
	if(this == &setting)
		return *this;

	strPolynomial = setting.strPolynomial;
	variables = setting.variables;
	result = setting.result;

	return *this;
}

void ParsePolynomial::clear()
{
	variables.clear();
}











namespace flowstar
{

void compute_factorial_rec(const int order)
{
	Interval I(1);

	factorial_rec.push_back(I);

	for(int i=1; i<=order; ++i)
	{
		I.div_assign((double)i);
		factorial_rec.push_back(I);
	}
}

void compute_power_4(const int order)
{
	Interval I(1);

	power_4.push_back(I);

	for(int i=1; i<=order; ++i)
	{
		I.mul_assign(4.0);
		power_4.push_back(I);
	}
}

void compute_double_factorial(const int order)
{
	Interval odd(1), even(1);

	double_factorial.push_back(even);
	double_factorial.push_back(odd);

	for(int i=2; i<=order; ++i)
	{
		if(i%2 == 0)
		{
			even.mul_assign((double)i);
			double_factorial.push_back(even);
		}
		else
		{
			odd.mul_assign((double)i);
			double_factorial.push_back(odd);
		}
	}
}

void computeTaylorExpansion(std::vector<HornerForm> & result, const std::vector<Polynomial> & ode, const int order)
{
	int rangeDim = ode.size();

	std::vector<Polynomial> taylorExpansion;
	std::vector<Polynomial> LieDeriv_n;

	for(int i=0; i<rangeDim; ++i)
	{
		RowVector row(rangeDim+1);
		row.set(1,i+1);
		Polynomial P(row);
		taylorExpansion.push_back(P);
		LieDeriv_n.push_back(P);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=1; j<=order; ++j)
		{
			Polynomial P1;
			LieDeriv_n[i].LieDerivative(P1, ode);
			LieDeriv_n[i] = P1;

			P1.mul_assign(factorial_rec[j]);
			P1.mul_assign(0,j);

			taylorExpansion[i] += P1;
		}
	}

	result.clear();

	for(int i=0; i<taylorExpansion.size(); ++i)
	{
		HornerForm hf;
		taylorExpansion[i].toHornerForm(hf);
		result.push_back(hf);
	}
}

void computeTaylorExpansion(std::vector<HornerForm> & result, const std::vector<Polynomial> & ode, const std::vector<int> & orders)
{
	int rangeDim = ode.size();

	std::vector<Polynomial> taylorExpansion;
	std::vector<Polynomial> LieDeriv_n;

	for(int i=0; i<rangeDim; ++i)
	{
		RowVector row(rangeDim+1);
		row.set(1,i+1);
		Polynomial P(row);
		taylorExpansion.push_back(P);
		LieDeriv_n.push_back(P);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=1; j<=orders[i]; ++j)
		{
			Polynomial P1;
			LieDeriv_n[i].LieDerivative(P1, ode);
			LieDeriv_n[i] = P1;

			P1.mul_assign(factorial_rec[j]);
			P1.mul_assign(0,j);

			taylorExpansion[i] += P1;
		}
	}

	result.clear();

	for(int i=0; i<taylorExpansion.size(); ++i)
	{
		HornerForm hf;
		taylorExpansion[i].toHornerForm(hf);
		result.push_back(hf);
	}
}

void computeTaylorExpansion(std::vector<HornerForm> & resultHF, std::vector<Polynomial> & resultMF, std::vector<Polynomial> & highest, const std::vector<Polynomial> & ode, const int order)
{
	int rangeDim = ode.size();

	std::vector<Polynomial> taylorExpansion;
	std::vector<Polynomial> LieDeriv_n;

	for(int i=0; i<rangeDim; ++i)
	{
		RowVector row(rangeDim+1);
		row.set(1,i+1);
		Polynomial P(row);
		taylorExpansion.push_back(P);
		LieDeriv_n.push_back(P);
	}

	highest.clear();

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=1; j<=order; ++j)
		{
			Polynomial P;
			LieDeriv_n[i].LieDerivative(P, ode);
			LieDeriv_n[i] = P;

			if(j == order)
			{
				highest.push_back(P);
			}

			P.mul_assign(factorial_rec[j]);
			P.mul_assign(0,j);

			taylorExpansion[i] += P;
		}
	}

	resultMF = taylorExpansion;

	resultHF.clear();
	for(int i=0; i<taylorExpansion.size(); ++i)
	{
		HornerForm hf;
		taylorExpansion[i].toHornerForm(hf);
		resultHF.push_back(hf);
	}
}

void computeTaylorExpansion(std::vector<HornerForm> & resultHF, std::vector<Polynomial> & resultMF, std::vector<Polynomial> & highest, const std::vector<Polynomial> & ode, const std::vector<int> & orders)
{
	int rangeDim = ode.size();

	std::vector<Polynomial> taylorExpansion;
	std::vector<Polynomial> LieDeriv_n;

	for(int i=0; i<rangeDim; ++i)
	{
		RowVector row(rangeDim+1);
		row.set(1,i+1);
		Polynomial P(row);
		taylorExpansion.push_back(P);
		LieDeriv_n.push_back(P);
	}

	highest.clear();

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=1; j<=orders[i]; ++j)
		{
			Polynomial P;
			LieDeriv_n[i].LieDerivative(P, ode);
			LieDeriv_n[i] = P;

			if(j == orders[i])
			{
				highest.push_back(P);
			}

			P.mul_assign(factorial_rec[j]);
			P.mul_assign(0,j);

			taylorExpansion[i] += P;
		}
	}

	resultMF = taylorExpansion;

	resultHF.clear();
	for(int i=0; i<taylorExpansion.size(); ++i)
	{
		HornerForm hf;
		taylorExpansion[i].toHornerForm(hf);
		resultHF.push_back(hf);
	}
}

void increaseExpansionOrder(std::vector<HornerForm> & resultHF, std::vector<Polynomial> & resultMF, std::vector<Polynomial> & highest, const std::vector<Polynomial> & taylorExpansion, const std::vector<Polynomial> & ode, const int order)
{
	int rangeDim = ode.size();

	std::vector<Polynomial> expansion = taylorExpansion;
	std::vector<Polynomial> LieDeriv_n = highest;

	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial P1;
		LieDeriv_n[i].LieDerivative(P1, ode);

		highest[i] = P1;

		P1.mul_assign(factorial_rec[order+1]);
		P1.mul_assign(0, order+1);

		expansion[i] += P1;
	}

	resultMF = expansion;

	resultHF.clear();
	for(int i=0; i<expansion.size(); ++i)
	{
		HornerForm hf;
		expansion[i].toHornerForm(hf);
		resultHF.push_back(hf);
	}
}

void increaseExpansionOrder(HornerForm & resultHF, Polynomial & resultMF, Polynomial & highest, const Polynomial & taylorExpansion, const std::vector<Polynomial> & ode, const int order)
{
	int rangeDim = ode.size();

	Polynomial expansion = taylorExpansion;
	Polynomial LieDeriv_n = highest;

	Polynomial P1;
	LieDeriv_n.LieDerivative(P1, ode);

	highest = P1;

	P1.mul_assign(factorial_rec[order+1]);
	P1.mul_assign(0, order+1);

	expansion += P1;

	resultMF = expansion;
	expansion.toHornerForm(resultHF);
}

}


