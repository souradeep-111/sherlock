/* A Bison parser, made by GNU Bison 3.0.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_MODELPARSER_TAB_H_INCLUDED
# define YY_YY_MODELPARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NUM = 258,
    IDENT = 259,
    STATEVAR = 260,
    TMVAR = 261,
    TM = 262,
    EQ = 263,
    GEQ = 264,
    LEQ = 265,
    ASSIGN = 266,
    END = 267,
    MODE = 268,
    INIT = 269,
    BELONGSTO = 270,
    POLYODE1 = 271,
    POLYODE2 = 272,
    POLYODE3 = 273,
    VISUALIZE = 274,
    PARAAGGREG = 275,
    INTAGGREG = 276,
    TMAGGREG = 277,
    OUTPUT = 278,
    NOOUTPUT = 279,
    CONTINUOUS = 280,
    HYBRID = 281,
    SETTING = 282,
    FIXEDST = 283,
    FIXEDORD = 284,
    ADAPTIVEST = 285,
    ADAPTIVEORD = 286,
    ORDER = 287,
    MIN = 288,
    MAX = 289,
    REMEST = 290,
    INTERVAL = 291,
    OCTAGON = 292,
    GRID = 293,
    PLOT = 294,
    QRPRECOND = 295,
    IDPRECOND = 296,
    TIME = 297,
    MODES = 298,
    JUMPS = 299,
    INV = 300,
    GUARD = 301,
    RESET = 302,
    START = 303,
    MAXJMPS = 304,
    PRINTON = 305,
    PRINTOFF = 306,
    UNSAFESET = 307,
    CONTINUOUSFLOW = 308,
    HYBRIDFLOW = 309,
    TAYLOR_PICARD = 310,
    TAYLOR_REMAINDER = 311,
    TAYLOR_POLYNOMIAL = 312,
    NONPOLY_CENTER = 313,
    EXP = 314,
    SIN = 315,
    COS = 316,
    LOG = 317,
    SQRT = 318,
    NPODE_TAYLOR = 319,
    CUTOFF = 320,
    PRECISION = 321,
    GNUPLOT = 322,
    MATLAB = 323,
    COMPUTATIONPATHS = 324,
    LTIODE = 325,
    LTV_ODE = 326,
    PAR = 327,
    UNC = 328,
    UNIVARIATE_POLY = 329,
    MULTIVARIATE_POLY = 330,
    TIME_INV = 331,
    TIME_VAR = 332,
    STEP = 333,
    TRUE = 334,
    FALSE = 335,
    LINEARCONTINUOUSFLOW = 336,
    EXPRESSION = 337,
    MATRIX = 338,
    uminus = 339
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 61 "modelParser.y" /* yacc.c:1909  */

	double dblVal;
	int intVal;
	std::string *identifier;
	std::vector<flowstar::Interval> *intVec;
	std::vector<int> *iVec;
	std::vector<double> *dVec;
	std::vector<flowstar::Monomial> *monoVec;
	std::vector<flowstar::Polynomial> *polyVec;
	flowstar::Monomial *mono;
	flowstar::Polynomial *poly;
	flowstar::TaylorModelVec *tmVec;
	flowstar::Matrix *mat;
	std::vector<std::vector<double> > *dVecVec;
	std::vector<flowstar::PolynomialConstraint> *vecConstraints;
	flowstar::ResetMap *resetMap;
	flowstar::Flowpipe *pFlowpipe;
	std::vector<flowstar::Flowpipe> *pVecFlowpipe;
	flowstar::TaylorModel *ptm;
	flowstar::Interval *pint;
	std::vector<std::string> *strVec;
	flowstar::TreeNode *pNode;
	flowstar::UnivariatePolynomial *pUpoly;
	LTI_Term *p_LTI_Term;
	LTV_Term *p_LTV_Term;
	ODE_String *p_ODE_String;
	flowstar::Expression_AST *pExpression;
	std::vector<std::vector<flowstar::Interval> > *piMatrix;

#line 169 "modelParser.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_MODELPARSER_TAB_H_INCLUDED  */
