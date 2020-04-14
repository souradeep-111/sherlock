/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.
  
  The code is released as is under the GNU General Public License (GPL).
---*/

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "include.h"
#include "Constraints.h"

namespace flowstar
{

class Polyhedron
{
public:
	std::vector<LinearConstraint> constraints;
public:
	Polyhedron();
	Polyhedron(const std::vector<LinearConstraint> & cs);
	Polyhedron(const Polyhedron & P);
	Polyhedron(const Matrix & A, const ColVector & b);
	Polyhedron(const std::vector<std::vector<Interval> > & A, const std::vector<Interval> & B);
	~Polyhedron();

	Interval rho(const std::vector<Interval> & l) const;
	Interval rho(iMatrix & l) const;
	void tightenConstraints();
	bool empty() const;
	void get(std::vector<std::vector<Interval> > & A, std::vector<Interval> & B) const;

	void dump(FILE *fp, std::vector<std::string> const & varNames) const;

	Polyhedron & operator = (const Polyhedron & P);
};

class Parallelotope						// in their constraint-based representations.
{
public:
	Matrix paraTemplate;					// only half of the facet normals are kept
	ColVector b;

	Parallelotope(const Matrix & template_input, const ColVector & b_input);
	Parallelotope(const Parallelotope & P);
	~Parallelotope();

	void center(ColVector & c) const;		// Compute the center point of the parallelotope.
	void dump(FILE *fp) const;

	void toTaylorModel(TaylorModelVec & result) const;		// converse a parallelotope to a Taylor model, the domain is normalized.

	Parallelotope & operator = (const Parallelotope & P);
};

class Zonotope
{
public:
	iMatrix center;
	std::list<iMatrix> generators;

public:
	Zonotope();
	Zonotope(const iMatrix & c, const std::list<iMatrix> & gen);
	Zonotope(const Zonotope & zonotope);
	Zonotope(iMatrix & box);
//	Zonotope(iMatrix2 & box);
	Zonotope(const int d);
	~Zonotope();

	bool isEmpty() const;
	bool isIntersected(iMatrix & box) const;

	int numOfGen() const;

	void linearTrans(Zonotope & result, const iMatrix & map) const;
	void linearTrans_assign(const iMatrix & map);

	void MinSum(Zonotope & result, const Zonotope & zonotope) const;
	void MinSum_assign(const Zonotope & zonotope);

	void simplify();

	void intEval(iMatrix & range);

	void output(FILE *fp) const;

	bool belongsto(const std::vector<double> & x);
	int contract(const LinearConstraint & constraint);

	// translate the zonotope to a Taylor model with a zero remainder
	// the first variable is NOT reserved by t
	void toPolynomial(std::vector<Polynomial> & result);

	Zonotope & operator = (const Zonotope & zonotope);
	Zonotope & operator = (iMatrix & box);
//	Zonotope & operator = (iMatrix2 & box);
};


}

#endif /* GEOMETRY_H_ */
