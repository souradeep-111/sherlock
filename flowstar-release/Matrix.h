/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.
  
  The code is released as is under the GNU General Public License (GPL).
---*/

#ifndef MATRIX_H_
#define MATRIX_H_

#include "include.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>

namespace flowstar
{

class Real;
class Interval;
class UnivariatePolynomial;
class Polynomial;
class RowVector;
class ColVector;
class rMatrix;
class iMatrix;
class upMatrix;
class mpMatrix;
class iMatrix2;
class HybridSystem;
class TaylorModelVec;


// The matrix class is implemented based on the data structure of gsl matrix.
class Matrix
{
private:
	gsl_matrix *data;
public:
	Matrix();
	Matrix(const int m, const int n);	// Create an m x n matrix, all of the entries are 0.
	Matrix(const int n);				// Create an n x n matrix, all of the entries are 0.
	Matrix(const Matrix & A);
	~Matrix();

	double get(const int i, const int j) const;				// Get the entry at position [i,j].
	void set(const double v, const int i, const int j);		// Set A[i,j] = v.

	int rows() const;
	int cols() const;

	void row(RowVector & result, const int i) const;			// Return the (i+1)-st row.

	void sortColumns();		// Sort the columns by size in descending order.
	int rank() const;

	void neg(Matrix & result) const;
	void neg_assign();

	void inverse(Matrix & result) const;
	void inverse_assign();

	void transpose(Matrix & result) const;
	void svd(Matrix & U) const;

	void QR(Matrix & D);
	void QRfactor(Matrix & Q);

	void output(FILE *fp) const;

	Matrix & operator += (const Matrix & A);
	Matrix & operator -= (const Matrix & A);
	Matrix & operator *= (const Matrix & A);

	Matrix operator + (const Matrix & A) const;
	Matrix operator - (const Matrix & A) const;
	Matrix operator * (const Matrix & A) const;

	Matrix & operator = (const Matrix & A);
};

class RowVector
{
private:
	Matrix vec;
public:
	RowVector();
	RowVector(const int n);
	RowVector(const RowVector & v);
	~RowVector();

	void set(const double v, const int pos);
	double get(const int pos) const;
	int size() const;

	void transpose(ColVector & result) const;

	void neg(RowVector & result) const;
	void neg_assign();

	void dump(FILE *fp) const;

	double innerProd(const RowVector & v) const;
	double EuclideanNorm() const;
	void normalize();

	bool operator == (const RowVector & v) const;

	RowVector & operator += (const RowVector & v);
	RowVector & operator -= (const RowVector & v);
	RowVector operator + (const RowVector & v) const;
	RowVector operator - (const RowVector & v) const;

	RowVector & operator = (const RowVector & v);

	friend class ColVector;
};

class ColVector
{
private:
	Matrix vec;
public:
	ColVector();
	ColVector(const int n);
	ColVector(const ColVector & v);
	~ColVector();

	void set(const double v, const int pos);
	double get(const int pos) const;
	int size() const;

	void transpose(RowVector & result) const;

	void neg(ColVector & result) const;
	void neg_assign();

	void mul(ColVector & result, const Matrix & m) const;
	void mul_assign(const Matrix & m);

	ColVector & operator += (const ColVector & v);
	ColVector & operator -= (const ColVector & v);
	ColVector operator + (const ColVector & v) const;
	ColVector operator - (const ColVector & v) const;

	ColVector & operator = (const ColVector & v);

	friend class RowVector;
};


class bMatrix
{
protected:
	bool *data;
	int size1;
	int size2;

public:
	bMatrix();
	bMatrix(const int m, const int n);
	bMatrix(const bMatrix & B);
	~bMatrix();

	int rows() const;
	int cols() const;

	void output(FILE *fp) const;

	bMatrix & operator += (const bMatrix & B);
	bool * operator [] (const int i);

	bMatrix & operator = (const bMatrix & B);
};


class rMatrix
{
protected:
	Real *data;
	int size1;
	int size2;

public:
	rMatrix();
	rMatrix(const int m, const int n);	// zero matrix
	rMatrix(const int n);				// identity matrix
	rMatrix(const rMatrix & rmatrix);
	~rMatrix();

	int rows() const;
	int cols() const;

	void abs(rMatrix & result) const;
	void transpose(rMatrix & result) const;

	void add_RNDD(rMatrix & result, const rMatrix & rmatrix) const;
	void add_assign_RNDD(const rMatrix & rmatrix);
	void add_RNDU(rMatrix & result, const rMatrix & rmatrix) const;
	void add_assign_RNDU(const rMatrix & rmatrix);

	void mul_RNDD(rMatrix & result, const rMatrix & rmatrix) const;
	void mul_RNDU(rMatrix & result, const rMatrix & rmatrix) const;

	void output(FILE *fp) const;

	rMatrix & operator += (const rMatrix & B);
	rMatrix & operator -= (const rMatrix & B);
	rMatrix & operator *= (const rMatrix & B);
	rMatrix operator + (const rMatrix & B) const;
	rMatrix operator - (const rMatrix & B) const;
	rMatrix operator * (const rMatrix & B) const;

	rMatrix & operator *= (const Real & r);
	rMatrix & operator *= (const double d);

	rMatrix & operator = (const rMatrix & rmatrix);		// should always be the same precision

	Real * operator [] (const int i);

	friend void to_iMatrix2(iMatrix2 & result, const rMatrix & lo, const rMatrix & up);

	friend class iMatrix;
	friend class iMatrix2;
	friend class upMatrix;
};



// matrix for intervals
class iMatrix
{
protected:
	Interval *data;
	int size1;
	int size2;

public:
	iMatrix();
	iMatrix(const int m, const int n);	// zero matrix
	iMatrix(const int n);				// identity matrix
	iMatrix(const iMatrix & A);
	iMatrix(const rMatrix & A);
	iMatrix(const iMatrix2 & A);
	iMatrix(const std::vector<Interval> & box);
	iMatrix(const std::string & matlab_format);
	~iMatrix();

	void clear();

	void mul(iMatrix & result, iMatrix & A);

	int rows() const;
	int cols() const;

	bool isZero() const;

	void pow(iMatrix & result, const int order) const;
	void pow_assign(const int order);

	double max_norm() const;
	void max_norm(Real & norm) const;

	void transpose(iMatrix & result) const;
	void times_pars(mpMatrix & result) const;
	void center();

	void linearTrans(std::vector<Polynomial> & result, const std::vector<Polynomial> & polyVec) const;

	void right_scale_assign(const std::vector<Interval> & scalars);

	void to_iMatrix2(iMatrix2 & A) const;

	void output(FILE *fp) const;

	iMatrix & operator += (const iMatrix & A);
	iMatrix & operator += (const Real & rad);
	iMatrix & operator -= (const iMatrix & A);
	iMatrix & operator *= (const iMatrix & A);
	iMatrix & operator *= (const Interval & I);
	iMatrix & operator *= (const double c);
	iMatrix & operator /= (const Interval & I);
	iMatrix & operator /= (const double c);

	iMatrix operator + (const iMatrix & A) const;
	iMatrix operator - (const iMatrix & A) const;
	iMatrix operator * (const iMatrix & A) const;
	iMatrix operator * (const Interval & I) const;
	iMatrix operator * (const double c) const;
	upMatrix operator * (const upMatrix & upm) const;

	mpMatrix operator * (const mpMatrix & mpm) const;
	TaylorModelVec operator * (const TaylorModelVec & tmv) const;

	Interval * operator [] (const int i);
	iMatrix & operator = (const iMatrix & A);
	iMatrix & operator = (const rMatrix & A);
	iMatrix & operator = (const iMatrix2 & A);
	iMatrix & operator = (const std::vector<Interval> & box);

	friend class upMatrix;
	friend class mpMatrix;
	friend class rMatrix;
	friend class iMatrix2;
	friend class Zonotope;
};


class iMatrix2
{
public:
	rMatrix center;
	rMatrix radius;

public:
	iMatrix2();
	iMatrix2(const int m, const int n);	// zero matrix
	iMatrix2(const int n);				// identity matrix
	iMatrix2(const iMatrix2 & A);
	iMatrix2(const iMatrix & A);
	iMatrix2(const std::vector<Interval> & box);
	~iMatrix2();

	int rows() const;
	int cols() const;

	void to_iMatrix(iMatrix & A) const;
	void transpose(iMatrix2 & result) const;
	void mag(Interval & I, const int i, const int j);
	void mag(Real & r, const int i, const int j);
	void add_assign(const Interval & I, const int i, const int j);

	iMatrix2 & operator += (const iMatrix2 & A);
	iMatrix2 & operator += (const iMatrix & A);
	iMatrix2 & operator += (const Real & rad);
	iMatrix2 & operator *= (const iMatrix2 & A);
	iMatrix2 & operator *= (const Interval & I);

	iMatrix2 operator + (const iMatrix2 & A) const;
	iMatrix2 operator * (const iMatrix2 & A) const;
	iMatrix operator * (const iMatrix & A) const;
	upMatrix operator * (const upMatrix & upm) const;
	TaylorModelVec operator * (const TaylorModelVec & tmv) const;

	iMatrix2 & operator = (const iMatrix2 & A);

	void output(FILE *fp) const;

	friend void to_iMatrix2(iMatrix2 & result, const rMatrix & lo, const rMatrix & up);

	friend class upMatrix;
	friend class mpMatrix;
	friend class rMatrix;
	friend class iMatrix;
	friend class Zonotope;
	friend class Polynomial;
	friend class HybridSystem;
};


// matrix for univariate polynomials
class upMatrix
{
protected:
	UnivariatePolynomial *data;
	int size1;
	int size2;

public:
	upMatrix();
	upMatrix(const int m, const int n);
	upMatrix(const int n);
	upMatrix(const iMatrix & A);
	upMatrix(const iMatrix2 & A);
	upMatrix(const upMatrix & upm);
	~upMatrix();

	int rows() const;
	int cols() const;
	int degree() const;

	bool isZero() const;

	void intEval(iMatrix & result, const std::vector<Interval> & val_exp_table) const;	// interval evaluation based on the monomial form
	void intEval(iMatrix & result, const Interval & val) const;							// interval evaluation based on the Horner form

	void intEval(iMatrix2 & result, const std::vector<Interval> & val_exp_table) const;
	void intEval(iMatrix2 & result, const Interval & val) const;

	void integral();
	void times_x(const int order);
	void transpose(upMatrix & result) const;

	void times_pars(mpMatrix & result) const;

	void ctrunc(iMatrix & rem, const int order, const std::vector<Interval> & val_exp_table);
	void ctrunc(iMatrix & rem, const int order, const Interval & val);

	void ctrunc(iMatrix & rem1, iMatrix & rem2, const int order, const std::vector<Interval> & val1_exp_table, const std::vector<Interval> & val2_exp_table);
	void ctrunc(iMatrix & rem1, iMatrix & rem2, const int order, const Interval & val1, const Interval & val2);

	void ctrunc(const int order, const std::vector<Interval> & val_exp_table);
	void ctrunc(const int order, const Interval & val);

	void nctrunc(const int order);

	void substitute(upMatrix & result, const std::vector<UnivariatePolynomial> & t_exp_table) const;
	void substitute(upMatrix & result, const UnivariatePolynomial & t) const;

	void output(FILE *fp) const;

	void decompose(upMatrix & positive, upMatrix & negative, iMatrix2 & im2_rem) const;

	upMatrix & operator += (const upMatrix & upm);
	upMatrix & operator += (const iMatrix & A);
	upMatrix & operator += (const Real & rad);
	upMatrix & operator -= (const upMatrix & upm);
	upMatrix & operator -= (const iMatrix & A);
	upMatrix & operator *= (const upMatrix & upm);
	upMatrix & operator *= (const iMatrix & A);
	upMatrix & operator *= (const Interval & I);

	upMatrix operator + (const upMatrix & upm) const;
	upMatrix operator + (const iMatrix & A) const;
	upMatrix operator - (const upMatrix & upm) const;
	upMatrix operator - (const iMatrix & A) const;
	upMatrix operator * (const upMatrix & upm) const;
	upMatrix operator * (const iMatrix & A) const;
	upMatrix operator * (const iMatrix2 & A) const;
	upMatrix operator * (const Interval & I) const;
	mpMatrix operator * (const mpMatrix & mpm) const;

	UnivariatePolynomial * operator [] (const int i);

	upMatrix & operator = (const upMatrix & upm);

	friend class iMatrix;
	friend class iMatrix2;
};




// matrix for multivariate polynomials
class mpMatrix
{
protected:
	Polynomial *data;
	int size1;
	int size2;

public:
	mpMatrix();
	mpMatrix(const int m, const int n);
	mpMatrix(const int n);
	mpMatrix(const mpMatrix & mpm);
	~mpMatrix();

	int rows() const;
	int cols() const;

	void intEval(mpMatrix & result, const std::vector<Interval> & val_exp_table) const;
	void intEval(iMatrix & result, const std::vector<Interval> & domain) const;

	void output(FILE *fp, const std::vector<std::string> & varNames) const;

	mpMatrix & operator += (const mpMatrix & mpm);

	mpMatrix operator + (const mpMatrix & mpm) const;

	Polynomial * operator [] (const int i);
	mpMatrix & operator = (const mpMatrix & mpm);

	friend class iMatrix;
	friend class upMatrix;
};


class MatrixParseSetting
{
public:
	std::string strExpression;
	iMatrix result;

public:
	MatrixParseSetting();
	MatrixParseSetting(const MatrixParseSetting & setting);
	~MatrixParseSetting();

	MatrixParseSetting & operator = (const MatrixParseSetting & setting);
};


void to_iMatrix2(iMatrix2 & result, const rMatrix & lo, const rMatrix & up);

extern MatrixParseSetting matrixParseSetting;

}

void parse_Matrix();

#endif /* MATRIX_H_ */
