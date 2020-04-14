#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include "TaylorModel.h"
#include <memory>

#define OPT_PLUS		0
#define OPT_MINU		1
#define OPT_MULT		2
#define OPT_DIV			3
#define OPT_POW			4
#define OPT_NEG			5

#define OPT_SIN			6
#define OPT_COS			7
#define OPT_EXP			8
#define OPT_LOG			9
#define OPT_SQRT		10

#define NODE_BIN_OPT	0
#define NODE_UNA_OPT	1
#define NODE_VAR		2
#define NODE_CONST		3

#define VAR_ID			0
#define PAR_ID			1


namespace flowstar
{

class AST_Node;

class Node_Operator
{
public:
	int type;
	std::shared_ptr<AST_Node> left_operand;		// only the left operand is used if the operator is unary
	std::shared_ptr<AST_Node> right_operand;

public:
	Node_Operator()
	{
		type = -1;
		left_operand = nullptr;
		right_operand = nullptr;
	}

	Node_Operator(const int opt_type, const std::shared_ptr<AST_Node> & left)
	{
		type = opt_type;
		left_operand = left;
		right_operand = nullptr;
	}

	Node_Operator(const int opt_type, const std::shared_ptr<AST_Node> & left,const std::shared_ptr<AST_Node> & right)
	{
		type = opt_type;
		left_operand = left;
		right_operand = right;
	}

	~Node_Operator()
	{
		left_operand.reset();
		right_operand.reset();
	}
};

class Node_Variable
{
public:
	int type;		// can only be a state variable in the current version
	int id;

public:
	Node_Variable()
	{
		type = -1;
		id = -1;
	}

	Node_Variable(const int var_type, const int var_id)
	{
		type = var_type;
		id = var_id;
	}

	~Node_Variable()
	{
	}
};

// node of the abstract syntax tree
class AST_Node
{
protected:
	int node_type;

	struct Node_Value
	{
		Node_Operator opt;
		Node_Variable var;
		Interval constant;

		Node_Value()
		{
		}

		~Node_Value()
		{
		}
	} node_value;

public:
	AST_Node();
	AST_Node(const int opt_type, const std::shared_ptr<AST_Node> & left);
	AST_Node(const int opt_type, const std::shared_ptr<AST_Node> & left,const std::shared_ptr<AST_Node> & right);
	AST_Node(const int var_type, const int var_id);
	AST_Node(const Interval & I);
	~AST_Node();

	void evaluate(Interval & result, const Taylor_Model_Computation_Setting & setting) const;
	void evaluate(Real & result, const std::vector<Real> & values_of_vars) const;

	void evaluate(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const Taylor_Model_Computation_Setting & setting) const;
	void evaluate(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold, const int numVars) const;

	// for internal use
	void evaluate_no_remainder(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const Interval & cutoff_threshold, const int numVars) const;
	void evaluate(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold, const int numVars, std::list<Interval> & intermediate_ranges) const;
	void evaluate_remainder(Interval & result, const std::vector<TaylorModel> & tms_of_vars, const int order, std::list<Interval>::iterator & iter) const;

	void center();

	void output(std::string & expression, const Taylor_Model_Computation_Setting & setting) const;
	void output(std::string & expression, const Variables & variables) const;

	friend class Expression_AST;
};

// abstract syntax tree
class Expression_AST
{
protected:
	std::shared_ptr<AST_Node> root;

public:
	Expression_AST();
	Expression_AST(const std::string & varName, const Taylor_Model_Computation_Setting & setting);
	Expression_AST(const std::string & varName, const Variables & variables, const int dummy);
	Expression_AST(const double constant);
	Expression_AST(const Interval & constant);
	~Expression_AST();

	// using lex
	Expression_AST(const std::string & strExpression, const Variables & vars);

	void output(FILE *fp, const Taylor_Model_Computation_Setting & setting) const;
	void output(FILE *fp, const Variables & variables) const;

	void evaluate_no_remainder(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const Interval & cutoff_threshold, const int numVars) const;
	void evaluate(TaylorModel & result, const std::vector<TaylorModel> & tms_of_vars, const int order, const std::vector<Interval> & step_exp_table, const Interval & cutoff_threshold, const int numVars, std::list<Interval> & intermediate_ranges) const;
	void evaluate_remainder(Interval & result, const std::vector<TaylorModel> & tms_of_vars, const int order, std::list<Interval>::iterator & iter) const;

	void evaluate(Real & result, const std::vector<Real> & values_of_vars) const;

	void center();
	bool isConstant(Interval & I) const;

	Expression_AST & operator = (const Expression_AST & expression);
	Expression_AST & operator += (const Expression_AST & expression);
	Expression_AST & operator -= (const Expression_AST & expression);
	Expression_AST & operator *= (const Expression_AST & expression);
	Expression_AST & operator /= (const Expression_AST & expression);

	void pow_assign(const int n);
	void inv_assign();
	void sin_assign();
	void cos_assign();
	void exp_assign();
	void log_assign();
	void sqrt_assign();
};

class ParseExpression
{
public:
	std::string strExpression;
	Variables variables;
	Expression_AST expression;

public:
	ParseExpression();
	ParseExpression(const ParseExpression & setting);
	~ParseExpression();

	ParseExpression & operator = (const ParseExpression & setting);

	void clear();
};

extern ParseExpression parseExpression;

}

void parse_ODE_Expression();

#endif /* EXPRESSION_H_ */
