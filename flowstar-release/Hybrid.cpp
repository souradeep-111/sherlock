/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.

  The code is released as is under the GNU General Public License (GPL).
---*/

#include "Hybrid.h"

using namespace flowstar;

// class ResetMap

ResetMap::ResetMap()
{
}

ResetMap::ResetMap(const TaylorModelVec & tmv)
{
	tmvReset = tmv;
}

ResetMap::ResetMap(const ResetMap & reset)
{
	tmvReset = reset.tmvReset;
}

ResetMap::~ResetMap()
{
}

void ResetMap::reset(TaylorModelVec & result, const TaylorModelVec & tmv, const std::vector<Interval> & domain, const int order, const Interval & cutoff_threshold) const
{
	std::vector<Interval> tmvPolyRange;
	tmv.polyRange(tmvPolyRange, domain);
	tmvReset.insert_ctrunc(result, tmv, tmvPolyRange, domain, order, cutoff_threshold);
}

ResetMap & ResetMap::operator = (const ResetMap & reset)
{
	if(this == &reset)
		return *this;

	tmvReset = reset.tmvReset;
	return *this;
}























// class DiscTrans

DiscTrans::DiscTrans()
{
}

DiscTrans::DiscTrans(const int id, const int start, const int target, const std::vector<PolynomialConstraint> & lcs, const ResetMap & reset)
{
	jumpID = id;
	startID = start;
	targetID = target;
	guard = lcs;
	resetMap = reset;
}

DiscTrans::DiscTrans(const DiscTrans & trans)
{
	jumpID = trans.jumpID;
	startID = trans.startID;
	targetID = trans.targetID;
	guard = trans.guard;
	resetMap = trans.resetMap;
}

DiscTrans::~DiscTrans()
{
	guard.clear();
}

DiscTrans & DiscTrans::operator = (const DiscTrans & trans)
{
	if(this == &trans)
		return *this;

	jumpID = trans.jumpID;
	startID = trans.startID;
	targetID = trans.targetID;
	guard = trans.guard;
	resetMap = trans.resetMap;
	return *this;
}


























// computation tree

TreeNode::TreeNode(const int jump, const int mode, const Interval & t)
{
	jumpID = jump;
	modeID = mode;
	localTime = t;
	parent = NULL;
}

TreeNode::TreeNode(const TreeNode & node)
{
	jumpID = node.jumpID;
	modeID = node.modeID;
	localTime = node.localTime;
	parent = node.parent;
	children = node.children;
}

TreeNode::~TreeNode()
{
	std::list<TreeNode *>::iterator iter = children.begin();

	for(; iter!=children.end(); ++iter)
	{
		delete *iter;
	}

	children.clear();
}

void TreeNode::dump(FILE *fp, const std::string & prefix, const std::vector<std::string> & modeNames) const
{
	char buffer[NAME_SIZE];

	if(jumpID == 0)
	{
		sprintf(buffer, "%s", modeNames[modeID].c_str());
	}
	else
	{
		std::string strTime;
		localTime.toString(strTime);

		sprintf(buffer, " ( %d , %s ) -> %s", jumpID, strTime.c_str(), modeNames[modeID].c_str());
	}

	std::string strTemp(buffer);
	std::string strPath = prefix + strTemp;

	if(children.size() == 0)
	{
		fprintf(fp, "%s;\n\n", strPath.c_str());
	}
	else
	{
		std::list<TreeNode *>::const_iterator iter = children.begin();

		for(; iter!=children.end(); ++iter)
		{
			(*iter)->dump(fp, strPath, modeNames);
		}
	}
}

TreeNode & TreeNode::operator = (const TreeNode & node)
{
	if(this == &node)
		return *this;

	jumpID = node.jumpID;
	modeID = node.modeID;
	localTime = node.localTime;
	parent = node.parent;
	children = node.children;
	return *this;
}

































// class HybridSystem

HybridSystem::HybridSystem()
{
}

HybridSystem::HybridSystem(const HybridSystem & hybsys)
{
	modes				= hybsys.modes;
	odes				= hybsys.odes;
	hfOdes				= hybsys.hfOdes;
	strOdes				= hybsys.strOdes;
	odes_centered		= hybsys.odes_centered;
	hfOdes_centered		= hybsys.hfOdes_centered;
	strOdes_centered	= hybsys.strOdes_centered;
	invariants			= hybsys.invariants;
	transitions			= hybsys.transitions;
	initialMode			= hybsys.initialMode;
	initialSet			= hybsys.initialSet;
	dyn_class			= hybsys.dyn_class;
	lti_dynamics		= hybsys.lti_dynamics;
	ltv_dynamics		= hybsys.ltv_dynamics;
	constant			= hybsys.constant;
	strOde_constant		= hybsys.strOde_constant;
}

HybridSystem::~HybridSystem()
{
	modes.clear();
	odes.clear();
	hfOdes.clear();
	strOdes.clear();
	odes_centered.clear();
	hfOdes_centered.clear();
	strOdes_centered.clear();
	invariants.clear();
	transitions.clear();
	dyn_class.clear();
	lti_dynamics.clear();
	ltv_dynamics.clear();
	constant.clear();
	strOde_constant.clear();
}

HybridSystem & HybridSystem::operator = (const HybridSystem & hybsys)
{
	if(this == &hybsys)
		return *this;

	modes				= hybsys.modes;
	odes				= hybsys.odes;
	hfOdes				= hybsys.hfOdes;
	strOdes				= hybsys.strOdes;
	odes_centered		= hybsys.odes_centered;
	hfOdes_centered		= hybsys.hfOdes_centered;
	strOdes_centered	= hybsys.strOdes_centered;
	invariants			= hybsys.invariants;
	transitions			= hybsys.transitions;
	initialMode			= hybsys.initialMode;
	initialSet			= hybsys.initialSet;
	dyn_class			= hybsys.dyn_class;
	lti_dynamics		= hybsys.lti_dynamics;
	ltv_dynamics		= hybsys.ltv_dynamics;
	constant			= hybsys.constant;
	strOde_constant		= hybsys.strOde_constant;

	return *this;
}


// For LTI ODEs, the local flowmap overapproximations can be reused.

int HybridSystem::reach_continuous_lti(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
		upMatrix & up_Phi_0, TaylorModelVec & tmv_Phi, std::vector<iMatrix> & Phi_exp_table,
		iMatrix & im_Psi, iMatrix & im_global_Psi, TaylorModelVec & tmv_Psi, std::vector<TaylorModelVec> & tmv_Psi_table,
		const int mode, const TaylorModelVec & init_set, const std::vector<Interval> & init_domain,
		const double step, const double time, const int order, const bool bPrint, std::vector<bool> & invariant_boundary_intersected,
		const std::vector<std::string> & modeNames, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump)
{
	Real rStep(step);
	Interval intStep(0, step), intOne(1), intUnit(-1,1), intZero;

	std::vector<Interval> step_exp_table, step_end_exp_table;
	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1)+1);

	int rangeDim = lti_dynamics[mode].im_dyn_A.rows();
	int domainDim = init_domain.size();
	bool bAuto = lti_dynamics[mode].bAuto;


	TaylorModelVec flowpipe_Phi;
	TaylorModelVec firstFlowpipe;
	std::vector<Interval> domain = init_domain;
	domain[0] = step_exp_table[1];

	if(Phi_exp_table.size() == 0)		// it is the first time to compute the flowpipes for this mode
	{

		// identity matrix
		iMatrix im_identity(rangeDim);

		// 2. Compute the first flowpipe
		// compute A^n for 1 <= n <= k
		std::vector<iMatrix> A_exp_table;
		compute_int_mat_pow(A_exp_table, lti_dynamics[mode].im_dyn_A, order + 1);

		// compute the expansion for exp(At)
		upMatrix expansion_exp_A_t_k = im_identity;

		for(int i=1; i<=order; ++i)
		{
			upMatrix A_t_i = A_exp_table[i];
			A_t_i.times_x(i);
			A_t_i *= factorial_rec[i];

			expansion_exp_A_t_k += A_t_i;
		}

		up_Phi_0 = expansion_exp_A_t_k;

		// compute a remainder for exp(A*delta)
		Real factor_k_plus_1;
		factorial_rec[order+1].sup(factor_k_plus_1);

		Real step_pow_k_plus_1(step);
		step_pow_k_plus_1.pow_assign_RNDU(order + 1);

		factor_k_plus_1.mul_assign_RNDU(step_pow_k_plus_1);

		Real bound_exp_A_delta;
		lti_dynamics[mode].im_dyn_A.max_norm(bound_exp_A_delta);
		bound_exp_A_delta.mul_assign_RNDU(rStep);
		bound_exp_A_delta.exp_assign_RNDU();

		factor_k_plus_1.mul_assign_RNDU(bound_exp_A_delta);

		Interval intErr;
		factor_k_plus_1.to_sym_int(intErr);

		iMatrix im_rem(rangeDim, rangeDim);
		for(int i=0; i<rangeDim; ++i)
		{
			for(int j=0; j<rangeDim; ++j)
			{
				if(lti_dynamics[mode].connectivity[i][j])
				{
					im_rem[i][j] = intErr;
				}
			}
		}

		im_rem = A_exp_table[order+1] * im_rem;

		iMatrix im_Phi_0_rem(rangeDim, rangeDim);

		for(int i=0; i<rangeDim; ++i)
		{
			for(int j=0; j<rangeDim; ++j)
			{
				if(lti_dynamics[mode].connectivity[i][j])
				{
					im_Phi_0_rem[i][j] = im_rem[i][j];
				}
			}
		}

		// compute the first flowpipe
		tmv_Phi.clear();

		for(int i=0; i<rangeDim; ++i)
		{
			TaylorModel tmTemp;

			for(int j=0; j<rangeDim; ++j)
			{
				Polynomial polyTemp(up_Phi_0[i][j], domainDim);
				polyTemp.mul_assign(j+1, 1);

				tmTemp.expansion += polyTemp;
			}

			tmTemp.remainder = im_Phi_0_rem[i][0];
			tmv_Phi.tms.push_back(tmTemp);
		}

		up_Phi_0 += im_Phi_0_rem;
		iMatrix im_Phi;
		up_Phi_0.intEval(im_Phi, step_end_exp_table);

		upMatrix up_Psi_0;

		if(!bAuto)
		{
			up_Psi_0 = up_Phi_0 * lti_dynamics[mode].im_dyn_B;
			up_Psi_0.integral();

			iMatrix im_trunc_step, im_trunc_step_end;
			up_Psi_0.ctrunc(im_trunc_step, im_trunc_step_end, order, step_exp_table, step_end_exp_table);

			up_Psi_0.intEval(im_Psi, step_end_exp_table);
			im_Psi += im_trunc_step_end;
			up_Psi_0 += im_trunc_step;
		}

		std::vector<Interval> initPolyRange;
		init_set.polyRangeNormal(initPolyRange, step_exp_table);

		tmv_Phi.insert_ctrunc_normal(flowpipe_Phi, init_set, initPolyRange, step_exp_table, domainDim, order, cutoff_threshold);
		flowpipe_Phi.cutoff_normal(step_exp_table, cutoff_threshold);

		if(!bAuto)
		{
			tmv_Psi.clear();

			for(int i=0; i<rangeDim; ++i)
			{
				Polynomial polyTemp(up_Psi_0[i][0], domainDim);
				TaylorModel tmTemp(polyTemp);
				tmv_Psi.tms.push_back(tmTemp);
			}

			tmv_Psi.cutoff_normal(step_exp_table, cutoff_threshold);

			flowpipe_Phi.add(firstFlowpipe, tmv_Psi);
			tmv_Psi_table.push_back(tmv_Psi);
		}

		Phi_exp_table.push_back(im_identity);
		Phi_exp_table.push_back(im_Phi);
	}
	else
	{
		std::vector<Interval> initPolyRange;
		init_set.polyRangeNormal(initPolyRange, step_exp_table);

		tmv_Phi.insert_ctrunc_normal(flowpipe_Phi, init_set, initPolyRange, step_exp_table, domainDim, order, cutoff_threshold);

		flowpipe_Phi.cutoff_normal(step_exp_table, cutoff_threshold);

		if(!bAuto)
		{
			flowpipe_Phi.add(firstFlowpipe, tmv_Psi);
		}
	}


	// intersect the invariant
	std::vector<bool> local_boundary_intersected;
	int type = contract_interval_arithmetic(firstFlowpipe, domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

	if(type >= 0)
	{
		// collect the intersected invariant boundary
		if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
		{
			invariant_boundary_intersected = local_boundary_intersected;
		}
		else
		{
			for(int i=0; i<local_boundary_intersected.size(); ++i)
			{
				if(local_boundary_intersected[i])
				{
					invariant_boundary_intersected[i] = true;
				}
			}
		}
	}

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	switch(type)
	{
	case -1:	// invariant violated
		return checking_result;
	case 0:		// flowpipe is entirely contained in the invariant
	{
		++num_of_flowpipes;
		flowpipes_contracted.push_back(bContracted);

		if(bSafetyChecking)
		{
			int safety = safetyChecking2(firstFlowpipe, domain, unsafeSet, order, cutoff_threshold);

			if(safety == SAFE)
			{
				flowpipes.push_back(firstFlowpipe);
				domains.push_back(domain);
				flowpipes_safety.push_back(SAFE);
			}
			else if(safety == UNSAFE && !bContracted)
			{
				flowpipes.push_back(firstFlowpipe);
				domains.push_back(domain);
				flowpipes_safety.push_back(UNSAFE);

				return COMPLETED_UNSAFE;
			}
			else
			{
				flowpipes.push_back(firstFlowpipe);
				domains.push_back(domain);
				flowpipes_safety.push_back(UNKNOWN);

				if(checking_result == COMPLETED_SAFE)
				{
					checking_result = COMPLETED_UNKNOWN;
				}
			}
		}
		else
		{
			flowpipes.push_back(firstFlowpipe);
			domains.push_back(domain);
			flowpipes_safety.push_back(SAFE);
		}
		break;
	}
	case 1:			// domain is contracted but not the time interval
	{
		++num_of_flowpipes;
		bContracted = true;
		flowpipes_contracted.push_back(bContracted);

		if(bSafetyChecking)
		{
			int safety = safetyChecking2(firstFlowpipe, domain, unsafeSet, order, cutoff_threshold);

			if(safety == SAFE)
			{
				flowpipes.push_back(firstFlowpipe);
				domains.push_back(domain);
				flowpipes_safety.push_back(SAFE);
			}
			else
			{
				flowpipes.push_back(firstFlowpipe);
				domains.push_back(domain);
				flowpipes_safety.push_back(UNKNOWN);

				if(checking_result == COMPLETED_SAFE)
				{
					checking_result = COMPLETED_UNKNOWN;
				}
			}
		}
		else
		{
			flowpipes.push_back(firstFlowpipe);
			domains.push_back(domain);
			flowpipes_safety.push_back(SAFE);
		}
		break;
	}
	case 2: 	// time interval is contracted
	{
		if(domain[0] > intZero)
		{
			return checking_result;
		}
		else
		{
			++num_of_flowpipes;
			bContracted = true;

			flowpipes_contracted.push_back(bContracted);

			if(bSafetyChecking)
			{
				int safety = safetyChecking2(firstFlowpipe, domain, unsafeSet, order, cutoff_threshold);

				if(safety == SAFE)
				{
					flowpipes.push_back(firstFlowpipe);
					domains.push_back(domain);
					flowpipes_safety.push_back(SAFE);
				}
				else
				{
					flowpipes.push_back(firstFlowpipe);
					domains.push_back(domain);
					flowpipes_safety.push_back(UNKNOWN);

					if(checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
			}
			else
			{
				flowpipes.push_back(firstFlowpipe);
				domains.push_back(domain);
				flowpipes_safety.push_back(SAFE);
			}

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", domain[0].sup());
				printf("step = %f,\t", domain[0].sup());
				printf("order = %d\n", order);
			}

			return checking_result;
		}
	}
	}

	if(bPrint)
	{
		printf("mode: %s,\t", modeNames[mode].c_str());
		printf("time = %f,\t", domain[0].sup());
		printf("step = %f,\t", domain[0].sup());
		printf("order = %d\n", order);
	}


	iMatrix last_remainder(rangeDim, 1);
	for(int i=0; i<rangeDim; ++i)
	{
		last_remainder[i][0] = firstFlowpipe.tms[i].remainder;
	}

	int N = (int)ceil(time/step);

	iMatrix im_step_rem(rangeDim, 1);
	for(int i=0; i<rangeDim; ++i)
	{
		Interval I = im_Psi[i][0];
		I.remove_midpoint();
		im_step_rem[i][0] = I;
	}

	double t = step;

	for(int i=1; i<N; ++i)
	{
		if(i >= Phi_exp_table.size())
		{
			if(i%2 == 0)
			{
				int j = i/2;
				iMatrix im_temp = Phi_exp_table[j] * Phi_exp_table[j];
				Phi_exp_table.push_back(im_temp);
			}
			else
			{
				int j = i/2;
				iMatrix im_temp = Phi_exp_table[j] * Phi_exp_table[j+1];
				Phi_exp_table.push_back(im_temp);
			}
		}

		TaylorModelVec xi = Phi_exp_table[i] * flowpipe_Phi;

		if(!bAuto)
		{
			if(i >= tmv_Psi_table.size())
			{
				im_global_Psi += Phi_exp_table[i-1] * im_Psi;

				upMatrix up_global_Psi = up_Phi_0 * im_global_Psi;

				TaylorModelVec tmv_global_Psi;
				for(int j=0; j<rangeDim; ++j)
				{
					Polynomial polyTemp(up_global_Psi[j][0], domainDim);
					TaylorModel tmTemp(polyTemp);
					tmv_global_Psi.tms.push_back(tmTemp);
				}

				tmv_global_Psi.cutoff_normal(step_exp_table, cutoff_threshold);

				xi.add_assign(tmv_global_Psi);
				xi.add_assign(tmv_Psi);

				tmv_Psi_table.push_back(tmv_global_Psi);
			}
			else
			{
				xi.add_assign(tmv_Psi_table[i]);
				xi.add_assign(tmv_Psi);
			}
		}

		if(bContracted)
		{
			// refine the remainder
			iMatrix refinement = Phi_exp_table[1] * last_remainder + im_step_rem;

			for(int j=0; j<rangeDim; ++j)
			{
				xi.tms[j].remainder.intersect_assign(refinement[j][0]);
			}
		}

		std::vector<bool> local_boundary_intersected;
		int type = contract_interval_arithmetic(xi, domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

		if(type >= 0)
		{
			// collect the intersected invariant boundary
			if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
			{
				invariant_boundary_intersected = local_boundary_intersected;
			}
			else
			{
				for(int j=0; j<local_boundary_intersected.size(); ++j)
				{
					if(local_boundary_intersected[j])
					{
						invariant_boundary_intersected[j] = true;
					}
				}
			}
		}

		switch(type)
		{
		case -1:	// invariant violated
			return checking_result;
		case 0:		// flowpipe is entirely contained in the invariant
		{
			++num_of_flowpipes;
			flowpipes_contracted.push_back(bContracted);

			if(bSafetyChecking)
			{
				int safety = safetyChecking2(xi, domain, unsafeSet, order, cutoff_threshold);

				if(safety == SAFE)
				{
					flowpipes.push_back(xi);
					domains.push_back(domain);
					flowpipes_safety.push_back(SAFE);
				}
				else if(safety == UNSAFE && !bContracted)
				{
					flowpipes.push_back(xi);
					domains.push_back(domain);
					flowpipes_safety.push_back(UNSAFE);

					return COMPLETED_UNSAFE;
				}
				else
				{
					flowpipes.push_back(xi);
					domains.push_back(domain);
					flowpipes_safety.push_back(UNKNOWN);

					if(checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
			}
			else
			{
				flowpipes.push_back(xi);
				domains.push_back(domain);
				flowpipes_safety.push_back(SAFE);
			}

			break;
		}
		case 1:			// domain is contracted but not the time interval
		{
			++num_of_flowpipes;
			bContracted = true;
			flowpipes_contracted.push_back(bContracted);

			if(bSafetyChecking)
			{
				int safety = safetyChecking2(xi, domain, unsafeSet, order, cutoff_threshold);

				if(safety == SAFE)
				{
					flowpipes.push_back(xi);
					domains.push_back(domain);
					flowpipes_safety.push_back(SAFE);
				}
				else
				{
					flowpipes.push_back(xi);
					domains.push_back(domain);
					flowpipes_safety.push_back(UNKNOWN);

					if(checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
			}
			else
			{
				flowpipes.push_back(xi);
				domains.push_back(domain);
				flowpipes_safety.push_back(SAFE);
			}

			break;
		}
		case 2: 	// time interval is contracted
		{
			if(domain[0] > intZero)
			{
				return checking_result;
			}
			else
			{
				++num_of_flowpipes;
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(xi, domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(xi);
						domains.push_back(domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(xi);
						domains.push_back(domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(xi);
					domains.push_back(domain);
					flowpipes_safety.push_back(SAFE);
				}

				if(bPrint)
				{
					printf("mode: %s,\t", modeNames[mode].c_str());
					printf("time = %f,\t", t + domain[0].sup());
					printf("step = %f,\t", domain[0].sup());
					printf("order = %d\n", order);
				}

				return checking_result;
			}
		}
		}

		if(bContracted)
		{
			for(int j=0; j<rangeDim; ++j)
			{
				last_remainder[j][0] = xi.tms[j].remainder;
			}
		}

		if(bPrint)
		{
			t += step;
			printf("mode: %s,\t", modeNames[mode].c_str());
			printf("time = %f,\t", t);
			printf("step = %f,\t", step);
			printf("order = %d\n", order);
		}
	}

	return checking_result;
}



int HybridSystem::reach_continuous_ltv(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
		const int mode, const TaylorModelVec & init_set, const std::vector<Interval> & init_domain,
		const double step, const double time, const int order, const bool bPrint, std::vector<bool> & invariant_boundary_intersected,
		const std::vector<std::string> & modeNames, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump)
{
	const int rangeDim = ltv_dynamics[mode].up_dyn_A.rows();
	const int domainDim = init_domain.size();
	const int numTVPar = ltv_dynamics[mode].up_tv_part.cols();

	Interval intZero, intOne(1), intStep(0, step);
	Real rStep(step);

	int maxOrder = ltv_dynamics[mode].up_dyn_A.degree();
	int maxOrder_B = ltv_dynamics[mode].up_dyn_B.degree();
	bool bAuto = ltv_dynamics[mode].bAuto;


	if(maxOrder < maxOrder_B)
	{
		maxOrder = maxOrder_B;
	}

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	std::vector<Interval> step_exp_table, step_end_exp_table;
	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1+maxOrder)+1);

//	Zonotope global_tv_remainder(rangeDim);

	std::vector<Interval> domain = init_domain;
	domain[0] = step_exp_table[1];

	int N = (int)ceil(time/step);
	iMatrix last_remainder(rangeDim, 1);

	double t = 0;

	iMatrix2 global_Phi(rangeDim), global_Psi(rangeDim, 1);


	// compute the remaining flowpipes
	for(int i=0; i<N; ++i)
	{
		TaylorModelVec flowpipe;

		Interval int_t0(i*step);

		std::vector<Interval> t0_coefficients;
		t0_coefficients.push_back(int_t0);
		t0_coefficients.push_back(intOne);
		UnivariatePolynomial up_t0(t0_coefficients);

		iMatrix Phi_step_trunc;
		iMatrix Phi_step_end_trunc;
		iMatrix Phi_rem;

		iMatrix Psi_step_trunc;
		iMatrix Psi_step_end_trunc;
		iMatrix Psi_rem;

		iMatrix tv_part;

		upMatrix trans_Phi, trans_Psi;

		compute_one_step_trans_4hybrid(trans_Phi, trans_Psi,
				Phi_step_trunc, Phi_step_end_trunc, Phi_rem,
				Psi_step_trunc, Psi_step_end_trunc, Psi_rem, tv_part,
				ltv_dynamics[mode].up_dyn_A, ltv_dynamics[mode].up_dyn_B, ltv_dynamics[mode].up_tv_part,
				ltv_dynamics[mode].connectivity, bAuto, up_t0, order,
				step_exp_table, step_end_exp_table);

		trans_Phi += Phi_rem;

		if(!bAuto)
		{
			trans_Psi += Psi_rem;
		}

		upMatrix bloated_trans_Phi = trans_Phi + Phi_step_trunc;
		upMatrix up_Phi = bloated_trans_Phi * global_Phi;

		TaylorModelVec tmv_Phi;

		for(int i1=0; i1<rangeDim; ++i1)
		{
			TaylorModel tmTemp;

			for(int j1=0; j1<rangeDim; ++j1)
			{
				Polynomial polyTemp(up_Phi[i1][j1], domainDim);
				polyTemp.mul_assign(j1+1, 1);

				tmTemp.expansion += polyTemp;
			}

			tmv_Phi.tms.push_back(tmTemp);
		}

		std::vector<Interval> initPolyRange;
		init_set.polyRangeNormal(initPolyRange, step_exp_table);

		tmv_Phi.insert_ctrunc_normal(flowpipe, init_set, initPolyRange, step_exp_table, domainDim, order, cutoff_threshold);
		flowpipe.cutoff_normal(step_exp_table, cutoff_threshold);

		if(!bAuto)
		{
			TaylorModelVec tmv_Psi;

			upMatrix up_Psi = bloated_trans_Phi * global_Psi + trans_Psi + Psi_step_trunc;

			for(int i1=0; i1<rangeDim; ++i1)
			{
				Polynomial polyTemp(up_Psi[i1][0], domainDim);
				TaylorModel tmTemp(polyTemp);
				tmv_Psi.tms.push_back(tmTemp);
			}

			tmv_Psi.cutoff_normal(step_exp_table, cutoff_threshold);

			flowpipe.add_assign(tmv_Psi);
		}

		iMatrix2 Phi_step_end;
		trans_Phi.intEval(Phi_step_end, step_end_exp_table);
		Phi_step_end += Phi_step_end_trunc;
		global_Phi = Phi_step_end * global_Phi;

		iMatrix im_step_rem;

		if(!bAuto)
		{
			iMatrix2 Psi_step_end;
			trans_Psi.intEval(Psi_step_end, step_end_exp_table);
			Psi_step_end += Psi_step_end_trunc;
			global_Psi = Phi_step_end * global_Psi + Psi_step_end;
			Psi_step_end.to_iMatrix(im_step_rem);

			for(int i1=0; i1<rangeDim; ++i1)
			{
				im_step_rem[i1][0].remove_midpoint();
			}
		}


		if(bContracted && i > 0)
		{
			// refine the remainder
			iMatrix refinement;

			if(bAuto)
			{
				refinement = Phi_step_end * last_remainder;
			}
			else
			{
				refinement = Phi_step_end * last_remainder +im_step_rem;
			}

			for(int i1=0; i1<rangeDim; ++i1)
			{
				flowpipe.tms[i1].remainder.intersect_assign(refinement[i1][0]);
			}
		}


		// intersect the invariant
		std::vector<bool> local_boundary_intersected;
		int type = contract_interval_arithmetic(flowpipe, domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

		if(type >= 0)
		{
			// collect the intersected invariant boundary
			if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
			{
				invariant_boundary_intersected = local_boundary_intersected;
			}
			else
			{
				for(int i=0; i<local_boundary_intersected.size(); ++i)
				{
					if(local_boundary_intersected[i])
					{
						invariant_boundary_intersected[i] = true;
					}
				}
			}
		}


		switch(type)
		{
		case -1:	// invariant violated
			return checking_result;
		case 0:		// flowpipe is entirely contained in the invariant
		{
			++num_of_flowpipes;
			flowpipes_contracted.push_back(bContracted);

			if(bSafetyChecking)
			{
				int safety = safetyChecking2(flowpipe, domain, unsafeSet, order, cutoff_threshold);

				if(safety == SAFE)
				{
					flowpipes.push_back(flowpipe);
					domains.push_back(domain);
					flowpipes_safety.push_back(SAFE);
				}
				else if(safety == UNSAFE && !bContracted)
				{
					flowpipes.push_back(flowpipe);
					domains.push_back(domain);
					flowpipes_safety.push_back(UNSAFE);

					return COMPLETED_UNSAFE;
				}
				else
				{
					flowpipes.push_back(flowpipe);
					domains.push_back(domain);
					flowpipes_safety.push_back(UNKNOWN);

					if(checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
			}
			else
			{
				flowpipes.push_back(flowpipe);
				domains.push_back(domain);
				flowpipes_safety.push_back(SAFE);
			}
			break;
		}
		case 1:			// domain is contracted but not the time interval
		{
			++num_of_flowpipes;
			bContracted = true;
			flowpipes_contracted.push_back(bContracted);

			if(bSafetyChecking)
			{
				int safety = safetyChecking2(flowpipe, domain, unsafeSet, order, cutoff_threshold);

				if(safety == SAFE)
				{
					flowpipes.push_back(flowpipe);
					domains.push_back(domain);
					flowpipes_safety.push_back(SAFE);
				}
				else
				{
					flowpipes.push_back(flowpipe);
					domains.push_back(domain);
					flowpipes_safety.push_back(UNKNOWN);

					if(checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
			}
			else
			{
				flowpipes.push_back(flowpipe);
				domains.push_back(domain);
				flowpipes_safety.push_back(SAFE);
			}
			break;
		}
		case 2: 	// time interval is contracted
		{
			if(domain[0] > intZero)
			{
				return checking_result;
			}
			else
			{
				++num_of_flowpipes;
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(flowpipe, domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(flowpipe);
						domains.push_back(domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(flowpipe);
						domains.push_back(domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(flowpipe);
					domains.push_back(domain);
					flowpipes_safety.push_back(SAFE);
				}

				if(bPrint)
				{
					printf("mode: %s,\t", modeNames[mode].c_str());
					printf("time = %f,\t", t + domain[0].sup());
					printf("step = %f,\t", domain[0].sup());
					printf("order = %d\n", order);
				}

				return checking_result;
			}
		}
		}

		if(bContracted)
		{
			for(int j=0; j<rangeDim; ++j)
			{
				last_remainder[j][0] = flowpipe.tms[j].remainder;
			}
		}

		if(bPrint)
		{
			t += step;
			printf("mode: %s,\t", modeNames[mode].c_str());
			printf("time = %f,\t", t);
			printf("step = %f,\t", domain[0].sup());
			printf("order = %d\n", order);
		}
	}

	return checking_result;
}





// only use Picard operation
// fixed step sizes and orders
int HybridSystem::reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_picard(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, order, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, order, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;

			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;
				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", order);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("order = %d\n", order);
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_picard(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, orders, globalMaxOrder, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, orders, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
				}

				printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

// adaptive step sizes and fixed orders
int HybridSystem::reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double miniStep, const double time, const int order, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	double newStep = 0;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_picard(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newStep, miniStep, order, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", order);
					}

					return checking_result;
				}
			}
			}

			t += step_exp_table[1].sup();

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step_exp_table[1].sup());
				printf("order = %d\n", order);
			}

			newStep = step_exp_table[1].sup() * LAMBDA_UP;
			if(newStep > step - THRESHOLD_HIGH)
			{
				newStep = 0;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder,
		const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	double newStep = 0;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_picard(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newStep, miniStep, orders, globalMaxOrder, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step_exp_table[1].sup();

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step_exp_table[1].sup());
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
				}

				printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
			}

			newStep = step_exp_table[1].sup() * LAMBDA_UP;
			if(newStep > step - THRESHOLD_HIGH)
			{
				newStep = 0;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

// adaptive orders and fixed step sizes
int HybridSystem::reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected,
		const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(maxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	int newOrder = order;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_picard(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newOrder, maxOrder, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, newOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, newOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, newOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", newOrder);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("order = %d\n", newOrder);
			}

			if(newOrder > order)
			{
				--newOrder;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_picard(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
		const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	std::vector<int> newOrders = orders;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int localMaxOrder = newOrders[0];
		for(int i=1; i<newOrders.size(); ++i)
		{
			if(localMaxOrder < newOrders[i])
			{
				localMaxOrder = newOrders[i];
			}
		}

		int res = currentFlowpipe.advance_picard(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newOrders, localMaxOrder, maxOrders, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, localMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, localMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, localMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), newOrders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), newOrders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), newOrders[i]);
				}

				printf("%s : %d\n", stateVarNames[num].c_str(), newOrders[num]);
			}

			for(int i=0; i<newOrders.size(); ++i)
			{
				if(newOrders[i] > orders[i])
				{
					--newOrders[i];
				}
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}









// for low-degree ODEs
// fixed step sizes and orders

int HybridSystem::reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	std::vector<Polynomial> polyODE;
	for(int i=0; i<odes_centered[mode].tms.size(); ++i)
	{
		polyODE.push_back(odes_centered[mode].tms[i].expansion);
	}

	std::vector<HornerForm> taylorExpansion;
	computeTaylorExpansion(taylorExpansion, polyODE, order);

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOdes[mode], taylorExpansion, precondition, step_exp_table, step_end_exp_table, order, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", order);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("order = %d\n", order);
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	std::vector<Polynomial> polyODE;
	for(int i=0; i<odes_centered[mode].tms.size(); ++i)
	{
		polyODE.push_back(odes_centered[mode].tms[i].expansion);
	}

	std::vector<HornerForm> taylorExpansion;
	computeTaylorExpansion(taylorExpansion, polyODE, orders);

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOdes[mode], taylorExpansion, precondition, step_exp_table, step_end_exp_table, orders, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
				}
				printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

// adaptive step sizes and fixed orders
int HybridSystem::reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double miniStep, const double time, const int order, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;


	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	double newStep = 0;

	std::vector<Polynomial> polyODE;
	for(int i=0; i<odes_centered[mode].tms.size(); ++i)
	{
		polyODE.push_back(odes_centered[mode].tms[i].expansion);
	}

	std::vector<HornerForm> taylorExpansion;
	computeTaylorExpansion(taylorExpansion, polyODE, order);

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOdes[mode], taylorExpansion, precondition, step_exp_table, step_end_exp_table, newStep, miniStep, order, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", order);
					}

					return checking_result;
				}
			}
			}

			t += step_exp_table[1].sup();

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step_exp_table[1].sup());
				printf("order = %d\n", order);
			}

			newStep = step_exp_table[1].sup() * LAMBDA_UP;
			if(newStep > step - THRESHOLD_HIGH)
			{
				newStep = 0;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder,
		const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	double newStep = 0;

	std::vector<Polynomial> polyODE;
	for(int i=0; i<odes_centered[mode].tms.size(); ++i)
	{
		polyODE.push_back(odes_centered[mode].tms[i].expansion);
	}

	std::vector<HornerForm> taylorExpansion;
	computeTaylorExpansion(taylorExpansion, polyODE, orders);

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOdes[mode], taylorExpansion, precondition, step_exp_table, step_end_exp_table, newStep, miniStep, orders, globalMaxOrder, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step_exp_table[1].sup();

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step_exp_table[1].sup());
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
				}

				printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
			}

			newStep = step_exp_table[1].sup() * LAMBDA_UP;
			if(newStep > step - THRESHOLD_HIGH)
			{
				newStep = 0;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}


// adaptive orders and fixed step sizes
int HybridSystem::reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected,
		const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(maxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	int newOrder = order;
	int localMaxOrder = order;

	std::vector<Polynomial> polyODE;
	for(int i=0; i<odes_centered[mode].tms.size(); ++i)
	{
		polyODE.push_back(odes_centered[mode].tms[i].expansion);
	}

	std::vector<HornerForm> taylorExpansion;
	computeTaylorExpansion(taylorExpansion, polyODE, order);

	std::vector<std::vector<HornerForm> > expansions;
	expansions.push_back(taylorExpansion);

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], expansions[newOrder-order], precondition, step_exp_table, step_end_exp_table, newOrder, maxOrder, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, newOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, newOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, newOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", newOrder);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("order = %d\n", newOrder);
			}

			if(newOrder > order)
			{
				if(newOrder > localMaxOrder)
				{
					for(int i=localMaxOrder+1; i<=newOrder; ++i)
					{
						std::vector<HornerForm> newTaylorExpansion;
						computeTaylorExpansion(newTaylorExpansion, polyODE, i);
						expansions.push_back(newTaylorExpansion);
					}

					localMaxOrder = newOrder;
				}

				--newOrder;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_low_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
		const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	std::vector<int> newOrders = orders;
	std::vector<int> localMaxOrders = orders;

	std::vector<Polynomial> polyODE;
	for(int i=0; i<odes_centered[mode].tms.size(); ++i)
	{
		polyODE.push_back(odes_centered[mode].tms[i].expansion);
	}

	std::vector<HornerForm> taylorExpansionHF;
	std::vector<Polynomial> taylorExpansionMF;
	std::vector<Polynomial> highestTerms;

	computeTaylorExpansion(taylorExpansionHF, taylorExpansionMF, highestTerms, polyODE, orders);

	std::vector<std::vector<HornerForm> > expansions;
	std::vector<HornerForm> emptySet;
	for(int i=0; i<taylorExpansionHF.size(); ++i)
	{
		expansions.push_back(emptySet);
		expansions[i].push_back(taylorExpansionHF[i]);
	}

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int localMaxOrder = newOrders[0];
		for(int i=1; i<newOrders.size(); ++i)
		{
			if(localMaxOrder < newOrders[i])
				localMaxOrder = newOrders[i];
		}

		int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], taylorExpansionHF, precondition, step_exp_table, step_end_exp_table, newOrders, maxOrders, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, localMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, localMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, localMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), newOrders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), newOrders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), newOrders[i]);
				}

				printf("%s : %d\n", stateVarNames[num].c_str(), newOrders[num]);
			}

			for(int i=0; i<newOrders.size(); ++i)
			{
				if(newOrders[i] > orders[i])
				{
					--newOrders[i];

					if(newOrders[i] > localMaxOrders[i])
					{
						for(int j=localMaxOrders[i]; j<newOrders[i]; ++j)
						{
							HornerForm newTaylorExpansionHF;
							Polynomial newTaylorExpansionMF;

							increaseExpansionOrder(newTaylorExpansionHF, newTaylorExpansionMF, highestTerms[i], taylorExpansionMF[i], polyODE, j);

							expansions[i].push_back(newTaylorExpansionHF);
							taylorExpansionMF[i] = newTaylorExpansionMF;
						}
					}

					localMaxOrders[i] = newOrders[i];

					taylorExpansionHF[i] = expansions[i][newOrders[i]-orders[i]];
				}
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}


// for high-degree ODEs
// fixed step sizes and orders
int HybridSystem::reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, order, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;
				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", order);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("order = %d\n", order);
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, orders, globalMaxOrder, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
				}

				printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}


// adaptive step sizes and fixed orders
int HybridSystem::reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double miniStep, const double time, const int order, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	double newStep = 0;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newStep, miniStep, order, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", order);
					}

					return checking_result;
				}
			}
			}

			t += step_exp_table[1].sup();

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step_exp_table[1].sup());
				printf("order = %d\n", order);
			}

			newStep = step_exp_table[1].sup() * LAMBDA_UP;
			if(newStep > step - THRESHOLD_HIGH)
			{
				newStep = 0;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder,
		const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	double newStep = 0;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newStep, miniStep, orders, globalMaxOrder, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step_exp_table[1].sup();

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step_exp_table[1].sup());
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
				}

				printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
			}

			newStep = step_exp_table[1].sup() * LAMBDA_UP;
			if(newStep > step - THRESHOLD_HIGH)
			{
				newStep = 0;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

// adaptive orders and fixed step sizes
int HybridSystem::reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected,
		const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(maxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	int newOrder = order;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newOrder, maxOrder, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, newOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, newOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, newOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", newOrder);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("order = %d\n", newOrder);
			}

			if(newOrder > order)
			{
				--newOrder;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_high_degree(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
		const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	std::vector<int> newOrders = orders;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int localMaxOrder = newOrders[0];
		for(int i=1; i<newOrders.size(); ++i)
		{
			if(localMaxOrder < newOrders[i])
				localMaxOrder = newOrders[i];
		}

		int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOdes[mode], hfOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newOrders, localMaxOrder, maxOrders, estimation, invariants[mode], cutoff_threshold, constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, localMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, localMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, localMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), newOrders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), newOrders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), newOrders[i]);
				}

				printf("%s : %d\n", stateVarNames[num].c_str(), newOrders[num]);
			}

			for(int i=0; i<newOrders.size(); ++i)
			{
				if(newOrders[i] > orders[i])
				{
					--newOrders[i];
				}
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}


// for non-polynomial ODEs (using Taylor approximations)
// fixed step sizes and orders
int HybridSystem::reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOdes[mode], strOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, order, estimation, invariants[mode], cutoff_threshold, constant[mode], strOde_constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;
				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", order);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("order = %d\n", order);
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes, const int mode, const Flowpipe & initFp,
		const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOdes[mode], strOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, orders, globalMaxOrder, estimation, invariants[mode], cutoff_threshold, constant[mode], strOde_constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
				}

				printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}


// adaptive step sizes and fixed orders
int HybridSystem::reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
		const int mode, const Flowpipe & initFp, const double step, const double miniStep, const double time, const int order, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	double newStep = 0;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOdes[mode], strOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newStep, miniStep, order, estimation, invariants[mode], cutoff_threshold, constant[mode], strOde_constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, order, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", order);
					}

					return checking_result;
				}
			}
			}

			t += step_exp_table[1].sup();

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step_exp_table[1].sup());
				printf("order = %d\n", order);
			}

			newStep = step_exp_table[1].sup() * LAMBDA_UP;
			if(newStep > step - THRESHOLD_HIGH)
			{
				newStep = 0;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
		const int mode, const Flowpipe & initFp, const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder,
		const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	double newStep = 0;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOdes[mode], strOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newStep, miniStep, orders, globalMaxOrder, estimation, invariants[mode], cutoff_threshold, constant[mode], strOde_constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, globalMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step_exp_table[1].sup();

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step_exp_table[1].sup());
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), orders[i]);
				}

				printf("%s : %d\n", stateVarNames[num].c_str(), orders[num]);
			}

			newStep = step_exp_table[1].sup() * LAMBDA_UP;
			if(newStep > step - THRESHOLD_HIGH)
			{
				newStep = 0;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}


// adaptive orders and fixed step sizes
int HybridSystem::reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
		const int mode, const Flowpipe & initFp, const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, std::vector<bool> & invariant_boundary_intersected,
		const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(maxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	int newOrder = order;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOdes[mode], strOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newOrder, maxOrder, estimation, invariants[mode], cutoff_threshold, constant[mode], strOde_constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, newOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, newOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, newOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("order = %d\n", newOrder);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("order = %d\n", newOrder);
			}

			if(newOrder > order)
			{
				--newOrder;
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}

int HybridSystem::reach_continuous_non_polynomial_taylor(std::list<TaylorModelVec> & flowpipes, std::list<std::vector<Interval> > & domains, std::list<int> & flowpipes_safety,
		std::list<bool> & flowpipes_contracted, long & num_of_flowpipes,
		const int mode, const Flowpipe & initFp, const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
		const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames,
		std::vector<bool> & invariant_boundary_intersected, const std::vector<std::string> & modeNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	Interval intZero;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(globalMaxOrder+1)+1);

	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();

	int checking_result = COMPLETED_SAFE;

	bool bContracted = false;

	Flowpipe newFlowpipe, currentFlowpipe = initFp;

	std::vector<int> newOrders = orders;

	for(double t=THRESHOLD_HIGH; t < time;)
	{
		int localMaxOrder = newOrders[0];
		for(int i=1; i<newOrders.size(); ++i)
		{
			if(localMaxOrder < newOrders[i])
				localMaxOrder = newOrders[i];
		}

		int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOdes[mode], strOdes_centered[mode], precondition, step_exp_table, step_end_exp_table, newOrders, localMaxOrder, maxOrders, estimation, invariants[mode], cutoff_threshold, constant[mode], strOde_constant[mode]);

		if(res == -1)
		{
			return checking_result;
		}
		else if(res == 1)
		{
			// over-approximate the flowpipe/invariant intersection
			TaylorModelVec tmvCompo;
			newFlowpipe.composition_normal(tmvCompo, step_exp_table, cutoff_threshold);

			std::vector<Interval> contracted_domain = newFlowpipe.domain;
			std::vector<bool> local_boundary_intersected;
			int type = contract_interval_arithmetic(tmvCompo, contracted_domain, invariants[mode], local_boundary_intersected, cutoff_threshold);

			if(type >= 0)
			{
				// collect the intersected invariant boundary
				if(invariant_boundary_intersected.size() != local_boundary_intersected.size())
				{
					invariant_boundary_intersected = local_boundary_intersected;
				}
				else
				{
					for(int i=0; i<local_boundary_intersected.size(); ++i)
					{
						if(local_boundary_intersected[i])
						{
							invariant_boundary_intersected[i] = true;
						}
					}
				}
			}

			switch(type)
			{
			case -1:	// invariant violated
				return checking_result;
			case 0:		// domain is not contracted
			{
				++num_of_flowpipes;
				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, newFlowpipe.domain, unsafeSet, localMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(SAFE);
					}
					else if(safety == UNSAFE && !bContracted)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNSAFE);

						return COMPLETED_UNSAFE;
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(newFlowpipe.domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(newFlowpipe.domain);
					flowpipes_safety.push_back(SAFE);
				}

				currentFlowpipe = newFlowpipe;

				break;
			}
			case 1: 	// time interval is not contracted
			{
				++num_of_flowpipes;
//				tmvCompo.normalize(contracted_domain);
				bContracted = true;

				flowpipes_contracted.push_back(bContracted);

				if(bSafetyChecking)
				{
					int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, localMaxOrder, cutoff_threshold);

					if(safety == SAFE)
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(UNKNOWN);

						if(checking_result == COMPLETED_SAFE)
						{
							checking_result = COMPLETED_UNKNOWN;
						}
					}
				}
				else
				{
					flowpipes.push_back(tmvCompo);
					domains.push_back(contracted_domain);
					flowpipes_safety.push_back(SAFE);
				}

				newFlowpipe.domain = contracted_domain;
				newFlowpipe.normalize();
				currentFlowpipe = newFlowpipe;

				break;
			}
			case 2: 	// time interval is contracted
			{
				if(contracted_domain[0] > intZero)
				{
					return checking_result;
				}
				else
				{
					++num_of_flowpipes;
//					tmvCompo.normalize(contracted_domain);

					bContracted = true;
					flowpipes_contracted.push_back(true);

					if(bSafetyChecking)
					{
						int safety = safetyChecking2(tmvCompo, contracted_domain, unsafeSet, localMaxOrder, cutoff_threshold);

						if(safety == SAFE)
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(SAFE);
						}
						else
						{
							flowpipes.push_back(tmvCompo);
							domains.push_back(contracted_domain);
							flowpipes_safety.push_back(UNKNOWN);

							if(checking_result == COMPLETED_SAFE)
							{
								checking_result = COMPLETED_UNKNOWN;
							}
						}
					}
					else
					{
						flowpipes.push_back(tmvCompo);
						domains.push_back(contracted_domain);
						flowpipes_safety.push_back(SAFE);
					}

					t += contracted_domain[0].sup();

					if(bPrint)
					{
						printf("mode: %s,\t", modeNames[mode].c_str());
						printf("time = %f,\t", t);
						printf("step = %f,\t", contracted_domain[0].sup());
						printf("orders:\t");
						int num = orders.size()-1;
						for(int i=0; i<num; ++i)
						{
							printf("%s : %d, ", stateVarNames[i].c_str(), newOrders[i]);
						}

						printf("%s : %d\n", stateVarNames[num].c_str(), newOrders[num]);
					}

					return checking_result;
				}
			}
			}

			t += step;

			if(bPrint)
			{
				printf("mode: %s,\t", modeNames[mode].c_str());
				printf("time = %f,\t", t);
				printf("step = %f,\t", step);
				printf("orders:\t");
				int num = orders.size()-1;
				for(int i=0; i<num; ++i)
				{
					printf("%s : %d, ", stateVarNames[i].c_str(), newOrders[i]);
				}

				printf("%s : %d\n", stateVarNames[num].c_str(), newOrders[num]);
			}

			for(int i=0; i<newOrders.size(); ++i)
			{
				if(newOrders[i] > orders[i])
					--newOrders[i];
			}
		}
		else
		{
			switch(checking_result)
			{
			case COMPLETED_SAFE:
				return UNCOMPLETED_SAFE;
			case COMPLETED_UNSAFE:
				return UNCOMPLETED_UNSAFE;
			case COMPLETED_UNKNOWN:
				return UNCOMPLETED_UNKNOWN;
			}
		}
	}

	return checking_result;
}


// hybrid reachability
int HybridSystem::reach_hybrid(std::list<std::list<TaylorModelVec> > & flowpipes, std::list<std::list<std::vector<Interval> > > & domains,
		std::list<std::list<int> > & flowpipes_safety, std::list<std::list<bool> > & flowpipes_contracted, long & num_of_flowpipes,
		std::list<int> & modeIDs, std::list<TreeNode *> & traceNodes, TreeNode * & traceTree, const std::vector<int> & integrationSchemes, const double time, const int maxJmps,
		const ReachabilitySetting & global_setting, const std::vector<ReachabilitySetting> & local_settings, const std::vector<std::vector<int> > & aggregType,
		const std::vector<std::vector<std::vector<RowVector> > > aggregationTemplate_candidates, const std::vector<RowVector> default_aggregation_template,
		const std::vector<std::vector<Matrix> > & weightTab, const std::vector<std::vector<std::vector<bool> > > & linear_auto,
		const std::vector<std::vector<std::vector<RowVector> > > & template_auto, const bool bPrint, const std::vector<std::string> & stateVarNames,
		const std::vector<std::string> & modeNames, const std::vector<std::string> & tmVarNames,
		const std::vector<std::vector<PolynomialConstraint> > & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump)
{
	std::list<int> modeQueue;
	std::list<Flowpipe> flowpipeQueue;
	std::list<double> timePassedQueue;
	std::list<int> jumpsExecutedQueue;
	std::list<bool> contractedQueue;

	Interval intZero;
	int rangeDim = initialSet.tmv.tms.size();

	modeQueue.push_back(initialMode);
	flowpipeQueue.push_back(initialSet);
	timePassedQueue.push_back(0);
	jumpsExecutedQueue.push_back(0);
	contractedQueue.push_back(false);

	// mode trace
	std::list<TreeNode *> nodeQueue;
	traceTree = new TreeNode(0, initialMode, intZero);
	nodeQueue.push_back(traceTree);

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;



	std::vector<upMatrix> vec_up_Phi_0;
	std::vector<TaylorModelVec> vec_tmv_Phi;
	std::vector<std::vector<iMatrix> > vec_Phi_exp_table;
	std::vector<iMatrix> vec_im_Psi;
	std::vector<iMatrix> vec_im_global_Psi;
	std::vector<TaylorModelVec> vec_tmv_Psi;
	std::vector<std::vector<TaylorModelVec> > vec_tmv_Psi_table;

	upMatrix up_empty;
	TaylorModelVec tmv_empty;
	std::vector<iMatrix> vec_im_empty;
	iMatrix im_empty;
	iMatrix im_zero(rangeDim, 1);
	std::vector<TaylorModelVec> vec_tmv_empty;

	for(int i=0; i<modes.size(); ++i)
	{
		vec_up_Phi_0.push_back(up_empty);
		vec_tmv_Phi.push_back(tmv_empty);
		vec_Phi_exp_table.push_back(vec_im_empty);
		vec_im_Psi.push_back(im_empty);
		vec_im_global_Psi.push_back(im_zero);
		vec_tmv_Psi.push_back(tmv_empty);
		vec_tmv_Psi_table.push_back(vec_tmv_empty);
	}


	for(; modeQueue.size() != 0;)
	{
		int initMode = modeQueue.front();
		Flowpipe initFp = flowpipeQueue.front();
		double timePassed = timePassedQueue.front();
		int jumpsExecuted = jumpsExecutedQueue.front();
		bool bContracted = contractedQueue.front();

		TreeNode *node = nodeQueue.front();

		modeQueue.pop_front();
		flowpipeQueue.pop_front();
		timePassedQueue.pop_front();
		jumpsExecutedQueue.pop_front();
		contractedQueue.pop_front();

		nodeQueue.pop_front();


		std::list<TaylorModelVec> mode_flowpipes;
		std::list<std::vector<Interval> > mode_domains;
		std::list<int> mode_flowpipes_safety;
		std::list<bool> mode_flowpipes_contracted;


		int orderType;
		if(local_settings[initMode].orderType >= 0)
		{
			orderType = local_settings[initMode].orderType;
		}
		else
		{
			orderType = global_setting.orderType;
		}

		bool bAdaptiveSteps;
		double step, miniStep;
		if(local_settings[initMode].step > 0)
		{
			bAdaptiveSteps = local_settings[initMode].bAdaptiveSteps;
			step = local_settings[initMode].step;
			miniStep = local_settings[initMode].miniStep;
		}
		else
		{
			bAdaptiveSteps = global_setting.bAdaptiveSteps;
			step = global_setting.step;
			miniStep = global_setting.miniStep;
		}

		bool bAdaptiveOrders;
		const std::vector<int> *porders;
		const std::vector<int> *pmaxOrders;
		int globalMaxOrder;
		if(local_settings[initMode].orders.size() > 0)
		{
			bAdaptiveOrders = local_settings[initMode].bAdaptiveOrders;
			porders = &(local_settings[initMode].orders);
			pmaxOrders = &(local_settings[initMode].maxOrders);
			globalMaxOrder = local_settings[initMode].globalMaxOrder;
		}
		else
		{
			bAdaptiveOrders = global_setting.bAdaptiveOrders;
			porders = &(global_setting.orders);
			pmaxOrders = &(global_setting.maxOrders);
			globalMaxOrder = global_setting.globalMaxOrder;
		}

		int precondition;
		if(local_settings[initMode].precondition >= 0)
		{
			precondition = local_settings[initMode].precondition;
		}
		else
		{
			precondition = global_setting.precondition;
		}

		const std::vector<Interval> *pestimation;
		if(local_settings[initMode].estimation.size() > 0)
		{
			pestimation = &(local_settings[initMode].estimation);
		}
		else
		{
			pestimation = &(global_setting.estimation);
		}

		Interval cutoff_threshold;
		if(local_settings[initMode].cutoff_threshold.sup() > 0)
		{
			cutoff_threshold = local_settings[initMode].cutoff_threshold;
		}
		else
		{
			cutoff_threshold = global_setting.cutoff_threshold;
		}

		std::vector<bool> invariant_boundary_intersected;
		int result;

		switch(integrationSchemes[initMode])
		{
		case ONLY_PICARD:
		{
			switch(orderType)
			{
			case UNIFORM:
				if(bAdaptiveSteps)
				{
					result = reach_continuous_picard(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, miniStep, time-timePassed, (*porders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else if(bAdaptiveOrders)
				{
					result = reach_continuous_picard(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, (*porders)[0], (*pmaxOrders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else
				{
					result = reach_continuous_picard(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, (*porders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				break;
			case MULTI:
				if(bAdaptiveSteps)
				{
					result = reach_continuous_picard(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, miniStep, time-timePassed, *porders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else if(bAdaptiveOrders)
				{
					result = reach_continuous_picard(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, *porders, *pmaxOrders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else
				{
					result = reach_continuous_picard(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, *porders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				break;
			}
			break;
		}

		case LOW_DEGREE:
		{
			switch(orderType)
			{
			case UNIFORM:
				if(bAdaptiveSteps)
				{
					result = reach_continuous_low_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, miniStep, time-timePassed, (*porders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else if(bAdaptiveOrders)
				{
					result = reach_continuous_low_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, (*porders)[0], (*pmaxOrders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else
				{
					result = reach_continuous_low_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, (*porders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				break;
			case MULTI:
				if(bAdaptiveSteps)
				{
					result = reach_continuous_low_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, miniStep, time-timePassed, *porders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else if(bAdaptiveOrders)
				{
					result = reach_continuous_low_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, *porders, *pmaxOrders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else
				{
					result = reach_continuous_low_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, *porders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				break;
			}
			break;
		}

		case HIGH_DEGREE:
		{
			switch(orderType)
			{
			case UNIFORM:
				if(bAdaptiveSteps)
				{
					result = reach_continuous_high_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, miniStep, time-timePassed, (*porders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else if(bAdaptiveOrders)
				{
					result = reach_continuous_high_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, (*porders)[0], (*pmaxOrders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else
				{
					result = reach_continuous_high_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, (*porders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				break;
			case MULTI:
				if(bAdaptiveSteps)
				{
					result = reach_continuous_high_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, miniStep, time-timePassed, *porders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else if(bAdaptiveOrders)
				{
					result = reach_continuous_high_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, *porders, *pmaxOrders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else
				{
					result = reach_continuous_high_degree(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, *porders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				break;
			}
			break;
		}

		case NONPOLY_TAYLOR:
		{
			switch(orderType)
			{
			case UNIFORM:
				if(bAdaptiveSteps)
				{
					result = reach_continuous_non_polynomial_taylor(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, miniStep, time-timePassed, (*porders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else if(bAdaptiveOrders)
				{
					result = reach_continuous_non_polynomial_taylor(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, (*porders)[0], (*pmaxOrders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else
				{
					result = reach_continuous_non_polynomial_taylor(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, (*porders)[0], precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				break;
			case MULTI:
				if(bAdaptiveSteps)
				{
					result = reach_continuous_non_polynomial_taylor(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, miniStep, time-timePassed, *porders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else if(bAdaptiveOrders)
				{
					result = reach_continuous_non_polynomial_taylor(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, *porders, *pmaxOrders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				else
				{
					result = reach_continuous_non_polynomial_taylor(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted,
							num_of_flowpipes, initMode, initFp, step, time-timePassed, *porders, globalMaxOrder, precondition, *pestimation, bPrint, stateVarNames, invariant_boundary_intersected, modeNames, cutoff_threshold,
							unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
				}
				break;
			}
			break;
		}

		case LTI:
		{
			result = reach_continuous_lti(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted, num_of_flowpipes,
					vec_up_Phi_0[initMode], vec_tmv_Phi[initMode], vec_Phi_exp_table[initMode], vec_im_Psi[initMode], vec_im_global_Psi[initMode], vec_tmv_Psi[initMode], vec_tmv_Psi_table[initMode],
					initMode, initFp.tmvPre, initFp.domain, step, time-timePassed, (*porders)[0], bPrint, invariant_boundary_intersected, modeNames, stateVarNames, cutoff_threshold,
					unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
			break;
		}

		case LTV:
		{
			result = reach_continuous_ltv(mode_flowpipes, mode_domains, mode_flowpipes_safety, mode_flowpipes_contracted, num_of_flowpipes,
					initMode, initFp.tmvPre, initFp.domain, step, time-timePassed, (*porders)[0], bPrint, invariant_boundary_intersected, modeNames, stateVarNames, cutoff_threshold,
					unsafeSet[initMode], bSafetyChecking, bPlot, bDump);
			break;
		}
		}

		if(bPlot || bDump)
		{
			flowpipes.push_back(mode_flowpipes);
			domains.push_back(mode_domains);
			flowpipes_safety.push_back(mode_flowpipes_safety);
			flowpipes_contracted.push_back(mode_flowpipes_contracted);

			modeIDs.push_back(initMode);
			traceNodes.push_back(node);
		}

		if(result >= UNCOMPLETED_SAFE || result == COMPLETED_UNSAFE)
		{
			return result;
		}
		else if(result == COMPLETED_UNSAFE && bContracted)
		{
			checking_result = COMPLETED_UNKNOWN;
		}
		else
		{
			if(checking_result != COMPLETED_UNKNOWN)
			{
				checking_result = result;
			}
		}


		if(jumpsExecuted == maxJmps)
		{
			if(bPrint)
			{
				printf("Maximum jump depth is reached.\n");
			}

			continue;
		}


		// overapproximate the intersection for each jump
		for(int i=0; i<transitions[initMode].size(); ++i)
		{
			std::vector<TaylorModelVec> intersected_flowpipes;
			std::vector<std::vector<Interval> > intersected_domains;

			double startTime = timePassed;
			double newTimePassed = 0;
			bool brecorded = false;

			if(bPrint)
			{
				printf("Dealing with the jump from %s to %s ...\n", modeNames[initMode].c_str(), modeNames[transitions[initMode][i].targetID].c_str());
			}

			// collect the intersected flowpipes
			std::vector<bool> guard_boundary_intersected;

			std::list<TaylorModelVec>::iterator tmvIter = mode_flowpipes.begin();
			std::list<std::vector<Interval> >::iterator doIter = mode_domains.begin();
			std::list<int>::iterator safetyIter = mode_flowpipes_safety.begin();
			std::list<bool>::iterator contractedIter = mode_flowpipes_contracted.begin();


			Interval triggeredTime;
			bool mode_contracted = false;

			for(; tmvIter!=mode_flowpipes.end(); ++tmvIter, ++doIter, ++safetyIter, ++contractedIter)
			{
				TaylorModelVec tmvIntersection = *tmvIter;
				std::vector<Interval> doIntersection = *doIter;

				std::vector<bool> local_boundary_intersected;
				int type = contract_interval_arithmetic(tmvIntersection, doIntersection, transitions[initMode][i].guard, local_boundary_intersected, cutoff_threshold);

				if(type >= 0 && aggregType[initMode][i] == PARA_AGGREG)
				{
					// collect the intersected guard boundary
					if(guard_boundary_intersected.size() != local_boundary_intersected.size())
					{
						guard_boundary_intersected = local_boundary_intersected;
					}
					else
					{
						for(int j=0; j<local_boundary_intersected.size(); ++j)
						{
							if(local_boundary_intersected[j])
							{
								guard_boundary_intersected[j] = true;
							}
						}
					}
				}

				if(type != -1)
				{
					if(!brecorded)
					{
						brecorded = true;

						triggeredTime = doIntersection[0];
						triggeredTime.add_assign(newTimePassed);
						newTimePassed += doIntersection[0].inf();
					}
					else
					{
						// compute the time interval when the jump is triggered
						triggeredTime.setSup(triggeredTime.sup() + doIntersection[0].sup());
					}

//					tmvIntersection.normalize(doIntersection);
					intersected_flowpipes.push_back(tmvIntersection);
					intersected_domains.push_back(doIntersection);

					if(!mode_contracted && *contractedIter)
					{
						mode_contracted = true;
					}
				}
				else
				{
					if(!brecorded)
					{
						newTimePassed += doIntersection[0].sup();
					}
				}
			}


			std::vector<bool> boundary_intersected = invariant_boundary_intersected;
			for(int j=0; j<guard_boundary_intersected.size(); ++j)
			{
				boundary_intersected.push_back(guard_boundary_intersected[j]);
			}


			if(intersected_flowpipes.size() == 1)
			{
//				printf("Only one interseted flowpipe.\n");

				TaylorModelVec tmvAggregation;
				std::vector<Interval> doAggregation = intersected_domains[0];
				Flowpipe fpAggregation;

				std::vector<Interval> step_exp_table;
				construct_step_exp_table(step_exp_table, doAggregation[0], (globalMaxOrder+1)*2);
				intersected_flowpipes[0].evaluate_t(tmvAggregation, step_exp_table);


				//reset map
				TaylorModelVec tmvImage;
				transitions[initMode][i].resetMap.reset(tmvImage, tmvAggregation, doAggregation, globalMaxOrder, cutoff_threshold);

				std::vector<bool> bVecDummy;
				int type = contract_interval_arithmetic(tmvImage, doAggregation, invariants[transitions[initMode][i].targetID], bVecDummy, cutoff_threshold);

				if(type == -1)
				{
					if(bPrint)
					{
						printf("No intersection detected.\n");
					}

					continue;
				}
				else
				{
					tmvImage.normalize(doAggregation);

					int rangeDim = tmvImage.tms.size();
					Matrix coefficients(rangeDim, rangeDim+1);

					for(int i=0; i<rangeDim; ++i)
					{
						coefficients.set(1, i, i+1);
					}

					TaylorModelVec tmvTemp(coefficients);

					fpAggregation.tmv = tmvTemp;
					fpAggregation.tmvPre = tmvImage;
					fpAggregation.domain = doAggregation;
				}

				startTime += newTimePassed;
				if(startTime < time - THRESHOLD_HIGH)
				{
					modeQueue.push_back(transitions[initMode][i].targetID);
					flowpipeQueue.push_back(fpAggregation);
					timePassedQueue.push_back(startTime);
					jumpsExecutedQueue.push_back(jumpsExecuted+1);
					contractedQueue.push_back(mode_contracted | bContracted);

					TreeNode *child = new TreeNode(transitions[initMode][i].jumpID, transitions[initMode][i].targetID, triggeredTime);
					child->parent = node;
					node->children.push_back(child);

					nodeQueue.push_back(child);
				}
			}
			else if(intersected_flowpipes.size() > 0)
			{
				// aggregate the intersections
				Flowpipe fpAggregation;
				TaylorModelVec tmvAggregation;
				std::vector<Interval> doAggregation;

/*
				// ==========test begin==========
				resultsCompo.clear();
				domains.clear();

				list<TaylorModelVec> interfp;
				list<vector<Interval> > interdom;

				for(int m=0; m<intersection_flowpipes.size(); ++m)
				{
					interfp.push_back(intersection_flowpipes[m]);
					interdom.push_back(intersection_domains[m]);
				}

				resultsCompo.push_back(interfp);
				domains.push_back(interdom);
				return;
				// ==========test end==========
*/

//				printf("intersected flowpipes = %d\n", (int)intersected_flowpipes.size());


				switch(aggregType[initMode][i])
				{
				case INTERVAL_AGGREG:
				{
					aggregate_flowpipes_by_interval(tmvAggregation, doAggregation, intersected_flowpipes, intersected_domains);

					break;
				}
				case PARA_AGGREG:
				{
					aggregate_flowpipes_by_Parallelotope(tmvAggregation, doAggregation, intersected_flowpipes, intersected_domains,
							invariants[initMode], transitions[initMode][i], boundary_intersected,
							aggregationTemplate_candidates[initMode][i], default_aggregation_template,
							weightTab[initMode][i], linear_auto[initMode][i], template_auto[initMode][i], globalMaxOrder, rangeDim);

					break;
				}
				}


				// contract the aggregation regarding to the guard and invariant
				std::vector<PolynomialConstraint> constraints = transitions[initMode][i].guard;
				for(int j=0; j<invariants[initMode].size(); ++j)
				{
					constraints.push_back(invariants[initMode][j]);
				}

				std::vector<bool> bVecDummy;
				int type = contract_interval_arithmetic(tmvAggregation, doAggregation, constraints, bVecDummy, cutoff_threshold);

				if(type == -1)
				{
					if(bPrint)
					{
						printf("No intersection detected.\n");
					}

					continue;
				}
//				else
//				{
//					tmvAggregation.normalize(doAggregation);
//				}

				//reset map
				TaylorModelVec tmvImage;
				transitions[initMode][i].resetMap.reset(tmvImage, tmvAggregation, doAggregation, globalMaxOrder, cutoff_threshold);

				type = contract_interval_arithmetic(tmvImage, doAggregation, invariants[transitions[initMode][i].targetID], bVecDummy, cutoff_threshold);

				if(type == -1)
				{
					if(bPrint)
					{
						printf("No intersection detected.\n");
					}

					continue;
				}
				else
				{
					tmvImage.normalize(doAggregation);

					int rangeDim = tmvImage.tms.size();
					Matrix coefficients(rangeDim, rangeDim+1);

					for(int i=0; i<rangeDim; ++i)
					{
						coefficients.set(1, i, i+1);
					}

					TaylorModelVec tmvTemp(coefficients);

					fpAggregation.tmv = tmvTemp;
					fpAggregation.tmvPre = tmvImage;
					fpAggregation.domain = doAggregation;
				}

				startTime += newTimePassed;
				if(startTime < time - THRESHOLD_HIGH)
				{
					modeQueue.push_back(transitions[initMode][i].targetID);
					flowpipeQueue.push_back(fpAggregation);
					timePassedQueue.push_back(startTime);
					jumpsExecutedQueue.push_back(jumpsExecuted+1);
					contractedQueue.push_back(mode_contracted | bContracted);

					TreeNode *child = new TreeNode(transitions[initMode][i].jumpID, transitions[initMode][i].targetID, triggeredTime);
					child->parent = node;
					node->children.push_back(child);

					nodeQueue.push_back(child);
				}
			}
			else
			{
				if(bPrint)
				{
					printf("No intersection detected.\n");
				}
			}

			if(bPrint)
			{
				printf("Done.\n");
			}
		}
	}

	return checking_result;
}








// class ReachabilitySetting

ReachabilitySetting::ReachabilitySetting()
{
	step			= -1.0;
	precondition	= -1;
	orderType		= -1;
	bAdaptiveSteps	= false;
	bAdaptiveOrders	= false;
	miniStep		= -1.0;
	globalMaxOrder	= -1;

	Interval I(-1);
	cutoff_threshold = I;
}

ReachabilitySetting::~ReachabilitySetting()
{
}

ReachabilitySetting::ReachabilitySetting(const ReachabilitySetting & setting)
{
	step			= setting.step;
	precondition	= setting.precondition;
	orderType		= setting.orderType;
	bAdaptiveSteps	= setting.bAdaptiveSteps;
	bAdaptiveOrders	= setting.bAdaptiveOrders;
	estimation		= setting.estimation;
	miniStep		= setting.miniStep;
	orders			= setting.orders;
	maxOrders		= setting.maxOrders;
	globalMaxOrder	= setting.globalMaxOrder;
	cutoff_threshold= setting.cutoff_threshold;
}

void ReachabilitySetting::clear()
{
	step			= -1.0;
	precondition	= -1;
	orderType		= -1;
	bAdaptiveSteps	= false;
	bAdaptiveOrders	= false;
	miniStep		= -1.0;
	globalMaxOrder	= -1;

	Interval I(-1);
	cutoff_threshold = I;
}

ReachabilitySetting & ReachabilitySetting::operator = (const ReachabilitySetting & setting)
{
	if(this == &setting)
		return *this;

	step			= setting.step;
	precondition	= setting.precondition;
	orderType		= setting.orderType;
	bAdaptiveSteps	= setting.bAdaptiveSteps;
	bAdaptiveOrders	= setting.bAdaptiveOrders;
	estimation		= setting.estimation;
	miniStep		= setting.miniStep;
	orders			= setting.orders;
	maxOrders		= setting.maxOrders;
	globalMaxOrder	= setting.globalMaxOrder;
	cutoff_threshold= setting.cutoff_threshold;

	return *this;
}








// class LTI_Dynamics

LTI_Dynamics::LTI_Dynamics()
{
}

LTI_Dynamics::LTI_Dynamics(const iMatrix & dyn_A, const iMatrix & dyn_B)
{
	im_dyn_A = dyn_A;
	im_dyn_B = dyn_B;

	int n = im_dyn_A.rows();
	bMatrix conMatrix(n, n), adjMatrix(n, n);

	Interval intZero;
	for(int i=0; i<n; ++i)
	{
		for(int j=0; j<n; ++j)
		{
			if(!im_dyn_A[i][j].subseteq(intZero))
			{
				adjMatrix[i][j] = true;
			}
		}
	}

	check_connectivities(conMatrix, adjMatrix);
	connectivity = conMatrix;

	if(dyn_B.isZero())
	{
		bAuto = true;
	}
	else
	{
		bAuto = false;
	}
}

LTI_Dynamics::LTI_Dynamics(const LTI_Dynamics & lti_dyn)
{
	im_dyn_A = lti_dyn.im_dyn_A;
	im_dyn_B = lti_dyn.im_dyn_B;
	connectivity = lti_dyn.connectivity;
	bAuto = lti_dyn.bAuto;
}

LTI_Dynamics::~LTI_Dynamics()
{
}

LTI_Dynamics & LTI_Dynamics::operator = (const LTI_Dynamics & lti_dyn)
{
	if(this == &lti_dyn)
		return *this;

	im_dyn_A = lti_dyn.im_dyn_A;
	im_dyn_B = lti_dyn.im_dyn_B;
	connectivity = lti_dyn.connectivity;
	bAuto = lti_dyn.bAuto;

	return *this;
}





// class LTV_dynamics

LTV_Dynamics::LTV_Dynamics()
{
}

LTV_Dynamics::LTV_Dynamics(const upMatrix & dyn_A, const upMatrix & dyn_B)
{
	up_dyn_A = dyn_A;
	up_dyn_B = dyn_B;

	int n = up_dyn_A.rows();
	bMatrix conMatrix(n, n), adjMatrix(n, n);

	for(int i=0; i<n; ++i)
	{
		for(int j=0; j<n; ++j)
		{
			if(!up_dyn_A[i][j].isZero())
			{
				adjMatrix[i][j] = true;
			}
		}
	}

	check_connectivities(conMatrix, adjMatrix);
	connectivity = conMatrix;

	if(dyn_B.isZero())
	{
		bAuto = true;
	}
	else
	{
		bAuto = false;
	}
}

LTV_Dynamics::LTV_Dynamics(const upMatrix & dyn_A, const upMatrix & dyn_B, const upMatrix & tv_part)
{
	up_dyn_A = dyn_A;
	up_dyn_B = dyn_B;
	up_tv_part = tv_part;

	int n = up_dyn_A.rows();
	bMatrix conMatrix(n, n), adjMatrix(n, n);

	for(int i=0; i<n; ++i)
	{
		for(int j=0; j<n; ++j)
		{
			if(!up_dyn_A[i][j].isZero())
			{
				adjMatrix[i][j] = true;
			}
		}
	}

	check_connectivities(conMatrix, adjMatrix);
	connectivity = conMatrix;

	if(dyn_B.isZero())
	{
		bAuto = true;
	}
	else
	{
		bAuto = false;
	}
}

LTV_Dynamics::LTV_Dynamics(const LTV_Dynamics & ltv_dyn)
{
	up_dyn_A = ltv_dyn.up_dyn_A;
	up_dyn_B = ltv_dyn.up_dyn_B;
	up_tv_part = ltv_dyn.up_tv_part;
	connectivity = ltv_dyn.connectivity;
	bAuto = ltv_dyn.bAuto;
}

LTV_Dynamics::~LTV_Dynamics()
{
}

LTV_Dynamics & LTV_Dynamics::operator = (const LTV_Dynamics & ltv_dyn)
{
	if(this == &ltv_dyn)
		return *this;

	up_dyn_A = ltv_dyn.up_dyn_A;
	up_dyn_B = ltv_dyn.up_dyn_B;
	up_tv_part = ltv_dyn.up_tv_part;
	connectivity = ltv_dyn.connectivity;
	bAuto = ltv_dyn.bAuto;

	return *this;
}





// class HybridReachability

HybridReachability::HybridReachability()
{
	traceTree = NULL;
	numOfJumps = 0;
}

HybridReachability::~HybridReachability()
{
	outputAxes.clear();
	aggregationType.clear();
	default_aggregation_template.clear();
	aggregationTemplate_candidates.clear();
	weightTab.clear();
	flowpipes.clear();
	domains.clear();
	flowpipes_safety.clear();
	flowpipes_contracted.clear();
	modeIDs.clear();
	stateVarTab.clear();
	stateVarNames.clear();
	tmVarTab.clear();
	tmVarNames.clear();
	modeTab.clear();
	modeNames.clear();
	unsafeSet.clear();
	traceNodes.clear();
	integrationSchemes.clear();

	delete traceTree;
}

void HybridReachability::dump(FILE *fp) const
{
	fprintf(fp,"state var ");
	for(int i=0; i<stateVarNames.size()-1; ++i)
	{
		fprintf(fp, "%s,", stateVarNames[i].c_str());
	}

	fprintf(fp, "%s\n\n", stateVarNames[stateVarNames.size()-1].c_str());

	bool bDumpCounterexamples = true;
	FILE *fpDumpCounterexamples;

	int mkres = mkdir(counterexampleDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if(mkres < 0 && errno != EEXIST)
	{
		printf("Can not create the directory for counterexamples.\n");
		bDumpCounterexamples = false;
	}

	char filename_counterexamples[NAME_SIZE+10];

	if(bDumpCounterexamples)
	{
		sprintf(filename_counterexamples, "%s%s%s", counterexampleDir, outputFileName, str_counterexample_dumping_name_suffix);
		fpDumpCounterexamples = fopen(filename_counterexamples, "w");
	}

	// dump the constraints for the state space
	for(int i=0; i<system.invariants.size(); ++i)
	{
		fprintf(fp, "%s\n{\n", modeNames[i].c_str());

		fprintf(fp, "{\n");

		if(local_settings[i].globalMaxOrder > 0)
		{
			fprintf(fp, "order %d\n", local_settings[i].globalMaxOrder);
		}
		else
		{
			fprintf(fp, "order %d\n", global_setting.globalMaxOrder);
		}

		fprintf(fp, "}\n");
		fprintf(fp, "{\n");

		if(local_settings[i].cutoff_threshold.sup() > 0)
		{
			fprintf(fp, "cutoff %e\n", local_settings[i].cutoff_threshold.sup());
		}
		else
		{
			fprintf(fp, "cutoff %e\n", global_setting.cutoff_threshold.sup());
		}

		fprintf(fp, "}\n");
		fprintf(fp, "{\n");

		for(int j=0; j<system.invariants[i].size(); ++j)
		{
			system.invariants[i][j].dump(fp, stateVarNames);
		}

		fprintf(fp, "}\n}\n\n");
	}

	// dump the computation tree
	fprintf(fp, "computation paths\n{\n\n");

	std::string strEmpty;
	traceTree->dump(fp, strEmpty, modeNames);

	fprintf(fp, "}\n\n");

	switch(plotFormat)
	{
	case PLOT_GNUPLOT:
		switch(plotSetting)
		{
		case PLOT_INTERVAL:
			fprintf(fp, "gnuplot interval %s , %s\n\n", stateVarNames[outputAxes[0]].c_str(), stateVarNames[outputAxes[1]].c_str());
			break;
		case PLOT_OCTAGON:
			fprintf(fp, "gnuplot octagon %s , %s\n\n", stateVarNames[outputAxes[0]].c_str(), stateVarNames[outputAxes[1]].c_str());
			break;
		case PLOT_GRID:
			fprintf(fp, "gnuplot grid %d %s , %s\n\n", numSections, stateVarNames[outputAxes[0]].c_str(), stateVarNames[outputAxes[1]].c_str());
			break;
		}
		break;
	case PLOT_MATLAB:
		switch(plotSetting)
		{
		case PLOT_INTERVAL:
			fprintf(fp, "matlab interval %s , %s\n\n", stateVarNames[outputAxes[0]].c_str(), stateVarNames[outputAxes[1]].c_str());
			break;
		case PLOT_OCTAGON:
			fprintf(fp, "matlab octagon %s , %s\n\n", stateVarNames[outputAxes[0]].c_str(), stateVarNames[outputAxes[1]].c_str());
			break;
		case PLOT_GRID:
			fprintf(fp, "matlab grid %d %s , %s\n\n", numSections, stateVarNames[outputAxes[0]].c_str(), stateVarNames[outputAxes[1]].c_str());
			break;
		}
		break;
	}

	fprintf(fp, "output %s\n\n", outputFileName);

	if(bSafetyChecking)
	{
		// dump the unsafe set
		fprintf(fp, "unsafe\n{\n");

		for(int i=0; i<bVecUnderCheck.size(); ++i)
		{
			if(bVecUnderCheck[i])
			{
				fprintf(fp, "%s\n{\n", modeNames[i].c_str());

				for(int j=0; j<unsafeSet[i].size(); ++j)
				{
					unsafeSet[i][j].dump(fp, stateVarNames);
				}

				fprintf(fp, "}\n\n");
			}
		}

		fprintf(fp, "}\n\n");
	}

	int rangeDim = system.hfOdes.size();

	std::list<std::list<TaylorModelVec> >::const_iterator fpIter = flowpipes.begin();
	std::list<std::list<std::vector<Interval> > >::const_iterator fpdoIter = domains.begin();
	std::list<int>::const_iterator modeIter = modeIDs.begin();
	std::list<std::list<bool> >::const_iterator fpConIter = flowpipes_contracted.begin();
	std::list<std::list<int> >::const_iterator fpSafetyIter = flowpipes_safety.begin();

	std::list<TreeNode *>::const_iterator nodeIter = traceNodes.begin();

	std::list<TaylorModelVec>::const_iterator tmvIter;
	std::list<std::vector<Interval> >::const_iterator doIter;
	std::list<bool>::const_iterator conIter;
	std::list<int>::const_iterator safetyIter;

	fprintf(fp, "hybrid flowpipes\n{\n");

	for(; fpIter!=flowpipes.end(); ++fpIter, ++fpdoIter, ++modeIter, ++fpSafetyIter, ++nodeIter)
	{
		tmvIter = fpIter->begin();
		doIter = fpdoIter->begin();
		conIter = fpConIter->begin();
		safetyIter = fpSafetyIter->begin();

		if(fpIter->size() > 0)
		{
			std::vector<std::string> newNames;

			if(doIter->size() != tmVarNames.size())
			{
				std::string tVar("local_t");
				newNames.push_back(tVar);

				// rename the variables
				for(int i=1; i<doIter->size(); ++i)
				{
					char name[NAME_SIZE];
					sprintf(name, "%s%d", local_var_name, i);
					std::string strName(name);
					newNames.push_back(strName);
				}
			}
			else
			{
				newNames = tmVarNames;
			}

			fprintf(fp, "%s\n{\n", modeNames[*modeIter].c_str());

			fprintf(fp, "tm var ");
			for(int i=1; i<newNames.size()-1; ++i)
			{
				fprintf(fp, "%s,", newNames[i].c_str());
			}

			fprintf(fp, "%s\n\n", newNames[newNames.size()-1].c_str());

			std::list<TaylorModelVec> unsafe_tm_flowpipes;
			std::list<std::vector<Interval> > unsafe_flowpipe_domains;
			std::list<Interval> unsafe_time_points;

			std::list<TaylorModelVec> unknown_tm_flowpipes;
			std::list<std::vector<Interval> > unknown_flowpipe_domains;
			std::list<Interval> unknown_time_points;

			Interval timePoint;

			for(; tmvIter!=fpIter->end(); ++tmvIter, ++doIter, ++conIter, ++safetyIter)
			{
				fprintf(fp, "{\n");
				tmvIter->dump_interval(fp, stateVarNames, newNames);

				for(int i=0; i<doIter->size(); ++i)
				{
					fprintf(fp, "%s in ", newNames[i].c_str());
					(*doIter)[i].dump(fp);
					fprintf(fp, "\n");
				}

				if(*conIter == true)
				{
					fprintf(fp, "\ntrue\n");
				}
				else
				{
					fprintf(fp, "\nfalse\n");
				}

				fprintf(fp, "}\n\n");

				if(*safetyIter == UNSAFE)
				{
					unsafe_tm_flowpipes.push_back(*tmvIter);
					unsafe_flowpipe_domains.push_back(*doIter);
					unsafe_time_points.push_back(timePoint);
				}
				else if(*safetyIter == UNKNOWN)
				{
					unknown_tm_flowpipes.push_back(*tmvIter);
					unknown_flowpipe_domains.push_back(*doIter);
					unknown_time_points.push_back(timePoint);
				}

				timePoint += (*doIter)[0];
			}

			fprintf(fp, "}\n\n");

			if(bDumpCounterexamples)
			{
				fprintf(fpDumpCounterexamples, "Unsafe flowpipes:\n\n");
				dump_counterexample(fpDumpCounterexamples, unsafe_tm_flowpipes, unsafe_flowpipe_domains, *nodeIter, unsafe_time_points);
				fprintf(fpDumpCounterexamples, "Unknown flowpipes:\n\n");
				dump_counterexample(fpDumpCounterexamples, unknown_tm_flowpipes, unknown_flowpipe_domains, *nodeIter, unknown_time_points);
			}
		}
	}

	fprintf(fp, "}\n");
}

int HybridReachability::run()
{
	// normalize the candidate vectors
	for(int i=0; i<aggregationTemplate_candidates.size(); ++i)
	{
		for(int j=0; j<aggregationTemplate_candidates[i].size(); ++j)
		{
			for(int k=0; k<aggregationTemplate_candidates[i][j].size(); ++k)
			{
				aggregationTemplate_candidates[i][j][k].normalize();
			}
		}
	}

	set_default_template();

	constructWeightTab();


/*
	// ==== test begin====
	// print out the jumps
	int num = 0;
	for(int i=0; i<system.transitions.size(); ++i)
	{
		for(int j=0; j<system.transitions[i].size(); ++j)
		{
			int start = system.transitions[i][j].startID;
			int end = system.transitions[i][j].targetID;

			printf("start:  %s,\tend:  %s\n", modeNames[start].c_str(), modeNames[end].c_str());

			switch(aggregationType[start][j])
			{
			case INTERVAL_AGGREG:
				printf("interval aggregation\n");
				break;
			case PARA_AGGREG:
				printf("paralleletope aggregation {\n");

				for(int k=0; k<aggregationTemplate_candidates[start][end].size(); ++k)
				{
					aggregationTemplate_candidates[start][end][k].dump(stdout);
				}

				printf("}\n");
				break;
			}

			++num;
		}
	}

	printf("total:  %d  jump(s)\n", num);

	exit(0);

	// ==== test end ====
*/

	int maxOrder = global_setting.globalMaxOrder;

	for(int i=0; i<local_settings.size(); ++i)
	{
		if(maxOrder < local_settings[i].globalMaxOrder)
		{
			maxOrder = local_settings[i].globalMaxOrder;
		}
	}

	compute_factorial_rec(maxOrder+2);
	compute_power_4(maxOrder+2);
	compute_double_factorial(maxOrder+4);

//	build_constraint_template((int)(stateVarNames.size()));

	return system.reach_hybrid(flowpipes, domains, flowpipes_safety, flowpipes_contracted, num_of_flowpipes, modeIDs, traceNodes, traceTree, integrationSchemes, time, maxJumps,
			global_setting, local_settings, aggregationType, aggregationTemplate_candidates, default_aggregation_template, weightTab,
			linear_auto, template_auto, bPrint, stateVarNames, modeNames, tmVarNames, unsafeSet, bSafetyChecking, bPlot, bDump);
}

void HybridReachability::prepareForPlotting()
{
	printf(BOLD_FONT "%%100\n" RESET_COLOR);
}

void HybridReachability::prepareForDumping()
{
	printf(BOLD_FONT "%%100\n" RESET_COLOR);
}

void HybridReachability::plot_2D(const bool bProjected) const
{
	char filename[NAME_SIZE+10];

	switch(plotFormat)
	{
	case PLOT_GNUPLOT:
		sprintf(filename, "%s%s.plt", outputDir, outputFileName);
		break;
	case PLOT_MATLAB:
		sprintf(filename, "%s%s.m", outputDir, outputFileName);
		break;
	}

	FILE *fpPlotting = fopen(filename, "w");

	if(fpPlotting == NULL)
	{
		printf("Can not create the plotting file.\n");
		exit(1);
	}

	printf("Generating the plotting file...\n");
	switch(plotFormat)
	{
	case PLOT_GNUPLOT:
		plot_2D_GNUPLOT(fpPlotting, bProjected);
		break;
	case PLOT_MATLAB:
		plot_2D_MATLAB(fpPlotting, bProjected);
		break;
	}
	printf("Done.\n");

	fclose(fpPlotting);
}

void HybridReachability::plot_2D_GNUPLOT(FILE *fp, const bool bProjected) const
{
	switch(plotSetting)
	{
	case PLOT_INTERVAL:
		plot_2D_interval_GNUPLOT(fp, bProjected);
		break;
	case PLOT_OCTAGON:
		plot_2D_octagon_GNUPLOT(fp, bProjected);
		break;
	case PLOT_GRID:
		plot_2D_grid_GNUPLOT(fp, bProjected);
		break;
	}
}

void HybridReachability::plot_2D_interval_GNUPLOT(FILE *fp, const bool bProjected) const
{
	fprintf(fp, "set terminal postscript enhanced color\n");

	char filename[NAME_SIZE+10];
	sprintf(filename, "%s%s.eps", imageDir, outputFileName);
	fprintf(fp, "set output '%s'\n", filename);

	fprintf(fp, "set style line 1 linecolor rgb \"blue\"\n");
	fprintf(fp, "set autoscale\n");
	fprintf(fp, "unset label\n");
	fprintf(fp, "set xtic auto\n");
	fprintf(fp, "set ytic auto\n");
	fprintf(fp, "set xlabel \"%s\"\n", stateVarNames[outputAxes[0]].c_str());
	fprintf(fp, "set ylabel \"%s\"\n", stateVarNames[outputAxes[1]].c_str());
	fprintf(fp, "plot '-' notitle with lines ls 1\n");

	std::list<std::list<TaylorModelVec> >::const_iterator fpIter = flowpipes.begin();
	std::list<std::list<std::vector<Interval> > >::const_iterator fpdoIter = domains.begin();
	std::list<int>::const_iterator modeIter = modeIDs.begin();
	std::list<std::list<int> >::const_iterator fpSafetyIter = flowpipes_safety.begin();

	std::list<TaylorModelVec>::const_iterator tmvIter;
	std::list<std::vector<Interval> >::const_iterator doIter;
	std::list<int>::const_iterator safetyIter;

	int prog = 0, total_size = 0;

	for(; fpSafetyIter!=flowpipes_safety.end(); ++fpSafetyIter)
	{
		total_size += fpSafetyIter->size();
	}

	fpSafetyIter = flowpipes_safety.begin();

	for(; fpSafetyIter!=flowpipes_safety.end(); ++fpIter, ++fpdoIter, ++modeIter, ++fpSafetyIter)
	{
		tmvIter = fpIter->begin();
		doIter = fpdoIter->begin();
		safetyIter = fpSafetyIter->begin();

		for(; safetyIter != fpSafetyIter->end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			std::vector<Interval> box;
			tmvIter->intEval(box, *doIter);

			// contract the interval according to the invariant
			std::vector<Interval> new_domain;
			std::vector<bool> bVecTemp;
			TaylorModelVec tmvInterval(box, new_domain);

			Interval cutoff_threshold;
			if(local_settings[*modeIter].cutoff_threshold.sup() > 0)
			{
				cutoff_threshold = local_settings[*modeIter].cutoff_threshold;
			}
			else
			{
				cutoff_threshold = global_setting.cutoff_threshold;
			}

			int type = contract_interval_arithmetic(tmvInterval, new_domain, system.invariants[*modeIter], bVecTemp, cutoff_threshold);

			if(type < 0)
			{
				continue;
			}

			tmvInterval.intEval(box, new_domain);

			Interval X(box[outputAxes[0]]), Y(box[outputAxes[1]]);

			// output the vertices
			fprintf(fp, "%lf %lf\n", X.inf(), Y.inf());
			fprintf(fp, "%lf %lf\n", X.sup(), Y.inf());
			fprintf(fp, "%lf %lf\n", X.sup(), Y.sup());
			fprintf(fp, "%lf %lf\n", X.inf(), Y.sup());
			fprintf(fp, "%lf %lf\n", X.inf(), Y.inf());
			fprintf(fp, "\n\n");

			++prog;
			printf("\b\b\b\b");
			printf(BOLD_FONT "%%" RESET_COLOR);
			printf(BOLD_FONT "%3d" RESET_COLOR, (int)(prog*100/total_size));
			fflush(stdout);

			if(*safetyIter == UNSAFE)
			{
				break;
			}
		}

		if(*safetyIter == UNSAFE)
		{
			break;
		}
	}

	fprintf(fp, "e\n");
	printf("\b\b\b\b");
	printf(BOLD_FONT "%%100\n" RESET_COLOR);
	fflush(stdout);
}

void HybridReachability::plot_2D_octagon_GNUPLOT(FILE *fp, const bool bProjected) const
{
	int x = outputAxes[0];
	int y = outputAxes[1];

	Interval intZero;

	int rangeDim = stateVarNames.size();
	Matrix output_poly_temp(8, rangeDim);

	output_poly_temp.set(1, 0, x);
	output_poly_temp.set(1, 1, y);
	output_poly_temp.set(-1, 2, x);
	output_poly_temp.set(-1, 3, y);
	output_poly_temp.set(1/sqrt(2), 4, x);
	output_poly_temp.set(1/sqrt(2), 4, y);
	output_poly_temp.set(1/sqrt(2), 5, x);
	output_poly_temp.set(-1/sqrt(2), 5, y);
	output_poly_temp.set(-1/sqrt(2), 6, x);
	output_poly_temp.set(1/sqrt(2), 6, y);
	output_poly_temp.set(-1/sqrt(2), 7, x);
	output_poly_temp.set(-1/sqrt(2), 7, y);

	// Construct the 2D template matrix.
	int rows = 8;
	int cols = rangeDim;

	Matrix sortedTemplate(rows, cols);
	RowVector rowVec(cols);
	std::vector<RowVector> sortedRows;
	std::vector<RowVector>::iterator iterp, iterq;

	output_poly_temp.row(rowVec, 0);
	sortedRows.push_back(rowVec);

	bool bInserted;

	// Sort the row vectors in the template by anti-clockwise order (only in the x-y space).
	for(int i=1; i<rows; ++i)
	{
		iterp = sortedRows.begin();
		iterq = iterp;
		++iterq;
		bInserted = false;

		for(; iterq != sortedRows.end();)
		{
			double tmp1 = output_poly_temp.get(i,x) * iterp->get(y) - output_poly_temp.get(i,y) * iterp->get(x);
			double tmp2 = output_poly_temp.get(i,x) * iterq->get(y) - output_poly_temp.get(i,y) * iterq->get(x);

			if(tmp1 < 0 && tmp2 > 0)
			{
				output_poly_temp.row(rowVec, i);
				sortedRows.insert(iterq, rowVec);
				bInserted = true;
				break;
			}
			else
			{
				++iterp;
				++iterq;
			}
		}

		if(!bInserted)
		{
			output_poly_temp.row(rowVec, i);
			sortedRows.push_back(rowVec);
		}
	}

	iterp = sortedRows.begin();
	for(int i=0; i<rows; ++i, ++iterp)
	{
		for(int j=0; j<cols; ++j)
		{
			sortedTemplate.set(iterp->get(j), i, j);
		}
	}

	ColVector b(rows);

	fprintf(fp, "set terminal postscript enhanced color\n");

	char filename[NAME_SIZE+10];
	sprintf(filename, "%s%s.eps", imageDir, outputFileName);
	fprintf(fp, "set output '%s'\n", filename);

	fprintf(fp, "set style line 1 linecolor rgb \"blue\"\n");
	fprintf(fp, "set autoscale\n");
	fprintf(fp, "unset label\n");
	fprintf(fp, "set xtic auto\n");
	fprintf(fp, "set ytic auto\n");
	fprintf(fp, "set xlabel \"%s\"\n", stateVarNames[outputAxes[0]].c_str());
	fprintf(fp, "set ylabel \"%s\"\n", stateVarNames[outputAxes[1]].c_str());
	fprintf(fp, "plot '-' notitle with lines ls 1\n");

	std::vector<Interval> step_exp_table;
	Interval I(1);
	step_exp_table.push_back(I);

	// Compute the intersections of two facets.
	// The vertices are ordered clockwisely.

	gsl_matrix *C = gsl_matrix_alloc(2,2);
	gsl_vector *d = gsl_vector_alloc(2);
	gsl_vector *vertex = gsl_vector_alloc(2);

	std::list<std::list<TaylorModelVec> >::const_iterator fpIter = flowpipes.begin();
	std::list<std::list<std::vector<Interval> > >::const_iterator fpdoIter = domains.begin();
	std::list<int>::const_iterator modeIter = modeIDs.begin();
	std::list<std::list<int> >::const_iterator fpSafetyIter = flowpipes_safety.begin();

	std::list<TaylorModelVec>::const_iterator tmvIter;
	std::list<std::vector<Interval> >::const_iterator doIter;
	std::list<int>::const_iterator safetyIter;

	int prog = 0, total_size = 0;

	for(; fpSafetyIter!=flowpipes_safety.end(); ++fpSafetyIter)
	{
		total_size += fpSafetyIter->size();
	}

	fpSafetyIter = flowpipes_safety.begin();

	for(; fpIter!=flowpipes.end(); ++fpIter, ++fpdoIter, ++modeIter)
	{
		tmvIter = fpIter->begin();
		doIter = fpdoIter->begin();
		safetyIter = fpSafetyIter->begin();

		for(; tmvIter != fpIter->end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			int rangeDim = tmvIter->tms.size();
			std::vector<Interval> box;
			tmvIter->intEval(box, *doIter);

			std::vector<Interval> new_domain;
			std::vector<bool> bVecTemp;
			TaylorModelVec tmvInterval(box, new_domain);

			Interval cutoff_threshold;
			if(local_settings[*modeIter].cutoff_threshold.sup() > 0)
			{
				cutoff_threshold = local_settings[*modeIter].cutoff_threshold;
			}
			else
			{
				cutoff_threshold = global_setting.cutoff_threshold;
			}

			int type = contract_interval_arithmetic(tmvInterval, new_domain, system.invariants[*modeIter], bVecTemp, cutoff_threshold);

			if(type < 0)
			{
				continue;
			}

			tmvInterval.intEval(box, new_domain);

			// the box template
			b.set(box[x].sup(), 0);
			b.set(box[y].sup(), 2);
			b.set(-box[x].inf(), 4);
			b.set(-box[y].inf(), 6);

			// consider the other vectors
			Matrix other_vectors(rangeDim, rangeDim+1);
			for(int i=0; i<rangeDim; ++i)
			{
				if(i != x && i != y)
				{
					other_vectors.set(1, i, i+1);
				}
			}

			other_vectors.set(1/sqrt(2), x, x+1);
			other_vectors.set(1/sqrt(2), x, y+1);
			other_vectors.set(1/sqrt(2), y, x+1);
			other_vectors.set(-1/sqrt(2), y, y+1);

			TaylorModelVec tmv_other_vectors(other_vectors);

			for(int i=0; i<rangeDim; ++i)
			{
				RowVector rowVecTemp(rangeDim);

				for(int j=0; j<rangeDim; ++j)
				{
					rowVecTemp.set(other_vectors.get(i,j+1), j);
				}

				Interval intTemp = rho(*tmvIter, rowVecTemp, *doIter);
				new_domain[i+1].setSup(intTemp);

				rowVecTemp.neg_assign();

				intTemp = rho(*tmvIter, rowVecTemp, *doIter);
				intTemp.inv_assign();
				new_domain[i+1].setInf(intTemp);
			}

			type = contract_interval_arithmetic(tmv_other_vectors, new_domain, system.invariants[*modeIter], bVecTemp, cutoff_threshold);

			if(type < 0)
			{
				continue;
			}


			RowVector template_vector_1(rangeDim);
			template_vector_1.set(1/sqrt(2), x);
			template_vector_1.set(1/sqrt(2), y);

			double sp = (rho(tmv_other_vectors, template_vector_1, new_domain)).sup();
			double sp2 = (1/sqrt(2))*b.get(0) + (1/sqrt(2))*b.get(2);
			b.set(sp, 1);


			RowVector template_vector_3(rangeDim);
			template_vector_3.set(-1/sqrt(2), x);
			template_vector_3.set(1/sqrt(2), y);

			sp = (rho(tmv_other_vectors, template_vector_3, new_domain)).sup();
			b.set(sp, 3);


			RowVector template_vector_5(rangeDim);
			template_vector_5.set(-1/sqrt(2), x);
			template_vector_5.set(-1/sqrt(2), y);

			sp = (rho(tmv_other_vectors, template_vector_5, new_domain)).sup();
			b.set(sp, 5);


			RowVector template_vector_7(rangeDim);
			template_vector_7.set(1/sqrt(2), x);
			template_vector_7.set(-1/sqrt(2), y);

			sp = (rho(tmv_other_vectors, template_vector_7, new_domain)).sup();
			b.set(sp, 7);

			Polyhedron polyTemplate(sortedTemplate, b);
			polyTemplate.tightenConstraints();

			double f1, f2;

			std::vector<LinearConstraint>::iterator iterp, iterq;
			iterp = iterq = polyTemplate.constraints.begin();
			++iterq;

			for(; iterq != polyTemplate.constraints.end(); ++iterp, ++iterq)
			{
				gsl_matrix_set(C, 0, 0, iterp->A[x].midpoint());
				gsl_matrix_set(C, 0, 1, iterp->A[y].midpoint());
				gsl_matrix_set(C, 1, 0, iterq->A[x].midpoint());
				gsl_matrix_set(C, 1, 1, iterq->A[y].midpoint());

				gsl_vector_set(d, 0, iterp->B.midpoint());
				gsl_vector_set(d, 1, iterq->B.midpoint());

				gsl_linalg_HH_solve(C, d, vertex);

				double v1 = gsl_vector_get(vertex, 0);
				double v2 = gsl_vector_get(vertex, 1);

				if(iterp == polyTemplate.constraints.begin())
				{
					f1 = v1;
					f2 = v2;
				}

				fprintf(fp, "%lf %lf\n", v1, v2);
			}

			iterp = polyTemplate.constraints.begin();
			--iterq;

			gsl_matrix_set(C, 0, 0, iterp->A[x].midpoint());
			gsl_matrix_set(C, 0, 1, iterp->A[y].midpoint());
			gsl_matrix_set(C, 1, 0, iterq->A[x].midpoint());
			gsl_matrix_set(C, 1, 1, iterq->A[y].midpoint());

			gsl_vector_set(d, 0, iterp->B.midpoint());
			gsl_vector_set(d, 1, iterq->B.midpoint());

			gsl_linalg_HH_solve(C, d, vertex);

			double v1 = gsl_vector_get(vertex, 0);
			double v2 = gsl_vector_get(vertex, 1);

			fprintf(fp, "%lf %lf\n", v1, v2);

			fprintf(fp, "%lf %lf\n", f1, f2);
			fprintf(fp, "\n\n");

			++prog;
			printf("\b\b\b\b");
			printf(BOLD_FONT "%%" RESET_COLOR);
			printf(BOLD_FONT "%3d" RESET_COLOR, (int)(prog*100/total_size));
			fflush(stdout);

			if(*safetyIter == UNSAFE)
			{
				break;
			}
		}

		if(*safetyIter == UNSAFE)
		{
			break;
		}
	}

	fprintf(fp, "e\n");
	printf("\b\b\b\b");
	printf(BOLD_FONT "%%100\n" RESET_COLOR);
	fflush(stdout);
}

void HybridReachability::plot_2D_grid_GNUPLOT(FILE *fp, const bool bProjected) const
{
	fprintf(fp, "set terminal postscript enhanced color\n");

	char filename[NAME_SIZE+10];
	sprintf(filename, "%s%s.eps", imageDir, outputFileName);
	fprintf(fp, "set output '%s'\n", filename);

	fprintf(fp, "set style line 1 linecolor rgb \"blue\"\n");
	fprintf(fp, "set autoscale\n");
	fprintf(fp, "unset label\n");
	fprintf(fp, "set xtic auto\n");
	fprintf(fp, "set ytic auto\n");
	fprintf(fp, "set xlabel \"%s\"\n", stateVarNames[outputAxes[0]].c_str());
	fprintf(fp, "set ylabel \"%s\"\n", stateVarNames[outputAxes[1]].c_str());
	fprintf(fp, "plot '-' notitle with lines ls 1\n");

	std::list<std::list<TaylorModelVec> >::const_iterator fpIter = flowpipes.begin();
	std::list<std::list<std::vector<Interval> > >::const_iterator fpdoIter = domains.begin();
	std::list<int>::const_iterator modeIter = modeIDs.begin();
	std::list<std::list<int> >::const_iterator fpSafetyIter = flowpipes_safety.begin();

	std::list<TaylorModelVec>::const_iterator tmvIter;
	std::list<std::vector<Interval> >::const_iterator doIter;
	std::list<int>::const_iterator safetyIter;

	int prog = 0, total_size = 0;

	for(; fpSafetyIter!=flowpipes_safety.end(); ++fpSafetyIter)
	{
		total_size += fpSafetyIter->size();
	}

	fpSafetyIter = flowpipes_safety.begin();

	for(; fpIter!=flowpipes.end(); ++fpIter, ++fpdoIter, ++modeIter)
	{
		tmvIter = fpIter->begin();
		doIter = fpdoIter->begin();
		safetyIter = fpSafetyIter->begin();

		int domainDim = doIter->size();

		for(; tmvIter != fpIter->end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			// decompose the domain
			std::list<std::vector<Interval> > grids;

			gridBox(grids, *doIter, numSections);

			// Transform the Taylor model into a Horner form
			std::vector<HornerForm> tmvHF;
			std::vector<Interval> remainders;
			int rangeDim = tmvIter->tms.size();

			for(int i=0; i<rangeDim; ++i)
			{
				HornerForm hfTemp;
				Interval intTemp;
				tmvIter->tms[i].toHornerForm(hfTemp, intTemp);
				tmvHF.push_back(hfTemp);
				remainders.push_back(intTemp);
			}

			// evaluate the images from all of the grids
			std::list<std::vector<Interval> >::const_iterator gIter = grids.begin();
			for(; gIter!=grids.end(); ++gIter)
			{
				std::vector<Interval> box;

				for(int i=0; i<rangeDim; ++i)
				{
					Interval intTemp;
					tmvHF[i].intEval(intTemp, *gIter);
					intTemp += remainders[i];
					box.push_back(intTemp);
				}

				// contract the interval according to the invariant
				std::vector<Interval> new_domain;
				std::vector<bool> bVecTemp;
				TaylorModelVec tmvInterval(box, new_domain);

				Interval cutoff_threshold;
				if(local_settings[*modeIter].cutoff_threshold.sup() > 0)
				{
					cutoff_threshold = local_settings[*modeIter].cutoff_threshold;
				}
				else
				{
					cutoff_threshold = global_setting.cutoff_threshold;
				}

				int type = contract_interval_arithmetic(tmvInterval, new_domain, system.invariants[*modeIter], bVecTemp, cutoff_threshold);

				if(type < 0)
				{
					continue;
				}

				tmvInterval.intEval(box, new_domain);

				Interval X(box[outputAxes[0]]), Y(box[outputAxes[1]]);

				// output the vertices
				fprintf(fp, "%lf %lf\n", X.inf(), Y.inf());
				fprintf(fp, "%lf %lf\n", X.sup(), Y.inf());
				fprintf(fp, "%lf %lf\n", X.sup(), Y.sup());
				fprintf(fp, "%lf %lf\n", X.inf(), Y.sup());
				fprintf(fp, "%lf %lf\n", X.inf(), Y.inf());
				fprintf(fp, "\n\n");
			}

			++prog;
			printf("\b\b\b\b");
			printf(BOLD_FONT "%%" RESET_COLOR);
			printf(BOLD_FONT "%3d" RESET_COLOR, (int)(prog*100/total_size));
			fflush(stdout);

			if(*safetyIter == UNSAFE)
			{
				break;
			}
		}

		if(*safetyIter == UNSAFE)
		{
			break;
		}
	}

	fprintf(fp, "e\n");
	printf("\b\b\b\b");
	printf(BOLD_FONT "%%100\n" RESET_COLOR);
	fflush(stdout);
}

void HybridReachability::plot_2D_MATLAB(FILE *fp, const bool bProjected) const
{
	switch(plotSetting)
	{
	case PLOT_INTERVAL:
		plot_2D_interval_MATLAB(fp, bProjected);
		break;
	case PLOT_OCTAGON:
		plot_2D_octagon_MATLAB(fp, bProjected);
		break;
	case PLOT_GRID:
		plot_2D_grid_MATLAB(fp, bProjected);
		break;
	}
}

void HybridReachability::plot_2D_interval_MATLAB(FILE *fp, const bool bProjected) const
{
	std::list<std::list<TaylorModelVec> >::const_iterator fpIter = flowpipes.begin();
	std::list<std::list<std::vector<Interval> > >::const_iterator fpdoIter = domains.begin();
	std::list<int>::const_iterator modeIter = modeIDs.begin();
	std::list<std::list<int> >::const_iterator fpSafetyIter = flowpipes_safety.begin();

	std::list<TaylorModelVec>::const_iterator tmvIter;
	std::list<std::vector<Interval> >::const_iterator doIter;
	std::list<int>::const_iterator safetyIter;

	Interval intZero;

	int prog = 0, total_size = 0;

	for(; fpSafetyIter!=flowpipes_safety.end(); ++fpSafetyIter)
	{
		total_size += fpSafetyIter->size();
	}

	fpSafetyIter = flowpipes_safety.begin();

	for(; fpSafetyIter!=flowpipes_safety.end(); ++fpIter, ++fpdoIter, ++modeIter, ++fpSafetyIter)
	{
		tmvIter = fpIter->begin();
		doIter = fpdoIter->begin();
		safetyIter = fpSafetyIter->begin();

		for(; safetyIter != fpSafetyIter->end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			std::vector<Interval> box;
			tmvIter->intEval(box, *doIter);

			// contract the interval according to the invariant
			std::vector<Interval> new_domain;
			std::vector<bool> bVecTemp;
			TaylorModelVec tmvInterval(box, new_domain);

			Interval cutoff_threshold;
			if(local_settings[*modeIter].cutoff_threshold.sup() > 0)
			{
				cutoff_threshold = local_settings[*modeIter].cutoff_threshold;
			}
			else
			{
				cutoff_threshold = global_setting.cutoff_threshold;
			}

			int type = contract_interval_arithmetic(tmvInterval, new_domain, system.invariants[*modeIter], bVecTemp, cutoff_threshold);

			if(type < 0)
			{
				continue;
			}

			tmvInterval.intEval(box, new_domain);

			Interval X(box[outputAxes[0]]), Y(box[outputAxes[1]]);

			switch(*safetyIter)
			{
			case SAFE:
				fprintf(fp,"plot( [%lf,%lf,%lf,%lf,%lf] , [%lf,%lf,%lf,%lf,%lf] , 'color' , '[0 0.4 0]');\nhold on;\nclear;\n",
						X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
				break;
			case UNSAFE:
				fprintf(fp,"plot( [%lf,%lf,%lf,%lf,%lf] , [%lf,%lf,%lf,%lf,%lf] , 'color' , '[1 0 0]');\nhold on;\nclear;\n",
						X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
				break;
			case UNKNOWN:
				fprintf(fp,"plot( [%lf,%lf,%lf,%lf,%lf] , [%lf,%lf,%lf,%lf,%lf] , 'color' , '[0 0 1]');\nhold on;\nclear;\n",
						X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
				break;
			}

			++prog;
			printf("\b\b\b\b");
			printf(BOLD_FONT "%%" RESET_COLOR);
			printf(BOLD_FONT "%3d" RESET_COLOR, (int)(prog*100/total_size));
			fflush(stdout);

			if(*safetyIter == UNSAFE)
			{
				break;
			}
		}

		if(*safetyIter == UNSAFE)
		{
			break;
		}
	}

	printf("\b\b\b\b");
	printf(BOLD_FONT "%%100\n" RESET_COLOR);
	fflush(stdout);
}

void HybridReachability::plot_2D_octagon_MATLAB(FILE *fp, const bool bProjected) const
{
	int x = outputAxes[0];
	int y = outputAxes[1];

	Interval intZero;

	int rangeDim = stateVarNames.size();
	Matrix output_poly_temp(8, rangeDim);

	output_poly_temp.set(1, 0, x);
	output_poly_temp.set(1, 1, y);
	output_poly_temp.set(-1, 2, x);
	output_poly_temp.set(-1, 3, y);
	output_poly_temp.set(1/sqrt(2), 4, x);
	output_poly_temp.set(1/sqrt(2), 4, y);
	output_poly_temp.set(1/sqrt(2), 5, x);
	output_poly_temp.set(-1/sqrt(2), 5, y);
	output_poly_temp.set(-1/sqrt(2), 6, x);
	output_poly_temp.set(1/sqrt(2), 6, y);
	output_poly_temp.set(-1/sqrt(2), 7, x);
	output_poly_temp.set(-1/sqrt(2), 7, y);

	// Construct the 2D template matrix.
	int rows = 8;
	int cols = rangeDim;

	Matrix sortedTemplate(rows, cols);
	RowVector rowVec(cols);
	std::vector<RowVector> sortedRows;
	std::vector<RowVector>::iterator iterp, iterq;

	output_poly_temp.row(rowVec, 0);
	sortedRows.push_back(rowVec);

	bool bInserted;

	// Sort the row vectors in the template by anti-clockwise order (only in the x-y space).
	for(int i=1; i<rows; ++i)
	{
		iterp = sortedRows.begin();
		iterq = iterp;
		++iterq;
		bInserted = false;

		for(; iterq != sortedRows.end();)
		{
			double tmp1 = output_poly_temp.get(i,x) * iterp->get(y) - output_poly_temp.get(i,y) * iterp->get(x);
			double tmp2 = output_poly_temp.get(i,x) * iterq->get(y) - output_poly_temp.get(i,y) * iterq->get(x);

			if(tmp1 < 0 && tmp2 > 0)
			{
				output_poly_temp.row(rowVec, i);
				sortedRows.insert(iterq, rowVec);
				bInserted = true;
				break;
			}
			else
			{
				++iterp;
				++iterq;
			}
		}

		if(!bInserted)
		{
			output_poly_temp.row(rowVec, i);
			sortedRows.push_back(rowVec);
		}
	}

	iterp = sortedRows.begin();
	for(int i=0; i<rows; ++i, ++iterp)
	{
		for(int j=0; j<cols; ++j)
		{
			sortedTemplate.set(iterp->get(j), i, j);
		}
	}

	ColVector b(rows);

	std::vector<Interval> step_exp_table;
	Interval I(1);
	step_exp_table.push_back(I);

	// Compute the intersections of two facets.
	// The vertices are ordered clockwisely.

	gsl_matrix *C = gsl_matrix_alloc(2,2);
	gsl_vector *d = gsl_vector_alloc(2);
	gsl_vector *vertex = gsl_vector_alloc(2);

	std::list<std::list<TaylorModelVec> >::const_iterator fpIter = flowpipes.begin();
	std::list<std::list<std::vector<Interval> > >::const_iterator fpdoIter = domains.begin();
	std::list<int>::const_iterator modeIter = modeIDs.begin();
	std::list<std::list<int> >::const_iterator fpSafetyIter = flowpipes_safety.begin();

	std::list<TaylorModelVec>::const_iterator tmvIter;
	std::list<std::vector<Interval> >::const_iterator doIter;
	std::list<int>::const_iterator safetyIter;

	int prog = 0, total_size = 0;

	for(; fpSafetyIter!=flowpipes_safety.end(); ++fpSafetyIter)
	{
		total_size += fpSafetyIter->size();
	}

	fpSafetyIter = flowpipes_safety.begin();

	for(; fpSafetyIter!=flowpipes_safety.end(); ++fpIter, ++fpdoIter, ++modeIter, ++fpSafetyIter)
	{
		tmvIter = fpIter->begin();
		doIter = fpdoIter->begin();
		safetyIter = fpSafetyIter->begin();

		for(; safetyIter != fpSafetyIter->end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			int rangeDim = tmvIter->tms.size();
			std::vector<Interval> box;
			tmvIter->intEval(box, *doIter);

			std::vector<Interval> new_domain;
			std::vector<bool> bVecTemp;
			TaylorModelVec tmvInterval(box, new_domain);

			Interval cutoff_threshold;
			if(local_settings[*modeIter].cutoff_threshold.sup() > 0)
			{
				cutoff_threshold = local_settings[*modeIter].cutoff_threshold;
			}
			else
			{
				cutoff_threshold = global_setting.cutoff_threshold;
			}

			int type = contract_interval_arithmetic(tmvInterval, new_domain, system.invariants[*modeIter], bVecTemp, cutoff_threshold);

			if(type < 0)
			{
				continue;
			}

			tmvInterval.intEval(box, new_domain);

			// the box template
			b.set(box[x].sup(), 0);
			b.set(box[y].sup(), 2);
			b.set(-box[x].inf(), 4);
			b.set(-box[y].inf(), 6);

			// consider the other vectors
			Matrix other_vectors(rangeDim, rangeDim+1);
			for(int i=0; i<rangeDim; ++i)
			{
				if(i != x && i != y)
				{
					other_vectors.set(1, i, i+1);
				}
			}

			other_vectors.set(1/sqrt(2), x, x+1);
			other_vectors.set(1/sqrt(2), x, y+1);
			other_vectors.set(1/sqrt(2), y, x+1);
			other_vectors.set(-1/sqrt(2), y, y+1);

			TaylorModelVec tmv_other_vectors(other_vectors);

			for(int i=0; i<rangeDim; ++i)
			{

				RowVector rowVecTemp(rangeDim);

				for(int j=0; j<rangeDim; ++j)
				{
					rowVecTemp.set(other_vectors.get(i,j+1), j);
				}

				Interval intTemp = rho(*tmvIter, rowVecTemp, *doIter);
				new_domain[i+1].setSup(intTemp);

				rowVecTemp.neg_assign();

				intTemp = rho(*tmvIter, rowVecTemp, *doIter);
				intTemp.inv_assign();
				new_domain[i+1].setInf(intTemp);
			}

			type = contract_interval_arithmetic(tmv_other_vectors, new_domain, system.invariants[*modeIter], bVecTemp, cutoff_threshold);

			if(type < 0)
			{
				continue;
			}


			RowVector template_vector_1(rangeDim);
			template_vector_1.set(1/sqrt(2), x);
			template_vector_1.set(1/sqrt(2), y);

			double sp = (rho(tmv_other_vectors, template_vector_1, new_domain)).sup();
			double sp2 = (1/sqrt(2))*b.get(0) + (1/sqrt(2))*b.get(2);
			b.set(sp, 1);


			RowVector template_vector_3(rangeDim);
			template_vector_3.set(-1/sqrt(2), x);
			template_vector_3.set(1/sqrt(2), y);

			sp = (rho(tmv_other_vectors, template_vector_3, new_domain)).sup();
			b.set(sp, 3);


			RowVector template_vector_5(rangeDim);
			template_vector_5.set(-1/sqrt(2), x);
			template_vector_5.set(-1/sqrt(2), y);

			sp = (rho(tmv_other_vectors, template_vector_5, new_domain)).sup();
			b.set(sp, 5);


			RowVector template_vector_7(rangeDim);
			template_vector_7.set(1/sqrt(2), x);
			template_vector_7.set(-1/sqrt(2), y);

			sp = (rho(tmv_other_vectors, template_vector_7, new_domain)).sup();
			b.set(sp, 7);

			Polyhedron polyTemplate(sortedTemplate, b);
			polyTemplate.tightenConstraints();

			double f1, f2;

			std::vector<LinearConstraint>::iterator iterp, iterq;
			iterp = iterq = polyTemplate.constraints.begin();
			++iterq;

			std::vector<double> vertices_x, vertices_y;

			for(; iterq != polyTemplate.constraints.end(); ++iterp, ++iterq)
			{
				gsl_matrix_set(C, 0, 0, iterp->A[x].midpoint());
				gsl_matrix_set(C, 0, 1, iterp->A[y].midpoint());
				gsl_matrix_set(C, 1, 0, iterq->A[x].midpoint());
				gsl_matrix_set(C, 1, 1, iterq->A[y].midpoint());

				gsl_vector_set(d, 0, iterp->B.midpoint());
				gsl_vector_set(d, 1, iterq->B.midpoint());

				gsl_linalg_HH_solve(C, d, vertex);

				double v1 = gsl_vector_get(vertex, 0);
				double v2 = gsl_vector_get(vertex, 1);

				if(iterp == polyTemplate.constraints.begin())
				{
					f1 = v1;
					f2 = v2;
				}

				vertices_x.push_back(v1);
				vertices_y.push_back(v2);
			}

			iterp = polyTemplate.constraints.begin();
			--iterq;

			gsl_matrix_set(C, 0, 0, iterp->A[x].midpoint());
			gsl_matrix_set(C, 0, 1, iterp->A[y].midpoint());
			gsl_matrix_set(C, 1, 0, iterq->A[x].midpoint());
			gsl_matrix_set(C, 1, 1, iterq->A[y].midpoint());

			gsl_vector_set(d, 0, iterp->B.midpoint());
			gsl_vector_set(d, 1, iterq->B.midpoint());

			gsl_linalg_HH_solve(C, d, vertex);

			double v1 = gsl_vector_get(vertex, 0);
			double v2 = gsl_vector_get(vertex, 1);

			vertices_x.push_back(v1);
			vertices_y.push_back(v2);
			vertices_x.push_back(f1);
			vertices_y.push_back(f2);

			fprintf(fp, "plot( ");

			fprintf(fp, "[ ");
			for(int i=0; i<vertices_x.size()-1; ++i)
			{
				fprintf(fp, "%lf , ", vertices_x[i]);
			}
			fprintf(fp, "%lf ] , ", vertices_x.back());

			fprintf(fp, "[ ");
			for(int i=0; i<vertices_y.size()-1; ++i)
			{
				fprintf(fp, "%lf , ", vertices_y[i]);
			}
			fprintf(fp, "%lf ] , ", vertices_y.back());

			switch(*safetyIter)
			{
			case SAFE:
				fprintf(fp, "'color' , '[0 0.4 0]');\nhold on;\nclear;\n");
				break;
			case UNSAFE:
				fprintf(fp, "'color' , '[1 0 0]');\nhold on;\nclear;\n");
				break;
			case UNKNOWN:
				fprintf(fp, "'color' , '[0 0 1]');\nhold on;\nclear;\n");
				break;
			}

			++prog;
			printf("\b\b\b\b");
			printf(BOLD_FONT "%%" RESET_COLOR);
			printf(BOLD_FONT "%3d" RESET_COLOR, (int)(prog*100/total_size));
			fflush(stdout);

			if(*safetyIter == UNSAFE)
			{
				break;
			}
		}

		if(*safetyIter == UNSAFE)
		{
			break;
		}
	}

	printf("\b\b\b\b");
	printf(BOLD_FONT "%%100\n" RESET_COLOR);
	fflush(stdout);
}

void HybridReachability::plot_2D_grid_MATLAB(FILE *fp, const bool bProjected) const
{
	std::list<std::list<TaylorModelVec> >::const_iterator fpIter = flowpipes.begin();
	std::list<std::list<std::vector<Interval> > >::const_iterator fpdoIter = domains.begin();
	std::list<int>::const_iterator modeIter = modeIDs.begin();
	std::list<std::list<int> >::const_iterator fpSafetyIter = flowpipes_safety.begin();

	std::list<TaylorModelVec>::const_iterator tmvIter;
	std::list<std::vector<Interval> >::const_iterator doIter;
	std::list<int>::const_iterator safetyIter;

	int prog = 0, total_size = 0;

	for(; fpSafetyIter!=flowpipes_safety.end(); ++fpSafetyIter)
	{
		total_size += fpSafetyIter->size();
	}

	fpSafetyIter = flowpipes_safety.begin();

	for(; fpSafetyIter!=flowpipes_safety.end(); ++fpIter, ++fpdoIter, ++modeIter, ++fpSafetyIter)
	{
		tmvIter = fpIter->begin();
		doIter = fpdoIter->begin();
		safetyIter = fpSafetyIter->begin();

		int domainDim = doIter->size();

		for(; safetyIter != fpSafetyIter->end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			// decompose the domain
			std::list<std::vector<Interval> > grids;

			gridBox(grids, *doIter, numSections);

			// Transform the Taylor model into a Horner form
			std::vector<HornerForm> tmvHF;
			std::vector<Interval> remainders;
			int rangeDim = tmvIter->tms.size();

			for(int i=0; i<rangeDim; ++i)
			{
				HornerForm hfTemp;
				Interval intTemp;
				tmvIter->tms[i].toHornerForm(hfTemp, intTemp);
				tmvHF.push_back(hfTemp);
				remainders.push_back(intTemp);
			}

			// evaluate the images from all of the grids
			std::list<std::vector<Interval> >::const_iterator gIter = grids.begin();
			for(; gIter!=grids.end(); ++gIter)
			{
				std::vector<Interval> box;

				for(int i=0; i<rangeDim; ++i)
				{
					Interval intTemp;
					tmvHF[i].intEval(intTemp, *gIter);
					intTemp += remainders[i];
					box.push_back(intTemp);
				}

				// contract the interval according to the invariant
				std::vector<Interval> new_domain;
				std::vector<bool> bVecTemp;
				TaylorModelVec tmvInterval(box, new_domain);

				Interval cutoff_threshold;
				if(local_settings[*modeIter].cutoff_threshold.sup() > 0)
				{
					cutoff_threshold = local_settings[*modeIter].cutoff_threshold;
				}
				else
				{
					cutoff_threshold = global_setting.cutoff_threshold;
				}

				int type = contract_interval_arithmetic(tmvInterval, new_domain, system.invariants[*modeIter], bVecTemp, cutoff_threshold);

				if(type < 0)
				{
					continue;
				}

				tmvInterval.intEval(box, new_domain);

				Interval X(box[outputAxes[0]]), Y(box[outputAxes[1]]);

				switch(*safetyIter)
				{
				case SAFE:
					fprintf(fp,"plot( [%lf,%lf,%lf,%lf,%lf] , [%lf,%lf,%lf,%lf,%lf] , 'color' , '[0 0.4 0]');\nhold on;\nclear;\n",
							X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
					break;
				case UNSAFE:
					fprintf(fp,"plot( [%lf,%lf,%lf,%lf,%lf] , [%lf,%lf,%lf,%lf,%lf] , 'color' , '[1 0 0]');\nhold on;\nclear;\n",
							X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
					break;
				case UNKNOWN:
					fprintf(fp,"plot( [%lf,%lf,%lf,%lf,%lf] , [%lf,%lf,%lf,%lf,%lf] , 'color' , '[0 0 1]');\nhold on;\nclear;\n",
							X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
					break;
				}
			}

			++prog;
			printf("\b\b\b\b");
			printf(BOLD_FONT "%%" RESET_COLOR);
			printf(BOLD_FONT "%3d" RESET_COLOR, (int)(prog*100/total_size));
			fflush(stdout);

			if(*safetyIter == UNSAFE)
			{
				break;
			}
		}

		if(*safetyIter == UNSAFE)
		{
			break;
		}
	}

	printf("\b\b\b\b");
	printf(BOLD_FONT "%%100\n" RESET_COLOR);
	fflush(stdout);
}

bool HybridReachability::declareStateVar(const std::string & vName)
{
	std::map<std::string,int>::const_iterator iter;

	if((iter = stateVarTab.find(vName)) == stateVarTab.end())
	{
		stateVarTab[vName] = stateVarNames.size();
		stateVarNames.push_back(vName);
		return true;
	}
	else
	{
		return false;
	}
}

int HybridReachability::getIDForStateVar(const std::string & vName) const
{
	std::map<std::string,int>::const_iterator iter;
	if((iter = stateVarTab.find(vName)) == stateVarTab.end())
	{
		return -1;
	}

	return iter->second;
}

bool HybridReachability::getStateVarName(std::string & vName, int id) const
{
	if(id >= 0 && id < stateVarNames.size())
	{
		vName = stateVarNames[id];
		return true;
	}
	else
	{
		return false;
	}
}

bool HybridReachability::declareTMVar(const std::string & vName)
{
	std::map<std::string,int>::const_iterator iter;

	if((iter = tmVarTab.find(vName)) == tmVarTab.end())
	{
		tmVarTab[vName] = tmVarNames.size();
		tmVarNames.push_back(vName);
		return true;
	}
	else
	{
		return false;
	}
}

int HybridReachability::getIDForTMVar(const std::string & vName) const
{
	std::map<std::string,int>::const_iterator iter;
	if((iter = tmVarTab.find(vName)) == tmVarTab.end())
	{
		return -1;
	}

	return iter->second;
}

bool HybridReachability::getTMVarName(std::string & vName, const int id) const
{
	if(id >= 0 && id < tmVarNames.begin()->size())
	{
		vName = (*tmVarNames.begin())[id];
		return true;
	}
	else
	{
		return false;
	}
}

bool HybridReachability::declarePar(const std::string & pName, const Interval & range)
{
	std::map<std::string,int>::const_iterator iter;

	if((iter = parTab.find(pName)) == parTab.end())
	{
		parTab[pName] = parNames.size();
		parNames.push_back(pName);
		parRanges.push_back(range);
		return true;
	}
	else
	{
		return false;
	}
}

int HybridReachability::getIDForPar(const std::string & pName) const
{
	std::map<std::string,int>::const_iterator iter;
	if((iter = parTab.find(pName)) == parTab.end())
	{
		return -1;
	}

	return iter->second;
}

bool HybridReachability::getParName(std::string & pName, int id) const
{
	if(id >= 0 && id < parNames.size())
	{
		pName = parNames[id];
		return true;
	}
	else
	{
		return false;
	}
}

bool HybridReachability::getRangeForPar(Interval & range, const std::string & pName) const
{
	int id = getIDForPar(pName);

	if(id == -1)
	{
		return false;
	}
	else
	{
		range = parRanges[id];
		return true;
	}
}


bool HybridReachability::declareMode(const std::string & mName, const LTI_Dynamics & lti_dyn, const std::vector<PolynomialConstraint> & inv, const ReachabilitySetting & local_setting)
{
	std::map<std::string,int>::const_iterator iter;

	if((iter = modeTab.find(mName)) == modeTab.end())
	{
		int modeID = modeNames.size();
		modeTab[mName] = modeID;
		modeNames.push_back(mName);
		system.modes.push_back(modeID);

		system.dyn_class.push_back(LTI_DYN);
		system.lti_dynamics.push_back(lti_dyn);
		system.invariants.push_back(inv);

		integrationSchemes.push_back(LTI);
		local_settings.push_back(local_setting);

		// for polynomial ODEs
		TaylorModelVec odeEmpty;
		std::vector<HornerForm> hfOdeEmpty;
		system.odes.push_back(odeEmpty);
		system.hfOdes.push_back(hfOdeEmpty);
		system.odes_centered.push_back(odeEmpty);
		system.hfOdes_centered.push_back(hfOdeEmpty);

		// for non-polynomial ODEs
		std::vector<std::string> strOdeEmpty;
		system.strOdes.push_back(strOdeEmpty);
		system.strOdes_centered.push_back(strOdeEmpty);

		std::vector<bool> mode_constant;
		system.constant.push_back(mode_constant);

		// for LTV ODEs
		LTV_Dynamics ltv_dyn;
		system.ltv_dynamics.push_back(ltv_dyn);

		return true;
	}
	else
	{
		return false;
	}
}

bool HybridReachability::declareMode(const std::string & mName, const LTV_Dynamics & ltv_dyn, const std::vector<PolynomialConstraint> & inv, const ReachabilitySetting & local_setting)
{
	std::map<std::string,int>::const_iterator iter;

	if((iter = modeTab.find(mName)) == modeTab.end())
	{
		int modeID = modeNames.size();
		modeTab[mName] = modeID;
		modeNames.push_back(mName);
		system.modes.push_back(modeID);

		system.dyn_class.push_back(LTV_DYN);
		system.ltv_dynamics.push_back(ltv_dyn);
		system.invariants.push_back(inv);

		integrationSchemes.push_back(LTV);
		local_settings.push_back(local_setting);

		// for polynomial ODEs
		TaylorModelVec odeEmpty;
		std::vector<HornerForm> hfOdeEmpty;
		system.odes.push_back(odeEmpty);
		system.hfOdes.push_back(hfOdeEmpty);
		system.odes_centered.push_back(odeEmpty);
		system.hfOdes_centered.push_back(hfOdeEmpty);

		// for non-polynomial ODEs
		std::vector<std::string> strOdeEmpty;
		system.strOdes.push_back(strOdeEmpty);
		system.strOdes_centered.push_back(strOdeEmpty);

		std::vector<bool> mode_constant;
		system.constant.push_back(mode_constant);

		// for LTI ODEs
		LTI_Dynamics lti_dyn;
		system.lti_dynamics.push_back(lti_dyn);

		return true;
	}
	else
	{
		return false;
	}
}


bool HybridReachability::declareMode(const std::string & mName, const TaylorModelVec & ode, const std::vector<PolynomialConstraint> & inv, const int integrationScheme, const ReachabilitySetting & local_setting)
{
	std::map<std::string,int>::const_iterator iter;
	Interval intZero;

	int rangeDim = ode.tms.size();
	TaylorModelVec tmvEmpty;
	std::vector<HornerForm> hfsEmpty;
	std::vector<bool> mode_constant;

	if((iter = modeTab.find(mName)) == modeTab.end())
	{
		int modeID = modeNames.size();
		modeTab[mName] = modeID;
		modeNames.push_back(mName);

		system.modes.push_back(modeID);

		TaylorModelVec tmvTemp = ode;
		std::vector<HornerForm> hfOde;

		TaylorModelVec tmvTemp_centered = ode;
		tmvTemp_centered.center_nc();

		std::vector<HornerForm> hfOde_centered;

		for(int i=0; i<tmvTemp.tms.size(); ++i)
		{
			HornerForm hf;
			tmvTemp.tms[i].expansion.toHornerForm(hf);
			hfOde.push_back(hf);

			if(tmvTemp.tms[i].expansion.degree() == 0)
			{
				mode_constant.push_back(true);
			}
			else
			{
				mode_constant.push_back(false);
			}
		}

		for(int i=0; i<tmvTemp.tms.size(); ++i)
		{
			HornerForm hf;
			tmvTemp_centered.tms[i].expansion.toHornerForm(hf);
			hfOde_centered.push_back(hf);
		}

		system.odes.push_back(tmvTemp);
		system.hfOdes.push_back(hfOde);
		system.odes_centered.push_back(tmvTemp_centered);
		system.hfOdes_centered.push_back(hfOde_centered);
		system.invariants.push_back(inv);

		system.constant.push_back(mode_constant);

		system.dyn_class.push_back(POLY_DYN);


		// for non-polynomial ODE
		std::vector<std::string> odeEmpty;
		system.strOdes.push_back(odeEmpty);
		system.strOdes_centered.push_back(odeEmpty);

		// for LTI ODE
		LTI_Dynamics lti_dyn;
		system.lti_dynamics.push_back(lti_dyn);

		// for LTV ODE
		LTV_Dynamics ltv_dyn;
		system.ltv_dynamics.push_back(ltv_dyn);


		integrationSchemes.push_back(integrationScheme);

		local_settings.push_back(local_setting);

		return true;
	}
	else
	{
		return false;
	}
}

bool HybridReachability::declareMode(const std::string & mName, const std::vector<std::string> & strOde, const std::vector<PolynomialConstraint> & inv, const int integrationScheme, const ReachabilitySetting & local_setting)
{
	std::map<std::string,int>::const_iterator iter;
	Interval intZero;

	std::vector<std::string> odeEmpty;

	if((iter = modeTab.find(mName)) == modeTab.end())
	{
		int modeID = modeNames.size();
		modeTab[mName] = modeID;
		modeNames.push_back(mName);

		system.modes.push_back(modeID);

		system.strOdes.push_back(strOde);

		std::vector<std::string> strOde_centered;
		std::string prefix(str_prefix_center);
		std::string suffix(str_suffix);

		std::vector<bool> mode_constant;
		std::vector<Interval> mode_strOde_constant;

		for(int i=0 ;i<strOde.size(); ++i)
		{
			parseSetting.clear();
			parseSetting.strODE = prefix + strOde[i] + suffix;
			parseResult.bConstant = true;

			parseODE();

			strOde_centered.push_back(parseResult.strExpansion);

			mode_constant.push_back(parseResult.bConstant);
			mode_strOde_constant.push_back(parseResult.constant);
		}

		system.strOdes_centered.push_back(strOde_centered);

		system.constant.push_back(mode_constant);

		system.strOde_constant.push_back(mode_strOde_constant);

		system.invariants.push_back(inv);

		system.dyn_class.push_back(NONPOLY_DYN);

		// for polynomial ODEs
		TaylorModelVec odeEmpty;
		std::vector<HornerForm> hfOdeEmpty;
		system.odes.push_back(odeEmpty);
		system.hfOdes.push_back(hfOdeEmpty);
		system.odes_centered.push_back(odeEmpty);
		system.hfOdes_centered.push_back(hfOdeEmpty);

		// for LTI ODE
		LTI_Dynamics lti_dyn;
		system.lti_dynamics.push_back(lti_dyn);

		// for LTV ODE
		LTV_Dynamics ltv_dyn;
		system.ltv_dynamics.push_back(ltv_dyn);

		integrationSchemes.push_back(integrationScheme);

		local_settings.push_back(local_setting);

		return true;
	}
	else
	{
		return false;
	}
}

int HybridReachability::getIDForMode(const std::string & mName) const
{
	std::map<std::string,int>::const_iterator iter;
	if((iter = modeTab.find (mName)) == modeTab.end())
	{
		return -1;
	}

	return iter->second;
}

bool HybridReachability::getModeName(std::string & mName, int id) const
{
	if(id>=0 && id<modeNames.size())
	{
		mName = modeNames[id];
		return true;
	}
	else
	{
		return false;
	}
}

void HybridReachability::declareTrans(const int start, const int end, const std::vector<PolynomialConstraint> & guard, const ResetMap & reset, const int aggregType, const std::vector<std::vector<double> > & candidates)
{
	DiscTrans transition(++numOfJumps, start, end, guard, reset);

	system.transitions[start].push_back(transition);
	aggregationType[start].push_back(aggregType);

	switch(aggregType)
	{
	case INTERVAL_AGGREG:
	{
		// put an empty template
		std::vector<RowVector> empty_template;
		aggregationTemplate_candidates[start].push_back(empty_template);
		break;
	}
	case PARA_AGGREG:
	{
		std::vector<RowVector> rowVecs;
		for(int i=0; i<candidates.size(); ++i)
		{
			RowVector rowVec(candidates[i].size());

			for(int j=0; j<candidates[i].size(); ++j)
			{
				rowVec.set(candidates[i][j], j);
			}

			rowVec.normalize();
			rowVecs.push_back(rowVec);
		}

		aggregationTemplate_candidates[start].push_back(rowVecs);
		break;
	}
	}
}

void HybridReachability::declareTrans()
{
	std::vector<DiscTrans> transVec;

	for(int i=0; i<system.modes.size(); ++i)
	{
		system.transitions.push_back(transVec);
	}

	// template types
	std::vector<int> iVec;

	for(int i=0; i<system.modes.size(); ++i)
	{
		aggregationType.push_back(iVec);
	}

	// parallelotopic templates
	std::vector<std::vector<RowVector> > paraTemplateVec;

	for(int i=0; i<system.modes.size(); ++i)
	{
		aggregationTemplate_candidates.push_back(paraTemplateVec);
	}
}

void HybridReachability::initialConfig(const int modeID, const Flowpipe & initialSet)
{
	system.initialMode = modeID;
	system.initialSet = initialSet;
}

void HybridReachability::set_default_template()
{
	int rangeDim = system.initialSet.tmv.tms.size();

	default_aggregation_template.clear();

	for(int i=0; i<rangeDim; ++i)
	{
		RowVector rowVec(rangeDim);
		rowVec.set(1, i);
		default_aggregation_template.push_back(rowVec);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=i+1; j<rangeDim; ++j)
		{
			RowVector rowVec(rangeDim);
			rowVec.set(1/sqrt(2), i);
			rowVec.set(1/sqrt(2), j);
			default_aggregation_template.push_back(rowVec);
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=i+1; j<rangeDim; ++j)
		{
			RowVector rowVec(rangeDim);
			rowVec.set(1/sqrt(2), i);
			rowVec.set(-1/sqrt(2), j);
			default_aggregation_template.push_back(rowVec);
		}
	}
}

void HybridReachability::constructWeightTab()
{
	int num_modes = system.modes.size();
	int rangeDim = default_aggregation_template[0].size();

	// construct the template information table
	std::vector<RowVector> rowVecs_empty;
	std::vector<bool> bVec_empty;
	std::vector<std::vector<bool> > bVecVec_temp;
	std::vector<std::vector<RowVector> > rowVecVecs_temp;

	for(int i=0; i<num_modes; ++i)
	{
		linear_auto.push_back(bVecVec_temp);
		template_auto.push_back(rowVecVecs_temp);
	}

	// construct the default weight table
	std::vector<Matrix> matVec;

	for(int i=0; i<num_modes; ++i)
	{
		weightTab.push_back(matVec);
	}

	// construct the weighted table for default template
	int num_default = default_aggregation_template.size();
	Matrix defaultWeightTab(num_default, num_default);
	for(int i=0; i<num_default; ++i)
	{
		for(int j=i+1; j<num_default; ++j)
		{
			// we assume that all the candidates and default template vectors are normalized
			double weight = 1 - fabs(default_aggregation_template[i].innerProd(default_aggregation_template[j]));
			defaultWeightTab.set(weight, i, j);
			defaultWeightTab.set(weight, j, i);
		}
	}

	std::vector<bool> linear_inv_empty;
	std::vector<RowVector> template_inv_empty;
	Matrix emptyMat(1);

	// construct the weighted table for every jump
	for(int i=0; i<num_modes; ++i)
	{
		int num_invariants = system.invariants[i].size();

		// extract the normal vectors from the linear invariant constraints
		std::vector<bool> linear_inv;
		std::vector<RowVector> template_inv;

		for(int j=0; j<num_invariants; ++j)
		{
			if(system.invariants[i][j].p.degree() == 1)
			{
				RowVector rowVec(rangeDim);
				system.invariants[i][j].p.constraintCoefficients(rowVec);
				rowVec.normalize();
				template_inv.push_back(rowVec);
				linear_inv.push_back(true);
			}
			else
			{
				RowVector rowVec(rangeDim);
				template_inv.push_back(rowVec);
				linear_inv.push_back(false);
			}
		}

		Matrix tabInv(1, 1);

		if(num_invariants > 0)
		{
			// table for invariant constraints
			Matrix tabInv_update(num_invariants, num_invariants);
			tabInv = tabInv_update;

			for(int j=0; j<num_invariants; ++j)
			{
				for(int k=j+1; k<num_invariants; ++k)
				{
					if(linear_inv[j] && linear_inv[k])
					{
						double weight = 1 - fabs(template_inv[j].innerProd(template_inv[k]));
						tabInv.set(weight, j, k);
						tabInv.set(weight, k, j);
					}
					else
					{
						tabInv.set(-1, j, k);
						tabInv.set(-1, k, j);
					}
				}
			}

	//		tabInv.output(stdout);
		}

		for(int j=0; j<system.transitions[i].size(); ++j)
		{
			if(aggregationType[i][j] != PARA_AGGREG)
			{
				linear_auto[i].push_back(linear_inv_empty);
				template_auto[i].push_back(template_inv_empty);
				weightTab[i].push_back(emptyMat);
				continue;
			}

			int num_candidates = aggregationTemplate_candidates[i][j].size();
			int auto_start = num_candidates;
			int guard_start = auto_start + num_invariants;
			int default_start = guard_start + system.transitions[i][j].guard.size();
			int weightMatSize = default_start + num_default;

//			printf("auto_start: %d,\tguard_start: %d,\tflow_start: %d,\tdefault_start: %d,\tdefault size: %d,\tmatrix size: %d\n", auto_start, guard_start, flow_start, default_start, num_default, weightMatSize);

			linear_auto[i].push_back(linear_inv);
			template_auto[i].push_back(template_inv);

			for(int k=0; k<system.transitions[i][j].guard.size(); ++k)
			{
				if(system.transitions[i][j].guard[k].p.degree() == 1)
				{
					RowVector rowVec(rangeDim);
					system.transitions[i][j].guard[k].p.constraintCoefficients(rowVec);
					rowVec.normalize();
					template_auto[i][j].push_back(rowVec);
					linear_auto[i][j].push_back(true);
				}
				else
				{
					RowVector rowVec(rangeDim);
					template_auto[i][j].push_back(rowVec);
					linear_auto[i][j].push_back(false);
				}
			}

			Matrix weightMat(weightMatSize, weightMatSize);
			// construct the whole weighted table
			for(int k=0; k<weightMatSize; ++k)
			{
				for(int m=k+1; m<weightMatSize; ++m)
				{
					if(k < auto_start)
					{
						// the first vector is a candidate

						if(m < auto_start)
						{
							// the second vector is a candidate
							double weight = 1 - fabs(aggregationTemplate_candidates[i][j][k].innerProd(aggregationTemplate_candidates[i][j][m]));
							weightMat.set(weight, k, m);
							weightMat.set(weight, m, k);
						}
						else if(m >= auto_start && m < default_start)
						{
							// the second vector is from the guard
							int posm = m - auto_start;
							if(linear_auto[i][j][posm])
							{
								double weight = 1 - fabs(aggregationTemplate_candidates[i][j][k].innerProd(template_auto[i][j][posm]));
								weightMat.set(weight, k, m);
								weightMat.set(weight, m, k);
							}
							else
							{
								weightMat.set(-1, k, m);
								weightMat.set(-1, m, k);
							}
						}
						else
						{
							// the second vector is from the default template
							double weight = 1 - fabs(aggregationTemplate_candidates[i][j][k].innerProd(default_aggregation_template[m - default_start]));
							weightMat.set(weight, k, m);
							weightMat.set(weight, m, k);
						}
					}
					else if(k >= auto_start && k < guard_start)
					{
						// the first vector is from the invariant
						int posk = k - auto_start;

						if(m >= auto_start && m < guard_start)
						{
							// the second vector is from the invariant
							int posm = m - auto_start;
							weightMat.set(tabInv.get(posk, posm), k, m);
							weightMat.set(tabInv.get(posm, posk), m, k);
						}
						else if(m >= guard_start && m < default_start)
						{
							// the second vector is from the guard
							int posm = m - auto_start;
							if(linear_auto[i][j][posk] && linear_auto[i][j][posm])
							{
								double weight = 1 - fabs(template_auto[i][j][posk].innerProd(template_auto[i][j][posm]));
								weightMat.set(weight, k, m);
								weightMat.set(weight, m, k);
							}
							else
							{
								weightMat.set(-1, k, m);
								weightMat.set(-1, m, k);
							}
						}
						else
						{
							// the second vector is from the default template
							if(linear_auto[i][j][posk])
							{
								double weight = 1 - fabs(template_auto[i][j][posk].innerProd(default_aggregation_template[m - default_start]));
								weightMat.set(weight, k, m);
								weightMat.set(weight, m, k);
							}
							else
							{
								weightMat.set(-1, k, m);
								weightMat.set(-1, m, k);
							}
						}
					}
					else if(k >= guard_start && k < default_start)
					{
						// the first vector is from the guard
						int posk = k - auto_start;

						if(m >= guard_start && m < default_start)
						{
							// the second vector is from the guard
							int posm = m - auto_start;
							if(linear_auto[i][j][posk] && linear_auto[i][j][posm])
							{
								double weight = 1 - fabs(template_auto[i][j][posk].innerProd(template_auto[i][j][posm]));
								weightMat.set(weight, k, m);
								weightMat.set(weight, m, k);
							}
							else
							{
								weightMat.set(-1, k, m);
								weightMat.set(-1, m, k);
							}
						}
						else
						{
							// the second vector is from the default template
							if(linear_auto[i][j][posk])
							{
								double weight = 1 - fabs(template_auto[i][j][posk].innerProd(default_aggregation_template[m - default_start]));
								weightMat.set(weight, k, m);
								weightMat.set(weight, m, k);
							}
							else
							{
								weightMat.set(-1, k, m);
								weightMat.set(-1, m, k);
							}
						}
					}
					else
					{
						// the first vector is from the default template
						int posk = k - default_start;
						int posm = m - default_start;

						double weight = 1 - fabs(default_aggregation_template[posk].innerProd(default_aggregation_template[posm]));
						weightMat.set(weight, k, m);
						weightMat.set(weight, m, k);
					}
				}
			}

			weightTab[i].push_back(weightMat);

//			weightMat.output(stdout); exit(0);// test work
		}
	}
}

/*
int HybridReachability::safetyChecking()
{
	int rangeDim = flowpipesCompo.front().front().tms.size();
	Interval intZero;

	bool bDumpCounterexamples = true;

	int mkres = mkdir(counterexampleDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if(mkres < 0 && errno != EEXIST)
	{
		printf("Can not create the directory for counterexamples.\n");
		bDumpCounterexamples = false;
	}

	char filename_counterexamples[NAME_SIZE+10];
	FILE *fpDumpCounterexamples;

	if(bDumpCounterexamples)
	{
		sprintf(filename_counterexamples, "%s%s%s", counterexampleDir, outputFileName, str_counterexample_dumping_name_suffix);
		fpDumpCounterexamples = fopen(filename_counterexamples, "w");
	}

	// we first check the intersection of the linear invariant constraints and linear unsafe constraints
	for(int i=0; i<unsafeSet.size(); ++i)
	{
		vector<LinearConstraint> lcs;

		for(int j=0; j<system.invariants[i].size(); ++j)
		{
			if(system.invariants[i][j].p.degree() == 1)
			{
				vector<Interval> intRowVec;

				for(int k=0; k<rangeDim; ++k)
				{
					intRowVec.push_back(intZero);
				}

				system.invariants[i][j].p.constraintCoefficients(intRowVec);
				LinearConstraint lc(intRowVec, system.invariants[i][j].B);
				lcs.push_back(lc);
			}
		}

		for(int j=0; j<unsafeSet[i].size(); ++j)
		{
			if(unsafeSet[i][j].p.degree() == 1)
			{
				vector<Interval> intRowVec;

				for(int k=0; k<rangeDim; ++k)
				{
					intRowVec.push_back(intZero);
				}

				unsafeSet[i][j].p.constraintCoefficients(intRowVec);
				LinearConstraint lc(intRowVec, unsafeSet[i][j].B);
				lcs.push_back(lc);
			}
		}

		Polyhedron P(lcs);

		if(bVecUnderCheck[i] && P.empty())
		{
			bVecUnderCheck[i] = false;
		}
	}

	// the main procedure of safety checking
	list<list<TaylorModelVec> >::const_iterator fpIter = flowpipesCompo.begin();
	list<list<vector<Interval> > >::const_iterator fpdoIter = domains.begin();
	list<int>::const_iterator modeIter = modeIDs.begin();
	list<TreeNode *>::const_iterator nodeIter = traceNodes.begin();

	vector<Interval> step_exp_table;

	list<TaylorModelVec>::const_iterator tmvIter;
	list<vector<Interval> >::const_iterator doIter;

	int maxOrder = 0;
	int result = SAFE;

	for(; fpIter!=flowpipesCompo.end(); ++fpIter, ++fpdoIter, ++modeIter, ++nodeIter)
	{
		if(!bVecUnderCheck[*modeIter])
		{
			continue;
		}

		tmvIter = fpIter->begin();
		doIter = fpdoIter->begin();

		if(unsafeSet[*modeIter].size() == 0)
		{
			return UNSAFE;
		}

		int domainDim = doIter->size();

		list<TaylorModelVec> flowpipe_counterexamples;
		list<vector<Interval> > counterexample_domains;
		list<Interval> localTimes;
		Interval localTime;

		for(; tmvIter!=fpIter->end(); ++tmvIter, ++doIter)
		{

			int tmp = maxOrder;
			for(int i=0; i<tmvIter->tms.size(); ++i)
			{
				int order = tmvIter->tms[i].expansion.degree();
				if(maxOrder < order)
				{
					maxOrder = order;
				}
			}

			if(step_exp_table.size() == 0 || step_exp_table[1] != (*doIter)[0] || maxOrder > tmp)
			{
				construct_step_exp_table(step_exp_table, (*doIter)[0], 2*maxOrder);
			}

			bool bsafe = false;

			vector<Interval> tmvPolyRange;
			tmvIter->polyRangeNormal(tmvPolyRange, step_exp_table);

			for(int i=0; i<unsafeSet[*modeIter].size(); ++i)
			{
				TaylorModel tmTemp;

				// interval evaluation on the constraint
				unsafeSet[*modeIter][i].hf.insert_normal(tmTemp, *tmvIter, tmvPolyRange, step_exp_table, domainDim);

				Interval intTemp;
				tmTemp.intEvalNormal(intTemp, step_exp_table);

				if(intTemp > unsafeSet[*modeIter][i].B)
				{
					// no intersection with the unsafe set
					bsafe = true;
					break;
				}
				else
				{
					continue;
				}
			}

			if(!bsafe)
			{
				// collect the skeptical counterexamples

				if(bDumpCounterexamples)
				{
					flowpipe_counterexamples.push_back(*tmvIter);
					counterexample_domains.push_back(*doIter);
					localTimes.push_back(localTime);
				}

				result = UNKNOWN;
			}

			localTime += (*doIter)[0];
		}

		if(bDumpCounterexamples && flowpipe_counterexamples.size() > 0)
		{
			// dump the skeptical counterexamples
			dump_potential_counterexample(fpDumpCounterexamples, flowpipe_counterexamples, counterexample_domains, *nodeIter, localTimes);
		}
	}

	if(bDumpCounterexamples)
	{
		fclose(fpDumpCounterexamples);
	}

	return result;
}
*/

int HybridReachability::safetyChecking()
{
	int rangeDim = flowpipes.front().front().tms.size();
	Interval intZero;

	bool bDumpCounterexamples = false;
	FILE *fpDumpCounterexamples;

	if(bDump)
	{
		bDumpCounterexamples = true;

		int mkres = mkdir(counterexampleDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
		if(mkres < 0 && errno != EEXIST)
		{
			printf("Can not create the directory for counterexamples.\n");
			bDumpCounterexamples = false;
		}

		char filename_counterexamples[NAME_SIZE+10];

		if(bDumpCounterexamples)
		{
			sprintf(filename_counterexamples, "%s%s%s", counterexampleDir, outputFileName, str_counterexample_dumping_name_suffix);
			fpDumpCounterexamples = fopen(filename_counterexamples, "w");
		}
	}

	// we first check the intersection of the linear invariant constraints and linear unsafe constraints
	for(int i=0; i<unsafeSet.size(); ++i)
	{
		std::vector<LinearConstraint> lcs;

		for(int j=0; j<system.invariants[i].size(); ++j)
		{
			if(system.invariants[i][j].p.degree() == 1)
			{
				std::vector<Interval> intRowVec;

				for(int k=0; k<rangeDim; ++k)
				{
					intRowVec.push_back(intZero);
				}

				system.invariants[i][j].p.constraintCoefficients(intRowVec);
				LinearConstraint lc(intRowVec, system.invariants[i][j].B);
				lcs.push_back(lc);
			}
		}

		for(int j=0; j<unsafeSet[i].size(); ++j)
		{
			if(unsafeSet[i][j].p.degree() == 1)
			{
				std::vector<Interval> intRowVec;

				for(int k=0; k<rangeDim; ++k)
				{
					intRowVec.push_back(intZero);
				}

				unsafeSet[i][j].p.constraintCoefficients(intRowVec);
				LinearConstraint lc(intRowVec, unsafeSet[i][j].B);
				lcs.push_back(lc);
			}
		}

		Polyhedron P(lcs);

		if(bVecUnderCheck[i] && P.empty())
		{
			bVecUnderCheck[i] = false;
		}
	}

	// the main procedure of safety checking
	std::list<std::list<TaylorModelVec> >::const_iterator fpIter = flowpipes.begin();
	std::list<std::list<std::vector<Interval> > >::const_iterator fpDoIter = domains.begin();
	std::list<std::list<bool> >::const_iterator fpConIter = flowpipes_contracted.begin();
	std::list<std::list<int> >::iterator fpSafetyIter = flowpipes_safety.begin();

	std::list<int>::const_iterator modeIter = modeIDs.begin();
	std::list<TreeNode *>::const_iterator nodeIter = traceNodes.begin();


	std::list<TaylorModelVec>::const_iterator tmvIter;
	std::list<std::vector<Interval> >::const_iterator doIter;
	std::list<bool>::const_iterator conIter;
	std::list<int>::iterator safetyIter;

	int result = SAFE;

	int prog = 0, total_size = 0;

	for(; fpSafetyIter!=flowpipes_safety.end(); ++fpSafetyIter)
	{
		total_size += fpSafetyIter->size();
	}

	fpSafetyIter = flowpipes_safety.begin();

	for(; fpIter!=flowpipes.end(); ++fpIter, ++fpDoIter, ++fpConIter, ++modeIter, ++fpSafetyIter, ++nodeIter)
	{
		if(!bVecUnderCheck[*modeIter])
		{
			continue;
		}

		tmvIter = fpIter->begin();
		doIter = fpDoIter->begin();
		conIter = fpConIter->begin();
		safetyIter = fpSafetyIter->begin();

		if(unsafeSet[*modeIter].size() == 0)	// the whole mode state space is unsafe
		{
			return UNSAFE;
		}

		int domainDim = doIter->size();

		std::list<TaylorModelVec> unsafe_tm_flowpipes;
		std::list<std::vector<Interval> > unsafe_flowpipe_domains;
		std::list<Interval> unsafe_time_points;

		std::list<TaylorModelVec> unknown_tm_flowpipes;
		std::list<std::vector<Interval> > unknown_flowpipe_domains;
		std::list<Interval> unknown_time_points;

		Interval timePoint;

		for(; tmvIter!=fpIter->end(); ++tmvIter, ++doIter, ++conIter, ++safetyIter)
		{
			Interval cutoff_threshold;
			if(local_settings[*modeIter].cutoff_threshold.sup() > 0)
			{
				cutoff_threshold = local_settings[*modeIter].cutoff_threshold;
			}
			else
			{
				cutoff_threshold = global_setting.cutoff_threshold;
			}

			int order;
			if(local_settings[*modeIter].globalMaxOrder > 0)
			{
				order = local_settings[*modeIter].globalMaxOrder;
			}
			else
			{
				order = global_setting.globalMaxOrder;
			}

			int safety = safetyChecking2(*tmvIter, *doIter, unsafeSet[*modeIter], order, cutoff_threshold);

			if(safety == UNSAFE)
			{
				*safetyIter = UNSAFE;
				result = UNSAFE;

				if(bDumpCounterexamples)
				{
					unsafe_tm_flowpipes.push_back(*tmvIter);
					unsafe_flowpipe_domains.push_back(*doIter);
					unsafe_time_points.push_back(timePoint);
				}

				break;
			}
			else if(safety != SAFE)
			{
				*safetyIter = UNKNOWN;

				if(result == SAFE)
				{
					result = UNKNOWN;
				}

				if(bDumpCounterexamples)
				{
					unknown_tm_flowpipes.push_back(*tmvIter);
					unknown_flowpipe_domains.push_back(*doIter);
					unknown_time_points.push_back(timePoint);
				}
			}

			++prog;
			printf("\b\b\b\b");
			printf(BOLD_FONT "%%" RESET_COLOR);
			printf(BOLD_FONT "%3d" RESET_COLOR, (int)(prog*100/total_size));
			fflush(stdout);

			timePoint += (*doIter)[0];

			if(bDumpCounterexamples)
			{
				fprintf(fpDumpCounterexamples, "Unsafe flowpipes:\n\n");
				dump_counterexample(fpDumpCounterexamples, unsafe_tm_flowpipes, unsafe_flowpipe_domains, *nodeIter, unsafe_time_points);
				fprintf(fpDumpCounterexamples, "Unknown flowpipes:\n\n");
				dump_counterexample(fpDumpCounterexamples, unknown_tm_flowpipes, unknown_flowpipe_domains, *nodeIter, unknown_time_points);
			}

			if(result == UNSAFE)
			{
				break;
			}
		}

		if(result == UNSAFE)
		{
			break;
		}
	}

	if(bDumpCounterexamples)
	{
		fclose(fpDumpCounterexamples);
	}

	printf("\b\b\b\b");
	printf(BOLD_FONT "%%100\n" RESET_COLOR);
	fflush(stdout);

	return result;
}

long HybridReachability::numOfFlowpipes() const
{
	return num_of_flowpipes;
}

void HybridReachability::dump_counterexample(FILE *fp, const std::list<TaylorModelVec> & flowpipes, const std::list<std::vector<Interval> > & domains, TreeNode * const node, const std::list<Interval> & localTimes) const
{
	// dump the flowpipes
	fprintf(fp, "%s\n{\n", modeNames[node->modeID].c_str());

	std::list<TaylorModelVec>::const_iterator fpIter = flowpipes.begin();
	std::list<std::vector<Interval> >::const_iterator doIter = domains.begin();
	std::list<Interval>::const_iterator timeIter = localTimes.begin();

	for(; fpIter!=flowpipes.end(); ++fpIter, ++doIter, ++timeIter)
	{
		fprintf(fp, "starting time %lf\n{\n", timeIter->sup());

		fpIter->dump_interval(fp, stateVarNames, tmVarNames);

		for(int i=0; i<doIter->size(); ++i)
		{
			fprintf(fp, "%s in ", tmVarNames[i].c_str());
			(*doIter)[i].dump(fp);
			fprintf(fp, "\n");
		}

		fprintf(fp, "}\n\n");
	}

	fprintf(fp, "computation path\n{\n\n");

	TreeNode *iterator = node;

	std::string strTrace;
	char buffer[NAME_SIZE];

	for(; iterator->parent != NULL; iterator = iterator->parent)
	{
		std::string strLocalTime;
		iterator->localTime.toString(strLocalTime);

		sprintf(buffer, "( %d ", iterator->jumpID);
		std::string strJumpID(buffer);

		strTrace = ' ' + strJumpID + ',' + ' ' + strLocalTime + ')' + ' ' + '-' + '>' + ' ' + modeNames[iterator->modeID] + strTrace;
	}

	strTrace = modeNames[iterator->modeID] + strTrace;

	fprintf(fp, "%s;\n\n}\n\n}\n\n", strTrace.c_str());
}

































// class FactorTab

FactorTab::FactorTab()
{
	index = 0;
}

FactorTab::FactorTab(const int index_input, const Interval & factor_input, const Interval & intercept_input)
{
	index = index_input;
	factor = factor_input;
	intercept = intercept_input;
}

FactorTab::~FactorTab()
{
}

namespace flowstar
{

bool compareFactor(const FactorTab & a, const FactorTab & b)
{
	if(a.factor > b.factor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool compareIntercept(const FactorTab & a, const FactorTab & b)
{
	if(a.intercept < b.intercept)
	{
		return true;
	}
	else
	{
		return false;
	}
}















/*
int safetyChecking(const TaylorModelVec & flowpipe, const std::vector<Interval> & step_exp_table, const std::vector<PolynomialConstraint> & unsafeSet, const int order, const Interval & cutoff_threshold)
{
	int rangeDim = flowpipe.tms.size();
	int result = UNKNOWN;
	bool bContained = true;

	std::vector<Interval> tmvPolyRange;
	flowpipe.polyRangeNormal(tmvPolyRange, step_exp_table);

	for(int i=0; i<unsafeSet.size(); ++i)
	{
		TaylorModel tmTemp;

		// interval evaluation on the constraint
		unsafeSet[i].hf.insert_ctrunc_normal(tmTemp, flowpipe, tmvPolyRange, step_exp_table, rangeDim+1, order, cutoff_threshold);

		Interval intTemp;
		tmTemp.intEvalNormal(intTemp, step_exp_table);

		if(intTemp > unsafeSet[i].B)
		{
			// no intersection with the unsafe set
			result = SAFE;
			break;
		}
		else
		{
			if(!intTemp.smallereq(unsafeSet[i].B) && bContained)
			{
				bContained = false;
			}
		}
	}

	if(result == UNKNOWN && bContained)
	{
		return UNSAFE;
	}
	else
	{
		return result;
	}
}
*/





void generateNodeSeq(std::list<TreeNode *> & result, TreeNode *root)
{
	std::list<TreeNode *> queue;
	queue.push_back(root);

	result.clear();

	for(; queue.size() > 0;)
	{
		TreeNode *current = queue.front();
		queue.pop_front();

		result.push_back(current);

		if(current->children.size() == 0)
		{
			continue;
		}

		std::list<TreeNode *>::iterator iter = current->children.begin();
		for(; iter != current->children.end(); ++iter)
		{
			queue.push_back(*iter);
		}
	}
}


































void aggregate_flowpipes_by_interval(TaylorModelVec & tmvAggregation, std::vector<Interval> & doAggregation, const std::vector<TaylorModelVec> & flowpipes, const std::vector<std::vector<Interval> > & domains)
{
	std::vector<Interval> intVecAggregation;

	flowpipes[0].intEval(intVecAggregation, domains[0]);

	Interval I1, I2;

	for(int k=0; k<flowpipes.size(); ++k)
	{
		std::vector<Interval> intVecTemp;
		flowpipes[k].intEval(intVecTemp, domains[k]);

		for(int i=0; i<intVecAggregation.size(); ++i)
		{

			intVecAggregation[i].inf(I1);
			intVecTemp[i].inf(I2);

			if(I2 <= I1)
			{
				intVecAggregation[i].setInf(I2);
			}

			intVecAggregation[i].sup(I1);
			intVecTemp[i].sup(I2);

			if(I2 >= I1)
			{
				intVecAggregation[i].setSup(I2);
			}
		}
	}

	TaylorModelVec tmvTemp(intVecAggregation, doAggregation);

	tmvAggregation = tmvTemp;
}

bool vector_selection(FactorTab & lst_selected, std::list<FactorTab> & lst_unselected, Matrix & matTemplate, const std::vector<RowVector> & rowVecs, int & rank)
{
	lst_unselected.sort(compareFactor);

	std::list<FactorTab> candidates;
	bool bvalid = false;


/*
	// =============== test begin ==================
	printf("Candidates:\n");
	list<FactorTab>::iterator testIter = lst_unselected.begin();
	for(; testIter!=lst_unselected.end(); ++testIter)
	{
		printf("vector: ");
		rowVecs[testIter->index].dump(stdout);
		printf("\tintercept: %lf, factor: %lf\n\n", testIter->intercept.midpoint(), testIter->factor.midpoint());
	}
	// =============== test end ==================
*/


	Interval intZero;

	for(; !bvalid && lst_unselected.size()!=0;)
	{
		std::list<FactorTab>::iterator facIter = lst_unselected.begin();

		Interval factor = facIter->factor;
		candidates.push_back(*facIter);
		facIter = lst_unselected.erase(facIter);

		for(; facIter!=lst_unselected.end(); )
		{
			if(facIter->factor < intZero)
			{
				facIter = lst_unselected.erase(facIter);
			}
			else if(facIter->factor.within(factor, THRESHOLD_LOW))
			{
				candidates.push_back(*facIter);
				facIter = lst_unselected.erase(facIter);
			}
			else
			{
				break;
			}
		}

		for(; !bvalid && candidates.size()!=0; )
		{
			facIter = candidates.begin();
			Interval min_intercept = facIter->intercept;
			std::list<FactorTab>::iterator iter_selected = facIter;

			++facIter;

			for(; facIter!=candidates.end(); ++facIter)
			{
				if(min_intercept > facIter->intercept)
				{
					min_intercept = facIter->intercept;
					iter_selected = facIter;
				}
			}

			lst_selected = *iter_selected;
			candidates.erase(iter_selected);

			bvalid = flowstar::check_validity(matTemplate, rowVecs[lst_selected.index], rank);
		}
	}

	if(bvalid)
	{
		++rank;

		// insert the unselected elements back
		std::list<FactorTab>::iterator facIter = candidates.begin();

		for(; facIter!=candidates.end(); ++facIter)
		{
			lst_unselected.push_back(*facIter);
		}


/*
		// =============== test begin ==================
		printf("selected: ");
		rowVecs[lst_selected.index].dump(stdout);
		printf("\tintercept: %lf, factor: %lf\n\n\n", lst_selected.intercept.midpoint(), lst_selected.factor.midpoint());
		// =============== test end ==================
*/




		return true;	// one element is selected
	}
	else
	{
		return false;	// nothing in the unselected list is selectable
	}
}

bool check_validity(Matrix & matTemplate, const RowVector & rowVec, const int rank)
{
	int num = rowVec.size();
	for(int i=0; i<num; ++i)
	{
		matTemplate.set(rowVec.get(i), rank, i);
	}

	int r = matTemplate.rank();

	if(r == rank+1)
	{
		return true;
	}
	else
	{
		return false;
	}
}










void aggregate_flowpipes_by_Parallelotope(TaylorModelVec & tmvAggregation, std::vector<Interval> & doAggregation, const std::vector<TaylorModelVec> & flowpipes,
		const std::vector<std::vector<Interval> > & domains, const std::vector<PolynomialConstraint> & invariant, const DiscTrans & jump, std::vector<bool> & boundary_intersected,
		const std::vector<RowVector> & template_candidates, const std::vector<RowVector> & template_default, const Matrix & weightTab,
		const std::vector<bool> & linear_auto, const std::vector<RowVector> & template_auto, const int globalMaxOrder, const int rangeDim)
{
	int num_invariants = invariant.size();
	int num_candidates = template_candidates.size();

	int auto_start = num_candidates;
	int guard_start = auto_start + num_invariants;
	int default_start = guard_start + jump.guard.size();

	ColVector parallelotope_b(2*rangeDim);

//	int startID = jump.startID;
//	int targetID = jump.targetID;
	Matrix weightMat = weightTab;

	std::vector<Interval> rhoPos;
	std::vector<Interval> rhoNeg;

	Interval intZero, intInvalid(INVALID), intOne(1), intUnit(-1,1);

	std::list<FactorTab> lst_unselected;
	std::list<FactorTab> lst_selected;

	Matrix paraTemplate(rangeDim, rangeDim);
	int num_selected = 0;

//	std::vector<Interval> step_exp_table;
//	Interval intStep;

//	std::vector<std::vector<Interval> > step_exp_tables;

//	for(int i=0; i<flowpipes.size(); ++i)
//	{
//		if(step_exp_table.size() == 0 || intStep != domains[i][0])
//		{
//			construct_step_exp_table(step_exp_table, domains[i][0], 2*globalMaxOrder);
//			intStep = domains[i][0];
//		}

//		step_exp_tables.push_back(step_exp_table);
//	}

	// 1: we first consider the user specified template vectors

	if(template_candidates.size() > 0)
	{
		for(int i=0; i<template_candidates.size(); ++i)
		{
			FactorTab facT(i, intZero, intInvalid);
			lst_unselected.push_back(facT);
			rhoPos.push_back(intInvalid);
			rhoNeg.push_back(intInvalid);
		}

		// 1.1: compute the intercepts

//		step_exp_table = step_exp_tables[0];

		for(int i=0, k=0; i<flowpipes.size(); ++i)
		{
//			if(step_exp_table[1] != domains[i][0])
//			{
//				step_exp_table = step_exp_tables[++k];
//			}

			for(int j=0; j<template_candidates.size(); ++j)
			{
//				Interval tmp1 = rhoNormal(flowpipes[i], template_candidates[j], step_exp_table);
				Interval tmp1 = rho(flowpipes[i], template_candidates[j], domains[i]);

				RowVector rowVec = template_candidates[j];
				rowVec.neg_assign();

//				Interval tmp2 = rhoNormal(flowpipes[i], rowVec, step_exp_table);
				Interval tmp2 = rho(flowpipes[i], rowVec, domains[i]);

				if(tmp1 >= rhoPos[j])	// tmp1.up > rhoPos[j].up
				{
					rhoPos[j] = tmp1;
				}

				if(tmp2 >= rhoNeg[j])	// tmp2.up > rhoNeg[j].up
				{
					rhoNeg[j] = tmp2;
				}
			}
		}

		// 1.2: select the vector with the smallest intercept
		std::list<FactorTab>::iterator facIter = lst_unselected.begin();
		std::list<FactorTab>::iterator min_facIter = facIter;

		facIter->intercept = rhoPos[0] + rhoNeg[0];
		++facIter;

		for(int i=1; facIter!=lst_unselected.end(); ++facIter, ++i)
		{
			facIter->intercept = rhoPos[i] + rhoNeg[i];

			if(facIter->intercept < min_facIter->intercept)
			{
				min_facIter = facIter;
			}
		}

		int index_selected = min_facIter->index;


/*
		// =============== test begin ==================
		printf("Candidates:\n");
		list<FactorTab>::iterator testIter = lst_unselected.begin();
		for(; testIter!=lst_unselected.end(); ++testIter)
		{
			printf("vector: ");
			template_candidates[testIter->index].dump(stdout);
			printf("\tintercept: %lf, factor: %lf\n\n", testIter->intercept.midpoint(), testIter->factor.midpoint());
		}

		printf("selected: ");
		template_candidates[index_selected].dump(stdout);
		printf("\tintercept: %lf, factor: %lf\n\n\n", min_facIter->intercept.midpoint(), min_facIter->factor.midpoint());
		// =============== test end ==================
*/



		lst_selected.push_back(*min_facIter);

		parallelotope_b.set(rhoPos[index_selected].sup(), num_selected);
		parallelotope_b.set(rhoNeg[index_selected].sup(), num_selected+rangeDim);

		lst_unselected.erase(min_facIter);

		for(int i=0; i<rangeDim; ++i)
		{
			paraTemplate.set(template_candidates[index_selected].get(i), num_selected, i);
		}

		++num_selected;

		if(num_selected < rangeDim)
		{
			// 1.3: update the factor table according to the selected vectors

			for(facIter=lst_unselected.begin(); facIter!=lst_unselected.end(); ++facIter)
			{
				facIter->factor.set( weightMat.get(index_selected, facIter->index) );
			}

			// 1.4: consider the remaining vectors

			for(; lst_unselected.size() != 0 && num_selected < rangeDim; )
			{
				FactorTab vector_selected;
				bool bselected = flowstar::vector_selection(vector_selected, lst_unselected, paraTemplate, template_candidates, num_selected);

				// update the factors
				if(bselected)
				{
					parallelotope_b.set(rhoPos[vector_selected.index].sup(), num_selected-1);
					parallelotope_b.set(rhoNeg[vector_selected.index].sup(), num_selected-1+rangeDim);

					lst_selected.push_back(vector_selected);

					facIter = lst_unselected.begin();
					for(; facIter!=lst_unselected.end(); ++facIter)
					{
						facIter->factor.mul_assign( weightMat.get(facIter->index, vector_selected.index) );
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	// 2: we consider the auto selected template vectors

	if(num_selected < rangeDim)
	{
/*
		// evaluate the template vector from the flowpipes
		int mid = flowpipes.size() / 2;
		vector<Interval> domainCenterPoint;
		for(int i=0; i<domains[mid].size(); ++i)
		{
			Interval I(domains[mid][i].midpoint());
			domainCenterPoint.push_back(I);
		}

		vector<Interval> flowCenterPoint;
		flowpipes[mid].intEval(flowCenterPoint, domainCenterPoint);

		Interval intZero;
		flowCenterPoint.insert(flowCenterPoint.begin(), intZero);

		bool bvalid_template_flow = false;
		Interval intSparsity(-SPARSITY, SPARSITY);

		RowVector template_flow(rangeDim);
		for(int i=0; i<odeHF.size(); ++i)
		{
			Interval I;
			odeHF[i].intEval(I, flowCenterPoint);
			template_flow.set(I.midpoint(), i);

			if(!bvalid_template_flow && !I.subseteq(intSparsity))
			{
				bvalid_template_flow = true;
			}
		}

		if(bvalid_template_flow)
		{
			template_flow.normalize();
		}

		// update the weight matrix

		if(bvalid_template_flow)
		{
			for(int i=0; i<weightMat.cols(); ++i)
			{
				if(i < auto_start)
				{
					// candidate vector
					double weight = 1 - fabs(template_candidates[i].innerProd(template_flow));
					weightMat.set(weight, i, flow_start);
					weightMat.set(weight, flow_start, i);
				}
				else if(i >= auto_start && i < flow_start)
				{
					// auto selected vector
					int posi = i - auto_start;
					if(linear_auto[startID][targetID][posi])
					{
						double weight = 1 - fabs(template_auto[startID][targetID][posi].innerProd(template_flow));
						weightMat.set(weight, i, flow_start);
						weightMat.set(weight, flow_start, i);
					}
					else
					{
						weightMat.set(-1, i, flow_start);
						weightMat.set(-1, flow_start, i);
					}
				}
				else if(i >= flow_start && i < default_start)
				{
					// flow vector
					double weight = 1 - fabs(template_flow.innerProd(template_flow));
					weightMat.set(weight, i, flow_start);
					weightMat.set(weight, flow_start, i);
				}
				else
				{
					// default vector
					double weight = 1 - fabs(template_default[i - default_start].innerProd(template_flow));
					weightMat.set(weight, i, flow_start);
					weightMat.set(weight, flow_start, i);
				}
			}
		}

		local_template_auto.push_back(template_flow);
*/


		rhoPos.clear();
		rhoNeg.clear();
		lst_unselected.clear();

		if(template_auto.size() > 0)
		{
			for(int i=0; i<template_auto.size(); ++i)
			{
				if(linear_auto[i] && boundary_intersected[i])
                {
					FactorTab facT(i, intOne, intInvalid);
					lst_unselected.push_back(facT);
				}

				rhoPos.push_back(intInvalid);
				rhoNeg.push_back(intInvalid);
			}

			// 2.1: compute the support functions

//			step_exp_table = step_exp_tables[0];

			for(int i=0, k=0; i<flowpipes.size(); ++i)
			{
//				if(step_exp_table[1] != domains[i][0])
//				{
//					step_exp_table = step_exp_tables[++k];
//				}

				std::list<FactorTab>::iterator vectorIter = lst_unselected.begin();
				for(; vectorIter!=lst_unselected.end(); ++vectorIter)
				{
//					Interval tmp1 = rhoNormal(flowpipes[i], template_auto[vectorIter->index], step_exp_table);
					Interval tmp1 = rho(flowpipes[i], template_auto[vectorIter->index], domains[i]);

					RowVector rowVec = template_auto[vectorIter->index];
					rowVec.neg_assign();

//					Interval tmp2 = rhoNormal(flowpipes[i], rowVec, step_exp_table);
					Interval tmp2 = rho(flowpipes[i], rowVec, domains[i]);

					if(tmp1 >= rhoPos[vectorIter->index])
					{
						rhoPos[vectorIter->index] = tmp1;
					}

					if(tmp2 >= rhoNeg[vectorIter->index])
					{
						rhoNeg[vectorIter->index] = tmp2;
					}
				}
			}

			// 2.2: compute the intercepts
			std::list<FactorTab>::iterator facIter = lst_unselected.begin();

			for(; facIter!=lst_unselected.end(); ++facIter)
			{
				facIter->intercept = rhoPos[facIter->index] + rhoNeg[facIter->index];
			}

			// 2.3: update the factor table
			std::list<FactorTab>::iterator selectedIter = lst_selected.begin();
			for(; selectedIter!=lst_selected.end(); ++selectedIter)
			{
				for(facIter=lst_unselected.begin(); facIter!=lst_unselected.end(); ++facIter)
				{
					facIter->factor.mul_assign( weightMat.get(selectedIter->index, facIter->index+auto_start) );
				}
			}

			// 2.4: consider the remaining vectors

			for(; lst_unselected.size() != 0 && num_selected < rangeDim; )
			{
				FactorTab vector_selected;
				bool bselected = flowstar::vector_selection(vector_selected, lst_unselected, paraTemplate, template_auto, num_selected);

				// update the factors
				if(bselected)
				{
					parallelotope_b.set(rhoPos[vector_selected.index].sup(), num_selected-1);
					parallelotope_b.set(rhoNeg[vector_selected.index].sup(), num_selected-1+rangeDim);

					vector_selected.index += auto_start;
					lst_selected.push_back(vector_selected);

					facIter = lst_unselected.begin();
					for(; facIter!=lst_unselected.end(); ++facIter)
					{
						facIter->factor.mul_assign( weightMat.get(facIter->index+auto_start, vector_selected.index) );
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	// 3: if the vectors are not enough, we find the remaining ones in the default template

	if(num_selected < rangeDim)
	{
		rhoPos.clear();
		rhoNeg.clear();
		lst_unselected.clear();

		if(template_default.size() > 0)
		{
			for(int i=0; i<template_default.size(); ++i)
			{
				FactorTab facT(i, intOne, intInvalid);
				lst_unselected.push_back(facT);
				rhoPos.push_back(intInvalid);
				rhoNeg.push_back(intInvalid);
			}

			// 3.1: compute the support functions

//			step_exp_table = step_exp_tables[0];

			for(int i=0, k=0; i<flowpipes.size(); ++i)
			{
//				if(step_exp_table[1] != domains[i][0])
//				{
//					step_exp_table = step_exp_tables[++k];
//				}

				for(int j=0; j<template_default.size(); ++j)
				{
//					Interval tmp1 = rhoNormal(flowpipes[i], template_default[j], step_exp_table);
					Interval tmp1 = rho(flowpipes[i], template_default[j], domains[i]);

					RowVector rowVec = template_default[j];
					rowVec.neg_assign();

//					Interval tmp2 = rhoNormal(flowpipes[i], rowVec, step_exp_table);
					Interval tmp2 = rho(flowpipes[i], rowVec, domains[i]);

					if(tmp1 >= rhoPos[j])
					{
						rhoPos[j] = tmp1;
					}

					if(tmp2 >= rhoNeg[j])
					{
						rhoNeg[j] = tmp2;
					}
				}
			}

			// 3.2: compute the intercepts
			std::list<FactorTab>::iterator facIter = lst_unselected.begin();

			for(int i=0; facIter!=lst_unselected.end(); ++facIter, ++i)
			{
				facIter->intercept = rhoPos[i] + rhoNeg[i];
			}

			// 3.3: update the factor table
			std::list<FactorTab>::iterator selectedIter = lst_selected.begin();
			for(; selectedIter!=lst_selected.end(); ++selectedIter)
			{
				for(facIter=lst_unselected.begin(); facIter!=lst_unselected.end(); ++facIter)
				{
					facIter->factor.mul_assign( weightMat.get(selectedIter->index, facIter->index+default_start) );
				}
			}

			// 3.4: consider the remaining vectors
			for(; lst_unselected.size() != 0 && num_selected < rangeDim; )
			{
				FactorTab vector_selected;
				bool bselected = flowstar::vector_selection(vector_selected, lst_unselected, paraTemplate, template_default, num_selected);

				// update the factors
				if(bselected)
				{
					parallelotope_b.set(rhoPos[vector_selected.index].sup(), num_selected-1);
					parallelotope_b.set(rhoNeg[vector_selected.index].sup(), num_selected-1+rangeDim);

					vector_selected.index += default_start;
					lst_selected.push_back(vector_selected);

					facIter = lst_unselected.begin();
					for(; facIter!=lst_unselected.end(); ++facIter)
					{
						facIter->factor.mul_assign( weightMat.get(facIter->index+default_start, vector_selected.index) );
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	// 4: we use the template parallelotope to over-approximate the flowpipe union

	Parallelotope paraAggregation(paraTemplate, parallelotope_b);

	// converse the parallelotope to a Taylor model and construct a normalized domain for it
	paraAggregation.toTaylorModel(tmvAggregation);

	doAggregation.clear();

	doAggregation.push_back(intZero);

	for(int i=0; i<rangeDim; ++i)
	{
		doAggregation.push_back(intUnit);
	}
}

}



