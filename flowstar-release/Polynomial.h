/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.

  The code is released as is under the GNU General Public License (GPL).
---*/

#ifndef POLYNOMIAL_H_
#define POLYNOMIAL_H_

#include "Monomial.h"
#include "Matrix.h"

namespace flowstar
{

class Polynomial;
class UnivariatePolynomial;
class TaylorModel;
class TaylorModelVec;
class Flowpipe;

extern std::vector<Interval> factorial_rec;
extern std::vector<Interval> power_4;
extern std::vector<Interval> double_factorial;


class Variables
{
public:
	std::map<std::string,int> varTab;
	std::vector<std::string> varNames;

public:
	Variables();
	~Variables();
	Variables(const Variables & variables);

	Variables & operator = (const Variables & variables);

	bool declareVar(const std::string & vName);
	int getIDForVar(const std::string & vName) const;
	bool getVarName(std::string & vName, const int id) const;
	int size() const;

	void clear();
};



class RangeTree
{
public:
	std::list<Interval> ranges;
	std::list<RangeTree *> children;

	RangeTree();
	RangeTree(const std::list<Interval> & ranges_input, const std::list<RangeTree *> & children_input);
	RangeTree(const RangeTree & tree);
	~RangeTree();

	RangeTree & operator = (const RangeTree & tree);
};

class HornerForm							// c + (...)*x1 + (...)*x2 + ... + (...)*xn
{
protected:
	Interval constant;						// constant part
	std::vector<HornerForm> hornerForms;			// other parts
public:
	HornerForm();
	HornerForm(const Interval & I);
	HornerForm(const Interval & I, const std::vector<HornerForm> & hfs);
	HornerForm(const HornerForm & hf);
	~HornerForm();

	void clear();
	void intEval(Interval & result, const std::vector<Interval> & domain) const;		// interval evaluation of the Horner form

	// substitute the variables by the given Taylor models
	void insert(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold) const;
	void insert_normal(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const Interval & cutoff_threshold) const;

	// with conservative truncation
	void insert_ctrunc(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const;
	// with non-conservative truncation
	void insert_no_remainder(TaylorModel & result, const TaylorModelVec & vars, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void insert_no_remainder_no_cutoff(TaylorModel & result, const TaylorModelVec & vars, const int numVars, const int order) const;
	void insert_no_remainder_no_cutoff(Polynomial & result, const TaylorModelVec & vars, const int numVars) const;
	void insert_no_remainder_no_cutoff(Polynomial & result, const std::vector<Polynomial> & vars, const int numVars) const;

	void insert_ctrunc_normal(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void insert_ctrunc_normal_no_cutoff(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const;

	void insert_ctrunc_normal(TaylorModel & result, RangeTree * & tree, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void insert_ctrunc_normal_no_cutoff(TaylorModel & result, RangeTree * & tree, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const;
	void insert_only_remainder(Interval & result, RangeTree *tree, const TaylorModelVec & vars, const Interval & timeStep) const;

	void dump(FILE *fp, const std::vector<std::string> & varNames) const;	// only for tests

	HornerForm & operator = (const HornerForm & hf);

	friend class Polynomial;
	friend class TaylorModel;
	friend class TaylorModelVec;
};

class Polynomial				// polynomials in monomial form
{
public:
	std::list<Monomial> monomials;
public:
	Polynomial();														// empty polynomial
	Polynomial(const Interval & constant, const int numVars);			// constant polynomial where dim is the number of the variables
	Polynomial(const RowVector & coefficients);
	Polynomial(const std::vector<Interval> & coefficients);				// linear polynomial with the given coefficients, the input matrix is a row vector
	Polynomial(const Interval *pcoefficients, const int numVars);
	Polynomial(const Monomial & monomial);								// polynomial with one monomial
	Polynomial(const std::list<Monomial> & monos);
	Polynomial(const int varID, const int degree, const int numVars);
	Polynomial(const UnivariatePolynomial & up, const int numVars);
	Polynomial(const Polynomial & polynomial);
	virtual ~Polynomial();

	Polynomial(const std::string & strPolynomial, const Variables & vars);

	void reorder();														// sort the monomials.
	void clear();

	void dump_interval(FILE *fp, const std::vector<std::string> & varNames) const;
	void dump_constant(FILE *fp, const std::vector<std::string> & varNames) const;

	void constant(Interval & result) const;											// constant part of the polynomial
	void constant(Real & result) const;

	void intEval(Interval & result, const std::vector<Interval> & domain) const;	// interval evaluation of the polynomial
	void intEvalNormal(Interval & result, const std::vector<Interval> & step_exp_table) const;	// fast evaluation over normalized domain

	void inv(Polynomial & result) const;											// additive inverse
	void inv_assign();

	// degree > 0
	void pow(Polynomial & result, const int degree) const;
	void pow_assign(const int degree);
	void pow(Polynomial & result, const int degree, const int order) const;
	void pow_assign(const int degree, const int order);

	void center();

	void add_assign(const Monomial & monomial);										// add a monomial
	void sub_assign(const Monomial & monomial);										// subtract a monomial
	void mul_assign(const Monomial & monomial);										// multiplied by a monomial

	void mul_assign(const Interval & I);											// multiplied by an interval
	void div_assign(const Interval & I);											// divided by an interval
	void mul(Polynomial & result, const Interval & I) const;
	void div(Polynomial & result, const Interval & I) const;

	void mul_assign(const int varIndex, const int degree);							// multiplied by a term x^d
	void mul(Polynomial result, const int varIndex, const int degree) const;

	Polynomial & operator = (const Polynomial & polynomial);
	Polynomial & operator += (const Polynomial & polynomial);
	Polynomial & operator -= (const Polynomial & polynomial);
	Polynomial & operator *= (const Polynomial & polynomial);
	Polynomial & operator *= (const Interval & I);

	Polynomial operator + (const Polynomial & polynomial) const;
	Polynomial operator - (const Polynomial & polynomial) const;
	Polynomial operator * (const Polynomial & polynomial) const;
	Polynomial operator * (const Interval & I) const;

	void ctrunc(Interval & remainder, const std::vector<Interval> & domain, const int order);	// conservative truncation
	void nctrunc(const int order);																// non-conservative truncation
	void ctrunc_normal(Interval & remainder, const std::vector<Interval> & step_exp_table, const int order);

	void linearCoefficients(std::vector<Interval> & result) const;								// the coefficients of the linear part, e.g. x+2y+z^3 returns [1,2,0]
	void linearCoefficients(RowVector & result) const;
	void linearCoefficients(iMatrix & coefficients, const int row) const;
	void linearCoefficients(iMatrix2 & coefficients, const int row) const;

	void constraintCoefficients(RowVector & result) const;
	void constraintCoefficients(std::vector<Interval> & result) const;
	void toHornerForm(HornerForm & result) const;												// transform the polynomial into a Horner form

	void rmConstant();				// remove the constant part
	void decompose(Polynomial & linear, Polynomial & other) const;
	int degree() const;				// degree of the polynomial
	int degree_wo_t() const;		// degree of the polynomial without the time variable

	bool isLinear_wo_t() const;
	bool isZero() const;

	void rmZeroTerms(const std::vector<int> & indices);

	void integral_t();

	void cutoff_normal(Interval & intRem, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold);
	void cutoff(Interval & intRem, const std::vector<Interval> & domain, const Interval & cutoff_threshold);
	void cutoff(const Interval & cutoff_threshold);

	void derivative(Polynomial & result, const int varIndex) const;					// derivative with respect to a variable
	void LieDerivative(Polynomial & result, const std::vector<Polynomial> & f) const;	// Lie derivative without truncation

	void sub(Polynomial & result, const Polynomial & P, const int order) const;		// compute the subtraction of the monomials with some order

	void exp_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void rec_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void sin_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void cos_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void log_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void sqrt_taylor(Polynomial & result, const int numVars, const int order, const Interval & cutoff_threshold) const;

	void toString(std::string & result, const std::vector<std::string> & varNames) const;	// transform a polynomial to a string

	void substitute(const int varID, const Interval & intVal);									// substitute a variable by an interval
	void substitute(const std::vector<int> & varIDs, const std::vector<Interval> & intVals);	// substitute a set of variables by intervals

	void substitute_with_precond(Interval & intRem, const std::vector<bool> & substitution, const std::vector<Interval> & step_exp_table);
	void substitute_with_precond_no_remainder(const std::vector<bool> & substitution);

	void simplification_in_decomposition(const std::vector<bool> & substitution);

	void substitute(Polynomial & result, const int varID, const Interval & intVal) const;
	void substitute(Polynomial & result, const std::vector<int> & varIDs, const std::vector<Interval> & intVals) const;

	void evaluate_t(Polynomial & result, const std::vector<Interval> & step_exp_table) const;

	void extend(const int num);		// current dim -> num

	void insert(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold) const;
	void insert_normal(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const Interval & cutoff_threshold) const;

	friend class TaylorModel;
	friend class TaylorModelVec;
	friend class UnivariatePolynomial;
	friend class mpMatrix;
	friend class Flowpipe;
	friend class ContinuousSystem;
};

class UnivariatePolynomial
{
protected:
	std::vector<Interval> coefficients;

public:
	UnivariatePolynomial();
	UnivariatePolynomial(const std::vector<Interval> & coeffs);
	UnivariatePolynomial(const UnivariatePolynomial & polynomial);
	UnivariatePolynomial(const Interval & I);
	UnivariatePolynomial(const double c);
	UnivariatePolynomial(const double c, const int d);
	UnivariatePolynomial(const Interval & I, const int d);
	UnivariatePolynomial(const std::string & strPolynomial);
	~UnivariatePolynomial();

	void set2zero();
	int degree() const;

	bool isZero() const;

	Interval intEval(const std::vector<Interval> & val_exp_table) const;	// interval evaluation based on the monomial form
	Interval intEval(const Interval & val) const;							// interval evaluation based on the Horner form

	void intEval(Real & c, Real & r, const std::vector<Interval> & val_exp_table) const;
	void intEval(Real & c, Real & r, const Interval & val) const;

	void integral();
	void times_x(const int order);
	void decompose(UnivariatePolynomial & pos, UnivariatePolynomial & neg, Interval & rem) const;

	void pow(UnivariatePolynomial & result, const int degree) const;		// degree > 0

	void ctrunc(Interval & rem, const int order, const std::vector<Interval> & val_exp_table);
	void ctrunc(Interval & rem, const int order, const Interval & val);

	void ctrunc(Interval & rem1, Interval & rem2, const int order, const std::vector<Interval> & val1_exp_table, const std::vector<Interval> & val2_exp_table);
	void ctrunc(Interval & rem1, Interval & rem2, const int order, const Interval & val1, const Interval & val2);

	void ctrunc(const int order, const std::vector<Interval> & val_exp_table);
	void ctrunc(const int order, const Interval & val);

	void nctrunc(const int order);

	void substitute(UnivariatePolynomial & result, const std::vector<UnivariatePolynomial> & t_exp_table) const;
	void substitute(UnivariatePolynomial & result, const UnivariatePolynomial & t) const;

	void output(FILE *fp) const;

	UnivariatePolynomial & operator = (const UnivariatePolynomial & polynomial);
	UnivariatePolynomial & operator = (const Interval & I);

	UnivariatePolynomial & operator += (const UnivariatePolynomial & polynomial);
	UnivariatePolynomial & operator -= (const UnivariatePolynomial & polynomial);
	UnivariatePolynomial & operator *= (const UnivariatePolynomial & polynomial);

	UnivariatePolynomial & operator += (const Interval & I);
	UnivariatePolynomial & operator -= (const Interval & I);
	UnivariatePolynomial & operator *= (const Interval & I);
	UnivariatePolynomial & operator /= (const Interval & I);

	UnivariatePolynomial operator + (const UnivariatePolynomial & polynomial) const;
	UnivariatePolynomial operator - (const UnivariatePolynomial & polynomial) const;
	UnivariatePolynomial operator * (const UnivariatePolynomial & polynomial) const;

	Polynomial operator * (const Polynomial & polynomial) const;

	UnivariatePolynomial operator + (const Interval & I) const;
	UnivariatePolynomial operator - (const Interval & I) const;
	UnivariatePolynomial operator * (const Interval & I) const;
	UnivariatePolynomial operator / (const Interval & I) const;

	friend class Polynomial;
	friend class TaylorModel;
	friend class TaylorModelVec;
	friend class upMatrix;
};


class ParsePolynomial
{
public:
	std::string strPolynomial;
	Variables variables;
	Polynomial result;

public:
	ParsePolynomial();
	ParsePolynomial(const ParsePolynomial & setting);
	~ParsePolynomial();

	ParsePolynomial & operator = (const ParsePolynomial & setting);

	void clear();
};


void compute_factorial_rec(const int order);
void compute_power_4(const int order);
void compute_double_factorial(const int order);

void computeTaylorExpansion(std::vector<HornerForm> & result, const std::vector<Polynomial> & ode, const int order);
void computeTaylorExpansion(std::vector<HornerForm> & result, const std::vector<Polynomial> & ode, const std::vector<int> & orders);

void computeTaylorExpansion(std::vector<HornerForm> & resultHF, std::vector<Polynomial> & resultMF, std::vector<Polynomial> & highest, const std::vector<Polynomial> & ode, const int order);
void computeTaylorExpansion(std::vector<HornerForm> & resultHF, std::vector<Polynomial> & resultMF, std::vector<Polynomial> & highest, const std::vector<Polynomial> & ode, const std::vector<int> & orders);

void increaseExpansionOrder(std::vector<HornerForm> & resultHF, std::vector<Polynomial> & resultMF, std::vector<Polynomial> & highest, const std::vector<Polynomial> & taylorExpansion, const std::vector<Polynomial> & ode, const int order);
void increaseExpansionOrder(HornerForm & resultHF, Polynomial & resultMF, Polynomial & highest, const Polynomial & taylorExpansion, const std::vector<Polynomial> & ode, const int order);

extern UnivariatePolynomial up_parseresult;
extern ParsePolynomial parsePolynomial;
}

void parseUnivariatePolynomial(const std::string & strPolynomial);
void parseMultivariatePolynomial();

#endif /* POLYNOMIAL_H_ */
