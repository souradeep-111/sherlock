/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.

  The code is released as is under the GNU General Public License (GPL).
---*/

#ifndef TAYLORMODEL_H_
#define TAYLORMODEL_H_

#include "Polynomial.h"

namespace flowstar
{

class Expression_AST;

class Taylor_Model_Computation_Setting
{
protected:
	Variables vars;		// state variables
	Variables pars;		// parameters which are defined based on the state variables
	Interval cutoff_threshold;
	std::vector<Interval> domain;

public:
	Taylor_Model_Computation_Setting(const Variables & variables, const std::vector<Interval> & ranges, const Variables & parameters);
	Taylor_Model_Computation_Setting(const Taylor_Model_Computation_Setting & setting);
	~Taylor_Model_Computation_Setting();

	Taylor_Model_Computation_Setting & operator = (const Taylor_Model_Computation_Setting & setting);

	bool setCutoff(const Interval & cutoff);
	bool setPrecision(const int prec);

	friend class TaylorModel;
	friend class AST_Node;
	friend class Expression_AST;
};



class TaylorModel			// Taylor models: R^n -> R. We use t to denote the time variable and x to denote the state variable.
{
public:
	Polynomial expansion;	// Taylor expansion
	Interval remainder;		// remainder interval
public:
	TaylorModel();			// empty Taylor model.
	TaylorModel(const Interval & I, const int numVars);				// constant
	TaylorModel(const Polynomial & polyExp);
	TaylorModel(const Polynomial & polyExp, const Interval & I);	// Taylor model (P,I)
	TaylorModel(const RowVector & coefficients);
	TaylorModel(const RowVector & coefficients, const Interval & I);
	TaylorModel(const std::vector<Interval> & coefficients);
	TaylorModel(const std::vector<Interval> & coefficients, const Interval & I);
	TaylorModel(const Interval *pcoefficients, const int numVars);
	TaylorModel(const TaylorModel & tm);
	virtual ~TaylorModel();

	TaylorModel(const std::string & strPolynomial, const Variables & vars);
	TaylorModel(const std::string & strPolynomial, const Interval & rem, const Variables & vars);

	void clear();
	void dump_interval(FILE *fp, const std::vector<std::string> & varNames) const;
	void dump_constant(FILE *fp, const std::vector<std::string> & varNames) const;
	void output(FILE *fp, const Variables & vars) const;

	void constant(Interval & result) const;									// Return the constant part of the expansion.
	void constant(Real & result) const;

	void intEval(Interval & result, const std::vector<Interval> & domain) const;
	void intEvalNormal(Interval & result, const std::vector<Interval> & step_exp_table) const;

	void ctrunc(const std::vector<Interval> & domain, const int order);		// conservative truncation
	void nctrunc(const int order);										// non-conservative truncation
	void ctrunc_normal(const std::vector<Interval> & step_exp_table, const int order);

	void inv(TaylorModel & result) const;									// additive inverse
	void inv_assign();

	void add_assign(const TaylorModel & tm);
	void sub_assign(const TaylorModel & tm);

	void mul_ctrunc(TaylorModel & result, const TaylorModel & tm, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const;
	void mul_ctrunc_normal(TaylorModel & result, const TaylorModel & tm, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold) const;
	void mul_no_remainder(TaylorModel & result, const TaylorModel & tm, const int order, const Interval & cutoff_threshold) const;
	void mul_no_remainder_no_cutoff(TaylorModel & result, const TaylorModel & tm, const int order) const;
	void mul(TaylorModel & result, const Interval & I) const;

	void mul_ctrunc_assign(const TaylorModel & tm, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold);
	void mul_ctrunc_normal_assign(const TaylorModel & tm, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold);
	void mul_no_remainder_assign(const TaylorModel & tm, const int order, const Interval & cutoff_threshold);
	void mul_no_remainder_no_cutoff_assign(const TaylorModel & tm, const int order);
	void mul_assign(const Interval & I);

	void mul_insert(TaylorModel & result, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold) const;
	void mul_insert_normal(TaylorModel & result, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold) const;
	void mul_insert_ctrunc(TaylorModel & result, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const;
	void mul_insert_ctrunc_normal(TaylorModel & result, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold) const;
	void mul_insert_ctrunc_normal_no_cutoff(TaylorModel & result, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order) const;
	void mul_insert_ctrunc_normal(TaylorModel & result, Interval & tm1, Interval & intTrunc, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold) const;
	void mul_insert_ctrunc_normal_no_cutoff(TaylorModel & result, Interval & tm1, Interval & intTrunc, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order) const;

	void mul_insert_assign(const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold);
	void mul_insert_normal_assign(const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold);
	void mul_insert_ctrunc_assign(const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold);
	void mul_insert_ctrunc_normal_assign(const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold);
	void mul_insert_ctrunc_normal_no_cutoff_assign(const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order);

	void mul_insert_ctrunc_normal_assign(Interval & tm1, Interval & intTrunc, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold);
	void mul_insert_ctrunc_normal_no_cutoff_assign(Interval & tm1, Interval & intTrunc, const TaylorModel & tm, const Interval & tmPolyRange, const std::vector<Interval> & step_exp_table, const int order);

	void div(TaylorModel & result, const Interval & I) const;
	void div_assign(const Interval & I);

	void derivative(TaylorModel & result, const int varIndex) const;		// derivative with respect to a variable

	// Lie derivative, the vector field is given by f
	void LieDerivative_no_remainder(TaylorModel & result, const TaylorModelVec & f, const int order, const Interval & cutoff_threshold) const;

	void integral(TaylorModel & result, const Interval & I) const;				// Integral with respect to t
	void integral_no_remainder(TaylorModel & result) const;

	void linearCoefficients(std::vector<Interval> & result) const;
	void linearCoefficients(iMatrix & coefficients, const int row) const;
	void linearCoefficients(iMatrix2 & coefficients, const int row) const;

	void toHornerForm(HornerForm & result, Interval & I) const;

	void insert(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold) const;
	void insert_normal(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const Interval & cutoff_threshold) const;
	void insert_ctrunc(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const;
	void insert_no_remainder(TaylorModel & result, const TaylorModelVec & vars, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void insert_no_remainder_no_cutoff(TaylorModel & result, const TaylorModelVec & vars, const int numVars, const int order) const;
	void insert_ctrunc_normal(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void insert_ctrunc_normal_no_cutoff(TaylorModel & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const;

	void evaluate_t(TaylorModel & result, const std::vector<Interval> & step_exp_table) const;			// evaluate the Taylor model at time t

	void mul(TaylorModel & result, const int varIndex, const int degree) const;		// multiplied by a term x^d
	void mul_assign(const int varIndex, const int degree);

	void rmConstant();
	void decompose(TaylorModel & linear, TaylorModel & other) const;
	void cutoff_normal(const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold);
	void cutoff(const std::vector<Interval> & domain, const Interval & cutoff_threshold);
	void cutoff(const Interval & cutoff_threshold);

	int degree() const;
	bool isZero() const;

	void center_nc();

	void rmZeroTerms(const std::vector<int> & indices);

	void normalize(std::vector<Interval> & domain);

	void polyRange(Interval & result, const std::vector<Interval> & domain) const;
	void polyRangeNormal(Interval & result, const std::vector<Interval> & step_exp_table) const;

	void exp_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void rec_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void sin_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void cos_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void log_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void sqrt_taylor(TaylorModel & result, std::list<Interval> & ranges, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;

	void exp_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void rec_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void sin_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void cos_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void log_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void sqrt_taylor(TaylorModel & result, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;


	// ================== API ==================
	void exp_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const;
	void rec_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const;
	void sin_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const;
	void cos_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const;
	void log_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const;
	void sqrt_taylor(TaylorModel & result, const int order, const Taylor_Model_Computation_Setting & setting) const;

	void add(TaylorModel & result, const TaylorModel & tm) const;			// addition
	void sub(TaylorModel & result, const TaylorModel & tm) const;			// subtraction
	void mul(TaylorModel & result, const TaylorModel & tm, const int order, const Taylor_Model_Computation_Setting & setting) const;
	void intEval(Interval & result, const Taylor_Model_Computation_Setting & setting) const;
	// =========================================


	Interval getRemainder() const;
	void getExpansion(Polynomial & P) const;

	void extend(const int num);

	void substitute(TaylorModel & result, const std::vector<int> & varIDs, const std::vector<Interval> & intVals) const;
	void substitute_with_precond(const std::vector<bool> & substitution, const std::vector<Interval> & step_exp_table);
	void substitute_with_precond_no_remainder(const std::vector<bool> & substitution);

	TaylorModel & operator = (const TaylorModel & tm);

	friend class HornerForm;
	friend class Polynomial;
	friend class TaylorModelVec;
	friend class Flowpipe;
	friend class ContinuousSystem;
	friend class ContinuousReachability;
	friend class HybridSystem;
	friend class HybridReachability;
};

class TaylorModelVec			// Taylor models: R^n -> R^m
{
public:
	std::vector<TaylorModel> tms;
public:
	TaylorModelVec();
	TaylorModelVec(const std::vector<TaylorModel> & tms_input);
	TaylorModelVec(const std::vector<Interval> & constants, const int numVars);
	TaylorModelVec(const Matrix & coefficients);
	TaylorModelVec(iMatrix & coefficients);
	TaylorModelVec(const std::vector<Interval> & coefficients);
	TaylorModelVec(const Matrix & coefficients, const std::vector<Interval> & remainders);
	TaylorModelVec(const std::vector<std::vector<Interval> > & coefficients);
	TaylorModelVec(const std::vector<std::vector<Interval> > & coefficients, const std::vector<Interval> & remainders);
	TaylorModelVec(const std::vector<Interval> & intVec, std::vector<Interval> & domain);
	TaylorModelVec(const TaylorModelVec & tmv);
	~TaylorModelVec();

	void clear();
	void dump_interval(FILE *fp, const std::vector<std::string> & stateVarNames, const std::vector<std::string> & tmVarNames) const;
	void dump_constant(FILE *fp, const std::vector<std::string> & stateVarNames, const std::vector<std::string> & tmVarNames) const;
	void constant(std::vector<Interval> & result) const;

	void intEval(std::vector<Interval> & result, const std::vector<Interval> & domain) const;
	void intEval(std::vector<Interval> & result, const std::vector<Interval> & domain, const std::vector<int> & varIDs) const;

	void intEvalNormal(std::vector<Interval> & result, const std::vector<Interval> & step_exp_table) const;
	void intEvalNormal(std::vector<Interval> & result, const std::vector<Interval> & step_exp_table, const std::vector<int> & varIDs) const;

	void ctrunc(const std::vector<Interval> & domain, const int order);		// conservative truncation
	void nctrunc(const int order);										// non-conservative truncation
	void ctrunc_normal(const std::vector<Interval> & step_exp_table, const int order);

	void ctrunc(const std::vector<Interval> & domain, const std::vector<int> & orders);		// conservative truncation
	void nctrunc(const std::vector<int> & orders);										// non-conservative truncation
	void ctrunc_normal(const std::vector<Interval> & step_exp_table, const std::vector<int> & orders);

	void inv(TaylorModelVec & result) const;									// additive inverse
	void add(TaylorModelVec & result, const TaylorModelVec & tmv) const;		// addition
	void sub(TaylorModelVec & result, const TaylorModelVec & tmv) const;		// subtraction

	void add_assign(const TaylorModelVec & tmv);
	void sub_assign(const TaylorModelVec & tmv);

	void mul(TaylorModelVec & result, const Interval & I) const;				// Multiplication of a Taylor model and a constant.
	void mul_assign(const Interval & I);

	void div(TaylorModelVec & result, const Interval & I) const;				// Divide the Taylor model by a constant.
	void div_assign(const Interval & I);

	void derivative(TaylorModelVec & result, const int varIndex) const;

	void LieDerivative_no_remainder(TaylorModelVec & result, const TaylorModelVec & f, const int order, const Interval & cutoff_threshold) const;
	void LieDerivative_no_remainder(TaylorModelVec & result, const TaylorModelVec & f, const std::vector<int> & orders, const Interval & cutoff_threshold) const;

	void integral(TaylorModelVec & result, const Interval & I) const;
	void integral_no_remainder(TaylorModelVec & result) const;

	void linearCoefficients(std::vector<std::vector<Interval> > & result) const;
	void linearCoefficients(iMatrix & coefficients) const;
	void linearCoefficients(iMatrix2 & coefficients) const;

	void rmZeroTerms(const std::vector<int> & indices);

	void insert(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const Interval & cutoff_threshold) const;
	void insert_normal(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const Interval & cutoff_threshold) const;

	void insert_ctrunc(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const;
	void insert_no_remainder(TaylorModelVec & result, const TaylorModelVec & vars, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void insert_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void insert_ctrunc_normal_no_cutoff(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const;


	void insert_ctrunc(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & domain, const std::vector<int> & orders, const Interval & cutoff_threshold) const;
	void insert_no_remainder(TaylorModelVec & result, const TaylorModelVec & vars, const int numVars, const std::vector<int> & orders, const Interval & cutoff_threshold) const;
	void insert_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & vars, const std::vector<Interval> & varsPolyRange, const std::vector<Interval> & step_exp_table, const int numVars, const std::vector<int> & orders, const Interval & cutoff_threshold) const;

	void insert_no_remainder_no_cutoff(TaylorModelVec & result, const TaylorModelVec & vars, const int numVars, const int order) const;

	void evaluate_t(TaylorModelVec & result, const std::vector<Interval> & step_exp_table) const;

	void mul(TaylorModelVec & result, const int varIndex, const int degree) const;
	void mul_assign(const int varIndex, const int degree);

	void linearTrans(TaylorModelVec & result, const Matrix & A) const;		// linear transformation
	void linearTrans(TaylorModelVec & result, rMatrix & A) const;
	void linearTrans_assign(const Matrix & A);
	void linearTrans_assign(rMatrix & A);

	void scale(TaylorModelVec & result, const std::vector<Interval> & S);
	void scale_assign(const std::vector<Interval> & S);

	void rmConstant();
	void decompose(TaylorModelVec & linear, TaylorModelVec & other) const;
	void cutoff_normal(const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold);
	void cutoff(const std::vector<Interval> & domain, const Interval & cutoff_threshold);
	void cutoff(const Interval & cutoff_threshold);
	void cutoff_normal(iMatrix & M, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold);

	void center_nc();
	void Expansion(std::vector<Polynomial> & polys) const;
	void Remainder(iMatrix & rem) const;

	void extend(const int num);

	void get_lti_matrices(iMatrix & A, iMatrix & B) const;
	void get_ltv_matrices(upMatrix & A, upMatrix & B) const;

	void Picard_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void Picard_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const std::vector<std::vector<bool> > & substitution, const int numVars, const int order, const Interval & cutoff_threshold) const;

	void Picard_no_remainder_no_cutoff(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const int order) const;
//	void Picard_no_remainder_no_cutoff(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const std::vector<TaylorModelVec> & sub_tmvs, const int numVars, const int order) const;

	void Picard_no_remainder_assign(const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const int order, const Interval & cutoff_threshold);
	void Picard_no_remainder_assign(const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const std::vector<std::vector<bool> > & substitution, const int numVars, const int order, const Interval & cutoff_threshold);

	void Picard_no_remainder_no_cutoff_assign(const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const int order);
//	void Picard_no_remainder_no_cutoff_assign(const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const std::vector<TaylorModelVec> & sub_tmvs, const int numVars, const int order);

	void Picard_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void Picard_ctrunc_normal_assign(const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold);

	void Picard_ctrunc_normal(TaylorModelVec & result, std::vector<RangeTree *> & trees, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;
	void Picard_ctrunc_normal(TaylorModelVec & result, std::vector<RangeTree *> & trees, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const std::vector<int> & orders, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;

	void Picard_ctrunc_normal(TaylorModelVec & result, std::vector<RangeTree *> & trees, std::vector<std::vector<Interval> > & trunc_parts, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<std::vector<bool> > & substitution, const std::vector<Interval> & step_exp_table, const int numVars, const int order, const Interval & cutoff_threshold) const;
	void Picard_only_remainder(std::vector<Interval> & result, std::vector<RangeTree *> & trees, std::vector<std::vector<Interval> > & trunc_parts, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const Interval & timeStep) const;

	void Picard_ctrunc_normal_no_cutoff(TaylorModelVec & result, std::vector<RangeTree *> & trees, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const;
//	void Picard_ctrunc_normal_no_cutoff(TaylorModelVec & result, std::vector<RangeTree *> & trees, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<TaylorModelVec> & sub_tmv, const std::vector<Interval> & step_exp_table, const int numVars, const int order) const;

	void Picard_only_remainder(std::vector<Interval> & result, std::vector<RangeTree *> & trees, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const Interval & timeStep, const std::vector<bool> & constant) const;

	void Picard_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const std::vector<int> & orders, const std::vector<bool> & bIncreased, const Interval & cutoff_threshold) const;
	void Picard_no_remainder_assign(const TaylorModelVec & x0, const std::vector<HornerForm> & ode, const int numVars, const std::vector<int> & orders, const std::vector<bool> & bIncreased, const Interval & cutoff_threshold);
	void Picard_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const std::vector<int> & orders, const Interval & cutoff_threshold) const;
	void Picard_ctrunc_normal_assign(const TaylorModelVec & x0, const std::vector<Interval> & polyRange, const std::vector<HornerForm> & ode, const std::vector<Interval> & step_exp_table, const int numVars, const std::vector<int> & orders, const Interval & cutoff_threshold);

	// using Taylor approximation
	void Picard_non_polynomial_taylor_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const int order, const Interval & cutoff_threshold) const;
	void Picard_non_polynomial_taylor_no_remainder_assign(const TaylorModelVec & x0, const std::vector<std::string> & strOde, const int order, const Interval & cutoff_threshold);

	void Picard_non_polynomial_taylor_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<int> & orders, const std::vector<bool> & bIncreased, const Interval & cutoff_threshold) const;
	void Picard_non_polynomial_taylor_no_remainder_assign(const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<int> & orders, const std::vector<bool> & bIncreased, const Interval & cutoff_threshold);

	void Picard_non_polynomial_taylor_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;
//	void Picard_non_polynomial_taylor_ctrunc_normal_assign(const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold);

	void Picard_non_polynomial_taylor_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<Interval> & step_exp_table, const std::vector<int> & orders, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;
//	void Picard_non_polynomial_taylor_ctrunc_normal_assign(const TaylorModelVec & x0, const std::vector<std::string> & strOde, const std::vector<Interval> & step_exp_table, const std::vector<int> & orders, const Interval & cutoff_threshold);

	void Picard_non_polynomial_taylor_only_remainder(std::vector<Interval> & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const Interval & timeStep, const int order, const std::vector<bool> & constant) const;
	void Picard_non_polynomial_taylor_only_remainder(std::vector<Interval> & result, const TaylorModelVec & x0, const std::vector<std::string> & strOde, const Interval & timeStep, const std::vector<int> & orders, const std::vector<bool> & constant) const;

	// ================ Picard operation for the new expression data structure ================

	void Picard_trunc_no_remainder(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<Expression_AST> & ode, const int numVars, const int order, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;
	void Picard_trunc_no_remainder_assign(const TaylorModelVec & x0, const std::vector<Expression_AST> & ode, const int numVars, const int order, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part);
	void Picard_ctrunc_normal(TaylorModelVec & result, const TaylorModelVec & x0, const std::vector<Expression_AST> & ode, const std::vector<Interval> & step_exp_table, const int order, const int numVars, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part, std::list<Interval> & intermediate_ranges) const;
	void Picard_ctrunc_normal_remainder(std::vector<Interval> & result, const TaylorModelVec & x0, const std::vector<Expression_AST> & ode, const Interval & timeStep, const int order, const std::vector<bool> & constant, std::list<Interval> & intermediate_ranges) const;

	// ========================================================================================


	void normalize(std::vector<Interval> & domain);		// we assume that the original domain is full-dimensional

	void polyRange(std::vector<Interval> & result, const std::vector<Interval> & domain) const;
	void polyRangeNormal(std::vector<Interval> & result, const std::vector<Interval> & step_exp_table) const;

	void substitute(TaylorModelVec & result, const std::vector<std::vector<int> > & varIDs, const std::vector<std::vector<Interval> > & intVals) const;

	void substitute_with_precond(const std::vector<bool> & substitution, const std::vector<Interval> & step_exp_table);
	void substitute_with_precond(const std::vector<std::vector<bool> > & substitution, const std::vector<Interval> & step_exp_table);
	void substitute_with_precond_no_remainder(const std::vector<bool> & substitution);

	void get_samples(rMatrix & samples) const;	// the domain is assumed to be normalized to [-1,1]

	TaylorModelVec & operator = (const TaylorModelVec & tmv);
};

class ParseSetting
{
public:
	std::string strODE;

	std::list<Interval> ranges;
	std::list<Interval>::iterator iterRange;

	std::vector<Interval> step_exp_table;
	TaylorModelVec flowpipe;
	int order;

	Interval cutoff_threshold;

public:
	ParseSetting();
	ParseSetting(const ParseSetting & setting);
	virtual ~ParseSetting();

	virtual void clear();

	virtual ParseSetting & operator = (const ParseSetting & setting);
};

class ParseResult					// the data structure of a parsed non-polynomial ODE with its remainder
{
public:
	Polynomial		expansion;
	Interval		remainder;
	std::string		strExpansion;
	bool			bConstant;
	Interval		constant;

	ParseResult();
	ParseResult(const ParseResult & result);
	~ParseResult();

	ParseResult & operator = (const ParseResult & result);
};


void exp_taylor_remainder(Interval & result, const Interval & tmRange, const int order);
void rec_taylor_remainder(Interval & result, const Interval & tmRange, const int order);
void sin_taylor_remainder(Interval & result, const Interval & C, const Interval & tmRange, const int order);
void cos_taylor_remainder(Interval & result, const Interval & C, const Interval & tmRange, const int order);
void log_taylor_remainder(Interval & result, const Interval & tmRange, const int order);
void sqrt_taylor_remainder(Interval & result, const Interval & tmRange, const int order);


void exp_taylor_remainder_external(Interval & result, const Interval & tmRange, const int order);
void rec_taylor_remainder_external(Interval & result, const Interval & tmRange, const int order);
void sin_taylor_remainder_external(Interval & result, const Interval & C, const Interval & tmRange, const int order);
void cos_taylor_remainder_external(Interval & result, const Interval & C, const Interval & tmRange, const int order);
void log_taylor_remainder_external(Interval & result, const Interval & tmRange, const int order);
void sqrt_taylor_remainder_external(Interval & result, const Interval & tmRange, const int order);


void exp_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order);
void rec_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order);
void sin_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order);
void cos_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order);
void log_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order);
void sqrt_taylor_only_remainder(Interval & result, const Interval & remainder, std::list<Interval>::iterator & iterRange, const int order);

}

extern flowstar::ParseSetting parseSetting;
extern flowstar::ParseResult parseResult;

#endif /* TAYLORMODEL_H_ */
