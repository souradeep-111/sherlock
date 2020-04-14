/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.

  The code is released as is under the GNU General Public License (GPL).
---*/

#include "Monomial.h"

using namespace flowstar;

Monomial::Monomial()
{
	d = 0;
}

Monomial::Monomial(const Interval & I, const std::vector<int> & degs):coefficient(I), degrees(degs), d(0)
{
	for(int i=0; i<degs.size(); ++i)
	{
		d += degs[i];
	}
}

Monomial::Monomial(const Monomial & monomial): coefficient(monomial.coefficient), degrees(monomial.degrees), d(monomial.d)
{
}

Monomial::Monomial(const Interval & I, const int numVars):d(0)
{
	for(int i=0; i<numVars; ++i)
	{
		degrees.push_back(0);
	}

	coefficient = I;
}

Monomial::~Monomial()
{
	degrees.clear();
}

int Monomial::degree() const
{
	return d;
}

int Monomial::dimension() const
{
	return degrees.size();
}

void Monomial::intEval(Interval & result, const std::vector<Interval> & domain) const
{
	result = coefficient;

	for(int i=0; i<degrees.size(); ++i)
	{
		Interval tmpI(1,1);
		for(int j=0; j<degrees[i]; ++j)
		{
			tmpI *= domain[i];
		}

		result *= tmpI;
	}
}

void Monomial::intEvalNormal(Interval & result, const std::vector<Interval> & step_exp_table) const
{
	Interval intZero;
	result = intZero;

	if(degrees.size() == 0)
		return;

	result = coefficient;
	result *= step_exp_table[degrees[0]];

	Interval evenInt(0,1), oddInt(-1,1);
	Interval intFactor(1);
	bool bSet = false;

	for(int i=1; i<degrees.size(); ++i)
	{
		if(degrees[i] == 0)			// degree is zero
		{
			continue;
		}
		else if(degrees[i]%2 == 0)	// degree is an even number
		{
			if(!bSet)
			{
				intFactor = evenInt;
				bSet = true;
			}
		}
		else						// degree is an odd number
		{
			intFactor = oddInt;
			break;
		}
	}

	result *= intFactor;
}

void Monomial::inv(Monomial & result) const
{
	result = *this;
	coefficient.inv(result.coefficient);
}

Monomial & Monomial::operator = (const Monomial & monomial)
{
	if(this == &monomial)
		return *this;

	coefficient = monomial.coefficient;
	degrees = monomial.degrees;
	d = monomial.d;

	return *this;
}

Monomial & Monomial::operator += (const Monomial & monomial)
{
	coefficient += monomial.coefficient;
	return *this;
}

Monomial & Monomial::operator *= (const Monomial & monomial)
{
	coefficient *= monomial.coefficient;

	for(int i=0; i<degrees.size(); ++i)
	{
		degrees[i] += monomial.degrees[i];
	}

	d += monomial.d;
	return *this;
}

const Monomial Monomial::operator + (const Monomial & monomial) const
{
	Monomial result = *this;
	result += monomial;
	return result;
}

const Monomial Monomial::operator * (const Monomial & monomial) const
{
	Monomial result = *this;
	result *= monomial;
	return result;
}

bool Monomial::isLinear(int & index) const
{
	if(d == 1)
	{
		for(int i=0; i<degrees.size(); ++i)
		{
			if(degrees[i] == 1)
			{
				index = i;
				return true;
			}
		}
	}

	return false;
}

void Monomial::dump_interval(FILE *fp, const std::vector<std::string> & varNames) const
{
	coefficient.dump(fp);

	for(int i=0; i<degrees.size()-1; i++)
	{
		if(degrees[i] != 0)
		{
			if(degrees[i] == 1)
				fprintf(fp, " * %s", varNames[i].c_str());
			else
				fprintf(fp, " * %s^%d", varNames[i].c_str(), degrees[i]);
		}
	}

	if(degrees[degrees.size()-1] != 0)
	{
		if(degrees[degrees.size()-1] == 1)
			fprintf(fp, " * %s", varNames[degrees.size()-1].c_str());
		else
			fprintf(fp, " * %s^%d", varNames[degrees.size()-1].c_str(), degrees[degrees.size()-1]);
	}
}

void Monomial::dump_constant(FILE *fp, const std::vector<std::string> & varNames) const
{
	double c = coefficient.sup();
	fprintf(fp, "(%lf)", c);

	for(int i=0; i<degrees.size()-1; i++)
	{
		if(degrees[i] != 0)
		{
			if(degrees[i] == 1)
				fprintf(fp, " * %s", varNames[i].c_str());
			else
				fprintf(fp, " * %s^%d", varNames[i].c_str(), degrees[i]);
		}
	}

	if(degrees[degrees.size()-1] != 0)
	{
		if(degrees[degrees.size()-1] == 1)
			fprintf(fp, " * %s", varNames[degrees.size()-1].c_str());
		else
			fprintf(fp, " * %s^%d", varNames[degrees.size()-1].c_str(), degrees[degrees.size()-1]);
	}
}

void Monomial::toString(std::string & result, const std::vector<std::string> & varNames) const
{
	std::string strMono;

	strMono += '(';

	std::string strInt;
	coefficient.toString(strInt);
	strMono += strInt;

	for(int i=0; i<degrees.size(); i++)
	{
		if(degrees[i] != 0)
		{
			if(degrees[i] == 1)
			{
				strMono += ' ';
				strMono += '*';
				strMono += ' ';
				strMono += varNames[i];
			}
			else
			{
				strMono += ' ';
				strMono += '*';
				strMono += ' ';
				strMono += varNames[i];
				strMono += '^';

				char strNum[NUM_LENGTH];
				sprintf(strNum, "%d", degrees[i]);
				std::string num(strNum);
				strMono += num;
			}
		}
	}

	strMono += ')';

	result = strMono;
}

bool Monomial::classInvariantOK() const
{
	int sum = 0;

	for(int i = 0; i<degrees.size(); ++i)
		sum += degrees[i];

	return (sum == d);
}

int Monomial::cutoff(Monomial & monoRem, const Interval & cutoff_threshold)
{
	Interval M;
	coefficient.midpoint(M);

	if(M.subseteq(cutoff_threshold))
	{
		return 2;
	}
	else if(coefficient.width() >= MAX_WIDTH)
	{
		monoRem = *this;
		monoRem.coefficient.remove_midpoint();
		coefficient = M;
		return 1;
	}
	else
	{
		return 0;
	}
}

int Monomial::cutoff(const Interval & cutoff_threshold)
{
	Interval M;
	coefficient.midpoint(M);

	if(M.subseteq(cutoff_threshold))
	{
		return 2;
	}
	else if(coefficient.width() >= MAX_WIDTH)
	{
		coefficient = M;
		return 1;
	}
	else
	{
		return 0;
	}
}

bool Monomial::center()
{
	Interval M, intZero;

	coefficient.midpoint(M);
	if(M.subseteq(intZero))
	{
		return false;
	}
	else
	{
		coefficient = M;
		return true;
	}
}

bool Monomial::operator == (const Monomial & b) const
{
	if (d == b.d)
	{
		for(int i=0; i<degrees.size(); i++)
		{
			if(degrees[i] != b.degrees[i])
				return false;
		}

		return true;	// The two monomials are identical without considering the coefficients.
	}
	else
		return false;
}

bool Monomial::operator < (const Monomial & b) const
{
	if(d < b.d)
	{
		return true;
	}
	else if(d > b.d)
	{
		return false;
	}
	else	// a.d == b.d
	{
		for(int i=0; i<degrees.size(); ++i)
		{
			if(degrees[i] < b.degrees[i])
				return true;
			else if(degrees[i] > b.degrees[i])
				return false;
		}
	}

	return false;	// a == b
}

void Monomial::substitute(const int varID, const Interval & intVal)
{
	int deg = degrees[varID];

	if(deg > 0)
	{
		d -= deg;
		degrees[varID] = 0;

		Interval pow = intVal.pow(deg);
		coefficient *= pow;
	}
}

void Monomial::substitute(const std::vector<int> & varIDs, const std::vector<Interval> & intVals)
{
	for(int i=0; i<varIDs.size(); ++i)
	{
		int deg = degrees[varIDs[i]];

		if(deg > 0)
		{
			d -= deg;
			degrees[varIDs[i]] = 0;

			Interval pow = intVals[i].pow(deg);
			coefficient *= pow;
		}
	}
}

bool Monomial::substitute_with_precond(const std::vector<bool> & substitution)
{
	int multi = 0;
	bool breformulate = false;

	for(int i=1; i<substitution.size(); ++i)
	{
		if(substitution[i])
		{
			int deg = degrees[i];

			if(deg > 1)
			{
				return 2;
			}

			if(deg >= 1)
			{
				if(multi <= 1)
				{
					++multi;
				}
				else
				{
					return true;
				}
			}
		}
	}

	return false;
}

void Monomial::substitute(Monomial & result, const int varID, const Interval & intVal) const
{
	result = *this;
	result.substitute(varID, intVal);
}

void Monomial::substitute(Monomial & result, const std::vector<int> & varIDs, const std::vector<Interval> & intVals) const
{
	result = *this;
	result.substitute(varIDs, intVals);
}

void Monomial::extend(const int num)
{
	for(int i=0; i<num; ++i)
	{
		degrees.push_back(0);
	}
}

