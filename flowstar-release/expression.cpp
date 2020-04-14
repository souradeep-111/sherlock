#include "expression.h"

using namespace flowstar;

namespace flowstar
{
ParseExpression parseExpression;
}

AST_Node::AST_Node()
{
	node_type = -1;
}

AST_Node::AST_Node(const int opt_type, const std::shared_ptr<AST_Node> & left)
{
	node_type = NODE_UNA_OPT;

	node_value.opt.type = opt_type;
	node_value.opt.left_operand = left;
	node_value.opt.right_operand = nullptr;
}

AST_Node::AST_Node(const int opt_type, const std::shared_ptr<AST_Node> & left,const std::shared_ptr<AST_Node> & right)
{
	node_type = NODE_BIN_OPT;

	node_value.opt.type = opt_type;
	node_value.opt.left_operand = left;
	node_value.opt.right_operand = right;
}

AST_Node::AST_Node(const int var_type, const int var_id)
{
	node_type = NODE_VAR;

	node_value.var.type = var_type;
	node_value.var.id = var_id;
}

AST_Node::AST_Node(const Interval & I)
{
	node_type = NODE_CONST;
	node_value.constant = I;
}

AST_Node::~AST_Node()
{
}

void AST_Node::evaluate(Interval & result, const Taylor_Model_Computation_Setting & setting) const
{
	switch(node_type)
	{
	case NODE_UNA_OPT:
	{
		node_value.opt.left_operand->evaluate(result, setting);

		switch(node_value.opt.type)
		{
		case OPT_NEG:
			result.inv_assign();
			break;

		case OPT_SIN:
			result.sin_assign();
			break;

		case OPT_COS:
			result.cos_assign();
			break;

		case OPT_EXP:
			result.exp_assign();
			break;

		case OPT_LOG:
			result.log_assign();
			break;

		case OPT_SQRT:
			result.sqrt_assign();
			break;
		}

		break;
	}
	case NODE_BIN_OPT:
	{
		Interval I1, I2;

		switch(node_value.opt.type)
		{
		case OPT_PLUS:
			node_value.opt.left_operand->evaluate(I1, setting);
			node_value.opt.right_operand->evaluate(I2, setting);
			result = I1 + I2;
			break;
		case OPT_MINU:
			node_value.opt.left_operand->evaluate(I1, setting);
			node_value.opt.right_operand->evaluate(I2, setting);
			result = I1 - I2;
			break;
		case OPT_MULT:
			node_value.opt.left_operand->evaluate(I1, setting);
			node_value.opt.right_operand->evaluate(I2, setting);
			result = I1 * I2;
			break;
		case OPT_DIV:
			node_value.opt.left_operand->evaluate(I1, setting);
			node_value.opt.right_operand->evaluate(I2, setting);
			result = I1 / I2;
			break;
		case OPT_POW:
			node_value.opt.left_operand->evaluate(result, setting);
			result.pow_assign((int)node_value.opt.right_operand->node_value.constant.sup());
			break;
		}

		break;
	}

	case NODE_VAR:
		if(node_value.var.type == VAR_ID)
		{
			result = setting.domain[node_value.var.id];
		}
		else
		{
			// for the other variable types
		}

		break;

	case NODE_CONST:
		result = node_value.constant;
		break;
	}
}

void AST_Node::evaluate(Real & result, const std::vector<Real> & values_of_vars) const
{
	switch(node_type)
	{
	case NODE_UNA_OPT:
	{
		node_value.opt.left_operand->evaluate(result, values_of_vars);

		switch(node_value.opt.type)
		{
		case OPT_NEG:
			result *= -1.0;
			break;

		case OPT_SIN:
			result.sin_assign();
			break;

		case OPT_COS:
			result.cos_assign();
			break;

		case OPT_EXP:
			result.exp_assign();
			break;

		case OPT_LOG:
			result.log_assign();
			break;

		case OPT_SQRT:
			result.sqrt_assign();
			break;
		}

		break;
	}
	case NODE_BIN_OPT:
	{
		Real r1, r2;

		switch(node_value.opt.type)
		{
		case OPT_PLUS:
			node_value.opt.left_operand->evaluate(r1, values_of_vars);
			node_value.opt.right_operand->evaluate(r2, values_of_vars);
			result = r1 + r2;
			break;
		case OPT_MINU:
			node_value.opt.left_operand->evaluate(r1, values_of_vars);
			node_value.opt.right_operand->evaluate(r2, values_of_vars);
			result = r1 - r2;
			break;
		case OPT_MULT:
			node_value.opt.left_operand->evaluate(r1, values_of_vars);
			node_value.opt.right_operand->evaluate(r2, values_of_vars);
			result = r1 * r2;
			break;
		case OPT_DIV:
			node_value.opt.left_operand->evaluate(r1, values_of_vars);
			node_value.opt.right_operand->evaluate(r2, values_of_vars);
			result = r1 / r2;
			break;
		case OPT_POW:
			node_value.opt.left_operand->evaluate(result, values_of_vars);
			result.pow_assign((int)node_value.opt.right_operand->node_value.constant.sup());
			break;
		}

		break;
	}

	case NODE_VAR:
		if(node_value.var.type == VAR_ID)
		{
			result = values_of_vars[node_value.var.id];
		}
		else
		{
			// for the other variable types
		}

		break;

	case NODE_CONST:
		node_value.constant.midpoint(result);
		break;
	}
}

void AST_Node::evaluate(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const Taylor_Model_Computation_Setting & setting) const
{
	switch(node_type)
	{
	case NODE_UNA_OPT:
	{
		TaylorModel tmTemp;
		node_value.opt.left_operand->evaluate(tmTemp, tms_of_vars, order, setting);

		switch(node_value.opt.type)
		{
		case OPT_NEG:
			tmTemp.inv(result);
			break;

		case OPT_SIN:
			tmTemp.sin_taylor(result, order, setting);
			break;

		case OPT_COS:
			tmTemp.cos_taylor(result, order, setting);
			break;

		case OPT_EXP:
			tmTemp.exp_taylor(result, order, setting);
			break;

		case OPT_LOG:
			tmTemp.log_taylor(result, order, setting);
			break;

		case OPT_SQRT:
			tmTemp.sqrt_taylor(result, order, setting);
			break;
		}
		break;
	}
	case NODE_BIN_OPT:
	{
		TaylorModel tm1, tm2;

		switch(node_value.opt.type)
		{
		case OPT_PLUS:
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, setting);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, setting);
			tm1.add(result, tm2);
			break;
		case OPT_MINU:
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, setting);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, setting);
			tm1.sub(result, tm2);
			break;
		case OPT_MULT:
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, setting);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, setting);
			tm1.mul_ctrunc(result, tm2, setting.domain, order, setting.cutoff_threshold);
			break;

		case OPT_DIV:
		{
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, setting);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, setting);

			TaylorModel tmTemp;
			tm2.rec_taylor(tmTemp, order, setting);
			tm1.mul_ctrunc(result, tmTemp, setting.domain, order, setting.cutoff_threshold);

			break;
		}

		case OPT_POW:
		{
			node_value.opt.left_operand->evaluate(result, tms_of_vars, order, setting);

			int degree = (int)node_value.opt.right_operand->node_value.constant.sup();

			if(degree == 0)
			{
				TaylorModel tm(1, setting.vars.size());
				result = tm;
			}
			else if(degree > 1)
			{
				TaylorModel temp = result;

				for(int i = degree - 1; i > 0;)
				{
					if(i & 1)
					{
						// result *= temp
						result.mul_ctrunc_assign(temp, setting.domain, order, setting.cutoff_threshold);
					}

					i >>= 1;

					if(i > 0)
					{
						// temp *= temp
						temp.mul_ctrunc_assign(temp, setting.domain, order, setting.cutoff_threshold);
					}
				}
			}

			break;
		}
		}

		break;
	}

	case NODE_VAR:
		if(node_value.var.type == VAR_ID)
		{
			result = tms_of_vars[node_value.var.id - 1];
		}
		else
		{
			// for the other variable types
		}

		break;

	case NODE_CONST:
	{
		TaylorModel temp(node_value.constant, setting.vars.size());
		result = temp;
		break;
	}
	}
}

void AST_Node::evaluate(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold, const int numVars) const
{
	switch(node_type)
	{
	case NODE_UNA_OPT:
	{
		TaylorModel tmTemp;
		node_value.opt.left_operand->evaluate(tmTemp, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars);

		switch(node_value.opt.type)
		{
		case OPT_NEG:
			tmTemp.inv(result);
			break;

		case OPT_SIN:
			tmTemp.sin_taylor(result, step_exp_table, numVars, order, cutoff_threshold);
			break;

		case OPT_COS:
			tmTemp.cos_taylor(result, step_exp_table, numVars, order, cutoff_threshold);
			break;

		case OPT_EXP:
			tmTemp.exp_taylor(result, step_exp_table, numVars, order, cutoff_threshold);
			break;

		case OPT_LOG:
			tmTemp.log_taylor(result, step_exp_table, numVars, order, cutoff_threshold);
			break;

		case OPT_SQRT:
			tmTemp.sqrt_taylor(result, step_exp_table, numVars, order, cutoff_threshold);
			break;
		}
		break;
	}
	case NODE_BIN_OPT:
	{
		TaylorModel tm1, tm2;

		switch(node_value.opt.type)
		{
		case OPT_PLUS:
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars);
			tm1.add(result, tm2);
			break;
		case OPT_MINU:
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars);
			tm1.sub(result, tm2);
			break;
		case OPT_MULT:
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars);
			tm1.mul_ctrunc_normal(result, tm2, step_exp_table, order, cutoff_threshold);
			break;

		case OPT_DIV:
		{
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars);

			TaylorModel tmTemp;
			tm2.rec_taylor(tmTemp, step_exp_table, numVars, order, cutoff_threshold);

			tm1.mul_ctrunc_normal(result, tmTemp, step_exp_table, order, cutoff_threshold);

			break;
		}

		case OPT_POW:
		{
			node_value.opt.left_operand->evaluate(result, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars);

			int degree = (int)node_value.opt.right_operand->node_value.constant.sup();

			if(degree == 0)
			{
				TaylorModel tm(1, numVars);
				result = tm;
			}
			else if(degree > 1)
			{
				TaylorModel temp = result;

				for(int i = degree - 1; i > 0;)
				{
					if(i & 1)
					{
						// result *= temp
						result.mul_ctrunc_normal_assign(temp, step_exp_table, order, cutoff_threshold);
					}

					i >>= 1;

					if(i > 0)
					{
						// temp *= temp
						temp.mul_ctrunc_normal_assign(temp, step_exp_table, order, cutoff_threshold);
					}
				}
			}

			break;
		}
		}

		break;
	}

	case NODE_VAR:
		if(node_value.var.type == VAR_ID)
		{
			result = tms_of_vars[node_value.var.id - 1];
		}
		else
		{
			// for the other variable types
		}

		break;

	case NODE_CONST:
	{
		TaylorModel temp(node_value.constant, numVars);
		result = temp;
		break;
	}
	}
}

void AST_Node::evaluate_no_remainder(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const Interval & cutoff_threshold, const int numVars) const
{
	Interval intZero;

	result.remainder = intZero;

	switch(node_type)
	{
	case NODE_UNA_OPT:
	{
		TaylorModel tmTemp;
		node_value.opt.left_operand->evaluate_no_remainder(tmTemp, tms_of_vars, order, cutoff_threshold, numVars);

		switch(node_value.opt.type)
		{
		case OPT_NEG:
			tmTemp.expansion.inv(result.expansion);
			break;

		case OPT_SIN:
			tmTemp.expansion.sin_taylor(result.expansion, numVars, order, cutoff_threshold);
			break;

		case OPT_COS:
			tmTemp.expansion.cos_taylor(result.expansion, numVars, order, cutoff_threshold);
			break;

		case OPT_EXP:
			tmTemp.expansion.exp_taylor(result.expansion, numVars, order, cutoff_threshold);
			break;

		case OPT_LOG:
			tmTemp.expansion.log_taylor(result.expansion, numVars, order, cutoff_threshold);
			break;

		case OPT_SQRT:
			tmTemp.expansion.sqrt_taylor(result.expansion, numVars, order, cutoff_threshold);
			break;
		}
		break;
	}
	case NODE_BIN_OPT:
	{
		TaylorModel tm1, tm2;

		switch(node_value.opt.type)
		{
		case OPT_PLUS:
			node_value.opt.left_operand->evaluate_no_remainder(tm1, tms_of_vars, order, cutoff_threshold, numVars);
			node_value.opt.right_operand->evaluate_no_remainder(tm2, tms_of_vars, order, cutoff_threshold, numVars);
			tm1.add(result, tm2);
			break;
		case OPT_MINU:
			node_value.opt.left_operand->evaluate_no_remainder(tm1, tms_of_vars, order, cutoff_threshold, numVars);
			node_value.opt.right_operand->evaluate_no_remainder(tm2, tms_of_vars, order, cutoff_threshold, numVars);
			tm1.sub(result, tm2);
			break;
		case OPT_MULT:
			node_value.opt.left_operand->evaluate_no_remainder(tm1, tms_of_vars, order, cutoff_threshold, numVars);
			node_value.opt.right_operand->evaluate_no_remainder(tm2, tms_of_vars, order, cutoff_threshold, numVars);
			result.expansion = tm1.expansion * tm2.expansion;
			result.expansion.nctrunc(order);
			result.expansion.cutoff(cutoff_threshold);
			break;

		case OPT_DIV:
		{
			node_value.opt.left_operand->evaluate_no_remainder(tm1, tms_of_vars, order, cutoff_threshold, numVars);
			node_value.opt.right_operand->evaluate_no_remainder(tm2, tms_of_vars, order, cutoff_threshold, numVars);

			Polynomial polyTemp;
			tm2.expansion.rec_taylor(polyTemp, numVars, order, cutoff_threshold);

			result.expansion = tm1.expansion * polyTemp;
			result.expansion.nctrunc(order);
			result.expansion.cutoff(cutoff_threshold);

			break;
		}

		case OPT_POW:
		{
			node_value.opt.left_operand->evaluate_no_remainder(result, tms_of_vars, order, cutoff_threshold, numVars);

			int degree = (int)node_value.opt.right_operand->node_value.constant.sup();

			if(degree == 0)
			{
				TaylorModel tm(1, numVars);
				result = tm;
			}
			else if(degree > 1)
			{
				TaylorModel temp = result;

				for(int i = degree - 1; i > 0;)
				{
					if(i & 1)
					{
						// result *= temp
						result.expansion *= temp.expansion;
						result.expansion.nctrunc(order);
						result.expansion.cutoff(cutoff_threshold);
					}

					i >>= 1;

					if(i > 0)
					{
						// temp *= temp
						temp.expansion *= temp.expansion;
						temp.expansion.nctrunc(order);
						temp.expansion.cutoff(cutoff_threshold);
					}
				}
			}

			break;
		}
		}

		break;
	}

	case NODE_VAR:
		if(node_value.var.type == VAR_ID)
		{
			result.expansion = tms_of_vars[node_value.var.id - 1].expansion;
		}
		else
		{
			// for the other variable types
		}

		break;

	case NODE_CONST:
	{
		TaylorModel temp(node_value.constant, numVars);
		result = temp;
		break;
	}
	}
}

void AST_Node::evaluate(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold, const int numVars, std::list<Interval> & intermediate_ranges) const
{
	switch(node_type)
	{
	case NODE_UNA_OPT:
	{
		TaylorModel tmTemp;
		node_value.opt.left_operand->evaluate(tmTemp, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);

		switch(node_value.opt.type)
		{
		case OPT_NEG:
			tmTemp.inv(result);
			break;

		case OPT_SIN:
			tmTemp.sin_taylor(result, intermediate_ranges, step_exp_table, numVars, order, cutoff_threshold);
			break;

		case OPT_COS:
			tmTemp.cos_taylor(result, intermediate_ranges, step_exp_table, numVars, order, cutoff_threshold);
			break;

		case OPT_EXP:
			tmTemp.exp_taylor(result, intermediate_ranges, step_exp_table, numVars, order, cutoff_threshold);
			break;

		case OPT_LOG:
			tmTemp.log_taylor(result, intermediate_ranges, step_exp_table, numVars, order, cutoff_threshold);
			break;

		case OPT_SQRT:
			tmTemp.sqrt_taylor(result, intermediate_ranges, step_exp_table, numVars, order, cutoff_threshold);
			break;
		}
		break;
	}
	case NODE_BIN_OPT:
	{
		TaylorModel tm1, tm2;

		switch(node_value.opt.type)
		{
		case OPT_PLUS:
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);
			tm1.add(result, tm2);
			break;
		case OPT_MINU:
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);
			tm1.sub(result, tm2);
			break;
		case OPT_MULT:
		{
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);

			Interval intPoly1, intPoly2, intTrunc;

			tm2.polyRangeNormal(intPoly2, step_exp_table);
			tm1.mul_insert_ctrunc_normal(result, intPoly1, intTrunc, tm2, intPoly2, step_exp_table, order, cutoff_threshold);

			intermediate_ranges.push_back(intPoly1);
			intermediate_ranges.push_back(intPoly2);
			intermediate_ranges.push_back(intTrunc);

			break;
		}

		case OPT_DIV:
		{
			node_value.opt.left_operand->evaluate(tm1, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);
			node_value.opt.right_operand->evaluate(tm2, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);

			TaylorModel tmTemp;
			tm2.rec_taylor(tmTemp, intermediate_ranges, step_exp_table, numVars, order, cutoff_threshold);

			Interval intPoly1, intPoly2, intTrunc;

			tmTemp.polyRangeNormal(intPoly2, step_exp_table);
			tm1.mul_insert_ctrunc_normal_assign(intPoly1, intTrunc, tmTemp, intPoly2, step_exp_table, order, cutoff_threshold);

			intermediate_ranges.push_back(intPoly1);
			intermediate_ranges.push_back(intPoly2);
			intermediate_ranges.push_back(intTrunc);

			break;
		}

		case OPT_POW:
		{
			node_value.opt.left_operand->evaluate(result, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);

			int degree = (int)node_value.opt.right_operand->node_value.constant.sup();

			if(degree == 0)
			{
				TaylorModel tm(1, numVars);
				result = tm;
			}
			else if(degree > 1)
			{
				TaylorModel temp = result;
				Interval intPoly1, intPoly2, intTrunc;

				for(int i = degree - 1; i > 0;)
				{
					temp.polyRangeNormal(intPoly2, step_exp_table);

					if(i & 1)
					{
						// result *= temp
						result.mul_insert_ctrunc_normal_assign(intPoly1, intTrunc, temp, intPoly2, step_exp_table, order, cutoff_threshold);

						intermediate_ranges.push_back(intPoly1);
						intermediate_ranges.push_back(intPoly2);
						intermediate_ranges.push_back(intTrunc);
					}

					i >>= 1;

					if(i > 0)
					{
						// temp *= temp
						temp.mul_insert_ctrunc_normal_assign(intPoly1, intTrunc, temp, intPoly2, step_exp_table, order, cutoff_threshold);

						intermediate_ranges.push_back(intPoly1);
						intermediate_ranges.push_back(intPoly2);
						intermediate_ranges.push_back(intTrunc);
					}
				}
			}

			break;
		}
		}

		break;
	}

	case NODE_VAR:
		if(node_value.var.type == VAR_ID)
		{
			result = tms_of_vars[node_value.var.id - 1];
		}
		else
		{
			// for the other variable types
		}

		break;

	case NODE_CONST:
	{
		TaylorModel temp(node_value.constant, numVars);
		result = temp;
		break;
	}
	}
}

void AST_Node::evaluate_remainder(Interval & result, const std::vector<TaylorModel> & tms_of_vars, const int order, std::list<Interval>::iterator & iter) const
{
	switch(node_type)
	{
	case NODE_UNA_OPT:
	{
		Interval intTemp;
		node_value.opt.left_operand->evaluate_remainder(intTemp, tms_of_vars, order, iter);

		switch(node_value.opt.type)
		{
		case OPT_NEG:
			intTemp.inv(result);
			break;

		case OPT_SIN:
			sin_taylor_only_remainder(result, intTemp, iter, order);
			break;

		case OPT_COS:
			cos_taylor_only_remainder(result, intTemp, iter, order);
			break;

		case OPT_EXP:
			exp_taylor_only_remainder(result, intTemp, iter, order);
			break;

		case OPT_LOG:
			log_taylor_only_remainder(result, intTemp, iter, order);
			break;

		case OPT_SQRT:
			sqrt_taylor_only_remainder(result, intTemp, iter, order);
			break;
		}
		break;
	}
	case NODE_BIN_OPT:
	{
		Interval remainder1, remainder2;

		switch(node_value.opt.type)
		{
		case OPT_PLUS:
			node_value.opt.left_operand->evaluate_remainder(remainder1, tms_of_vars, order, iter);
			node_value.opt.right_operand->evaluate_remainder(remainder2, tms_of_vars, order, iter);
			result = remainder1 + remainder2;
			break;
		case OPT_MINU:
			node_value.opt.left_operand->evaluate_remainder(remainder1, tms_of_vars, order, iter);
			node_value.opt.right_operand->evaluate_remainder(remainder2, tms_of_vars, order, iter);
			result = remainder1 - remainder2;
			break;
		case OPT_MULT:
			node_value.opt.left_operand->evaluate_remainder(remainder1, tms_of_vars, order, iter);
			node_value.opt.right_operand->evaluate_remainder(remainder2, tms_of_vars, order, iter);

			result = (*iter) * remainder2;
			++iter;
			result += (*iter) * remainder1;
			result += remainder1 * remainder2;
			++iter;
			result += (*iter);
			++iter;

			break;

		case OPT_DIV:
		{
			node_value.opt.left_operand->evaluate_remainder(remainder1, tms_of_vars, order, iter);
			node_value.opt.right_operand->evaluate_remainder(remainder2, tms_of_vars, order, iter);

			Interval intTemp;
			rec_taylor_only_remainder(intTemp, remainder2, iter, order);

			result = (*iter) * intTemp;
			++iter;
			result += (*iter) * remainder1;
			result += remainder1 * intTemp;
			++iter;
			result += (*iter);
			++iter;
			break;
		}

		case OPT_POW:
		{
			node_value.opt.left_operand->evaluate_remainder(result, tms_of_vars, order, iter);

			int degree = (int)node_value.opt.right_operand->node_value.constant.sup();

			if(degree == 0)
			{
				Interval intZero;
				result = intZero;
			}
			else if(degree > 1)
			{
				Interval temp = result;

				for(int i = degree - 1; i > 0;)
				{
					if(i & 1)
					{
						Interval temp2;
						temp2 = (*iter) * temp;
						++iter;
						temp2 += (*iter) * result;
						temp2 += temp * result;
						++iter;
						temp2 += (*iter);
						++iter;

						result = temp2;
					}

					i >>= 1;

					if(i > 0)
					{
						Interval temp2;
						temp2 = (*iter) * temp;
						++iter;
						temp2 += (*iter) * temp;
						temp2 += temp * temp;
						++iter;
						temp2 += (*iter);
						++iter;

						temp = temp2;
					}
				}
			}

			break;
		}
		}

		break;
	}

	case NODE_VAR:
		if(node_value.var.type == VAR_ID)
		{
			result = tms_of_vars[node_value.var.id - 1].remainder;
		}
		else
		{
			// for the other variable types
		}

		break;

	case NODE_CONST:
	{
		Interval intZero;
		result = intZero;
		break;
	}
	}
}

void AST_Node::center()
{
	switch(node_type)
	{
	case NODE_UNA_OPT:
	{
		node_value.opt.left_operand->center();
		break;
	}
	case NODE_BIN_OPT:
	{
		node_value.opt.left_operand->center();
		node_value.opt.right_operand->center();
		break;
	}
	case NODE_CONST:
	{
		Interval M;
		node_value.constant.midpoint(M);
		node_value.constant = M;
		break;
	}
	}
}

void AST_Node::output(std::string & expression, const Taylor_Model_Computation_Setting & setting) const
{
	switch(node_type)
	{
	case NODE_UNA_OPT:
	{
		std::string temp;
		node_value.opt.left_operand->output(temp, setting);

		switch(node_value.opt.type)
		{
		case OPT_NEG:
			expression = "(-" + temp + ")";
			break;

		case OPT_SIN:
			expression = "(SIN(" + temp + "))";
			break;

		case OPT_COS:
			expression = "(COS(" + temp + "))";
			break;

		case OPT_EXP:
			expression = "(EXP(" + temp + "))";
			break;

		case OPT_LOG:
			expression = "(LOG(" + temp + "))";
			break;

		case OPT_SQRT:
			expression = "(SQRT(" + temp + "))";
			break;
		}
		break;
	}
	case NODE_BIN_OPT:
	{
		std::string temp1, temp2;

		switch(node_value.opt.type)
		{
		case OPT_PLUS:
			node_value.opt.left_operand->output(temp1, setting);
			node_value.opt.right_operand->output(temp2, setting);
			expression = "(" + temp1 + "+" + temp2 + ")";
			break;
		case OPT_MINU:
			node_value.opt.left_operand->output(temp1, setting);
			node_value.opt.right_operand->output(temp2, setting);
			expression = "(" + temp1 + "-" + temp2 + ")";
			break;
		case OPT_MULT:
			node_value.opt.left_operand->output(temp1, setting);
			node_value.opt.right_operand->output(temp2, setting);
			expression = "(" + temp1 + "*" + temp2 + ")";
			break;
		case OPT_DIV:
			node_value.opt.left_operand->output(temp1, setting);
			node_value.opt.right_operand->output(temp2, setting);
			expression = "(" + temp1 + "/" + temp2 + ")";
			break;
		case OPT_POW:
			node_value.opt.left_operand->output(temp1, setting);
			expression = temp1 + "^" + std::to_string((int)node_value.opt.right_operand->node_value.constant.sup());
			break;
		}

		break;
	}

	case NODE_VAR:
		if(node_value.var.type == VAR_ID)
		{
			setting.vars.getVarName(expression, node_value.var.id);
		}
		else
		{
			setting.pars.getVarName(expression, node_value.var.id);
		}

		break;

	case NODE_CONST:
		node_value.constant.toString(expression);
		break;
	}
}

void AST_Node::output(std::string & expression, const Variables & variables) const
{
	switch(node_type)
	{
	case NODE_UNA_OPT:
	{
		std::string temp;
		node_value.opt.left_operand->output(temp, variables);

		switch(node_value.opt.type)
		{
		case OPT_NEG:
			expression = "(-" + temp + ")";
			break;

		case OPT_SIN:
			expression = "(SIN(" + temp + "))";
			break;

		case OPT_COS:
			expression = "(COS(" + temp + "))";
			break;

		case OPT_EXP:
			expression = "(EXP(" + temp + "))";
			break;

		case OPT_LOG:
			expression = "(LOG(" + temp + "))";
			break;

		case OPT_SQRT:
			expression = "(SQRT(" + temp + "))";
			break;
		}
		break;
	}
	case NODE_BIN_OPT:
	{
		std::string temp1, temp2;

		switch(node_value.opt.type)
		{
		case OPT_PLUS:
			node_value.opt.left_operand->output(temp1, variables);
			node_value.opt.right_operand->output(temp2, variables);
			expression = "(" + temp1 + "+" + temp2 + ")";
			break;
		case OPT_MINU:
			node_value.opt.left_operand->output(temp1, variables);
			node_value.opt.right_operand->output(temp2, variables);
			expression = "(" + temp1 + "-" + temp2 + ")";
			break;
		case OPT_MULT:
			node_value.opt.left_operand->output(temp1, variables);
			node_value.opt.right_operand->output(temp2, variables);
			expression = "(" + temp1 + "*" + temp2 + ")";
			break;
		case OPT_DIV:
			node_value.opt.left_operand->output(temp1, variables);
			node_value.opt.right_operand->output(temp2, variables);
			expression = "(" + temp1 + "/" + temp2 + ")";
			break;
		case OPT_POW:
			node_value.opt.left_operand->output(temp1, variables);
			expression = temp1 + "^" + std::to_string((int)node_value.opt.right_operand->node_value.constant.sup());
			break;
		}

		break;
	}

	case NODE_VAR:
		if(node_value.var.type == VAR_ID)
		{
			variables.getVarName(expression, node_value.var.id);
		}
		else
		{
			variables.getVarName(expression, node_value.var.id);
		}

		break;

	case NODE_CONST:
		node_value.constant.toString(expression);
		break;
	}
}






Expression_AST::Expression_AST()
{
	root = nullptr;
}

Expression_AST::Expression_AST(const std::string & varName, const Taylor_Model_Computation_Setting & setting)
{
	int id = setting.vars.getIDForVar(varName);

	if(id < 0)
	{
		printf("Variable %s is not declared.\n", varName.c_str());
		exit(0);
	}

	root = std::shared_ptr<AST_Node> (new AST_Node(VAR_ID, id));
}

Expression_AST::Expression_AST(const std::string & varName, const Variables & variables, const int dummy)
{
	int id = variables.getIDForVar(varName);

	if(id < 0)
	{
		printf("Variable %s is not declared.\n", varName.c_str());
		exit(0);
	}

	root = std::shared_ptr<AST_Node> (new AST_Node(VAR_ID, id));
}

Expression_AST::Expression_AST(const double constant)
{
	root = std::shared_ptr<AST_Node> (new AST_Node(constant));
}

Expression_AST::Expression_AST(const Interval & constant)
{
	root = std::shared_ptr<AST_Node> (new AST_Node(constant));
}

Expression_AST::~Expression_AST()
{
	root.reset();
}

Expression_AST::Expression_AST(const std::string & strExpression, const Variables & vars)
{
	parseExpression.clear();

	std::string prefix(str_prefix_expression);
	std::string suffix(str_suffix);

	parseExpression.strExpression = prefix + strExpression + suffix;
	parseExpression.variables = vars;

	parse_ODE_Expression();

	*this = parseExpression.expression;
}

void Expression_AST::output(FILE *fp, const Taylor_Model_Computation_Setting & setting) const
{
	std::string expression;
	root->output(expression, setting);

	printf("%s\n", expression.c_str());
}

void Expression_AST::output(FILE *fp, const Variables & variables) const
{
	std::string expression;
	root->output(expression, variables);

	printf("%s\n", expression.c_str());
}

void Expression_AST::evaluate_no_remainder(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const Interval & cutoff_threshold, const int numVars) const
{
	root->evaluate_no_remainder(result, tms_of_vars, order, cutoff_threshold, numVars);
}

void Expression_AST::evaluate(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold, const int numVars, std::list<Interval> & intermediate_ranges) const
{
	root->evaluate(result, tms_of_vars, order, step_exp_table, cutoff_threshold, numVars, intermediate_ranges);
}

void Expression_AST::evaluate_remainder(Interval & result, const std::vector<TaylorModel> & tms_of_vars, const int order, std::list<Interval>::iterator & iter) const
{
	root->evaluate_remainder(result, tms_of_vars, order, iter);
}

void Expression_AST::evaluate(Real & result, const std::vector<Real> & values_of_vars) const
{
	root->evaluate(result, values_of_vars);
}

void Expression_AST::center()
{
	root->center();
}

bool Expression_AST::isConstant(Interval & I) const
{
	if(root->node_type == NODE_CONST)
	{
		I = root->node_value.constant;
		return true;
	}
	else
	{
		return false;
	}
}

Expression_AST & Expression_AST::operator = (const Expression_AST & expression)
{
	if(this == &expression)
		return *this;

	root = expression.root;

	return *this;
}

Expression_AST & Expression_AST::operator += (const Expression_AST & expression)
{
	std::shared_ptr<AST_Node> tmp(new AST_Node(OPT_PLUS, root, expression.root));
	root = tmp;

	return *this;
}

Expression_AST & Expression_AST::operator -= (const Expression_AST & expression)
{
	std::shared_ptr<AST_Node> tmp(new AST_Node(OPT_MINU, root, expression.root));
	root = tmp;

	return *this;
}

Expression_AST & Expression_AST::operator *= (const Expression_AST & expression)
{
	std::shared_ptr<AST_Node> tmp(new AST_Node(OPT_MULT, root, expression.root));
	root = tmp;

	return *this;
}

Expression_AST & Expression_AST::operator /= (const Expression_AST & expression)
{
	std::shared_ptr<AST_Node> tmp(new AST_Node(OPT_DIV, root, expression.root));
	root = tmp;

	return *this;
}

void Expression_AST::pow_assign(const int n)
{
	std::shared_ptr<AST_Node> exponent(new AST_Node(n));

	std::shared_ptr<AST_Node> tmp(new AST_Node(OPT_POW, root, exponent));
	root = tmp;
}

void Expression_AST::inv_assign()
{
	std::shared_ptr<AST_Node> tmp(new AST_Node(OPT_NEG, root));
	root = tmp;
}

void Expression_AST::sin_assign()
{
	std::shared_ptr<AST_Node> tmp(new AST_Node(OPT_SIN, root));
	root = tmp;
}

void Expression_AST::cos_assign()
{
	std::shared_ptr<AST_Node> tmp(new AST_Node(OPT_COS, root));
	root = tmp;
}

void Expression_AST::exp_assign()
{
	std::shared_ptr<AST_Node> tmp(new AST_Node(OPT_EXP, root));
	root = tmp;
}

void Expression_AST::log_assign()
{
	std::shared_ptr<AST_Node> tmp(new AST_Node(OPT_LOG, root));
	root = tmp;
}

void Expression_AST::sqrt_assign()
{
	std::shared_ptr<AST_Node> tmp(new AST_Node(OPT_SQRT, root));
	root = tmp;
}




// using lex
ParseExpression::ParseExpression()
{
}

ParseExpression::ParseExpression(const ParseExpression & setting)
{
	strExpression = setting.strExpression;
	variables = setting.variables;
	expression = setting.expression;
}

ParseExpression::~ParseExpression()
{
	variables.clear();
}

ParseExpression & ParseExpression::operator = (const ParseExpression & setting)
{
	if(this == &setting)
		return *this;

	strExpression = setting.strExpression;
	variables = setting.variables;
	expression = setting.expression;

	return *this;
}

void ParseExpression::clear()
{
	variables.clear();
}

