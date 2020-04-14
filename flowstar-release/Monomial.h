/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.

  The code is released as is under the GNU General Public License (GPL).
---*/

#ifndef MONOMIAL_H_
#define MONOMIAL_H_

#include "Interval.h"

namespace flowstar
{

class Monomial
{
protected:
	Interval coefficient;			// the coefficient of the monomial
	std::vector<int> degrees;		// the degrees of the variables, e.g., [2,0,4] is the notation for x1^2 x3^4
	int d;			        		// the degree of the monomial, it is the sum of the values in degrees.

public:
	Monomial();													// empty monomial.
	Monomial(const Interval & I, const std::vector<int> & degs);
	Monomial(const Monomial & monomial);
	Monomial(const Interval & I, const int numVars);			// a constant
	~Monomial();

	int degree() const;											// degree of the monomial
	int dimension() const;										// dimension of the monomial

	void intEval(Interval & result, const std::vector<Interval> & domain) const;	// interval evaluation of the monomial

	// interval evaluation of the monomial, we assume that the domain is normalized to [0,s] x [-1,1]^(d-1)
	void intEvalNormal(Interval & result, const std::vector<Interval> & step_exp_table) const;
	void inv(Monomial & result) const;							// additive inverse

	Monomial & operator = (const Monomial & monomial);
	Monomial & operator += (const Monomial & monomial);			// we assume the two monomials can be added up
	Monomial & operator *= (const Monomial & monomial);
	const Monomial operator + (const Monomial & monomial) const;
	const Monomial operator * (const Monomial & monomial) const;

	bool isLinear(int & index) const;							// Check if the degree of the monomial is 1. If so then return the index of the variable of degree 1.

	void dump_interval(FILE *fp, const std::vector<std::string> & varNames) const;	// coefficients are dumped as intervals
	void dump_constant(FILE *fp, const std::vector<std::string> & varNames) const;	// coefficients are dumped as constants

	void toString(std::string & result, const std::vector<std::string> & varNames) const;

	bool classInvariantOK() const;

	bool operator < (const Monomial & b) const;					// Define a partial order over the monomials
	bool operator == (const Monomial & b) const;

	bool center();

	int cutoff(Monomial & monoRem, const Interval & cutoff_threshold);
	int cutoff(const Interval & cutoff_threshold);

	void substitute(const int varID, const Interval & intVal);									// substitute a variable by an Interval
	void substitute(const std::vector<int> & varIDs, const std::vector<Interval> & intVals);	// substitute a set of variables by intervals

	bool substitute_with_precond(const std::vector<bool> & substitution);

	void substitute(Monomial & result, const int varID, const Interval & intVal) const;
	void substitute(Monomial & result, const std::vector<int> & varIDs, const std::vector<Interval> & intVals) const;

	void extend(const int num);

	friend class Polynomial;
	friend class TaylorModel;
	friend class TaylorModelVec;
	friend class UnivariatePolynomial;
	friend class iMatrix;
	friend class mpMatrix;
	friend class Flowpipe;
	friend class upMatrix;
};

}

#endif
