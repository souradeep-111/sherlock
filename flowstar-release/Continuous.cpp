/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.

  The code is released as is under the GNU General Public License (GPL).
---*/

#include "Continuous.h"

using namespace flowstar;

Flowpipe::Flowpipe()
{
}

Flowpipe::Flowpipe(const TaylorModelVec & tmvPre_input, const TaylorModelVec & tmv_input, const std::vector<Interval> & domain_input):
		tmvPre(tmvPre_input), tmv(tmv_input), domain(domain_input)
{
}

Flowpipe::Flowpipe(const std::vector<Interval> & box, const Interval & I)
{
	int rangeDim = box.size();
	int domainDim = rangeDim + 1;
	Interval intUnit(-1,1), intZero;

	TaylorModelVec tmvCenter;
	std::vector<double> scalars;

	domain.push_back(I);		// time interval

	// normalize the domain to [-1,1]^n
	for(int i=0; i<rangeDim; ++i)
	{
		double midpoint = box[i].midpoint();
		Interval intMid(midpoint);
		TaylorModel tmTemp(intMid, domainDim);
		tmvCenter.tms.push_back(tmTemp);

		Interval intTemp = box[i];
		intTemp.sub_assign(midpoint);
		scalars.push_back( intTemp.sup() );
		domain.push_back(intUnit);
	}

	Matrix coefficients_of_tmvPre(rangeDim, rangeDim+1);

	for(int i=0; i<rangeDim; ++i)
	{
		coefficients_of_tmvPre.set(scalars[i], i, i+1);
	}

	TaylorModelVec tmvTemp(coefficients_of_tmvPre);
	tmvTemp.add(tmvPre, tmvCenter);

	Matrix coefficients_of_tmv(rangeDim, rangeDim+1);

	for(int i=0; i<rangeDim; ++i)
	{
		coefficients_of_tmv.set(1, i, i+1);
	}

	TaylorModelVec tmvTemp2(coefficients_of_tmv);
	tmv = tmvTemp2;
}

Flowpipe::Flowpipe(const TaylorModelVec & tmv_input, const std::vector<Interval> & domain_input)
{
	int rangeDim = tmv_input.tms.size();

	tmv = tmv_input;
	domain = domain_input;

	Matrix coefficients_of_tmvPre(rangeDim, rangeDim+1);

	for(int i=0; i<rangeDim; ++i)
	{
		coefficients_of_tmvPre.set(1, i, i+1);
	}

	TaylorModelVec tmvTemp(coefficients_of_tmvPre);
	tmvPre = tmvTemp;

	normalize();
}

Flowpipe::Flowpipe(const Flowpipe & flowpipe):tmvPre(flowpipe.tmvPre), tmv(flowpipe.tmv), domain(flowpipe.domain)
{
}

Flowpipe::~Flowpipe()
{
	clear();
}

void Flowpipe::clear()
{
	tmvPre.clear();
	tmv.clear();
	domain.clear();
}

void Flowpipe::dump(FILE *fp, const std::vector<std::string> & stateVarNames, const std::vector<std::string> & tmVarNames, const Interval & cutoff_threshold) const
{
	TaylorModelVec tmvTemp;

	composition(tmvTemp, cutoff_threshold);

	// dump the Taylor model
	tmvTemp.dump_interval(fp, stateVarNames, tmVarNames);

	//dump the domain
	for(int i=0; i<domain.size(); ++i)
	{
		fprintf(fp, "%s in ", tmVarNames[i].c_str());
		domain[i].dump(fp);
		fprintf(fp, "\n");
	}
}

void Flowpipe::dump_normal(FILE *fp, const std::vector<std::string> & stateVarNames, const std::vector<std::string> & tmVarNames, std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold) const
{
	TaylorModelVec tmvTemp;

	composition_normal(tmvTemp, step_exp_table, cutoff_threshold);

	// dump the Taylor model
	tmvTemp.dump_interval(fp, stateVarNames, tmVarNames);

	//dump the domain
	for(int i=0; i<domain.size(); ++i)
	{
		fprintf(fp, "%s in ", tmVarNames[i].c_str());
		domain[i].dump(fp);
		fprintf(fp, "\n");
	}

	fprintf(fp, "\n");
}

void Flowpipe::composition(TaylorModelVec & result, const Interval & cutoff_threshold) const
{
	std::vector<int> orders;

	for(int i=0; i<tmv.tms.size(); ++i)
	{
		int d1 = tmv.tms[i].degree();
		int d2 = tmvPre.tms[i].degree();

		if(d1 > d2)
		{
			orders.push_back(d1);
		}
		else
		{
			orders.push_back(d2);
		}
	}

	std::vector<Interval> tmvPolyRange;
	tmv.polyRange(tmvPolyRange, domain);
	tmvPre.insert_ctrunc(result, tmv, tmvPolyRange, domain, orders, cutoff_threshold);
}

void Flowpipe::composition(TaylorModelVec & result, const int order, const Interval & cutoff_threshold) const
{
	std::vector<Interval> tmvPolyRange;
	tmv.polyRange(tmvPolyRange, domain);
	tmvPre.insert_ctrunc(result, tmv, tmvPolyRange, domain, order, cutoff_threshold);
}

void Flowpipe::composition(TaylorModelVec & result, const std::vector<int> & orders, const Interval & cutoff_threshold) const
{
	std::vector<Interval> tmvPolyRange;
	tmv.polyRange(tmvPolyRange, domain);
	tmvPre.insert_ctrunc(result, tmv, tmvPolyRange, domain, orders, cutoff_threshold);
}

void Flowpipe::composition(TaylorModelVec & result, const std::vector<int> & outputAxes, const int order, const Interval & cutoff_threshold) const
{
	std::vector<Interval> tmvPolyRange;
	tmv.polyRange(tmvPolyRange, domain);

	result.clear();

	for(int i=0; i<outputAxes.size(); ++i)
	{
		TaylorModel tmTemp;
		tmvPre.tms[outputAxes[i]].insert_ctrunc(tmTemp, tmv, tmvPolyRange, domain, order, cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void Flowpipe::composition_normal(TaylorModelVec & result, const std::vector<Interval> & step_exp_table, const int order, const Interval & cutoff_threshold) const
{
	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_exp_table);
	tmvPre.insert_ctrunc_normal(result, tmv, tmvPolyRange, step_exp_table, domain.size(), order, cutoff_threshold);
}

void Flowpipe::composition_normal(TaylorModelVec & result, const std::vector<Interval> & step_exp_table, const std::vector<int> & orders, const Interval & cutoff_threshold) const
{
	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_exp_table);
	tmvPre.insert_ctrunc_normal(result, tmv, tmvPolyRange, step_exp_table, domain.size(), orders, cutoff_threshold);
}

void Flowpipe::composition_normal(TaylorModelVec & result, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold) const
{
	int domainDim = domain.size();

	std::vector<int> orders;

	for(int i=0; i<tmv.tms.size(); ++i)
	{
		int d1 = tmv.tms[i].degree();
		int d2 = tmvPre.tms[i].degree();

		if(d1 > d2)
		{
			orders.push_back(d1);
		}
		else
		{
			orders.push_back(d2);
		}
	}

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_exp_table);
	tmvPre.insert_ctrunc_normal(result, tmv, tmvPolyRange, step_exp_table, domainDim, orders, cutoff_threshold);
}

void Flowpipe::composition_normal(TaylorModelVec & result, const std::vector<int> & outputAxes, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold) const
{
	int domainDim = domain.size();

	std::vector<int> orders;

	for(int i=0; i<outputAxes.size(); ++i)
	{
		int d1 = tmv.tms[outputAxes[i]].degree();
		int d2 = tmvPre.tms[outputAxes[i]].degree();

		if(d1 > d2)
		{
			orders.push_back(d1);
		}
		else
		{
			orders.push_back(d2);
		}
	}

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_exp_table);

	result.clear();

	for(int i=0; i<outputAxes.size(); ++i)
	{
		TaylorModel tmTemp;
		tmvPre.tms[outputAxes[i]].insert_ctrunc_normal(tmTemp, tmv, tmvPolyRange, step_exp_table, domainDim, orders[i], cutoff_threshold);
		result.tms.push_back(tmTemp);
	}
}

void Flowpipe::intEval(std::vector<Interval> & result, const Interval & cutoff_threshold) const
{
	TaylorModelVec tmvTemp;
	composition(tmvTemp, cutoff_threshold);

	tmvTemp.intEval(result, domain);
}

void Flowpipe::intEvalNormal(std::vector<Interval> & result, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold) const
{
	TaylorModelVec tmvTemp;
	composition_normal(tmvTemp, step_exp_table, cutoff_threshold);
	tmvTemp.intEvalNormal(result, step_exp_table);
}

void Flowpipe::normalize()
{
	Interval intZero;

	// we first normalize the Taylor model tmv
	tmv.normalize(domain);

	int rangeDim = tmv.tms.size();

	// compute the center point of tmv
	std::vector<Interval> intVecCenter;
	tmv.constant(intVecCenter);
	tmv.rmConstant();

	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		tmv.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	std::vector<Interval> tmvRange;
	tmv.intEval(tmvRange, domain);

	std::vector<std::vector<Interval> > coefficients;
	std::vector<Interval> row;

	for(int i=0; i<rangeDim+1; ++i)
	{
		row.push_back(intZero);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		coefficients.push_back(row);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intScalor;
		tmvRange[i].mag(intScalor);

		if(intScalor.subseteq(intZero))
		{
			coefficients[i][i+1] = intZero;
		}
		else
		{
			coefficients[i][i+1] = intScalor;
			tmv.tms[i].div_assign(intScalor);
		}
	}

	TaylorModelVec newVars(coefficients);
	for(int i=0; i<rangeDim; ++i)
	{
		TaylorModel tmTemp(intVecCenter[i], rangeDim+1);
		newVars.tms[i].add_assign(tmTemp);
	}

	for(int i=0; i<tmvPre.tms.size(); ++i)
	{
		TaylorModel tmTemp;
		tmvPre.tms[i].insert_no_remainder_no_cutoff(tmTemp, newVars, rangeDim+1, tmvPre.tms[i].degree());
		tmvPre.tms[i].expansion = tmTemp.expansion;
	}
}

int Flowpipe::safetyChecking(const std::vector<Interval> & step_exp_table, const std::vector<PolynomialConstraint> & unsafeSet, const int order, const Interval & cutoff_threshold) const
{
	int rangeDim = tmvPre.tms.size();
	int result = UNKNOWN;
	bool bContained = true;

	for(int i=0; i<unsafeSet.size(); ++i)
	{
		std::vector<Interval> tmvPolyRange;
		tmvPre.polyRangeNormal(tmvPolyRange, step_exp_table);

		TaylorModel tmTemp;

		// interval evaluation on the constraint
		unsafeSet[i].hf.insert_ctrunc_normal(tmTemp, tmvPre, tmvPolyRange, step_exp_table, rangeDim+1, order, cutoff_threshold);

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

	if(result == UNKNOWN)
	{
		if(bContained)
		{
			return UNSAFE;
		}
		else
		{
			// do a simple branch & bound for safety checking
			TaylorModelVec tmvFlowpipe;
			composition(tmvFlowpipe, order, cutoff_threshold);

			std::vector<Interval> tmvPolyRange;
			tmvFlowpipe.polyRange(tmvPolyRange, domain);

			std::vector<HornerForm> obj_hfs;
			std::vector<Interval> obj_rems;

			result = SAFE;

			for(int i=0; i<unsafeSet.size(); ++i)
			{
				TaylorModel tmTemp;

				// interval evaluation on the constraint
				unsafeSet[i].hf.insert_ctrunc(tmTemp, tmvFlowpipe, tmvPolyRange, domain, order, cutoff_threshold);

				HornerForm obj_hf;
				tmTemp.expansion.toHornerForm(obj_hf);
				obj_hfs.push_back(obj_hf);
				obj_rems.push_back(tmTemp.remainder);
			}

			std::vector<Interval> refined_domain = domain;

			std::list<Interval> subdivisions;

			if(domain[0].width() > REFINEMENT_PREC)
			{
				subdivisions.push_back(domain[0]);
			}

			for(; subdivisions.size() > 0; )
			{
				Interval subdivision = subdivisions.front();
				subdivisions.pop_front();

				int result_iter = UNKNOWN;
				bool bContained_iter = true;

				refined_domain[0] = subdivision;

				for(int i=0; i<unsafeSet.size(); ++i)
				{
					Interval intTemp;
					obj_hfs[i].intEval(intTemp, refined_domain);

					intTemp += obj_rems[i];

					if(intTemp > unsafeSet[i].B)
					{
						// no intersection with the unsafe set
						result_iter = SAFE;
						break;
					}
					else
					{
						if(!intTemp.smallereq(unsafeSet[i].B) && bContained_iter)
						{
							bContained_iter = false;
						}
					}
				}

				if(result_iter == UNKNOWN)
				{
					if(bContained_iter)
					{
						return UNSAFE;
					}
					else
					{
						if(subdivision.width() <= REFINEMENT_PREC)
						{
							return UNKNOWN;
						}

						// split the domain
						Interval I1, I2;
						subdivision.split(I1, I2);

						if(I1.width() <= REFINEMENT_PREC)
						{
							if(result == SAFE)
								result = UNKNOWN;
						}
						else
						{
							subdivisions.push_back(I1);
						}

						if(I2.width() <= REFINEMENT_PREC)
						{
							if(result == SAFE)
								result = UNKNOWN;
						}
						else
						{
							subdivisions.push_back(I2);
						}
					}
				}
			}

			return result;
		}
	}
	else
	{
		return SAFE;
	}
}

int Flowpipe::safetyChecking(const std::vector<Interval> & step_exp_table, const std::vector<PolynomialConstraint> & unsafeSet, const std::vector<int> & orders, const int maxOrder, const Interval & cutoff_threshold) const
{
	int rangeDim = tmvPre.tms.size();
	int result = UNKNOWN;
	bool bContained = true;

	for(int i=0; i<unsafeSet.size(); ++i)
	{
		std::vector<Interval> tmvPolyRange;
		tmvPre.polyRangeNormal(tmvPolyRange, step_exp_table);

		TaylorModel tmTemp;

		// interval evaluation on the constraint
		unsafeSet[i].hf.insert_ctrunc_normal(tmTemp, tmvPre, tmvPolyRange, step_exp_table, rangeDim+1, maxOrder, cutoff_threshold);

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

	if(result == UNKNOWN)
	{
		if(bContained)
		{
			return UNSAFE;
		}
		else
		{
			// do a simple branch & bound for safety checking
			TaylorModelVec tmvFlowpipe;
			composition(tmvFlowpipe, orders, cutoff_threshold);

			std::vector<Interval> tmvPolyRange;
			tmvFlowpipe.polyRange(tmvPolyRange, domain);

			std::vector<HornerForm> obj_hfs;
			std::vector<Interval> obj_rems;

			result = SAFE;

			for(int i=0; i<unsafeSet.size(); ++i)
			{
				TaylorModel tmTemp;

				// interval evaluation on the constraint
				unsafeSet[i].hf.insert_ctrunc(tmTemp, tmvFlowpipe, tmvPolyRange, domain, maxOrder, cutoff_threshold);

				HornerForm obj_hf;
				tmTemp.expansion.toHornerForm(obj_hf);
				obj_hfs.push_back(obj_hf);
				obj_rems.push_back(tmTemp.remainder);
			}

			std::vector<Interval> refined_domain = domain;

			std::list<Interval> subdivisions;

			if(domain[0].width() > REFINEMENT_PREC)
			{
				subdivisions.push_back(domain[0]);
			}

			for(; subdivisions.size() > 0; )
			{
				Interval subdivision = subdivisions.front();
				subdivisions.pop_front();

				int result_iter = UNKNOWN;
				bool bContained_iter = true;

				refined_domain[0] = subdivision;

				for(int i=0; i<unsafeSet.size(); ++i)
				{
					Interval intTemp;
					obj_hfs[i].intEval(intTemp, refined_domain);

					intTemp += obj_rems[i];

					if(intTemp > unsafeSet[i].B)
					{
						// no intersection with the unsafe set
						result_iter = SAFE;
						break;
					}
					else
					{
						if(!intTemp.smallereq(unsafeSet[i].B) && bContained_iter)
						{
							bContained_iter = false;
						}
					}
				}

				if(result_iter == UNKNOWN)
				{
					if(bContained_iter)
					{
						return UNSAFE;
					}
					else
					{
						if(subdivision.width() <= REFINEMENT_PREC)
						{
							return UNKNOWN;
						}

						// split the domain
						Interval I1, I2;
						subdivision.split(I1, I2);

						if(I1.width() <= REFINEMENT_PREC)
						{
							if(result == SAFE)
								result = UNKNOWN;
						}
						else
						{
							subdivisions.push_back(I1);
						}

						if(I2.width() <= REFINEMENT_PREC)
						{
							if(result == SAFE)
								result = UNKNOWN;
						}
						else
						{
							subdivisions.push_back(I2);
						}
					}
				}
			}

			return result;
		}
	}
	else
	{
		return SAFE;
	}
}








// Taylor model integration by only using Picard iteration
int Flowpipe::advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);
	TaylorModelVec x = x0;

	for(int i=1; i<=order; ++i)
	{
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, i, cutoff_threshold);
	}

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	// add the uncertainties and the cutoff intervals onto the result
	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		// add the uncertainties and the cutoff intervals onto the result
		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{

			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}


	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

int Flowpipe::advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1,1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);
	TaylorModelVec x = x0;

	for(int i=1; i<=globalMaxOrder; ++i)
	{
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, i, cutoff_threshold);
	}

	x.nctrunc(orders);

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	// add the uncertainties and the cutoff intervals onto the result
	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		// add the uncertainties and the cutoff intervals onto the result
		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{

			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}


// adaptive step sizes and fixed orders
int Flowpipe::advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);
	TaylorModelVec x = x0;

	for(int i=1; i<=order; ++i)
	{
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, i, cutoff_threshold);
	}

	x.cutoff(cutoff_threshold);

	if(step > THRESHOLD_HIGH)		// step size is changed
	{
		construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	}

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	std::vector<Polynomial> polyDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;
		polyDifferences.push_back(polyTemp);

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	for(; !bfound;)
	{
		bfound = true;
		double newStep = step_exp_table[1].sup() * LAMBDA_DOWN;	// reduce the time step size

		if(newStep < miniStep)
		{
			return 0;
		}

		construct_step_exp_table(step_exp_table, step_end_exp_table, newStep, 2*order);

		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

		// recompute the interval evaluation of the polynomial differences
		for(int i=0; i<rangeDim; ++i)
		{
			polyDifferences[i].intEvalNormal(intDifferences[i], step_exp_table);
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

int Flowpipe::advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const double step, const double miniStep, const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);
	TaylorModelVec x = x0;

	for(int i=1; i<=globalMaxOrder; ++i)
	{
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, i, cutoff_threshold);
	}

	x.nctrunc(orders);

	x.cutoff(cutoff_threshold);

	if(step > THRESHOLD_HIGH)		// step size is changed
	{
		construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	}

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];		// apply the remainder estimation
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	std::vector<Polynomial> polyDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;
		polyDifferences.push_back(polyTemp);

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	for(; !bfound;)
	{
		bfound = true;
		double newStep = step_exp_table[1].sup() * LAMBDA_DOWN;	// reduce the time step size

		if(newStep < miniStep)
		{
			return -1;
		}

		construct_step_exp_table(step_exp_table, step_end_exp_table, newStep, 2*globalMaxOrder);

		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			polyDifferences[i].intEvalNormal(intDifferences[i], step_exp_table);
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

// adaptive orders and fixed step sizes
int Flowpipe::advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		int & order, const int maxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);
	TaylorModelVec x = x0;

	for(int i=1; i<=order; ++i)
	{
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, i, cutoff_threshold);
	}

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	int newOrder = order;

	for(; !bfound;)
	{
		++newOrder;

		if(newOrder > maxOrder)
		{
			return 0;
		}

		// increase the approximation orders by 1
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, newOrder, cutoff_threshold);
		x.cutoff(cutoff_threshold);

		for(int i=0; i<rangeDim; ++i)	// apply the estimation again
		{
			x.tms[i].remainder = estimation[i];
		}

		// compute the Picard operation again
		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, newOrder, cutoff_threshold, constant);

		// Update the irreducible part
		for(int i=0; i<rangeDim; ++i)
		{
			Polynomial polyTemp;
			polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

			Interval intTemp;
			polyTemp.intEvalNormal(intTemp, step_exp_table);
			intDifferences[i] = intTemp;
		}

		// add the uncertainties onto the result
		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		bfound = true;
		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		// add the uncertainties
		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	order = newOrder;
	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

int Flowpipe::advance_picard(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		std::vector<int> & orders, const int localMaxOrder, const std::vector<int> & maxOrders, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);
	TaylorModelVec x = x0;

	for(int i=1; i<=localMaxOrder; ++i)
	{
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, i, cutoff_threshold);
	}

	x.nctrunc(orders);

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	std::vector<bool> bIncrease;
	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bIncrease.push_back(true);
			if(bfound)
				bfound = false;
		}
		else
		{
			bIncrease.push_back(false);
		}
	}

	std::vector<int> newOrders = orders;
	bool bIncreaseOthers = false;
	int numIncrease = 0;

	std::vector<bool> bIncreased;
	for(int i=0; i<rangeDim; ++i)
	{
		bIncreased.push_back(false);
	}

	for(; !bfound;)
	{
		bool bChanged = false;

		if(bIncreaseOthers)
		{
			for(int i=0; i<bIncrease.size(); ++i)
			{
				if(!bIncrease[i] && newOrders[i] < maxOrders[i])
				{
					++newOrders[i];
					bIncreased[i] = true;

					if(!bChanged)
						bChanged = true;
				}
			}

			bIncreaseOthers = false;
		}
		else
		{
			numIncrease = 0;
			for(int i=0; i<bIncrease.size(); ++i)
			{
				if(bIncrease[i] && newOrders[i] < maxOrders[i])
				{
					++newOrders[i];
					++numIncrease;

					bIncreased[i] = true;

					if(!bChanged)
						bChanged = true;
				}
			}

			if(numIncrease < newOrders.size())
				bIncreaseOthers = true;
		}

		if(!bChanged)
		{
			return 0;
		}

		// increase the approximation orders
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, newOrders, bIncreased, cutoff_threshold);
		x.cutoff(cutoff_threshold);

		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = estimation[i];
		}

		// compute the Picard operation again
		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, newOrders, cutoff_threshold, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			// update the irreducible part if necessary
			if(bIncreased[i])
			{
				Polynomial polyTemp;
				polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

				Interval intTemp;
				polyTemp.intEvalNormal(intTemp, step_exp_table);
				intDifferences[i] = intTemp;
			}
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		bfound = true;

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				if(!bIncreaseOthers)
				{
					bIncrease[i] = true;
					if(bfound)
						bfound = false;
				}
				else
				{
					bfound = false;
					break;
				}
			}
			else
			{
				if(!bIncreaseOthers)
				{
					bIncrease[i] = false;
				}
			}
		}

		for(int i=0; i<rangeDim; ++i)
		{
			bIncreased[i] = false;
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	orders = newOrders;
	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}








// for low-degree ODEs
// fixed step sizes and orders

int Flowpipe::advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);

	TaylorModelVec x;

	for(int i=0; i<taylorExpansion.size(); ++i)
	{
		TaylorModel tmTemp;
		taylorExpansion[i].insert_no_remainder(tmTemp, x0, rangeDim+1, order, cutoff_threshold);
		x.tms.push_back(tmTemp);
	}

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	// add the uncertainties and the cutoff intervals onto the result
	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		// add the uncertainties and the cutoff intervals onto the result
		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{

			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}
			}
			else
			{
				bfinished = true;
				break;
			}

			x.tms[i].remainder = newRemainders[i];
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

int Flowpipe::advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const std::vector<int> & orders, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);

	TaylorModelVec x;

	for(int i=0; i<taylorExpansion.size(); ++i)
	{
		TaylorModel tmTemp;
		taylorExpansion[i].insert_no_remainder(tmTemp, x0, rangeDim+1, orders[i], cutoff_threshold);
		x.tms.push_back(tmTemp);
	}

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}


// adaptive step sizes and fixed orders
int Flowpipe::advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);

	TaylorModelVec x;

	for(int i=0; i<taylorExpansion.size(); ++i)
	{
		TaylorModel tmTemp;
		taylorExpansion[i].insert_no_remainder(tmTemp, x0, rangeDim+1, order, cutoff_threshold);
		x.tms.push_back(tmTemp);
	}

	x.cutoff(cutoff_threshold);

	if(step > THRESHOLD_HIGH)		// step size is changed
	{
		construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	}

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	std::vector<Polynomial> polyDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;
		polyDifferences.push_back(polyTemp);

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	for(; !bfound;)
	{
		bfound = true;
		double newStep = step_exp_table[1].sup() * LAMBDA_DOWN;	// reduce the time step size

		if(newStep < miniStep)
		{
			return 0;
		}

		construct_step_exp_table(step_exp_table, step_end_exp_table, newStep, 2*order);

		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

		// recompute the interval evaluation of the polynomial differences
		for(int i=0; i<rangeDim; ++i)
		{
			polyDifferences[i].intEvalNormal(intDifferences[i], step_exp_table);
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

int Flowpipe::advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const double step, const double miniStep, const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);

	TaylorModelVec x;

	for(int i=0; i<taylorExpansion.size(); ++i)
	{
		TaylorModel tmTemp;
		taylorExpansion[i].insert_no_remainder(tmTemp, x0, rangeDim+1, orders[i], cutoff_threshold);
		x.tms.push_back(tmTemp);
	}

	x.cutoff(cutoff_threshold);

	if(step > THRESHOLD_HIGH)		// step size is changed
	{
		construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	}

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	std::vector<Polynomial> polyDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;
		polyDifferences.push_back(polyTemp);

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	for(; !bfound;)
	{
		bfound = true;
		double newStep = step_exp_table[1].sup() * LAMBDA_DOWN;	// reduce the time step size

		if(newStep < miniStep)
		{
			return 0;
		}

		construct_step_exp_table(step_exp_table, step_end_exp_table, newStep, 2*globalMaxOrder);

		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			polyDifferences[i].intEvalNormal(intDifferences[i], step_exp_table);
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

// adaptive orders and fixed step sizes
int Flowpipe::advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		int & order, const int maxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);

	TaylorModelVec x;

	for(int i=0; i<taylorExpansion.size(); ++i)
	{
		TaylorModel tmTemp;
		taylorExpansion[i].insert_no_remainder(tmTemp, x0, rangeDim+1, order, cutoff_threshold);
		x.tms.push_back(tmTemp);
	}

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	int newOrder = order;

	for(; !bfound;)
	{
		++newOrder;

		if(newOrder > maxOrder)
		{
			return 0;
		}

		// increase the approximation orders by 1
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, newOrder, cutoff_threshold);
		x.cutoff(cutoff_threshold);

		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = estimation[i];
		}

		// compute the Picard operation again
		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, newOrder, cutoff_threshold, constant);

		// Update the irreducible part
		for(int i=0; i<rangeDim; ++i)
		{
			Polynomial polyTemp;
			polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

			Interval intTemp;
			polyTemp.intEvalNormal(intTemp, step_exp_table);
			intDifferences[i] = intTemp;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		bfound = true;
		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		// add the uncertainties
		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	order = newOrder;
	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

int Flowpipe::advance_low_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const std::vector<HornerForm> & taylorExpansion, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		std::vector<int> & orders, const std::vector<int> & maxOrders, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);

	TaylorModelVec x;

	for(int i=0; i<taylorExpansion.size(); ++i)
	{
		TaylorModel tmTemp;
		taylorExpansion[i].insert_no_remainder(tmTemp, x0, rangeDim+1, orders[i], cutoff_threshold);
		x.tms.push_back(tmTemp);
	}

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	std::vector<bool> bIncrease;
	for(int i=0; i<rangeDim; ++i)
	{
		if( !tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bIncrease.push_back(true);
			if(bfound)
				bfound = false;
		}
		else
		{
			bIncrease.push_back(false);
		}
	}

	std::vector<int> newOrders = orders;
	bool bIncreaseOthers = false;
	int numIncrease = 0;

	std::vector<bool> bIncreased;
	for(int i=0; i<rangeDim; ++i)
	{
		bIncreased.push_back(false);
	}

	for(; !bfound;)
	{
		bool bChanged = false;

		if(bIncreaseOthers)
		{
			for(int i=0; i<bIncrease.size(); ++i)
			{
				if(!bIncrease[i] && newOrders[i] < maxOrders[i])
				{
					++newOrders[i];
					bIncreased[i] = true;

					if(!bChanged)
						bChanged = true;
				}
			}

			bIncreaseOthers = false;
		}
		else
		{
			numIncrease = 0;
			for(int i=0; i<bIncrease.size(); ++i)
			{
				if(bIncrease[i] && newOrders[i] < maxOrders[i])
				{
					++newOrders[i];
					++numIncrease;

					bIncreased[i] = true;

					if(!bChanged)
						bChanged = true;
				}
			}

			if(numIncrease < newOrders.size())
				bIncreaseOthers = true;
		}

		if(!bChanged)
		{
			return 0;
		}

		// increase the approximation orders
		x.Picard_no_remainder_assign(x0, ode, rangeDim+1, newOrders, bIncreased, cutoff_threshold);
		x.cutoff(cutoff_threshold);

		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = estimation[i];
		}

		// compute the Picard operation again
		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, newOrders, cutoff_threshold, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			// update the irreducible part if necessary
			if(bIncreased[i])
			{
				Polynomial polyTemp;
				polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

				Interval intTemp;
				polyTemp.intEvalNormal(intTemp, step_exp_table);
				intDifferences[i] = intTemp;
			}
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		bfound = true;

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				if(!bIncreaseOthers)
				{
					bIncrease[i] = true;
					if(bfound)
						bfound = false;
				}
				else
				{
					bfound = false;
					break;
				}
			}
			else
			{
				if(!bIncreaseOthers)
				{
					bIncrease[i] = false;
				}
			}
		}

		for(int i=0; i<rangeDim; ++i)
		{
			bIncreased[i] = false;
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	orders = newOrders;
	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}























// for high-degree ODEs
// fixed step sizes and orders

int Flowpipe::advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}


	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}


	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=order; ++i)
	{
		c.Picard_no_remainder_assign(c0, ode_centered, rangeDim+1, i, cutoff_threshold);	// compute c(t)
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);


	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)
	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		TaylorModel tmTemp;
		ode_centered[i].insert_no_remainder(tmTemp, c_plus_Ar, rangeDim+1, order - 1, cutoff_threshold);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, order, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{

			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

int Flowpipe::advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}

	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=globalMaxOrder; ++i)
	{
		c.Picard_no_remainder_assign(c0, ode_centered, rangeDim+1, i, cutoff_threshold);	// compute c(t)
	}

	c.nctrunc(orders);

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt

	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)
	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		TaylorModel tmTemp;
		ode_centered[i].insert_no_remainder(tmTemp, c_plus_Ar, rangeDim+1, orders[i] - 1, cutoff_threshold);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, orders, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

// adaptive step sizes and fixed orders
int Flowpipe::advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}

	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}


	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=order; ++i)
	{
		c.Picard_no_remainder_assign(c0, ode_centered, rangeDim+1, i, cutoff_threshold);	// compute c(t)
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt


	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)
	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		TaylorModel tmTemp;
		ode_centered[i].insert_no_remainder(tmTemp, c_plus_Ar, rangeDim+1, order - 1, cutoff_threshold);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, order, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	if(step > THRESHOLD_HIGH)		// step size is changed
	{
		construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	}

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	std::vector<Polynomial> polyDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;
		polyDifferences.push_back(polyTemp);

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	for(; !bfound;)
	{
		bfound = true;
		double newStep = step_exp_table[1].sup() * LAMBDA_DOWN;	// reduce the time step size

		if(newStep < miniStep)
		{
			return 0;
		}

		construct_step_exp_table(step_exp_table, step_end_exp_table, newStep, 2*order);

		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

		// recompute the interval evaluation of the polynomial differences
		for(int i=0; i<rangeDim; ++i)
		{
			polyDifferences[i].intEvalNormal(intDifferences[i], step_exp_table);
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

int Flowpipe::advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const double step, const double miniStep, const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}

	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=globalMaxOrder; ++i)
	{
		c.Picard_no_remainder_assign(c0, ode_centered, rangeDim+1, i, cutoff_threshold);	// compute c(t)
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt


	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)
	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		TaylorModel tmTemp;
		ode_centered[i].insert_no_remainder(tmTemp, c_plus_Ar, rangeDim+1, orders[i] - 1, cutoff_threshold);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, orders, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	if(step > THRESHOLD_HIGH)		// step size is changed
	{
		construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	}

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	std::vector<Polynomial> polyDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;
		polyDifferences.push_back(polyTemp);

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	for(; !bfound;)
	{
		bfound = true;
		double newStep = step_exp_table[1].sup() * LAMBDA_DOWN;	// reduce the time step size

		if(newStep < miniStep)
		{
			return 0;
		}

		construct_step_exp_table(step_exp_table, step_end_exp_table, newStep, 2*globalMaxOrder);

		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			polyDifferences[i].intEvalNormal(intDifferences[i], step_exp_table);
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}


// adaptive orders and fixed step sizes
int Flowpipe::advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		int & order, const int maxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}

	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=order; ++i)
	{
		c.Picard_no_remainder_assign(c0, ode_centered, rangeDim+1, i, cutoff_threshold);	// compute c(t)
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt


	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)
	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		TaylorModel tmTemp;
		ode_centered[i].insert_no_remainder(tmTemp, c_plus_Ar, rangeDim+1, order - 1, cutoff_threshold);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, order, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	// add the uncertainties onto the reault
	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	int newOrder = order;

	for(; !bfound;)
	{
		++newOrder;

		if(newOrder > maxOrder)
		{
			return 0;
		}

		// increase the approximation orders by 1
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, newOrder, cutoff_threshold);
		x.cutoff(cutoff_threshold);

		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = estimation[i];
		}

		// compute the Picard operation again
		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, newOrder, cutoff_threshold, constant);

		// Update the irreducible part
		for(int i=0; i<rangeDim; ++i)
		{
			Polynomial polyTemp;
			polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

			Interval intTemp;
			polyTemp.intEvalNormal(intTemp, step_exp_table);
			intDifferences[i] = intTemp;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		bfound = true;
		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	order = newOrder;
	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

int Flowpipe::advance_high_degree(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		std::vector<int> & orders, const int localMaxOrder, const std::vector<int> & maxOrders, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);
	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}

	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=localMaxOrder; ++i)
	{
		c.Picard_no_remainder_assign(c0, ode_centered, rangeDim+1, i, cutoff_threshold);	// compute c(t)
	}

	c.nctrunc(orders);

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt

	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)
	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		TaylorModel tmTemp;
		ode_centered[i].insert_no_remainder(tmTemp, c_plus_Ar, rangeDim+1, orders[i] - 1, cutoff_threshold);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, orders, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, orders, cutoff_threshold, constant);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	std::vector<bool> bIncrease;
	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bIncrease.push_back(true);
			if(bfound)
				bfound = false;
		}
		else
		{
			bIncrease.push_back(false);
		}
	}

	std::vector<int> newOrders = orders;
	bool bIncreaseOthers = false;
	int numIncrease = 0;

	std::vector<bool> bIncreased;
	for(int i=0; i<rangeDim; ++i)
	{
		bIncreased.push_back(false);
	}

	for(; !bfound;)
	{
		bool bChanged = false;

		if(bIncreaseOthers)
		{
			for(int i=0; i<bIncrease.size(); ++i)
			{
				if(!bIncrease[i] && newOrders[i] < maxOrders[i])
				{
					++newOrders[i];
					bIncreased[i] = true;

					if(!bChanged)
						bChanged = true;
				}
			}

			bIncreaseOthers = false;
		}
		else
		{
			numIncrease = 0;
			for(int i=0; i<bIncrease.size(); ++i)
			{
				if(bIncrease[i] && newOrders[i] < maxOrders[i])
				{
					++newOrders[i];
					++numIncrease;

					bIncreased[i] = true;

					if(!bChanged)
						bChanged = true;
				}
			}

			if(numIncrease < newOrders.size())
				bIncreaseOthers = true;
		}

		if(!bChanged)
		{
			return 0;
		}

		// increase the approximation orders
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, newOrders, bIncreased, cutoff_threshold);
		x.cutoff(cutoff_threshold);

		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = estimation[i];
		}

		// compute the Picard operation again
		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, newOrders, cutoff_threshold, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			// update the irreducible part if necessary
			if(bIncreased[i])
			{
				Polynomial polyTemp;
				polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

				Interval intTemp;
				polyTemp.intEvalNormal(intTemp, step_exp_table);
				intDifferences[i] = intTemp;
			}
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		bfound = true;

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				if(!bIncreaseOthers)
				{
					bIncrease[i] = true;
					if(bfound)
						bfound = false;
				}
				else
				{
					bfound = false;
					break;
				}
			}
			else
			{
				if(!bIncreaseOthers)
				{
					bIncrease[i] = false;
				}
			}
		}

		for(int i=0; i<rangeDim; ++i)
		{
			bIncreased[i] = false;
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	orders = newOrders;
	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}







// integration scheme for non-polynomial ODEs (using Taylor approximations)
// fixed step sizes and orders
int Flowpipe::advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	int rangeDim = strOde.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}

	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=order; ++i)
	{
		c.Picard_non_polynomial_taylor_no_remainder_assign(c0, strOde_centered, i, cutoff_threshold);
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt


	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)

	parseSetting.clear();
	parseSetting.order = order - 1;
	parseSetting.flowpipe = c_plus_Ar;
	parseSetting.cutoff_threshold = cutoff_threshold;

	std::string prefix(str_prefix_taylor_polynomial);
	std::string suffix(str_suffix);

	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		parseSetting.strODE = prefix + strOde_centered[i] + suffix;

		parseODE();		// call the parser

		TaylorModel tmTemp(parseResult.expansion, intZero);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, order, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, order, cutoff_threshold, constant, constant_part);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_non_polynomial_taylor_only_remainder(newRemainders, x0, strOde, step_exp_table[1], order, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return 1;
}

int Flowpipe::advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	int rangeDim = strOde.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}

	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=globalMaxOrder; ++i)
	{
		c.Picard_non_polynomial_taylor_no_remainder_assign(c0, strOde_centered, i, cutoff_threshold);
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt


	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)

	parseSetting.clear();
	parseSetting.flowpipe = c_plus_Ar;
	parseSetting.cutoff_threshold = cutoff_threshold;

	std::string prefix(str_prefix_taylor_polynomial);
	std::string suffix(str_suffix);

	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		parseSetting.strODE = prefix + strOde_centered[i] + suffix;
		parseSetting.order = orders[i] - 1;

		parseODE();		// call the parser

		TaylorModel tmTemp(parseResult.expansion, intZero);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, orders, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];// + step_uncertainties[i];	// apply the remainder estimation
	}

	TaylorModelVec tmvTemp;
	x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, orders, cutoff_threshold, constant, constant_part);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_non_polynomial_taylor_only_remainder(newRemainders, x0, strOde, step_exp_table[1], orders, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return -1;
}


// adaptive step sizes and fixed orders
int Flowpipe::advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table, const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	int rangeDim = strOde.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}

	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=order; ++i)
	{
		c.Picard_non_polynomial_taylor_no_remainder_assign(c0, strOde_centered, i, cutoff_threshold);
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt

	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)

	parseSetting.clear();
	parseSetting.flowpipe = c_plus_Ar;
	parseSetting.order = order - 1;
	parseSetting.cutoff_threshold = cutoff_threshold;

	std::string prefix(str_prefix_taylor_polynomial);
	std::string suffix(str_suffix);

	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		parseSetting.strODE = prefix + strOde_centered[i] + suffix;

		parseODE();		// call the parser

		TaylorModel tmTemp(parseResult.expansion, intZero);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, order, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	if(step > THRESHOLD_HIGH)		// step size is changed
	{
		construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	}

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, order, cutoff_threshold, constant, constant_part);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	std::vector<Polynomial> polyDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;
		polyDifferences.push_back(polyTemp);

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	for(; !bfound;)
	{
		bfound = true;
		double newStep = step_exp_table[1].sup() * LAMBDA_DOWN;	// reduce the time step size

		if(newStep < miniStep)
		{
			return 0;
		}

		construct_step_exp_table(step_exp_table, step_end_exp_table, newStep, 2*order);

		x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, order, cutoff_threshold, constant, constant_part);

		// recompute the interval evaluation of the polynomial differences
		for(int i=0; i<rangeDim; ++i)
		{
			polyDifferences[i].intEvalNormal(intDifferences[i], step_exp_table);
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_non_polynomial_taylor_only_remainder(newRemainders, x0, strOde, step_exp_table[1], order, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return 1;
}

int Flowpipe::advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table, const double step, const double miniStep, const std::vector<int> & orders, const int globalMaxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	int rangeDim = strOde.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}

	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=globalMaxOrder; ++i)
	{
		c.Picard_non_polynomial_taylor_no_remainder_assign(c0, strOde_centered, i, cutoff_threshold);
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt

	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)

	parseSetting.clear();
	parseSetting.flowpipe = c_plus_Ar;
	parseSetting.cutoff_threshold = cutoff_threshold;

	std::string prefix(str_prefix_taylor_polynomial);
	std::string suffix(str_suffix);

	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		parseSetting.strODE = prefix + strOde_centered[i] + suffix;
		parseSetting.order = orders[i] - 1;

		parseODE();		// call the parser

		TaylorModel tmTemp(parseResult.expansion, intZero);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, orders, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	if(step > THRESHOLD_HIGH)		// step size is changed
	{
		construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	}

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, orders, cutoff_threshold, constant, constant_part);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	std::vector<Polynomial> polyDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;
		polyDifferences.push_back(polyTemp);

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	for(; !bfound;)
	{
		bfound = true;
		double newStep = step_exp_table[1].sup() * LAMBDA_DOWN;	// reduce the time step size

		if(newStep < miniStep)
		{
			return 0;
		}

		construct_step_exp_table(step_exp_table, step_end_exp_table, newStep, 2*globalMaxOrder);

		x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, orders, cutoff_threshold, constant, constant_part);

		// recompute the interval evaluation of the polynomial differences
		for(int i=0; i<rangeDim; ++i)
		{
			polyDifferences[i].intEvalNormal(intDifferences[i], step_exp_table);
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_non_polynomial_taylor_only_remainder(newRemainders, x0, strOde, step_exp_table[1], orders, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return 1;
}


// adaptive orders and fixed step sizes
int Flowpipe::advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table, int & order, const int maxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	int rangeDim = strOde.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}

	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=order; ++i)
	{
		c.Picard_non_polynomial_taylor_no_remainder_assign(c0, strOde_centered, i, cutoff_threshold);
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt

	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)

	parseSetting.clear();
	parseSetting.order = order - 1;
	parseSetting.flowpipe = c_plus_Ar;
	parseSetting.cutoff_threshold = cutoff_threshold;

	std::string prefix(str_prefix_taylor_polynomial);
	std::string suffix(str_suffix);

	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		parseSetting.strODE = prefix + strOde_centered[i] + suffix;

		parseODE();		// call the parser

		TaylorModel tmTemp(parseResult.expansion, intZero);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, order, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, order, cutoff_threshold, constant, constant_part);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	int newOrder = order;

	for(; !bfound;)
	{
		++newOrder;

		if(newOrder > maxOrder)
		{
			return 0;
		}

		// increase the approximation orders by 1
		x.Picard_non_polynomial_taylor_no_remainder_assign(x0, strOde_centered, newOrder, cutoff_threshold);
		x.cutoff(cutoff_threshold);

		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = estimation[i];
		}

		// compute the Picard operation again
		x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, newOrder, cutoff_threshold, constant, constant_part);

		// Update the irreducible part
		for(int i=0; i<rangeDim; ++i)
		{
			Polynomial polyTemp;
			polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

			Interval intTemp;
			polyTemp.intEvalNormal(intTemp, step_exp_table);
			intDifferences[i] = intTemp;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		bfound = true;
		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_non_polynomial_taylor_only_remainder(newRemainders, x0, strOde, step_exp_table[1], newOrder, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	order = newOrder;
	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return 1;
}

int Flowpipe::advance_non_polynomial_taylor(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table, std::vector<int> & orders, const int localMaxOrder, const std::vector<int> & maxOrders, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	int rangeDim = strOde.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), orders, cutoff_threshold);

	std::vector<Interval> boundOfx0;

	// contract the remainder part of the initial set
	if(invariant.size() > 0)
	{
		std::vector<Interval> polyRangeOfx0;
		result.tmv.polyRangeNormal(polyRangeOfx0, step_end_exp_table);

		std::vector<Interval> polyRangeOfx0_wc;
		for(int i=0; i<polyRangeOfx0.size(); ++i)
		{
			polyRangeOfx0_wc.push_back(polyRangeOfx0[i] + intVecCenter[i]);
		}

		std::vector<Interval> contracted_remainders;
		for(int i=0; i<rangeDim; ++i)
		{
			contracted_remainders.push_back(result.tmv.tms[i].remainder);
		}

		int res = contract_remainder(polyRangeOfx0_wc, contracted_remainders, invariant);

		if(res < 0)
		{
			return -1;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = contracted_remainders[i];
			boundOfx0.push_back(polyRangeOfx0[i] + contracted_remainders[i]);
		}
	}
	else
	{
		result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);
	}

	std::vector<int> zeroIndices;
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			zeroIndices.push_back(i+1);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=localMaxOrder; ++i)
	{
		c.Picard_non_polynomial_taylor_no_remainder_assign(c0, strOde_centered, i, cutoff_threshold);
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt


	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)

	parseSetting.clear();
	parseSetting.flowpipe = c_plus_Ar;
	parseSetting.cutoff_threshold = cutoff_threshold;

	std::string prefix(str_prefix_taylor_polynomial);
	std::string suffix(str_suffix);

	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		parseSetting.strODE = prefix + strOde_centered[i] + suffix;
		parseSetting.order = orders[i] - 1;

		parseODE();		// call the parser

		TaylorModel tmTemp(parseResult.expansion, intZero);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, orders, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, orders, cutoff_threshold, constant, constant_part);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	std::vector<bool> bIncrease;
	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bIncrease.push_back(true);
			if(bfound)
				bfound = false;
		}
		else
		{
			bIncrease.push_back(false);
		}
	}

	std::vector<int> newOrders = orders;
	bool bIncreaseOthers = false;
	int numIncrease = 0;

	std::vector<bool> bIncreased;
	for(int i=0; i<rangeDim; ++i)
	{
		bIncreased.push_back(false);
	}

	for(; !bfound;)
	{
		bool bChanged = false;

		if(bIncreaseOthers)
		{
			for(int i=0; i<bIncrease.size(); ++i)
			{
				if(!bIncrease[i] && newOrders[i] < maxOrders[i])
				{
					++newOrders[i];
					bIncreased[i] = true;

					if(!bChanged)
						bChanged = true;
				}
			}

			bIncreaseOthers = false;
		}
		else
		{
			numIncrease = 0;
			for(int i=0; i<bIncrease.size(); ++i)
			{
				if(bIncrease[i] && newOrders[i] < maxOrders[i])
				{
					++newOrders[i];
					++numIncrease;

					bIncreased[i] = true;

					if(!bChanged)
						bChanged = true;
				}
			}

			if(numIncrease < newOrders.size())
				bIncreaseOthers = true;
		}

		if(!bChanged)
		{
			return 0;
		}

		// increase the approximation orders
		x.Picard_non_polynomial_taylor_no_remainder_assign(x0, strOde_centered, newOrders, bIncreased, cutoff_threshold);
		x.cutoff(cutoff_threshold);

		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = estimation[i];
		}

		// compute the Picard operation again
		x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, newOrders, cutoff_threshold, constant, constant_part);

		for(int i=0; i<rangeDim; ++i)
		{
			// update the irreducible part if necessary
			if(bIncreased[i])
			{
				Polynomial polyTemp;
				polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

				Interval intTemp;
				polyTemp.intEvalNormal(intTemp, step_exp_table);
				intDifferences[i] = intTemp;
			}
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		bfound = true;

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				if(!bIncreaseOthers)
				{
					bIncrease[i] = true;
					if(bfound)
						bfound = false;
				}
				else
				{
					bfound = false;
					break;
				}
			}
			else
			{
				if(!bIncreaseOthers)
				{
					bIncrease[i] = false;
				}
			}
		}

		for(int i=0; i<rangeDim; ++i)
		{
			bIncreased[i] = false;
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_non_polynomial_taylor_only_remainder(newRemainders, x0, strOde, step_exp_table[1], newOrders, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	orders = newOrders;
	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return 1;
}





int Flowpipe::advance_picard_symbolic_remainder(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const int order, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	// decompose the linear and nonlinear part
	TaylorModelVec x0_linear, x0_other;
	range_of_x0.decompose(x0_linear, x0_other);

	iMatrix Phi_L_i(rangeDim, rangeDim);

	x0_linear.linearCoefficients(Phi_L_i);

	iMatrix local_trans_linear = Phi_L_i;

	Phi_L_i.right_scale_assign(scalars);

	// compute the remainder part under the linear transformation
	iMatrix J_i(rangeDim, 1);

	for(int i=0; i<Phi_L.size(); ++i)
	{
		Phi_L[i] = Phi_L_i * Phi_L[i];
	}

	Phi_L.push_back(Phi_L_i);

	for(int i=1; i<Phi_L.size(); ++i)
	{
		J_i += Phi_L[i] * J[i-1];
	}

	iMatrix J_ip1(rangeDim, 1);

	// compute the local initial set
	if(J.size() > 0)
	{
		// compute the polynomial part under the linear transformation
		std::vector<Polynomial> initial_linear;
		Phi_L[0].linearTrans(initial_linear, initial_set_poly);

		// compute the other part
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		x0_other.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

		result.tmv.Remainder(J_ip1);

		iMatrix x0_rem(rangeDim, 1);
		range_of_x0.Remainder(x0_rem);
		J_ip1 += x0_rem;

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].expansion += initial_linear[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = J_ip1[i][0] + J_i[i][0];
		}
	}
	else
	{
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);
		result.tmv.Remainder(J_ip1);
	}

	J.push_back(J_ip1);

	std::vector<Interval> boundOfx0;
	result.tmv.intEvalNormal(boundOfx0, step_exp_table);

	// Compute the scaling matrix S
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			scalars[i] = intZero;
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
			scalars[i] = intRecSup;
		}
	}

	// apply the scaling matrix S
	result.tmv.scale_assign(invS);

	TaylorModelVec x, x0;

	TaylorModelVec c0_plus_Ar0(S);
	c0_plus_Ar0.add_assign(c0);

	x0 = c0_plus_Ar0;
	x = x0;

	for(int i=1; i<=order; ++i)
	{
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, i, cutoff_threshold);
	}

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);


	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	// add the uncertainties and the cutoff intervals onto the result
	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		// add the uncertainties and the cutoff intervals onto the result
		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{

			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

int Flowpipe::advance_picard_symbolic_remainder(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);


	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	// decompose the linear and nonlinear part
	TaylorModelVec x0_linear, x0_other;
	range_of_x0.decompose(x0_linear, x0_other);

	iMatrix Phi_L_i(rangeDim, rangeDim);

	x0_linear.linearCoefficients(Phi_L_i);

	iMatrix local_trans_linear = Phi_L_i;

	Phi_L_i.right_scale_assign(scalars);

	// compute the remainder part under the linear transformation
	iMatrix J_i(rangeDim, 1);

	for(int i=0; i<Phi_L.size(); ++i)
	{
		Phi_L[i] = Phi_L_i * Phi_L[i];
	}

	Phi_L.push_back(Phi_L_i);

	for(int i=1; i<Phi_L.size(); ++i)
	{
		J_i += Phi_L[i] * J[i-1];
	}

	iMatrix J_ip1(rangeDim, 1);

	// compute the local initial set
	if(J.size() > 0)
	{
		// compute the polynomial part under the linear transformation
		std::vector<Polynomial> initial_linear;
		Phi_L[0].linearTrans(initial_linear, initial_set_poly);

		// compute the other part
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		x0_other.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

		result.tmv.Remainder(J_ip1);

		iMatrix x0_rem(rangeDim, 1);
		range_of_x0.Remainder(x0_rem);
		J_ip1 += x0_rem;

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].expansion += initial_linear[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = J_ip1[i][0] + J_i[i][0];
		}
	}
	else
	{
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);
		result.tmv.Remainder(J_ip1);
	}

	J.push_back(J_ip1);

	std::vector<Interval> boundOfx0;
	result.tmv.intEvalNormal(boundOfx0, step_exp_table);

	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			scalars[i] = intZero;
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
			scalars[i] = intRecSup;
		}
	}

	// apply the scaling matrix S
	result.tmv.scale_assign(invS);

	TaylorModelVec x, x0;
	TaylorModelVec c0_plus_Ar0(S);
	c0_plus_Ar0.add_assign(c0);

	x0 = c0_plus_Ar0;
	x = x0;

	for(int i=1; i<=order; ++i)
	{
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, i, cutoff_threshold);
	}

	x.cutoff(cutoff_threshold);

	if(step > THRESHOLD_HIGH)		// step size is changed
	{
		construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	}

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);


	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	std::vector<Polynomial> polyDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;
		polyDifferences.push_back(polyTemp);

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	for(; !bfound;)
	{
		bfound = true;
		double newStep = step_exp_table[1].sup() * LAMBDA_DOWN;	// reduce the time step size

		if(newStep < miniStep)
		{
			return 0;
		}

		construct_step_exp_table(step_exp_table, step_end_exp_table, newStep, 2*order);

		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);

		// recompute the interval evaluation of the polynomial differences
		for(int i=0; i<rangeDim; ++i)
		{
			polyDifferences[i].intEvalNormal(intDifferences[i], step_exp_table);
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		// add the uncertainties and the cutoff intervals onto the result
		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{

			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}

int Flowpipe::advance_picard_symbolic_remainder(Flowpipe & result, const std::vector<HornerForm> & ode, const std::vector<HornerForm> & ode_centered, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		int & order, const int maxOrder, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L, const std::vector<bool> & constant) const
{
	int rangeDim = ode.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);


	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	// decompose the linear and nonlinear part
	TaylorModelVec x0_linear, x0_other;
	range_of_x0.decompose(x0_linear, x0_other);

	iMatrix Phi_L_i(rangeDim, rangeDim);

	x0_linear.linearCoefficients(Phi_L_i);

	iMatrix local_trans_linear = Phi_L_i;

	Phi_L_i.right_scale_assign(scalars);

	// compute the remainder part under the linear transformation
	iMatrix J_i(rangeDim, 1);

	for(int i=0; i<Phi_L.size(); ++i)
	{
		Phi_L[i] = Phi_L_i * Phi_L[i];
	}

	Phi_L.push_back(Phi_L_i);

	for(int i=1; i<Phi_L.size(); ++i)
	{
		J_i += Phi_L[i] * J[i-1];
	}

	iMatrix J_ip1(rangeDim, 1);

	// compute the local initial set
	if(J.size() > 0)
	{
		// compute the polynomial part under the linear transformation
		std::vector<Polynomial> initial_linear;
		Phi_L[0].linearTrans(initial_linear, initial_set_poly);

		// compute the other part
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		x0_other.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

		result.tmv.Remainder(J_ip1);

		iMatrix x0_rem(rangeDim, 1);
		range_of_x0.Remainder(x0_rem);
		J_ip1 += x0_rem;

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].expansion += initial_linear[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = J_ip1[i][0] + J_i[i][0];
		}
	}
	else
	{
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);
		result.tmv.Remainder(J_ip1);
	}

	J.push_back(J_ip1);

	std::vector<Interval> boundOfx0;
	result.tmv.intEvalNormal(boundOfx0, step_exp_table);

	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			scalars[i] = intZero;
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
			scalars[i] = intRecSup;
		}
	}

	// apply the scaling matrix S
	result.tmv.scale_assign(invS);

	TaylorModelVec x, x0;

	TaylorModelVec c0_plus_Ar0(S);
	c0_plus_Ar0.add_assign(c0);

	x0 = c0_plus_Ar0;
	x = x0;

	for(int i=1; i<=order; ++i)
	{
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, i, cutoff_threshold);
	}

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	std::vector<RangeTree *> trees;

	std::vector<Interval> xPolyRange;
	x.polyRangeNormal(xPolyRange, step_exp_table);
	x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, order, cutoff_threshold, constant);


	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	// add the uncertainties and the cutoff intervals onto the result
	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	int newOrder = order;

	for(; !bfound;)
	{
		++newOrder;

		if(newOrder > maxOrder)
		{
			return 0;
		}

		// increase the approximation orders by 1
		x.Picard_no_remainder_assign(x0, ode_centered, rangeDim+1, newOrder, cutoff_threshold);
		x.cutoff(cutoff_threshold);

		for(int i=0; i<rangeDim; ++i)	// apply the estimation again
		{
			x.tms[i].remainder = estimation[i];
		}

		// compute the Picard operation again
		x.polyRangeNormal(xPolyRange, step_exp_table);
		x.Picard_ctrunc_normal(tmvTemp, trees, x0, xPolyRange, ode, step_exp_table, rangeDim+1, newOrder, cutoff_threshold, constant);

		// Update the irreducible part
		for(int i=0; i<rangeDim; ++i)
		{
			Polynomial polyTemp;
			polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

			Interval intTemp;
			polyTemp.intEvalNormal(intTemp, step_exp_table);
			intDifferences[i] = intTemp;
		}

		// add the uncertainties onto the result
		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		bfound = true;
		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_only_remainder(newRemainders, trees, x0, ode, step_exp_table[1], constant);

		// add the uncertainties and the cutoff intervals onto the result
		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{

			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	order = newOrder;
	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	trees.clear();
	return 1;
}





// non-polynomial ODEs
int Flowpipe::advance_non_polynomial_taylor_symbolic_remainder(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L,
		const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	int rangeDim = strOde.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	// decompose the linear and nonlinear part
	TaylorModelVec x0_linear, x0_other;
	range_of_x0.decompose(x0_linear, x0_other);

	iMatrix Phi_L_i(rangeDim, rangeDim);

	x0_linear.linearCoefficients(Phi_L_i);

	iMatrix local_trans_linear = Phi_L_i;

	Phi_L_i.right_scale_assign(scalars);

	// compute the remainder part under the linear transformation
	iMatrix J_i(rangeDim, 1);

	for(int i=0; i<Phi_L.size(); ++i)
	{
		Phi_L[i] = Phi_L_i * Phi_L[i];
	}

	Phi_L.push_back(Phi_L_i);

	for(int i=1; i<Phi_L.size(); ++i)
	{
		J_i += Phi_L[i] * J[i-1];
	}

	iMatrix J_ip1(rangeDim, 1);

	// compute the local initial set
	if(J.size() > 0)
	{
		// compute the polynomial part under the linear transformation
		std::vector<Polynomial> initial_linear;
		Phi_L[0].linearTrans(initial_linear, initial_set_poly);

		// compute the other part
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		x0_other.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

		result.tmv.Remainder(J_ip1);

		iMatrix x0_rem(rangeDim, 1);
		range_of_x0.Remainder(x0_rem);
		J_ip1 += x0_rem;

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].expansion += initial_linear[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = J_ip1[i][0] + J_i[i][0];
		}
	}
	else
	{
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);
		result.tmv.Remainder(J_ip1);
	}

	J.push_back(J_ip1);

	std::vector<Interval> boundOfx0;
	result.tmv.intEvalNormal(boundOfx0, step_exp_table);

	// vector storing the zero ranges
	std::vector<int> zeroIndices;

	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			scalars[i] = intZero;
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
			scalars[i] = intRecSup;
		}
	}

	// apply the scaling matrix S
	result.tmv.scale_assign(invS);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=order; ++i)
	{
		c.Picard_non_polynomial_taylor_no_remainder_assign(c0, strOde_centered, i, cutoff_threshold);
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt

	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)

	parseSetting.clear();
	parseSetting.order = order - 1;
	parseSetting.flowpipe = c_plus_Ar;
	parseSetting.cutoff_threshold = cutoff_threshold;

	std::string prefix(str_prefix_taylor_polynomial);
	std::string suffix(str_suffix);

	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		parseSetting.strODE = prefix + strOde_centered[i] + suffix;

		parseODE();		// call the parser

		TaylorModel tmTemp(parseResult.expansion, intZero);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, order, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, order, cutoff_threshold, constant, constant_part);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_non_polynomial_taylor_only_remainder(newRemainders, x0, strOde, step_exp_table[1], order, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return 1;
}


int Flowpipe::advance_non_polynomial_taylor_symbolic_remainder(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const double step, const double miniStep, const int order, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L,
		const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	int rangeDim = strOde.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	// decompose the linear and nonlinear part
	TaylorModelVec x0_linear, x0_other;
	range_of_x0.decompose(x0_linear, x0_other);

	iMatrix Phi_L_i(rangeDim, rangeDim);

	x0_linear.linearCoefficients(Phi_L_i);

	iMatrix local_trans_linear = Phi_L_i;

	Phi_L_i.right_scale_assign(scalars);

	// compute the remainder part under the linear transformation
	iMatrix J_i(rangeDim, 1);

	for(int i=0; i<Phi_L.size(); ++i)
	{
		Phi_L[i] = Phi_L_i * Phi_L[i];
	}

	Phi_L.push_back(Phi_L_i);

	for(int i=1; i<Phi_L.size(); ++i)
	{
		J_i += Phi_L[i] * J[i-1];
	}

	iMatrix J_ip1(rangeDim, 1);

	// compute the local initial set
	if(J.size() > 0)
	{
		// compute the polynomial part under the linear transformation
		std::vector<Polynomial> initial_linear;
		Phi_L[0].linearTrans(initial_linear, initial_set_poly);

		// compute the other part
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		x0_other.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

		result.tmv.Remainder(J_ip1);

		iMatrix x0_rem(rangeDim, 1);
		range_of_x0.Remainder(x0_rem);
		J_ip1 += x0_rem;

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].expansion += initial_linear[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = J_ip1[i][0] + J_i[i][0];
		}
	}
	else
	{
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);
		result.tmv.Remainder(J_ip1);
	}

	J.push_back(J_ip1);

	std::vector<Interval> boundOfx0;
	result.tmv.intEvalNormal(boundOfx0, step_exp_table);

	// vector storing the zero ranges
	std::vector<int> zeroIndices;

	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			scalars[i] = intZero;
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
			scalars[i] = intRecSup;
		}
	}

	// apply the scaling matrix S
	result.tmv.scale_assign(invS);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=order; ++i)
	{
		c.Picard_non_polynomial_taylor_no_remainder_assign(c0, strOde_centered, i, cutoff_threshold);
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt

	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)

	parseSetting.clear();
	parseSetting.order = order - 1;
	parseSetting.flowpipe = c_plus_Ar;
	parseSetting.cutoff_threshold = cutoff_threshold;

	std::string prefix(str_prefix_taylor_polynomial);
	std::string suffix(str_suffix);

	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		parseSetting.strODE = prefix + strOde_centered[i] + suffix;

		parseODE();		// call the parser

		TaylorModel tmTemp(parseResult.expansion, intZero);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, order, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);


	if(step > THRESHOLD_HIGH)		// step size is changed
	{
		construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	}


	TaylorModelVec x0;
	Ar0.add(x0, c0);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, order, cutoff_threshold, constant, constant_part);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	std::vector<Polynomial> polyDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;
		polyDifferences.push_back(polyTemp);

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);
		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	for(; !bfound;)
	{
		bfound = true;
		double newStep = step_exp_table[1].sup() * LAMBDA_DOWN;	// reduce the time step size

		if(newStep < miniStep)
		{
			return 0;
		}

		construct_step_exp_table(step_exp_table, step_end_exp_table, newStep, 2*order);

		x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, order, cutoff_threshold, constant, constant_part);

		// recompute the interval evaluation of the polynomial differences
		for(int i=0; i<rangeDim; ++i)
		{
			polyDifferences[i].intEvalNormal(intDifferences[i], step_exp_table);
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_non_polynomial_taylor_only_remainder(newRemainders, x0, strOde, step_exp_table[1], order, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return 1;
}

// adaptive orders and fixed step sizes
int Flowpipe::advance_non_polynomial_taylor_symbolic_remainder(Flowpipe & result, const std::vector<std::string> & strOde, const std::vector<std::string> & strOde_centered, const int precondition, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		int & order, const int maxOrder, const std::vector<Interval> & estimation, const std::vector<PolynomialConstraint> & invariant, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L,
		const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	int rangeDim = strOde.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	// decompose the linear and nonlinear part
	TaylorModelVec x0_linear, x0_other;
	range_of_x0.decompose(x0_linear, x0_other);

	iMatrix Phi_L_i(rangeDim, rangeDim);

	x0_linear.linearCoefficients(Phi_L_i);

	iMatrix local_trans_linear = Phi_L_i;

	Phi_L_i.right_scale_assign(scalars);

	// compute the remainder part under the linear transformation
	iMatrix J_i(rangeDim, 1);

	for(int i=0; i<Phi_L.size(); ++i)
	{
		Phi_L[i] = Phi_L_i * Phi_L[i];
	}

	Phi_L.push_back(Phi_L_i);

	for(int i=1; i<Phi_L.size(); ++i)
	{
		J_i += Phi_L[i] * J[i-1];
	}

	iMatrix J_ip1(rangeDim, 1);

	// compute the local initial set
	if(J.size() > 0)
	{
		// compute the polynomial part under the linear transformation
		std::vector<Polynomial> initial_linear;
		Phi_L[0].linearTrans(initial_linear, initial_set_poly);

		// compute the other part
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		x0_other.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

		result.tmv.Remainder(J_ip1);

		iMatrix x0_rem(rangeDim, 1);
		range_of_x0.Remainder(x0_rem);
		J_ip1 += x0_rem;

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].expansion += initial_linear[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = J_ip1[i][0] + J_i[i][0];
		}
	}
	else
	{
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);
		result.tmv.Remainder(J_ip1);
	}

	J.push_back(J_ip1);

	std::vector<Interval> boundOfx0;
	result.tmv.intEvalNormal(boundOfx0, step_exp_table);

	// vector storing the zero ranges
	std::vector<int> zeroIndices;

	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			scalars[i] = intZero;
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
			scalars[i] = intRecSup;
		}
	}

	// apply the scaling matrix S
	result.tmv.scale_assign(invS);

	// compute the Taylor expansion of r(t)
	// since r(t) = A^{-1} * (x(t) - c(t)), we have that r'(t) = A^{-1} * (x'(t) - c'(t)) = A^{-1} * (f(c(t) + A*r(t), t) - c'(t))

	TaylorModelVec c = c0;
	for(int i=1; i<=order; ++i)
	{
		c.Picard_non_polynomial_taylor_no_remainder_assign(c0, strOde_centered, i, cutoff_threshold);
	}

	TaylorModelVec dcdt;
	c.derivative(dcdt, 0);	// compute dc/dt

	TaylorModelVec Ar0(S);
	TaylorModelVec c_plus_Ar;
	Ar0.add(c_plus_Ar, c);

	// compute the Taylor expansion of the ODE of A*r(t)

	parseSetting.clear();
	parseSetting.order = order - 1;
	parseSetting.flowpipe = c_plus_Ar;
	parseSetting.cutoff_threshold = cutoff_threshold;

	std::string prefix(str_prefix_taylor_polynomial);
	std::string suffix(str_suffix);

	TaylorModelVec Adrdt;
	for(int i=0; i<rangeDim; ++i)
	{
		parseSetting.strODE = prefix + strOde_centered[i] + suffix;

		parseODE();		// call the parser

		TaylorModel tmTemp(parseResult.expansion, intZero);
		Adrdt.tms.push_back(tmTemp);
	}

	Adrdt.sub_assign(dcdt);

	TaylorModelVec drdt;
	Adrdt.scale(drdt, invS);

	TaylorModelVec taylorExp_Ar;

	computeTaylorExpansion(taylorExp_Ar, Adrdt, drdt, order, cutoff_threshold);

	// remove the zero terms
	taylorExp_Ar.rmZeroTerms(zeroIndices);
	Ar0.rmZeroTerms(zeroIndices);

	taylorExp_Ar.add_assign(Ar0);

	TaylorModelVec x = c;
	x.add_assign(taylorExp_Ar);		// the Taylor expansion of x(t)

	x.cutoff(cutoff_threshold);

	TaylorModelVec x0;
	Ar0.add(x0, c0);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;
	x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, order, cutoff_threshold, constant, constant_part);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	int newOrder = order;

	for(; !bfound;)
	{
		++newOrder;

		if(newOrder > maxOrder)
		{
			return 0;
		}

		// increase the approximation orders by 1
		x.Picard_non_polynomial_taylor_no_remainder_assign(x0, strOde_centered, newOrder, cutoff_threshold);
		x.cutoff(cutoff_threshold);

		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = estimation[i];
		}

		// compute the Picard operation again
		x.Picard_non_polynomial_taylor_ctrunc_normal(tmvTemp, x0, strOde, step_exp_table, newOrder, cutoff_threshold, constant, constant_part);

		// Update the irreducible part
		for(int i=0; i<rangeDim; ++i)
		{
			Polynomial polyTemp;
			polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

			Interval intTemp;
			polyTemp.intEvalNormal(intTemp, step_exp_table);
			intDifferences[i] = intTemp;
		}

		for(int i=0; i<rangeDim; ++i)
		{
			tmvTemp.tms[i].remainder += intDifferences[i];
		}

		bfound = true;
		for(int i=0; i<rangeDim; ++i)
		{
			if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
			{
				bfound = false;
				break;
			}
		}
	}

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = tmvTemp.tms[i].remainder;
	}


	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_non_polynomial_taylor_only_remainder(newRemainders, x0, strOde, step_exp_table[1], order, constant);

		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	order = newOrder;
	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return 1;
}


Flowpipe & Flowpipe::operator = (const Flowpipe & flowpipe)
{
	if(this == &flowpipe)
		return *this;

	tmvPre = flowpipe.tmvPre;
	tmv = flowpipe.tmv;
	domain = flowpipe.domain;
	return *this;
}


int Flowpipe::advance_picard(Flowpipe & result, const std::vector<Expression_AST> & ode, const std::vector<Expression_AST> & ode_centered, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const int order, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	int rangeDim = tmvPre.tms.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	std::vector<Interval> boundOfx0;
	result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);


	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);

	TaylorModelVec x0(S);
	x0.add_assign(c0);
	TaylorModelVec x = x0;

	for(int i=1; i<=order; ++i)
	{
		x.Picard_trunc_no_remainder_assign(x0, ode_centered, rangeDim+1, i, cutoff_threshold, constant, constant_part);
	}

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;

	std::list<Interval> intermediate_ranges;
	x.Picard_ctrunc_normal(tmvTemp, x0, ode, step_exp_table, order, rangeDim+1, cutoff_threshold, constant, constant_part, intermediate_ranges);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	// add the uncertainties and the cutoff intervals onto the result
	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_ctrunc_normal_remainder(newRemainders, x0, ode, step_exp_table[1], order, constant, intermediate_ranges);

		// add the uncertainties and the cutoff intervals onto the result
		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{

			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}


	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return 1;
}

int Flowpipe::advance_picard_symbolic_remainder(Flowpipe & result, const std::vector<Expression_AST> & ode, const std::vector<Expression_AST> & ode_centered, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table,
		const int order, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, const std::vector<Polynomial> & initial_set_poly, std::vector<Interval> & scalars, std::vector<iMatrix> & J, std::vector<iMatrix> & Phi_L,
		const std::vector<bool> & constant, const std::vector<Interval> & constant_part) const
{
	int rangeDim = tmvPre.tms.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	// introduce a new variable r0 such that x0 = c0 + A*r0, then r0 is origin-centered
	range_of_x0.rmConstant();

	// decompose the linear and nonlinear part
	TaylorModelVec x0_linear, x0_other;
	range_of_x0.decompose(x0_linear, x0_other);

	iMatrix Phi_L_i(rangeDim, rangeDim);

	x0_linear.linearCoefficients(Phi_L_i);

	iMatrix local_trans_linear = Phi_L_i;

	Phi_L_i.right_scale_assign(scalars);

	// compute the remainder part under the linear transformation
	iMatrix J_i(rangeDim, 1);

	for(int i=0; i<Phi_L.size(); ++i)
	{
		Phi_L[i] = Phi_L_i * Phi_L[i];
	}

	Phi_L.push_back(Phi_L_i);

	for(int i=1; i<Phi_L.size(); ++i)
	{
		J_i += Phi_L[i] * J[i-1];
	}

	iMatrix J_ip1(rangeDim, 1);

	// compute the local initial set
	if(J.size() > 0)
	{
		// compute the polynomial part under the linear transformation
		std::vector<Polynomial> initial_linear;
		Phi_L[0].linearTrans(initial_linear, initial_set_poly);

		// compute the other part
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		x0_other.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

		result.tmv.Remainder(J_ip1);

		iMatrix x0_rem(rangeDim, 1);
		range_of_x0.Remainder(x0_rem);
		J_ip1 += x0_rem;

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].expansion += initial_linear[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{
			result.tmv.tms[i].remainder = J_ip1[i][0] + J_i[i][0];
		}
	}
	else
	{
		std::vector<Interval> tmvPolyRange;
		tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
		range_of_x0.insert_ctrunc_normal(result.tmv, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);
		result.tmv.Remainder(J_ip1);
	}

	J.push_back(J_ip1);

	std::vector<Interval> boundOfx0;
	result.tmv.intEvalNormal(boundOfx0, step_exp_table);

	// Compute the scaling matrix S
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
			scalars[i] = intZero;
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
			scalars[i] = intRecSup;
		}
	}

	// apply the scaling matrix S
	result.tmv.scale_assign(invS);

	TaylorModelVec x, x0;

	TaylorModelVec c0_plus_Ar0(S);
	c0_plus_Ar0.add_assign(c0);

	x0 = c0_plus_Ar0;
	x = x0;

	for(int i=1; i<=order; ++i)
	{
		x.Picard_trunc_no_remainder_assign(x0, ode_centered, rangeDim+1, i, cutoff_threshold, constant, constant_part);
	}

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;

	std::list<Interval> intermediate_ranges;
	x.Picard_ctrunc_normal(tmvTemp, x0, ode, step_exp_table, order, rangeDim+1, cutoff_threshold, constant, constant_part, intermediate_ranges);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	// add the uncertainties and the cutoff intervals onto the result
	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		return 0;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_ctrunc_normal_remainder(newRemainders, x0, ode, step_exp_table[1], order, constant, intermediate_ranges);

		// add the uncertainties and the cutoff intervals onto the result
		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{

			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return 1;
}
/*
int Flowpipe::advance_regression_picard(Flowpipe & result, const DynamicSys & system, std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table, vector<vector<int> > & table_of_monomials,
		const int order, const std::vector<Interval> & estimation, const Interval & cutoff_threshold, std::vector<bool> & constant, std::vector<Interval> & constant_part) const
{
	int rangeDim = tmvPre.tms.size();
	Interval intZero, intOne(1), intUnit(-1,1);
	result.clear();

	// evaluate the the initial set x0
	TaylorModelVec range_of_x0;
	tmvPre.evaluate_t(range_of_x0, step_end_exp_table);

	// the center point of x0's polynomial part
	std::vector<Interval> intVecCenter;
	range_of_x0.constant(intVecCenter);

	// the center point of the remainder of x0
	for(int i=0; i<rangeDim; ++i)
	{
		Interval M;
		range_of_x0.tms[i].remainder.remove_midpoint(M);
		intVecCenter[i] += M;
	}

	// compute the local initial set

	TaylorModelVec localInitialSet;
	std::vector<Interval> tmvPolyRange;
	tmv.polyRangeNormal(tmvPolyRange, step_end_exp_table);
	range_of_x0.insert_ctrunc_normal(localInitialSet, tmv, tmvPolyRange, step_end_exp_table, domain.size(), order, cutoff_threshold);

	TaylorModelVec tmv_regression;
	std::vector<double> error;
	PolyRegssionSim(tmv_regression, table_of_monomials, localInitialSet, rangeDim, system, step_exp_table[1].sup(), 5, error, order);

	// compute the preconditioning Taylor model x0 = c0 + A*r0
	localInitialSet.rmConstant();
	result.tmv = localInitialSet;

	TaylorModelVec c0(intVecCenter, rangeDim+1);

	std::vector<Interval> boundOfx0;
	result.tmv.intEvalNormal(boundOfx0, step_end_exp_table);


	// Compute the scaling matrix S.
	std::vector<Interval> S, invS;

	for(int i=0; i<rangeDim; ++i)
	{
		Interval intSup;
		boundOfx0[i].mag(intSup);

		if(intSup.subseteq(intZero))
		{
			S.push_back(intZero);
			invS.push_back(intOne);
		}
		else
		{
			S.push_back(intSup);
			Interval intRecSup;
			intSup.rec(intRecSup);
			invS.push_back(intRecSup);
			boundOfx0[i] = intUnit;
		}
	}

	result.tmv.scale_assign(invS);
	result.tmv.cutoff_normal(step_end_exp_table, cutoff_threshold);


	TaylorModelVec x0(S);
	x0.add_assign(c0);

	TaylorModelVec x;
	tmv_regression.insert_no_remainder_no_cutoff(x, x0, rangeDim+1, order);

	x.cutoff(cutoff_threshold);

	bool bfound = true;

	for(int i=0; i<rangeDim; ++i)
	{
		x.tms[i].remainder = estimation[i];
	}

	TaylorModelVec tmvTemp;

	std::list<Interval> intermediate_ranges;
	x.Picard_ctrunc_normal(tmvTemp, x0, *system.Pnums, step_exp_table, order, rangeDim+1, cutoff_threshold, constant, constant_part, intermediate_ranges);

	// compute the interval evaluation of the polynomial difference, this part is not able to be reduced by Picard iteration
	std::vector<Interval> intDifferences;
	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp;
		polyTemp = tmvTemp.tms[i].expansion - x.tms[i].expansion;

		Interval intTemp;
		polyTemp.intEvalNormal(intTemp, step_exp_table);

		intDifferences.push_back(intTemp);
	}

	// add the uncertainties and the cutoff intervals onto the result
	for(int i=0; i<rangeDim; ++i)
	{
		tmvTemp.tms[i].remainder += intDifferences[i];
	}

	for(int i=0; i<rangeDim; ++i)
	{
		if( ! tmvTemp.tms[i].remainder.subseteq(x.tms[i].remainder) )
		{
			bfound = false;
			break;
		}
	}

	if(!bfound)
	{
		result.tmvPre = x;
		result.domain = domain;
		result.domain[0] = step_exp_table[1];
		return 1;
	}
	else
	{
		for(int i=0; i<rangeDim; ++i)
		{
			x.tms[i].remainder = tmvTemp.tms[i].remainder;
		}
	}

	bool bfinished = false;
	for(int rSteps = 0; !bfinished && (rSteps <= MAX_REFINEMENT_STEPS); ++rSteps)
	{
		bfinished = true;

		std::vector<Interval> newRemainders;
		x.Picard_ctrunc_normal_remainder(newRemainders, x0, *system.Pnums, step_exp_table[1], order, constant, intermediate_ranges);

		// add the uncertainties and the cutoff intervals onto the result
		for(int i=0; i<rangeDim; ++i)
		{
			newRemainders[i] += intDifferences[i];
		}

		for(int i=0; i<rangeDim; ++i)
		{

			if(newRemainders[i].subseteq(x.tms[i].remainder))
			{
				if(x.tms[i].remainder.widthRatio(newRemainders[i]) <= STOP_RATIO)
				{
					bfinished = false;
				}

				x.tms[i].remainder = newRemainders[i];
			}
			else
			{
				bfinished = true;
				break;
			}
		}
	}

	result.tmvPre = x;
	result.domain = domain;
	result.domain[0] = step_exp_table[1];

	return 1;
}
*/


void Flowpipe::center(std::vector<double> & center_point) const
{
	std::vector<Interval> constant;
	tmvPre.constant(constant);

	for(int i=0; i<constant.size(); ++i)
	{
		center_point.push_back(constant[i].midpoint());
	}
}

double Flowpipe::max(const std::vector<double> & l) const
{
	std::vector<flowstar::Interval> tmvRange;
	tmv.intEval(tmvRange, domain);

	tmvRange.insert(tmvRange.begin(), domain[0]);

	std::vector<flowstar::Interval> fpRange;
	tmvPre.intEval(fpRange, tmvRange);

	double maximum = 1e100;

	for(int i=0; i<l.size(); ++i)
	{
		double v;

		if(l[i] == 0)
		{
			continue;
		}
		else if(l[i] > 0)
		{
			flowstar::Interval I;
			fpRange[i].sup(I);
			I.div_assign(l[i]);
			v = I.sup();
		}
		else
		{
			flowstar::Interval I;
			fpRange[i].inf(I);
			I.div_assign(l[i]);
			v = I.inf();
		}

		if(v < maximum)
		{
			maximum = v;
		}
	}

	return maximum;
}






// class LinearFlowpipe

LinearFlowpipe::LinearFlowpipe()
{
}

LinearFlowpipe::LinearFlowpipe(const LinearFlowpipe & flowpipe)
{
	init_Phi		= flowpipe.init_Phi;
	init_Psi		= flowpipe.init_Psi;
	init_Omega		= flowpipe.init_Omega;

	trans_Phi		= flowpipe.trans_Phi;
	trans_Psi		= flowpipe.trans_Psi;
	trans_Omega		= flowpipe.trans_Omega;

	tv_remainder	= flowpipe.tv_remainder;
}

LinearFlowpipe::~LinearFlowpipe()
{
}

int LinearFlowpipe::safetyChecking(const Flowpipe & X0, const std::vector<Interval> & polyRangeX0, const std::vector<Interval> & step_exp_table, const bool bVarying, const bool bAuto,
		const std::vector<iMatrix> & im_precond_trans_Phi, const std::vector<iMatrix> & im_precond_trans_Psi, const std::vector<iMatrix> & im_precond_trans_Omega,
		const std::vector<iMatrix> & constraints, const iMatrix & TIPar_range, const iMatrix & rangeX0, const std::vector<PolynomialConstraint> & unsafeSet,
		const std::vector<Interval> & checking_domain, const std::vector<Interval> & ti_domain, const std::vector<Interval> & extended_domain,
		const int order, const Interval & cutoff_threshold)
{
	if(extended_domain.size() == 0)
	{
		int rangeDim = init_Phi.rows();
		int numTIPar = init_Omega.rows();
		bool bTV = !tv_remainder.isEmpty();

		int result = UNKNOWN;
		bool bContained = true;

		std::vector<HornerForm> obj_hfs;
		std::vector<Interval> obj_rems;
		std::vector<HornerForm> TIParts;
		std::vector<Interval> TVParts;

		for(int i=0; i<unsafeSet.size(); ++i)
		{
			// do a coarse check, if the safety is not proved, we go for a more accurate check
			Interval intTemp;
			iMatrix linear_coefficients(1, rangeDim);

			if(constraints.size() == 0)
			{
				unsafeSet[i].p.linearCoefficients(linear_coefficients, 0);
				intEval(intTemp, linear_coefficients, bVarying, bAuto, im_precond_trans_Phi[i], im_precond_trans_Psi[i], im_precond_trans_Omega[i], step_exp_table, TIPar_range, rangeX0);
			}
			else
			{
				intEval(intTemp, constraints[i], bVarying, bAuto, im_precond_trans_Phi[i], im_precond_trans_Psi[i], im_precond_trans_Omega[i], step_exp_table, TIPar_range, rangeX0);
			}

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

			if(result == UNKNOWN)
			{
				HornerForm obj_hf;
				Interval obj_rem;
				HornerForm TIPart_hf;
				Interval int_TVPart;

				if(constraints.size() == 0)
				{
					tmEval(obj_hf, obj_rem, TIPart_hf, int_TVPart, bAuto, linear_coefficients, X0, checking_domain, ti_domain, polyRangeX0, cutoff_threshold);
				}
				else
				{
					tmEval(obj_hf, obj_rem, TIPart_hf, int_TVPart, bAuto, constraints[i], X0, checking_domain, ti_domain, polyRangeX0, cutoff_threshold);
				}

				obj_hfs.push_back(obj_hf);
				obj_rems.push_back(obj_rem);

				obj_hf.intEval(intTemp, checking_domain);
				intTemp += obj_rem;

				if(numTIPar > 0)
				{
					TIParts.push_back(TIPart_hf);

					Interval intTemp2;
					TIPart_hf.intEval(intTemp2, ti_domain);
					intTemp += intTemp2;
				}

				if(bTV)
				{
					TVParts.push_back(int_TVPart);
					intTemp += int_TVPart;
				}


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
		}

		if(result == UNKNOWN)
		{
			if(bContained)
			{
				return UNSAFE;
			}
			else
			{
				// do a simple branch & bound for safety checking
				std::vector<Interval> refined_checking_domain = checking_domain;
				std::vector<Interval> refined_ti_domain = ti_domain;

				std::list<Interval> subdivisions;
				Interval intLeft, intRight;
				checking_domain[0].split(intLeft, intRight);

				result = SAFE;

				if(intLeft.width() > REFINEMENT_PREC)
				{
					subdivisions.push_back(intLeft);
				}

				if(intRight.width() > REFINEMENT_PREC)
				{
					subdivisions.push_back(intRight);
				}

				for(; subdivisions.size() > 0; )
				{
					Interval subdivision = subdivisions.front();
					subdivisions.pop_front();

					int result_iter = UNKNOWN;
					bool bContained_iter = true;

					refined_checking_domain[0] = subdivision;

					for(int i=0; i<unsafeSet.size(); ++i)
					{
						Interval intTemp;

						obj_hfs[i].intEval(intTemp, refined_checking_domain);
						intTemp += obj_rems[i];

						if(numTIPar > 0)
						{
							Interval intTemp2;

							refined_ti_domain[0] = subdivision;

							TIParts[i].intEval(intTemp2, refined_ti_domain);
							intTemp += intTemp2;
						}

						if(bTV)
						{
							intTemp += TVParts[i];
						}

						if(intTemp > unsafeSet[i].B)
						{
							// no intersection with the unsafe set
							result_iter = SAFE;
							break;
						}
						else
						{
							if(!intTemp.smallereq(unsafeSet[i].B) && bContained_iter)
							{
								bContained_iter = false;
							}
						}
					}

					if(result_iter == UNKNOWN)
					{
						if(bContained_iter)
						{
							return UNSAFE;
						}
						else
						{
							// split the domain
							Interval I1, I2;
							subdivision.split(I1, I2);

							if(I1.width() <= REFINEMENT_PREC)
							{
								if(result == SAFE)
									result = UNKNOWN;
							}
							else
							{
								subdivisions.push_back(I1);
							}

							if(I2.width() <= REFINEMENT_PREC)
							{
								if(result == SAFE)
									result = UNKNOWN;
							}
							else
							{
								subdivisions.push_back(I2);
							}
						}
					}
				}

				return result;
			}
		}
		else
		{
			return SAFE;
		}
	}
	else
	{
		int numTIPar = init_Omega.rows();

		int result = UNKNOWN;
		bool bContained = true;

		// do a coarse check, if the safety is not proved, we go for a more accurate check
		std::vector<Interval> fp_range;
		fp_range.push_back(step_exp_table[1]);

		intEval(fp_range, bVarying, bAuto, im_precond_trans_Phi[0], im_precond_trans_Psi[0], im_precond_trans_Omega[0], step_exp_table, TIPar_range, rangeX0);

		for(int i=0; i<unsafeSet.size(); ++i)
		{
			Interval intTemp;
			unsafeSet[i].hf.intEval(intTemp, fp_range);

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

		if(result == UNKNOWN)
		{
			TaylorModelVec tmvFlowpipe;
			toTaylorModel(tmvFlowpipe, bAuto, X0, checking_domain, numTIPar, polyRangeX0, cutoff_threshold);

			std::vector<Interval> fpPolyRange;
			tmvFlowpipe.polyRange(fpPolyRange, extended_domain);

			std::vector<HornerForm> obj_hfs;
			std::vector<Interval> obj_rems;

			for(int i=0; i<unsafeSet.size(); ++i)
			{
				TaylorModel tmTemp;
				unsafeSet[i].hf.insert_ctrunc(tmTemp, tmvFlowpipe, fpPolyRange, extended_domain, order, cutoff_threshold);

				Interval intTemp;

				HornerForm obj_hf;
				tmTemp.expansion.toHornerForm(obj_hf);

				obj_hfs.push_back(obj_hf);
				obj_rems.push_back(tmTemp.remainder);

				obj_hf.intEval(intTemp, extended_domain);
				intTemp += tmTemp.remainder;

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

			if(result == UNKNOWN)
			{
				if(bContained)
				{
					return UNSAFE;
				}
				else
				{
					// do a simple branch & bound for safety checking
					std::vector<Interval> refined_checking_domain = extended_domain;

					std::list<Interval> subdivisions;
					Interval intLeft, intRight;
					extended_domain[0].split(intLeft, intRight);

					if(intLeft.width() > REFINEMENT_PREC)
					{
						subdivisions.push_back(intLeft);
					}

					if(intRight.width() > REFINEMENT_PREC)
					{
						subdivisions.push_back(intRight);
					}

					for(; subdivisions.size() > 0; )
					{
						Interval subdivision = subdivisions.front();
						subdivisions.pop_front();

						int result_iter = UNKNOWN;
						bool bContained_iter = true;

						refined_checking_domain[0] = subdivision;

						for(int i=0; i<unsafeSet.size(); ++i)
						{
							Interval intTemp;
							obj_hfs[i].intEval(intTemp, refined_checking_domain);

							intTemp += obj_rems[i];

							if(intTemp > unsafeSet[i].B)
							{
								// no intersection with the unsafe set
								result_iter = SAFE;
								break;
							}
							else
							{
								if(!intTemp.smallereq(unsafeSet[i].B) && bContained_iter)
								{
									bContained_iter = false;
								}
							}
						}

						if(result_iter == UNKNOWN)
						{
							if(bContained_iter)
							{
								return UNSAFE;
							}
							else
							{
								Interval I1, I2;
								subdivision.split(I1, I2);

								if(I1.width() <= REFINEMENT_PREC)
								{
									if(result == SAFE)
										result = UNKNOWN;
								}
								else
								{
									subdivisions.push_back(I1);
								}

								if(I2.width() <= REFINEMENT_PREC)
								{
									if(result == SAFE)
										result = UNKNOWN;
								}
								else
								{
									subdivisions.push_back(I2);
								}
							}
						}
					}

					return result;
				}
			}
			else
			{
				return SAFE;
			}
		}

		return SAFE;
	}
}
/*
void LinearFlowpipe::intEval(iMatrix & range, const std::vector<Interval> & t_exp_table, const iMatrix & X0, const iMatrix & ti_range)
{
	upMatrix Phi = trans_Phi * init_Phi;

	iMatrix range_Phi;
	Phi.intEval(range_Phi, t_exp_table);

	upMatrix Psi = trans_Phi * init_Psi + trans_Psi;
	iMatrix range_Psi;
	Psi.intEval(range_Psi, t_exp_table);

	range = range_Phi * X0 + range_Psi;

	if(ti_range.rows() > 0)
	{
		upMatrix Omega = trans_Phi * init_Omega + trans_Omega;
		iMatrix range_Omega;
		Omega.intEval(range_Omega, t_exp_table);
		range += range_Omega * ti_range;
	}

	if(!tv_remainder.isEmpty())
	{
		iMatrix im_tv_remainder;
		tv_remainder.intEval(im_tv_remainder);
		range += im_tv_remainder;
	}
}

void LinearFlowpipe::intEvalNormal(Interval & result, const iMatrix & constraint, const Flowpipe & X0, const int numTIPar,
		const std::vector<Interval> & step_exp_table, const std::vector<Interval> & polyRangeX0, const Interval & cutoff_threshold)
{
	int rangeDim = X0.tmvPre.tms.size();
	int domainDim = X0.domain.size();

	upMatrix precond_trans_Phi = constraint * trans_Phi;
	upMatrix Phi = precond_trans_Phi * init_Phi;
	upMatrix Psi = precond_trans_Phi * init_Psi + constraint * trans_Psi;

	Polynomial objfunc;

	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp1(Phi[0][i], domainDim);
		polyTemp1.mul_assign(i+1, 1);
		objfunc += polyTemp1;
	}

	TaylorModel tmTemp;
	objfunc.insert_normal(tmTemp, X0.tmvPre, polyRangeX0, step_exp_table, domainDim, cutoff_threshold);

	Polynomial polyTemp2(Psi[0][0], domainDim);
	tmTemp.expansion += polyTemp2;

	tmTemp.intEvalNormal(result, step_exp_table);


	if(numTIPar > 0)
	{
		upMatrix Omega = precond_trans_Phi * init_Omega + constraint * trans_Omega;

		Polynomial objfunc_ti;
		for(int i=0; i<numTIPar; ++i)
		{
			Polynomial polyTemp(Phi[0][i], numTIPar+1);
			polyTemp.mul_assign(i+1, 1);
			objfunc_ti += polyTemp;
		}

		Interval range;
		objfunc_ti.intEvalNormal(range, step_exp_table);
		result += range;
	}


	if(!tv_remainder.isEmpty())
	{
		Zonotope zonoTemp;
		tv_remainder.linearTrans(zonoTemp, constraint);

		iMatrix im_tv_remainder;
		zonoTemp.intEval(im_tv_remainder);
		result += im_tv_remainder[0][0];
	}
}
*/


void LinearFlowpipe::intEval(Interval & result, const iMatrix & constraint, const bool bVarying, const bool bAuto, const iMatrix & im_precond_trans_Phi, const iMatrix & im_precond_trans_Psi, const iMatrix & im_precond_trans_Omega, const std::vector<Interval> & step_exp_table, const iMatrix & TIPar_range, const iMatrix & rangeX0)
{
	iMatrix im_range;

	if(bVarying)
	{
		iMatrix im_trans_Phi_tv;
		trans_Phi.intEval(im_trans_Phi_tv, step_exp_table);
		im_trans_Phi_tv = constraint * im_trans_Phi_tv;

		im_range = im_trans_Phi_tv * init_Phi * rangeX0;

		if(!bAuto)
		{
			iMatrix im_trans_Psi_tv;
			trans_Psi.intEval(im_trans_Psi_tv, step_exp_table);
			im_range += im_trans_Phi_tv * init_Psi + constraint * im_trans_Psi_tv;
		}

		int numTIPar = TIPar_range.rows();
		if(numTIPar > 0)
		{
			iMatrix im_trans_Omega_tv;
			trans_Omega.intEval(im_trans_Omega_tv, step_exp_table);
			im_range += (im_trans_Phi_tv * init_Omega + constraint * im_trans_Omega_tv) * TIPar_range;
		}
	}
	else
	{
		if(bAuto)
		{
			im_range = im_precond_trans_Phi * init_Phi * rangeX0;
		}
		else
		{
			im_range = im_precond_trans_Phi * init_Phi * rangeX0 + im_precond_trans_Phi * init_Psi + im_precond_trans_Psi;
		}

		int numTIPar = TIPar_range.rows();
		if(numTIPar > 0)
		{
			im_range += (im_precond_trans_Phi * init_Omega + im_precond_trans_Omega) * TIPar_range;
		}
	}

	if(!tv_remainder.isEmpty())
	{
		Zonotope zonoTemp;
		tv_remainder.linearTrans(zonoTemp, constraint);

		iMatrix im_tv_remainder(1,1);
		zonoTemp.intEval(im_tv_remainder);
		im_range[0][0] += im_tv_remainder[0][0];
	}

	result = im_range[0][0];
}

void LinearFlowpipe::intEval(std::vector<Interval> & result, const bool bVarying, const bool bAuto, const iMatrix & im_trans_Phi, const iMatrix & im_trans_Psi, const iMatrix & im_trans_Omega, const std::vector<Interval> & step_exp_table, const iMatrix & TIPar_range, const iMatrix & rangeX0)
{
	iMatrix im_range;

	if(bVarying)
	{
		iMatrix im_trans_Phi_tv;
		trans_Phi.intEval(im_trans_Phi_tv, step_exp_table);

		im_range = im_trans_Phi_tv * init_Phi * rangeX0;

		if(!bAuto)
		{
			iMatrix im_trans_Psi_tv;
			trans_Psi.intEval(im_trans_Psi_tv, step_exp_table);
			im_range += im_trans_Phi_tv * init_Psi + im_trans_Psi_tv;
		}

		int numTIPar = TIPar_range.rows();
		if(numTIPar > 0)
		{
			iMatrix im_trans_Omega_tv;
			trans_Omega.intEval(im_trans_Omega_tv, step_exp_table);
			im_range += (im_trans_Phi_tv * init_Omega + im_trans_Omega_tv) * TIPar_range;
		}
	}
	else
	{
		if(bAuto)
		{
			im_range = im_trans_Phi * init_Phi * rangeX0;
		}
		else
		{
			im_range = im_trans_Phi * init_Phi * rangeX0 + im_trans_Phi * init_Psi + im_trans_Psi;
		}

		int numTIPar = TIPar_range.rows();
		if(numTIPar > 0)
		{
			im_range += (im_trans_Phi * init_Omega + im_trans_Omega) * TIPar_range;
		}
	}

	if(!tv_remainder.isEmpty())
	{
		iMatrix im_rem(rangeX0.rows(), 1);
		tv_remainder.intEval(im_rem);
		im_range += im_rem;
	}

	for(int i=0; i<im_range.rows(); ++i)
	{
		result.push_back(im_range[i][0]);
	}
}

/*
void LinearFlowpipe::intEval(Interval & result, const iMatrix & constraint, const Flowpipe & X0, const std::vector<Interval> & checking_domain, const std::vector<Interval> & ti_domain, const std::vector<Interval> & polyRangeX0, const Interval & cutoff_threshold)
{
	int rangeDim = X0.tmvPre.tms.size();
	int domainDim = checking_domain.size();
	int numTIPar = ti_domain.size() - 1;

	upMatrix precond_trans_Phi = constraint * trans_Phi;
	upMatrix Phi = precond_trans_Phi * init_Phi;
	upMatrix Psi = precond_trans_Phi * init_Psi + constraint * trans_Psi;

	Polynomial objfunc;

	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp1(Phi[0][i], domainDim);
		polyTemp1.mul_assign(i+1, 1);
		objfunc += polyTemp1;
	}

	TaylorModel tmTemp;
	objfunc.insert(tmTemp, X0.tmvPre, polyRangeX0, checking_domain, cutoff_threshold);

	Polynomial polyTemp2(Psi[0][0], domainDim);
	tmTemp.expansion += polyTemp2;

	tmTemp.intEval(result, checking_domain);


	if(numTIPar > 0)
	{
		upMatrix Omega = precond_trans_Phi * init_Omega + constraint * trans_Omega;

		Polynomial objfunc_ti;
		for(int i=0; i<numTIPar; ++i)
		{
			Polynomial polyTemp(Omega[0][i], numTIPar+1);
			polyTemp.mul_assign(i+1, 1);
			objfunc_ti += polyTemp;
		}

		Interval range;
		objfunc_ti.intEval(range, ti_domain);
		result += range;
	}


	if(!tv_remainder.isEmpty())
	{
		Zonotope zonoTemp;
		tv_remainder.linearTrans(zonoTemp, constraint);

		iMatrix im_tv_remainder(1,1);
		zonoTemp.intEval(im_tv_remainder);
		result += im_tv_remainder[0][0];
	}
}
*/

void LinearFlowpipe::tmEval(HornerForm & obj_hf, Interval & obj_rem, HornerForm & TIPart_hf, Interval & int_TVPart, const bool bAuto, const iMatrix & constraint, const Flowpipe & X0, const std::vector<Interval> & checking_domain, const std::vector<Interval> & ti_domain, const std::vector<Interval> & polyRangeX0, const Interval & cutoff_threshold)
{
	int rangeDim = X0.tmvPre.tms.size();
	int domainDim = checking_domain.size();
	int numTIPar = ti_domain.size() - 1;

	upMatrix precond_trans_Phi = constraint * trans_Phi;
	upMatrix Phi = precond_trans_Phi * init_Phi;

	Polynomial objfunc;

	for(int i=0; i<rangeDim; ++i)
	{
		Polynomial polyTemp1(Phi[0][i], domainDim);
		polyTemp1.mul_assign(i+1, 1);
		objfunc += polyTemp1;
	}

	TaylorModel tmTemp;
	objfunc.insert(tmTemp, X0.tmvPre, polyRangeX0, checking_domain, cutoff_threshold);

	if(!bAuto)
	{
		upMatrix Psi;
		Psi = precond_trans_Phi * init_Psi + constraint * trans_Psi;
		Polynomial polyTemp2(Psi[0][0], domainDim);
		tmTemp.expansion += polyTemp2;
	}

	tmTemp.expansion.toHornerForm(obj_hf);
	obj_rem = tmTemp.remainder;


	if(numTIPar > 0)
	{
		upMatrix Omega = precond_trans_Phi * init_Omega + constraint * trans_Omega;
		Polynomial polyTIPart;

		for(int i=0; i<numTIPar; ++i)
		{
			Polynomial polyTemp(Omega[0][i], numTIPar+1);
			polyTemp.mul_assign(i+1, 1);
			polyTIPart += polyTemp;
		}

		polyTIPart.toHornerForm(TIPart_hf);
	}


	if(!tv_remainder.isEmpty())
	{
		Zonotope zonoTemp;
		tv_remainder.linearTrans(zonoTemp, constraint);

		iMatrix im_temp(1,1);
		zonoTemp.intEval(im_temp);
		int_TVPart = im_temp[0][0];
	}
}

void LinearFlowpipe::toTaylorModel(TaylorModelVec & flowpipe, const bool bAuto, const Flowpipe & X0, const std::vector<Interval> & checking_domain, const int numTIPar, const std::vector<Interval> & polyRangeX0, const Interval & cutoff_threshold)
{
	int rangeDim = X0.tmvPre.tms.size();
	int domainDim = checking_domain.size();

	flowpipe.clear();

	upMatrix Phi = trans_Phi * init_Phi;

	TaylorModelVec tmvTemp1;

	for(int i=0; i<rangeDim; ++i)
	{
		TaylorModel tmTemp;

		for(int j=0; j<rangeDim; ++j)
		{
			Polynomial polyTemp(Phi[i][j], domainDim);
			polyTemp.mul_assign(j+1, 1);

			tmTemp.expansion += polyTemp;
		}

		tmvTemp1.tms.push_back(tmTemp);
	}

	tmvTemp1.insert(flowpipe, X0.tmvPre, polyRangeX0, checking_domain, cutoff_threshold);


	if(!bAuto)
	{
		upMatrix Psi = trans_Phi * init_Psi + trans_Psi;

		for(int i=0; i<rangeDim; ++i)
		{
			Polynomial polyTemp(Psi[i][0], domainDim);
			flowpipe.tms[i].expansion += polyTemp;
		}
	}

	if(numTIPar > 0)
	{
		upMatrix Omega = trans_Phi * init_Omega + trans_Omega;

		int newDomainDim = domainDim + numTIPar;
		flowpipe.extend(newDomainDim);

		for(int i=0; i<rangeDim; ++i)
		{
			for(int j=0; j<numTIPar; ++j)
			{
				Polynomial polyTemp(Omega[i][j], newDomainDim);
				polyTemp.mul_assign(domainDim + j, 1);

				flowpipe.tms[i].expansion += polyTemp;
			}
		}
	}


	if(!tv_remainder.isEmpty())
	{
		iMatrix im_tv_remainder(rangeDim, 1);
		tv_remainder.intEval(im_tv_remainder);

		for(int i=0; i<rangeDim; ++i)
		{
			flowpipe.tms[i].remainder += im_tv_remainder[i][0];
		}
	}
}

void LinearFlowpipe::toTaylorModel(TaylorModelVec & flowpipe, const bool bAuto, const std::vector<int> & outputAxes, const Flowpipe & X0, const std::vector<Interval> & checking_domain, const int numTIPar, const std::vector<Interval> & polyRangeX0, const Interval & cutoff_threshold)
{
	int rangeDim = X0.tmvPre.tms.size();
	int domainDim = X0.domain.size();

	flowpipe.clear();

	upMatrix proj_Phi(outputAxes.size(), rangeDim);

	for(int i=0; i<outputAxes.size(); ++i)
	{
		for(int j=0; j<rangeDim; ++j)
		{
			proj_Phi[i][j] = trans_Phi[outputAxes[i]][j];
		}
	}


	upMatrix Phi = proj_Phi * init_Phi;

	TaylorModelVec tmvTemp1;

	for(int i=0; i<outputAxes.size(); ++i)
	{
		TaylorModel tmTemp;

		for(int j=0; j<rangeDim; ++j)
		{
			Polynomial polyTemp(Phi[i][j], domainDim);
			polyTemp.mul_assign(j+1, 1);

			tmTemp.expansion += polyTemp;
		}

		tmvTemp1.tms.push_back(tmTemp);
	}

	tmvTemp1.insert(flowpipe, X0.tmvPre, polyRangeX0, checking_domain, cutoff_threshold);

	if(!bAuto)
	{
		upMatrix proj_Psi(outputAxes.size(), 1);
		for(int i=0; i<outputAxes.size(); ++i)
		{
			proj_Psi[i][0] = trans_Psi[outputAxes[i]][0];
		}

		upMatrix Psi = proj_Phi * init_Psi + proj_Psi;

		for(int i=0; i<outputAxes.size(); ++i)
		{
			Polynomial polyTemp(Psi[i][0], domainDim);
			flowpipe.tms[i].expansion += polyTemp;
		}
	}

	if(numTIPar > 0)
	{
		upMatrix proj_Omega(outputAxes.size(), rangeDim);
		for(int i=0; i<outputAxes.size(); ++i)
		{
			for(int j=0; j<rangeDim; ++j)
			{
				proj_Omega[i][j] = trans_Omega[outputAxes[i]][j];
			}
		}

		upMatrix Omega = proj_Phi * init_Omega + proj_Omega;

		int newDomainDim = domainDim + numTIPar;
		flowpipe.extend(newDomainDim);

		for(int i=0; i<outputAxes.size(); ++i)
		{
			for(int j=0; j<numTIPar; ++j)
			{
				Polynomial polyTemp(Omega[i][j], newDomainDim);
				polyTemp.mul_assign(domainDim + j, 1);

				flowpipe.tms[i].expansion += polyTemp;
			}
		}
	}


	if(!tv_remainder.isEmpty())
	{
		iMatrix im_tv_remainder(rangeDim, 1);
		tv_remainder.intEval(im_tv_remainder);

		for(int i=0; i<outputAxes.size(); ++i)
		{
			flowpipe.tms[i].remainder += im_tv_remainder[outputAxes[i]][0];
		}
	}
}

void LinearFlowpipe::toTaylorModel(TaylorModelVec & flowpipe, const bool bAuto)
{
	int rangeDim = init_Phi.cols();
	int numTIPar = init_Omega.cols();
	int domainDim = rangeDim + numTIPar + 1;

	flowpipe.clear();

	upMatrix Phi = trans_Phi * init_Phi;

	for(int i=0; i<rangeDim; ++i)
	{
		TaylorModel tmTemp;

		for(int j=0; j<rangeDim; ++j)
		{
			Polynomial polyTemp(Phi[i][j], domainDim);
			polyTemp.mul_assign(j+1, 1);

			tmTemp.expansion += polyTemp;
		}

		flowpipe.tms.push_back(tmTemp);
	}

	if(!bAuto)
	{
		upMatrix Psi = trans_Phi * init_Psi + trans_Psi;

		for(int i=0; i<rangeDim; ++i)
		{
			Polynomial polyTemp(Psi[i][0], domainDim);
			flowpipe.tms[i].expansion += polyTemp;
		}
	}

	if(numTIPar > 0)
	{
		upMatrix Omega = trans_Phi * init_Omega + trans_Omega;

		int newDomainDim = domainDim + numTIPar;
		flowpipe.extend(newDomainDim);

		for(int i=0; i<rangeDim; ++i)
		{
			for(int j=0; j<numTIPar; ++j)
			{
				Polynomial polyTemp(Omega[i][j], domainDim);
				polyTemp.mul_assign(rangeDim + j + 1, 1);

				flowpipe.tms[i].expansion += polyTemp;
			}
		}
	}

	if(!tv_remainder.isEmpty())
	{
		iMatrix im_tv_remainder(rangeDim, 1);
		tv_remainder.intEval(im_tv_remainder);

		for(int i=0; i<rangeDim; ++i)
		{
			flowpipe.tms[i].remainder += im_tv_remainder[i][0];
		}
	}
}

/*
void LinearFlowpipe::toTaylorModel(TaylorModelVec & flowpipe, const std::vector<int> & outputAxes, const Flowpipe & X0, const int numTIPar, const std::vector<Interval> & step_exp_table, const std::vector<Interval> & polyRangeX0, const Interval & cutoff_threshold)
{
	int rangeDim = X0.tmvPre.tms.size();
	int domainDim = X0.domain.size();

	flowpipe.clear();

	upMatrix proj_Phi(outputAxes.size(), rangeDim);

	for(int i=0; i<outputAxes.size(); ++i)
	{
		for(int j=0; j<rangeDim; ++j)
		{
			proj_Phi[i][j] = trans_Phi[outputAxes[i]][j];
		}
	}


	upMatrix Phi = proj_Phi * init_Phi;

	TaylorModelVec tmvTemp1;

	for(int i=0; i<outputAxes.size(); ++i)
	{
		TaylorModel tmTemp;

		for(int j=0; j<rangeDim; ++j)
		{
			Polynomial polyTemp(Phi[i][j], domainDim);
			polyTemp.mul_assign(j+1, 1);

			tmTemp.expansion += polyTemp;
		}

		tmvTemp1.tms.push_back(tmTemp);
	}

	tmvTemp1.insert_normal(flowpipe, X0.tmvPre, polyRangeX0, step_exp_table, domainDim, cutoff_threshold);

	upMatrix proj_Psi(outputAxes.size(), 1);
	for(int i=0; i<outputAxes.size(); ++i)
	{
		proj_Psi[i][0] = trans_Psi[outputAxes[i]][0];
	}

	upMatrix Psi = proj_Phi * init_Psi + proj_Psi;

	for(int i=0; i<outputAxes.size(); ++i)
	{
		Polynomial polyTemp(Psi[i][0], domainDim);
		flowpipe.tms[i].expansion += polyTemp;
	}


	if(numTIPar > 0)
	{
		upMatrix proj_Omega(outputAxes.size(), rangeDim);
		for(int i=0; i<outputAxes.size(); ++i)
		{
			for(int j=0; j<rangeDim; ++j)
			{
				proj_Omega[i][j] = trans_Omega[outputAxes[i]][j];
			}
		}

		upMatrix Omega = proj_Phi * init_Omega + proj_Omega;

		int newDomainDim = domainDim + numTIPar;
		flowpipe.extend(newDomainDim);

		for(int i=0; i<outputAxes.size(); ++i)
		{
			for(int j=0; j<numTIPar; ++j)
			{
				Polynomial polyTemp(Omega[i][j], newDomainDim);
				polyTemp.mul_assign(domainDim + j, 1);

				flowpipe.tms[i].expansion += polyTemp;
			}
		}
	}


	if(!tv_remainder.isEmpty())
	{
		iMatrix im_tv_remainder;
		tv_remainder.intEval(im_tv_remainder);

		for(int i=0; i<outputAxes.size(); ++i)
		{
			flowpipe.tms[i].remainder += im_tv_remainder[outputAxes[i]][0];
		}
	}
}
*/

LinearFlowpipe & LinearFlowpipe::operator = (const LinearFlowpipe & flowpipe)
{
	if(this == &flowpipe)
		return *this;

	init_Phi		= flowpipe.init_Phi;
	init_Psi		= flowpipe.init_Psi;
	init_Omega		= flowpipe.init_Omega;

	trans_Phi		= flowpipe.trans_Phi;
	trans_Psi		= flowpipe.trans_Psi;
	trans_Omega		= flowpipe.trans_Omega;

	tv_remainder	= flowpipe.tv_remainder;

	return *this;
}






















// class ContinuousSystem

ContinuousSystem::ContinuousSystem()
{
}

ContinuousSystem::ContinuousSystem(const iMatrix & A_input, const iMatrix & B_input, const iMatrix & ti_input, const iMatrix & tv_input, const std::vector<Flowpipe> & initialSets_input)
{
	im_dyn_A = A_input;
	im_dyn_B = B_input;
	im_dyn_ti = ti_input;
	im_dyn_tv = tv_input;

	initialSets = initialSets_input;

	int numTVPar = tv_input.cols();
	iMatrix im_temp(numTVPar, 1);
	im_tv_range = im_temp;

	Interval intUnit(-1,1);

	for(int i=0; i<numTVPar; ++i)
	{
		im_tv_range[i][0] = intUnit;
	}

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

	if(B_input.isZero())
	{
		bAuto = true;
	}
	else
	{
		bAuto = false;
	}
}

ContinuousSystem::ContinuousSystem(const upMatrix & A_input, const upMatrix & B_input, const upMatrix & ti_input, const upMatrix & tv_input, const std::vector<Flowpipe> & initialSets_input)
{
	up_dyn_A = A_input;
	up_dyn_B = B_input;
	up_dyn_ti = ti_input;
	up_dyn_tv = tv_input;

	initialSets = initialSets_input;

	int numTVPar = tv_input.cols();
	iMatrix im_temp(numTVPar, 1);
	im_tv_range = im_temp;

	Interval intUnit(-1,1);

	for(int i=0; i<numTVPar; ++i)
	{
		im_tv_range[i][0] = intUnit;
	}

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

	if(B_input.isZero())
	{
		bAuto = true;
	}
	else
	{
		bAuto = false;
	}
}

ContinuousSystem::ContinuousSystem(const TaylorModelVec & ode_input, const std::vector<Flowpipe> & initialSets_input)
{
	int rangeDim = ode_input.tms.size();
	Interval intZero;

	initialSets = initialSets_input;

	tmvOde = ode_input;

	for(int i=0; i<rangeDim; ++i)
	{
		HornerForm hf;
		tmvOde.tms[i].expansion.toHornerForm(hf);
		hfOde.push_back(hf);

		if(tmvOde.tms[i].expansion.degree() == 0)
		{
			constant.push_back(true);
		}
		else
		{
			constant.push_back(false);
		}
	}

	tmvOde_centered = ode_input;
	tmvOde_centered.center_nc();

	for(int i=0; i<rangeDim; ++i)
	{
		HornerForm hf;
		tmvOde_centered.tms[i].expansion.toHornerForm(hf);
		hfOde_centered.push_back(hf);
	}

	bAuto = false;
}

ContinuousSystem::ContinuousSystem(const std::vector<std::string> & strOde_input, const std::vector<Flowpipe> & initialSets_input)
{
	strOde = strOde_input;

	std::string prefix(str_prefix_center);
	std::string suffix(str_suffix);

	for(int i=0 ;i<strOde.size(); ++i)
	{
		parseSetting.clear();
		parseSetting.strODE = prefix + strOde[i] + suffix;
		parseResult.bConstant = true;

		parseODE();

		strOde_centered.push_back(parseResult.strExpansion);
		constant.push_back(parseResult.bConstant);
		strOde_constant.push_back(parseResult.constant);
	}

	initialSets = initialSets_input;
	bAuto = false;
}

ContinuousSystem::ContinuousSystem(const ContinuousSystem & system)
{
	tmvOde				= system.tmvOde;
	hfOde				= system.hfOde;
	initialSets			= system.initialSets;
	strOde				= system.strOde;
	tmvOde_centered		= system.tmvOde_centered;
	hfOde_centered		= system.hfOde_centered;
	strOde_centered		= system.strOde_centered;
	bAuto				= system.bAuto;
	im_dyn_A			= system.im_dyn_A;
	im_dyn_B			= system.im_dyn_B;
	im_dyn_ti			= system.im_dyn_ti;
	im_dyn_tv			= system.im_dyn_tv;
	up_dyn_A			= system.up_dyn_A;
	up_dyn_B			= system.up_dyn_B;
	up_dyn_ti			= system.up_dyn_ti;
	up_dyn_tv			= system.up_dyn_tv;
	im_tv_range			= system.im_tv_range;
	connectivity		= system.connectivity;
	constant			= system.constant;
	strOde_constant		= system.strOde_constant;
}

ContinuousSystem::~ContinuousSystem()
{
	hfOde.clear();
	hfOde_centered.clear();
	strOde.clear();
	strOde_centered.clear();
	initialSets.clear();
	constant.clear();
}

int ContinuousSystem::reach_lti(std::list<LinearFlowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const bool bPrint, const Interval & cutoff_threshold, const int order_cutoff,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump)
{
	Real rStep(step);
	Interval intZero, intStep(0, step), intUnit(-1,1);

	std::vector<Interval> step_exp_table, step_end_exp_table;
	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1)+1);

	int rangeDim = im_dyn_A.rows(), numTIPar = im_dyn_ti.cols(), numTVPar = im_dyn_tv.cols();

	// identity matrix
	iMatrix im_identity(rangeDim);

	// 2. Compute the first flowpipe
	// compute A^n for 1 <= n <= k
	std::vector<iMatrix> A_exp_table;
	compute_int_mat_pow(A_exp_table, im_dyn_A, order + 1);

	// compute the expansion for exp(At)
	upMatrix expansion_exp_A_t_k = im_identity;

	for(int i=1; i<=order; ++i)
	{
		upMatrix A_t_i = A_exp_table[i];
		A_t_i.times_x(i);
		A_t_i *= factorial_rec[i];

		expansion_exp_A_t_k += A_t_i;
	}

	upMatrix up_Phi_0 = expansion_exp_A_t_k;

	// compute a remainder for exp(A*delta)
	Real factor_k_plus_1;
	factorial_rec[order+1].sup(factor_k_plus_1);

	Real step_pow_k_plus_1(step);
	step_pow_k_plus_1.pow_assign_RNDU(order + 1);

	factor_k_plus_1.mul_assign_RNDU(step_pow_k_plus_1);

	Real bound_exp_A_delta;
	im_dyn_A.max_norm(bound_exp_A_delta);
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
			if(connectivity[i][j])
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
			if(connectivity[i][j])
			{
				im_Phi_0_rem[i][j] = im_rem[i][j];
			}
		}
	}

	up_Phi_0 += im_Phi_0_rem;

	iMatrix im_B_zero(rangeDim, 1), im_TI_zero(rangeDim, numTIPar), im_TV_zero(rangeDim, numTVPar);

	LinearFlowpipe flowpipe;
	flowpipe.init_Phi = im_identity;

	iMatrix im_Phi;
	up_Phi_0.intEval(im_Phi, step_end_exp_table);


	upMatrix up_Psi_0;
	iMatrix im_Psi, im_trans_Psi;

	if(!bAuto)
	{
		up_Psi_0 = up_Phi_0 * im_dyn_B;
		up_Psi_0.integral();

		iMatrix im_trunc_step, im_trunc_step_end;
		up_Psi_0.ctrunc(im_trunc_step, im_trunc_step_end, order, step_exp_table, step_end_exp_table);

		up_Psi_0.intEval(im_Psi, step_end_exp_table);
		im_Psi += im_trunc_step_end;
		up_Psi_0 += im_trunc_step;

		flowpipe.init_Psi = im_B_zero;

		up_Psi_0.intEval(im_trans_Psi, step_exp_table);

		// simplify up_Psi_0
		up_Psi_0.ctrunc(order_cutoff, step_exp_table);

		flowpipe.trans_Psi = up_Psi_0;
	}

	iMatrix im_trans_Phi;
	up_Phi_0.intEval(im_trans_Phi, step_exp_table);


	// handle time-invariant uncertainties
	iMatrix im_Omega;
	upMatrix up_Omega_0;
	iMatrix im_trans_Omega;

	if(numTIPar > 0)
	{
		iMatrix im_trunc_step, im_trunc_step_end;
		up_Omega_0 = up_Phi_0 * im_dyn_ti;
		up_Omega_0.integral();
		up_Omega_0.ctrunc(im_trunc_step, im_trunc_step_end, order, step_exp_table, step_end_exp_table);

		up_Omega_0.intEval(im_Omega, step_end_exp_table);
		im_Omega += im_trunc_step_end;
		up_Omega_0 += im_trunc_step;

		flowpipe.init_Omega = im_TI_zero;

		up_Omega_0.intEval(im_trans_Omega, step_exp_table);

		// simplify up_Omega_0
		up_Omega_0.ctrunc(order_cutoff, step_exp_table);

		flowpipe.trans_Omega = up_Omega_0;
	}


	// simplify up_Phi_0
	up_Phi_0.ctrunc(order_cutoff, step_exp_table);
	flowpipe.trans_Phi = up_Phi_0;

	// handle time-varying uncertainties
	iMatrix tv_part;

	if(numTVPar > 0)
	{
		upMatrix up_tv = up_Phi_0 * im_dyn_tv;
		up_tv.intEval(tv_part, step_exp_table);
		tv_part *= im_tv_range;
		tv_part *= step_exp_table[1];
		flowpipe.tv_remainder = tv_part;
	}

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 1;

	int checking_result = COMPLETED_SAFE;
	std::vector<std::vector<Interval> > polyRangeX0;
	std::vector<iMatrix> set_of_rangeX0;
	std::vector<std::vector<Interval> > extended_domains;
	std::vector<Interval> vec_int_empty;

	std::vector<iMatrix> constraints, precond_Phi, precond_Psi, precond_Omega;

	bool simple_check;

	if(unsafeSet.size() >= rangeDim)
	{
		simple_check = false;
	}
	else
	{
		bool bLinear = true;

		for(int i=0; i<unsafeSet.size(); ++i)
		{
			if(unsafeSet[i].p.degree() > 1)
			{
				bLinear = false;
				break;
			}
		}

		if(bLinear)
		{
			simple_check = true;
		}
		else
		{
			simple_check = false;
		}
	}

	std::vector<Interval> checking_domain = initialSets[0].domain;
	checking_domain[0] = intStep;

	std::vector<Interval> ti_domain;
	iMatrix TIPar_range(numTIPar, 1);
	if(numTIPar > 0)
	{
		ti_domain.push_back(intZero);

		for(int j=0; j<numTIPar; ++j)
		{
			ti_domain.push_back(intUnit);
			TIPar_range[j][0] = intUnit;
		}
	}

	if(bSafetyChecking)
	{
		for(int m=0; m<initialSets.size(); ++m)
		{
			std::vector<Interval> intVecTemp;
			initialSets[m].tmvPre.polyRangeNormal(intVecTemp, step_exp_table);
			polyRangeX0.push_back(intVecTemp);

			iMatrix rangeX0(rangeDim, 1);
			for(int i1=0; i1<rangeDim; ++i1)
			{
				rangeX0[i1][0] = intVecTemp[i1] + initialSets[m].tmvPre.tms[i1].remainder;
			}

			set_of_rangeX0.push_back(rangeX0);

			if(!simple_check)
			{
				extended_domains.push_back(initialSets[m].domain);
				extended_domains[m][0] = intStep;

				for(int j=0; j<numTIPar; ++j)
				{
					extended_domains[m].push_back(intUnit);
				}
			}
			else
			{
				extended_domains.push_back(vec_int_empty);
			}
		}

		if(simple_check)
		{
			for(int i=0; i<unsafeSet.size(); ++i)
			{
				iMatrix linear_coefficients(1, rangeDim);
				unsafeSet[i].p.linearCoefficients(linear_coefficients, 0);

				constraints.push_back(linear_coefficients);

				precond_Phi.push_back(linear_coefficients * im_trans_Phi);

				if(!bAuto)
				{
					precond_Psi.push_back(linear_coefficients * im_trans_Psi);
				}

				if(numTIPar > 0)
				{
					precond_Omega.push_back(linear_coefficients * im_trans_Omega);
				}
			}
		}
		else
		{
			precond_Phi.push_back(im_trans_Phi);

			if(!bAuto)
			{
				precond_Psi.push_back(im_trans_Psi);
			}

			if(numTIPar > 0)
			{
				precond_Omega.push_back(im_trans_Omega);
			}
		}
	}

	if(bSafetyChecking)
	{
		if(bDump || bPlot)
		{
			flowpipes.push_back(flowpipe);
		}

		for(int m=0; m<initialSets.size(); ++m)
		{
			int safety;

			if(simple_check)
			{
				safety = flowpipe.safetyChecking(initialSets[m], polyRangeX0[m], step_exp_table, false, bAuto, precond_Phi, precond_Psi, precond_Omega, constraints, TIPar_range, set_of_rangeX0[m], unsafeSet, checking_domain, ti_domain, extended_domains[m], order, cutoff_threshold);
			}
			else
			{
				safety = flowpipe.safetyChecking(initialSets[m], polyRangeX0[m], step_exp_table, false, bAuto, precond_Phi, precond_Psi, precond_Omega, constraints, TIPar_range, set_of_rangeX0[m], unsafeSet, checking_domain, ti_domain, extended_domains[m], order, cutoff_threshold);
			}

			if(bDump || bPlot)
			{
				flowpipes_safety.push_back(safety);
			}

			if(safety == UNSAFE)
			{
				return COMPLETED_UNSAFE;
			}
			else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
			{
				checking_result = COMPLETED_UNKNOWN;
			}
		}
	}
	else
	{
		if(bDump || bPlot)
		{
			flowpipes.push_back(flowpipe);

			for(int m=0; m<initialSets.size(); ++m)
			{
				flowpipes_safety.push_back(SAFE);
			}
		}
	}


	iMatrix im_global_Psi = im_B_zero, im_global_Omega = im_TI_zero, im_global_TV = tv_part;

	int N = (int)ceil(time/step);
	std::vector<iMatrix> exp_A_delta_exp_table;

	exp_A_delta_exp_table.push_back(im_identity);
	exp_A_delta_exp_table.push_back(im_Phi);


	if(bPrint)
	{
		printf("time = %f,\t", step);
		printf("step = %f,\t", step);
		printf("order = %d\n", order);
	}

	for(int i=1; i<N; ++i)
	{
		LinearFlowpipe newFlowpipe;

		newFlowpipe.trans_Phi = up_Phi_0;

		if(i >= exp_A_delta_exp_table.size())
		{
			if(i%2 == 0)
			{
				int j = i/2;
				iMatrix im_temp = exp_A_delta_exp_table[j] * exp_A_delta_exp_table[j];
				exp_A_delta_exp_table.push_back(im_temp);
			}
			else
			{
				int j = i/2;
				iMatrix im_temp = exp_A_delta_exp_table[j] * exp_A_delta_exp_table[j+1];
				exp_A_delta_exp_table.push_back(im_temp);
			}
		}

		newFlowpipe.init_Phi = exp_A_delta_exp_table[i];

		if(!bAuto)
		{
			im_global_Psi += exp_A_delta_exp_table[i-1] * im_Psi;

			newFlowpipe.trans_Psi = up_Psi_0;
			newFlowpipe.init_Psi = im_global_Psi;
		}

		if(numTIPar > 0)
		{
			im_global_Omega += exp_A_delta_exp_table[i-1] * im_Omega;

			newFlowpipe.trans_Omega = up_Omega_0;
			newFlowpipe.init_Omega = im_global_Omega;
		}

		if(numTVPar > 0)
		{
			im_global_TV += exp_A_delta_exp_table[i] * tv_part;
			newFlowpipe.tv_remainder = im_global_TV;
		}

		++num_of_flowpipes;

		if(bSafetyChecking)
		{
			if(bDump || bPlot)
			{
				flowpipes.push_back(newFlowpipe);
			}

			for(int m=0; m<initialSets.size(); ++m)
			{
				int safety;

				if(simple_check)
				{
					safety = newFlowpipe.safetyChecking(initialSets[m], polyRangeX0[m], step_exp_table, false, bAuto, precond_Phi, precond_Psi, precond_Omega, constraints, TIPar_range, set_of_rangeX0[m], unsafeSet, checking_domain, ti_domain, extended_domains[m], order, cutoff_threshold);
				}
				else
				{
					safety = newFlowpipe.safetyChecking(initialSets[m], polyRangeX0[m], step_exp_table, false, bAuto, precond_Phi, precond_Psi, precond_Omega, constraints, TIPar_range, set_of_rangeX0[m], unsafeSet, checking_domain, ti_domain, extended_domains[m], order, cutoff_threshold);
				}

				if(bDump || bPlot)
				{
					flowpipes_safety.push_back(safety);
				}

				if(safety == UNSAFE)
				{
					if(bPrint)
					{
						printf("time = %f,\t", (i+1)*step);
						printf("step = %f,\t", step);
						printf("order = %d\n", order);
					}

					return COMPLETED_UNSAFE;
				}
				else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
				{
					checking_result = COMPLETED_UNKNOWN;
				}
			}
		}
		else
		{
			if(bDump || bPlot)
			{
				flowpipes.push_back(newFlowpipe);

				for(int m=0; m<initialSets.size(); ++m)
				{
					flowpipes_safety.push_back(SAFE);
				}
			}
		}


		if(bPrint)
		{
			printf("time = %f,\t", (i+1)*step);
			printf("step = %f,\t", step);
			printf("order = %d\n", order);
		}
	}

	return checking_result;
}

int ContinuousSystem::reach_ltv(std::list<LinearFlowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int maxNumSteps, const bool bPrint, const Interval & cutoff_threshold, const int order_cutoff,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump)
{
	const int rangeDim = up_dyn_A.rows();
	Interval intZero, intOne(1), intStep(0, step), intUnit(-1,1);
	Real rStep(step);

	int maxOrder = up_dyn_A.degree();
	int maxOrder_B = up_dyn_B.degree();
	int maxOrder_ti = up_dyn_ti.degree();

	if(maxOrder < maxOrder_B)
	{
		maxOrder = maxOrder_B;
	}

	if(maxOrder < maxOrder_ti)
	{
		maxOrder = maxOrder_ti;
	}

	std::vector<Interval> step_exp_table, step_end_exp_table;
	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1+maxOrder)+1);

	int numTIPar = up_dyn_ti.cols(), numTVPar = up_dyn_tv.cols();

	bool b_last_fp = false;
	iMatrix2 Phi_t_0(rangeDim), Psi_t_0(rangeDim, 1), Omega_t_0(rangeDim, numTIPar);
	Zonotope global_tv_remainder(rangeDim);

	flowpipes.clear();
	flowpipes_safety.clear();

	int checking_result = COMPLETED_SAFE;
	std::vector<std::vector<Interval> > polyRangeX0;
	std::vector<iMatrix> set_of_rangeX0;
	std::vector<std::vector<Interval> > extended_domains;
	std::vector<Interval> vec_int_empty;

	bool simple_check;

	if(unsafeSet.size() >= rangeDim)
	{
		simple_check = false;
	}
	else
	{
		bool bLinear = true;

		for(int i=0; i<unsafeSet.size(); ++i)
		{
			if(unsafeSet[i].p.degree() > 1)
			{
				bLinear = false;
				break;
			}
		}

		if(bLinear)
		{
			simple_check = true;
		}
		else
		{
			simple_check = false;
		}
	}

	if(bSafetyChecking)
	{
		for(int m=0; m<initialSets.size(); ++m)
		{
			std::vector<Interval> intVecTemp;
			initialSets[m].tmvPre.polyRangeNormal(intVecTemp, step_exp_table);
			polyRangeX0.push_back(intVecTemp);

			iMatrix rangeX0(rangeDim, 1);
			for(int i1=0; i1<rangeDim; ++i1)
			{
				rangeX0[i1][0] = intVecTemp[i1] + initialSets[m].tmvPre.tms[i1].remainder;
			}

			set_of_rangeX0.push_back(rangeX0);

			if(!simple_check)
			{
				extended_domains.push_back(initialSets[m].domain);
				extended_domains[m][0] = intStep;

				for(int j=0; j<numTIPar; ++j)
				{
					extended_domains[m].push_back(intUnit);
				}
			}
			else
			{
				extended_domains.push_back(vec_int_empty);
			}
		}
	}

	std::vector<Interval> checking_domain = initialSets[0].domain;
	checking_domain[0] = intStep;

	std::vector<Interval> ti_domain;
	iMatrix TIPar_range(numTIPar, 1);
	if(numTIPar > 0)
	{
		ti_domain.push_back(intZero);

		for(int j=0; j<numTIPar; ++j)
		{
			ti_domain.push_back(intUnit);
			TIPar_range[j][0] = intUnit;
		}
	}

	int num = 0;

	num_of_flowpipes = 0;

	for(double t0 = 0; ; )
	{
		if(t0 + step >= time - THRESHOLD_HIGH)
		{
			b_last_fp = true;
		}

		Interval int_t0(t0);

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

		iMatrix Omega_step_trunc;
		iMatrix Omega_step_end_trunc;
		iMatrix Omega_rem;

		LinearFlowpipe flowpipe;
		flowpipe.init_Phi = Phi_t_0;

		if(!bAuto)
		{
			flowpipe.init_Psi = Psi_t_0;
		}

		iMatrix tv_part;

		if(numTIPar > 0)
		{
			flowpipe.init_Omega = Omega_t_0;
		}

		compute_one_step_trans(flowpipe.trans_Phi, flowpipe.trans_Psi, flowpipe.trans_Omega,
				Phi_step_trunc, Phi_step_end_trunc, Phi_rem,
				Psi_step_trunc, Psi_step_end_trunc, Psi_rem,
				Omega_step_trunc, Omega_step_end_trunc, Omega_rem, tv_part,
				up_dyn_A, up_dyn_B, up_dyn_ti, up_dyn_tv,
				connectivity, bAuto, up_t0, order,
				step_exp_table, step_end_exp_table);

		flowpipe.trans_Phi += Phi_rem;

		if(!bAuto)
		{
			flowpipe.trans_Psi += Psi_rem;
		}

		if(numTIPar > 0)
		{
			flowpipe.trans_Omega += Omega_rem;
		}

		if(numTVPar > 0)
		{
			if(maxNumSteps >= 0 && num > maxNumSteps)
			{
				num = 0;
				global_tv_remainder.simplify();
			}

			tv_part *= im_tv_range;
			tv_part *= step_exp_table[1];
			Zonotope zonoTmp(tv_part);
			global_tv_remainder.MinSum(flowpipe.tv_remainder, zonoTmp);

			++num;
		}

		if(!b_last_fp)
		{
			iMatrix2 Phi_step_end;
			flowpipe.trans_Phi.intEval(Phi_step_end, step_end_exp_table);
			Phi_step_end += Phi_step_end_trunc;
			Phi_t_0 = Phi_step_end * Phi_t_0;

			if(!bAuto)
			{
				iMatrix2 Psi_step_end;
				flowpipe.trans_Psi.intEval(Psi_step_end, step_end_exp_table);
				Psi_step_end += Psi_step_end_trunc;
				Psi_t_0 = Phi_step_end * Psi_t_0 + Psi_step_end;
			}

			if(numTIPar > 0)
			{
				iMatrix2 Omega_step_end;
				flowpipe.trans_Omega.intEval(Omega_step_end, step_end_exp_table);
				Omega_step_end += Omega_step_end_trunc;
				Omega_t_0 = Phi_step_end * Omega_t_0 + Omega_step_end;
			}

			if(numTVPar > 0)
			{
				flowpipe.tv_remainder.linearTrans(global_tv_remainder, Phi_step_end);
			}
		}

		flowpipe.trans_Phi += Phi_step_trunc;

		if(!bAuto)
		{
			flowpipe.trans_Psi += Psi_step_trunc;
		}

		if(numTIPar > 0)
		{
			flowpipe.trans_Omega += Omega_step_trunc;
		}

		++num_of_flowpipes;

		if(bSafetyChecking)
		{
			if(bDump || bPlot)
			{
				flowpipes.push_back(flowpipe);
			}

			for(int m=0; m<initialSets.size(); ++m)
			{
				int safety;

				std::vector<iMatrix> constraints, precond_Phi, precond_Psi, precond_Omega;

				if(simple_check)
				{
					safety = flowpipe.safetyChecking(initialSets[m], polyRangeX0[m], step_exp_table, true, bAuto, precond_Phi, precond_Psi, precond_Omega, constraints, TIPar_range, set_of_rangeX0[m], unsafeSet, checking_domain, ti_domain, extended_domains[m], order, cutoff_threshold);
				}
				else
				{
					safety = flowpipe.safetyChecking(initialSets[m], polyRangeX0[m], step_exp_table, true, bAuto, precond_Phi, precond_Psi, precond_Omega, constraints, TIPar_range, set_of_rangeX0[m], unsafeSet, checking_domain, ti_domain, extended_domains[m], order, cutoff_threshold);
				}

				if(bDump || bPlot)
				{
					flowpipes_safety.push_back(safety);
				}

				if(safety == UNSAFE)
				{
					if(bPrint)
					{
						printf("time = %f,\t", t0 + step);
						printf("step = %f,\t", step);
						printf("order = %d\n", order);
					}

					return COMPLETED_UNSAFE;
				}
				else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
				{
					checking_result = COMPLETED_UNKNOWN;
				}
			}
		}
		else
		{
			if(bDump || bPlot)
			{
				flowpipes.push_back(flowpipe);
				flowpipes_safety.push_back(SAFE);
			}
		}

		t0 += step;

		if(bPrint)
		{
			printf("time = %f,\t", t0);
			printf("step = %f,\t", step);
			printf("order = %d\n", order);
		}

		if(b_last_fp)
		{
			break;
		}
	}

	return checking_result;
}


int ContinuousSystem::reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const std::vector<PolynomialConstraint> & unsafeSet,
		const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_picard(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, order, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
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
	}

	return checking_result;
}

int ContinuousSystem::reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_picard(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, orders, globalMaxOrder, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, orders, globalMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
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
	}

	return checking_result;
}

// adaptive step sizes and fixed orders
int ContinuousSystem::reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double miniStep, const double time, const int order, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
		double newStep = 0;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_picard(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, newStep, miniStep, order, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step_exp_table[1].sup();

				if(bPrint)
				{
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
	}

	return checking_result;
}

int ContinuousSystem::reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
		double newStep = 0;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_picard(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, newStep, miniStep, orders, globalMaxOrder, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, orders, globalMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step_exp_table[1].sup();

				if(bPrint)
				{
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
	}

	return checking_result;
}

// adaptive orders and fixed step sizes
int ContinuousSystem::reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*maxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		int newOrder = order;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_picard(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, newOrder, maxOrder, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, newOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
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
	}

	return checking_result;
}

int ContinuousSystem::reach_picard(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
		const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
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

			int res = currentFlowpipe.advance_picard(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, newOrders, localMaxOrder, maxOrders, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, newOrders, localMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step);
					printf("orders:\t");
					int num = newOrders.size()-1;
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
	}

	return checking_result;
}


// fixed step sizes and orders
int ContinuousSystem::reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	std::vector<PolynomialConstraint> dummy_invariant;

	std::vector<Polynomial> polyODE;
	for(int i=0; i<tmvOde_centered.tms.size(); ++i)
	{
		polyODE.push_back(tmvOde_centered.tms[i].expansion);
	}

	std::vector<HornerForm> taylorExpansion;
	computeTaylorExpansion(taylorExpansion, polyODE, order);

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOde, taylorExpansion, precondition, step_exp_table, step_end_exp_table, order, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
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
	}

	return checking_result;
}

int ContinuousSystem::reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	std::vector<Polynomial> polyODE;
	for(int i=0; i<tmvOde_centered.tms.size(); ++i)
	{
		polyODE.push_back(tmvOde_centered.tms[i].expansion);
	}

	std::vector<HornerForm> taylorExpansion;
	computeTaylorExpansion(taylorExpansion, polyODE, orders);

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOde, taylorExpansion, precondition, step_exp_table, step_end_exp_table, orders, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, orders, globalMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
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
	}

	return checking_result;
}

// adaptive step sizes and fixed orders
int ContinuousSystem::reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double miniStep, const double time, const int order, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	std::vector<PolynomialConstraint> dummy_invariant;

	std::vector<Polynomial> polyODE;
	for(int i=0; i<tmvOde_centered.tms.size(); ++i)
	{
		polyODE.push_back(tmvOde_centered.tms[i].expansion);
	}

	std::vector<HornerForm> taylorExpansion;
	computeTaylorExpansion(taylorExpansion, polyODE, order);

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
		double newStep = 0;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOde, taylorExpansion, precondition, step_exp_table, step_end_exp_table, newStep, miniStep, order, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step_exp_table[1].sup();

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step_exp_table[1].sup());
					printf("order = %d\n", order);
				}

				newStep = step_exp_table[1].sup() * LAMBDA_UP;

				double tDiffer = time - t;

				if(newStep > tDiffer)
				{
					newStep = tDiffer;
				}

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
	}

	return checking_result;
}

int ContinuousSystem::reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	std::vector<Polynomial> polyODE;
	for(int i=0; i<tmvOde_centered.tms.size(); ++i)
	{
		polyODE.push_back(tmvOde_centered.tms[i].expansion);
	}

	std::vector<HornerForm> taylorExpansion;
	computeTaylorExpansion(taylorExpansion, polyODE, orders);

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
		double newStep = 0;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOde, taylorExpansion, precondition, step_exp_table, step_end_exp_table, newStep, miniStep, orders, globalMaxOrder, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, orders, globalMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step_exp_table[1].sup();

				if(bPrint)
				{
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
	}

	return checking_result;
}

// adaptive orders and fixed step sizes
int ContinuousSystem::reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*maxOrder);

	std::vector<Polynomial> polyODE;
	for(int i=0; i<tmvOde_centered.tms.size(); ++i)
	{
		polyODE.push_back(tmvOde_centered.tms[i].expansion);
	}

	std::vector<HornerForm> taylorExpansionHF;
	std::vector<Polynomial> taylorExpansionMF;
	std::vector<Polynomial> highestTerms;

	computeTaylorExpansion(taylorExpansionHF, taylorExpansionMF, highestTerms, polyODE, order);

	std::vector<std::vector<HornerForm> > expansions;
	expansions.push_back(taylorExpansionHF);

	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		int newOrder = order;
		int currentMaxOrder = order;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOde, hfOde_centered, expansions[newOrder-order], precondition, step_exp_table, step_end_exp_table, newOrder, maxOrder, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, newOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step);
					printf("order = %d\n", newOrder);
				}

				if(newOrder > order)
				{
					--newOrder;

					if(newOrder > currentMaxOrder)
					{
						for(int i=currentMaxOrder; i<newOrder; ++i)
						{
							std::vector<HornerForm> newTaylorExpansionHF;
							std::vector<Polynomial> newTaylorExpansionMF;

							increaseExpansionOrder(newTaylorExpansionHF, newTaylorExpansionMF, highestTerms, taylorExpansionMF, polyODE, i);

							expansions.push_back(newTaylorExpansionHF);
							taylorExpansionMF = newTaylorExpansionMF;
						}

						currentMaxOrder = newOrder;
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
	}

	return checking_result;
}

int ContinuousSystem::reach_low_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
		const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);

	std::vector<Polynomial> polyODE;
	for(int i=0; i<tmvOde_centered.tms.size(); ++i)
	{
		polyODE.push_back(tmvOde_centered.tms[i].expansion);
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

	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		std::vector<int> newOrders = orders;
		std::vector<int> localMaxOrders = orders;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_low_degree(newFlowpipe, hfOde, hfOde_centered, taylorExpansionHF, precondition, step_exp_table, step_end_exp_table, newOrders, maxOrders, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, newOrders, globalMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step);
					printf("orders:\t");
					int num = newOrders.size()-1;
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
	}

	return checking_result;
}




// for high-degree ODEs
// fixed step sizes and orders

int ContinuousSystem::reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, order, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
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
	}

	return checking_result;
}

int ContinuousSystem::reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, orders, globalMaxOrder, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, orders, globalMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
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
	}

	return checking_result;
}

// adaptive step sizes and fixed orders
int ContinuousSystem::reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double miniStep, const double time, const int order, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
		double newStep = 0;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, newStep, miniStep, order, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step_exp_table[1].sup();

				if(bPrint)
				{
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
	}

	return checking_result;
}

int ContinuousSystem::reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
		double newStep = 0;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, newStep, miniStep, orders, globalMaxOrder, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, orders, globalMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step_exp_table[1].sup();

				if(bPrint)
				{
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
	}

	return checking_result;
}

// adaptive orders and fixed step sizes
int ContinuousSystem::reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*maxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
		int newOrder = order;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, newOrder, maxOrder, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, newOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
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
	}

	return checking_result;
}

int ContinuousSystem::reach_high_degree(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder,
		const int precondition, const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
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

			int res = currentFlowpipe.advance_high_degree(newFlowpipe, hfOde, hfOde_centered, precondition, step_exp_table, step_end_exp_table, newOrders, localMaxOrder, maxOrders, estimation, dummy_invariant, cutoff_threshold, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, newOrders, localMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step);
					printf("orders:\t");
					int num = newOrders.size()-1;
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
	}

	return checking_result;
}













// for non-polynomial ODEs (using Taylor approximations)
// fixed step sizes and orders
int ContinuousSystem::reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOde, strOde_centered, precondition, step_exp_table, step_end_exp_table, order, estimation, dummy_invariant, cutoff_threshold, constant, strOde_constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
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
	}

	return checking_result;
}

int ContinuousSystem::reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOde, strOde_centered, precondition, step_exp_table, step_end_exp_table, orders, globalMaxOrder, estimation, dummy_invariant, cutoff_threshold, constant, strOde_constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, orders, globalMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
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
	}

	return checking_result;
}

// adaptive step sizes and fixed orders
int ContinuousSystem::reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double miniStep, const double time, const int order, const int precondition, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
		double newStep = 0;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOde, strOde_centered, precondition, step_exp_table, step_end_exp_table, newStep, miniStep, order, estimation, dummy_invariant, cutoff_threshold, constant, strOde_constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step_exp_table[1].sup();

				if(bPrint)
				{
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
	}

	return checking_result;
}

int ContinuousSystem::reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double miniStep, const double time, const std::vector<int> & orders, const int globalMaxOrder, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
		double newStep = 0;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOde, strOde_centered, precondition, step_exp_table, step_end_exp_table, newStep, miniStep, orders, globalMaxOrder, estimation, dummy_invariant, cutoff_threshold, constant, strOde_constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, orders, globalMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step_exp_table[1].sup();

				if(bPrint)
				{
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
	}

	return checking_result;
}


// adaptive orders and fixed step sizes
int ContinuousSystem::reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*maxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
		int newOrder = order;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOde, strOde_centered, precondition, step_exp_table, step_end_exp_table, newOrder, maxOrder, estimation, dummy_invariant, cutoff_threshold, constant, strOde_constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, newOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
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
	}

	return checking_result;
}

int ContinuousSystem::reach_non_polynomial_taylor(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const std::vector<int> & orders, const std::vector<int> & maxOrders, const int globalMaxOrder, const int precondition,
		const std::vector<Interval> & estimation, const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*globalMaxOrder);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];
		std::vector<int> newOrders = orders;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int localMaxOrder = newOrders[0];
			for(int i=1; i<newOrders.size(); ++i)
			{
				if(localMaxOrder < newOrders[i])
					localMaxOrder = newOrders[i];
			}

			int res = currentFlowpipe.advance_non_polynomial_taylor(newFlowpipe, strOde, strOde_centered, precondition, step_exp_table, step_end_exp_table, newOrders, localMaxOrder, maxOrders, estimation, dummy_invariant, cutoff_threshold, constant, strOde_constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, newOrders, localMaxOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step);
					printf("orders:\t");
					int num = newOrders.size()-1;
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
	}

	return checking_result;
}






// fixed orders and step sizes
int ContinuousSystem::reach_picard_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	int rangeDim = tmvOde.tms.size();

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	Interval intOne(1);
	std::vector<Interval> initial_scalars;

	for(int i=0; i<rangeDim; ++i)
	{
		initial_scalars.push_back(intOne);
	}

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		std::vector<iMatrix> J;
		std::vector<iMatrix> Phi_L;

		std::vector<Interval> scalars = initial_scalars;

		std::vector<Polynomial> initial_set_poly;
		initialSets[m].tmv.Expansion(initial_set_poly);

		int iterNum = 0;

		for(double t=THRESHOLD_HIGH; t < time; )
		{
			int res = currentFlowpipe.advance_picard_symbolic_remainder(newFlowpipe, hfOde, hfOde_centered, step_exp_table, step_end_exp_table, order, estimation, cutoff_threshold, initial_set_poly, scalars, J, Phi_L, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step);
					printf("order = %d\n", order);
				}

				++iterNum;

				if(iterNum == N)
				{
					currentFlowpipe.tmv.Expansion(initial_set_poly);
					scalars = initial_scalars;
					J.clear();
					Phi_L.clear();
					iterNum = 0;
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
	}

	return checking_result;
}

// adaptive step sizes and fixed orders
int ContinuousSystem::reach_picard_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double miniStep, const double time, const int order, const std::vector<Interval> & estimation,
		const bool bPrint, const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	int rangeDim = tmvOde.tms.size();

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	Interval intOne(1);
	std::vector<Interval> initial_scalars;

	for(int i=0; i<rangeDim; ++i)
	{
		initial_scalars.push_back(intOne);
	}

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		double newStep = 0;

		std::vector<iMatrix> J;
		std::vector<iMatrix> Phi_L;

		std::vector<Interval> scalars = initial_scalars;

		std::vector<Polynomial> initial_set_poly;
		initialSets[m].tmv.Expansion(initial_set_poly);

		int iterNum = 0;

		for(double t=THRESHOLD_HIGH; t < time; )
		{
			int res = currentFlowpipe.advance_picard_symbolic_remainder(newFlowpipe, hfOde, hfOde_centered, step_exp_table, step_end_exp_table, newStep, miniStep, order, estimation, cutoff_threshold, initial_set_poly, scalars, J, Phi_L, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step_exp_table[1].sup();

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step_exp_table[1].sup());
					printf("order = %d\n", order);
				}

				newStep = step_exp_table[1].sup() * LAMBDA_UP;
				if(newStep > step - THRESHOLD_HIGH)
				{
					newStep = 0;
				}

				++iterNum;

				if(iterNum == N)
				{
					currentFlowpipe.tmv.Expansion(initial_set_poly);
					scalars = initial_scalars;
					J.clear();
					Phi_L.clear();
					iterNum = 0;
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
	}

	return checking_result;
}

int ContinuousSystem::reach_picard_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int maxOrder, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*maxOrder);
	int rangeDim = tmvOde.tms.size();

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	Interval intOne(1);
	std::vector<Interval> initial_scalars;

	for(int i=0; i<rangeDim; ++i)
	{
		initial_scalars.push_back(intOne);
	}

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		int newOrder = order;

		std::vector<iMatrix> J;
		std::vector<iMatrix> Phi_L;

		std::vector<Interval> scalars = initial_scalars;

		std::vector<Polynomial> initial_set_poly;
		initialSets[m].tmv.Expansion(initial_set_poly);

		int iterNum = 0;

		for(double t=THRESHOLD_HIGH; t < time; )
		{
			int res = currentFlowpipe.advance_picard_symbolic_remainder(newFlowpipe, hfOde, hfOde_centered, step_exp_table, step_end_exp_table, newOrder, maxOrder, estimation, cutoff_threshold, initial_set_poly, scalars, J, Phi_L, constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step);
					printf("order = %d\n", newOrder);
				}

				++iterNum;

				if(iterNum == N)
				{
					currentFlowpipe.tmv.Expansion(initial_set_poly);
					scalars = initial_scalars;
					J.clear();
					Phi_L.clear();
					iterNum = 0;
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
	}

	return checking_result;
}



// solving non-polynomial ODEs with symbolic remainders

int ContinuousSystem::reach_non_polynomial_taylor_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	int rangeDim = strOde.size();

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	Interval intOne(1);
	std::vector<Interval> initial_scalars;

	for(int i=0; i<rangeDim; ++i)
	{
		initial_scalars.push_back(intOne);
	}

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		std::vector<iMatrix> J;
		std::vector<iMatrix> Phi_L;

		std::vector<Interval> scalars = initial_scalars;

		std::vector<Polynomial> initial_set_poly;
		initialSets[m].tmv.Expansion(initial_set_poly);

		int iterNum = 0;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_non_polynomial_taylor_symbolic_remainder(newFlowpipe, strOde, strOde_centered, precondition, step_exp_table, step_end_exp_table, order, estimation, dummy_invariant, cutoff_threshold, initial_set_poly, scalars, J, Phi_L, constant, strOde_constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step);
					printf("order = %d\n", order);
				}

				++iterNum;

				if(iterNum == N)
				{
					currentFlowpipe.tmv.Expansion(initial_set_poly);
					scalars = initial_scalars;
					J.clear();
					Phi_L.clear();
					iterNum = 0;
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
	}

	return checking_result;
}

int ContinuousSystem::reach_non_polynomial_taylor_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double miniStep, const double time, const int order, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	int rangeDim = strOde.size();

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	Interval intOne(1);
	std::vector<Interval> initial_scalars;

	for(int i=0; i<rangeDim; ++i)
	{
		initial_scalars.push_back(intOne);
	}

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		double newStep = 0;

		std::vector<iMatrix> J;
		std::vector<iMatrix> Phi_L;

		std::vector<Interval> scalars = initial_scalars;

		std::vector<Polynomial> initial_set_poly;
		initialSets[m].tmv.Expansion(initial_set_poly);

		int iterNum = 0;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_non_polynomial_taylor_symbolic_remainder(newFlowpipe, strOde, strOde_centered, precondition, step_exp_table, step_end_exp_table, newStep, miniStep, order, estimation, dummy_invariant, cutoff_threshold, initial_set_poly, scalars, J, Phi_L, constant, strOde_constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, order, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step_exp_table[1].sup();

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step_exp_table[1].sup());
					printf("order = %d\n", order);
				}

				newStep = step_exp_table[1].sup() * LAMBDA_UP;
				if(newStep > step - THRESHOLD_HIGH)
				{
					newStep = 0;
				}

				++iterNum;

				if(iterNum == N)
				{
					currentFlowpipe.tmv.Expansion(initial_set_poly);
					scalars = initial_scalars;
					J.clear();
					Phi_L.clear();
					iterNum = 0;
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
	}

	return checking_result;
}

int ContinuousSystem::reach_non_polynomial_taylor_symbolic_remainder(std::list<Flowpipe> & flowpipes, std::list<int> & flowpipes_safety, long & num_of_flowpipes,
		const double step, const double time, const int order, const int maxOrder, const int precondition, const std::vector<Interval> & estimation, const bool bPrint,
		const std::vector<std::string> & stateVarNames, const Interval & cutoff_threshold, const int N,
		const std::vector<PolynomialConstraint> & unsafeSet, const bool bSafetyChecking, const bool bPlot, const bool bDump) const
{
	std::vector<Interval> step_exp_table, step_end_exp_table;
	int rangeDim = strOde.size();

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(maxOrder+1)+1);
	std::vector<PolynomialConstraint> dummy_invariant;

	flowpipes.clear();
	flowpipes_safety.clear();

	num_of_flowpipes = 0;

	int checking_result = COMPLETED_SAFE;

	Interval intOne(1);
	std::vector<Interval> initial_scalars;

	for(int i=0; i<rangeDim; ++i)
	{
		initial_scalars.push_back(intOne);
	}

	for(int m=0; m<initialSets.size(); ++m)
	{
		Flowpipe newFlowpipe, currentFlowpipe = initialSets[m];

		int newOrder = order;

		std::vector<iMatrix> J;
		std::vector<iMatrix> Phi_L;

		std::vector<Interval> scalars = initial_scalars;

		std::vector<Polynomial> initial_set_poly;
		initialSets[m].tmv.Expansion(initial_set_poly);

		int iterNum = 0;

		for(double t=THRESHOLD_HIGH; t < time;)
		{
			int res = currentFlowpipe.advance_non_polynomial_taylor_symbolic_remainder(newFlowpipe, strOde, strOde_centered, precondition, step_exp_table, step_end_exp_table, newOrder, maxOrder, estimation, dummy_invariant, cutoff_threshold, initial_set_poly, scalars, J, Phi_L, constant, strOde_constant);

			if(res == 1)
			{
				++num_of_flowpipes;

				if(bSafetyChecking)
				{
					int safety = newFlowpipe.safetyChecking(step_exp_table, unsafeSet, newOrder, cutoff_threshold);

					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(safety);
					}

					if(safety == UNSAFE)
					{
						return COMPLETED_UNSAFE;
					}
					else if(safety == UNKNOWN && checking_result == COMPLETED_SAFE)
					{
						checking_result = COMPLETED_UNKNOWN;
					}
				}
				else
				{
					if(bDump || bPlot)
					{
						flowpipes.push_back(newFlowpipe);
						flowpipes_safety.push_back(SAFE);
					}
				}

				currentFlowpipe = newFlowpipe;

				t += step;

				if(bPrint)
				{
					printf("time = %f,\t", t);
					printf("step = %f,\t", step);
					printf("order = %d\n", newOrder);
				}

				++iterNum;

				if(iterNum == N)
				{
					currentFlowpipe.tmv.Expansion(initial_set_poly);
					scalars = initial_scalars;
					J.clear();
					Phi_L.clear();
					iterNum = 0;
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
	}

	return checking_result;
}

ContinuousSystem & ContinuousSystem::operator = (const ContinuousSystem & system)
{
	if(this == &system)
		return *this;

	tmvOde				= system.tmvOde;
	hfOde				= system.hfOde;
	initialSets			= system.initialSets;
	strOde				= system.strOde;
	tmvOde_centered		= system.tmvOde_centered;
	hfOde_centered		= system.hfOde_centered;
	strOde_centered		= system.strOde_centered;
	bAuto				= system.bAuto;
	im_dyn_A			= system.im_dyn_A;
	im_dyn_B			= system.im_dyn_B;
	im_dyn_ti			= system.im_dyn_ti;
	im_dyn_tv			= system.im_dyn_tv;
	up_dyn_A			= system.up_dyn_A;
	up_dyn_B			= system.up_dyn_B;
	up_dyn_ti			= system.up_dyn_ti;
	up_dyn_tv			= system.up_dyn_tv;
	im_tv_range			= system.im_tv_range;
	connectivity		= system.connectivity;
	constant			= system.constant;
	strOde_constant		= system.strOde_constant;

	return *this;
}




































// class ContinuousReachability

ContinuousReachability::ContinuousReachability()
{
	bPlot = false;
	bDump = true;
}

ContinuousReachability::~ContinuousReachability()
{
	outputAxes.clear();
	flowpipes.clear();
	orders.clear();
	maxOrders.clear();
	flowpipesCompo.clear();
	domains.clear();
	flowpipes_safety.clear();
	unsafeSet.clear();
	stateVarTab.clear();
	stateVarNames.clear();
	tmVarTab.clear();
	tmVarNames.clear();
	parTab.clear();
	parNames.clear();
}

void ContinuousReachability::dump(FILE *fp) const
{
	fprintf(fp,"state var ");
	for(int i=0; i<stateVarNames.size()-1; ++i)
	{
		fprintf(fp, "%s,", stateVarNames[i].c_str());
	}

	fprintf(fp, "%s\n\n", stateVarNames[stateVarNames.size()-1].c_str());

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

	if(integrationScheme == LTI || integrationScheme == LTV)
	{
		fprintf(fp, "step %f\n\n", step);
	}

	fprintf(fp, "order %d\n\n", globalMaxOrder);
	fprintf(fp, "cutoff %e\n\n", cutoff_threshold.sup());
	fprintf(fp, "output %s\n\n", outputFileName);

	if(bSafetyChecking)
	{
		// dump the unsafe set
		fprintf(fp, "unsafe\n{\n");

		for(int i=0; i<unsafeSet.size(); ++i)
		{
			unsafeSet[i].dump(fp, stateVarNames);
		}

		fprintf(fp, "}\n\n");
	}

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


	// separate the initial sets and the symbolic flowpipes
	if(integrationScheme == LTI || integrationScheme == LTV)
	{
		fprintf(fp, "time-inv { ");

		if(TI_Par_Names.size() > 0)
		{
			for(int i=0; i<TI_Par_Names.size()-1; ++i)
			{
				fprintf(fp, "%s, ", TI_Par_Names[i].c_str());
			}

			fprintf(fp, "%s", TI_Par_Names[TI_Par_Names.size()-1].c_str());
		}

		fprintf(fp, " }\n\n");

		fprintf(fp, "init\n{\n");

		fprintf(fp, "tm var ");

		for(int i=1; i<tmVarNames.size()-1; ++i)
		{
			fprintf(fp, "%s,", tmVarNames[i].c_str());
		}

		fprintf(fp, "%s\n\n", tmVarNames[tmVarNames.size()-1].c_str());

		std::vector<std::list<TaylorModelVec> > unsafe_flowpipes;
		std::vector<std::list<TaylorModelVec> > unknown_flowpipes;
		std::list<TaylorModelVec> empty_list;

		for(int m=0; m<system.initialSets.size(); ++m)
		{
			fprintf(fp, "{\n");

			system.initialSets[m].tmvPre.dump_interval(fp, stateVarNames, tmVarNames);

			for(int i=0; i<system.initialSets[m].domain.size(); ++i)
			{
				fprintf(fp, "%s in ", tmVarNames[i].c_str());
				system.initialSets[m].domain[i].dump(fp);
				fprintf(fp, "\n");
			}

			fprintf(fp, "}\n\n");

			unsafe_flowpipes.push_back(empty_list);
			unknown_flowpipes.push_back(empty_list);
		}

		fprintf(fp, "}\n");

		std::vector<std::string> initialVarNames;
		initialVarNames.push_back(tmVarNames[0]);

		for(int i=0; i<stateVarNames.size(); ++i)
		{
			initialVarNames.push_back(stateVarNames[i] + "0");
		}

		for(int i=0; i<TI_Par_Names.size(); ++i)
		{
			initialVarNames.push_back(TI_Par_Names[i]);
		}

		fprintf(fp, "linear continuous flowpipes\n{\n");

		fprintf(fp, "tm var ");

		for(int i=1; i<initialVarNames.size()-1; ++i)
		{
			fprintf(fp, "%s,", initialVarNames[i].c_str());
		}

		fprintf(fp, "%s\n\n", initialVarNames[initialVarNames.size()-1].c_str());

		std::list<TaylorModelVec>::const_iterator fpIter = flowpipesCompo.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();

		for(; fpIter != flowpipesCompo.end() && safetyIter != flowpipes_safety.end(); ++fpIter)
		{
			fprintf(fp, "{\n");

			fpIter->dump_interval(fp, stateVarNames, initialVarNames);

			fprintf(fp, "}\n\n");

			for(int m=0; m<system.initialSets.size() && safetyIter != flowpipes_safety.end(); ++m, ++safetyIter)
			{
				if(*safetyIter == UNSAFE)
				{
					unsafe_flowpipes[m].push_back(*fpIter);
				}
				else if(*safetyIter == UNKNOWN)
				{
					unknown_flowpipes[m].push_back(*fpIter);
				}
			}
		}

		fprintf(fp, "}\n");

		if(bDumpCounterexamples)
		{
			fprintf(fpDumpCounterexamples, "Unsafe flowpipes:\n\n");
			dump_counterexample(fpDumpCounterexamples, unsafe_flowpipes, system.initialSets, initialVarNames);
			fprintf(fpDumpCounterexamples, "Unknown flowpipes:\n\n");
			dump_counterexample(fpDumpCounterexamples, unknown_flowpipes, system.initialSets, initialVarNames);
			fclose(fpDumpCounterexamples);
		}
	}
	else
	{
		fprintf(fp, "continuous flowpipes\n{\n");

		fprintf(fp, "tm var ");
		for(int i=1; i<tmVarNames.size()-1; ++i)
		{
			fprintf(fp, "%s,", tmVarNames[i].c_str());
		}

		fprintf(fp, "%s\n\n", tmVarNames[tmVarNames.size()-1].c_str());

		std::list<TaylorModelVec> unsafe_tm_flowpipes;
		std::list<std::vector<Interval> > unsafe_flowpipe_domains;

		std::list<TaylorModelVec> unknown_tm_flowpipes;
		std::list<std::vector<Interval> > unknown_flowpipe_domains;

		std::list<TaylorModelVec>::const_iterator fpIter = flowpipesCompo.begin();
		std::list<std::vector<Interval> >::const_iterator doIter = domains.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();

		for(; safetyIter != flowpipes_safety.end(); ++fpIter, ++doIter, ++safetyIter)
		{
			if(*safetyIter == UNSAFE)
			{
				unsafe_tm_flowpipes.push_back(*fpIter);
				unsafe_flowpipe_domains.push_back(*doIter);
			}
			else if(*safetyIter == UNKNOWN)
			{
				unknown_tm_flowpipes.push_back(*fpIter);
				unknown_flowpipe_domains.push_back(*doIter);
			}

			fprintf(fp, "{\n");
			fpIter->dump_interval(fp, stateVarNames, tmVarNames);

			for(int i=0; i<doIter->size(); ++i)
			{
				fprintf(fp, "%s in ", tmVarNames[i].c_str());
				(*doIter)[i].dump(fp);
				fprintf(fp, "\n");
			}

			fprintf(fp, "}\n\n");
		}

		fprintf(fp, "}\n");

		if(bDumpCounterexamples)
		{
			fprintf(fpDumpCounterexamples, "Unsafe flowpipes:\n\n");
			dump_counterexample(fpDumpCounterexamples, unsafe_tm_flowpipes, unsafe_flowpipe_domains);
			fprintf(fpDumpCounterexamples, "Unknown flowpipes:\n\n");
			dump_counterexample(fpDumpCounterexamples, unknown_tm_flowpipes, unknown_flowpipe_domains);
			fclose(fpDumpCounterexamples);
		}
	}

/*
	// ======================= test begin ========================
	fpIter = flowpipesCompo.end();
	--fpIter;
	doIter = domains.end();
	--doIter;
	TaylorModelVec tmvTemp;
	vector<Interval> step_exp_table;
	Interval intStep((*doIter)[0].sup());
	construct_step_exp_table(step_exp_table, intStep, globalMaxOrder);
	fpIter->evaluate_t(tmvTemp, step_exp_table);
	vector<Interval> enclosure;
	tmvTemp.intEval(enclosure, *doIter);

	for(int i=0; i<enclosure.size(); ++i)
	{
		enclosure[i].dump(stdout);
		printf("\n");
	}
	// ======================= test end ========================
*/
}

int ContinuousReachability::run()
{
	compute_factorial_rec(globalMaxOrder+2);
	compute_power_4(globalMaxOrder+2);
	compute_double_factorial(2*globalMaxOrder+4);

	int result = 1;

	switch(integrationScheme)
	{
	case ONLY_PICARD:
	{
		switch(orderType)
		{
		case UNIFORM:
			if(bAdaptiveSteps)
			{
				result = system.reach_picard(flowpipes, flowpipes_safety, num_of_flowpipes, step, miniStep, time, orders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else if(bAdaptiveOrders)
			{
				result = system.reach_picard(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], maxOrders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else
			{
				result = system.reach_picard(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], precondition,
						estimation, bPrint, stateVarNames, cutoff_threshold, unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			break;
		case MULTI:
			if(bAdaptiveSteps)
			{
				result = system.reach_picard(flowpipes, flowpipes_safety, num_of_flowpipes, step, miniStep, time, orders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else if(bAdaptiveOrders)
			{
				result = system.reach_picard(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders, maxOrders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else
			{
				result = system.reach_picard(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
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
				result = system.reach_low_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, miniStep, time, orders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else if(bAdaptiveOrders)
			{
				result = system.reach_low_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], maxOrders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else
			{
				result = system.reach_low_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			break;
		case MULTI:
			if(bAdaptiveSteps)
			{
				result = system.reach_low_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, miniStep, time, orders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else if(bAdaptiveOrders)
			{
				result = system.reach_low_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders, maxOrders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else
			{
				result = system.reach_low_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
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
				result = system.reach_high_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, miniStep, time, orders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else if(bAdaptiveOrders)
			{
				result = system.reach_high_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], maxOrders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else
			{
				result = system.reach_high_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			break;
		case MULTI:
			if(bAdaptiveSteps)
			{
				result = system.reach_high_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, miniStep, time, orders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else if(bAdaptiveOrders)
			{
				result = system.reach_high_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders, maxOrders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else
			{
				result = system.reach_high_degree(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
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
				result = system.reach_non_polynomial_taylor(flowpipes, flowpipes_safety, num_of_flowpipes, step, miniStep, time, orders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else if(bAdaptiveOrders)
			{
				result = system.reach_non_polynomial_taylor(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], maxOrders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else
			{
				result = system.reach_non_polynomial_taylor(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			break;
		case MULTI:
			if(bAdaptiveSteps)
			{
				result = system.reach_non_polynomial_taylor(flowpipes, flowpipes_safety, num_of_flowpipes, step, miniStep, time, orders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else if(bAdaptiveOrders)
			{
				result = system.reach_non_polynomial_taylor(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders, maxOrders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			else
			{
				result = system.reach_non_polynomial_taylor(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders, globalMaxOrder, precondition, estimation, bPrint, stateVarNames, cutoff_threshold,
						unsafeSet, bSafetyChecking, bPlot, bDump);
			}
			break;
		}
		break;
	}

	case LTI:
	{
		result = system.reach_lti(linearFlowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], bPrint, cutoff_threshold, orders[0], unsafeSet, bSafetyChecking, bPlot, bDump);
		break;
	}

	case LTV:
	{
		result = system.reach_ltv(linearFlowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], maxNumSteps, bPrint, cutoff_threshold, orders[0], unsafeSet, bSafetyChecking, bPlot, bDump);
		break;
	}

	case ONLY_PICARD_SYMB:
	{
		if(bAdaptiveSteps)
		{
			result = system.reach_picard_symbolic_remainder(flowpipes, flowpipes_safety, num_of_flowpipes, step, miniStep, time, orders[0], estimation, bPrint, stateVarNames, cutoff_threshold, max_remainder_queue,
					unsafeSet, bSafetyChecking, bPlot, bDump);
		}
		else if(bAdaptiveOrders)
		{
			result = system.reach_picard_symbolic_remainder(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], maxOrders[0], estimation, bPrint, stateVarNames, cutoff_threshold, max_remainder_queue,
					unsafeSet, bSafetyChecking, bPlot, bDump);
		}
		else
		{
			result = system.reach_picard_symbolic_remainder(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], estimation, bPrint, stateVarNames, cutoff_threshold, max_remainder_queue,
					unsafeSet, bSafetyChecking, bPlot, bDump);
		}
		break;
	}

	case NONPOLY_TAYLOR_SYMB:
	{
		if(bAdaptiveSteps)
		{
			result = system.reach_non_polynomial_taylor_symbolic_remainder(flowpipes, flowpipes_safety, num_of_flowpipes, step, miniStep, time, orders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold, max_remainder_queue,
					unsafeSet, bSafetyChecking, bPlot, bDump);
		}
		else if(bAdaptiveOrders)
		{
			result = system.reach_non_polynomial_taylor_symbolic_remainder(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], maxOrders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold, max_remainder_queue,
					unsafeSet, bSafetyChecking, bPlot, bDump);
		}
		else
		{
			result = system.reach_non_polynomial_taylor_symbolic_remainder(flowpipes, flowpipes_safety, num_of_flowpipes, step, time, orders[0], precondition, estimation, bPrint, stateVarNames, cutoff_threshold, max_remainder_queue,
					unsafeSet, bSafetyChecking, bPlot, bDump);
		}

		break;
	}
	}

	return result;
}

void ContinuousReachability::prepareForPlotting()
{
	flowpipesCompo.clear();
	domains.clear();

	if(integrationScheme == LTI || integrationScheme == LTV)
	{
		Interval intStep(0, step), intUnit(-1,1);

		std::vector<Interval> checking_domain = system.initialSets[0].domain;
		checking_domain[0] = intStep;

		std::vector<Interval> newDomain = system.initialSets[0].domain;
		newDomain[0] = intStep;
		int numTIPar = TI_Par_Names.size();

		for(int i=0; i<numTIPar; ++i)
		{
			newDomain.push_back(intUnit);
			declareTMVar(TI_Par_Names[i]);
		}

		int i = 0, total_size = system.initialSets.size() * linearFlowpipes.size();

		for(int m=0; m<system.initialSets.size(); ++m)
		{
			std::vector<Interval> polyRangeX0;
			system.initialSets[m].tmvPre.polyRange(polyRangeX0, system.initialSets[m].domain);

			std::list<LinearFlowpipe>::iterator iter;

			for(iter = linearFlowpipes.begin(); iter != linearFlowpipes.end(); )
			{
				TaylorModelVec tmvFlowpipe;
				iter->toTaylorModel(tmvFlowpipe, system.bAuto, outputAxes, system.initialSets[m], checking_domain, numTIPar, polyRangeX0, cutoff_threshold);

				flowpipesCompo.push_back(tmvFlowpipe);
				domains.push_back(newDomain);

				++i;
				printf("\b\b\b");
				printf(BOLD_FONT "%%" RESET_COLOR);
				printf(BOLD_FONT "%2d" RESET_COLOR, (int)(i*100/total_size));
				fflush(stdout);

				iter = linearFlowpipes.erase(iter);
			}
		}

		printf("\n");
	}
	else
	{
		Interval intStep;

		int i = 0, total_size = flowpipes.size();

		std::list<Flowpipe>::const_iterator iter;

		for(iter = flowpipes.begin(); iter != flowpipes.end(); )
		{
			TaylorModelVec tmvTemp;

			iter->composition(tmvTemp, outputAxes, globalMaxOrder, cutoff_threshold);

			flowpipesCompo.push_back(tmvTemp);
			domains.push_back(iter->domain);

			++i;
			printf("\b\b\b");
			printf(BOLD_FONT "%%" RESET_COLOR);
			printf(BOLD_FONT "%2d" RESET_COLOR, (int)(i*100/total_size));
			fflush(stdout);

			iter = flowpipes.erase(iter);
		}

		printf("\n");
	}
}

void ContinuousReachability::prepareForDumping()
{
	flowpipesCompo.clear();
	domains.clear();

	if(integrationScheme == LTI || integrationScheme == LTV)
	{
		std::list<LinearFlowpipe>::iterator iter;

		int prog = 0, total_size = linearFlowpipes.size();

		for(iter = linearFlowpipes.begin(); iter != linearFlowpipes.end(); ++iter)
		{
			TaylorModelVec flowpipe;
			iter->toTaylorModel(flowpipe, system.bAuto);
			flowpipesCompo.push_back(flowpipe);

			++prog;
			printf("\b\b\b");
			printf(BOLD_FONT "%%" RESET_COLOR);
			printf(BOLD_FONT "%2d" RESET_COLOR, (int)(prog*100/total_size));
			fflush(stdout);
		}

		printf("\n");


/*
		Interval intStep(0, step);

		std::vector<Interval> checking_domain = system.initialSets[0].domain;
		checking_domain[0] = intStep;

		std::vector<Interval> newDomain = system.initialSets[0].domain;
		newDomain[0] = intStep;
		int numTIPar = TI_Par_Ranges.size();

		for(int i=0; i<numTIPar; ++i)
		{
			newDomain.push_back(TI_Par_Ranges[i]);
			declareTMVar(TI_Par_Names[i]);
		}

		int prog = 0, total_size = system.initialSets.size() * linearFlowpipes.size();

		for(int m=0; m<system.initialSets.size(); ++m)
		{
			std::vector<Interval> newDomain = system.initialSets[m].domain;
			newDomain[0] = intStep;
			int numTIPar = TI_Par_Ranges.size();

			for(int i=0; i<numTIPar; ++i)
			{
				newDomain.push_back(TI_Par_Ranges[i]);
				declareTMVar(TI_Par_Names[i]);
			}

			std::vector<Interval> polyRangeX0;
			system.initialSets[m].tmvPre.polyRange(polyRangeX0, system.initialSets[m].domain);

			std::list<LinearFlowpipe>::iterator iter;

			for(iter = linearFlowpipes.begin(); iter != linearFlowpipes.end(); ++iter)
			{
				TaylorModelVec tmvFlowpipe;
				iter->toTaylorModel(tmvFlowpipe, system.initialSets[m], checking_domain, numTIPar, polyRangeX0, cutoff_threshold);

				flowpipesCompo.push_back(tmvFlowpipe);
				domains.push_back(newDomain);

				++prog;
				printf("\b\b\b");
				printf(BOLD_FONT "%%" RESET_COLOR);
				printf(BOLD_FONT "%2d" RESET_COLOR, (int)(prog*100/total_size));
				fflush(stdout);
			}
		}

		printf("\n");
*/
	}
	else
	{
//		std::vector<Interval> step_exp_table;
		Interval intStep;

		int prog = 0, total_size = flowpipes.size();

		std::list<Flowpipe>::const_iterator iter;

		for(iter = flowpipes.begin(); iter != flowpipes.end(); ++iter)
		{
//			if(step_exp_table.size() == 0 || intStep != iter->domain[0])
//			{
//				construct_step_exp_table(step_exp_table, iter->domain[0], globalMaxOrder);
//				intStep = iter->domain[0];
//			}
/*
			for(int i=0; i<iter->tmvPre.tms.size(); ++i)
			{
				iter->tmvPre.tms[i].remainder.dump(stdout);
				printf("\n");
			}
			printf("\n");
			for(int i=0; i<iter->tmv.tms.size(); ++i)
			{
				iter->tmv.tms[i].remainder.dump(stdout);
				printf("\n");
			}
*/
			TaylorModelVec tmvTemp;

			iter->composition(tmvTemp, globalMaxOrder, cutoff_threshold);

			flowpipesCompo.push_back(tmvTemp);
			domains.push_back(iter->domain);

			++prog;
			printf("\b\b\b");
			printf(BOLD_FONT "%%" RESET_COLOR);
			printf(BOLD_FONT "%2d" RESET_COLOR, (int)(prog*100/total_size));
			fflush(stdout);
		}

		printf("\n");
	}
}
/*
void ContinuousReachability::composition()
{
	flowpipesCompo.clear();
	domains.clear();

	std::vector<Interval> step_exp_table;
	Interval intStep;

	std::list<Flowpipe>::const_iterator iter;

	for(iter = flowpipes.begin(); iter != flowpipes.end(); ++iter)
	{
		if(step_exp_table.size() == 0 || intStep != iter->domain[0])
		{
			construct_step_exp_table(step_exp_table, iter->domain[0], globalMaxOrder);
			intStep = iter->domain[0];
		}

		TaylorModelVec tmvTemp;

		iter->composition_normal(tmvTemp, step_exp_table, cutoff_threshold);

		flowpipesCompo.push_back(tmvTemp);
		domains.push_back(iter->domain);
	}
}
*/
int ContinuousReachability::safetyChecking()
{
	if(unsafeSet.size() == 0)
	{
		return UNSAFE;	// since the whole state space is unsafe, the system is not safe
	}

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

	int prog = 0, total_size = flowpipes_safety.size();

	if(domains.size() == 0)		// linear flowpipes
	{
		int checking_result = SAFE;
		Interval intStep(0, step);
//		int rangeDim = system.initialSets[0].tmvPre.tms.size();
		int num_initialSets = system.initialSets.size();

		std::vector<TaylorModelVec> initialSets;
		std::vector<std::vector<Interval> > initialSetDomains;
		std::vector<std::vector<Interval> > initialSetPolyRanges;

		int numTIPar = TI_Par_Names.size();

		tmVarNames = domainVarNames;

		std::vector<std::string> initialVarNames;
		std::string tVar("local_t");
		initialVarNames.push_back(tVar);

		for(int i=0; i<stateVarNames.size(); ++i)
		{
			initialVarNames.push_back(stateVarNames[i] + "0");
		}

		for(int i=0; i<TI_Par_Names.size(); ++i)
		{
			initialVarNames.push_back(TI_Par_Names[i]);
		}

		std::vector<std::list<TaylorModelVec> > unsafe_flowpipes;
		std::vector<std::list<TaylorModelVec> > unknown_flowpipes;
		std::list<TaylorModelVec> empty_list;

		for(int m=0; m<system.initialSets.size(); ++m)
		{
			initialSets.push_back(system.initialSets[m].tmvPre);

			std::vector<Interval> domain = system.initialSets[m].domain;
			domain[0] = intStep;

			int domainDim = domain.size();
			int newDomainDim = domainDim + numTIPar;

			Interval intOne(1), intUnit(-1,1);

			for(int i=0; i<TI_Par_Names.size(); ++i)
			{
				TaylorModel tmTemp(intOne, newDomainDim);
				tmTemp.expansion.mul_assign(domainDim + i, 1);
				initialSets[m].tms.push_back(tmTemp);

				domain.push_back(intUnit);
			}

			initialSetDomains.push_back(domain);

			std::vector<Interval> polyRange;
			initialSets[m].polyRange(polyRange, domain);

			initialSetPolyRanges.push_back(polyRange);

			initialSets[m].extend(numTIPar);

			unsafe_flowpipes.push_back(empty_list);
			unknown_flowpipes.push_back(empty_list);
		}

		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<int>::iterator safetyIter = flowpipes_safety.begin();
		bool bTerminate = false;

		for(; safetyIter != flowpipes_safety.end() && !bTerminate; ++tmvIter)
		{
			for(int m=0; m<num_initialSets && safetyIter != flowpipes_safety.end(); ++m, ++safetyIter)
			{
				TaylorModelVec flowpipe;
				tmvIter->insert(flowpipe, initialSets[m], initialSetPolyRanges[m], initialSetDomains[m], cutoff_threshold);

				int safety = safetyChecking2(flowpipe, initialSetDomains[m], unsafeSet, globalMaxOrder, cutoff_threshold);

				if(safety == UNSAFE)
				{
					*safetyIter = UNSAFE;
					checking_result = UNSAFE;

					if(bDumpCounterexamples)
					{
						unsafe_flowpipes[m].push_back(*tmvIter);
					}

					bTerminate = true;
					break;
				}
				else if(safety == UNKNOWN)
				{
					*safetyIter = UNKNOWN;

					if(checking_result == SAFE)
					{
						checking_result = UNKNOWN;
					}

					if(bDumpCounterexamples)
					{
						unknown_flowpipes[m].push_back(*tmvIter);
					}
				}

				++prog;
				printf("\b\b\b\b");
				printf(BOLD_FONT "%%" RESET_COLOR);
				printf(BOLD_FONT "%3d" RESET_COLOR, (int)(prog*100/total_size));
				fflush(stdout);
			}
		}

		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);

		if(bDumpCounterexamples)
		{
			fprintf(fpDumpCounterexamples, "Unsafe flowpipes:\n\n");
			dump_counterexample(fpDumpCounterexamples, unsafe_flowpipes, system.initialSets, initialVarNames);
			fprintf(fpDumpCounterexamples, "Unknown flowpipes:\n\n");
			dump_counterexample(fpDumpCounterexamples, unknown_flowpipes, system.initialSets, initialVarNames);
			fclose(fpDumpCounterexamples);
		}

		return checking_result;
	}
	else if(flowpipesCompo.size() > 0)
	{
		std::list<TaylorModelVec> unsafe_tm_flowpipes;
		std::list<std::vector<Interval> > unsafe_flowpipe_domains;

		std::list<TaylorModelVec> unknown_tm_flowpipes;
		std::list<std::vector<Interval> > unknown_flowpipe_domains;

		int checking_result = SAFE;

//		flowpipes_safety.clear();

		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<std::vector<Interval> >::const_iterator doIter = domains.begin();
		std::list<int>::iterator safetyIter = flowpipes_safety.begin();

//		int domainDim = (*doIter).size();

		for(; tmvIter!=flowpipesCompo.end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			int safety = safetyChecking2(*tmvIter, *doIter, unsafeSet, globalMaxOrder, cutoff_threshold);

			if(safety == UNSAFE)
			{
				*safetyIter = UNSAFE;
				checking_result = UNSAFE;

				if(bDumpCounterexamples)
				{
					unsafe_tm_flowpipes.push_back(*tmvIter);
					unsafe_flowpipe_domains.push_back(*doIter);
				}

				break;
			}
			else if(safety == UNKNOWN)
			{
				*safetyIter = UNKNOWN;

				if(checking_result == SAFE)
				{
					checking_result = UNKNOWN;
				}

				if(bDumpCounterexamples)
				{
					unknown_tm_flowpipes.push_back(*tmvIter);
					unknown_flowpipe_domains.push_back(*doIter);
				}
			}

			++prog;
			printf("\b\b\b\b");
			printf(BOLD_FONT "%%" RESET_COLOR);
			printf(BOLD_FONT "%3d" RESET_COLOR, (int)(prog*100/total_size));
			fflush(stdout);
		}

		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);

		if(bDumpCounterexamples)
		{
			fprintf(fpDumpCounterexamples, "Unsafe flowpipes:\n\n");
			dump_counterexample(fpDumpCounterexamples, unsafe_tm_flowpipes, unsafe_flowpipe_domains);
			fprintf(fpDumpCounterexamples, "Unknown flowpipes:\n\n");
			dump_counterexample(fpDumpCounterexamples, unknown_tm_flowpipes, unknown_flowpipe_domains);
			fclose(fpDumpCounterexamples);
		}

		return checking_result;
	}
	else
	{
		return SAFE;	// no flowpipe is computed
	}
}

long ContinuousReachability::numOfFlowpipes() const
{
	return num_of_flowpipes;
}

void ContinuousReachability::dump_counterexample(FILE *fp, const std::list<TaylorModelVec> & flowpipes, const std::list<std::vector<Interval> > & domains) const
{
	std::list<TaylorModelVec>::const_iterator fpIter = flowpipes.begin();
	std::list<std::vector<Interval> >::const_iterator doIter = domains.begin();

	for(; fpIter!=flowpipes.end(); ++fpIter, ++doIter)
	{
		fprintf(fp, "{\n");

		fpIter->dump_interval(fp, stateVarNames, tmVarNames);

		for(int i=0; i<doIter->size(); ++i)
		{
			fprintf(fp, "%s in ", tmVarNames[i].c_str());
			(*doIter)[i].dump(fp);
			fprintf(fp, "\n");
		}

		fprintf(fp, "}\n\n\n");
	}
}

void ContinuousReachability::dump_counterexample(FILE *fp, const std::vector<std::list<TaylorModelVec> > & flowpipes, const std::vector<Flowpipe> & initialSets, const std::vector<std::string> & initialVarNames) const
{
	for(int m=0; m<initialSets.size(); ++m)
	{
		if(flowpipes[m].size() > 0)
		{
			std::list<TaylorModelVec>::const_iterator fpIter = flowpipes[m].begin();

			fprintf(fp, "{\ninitial set:\n\n");

			initialSets[m].tmvPre.dump_interval(fp, stateVarNames, tmVarNames);

			for(int i=0; i<initialSets[m].domain.size(); ++i)
			{
				fprintf(fp, "%s in ", tmVarNames[i].c_str());
				initialSets[m].domain[i].dump(fp);
				fprintf(fp, "\n");
			}

			fprintf(fp, "\n\nflowpipes:\n\n");

			for(; fpIter != flowpipes[m].end(); ++fpIter)
			{
				fpIter->dump_interval(fp, stateVarNames, initialVarNames);
				fprintf(fp, "\n\n");
			}

			fprintf(fp, "}\n\n");
		}
	}
}

void ContinuousReachability::plot_2D(const bool bProjected)
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

	printf("Generating the plot file...\n");

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

void ContinuousReachability::plot_2D_GNUPLOT(FILE *fp, const bool bProjected) const
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

void ContinuousReachability::plot_2D_interval_GNUPLOT(FILE *fp, const bool bProjected) const
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

	std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
	std::list<std::vector<Interval> >::const_iterator doIter = domains.begin();
	std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();


	std::vector<int> varIDs;
	if(bProjected)
	{
		varIDs.push_back(0);
		varIDs.push_back(1);
	}
	else
	{
		varIDs.push_back(outputAxes[0]);
		varIDs.push_back(outputAxes[1]);
	}

	int prog = 0, total_size = flowpipes_safety.size();

	if(domains.size() == 0)
	{
		Interval intStep(0, step);
//		int rangeDim = system.initialSets[0].tmvPre.tms.size();
		int num_initialSets = system.initialSets.size();

		std::vector<TaylorModelVec> initialSets;
		std::vector<std::vector<Interval> > initialSetDomains;
		std::vector<std::vector<Interval> > initialSetPolyRanges;

		int numTIPar = TI_Par_Names.size();

		for(int m=0; m<system.initialSets.size(); ++m)
		{
			initialSets.push_back(system.initialSets[m].tmvPre);

			std::vector<Interval> domain = system.initialSets[m].domain;
			domain[0] = intStep;

			int domainDim = domain.size();
			int newDomainDim = domainDim + numTIPar;

			Interval intOne(1), intUnit(-1,1);

			for(int i=0; i<TI_Par_Names.size(); ++i)
			{
				TaylorModel tmTemp(intOne, newDomainDim);
				tmTemp.expansion.mul_assign(domainDim + i, 1);
				initialSets[m].tms.push_back(tmTemp);

				domain.push_back(intUnit);
			}

			initialSetDomains.push_back(domain);

			std::vector<Interval> polyRange;
			initialSets[m].polyRange(polyRange, domain);

			initialSetPolyRanges.push_back(polyRange);

			initialSets[m].extend(numTIPar);
		}

		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();
		bool bTerminate = false;

		for(; safetyIter != flowpipes_safety.end() && !bTerminate; ++tmvIter)
		{
			for(int m=0; m<num_initialSets && safetyIter != flowpipes_safety.end(); ++m)
			{
				TaylorModel tmTemp;
				tmvIter->tms[outputAxes[0]].insert(tmTemp, initialSets[m], initialSetPolyRanges[m], initialSetDomains[m], cutoff_threshold);

				Interval X;
				tmTemp.intEval(X, initialSetDomains[m]);

				tmvIter->tms[outputAxes[1]].insert(tmTemp, initialSets[m], initialSetPolyRanges[m], initialSetDomains[m], cutoff_threshold);

				Interval Y;
				tmTemp.intEval(Y, initialSetDomains[m]);

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
					bTerminate = true;
					break;
				}

				++safetyIter;
			}
		}

		fprintf(fp, "e\n");
		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}
	else
	{
		for(; safetyIter != flowpipes_safety.end() && doIter != domains.end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			std::vector<Interval> box;
			tmvIter->intEval(box, *doIter, varIDs);

			// output the vertices
			fprintf(fp, "%lf %lf\n", box[0].inf(), box[1].inf());
			fprintf(fp, "%lf %lf\n", box[0].sup(), box[1].inf());
			fprintf(fp, "%lf %lf\n", box[0].sup(), box[1].sup());
			fprintf(fp, "%lf %lf\n", box[0].inf(), box[1].sup());
			fprintf(fp, "%lf %lf\n", box[0].inf(), box[1].inf());
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

		fprintf(fp, "e\n");
		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}
}

void ContinuousReachability::plot_2D_octagon_GNUPLOT(FILE *fp, const bool bProjected) const
{
	int x = outputAxes[0];
	int y = outputAxes[1];

	int rangeDim;

	if(bProjected)
	{
		x = 0;
		y = 1;
		rangeDim = 2;
	}
	else
	{
		x = outputAxes[0];
		y = outputAxes[1];
		rangeDim = stateVarNames.size();
	}

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
	Polyhedron polyTemplate(sortedTemplate, b);

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

	// Compute the intersections of two facets.
	// The vertices are ordered clockwisely.

	gsl_matrix *C = gsl_matrix_alloc(2,2);
	gsl_vector *d = gsl_vector_alloc(2);
	gsl_vector *vertex = gsl_vector_alloc(2);

	int prog = 0, total_size = flowpipes_safety.size();

	if(domains.size() == 0)
	{
		Interval intStep(0, step);
//		int rangeDim = system.initialSets[0].tmvPre.tms.size();
		int num_initialSets = system.initialSets.size();

		std::vector<TaylorModelVec> initialSets;
		std::vector<std::vector<Interval> > initialSetDomains;
		std::vector<std::vector<Interval> > initialSetPolyRanges;

		int numTIPar = TI_Par_Names.size();

		for(int m=0; m<system.initialSets.size(); ++m)
		{
			initialSets.push_back(system.initialSets[m].tmvPre);

			std::vector<Interval> domain = system.initialSets[m].domain;
			domain[0] = intStep;

			int domainDim = domain.size();
			int newDomainDim = domainDim + numTIPar;

			Interval intOne(1), intUnit(-1,1);

			for(int i=0; i<TI_Par_Names.size(); ++i)
			{
				TaylorModel tmTemp(intOne, newDomainDim);
				tmTemp.expansion.mul_assign(domainDim + i, 1);
				initialSets[m].tms.push_back(tmTemp);

				domain.push_back(intUnit);
			}

			initialSetDomains.push_back(domain);

			std::vector<Interval> polyRange;
			initialSets[m].polyRange(polyRange, domain);

			initialSetPolyRanges.push_back(polyRange);

			initialSets[m].extend(numTIPar);
		}

		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();
		bool bTerminate = false;

		for(; safetyIter != flowpipes_safety.end() && !bTerminate; ++tmvIter)
		{
			for(int m=0; m<num_initialSets && safetyIter != flowpipes_safety.end(); ++m)
			{
				TaylorModelVec tmvTemp;
				tmvIter->insert(tmvTemp, initialSets[m], initialSetPolyRanges[m], initialSetDomains[m], cutoff_threshold);

				templatePolyhedron(polyTemplate, tmvTemp, initialSetDomains[m]);

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
					bTerminate = true;
					break;
				}

				++safetyIter;
			}
		}

		fprintf(fp, "e\n");
		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}
	else
	{
		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<std::vector<Interval> >::const_iterator doIter = domains.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();

		int prog = 0, total_size = flowpipes_safety.size();

		for(; safetyIter != flowpipes_safety.end() && doIter != domains.end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			templatePolyhedron(polyTemplate, *tmvIter, *doIter);

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

		fprintf(fp, "e\n");
		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}

	gsl_matrix_free(C);
	gsl_vector_free(d);
	gsl_vector_free(vertex);
}

void ContinuousReachability::plot_2D_grid_GNUPLOT(FILE *fp, const bool bProjected) const
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

	int x, y;
	if(bProjected)
	{
		x = 0;
		y = 1;
	}
	else
	{
		x = outputAxes[0];
		y = outputAxes[1];
	}

	int prog = 0, total_size = flowpipes_safety.size();

	if(domains.size() == 0)
	{
		Interval intStep(0, step);
//		int rangeDim = system.initialSets[0].tmvPre.tms.size();
		int num_initialSets = system.initialSets.size();

		std::vector<TaylorModelVec> initialSets;
		std::vector<std::vector<Interval> > initialSetDomains;
		std::vector<std::vector<Interval> > initialSetPolyRanges;

		int numTIPar = TI_Par_Names.size();

		for(int m=0; m<system.initialSets.size(); ++m)
		{
			initialSets.push_back(system.initialSets[m].tmvPre);

			std::vector<Interval> domain = system.initialSets[m].domain;
			domain[0] = intStep;

			int domainDim = domain.size();
			int newDomainDim = domainDim + numTIPar;

			Interval intOne(1), intUnit(-1,1);

			for(int i=0; i<TI_Par_Names.size(); ++i)
			{
				TaylorModel tmTemp(intOne, newDomainDim);
				tmTemp.expansion.mul_assign(domainDim + i, 1);
				initialSets[m].tms.push_back(tmTemp);

				domain.push_back(intUnit);
			}

			initialSetDomains.push_back(domain);

			std::vector<Interval> polyRange;
			initialSets[m].polyRange(polyRange, domain);

			initialSetPolyRanges.push_back(polyRange);

			initialSets[m].extend(numTIPar);
		}

		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();
		bool bTerminate = false;

		for(; safetyIter != flowpipes_safety.end() && !bTerminate; ++tmvIter)
		{
			for(int m=0; m<num_initialSets && safetyIter != flowpipes_safety.end(); ++m)
			{
				// decompose the domain
				std::list<std::vector<Interval> > grids;

				gridBox(grids, initialSetDomains[m], numSections);

				TaylorModel tmTemp;
				tmvIter->tms[outputAxes[0]].insert(tmTemp, initialSets[m], initialSetPolyRanges[m], initialSetDomains[m], cutoff_threshold);

				HornerForm hfOutputX;
				Interval remainderX;

				tmTemp.toHornerForm(hfOutputX, remainderX);


				tmvIter->tms[outputAxes[1]].insert(tmTemp, initialSets[m], initialSetPolyRanges[m], initialSetDomains[m], cutoff_threshold);

				HornerForm hfOutputY;
				Interval remainderY;

				tmTemp.toHornerForm(hfOutputY, remainderY);

				// evaluate the images from all of the grids
				std::list<std::vector<Interval> >::const_iterator gIter = grids.begin();
				for(; gIter!=grids.end(); ++gIter)
				{
					Interval X;
					hfOutputX.intEval(X, *gIter);
					X += remainderX;

					Interval Y;
					hfOutputY.intEval(Y, *gIter);
					Y += remainderY;

					fprintf(fp, "%e %e\n", X.inf(), Y.inf());
					fprintf(fp, "%e %e\n", X.sup(), Y.inf());
					fprintf(fp, "%e %e\n", X.sup(), Y.sup());
					fprintf(fp, "%e %e\n", X.inf(), Y.sup());
					fprintf(fp, "%e %e\n", X.inf(), Y.inf());
					fprintf(fp, "\n\n");
				}

				++prog;
				printf("\b\b\b\b");
				printf(BOLD_FONT "%%" RESET_COLOR);
				printf(BOLD_FONT "%3d" RESET_COLOR, (int)(prog*100/total_size));
				fflush(stdout);

				if(*safetyIter == UNSAFE)
				{
					bTerminate = true;
					break;
				}

				++safetyIter;
			}
		}

		fprintf(fp, "e\n");
		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}
	else
	{
		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<std::vector<Interval> >::const_iterator doIter = domains.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();

		for(; safetyIter != flowpipes_safety.end() && doIter != domains.end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			// decompose the domain
			std::list<std::vector<Interval> > grids;

			gridBox(grids, *doIter, numSections);

			// we only consider the output dimensions
			HornerForm hfOutputX;
			Interval remainderX;

			tmvIter->tms[x].toHornerForm(hfOutputX, remainderX);

			HornerForm hfOutputY;
			Interval remainderY;

			tmvIter->tms[y].toHornerForm(hfOutputY, remainderY);

			// evaluate the images from all of the grids
			std::list<std::vector<Interval> >::const_iterator gIter = grids.begin();
			for(; gIter!=grids.end(); ++gIter)
			{
				Interval X;
				hfOutputX.intEval(X, *gIter);
				X += remainderX;

				Interval Y;
				hfOutputY.intEval(Y, *gIter);
				Y += remainderY;

				// output the vertices
				fprintf(fp, "%e %e\n", X.inf(), Y.inf());
				fprintf(fp, "%e %e\n", X.sup(), Y.inf());
				fprintf(fp, "%e %e\n", X.sup(), Y.sup());
				fprintf(fp, "%e %e\n", X.inf(), Y.sup());
				fprintf(fp, "%e %e\n", X.inf(), Y.inf());
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

		fprintf(fp, "e\n");
		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}
}

void ContinuousReachability::plot_2D_MATLAB(FILE *fp, const bool bProjected)
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

void ContinuousReachability::plot_2D_interval_MATLAB(FILE *fp, const bool bProjected) const
{
	std::vector<int> varIDs;
	if(bProjected)
	{
		varIDs.push_back(0);
		varIDs.push_back(1);
	}
	else
	{
		varIDs.push_back(outputAxes[0]);
		varIDs.push_back(outputAxes[1]);
	}

	int prog = 0, total_size = flowpipes_safety.size();

	if(domains.size() == 0)
	{
		Interval intStep(0, step);
//		int rangeDim = system.initialSets[0].tmvPre.tms.size();
		int num_initialSets = system.initialSets.size();

		std::vector<TaylorModelVec> initialSets;
		std::vector<std::vector<Interval> > initialSetDomains;
		std::vector<std::vector<Interval> > initialSetPolyRanges;

		int numTIPar = TI_Par_Names.size();

		for(int m=0; m<system.initialSets.size(); ++m)
		{
			initialSets.push_back(system.initialSets[m].tmvPre);

			std::vector<Interval> domain = system.initialSets[m].domain;
			domain[0] = intStep;

			int domainDim = domain.size();
			int newDomainDim = domainDim + numTIPar;

			Interval intOne(1), intUnit(-1,1);

			for(int i=0; i<TI_Par_Names.size(); ++i)
			{
				TaylorModel tmTemp(intOne, newDomainDim);
				tmTemp.expansion.mul_assign(domainDim + i, 1);
				initialSets[m].tms.push_back(tmTemp);

				domain.push_back(intUnit);
			}

			initialSetDomains.push_back(domain);

			std::vector<Interval> polyRange;
			initialSets[m].polyRange(polyRange, domain);

			initialSetPolyRanges.push_back(polyRange);

			initialSets[m].extend(numTIPar);
		}

		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();
		bool bTerminate = false;

		for(; safetyIter != flowpipes_safety.end() && !bTerminate; ++tmvIter)
		{
			for(int m=0; m<num_initialSets && safetyIter != flowpipes_safety.end(); ++m)
			{
				TaylorModel tmTemp;
				tmvIter->tms[outputAxes[0]].insert(tmTemp, initialSets[m], initialSetPolyRanges[m], initialSetDomains[m], cutoff_threshold);

				Interval X;
				tmTemp.intEval(X, initialSetDomains[m]);

				tmvIter->tms[outputAxes[1]].insert(tmTemp, initialSets[m], initialSetPolyRanges[m], initialSetDomains[m], cutoff_threshold);

				Interval Y;
				tmTemp.intEval(Y, initialSetDomains[m]);

				switch(*safetyIter)
				{
				case SAFE:
					fprintf(fp,"plot( [%e,%e,%e,%e,%e] , [%e,%e,%e,%e,%e] , 'color' , '[0 0.4 0]');\nhold on;\nclear;\n",
							X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
					break;
				case UNSAFE:
					fprintf(fp,"plot( [%e,%e,%e,%e,%e] , [%e,%e,%e,%e,%e] , 'color' , '[1 0 0]');\nhold on;\nclear;\n",
							X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
					break;
				case UNKNOWN:
					fprintf(fp,"plot( [%e,%e,%e,%e,%e] , [%e,%e,%e,%e,%e] , 'color' , '[0 0 1]');\nhold on;\nclear;\n",
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
					bTerminate = true;
					break;
				}

				++safetyIter;
			}
		}

		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}
	else
	{
		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<std::vector<Interval> >::const_iterator doIter = domains.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();

		for(; safetyIter != flowpipes_safety.end() ; ++tmvIter, ++doIter, ++safetyIter)
		{
			std::vector<Interval> box;
			tmvIter->intEval(box, *doIter, varIDs);

			Interval X = box[0], Y = box[1];

			switch(*safetyIter)
			{
			case SAFE:
				fprintf(fp,"plot( [%e,%e,%e,%e,%e] , [%e,%e,%e,%e,%e] , 'color' , '[0 0.4 0]');\nhold on;\nclear;\n",
						X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
				break;
			case UNSAFE:
				fprintf(fp,"plot( [%e,%e,%e,%e,%e] , [%e,%e,%e,%e,%e] , 'color' , '[1 0 0]');\nhold on;\nclear;\n",
						X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
				break;
			case UNKNOWN:
				fprintf(fp,"plot( [%e,%e,%e,%e,%e] , [%e,%e,%e,%e,%e] , 'color' , '[0 0 1]');\nhold on;\nclear;\n",
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

		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}
}

void ContinuousReachability::plot_2D_octagon_MATLAB(FILE *fp, const bool bProjected) const
{
	int x, y;

	int rangeDim;

	if(bProjected)
	{
		x = 0;
		y = 1;
		rangeDim = 2;
	}
	else
	{
		x = outputAxes[0];
		y = outputAxes[1];
		rangeDim = stateVarNames.size();
	}

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
	Polyhedron polyTemplate(sortedTemplate, b);

	// Compute the intersections of two facets.
	// The vertices are ordered clockwisely.

	gsl_matrix *C = gsl_matrix_alloc(2,2);
	gsl_vector *d = gsl_vector_alloc(2);
	gsl_vector *vertex = gsl_vector_alloc(2);

	int prog = 0, total_size = flowpipes_safety.size();

	if(domains.size() == 0)
	{
		Interval intStep(0, step);
//		int rangeDim = system.initialSets[0].tmvPre.tms.size();
		int num_initialSets = system.initialSets.size();

		std::vector<TaylorModelVec> initialSets;
		std::vector<std::vector<Interval> > initialSetDomains;
		std::vector<std::vector<Interval> > initialSetPolyRanges;

		int numTIPar = TI_Par_Names.size();

		for(int m=0; m<system.initialSets.size(); ++m)
		{
			initialSets.push_back(system.initialSets[m].tmvPre);

			std::vector<Interval> domain = system.initialSets[m].domain;
			domain[0] = intStep;

			int domainDim = domain.size();
			int newDomainDim = domainDim + numTIPar;

			Interval intOne(1), intUnit(-1,1);

			for(int i=0; i<TI_Par_Names.size(); ++i)
			{
				TaylorModel tmTemp(intOne, newDomainDim);
				tmTemp.expansion.mul_assign(domainDim + i, 1);
				initialSets[m].tms.push_back(tmTemp);

				domain.push_back(intUnit);
			}

			initialSetDomains.push_back(domain);

			std::vector<Interval> polyRange;
			initialSets[m].polyRange(polyRange, domain);

			initialSetPolyRanges.push_back(polyRange);

			initialSets[m].extend(numTIPar);
		}

		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();
		bool bTerminate = false;

		for(; safetyIter != flowpipes_safety.end() && !bTerminate; ++tmvIter)
		{
			for(int m=0; m<num_initialSets && safetyIter != flowpipes_safety.end(); ++m)
			{
				TaylorModelVec tmvTemp;
				tmvIter->insert(tmvTemp, initialSets[m], initialSetPolyRanges[m], initialSetDomains[m], cutoff_threshold);

				templatePolyhedron(polyTemplate, tmvTemp, initialSetDomains[m]);

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
					fprintf(fp, "%e , ", vertices_y[i]);
				}
				fprintf(fp, "%e ] , ", vertices_y.back());

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
					bTerminate = true;
					break;
				}

				++safetyIter;
			}
		}

		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}
	else
	{
		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<std::vector<Interval> >::const_iterator doIter = domains.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();

		for(; safetyIter != flowpipes_safety.end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			templatePolyhedron(polyTemplate, *tmvIter, *doIter);

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
				fprintf(fp, "%e , ", vertices_y[i]);
			}
			fprintf(fp, "%e ] , ", vertices_y.back());

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

		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}

	gsl_matrix_free(C);
	gsl_vector_free(d);
	gsl_vector_free(vertex);
}

void ContinuousReachability::plot_2D_grid_MATLAB(FILE *fp, const bool bProjected) const
{
	int x, y;
	if(bProjected)
	{
		x = 0;
		y = 1;
	}
	else
	{
		x = outputAxes[0];
		y = outputAxes[1];
	}

	int prog = 0, total_size = flowpipes_safety.size();

	if(domains.size() == 0)
	{
		Interval intStep(0, step);
//		int rangeDim = system.initialSets[0].tmvPre.tms.size();
		int num_initialSets = system.initialSets.size();

		std::vector<TaylorModelVec> initialSets;
		std::vector<std::vector<Interval> > initialSetDomains;
		std::vector<std::vector<Interval> > initialSetPolyRanges;

		int numTIPar = TI_Par_Names.size();

		for(int m=0; m<system.initialSets.size(); ++m)
		{
			initialSets.push_back(system.initialSets[m].tmvPre);

			std::vector<Interval> domain = system.initialSets[m].domain;
			domain[0] = intStep;

			int domainDim = domain.size();
			int newDomainDim = domainDim + numTIPar;

			Interval intOne(1), intUnit(-1,1);

			for(int i=0; i<TI_Par_Names.size(); ++i)
			{
				TaylorModel tmTemp(intOne, newDomainDim);
				tmTemp.expansion.mul_assign(domainDim + i, 1);
				initialSets[m].tms.push_back(tmTemp);

				domain.push_back(intUnit);
			}

			initialSetDomains.push_back(domain);

			std::vector<Interval> polyRange;
			initialSets[m].polyRange(polyRange, domain);

			initialSetPolyRanges.push_back(polyRange);

			initialSets[m].extend(numTIPar);
		}

		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();
		bool bTerminate = false;

		for(; safetyIter != flowpipes_safety.end() && !bTerminate; ++tmvIter)
		{
			for(int m=0; m<num_initialSets && safetyIter != flowpipes_safety.end(); ++m)
			{
				// decompose the domain
				std::list<std::vector<Interval> > grids;

				gridBox(grids, initialSetDomains[m], numSections);

				TaylorModel tmTemp;
				tmvIter->tms[outputAxes[0]].insert(tmTemp, initialSets[m], initialSetPolyRanges[m], initialSetDomains[m], cutoff_threshold);

				HornerForm hfOutputX;
				Interval remainderX;

				tmTemp.toHornerForm(hfOutputX, remainderX);


				tmvIter->tms[outputAxes[1]].insert(tmTemp, initialSets[m], initialSetPolyRanges[m], initialSetDomains[m], cutoff_threshold);

				HornerForm hfOutputY;
				Interval remainderY;

				tmTemp.toHornerForm(hfOutputY, remainderY);

				// evaluate the images from all of the grids
				std::list<std::vector<Interval> >::const_iterator gIter = grids.begin();
				for(; gIter!=grids.end(); ++gIter)
				{
					Interval X;
					hfOutputX.intEval(X, *gIter);
					X += remainderX;

					Interval Y;
					hfOutputY.intEval(Y, *gIter);
					Y += remainderY;

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
					bTerminate = true;
					break;
				}

				++safetyIter;
			}
		}

		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}
	else
	{
		std::list<TaylorModelVec>::const_iterator tmvIter = flowpipesCompo.begin();
		std::list<std::vector<Interval> >::const_iterator doIter = domains.begin();
		std::list<int>::const_iterator safetyIter = flowpipes_safety.begin();

		for(; safetyIter != flowpipes_safety.end(); ++tmvIter, ++doIter, ++safetyIter)
		{
			// decompose the domain
			std::list<std::vector<Interval> > grids;

			gridBox(grids, *doIter, numSections);

			// we only consider the output dimensions
			HornerForm hfOutputX;
			Interval remainderX;

			tmvIter->tms[x].toHornerForm(hfOutputX, remainderX);


			HornerForm hfOutputY;
			Interval remainderY;

			tmvIter->tms[y].toHornerForm(hfOutputY, remainderY);


			// evaluate the images from all of the grids
			std::list<std::vector<Interval> >::const_iterator gIter = grids.begin();
			for(; gIter!=grids.end(); ++gIter)
			{
				Interval X;
				hfOutputX.intEval(X, *gIter);
				X += remainderX;

				Interval Y;
				hfOutputY.intEval(Y, *gIter);
				Y += remainderY;

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

		printf("\b\b\b\b");
		printf(BOLD_FONT "%%100\n" RESET_COLOR);
		fflush(stdout);
	}
}

bool ContinuousReachability::declareStateVar(const std::string & vName)
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

int ContinuousReachability::getIDForStateVar(const std::string & vName) const
{
	std::map<std::string,int>::const_iterator iter;
	if((iter = stateVarTab.find(vName)) == stateVarTab.end())
	{
		return -1;
	}

	return iter->second;
}

bool ContinuousReachability::getStateVarName(std::string & vName, int id) const
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



bool ContinuousReachability::declareTMVar(const std::string & vName)
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

int ContinuousReachability::getIDForTMVar(const std::string & vName) const
{
	std::map<std::string,int>::const_iterator iter;
	if((iter = tmVarTab.find(vName)) == tmVarTab.end())
	{
		return -1;
	}

	return iter->second;
}

bool ContinuousReachability::getTMVarName(std::string & vName, int id) const
{
	if(id >= 0 && id < tmVarNames.size())
	{
		vName = tmVarNames[id];
		return true;
	}
	else
	{
		return false;
	}
}




bool ContinuousReachability::declarePar(const std::string & pName, const Interval & range)
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

int ContinuousReachability::getIDForPar(const std::string & pName) const
{
	std::map<std::string,int>::const_iterator iter;
	if((iter = parTab.find(pName)) == parTab.end())
	{
		return -1;
	}

	return iter->second;
}

bool ContinuousReachability::getParName(std::string & pName, int id) const
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

bool ContinuousReachability::getRangeForPar(Interval & range, const std::string & pName) const
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

bool ContinuousReachability::declareTIPar(const std::string & pName)
{
	std::map<std::string,int>::const_iterator iter;

	if((iter = TI_Par_Tab.find(pName)) == TI_Par_Tab.end())
	{
		TI_Par_Tab[pName] = TI_Par_Names.size();
		TI_Par_Names.push_back(pName);
		return true;
	}
	else
	{
		return false;
	}
}

int ContinuousReachability::getIDForTIPar(const std::string & pName) const
{
	std::map<std::string,int>::const_iterator iter;
	if((iter = TI_Par_Tab.find(pName)) == TI_Par_Tab.end())
	{
		return -1;
	}

	return iter->second;
}

bool ContinuousReachability::declareTVPar(const std::string & pName)
{
	std::map<std::string,int>::const_iterator iter;

	if((iter = TV_Par_Tab.find(pName)) == TV_Par_Tab.end())
	{
		TV_Par_Tab[pName] = TV_Par_Names.size();
		TV_Par_Names.push_back(pName);
		return true;
	}
	else
	{
		return false;
	}
}

int ContinuousReachability::getIDForTVPar(const std::string & pName) const
{
	std::map<std::string,int>::const_iterator iter;
	if((iter = TV_Par_Tab.find(pName)) == TV_Par_Tab.end())
	{
		return -1;
	}

	return iter->second;
}









// reachable set of SDE at a time point

SDE_reachset::SDE_reachset()
{
}

SDE_reachset::SDE_reachset(const iMatrix2 & Phi_input, const iMatrix2 & Psi_input, const iMatrix2 & Omega_input)
{
	Phi = Phi_input;
	Psi = Psi_input;
	Omega = Omega_input;
}

SDE_reachset::SDE_reachset(const SDE_reachset & reachset)
{
	Phi = reachset.Phi;
	Psi = reachset.Psi;
	Omega = reachset.Omega;
}

SDE_reachset::~SDE_reachset()
{
}

void SDE_reachset::toDistribution(iMatrix & mean, iMatrix & covar, const iMatrix & initial_mean, const iMatrix & initial_covar, const iMatrix & control_input) const
{
	iMatrix2 mean2, covar2, initial_mean2, initial_covar2, control_input2;

	initial_mean.to_iMatrix2(initial_mean2);
	initial_covar.to_iMatrix2(initial_covar2);
	control_input.to_iMatrix2(control_input2);

	iMatrix2 transpose_Phi;
	Phi.transpose(transpose_Phi);

	mean2 = Phi*initial_mean2 + Psi*control_input2;
	covar2 = Phi*initial_covar2*transpose_Phi + Omega;

	mean2.to_iMatrix(mean);
	covar2.to_iMatrix(covar);
}

void SDE_reachset::toDistribution(iMatrix2 & mean, iMatrix2 & covar, const iMatrix2 & initial_mean, const iMatrix2 & initial_covar, const iMatrix2 & control_input) const
{
	iMatrix2 transpose_Phi;
	Phi.transpose(transpose_Phi);

	mean = Phi*initial_mean + Psi*control_input;
	covar = Phi*initial_covar*transpose_Phi + Omega;
}

void SDE_reachset::output(FILE *fp) const
{
	fprintf(fp, "Matrix Phi:\n");
	Phi.output(fp);
	fprintf(fp, "\nMatrix Psi:\n");
	Psi.output(fp);
	fprintf(fp, "\nMatrix Omega:\n");
	Omega.output(fp);
}

SDE_reachset & SDE_reachset::operator = (const SDE_reachset & reachset)
{
	if(this == &reachset)
		return *this;

	Phi = reachset.Phi;
	Psi = reachset.Psi;
	Omega = reachset.Omega;

	return *this;
}


// class of linear time-varying stochastic differential equations

LTV_SDE::LTV_SDE(const upMatrix & A_t_input, const upMatrix & B_t_input, const upMatrix & C_t_input)
{
	A_t = A_t_input;
	B_t = B_t_input;
	C_t = C_t_input;
}

LTV_SDE::~LTV_SDE()
{
}

void LTV_SDE::reach(SDE_reachset & result, const double step, const int N, const int order) const
{
	const int rangeDim = A_t.rows();
	const int control_num = B_t.cols();

	Interval intOne(1), intStepEnd(step);

	int maxOrder = A_t.degree();
	int maxOrder_B = B_t.degree();
	int maxOrder_C = C_t.degree();

	if(maxOrder < maxOrder_B)
	{
		maxOrder = maxOrder_B;
	}

	if(maxOrder < maxOrder_C)
	{
		maxOrder = maxOrder_C;
	}

	std::vector<Interval> step_exp_table, step_end_exp_table;
	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1+maxOrder)+1);

	iMatrix2 Phi_t_0(rangeDim), Psi_t_0(rangeDim, control_num), Omega_t_0(rangeDim, rangeDim);

	Interval int_t0(0);

	for(int i=1; ; ++i)
	{
		std::vector<Interval> t_t0_coefficients;
		t_t0_coefficients.push_back(int_t0);
		t_t0_coefficients.push_back(intOne);
		UnivariatePolynomial up_t_t0(t_t0_coefficients);

		iMatrix2 Phi_delta;
		iMatrix2 Psi_delta;
		iMatrix2 Omega_delta;

		compute_one_step_trans_LTV_SDE(Phi_delta, Psi_delta, Omega_delta, A_t, B_t, C_t, step_exp_table, step_end_exp_table, up_t_t0, order);

		Phi_t_0 = Phi_delta * Phi_t_0;
		Psi_t_0 = Phi_delta * Psi_t_0 + Psi_delta;

		iMatrix2 Phi_delta_transpose;
		Phi_delta.transpose(Phi_delta_transpose);
		Omega_t_0 = Phi_delta * Omega_t_0 * Phi_delta_transpose + Omega_delta;

		if(i == N)
		{
			result.Phi = Phi_t_0;
			result.Psi = Psi_t_0;
			result.Omega = Omega_t_0;
			break;
		}

		int_t0 += intStepEnd;

//		printf("time = %f,\t", t0);
//		printf("step = %f,\t", step);
//		printf("order = %d\n", order);
	}
}

void LTV_SDE::reach(std::vector<SDE_reachset> & result, const double delta1, const int N, const double delta2, const int M, const int order) const
{
	// compute the reachable set at t = t0
	const int rangeDim = A_t.rows();
	const int control_num = B_t.cols();

	Interval intOne(1), int_delta1(delta1);

	int maxOrder = A_t.degree();
	int maxOrder_B = B_t.degree();
	int maxOrder_C = C_t.degree();

	if(maxOrder < maxOrder_B)
	{
		maxOrder = maxOrder_B;
	}

	if(maxOrder < maxOrder_C)
	{
		maxOrder = maxOrder_C;
	}

	std::vector<Interval> step_exp_table, step_end_exp_table;
	construct_step_exp_table(step_exp_table, step_end_exp_table, delta1, 2*(order+1+maxOrder)+1);

	iMatrix2 Phi_t_0(rangeDim), Psi_t_0(rangeDim, control_num), Omega_t_0(rangeDim, rangeDim);

	Interval int_t0(0);

	for(int i=1; i<=N; ++i)
	{
		std::vector<Interval> t_t0_coefficients;
		t_t0_coefficients.push_back(int_t0);
		t_t0_coefficients.push_back(intOne);
		UnivariatePolynomial up_t_t0(t_t0_coefficients);

		iMatrix2 Phi_delta;
		iMatrix2 Psi_delta;
		iMatrix2 Omega_delta;

		compute_one_step_trans_LTV_SDE(Phi_delta, Psi_delta, Omega_delta, A_t, B_t, C_t, step_exp_table, step_end_exp_table, up_t_t0, order);

		Phi_t_0 = Phi_delta * Phi_t_0;
		Psi_t_0 = Phi_delta * Psi_t_0 + Psi_delta;

		iMatrix2 Phi_delta_transpose;
		Phi_delta.transpose(Phi_delta_transpose);
		Omega_t_0 = Phi_delta * Omega_t_0 * Phi_delta_transpose + Omega_delta;

		int_t0 += int_delta1;
	}

	// compute the reachable set in the time interval of [delta1*N + delta2*i , delta1*N + delta2*(i+1)] for i=0,...,M-1
	construct_step_exp_table(step_exp_table, step_end_exp_table, delta2, 2*(order+1+maxOrder)+1);

	Interval int_delta2(delta2);
	result.clear();

	for(int i=1; i<M; ++i)
	{
		std::vector<Interval> t_t0_coefficients;
		t_t0_coefficients.push_back(int_t0);
		t_t0_coefficients.push_back(intOne);
		UnivariatePolynomial up_t_t0(t_t0_coefficients);

		iMatrix2 Phi_delta;
		iMatrix2 Psi_delta;
		iMatrix2 Omega_delta;

		iMatrix2 Phi_0_delta;
		iMatrix2 Psi_0_delta;
		iMatrix2 Omega_0_delta;

		compute_one_step_trans_LTV_SDE(Phi_delta, Psi_delta, Omega_delta, Phi_0_delta, Psi_0_delta, Omega_0_delta, A_t, B_t, C_t, step_exp_table, step_end_exp_table, up_t_t0, order);

		Phi_t_0 = Phi_delta * Phi_t_0;
		Psi_t_0 = Phi_delta * Psi_t_0 + Psi_delta;

		iMatrix2 Phi_delta_transpose;
		Phi_delta.transpose(Phi_delta_transpose);
		Omega_t_0 = Phi_delta * Omega_t_0 * Phi_delta_transpose + Omega_delta;

		SDE_reachset reachset;
		iMatrix2 Phi_0_delta_transpose;
		Phi_0_delta.transpose(Phi_0_delta_transpose);

		reachset.Phi = Phi_0_delta * Phi_t_0;
		reachset.Psi = Phi_0_delta * Psi_t_0 + Psi_delta;
		reachset.Omega = Phi_0_delta * Omega_t_0 * Phi_0_delta_transpose + Omega_0_delta;

		result.push_back(reachset);

		int_t0 += int_delta2;
	}
}




// class LTI_Flowpipe

LTI_Flowpipe::LTI_Flowpipe()
{
}

LTI_Flowpipe::LTI_Flowpipe(const LTI_Flowpipe & flowpipe)
{
	init_Phi = flowpipe.init_Phi;
	init_Psi = flowpipe.init_Psi;

	trans_Phi = flowpipe.trans_Phi;
	trans_Psi = flowpipe.trans_Psi;
}

LTI_Flowpipe::~LTI_Flowpipe()
{
}

void LTI_Flowpipe::intEval(std::vector<Interval> & result, const std::vector<int> & varIDs, const std::vector<Interval> & X0, const Continuous_Reachability_Setting & crs)
{
	int rangeDim = X0.size();
	iMatrix range_X0(X0);

	iMatrix im_Psi(rangeDim, 1);

	if(init_Psi.rows() > 0)
	{
		trans_Psi.intEval(im_Psi, crs.step_exp_table);
	}

	for(int i=0; i<varIDs.size(); ++i)
	{
		upMatrix up_temp(1, rangeDim);

		for(int j=0; j<rangeDim; ++j)
		{
			up_temp[0][j] = trans_Phi[varIDs[i]][j];
		}

		iMatrix im_temp(1, rangeDim);
		up_temp.intEval(im_temp, crs.step_exp_table);

		iMatrix range = im_temp * init_Phi * range_X0;

		if(init_Psi.rows() > 0)
		{
			range += im_temp * init_Psi;
			range[0][0] += im_Psi[varIDs[i]][0];
		}

		result.push_back(range[0][0]);
	}
}

LTI_Flowpipe & LTI_Flowpipe::operator = (const LTI_Flowpipe & flowpipe)
{
	if(this == &flowpipe)
		return *this;

	init_Phi = flowpipe.init_Phi;
	init_Psi = flowpipe.init_Psi;

	trans_Phi = flowpipe.trans_Phi;
	trans_Psi = flowpipe.trans_Psi;

	return *this;
}





// class LTI_ODE

LTI_ODE::LTI_ODE(iMatrix & A_input, iMatrix & B_input)
{
	A = A_input;
	B = B_input;

	if(B.rows() == 0)
	{
		bAuto = true;
	}
	else
	{
		bAuto = false;
	}

	int n = A.rows();
	Real zero;

	bMatrix conMatrix(n, n), adjMatrix(n, n);

	for(int i=0; i<n; ++i)
	{
		for(int j=0; j<n; ++j)
		{
			if(A_input[i][j] != zero)
			{
				adjMatrix[i][j] = true;
			}
		}
	}

	check_connectivities(conMatrix, adjMatrix);
	connectivity = conMatrix;
}
/*
LTI_ODE::LTI_ODE(iMatrix & A_input, iMatrix & B_input, iMatrix & C_input, iMatrix & constant_input, const std::vector<Interval> & dist_range_input)
{
	A = A_input;
	B = B_input;
	C = C_input;
	im_tv_range = dist_range_input;
	constant = constant_input;

	if(B.rows() == 0)
	{
		bAuto = true;
	}
	else
	{
		bAuto = false;
	}

	int n = A.rows();
	Real zero;

	bMatrix conMatrix(n, n), adjMatrix(n, n);

	for(int i=0; i<n; ++i)
	{
		for(int j=0; j<n; ++j)
		{
			if(A_input[i][j] != zero)
			{
				adjMatrix[i][j] = true;
			}
		}
	}

	check_connectivities(conMatrix, adjMatrix);
	connectivity = conMatrix;
}
*/
LTI_ODE::~LTI_ODE()
{
}
/*
void LTI_ODE::one_step_trans(iMatrix & Phi, iMatrix & Psi, iMatrix & trans_constant, Zonotope & dist, const double step, const int order)
{
	Real rStep(step);
	Interval intZero, intStep(0, step), intUnit(-1,1);

	std::vector<Interval> step_exp_table, step_end_exp_table;
	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*(order+1)+1);

	int rangeDim = A.rows(), numTIPar = B.cols(), numTVPar = C.cols();


	// identity matrix
	iMatrix im_identity(rangeDim);

	// 2. Compute the first flowpipe
	// compute A^n for 1 <= n <= k
	std::vector<iMatrix> A_exp_table;
	compute_int_mat_pow(A_exp_table, A, order + 1);

	// compute the expansion for exp(At)
	upMatrix expansion_exp_A_t_k = im_identity;

	for(int i=1; i<=order; ++i)
	{
		upMatrix A_t_i = A_exp_table[i];
		A_t_i.times_x(i);
		A_t_i *= factorial_rec[i];

		expansion_exp_A_t_k += A_t_i;
	}

	upMatrix up_Phi_0 = expansion_exp_A_t_k;

	// compute a remainder for exp(A*delta)
	Real factor_k_plus_1;
	factorial_rec[order+1].sup(factor_k_plus_1);

	Real step_pow_k_plus_1(step);
	step_pow_k_plus_1.pow_assign_RNDU(order + 1);

	factor_k_plus_1.mul_assign_RNDU(step_pow_k_plus_1);

	Real bound_exp_A_delta;
	A.max_norm(bound_exp_A_delta);
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
			if(connectivity[i][j])
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
			if(connectivity[i][j])
			{
				im_Phi_0_rem[i][j] = im_rem[i][j];
			}
		}
	}

	up_Phi_0 += im_Phi_0_rem;
	up_Phi_0.intEval(Phi, step_end_exp_table);


	iMatrix im_TI_zero(rangeDim, numTIPar), im_TV_zero(rangeDim, numTVPar);

	iMatrix im_trunc_step_end;
	upMatrix up_Psi_0 = up_Phi_0 * B;
	up_Psi_0.integral();
	up_Psi_0.intEval(Psi, step_end_exp_table);

	upMatrix up_tv = up_Phi_0 * C;
	iMatrix tv_part;
	up_tv.intEval(tv_part, step_exp_table);
	tv_part *= im_tv_range;
	tv_part *= step_exp_table[1];

	// translate it to a zonotope
	dist = tv_part;

	upMatrix up_constant = up_Phi_0 * constant;
	up_constant.integral();
	up_constant.intEval(trans_constant, step_end_exp_table);
}
*/

void LTI_ODE::reach(std::list<LTI_Flowpipe> & flowpipes, iMatrix & Phi_end, std::vector<std::vector<iMatrix> > & Psi_end, std::vector<iMatrix> & uncertainties, const Continuous_Reachability_Setting & crs, const int N)
{
	Real rStep(crs.step);
	Interval intZero, intUnit(-1,1);

	int rangeDim = A.rows();

	// identity matrix
	iMatrix im_identity(rangeDim);

	// 2. Compute the first flowpipe
	// compute A^n for 1 <= n <= k
	std::vector<iMatrix> A_exp_table;
	compute_int_mat_pow(A_exp_table, A, crs.order + 1);

	// compute the expansion for exp(At)
	upMatrix expansion_exp_A_t_k = im_identity;

	for(int i=1; i<=crs.order; ++i)
	{
		upMatrix A_t_i = A_exp_table[i];
		A_t_i.times_x(i);
		A_t_i *= factorial_rec[i];

		expansion_exp_A_t_k += A_t_i;
	}

	upMatrix up_Phi_0 = expansion_exp_A_t_k;

	// compute a remainder for exp(A*delta)
	Real factor_k_plus_1;
	factorial_rec[crs.order+1].sup(factor_k_plus_1);

	Real step_pow_k_plus_1(crs.step);
	step_pow_k_plus_1.pow_assign_RNDU(crs.order + 1);

	factor_k_plus_1.mul_assign_RNDU(step_pow_k_plus_1);

	Real bound_exp_A_delta;
	A.max_norm(bound_exp_A_delta);
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
			if(connectivity[i][j])
			{
				im_rem[i][j] = intErr;
			}
		}
	}

	im_rem = A_exp_table[crs.order+1] * im_rem;

	iMatrix im_Phi_0_rem(rangeDim, rangeDim);

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=0; j<rangeDim; ++j)
		{
			if(connectivity[i][j])
			{
				im_Phi_0_rem[i][j] = im_rem[i][j];
			}
		}
	}

	up_Phi_0 += im_Phi_0_rem;

	iMatrix im_B_zero(rangeDim, 1);

	LTI_Flowpipe flowpipe;
	flowpipe.init_Phi = Phi_end;

	iMatrix im_Phi;
	up_Phi_0.intEval(im_Phi, crs.step_end_exp_table);


	upMatrix up_Psi_0;
	iMatrix im_Psi;

	if(!bAuto)
	{
		up_Psi_0 = up_Phi_0 * B;
		up_Psi_0.integral();

		iMatrix im_trunc_step, im_trunc_step_end;
		up_Psi_0.ctrunc(im_trunc_step, im_trunc_step_end, crs.order, crs.step_exp_table, crs.step_end_exp_table);

		up_Psi_0.intEval(im_Psi, crs.step_end_exp_table);
		im_Psi += im_trunc_step_end;
		up_Psi_0 += im_trunc_step;

		flowpipe.init_Psi = im_B_zero;

		for(int k=0; k<Psi_end.size(); ++k)
		{
			for(int m=0; m<Psi_end[k].size(); ++m)
			{
				flowpipe.init_Psi += Psi_end[k][m] * uncertainties[k];
			}
		}

		flowpipe.trans_Psi = up_Psi_0;

		uncertainties.push_back(im_Psi);
	}
	else
	{
		uncertainties.push_back(im_B_zero);
	}

	iMatrix im_trans_Phi;
	up_Phi_0.intEval(im_trans_Phi, crs.step_exp_table);

	flowpipe.trans_Phi = up_Phi_0;

	flowpipes.push_back(flowpipe);


	iMatrix im_global_Psi(rangeDim, 1);

	std::vector<iMatrix> exp_A_delta_exp_table;

	exp_A_delta_exp_table.push_back(im_identity);
	exp_A_delta_exp_table.push_back(im_Phi);

	for(int i=1; i<N; ++i)
	{
		LTI_Flowpipe newFlowpipe;

		newFlowpipe.trans_Phi = up_Phi_0;

		if(i >= exp_A_delta_exp_table.size())
		{
			if(i%2 == 0)
			{
				int j = i/2;
				iMatrix im_temp = exp_A_delta_exp_table[j] * exp_A_delta_exp_table[j];
				exp_A_delta_exp_table.push_back(im_temp);
			}
			else
			{
				int j = i/2;
				iMatrix im_temp = exp_A_delta_exp_table[j] * exp_A_delta_exp_table[j+1];
				exp_A_delta_exp_table.push_back(im_temp);
			}
		}

		newFlowpipe.init_Phi = exp_A_delta_exp_table[i] * Phi_end;

		iMatrix im_temp(rangeDim, 1);

		for(int k=0; k<Psi_end.size(); ++k)
		{
			for(int m=0; m<Psi_end[k].size(); ++m)
			{
				im_temp += (exp_A_delta_exp_table[i-1] * Psi_end[k][m]) * uncertainties[k];
			}
		}

		if(!bAuto)
		{
			im_global_Psi += exp_A_delta_exp_table[i-1] * im_Psi;
		}

		newFlowpipe.trans_Psi = up_Psi_0;
		newFlowpipe.init_Psi = im_global_Psi + im_temp;

		flowpipes.push_back(newFlowpipe);
	}

	iMatrix local_Phi_end = im_Phi * exp_A_delta_exp_table[N-1];

	for(int i=0; i<Psi_end.size(); ++i)
	{
		for(int j=0; j<Psi_end[i].size(); ++j)
		{
			Psi_end[i][j] = local_Phi_end * Psi_end[i][j];
		}
	}

	Phi_end = local_Phi_end * Phi_end;

	if(!bAuto)
	{
		Psi_end.push_back(exp_A_delta_exp_table);
	}
	else
	{
		std::vector<iMatrix> empty;
		Psi_end.push_back(empty);
	}
}










Continuous_Reachability_Setting::Continuous_Reachability_Setting()
{
	step				= 0;
	time				= 0;
	order				= 0;
	bPrint				= true;

	Interval cutoff(-1e-12,1e-12);
	cutoff_threshold = cutoff;
}

Continuous_Reachability_Setting::~Continuous_Reachability_Setting()
{
	estimation.clear();
	step_exp_table.clear();
	step_end_exp_table.clear();
}

Continuous_Reachability_Setting::Continuous_Reachability_Setting(const Continuous_Reachability_Setting & rs)
{
	step				= rs.step;
	time				= rs.time;
	order				= rs.order;
	bPrint				= rs.bPrint;
	estimation			= rs.estimation;
	cutoff_threshold	= rs.cutoff_threshold;
	step_exp_table		= rs.step_exp_table;
	step_end_exp_table	= rs.step_end_exp_table;
}

Continuous_Reachability_Setting & Continuous_Reachability_Setting::operator = (const Continuous_Reachability_Setting & rs)
{
	if(this == &rs)
		return *this;

	step				= rs.step;
	time				= rs.time;
	bPrint				= rs.bPrint;
	estimation			= rs.estimation;
	cutoff_threshold	= rs.cutoff_threshold;
	step_exp_table		= rs.step_exp_table;
	step_end_exp_table	= rs.step_end_exp_table;

	return *this;
}

bool Continuous_Reachability_Setting::setTime(const double t)
{
	if(t <= 0)
	{
		printf("Time horizon must be a positive number.\n");
		return false;
	}
	else
	{
		time = t;
		return true;
	}
}

bool Continuous_Reachability_Setting::setFixedStepsize(const double delta)
{
	if(delta > 0)
	{
		step = delta;
		return true;
	}
	else
	{
		printf("A time step-size should be larger than 0.\n");
		return false;
	}
}

bool Continuous_Reachability_Setting::setFixedOrder(const int k)
{
	if(k > 0)
	{
		order = k;
		return true;
	}
	else
	{
		printf("Orders should be larger than zero.\n");
		return false;
	}
}

bool Continuous_Reachability_Setting::setRemainderEstimation(const std::vector<Interval> & intervals)
{
	for(int i=0; i<intervals.size(); ++i)
	{
		if(!intervals[i].valid())
		{
			printf("Remainder estimation is not valid.\n");
			return false;
		}
		else
		{
			estimation.push_back(intervals[i]);
		}
	}

	return true;
}

bool Continuous_Reachability_Setting::setCutoff(const Interval & cutoff)
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

bool Continuous_Reachability_Setting::setPrecision(const int prec)
{
	if(prec >= 53)
	{
		intervalNumPrecision = prec;
		return true;
	}
	else
	{
		printf("Precision should be large than 53.\n");
		return false;
	}
}

void Continuous_Reachability_Setting::printon()
{
	bPrint = true;
}

void Continuous_Reachability_Setting::printoff()
{
	bPrint = false;
}

void Continuous_Reachability_Setting::prepareForReachability()
{
	compute_factorial_rec(order+5);
	compute_power_4(order+5);
	compute_double_factorial(2*order+10);

	construct_step_exp_table(step_exp_table, step_end_exp_table, step, 2*order);
}







Symbolic_Remainder::Symbolic_Remainder(const Flowpipe & initial_set)
{
	int rangeDim = initial_set.tmvPre.tms.size();

	Interval intOne(1);

	for(int i=0; i<rangeDim; ++i)
	{
		scalars.push_back(intOne);
	}

	initial_set.tmv.Expansion(initial_set_poly);
}

Symbolic_Remainder::Symbolic_Remainder(const Symbolic_Remainder & symb_rem)
{
	J = symb_rem.J;
	Phi_L = symb_rem.Phi_L;
	scalars = symb_rem.scalars;
	initial_set_poly = symb_rem.initial_set_poly;
}

Symbolic_Remainder::~Symbolic_Remainder()
{
	J.clear();
	Phi_L.clear();
	scalars.clear();
	initial_set_poly.clear();
}

Symbolic_Remainder & Symbolic_Remainder::operator = (const Symbolic_Remainder & symb_rem)
{
	if(this == &symb_rem)
		return *this;

	J = symb_rem.J;
	Phi_L = symb_rem.Phi_L;
	scalars = symb_rem.scalars;
	initial_set_poly = symb_rem.initial_set_poly;

	return *this;
}








Polynomial_ODE::Polynomial_ODE(const Variables & vars)
{
	variables = vars;
	Polynomial polynomial;
	HornerForm hf;
	Interval I;

	for(int i=1; i<vars.varNames.size(); ++i)
	{
		// the derivatives of all variables are zero
		derivatives.push_back(polynomial);
		derivatives_hf.push_back(hf);
		derivatives_hf_centered.push_back(hf);
		remainders.push_back(I);
		constant.push_back(false);
	}
}

Polynomial_ODE::Polynomial_ODE(const Polynomial_ODE & ode)
{
	variables = ode.variables;
	derivatives = ode.derivatives;
	derivatives_hf = ode.derivatives_hf;
	remainders = ode.remainders;
	constant = ode.constant;
}

Polynomial_ODE::~Polynomial_ODE()
{
	variables.clear();
	derivatives.clear();
	derivatives_hf.clear();
	derivatives_hf_centered.clear();
	remainders.clear();
	constant.clear();
}

Polynomial_ODE & Polynomial_ODE::operator = (const Polynomial_ODE & ode)
{
	if(this == &ode)
		return *this;

	variables = ode.variables;
	derivatives = ode.derivatives;
	derivatives_hf = ode.derivatives_hf;
	derivatives_hf_centered = ode.derivatives_hf_centered;
	remainders = ode.remainders;
	constant = ode.constant;

	return *this;
}

bool Polynomial_ODE::assignDerivative(const std::string & varName, const Polynomial & polynomial)
{
	int id = variables.getIDForVar(varName);

	if(id == -1)
	{
		return false;
	}
	else
	{
		derivatives[id-1] = polynomial;

		HornerForm hf;
		polynomial.toHornerForm(hf);
		derivatives_hf[id-1] = hf;

		Polynomial polynomial_centered = polynomial;
		polynomial_centered.center();

		polynomial_centered.toHornerForm(hf);
		derivatives_hf_centered[id-1] = hf;

		if(polynomial.degree() == 0)
		{
			constant[id-1] = true;
		}

		return true;
	}
}
/*
bool Polynomial_ODE::assignDerivative(const std::string & varName, const TaylorModel & taylormodel)
{
	int id = variables.getIDForVar(varName);

	if(id == -1)
	{
		return false;
	}
	else
	{
		derivatives[id-1] = taylormodel.expansion;
		remainders[id-1] = taylormodel.remainder;

		flowstar::HornerForm hf;
		taylormodel.expansion.toHornerForm(hf);
		derivatives_hf[id-1] = hf;

		return true;
	}
}
*/
void Polynomial_ODE::output(FILE *fp) const
{
	for(int i=1; i<variables.size(); ++i)
	{
		std::string strPoly;
		derivatives[i-1].toString(strPoly, variables.varNames);
		fprintf(fp, "%s' = %s", variables.varNames[i].c_str(), strPoly.c_str());
		fprintf(fp, " + ");
		remainders[i-1].dump(fp);
		fprintf(fp, "\n");
	}

	fprintf(fp, "\n");
}

void Polynomial_ODE::doubleEval(std::vector<double> & derivative, const std::vector<double> & state) const
{
	flowstar::Interval intZero;
	std::vector<flowstar::Interval> domain;
	domain.push_back(intZero);

	for(int i=0; i<state.size(); ++i)
	{
		domain.push_back(state[i]);
	}

	derivative.clear();
	for(int i=0; i<derivatives.size(); ++i)
	{
		flowstar::Interval deriv;
		derivatives[i].intEval(deriv, domain);
		derivative.push_back(deriv.midpoint());
	}
}

bool Polynomial_ODE::reach_symbolic_remainder(std::list<Flowpipe> & flowpipes, Flowpipe & fp_last, Symbolic_Remainder & symb_rem, Continuous_Reachability_Setting & crs, const Flowpipe & initial_set, const int N, const int M) const
{
	int rangeDim = derivatives.size();

	Flowpipe newFlowpipe, currentFlowpipe = initial_set;

	Interval intOne(1);
	std::vector<Interval> initial_scalars;

	for(int i=0; i<rangeDim; ++i)
	{
		initial_scalars.push_back(intOne);
	}

	for(int i=0; i<N; ++i)
	{
		int res = currentFlowpipe.advance_picard_symbolic_remainder(newFlowpipe, derivatives_hf, derivatives_hf_centered, crs.step_exp_table, crs.step_end_exp_table, crs.order, crs.estimation, crs.cutoff_threshold, symb_rem.initial_set_poly, symb_rem.scalars, symb_rem.J, symb_rem.Phi_L, constant);

		if(res == 1)
		{
			flowpipes.push_back(newFlowpipe);

			currentFlowpipe = newFlowpipe;

			if(symb_rem.J.size() >= M)
			{
				currentFlowpipe.tmv.Expansion(symb_rem.initial_set_poly);
				symb_rem.scalars = initial_scalars;
				symb_rem.J.clear();
				symb_rem.Phi_L.clear();
			}

//			printf("Step %d.\n", i);
		}
		else
		{
			return false;
		}
	}

	// compute the flowpipe at the end of the time
	Interval intZero;
	fp_last = flowpipes.back();

	TaylorModelVec tmvTemp;
	fp_last.tmvPre.evaluate_t(tmvTemp, crs.step_end_exp_table);
	fp_last.tmvPre = tmvTemp;
	fp_last.domain[0] = intZero;

	return true;
}


ODE::ODE(const Variables & vars)
{
	variables = vars;

	Expression_AST zero_derivative;
	Interval I;

	for(int i=1; i<variables.size(); ++i)
	{
		derivatives.push_back(zero_derivative);
		derivative_centers.push_back(zero_derivative);
		constant.push_back(false);
		constant_part.push_back(I);
	}
}

ODE::ODE(const ODE & ode)
{
	variables = ode.variables;
	derivatives = ode.derivatives;
	derivative_centers = ode.derivative_centers;
	constant = ode.constant;
	constant_part = ode.constant_part;
}

ODE::~ODE()
{
	derivatives.clear();
	derivative_centers.clear();
	constant.clear();
	constant_part.clear();
}

ODE & ODE::operator = (const ODE & ode)
{
	if(this == &ode)
		return *this;

	variables = ode.variables;
	derivatives = ode.derivatives;
	derivative_centers = ode.derivative_centers;
	constant = ode.constant;
	constant_part = ode.constant_part;

	return *this;
}

bool ODE::assignDerivative(const std::string & varName, const Expression_AST & derivative)
{
	int id = variables.getIDForVar(varName);

	if(id == -1)
	{
		return false;
	}
	else
	{
		derivatives[id-1] = derivative;

		Expression_AST tmp = derivative;
		// tmp.center();

		derivative_centers[id-1] = tmp;

		Interval I;
		if(derivative.isConstant(I))
		{
			constant[id-1] = true;
			constant_part[id-1] = I;
		}

		return true;
	}
}

void ODE::output(FILE *fp, const Taylor_Model_Computation_Setting & setting) const
{
	for(int i=1; i<variables.size(); ++i)
	{
		fprintf(fp, "%s' = ", variables.varNames[i].c_str());
		derivatives[i-1].output(fp, setting);
		fprintf(fp, "\n");
	}

	fprintf(fp, "\n");
}

bool ODE::reach_interval_remainder(std::list<Flowpipe> & flowpipes, Flowpipe & fp_last, Continuous_Reachability_Setting & crs, const Flowpipe & initial_set, const int N) const
{
	int rangeDim = derivatives.size();

	Flowpipe newFlowpipe, currentFlowpipe = initial_set;

	for(int i=0; i<N; ++i)
	{
		int res = currentFlowpipe.advance_picard(newFlowpipe, derivatives, derivative_centers, crs.step_exp_table, crs.step_end_exp_table, crs.order, crs.estimation, crs.cutoff_threshold, constant, constant_part);

		if(res == 1)
		{
			flowpipes.push_back(newFlowpipe);

			currentFlowpipe = newFlowpipe;

//			printf("Step %d.\n", i);
		}
		else
		{
			return false;
		}
	}

	// compute the flowpipe at the end of the time
	Interval intZero;
	fp_last = flowpipes.back();

	TaylorModelVec tmvTemp;
	fp_last.tmvPre.evaluate_t(tmvTemp, crs.step_end_exp_table);
	fp_last.tmvPre = tmvTemp;
	fp_last.domain[0] = intZero;

	return true;
}

bool ODE::reach_symbolic_remainder(std::list<Flowpipe> & flowpipes, Flowpipe & fp_last, Symbolic_Remainder & symb_rem, Continuous_Reachability_Setting & crs, const Flowpipe & initial_set, const int N, const int M) const
{
	int rangeDim = derivatives.size();

	Flowpipe newFlowpipe, currentFlowpipe = initial_set;

	Interval intOne(1);
	std::vector<Interval> initial_scalars;

	for(int i=0; i<rangeDim; ++i)
	{
		initial_scalars.push_back(intOne);
	}

	for(int i=0; i<N; ++i)
	{
		int res = currentFlowpipe.advance_picard_symbolic_remainder(newFlowpipe, derivatives, derivative_centers, crs.step_exp_table, crs.step_end_exp_table, crs.order, crs.estimation, crs.cutoff_threshold, symb_rem.initial_set_poly, symb_rem.scalars, symb_rem.J, symb_rem.Phi_L, constant, constant_part);

		if(res == 1)
		{
			flowpipes.push_back(newFlowpipe);

			currentFlowpipe = newFlowpipe;

			if(symb_rem.J.size() >= M)
			{
				currentFlowpipe.tmv.Expansion(symb_rem.initial_set_poly);
				symb_rem.scalars = initial_scalars;
				symb_rem.J.clear();
				symb_rem.Phi_L.clear();
			}

//			printf("Step %d.\n", i);
		}
		else
		{
			return false;
		}
	}

	// compute the flowpipe at the end of the time
	Interval intZero;
	fp_last = flowpipes.back();

	TaylorModelVec tmvTemp;
	fp_last.tmvPre.evaluate_t(tmvTemp, crs.step_end_exp_table);
	fp_last.tmvPre = tmvTemp;
	fp_last.domain[0] = intZero;

	return true;
}


namespace flowstar
{
/*
bool reach_interval_remainder_using_regression(std::list<Flowpipe> & flowpipes, Flowpipe & fp_last, DynamicSys & system, Continuous_Reachability_Setting & crs, const Flowpipe & initial_set, const int N)
{
	int rangeDim = initial_set.tmvPre.tms.size();

	Flowpipe newFlowpipe, currentFlowpipe = initial_set;

	std::vector<std::vector<int> > table_of_monomials;

	for(int i=0; i<N; ++i)
	{
		int res = currentFlowpipe.advance_regression_picard(newFlowpipe, system, crs.step_exp_table, crs.step_end_exp_table, table_of_monomials, crs.order, crs.estimation, crs.cutoff_threshold, *(system.p_constant), *(system.p_constant_part));

		if(res == 1)
		{
			flowpipes.push_back(newFlowpipe);
//			printf("domain size = %d\n", (int)newFlowpipe.domain.size());
			currentFlowpipe = newFlowpipe;

			printf("Step %d.\n", i);
		}
		else
		{
			return false;
		}
	}

	// compute the flowpipe at the end of the time
	Interval intZero;
	fp_last = flowpipes.back();

	TaylorModelVec tmvTemp;
	fp_last.tmvPre.evaluate_t(tmvTemp, crs.step_end_exp_table);
	fp_last.tmvPre = tmvTemp;
	fp_last.domain[0] = intZero;

	return true;
}
*/






std::vector<std::string> domainVarNames;


void hybrid_LTI_reach(std::list<LTI_Flowpipe> & flowpipes, std::vector<LTI_ODE> & odes, const std::vector<int> & steps, Continuous_Reachability_Setting & crs)
{
	flowpipes.clear();

	if(odes.size() == 0)
		return;

	int rangeDim = odes[0].A.rows();

	std::vector<std::vector<iMatrix> > Psi_end;
	iMatrix Phi_end(rangeDim);
	std::vector<iMatrix> uncertainties;

	for(int i=0; i<odes.size(); ++i)
	{
		odes[i].reach(flowpipes, Phi_end, Psi_end, uncertainties, crs, steps[i]);
	}
}


void plot_2D_interval_MATLAB(FILE *fp, const std::string & x, const std::string & y, const Variables & vars, const std::list<Flowpipe> & flowpipes)
{
	std::list<Flowpipe>::const_iterator fpIter = flowpipes.begin();

	int x_id = vars.getIDForVar(x);

	if(x_id == -1)
	{
		printf("The variable %s is not defined.\n", x.c_str());
		return;
	}

	int y_id = vars.getIDForVar(y);

	if(y_id == -1)
	{
		printf("The variable %s is not defined.\n", y.c_str());
		return;
	}

	std::vector<int> varIDs;
	varIDs.push_back(x_id - 1);
	varIDs.push_back(y_id - 1);

	for(; fpIter != flowpipes.end() ; ++fpIter)
	{
		std::vector<Interval> tmvRange;
		fpIter->tmv.intEval(tmvRange, fpIter->domain);

		tmvRange.insert(tmvRange.begin(), fpIter->domain[0]);

		std::vector<Interval> fpRange;
		fpIter->tmvPre.intEval(fpRange, tmvRange, varIDs);

		Interval X = fpRange[0], Y = fpRange[1];

		fprintf(fp,"plot( [%e,%e,%e,%e,%e] , [%e,%e,%e,%e,%e] , 'color' , 'b');\nhold on;\nclear;\n",
				X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
	}
}

void plot_2D_interval_MATLAB(FILE *fp, const int x, const int y, std::list<LTI_Flowpipe> & flowpipes, const std::vector<Interval> & X0, const Continuous_Reachability_Setting & crs)
{
	std::vector<int> varIDs;
	varIDs.push_back(x);
	varIDs.push_back(y);

	std::list<LTI_Flowpipe>::iterator iter = flowpipes.begin();

	for(; iter != flowpipes.end(); ++iter)
	{
		std::vector<Interval> range;
		iter->intEval(range, varIDs, X0, crs);

		Interval X = range[0];
		Interval Y = range[1];

		fprintf(fp,"plot( [%e,%e,%e,%e,%e] , [%e,%e,%e,%e,%e] , 'color' , 'b');\nhold on;\nclear;\n",
				X.inf(), X.sup(), X.sup(), X.inf(), X.inf(), Y.inf(), Y.inf(), Y.sup(), Y.sup(), Y.inf());
	}
}

void computeTaylorExpansion(TaylorModelVec & result, const TaylorModelVec & first_order_deriv, const TaylorModelVec & ode, const int order, const Interval & cutoff_threshold)
{
	std::vector<Interval> intVecZero;
	Interval intZero, intOne(1,1);

	intVecZero.push_back(intOne);
	intVecZero.push_back(intZero);

	// we compute the Taylor expansion (without the 0-order term)
	TaylorModelVec taylorExpansion;
	first_order_deriv.evaluate_t(taylorExpansion, intVecZero);
//	taylorExpansion.nctrunc(order - 1);
	taylorExpansion.mul_assign(0, 1);

	TaylorModelVec tmvLieDeriv_n = first_order_deriv;

	for(int i=2; i<=order; ++i)
	{
		TaylorModelVec tmvTemp;
		tmvLieDeriv_n.LieDerivative_no_remainder(tmvTemp, ode, order - i, cutoff_threshold);

		TaylorModelVec tmvTemp2;
		tmvTemp.evaluate_t(tmvTemp2, intVecZero);
		tmvTemp2.mul_assign(factorial_rec[i]);
		tmvTemp2.mul_assign(0,i);			// multiplied by t^i
//		tmvTemp2.nctrunc(order);

		taylorExpansion.add_assign(tmvTemp2);

		tmvLieDeriv_n = tmvTemp;
	}

	taylorExpansion.cutoff(cutoff_threshold);

	result = taylorExpansion;
}

void computeTaylorExpansion(TaylorModelVec & result, const TaylorModelVec & first_order_deriv, const TaylorModelVec & ode, const std::vector<int> & orders, const Interval & cutoff_threshold)
{
	int rangeDim = ode.tms.size();
	std::vector<Interval> intVecZero;
	Interval intZero, intOne(1,1);

	intVecZero.push_back(intOne);
	intVecZero.push_back(intZero);

	// we compute the Taylor expansion (without the 0-order term)
	TaylorModelVec taylorExpansion;
	first_order_deriv.evaluate_t(taylorExpansion, intVecZero);

	taylorExpansion.mul_assign(0, 1);

	TaylorModelVec tmvLieDeriv_n = first_order_deriv;

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=2; j<=orders[i]; ++j)
		{
			TaylorModel tmTemp;
			tmvLieDeriv_n.tms[i].LieDerivative_no_remainder(tmTemp, ode, orders[i] - j, cutoff_threshold);

			TaylorModel tmTemp2;
			tmTemp.evaluate_t(tmTemp2, intVecZero);
			tmTemp2.mul_assign(factorial_rec[j]);
			tmTemp2.mul_assign(0,j);

			taylorExpansion.tms[i].add_assign(tmTemp2);

			tmvLieDeriv_n.tms[i] = tmTemp;
		}
	}

	taylorExpansion.cutoff(cutoff_threshold);

	result = taylorExpansion;
}

void construct_step_exp_table(std::vector<Interval> & step_exp_table, std::vector<Interval> & step_end_exp_table, const double step, const int order)
{
	step_exp_table.clear();
	step_end_exp_table.clear();

	Interval intProd(1), intStep(0,step);

	for(int i=0; i<=order; ++i)
	{
		step_exp_table.push_back(intProd);

		Interval intTend(intProd.sup());
		step_end_exp_table.push_back(intTend);

		intProd *= intStep;
	}
}

void construct_step_exp_table(std::vector<Interval> & step_exp_table, const Interval & step, const int order)
{
	step_exp_table.clear();

	Interval intProd(1);
	step_exp_table.push_back(intProd);

	for(int i=1; i<=order; ++i)
	{
		intProd *= step;
		step_exp_table.push_back(intProd);
	}
}

void preconditionQR(Matrix & result, const TaylorModelVec & x0, const int rangeDim, const int domainDim)
{
	Interval intZero;
	std::vector<std::vector<Interval> > intCoefficients;

	std::vector<Interval> intVecTemp;
	for(int i=0; i<domainDim; ++i)
	{
		intVecTemp.push_back(intZero);
	}

	for(int i=0; i<rangeDim; ++i)
	{
		intCoefficients.push_back(intVecTemp);
	}

	x0.linearCoefficients(intCoefficients);
	Matrix matCoefficients(rangeDim, rangeDim);

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=1; j<=rangeDim; ++j)
		{
			matCoefficients.set(intCoefficients[i][j].midpoint(), i, j-1);
		}
	}

	matCoefficients.sortColumns();
	matCoefficients.QRfactor(result);
}

Interval rho(const TaylorModelVec & tmv, const std::vector<Interval> & l, const std::vector<Interval> & domain)
{
	int d = l.size();
	TaylorModel tmObj;

	for(int i=0; i<d; ++i)
	{
		TaylorModel tmTemp;
		tmv.tms[i].mul(tmTemp, l[i]);
		tmObj.add_assign(tmTemp);
	}

	Interval intRange;
	tmObj.intEval(intRange, domain);

	Interval S;
	intRange.sup(S);

	return S;
}

Interval rhoNormal(const TaylorModelVec & tmv, const std::vector<Interval> & l, const std::vector<Interval> & step_exp_table)
{
	int d = l.size();
	TaylorModel tmObj;

	for(int i=0; i<d; ++i)
	{
		TaylorModel tmTemp;
		tmv.tms[i].mul(tmTemp, l[i]);
		tmObj.add_assign(tmTemp);
	}

	Interval intRange;
	tmObj.intEvalNormal(intRange, step_exp_table);

	Interval S;
	intRange.sup(S);

	return S;
}

Interval rho(const TaylorModelVec & tmv, const RowVector & l, const std::vector<Interval> & domain)
{
	int d = l.size();
	TaylorModel tmObj;

	for(int i=0; i<d; ++i)
	{
		TaylorModel tmTemp;
		Interval intTemp(l.get(i));
		tmv.tms[i].mul(tmTemp, intTemp);
		tmObj.add_assign(tmTemp);
	}

	Interval intRange;
	tmObj.intEval(intRange, domain);

	Interval S;
	intRange.sup(S);

	return S;
}

Interval rhoNormal(const TaylorModelVec & tmv, const RowVector & l, const std::vector<Interval> & step_exp_table)
{
	int d = l.size();
	TaylorModel tmObj;

	for(int i=0; i<d; ++i)
	{
		TaylorModel tmTemp;
		Interval intTemp(l.get(i));
		tmv.tms[i].mul(tmTemp, intTemp);
		tmObj.add_assign(tmTemp);
	}

	Interval intRange;
	tmObj.intEvalNormal(intRange, step_exp_table);

	Interval S;
	intRange.sup(S);

	return S;
}

void templatePolyhedron(Polyhedron & result, const TaylorModelVec & tmv, const std::vector<Interval> & domain)
{
	for(int i=0; i<result.constraints.size(); ++i)
	{
		result.constraints[i].B = rho(tmv, result.constraints[i].A, domain);
	}
}

void templatePolyhedronNormal(Polyhedron & result, const TaylorModelVec & tmv, std::vector<Interval> & step_exp_table)
{
	for(int i=0; i<result.constraints.size(); ++i)
	{
		result.constraints[i].B = rhoNormal(tmv, result.constraints[i].A, step_exp_table);
	}
}

void check_connectivities(bMatrix & result, bMatrix & adjMatrix)
{
	int n = adjMatrix.rows();
	result = adjMatrix;

	std::vector<bool> bselected;
	for(int i=0; i<n; ++i)
	{
		bselected.push_back(false);
	}

	for(int i=0; i<n; ++i)
	{
		// BFS is used to check the connectivity of two nodes
		for(int j=0; j<n; ++j)
		{
			bselected[j] = false;
		}

		std::list<int> unvisited;
		unvisited.push_back(i);

		while(unvisited.size() > 0)
		{
			int j = unvisited.front();
			unvisited.pop_front();

			for(int k=0; k<n; ++k)
			{
				if(bselected[k] == false && adjMatrix[j][k] == true)
				{
					bselected[k] = true;
					unvisited.push_back(k);
					result[i][k] = true;
				}
			}
		}
	}
}



/*
int intersection_check_interval_arithmetic(const vector<PolynomialConstraint> & pcs, const vector<HornerForm> & objFuncs, const vector<Interval> & remainders, const vector<Interval> & domain, vector<bool> & bNeeded)
{
	int counter = 0;
	bNeeded.clear();

	for(int i=0; i<pcs.size(); ++i)
	{
		Interval intTemp;
		objFuncs[i].intEval(intTemp, domain);
		intTemp += remainders[i];

		if(intTemp > pcs[i].B)
		{
			// no intersection
			return -1;
		}
		else if(intTemp.smallereq(pcs[i].B))
		{
			// the flowpipe is entirely contained in the guard, domain contraction is not needed
			bNeeded.push_back(false);
			++counter;
		}
		else
		{
			bNeeded.push_back(true);
		}
	}

	return counter;
}

bool boundary_intersected_collection(const vector<PolynomialConstraint> & pcs, const vector<HornerForm> & objFuncs, const vector<Interval> & remainders, const vector<Interval> & domain, vector<bool> & boundary_intersected)
{
	boundary_intersected.clear();

	for(int i=0; i<pcs.size(); ++i)
	{
		Interval intTemp;
		objFuncs[i].intEval(intTemp, domain);
		intTemp += remainders[i];

		if(intTemp > pcs[i].B)
		{
			// no intersection
			return false;
		}
		else if(intTemp < pcs[i].B)
		{
			// do not intersect the boundary
			boundary_intersected.push_back(false);
		}
		else
		{
			boundary_intersected.push_back(true);
		}
	}

	return true;
}
*/

int contract_interval_arithmetic(TaylorModelVec & flowpipe, std::vector<Interval> & domain, const std::vector<PolynomialConstraint> & pcs, std::vector<bool> & boundary_intersected, const Interval & cutoff_threshold)
{
	if(pcs.size() == 0)
	{
		return 0;
	}

	int rangeDim = flowpipe.tms.size();
	int domainDim = domain.size();

	// contract the remainder firstly
	bool bvalid = true;
	bool bcontinue = true;
	Interval W;
	Interval intZero;

	std::vector<bool> bNeeded;
	int counter = 0;

	for(int i=0; i<pcs.size(); ++i)
	{
		bNeeded.push_back(true);
	}

	boundary_intersected.clear();

	std::vector<Interval> remainders;
	for(int i=0; i<rangeDim; ++i)
	{
		remainders.push_back(flowpipe.tms[i].remainder);
	}

	std::vector<Interval> flowpipePolyRange;
	flowpipe.polyRange(flowpipePolyRange, domain);

	std::vector<Interval> intVecTemp = flowpipePolyRange;
	intVecTemp.insert(intVecTemp.begin(), intZero);

	// 1: we check the intersection with every constraint
	for(int i=0; i<rangeDim; ++i)
	{
		intVecTemp[i+1] = flowpipePolyRange[i] + remainders[i];
	}

	for(int i=0; i<pcs.size(); ++i)
	{
		Interval intTemp;

		pcs[i].hf.intEval(intTemp, intVecTemp);

		if(intTemp > pcs[i].B)
		{
			// no intersection on the left half
			bvalid = false;
			break;
		}
		else if(intTemp.smallereq(pcs[i].B))
		{
			// do not need to apply domain contraction w.r.t. the current constraint
			boundary_intersected.push_back(false);
			bNeeded[i] = false;
			++counter;
		}
		else
		{
			boundary_intersected.push_back(true);
			bNeeded[i] = true;
			continue;
		}
	}

	if(!bvalid)
	{
		boundary_intersected.clear();
		return -1;	// no intersection is detected
	}
	else if(counter == pcs.size())
	{
		return 0;	// no need to do contraction
	}


	// 2: remainder contraction
	for(; bcontinue; )
	{
		std::vector<Interval> oldRemainders = remainders;

		for(int i=0; i<rangeDim; ++i)
		{
			Interval newInt = remainders[i];
			std::vector<bool> localNeeded = bNeeded;
			int localCounter = counter;

			for(int k=0; k<rangeDim; ++k)
			{
				if(k != i)
				{
					intVecTemp[k+1] = flowpipePolyRange[k] + remainders[k];
				}
				else
				{
					intVecTemp[k+1] = flowpipePolyRange[k];
				}
			}

			newInt.width(W);

			// search an approximation for the lower bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<pcs.size(); ++j)
				{
					if(localNeeded[j])
					{
						Interval intTemp;
						std::vector<Interval> newIntVecTemp = intVecTemp;
						newIntVecTemp[i+1] += intLeft;

						pcs[j].hf.intEval(intTemp, newIntVecTemp);

						if(intTemp > pcs[j].B)
						{
							// no intersection on the left half
							newInt = intRight;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(pcs[j].B))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intLeft;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intLeft;
							newInt.width(W);

							continue;
						}
					}
				}

				if(localCounter == pcs.size())
				{
					break;
				}
			}

			// set the lower bound
			Interval Inf;
			newInt.inf(Inf);
			remainders[i].setInf(Inf);

			newInt = remainders[i];
			newInt.width(W);

			localNeeded = bNeeded;
			localCounter = counter;

			// search an approximation for the upper bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<pcs.size(); ++j)
				{
					if(localNeeded[j])
					{
						Interval intTemp;
						std::vector<Interval> newIntVecTemp = intVecTemp;
						newIntVecTemp[i+1] += intRight;

						pcs[j].hf.intEval(intTemp, newIntVecTemp);

						if(intTemp > pcs[j].B)
						{
							// no intersection on the right half
							newInt = intLeft;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(pcs[j].B))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intRight;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intRight;
							newInt.width(W);
							continue;
						}
					}
				}

				if(localCounter == pcs.size())
				{
					break;
				}
			}

			Interval Sup;
			newInt.sup(Sup);
			remainders[i].setSup(Sup);	// set the upper bound

			if(!remainders[i].valid())
			{
				bvalid = false;
				break;
			}
		}

		if(!bvalid)
		{
			break;
		}

		bcontinue = false;
		for(int i=0; i<rangeDim; ++i)
		{
			if(oldRemainders[i].widthRatio(remainders[i]) <= DC_THRESHOLD_IMPROV)
			{
				bcontinue = true;
				break;
			}
		}
	}

	if(!bvalid)
	{
		boundary_intersected.clear();
		return -1;	// no intersection is detected
	}

	for(int i=0; i<rangeDim; ++i)
	{
		flowpipe.tms[i].remainder = remainders[i];
	}


	// the Horner forms of p(T(x))
	std::vector<HornerForm> objHF;

	std::vector<Interval> eval_remainders;

	for(int i=0; i<pcs.size(); ++i)
	{
		TaylorModel tmTemp;
		pcs[i].hf.insert(tmTemp, flowpipe, flowpipePolyRange, domain, cutoff_threshold);

		HornerForm hf;
		Interval remainder;
		tmTemp.toHornerForm(hf, remainder);
		objHF.push_back(hf);
		eval_remainders.push_back(remainder);
	}

	Interval intTime = domain[0];

	bvalid = true;
	bcontinue = true;

	// 3: domain contraction
	for(; bcontinue; )
	{
		std::vector<Interval> oldDomain = domain;

		// contract the domain
		for(int i=0; i<domainDim; ++i)
		{
			Interval newInt = domain[i];
			std::vector<bool> localNeeded = bNeeded;
			int localCounter = counter;

			newInt.width(W);

			// search an approximation for the lower bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<pcs.size(); ++j)
				{
					if(localNeeded[j])
					{
						std::vector<Interval> newDomain = domain;
						newDomain[i] = intLeft;

						Interval intTemp;
						objHF[j].intEval(intTemp, newDomain);
						intTemp += eval_remainders[j];

						if(intTemp > pcs[j].B)
						{
							// no intersection on the left half
							newInt = intRight;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(pcs[j].B))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intLeft;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intLeft;
							newInt.width(W);

							continue;
						}
					}
				}

				if(localCounter == pcs.size())
				{
					break;
				}
			}

			// set the lower bound
			Interval Inf;
			newInt.inf(Inf);
			domain[i].setInf(Inf);

			newInt = domain[i];

			localNeeded = bNeeded;
			localCounter = counter;

			newInt.width(W);

			// search an approximation for the upper bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<pcs.size(); ++j)
				{
					if(localNeeded[j])
					{
						std::vector<Interval> newDomain = domain;
						newDomain[i] = intRight;

						Interval intTemp;
						objHF[j].intEval(intTemp, newDomain);
						intTemp += eval_remainders[j];

						if(intTemp > pcs[j].B)
						{
							// no intersection on the right half
							newInt = intLeft;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(pcs[j].B))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intRight;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intRight;
							newInt.width(W);
							continue;
						}
					}
				}

				if(localCounter == pcs.size())
				{
					break;
				}
			}

			Interval Sup;
			newInt.sup(Sup);
			domain[i].setSup(Sup);	// set the upper bound

			if(!domain[i].valid())
			{
				bvalid = false;
				break;
			}
		}

		if(!bvalid)
		{
			break;
		}

		bcontinue = false;
		for(int i=0; i<domainDim; ++i)
		{
			if(oldDomain[i].widthRatio(domain[i]) <= DC_THRESHOLD_IMPROV)
			{
				bcontinue = true;
				break;
			}
		}

		if(bcontinue)
		{
			objHF.clear();
			eval_remainders.clear();

			flowpipe.polyRange(flowpipePolyRange, domain);

			for(int i=0; i<pcs.size(); ++i)
			{
				TaylorModel tmTemp;

				pcs[i].hf.insert(tmTemp, flowpipe, flowpipePolyRange, domain, cutoff_threshold);

				HornerForm hf;
				Interval remainder;
				tmTemp.toHornerForm(hf, remainder);
				objHF.push_back(hf);
				eval_remainders.push_back(remainder);
			}
		}
	}

	if(!bvalid)
	{
		boundary_intersected.clear();
		return -1;
	}

	if(intTime != domain[0])
	{
		return 2;
	}
	else
	{
		return 1;
	}
}

/*
int contract_interval_arithmetic(TaylorModelVec & flowpipe, std::vector<Interval> & domain, const Polyhedron & inv, std::vector<bool> & boundary_intersected)
{
	int rangeDim = flowpipe.tms.size();
	int domainDim = domain.size();

	boundary_intersected.clear();

	// contract the remainder firstly
	bool bvalid = true;
	bool bcontinue = true;
	Interval W;
	Interval intZero;

	std::vector<bool> bNeeded;
	int counter = 0;

	for(int i=0; i<inv.constraints.size(); ++i)
	{
		bNeeded.push_back(true);
	}

	boundary_intersected.clear();

	std::vector<Interval> remainders;
	for(int i=0; i<rangeDim; ++i)
	{
		remainders.push_back(flowpipe.tms[i].remainder);
	}

	std::vector<Interval> flowpipePolyRange;
	flowpipe.polyRange(flowpipePolyRange, domain);

	std::vector<Interval> intVecTemp = flowpipePolyRange;

	// 1: we check the intersection with every constraint
	for(int i=0; i<rangeDim; ++i)
	{
		intVecTemp[i] = flowpipePolyRange[i] + remainders[i];
	}

	for(int i=0; i<inv.constraints.size(); ++i)
	{
		Interval intTemp;

		for(int j=0; j<inv.constraints[i].A.size(); ++j)
		{
			intTemp += inv.constraints[i].A[j] * intVecTemp[j];
		}

		if(intTemp > inv.constraints[i].B)
		{
			// no intersection on the left half
			bvalid = false;
			break;
		}
		else if(intTemp.smallereq(inv.constraints[i].B))
		{
			// do not need to apply domain contraction w.r.t. the current constraint
			boundary_intersected.push_back(false);
			bNeeded[i] = false;
			++counter;
		}
		else
		{
			boundary_intersected.push_back(true);
			bNeeded[i] = true;
			continue;
		}
	}

	if(!bvalid)
	{
		boundary_intersected.clear();
		return -1;	// no intersection is detected
	}
	else if(counter == inv.constraints.size())
	{
		return 0;	// no need to do contraction
	}


	// 2: remainder contraction
	for(; bcontinue; )
	{
		std::vector<Interval> oldRemainders = remainders;

		for(int i=0; i<rangeDim; ++i)
		{
			Interval newInt = remainders[i];
			std::vector<bool> localNeeded = bNeeded;
			int localCounter = counter;

			for(int k=0; k<rangeDim; ++k)
			{
				if(k != i)
				{
					intVecTemp[k] = flowpipePolyRange[k] + remainders[k];
				}
				else
				{
					intVecTemp[k] = flowpipePolyRange[k];
				}
			}

			newInt.width(W);

			// search an approximation for the lower bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<inv.constraints.size(); ++j)
				{
					if(localNeeded[j])
					{
						Interval intTemp;
						std::vector<Interval> newIntVecTemp = intVecTemp;
						newIntVecTemp[i] += intLeft;

						for(int k=0; k<inv.constraints[j].A.size(); ++k)
						{
							intTemp += inv.constraints[j].A[k] * newIntVecTemp[k];
						}

						if(intTemp > inv.constraints[j].B)
						{
							// no intersection on the left half
							newInt = intRight;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(inv.constraints[j].B))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intLeft;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intLeft;
							newInt.width(W);

							continue;
						}
					}
				}

				if(localCounter == inv.constraints.size())
				{
					break;
				}
			}

			// set the lower bound
			Interval Inf;
			newInt.inf(Inf);
			remainders[i].setInf(Inf);

			newInt = remainders[i];
			newInt.width(W);

			localNeeded = bNeeded;
			localCounter = counter;

			// search an approximation for the upper bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<inv.constraints.size(); ++j)
				{
					if(localNeeded[j])
					{
						Interval intTemp;
						std::vector<Interval> newIntVecTemp = intVecTemp;
						newIntVecTemp[i] += intRight;

						for(int k=0; k<inv.constraints[j].A.size(); ++k)
						{
							intTemp += inv.constraints[j].A[k] * newIntVecTemp[k];
						}

						if(intTemp > inv.constraints[j].B)
						{
							// no intersection on the right half
							newInt = intLeft;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(inv.constraints[j].B))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intRight;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intRight;
							newInt.width(W);
							continue;
						}
					}
				}

				if(localCounter == inv.constraints.size())
				{
					break;
				}
			}

			Interval Sup;
			newInt.sup(Sup);
			remainders[i].setSup(Sup);	// set the upper bound

			if(!remainders[i].valid())
			{
				bvalid = false;
				break;
			}
		}

		if(!bvalid)
		{
			break;
		}

		bcontinue = false;
		for(int i=0; i<rangeDim; ++i)
		{
			if(oldRemainders[i].widthRatio(remainders[i]) <= DC_THRESHOLD_IMPROV)
			{
				bcontinue = true;
				break;
			}
		}
	}

	if(!bvalid)
	{
		boundary_intersected.clear();
		return -1;	// no intersection is detected
	}

	for(int i=0; i<rangeDim; ++i)
	{
		flowpipe.tms[i].remainder = remainders[i];
	}


	// the Horner forms of p(T(x))
	std::vector<HornerForm> objHF;
	std::vector<Interval> eval_remainders;

	for(int i=0; i<inv.constraints.size(); ++i)
	{
		TaylorModel tmTemp;

		for(int j=0; j<inv.constraints[i].A.size(); ++j)
		{
			TaylorModel tmTemp2;
			flowpipe.tms[j].mul(tmTemp2, inv.constraints[i].A[j]);
			tmTemp.add_assign(tmTemp2);
		}

		HornerForm hf;
		Interval remainder;
		tmTemp.toHornerForm(hf, remainder);

		objHF.push_back(hf);
		eval_remainders.push_back(remainder);
	}

	Interval intTime = domain[0];

	bvalid = true;
	bcontinue = true;

	// 3: domain contraction

	for(; bcontinue; )
	{
		std::vector<Interval> oldDomain = domain;

		// contract the domain
		for(int i=0; i<domainDim; ++i)
		{
			Interval newInt = domain[i];
			std::vector<bool> localNeeded = bNeeded;
			int localCounter = counter;

			newInt.width(W);

			// search an approximation for the lower bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<inv.constraints.size(); ++j)
				{
					if(localNeeded[j])
					{
						std::vector<Interval> newDomain = domain;
						newDomain[i] = intLeft;

						Interval intTemp;
						objHF[j].intEval(intTemp, newDomain);
						intTemp += eval_remainders[j];

						if(intTemp > inv.constraints[j].B)
						{
							// no intersection on the left half
							newInt = intRight;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(inv.constraints[j].B))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intLeft;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intLeft;
							newInt.width(W);

							continue;
						}
					}
				}

				if(localCounter == inv.constraints.size())
				{
					break;
				}
			}

			// set the lower bound
			Interval Inf;
			newInt.inf(Inf);
			domain[i].setInf(Inf);

			newInt = domain[i];

			localNeeded = bNeeded;
			localCounter = counter;

			newInt.width(W);

			// search an approximation for the upper bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<inv.constraints.size(); ++j)
				{
					if(localNeeded[j])
					{
						std::vector<Interval> newDomain = domain;
						newDomain[i] = intRight;

						Interval intTemp;
						objHF[j].intEval(intTemp, newDomain);
						intTemp += eval_remainders[j];

						if(intTemp > inv.constraints[j].B)
						{
							// no intersection on the right half
							newInt = intLeft;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(inv.constraints[j].B))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intRight;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intRight;
							newInt.width(W);
							continue;
						}
					}
				}

				if(localCounter == inv.constraints.size())
				{
					break;
				}
			}

			Interval Sup;
			newInt.sup(Sup);
			domain[i].setSup(Sup);	// set the upper bound

			if(!domain[i].valid())
			{
				bvalid = false;
				break;
			}
		}

		if(!bvalid)
		{
			break;
		}

		bcontinue = false;
		for(int i=0; i<domainDim; ++i)
		{
			if(oldDomain[i].widthRatio(domain[i]) <= DC_THRESHOLD_IMPROV)
			{
				bcontinue = true;
				break;
			}
		}
	}

	if(!bvalid)
	{
		boundary_intersected.clear();
		return -1;
	}

	if(intTime != domain[0])
	{
		return 2;
	}
	else
	{
		return 1;
	}
}
*/

int contract_remainder(const std::vector<Interval> & polyRange, std::vector<Interval> & remainders, const std::vector<HornerForm> & hfs, const std::vector<Interval> & b)
{
	bool bvalid = true;
	bool bcontinue = true;
	Interval W;
	Interval intZero;

	std::vector<bool> bNeeded;
	int counter = 0;

	for(int i=0; i<hfs.size(); ++i)
	{
		bNeeded.push_back(true);
	}

	int rangeDim = polyRange.size();

	std::vector<Interval> intVecTemp = polyRange;
	intVecTemp.insert(intVecTemp.begin(), intZero);		// range of the dummy time variable

	// 1: we check the intersection with every constraint
	for(int i=0; i<rangeDim; ++i)
	{
		intVecTemp[i+1] = polyRange[i] + remainders[i];
	}

	for(int i=0; i<hfs.size(); ++i)
	{
		Interval intTemp;

		hfs[i].intEval(intTemp, intVecTemp);

		if(intTemp > b[i])
		{
			// no intersection on the left half
			bvalid = false;
			break;
		}
		else if(intTemp.smallereq(b[i]))
		{
			// do not need to apply domain contraction w.r.t. the current constraint
			bNeeded[i] = false;
			++counter;
		}
		else
		{
			bNeeded[i] = true;
			continue;
		}
	}

	if(!bvalid)
	{
		return -1;	// no intersection is detected
	}
	else if(counter == hfs.size())
	{
		return 0;	// no need to do contraction
	}

	// 2: contract the remainder
	for(; bcontinue; )
	{
		std::vector<Interval> oldRemainders = remainders;

		for(int i=0; i<rangeDim; ++i)
		{
			Interval newInt = remainders[i];
			std::vector<bool> localNeeded = bNeeded;
			int localCounter = counter;

			for(int k=0; k<rangeDim; ++k)
			{
				if(k != i)
				{
					intVecTemp[k+1] = polyRange[k] + remainders[k];
				}
				else
				{
					intVecTemp[k+1] = polyRange[k];
				}
			}

			newInt.width(W);

			// search an approximation for the lower bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<hfs.size(); ++j)
				{
					if(localNeeded[j])
					{
						Interval intTemp;
						std::vector<Interval> newIntVecTemp = intVecTemp;
						newIntVecTemp[i+1] += intLeft;

						hfs[j].intEval(intTemp, newIntVecTemp);

						if(intTemp > b[j])
						{
							// no intersection on the left half
							newInt = intRight;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(b[j]))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intLeft;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intLeft;
							newInt.width(W);

							continue;
						}
					}
				}

				if(localCounter == hfs.size())
				{
					break;
				}
			}

			// set the lower bound
			Interval Inf;
			newInt.inf(Inf);
			remainders[i].setInf(Inf);

			newInt = remainders[i];
			newInt.width(W);

			localNeeded = bNeeded;
			localCounter = counter;

			// search an approximation for the upper bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<hfs.size(); ++j)
				{
					if(localNeeded[j])
					{
						Interval intTemp;
						std::vector<Interval> newIntVecTemp = intVecTemp;
						newIntVecTemp[i+1] += intRight;

						hfs[j].intEval(intTemp, newIntVecTemp);

						if(intTemp > b[j])
						{
							// no intersection on the right half
							newInt = intLeft;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(b[j]))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intRight;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intRight;
							newInt.width(W);
							continue;
						}
					}
				}

				if(localCounter == hfs.size())
				{
					break;
				}
			}

			Interval Sup;
			newInt.sup(Sup);
			remainders[i].setSup(Sup);	// set the upper bound

			if(!remainders[i].valid())
			{
				bvalid = false;
				break;
			}
		}

		if(!bvalid)
		{
			break;
		}

		bcontinue = false;
		for(int i=0; i<rangeDim; ++i)
		{
			if(oldRemainders[i].widthRatio(remainders[i]) <= DC_THRESHOLD_IMPROV)
			{
				bcontinue = true;
				break;
			}
		}
	}

	if(!bvalid)
	{
		return -1;	// no intersection is detected
	}
	else
	{
		return 1;
	}
}

int contract_remainder(const std::vector<Interval> & polyRange, std::vector<Interval> & remainders, const std::vector<PolynomialConstraint> & constraints)
{
	if(constraints.size() == 0)
	{
		return 0;
	}

	bool bvalid = true;
	bool bcontinue = true;
	Interval W;
	Interval intZero;

	std::vector<bool> bNeeded;
	int counter = 0;

	for(int i=0; i<constraints.size(); ++i)
	{
		bNeeded.push_back(true);
	}

	int rangeDim = polyRange.size();

	std::vector<Interval> intVecTemp = polyRange;
	intVecTemp.insert(intVecTemp.begin(), intZero);		// range of the dummy time variable

	// 1: we check the intersection with every constraint
	for(int i=0; i<rangeDim; ++i)
	{
		intVecTemp[i+1] = polyRange[i] + remainders[i];
	}

	for(int i=0; i<constraints.size(); ++i)
	{
		Interval intTemp;

		constraints[i].hf.intEval(intTemp, intVecTemp);

		if(intTemp > constraints[i].B)
		{
			// no intersection on the left half
			bvalid = false;
			break;
		}
		else if(intTemp.smallereq(constraints[i].B))
		{
			// do not need to apply domain contraction w.r.t. the current constraint
			bNeeded[i] = false;
			++counter;
		}
		else
		{
			bNeeded[i] = true;
			continue;
		}
	}

	if(!bvalid)
	{
		return -1;	// no intersection is detected
	}
	else if(counter == constraints.size())
	{
		return 0;	// no need to do contraction
	}

	// 2: contract the remainder
	for(; bcontinue; )
	{
		std::vector<Interval> oldRemainders = remainders;

		for(int i=0; i<rangeDim; ++i)
		{
			Interval newInt = remainders[i];
			std::vector<bool> localNeeded = bNeeded;
			int localCounter = counter;

			for(int k=0; k<rangeDim; ++k)
			{
				if(k != i)
				{
					intVecTemp[k+1] = polyRange[k] + remainders[k];
				}
				else
				{
					intVecTemp[k+1] = polyRange[k];
				}
			}

			newInt.width(W);

			// search an approximation for the lower bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<constraints.size(); ++j)
				{
					if(localNeeded[j])
					{
						Interval intTemp;
						std::vector<Interval> newIntVecTemp = intVecTemp;
						newIntVecTemp[i+1] += intLeft;

						constraints[j].hf.intEval(intTemp, newIntVecTemp);

						if(intTemp > constraints[j].B)
						{
							// no intersection on the left half
							newInt = intRight;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(constraints[j].B))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intLeft;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intLeft;
							newInt.width(W);

							continue;
						}
					}
				}

				if(localCounter == constraints.size())
				{
					break;
				}
			}

			// set the lower bound
			Interval Inf;
			newInt.inf(Inf);
			remainders[i].setInf(Inf);

			newInt = remainders[i];
			newInt.width(W);

			localNeeded = bNeeded;
			localCounter = counter;

			// search an approximation for the upper bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				for(int j=0; j<constraints.size(); ++j)
				{
					if(localNeeded[j])
					{
						Interval intTemp;
						std::vector<Interval> newIntVecTemp = intVecTemp;
						newIntVecTemp[i+1] += intRight;

						constraints[j].hf.intEval(intTemp, newIntVecTemp);

						if(intTemp > constraints[j].B)
						{
							// no intersection on the right half
							newInt = intLeft;
							newInt.width(W);
							break;
						}
						else if(intTemp.smallereq(constraints[j].B))
						{
							// do not need to apply domain contraction w.r.t. the current constraint
							newInt = intRight;
							newInt.width(W);
							localNeeded[j] = false;
							++localCounter;
						}
						else
						{
							// refine the interval
							newInt = intRight;
							newInt.width(W);
							continue;
						}
					}
				}

				if(localCounter == constraints.size())
				{
					break;
				}
			}

			Interval Sup;
			newInt.sup(Sup);
			remainders[i].setSup(Sup);	// set the upper bound

			if(!remainders[i].valid())
			{
				bvalid = false;
				break;
			}
		}

		if(!bvalid)
		{
			break;
		}

		bcontinue = false;
		for(int i=0; i<rangeDim; ++i)
		{
			if(oldRemainders[i].widthRatio(remainders[i]) <= DC_THRESHOLD_IMPROV)
			{
				bcontinue = true;
				break;
			}
		}
	}

	if(!bvalid)
	{
		return -1;	// no intersection is detected
	}
	else
	{
		return 1;
	}
}

void gridBox(std::list<std::vector<Interval> > & grids, const std::vector<Interval> & box, const int num)
{
	grids.clear();
	grids.push_back(box);

	for(int i=0; i<box.size(); ++i)
	{
		std::list<std::vector<Interval> >::iterator gridIter;
		std::list<std::vector<Interval> > newGrids;

		for(; grids.size() > 0;)
		{
			gridIter = grids.begin();

			std::list<Interval> queue;
			(*gridIter)[i].split(queue, num);

			std::list<Interval>::iterator iterComponent = queue.begin();
			for(; iterComponent != queue.end(); ++iterComponent)
			{
				std::vector<Interval> tmpBox = *gridIter;
				tmpBox[i] = *iterComponent;
				newGrids.push_back(tmpBox);
			}

			grids.pop_front();
		}

		grids = newGrids;
	}
}

/*
void exp_int_mat(iMatrix & result_ts, iMatrix & result_rem, const iMatrix & A, const int order)
{
	int dim = A.cols();

	iMatrix identity(dim, dim), zero(dim, dim);
	Interval intOne(1);

	for(int i=0; i<dim; ++i)
	{
		identity[i][i] = intOne;
	}

	if(order == 0)
	{
		result_ts = identity;
		result_rem = zero;
	}
	else
	{
		result_ts = identity;

		for(int k=order; k>0; --k)
		{
			result_ts /= k;
			result_ts *= A;
			result_ts += identity;
		}

		iMatrix matTemp = A;
		matTemp.pow_assign(order+1);
		matTemp *= factorial_rec[order+1];

		double max_A = A.max_norm();
		Interval intTemp(max_A);
		intTemp.exp_assign();

		double maxNorm = intTemp.mag();
		Interval remainder(-maxNorm, maxNorm);

		iMatrix matRemainder(dim, dim);
		for(int i=0; i<dim; ++i)
		{
			for(int j=0; j<dim; ++j)
			{
				matRemainder[i][j] = remainder;
			}
		}

		matTemp *= matRemainder;
		result_rem = matTemp;
	}
}

void int_exp_int_mat(Interval_matrix & result_ts, Interval_matrix & result_rem, const Interval_matrix & A, const double step, const int order)
{
	int dim = A.cols();

	Interval_matrix identity(dim, dim), zero(dim, dim);
	Interval intOne(1);

	for(int i=0; i<dim; ++i)
	{
		identity.set(intOne, i, i);
	}

	if(order == 0)
	{
		result_ts = identity;
		result_rem = zero;
	}
	else
	{
		Interval_matrix R(dim, dim);
		Interval intStep(step);

		for(int i=0; i<dim; ++i)
		{
			R.set(intStep, i, i);
		}

		result_ts = R;
		Interval_matrix mA = A;
		mA.mul_assign(-1.0);

		for(int k=order+1; k>1; --k)
		{
			result_ts.div_assign(k);
			result_ts *= mA;
			result_ts += R;
		}

		Interval_matrix matTemp = mA;
		matTemp.pow_assign(order+1);
		matTemp.mul_assign(factorial_rec[order+2]);
		matTemp.mul_assign(step);

		double max_mA = mA.max_norm();
		Interval intTemp(max_mA);
		intTemp.exp_assign();

		double maxNorm = intTemp.mag();
		Interval remainder(-maxNorm, maxNorm);

		Interval_matrix matRemainder(dim, dim);
		for(int i=0; i<dim; ++i)
		{
			for(int j=0; j<dim; ++j)
			{
				matRemainder.set(remainder, i, j);
			}
		}

		matTemp *= matRemainder;
		result_rem = matTemp;
	}
}
*/

void compute_int_mat_pow(std::vector<iMatrix> & result, const iMatrix & A, const int order)
{
	int d = A.rows();
	Interval intOne(1);

	iMatrix identity(d, d);

	// identity matrix
	for(int i=0; i<d; ++i)
	{
		identity[i][i] = intOne;
	}

	std::vector<bool> pow_A_computed;

	iMatrix imEmpty;

	for(int i=0; i<=order; ++i)
	{
		pow_A_computed.push_back(false);
		result.push_back(imEmpty);
	}

	pow_A_computed[0] = true;
	result[0] = identity;

	if(order < 1)
	{
		return;
	}

	pow_A_computed[1] = true;
	result[1] = A;

	for(int i=order; i>1; --i)
	{
		if(!pow_A_computed[i])
		{
			iMatrix temp = A;
			iMatrix temp2 = A;
			int pos_temp = 1;
			int pos_result = 1;

			for(int d=i-1; d > 0;)
			{
				if(d & 1)
				{
					pos_result += pos_temp;

					if(pow_A_computed[pos_result])
					{
						temp2 = result[pos_result];
					}
					else
					{
						temp2 *= temp;
						pow_A_computed[pos_result] = true;
						result[pos_result] = temp2;
					}
				}

				d >>= 1;

				if(d > 0)
				{
					pos_temp <<= 1;

					if(pow_A_computed[pos_temp])
					{
						temp = result[pos_temp];
					}
					else
					{
						temp *= temp;
						pow_A_computed[pos_temp] = true;
						result[pos_temp] = temp;
					}
				}
			}
		}
	}
}


void compute_int_mat2_pow(std::vector<iMatrix2> & result, const iMatrix2 & A, const int order)
{
	int d = A.rows();

	iMatrix2 im2_id(d);

	std::vector<bool> pow_A_computed;

	iMatrix imEmpty;

	for(int i=0; i<=order; ++i)
	{
		pow_A_computed.push_back(false);
		result.push_back(imEmpty);
	}

	pow_A_computed[0] = true;
	result[0] = im2_id;

	if(order < 1)
	{
		return;
	}

	pow_A_computed[1] = true;
	result[1] = A;

	for(int i=order; i>1; --i)
	{
		if(!pow_A_computed[i])
		{
			iMatrix2 temp = A;
			iMatrix2 temp2 = A;
			int pos_temp = 1;
			int pos_result = 1;

			for(int d=i-1; d > 0;)
			{
				if(d & 1)
				{
					pos_result += pos_temp;

					if(pow_A_computed[pos_result])
					{
						temp2 = result[pos_result];
					}
					else
					{
						temp2 *= temp;
						pow_A_computed[pos_result] = true;
						result[pos_result] = temp2;
					}
				}

				d >>= 1;

				if(d > 0)
				{
					pos_temp <<= 1;

					if(pow_A_computed[pos_temp])
					{
						temp = result[pos_temp];
					}
					else
					{
						temp *= temp;
						pow_A_computed[pos_temp] = true;
						result[pos_temp] = temp;
					}
				}
			}
		}
	}
}

/*
void compute_Peano_Baker_series_certain(Polynomial_matrix & nonconst_series, Interval_matrix & nonconst_remainder, Polynomial_matrix & const_series, Interval_matrix & const_remainder,
		const Polynomial_matrix & A, const int A_degree, const Polynomial_matrix & B, const int B_degree, const double t0, const int order,
		const std::vector<Interval> & step_exp_table, const std::vector<std::string> & varNames)
{
	Interval intOne(1), int_t0(t0), intProd(t0);

	int rangeDim = A.rows();

	// identity matrix
	Interval_matrix identity(rangeDim, rangeDim);
	for(int i=0; i<rangeDim; ++i)
	{
		identity.set(intOne, i, i);
	}

	// t -> t0 + t
	Polynomial poly_1(intOne, rangeDim+1), poly_t0(int_t0, rangeDim+1), poly_t(0, 1, rangeDim+1);
	poly_t += poly_t0;

	Polynomial poly_tmp = poly_t;

	std::vector<Polynomial> poly_t_exp_table;
	poly_t_exp_table.push_back(poly_1);
	poly_t_exp_table.push_back(poly_t);

	for(int i=2; i<=A_degree; ++i)
	{
		poly_tmp *= poly_t;
		poly_t_exp_table.push_back(poly_tmp);
	}

	Polynomial_matrix precond_A;
	A.substitute(precond_A, poly_t_exp_table);

	Polynomial_matrix pm_identity(identity, rangeDim+1), pm_tmp1(rangeDim, rangeDim);

	pm_tmp1 = pm_identity;
	nonconst_series = pm_identity;
	const_series = pm_identity;

	// compute the series for the state-transition matrix
	for(int i=1; i<=order; ++i)
	{
		pm_tmp1 = precond_A * pm_tmp1;

		for(int j=0; j<rangeDim; ++j)
		{
			for(int k=0; k<rangeDim; ++k)
			{
				pm_tmp1[j][k].integral_t();
			}
		}

		nonconst_series += pm_tmp1;

		// method 1
		if(i%2 == 0)
		{
			const_series += pm_tmp1;
		}
		else
		{
			const_series -= pm_tmp1;
		}
	}

	// compute the remainder
	Interval_matrix im_precond_A;
	precond_A.evaluate(im_precond_A, step_exp_table);
	double max_precond_A = im_precond_A.max_norm();

	Interval intTemp(max_precond_A * step_exp_table[1].sup());
	intTemp.exp_assign();
	double maxNorm = intTemp.mag();
	Interval intMaxA(-maxNorm, maxNorm);

	Interval rem_A(max_precond_A);
	rem_A.pow_assign(order+1);
	rem_A *= step_exp_table[order+1];
	rem_A *= factorial_rec[order+1];
	rem_A *= intMaxA;

	Interval_matrix im_rem_A(rangeDim, rangeDim);
	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=0; j<rangeDim; ++j)
		{
			im_rem_A[i][j] = rem_A;
		}
	}

	nonconst_remainder = im_rem_A;

	Polynomial_matrix precond_B;
	B.substitute(precond_B, poly_t_exp_table);
	const_series *= precond_B;

	for(int i=0; i<rangeDim; ++i)
	{
		const_series[i][0].integral_t();
	}

	Interval_matrix im_precond_B;
	precond_B.evaluate(im_precond_B, step_exp_table);
	double max_precond_B = im_precond_B.max_norm();
	Interval intMaxB(-max_precond_B, max_precond_B);

	// compute the remainder
	Interval rem_B = rem_A;
	rem_B.mul_assign(step_exp_table[1].sup());
	rem_B.div_assign(order+2);
	rem_B *= intMaxB;

	Interval_matrix im_rem_B(rangeDim, 1);
	for(int i=0; i<rangeDim; ++i)
	{
		im_rem_B[i][0] = rem_B;
	}

	const_remainder = im_rem_B;
}

void compute_Peano_Baker_series_uncertain(Polynomial_matrix & nonconst_series, Interval_matrix & nonconst_remainder, Polynomial_matrix & const_series, Interval_matrix & const_remainder,
		std::vector<Polynomial> & poly_t_table, const Polynomial_matrix & A, const int A_degree, const Polynomial_matrix & B, const int B_degree, const double t0, const int order,
		const std::vector<Interval> & step_exp_table)
{
	Interval intOne(1), int_t0(t0), intProd(t0);

	int rangeDim = A.rows();

	// identity matrix
	Interval_matrix identity(rangeDim, rangeDim);
	for(int i=0; i<rangeDim; ++i)
	{
		identity.set(intOne, i, i);
	}

	// t -> t0 + t
	Polynomial poly_1(intOne, rangeDim+1), poly_t0(int_t0, rangeDim+1), poly_t(0, 1, rangeDim+1);
	poly_t += poly_t0;

	Polynomial poly_tmp = poly_t;

	std::vector<Polynomial> poly_t_exp_table;
	poly_t_exp_table.push_back(poly_1);
	poly_t_exp_table.push_back(poly_t);

	for(int i=2; i<=A_degree; ++i)
	{
		poly_tmp *= poly_t;
		poly_t_exp_table.push_back(poly_tmp);
	}

	poly_t_table = poly_t_exp_table;

	Polynomial_matrix precond_A;
	A.substitute(precond_A, poly_t_exp_table);

	Polynomial_matrix pm_identity(identity, rangeDim+1), pm_tmp1(rangeDim, rangeDim);

	pm_tmp1 = pm_identity;
	nonconst_series = pm_identity;
	const_series = pm_identity;

	// compute the series for the state-transition matrix
	for(int i=1; i<=order; ++i)
	{
		pm_tmp1 = precond_A * pm_tmp1;

		for(int j=0; j<rangeDim; ++j)
		{
			for(int k=0; k<rangeDim; ++k)
			{
				pm_tmp1[j][k].integral_t();
			}
		}

		nonconst_series += pm_tmp1;

		// method 1
		if(i%2 == 0)
		{
			const_series += pm_tmp1;
		}
		else
		{
			const_series -= pm_tmp1;
		}
	}

	// compute the remainder
	Interval_matrix im_precond_A;
	precond_A.evaluate(im_precond_A, step_exp_table);
	double max_precond_A = im_precond_A.max_norm();

	Interval intTemp(max_precond_A * step_exp_table[1].sup());
	intTemp.exp_assign();
	double maxNorm = intTemp.mag();
	Interval intMaxA(-maxNorm, maxNorm);

	Interval rem_A(max_precond_A);
	rem_A.pow_assign(order+1);
	rem_A *= step_exp_table[order+1];
	rem_A *= factorial_rec[order+1];
	rem_A *= intMaxA;

	Interval_matrix im_rem_A(rangeDim, rangeDim);
	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=0; j<rangeDim; ++j)
		{
			im_rem_A[i][j] = rem_A;
		}
	}

	nonconst_remainder = im_rem_A;

	// compute the remainder
	Interval rem_B = rem_A;
	rem_B.mul_assign(step_exp_table[1].sup());
	rem_B.div_assign(order+2);

	Interval_matrix im_rem_B(rangeDim, rangeDim);
	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=0; j<rangeDim; ++j)
		{
			im_rem_B[i][j] = rem_B;
		}
	}

	const_remainder = im_rem_B;
}


void compute_one_step_trans(Polynomial_matrix & p_Phi_t_t0, Interval_matrix & p_Phi_t_t0_remainder, Polynomial_matrix & p_Phi_t0_s, Interval_matrix & p_Phi_t0_s_remainder,
		const Polynomial_matrix & A, const int A_degree, const double t0, std::vector<Polynomial> & poly_t_exp_table, const int order, const std::vector<Interval> & step_exp_table, const std::vector<Interval> & inv_step_exp_table)
{
	Interval intOne(1);

	int rangeDim = A.rows();

	// identity matrix
	Interval_matrix identity(rangeDim, rangeDim);
	for(int i=0; i<rangeDim; ++i)
	{
		identity.set(intOne, i, i);
	}

	Polynomial_matrix precond_A;
	A.substitute(precond_A, poly_t_exp_table);

	Polynomial_matrix pm_identity(identity, rangeDim+1), pm_tmp1(rangeDim, rangeDim);

	pm_tmp1 = pm_identity;
	p_Phi_t_t0 = pm_identity;
	p_Phi_t0_s = pm_identity;

	// compute the series for the state-transition matrix
	for(int i=1; i<=order; ++i)
	{
		pm_tmp1 = precond_A * pm_tmp1;

		for(int j=0; j<rangeDim; ++j)
		{
			for(int k=0; k<rangeDim; ++k)
			{
				pm_tmp1[j][k].integral_t();
			}
		}

		p_Phi_t_t0 += pm_tmp1;

		if(i%2 == 0)
		{
			p_Phi_t0_s += pm_tmp1;
		}
		else
		{
			p_Phi_t0_s -= pm_tmp1;
		}
	}

	// compute the remainder intervals

	Interval_matrix im_precond_A;
	precond_A.evaluate(im_precond_A, step_exp_table);
	double max_precond_A = im_precond_A.max_norm();

	Interval A_delta(max_precond_A * step_exp_table[1].sup());
	Interval A_delta_pow = A_delta.pow(order + 1);

//	A_delta.exp_assign();
	Interval k_p_2(order + 2);
	A_delta = intOne - A_delta / k_p_2;
	A_delta.rec_assign();

	A_delta *= A_delta_pow;
	A_delta	*= factorial_rec[order+1];
	double maxNorm = A_delta.mag();
	Interval intMaxNorm(-maxNorm, maxNorm);

	Interval_matrix im_rem(rangeDim, rangeDim);
	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=0; j<rangeDim; ++j)
		{
			im_rem[i][j] = intMaxNorm;
		}
	}

	p_Phi_t_t0_remainder = im_rem;


	Interval_matrix im_inv_precond_A;
	precond_A.evaluate(im_inv_precond_A, inv_step_exp_table);
	double max_inv_precond_A = im_inv_precond_A.max_norm();

	Interval A_inv_delta(max_inv_precond_A * step_exp_table[1].sup());
	Interval A_inv_delta_pow = A_inv_delta.pow(order + 1);
//	Interval A_inv_delta_pow = A_inv_delta.pow(1);

	A_inv_delta.exp_assign();
	A_inv_delta *= A_inv_delta_pow;
	A_inv_delta	*= factorial_rec[order+1];
	double maxNormInv = A_inv_delta.mag();
	Interval intMaxNormInv(-maxNormInv, maxNormInv);

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=0; j<rangeDim; ++j)
		{
			im_rem[i][j] = intMaxNormInv;
		}
	}

	p_Phi_t0_s_remainder = im_rem;
}
*/


void compute_one_step_trans(upMatrix & p_Phi_t, upMatrix & p_Psi_t, upMatrix & p_Omega_t,
		iMatrix & Phi_step_trunc, iMatrix & Phi_step_end_trunc, iMatrix & Phi_rem,
		iMatrix & Psi_step_trunc, iMatrix & Psi_step_end_trunc, iMatrix & Psi_rem,
		iMatrix & Omega_step_trunc, iMatrix & Omega_step_end_trunc, iMatrix & Omega_rem, iMatrix & tv_part,
		const upMatrix & A_t, const upMatrix & B_t, const upMatrix & ti_t, const upMatrix & tv_t,
		bMatrix & connectivity, const bool bAuto, const UnivariatePolynomial & up_t, const int order,
		const std::vector<Interval> & step_exp_table, const std::vector<Interval> & step_end_exp_table)
{
	Interval intOne(1);

	int rangeDim = A_t.rows(), numTIPar = ti_t.cols(), numTVPar = tv_t.cols();

	iMatrix identity(rangeDim);
	iMatrix im_zero_Phi(rangeDim, rangeDim), im_zero_Psi(rangeDim, 1), im_zero_ti(rangeDim, numTIPar);

	Phi_step_trunc = im_zero_Phi;
	Phi_step_end_trunc = im_zero_Phi;
	Phi_rem = im_zero_Phi;

	if(!bAuto)
	{
		Psi_step_trunc = im_zero_Psi;
		Psi_step_end_trunc = im_zero_Psi;
	}

	if(numTIPar > 0)
	{
		Omega_step_trunc = im_zero_ti;
		Omega_step_end_trunc = im_zero_ti;
	}

	upMatrix local_A_t;
	A_t.substitute(local_A_t, up_t);
	p_Phi_t = identity;

	upMatrix upm_tmp_Psi;
	upMatrix local_B_t;
	if(!bAuto)
	{
		B_t.substitute(local_B_t, up_t);
		p_Psi_t = local_B_t;
		p_Psi_t.integral();
		p_Psi_t.ctrunc(Psi_step_trunc, Psi_step_end_trunc, order, step_exp_table, step_end_exp_table);
		upm_tmp_Psi = p_Psi_t;
	}

	upMatrix upm_tmp_Phi = p_Phi_t;

	upMatrix local_ti_t;
	if(numTIPar > 0)
	{
		ti_t.substitute(local_ti_t, up_t);
		p_Omega_t = local_ti_t;
		p_Omega_t.integral();
		p_Omega_t.ctrunc(Omega_step_trunc, Omega_step_end_trunc, order, step_exp_table, step_end_exp_table);
	}

	upMatrix upm_tmp_Omega = p_Omega_t;


	// compute the polynomial approximations
	for(int i=1; i<=order; ++i)
	{
		upm_tmp_Phi = local_A_t * upm_tmp_Phi;
		upm_tmp_Phi.integral();

		if(i < order)
		{
			upm_tmp_Phi.ctrunc(order, step_exp_table);
		}
		else
		{
			upm_tmp_Phi.ctrunc(order, step_end_exp_table);
		}

		p_Phi_t += upm_tmp_Phi;

		if(!bAuto)
		{
			upm_tmp_Psi = local_A_t * upm_tmp_Psi;
			upm_tmp_Psi.integral();
			if(i < order)
			{
				upm_tmp_Psi.ctrunc(order, step_exp_table);
			}
			else
			{
				upm_tmp_Psi.ctrunc(order, step_end_exp_table);
			}

			p_Psi_t += upm_tmp_Psi;
		}


		if(numTIPar > 0)
		{
			upm_tmp_Omega = local_A_t * upm_tmp_Omega;
			upm_tmp_Omega.integral();
			if(i < order)
			{
				upm_tmp_Omega.ctrunc(order, step_exp_table);
			}
			else
			{
				upm_tmp_Omega.ctrunc(order, step_end_exp_table);
			}

			p_Omega_t += upm_tmp_Omega;
		}
	}

	// compute the remainder intervals
	Real factor_k_plus_1;
	factorial_rec[order+1].sup(factor_k_plus_1);

	Real step_pow_k_plus_1, rStep;
	step_end_exp_table[1].sup(rStep);
	step_pow_k_plus_1 = rStep;
	step_pow_k_plus_1.pow_assign_RNDU(order + 1);

	factor_k_plus_1.mul_assign_RNDU(step_pow_k_plus_1);

	iMatrix im_A_t;
	local_A_t.intEval(im_A_t, step_exp_table);

	Real bound_exp_A_delta;
	im_A_t.max_norm(bound_exp_A_delta);
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
			if(connectivity[i][j])
			{
				im_rem[i][j] = intErr;
			}
		}
	}

	iMatrix im_A_t_pow;
	im_A_t.pow(im_A_t_pow, order+1);
	im_rem = im_A_t_pow * im_rem;

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=0; j<rangeDim; ++j)
		{
			if(connectivity[i][j])
			{
				Phi_rem[i][j] = im_rem[i][j];
			}
		}
	}


	if(!bAuto)
	{
		iMatrix im_B_t;
		local_B_t.intEval(im_B_t, step_exp_table);
		Psi_rem = Phi_rem * im_B_t;
		Psi_rem *= step_exp_table[1];
	}

	if(numTIPar > 0)
	{
		iMatrix im_ti_t;
		local_ti_t.intEval(im_ti_t, step_exp_table);
		Omega_rem = Phi_rem * im_ti_t;
		Omega_rem *= step_exp_table[1];
	}

	if(numTVPar > 0)
	{
		upMatrix local_tv_t;
		tv_t.substitute(local_tv_t, up_t);
		local_tv_t.intEval(tv_part, step_exp_table);
	}
}





void compute_one_step_trans_4hybrid(upMatrix & p_Phi_t, upMatrix & p_Psi_t,
		iMatrix & Phi_step_trunc, iMatrix & Phi_step_end_trunc, iMatrix & Phi_rem,
		iMatrix & Psi_step_trunc, iMatrix & Psi_step_end_trunc, iMatrix & Psi_rem, iMatrix & tv_part,
		const upMatrix & A_t, const upMatrix & B_t, const upMatrix & tv_t,
		bMatrix & connectivity, const bool bAuto, const UnivariatePolynomial & up_t, const int order,
		const std::vector<Interval> & step_exp_table, const std::vector<Interval> & step_end_exp_table)
{
	Interval intOne(1);

	int rangeDim = A_t.rows();

	iMatrix identity(rangeDim);
	iMatrix im_zero_Phi(rangeDim, rangeDim), im_zero_Psi(rangeDim, 1);

	Phi_step_trunc = im_zero_Phi;
	Phi_step_end_trunc = im_zero_Phi;
	Phi_rem = im_zero_Phi;

	if(!bAuto)
	{
		Psi_step_trunc = im_zero_Psi;
		Psi_step_end_trunc = im_zero_Psi;
	}

	upMatrix local_A_t;
	A_t.substitute(local_A_t, up_t);
	p_Phi_t = identity;

	upMatrix local_B_t;
	if(!bAuto)
	{
		B_t.substitute(local_B_t, up_t);
		p_Psi_t = local_B_t;
		p_Psi_t.integral();
		p_Psi_t.ctrunc(Psi_step_trunc, Psi_step_end_trunc, order, step_exp_table, step_end_exp_table);
	}

	upMatrix upm_tmp_Phi = p_Phi_t;
	upMatrix upm_tmp_Psi = p_Psi_t;

	// compute the polynomial approximations
	for(int i=1; i<=order; ++i)
	{
		upm_tmp_Phi = local_A_t * upm_tmp_Phi;
		upm_tmp_Phi.integral();

		if(i < order)
		{
			upm_tmp_Phi.ctrunc(order, step_exp_table);
		}
		else
		{
			upm_tmp_Phi.ctrunc(order, step_end_exp_table);
		}

		p_Phi_t += upm_tmp_Phi;

		if(!bAuto)
		{
			upm_tmp_Psi = local_A_t * upm_tmp_Psi;
			upm_tmp_Psi.integral();
			if(i < order)
			{
				upm_tmp_Psi.ctrunc(order, step_exp_table);
			}
			else
			{
				upm_tmp_Psi.ctrunc(order, step_end_exp_table);
			}

			p_Psi_t += upm_tmp_Psi;
		}
	}

	// compute the remainder intervals
	Real factor_k_plus_1;
	factorial_rec[order+1].sup(factor_k_plus_1);

	Real step_pow_k_plus_1, rStep;
	step_end_exp_table[1].sup(rStep);
	step_pow_k_plus_1 = rStep;
	step_pow_k_plus_1.pow_assign_RNDU(order + 1);

	factor_k_plus_1.mul_assign_RNDU(step_pow_k_plus_1);

	iMatrix im_A_t;
	local_A_t.intEval(im_A_t, step_exp_table);

	Real bound_exp_A_delta;
	im_A_t.max_norm(bound_exp_A_delta);
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
			if(connectivity[i][j])
			{
				im_rem[i][j] = intErr;
			}
		}
	}

	iMatrix im_A_t_pow;
	im_A_t.pow(im_A_t_pow, order+1);
	im_rem = im_A_t_pow * im_rem;

	for(int i=0; i<rangeDim; ++i)
	{
		for(int j=0; j<rangeDim; ++j)
		{
			if(connectivity[i][j])
			{
				Phi_rem[i][j] = im_rem[i][j];
			}
		}
	}

	if(!bAuto)
	{
		iMatrix im_B_t;
		local_B_t.intEval(im_B_t, step_exp_table);
		Psi_rem = Phi_rem * im_B_t;
		Psi_rem *= step_exp_table[1];
	}

/*
	if(numTVPar > 0)
	{
		upMatrix local_tv_t;
		tv_t.substitute(local_tv_t, up_t);
		local_tv_t.intEval(tv_part, step_exp_table);
	}
*/
}






void compute_one_step_trans_LTV_SDE(iMatrix2 & Phi_delta, iMatrix2 & Psi_delta, iMatrix2 & Omega_delta,
		const upMatrix & A_t, const upMatrix & B_t, const upMatrix & C_t,
		const std::vector<Interval> step_exp_table, const std::vector<Interval> step_end_exp_table,
		const UnivariatePolynomial & up_t_t0, const int order)
{
	Real rStep;
	step_end_exp_table[1].sup(rStep);

	int rangeDim = A_t.rows();

	// identity matrix
	iMatrix identity(rangeDim);
	iMatrix im_trunc, im_trunc_end;

	upMatrix A_t_t0, p_Phi_t_t0 = identity;
	A_t.substitute(A_t_t0, up_t_t0);
	Phi_delta = identity;

	upMatrix B_t_t0;
	B_t.substitute(B_t_t0, up_t_t0);

	upMatrix C_t_t0;
	C_t.substitute(C_t_t0, up_t_t0);

	upMatrix upm_tmp_A = identity;

	// compute the series for the state-transition matrix
	for(int i=1; i<=order; ++i)
	{
		upm_tmp_A = A_t_t0 * upm_tmp_A;
		upm_tmp_A.ctrunc(order-1, step_exp_table);
		upm_tmp_A.integral();
		p_Phi_t_t0 += upm_tmp_A;
	}

	// compute the series for the inverse of the state-transition matrix
	upMatrix p_Phi_t0_t = identity;
	upm_tmp_A = identity;
	Interval iTmp(-1);
	upMatrix m_A_t_t0 = A_t_t0 * iTmp;

	for(int i=1; i<=order; ++i)
	{
		upm_tmp_A = upm_tmp_A * m_A_t_t0;
		upm_tmp_A.ctrunc(order-1, step_exp_table);
		upm_tmp_A.integral();

		iMatrix im_tmp;
		upm_tmp_A.intEval(im_tmp, step_end_exp_table);
		upm_tmp_A -= im_tmp;

		p_Phi_t0_t += upm_tmp_A;
	}

	upMatrix p_Psi_t_t0 = p_Phi_t0_t * B_t_t0;
	p_Psi_t_t0.integral();


	// compute the remainder intervals
	Real max_norm_A_delta;
	iMatrix im_A_t_t0;
	A_t_t0.intEval(im_A_t_t0, step_end_exp_table);
	im_A_t_t0.max_norm(max_norm_A_delta);
	max_norm_A_delta.mul_assign_RNDU(rStep);

	Real rTmp(1), bound_expansion_exp_A_delta(1);

	for(int i=1; i<=order; ++i)
	{
		rTmp.div_assign_RNDD(i);
		rTmp.mul_assign_RNDD(max_norm_A_delta);

		bound_expansion_exp_A_delta.add_assign_RNDD(rTmp);
	}

	Real bound_exp_A_delta;
	max_norm_A_delta.exp_RNDU(bound_exp_A_delta);

	Real Phi_rem_rad;
	bound_exp_A_delta.sub_RNDU(Phi_rem_rad, bound_expansion_exp_A_delta);
	p_Phi_t_t0.intEval(Phi_delta, step_end_exp_table);
	Phi_delta += Phi_rem_rad;


	Real max_norm_B_delta;
	iMatrix im_B_t_t0;
	B_t_t0.intEval(im_B_t_t0, step_end_exp_table);
	im_B_t_t0.max_norm(max_norm_B_delta);
	max_norm_B_delta.mul_assign_RNDU(rStep);

	Real Psi_rem_rad;
	max_norm_B_delta.mul_RNDU(Psi_rem_rad, Phi_rem_rad);
	p_Psi_t_t0.intEval(Psi_delta, step_end_exp_table);
	Psi_delta += Psi_rem_rad;


	// handle the matrix C(t)
	p_Phi_t0_t += Phi_rem_rad;
	upMatrix p_Phi_t0_t_transpose, C_t_t0_transpose;
	p_Phi_t0_t.transpose(p_Phi_t0_t_transpose);
	C_t_t0.transpose(C_t_t0_transpose);


	upMatrix stcIntegral = p_Phi_t0_t * C_t_t0 * C_t_t0_transpose * p_Phi_t0_t_transpose;

	stcIntegral.integral();
	stcIntegral.intEval(Omega_delta, step_end_exp_table);
}



void compute_one_step_trans_LTV_SDE(iMatrix2 & Phi_delta, iMatrix2 & Psi_delta, iMatrix2 & Omega_delta,
		iMatrix2 & Phi_0_delta, iMatrix2 & Psi_0_delta, iMatrix2 & Omega_0_delta,
		const upMatrix & A_t, const upMatrix & B_t, const upMatrix & C_t,
		const std::vector<Interval> step_exp_table, const std::vector<Interval> step_end_exp_table,
		const UnivariatePolynomial & up_t_t0, const int order)
{
	Real rStep;
	step_end_exp_table[1].sup(rStep);

	int rangeDim = A_t.rows();

	// identity matrix
	iMatrix identity(rangeDim);
	iMatrix im_trunc, im_trunc_end;

	upMatrix A_t_t0, p_Phi_t_t0 = identity;
	A_t.substitute(A_t_t0, up_t_t0);
	Phi_delta = identity;

	upMatrix B_t_t0;
	B_t.substitute(B_t_t0, up_t_t0);

	upMatrix C_t_t0;
	C_t.substitute(C_t_t0, up_t_t0);

	upMatrix upm_tmp_A = identity;

	// compute the series for the state-transition matrix
	for(int i=1; i<=order; ++i)
	{
		upm_tmp_A = A_t_t0 * upm_tmp_A;
		upm_tmp_A.ctrunc(order-1, step_exp_table);
		upm_tmp_A.integral();
		p_Phi_t_t0 += upm_tmp_A;
	}

	// compute the series for the inverse of the state-transition matrix
	upMatrix p_Phi_t0_t = identity;
	upm_tmp_A = identity;
	Interval iTmp(-1);
	upMatrix m_A_t_t0 = A_t_t0 * iTmp;

	for(int i=1; i<=order; ++i)
	{
		upm_tmp_A = upm_tmp_A * m_A_t_t0;
		upm_tmp_A.ctrunc(order-1, step_exp_table);
		upm_tmp_A.integral();

		iMatrix im_tmp;
		upm_tmp_A.intEval(im_tmp, step_end_exp_table);
		upm_tmp_A -= im_tmp;

		p_Phi_t0_t += upm_tmp_A;
	}

	upMatrix p_Psi_t_t0 = p_Phi_t0_t * B_t_t0;
	p_Psi_t_t0.integral();


	// compute the remainder intervals
	Real max_norm_A_delta;
	iMatrix im_A_t_t0;
	A_t_t0.intEval(im_A_t_t0, step_end_exp_table);
	im_A_t_t0.max_norm(max_norm_A_delta);
	max_norm_A_delta.mul_assign_RNDU(rStep);

	Real rTmp(1), bound_expansion_exp_A_delta(1);

	for(int i=1; i<=order; ++i)
	{
		rTmp.div_assign_RNDD(i);
		rTmp.mul_assign_RNDD(max_norm_A_delta);

		bound_expansion_exp_A_delta.add_assign_RNDD(rTmp);
	}

	Real bound_exp_A_delta;
	max_norm_A_delta.exp_RNDU(bound_exp_A_delta);

	Real Phi_rem_rad;
	bound_exp_A_delta.sub_RNDU(Phi_rem_rad, bound_expansion_exp_A_delta);
	p_Phi_t_t0.intEval(Phi_delta, step_end_exp_table);
	Phi_delta += Phi_rem_rad;

	p_Phi_t_t0.intEval(Phi_0_delta, step_exp_table);
	Phi_0_delta += Phi_rem_rad;


	Real max_norm_B_delta;
	iMatrix im_B_t_t0;
	B_t_t0.intEval(im_B_t_t0, step_end_exp_table);
	im_B_t_t0.max_norm(max_norm_B_delta);
	max_norm_B_delta.mul_assign_RNDU(rStep);

	Real Psi_rem_rad;
	max_norm_B_delta.mul_RNDU(Psi_rem_rad, Phi_rem_rad);
	p_Psi_t_t0.intEval(Psi_delta, step_end_exp_table);
	Psi_delta += Psi_rem_rad;

	p_Psi_t_t0.intEval(Psi_0_delta, step_exp_table);
	Psi_0_delta += Psi_rem_rad;


	// handle the matrix C(t)
	p_Phi_t0_t += Phi_rem_rad;
	upMatrix p_Phi_t0_t_transpose, C_t_t0_transpose;
	p_Phi_t0_t.transpose(p_Phi_t0_t_transpose);
	C_t_t0.transpose(C_t_t0_transpose);


	upMatrix stcIntegral = p_Phi_t0_t * C_t_t0 * C_t_t0_transpose * p_Phi_t0_t_transpose;

	stcIntegral.integral();
	stcIntegral.intEval(Omega_delta, step_end_exp_table);
	stcIntegral.intEval(Omega_0_delta, step_exp_table);
}


int safetyChecking2(const TaylorModelVec & flowpipe, const std::vector<Interval> & domain, const std::vector<PolynomialConstraint> & unsafeSet, const int order, const Interval & cutoff_threshold)
{
//	int rangeDim = flowpipe.tms.size();
	int result = UNKNOWN;
	bool bContained = true;

	std::vector<Interval> tmvPolyRange;
	flowpipe.polyRange(tmvPolyRange, domain);

	std::vector<HornerForm> obj_hfs;
	std::vector<Interval> obj_rems;

	for(int i=0; i<unsafeSet.size(); ++i)
	{
		TaylorModel tmTemp;

		// interval evaluation on the constraint
		unsafeSet[i].hf.insert_ctrunc(tmTemp, flowpipe, tmvPolyRange, domain, order, cutoff_threshold);

		HornerForm hf;
		tmTemp.expansion.toHornerForm(hf);

		obj_hfs.push_back(hf);
		obj_rems.push_back(tmTemp.remainder);

		Interval intTemp;
		hf.intEval(intTemp, domain);
		intTemp += tmTemp.remainder;

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

	if(result == UNKNOWN)
	{
		if(bContained)
		{
			return UNSAFE;
		}
		else
		{
			// do a simple branch & bound for safety checking
			std::vector<Interval> refined_domain = domain;

			std::list<Interval> subdivisions;
			Interval intLeft, intRight;
			domain[0].split(intLeft, intRight);

			result = SAFE;

			if(intLeft.width() > REFINEMENT_PREC)
			{
				subdivisions.push_back(intLeft);
			}

			if(intRight.width() > REFINEMENT_PREC)
			{
				subdivisions.push_back(intRight);
			}

			for(; subdivisions.size() > 0; )
			{
				Interval subdivision = subdivisions.front();
				subdivisions.pop_front();

				int result_iter = UNKNOWN;
				bool bContained_iter = true;

				refined_domain[0] = subdivision;

				for(int i=0; i<unsafeSet.size(); ++i)
				{
					Interval intTemp;
					obj_hfs[i].intEval(intTemp, refined_domain);
					intTemp += obj_rems[i];

					if(intTemp > unsafeSet[i].B)
					{
						// no intersection with the unsafe set
						result_iter = SAFE;
						break;
					}
					else
					{
						if(!intTemp.smallereq(unsafeSet[i].B) && bContained_iter)
						{
							bContained_iter = false;
						}
					}
				}

				if(result_iter == UNKNOWN)
				{
					if(bContained_iter)
					{
						return UNSAFE;
					}
					else
					{
						// split the domain
						Interval I1, I2;
						subdivision.split(I1, I2);

						if(I1.width() <= REFINEMENT_PREC)
						{
							if(result == SAFE)
								result = UNKNOWN;
						}
						else
						{
							subdivisions.push_back(I1);
						}

						if(I2.width() <= REFINEMENT_PREC)
						{
							if(result == SAFE)
								result = UNKNOWN;
						}
						else
						{
							subdivisions.push_back(I2);
						}
					}
				}
			}

			return result;
		}
	}
	else
	{
		return SAFE;
	}
}


/*
void build_constraint_template(const int d)
{
	vector<Interval> intVecZero;
	Interval intZero, intOne(1), intMOne(-1);

	for(int i=0; i<d; ++i)
	{
		intVecZero.push_back(intZero);
	}

	for(int i=0; i<d; ++i)
	{
		vector<Interval> A = intVecZero;
		A[i] = intOne;

		LinearConstraint lc(A, intZero);
		constraint_template.push_back(lc);
	}

	for(int i=0; i<d; ++i)
	{
		vector<Interval> A = intVecZero;
		A[i] = intMOne;

		LinearConstraint lc(A, intZero);
		constraint_template.push_back(lc);
	}
}
*/

}

void plot_2D_box_gnuplot(FILE *fp, const std::string & epsFileName, std::list<Flowpipe> & flowpipes, const std::string & x, const std::string & y, const Variables & variables, const Interval & cutoff)
{
	fprintf(fp, "set terminal postscript enhanced color\n");

	char filename[NAME_SIZE+10];
	sprintf(filename, "%s.eps", epsFileName.c_str());
	fprintf(fp, "set output '%s'\n", filename);

	fprintf(fp, "set style line 1 linecolor rgb \"blue\"\n");
	fprintf(fp, "set autoscale\n");
	fprintf(fp, "unset label\n");
	fprintf(fp, "set xtic auto\n");
	fprintf(fp, "set ytic auto\n");
	fprintf(fp, "set xlabel \"%s\"\n", x.c_str());
	fprintf(fp, "set ylabel \"%s\"\n", y.c_str());
	fprintf(fp, "plot '-' notitle with lines ls 1\n");

	int idx = variables.getIDForVar(x) - 1;
	int idy = variables.getIDForVar(y) - 1;

	std::vector<flowstar::Interval> step_exp_table, step_end_exp_table;
	Interval intStep;

	std::list<Flowpipe>::const_iterator fpIter = flowpipes.begin();

	int maxOrder = 0;

	for(; fpIter != flowpipes.end(); ++fpIter)
	{
		int tmp = maxOrder;
		for(int i=0; i<fpIter->tmvPre.tms.size(); ++i)
		{
			int order = fpIter->tmvPre.tms[i].degree();
			if(maxOrder < order)
			{
				maxOrder = order;
			}
		}

		for(int i=0; i<fpIter->tmv.tms.size(); ++i)
		{
			int order = fpIter->tmv.tms[i].degree();
			if(maxOrder < order)
			{
				maxOrder = order;
			}
		}

		if(step_exp_table.size() == 0 || intStep != fpIter->domain[0] || maxOrder > tmp)
		{
			flowstar::construct_step_exp_table(step_exp_table, step_end_exp_table, fpIter->domain[0].sup(), maxOrder);
			intStep = fpIter->domain[0];
		}

		std::vector<flowstar::Interval> box;
		flowstar::TaylorModelVec tmv;
		fpIter->composition_normal(tmv, step_exp_table, (flowstar::Interval)cutoff);

		tmv.intEvalNormal(box, step_exp_table);

		Interval X(box[idx]), Y(box[idy]);

		fprintf(fp, "%lf %lf\n", X.inf(), Y.inf());
		fprintf(fp, "%lf %lf\n", X.sup(), Y.inf());
		fprintf(fp, "%lf %lf\n", X.sup(), Y.sup());
		fprintf(fp, "%lf %lf\n", X.inf(), Y.sup());
		fprintf(fp, "%lf %lf\n", X.inf(), Y.inf());
		fprintf(fp, "\n\n");
	}

	fprintf(fp, "e\n");
}
