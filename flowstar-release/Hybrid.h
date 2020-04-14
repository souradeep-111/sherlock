/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.

  The code is released as is under the GNU General Public License (GPL).
---*/

#ifndef HYBRID_H_
#define HYBRID_H_

#include "Continuous.h"

extern flowstar::ParseSetting parseSetting;
extern flowstar::ParseResult parseResult;

namespace flowstar
{

class HybridSystem;

class ResetMap
{
public:
	TaylorModelVec tmvReset;
public:
	ResetMap();
	ResetMap(const TaylorModelVec & tmv);
	ResetMap(const ResetMap & reset);
	~ResetMap();

	void reset(TaylorModelVec & result, const TaylorModelVec & tmv, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const;

	ResetMap & operator = (const ResetMap & reset);
};

class DiscTrans
{
public:
	int jumpID;
	int startID;
	int targetID;
	std::vector<PolynomialConstraint> guard;
	ResetMap resetMap;
public:
	DiscTrans();
	DiscTrans(const int id, const int start, const int target, const std::vector<PolynomialConstraint> & lcs, const ResetMap & reset);
	DiscTrans(const DiscTrans & trans);
	~DiscTrans();

	DiscTrans & operator = (const DiscTrans & trans);
};

class TreeNode
{
public:
	int jumpID;
	int modeID;
	Interval localTime;
	TreeNode *parent;
	std::list<TreeNode *> children;

	TreeNode(const int jump, const int mode, const Interval & t);
	TreeNode(const TreeNode & node);
	~TreeNode();

	void dump(FILE *fp, const std::string & prefix, const std::vector<std::string> & modeNames) const;

	TreeNode & operator = (const TreeNode & node);
};


class ReachabilitySetting
{
public:
	double step;
	int precondition;
	int orderType;
	bool bAdaptiveSteps;
	bool bAdaptiveOrders;
	std::vector<Interval> estimation;
	double miniStep;
	std::vector<int> orders;
	std::vector<int> maxOrders;
	int globalMaxOrder;
	Interval cutoff_threshold;

public:
	ReachabilitySetting();
	~ReachabilitySetting();
	ReachabilitySetting(const ReachabilitySetting & setting);
	void clear();

	ReachabilitySetting & operator = (const ReachabilitySetting & setting);
};


class LTI_Dynamics
{
protected:
	iMatrix im_dyn_A;
	iMatrix im_dyn_B;

	bMatrix connectivity;
	bool bAuto;

public:
	LTI_Dynamics();
	LTI_Dynamics(const iMatrix & dyn_A, const iMatrix & dyn_B);
	LTI_Dynamics(const LTI_Dynamics & lti_dyn);
	~LTI_Dynamics();

	LTI_Dynamics & operator = (const LTI_Dynamics & lti_dyn);

	friend class HybridSystem;
};


class LTV_Dynamics
{
protected:
	upMatrix up_dyn_A;
	upMatrix up_dyn_B;
	upMatrix up_tv_part;

	bMatrix connectivity;
	bool bAuto;

public:
	LTV_Dynamics();
	LTV_Dynamics(const upMatrix & dyn_A, const upMatrix & dyn_B);
	LTV_Dynamics(const upMatrix & dyn_A, const upMatrix & dyn_B, const upMatrix & tv_part);
	LTV_Dynamics(const LTV_Dynamics & ltv_dyn);
	~LTV_Dynamics();

	LTV_Dynamics & operator = (const LTV_Dynamics & ltv_dyn);

	friend class HybridSystem;
};


class HybridSystem
{
private:
	std::vector<int> modes;
	std::vector<int> dyn_class;			// class of the dynamics

	std::vector<LTI_Dynamics> lti_dynamics;
	std::vector<LTV_Dynamics> ltv_dynamics;

	std::vector<TaylorModelVec> odes;
	std::vector<std::vector<HornerForm> > hfOdes;
	std::vector<std::vector<std::string> > strOdes;
	std::vector<TaylorModelVec> odes_centered;

	std::vector<std::vector<HornerForm> > hfOdes_centered;
	std::vector<std::vector<std::string> > strOdes_centered;

	std::vector<std::vector<bool> > constant;
	std::vector<std::vector<Interval> > strOde_constant;

	std::vector<std::vector<PolynomialConstraint> > invariants;
	std::vector<std::vector<DiscTrans> > transitions;

	int initialMode;
	Flowpipe initialSet;

public:
	HybridSystem();
	HybridSystem(const HybridSystem & hybsys);
	~HybridSystem();


	// efficient flowpipe construction method for linear continuous dynamics
	int reach_continuous_lti(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
			upMatrix & up_Phi_0, TaylorModelVec & tmv_Phi, std::vector<iMatrix> & Phi_exp_table,
			iMatrix & im_Psi, iMatrix & im_global_Psi, TaylorModelVec & tmv_Psi, std::vector<TaylorModelVec> & tmv_Psi_table,
			const int mode, const TaylorModelVec & init_set, const std::vector<Interval> & init_domain,
			const double step, const double time, const int order, const bool bPrint, std::vector<bool> & invariant_boundary_intersected,
			const std::vector<std::string> & modeNames, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump);


	int reach_continuous_ltv(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
			const int mode, const TaylorModelVec & init_set, const std::vector<Interval> & init_domain,
			const double step, const double time, const int order, const bool bPrint, std::vector<bool> & invariant_boundary_intersected,
			const std::vector<std::string> & modeNames, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump);


	// only use Picard operation
	// fixed step sizes and orders
	int reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive step sizes and fixed orders
	int reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double miniStep, const double time, const int order, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder,
			const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive orders and fixed step sizes
	int reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected,
			const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
			const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;


	// for low-degree ODEs
	// fixed step sizes and orders
	int reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive step sizes and fixed orders
	int reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double miniStep, const double time, const int order, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder,
			const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive orders and fixed step sizes
	int reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected,
			const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
			const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;


	// for high-degree ODEs
	// fixed step sizes and orders
	int reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive step sizes and fixed orders
	int reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double miniStep, const double time, const int order, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder,
			const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive orders and fixed step sizes
	int reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected,
			const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
			const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;


	// for non-polynomial ODEs (using Taylor approximations)
	// fixed step sizes and orders
	int reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
			const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
			const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive step sizes and fixed orders
	int reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
			const int mode, const Flowpipe & initFp, const double step, const double miniStep, const double time, const int order, const int precondition,
			const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
			const int mode, const Flowpipe & initFp, const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder,
			const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	// adaptive orders and fixed step sizes
	int reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
			const int mode, const Flowpipe & initFp, const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
			const bool bPrint, const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected,
			const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;

	int reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
			std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
			const int mode, const Flowpipe & initFp, const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
			const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
			std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
			const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const;


	// hybrid reachability
	int reach_hybrid(std::list<std::list<TaylorModelVec> > & flowpipes, std::list<std::list<std::vector<Interval> > > & domains,
			std::list<std::list<int> > & flowpipes_safety, std::list<std::list<bool> > & flowpipes_contracted, long & num_of_flowpipes,
			std::list<int> & modeIDs, std::list<TreeNode *> & traceNodes, TreeNode * & traceTree, const std::vector<int> & integrationSchemes, const double time, const int maxJmps,
			const ReachabilitySetting & global_setting, const std::vector<ReachabilitySetting> & local_settings, const std::vector<std::vector<int> > & aggregType,
			const std::vector<std::vector<std::vector<RowVector> > > aggregationTemplate_candidates, const std::vector<RowVector> default_aggregation_template,
			const std::vector<std::vector<Matrix> > & weightTab, const std::vector<std::vector<std::vector<bool> > > & linear_auto,
			const std::vector<std::vector<std::vector<RowVector> > > & template_auto, const bool bPrint, const std::vector<std::string> & stateVarNames,
			const std::vector<std::string> & modeNames, const std::vector<std::string> & tmVarNames,
			const std::vector<std::vector<PolynomialConstraint> > & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump);


	HybridSystem & operator = (const HybridSystem & hybsys);

	friend class HybridReachability;
};




class HybridReachability
{
public:
	HybridSystem system;			// the hybrid system
	double time;					// the time horizon for the reachability analysis
	std::vector<int> outputAxes;	// the output axes
	int plotSetting;
	int plotFormat;
	int numSections;			// the number of sections in each dimension

	int maxJumps;
	int numOfJumps;

	bool bPrint;
	bool bSafetyChecking;
	bool bPlot;
	bool bDump;

	std::vector<int> integrationSchemes;

	ReachabilitySetting global_setting;
	std::vector<ReachabilitySetting> local_settings;

	TreeNode *traceTree;

	std::vector<bool> bVecUnderCheck;

	std::vector<std::vector<int> > aggregationType;
	std::vector<RowVector> default_aggregation_template;
	std::vector<std::vector<std::vector<RowVector> > > aggregationTemplate_candidates;


	// information for template selection
	std::vector<std::vector<std::vector<bool> > > linear_auto;
	std::vector<std::vector<std::vector<RowVector> > > template_auto;


	std::vector<std::vector<Matrix> > weightTab;

//	std::list<std::list<TaylorModelVec> > flowpipesCompo;
//	std::list<std::list<std::vector<Interval> > > domains;


	std::list<std::list<TaylorModelVec> > flowpipes;
	std::list<std::list<std::vector<Interval> > > domains;
	std::list<std::list<int> > flowpipes_safety;
	std::list<std::list<bool> > flowpipes_contracted;


	long num_of_flowpipes;


	std::list<int> modeIDs;
	std::list<TreeNode *> traceNodes;

	std::map<std::string,int> stateVarTab;
	std::vector<std::string> stateVarNames;

	std::map<std::string,int> tmVarTab;
	std::vector<std::string> tmVarNames;

	std::map<std::string,int> modeTab;
	std::vector<std::string> modeNames;

	std::map<std::string,int> parTab;
	std::vector<std::string> parNames;
	std::vector<Interval> parRanges;

	std::vector<std::vector<PolynomialConstraint> > unsafeSet;

	char outputFileName[NAME_SIZE];

public:
	HybridReachability();
	~HybridReachability();

	void dump(FILE *fp) const;

	int run();
	void prepareForPlotting();
	void prepareForDumping();

	void plot_2D(const bool bProjected) const;

	void plot_2D_GNUPLOT(FILE *fp, const bool bProjected) const;
	void plot_2D_interval_GNUPLOT(FILE *fp, const bool bProjected) const;
	void plot_2D_octagon_GNUPLOT(FILE *fp, const bool bProjected) const;
	void plot_2D_grid_GNUPLOT(FILE *fp, const bool bProjected) const;

	void plot_2D_MATLAB(FILE *fp, const bool bProjected) const;
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

	bool declareMode(const std::string & mName, const LTI_Dynamics & lti_dyn, const std::vector<PolynomialConstraint> & inv, const ReachabilitySetting & local_setting);
	bool declareMode(const std::string & mName, const LTV_Dynamics & ltv_dyn, const std::vector<PolynomialConstraint> & inv, const ReachabilitySetting & local_setting);
	bool declareMode(const std::string & mName, const TaylorModelVec & ode, const std::vector<PolynomialConstraint> & inv, const int integrationScheme, const ReachabilitySetting & local_setting);
	bool declareMode(const std::string & mName, const std::vector<std::string> & strOde, const std::vector<PolynomialConstraint> & inv, const int integrationScheme, const ReachabilitySetting & local_setting);

	int getIDForMode(const std::string & mName) const;
	bool getModeName(std::string & mName, const int id) const;

	void declareTrans(const int start, const int end, const std::vector<PolynomialConstraint> & guard, const ResetMap & reset, const int aggregType, const std::vector<std::vector<double> > & candidates);
	void declareTrans();

	void initialConfig(const int modeID, const Flowpipe & initialSet);
	void set_default_template();
	void constructWeightTab();

	int safetyChecking();
	long numOfFlowpipes() const;
	void dump_counterexample(FILE *fp, const std::list<TaylorModelVec> & flowpipes, const std::list<std::vector<Interval> > & domains, TreeNode * const node, const std::list<Interval> & globalTimes) const;

	// parallelotopic aggregation
	friend void aggregate_flowpipes_by_Parallelotope(TaylorModelVec & tmvAggregation, std::vector<Interval> & doAggregation, const std::vector<TaylorModelVec> & flowpipes,
			const std::vector<std::vector<Interval> > & domains, const std::vector<PolynomialConstraint> & invariant, const DiscTrans & jump, std::vector<bool> & boundary_intersected,
			const std::vector<RowVector> & template_candidates, const std::vector<RowVector> & template_default, const std::vector<std::vector<Matrix> > & weightTab,
			const std::vector<std::vector<std::vector<bool> > > & linear_auto, const std::vector<std::vector<std::vector<RowVector> > > & template_auto, const int globalMaxOrder, const int rangeDim);
};

class FactorTab
{
public:
	int index;
	Interval factor;
	Interval intercept;
public:
	FactorTab();
	FactorTab(const int index_input, const Interval & factor_input, const Interval & intercept_input);
	~FactorTab();

	friend bool compareFactor(const FactorTab & a, const FactorTab & b);
	friend bool compareIntercept(const FactorTab & a, const FactorTab & b);
};

//int safetyChecking(const TaylorModelVec & flowpipe, const std::vector<Interval> & step_exp_table, const std::vector<PolynomialConstraint> & unsafeSet, const int order, const Interval & cutoff_threshold);
int safetyChecking2(const TaylorModelVec & flowpipe, const std::vector<Interval> & domain, const std::vector<PolynomialConstraint> & unsafeSet, const int order, const Interval & cutoff_threshold);

void generateNodeSeq(std::list<TreeNode *> & result, TreeNode *root);

// interval aggregation
void aggregate_flowpipes_by_interval(TaylorModelVec & tmvAggregation, std::vector<Interval> & doAggregation, const std::vector<TaylorModelVec> & flowpipes, const std::vector<std::vector<Interval> > & domains);

bool vector_selection(FactorTab & lst_selected, std::list<FactorTab> & lst_unselected, Matrix & matTemplate, const std::vector<RowVector> & rowVecs, int & rank);
bool check_validity(Matrix & matTemplate, const RowVector & rowVec, const int rank);

void aggregate_flowpipes_by_Parallelotope(TaylorModelVec & tmvAggregation, std::vector<Interval> & doAggregation, const std::vector<TaylorModelVec> & flowpipes,
		const std::vector<std::vector<Interval> > & domains, const std::vector<PolynomialConstraint> & invariant, const DiscTrans & jump, std::vector<bool> & boundary_intersected,
		const std::vector<RowVector> & template_candidates, const std::vector<RowVector> & template_default, const Matrix & weightTab,
		const std::vector<bool> & linear_auto, const std::vector<RowVector> & template_auto, const int globalMaxOrder, const int rangeDim);

}


#endif /* HYBRID_H_ */
