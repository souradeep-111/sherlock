/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.
  
  The code is released as is under the GNU General Public License (GPL).
---*/

#include "Matrix.h"
#include "Polynomial.h"
#include "TaylorModel.h"

using namespace flowstar;

namespace flowstar
{
MatrixParseSetting matrixParseSetting;
}

Matrix::Matrix()
{
	data = NULL;
}

Matrix::Matrix(const int m, const int n)
{
	data = gsl_matrix_calloc(m,n);
}

Matrix::Matrix(const int n)
{
	data = gsl_matrix_calloc(n,n);
}

Matrix::Matrix(const Matrix & A)
{
	if(A.data != NULL)
	{
		data = gsl_matrix_alloc(A.data->size1, A.data->size2);
		gsl_matrix_memcpy(data, A.data);
	}
	else
	{
		data = NULL;
	}
}

Matrix::~Matrix()
{
	if(data != NULL)
	{
		gsl_matrix_free(data);
	}
}

double Matrix::get(const int i, const int j) const
{
	return gsl_matrix_get(data, i, j);
}

void Matrix::set(const double v, const int i, const int j)
{
	gsl_matrix_set(data, i, j, v);
}

int Matrix::rows() const
{
	return data->size1;
}

int Matrix::cols() const
{
	return data->size2;
}

void Matrix::row(RowVector & result, const int i) const
{
	for(int j=0; j<data->size2; ++j)
	{
		result.set(gsl_matrix_get(data, i, j), j);
	}
}

void Matrix::sortColumns()
{
	int m = data->size1;
	int n = data->size2;

	double *sizes = new double[n];

	// Compute the sizes of the columns
	for(int j=0; j<n; ++j)
	{
		double size = 0;
		for(int i=0; i<m; ++i)
		{
			double tmp = gsl_matrix_get(data, i, j);
			tmp *= tmp;
			size += tmp;
		}
		sizes[j] = size;
	}

	// Selection sort
	double tmp;
	int iMax;

	for(int i=0; i<n-1; ++i)
	{
		iMax = i;
		for(int j=i+1; j<n; ++j)
		{
			if(sizes[j] > sizes[iMax])
			{
				iMax = j;
			}
		}

		//Exchange the columns
		if(iMax != i)
		{
			gsl_matrix_swap_columns(data, i, iMax);
			tmp = sizes[i];
			sizes[i] = sizes[iMax];
			sizes[iMax] = tmp;
		}
	}

	delete[] sizes;
}

int Matrix::rank() const
{
	int m = data->size1;
	int n = data->size2;

	Matrix temp(*this);

	gsl_vector *work = gsl_vector_alloc(n);
	gsl_vector *S = gsl_vector_alloc(n);
	gsl_matrix *X = gsl_matrix_alloc(n,n);
	gsl_matrix *V = gsl_matrix_alloc(n,n);

	gsl_linalg_SV_decomp_mod(temp.data, X, V, S, work);

	int r = 0;
	double tmp;
	for(int i=0; i<n; ++i)
	{
		tmp = gsl_vector_get(S, i);
		if(tmp < THRESHOLD_HIGH)
			break;
		else
			++r;
	}

	gsl_vector_free(S);
	gsl_vector_free(work);
	gsl_matrix_free(X);
	gsl_matrix_free(V);

	return r;
}

void Matrix::transpose(Matrix & result) const
{
	gsl_matrix_transpose_memcpy(result.data, data);
}

void Matrix::svd(Matrix & U) const
{
	U = *this;
	int m = data->size1;
	int n = data->size2;

	gsl_vector *work = gsl_vector_alloc(n);
	gsl_vector *S = gsl_vector_alloc(n);
	gsl_matrix *V = gsl_matrix_alloc(n,n);

	gsl_linalg_SV_decomp(U.data, V, S, work);

	gsl_vector_free(S);
	gsl_vector_free(work);
	gsl_matrix_free(V);
}

void Matrix::neg(Matrix & result) const
{
	gsl_matrix_memcpy(result.data, data);
	gsl_matrix_scale(result.data, -1.0);
}

void Matrix::neg_assign()
{
	gsl_matrix_scale(data, -1.0);
}

void Matrix::inverse(Matrix & result) const
{
	// only square matrix
	int m = data->size1;
	int n = data->size2;

	if(m != n)
	{
		printf("Not a square matrix.\n");
		return;
	}

	// We use GSL library.
	gsl_matrix *A = gsl_matrix_alloc(m,m);
	gsl_permutation *p = gsl_permutation_alloc(m);
	gsl_matrix *invA = gsl_matrix_alloc(m,m);

	// Make a copy the matrix.
	gsl_matrix_memcpy(A, data);

	int *signum = new int[m];

	gsl_linalg_LU_decomp(A, p, signum);
	gsl_linalg_LU_invert(A, p, invA);

	gsl_matrix_memcpy(result.data, invA);

	gsl_matrix_free(A);
	gsl_permutation_free(p);
	gsl_matrix_free(invA);
	delete[] signum;
}

void Matrix::inverse_assign()
{
	// only square matrix
	int m = data->size1;
	int n = data->size2;

	if(m != n)
	{
		printf("Not a square matrix.\n");
		return;
	}

	// We use GSL library.
	gsl_matrix *A = gsl_matrix_alloc(m,m);
	gsl_permutation *p = gsl_permutation_alloc(m);
	gsl_matrix *invA = gsl_matrix_alloc(m,m);

	// Make a copy the matrix.
	gsl_matrix_memcpy(A, data);

	int *signum = new int[m];

	gsl_linalg_LU_decomp(A, p, signum);
	gsl_linalg_LU_invert(A, p, invA);

	gsl_matrix_memcpy(data, invA);

	gsl_matrix_free(A);
	gsl_permutation_free(p);
	gsl_matrix_free(invA);
	delete[] signum;
}

Matrix & Matrix::operator += (const Matrix & A)
{
	gsl_matrix_add(data, A.data);

	return *this;
}

Matrix & Matrix::operator -= (const Matrix & A)
{
	gsl_matrix_sub(data, A.data);

	return *this;
}

Matrix & Matrix::operator *= (const Matrix & A)
{
	int m = data->size1;
	int n = A.data->size2;
	int k = data->size2;

	Matrix result(m,n);

	for(int i=0; i<m; ++i)
	{
		for(int j=0; j<n; ++j)
		{
			double tmp = 0;
			for(int p=0; p<k; ++p)
			{
				tmp += gsl_matrix_get(data, i, p) * gsl_matrix_get(A.data, p, j);
			}

			gsl_matrix_set(result.data, i, j, tmp);
		}
	}

	*this = result;
	return *this;
}

Matrix Matrix::operator + (const Matrix & A) const
{
	Matrix result = *this;

	result += A;

	return result;
}

Matrix Matrix::operator - (const Matrix & A) const
{
	Matrix result = *this;

	result -= A;

	return result;
}

Matrix Matrix::operator * (const Matrix & A) const
{
	int m = data->size1;
	int n = A.data->size2;
	int k = data->size2;

	Matrix result(m,n);

	for(int i=0; i<m; ++i)
	{
		for(int j=0; j<n; ++j)
		{
			double tmp = 0;
			for(int p=0; p<k; ++p)
			{
				tmp += gsl_matrix_get(data, i, p) * gsl_matrix_get(A.data, p, j);
			}
			gsl_matrix_set(result.data, i, j, tmp);
		}
	}

	return result;
}

void Matrix::QR(Matrix & D)
{
	int m = data->size1;
	int n = data->size2;

	int i,j,k,l;

	k = 0;
	double s,t,x,r;

	for(l=0; l<n; ++l)
	{
		if(k == m)
		{
			gsl_matrix_set(D.data, 0, l, gsl_matrix_get(data, k, l));
			break;
		}
		s = 0;

		for (i=k; i<m; ++i)
		{
			x = gsl_matrix_get(data, i, l);
			s += x*x;
		}
		s = sqrt(s);

		if(s == 0)
		{
			gsl_matrix_set(D.data, 0, 0, 0);
			continue;
		}

		t = gsl_matrix_get(data, k, l);
		r = 1/sqrt( s*(s+fabs(t)) );
		if(t < 0)
			s = -s;

		gsl_matrix_set(D.data, 0, l, -s);
		gsl_matrix_set(data, k, k, r * (t + s));

		for(i=k+1; i<m; ++i)
		{
			double tmp = gsl_matrix_get(data, i, l) * r;
			gsl_matrix_set(data, i, k, tmp);
		}

		for(j=l+1; j<n; ++j)
		{
			t = 0;
			for(i=k; i<m; ++i)
				t += gsl_matrix_get(data, i, k) * gsl_matrix_get(data, i, j);

			for(i=k; i<m; ++i)
			{
				double tmp = gsl_matrix_get(data, i, j);
				gsl_matrix_set(data, i, j, tmp - gsl_matrix_get(data, i, k)*t);
			}
		}

		++k;
    }
}

void Matrix::QRfactor(Matrix & Q)
{
	int m = data->size1;
	int n = data->size2;

	if(n == 1)
    {
		gsl_matrix_set(Q.data, 0, 0, 1);
		return;
    }

	Q = *this;

	Matrix D(1,n);

	Q.QR(D);

	Matrix V(1,n);

	gsl_matrix_set(V.data, 0, n-2, gsl_matrix_get(Q.data, n-2, n-2));
	gsl_matrix_set(V.data, 0, n-1, gsl_matrix_get(Q.data, n-1, n-2));

	gsl_matrix_set(Q.data, n-1, n-2, - gsl_matrix_get(V.data, 0, n-2) * gsl_matrix_get(V.data, 0, n-1));
	gsl_matrix_set(Q.data, n-2, n-1, gsl_matrix_get(Q.data, n-1, n-2));

	gsl_matrix_set(Q.data, n-1, n-1, 1.0 - gsl_matrix_get(V.data, 0, n-1) * gsl_matrix_get(V.data, 0, n-1));
	gsl_matrix_set(Q.data, n-2, n-2, 1.0 - gsl_matrix_get(V.data, 0, n-2) * gsl_matrix_get(V.data, 0, n-2));

	int k, row, col, i;
	double a, b;
	for(k = n-3; k>=0; --k)
    {
		for(row = k; row<n; ++row)
			gsl_matrix_set(V.data, 0, row, gsl_matrix_get(Q.data, row, k));

		for(col = k+1; col<n; ++col)
		{
			a = 0;
			for(i = k+1; i<n; ++i)
				a += gsl_matrix_get(V.data, 0, i) * gsl_matrix_get(Q.data, i, col);

			for(row = k+1; row<n; ++row)
			{
				b = gsl_matrix_get(Q.data, row, col);
				gsl_matrix_set(Q.data, row, col, b - a * gsl_matrix_get(V.data, 0, row));
			}

			gsl_matrix_set(Q.data, k, col, -a * gsl_matrix_get(V.data, 0, k));
		}

		for(i = k+1; i<n; ++i)
			gsl_matrix_set(Q.data, i, k, - gsl_matrix_get(V.data, 0, k) * gsl_matrix_get(V.data, 0, i));

		gsl_matrix_set(Q.data, k, k, 1.0 - gsl_matrix_get(V.data, 0, k) * gsl_matrix_get(V.data, 0, k));
    }
}

void Matrix::output(FILE *fp) const
{
	int m = data->size1;
	int n = data->size2;

	fprintf(fp, "==========\n");
	for(int i=0; i<m; ++i)
	{
		for(int j=0; j<n-1; ++j)
		{
			fprintf(fp, "%lf, ", gsl_matrix_get(data, i, j));
		}
		fprintf(fp, "%lf\n", gsl_matrix_get(data, i, n-1));
	}
	fprintf(fp, "==========\n");
}

Matrix & Matrix::operator = (const Matrix & A)
{
	if(A.data == NULL)
	{
		if(data != NULL)
		{
			gsl_matrix_free(data);
			data = NULL;
		}
	}
	else
	{
		if(data != NULL)
			gsl_matrix_free(data);

		data = gsl_matrix_alloc(A.data->size1, A.data->size2);

		gsl_matrix_memcpy(data, A.data);
	}

	return *this;
}










// class RowVector

RowVector::RowVector()
{
}

RowVector::RowVector(const int n)
{
	Matrix mat(1,n);
	vec = mat;
}

RowVector::RowVector(const RowVector & v)
{
	vec = v.vec;
}

RowVector::~RowVector()
{
}

void RowVector::set(const double v, const int pos)
{
	vec.set(v, 0, pos);
}

double RowVector::get(const int pos) const
{
	return vec.get(0, pos);
}

int RowVector::size() const
{
	return vec.cols();
}

void RowVector::transpose(ColVector & result) const
{
	Matrix mat(vec.cols(), 1);
	vec.transpose(mat);
	result.vec = mat;
}

void RowVector::neg(RowVector & result) const
{
	result = *this;
	result.vec.neg_assign();
}

void RowVector::neg_assign()
{
	vec.neg_assign();
}

void RowVector::dump(FILE *fp) const
{
	fprintf(fp, "[ ");
	for(int i=0; i<vec.cols()-1; ++i)
	{
		fprintf(fp, "%lf, ", get(i));
	}
	fprintf(fp, "%lf ]\n", get(vec.cols()-1));
}

double RowVector::innerProd(const RowVector & v) const
{
	int n = vec.cols();

	if(n != v.size())
	{
		printf("Vector dimensions do not coincide.\n");
		return INVALID;
	}

	double result = 0;
	for(int i=0; i<n; ++i)
	{
		result += get(i)*v.get(i);
	}

	return result;
}

double RowVector::EuclideanNorm() const
{
	int n = vec.cols();
	double result = 0;

	for(int i=0; i<n; ++i)
	{
		result += get(i)*get(i);
	}

	return sqrt(result);
}

void RowVector::normalize()
{
	double norm = EuclideanNorm();
	int n = vec.cols();

	for(int i=0; i<n; ++i)
	{
		double tmp = get(i) / norm;
		set(tmp, i);
	}
}

bool RowVector::operator == (const RowVector & v) const
{
	if(vec.cols() == v.size())
	{
		for(int i=0; i<vec.cols(); ++i)
		{
			double d = vec.get(0,i) - v.get(i);
			if(!(d <= THRESHOLD_LOW && d >= -THRESHOLD_LOW))
			{
				return false;
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

RowVector & RowVector::operator += (const RowVector & v)
{
	vec += v.vec;
	return *this;
}

RowVector & RowVector::operator -= (const RowVector & v)
{
	vec -= v.vec;
	return *this;
}

RowVector RowVector::operator + (const RowVector & v) const
{
	RowVector result = *this;
	result += v;
	return result;
}

RowVector RowVector::operator - (const RowVector & v) const
{
	RowVector result = *this;
	result -= v;
	return result;
}

RowVector & RowVector::operator = (const RowVector & v)
{
	if(this == &v)
		return *this;

	vec = v.vec;
	return *this;
}









// class ColVector

ColVector::ColVector()
{
}

ColVector::ColVector(const int n)
{
	Matrix mat(n,1);
	vec = mat;
}

ColVector::ColVector(const ColVector & v)
{
	vec = v.vec;
}

ColVector::~ColVector()
{
}

void ColVector::set(const double v, const int pos)
{
	vec.set(v, pos, 0);
}

double ColVector::get(const int pos) const
{
	return vec.get(pos, 0);
}

int ColVector::size() const
{
	return vec.rows();
}

void ColVector::transpose(RowVector & result) const
{
	Matrix mat(1, vec.cols());
	vec.transpose(mat);
	result.vec = mat;
}

void ColVector::neg(ColVector & result) const
{
	result = *this;
	result.vec.neg_assign();
}

void ColVector::neg_assign()
{
	vec.neg_assign();
}

void ColVector::mul(ColVector & result, const Matrix & m) const
{
	int rows = m.rows();
	int cols = m.cols();

	if(cols != vec.rows())
	{
		printf("Vector multiplication error: invalid dimensions.\n");
		return;
	}

	for(int i=0; i<rows; ++i)
	{
		double sum = 0;
		for(int j=0; j<cols; ++j)
		{
			sum += m.get(i,j) * vec.get(j,0);
		}

		result.vec.set(sum, i, 0);
	}
}

void ColVector::mul_assign(const Matrix & m)
{
	ColVector result(m.rows());

	mul(result, m);
	*this = result;
}

ColVector & ColVector::operator += (const ColVector & v)
{
	vec += v.vec;
	return *this;
}

ColVector & ColVector::operator -= (const ColVector & v)
{
	vec -= v.vec;
	return *this;
}

ColVector ColVector::operator + (const ColVector & v) const
{
	ColVector result = *this;
	result += v;
	return result;
}

ColVector ColVector::operator - (const ColVector & v) const
{
	ColVector result = *this;
	result -= v;
	return result;
}

ColVector & ColVector::operator = (const ColVector & v)
{
	if(this == &v)
		return *this;

	vec = v.vec;
	return *this;
}



// class for boolean matrices

bMatrix::bMatrix()
{
	data = NULL;
	size1 = 0;
	size2 = 0;
}

bMatrix::bMatrix(const int m, const int n)
{
	size1 = m;
	size2 = n;
	data = new bool[m * n];
}

bMatrix::bMatrix(const bMatrix & B)
{
	size1 = B.size1;
	size2 = B.size2;

	int size_total = size1 * size2;
	data = new bool[size_total];
	std::copy(B.data, B.data + size_total, data);
}

bMatrix::~bMatrix()
{
	delete [] data;
}

int bMatrix::rows() const
{
	return size1;
}

int bMatrix::cols() const
{
	return size2;
}

void bMatrix::output(FILE *fp) const
{
	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<size2; ++j)
		{
			if(data[i* size2 + j])
			{
				fprintf(fp, "1\t");
			}
			else
			{
				fprintf(fp, "0\t");
			}
		}

		fprintf(fp, "\n");
	}
}

bMatrix & bMatrix::operator += (const bMatrix & B)
{
	if(size1 != B.size1 || size2 != B.size2)
	{
		printf("Boolean matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		if(data[i] == true || B.data[i] == true)
		{
			data[i] = true;
		}
		else
		{
			data[i] = false;
		}
	}

	return *this;
}

bool * bMatrix::operator [] (const int i)
{
	return &data[i * size2];
}

bMatrix & bMatrix::operator = (const bMatrix & B)
{
	if(this == &B)
		return *this;

	size1 = B.size1;
	size2 = B.size2;

	int size_total = size1 * size2;
	delete [] data;

	if(size_total > 0)
	{
		data = new bool[size_total];
		std::copy(B.data, B.data + size_total, data);
	}
	else
	{
		data = NULL;
	}

	return *this;
}






// class for real matrices

rMatrix::rMatrix()
{
	size1 = 0;
	size2 = 0;
	data = NULL;
}

rMatrix::rMatrix(const int m, const int n)
{
	size1 = m;
	size2 = n;
	data = new Real[m * n];
}

rMatrix::rMatrix(const int n)
{
	size1 = n;
	size2 = n;
	data = new Real[n * n];

	Real one(1);

	for(int i=0; i<n; ++i)
	{
		data[i*n + i] = one;
	}
}

rMatrix::rMatrix(const rMatrix & rmatrix)
{
	size1 = rmatrix.size1;
	size2 = rmatrix.size2;

	int size_total = size1 * size2;
	data = new Real[size_total];
	std::copy(rmatrix.data, rmatrix.data + size_total, data);
}

rMatrix::~rMatrix()
{
	delete [] data;
}

int rMatrix::rows() const
{
	return size1;
}

int rMatrix::cols() const
{
	return size2;
}

void rMatrix::abs(rMatrix & result) const
{
	result.size1 = size1;
	result.size2 = size2;

	int size_total = size1 * size2;
	delete [] result.data;
	result.data = new Real[size_total];

	for(int i=0; i<size_total; ++i)
	{
		data[i].abs(result.data[i]);
	}
}

void rMatrix::transpose(rMatrix & result) const
{
	delete [] result.data;
	result.size1 = size2;
	result.size2 = size1;
	result.data = new Real[size2 * size1];

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<size2; ++j)
		{
			result.data[i*size2 + j] = data[j*size1 + i];
		}
	}
}

void rMatrix::add_RNDD(rMatrix & result, const rMatrix & rmatrix) const
{
	if(size1 != rmatrix.size1 || size2 != rmatrix.size2)
	{
		printf("Real matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	result.size1 = size1;
	result.size2 = size2;

	int size_total = size1 * size2;
	delete [] result.data;
	result.data = new Real[size_total];

	for(int i=0; i<size_total; ++i)
	{
		data[i].add_RNDD(result.data[i], rmatrix.data[i]);
	}
}

void rMatrix::add_assign_RNDD(const rMatrix & rmatrix)
{
	if(size1 != rmatrix.size1 || size2 != rmatrix.size2)
	{
		printf("Real matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i].add_assign_RNDD(rmatrix.data[i]);
	}
}

void rMatrix::add_RNDU(rMatrix & result, const rMatrix & rmatrix) const
{
	if(size1 != rmatrix.size1 || size2 != rmatrix.size2)
	{
		printf("Real matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	result.size1 = size1;
	result.size2 = size2;

	int size_total = size1 * size2;
	delete [] result.data;
	result.data = new Real[size_total];

	for(int i=0; i<size_total; ++i)
	{
		data[i].add_RNDU(result.data[i], rmatrix.data[i]);
	}
}

void rMatrix::add_assign_RNDU(const rMatrix & rmatrix)
{
	if(size1 != rmatrix.size1 || size2 != rmatrix.size2)
	{
		printf("Real matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i].add_assign_RNDU(rmatrix.data[i]);
	}
}

void rMatrix::mul_RNDD(rMatrix & result, const rMatrix & rmatrix) const
{
	if(size2 != rmatrix.size1)
	{
		printf("Real matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	result.size1 = size1;
	result.size2 = rmatrix.size2;

	delete [] result.data;
	int size_total = size1 * rmatrix.size2;;
	result.data = new Real[size_total];

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<rmatrix.size2; ++j)
		{
			Real tmp1;

			for(int p=0; p<size2; ++p)
			{
				Real tmp2;
				data[i*size2 + p].mul_RNDD(tmp2, rmatrix.data[p*rmatrix.size2 + j]);
				tmp1.add_assign_RNDD(tmp2);
			}

			result.data[i*rmatrix.size2 + j] = tmp1;
		}
	}
}

void rMatrix::mul_RNDU(rMatrix & result, const rMatrix & rmatrix) const
{
	if(size2 != rmatrix.size1)
	{
		printf("Real matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	result.size1 = size1;
	result.size2 = rmatrix.size2;

	delete [] result.data;
	int size_total = size1 * rmatrix.size2;;
	result.data = new Real[size_total];

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<rmatrix.size2; ++j)
		{
			Real tmp1;

			for(int p=0; p<size2; ++p)
			{
				Real tmp2;
				data[i*size2 + p].mul_RNDU(tmp2, rmatrix.data[p*rmatrix.size2 + j]);
				tmp1.add_assign_RNDD(tmp2);
			}

			result.data[i*rmatrix.size2 + j] = tmp1;
		}
	}
}

void rMatrix::output(FILE *fp) const
{
	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<size2; ++j)
		{
			data[i*size2 + j].output(fp);
			fprintf(fp, "\t");
		}

		fprintf(fp, "\n");
	}
}

rMatrix & rMatrix::operator += (const rMatrix & B)
{
	if(size1 != B.size1 || size2 != B.size2)
	{
		printf("Real matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] += B.data[i];
	}

	return *this;
}

rMatrix & rMatrix::operator -= (const rMatrix & B)
{
	if(size1 != B.size1 || size2 != B.size2)
	{
		printf("Real matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] -= B.data[i];
	}

	return *this;
}

rMatrix & rMatrix::operator *= (const rMatrix & B)
{
	if(size2 != B.size1)
	{
		printf("Real matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	rMatrix result(size1, B.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<B.size2; ++j)
		{
			Real tmp;

			for(int p=0; p<size2; ++p)
			{
				tmp += data[i*size2 + p] * B.data[p*B.size2 + j];
			}

			result.data[i*B.size2 + j] = tmp;
		}
	}

	*this = result;
	return *this;
}

rMatrix rMatrix::operator + (const rMatrix & B) const
{
	rMatrix result = *this;
	result += B;
	return result;
}

rMatrix rMatrix::operator - (const rMatrix & B) const
{
	rMatrix result = *this;
	result -= B;
	return result;
}

rMatrix rMatrix::operator * (const rMatrix & B) const
{
	if(size2 != B.size1)
	{
		printf("Real matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	rMatrix result(size1, B.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<B.size2; ++j)
		{
			Real tmp;

			for(int p=0; p<size2; ++p)
			{
				tmp += data[i*size2 + p] * B.data[p*B.size2 + j];
			}

			result.data[i*B.size2 + j] = tmp;
		}
	}

	return result;
}

rMatrix & rMatrix::operator *= (const Real & r)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] *= r;
	}

	return *this;
}

rMatrix & rMatrix::operator *= (const double d)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] *= d;
	}

	return *this;
}

rMatrix & rMatrix::operator = (const rMatrix & rmatrix)
{
	if(this == &rmatrix)
		return *this;

	size1 = rmatrix.size1;
	size2 = rmatrix.size2;

	int size_total = size1 * size2;
	delete [] data;

	if(size_total > 0)
	{
		data = new Real[size_total];
		std::copy(rmatrix.data, rmatrix.data + size_total, data);
	}
	else
	{
		data = NULL;
	}

	return *this;
}

Real * rMatrix::operator [] (const int i)
{
	return &data[i * size2];
}




// matrix for intervals

iMatrix::iMatrix()
{
	size1 = 0;
	size2 = 0;
	data = NULL;
}

iMatrix::iMatrix(const int m, const int n)
{
	size1 = m;
	size2 = n;
	data = new Interval[m * n];
}

iMatrix::iMatrix(const int n)
{
	size1 = n;
	size2 = n;
	data = new Interval[n * n];

	Interval intOne(1);
	for(int i=0; i<n; ++i)
	{
		data[i*n + i] = intOne;
	}
}


iMatrix::iMatrix(const iMatrix & A)
{
	size1 = A.size1;
	size2 = A.size2;

	int size_total = size1 * size2;
	data = new Interval[size_total];
	std::copy(A.data, A.data + size_total, data);
}

iMatrix::iMatrix(const rMatrix & A)
{
	size1 = A.size1;
	size2 = A.size2;

	int size_total = size1 * size2;
	data = new Interval[size_total];

	for(int i=0; i<size_total; ++i)
	{
		Interval I(A.data[i]);
		data[i] = I;
	}
}

iMatrix::iMatrix(const iMatrix2 & A)
{
	size1 = A.center.size1;
	size2 = A.center.size2;

	int size_total = size1 * size2;
	data = new Interval[size_total];

	for(int i=0; i<size_total; ++i)
	{
		Interval I(A.center.data[i], A.radius.data[i]);
		data[i] = I;
	}
}

iMatrix::iMatrix(const std::vector<Interval> & box)
{
	size1 = (int)box.size();
	size2 = 1;
	data = new Interval[size1];

	for(int i=0; i<size1; ++i)
	{
		data[i] = box[i];
	}
}

iMatrix::iMatrix(const std::string & matlab_format)
{
	std::string prefix(str_prefix_matrix);
	std::string suffix(str_suffix);

	matrixParseSetting.strExpression = prefix + matlab_format + suffix;

	parse_Matrix();

	size1 = matrixParseSetting.result.size1;
	size2 = matrixParseSetting.result.size2;

	int size_total = size1 * size2;
	data = new Interval[size_total];

	for(int i=0; i<size_total; ++i)
	{
		Interval I(matrixParseSetting.result.data[i]);
		data[i] = I;
	}
}

iMatrix::~iMatrix()
{
	delete [] data;
}

void iMatrix::clear()
{
	size1 = 0;
	size2 = 0;
	delete [] data;
}

int iMatrix::rows() const
{
	return size1;
}

int iMatrix::cols() const
{
	return size2;
}

bool iMatrix::isZero() const
{
	bool result = true;

	int size_total = size1 * size2;
	Interval intZero;

	for(int i=0; i<size_total; ++i)
	{
		if(!data[i].subseteq(intZero))
		{
			result = false;
			break;
		}
	}

	return result;
}

void iMatrix::pow(iMatrix & result, const int order) const
{
	iMatrix temp = *this;
	result = *this;

	for(int d = order - 1; d > 0;)
	{
		if(d & 1)
		{
			result *= temp;
		}

		d >>= 1;

		if(d > 0)
		{
			temp *= temp;
		}
	}
}

void iMatrix::pow_assign(const int order)
{
	iMatrix temp = *this;
	iMatrix result = *this;

	for(int d = order - 1; d > 0;)
	{
		if(d & 1)
		{
			result *= temp;
		}

		d >>= 1;

		if(d > 0)
		{
			temp *= temp;
		}
	}

	*this = result;
}

double iMatrix::max_norm() const
{
	double max = 0;

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		double tmp = data[i].mag();

		if(tmp > max)
		{
			max = tmp;
		}
	}

	return max;
}

void iMatrix::max_norm(Real & norm) const
{
	double max = 0;

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		double tmp = data[i].mag();

		if(tmp > max)
		{
			max = tmp;
		}
	}

	Interval result(max);
	norm = max;
}

void iMatrix::transpose(iMatrix & result) const
{
	delete [] result.data;
	result.size1 = size2;
	result.size2 = size1;
	result.data = new Interval[size2 * size1];

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<size2; ++j)
		{
			result.data[i*size2 + j] = data[j*size1 + i];
		}
	}
}

void iMatrix::times_pars(mpMatrix & result) const
{
	delete [] result.data;
	result.size1 = size1;
	result.size2 = 1;
	result.data = new Polynomial[size1];

	for(int i=0; i<size1; ++i)
	{
		Polynomial tmp;
		for(int j=0; j<size2; ++j)
		{
			Monomial monomial(data[i * size2 + j], size1 + 1);
			monomial.d = 1;
			monomial.degrees[j] = 1;

			tmp.add_assign(monomial);
		}

		result[i][0] = tmp;
	}
}

void iMatrix::center()
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		Interval M;
		data[i].remove_midpoint(M);
		data[i] = M;
	}
}

void iMatrix::linearTrans(std::vector<Polynomial> & result, const std::vector<Polynomial> & polyVec) const
{
	if(size2 != polyVec.size())
	{
		printf("Interval matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	result.clear();

	for(int i=0; i<size1; ++i)
	{
		Polynomial poly1;

		for(int j=0; j<size2; ++j)
		{
			Polynomial poly2 = polyVec[j];
			poly2 *= data[i*size2 + j];
			poly1 += poly2;
		}

		result.push_back(poly1);
	}
}

void iMatrix::right_scale_assign(const std::vector<Interval> & scalars)
{
	if(size2 != scalars.size())
	{
		printf("Interval matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<size2; ++j)
		{
			data[i*size2 + j] *= scalars[j];
		}
	}
}

void iMatrix::to_iMatrix2(iMatrix2 & A) const
{
	int size_total = size1 * size2;

	rMatrix center(size1, size2);
	rMatrix radius(size1, size2);

	for(int i=0; i<size_total; ++i)
	{
		Real c, r;
		data[i].toCenterForm(c, r);

		center.data[i] = c;
		radius.data[i] = r;
	}

	A.center = center;
	A.radius = radius;
}

void iMatrix::output(FILE *fp) const
{
	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<size2; ++j)
		{
			data[i*size2 + j].dump(fp);
			fprintf(fp, "\t");
		}

		fprintf(fp, "\n");
	}
}

iMatrix & iMatrix::operator += (const iMatrix & A)
{
	if(size1 != A.size1 || size2 != A.size2)
	{
		printf("Interval matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] += A.data[i];
	}

	return *this;
}

iMatrix & iMatrix::operator += (const Real & rad)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i].bloat(rad);
	}

	return *this;
}

iMatrix & iMatrix::operator -= (const iMatrix & A)
{
	if(size1 != A.size1 || size2 != A.size2)
	{
		printf("Interval matrix subtraction: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] -= A.data[i];
	}

	return *this;
}

iMatrix & iMatrix::operator *= (const iMatrix & A)
{
	if(size2 != A.size1)
	{
		printf("Interval matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	iMatrix result(size1, A.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<A.size2; ++j)
		{
			Interval tmp;

			for(int p=0; p<size2; ++p)
			{
				tmp += data[i*size2 + p] * A.data[p*A.size2 + j];
			}

			result.data[i*A.size2 + j] = tmp;
		}
	}

	*this = result;
	return *this;
}

void iMatrix::mul(iMatrix & result, iMatrix & A)
{
	if(size2 != A.size1)
	{
		printf("Interval matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	iMatrix im_tmp(size1, A.size2);
	result = im_tmp;

	for(int i=0; i<size1; ++i)
	{
		Interval *iTmp = new Interval[A.size2];

		for(int j=0; j<size2; ++j)
		{
			data[i*size2 + j].mul_add(iTmp, &A.data[j*A.size2], A.size2);
		}

		for(int j=0; j<A.size2; ++j)
		{
			result[i][j] = iTmp[j];
		}

		delete [] iTmp;
	}
}

iMatrix & iMatrix::operator *= (const Interval & I)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] *= I;
	}

	return *this;
}

iMatrix & iMatrix::operator *= (const double c)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] *= c;
	}

	return *this;
}

iMatrix & iMatrix::operator /= (const Interval & I)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] /= I;
	}

	return *this;
}

iMatrix & iMatrix::operator /= (const double c)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] /= c;
	}

	return *this;
}

iMatrix iMatrix::operator + (const iMatrix & A) const
{
	iMatrix result = *this;
	result += A;
	return result;
}

iMatrix iMatrix::operator - (const iMatrix & A) const
{
	iMatrix result = *this;
	result -= A;
	return result;
}

iMatrix iMatrix::operator * (const iMatrix & A) const
{
	if(size2 != A.size1)
	{
		printf("Interval matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	iMatrix result(size1, A.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<A.size2; ++j)
		{
			Interval tmp;

			for(int p=0; p<size2; ++p)
			{
				tmp += data[i*size2 + p] * A.data[p*A.size2 + j];
			}

			result.data[i*A.size2 + j] = tmp;
		}
	}

	return result;
}

iMatrix iMatrix::operator * (const Interval & I) const
{
	iMatrix result = *this;
	result *= I;
	return result;
}

iMatrix iMatrix::operator * (const double c) const
{
	iMatrix result = *this;
	result *= c;
	return result;
}

upMatrix iMatrix::operator * (const upMatrix & upm) const
{
	if(size2 != upm.size1)
	{
		printf("Interval matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	upMatrix result(size1, upm.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<upm.size2; ++j)
		{
			UnivariatePolynomial tmp;

			for(int p=0; p<size2; ++p)
			{
				tmp += upm.data[p*upm.size2 + j] * data[i*size2 + p];
			}

			result.data[i*upm.size2 + j] = tmp;
		}
	}

	return result;
}

mpMatrix iMatrix::operator * (const mpMatrix & mpm) const
{
	if(size2 != mpm.size1)
	{
		printf("Interval matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	mpMatrix result(size1, mpm.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<mpm.size2; ++j)
		{
			Polynomial tmp;

			for(int p=0; p<size2; ++p)
			{
				tmp += mpm.data[p*mpm.size2 + j] * data[i*size2 + p];
			}

			result.data[i*mpm.size2 + j] = tmp;
		}
	}

	return result;
}

TaylorModelVec iMatrix::operator * (const TaylorModelVec & tmv) const
{
	if(size2 != tmv.tms.size())
	{
		printf("Interval matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	TaylorModelVec result;

	for(int i=0; i<size1; ++i)
	{
		TaylorModel tmTemp1;

		for(int j=0; j<size2; ++j)
		{
			TaylorModel tmTemp2;
			tmv.tms[j].mul(tmTemp2, data[i*size2 + j]);
			tmTemp1.add_assign(tmTemp2);
		}

		result.tms.push_back(tmTemp1);
	}

	return result;
}

Interval * iMatrix::operator [] (const int i)
{
	return &data[i * size2];
}

iMatrix & iMatrix::operator = (const iMatrix & A)
{
	if(this == &A)
		return *this;

	size1 = A.size1;
	size2 = A.size2;

	int size_total = size1 * size2;
	delete [] data;

	if(size_total > 0)
	{
		data = new Interval[size_total];
		std::copy(A.data, A.data + size_total, data);
	}
	else
	{
		data = NULL;
	}

	return *this;
}

iMatrix & iMatrix::operator = (const rMatrix & A)
{
	size1 = A.size1;
	size2 = A.size2;

	int size_total = size1 * size2;
	delete [] data;

	if(size_total > 0)
	{
		data = new Interval[size_total];

		for(int i=0; i<size_total; ++i)
		{
			Interval I(A.data[i]);
			data[i] = I;
		}
	}
	else
	{
		data = NULL;
	}

	return *this;
}

iMatrix & iMatrix::operator = (const iMatrix2 & A)
{
	size1 = A.center.size1;
	size2 = A.center.size2;

	int size_total = size1 * size2;
	delete [] data;

	if(size_total > 0)
	{
		data = new Interval[size_total];

		for(int i=0; i<size_total; ++i)
		{
			Interval I(A.center.data[i], A.radius.data[i]);
			data[i] = I;
		}
	}
	else
	{
		data = NULL;
	}

	return *this;
}

iMatrix & iMatrix::operator = (const std::vector<Interval> & box)
{
	size1 = (int)box.size();
	size2 = 1;
	delete [] data;

	if(size1 > 0)
	{
		data = new Interval[size1];

		for(int i=0; i<size1; ++i)
		{
			data[i] = box[i];
		}
	}
	else
	{
		data = NULL;
	}

	return *this;
}


// transformation matrix for linear systems

iMatrix2::iMatrix2()
{
}

iMatrix2::iMatrix2(const int m, const int n)
{
	rMatrix zero(m, n);

	center = zero;
	radius = zero;
}

iMatrix2::iMatrix2(const int n)
{
	rMatrix id(n), zero(n, n);

	center = id;
	radius = zero;
}

iMatrix2::iMatrix2(const iMatrix2 & A)
{
	center = A.center;
	radius = A.radius;
}

iMatrix2::iMatrix2(const iMatrix & A)
{
	rMatrix rm(A.size1, A.size2);

	center = rm;
	radius = rm;

	int size_total = A.size1 * A.size2;
	for(int i=0; i<size_total; ++i)
	{
		Real c, r;
		A.data[i].toCenterForm(c, r);

		center.data[i] = c;
		radius.data[i] = r;
	}
}

iMatrix2::iMatrix2(const std::vector<Interval> & box)
{
	int size1 = (int)box.size();

	rMatrix rm(size1, 1);
	center = rm;
	radius = rm;

	for(int i=0; i<size1; ++i)
	{
		Real c, r;
		box[i].toCenterForm(c, r);

		center.data[i] = c;
		radius.data[i] = r;
	}
}

iMatrix2::~iMatrix2()
{
}

int iMatrix2::rows() const
{
	return center.rows();
}

int iMatrix2::cols() const
{
	return center.cols();
}

void iMatrix2::to_iMatrix(iMatrix & A) const
{
	int size_total = center.size1 * center.size2;

	A.size1 = center.size1;
	A.size2 = center.size2;

	delete [] A.data;
	A.data = new Interval[size_total];

	for(int i=0; i<size_total; ++i)
	{
		Interval I(center.data[i], radius.data[i]);
		A.data[i] = I;
	}
}

void iMatrix2::transpose(iMatrix2 & result) const
{
	center.transpose(result.center);
	radius.transpose(result.radius);
}

void iMatrix2::mag(Interval & I, const int i, const int j)
{
	Real tmp1, tmp2;
	center[i][j].sub_RNDD(tmp1, radius[i][j]);
	center[i][j].add_RNDU(tmp2, radius[i][j]);

	tmp1.abs_assign();
	tmp2.abs_assign();

	if(tmp1 > tmp2)
	{
		I.set(tmp1);
	}
	else
	{
		I.set(tmp2);
	}
}

void iMatrix2::mag(Real & r, const int i, const int j)
{
	Real tmp1, tmp2;
	center[i][j].sub_RNDD(tmp1, radius[i][j]);
	center[i][j].add_RNDU(tmp2, radius[i][j]);

	tmp1.abs_assign();
	tmp2.abs_assign();

	if(tmp1 > tmp2)
	{
		r = tmp1;
	}
	else
	{
		r = tmp2;
	}
}

void iMatrix2::add_assign(const Interval & I, const int i, const int j)
{
	Interval J(center[i][j], radius[i][j]);
	J += I;

	Real c, r;
	J.toCenterForm(c, r);

	center[i][j] = c;
	radius[i][j] = r;
}

iMatrix2 & iMatrix2::operator += (const iMatrix2 & A)
{
	rMatrix rm_up, rm_lo;

	center.add_RNDD(rm_lo, A.center);
	center.add_RNDU(rm_up, A.center);

	iMatrix2 im2_center;
	to_iMatrix2(im2_center, rm_lo, rm_up);

	center = im2_center.center;

	radius.add_assign_RNDU(A.radius);
	radius.add_assign_RNDU(im2_center.radius);

	return *this;
}

iMatrix2 & iMatrix2::operator += (const iMatrix & A)
{
	iMatrix2 A2;
	A.to_iMatrix2(A2);

	*this += A2;
	return *this;
}

iMatrix2 & iMatrix2::operator += (const Real & rad)
{
	int size_total = radius.size1 * radius.size2;

	for(int i=0; i<size_total; ++i)
	{
		radius.data[i].add_assign_RNDU(rad);
	}

	return *this;
}

iMatrix2 & iMatrix2::operator *= (const iMatrix2 & A)
{
	rMatrix rm_lo, rm_up;

	center.mul_RNDD(rm_lo, A.center);
	center.mul_RNDU(rm_up, A.center);

	iMatrix2 im2_center;
	to_iMatrix2(im2_center, rm_lo, rm_up);

	rMatrix rm_abs1, remainder1;
	center.abs(rm_abs1);
	rm_abs1.mul_RNDU(remainder1, A.radius);

	rMatrix rm_abs2, remainder2;
	A.center.abs(rm_abs2);
	radius.mul_RNDU(remainder2, rm_abs2);

	rMatrix remainder;
	radius.mul_RNDU(remainder, A.radius);

	remainder.add_assign_RNDU(remainder1);
	remainder.add_assign_RNDU(remainder2);
	remainder.add_assign_RNDU(im2_center.radius);

	radius = remainder;
	center = im2_center.center;

	return *this;
}

iMatrix2 & iMatrix2::operator *= (const Interval & I)
{
	Real I_c, I_r;
	I.toCenterForm(I_c, I_r);

	int size_total = center.size1 * center.size2;

	for(int i=0; i<size_total; ++i)
	{
		Real result_c_up, result_c_lo, abs_c, abs_I_c;
		center.data[i].mul_RNDD(result_c_lo, I_c);
		center.data[i].mul_RNDU(result_c_up, I_c);

		Interval tmp(result_c_lo, result_c_up, 0);
		Real result_c, result_r;
		tmp.toCenterForm(result_c, result_r);

		center.data[i].abs(abs_c);
		I_c.abs(abs_I_c);

		Real r1, r2, r3;
		abs_c.mul_RNDU(r1, I_r);
		radius.data[i].mul_RNDU(r2, abs_I_c);
		radius.data[i].mul_RNDU(r3, I_r);

		result_r.add_assign_RNDU(r1);
		result_r.add_assign_RNDU(r2);
		result_r.add_assign_RNDU(r3);

		center.data[i] = result_c;
		radius.data[i] = result_r;
	}

	return *this;
}

iMatrix2 iMatrix2::operator + (const iMatrix2 & A) const
{
	iMatrix2 result = *this;

	result += A;

	return result;
}

iMatrix2 iMatrix2::operator * (const iMatrix2 & A) const
{
	iMatrix2 result;

	rMatrix rm_lo, rm_up;

	center.mul_RNDD(rm_lo, A.center);
	center.mul_RNDU(rm_up, A.center);

	iMatrix2 im2_center;
	to_iMatrix2(im2_center, rm_lo, rm_up);

	rMatrix rm_abs1, remainder1;
	center.abs(rm_abs1);
	rm_abs1.mul_RNDU(remainder1, A.radius);

	rMatrix rm_abs2, remainder2;
	A.center.abs(rm_abs2);
	radius.mul_RNDU(remainder2, rm_abs2);

	rMatrix remainder;
	radius.mul_RNDU(remainder, A.radius);

	remainder.add_assign_RNDU(remainder1);
	remainder.add_assign_RNDU(remainder2);
	remainder.add_assign_RNDU(im2_center.radius);

	result.radius = remainder;
	result.center = im2_center.center;

	return result;
}

iMatrix iMatrix2::operator * (const iMatrix & A) const
{
	iMatrix result;
	this->to_iMatrix(result);

	result *= A;
	return result;
}

upMatrix iMatrix2::operator * (const upMatrix & upm) const
{
	int size1 = center.rows();
	int size2 = center.cols();

	if(size2 != upm.size1)
	{
		printf("Interval matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	upMatrix result(size1, upm.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<upm.size2; ++j)
		{
			UnivariatePolynomial tmp;

			for(int p=0; p<size2; ++p)
			{
				Interval I(center.data[i*size2 + p], radius.data[i*size2 + p]);
				tmp += upm.data[p*upm.size2 + j] * I;
			}

			result.data[i*upm.size2 + j] = tmp;
		}
	}

	return result;
}

TaylorModelVec iMatrix2::operator * (const TaylorModelVec & tmv) const
{
	int size1 = center.rows();
	int size2 = center.cols();

	if(size2 != tmv.tms.size())
	{
		printf("Interval matrix multiplication: Dimensions do not match.\n");
		exit(1);
	}

	TaylorModelVec result;

	for(int i=0; i<size1; ++i)
	{
		TaylorModel tmTemp1;

		for(int j=0; j<size2; ++j)
		{
			Interval I(center.data[i*size2 + j], radius.data[i*size2 + j]);
			TaylorModel tmTemp2;
			tmv.tms[j].mul(tmTemp2, I);
			tmTemp1.add_assign(tmTemp2);
		}

		result.tms.push_back(tmTemp1);
	}

	return result;
}

iMatrix2 & iMatrix2::operator = (const iMatrix2 & A)
{
	if(this == &A)
		return *this;

	center = A.center;
	radius = A.radius;

	return *this;
}

void iMatrix2::output(FILE *fp) const
{
	fprintf(fp, "center:\n");
	center.output(fp);
	fprintf(fp, "\nradius:\n");
	radius.output(fp);
	fprintf(fp, "\n");
}





// matrix for univariate polynomials

upMatrix::upMatrix()
{
	size1 = 0;
	size2 = 0;
	data = NULL;
}

upMatrix::upMatrix(const int m, const int n)
{
	size1 = m;
	size2 = n;
	data = new UnivariatePolynomial[m * n];
}

upMatrix::upMatrix(const int n)
{
	size1 = n;
	size2 = n;
	data = new UnivariatePolynomial[n * n];
}

upMatrix::upMatrix(const iMatrix & A)
{
	size1 = A.size1;
	size2 = A.size2;

	int size_total = size1 * size2;
	data = new UnivariatePolynomial[size_total];

	for(int i=0; i<size_total; ++i)
	{
		data[i] = A.data[i];
	}
}

upMatrix::upMatrix(const iMatrix2 & A)
{
	size1 = A.center.size1;
	size2 = A.center.size2;

	int size_total = size1 * size2;
	data = new UnivariatePolynomial[size_total];

	for(int i=0; i<size_total; ++i)
	{
		Interval I(A.center.data[i], A.radius.data[i]);
		data[i] = I;
	}
}

upMatrix::upMatrix(const upMatrix & upm)
{
	size1 = upm.size1;
	size2 = upm.size2;

	int size_total = size1 * size2;
	data = new UnivariatePolynomial[size_total];
	std::copy(upm.data, upm.data + size_total, data);
}

upMatrix::~upMatrix()
{
	delete [] data;
}

int upMatrix::rows() const
{
	return size1;
}

int upMatrix::cols() const
{
	return size2;
}

bool upMatrix::isZero() const
{
	bool result = true;

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		if(!data[i].isZero())
		{
			result = false;
			break;
		}
	}

	return result;
}

int upMatrix::degree() const
{
	int maxDeg = 0;

	int size_total = size1 * size2;
	for(int i=0; i<size_total; ++i)
	{
		int deg = data[i].degree();

		if(deg > maxDeg)
		{
			maxDeg = deg;
		}
	}

	return maxDeg;
}

void upMatrix::intEval(iMatrix & result, const std::vector<Interval> & val_exp_table) const
{
	delete [] result.data;
	result.size1 = size1;
	result.size2 = size2;
	int size_total = size1 * size2;
	result.data = new Interval[size_total];

	for(int i=0; i<size_total; ++i)
	{
		result.data[i] = data[i].intEval(val_exp_table);
	}
}

void upMatrix::intEval(iMatrix & result, const Interval & val) const
{
	delete [] result.data;
	result.size1 = size1;
	result.size2 = size2;
	int size_total = size1 * size2;
	result.data = new Interval[size_total];

	for(int i=0; i<size_total; ++i)
	{
		result.data[i] = data[i].intEval(val);
	}
}

void upMatrix::intEval(iMatrix2 & result, const std::vector<Interval> & val_exp_table) const
{
	int size_total = size1 * size2;

	rMatrix rm(size1, size2);
	result.center = rm;
	result.radius = rm;

	for(int i=0; i<size_total; ++i)
	{
		Real c, r;
		data[i].intEval(c, r, val_exp_table);

		result.center.data[i] = c;
		result.radius.data[i] = r;
	}
}

void upMatrix::intEval(iMatrix2 & result, const Interval & val) const
{
	int size_total = size1 * size2;

	rMatrix rm(size1, size2);
	result.center = rm;
	result.radius = rm;

	for(int i=0; i<size_total; ++i)
	{
		Real c, r;
		data[i].intEval(c, r, val);

		result.center.data[i] = c;
		result.radius.data[i] = r;
	}
}

void upMatrix::integral()
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i].integral();
	}
}

void upMatrix::times_x(const int order)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i].times_x(order);
	}
}

void upMatrix::transpose(upMatrix & result) const
{
	delete [] result.data;
	result.size1 = size2;
	result.size2 = size1;
	result.data = new UnivariatePolynomial[size2 * size1];

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<size2; ++j)
		{
			result.data[i*size2 + j] = data[j*size1 + i];
		}
	}
}

void upMatrix::times_pars(mpMatrix & result) const
{
	delete [] result.data;
	result.size1 = size1;
	result.size2 = 1;
	result.data = new Polynomial[size1];

	for(int i=0; i<size1; ++i)
	{
		Polynomial tmp1;

		for(int j=0; j<size2; ++j)
		{
			Polynomial tmp2;
			int pos = i * size2 + j;

			for(int k=0; k<data[pos].coefficients.size(); ++k)
			{
				Monomial monomial(data[pos].coefficients[k], size1 + 1);
				monomial.d = k + 1;
				monomial.degrees[0] = k;
				monomial.degrees[j+1] = 1;

				tmp2.add_assign(monomial);
			}

			tmp1 += tmp2;
		}

		result[i][0] = tmp1;
	}
}

void upMatrix::ctrunc(iMatrix & rem, const int order, const std::vector<Interval> & val_exp_table)
{
	delete [] rem.data;
	rem.size1 = size1;
	rem.size2 = size2;
	int size_total = size1 * size2;
	rem.data = new Interval[size_total];

	for(int i=0; i<size_total; ++i)
	{
		data[i].ctrunc(rem.data[i], order, val_exp_table);
	}
}

void upMatrix::ctrunc(iMatrix & rem, const int order, const Interval & val)
{
	delete [] rem.data;
	rem.size1 = size1;
	rem.size2 = size2;
	int size_total = size1 * size2;
	rem.data = new Interval[size_total];

	for(int i=0; i<size_total; ++i)
	{
		data[i].ctrunc(rem.data[i], order, val);
	}
}

void upMatrix::ctrunc(iMatrix & rem1, iMatrix & rem2, const int order, const std::vector<Interval> & val1_exp_table, const std::vector<Interval> & val2_exp_table)
{
	int size_total = size1 * size2;

	delete [] rem1.data;
	rem1.size1 = size1;
	rem1.size2 = size2;
	rem1.data = new Interval[size_total];

	delete [] rem2.data;
	rem2.size1 = size1;
	rem2.size2 = size2;
	rem2.data = new Interval[size_total];

	rem2 = rem1;

	for(int i=0; i<size_total; ++i)
	{
		data[i].ctrunc(rem1.data[i], rem2.data[i], order, val1_exp_table, val2_exp_table);
	}
}

void upMatrix::ctrunc(iMatrix & rem1, iMatrix & rem2, const int order, const Interval & val1, const Interval & val2)
{
	int size_total = size1 * size2;

	delete [] rem1.data;
	rem1.size1 = size1;
	rem1.size2 = size2;
	rem1.data = new Interval[size_total];

	delete [] rem2.data;
	rem2.size1 = size1;
	rem2.size2 = size2;
	rem2.data = new Interval[size_total];

	for(int i=0; i<size_total; ++i)
	{
		data[i].ctrunc(rem1.data[i], rem2.data[i], order, val1, val2);
	}
}

void upMatrix::ctrunc(const int order, const std::vector<Interval> & val_exp_table)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i].ctrunc(order, val_exp_table);
	}
}

void upMatrix::ctrunc(const int order, const Interval & val)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i].ctrunc(order, val);
	}
}

void upMatrix::nctrunc(const int order)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i].nctrunc(order);
	}
}

void upMatrix::substitute(upMatrix & result, const std::vector<UnivariatePolynomial> & t_exp_table) const
{
	delete [] result.data;
	result.size1 = size1;
	result.size2 = size2;
	int size_total = size1 * size2;
	result.data = new UnivariatePolynomial[size_total];

	for(int i=0; i<size_total; ++i)
	{
		data[i].substitute(result.data[i], t_exp_table);
	}
}

void upMatrix::substitute(upMatrix & result, const UnivariatePolynomial & t) const
{
	delete [] result.data;
	result.size1 = size1;
	result.size2 = size2;
	int size_total = size1 * size2;
	result.data = new UnivariatePolynomial[size_total];

	for(int i=0; i<size_total; ++i)
	{
		data[i].substitute(result.data[i], t);
	}
}

void upMatrix::output(FILE *fp) const
{
	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<size2; ++j)
		{
			data[i*size2 + j].output(fp);
			fprintf(fp, "\t\t");
		}

		fprintf(fp, "\n");
	}
}

void upMatrix::decompose(upMatrix & positive, upMatrix & negative, iMatrix2 & im2_rem) const
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		UnivariatePolynomial pos, neg;
		Interval rem;

		data[i].decompose(pos, neg, rem);

		positive.data[i] = pos;
		negative.data[i] = neg;

		Real c, r;
		rem.toCenterForm(c, r);
		im2_rem.center.data[i] = c;
		im2_rem.radius.data[i] = r;
	}
}

upMatrix & upMatrix::operator += (const upMatrix & upm)
{
	if(size1 != upm.size1 || size2 != upm.size2)
	{
		printf("Univariate polynomial matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] += upm.data[i];
	}

	return *this;
}

upMatrix & upMatrix::operator += (const iMatrix & A)
{
	if(size1 != A.size1 || size2 != A.size2)
	{
		printf("Univariate polynomial matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] += A.data[i];
	}

	return *this;
}

upMatrix & upMatrix::operator += (const Real & rad)
{
	int size_total = size1 * size2;
	Interval I;
	rad.to_sym_int(I);

	for(int i=0; i<size_total; ++i)
	{
		data[i] += I;
	}

	return *this;
}

upMatrix & upMatrix::operator -= (const upMatrix & upm)
{
	if(size1 != upm.size1 || size2 != upm.size2)
	{
		printf("Univariate polynomial matrix subtraction: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] -= upm.data[i];
	}

	return *this;
}

upMatrix & upMatrix::operator -= (const iMatrix & A)
{
	if(size1 != A.size1 || size2 != A.size2)
	{
		printf("Univariate polynomial matrix subtraction: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] -= A.data[i];
	}

	return *this;
}

upMatrix & upMatrix::operator *= (const upMatrix & upm)
{
	if(size2 != upm.size1)
	{
		printf("Univariate polynomial multiplication: Dimensions do not match.\n");
		exit(1);
	}

	upMatrix result(size1, upm.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<upm.size2; ++j)
		{
			UnivariatePolynomial tmp;

			for(int p=0; p<size2; ++p)
			{
				tmp += data[i*size2 + p] * upm.data[p*upm.size2 + j];
			}

			result.data[i*upm.size2 + j] = tmp;
		}
	}

	*this = result;
	return *this;
}

upMatrix & upMatrix::operator *= (const iMatrix & A)
{
	if(size2 != A.size1)
	{
		printf("Univariate polynomial multiplication: Dimensions do not match.\n");
		exit(1);
	}

	upMatrix result(size1, A.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<A.size2; ++j)
		{
			UnivariatePolynomial tmp;

			for(int p=0; p<size2; ++p)
			{
				tmp += data[i*size2 + p] * A.data[p*A.size2 + j];
			}

			result.data[i*A.size2 + j] = tmp;
		}
	}

	*this = result;
	return *this;
}

upMatrix & upMatrix::operator *= (const Interval & I)
{
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] *= I;
	}

	return *this;
}

upMatrix upMatrix::operator + (const upMatrix & upm) const
{
	if(size1 != upm.size1 || size2 != upm.size2)
	{
		printf("Univariate polynomial matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	upMatrix result(size1, size2);
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		result.data[i] = data[i] + upm.data[i];
	}

	return result;
}

upMatrix upMatrix::operator + (const iMatrix & A) const
{
	if(size1 != A.size1 || size2 != A.size2)
	{
		printf("Univariate polynomial matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	upMatrix result(size1, size2);
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		result.data[i] = data[i] + A.data[i];
	}

	return result;
}

upMatrix upMatrix::operator - (const upMatrix & upm) const
{
	if(size1 != upm.size1 || size2 != upm.size2)
	{
		printf("Univariate polynomial matrix subtraction: Dimensions do not match.\n");
		exit(1);
	}

	upMatrix result(size1, size2);
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		result.data[i] = data[i] - upm.data[i];
	}

	return result;
}

upMatrix upMatrix::operator - (const iMatrix & A) const
{
	if(size1 != A.size1 || size2 != A.size2)
	{
		printf("Univariate polynomial matrix subtraction: Dimensions do not match.\n");
		exit(1);
	}

	upMatrix result(size1, size2);
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		result.data[i] = data[i] - A.data[i];
	}

	return result;
}

upMatrix upMatrix::operator * (const upMatrix & upm) const
{
	if(size2 != upm.size1)
	{
		printf("Univariate polynomial multiplication: Dimensions do not match.\n");
		exit(1);
	}

	upMatrix result(size1, upm.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<upm.size2; ++j)
		{
			UnivariatePolynomial tmp;

			for(int p=0; p<size2; ++p)
			{
				tmp += data[i*size2 + p] * upm.data[p*upm.size2 + j];
			}

			result.data[i*upm.size2 + j] = tmp;
		}
	}

	return result;
}

upMatrix upMatrix::operator * (const iMatrix & A) const
{
	if(size2 != A.size1)
	{
		printf("Univariate polynomial multiplication: Dimensions do not match.\n");
		exit(1);
	}

	upMatrix result(size1, A.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<A.size2; ++j)
		{
			UnivariatePolynomial tmp;

			for(int p=0; p<size2; ++p)
			{
				tmp += data[i*size2 + p] * A.data[p*A.size2 + j];
			}

			result.data[i*A.size2 + j] = tmp;
		}
	}

	return result;
}

upMatrix upMatrix::operator * (const iMatrix2 & A) const
{
	if(size2 != A.center.size1)
	{
		printf("Univariate polynomial multiplication: Dimensions do not match.\n");
		exit(1);
	}

	int A_size2 = A.center.size2;

	upMatrix result(size1, A_size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<A_size2; ++j)
		{
			UnivariatePolynomial tmp;

			for(int p=0; p<size2; ++p)
			{
				Interval I(A.center.data[p*A_size2 + j], A.radius.data[p*A_size2 + j]);
				tmp += data[i*size2 + p] * I;
			}

			result.data[i*A_size2 + j] = tmp;
		}
	}

	return result;
}

upMatrix upMatrix::operator * (const Interval & I) const
{
	upMatrix result = *this;
	result *= I;

	return result;
}

mpMatrix upMatrix::operator * (const mpMatrix & mpm) const
{
	if(size2 != mpm.size1)
	{
		printf("Univariate polynomial multiplication: Dimensions do not match.\n");
		exit(1);
	}

	mpMatrix result(size1, mpm.size2);

	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<mpm.size2; ++j)
		{
			Polynomial tmp;

			for(int p=0; p<size2; ++p)
			{
				tmp += data[i*size2 + p] * mpm.data[p*mpm.size2 + j];
			}

			result.data[i*mpm.size2 + j] = tmp;
		}
	}

	return result;
}

UnivariatePolynomial * upMatrix::operator [] (const int i)
{
	return &data[i * size2];
}

upMatrix & upMatrix::operator = (const upMatrix & upm)
{
	if(this == &upm)
		return *this;

	size1 = upm.size1;
	size2 = upm.size2;

	int size_total = size1 * size2;
	delete [] data;

	if(size_total > 0)
	{
		data = new UnivariatePolynomial[size_total];
		std::copy(upm.data, upm.data + size_total, data);
	}
	else
	{
		data = NULL;
	}

	return *this;
}












// matrix for multivariate polynomials

mpMatrix::mpMatrix()
{
	size1 = 0;
	size2 = 0;
	data = NULL;
}

mpMatrix::mpMatrix(const int m, const int n)
{
	size1 = m;
	size2 = n;
	data = new Polynomial[size1 * size2];
}

mpMatrix::mpMatrix(const int n)
{
	size1 = n;
	size2 = n;
	data = new Polynomial[size1 * size2];
}

mpMatrix::mpMatrix(const mpMatrix & mpm)
{
	size1 = mpm.size1;
	size2 = mpm.size2;

	int size_total = size1 * size2;
	data = new Polynomial[size_total];

	std::copy(mpm.data, mpm.data + size_total, data);
}

mpMatrix::~mpMatrix()
{
	delete [] data;
}

int mpMatrix::rows() const
{
	return size1;
}

int mpMatrix::cols() const
{
	return size2;
}

void mpMatrix::intEval(mpMatrix & result, const std::vector<Interval> & val_exp_table) const
{
	delete [] result.data;
	result.size1 = size1;
	result.size2 = size2;

	int size_total = size1 * size2;
	result.data = new Polynomial[size_total];

	for(int i=0; i<size_total; ++i)
	{
		data[i].evaluate_t(result.data[i], val_exp_table);
	}
}

void mpMatrix::intEval(iMatrix & result, const std::vector<Interval> & domain) const
{
	delete [] result.data;
	result.size1 = size1;
	result.size2 = size2;

	int size_total = size1 * size2;
	result.data = new Interval[size_total];

	for(int i=0; i<size_total; ++i)
	{
		Interval tmp;
		data[i].intEval(tmp, domain);
		result.data[i] = tmp;
	}
}

void mpMatrix::output(FILE *fp, const std::vector<std::string> & varNames) const
{
	for(int i=0; i<size1; ++i)
	{
		for(int j=0; j<size2; ++j)
		{
			data[i*size2 + j].dump_interval(fp, varNames);
			fprintf(fp, "\t\t");
		}

		fprintf(fp, "\n");
	}
}

mpMatrix & mpMatrix::operator += (const mpMatrix & mpm)
{
	if(size1 != mpm.size1 || size2 != mpm.size2)
	{
		printf("Univariate polynomial matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		data[i] += mpm.data[i];
	}

	return *this;
}

mpMatrix mpMatrix::operator + (const mpMatrix & mpm) const
{
	if(size1 != mpm.size1 || size2 != mpm.size2)
	{
		printf("Univariate polynomial matrix addition: Dimensions do not match.\n");
		exit(1);
	}

	mpMatrix result(size1, size2);
	int size_total = size1 * size2;

	for(int i=0; i<size_total; ++i)
	{
		result.data[i] = data[i] + mpm.data[i];
	}

	return result;
}

Polynomial * mpMatrix::operator [] (const int i)
{
	return &data[i * size2];
}

mpMatrix & mpMatrix::operator = (const mpMatrix & mpm)
{
	if(this == &mpm)
		return *this;

	size1 = mpm.size1;
	size2 = mpm.size2;

	int size_total = size1 * size2;
	delete [] data;

	if(size_total > 0)
	{
		data = new Polynomial[size_total];
		std::copy(mpm.data, mpm.data + size_total, data);
	}
	else
	{
		data = NULL;
	}

	return *this;
}





MatrixParseSetting::MatrixParseSetting()
{
}

MatrixParseSetting::MatrixParseSetting(const MatrixParseSetting & setting)
{
	strExpression = setting.strExpression;
	result = setting.result;
}

MatrixParseSetting::~MatrixParseSetting()
{
}

MatrixParseSetting & MatrixParseSetting::operator = (const MatrixParseSetting & setting)
{
	if(this == &setting)
		return *this;

	strExpression = setting.strExpression;
	result = setting.result;

	return *this;
}





namespace flowstar
{

void to_iMatrix2(iMatrix2 & result, const rMatrix & lo, const rMatrix & up)
{
	int size_total = lo.size1 * lo.size2;

	rMatrix rm(lo.size1, lo.size2);
	result.center = rm;
	result.radius = rm;

	for(int i=0; i<size_total; ++i)
	{
		Interval I(lo.data[i], up.data[i], 0);
		Real c, r;

		I.toCenterForm(c, r);
		result.center.data[i] = c;
		result.radius.data[i] = r;
	}
}

}

