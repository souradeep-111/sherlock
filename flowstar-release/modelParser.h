/*---
  Flow*: A Verification Tool for Cyber-Physical Systems.
  Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
  Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.
  
  The code is released as is under the GNU General Public License (GPL).
---*/

#ifndef MODELPARSER_H_
#define MODELPARSER_H_

#include "Hybrid.h"

extern int lineNum;

extern int yyparse();

void parseError(const char *str, int lnum);


class LTI_Term
{
public:
	flowstar::Interval coefficient;
	int stateVar_id;
	int ti_par_id;
	int tv_par_id;

public:
	LTI_Term()
	{
		stateVar_id = -1;
		ti_par_id = -1;
		tv_par_id = -1;
	}

	LTI_Term(const double l, const double u, const int id1, const int id2, const int id3)
	{
		flowstar::Interval I(l,u);
		coefficient = I;
		stateVar_id = id1;
		ti_par_id = id2;
		tv_par_id = id3;
	}

	~LTI_Term()
	{
	}

	LTI_Term & operator = (const LTI_Term & term)
	{
		if(this == &term)
			return *this;

		coefficient = term.coefficient;
		stateVar_id = term.stateVar_id;
		ti_par_id = term.ti_par_id;
		tv_par_id = term.tv_par_id;

		return *this;
	}
};


class LTV_Term
{
public:
	flowstar::UnivariatePolynomial coefficient;
	int stateVar_id;
	int ti_par_id;
	int tv_par_id;

public:
	LTV_Term()
	{
		stateVar_id = -1;
		ti_par_id = -1;
		tv_par_id = -1;
	}

	LTV_Term(const flowstar::UnivariatePolynomial & p, const int id1, const int id2, const int id3)
	{
		coefficient = p;
		stateVar_id = id1;
		ti_par_id = id2;
		tv_par_id = id3;
	}

	~LTV_Term()
	{
	}

	LTV_Term & operator = (const LTV_Term & term)
	{
		if(this == &term)
			return *this;

		coefficient = term.coefficient;
		stateVar_id = term.stateVar_id;
		ti_par_id = term.ti_par_id;
		tv_par_id = term.tv_par_id;

		return *this;
	}
};


class ODE_String
{
public:
	std::string ode;
	flowstar::Interval constant;

public:
	ODE_String()
	{
	}

	ODE_String(const std::string & ode_input, const flowstar::Interval & I)
	{
		ode = ode_input;
		constant = I;
	}

	~ODE_String()
	{
	}

	ODE_String & operator = (const ODE_String & ode_string)
	{
		if(this == &ode_string)
			return *this;

		ode = ode_string.ode;
		constant = ode_string.constant;

		return *this;
	}
};


class VarList
{
public:
	std::map<std::string,int> varTab;
	std::vector<std::string> varNames;
public:
	VarList()
	{
	}

	~VarList()
	{
		varTab.clear();
		varNames.clear();
	}

	VarList & operator = (const VarList & varlist)
	{
		if(this == &varlist)
			return *this;

		varTab = varlist.varTab;
		varNames = varlist.varNames;

		return *this;
	}

	bool declareVar(const std::string & vName)
	{
		std::map<std::string,int>::const_iterator iter;

		if((iter = varTab.find(vName)) == varTab.end())
		{
			varTab[vName] = varNames.size();
			varNames.push_back(vName);
			return true;
		}
		else
		{
			return false;
		}
	}

	int getIDForVar(const std::string & vName)
	{
		std::map<std::string,int>::const_iterator iter;
		if((iter = varTab.find(vName)) == varTab.end())
		{
			return -1;
		}

		return iter->second;
	}

	void clear()
	{
		varTab.clear();
		varNames.clear();
	}
};


#endif /* MODELPARSER_H_ */
