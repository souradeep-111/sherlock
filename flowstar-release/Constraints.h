/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.
  
  The code is released as is under the GNU General Public License (GPL).
---*/

#ifndef CONSTRAINTS_H_
#define CONSTRAINTS_H_

#include "Matrix.h"
#include "TaylorModel.h"

namespace flowstar
{

// Define the classes of linear and non-linear constraints

class LinearConstraint		// A x <= B
{
public:
	std::vector<Interval> A;
	Interval B;
public:
	LinearConstraint();
	LinearConstraint(const std::vector<Interval> & A_input, const Interval & B_input);
	LinearConstraint(iMatrix & A_input, const Interval & B_input);
	LinearConstraint(const LinearConstraint & lc);
	~LinearConstraint();

	void dump(FILE *fp, const std::vector<std::string> & stateVarNames) const;

	LinearConstraint & operator = (const LinearConstraint & lc);
};

class PolynomialConstraint	// p(x) <= b
{
public:
	Polynomial p;
	HornerForm hf;		// a HornerForm of p
	Interval B;
public:
	PolynomialConstraint();
	PolynomialConstraint(const Polynomial & p_input, const Interval & B_input);
	PolynomialConstraint(const PolynomialConstraint & pc);
	~PolynomialConstraint();

	void dump(FILE *fp, const std::vector<std::string> & stateVarNames) const;

	PolynomialConstraint & operator = (const PolynomialConstraint & pc);
};

}

#endif /* CONSTRAINTS_H_ */
