/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.
  
  The code is released as is under the GNU General Public License (GPL).
---*/

#include "Geometry.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>

using namespace flowstar;

// class Polyhedron

Polyhedron::Polyhedron()
{
}

Polyhedron::Polyhedron(const std::vector<LinearConstraint> & cs):constraints(cs)
{
}

Polyhedron::Polyhedron(const Polyhedron & P):constraints(P.constraints)
{
}

Polyhedron::Polyhedron(const Matrix & A, const ColVector & b)
{
	int rows = A.rows();
	int cols = A.cols();

	for(int i=0; i<rows; ++i)
	{
		std::vector<Interval> row;

		for(int j=0; j<cols; ++j)
		{
			Interval intTemp(A.get(i,j));
			row.push_back(intTemp);
		}

		Interval B(b.get(i));
		LinearConstraint lc(row, B);
		constraints.push_back(lc);
	}
}

Polyhedron::Polyhedron(const std::vector<std::vector<Interval> > & A, const std::vector<Interval> & B)
{
	for(int i=0; i<A.size(); ++i)
	{
		LinearConstraint lc(A[i], B[i]);
		constraints.push_back(lc);
	}
}

Polyhedron::~Polyhedron()
{
	constraints.clear();
}

Interval Polyhedron::rho(const std::vector<Interval> & l) const
{
	int d = l.size();
	int n = constraints.size();
	int size = n*d;

	int *rowInd = new int[ 1 + size ];
	int *colInd = new int[ 1 + size ];
	double *coes = new double [ 1 + size ];

	glp_term_out(GLP_OFF);

	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_obj_dir(lp, GLP_MAX);

	glp_add_rows(lp, n);

	for(int i=1; i<=n; ++i)
	{
		glp_set_row_bnds(lp, i, GLP_UP, 0.0, constraints[i-1].B.midpoint());
	}

	glp_add_cols(lp, d);
	for(int i=1; i<=d; ++i)
	{
		glp_set_col_bnds(lp, i, GLP_FR, 0.0, 0.0);
		glp_set_obj_coef(lp, i, l[i-1].midpoint());
	}

	for(int i=1; i<=n; ++i)
	{
		for(int j=1; j<=d; ++j)
		{
			int pos = j + (i-1)*d;
			rowInd[pos] = i;
			colInd[pos] = j;
			coes[pos] = constraints[i-1].A[j-1].midpoint();
		}
	}

	glp_load_matrix(lp, size, rowInd, colInd, coes);
	glp_simplex(lp, NULL);
	double result = glp_get_obj_val(lp);
	int status = glp_get_status(lp);

	if(status == GLP_INFEAS || status == GLP_NOFEAS)
	{
		result = INVALID;
	}
	else if(status == GLP_UNBND)
	{
		result = UNBOUNDED;
	}

	glp_delete_prob(lp);
	delete[] rowInd;
	delete[] colInd;
	delete[] coes;

	Interval intTemp(result);
	return intTemp;
}

Interval Polyhedron::rho(iMatrix & l) const
{
	int d = l.cols();
	int n = constraints.size();
	int size = n*d;

	int *rowInd = new int[ 1 + size ];
	int *colInd = new int[ 1 + size ];
	double *coes = new double [ 1 + size ];

	glp_term_out(GLP_OFF);

	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_obj_dir(lp, GLP_MAX);

	glp_add_rows(lp, n);

	for(int i=1; i<=n; ++i)
	{
		glp_set_row_bnds(lp, i, GLP_UP, 0.0, constraints[i-1].B.midpoint());
	}

	glp_add_cols(lp, d);
	for(int i=1; i<=d; ++i)
	{
		glp_set_col_bnds(lp, i, GLP_FR, 0.0, 0.0);

		Interval intTemp = l[0][i-1];
		glp_set_obj_coef(lp, i, intTemp.midpoint());
	}

	for(int i=1; i<=n; ++i)
	{
		for(int j=1; j<=d; ++j)
		{
			int pos = j + (i-1)*d;
			rowInd[pos] = i;
			colInd[pos] = j;
			coes[pos] = constraints[i-1].A[j-1].midpoint();
		}
	}

	glp_load_matrix(lp, size, rowInd, colInd, coes);
	glp_simplex(lp, NULL);
	double result = glp_get_obj_val(lp);
	int status = glp_get_status(lp);

	if(status == GLP_INFEAS || status == GLP_NOFEAS)
	{
		result = INVALID;
	}
	else if(status == GLP_UNBND)
	{
		result = UNBOUNDED;
	}

	glp_delete_prob(lp);
	delete[] rowInd;
	delete[] colInd;
	delete[] coes;

	Interval intTemp(result);
	return intTemp;
}

void Polyhedron::tightenConstraints()
{
	for(int i=0; i<constraints.size(); ++i)
	{
		Interval I = rho(constraints[i].A);

		if(I < constraints[i].B)
		{
			constraints[i].B = I;
		}
	}
}

bool Polyhedron::empty() const
{
	if(constraints.size() == 0)
	{
		return false;
	}
	else
	{
		int d = constraints.begin()->A.size();

		std::vector<Interval> l;
		Interval intZero, intOne(1);
		for(int i=0; i<d; ++i)
		{
			l.push_back(intZero);
		}

		l[0] = intOne;

		Interval result = this->rho(l);

		if(result.inf() <= INVALID + THRESHOLD_HIGH)
			return true;
		else
			return false;
	}
}

void Polyhedron::get(std::vector<std::vector<Interval> > & A, std::vector<Interval> & B) const
{
	A.clear();
	B.clear();

//	list<LinearConstraint>::const_iterator iter = constraints.begin();

	for(int i=0; i<constraints.size(); ++i)
	{
		A.push_back(constraints[i].A);
		B.push_back(constraints[i].B);
	}
}

void Polyhedron::dump(FILE *fp, std::vector<std::string> const & varNames) const
{
//	list<LinearConstraint>::const_iterator iter = constraints.begin();

	for(int i=0; i<constraints.size(); ++i)
	{
		constraints[i].dump(fp, varNames);
	}

	fprintf(fp, "\n");
}

Polyhedron & Polyhedron::operator = (const Polyhedron & P)
{
	if(this == &P)
		return *this;

	constraints = P.constraints;
	return *this;
}































// class Parallelotope

Parallelotope::Parallelotope(const Matrix & template_input, const ColVector & b_input):paraTemplate(template_input), b(b_input)
{
}

Parallelotope::Parallelotope(const Parallelotope & P): paraTemplate(P.paraTemplate), b(P.b)
{
}

Parallelotope::~Parallelotope()
{
}

void Parallelotope::center(ColVector & c) const
{
	int d = paraTemplate.cols();

	gsl_vector *r = gsl_vector_alloc(d);
	for(int i=0; i<d; ++i)
		gsl_vector_set( r, i, (b.get(i) - b.get(i+d))/2);

	// We use GSL to solve the linear equations B x = r.

	gsl_matrix *B = gsl_matrix_alloc(d, d);

	for(int i=0; i<d; ++i)
	{
		for(int j=0; j<d; ++j)
		{
			gsl_matrix_set(B, i, j, paraTemplate.get(i,j));
		}
	}

	gsl_vector *x = gsl_vector_alloc(d);

	gsl_linalg_HH_solve(B, r, x);

	for(int i=0; i<d; ++i)
	{
		c.set( gsl_vector_get(x,i), i);
	}

	gsl_vector_free(r);
	gsl_matrix_free(B);
	gsl_vector_free(x);
}

void Parallelotope::dump(FILE *fp) const
{
	int rows = paraTemplate.rows();
	int cols = rows;
	int rangeDim = rows;

	for(int i=0; i<rows; ++i)
	{
		fprintf(fp, "[ ");
		for(int j=0; j<cols-1; ++j)
		{
			fprintf(fp, "%lf,\t", paraTemplate.get(i,j));
		}
		fprintf(fp, "%lf ]\t<=\t%lf\n", paraTemplate.get(i,cols-1), b.get(i));
	}

	for(int i=0; i<rows; ++i)
	{
		fprintf(fp, "[ ");
		for(int j=0; j<cols-1; ++j)
		{
			fprintf(fp, "%lf,\t", -paraTemplate.get(i,j));
		}
		fprintf(fp, "%lf ]\t<=\t%lf\n", -paraTemplate.get(i,cols-1), b.get(i+rangeDim));
	}
}

void Parallelotope::toTaylorModel(TaylorModelVec & result) const
{
	int rangeDim = paraTemplate.rows();
	int domainDim = rangeDim + 1;

	// 1: we converse the center point to a Taylor model

	ColVector colVecCenter(rangeDim);
	center(colVecCenter);

	std::vector<Interval> coefficients;
	for(int i=0; i<rangeDim; ++i)
	{
		Interval I(colVecCenter.get(i));
		coefficients.push_back(I);
	}

	TaylorModelVec tmvCenter(coefficients, domainDim);

	// 2: we center the parallelotope at 0
	ColVector colVecDiff(rangeDim);
	colVecCenter.mul(colVecDiff, paraTemplate);

	// since a parallelotope is symmetric, we only need to consider half of the intercepts
	ColVector new_b(rangeDim);
	for(int i=0; i<rangeDim; ++i)
	{
		new_b.set( b.get(i) - colVecDiff.get(i), i);
	}

	// 3: compute the generators.
	Matrix generators(rangeDim, rangeDim);
	std::vector<int> zeroRows;	// the row indices for zero intercepts

	for(int i=0; i<rangeDim; ++i)
	{
		if(new_b.get(i) <= THRESHOLD_LOW && new_b.get(i) >= -THRESHOLD_LOW)	// zero
		{
			zeroRows.push_back(i);

			for(int j=0; j<rangeDim; ++j)
			{
				generators.set( paraTemplate.get(i,j), i, j);
			}
		}
		else
		{
			for(int j=0; j<rangeDim; ++j)
			{
				generators.set( paraTemplate.get(i,j) / new_b.get(i), i, j);
			}
		}
	}

	generators.inverse_assign();

	Matrix tmv_coefficients(rangeDim, domainDim);

	for(int j=0, k=0; j<rangeDim; ++j)
	{
		if(k < zeroRows.size() && j == zeroRows[k])	// neglect the zero length generators
		{
			++k;
		}
		else
		{
			for(int i=0; i<rangeDim; ++i)
			{
				tmv_coefficients.set( generators.get(i,j), i, j+1);
			}
		}
	}

	TaylorModelVec tmvParallelotope(tmv_coefficients);
	tmvParallelotope.add(result, tmvCenter);
}

Parallelotope & Parallelotope::operator = (const Parallelotope & P)
{
	if(this == &P)
		return *this;

	paraTemplate = P.paraTemplate;
	b = P.b;
	return *this;
}






// class zonotope

Zonotope::Zonotope()
{
}

Zonotope::Zonotope(const iMatrix & c, const std::list<iMatrix> & gen)
{
	center = c;
	generators = gen;
}

Zonotope::Zonotope(const Zonotope & zonotope)
{
	center = zonotope.center;
	generators = zonotope.generators;
}

Zonotope::Zonotope(iMatrix & box)
{
	int d = box.rows();
	iMatrix im_center(d,1);
	center = im_center;

	for(int i=0; i<d; ++i)
	{
		Real c, r;
		box[i][0].toCenterForm(c, r);
		center[i][0] = c;

		iMatrix im_generator(d,1);
		im_generator[i][0] = r;

		generators.push_back(im_generator);
	}
}
/*
Zonotope::Zonotope(iMatrix2 & box)
{
	int d = box.rows();
	iMatrix2 im2_center(d,1);
	center = im2_center;

	for(int i=0; i<d; ++i)
	{
		center.center[i][0] = box.center[i][0];

		iMatrix2 im2_generator(d,1);
		im2_generator.center[i][0] = box.radius[i][0];

		generators.push_back(im2_generator);
	}
}
*/
Zonotope::Zonotope(const int d)
{
	iMatrix im_zero(d, 1);
	center = im_zero;
}

Zonotope::~Zonotope()
{
	generators.clear();
}

bool Zonotope::isEmpty() const
{
	if(center.rows() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Zonotope::isIntersected(iMatrix & box) const
{
	// call GLPK to solve the LP
	return false;
}

int Zonotope::numOfGen() const
{
	return (int)generators.size();
}

void Zonotope::linearTrans(Zonotope & result, const iMatrix & map) const
{
	result.center = map * center;

	result.generators.clear();

	std::list<iMatrix>::const_iterator iter = generators.begin();
	for(; iter != generators.end(); ++iter)
	{
		iMatrix im_tmp = map * (*iter);
		result.generators.push_back(im_tmp);
	}
}

void Zonotope::linearTrans_assign(const iMatrix & map)
{
	center = map * center;

	std::list<iMatrix>::iterator iter = generators.begin();
	for(; iter != generators.end(); ++iter)
	{
		iMatrix im_tmp = map * (*iter);
		(*iter) = im_tmp;
	}
}

void Zonotope::MinSum(Zonotope & result, const Zonotope & zonotope) const
{
	result.center = center + zonotope.center;
	result.generators = generators;

	std::list<iMatrix>::const_iterator iter = zonotope.generators.begin();

	for(; iter != zonotope.generators.end(); ++iter)
	{
		result.generators.push_back(*iter);
	}
}

void Zonotope::MinSum_assign(const Zonotope & zonotope)
{
	center += zonotope.center;

	std::list<iMatrix>::const_iterator iter = zonotope.generators.begin();

	for(; iter != zonotope.generators.end(); ++iter)
	{
		generators.push_back(*iter);
	}
}

void Zonotope::simplify()
{
	std::list<iMatrix> result;

	int d = center.rows();
	iMatrix vecZero(d, 1);

	for(int i=0; i<d; ++i)
	{
		result.push_back(vecZero);
	}

	std::list<iMatrix>::iterator iter = generators.begin();
	for(; iter != generators.end(); ++iter)
	{
		std::list<iMatrix>::iterator iter2 = result.begin();
		for(int j=0; iter2 != result.end(); ++iter2, ++j)
		{
			Interval I;
			(*iter)[j][0].mag(I);
			(*iter2)[j][0] += I;
		}
	}

	generators = result;
}

void Zonotope::intEval(iMatrix & range)
{
	int d = center.rows();
	range = center;

	std::list<iMatrix>::iterator iter = generators.begin();
	for(; iter != generators.end(); ++iter)
	{
		for(int j=0; j<d; ++j)
		{
			double r = (*iter)[j][0].mag();
			range[j][0].bloat(r);
		}
	}
}

void Zonotope::output(FILE *fp) const
{
	fprintf(fp, "Center:\n");

	center.output(fp);
	fprintf(fp, "\nGenerators:\n");

	std::list<iMatrix>::const_iterator iter = generators.begin();
	for(; iter != generators.end(); ++iter)
	{
		(*iter).output(fp);
		fprintf(fp,"\n");
	}
}

Zonotope & Zonotope::operator = (const Zonotope & zonotope)
{
	if(this == &zonotope)
		return *this;

	center = zonotope.center;
	generators = zonotope.generators;

	return *this;
}

Zonotope & Zonotope::operator = (iMatrix & box)
{
	int d = box.rows();
	generators.clear();

	iMatrix im_center(d, 1);

	for(int i=0; i<d; ++i)
	{
		Interval M;
		box[i][0].midpoint(M);
		im_center[i][0] = M;

		Interval R;
		box[i][0].width(R);
		R /= 2.0;

		iMatrix generator(d,1);
		generator[i][0] = R;
		generators.push_back(generator);
	}

	center = im_center;

	return *this;
}

/*
Zonotope & Zonotope::operator = (iMatrix2 & box)
{
	int d = box.rows();

	iMatrix2 im2_center(d,1);

	for(int i=0; i<d; ++i)
	{
		im2_center.center[i][0] = box.center[i][0];

		iMatrix2 im2_generator(d,1);
		im2_generator.center[i][0] = box.radius[i][0];
		generators.push_back(im2_generator);
	}

	center = im2_center;

	return *this;
}
*/



bool Zonotope::belongsto(const std::vector<double> & x)
{
	int d = generators.size();
	int n = x.size();
	int size = n*d;

	int *rowInd = new int[ 1 + size ];
	int *colInd = new int[ 1 + size ];
	double *coes = new double [ 1 + size ];

	glp_term_out(GLP_OFF);

	glp_prob *lp;
	lp = glp_create_prob();
	glp_set_obj_dir(lp, GLP_MAX);

	glp_add_rows(lp, n);

	for(int i=1; i<=n; ++i)
	{
		double tmp = x[i-1] - center[i-1][0].midpoint();
		glp_set_row_bnds(lp, i, GLP_FX, tmp, 0.0);
	}

	glp_add_cols(lp, d);
	for(int i=1; i<=d; ++i)
	{
		glp_set_col_bnds(lp, i, GLP_DB, -1.0, 1.0);
		glp_set_obj_coef(lp, i, 1.0);
	}

	for(int i=1; i<=n; ++i)
	{
		std::list<iMatrix>::iterator iter = generators.begin();
		for(int j=1; j<=d; ++j)
		{
			int pos = j + (i-1)*d;
			rowInd[pos] = i;
			colInd[pos] = j;
			coes[pos] = (*iter)[i-1][0].midpoint();
			++iter;
		}
	}

	glp_load_matrix(lp, size, rowInd, colInd, coes);
	glp_simplex(lp, NULL);

	int status = glp_get_status(lp);

	glp_delete_prob(lp);
	delete[] rowInd;
	delete[] colInd;
	delete[] coes;

	if(status == GLP_OPT || status == GLP_FEAS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int Zonotope::contract(const LinearConstraint & constraint)
{
	int rangeDim = center.rows();
	int domainDim = generators.size();

	std::vector<Polynomial> polynomials;
	toPolynomial(polynomials);

	std::vector<Interval> domain;
	Interval intUnit(-1,1);
	for(int i=0; i<domainDim; ++i)
	{
		domain.push_back(intUnit);
	}

	Polynomial obj;
	for(int i=0; i<constraint.A.size(); ++i)
	{
		Polynomial polyTemp = polynomials[i];
		polyTemp.mul_assign(constraint.A[i]);
		obj += polyTemp;
	}

	HornerForm obj_hf;
	obj.toHornerForm(obj_hf);


	bool bvalid = true;
	bool bcontinue = true;
	Interval W;
	Interval intZero;

	for(; bcontinue; )
	{
		std::vector<Interval> oldDomain = domain;

		// contract the domain
		for(int i=0; i<domainDim; ++i)
		{
			Interval newInt = domain[i];

			newInt.width(W);

			// search an approximation for the lower bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				std::vector<Interval> newDomain = domain;
				newDomain[i] = intLeft;

				Interval intTemp;
				obj_hf.intEval(intTemp, newDomain);

				if(intTemp > constraint.B)
				{
					// no intersection on the left half
					newInt = intRight;
					newInt.width(W);
					break;
				}
				else if(intTemp.smallereq(constraint.B))
				{
					// do not need to apply domain contraction w.r.t. the current constraint
					newInt = intLeft;
					newInt.width(W);
				}
				else
				{
					// refine the interval
					newInt = intLeft;
					newInt.width(W);
				}
			}


			// set the lower bound
			Interval Inf;
			newInt.inf(Inf);
			domain[i].setInf(Inf);

			newInt = domain[i];

			newInt.width(W);

			// search an approximation for the upper bound
			for(; W >= DC_THRESHOLD_SEARCH;)
			{
				Interval intLeft;
				Interval intRight;
				newInt.split(intLeft, intRight);

				std::vector<Interval> newDomain = domain;
				newDomain[i] = intRight;

				Interval intTemp;
				obj_hf.intEval(intTemp, newDomain);

				if(intTemp > constraint.B)
				{
					// no intersection on the right half
					newInt = intLeft;
					newInt.width(W);
					break;
				}
				else if(intTemp.smallereq(constraint.B))
				{
					// do not need to apply domain contraction w.r.t. the current constraint
					newInt = intRight;
					newInt.width(W);
				}
				else
				{
					// refine the interval
					newInt = intRight;
					newInt.width(W);
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
		return -1;
	}
	else
	{
		// compute the contracted zonotope

		for(int i=0; i<rangeDim; ++i)
		{
			std::list<iMatrix>::iterator iter = generators.begin();
			for(int j=0; j<domain.size(); ++j)
			{
				Interval M;
				domain[j].remove_midpoint(M);

				center[i][0] += (*iter)[i][0] * M;

				(*iter)[i][0] *= domain[j].sup();

				++iter;
			}
		}

		return 1;
	}
}

void Zonotope::toPolynomial(std::vector<Polynomial> & result)
{
	int rangeDim = center.rows();
	int domainDim = generators.size();

	Interval intZero;

	result.clear();

	for(int i=0; i<rangeDim; ++i)
	{
		std::vector<Interval> coefficients;

		std::list<iMatrix>::iterator iter = generators.begin();

		for(; iter != generators.end(); ++iter)
		{
			coefficients.push_back((*iter)[i][0]);
		}

		Polynomial polyTemp(coefficients);
		Polynomial constant(center[i][0], domainDim);

		result.push_back(polyTemp + constant);
	}
}
