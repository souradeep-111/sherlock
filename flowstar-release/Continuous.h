/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.

  The code is released as is under the GNU General Public License (GPL).
---*/

#ifndef CONTINUOUS_H_
#define CONTINUOUS_H_

#include "TaylorModel.h"
#include "Geometry.h"
#include "expression.h"
//#include "polyreg.h"

namespace flowstar
{

extern std::vector<std::string> domainVarNames;

class Continuous_Reachability_Setting;

class Flowpipe					// A flowpipe is represented by a composition of two Taylor models. The left Taylor model is the preconditioning part.
{
public:
	TaylorModelVec tmvPre;
	TaylorModelVec tmv;

	std::vector<Interval> domain;	// domain of TMV_right, the first variable is t

public:
	Flowpipe();
	Flowpipe(const TaylorModelVec & tmvPre_input, const TaylorModelVec & tmv_input, const std::vector<Interval> & domain_input);
	Flowpipe(const std::vector<Interval> & box, const Interval & I);								// represent a box
	Flowpipe(const TaylorModelVec & tmv_input, const std::vector<Interval> & domain_input);		// construct a flowpipe from a Taylor model
	Flowpipe(const Flowpipe & flowpipe);
	~Flowpipe();

	void clear();
	void dump(FILE *fp, const std::vector<std::string> & stateVarNames, const std::vector<std::string> & tmVarNames, const Interval & cutoff_threshold) const;
	void dump_normal(FILE *fp, const std::vector<std::string> & stateVarNames, const std::vector<std::string> & tmVarNames, std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold) const;

	void composition(TaylorModelVec & result, const Interval & cutoff_threshold) const;
	void composition(TaylorModelVec & result, const int order, const Interval & cutoff_threshold) const;
	void composition(TaylorModelVec & result, const std::vector<int> & orders, const Interval & cutoff_threshold) const;
	void composition(TaylorModelVec & result, const std::vector<int> & outputAxes, const int order, const Interval & cutoff_threshold) const;

	void composition_normal(TaylorModelVec & result, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold) const;
	void composition_normal(TaylorModelVec & result, const std::vector<Interval> & step_exp_table, const std::vector<int> & order, const Interval & cutoff_threshold) const;

	void composition_normal(TaylorModelVec & result, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold) const;
	void composition_normal(TaylorModelVec & result, const std::vector<int> & outputAxes, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold) const;

	void intEval(std::vector<Interval> & result, const Interval & cutoff_threshold) const;
	void intEvalNormal(std::vector<Interval> & result, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold) const;

	void normalize();

	int safetyChecking(const std::vector<Interval> & step_exp_table, const std::vector<PolynomialConstraint> & unsafeSet, const int order, const Interval & cutoff_threshold) const;
	int safetyChecking(const std::vector<Interval> & step_exp_table, const std::vector<PolynomialConstraint> & unsafeSet, const std::vector<int> & orders, const int maxOrder, const Interval & cutoff_threshold) const;

	// Taylor model integration by only using Picard operation
	// fixed step sizes and orders
	int advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;
	int advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;

	// adaptive step sizes and fixed orders
	int advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;
	int advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const double step, const double miniStep, const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;

	// adaptive orders and fixed step sizes
	int advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			int & order, const int maxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;
	int advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			std::vector<int> & orders, const int localMaxOrder, const std::vector<int> & maxOrders, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;


	// fast integration scheme for low-degree ODEs
	// fixed step sizes and orders
	int advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;
	int advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const std::vector<int> & orders, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;

	// adaptive step sizes and fixed orders
	int advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;
	int advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const double step, const double miniStep, const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;

	// adaptive orders and fixed step sizes
	int advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			int & order, const int maxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;
	int advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			std::vector<int> & orders, const std::vector<int> & maxOrders, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;

	// integration scheme for high-degree ODEs
	// fixed step sizes and orders
	int advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;
	int advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;

	// adaptive step sizes and fixed orders
	int advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;
	int advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const double step, const double miniStep, const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;

	// adaptive orders and fixed step sizes
	int advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			int & order, const int maxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;
	int advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			std::vector<int> & orders, const int localMaxOrder, const std::vector<int> & maxOrders, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const;


	// integration scheme for non-polynomial ODEs (using Taylor approximations)
	// fixed step sizes and orders
	int advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;
	int advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;

	// adaptive step sizes and fixed orders
	int advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;
	int advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const double step, const double miniStep, const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;

	// adaptive orders and fixed step sizes
	int advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			int & order, const int maxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;
	int advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			std::vector<int> & orders, const int localMaxOrder, const std::vector<int> & maxOrders, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;

	// symbolic remainders
	// fixed orders and step sizes
	int advance_picard_symbolic_remainder(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const int order, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L, const std::vector<bool> & constant) const;

	// adaptive step sizes and fixed orders
	int advance_picard_symbolic_remainder(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L, const std::vector<bool> & constant) const;

	// adaptive orders and fixed step sizes
	int advance_picard_symbolic_remainder(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			int & order, const int maxOrder, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L, const std::vector<bool> & constant) const;

	// non-polynomial ODES
	// fixed orders and step sizes
	int advance_non_polynomial_taylor_symbolic_remainder(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L,
			const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;

	// adaptive step sizes and fixed orders
	int advance_non_polynomial_taylor_symbolic_remainder(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L,
			const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;

	// adaptive orders and fixed step sizes
	int advance_non_polynomial_taylor_symbolic_remainder(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			int & order, const int maxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L,
			const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;

	Flowpipe & operator = (const Flowpipe & flowpipe);


	// ======================= flowpipe construction for AST ODEs =======================

	int advance_picard(Flowpipe & result, const std::vector<Expression_AST> & ode, const std::vector<Expression_AST> & ode_centered, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const int order, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;

	int advance_picard_symbolic_remainder(Flowpipe & result, const std::vector<Expression_AST> & ode, const std::vector<Expression_AST> & ode_centered, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
			const int order, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L,
			const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const;

	// ======================= flowpipe construction using polynomial regression =======================
/*
	int advance_regression_picard(Flowpipe & result, const DynamicSys & system, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table, vector<vector<int> > & table_of_monomials,
			const int order, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, std::vector<bool> & constant, std::vector<Interval> & constant_part) const;
*/

	// ==================================================================================

	void center(std::vector<double> & center_point) const;
	double max(const std::vector<double> & l) const;

	friend class LinearFlowpipe;
	friend class ContinuousSystem;
	friend class ContinuousReachability;
	friend class HybridSystem;
	friend class HybridReachability;
};




// flowpipes for LTI and LTV systems

class LinearFlowpipe
{
protected:
	iMatrix init_Phi;
	iMatrix init_Psi;
	iMatrix init_Omega;

	upMatrix trans_Phi;
	upMatrix trans_Psi;
	upMatrix trans_Omega;

	Zonotope tv_remainder;

public:
	LinearFlowpipe();
	LinearFlowpipe(const LinearFlowpipe & flowpipe);
	~LinearFlowpipe();

	int safetyChecking(const Flowpipe & X0, const std::vector<Interval> & polyRangeX0, const std::vector<Interval> & step_exp_table, const bool bVarying, const bool bAuto,
			const std::vector<iMatrix> & im_precond_trans_Phi, const std::vector<iMatrix> & im_precond_trans_Psi, const std::vector<iMatrix> & im_precond_trans_Omega,
			const std::vector<iMatrix> & constraints, const iMatrix & TIPar_range, const iMatrix & rangeX0, const std::vector<PolynomialConstraint> & unsafeSet,
			const std::vector<Interval> & checking_domain, const std::vector<Interval> & ti_domain, const std::vector<Interval> & extended_domain,
			const int order, const Interval & cutoff_threshold);

//	void intEval(iMatrix & range, const std::vector<Interval> & t_exp_table, const iMatrix & X0, const iMatrix & ti_range);

//	void intEvalNormal(Interval & result, const iMatrix & constraint, const Flowpipe & X0, const int numTIPar,
//			const std::vector<Interval> & step_exp_table, const std::vector<Interval> & polyRangeX0, const Interval & cutoff_threshold);

	void intEval(Interval & result, const iMatrix & constraint, const bool bVarying, const bool bAuto, const iMatrix & im_precond_trans_Phi, const iMatrix & im_precond_trans_Psi, const iMatrix & im_precond_trans_Omega, const std::vector<Interval> & step_exp_table, const iMatrix & TIPar_range, const iMatrix & rangeX0);
	void intEval(std::vector<Interval> & result, const bool bVarying, const bool bAuto, const iMatrix & im_trans_Phi, const iMatrix & im_trans_Psi, const iMatrix & im_trans_Omega, const std::vector<Interval> & step_exp_table, const iMatrix & TIPar_range, const iMatrix & rangeX0);

	void tmEval(HornerForm & Obj, Interval & obj_rem, HornerForm & hf_TIPart, Interval & int_TVPart, const bool bAuto, const iMatrix & constraint, const Flowpipe & X0, const std::vector<Interval> & checking_domain, const std::vector<Interval> & ti_domain, const std::vector<Interval> & polyRangeX0, const Interval & cutoff_threshold);

	void toTaylorModel(TaylorModelVec & flowpipe, const bool bAuto, const Flowpipe & X0, const std::vector<Interval> & checking_domain, const int numTIPar, const std::vector<Interval> & polyRangeX0, const Interval & cutoff_threshold);
	void toTaylorModel(TaylorModelVec & flowpipe, const bool bAuto, const std::vector<int> & outputAxes, const Flowpipe & X0, const std::vector<Interval> & checking_domain, const int numTIPar, const std::vector<Interval> & polyRangeX0, const Interval & cutoff_threshold);
	void toTaylorModel(TaylorModelVec & flowpipe, const bool bAuto);

//	void toTaylorModel(TaylorModelVec & flowpipe, const std::vector<int> & outputAxes, const Flowpipe & X0, const int numTIPar, const std::vector<Interval> & step_exp_table, const std::vector<Interval> & polyRangeX0, const Interval & cutoff_threshold);

	LinearFlowpipe & operator = (const LinearFlowpipe & flowpipe);

	friend class ContinuousSystem;
	friend class ContinuousReachability;
	friend class HybridSystem;
	friend class HybridReachability;
	friend class LTI_ODE;
};

class ContinuousSystem
{
public:
	TaylorModelVec tmvOde;
	TaylorModelVec tmvOde_centered;
	std::vector<HornerForm> hfOde;				// a Horner form of the ode
	std::vector<HornerForm> hfOde_centered;
	std::vector<std::string> strOde;
	std::vector<std::string> strOde_centered;

	std::vector<bool> constant;					// whether the derivative is constant
	std::vector<Interval> strOde_constant;

	std::vector<Flowpipe> initialSets;						// the initial set

	// dx/dt = A(t) x + B(t) + C(t) u
	bool bAuto;

	iMatrix im_dyn_A;
	iMatrix im_dyn_B;
	iMatrix im_dyn_ti;
	iMatrix im_dyn_tv;

	upMatrix up_dyn_A;
	upMatrix up_dyn_B;
	upMatrix up_dyn_ti;
	upMatrix up_dyn_tv;

	iMatrix im_tv_range;

	bMatrix connectivity;

public:
	ContinuousSystem();
	ContinuousSystem(const iMatrix & A_input, const iMatrix & B_input, const iMatrix & ti_input, const iMatrix & tv_input, const std::vector<Flowpipe> & initialSets_input);
	ContinuousSystem(const upMatrix & A_input, const upMatrix & B_input, const upMatrix & ti_input, const upMatrix & tv_input, const std::vector<Flowpipe> & initialSets_input);
	ContinuousSystem(const TaylorModelVec & ode_input, const std::vector<Flowpipe> & initialSets_input);
	ContinuousSystem(const std::vector<std::string> & strOde_input, const std::vector<Flowpipe> & initialSets_input);
	ContinuousSystem(const ContinuousSystem & system);
	~ContinuousSystem();

	// efficient integration method for linear ODEs
	// since we can always compute a safe remainder, adaptive techniques are not needed
	// since the size of a Taylor series is linear w.r.t. the number of state variables, we may simply use uniform orders
	int reach_lti(std::list<LinearFlowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const bool bPrint, const Interval & cutoff_threshold, const int order_cutoff,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump);

	int reach_ltv(std::list<LinearFlowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int maxNumSteps, const bool bPrint, const Interval & cutoff_threshold, const int order_cutoff,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump);

	// only use Picard operation
	// fixed step sizes and orders
	int reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive step sizes and fixed orders
	int reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double miniStep, const double time, const int order, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive orders and fixed step sizes
	int reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
			const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;


	// for low-degree ODEs
	// fixed step sizes and orders
	int reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive step sizes and fixed orders
	int reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double miniStep, const double time, const int order, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive orders and fixed step sizes
	int reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
			const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// for high-degree ODEs
	// fixed step sizes and orders
	int reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive step sizes and fixed orders
	int reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double miniStep, const double time, const int order, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive orders and fixed step sizes
	int reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
			const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// for non-polynomial ODEs (using Taylor approximations)
	// fixed step sizes and orders
	int reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive step sizes and fixed orders
	int reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double miniStep, const double time, const int order, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive orders and fixed step sizes
	int reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// symbolic remainders
	// fixed orders and step sizes
	int reach_picard_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive step sizes and fixed orders
	int reach_picard_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double miniStep, const double time, const int order, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive orders and fixed step sizes
	int reach_picard_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int maxOrder, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;


	// solving non-polynomial ODEs with symbolic remainders
	// fixed orders and step sizes
	int reach_non_polynomial_taylor_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive step sizes and fixed orders
	int reach_non_polynomial_taylor_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double miniStep, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive orders and fixed step sizes
	int reach_non_polynomial_taylor_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
			const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	ContinuousSystem & operator = (const ContinuousSystem & system);

	friend class ContinuousReachability;
};


class ContinuousReachability			// The reachability analysis of continuous systems
{

public:
	ContinuousSystem system;			// the continuous system
	double step;						// the step size used in the reachability analysis
	double time;						// the time horizon for the reachability analysis
	int precondition;					// the preconditioning technique
	std::vector<int> outputAxes;		// the output axes
	int plotSetting;
	int plotFormat;
	int numSections;					// the number of sections in each dimension

	int orderType;
	bool bAdaptiveSteps;
	bool bAdaptiveOrders;

	std::vector<Interval> estimation;	// the remainder estimation for varying time step
	double miniStep;					// the minimum step size
	std::vector<int> orders;			// the order(s)
	std::vector<int> maxOrders;			// the maximum orders
	int globalMaxOrder;

	bool bPrint;
	bool bSafetyChecking;
	bool bPlot;
	bool bDump;

	int integrationScheme;

	Interval cutoff_threshold;

	std::list<Flowpipe> flowpipes;
	std::list<int> flowpipes_safety;

	std::list<TaylorModelVec> flowpipesCompo;
	std::list<std::vector<Interval> > domains;

	std::list<bool> flowpipes_contracted;

	std::list<LinearFlowpipe> linearFlowpipes;

	std::vector<PolynomialConstraint> unsafeSet;

	std::map<std::string,int> stateVarTab;
	std::vector<std::string> stateVarNames;

	std::map<std::string,int> tmVarTab;
	std::vector<std::string> tmVarNames;

	std::map<std::string,int> parTab;
	std::vector<std::string> parNames;
	std::vector<Interval> parRanges;

	std::map<std::string,int> TI_Par_Tab;
	std::vector<std::string> TI_Par_Names;

	std::map<std::string,int> TV_Par_Tab;
	std::vector<std::string> TV_Par_Names;

	int maxNumSteps;
	int max_remainder_queue;

	long num_of_flowpipes;

	char outputFileName[NAME_SIZE];

public:
	ContinuousReachability();
	~ContinuousReachability();

	void dump(FILE *fp) const;

	int run();
	void prepareForPlotting();
	void prepareForDumping();

//	void composition();

	int safetyChecking();

	long numOfFlowpipes() const;

	void dump_counterexample(FILE *fp, const std::list<TaylorModelVec> & flowpipes, const std::list<std::vector<Interval> > & domains) const;
	void dump_counterexample(FILE *fp, const std::vector<std::list<TaylorModelVec> > & flowpipes, const std::vector<Flowpipe> & initialSets, const std::vector<std::string> & initialVarNames) const;

	void plot_2D(const bool bProjected);

	void plot_2D_GNUPLOT(FILE *fp, const bool bProjected) const;
	void plot_2D_interval_GNUPLOT(FILE *fp, const bool bProjected) const;
	void plot_2D_octagon_GNUPLOT(FILE *fp, const bool bProjected) const;
	void plot_2D_grid_GNUPLOT(FILE *fp, const bool bProjected) const;

	void plot_2D_MATLAB(FILE *fp, const bool bProjected);
	void plot_2D_interval_MATLAB(FILE *fp, const bool bProjected) const;
	void plot_2D_octagon_MATLAB(FILE *fp, const bool bProjected) const;
	void plot_2D_grid_MATLAB(FILE *fp, const bool bProjected) const;

	bool declareStateVar(const std::string & vName);
	int getIDForStateVar(const std::string & vName) const;
	bool getStateVarName(std::string & vName, const int id) const;

	bool declareTMVar(const std::string & vName);
	int getIDForTMVar(const std::string & vName) const;
	bool getTMVarName(std::string & vName, const int id) const;

	bool declarePar(const std::string & pName, const Interval & range);
	int getIDForPar(const std::string & pName) const;
	bool getParName(std::string & pName, const int id) const;
	bool getRangeForPar(Interval & range, const std::string & pName) const;

	bool declareTIPar(const std::string & pName);
	int getIDForTIPar(const std::string & pName) const;

	bool declareTVPar(const std::string & pName);
	int getIDForTVPar(const std::string & pName) const;
};



class SDE_reachset
{
protected:
	iMatrix2 Phi;
	iMatrix2 Psi;
	iMatrix2 Omega;

public:
	SDE_reachset();
	SDE_reachset(const iMatrix2 & Phi_input, const iMatrix2 & Psi_input, const iMatrix2 & Omega_input);
	SDE_reachset(const SDE_reachset & reachset);
	~SDE_reachset();

	void toDistribution(iMatrix & mean, iMatrix & covar, const iMatrix & initial_mean, const iMatrix & initial_covar, const iMatrix & control_input) const;
	void toDistribution(iMatrix2 & mean, iMatrix2 & covar, const iMatrix2 & initial_mean, const iMatrix2 & initial_covar, const iMatrix2 & control_input) const;
	void output(FILE *fp) const;

	SDE_reachset & operator = (const SDE_reachset & reachset);

	friend class LTV_SDE;
};



class LTV_SDE
{
protected:
	upMatrix A_t;
	upMatrix B_t;
	upMatrix C_t;

public:
	LTV_SDE(const upMatrix & A_t_input, const upMatrix & B_t_input, const upMatrix & C_t_input);
	~LTV_SDE();

	void reach(SDE_reachset & result, const double step, const int N, const int order) const;

	// The function computes the reachable set in the time interval [delta1*N + delta2*i , delta1*N + delta2*(i+1)] for i=0,...,M-1
	void reach(std::vector<SDE_reachset> & result, const double delta1, const int N, const double delta2, const int M, const int order) const;
};



class LTI_Flowpipe
{
protected:
	iMatrix init_Phi;
	iMatrix init_Psi;

	upMatrix trans_Phi;
	upMatrix trans_Psi;

public:
	LTI_Flowpipe();
	LTI_Flowpipe(const LTI_Flowpipe & flowpipe);
	~LTI_Flowpipe();

	void intEval(std::vector<Interval> & result, const std::vector<int> & varIDs, const std::vector<Interval> & X0, const Continuous_Reachability_Setting & crs);

	LTI_Flowpipe & operator = (const LTI_Flowpipe & flowpipe);

	friend class LTI_ODE;
};


class LTI_ODE
{
protected:
	iMatrix A;
	iMatrix B;
/*
	iMatrix C;
	iMatrix constant;
*/

	bool bAuto;

	bMatrix connectivity;

//	iMatrix im_tv_range;

public:
	LTI_ODE(iMatrix & A_input, iMatrix & B_input);
//	LTI_ODE(iMatrix & A_input, iMatrix & B_input, iMatrix & C_input, iMatrix & constant_input, const std::vector<Interval> & dist_range_input);
	~LTI_ODE();

//	void one_step_trans(iMatrix & Phi, iMatrix & Psi, iMatrix & trans_constant, Zonotope & dist, const double step, const int order);
	void reach(std::list<LTI_Flowpipe> & flowpipes, iMatrix & Phi_end, std::vector<std::vector<iMatrix> > & Psi_end, std::vector<iMatrix> & uncertainties, const Continuous_Reachability_Setting & crs, const int N);

	friend void hybrid_LTI_reach(std::list<LTI_Flowpipe> & flowpipes, std::vector<LTI_ODE> & odes, const std::vector<int> & steps, Continuous_Reachability_Setting & crs);
};




class Continuous_Reachability_Setting
{
public:
	double step;						// step-size
	double time;						// time horizon
	int order;							// Taylor model order

	std::vector<Interval> estimation;	// remainder estimation for varying time step

	bool bPrint;
	Interval cutoff_threshold;

	std::vector<Interval> step_exp_table;
	std::vector<Interval> step_end_exp_table;

public:
	Continuous_Reachability_Setting();
	~Continuous_Reachability_Setting();
	Continuous_Reachability_Setting(const Continuous_Reachability_Setting & rs);

	Continuous_Reachability_Setting & operator = (const Continuous_Reachability_Setting & rs);

	bool setTime(const double t);
	bool setFixedStepsize(const double delta);
	bool setFixedOrder(const int k);
	bool setRemainderEstimation(const std::vector<Interval> & intervals);
	bool setCutoff(const Interval & cutoff);
	bool setPrecision(const int prec);
	void printon();
	void printoff();

	void prepareForReachability();	// call this function after all parameters are properly set
};


class Symbolic_Remainder
{
public:
	std::vector<iMatrix> J;
	std::vector<iMatrix> Phi_L;
	std::vector<Interval> scalars;
	std::vector<Polynomial> initial_set_poly;

public:
	Symbolic_Remainder(const Flowpipe & initial_set);
	Symbolic_Remainder(const Symbolic_Remainder & symb_rem);
	~Symbolic_Remainder();

	Symbolic_Remainder & operator = (const Symbolic_Remainder & symb_rem);
};


class Polynomial_ODE
{
protected:
	Variables variables;
	std::vector<Polynomial> derivatives;
	std::vector<HornerForm> derivatives_hf;
	std::vector<HornerForm> derivatives_hf_centered;
	std::vector<Interval> remainders;
	std::vector<bool> constant;

public:
	Polynomial_ODE(const Variables & vars);
	Polynomial_ODE(const Polynomial_ODE & ode);
	~Polynomial_ODE();

	Polynomial_ODE & operator = (const Polynomial_ODE & ode);

	bool assignDerivative(const std::string & varName, const Polynomial & polynomial);
//	bool assignDerivative(const std::string & varName, const TaylorModel & tm);

	void output(FILE *fp) const;

	void doubleEval(std::vector<double> & derivative, const std::vector<double> & state) const;

	bool reach_symbolic_remainder(std::list<Flowpipe> & flowpipes, Flowpipe & fp_last, Symbolic_Remainder & symb_rem, Continuous_Reachability_Setting & crs, const Flowpipe & initial_set, const int N, const int M) const;
};

class ODE
{
protected:
	Variables variables;
	std::vector<Expression_AST> derivatives;

	std::vector<Expression_AST> derivative_centers;
	std::vector<bool> constant;
	std::vector<Interval> constant_part;

public:
	ODE(const Variables & vars);
	ODE(const ODE & ode);
	~ODE();

	ODE & operator = (const ODE & ode);

	bool assignDerivative(const std::string & varName, const Expression_AST & derivative);

	void output(FILE *fp, const Taylor_Model_Computation_Setting & setting) const;

	bool reach_interval_remainder(std::list<Flowpipe> & flowpipes, Flowpipe & fp_last, Continuous_Reachability_Setting & crs, const Flowpipe & initial_set, const int N) const;
	bool reach_symbolic_remainder(std::list<Flowpipe> & flowpipes, Flowpipe & fp_last, Symbolic_Remainder & symb_rem, Continuous_Reachability_Setting & crs, const Flowpipe & initial_set, const int N, const int M) const;

//	bool reach_interval_remainder_using_regression(std::list<Flowpipe> & flowpipes, Flowpipe & fp_last, Continuous_Reachability_Setting & crs, const Flowpipe & initial_set, const int N) const;
};


// bool reach_interval_remainder_using_regression(std::list<Flowpipe> & flowpipes, Flowpipe & fp_last, DynamicSys & system, Continuous_Reachability_Setting & crs, const Flowpipe & initial_set, const int N);



void hybrid_LTI_reach(std::list<LTI_Flowpipe> & flowpipes, std::vector<LTI_ODE> & odes, const std::vector<int> & steps, Continuous_Reachability_Setting & crs);

void plot_2D_interval_MATLAB(FILE *fp, const std::string & x, const std::string & y, const Variables & vars, const std::list<Flowpipe> & flowpipes);
void plot_2D_interval_MATLAB(FILE *fp, const int x, const int y, std::list<LTI_Flowpipe> & flowpipes, const std::vector<Interval> & X0, const Continuous_Reachability_Setting & crs);


void computeTaylorExpansion(TaylorModelVec & result, const TaylorModelVec & first_order_deriv, const TaylorModelVec & ode, const int order, const Interval & cutoff_threshold);
void computeTaylorExpansion(TaylorModelVec & result, const TaylorModelVec & first_order_deriv, const TaylorModelVec & ode, const std::vector<int> & orders, const Interval & cutoff_threshold);

void construct_step_exp_table(std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table, const double step, const int order);
void construct_step_exp_table(std::vector<Interval> & step_exp_table, const Interval & step, const int order);

void preconditionQR(Matrix & result, const TaylorModelVec & tmv, const int rangeDim, const int domainDim);

Interval rho(const TaylorModelVec & tmv, const std::vector<Interval> & l, const std::vector<Interval> & domain);
Interval rhoNormal(const TaylorModelVec & tmv, const std::vector<Interval> & l, const std::vector<Interval> & step_end_exp_table);

Interval rho(const TaylorModelVec & tmv, const RowVector & l, const std::vector<Interval> & domain);
Interval rhoNormal(const TaylorModelVec & tmv, const RowVector & l, const std::vector<Interval> & step_end_exp_table);

void templatePolyhedron(Polyhedron & result, const TaylorModelVec & tmv, const std::vector<Interval> & domain);
void templatePolyhedronNormal(Polyhedron & result, const TaylorModelVec & tmv, std::vector<Interval> & step_end_exp_table);


void check_connectivities(bMatrix & result, bMatrix & adjMatrix);

// domain contraction by using interval arithmetic
int contract_interval_arithmetic(TaylorModelVec & flowpipe, std::vector<Interval> & domain, const std::vector<PolynomialConstraint> & pcs, std::vector<bool> & boundary_intersected, const Interval & cutoff_threshold);
// int contract_interval_arithmetic(TaylorModelVec & flowpipe, std::vector<Interval> & domain, const Polyhedron & inv, std::vector<bool> & boundary_intersected);

int contract_remainder(const std::vector<Interval> & polyRange, std::vector<Interval> & remainders, const std::vector<HornerForm> & hfs, const std::vector<Interval> & b);
int contract_remainder(const std::vector<Interval> & polyRange, std::vector<Interval> & remainders, const std::vector<PolynomialConstraint> & constraints);

void gridBox(std::list<std::vector<Interval> > & grids, const std::vector<Interval> & box, const int num);

void compute_int_mat_pow(std::vector<iMatrix> & result, const iMatrix & A, const int order);
void compute_int_mat2_pow(std::vector<iMatrix2> & result, const iMatrix2 & A, const int order);

void compute_one_step_trans(upMatrix & p_Phi_t, upMatrix & p_Psi_t, upMatrix & p_Omega_t,
		iMatrix & Phi_step_trunc, iMatrix & Phi_step_end_trunc, iMatrix & Phi_rem,
		iMatrix & Psi_step_trunc, iMatrix & Psi_step_end_trunc, iMatrix & Psi_rem,
		iMatrix & Omega_step_trunc, iMatrix & Omega_step_end_trunc, iMatrix & Omega_rem, iMatrix & tv_part,
		const upMatrix & A_t, const upMatrix & B_t, const upMatrix & ti_t, const upMatrix & tv_t,
		bMatrix & connectivity, const bool bAuto, const UnivariatePolynomial & up_t, const int order,
		const std::vector<Interval> & step_exp_table, const std::vector<Interval> & step_end_exp_table);

void compute_one_step_trans_4hybrid(upMatrix & p_Phi_t, upMatrix & p_Psi_t,
		iMatrix & Phi_step_trunc, iMatrix & Phi_step_end_trunc, iMatrix & Phi_rem,
		iMatrix & Psi_step_trunc, iMatrix & Psi_step_end_trunc, iMatrix & Psi_rem, iMatrix & tv_part,
		const upMatrix & A_t, const upMatrix & B_t, const upMatrix & tv_t,
		bMatrix & connectivity, const bool bAuto, const UnivariatePolynomial & up_t, const int order,
		const std::vector<Interval> & step_exp_table, const std::vector<Interval> & step_end_exp_table);



void compute_one_step_trans_LTV_SDE(iMatrix2 & Phi_delta, iMatrix2 & Psi_delta, iMatrix2 & Omega_delta,
		const upMatrix & A_t, const upMatrix & B_t, const upMatrix & C_t,
		const std::vector<Interval> step_exp_table, const std::vector<Interval> step_end_exp_table,
		const UnivariatePolynomial & up_t_t0, const int order);

void compute_one_step_trans_LTV_SDE(iMatrix2 & Phi_delta, iMatrix2 & Psi_delta, iMatrix2 & Omega_delta,
		iMatrix2 & Phi_0_delta, iMatrix2 & Psi_0_delta, iMatrix2 & Omega_0_delta,
		const upMatrix & A_t, const upMatrix & B_t, const upMatrix & C_t,
		const std::vector<Interval> step_exp_table, const std::vector<Interval> step_end_exp_table,
		const UnivariatePolynomial & up_t_t0, const int order);

int safetyChecking2(const TaylorModelVec & flowpipe, const std::vector<Interval> & domain, const std::vector<PolynomialConstraint> & unsafeSet, const int order, const Interval & cutoff_threshold);

void plot_2D_box_gnuplot(FILE *fp, const std::string & epsFileName, std::list<Flowpipe> & flowpipes, const std::string & x, const std::string & y, const Variables & variables, const Interval & cutoff);

}


#endif /* CONTINUOUS_H_ */
