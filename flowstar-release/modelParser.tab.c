/* A Bison parser, made by GNU Bison 3.0.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "modelParser.y" /* yacc.c:339  */

	/*---
	Flow*: A Verification Tool for Cyber-Physical Systems.
	Authors: Xin Chen, Sriram Sankaranarayanan, and Erika Abraham.
	Email: Xin Chen <chenxin415@gmail.com> if you have questions or comments.

	The code is released as is under the GNU General Public License (GPL).
	---*/


	#include "modelParser.h"

	extern int yyerror(const char *);
	extern int yyerror(std::string);
	extern int yylex();
	extern int yyparse();
	bool err;

	int lineNum = 1;

	flowstar::ContinuousReachability continuousProblem;
	flowstar::HybridReachability hybridProblem;
	flowstar::ReachabilitySetting mode_local_setting;

	flowstar::iMatrix dyn_A;
	flowstar::iMatrix dyn_A_row;

	flowstar::iMatrix dyn_B;
	flowstar::iMatrix dyn_B_row;

	flowstar::iMatrix dyn_ti;
	flowstar::iMatrix dyn_ti_row;

	flowstar::iMatrix dyn_tv;
	flowstar::iMatrix dyn_tv_row;

	flowstar::upMatrix dyn_A_t;
	flowstar::upMatrix dyn_A_t_row;

	flowstar::upMatrix dyn_B_t;
	flowstar::upMatrix dyn_B_t_row;

	flowstar::upMatrix dyn_ti_t;
	flowstar::upMatrix dyn_ti_t_row;

	flowstar::upMatrix dyn_tv_t;
	flowstar::upMatrix dyn_tv_t_row;

	VarList varlist;

	std::vector<flowstar::Flowpipe> initialSets;

	void parseError(const char *str, int lnum)
	{
		std::cerr << "Error @line " << lineNum << ":" << std::string(str) << std::endl;
		exit(1);
	}

#line 125 "modelParser.tab.c" /* yacc.c:339  */

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "modelParser.tab.h".  */
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
#line 61 "modelParser.y" /* yacc.c:355  */

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

#line 280 "modelParser.tab.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_MODELPARSER_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 295 "modelParser.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  26
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1851

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  101
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  84
/* YYNRULES -- Number of rules.  */
#define YYNRULES  372
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1378

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   339

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    99,
      93,    97,    86,    84,    94,    85,     2,    87,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   100,    92,
       2,     2,    98,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    95,     2,    96,    89,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    90,     2,    91,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    88
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   173,   173,   274,   400,   501,   629,   671,   690,   710,
     753,   787,   795,   802,   808,   814,   821,   827,   833,   839,
     870,   880,   890,   900,   909,   914,   919,   924,   929,   935,
     941,   946,   965,   986,   999,  1013,  1026,  1042,  1068,  1095,
    1120,  1146,  1150,  1155,  1190,  1224,  1230,  1237,  1244,  1249,
    1265,  1275,  1291,  1310,  1332,  1351,  1382,  1416,  1453,  1504,
    1747,  1769,  1791,  1813,  1835,  1855,  1875,  1899,  1923,  1945,
    1967,  1989,  2011,  2031,  2051,  2075,  2100,  2111,  2123,  2158,
    2177,  2187,  2197,  2207,  2217,  2227,  2237,  2247,  2257,  2267,
    2277,  2287,  2300,  2304,  2308,  2320,  2336,  2341,  2346,  2351,
    2367,  2379,  2399,  2424,  2451,  2492,  2496,  2527,  2551,  2556,
    2583,  2604,  2620,  2626,  2631,  2672,  2689,  2709,  2714,  2728,
    2745,  2750,  2775,  2802,  2833,  2864,  2869,  2902,  2935,  2939,
    2984,  3038,  3096,  3169,  3222,  3289,  3306,  3323,  3347,  3374,
    3391,  3415,  3421,  3428,  3478,  3528,  3580,  3630,  3680,  3733,
    3741,  3749,  3758,  3768,  3780,  3785,  3819,  3859,  3864,  3877,
    3897,  3924,  3935,  3985,  4044,  4069,  4087,  4109,  4123,  4143,
    4158,  4162,  4166,  4170,  4176,  4213,  4234,  4271,  4287,  4292,
    4297,  4302,  4307,  4312,  4319,  4356,  4377,  4402,  4429,  4437,
    4445,  4450,  4458,  4476,  4483,  4511,  4529,  4537,  4545,  4550,
    4558,  4576,  4583,  4625,  4632,  4642,  4650,  4658,  4663,  4671,
    4689,  4696,  4738,  4749,  4757,  4765,  4770,  4778,  4796,  4803,
    4845,  4857,  4884,  4929,  4937,  4945,  4950,  4958,  4976,  4983,
    5041,  5057,  5093,  5100,  5107,  5123,  5128,  5147,  5156,  5165,
    5174,  5183,  5192,  5239,  5246,  5269,  5303,  5310,  5317,  5333,
    5338,  5357,  5366,  5375,  5384,  5393,  5402,  5458,  5464,  5485,
    5512,  5520,  5528,  5538,  5543,  5556,  5565,  5574,  5583,  5592,
    5601,  5611,  5617,  5635,  5663,  5674,  5685,  5696,  5707,  5718,
    5729,  5740,  5751,  5762,  5773,  5785,  5795,  5823,  5842,  5876,
    5893,  5910,  5927,  5938,  5955,  5971,  5987,  6003,  6019,  6035,
    6052,  6067,  6086,  6105,  6244,  6264,  6284,  6304,  6326,  6377,
    6428,  6479,  6484,  6500,  6512,  6524,  6536,  6550,  6583,  6616,
    6649,  6654,  6684,  6704,  6724,  6744,  6767,  6817,  6867,  6879,
    6891,  6903,  6915,  6931,  6963,  6995,  7010,  7018,  7026,  7031,
    7039,  7056,  7063,  7079,  7085,  7097,  7105,  7113,  7118,  7126,
    7144,  7151,  7180,  7187,  7207,  7215,  7223,  7228,  7236,  7242,
    7248,  7255,  7261,  7267,  7275,  7281,  7287,  7293,  7299,  7312,
    7319,  7327,  7333
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUM", "IDENT", "STATEVAR", "TMVAR",
  "TM", "EQ", "GEQ", "LEQ", "ASSIGN", "END", "MODE", "INIT", "BELONGSTO",
  "POLYODE1", "POLYODE2", "POLYODE3", "VISUALIZE", "PARAAGGREG",
  "INTAGGREG", "TMAGGREG", "OUTPUT", "NOOUTPUT", "CONTINUOUS", "HYBRID",
  "SETTING", "FIXEDST", "FIXEDORD", "ADAPTIVEST", "ADAPTIVEORD", "ORDER",
  "MIN", "MAX", "REMEST", "INTERVAL", "OCTAGON", "GRID", "PLOT",
  "QRPRECOND", "IDPRECOND", "TIME", "MODES", "JUMPS", "INV", "GUARD",
  "RESET", "START", "MAXJMPS", "PRINTON", "PRINTOFF", "UNSAFESET",
  "CONTINUOUSFLOW", "HYBRIDFLOW", "TAYLOR_PICARD", "TAYLOR_REMAINDER",
  "TAYLOR_POLYNOMIAL", "NONPOLY_CENTER", "EXP", "SIN", "COS", "LOG",
  "SQRT", "NPODE_TAYLOR", "CUTOFF", "PRECISION", "GNUPLOT", "MATLAB",
  "COMPUTATIONPATHS", "LTIODE", "LTV_ODE", "PAR", "UNC", "UNIVARIATE_POLY",
  "MULTIVARIATE_POLY", "TIME_INV", "TIME_VAR", "STEP", "TRUE", "FALSE",
  "LINEARCONTINUOUSFLOW", "EXPRESSION", "MATRIX", "'+'", "'-'", "'*'",
  "'/'", "uminus", "'^'", "'{'", "'}'", "';'", "'('", "','", "'['", "']'",
  "')'", "'>'", "'\\''", "':'", "$accept", "model", "output_env",
  "unsafe_continuous_env", "unsafe_hybrid_env", "range_contracted",
  "init_flowpipes", "linear_flowpipes", "continuous_flowpipes",
  "initial_flowpipes", "linear_continuous_flowpipes", "modeDecls",
  "hybrid_flowpipes", "computation_paths", "computation_path", "print",
  "unsafe_continuous", "unsafe_hybrid", "hybrid_constraints",
  "polynomial_constraints", "continuous", "hybrid", "hybrid_init", "modes",
  "local_setting", "parameters", "jumps", "reset", "real_valued_vectors",
  "real_valued_vector", "vector_components", "stateVarDecls",
  "stateIdDeclList", "parDecls", "parDeclList", "TIParDeclList",
  "TVParDeclList", "settings", "remainder_estimation", "remainders",
  "orders", "precondition", "plotting", "init", "set_of_intervals",
  "tmVarDecls", "tmIdDeclList", "tmVarDecls2", "tmIdDeclList2",
  "taylor_model", "taylor_model_domain", "intervals", "ode", "npode",
  "lti_env", "lti_ode", "lti_ode_hybrid", "ltv_env", "ltv_ode",
  "ltv_ode_hybrid", "polynomial", "ODEpolynomial", "constraint_polynomial",
  "reset_polynomial", "interval_taylor_model", "interval_polynomial",
  "non_polynomial_rhs_picard", "non_polynomial_rhs_remainder",
  "non_polynomial_rhs_no_remainder", "non_polynomial_rhs_string",
  "non_polynomial_rhs_center", "lti_ode_rhs", "lti_ode_rhs_term",
  "lti_ode_hybrid_rhs", "lti_ode_hybrid_rhs_term", "ltv_ode_rhs",
  "ltv_ode_rhs_term", "ltv_ode_hybrid_rhs", "ltv_ode_hybrid_rhs_term",
  "univariate_polynomial", "multivariate_polynomial", "ode_expression",
  "matrix_matlab", "vector_matlab", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,    43,    45,    42,    47,   339,    94,
     123,   125,    59,    40,    44,    91,    93,    41,    62,    39,
      58
};
# endif

#define YYPACT_NINF -1178

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-1178)))

#define YYTABLE_NINF -223

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     659,    60,   -18,    22,   132,   148,   156,   169,   201,   224,
     240,   309,   171,   226, -1178,   186,   321,   321,   280,   318,
     329,   152,    64,    93,   190,   367, -1178,   314,   266,  1150,
      26,     8,   408,   330,    33,   342,    35, -1178,   350,   358,
     465,   491,   503,   280,   280,   566,   676, -1178,   520,   525,
     580,   587,   595,   318,   318,   579,  1010, -1178,   615,   620,
     625,   638,   646,   329,   329,   645,  1018, -1178, -1178,   656,
     662,   664,   666,   687,   152,   152,   783,  1026, -1178, -1178,
      64,    64,   787,  1042, -1178, -1178,    93,    93,   798,  1051,
   -1178, -1178,   695,   710,   712,   724,   731,   190,   190,   828,
    1034, -1178,   175,   410,   514,   663,   834,   842,   848,   866,
     860,   786,   792,   885,   887, -1178,   840,   813,   821,   882,
     867,   846,   907,   280,   280,   280,   280,   280,   858,   643,
     855,   280,   280,   280,   280,   948, -1178,   318,   318,   318,
     318,   318,   873,   661,   870,   318,   318,   318,   318,   963,
   -1178,   329,   329,   329,   329,   329,   888,   667,   900,   329,
     329,   329,   329,   976, -1178,   152,   152,   152,   152,   152,
     892,   692,   902,   152,   152,   152,   152,   989, -1178,   918,
     513,   915,    64,    64,    64,  1008, -1178,   933,   949,   930,
      93,    93,    93,  1023, -1178,   190,   190,   190,   190,   190,
     952,   698,   951,   190,   190,   190,   190,  1049, -1178, -1178,
     367,  1053,  1005,   966,   977,  1063,   981,   992,  1078,  1016,
    1094,  1043,  1082,  1032, -1178, -1178,    15,  1112,  1040,  1044,
   -1178, -1178,    15,  1057,   707,   713,   722,   729,   736, -1178,
    1180,   187,   187,   858,   858, -1178,   743,   750,   757,   764,
     771, -1178,  1217,   500,   500,   873,   873, -1178,   780,   794,
     800,   809,   815, -1178,  1218,   522,   522,   888,   888, -1178,
     829,   838,   844,   853,   859, -1178,  1219,   592,   592,   892,
     892, -1178, -1178,  1220,    12,    12,   918, -1178, -1178,  1221,
     210,   210,   933, -1178,   868,   874,   883,   889,   898, -1178,
    1222,   649,   649,   952,   952, -1178,   410, -1178,  1223,  1201,
    1224,  1030,  1225,  1226,  1133,  1199, -1178,    67,   532,  1229,
    1230, -1178,  1231,  1145,   651,  1228,   104,    15, -1178,  1189,
      15, -1178, -1178, -1178, -1178, -1178,  1141, -1178, -1178, -1178,
   -1178, -1178,  1143, -1178, -1178, -1178, -1178, -1178,  1144, -1178,
   -1178, -1178, -1178, -1178,  1146,  1147,  1148, -1178, -1178, -1178,
   -1178, -1178,  1149,  1155, -1178, -1178,  1237, -1178, -1178,  1243,
    1245,   575,  1060, -1178,  1246,   583,  1185,   216,  1209,  1227,
   -1178, -1178,  1162,  1249,  1247, -1178,   651,   107,  1251,  1207,
   -1178, -1178, -1178, -1178, -1178, -1178, -1178,  1168, -1178, -1178,
    1166,   583, -1178,  1165,  1238,  1258, -1178,  1240,  1106,  1261,
   -1178, -1178,    77, -1178,    77,   585,  1262,  1263,   612, -1178,
    1264,  1177,  1179, -1178,   651,  1267,  1206,  1182,   238,  1178,
    1270, -1178,  1271,  1187,  1232,   583,  1190,   958,  1161,  1167,
    1186,  1181,    77,    77,    77,  1275,  1248,  1192,  1191,  1194,
    1197,  1198,  1200,  1203, -1178,   623, -1178,  1204,   651,  1277,
    1233,  1210,  1235,  1279, -1178, -1178,  1285,  1211,   -10, -1178,
   -1178, -1178, -1178, -1178, -1178, -1178,  1188,   340,   340,  1190,
   -1178,   151,  1153,  1260,  1289, -1178, -1178,  1293,  1119,  1121,
    1212,  1213,  1214,  1215,  1216,  1234,   265,  1254,  1208,  1236,
    1304,  1305,  1239,  1241,  1306,  1242,  1285,  1244,  1250,  1252,
    1253, -1178,  1307, -1178, -1178,   575,  1309,  1257,   214,   256,
     273,  1259,   296,  1255,  1265,  1266,  1310,  1268,   310,  1269,
    1311,  1313, -1178, -1178,  1314,  1119,  1121, -1178,  1272,  1276,
    1273,  1274,  1278,   315,  1305,  1315, -1178,  1280,  1316,  1109,
    1242,  1317,  1281,  1319,  1321,  1256,   346,   626,  1282,  1286,
    1283,  1294,  1299,  1322,  1287,  1284,  1323,  1317,  1324,  1325,
    1288,  1317,  1290,  1324,  1326,  1291,  1295,   345,   370,   380,
    1297,   394,  1298,  1300,  1327,  1302, -1178,  1303,  1285,  1308,
   -1178,   411,  1301,  1329,  1330,  1337,  1316, -1178,  1111, -1178,
     529,  1317,  1312,  1318,  1320, -1178,  1338,   413,  1328,  1333,
   -1178,  1335,  1331,  1332,  1334, -1178,  1336,  1339,   542, -1178,
     543,  1340,  1341,   581,  1324,   601,  1342,  1343,  1344,  1345,
    1347,  1350,  1346,  1352,  1353,  1355,  1296,   416,  1327,   278,
   -1178,  1242,  1285, -1178,  1348, -1178,   193,    46,    30,  1337,
   -1178,  1356,  1349,   618, -1178, -1178,  1351,  1354,  1358,  1368,
    1361,  1375,   446,   213,    23,    23,    23,   450,   202,    23,
     -44,  1357,  1377,    23,    47,   -38,   675,  1359,    23,   308,
   -1178,  1360,  1362,  1363, -1178,  1365,  1366,  1367,  1369,  1370,
    1296,  1372,   452,  1371,   332,  1113,  1242,  1373, -1178, -1178,
     193,   193,  1376,  1065,   193,  1374,  1380, -1178, -1178,  1379,
      30,  1378, -1178,  1382,  1381,  1383,  1396,  1364,   768,   456,
    1248,  1386, -1178, -1178,   213,   213,  1398,  1071, -1178,  1385,
    1384, -1178,  1399,  1388,  1389,  1390, -1178, -1178,  1292,  1391,
    1392,  1393,  1394,   202,   202,  1402,  1059,  1397,  1387, -1178,
   -1178, -1178,  1400,  1395, -1178,    81,  1403,  1122, -1178,   457,
   -1178,  1404, -1178,  1401, -1178,    54,    64,  1124, -1178,   458,
      23,    23,    23,   459,    23,    23,    23, -1178,   636,  1405,
    1406,  1407, -1178,  1408, -1178,  1120,  1409,  1411,   964,  1410,
     225,   193,   193,  1414,  1077,  1416,  1413, -1178,  1412,  1419,
    1415,  1417,  1423,  1424,  1425,  1418,  1427,  1420,  1368,  1153,
    1421,  1426,   973,  1422,   213,   213,   213,  1430,   461, -1178,
   -1178,  1431,  1428, -1178, -1178,  1429,   202,   202,   202,   202,
     202,  1432,   904,  1433,   202,   202,   202,   202,  1434, -1178,
    1324,   462,   470, -1178,  1435, -1178,  1436,    81,    81,  1438,
    1324,   473, -1178,   474, -1178, -1178,   979,    54,    54,  1447,
    1440,  1441,  1442,  1449,  1443,  1444,  1445,   586,  1439,  1448,
    1450,  1451,  1452,  1453,   673, -1178,  1454, -1178, -1178,  1437,
     193, -1178,  1462,  1463,   819,   819,  1411, -1178,   232,  1455,
    1464, -1178,  1467,  1446,  1456,  1419,  1457,  1458,  1469, -1178,
    1470,  1459,  1468,   816,   575,    23, -1178,  1479,   831,   831,
    1426, -1178, -1178,   475,    74,  1485,  1460,    23,   913,   919,
     928,   934,   943, -1178,  1490,   785,   785,  1432,  1432, -1178,
     945, -1178, -1178, -1178,  1494, -1178, -1178,  1324,   960, -1178,
     476, -1178,  1461, -1178, -1178,  1466, -1178, -1178, -1178,  1471,
   -1178, -1178, -1178,  1495,   478,  1472,  1473,   480, -1178, -1178,
    1498, -1178, -1178, -1178, -1178, -1178, -1178, -1178,  1474,  1475,
    1476,  1477,  1478,  1480,   486, -1178, -1178,  1481,  1482,  1503,
    1506,  1484, -1178,  1486,  1488,  1316,  1483,  1491, -1178,  1507,
     583,  1492,  1509,  1510,  1487,  1489,  1493,  1496, -1178,   312,
    1514,  1497, -1178, -1178, -1178, -1178, -1178,  1499,  1500,  1501,
     975,  1504, -1178,  1516,    23,    23, -1178, -1178,  1368,  1512,
    1515, -1178,  1307, -1178,   488,   489,   490,   492,   494,   497,
   -1178, -1178, -1178, -1178, -1178, -1178,  1465,  1508,   501, -1178,
    1520,  1502,  1505,  1521,  1522,  1513,  1337,  1316,  1517,  1511,
   -1178,  1536, -1178,  1518,  1523,   511, -1178, -1178, -1178, -1178,
     312,   312,  1083,  1519, -1178, -1178, -1178,  1524,  1525, -1178,
   -1178,  1526,  1527,   990,  1548,   515,   999,  1529,  1530,  1534,
    1538,  1528,  1540,  1531,  1541,   521,   528,   540,   541,   544,
     547,  1533,  1532,  1546,  1537,  1551, -1178,  1539, -1178,  1419,
     553,  1337,  1535, -1178,   583,  1543,  1542,  1547,  1368,  1545,
     988,   236,   312,   312,  1568,  1569,   555,  1583, -1178,   556,
   -1178, -1178, -1178,  1544,  1549,  1368, -1178,  1550,  1552,  1554,
    1555,  1581,  1556,  1585,  1557,  1559,  1560,  1561,  1566,  1571,
    1574,  1587,  1595,  1558,   558,  1553, -1178,  1562,  1564, -1178,
     559,  1419, -1178,  1597,  1368,  1599,  1038,   312, -1178,  1611,
    1052,  1052,  1545, -1178,  1563, -1178, -1178,   567, -1178,  1586,
    1589,  1085, -1178, -1178, -1178, -1178,   253, -1178,   362, -1178,
    1565,  1567,  1570,  1572,  1573,  1575,  1578,  1576,  1577,  1595,
     568, -1178,  1316,  1579, -1178,  1580,  1584,  1086,  1590,  1593,
    1582, -1178, -1178,  1618,  1623,  1588,   379,   382,   391,   404,
    1591, -1178,    84,  1625,  1128, -1178,   415, -1178,    55,    64,
    1130, -1178,   424, -1178, -1178, -1178, -1178, -1178, -1178,  1594,
    1285, -1178,  1592,   569,  1316, -1178,  1626,  1596,  1633,  1634,
    1636,  1598,  1600,  1607,  1601,  1602,  1603,  1604,  1647, -1178,
    1605,    84,    84,  1606, -1178,   994,    55,    55,  1609,   428,
     431,   435,   438,   442,   445, -1178, -1178,   570, -1178,  1644,
   -1178,   571,   583,  1608,   583,  1612,  1610, -1178, -1178,  1613,
   -1178, -1178, -1178, -1178, -1178,  1658, -1178, -1178, -1178,  1615,
   -1178, -1178, -1178,  1614,  1616,  1617,  1619,  1620,  1621,   449,
    1431, -1178, -1178, -1178,  1661, -1178,  1664, -1178,  1368,  1622,
    1666, -1178, -1178, -1178, -1178, -1178, -1178,  1627,   572,  1624,
     583,  1090,  1628, -1178,  1629, -1178,  1669, -1178,  1630,  1671,
   -1178,   583, -1178, -1178,   574, -1178,  1631,  1196,  1668,  1632,
   -1178,   406, -1178, -1178, -1178,   406,   406,  1089,   153,  1635,
    1003,   239,   406,   406,  1677, -1178,  1681, -1178,   406, -1178,
    1678,  1096,  1096,  1635, -1178,  1637,   629,  1638,  1683,  1684,
   -1178,  1693, -1178,  1639,  1640,  1695, -1178, -1178
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   119,   117,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   244,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   258,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   272,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   303,   301,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   344,   342,
       0,     0,     0,     0,   353,   351,     0,     0,     0,     0,
     362,   360,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   372,     0,   370,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   118,     2,     0,     0,     0,
       4,     0,     0,     0,     0,     0,     0,     0,   243,     0,
       0,     0,     0,     0,     0,     0,    12,     0,     0,     0,
       0,     0,   257,     0,     0,     0,     0,     0,     0,     0,
      13,     0,     0,     0,     0,     0,   271,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
     300,     0,     0,     0,     0,     0,     0,     0,    15,   341,
       0,     0,     0,     0,     0,     0,    16,   350,     0,     0,
       0,     0,     0,     0,    17,     0,     0,     0,     0,     0,
     359,     0,     0,     0,     0,     0,     0,     0,    18,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     3,    24,     0,     0,     0,     0,
       5,    25,     0,     0,     0,     0,     0,     0,     0,   235,
       0,   232,   233,   234,   236,   242,     0,     0,     0,     0,
       0,   249,     0,   246,   247,   248,   250,   256,     0,     0,
       0,     0,     0,   263,     0,   260,   261,   262,   264,   270,
       0,     0,     0,     0,     0,   292,     0,   289,   290,   291,
     293,   299,   338,     0,   336,   337,   339,   340,   347,     0,
     345,   346,   348,   349,     0,     0,     0,     0,     0,   356,
       0,   354,   355,   357,   363,   358,   369,   371,     0,     0,
       0,     0,     0,     0,     0,     0,    44,     0,     0,     0,
       0,    59,     0,     0,     0,     0,     0,     0,    50,     0,
       0,   237,   238,   239,   240,   241,     0,   251,   252,   253,
     254,   255,     0,   265,   266,   267,   268,   269,     0,   294,
     295,   296,   297,   298,     0,     0,     0,   364,   365,   366,
     367,   368,     0,     0,   143,   144,     0,   146,   147,     0,
       0,     0,     0,    42,     0,     0,     0,     0,     0,     0,
      45,    46,     0,     0,     0,   120,     0,     0,     0,     0,
     245,   259,   273,   302,   343,   352,   361,     0,   145,   148,
       0,     0,    41,     0,     0,     0,    23,     0,     0,     0,
     212,   211,     0,    47,     0,     0,     0,     0,     0,   122,
       0,     0,     0,    48,     0,     0,     0,     0,     0,     0,
       0,    20,     0,     0,     0,     0,   210,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   121,     0,    59,     0,     0,     0,
       0,     0,     0,     0,    22,    21,     0,     0,     0,   207,
      53,    57,    52,    56,    51,    55,     0,   205,   206,   208,
     209,     0,     0,     0,   167,   167,   167,   169,   175,   185,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   135,     0,   141,   142,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   173,     0,     0,     0,
     183,   167,   167,   167,   169,   175,   185,    49,     0,     0,
       0,     0,     0,     0,     0,     0,   156,   154,     0,     0,
       0,   125,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   125,   128,     0,
       0,   125,     0,   128,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,     0,     0,     0,
      11,     0,     0,     0,     0,     0,     0,     7,     0,   124,
       0,   125,     0,     0,     0,   136,     0,     0,     0,     0,
     167,     0,     0,     0,     0,   169,     0,     0,     0,   127,
       0,     0,     0,     0,   128,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    93,     0,     0,     0,
      59,     0,     0,    10,     0,   155,     0,     0,    28,     0,
       6,     0,     0,     0,    54,    58,     0,     0,     0,     0,
       0,     0,     0,     0,   165,   165,   165,     0,     0,   165,
       0,     0,     0,   165,     0,     0,     0,     0,   165,     0,
     167,     0,     0,     0,   169,     0,     0,     0,     0,     0,
      93,     0,     0,     0,     0,     0,     0,     0,   231,   229,
       0,     0,     0,     0,     0,     0,     0,    26,    27,     0,
      28,     0,   123,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   204,   202,     0,     0,     0,   166,   165,     0,
     151,   161,   150,     0,     0,     0,   288,   286,     0,     0,
       0,     0,     0,     0,     0,     0,   168,     0,     0,   175,
     175,   126,     0,   312,   310,     0,     0,   174,   307,     0,
     185,     0,   185,     0,   327,     0,     0,   184,   325,     0,
     165,   165,   165,     0,   165,   165,   165,   105,     0,     0,
       0,     0,    38,     0,    40,     0,     0,   228,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    32,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   201,     0,     0,     0,     0,     0,     0,     0,    60,
     165,     0,     0,    61,    62,     0,     0,     0,     0,     0,
       0,   285,     0,     0,     0,     0,     0,     0,     0,    63,
     128,     0,     0,    64,     0,   306,     0,     0,     0,     0,
     128,     0,   185,     0,    65,   324,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   108,     0,    37,    39,     0,
       0,   225,     0,     0,   223,   224,   226,   227,     0,     0,
       0,    31,     0,     0,     0,     0,     0,     0,     0,   140,
       0,     0,     0,     0,     0,   165,   198,     0,   196,   197,
     199,   200,   153,     0,     0,   149,     0,   165,     0,     0,
       0,     0,     0,   277,     0,   274,   275,   276,   278,   284,
       0,   171,   172,   308,     0,   304,   305,   128,     0,   180,
       0,   181,   328,   322,   323,     0,    68,    69,    70,     0,
      71,    72,    73,     0,     0,     0,     0,     0,    97,    98,
       0,    92,   167,   167,   167,   169,   177,   187,     0,     0,
       0,     0,     0,     0,     0,   108,    43,     0,     0,     0,
       0,     0,   159,   157,     0,     0,    29,     0,   138,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   152,     0,
       0,     0,   279,   280,   281,   282,   283,     0,     0,     0,
       0,     0,   182,     0,   165,   165,    94,    99,     0,     0,
       0,    95,     0,   100,     0,     0,     0,     0,     0,     0,
     167,   167,   167,   169,   177,   187,     0,     0,     0,   230,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     129,     0,   139,     0,     0,     0,    66,   203,   195,   194,
       0,     0,     0,     0,    67,   287,   175,   311,     0,   185,
     326,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   163,     0,   158,     0,
       0,     0,     0,   137,     0,     0,     0,     0,     0,   193,
       0,     0,     0,     0,     0,     0,     0,     0,   185,     0,
      74,    75,   103,     0,     0,     0,    96,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   230,     0,   162,     0,     0,    34,
       0,     0,   131,     0,     0,     0,     0,     0,   190,     0,
     188,   189,   191,   192,     0,   170,   309,     0,   178,     0,
       0,     0,    59,    59,    59,    59,     0,    59,     0,    59,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     230,     8,     0,    30,    33,     0,     0,     0,     0,     0,
       0,   164,   179,     0,     0,     0,     0,     0,     0,     0,
     321,   319,     0,     0,   176,   316,     0,   334,     0,     0,
     186,   332,     0,    59,    59,    59,    59,    59,    59,     0,
     165,    76,     0,     0,     0,     9,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   315,
       0,     0,     0,     0,   331,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    59,   161,     0,    77,     0,
      36,     0,     0,     0,     0,     0,     0,   101,   102,     0,
      81,    83,    85,    87,   317,     0,   313,   314,    89,   335,
     329,   330,    91,     0,     0,     0,     0,     0,     0,     0,
       0,    78,    35,   130,     0,   133,     0,   160,     0,     0,
       0,    80,    82,    84,    86,    88,    90,     0,     0,     0,
       0,     0,   320,   333,     0,    79,     0,   134,     0,     0,
     111,     0,   104,   318,     0,   132,     0,     0,     0,     0,
     107,     0,   113,   220,   219,     0,     0,   110,     0,   218,
       0,     0,     0,     0,     0,   106,     0,   112,     0,   215,
       0,   213,   214,   216,   217,     0,     0,     0,     0,     0,
     114,     0,   116,     0,     0,     0,   109,   115
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1178, -1178,  -399, -1178, -1178,   642,   507,   179,  -527, -1178,
   -1178, -1178,   863, -1178,  1067,  -359,  -376,   982, -1178,  -453,
   -1178, -1178,   220,   773,   723, -1178,   437, -1178, -1178, -1178,
   -1178,  1202, -1178,  1673, -1178,  -518,  -561,  -208,   718,   405,
    -797,   631,  -365,  -647, -1178,  -462, -1178, -1058, -1178,   176,
    -644,  -720,  -476,  -521,   909,  -729,   412,   912,  -746,   669,
    -982,  -470,   254,  -642,  -589,  -631,   228,    62,    13,  -603,
      16, -1178,  -730, -1178, -1177, -1178,  -737, -1178,  -788,   -80,
     -39,   -60, -1178,  1641
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    12,   408,   224,   230,   709,   893,  1147,   549,   986,
    1193,    30,   543,   317,   318,   382,   225,   231,   387,   377,
      33,    35,  1188,   637,   689,   867,   974,  1334,  1348,  1357,
    1366,    13,    15,   119,   326,   600,   620,   324,   482,   556,
     718,   515,    31,   729,   730,   731,   547,   894,   983,   821,
     648,   732,   518,   522,   525,   526,  1028,   529,   530,  1029,
    1160,   727,   415,  1361,   595,   884,    46,    56,    66,   746,
      77,   757,   758,  1214,  1215,   767,   768,  1220,  1221,    83,
      89,   100,   102,   103
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     179,   180,   428,   496,   505,   710,   401,   649,   818,   519,
     520,   903,   625,   581,   851,   703,   853,  1062,   733,   734,
     841,   842,   747,   598,   329,   845,   752,   421,   855,   504,
     111,   763,   434,   748,   706,  1249,   468,   200,   201,   759,
     113,  1148,   223,   322,   550,   323,   749,   187,   188,   618,
     753,   754,   760,   623,   704,   577,   578,   579,   764,  1217,
     117,   705,   121,   676,    14,   457,   507,    78,    79,   787,
     788,   316,    16,   794,  1286,  1287,   156,   157,  1109,  1110,
     410,   411,   999,   653,   753,   754,   114,  1210,  1211,   705,
     170,   171,   508,  1148,   667,   112,    84,    85,   184,   498,
     913,   185,   284,   285,   286,   118,   940,   118,   384,   707,
     708,   422,    17,   728,   695,   142,   143,   935,   936,   386,
     943,   944,   389,   860,   861,   862,   641,   864,   865,   866,
    1161,  1162,   755,   639,   662,   294,   295,   296,   297,   298,
     831,   832,   756,   301,   302,   303,   304,   766,  1219,    80,
     557,   290,   291,   292,   511,    67,    68,    81,   371,    82,
     885,   886,   412,   773,   258,   259,   260,   261,   262,   785,
     414,    26,   265,   266,   267,   268,   756,   915,    86,  1213,
     696,   270,   271,   272,   273,   274,    87,   694,    88,   277,
     278,   279,   280,    90,    91,   385,   698,   699,   423,   246,
     247,   248,   249,   250,   769,   736,   737,   253,   254,   255,
     256,    69,    70,    71,    72,    73,   722,   723,   560,   410,
     411,  1073,    18,   918,   919,   920,   921,   922,   698,   699,
      27,   925,   926,   927,   928,   698,   699,    74,    19,  1058,
    1059,   512,  1343,  1344,  1355,    75,    20,    76,  1356,    92,
      93,    94,    95,    96,   811,   812,  1210,  1211,   996,    21,
     560,   738,   739,   740,   741,   742,   209,   210,   410,   411,
    1001,   128,   129,   133,   134,    97,   135,   560,   700,   930,
      32,   410,   411,    98,    37,    99,   701,   743,   702,   938,
     229,    22,   461,    28,    29,   744,   192,   745,   724,   193,
     565,   412,   105,   106,   107,   561,   725,   413,   726,   414,
     700,  1156,   764,   572,    23,  1058,  1059,   700,   701,   589,
     883,  1060,    47,  1119,  1345,   701,     1,   979,  1171,  1061,
      24,  1159,  1346,    57,  1360,   410,   411,  1116,  1212,    38,
      39,    40,    41,    42,   908,   909,   910,   562,  1213,   560,
     412,   234,   235,   236,   237,   238,   537,  1197,   414,   241,
     242,   243,   244,   412,   563,    43,  1217,  1071,  1072,   693,
     101,   414,  1167,    44,   560,    45,  1010,    48,    49,    50,
      51,    52,   410,   411,   560,   410,   411,   566,    58,    59,
      60,    61,    62,   765,   410,   411,  1046,  1060,   565,    25,
     573,   766,  1100,    53,   104,  1061,   590,   410,   411,  1343,
    1344,    54,   115,    55,    63,   589,   658,   412,   410,   411,
     690,   116,    64,   783,    65,   414,   444,   410,   411,   445,
    1254,   410,   411,   120,   410,   411,   629,   605,   410,   411,
     606,   410,   411,   123,  1027,   410,   411,  1218,   410,   411,
     560,   124,   410,   411,   565,  1219,   690,  1150,  1101,   807,
     849,   630,   560,   565,   412,   822,   570,   412,  1290,  1291,
    1244,   631,   414,  1245,   570,   414,   412,   575,   575,   822,
     575,  1017,  1246,  1021,   414,   633,  1024,  1025,  1026,   412,
    1036,  1345,   560,   560,   560,  1247,   565,   414,  1081,  1346,
     412,  1083,   643,   659,   211,  1036,  1253,   691,   414,   412,
    1267,  1321,  1088,   412,  1107,  1258,   412,   414,  1124,  1293,
     412,   414,  1294,   412,   414,   560,  1295,   412,   414,  1296,
     412,   414,   560,  1297,   412,   414,  1298,   721,   414,   995,
    1317,   735,   414,   781,   560,   565,   808,   850,  1081,   859,
     863,  1083,   912,   931,  1085,  1086,  1087,   706,   125,   570,
     575,   932,  -222,   706,   939,   941,   998,  1012,  1018,   130,
    1022,   575,  -221,  1269,   822,  1269,   706,  1037,  1336,  1077,
    1078,  1079,   144,  1080,   126,  1082,   147,   148,  1084,   149,
     404,  1050,  1093,   438,   439,   440,   127,   182,   183,   184,
     441,  1108,   185,  1233,   212,  1125,   405,   406,   161,   162,
     282,   163,  1135,   137,   953,   954,   955,   956,   138,  1136,
     651,   957,   407,   652,   373,   374,   958,   959,   448,   449,
     450,  1137,  1138,   670,   671,  1139,   652,   672,  1140,   490,
     491,   492,    28,    29,  1149,  1271,  1165,  1168,   158,  -222,
    1194,   960,   868,   869,   870,   607,  1318,   608,  1202,  -221,
    1270,  1301,  1302,  1325,     1,  1337,   436,   213,   437,   442,
     443,   444,   675,   139,   445,   652,   451,   961,   175,   176,
     140,   177,   452,   453,     2,     3,   856,   493,   141,   968,
     969,   970,   677,   494,   495,   672,   477,   478,   479,  1347,
     871,   380,   381,  1349,  1350,  1152,   872,   873,   151,   713,
    1362,  1363,   652,   152,     4,     5,     6,     7,   153,  1206,
    1207,  1208,  1209,  1369,  1216,  1370,  1222,   131,   132,   133,
     134,   154,   135,     8,     9,   205,   206,   971,   207,   155,
     239,    10,    11,   972,   973,   145,   146,   147,   148,   165,
     149,   159,   160,   161,   162,   166,   163,   167,   251,   168,
     131,   132,   133,   134,   263,   135,   761,   136,  1266,   672,
    1259,  1260,  1261,  1262,  1263,  1264,   173,   174,   175,   176,
     169,   177,   203,   204,   205,   206,   172,   207,   195,   275,
     181,   131,   132,   133,   134,   299,   135,   131,   132,   133,
     134,   189,   135,   196,   331,   197,   131,   132,   133,   134,
     332,   135,  1299,   131,   132,   133,   134,   198,   135,   333,
     131,   132,   133,   134,   199,   135,   334,   145,   146,   147,
     148,   202,   149,   335,   145,   146,   147,   148,   214,   149,
     337,   145,   146,   147,   148,   215,   149,   338,   145,   146,
     147,   148,   216,   149,   339,   145,   146,   147,   148,   805,
     149,   340,   806,   218,   159,   160,   161,   162,   341,   163,
     217,   836,   837,  1303,   838,  1305,   219,   343,   159,   160,
     161,   162,   220,   163,   159,   160,   161,   162,   221,   163,
     222,   344,   223,   159,   160,   161,   162,   345,   163,   159,
     160,   161,   162,   226,   163,   792,   346,   994,   793,   228,
     806,   227,   347,   173,   174,   175,   176,   816,   177,   229,
     817,  1327,   173,   174,   175,   176,   349,   177,   173,   174,
     175,   176,  1335,   177,   233,   350,   232,   173,   174,   175,
     176,   351,   177,   173,   174,   175,   176,   135,   177,   240,
     352,   245,   203,   204,   205,   206,   353,   207,   203,   204,
     205,   206,   149,   207,   252,   357,   257,   203,   204,   205,
     206,   358,   207,   203,   204,   205,   206,   163,   207,   269,
     359,   177,   203,   204,   205,   206,   360,   207,   834,   835,
     836,   837,   281,   838,   264,   361,   276,   834,   835,   836,
     837,   923,   838,   834,   835,   836,   837,   185,   838,   283,
    1002,   287,   834,   835,   836,   837,  1003,   838,   834,   835,
     836,   837,   193,   838,   289,  1004,   293,   834,   835,   836,
     837,  1005,   838,   190,   191,   192,  1008,   308,   193,   672,
    1006,   207,   442,   443,   444,   300,   288,   445,   880,   791,
     792,  1011,   305,   793,   672,   469,   307,   814,   815,   816,
     309,   881,   817,   182,   183,   184,  1068,   311,   185,   672,
     906,   310,  1157,  1112,  1113,   312,   942,  1114,   182,   183,
     184,  1122,   314,   185,   806,  1158,   313,  1358,  1352,  1353,
    1126,  1289,  1354,   606,   145,   146,   147,   148,   316,   149,
    1359,   150,   159,   160,   161,   162,   315,   163,   319,   164,
     173,   174,   175,   176,   320,   177,   325,   178,   203,   204,
     205,   206,   321,   207,   366,   208,   182,   183,   184,  1199,
     327,   185,   806,   186,   328,   190,   191,   192,  1113,  1255,
     193,  1114,   194,   834,   835,   836,   837,   330,   838,   790,
     791,   792,   402,   374,   793,   814,   815,   816,   223,   433,
     817,   888,   791,   792,   470,   471,   793,  1111,  1112,  1113,
     472,   473,  1114,  1351,  1352,  1353,  1205,  1237,  1354,   806,
     806,  1328,  1353,   336,   806,  1354,   108,   109,   110,   474,
     475,   509,   510,   513,   514,   523,   524,   527,   528,   596,
     597,   596,   650,   596,   784,   364,   847,   848,   857,   858,
     596,   878,  1251,  1252,  1256,  1257,  1339,  1340,    34,    36,
     342,   348,   354,   355,   356,   362,   363,   369,   365,   367,
     368,   370,   375,   376,   378,   379,   383,   390,   388,   391,
     392,   398,   393,   394,   395,   396,   397,   399,   400,   403,
     409,   416,   419,   418,   424,   420,   425,   427,   426,   429,
     417,   430,   431,   432,   435,   446,   447,   454,   455,   456,
     458,   459,   460,   463,   464,   465,   476,   466,   480,   445,
     499,   484,   503,   481,   485,   467,   483,   486,   487,   502,
     488,   504,   517,   489,   516,   497,   521,   538,   500,   539,
     501,   506,   531,   532,   533,   534,   535,   541,   612,   542,
     546,   555,   558,   613,   570,   575,   576,   580,   592,   585,
     594,   599,   602,   688,   536,   603,   552,   540,   619,   544,
    1195,   636,   548,   645,   551,   545,   614,   617,   646,   621,
     626,   647,   657,   663,   668,   567,   553,   554,   559,   674,
     564,   679,   798,   712,   714,   568,   604,   569,   571,   681,
     574,   682,   584,   586,   683,   587,   685,   686,   588,   687,
     711,   601,   717,   609,   593,   661,   610,   615,   720,   789,
     624,   751,   611,   616,   372,   826,   628,   622,   632,   634,
     627,   635,   638,   640,   719,   796,   800,   644,   642,   803,
     810,   813,   987,   822,   825,   833,   846,   591,   654,  1232,
     462,   692,  1038,   779,   655,   656,   780,   887,   660,   889,
     896,   664,   665,   716,   666,   892,   897,  1076,   899,   669,
     673,   901,   678,   911,   680,   914,   684,   929,   809,   933,
     904,   976,  1300,   916,   582,   697,  1089,   750,   583,   762,
     770,   876,   771,   772,   715,   774,   775,   776,   786,   777,
     778,   945,   782,   949,   804,   977,   978,   981,   799,   795,
     797,   982,   990,   991,   820,   801,   819,   840,   802,   823,
     824,   844,   997,   900,   827,   828,   829,   830,   839,   706,
     898,   843,   854,  1007,   852,   874,   875,  1009,  1016,   877,
     793,  1023,   993,   891,   882,   895,  1041,   879,   890,  1042,
    1049,   905,  1052,  1053,   902,   817,   907,  1063,  1055,   917,
    1070,   838,  1092,  1094,  1097,     0,  1098,   924,   937,   962,
     934,   946,   947,   948,   950,   951,   952,   984,   963,  1104,
     964,   965,   966,   967,   975,  1074,   985,  1013,  1075,   980,
    1091,  1123,   989,   988,  1145,  1000,  1014,  1106,  1051,   992,
    1143,  1015,  1019,  1020,  1030,  1031,  1032,  1033,  1034,  1045,
    1035,  1163,  1164,  1047,  1127,  1128,  1040,  1039,  1043,  1129,
    1044,  1054,  1048,  1130,  1056,  1132,  1134,  1166,  1064,  1176,
    1066,  1186,  1057,  1178,  1069,  1065,  1095,  1067,  1102,  1187,
    1196,  1096,  1198,  1099,  1180,  1181,  1182,  1103,  1153,  1105,
    1117,  1183,  1155,  1115,  1200,  1118,  1184,  1120,  1121,  1185,
    1203,  1241,  1142,  1204,  1229,  1151,  1242,  1131,  1250,  1272,
    1133,  1141,  1154,  1144,  1114,  1146,  1274,  1275,  1169,  1276,
    1172,  1279,  1173,  1170,  1174,  1175,  1177,  1179,  1189,  1190,
    1236,  1284,   704,  1191,  1192,  1223,  1238,  1224,  1239,  1201,
    1225,  1309,  1226,  1227,  1319,  1228,  1230,  1320,  1231,  1234,
    1323,  1235,  1331,  1304,  1324,  1333,  1240,  1248,  1306,  1341,
    1364,  1367,  1243,  1268,  1265,  1365,  1372,  1273,  1373,  1277,
    1326,  1278,  1280,  1281,  1282,  1283,  1374,  1288,  1377,  1285,
    1292,  1310,     0,  1308,  1090,  1311,  1307,  1312,  1313,   122,
    1314,  1315,  1316,     0,  1329,     0,     0,     0,  1322,  1330,
       0,  1332,  1342,     0,  1354,     0,     0,     0,     0,     0,
    1338,     0,  1371,     0,     0,     0,  1376,  1368,     0,  1375,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   306
};

static const yytype_int16 yycheck[] =
{
      80,    81,   401,   456,   466,   649,   371,   596,   728,   485,
     486,   808,   573,   534,   760,   646,   762,   999,   665,   666,
     749,   750,   669,   550,   232,   755,   673,   386,   765,     6,
       4,   678,   408,    77,     4,  1212,   435,    97,    98,    77,
      32,  1099,    52,    28,   506,    30,    90,    86,    87,   567,
       3,     4,    90,   571,     8,   531,   532,   533,     4,     4,
      27,    15,    27,   624,     4,   424,    76,     3,     4,   700,
     701,     4,    90,   704,  1251,  1252,    63,    64,  1060,  1061,
       3,     4,     8,   601,     3,     4,    78,     3,     4,    15,
      74,    75,   468,  1151,   615,    69,     3,     4,    86,   458,
     820,    89,   182,   183,   184,    72,   852,    72,     4,    79,
      80,     4,    90,    90,   641,    53,    54,   847,   848,   327,
     857,   858,   330,   770,   771,   772,   588,   774,   775,   776,
    1112,  1113,    85,   586,   610,   195,   196,   197,   198,   199,
     743,   744,    95,   203,   204,   205,   206,    93,    93,    85,
     515,   190,   191,   192,     3,     3,     4,    93,    91,    95,
     791,   792,    85,   684,   151,   152,   153,   154,   155,   696,
      93,     0,   159,   160,   161,   162,    95,   821,    85,    95,
     642,   165,   166,   167,   168,   169,    93,   640,    95,   173,
     174,   175,   176,     3,     4,    91,     3,     4,    91,   137,
     138,   139,   140,   141,   680,     3,     4,   145,   146,   147,
     148,    59,    60,    61,    62,    63,     3,     4,     4,     3,
       4,  1018,    90,   826,   827,   828,   829,   830,     3,     4,
       4,   834,   835,   836,   837,     3,     4,    85,    90,     3,
       4,    90,     3,     4,    91,    93,    90,    95,    95,    59,
      60,    61,    62,    63,   724,   725,     3,     4,   905,    90,
       4,    59,    60,    61,    62,    63,    91,    92,     3,     4,
     917,    43,    44,    86,    87,    85,    89,     4,    85,   840,
      94,     3,     4,    93,     4,    95,    93,    85,    95,   850,
      52,    90,    54,    67,    68,    93,    86,    95,    85,    89,
       4,    85,    36,    37,    38,    91,    93,    91,    95,    93,
      85,  1108,     4,     3,    90,     3,     4,    85,    93,     4,
      95,    85,     4,  1069,    85,    93,     5,    95,  1125,    93,
      90,    95,    93,     4,    95,     3,     4,  1066,    85,    59,
      60,    61,    62,    63,   814,   815,   816,    91,    95,     4,
      85,   123,   124,   125,   126,   127,    91,  1154,    93,   131,
     132,   133,   134,    85,    91,    85,     4,  1014,  1015,    91,
       3,    93,  1118,    93,     4,    95,   937,    59,    60,    61,
      62,    63,     3,     4,     4,     3,     4,    91,    59,    60,
      61,    62,    63,    85,     3,     4,   985,    85,     4,    90,
      90,    93,  1046,    85,    90,    93,    91,     3,     4,     3,
       4,    93,     4,    95,    85,     4,     3,    85,     3,     4,
       4,    91,    93,    91,    95,    93,    86,     3,     4,    89,
    1218,     3,     4,    91,     3,     4,    91,    91,     3,     4,
      94,     3,     4,    93,   965,     3,     4,    85,     3,     4,
       4,    93,     3,     4,     4,    93,     4,  1101,  1047,     3,
       3,    91,     4,     4,    85,     4,     4,    85,  1256,  1257,
      91,    91,    93,    91,     4,    93,    85,     4,     4,     4,
       4,     3,    91,     3,    93,    91,   962,   963,   964,    85,
       4,    85,     4,     4,     4,    91,     4,    93,     4,    93,
      85,     4,    91,    90,    94,     4,    91,    91,    93,    85,
    1230,  1308,  1033,    85,     3,    91,    85,    93,     3,    91,
      85,    93,    91,    85,    93,     4,    91,    85,    93,    91,
      85,    93,     4,    91,    85,    93,    91,    91,    93,   904,
      91,    91,    93,    91,     4,     4,    90,    90,     4,    91,
      91,     4,    91,    91,  1030,  1031,  1032,     4,    93,     4,
       4,    91,     4,     4,    91,    91,    91,    91,    90,     3,
      90,     4,     4,     4,     4,     4,     4,    91,     4,    91,
      91,    91,     3,    91,    93,    91,    86,    87,    91,    89,
       7,   990,    91,     8,     9,    10,    93,    84,    85,    86,
      15,    90,    89,  1192,    90,    90,    23,    24,    86,    87,
      97,    89,    91,    93,    28,    29,    30,    31,    93,    91,
      91,    35,    39,    94,    92,    93,    40,    41,    16,    17,
      18,    91,    91,    91,    91,    91,    94,    94,    91,    16,
      17,    18,    67,    68,    91,  1234,    91,    91,     3,    91,
      91,    65,    16,    17,    18,    29,  1300,    31,    91,    91,
      91,    91,    91,    91,     5,    91,   412,     4,   414,    84,
      85,    86,    91,    93,    89,    94,    64,    91,    86,    87,
      93,    89,    70,    71,    25,    26,   766,    64,    93,    16,
      17,    18,    91,    70,    71,    94,   442,   443,   444,  1341,
      64,    50,    51,  1345,  1346,  1104,    70,    71,    93,    91,
    1352,  1353,    94,    93,    55,    56,    57,    58,    93,  1172,
    1173,  1174,  1175,    94,  1177,    96,  1179,    84,    85,    86,
      87,    93,    89,    74,    75,    86,    87,    64,    89,    93,
      97,    82,    83,    70,    71,    84,    85,    86,    87,    93,
      89,    84,    85,    86,    87,    93,    89,    93,    97,    93,
      84,    85,    86,    87,    97,    89,    91,    91,  1230,    94,
    1223,  1224,  1225,  1226,  1227,  1228,    84,    85,    86,    87,
      93,    89,    84,    85,    86,    87,     3,    89,    93,    97,
       3,    84,    85,    86,    87,    97,    89,    84,    85,    86,
      87,     3,    89,    93,    97,    93,    84,    85,    86,    87,
      97,    89,  1265,    84,    85,    86,    87,    93,    89,    97,
      84,    85,    86,    87,    93,    89,    97,    84,    85,    86,
      87,     3,    89,    97,    84,    85,    86,    87,     4,    89,
      97,    84,    85,    86,    87,     3,    89,    97,    84,    85,
      86,    87,     4,    89,    97,    84,    85,    86,    87,    91,
      89,    97,    94,     3,    84,    85,    86,    87,    97,    89,
       4,    86,    87,  1272,    89,  1274,    90,    97,    84,    85,
      86,    87,    90,    89,    84,    85,    86,    87,     3,    89,
       3,    97,    52,    84,    85,    86,    87,    97,    89,    84,
      85,    86,    87,    90,    89,    86,    97,    91,    89,    27,
      94,    90,    97,    84,    85,    86,    87,    86,    89,    52,
      89,  1320,    84,    85,    86,    87,    97,    89,    84,    85,
      86,    87,  1331,    89,    27,    97,    90,    84,    85,    86,
      87,    97,    89,    84,    85,    86,    87,    89,    89,    94,
      97,     3,    84,    85,    86,    87,    97,    89,    84,    85,
      86,    87,    89,    89,    94,    97,     3,    84,    85,    86,
      87,    97,    89,    84,    85,    86,    87,    89,    89,     3,
      97,    89,    84,    85,    86,    87,    97,    89,    84,    85,
      86,    87,     3,    89,    94,    97,    94,    84,    85,    86,
      87,    97,    89,    84,    85,    86,    87,    89,    89,    94,
      97,     3,    84,    85,    86,    87,    97,    89,    84,    85,
      86,    87,    89,    89,    94,    97,     3,    84,    85,    86,
      87,    97,    89,    84,    85,    86,    91,    32,    89,    94,
      97,    89,    84,    85,    86,    94,    97,    89,    84,    85,
      86,    91,     3,    89,    94,    97,     3,    84,    85,    86,
      94,    97,    89,    84,    85,    86,    91,     4,    89,    94,
      97,    94,    84,    85,    86,    94,    97,    89,    84,    85,
      86,    91,     4,    89,    94,    97,    94,    84,    85,    86,
      91,    97,    89,    94,    84,    85,    86,    87,     4,    89,
      97,    91,    84,    85,    86,    87,    90,    89,    65,    91,
      84,    85,    86,    87,    32,    89,     4,    91,    84,    85,
      86,    87,    90,    89,    94,    91,    84,    85,    86,    91,
      90,    89,    94,    91,    90,    84,    85,    86,    86,  1219,
      89,    89,    91,    84,    85,    86,    87,    90,    89,    84,
      85,    86,    92,    93,    89,    84,    85,    86,    52,    53,
      89,    84,    85,    86,     3,     4,    89,    84,    85,    86,
       3,     4,    89,    84,    85,    86,    91,    91,    89,    94,
      94,    91,    86,     3,    94,    89,    36,    37,    38,     3,
       4,     3,     4,    40,    41,    76,    77,    76,    77,    90,
      91,    90,    91,    90,    91,     4,    84,    85,    84,    85,
      90,    91,    84,    85,    84,    85,    20,    21,    16,    17,
       3,     3,     3,     3,     3,     3,     3,    94,     4,     4,
       4,    32,     3,     3,     3,    90,     8,    96,    49,    96,
      96,     4,    96,    96,    96,    96,    91,     4,     3,     3,
      65,    42,     3,    91,     3,     8,    49,    91,    90,    94,
      33,    23,     4,    23,     3,     3,     3,     3,    91,    90,
       3,    65,    90,    95,     4,     4,    95,    90,     3,    89,
       3,    90,     3,    35,    90,    53,    94,    90,    90,    54,
      90,     6,     3,    90,    34,    91,     3,    43,    65,    91,
      90,    90,    90,    90,    90,    90,    90,     3,    14,     4,
       4,     4,     3,    14,     4,     4,     3,     3,     3,    43,
       4,     4,     3,    27,    90,     4,    76,    91,     4,    90,
    1151,     4,    90,     4,    90,    94,    14,    14,     8,    14,
      14,     4,     4,     8,     8,    90,    94,    94,    91,     8,
      91,     8,   710,     4,     3,    90,   100,    91,    90,    14,
      91,    14,    90,    90,    14,    91,    14,    14,    90,    14,
      14,    90,     4,    91,    94,    42,    90,    90,     3,     3,
      90,     4,    99,    99,   317,    93,    91,    99,    91,    91,
      99,    91,    90,    90,    33,    15,    14,    96,    90,     3,
      14,     3,   895,     4,    14,     3,     3,   544,    96,  1189,
     428,   638,   975,   690,    96,    95,    44,     3,    90,     3,
       3,    90,    90,    65,    90,     6,     3,  1022,     3,    90,
      90,     4,    90,     3,    90,     4,    90,     3,   720,     4,
     809,     4,  1266,    15,   535,    97,  1034,    90,   536,    90,
      90,    44,    90,    90,   100,    90,    90,    90,    85,    90,
      90,    14,    91,    14,   100,     3,     3,     3,    90,    95,
      91,     4,     3,     3,    90,    94,    91,    90,    95,    91,
      91,    86,     3,    65,    93,    93,    93,    93,    91,     4,
      66,    91,    91,     3,    90,    90,    90,     3,     3,    91,
      89,     3,    34,    91,    94,    90,     3,    98,    95,     3,
       3,    90,     3,     3,    94,    89,    94,     3,    29,    90,
       4,    89,    14,     3,     3,    -1,     4,    94,    90,    90,
      94,    91,    91,    91,    91,    91,    91,    91,    90,     3,
      90,    90,    90,    90,    90,    33,    90,    86,    33,    94,
      85,     3,    94,    96,     3,    95,    90,    34,    66,   100,
      14,    90,    90,    90,    90,    90,    90,    90,    90,    81,
      90,     3,     3,    90,    45,    45,    94,    96,    94,    45,
      94,    94,    91,    45,    91,    45,    45,     4,    91,     8,
      90,     4,    96,     8,    90,    96,    94,    96,    81,     4,
       3,    96,     3,    90,    45,    45,    45,    96,    65,    91,
      86,    45,    65,    94,     3,    90,    45,    91,    91,    45,
      34,     3,    90,    34,    46,    90,     3,    99,     3,     3,
      99,    98,    90,    96,    89,    96,     3,     3,    94,     3,
      90,    34,    90,    94,    90,    90,    90,    90,    90,    96,
      66,     4,     8,    91,    90,    90,    66,    90,    65,    96,
      90,     3,    90,    90,     3,    90,    90,     3,    91,    90,
       4,    91,     3,    65,    47,     4,    94,    86,    66,    11,
       3,     3,    94,    91,    90,     4,     3,    91,     4,    91,
      66,    91,    91,    91,    91,    91,     3,    91,     3,    94,
      91,    86,    -1,    90,  1035,    91,    96,    91,    91,    36,
      91,    91,    91,    -1,    86,    -1,    -1,    -1,    96,    90,
      -1,    91,    90,    -1,    89,    -1,    -1,    -1,    -1,    -1,
      99,    -1,    94,    -1,    -1,    -1,    96,   100,    -1,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   210
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,    25,    26,    55,    56,    57,    58,    74,    75,
      82,    83,   102,   132,     4,   133,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,     0,     4,    67,    68,
     112,   143,    94,   121,   132,   122,   132,     4,    59,    60,
      61,    62,    63,    85,    93,    95,   167,     4,    59,    60,
      61,    62,    63,    85,    93,    95,   168,     4,    59,    60,
      61,    62,    63,    85,    93,    95,   169,     3,     4,    59,
      60,    61,    62,    63,    85,    93,    95,   171,     3,     4,
      85,    93,    95,   180,     3,     4,    85,    93,    95,   181,
       3,     4,    59,    60,    61,    62,    63,    85,    93,    95,
     182,     3,   183,   184,    90,    36,    37,    38,    36,    37,
      38,     4,    69,    32,    78,     4,    91,    27,    72,   134,
      91,    27,   134,    93,    93,    93,    93,    93,   167,   167,
       3,    84,    85,    86,    87,    89,    91,    93,    93,    93,
      93,    93,   168,   168,     3,    84,    85,    86,    87,    89,
      91,    93,    93,    93,    93,    93,   169,   169,     3,    84,
      85,    86,    87,    89,    91,    93,    93,    93,    93,    93,
     171,   171,     3,    84,    85,    86,    87,    89,    91,   180,
     180,     3,    84,    85,    86,    89,    91,   181,   181,     3,
      84,    85,    86,    89,    91,    93,    93,    93,    93,    93,
     182,   182,     3,    84,    85,    86,    87,    89,    91,    91,
      92,    94,    90,     4,     4,     3,     4,     4,     3,    90,
      90,     3,     3,    52,   104,   117,    90,    90,    27,    52,
     105,   118,    90,    27,   167,   167,   167,   167,   167,    97,
      94,   167,   167,   167,   167,     3,   168,   168,   168,   168,
     168,    97,    94,   168,   168,   168,   168,     3,   169,   169,
     169,   169,   169,    97,    94,   169,   169,   169,   169,     3,
     171,   171,   171,   171,   171,    97,    94,   171,   171,   171,
     171,     3,    97,    94,   180,   180,   180,     3,    97,    94,
     181,   181,   181,     3,   182,   182,   182,   182,   182,    97,
      94,   182,   182,   182,   182,     3,   184,     3,    32,    94,
      94,     4,    94,    94,     4,    90,     4,   114,   115,    65,
      32,    90,    28,    30,   138,     4,   135,    90,    90,   138,
      90,    97,    97,    97,    97,    97,     3,    97,    97,    97,
      97,    97,     3,    97,    97,    97,    97,    97,     3,    97,
      97,    97,    97,    97,     3,     3,     3,    97,    97,    97,
      97,    97,     3,     3,     4,     4,    94,     4,     4,    94,
      32,    91,   115,    92,    93,     3,     3,   120,     3,    90,
      50,    51,   116,     8,     4,    91,   138,   119,    49,   138,
      96,    96,    96,    96,    96,    96,    96,    91,     4,     4,
       3,   143,    92,     3,     7,    23,    24,    39,   103,    65,
       3,     4,    85,    91,    93,   163,    42,    33,    91,     3,
       8,   116,     4,    91,     3,    49,    90,    91,   103,    94,
      23,     4,    23,    53,   117,     3,   163,   163,     8,     9,
      10,    15,    84,    85,    86,    89,     3,     3,    16,    17,
      18,    64,    70,    71,     3,    91,    90,   116,     3,    65,
      90,    54,   118,    95,     4,     4,    90,    53,   103,    97,
       3,     4,     3,     4,     3,     4,    95,   163,   163,   163,
       3,    35,   139,    94,    90,    90,    90,    90,    90,    90,
      16,    17,    18,    64,    70,    71,   120,    91,   116,     3,
      65,    90,    54,     3,     6,   146,    90,    76,   117,     3,
       4,     3,    90,    40,    41,   142,    34,     3,   153,   153,
     153,     3,   154,    76,    77,   155,   156,    76,    77,   158,
     159,    90,    90,    90,    90,    90,    90,    91,    43,    91,
      91,     3,     4,   113,    90,    94,     4,   147,    90,   109,
     146,    90,    76,    94,    94,     4,   140,   143,     3,    91,
       4,    91,    91,    91,    91,     4,    91,    90,    90,    91,
       4,    90,     3,    90,    91,     4,     3,   153,   153,   153,
       3,   154,   155,   158,    90,    43,    90,    91,    90,     4,
      91,   113,     3,    94,     4,   165,    90,    91,   109,     4,
     136,    90,     3,     4,   100,    91,    94,    29,    31,    91,
      90,    99,    14,    14,    14,    90,    99,    14,   136,     4,
     137,    14,    99,   136,    90,   137,    14,    99,    91,    91,
      91,    91,    91,    91,    91,    91,     4,   124,    90,   120,
      90,   146,    90,    91,    96,     4,     8,     4,   151,   165,
      91,    91,    94,   136,    96,    96,    95,     4,     3,    90,
      90,    42,   153,     8,    90,    90,    90,   154,     8,    90,
      91,    91,    94,    90,     8,    91,   137,    91,    90,     8,
      90,    14,    14,    14,    90,    14,    14,    14,    27,   125,
       4,    91,   124,    91,   120,   109,   146,    97,     3,     4,
      85,    93,    95,   166,     8,    15,     4,    79,    80,   106,
     151,    14,     4,    91,     3,   100,    65,     4,   141,    33,
       3,    91,     3,     4,    85,    93,    95,   162,    90,   144,
     145,   146,   152,   144,   144,    91,     3,     4,    59,    60,
      61,    62,    63,    85,    93,    95,   170,   144,    77,    90,
      90,     4,   144,     3,     4,    85,    95,   172,   173,    77,
      90,    91,    90,   144,     4,    85,    93,   176,   177,   153,
      90,    90,    90,   154,    90,    90,    90,    90,    90,   125,
      44,    91,    91,    91,    91,   109,    85,   166,   166,     3,
      84,    85,    86,    89,   166,    95,    15,    91,   106,    90,
      14,    94,    95,     3,   100,    91,    94,     3,    90,   139,
      14,   162,   162,     3,    84,    85,    86,    89,   152,    91,
      90,   150,     4,    91,    91,    14,    93,    93,    93,    93,
      93,   170,   170,     3,    84,    85,    86,    87,    89,    91,
      90,   156,   156,    91,    86,   173,     3,    84,    85,     3,
      90,   159,    90,   159,    91,   177,   180,    84,    85,    91,
     144,   144,   144,    91,   144,   144,   144,   126,    16,    17,
      18,    64,    70,    71,    90,    90,    44,    91,    91,    98,
      84,    97,    94,    95,   166,   166,   166,     3,    84,     3,
      95,    91,     6,   107,   148,    90,     3,     3,    66,     3,
      65,     4,    94,   141,   142,    90,    97,    94,   162,   162,
     162,     3,    91,   152,     4,   151,    15,    90,   170,   170,
     170,   170,   170,    97,    94,   170,   170,   170,   170,     3,
     137,    91,    91,     4,    94,   173,   173,    90,   137,    91,
     159,    91,    97,   177,   177,    14,    91,    91,    91,    14,
      91,    91,    91,    28,    29,    30,    31,    35,    40,    41,
      65,    91,    90,    90,    90,    90,    90,    90,    16,    17,
      18,    64,    70,    71,   127,    90,     4,     3,     3,    95,
      94,     3,     4,   149,    91,    90,   110,   107,    96,    94,
       3,     3,   100,    34,    91,   143,   144,     3,    91,     8,
      95,   144,    97,    97,    97,    97,    97,     3,    91,     3,
     137,    91,    91,    86,    90,    90,     3,     3,    90,    90,
      90,     3,    90,     3,   153,   153,   153,   154,   157,   160,
      90,    90,    90,    90,    90,    90,     4,    91,   127,    96,
      94,     3,     3,    94,    94,    81,   165,    90,    91,     3,
     103,    66,     3,     3,    94,    29,    91,    96,     3,     4,
      85,    93,   161,     3,    91,    96,    90,    96,    91,    90,
       4,   144,   144,   141,    33,    33,   140,    91,    91,    91,
      91,     4,    91,     4,    91,   153,   153,   153,   154,   157,
     160,    85,    14,    91,     3,    94,    96,     3,     4,    90,
     151,   165,    81,    96,     3,    91,    34,     3,    90,   161,
     161,    84,    85,    86,    89,    94,   156,    86,    90,   159,
      91,    91,    91,     3,     3,    90,    91,    45,    45,    45,
      45,    99,    45,    99,    45,    91,    91,    91,    91,    91,
      91,    98,    90,    14,    96,     3,    96,   108,   148,    91,
     151,    90,   103,    65,    90,    65,   141,    84,    97,    95,
     161,   161,   161,     3,     3,    91,     4,   159,    91,    94,
      94,   141,    90,    90,    90,    90,     8,    90,     8,    90,
      45,    45,    45,    45,    45,    45,     4,     4,   123,    90,
      96,    91,    90,   111,    91,   108,     3,   141,     3,    91,
       3,    96,    91,    34,    34,    91,   120,   120,   120,   120,
       3,     4,    85,    95,   174,   175,   120,     4,    85,    93,
     178,   179,   120,    90,    90,    90,    90,    90,    90,    46,
      90,    91,   123,   165,    90,    91,    66,    91,    66,    65,
      94,     3,     3,    94,    91,    91,    91,    91,    86,   175,
       3,    84,    85,    91,   179,   180,    84,    85,    91,   120,
     120,   120,   120,   120,   120,    90,   146,   152,    91,     4,
      91,   165,     3,    91,     3,     3,     3,    91,    91,    34,
      91,    91,    91,    91,     4,    94,   175,   175,    91,    97,
     179,   179,    91,    91,    91,    91,    91,    91,    91,   120,
     150,    91,    91,   103,    65,   103,    66,    96,    90,     3,
      86,    91,    91,    91,    91,    91,    91,    91,   151,     3,
       3,   141,    96,     4,    47,    91,    66,   103,    91,    86,
      90,     3,    91,     4,   128,   103,     4,    91,    99,    20,
      21,    11,    90,     3,     4,    85,    93,   164,   129,   164,
     164,    84,    85,    86,    89,    91,    95,   130,    84,    97,
      95,   164,   164,   164,     3,     4,   131,     3,   100,    94,
      96,    94,     3,     4,     3,   100,    96,     3
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   101,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     103,   103,   103,   103,   104,   105,   106,   106,   106,   107,
     108,   109,   109,   110,   110,   111,   111,   112,   112,   113,
     113,   114,   114,   115,   115,   116,   116,   117,   118,   119,
     119,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   122,   122,   123,   123,
     124,   124,   124,   124,   124,   124,   124,   124,   124,   124,
     124,   124,   125,   125,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   127,   127,   127,   128,
     128,   128,   129,   129,   130,   131,   131,   132,   133,   133,
     134,   135,   135,   136,   136,   136,   137,   137,   137,   138,
     138,   138,   138,   138,   138,   139,   139,   140,   140,   141,
     141,   142,   142,   143,   143,   143,   143,   143,   143,   144,
     144,   144,   145,   145,   146,   147,   147,   148,   149,   149,
     150,   150,   151,   151,   152,   152,   153,   153,   154,   154,
     155,   155,   155,   155,   156,   156,   157,   157,   158,   158,
     158,   158,   158,   158,   159,   159,   160,   160,   161,   161,
     161,   161,   161,   161,   161,   161,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   163,   163,   163,   163,   163,
     163,   163,   163,   164,   164,   164,   164,   164,   164,   164,
     164,   165,   165,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   172,   172,   172,   172,   173,   173,
     173,   173,   173,   174,   174,   174,   174,   175,   175,   175,
     175,   175,   176,   176,   176,   176,   177,   177,   177,   178,
     178,   178,   178,   179,   179,   179,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   183,
     183,   184,   184
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     5,     4,     5,    13,    12,    21,    22,
      13,    12,     4,     4,     4,     4,     4,     4,     4,     4,
       2,     3,     3,     1,     1,     1,     1,     1,     0,     2,
       2,     6,     5,     5,     4,     4,     3,    15,    14,     6,
       5,     3,     2,    13,     1,     1,     1,     4,     4,     5,
       0,     4,     4,     4,     8,     4,     4,     4,     8,     0,
      14,    14,    14,    14,    14,    14,    17,    17,    15,    15,
      15,    15,    15,    15,    18,    18,    20,    21,     4,     6,
      13,    12,    13,    12,    13,    12,    13,    12,    13,    12,
      13,    12,     4,     0,     3,     3,     5,     2,     2,     3,
       3,     9,     9,     5,    13,     0,    17,    14,     0,    11,
       5,     0,     2,     0,     3,     5,     3,     2,     3,     1,
       4,     4,     3,     3,     1,     0,     3,     1,     0,    14,
      20,    16,    24,    20,    22,     2,     4,     9,     7,     5,
       3,     1,     1,     5,     5,     6,     5,     5,     6,     3,
       1,     1,     4,     3,     2,     3,     1,     2,     3,     1,
      10,     0,     8,     7,     8,     0,     5,     0,     5,     0,
      11,     7,     7,     1,     5,     0,     5,     0,    11,    12,
       7,     7,     8,     1,     5,     0,     5,     0,     3,     3,
       3,     3,     3,     2,     1,     1,     3,     3,     3,     3,
       3,     2,     1,     5,     1,     3,     3,     3,     3,     3,
       2,     1,     1,     3,     3,     3,     3,     3,     2,     1,
       1,    10,     9,     3,     3,     3,     3,     3,     2,     1,
       5,     1,     3,     3,     3,     3,     3,     4,     4,     4,
       4,     4,     3,     2,     1,     5,     3,     3,     3,     3,
       3,     4,     4,     4,     4,     4,     3,     2,     1,     5,
       3,     3,     3,     3,     3,     4,     4,     4,     4,     4,
       3,     2,     1,     5,     3,     3,     3,     3,     3,     4,
       4,     4,     4,     4,     3,     2,     1,     5,     1,     3,
       3,     3,     3,     3,     4,     4,     4,     4,     4,     3,
       2,     1,     5,     1,     3,     3,     2,     1,     3,     7,
       1,     5,     1,     3,     3,     2,     1,     3,     7,     1,
       5,     1,     3,     3,     2,     1,     5,     1,     3,     3,
       3,     2,     1,     5,     1,     3,     3,     3,     3,     3,
       3,     2,     1,     5,     1,     3,     3,     3,     3,     3,
       2,     1,     5,     1,     3,     3,     3,     3,     3,     2,
       1,     5,     1,     3,     4,     4,     4,     4,     4,     3,
       1,     3,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 174 "modelParser.y" /* yacc.c:1646  */
    {
	int mkres = mkdir(outputDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if(mkres < 0 && errno != EEXIST)
	{
		printf("Can not create the directory for output files.\n");
		exit(1);
	}

	mkres = mkdir(imageDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if(mkres < 0 && errno != EEXIST)
	{
		printf("Can not create the directory for images.\n");
		exit(1);
	}

	int result;

	clock_t begin, end;
	begin = clock();
	result = continuousProblem.run();
	end = clock();

	printf("\n");

	if(result == COMPLETED_SAFE)
	{
		printf("Computation completed: %ld flowpipe(s) computed.\n", continuousProblem.numOfFlowpipes());
	}
	else
	{
		printf(BOLD_FONT RED_COLOR "Computation not completed: %ld flowpipe(s) computed.\n" RESET_COLOR, continuousProblem.numOfFlowpipes());
		printf(BOLD_FONT "Please try smaller step sizes or larger Taylor model orders.\n" RESET_COLOR);
	}

	printf("Total time cost:" BOLD_FONT " %lf" RESET_COLOR " seconds.\n\n", (double)(end - begin) / CLOCKS_PER_SEC);

	continuousProblem.bSafetyChecking = false;

	if(continuousProblem.bPlot)
	{
		if(continuousProblem.bDump)
		{
			printf("Preparing for plotting and dumping...\n");
			continuousProblem.prepareForDumping();
			printf("Done.\n");

			continuousProblem.plot_2D(false);

			char filename[NAME_SIZE+10];
			sprintf(filename, "%s%s.flow", outputDir, continuousProblem.outputFileName);
			FILE *fpDumping = fopen(filename, "w");

			if(fpDumping == NULL)
			{
				printf("Can not create the dumping file.\n");
				exit(1);
			}

			printf("Writing the flowpipe(s)...\n");
			continuousProblem.dump(fpDumping);
			printf("Done.\n");

			fclose(fpDumping);
		}
		else
		{
			printf("Preparing for plotting...\n");
			continuousProblem.prepareForPlotting();
			printf("Done.\n");

			continuousProblem.plot_2D(true);
		}
	}
	else
	{
		if(continuousProblem.bDump)
		{
			printf("Preparing for dumping...\n");
			continuousProblem.prepareForDumping();
			printf("Done.\n");

			char filename[NAME_SIZE+10];
			sprintf(filename, "%s%s.flow", outputDir, continuousProblem.outputFileName);
			FILE *fpDumping = fopen(filename, "w");

			if(fpDumping == NULL)
			{
				printf("Can not create the dumping file.\n");
				exit(1);
			}

			printf("Writing the flowpipe(s)...\n");
			continuousProblem.dump(fpDumping);
			printf("Done.\n");

			fclose(fpDumping);
		}
	}
}
#line 2402 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 275 "modelParser.y" /* yacc.c:1646  */
    {
	int mkres = mkdir(outputDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if(mkres < 0 && errno != EEXIST)
	{
		printf("Can not create the directory for output files.\n");
		exit(1);
	}

	mkres = mkdir(imageDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if(mkres < 0 && errno != EEXIST)
	{
		printf("Can not create the directory for images.\n");
		exit(1);
	}

	continuousProblem.bSafetyChecking = true;

	clock_t begin, end;
	begin = clock();
	int result = continuousProblem.run();
	end = clock();

	printf("\n");

	switch(result)
	{
	case COMPLETED_UNSAFE:
		printf("Computation completed: %ld flowpipe(s) computed.\n\n", continuousProblem.numOfFlowpipes());
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT RED_COLOR "UNSAFE\n\n" RESET_COLOR);
		break;
	case COMPLETED_SAFE:
		printf("Computation completed: %ld flowpipe(s) computed.\n\n", continuousProblem.numOfFlowpipes());
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT GREEN_COLOR "SAFE\n\n" RESET_COLOR);
		break;
	case COMPLETED_UNKNOWN:
		printf("Computation completed: %ld flowpipe(s) computed.\n\n", continuousProblem.numOfFlowpipes());
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT BLUE_COLOR "UNKNOWN\n\n" RESET_COLOR);
		break;
	case UNCOMPLETED_UNSAFE:
		printf(BOLD_FONT RED_COLOR "Computation not completed: %ld flowpipe(s) computed.\n" RESET_COLOR, continuousProblem.numOfFlowpipes());
		printf(BOLD_FONT "Please try smaller step sizes or larger Taylor model orders.\n\n" RESET_COLOR);
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT RED_COLOR "UNSAFE\n\n" RESET_COLOR);
		break;
	case UNCOMPLETED_SAFE:
		printf(BOLD_FONT RED_COLOR "Computation not completed: %ld flowpipe(s) computed.\n" RESET_COLOR, continuousProblem.numOfFlowpipes());
		printf(BOLD_FONT "Please try smaller step sizes or larger Taylor model orders.\n\n" RESET_COLOR);
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT GREEN_COLOR "SAFE\n\n" RESET_COLOR);
		break;
	case UNCOMPLETED_UNKNOWN:
		printf(BOLD_FONT RED_COLOR "Computation not completed: %ld flowpipe(s) computed.\n" RESET_COLOR, continuousProblem.numOfFlowpipes());
		printf(BOLD_FONT "Please try smaller step sizes or larger Taylor model orders.\n\n" RESET_COLOR);
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT BLUE_COLOR "UNKNOWN\n\n" RESET_COLOR);
		break;
	}

	printf("Total time cost:" BOLD_FONT " %lf" RESET_COLOR " seconds.\n\n", (double)(end - begin) / CLOCKS_PER_SEC);

	if(continuousProblem.bPlot)
	{
		if(continuousProblem.bDump)
		{
			printf("Preparing for plotting and dumping...\n");
			continuousProblem.prepareForDumping();
			printf("Done.\n");

			continuousProblem.plot_2D(false);

			char filename[NAME_SIZE+10];
			sprintf(filename, "%s%s.flow", outputDir, continuousProblem.outputFileName);
			FILE *fpDumping = fopen(filename, "w");

			if(fpDumping == NULL)
			{
				printf("Can not create the dumping file.\n");
				exit(1);
			}

			printf("Writing the flowpipe(s)...\n");
			continuousProblem.dump(fpDumping);
			printf("Done.\n");

			fclose(fpDumping);
		}
		else
		{
			printf("Preparing for plotting...\n");
			continuousProblem.prepareForPlotting();
			printf("Done.\n");

			continuousProblem.plot_2D(true);
		}
	}
	else
	{
		if(continuousProblem.bDump)
		{
			printf("Preparing for dumping...\n");
			continuousProblem.prepareForDumping();
			printf("Done.\n");

			char filename[NAME_SIZE+10];
			sprintf(filename, "%s%s.flow", outputDir, continuousProblem.outputFileName);
			FILE *fpDumping = fopen(filename, "w");

			if(fpDumping == NULL)
			{
				printf("Can not create the dumping file.\n");
				exit(1);
			}

			printf("Writing the flowpipe(s)...\n");
			continuousProblem.dump(fpDumping);
			printf("Done.\n");

			fclose(fpDumping);
		}
	}
}
#line 2531 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 401 "modelParser.y" /* yacc.c:1646  */
    {
	int mkres = mkdir(outputDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if(mkres < 0 && errno != EEXIST)
	{
		printf("Can not create the directory for output files.\n");
		exit(1);
	}

	mkres = mkdir(imageDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if(mkres < 0 && errno != EEXIST)
	{
		printf("Can not create the directory for images.\n");
		exit(1);
	}

	int result;

	clock_t begin, end;
	begin = clock();
	result = hybridProblem.run();
	end = clock();

	printf("\n");

	if(result == COMPLETED_SAFE)
	{
		printf("Computation completed: %ld flowpipe(s) computed.\n", hybridProblem.numOfFlowpipes());
	}
	else
	{
		printf(BOLD_FONT RED_COLOR "Computation not completed: %ld flowpipe(s) computed.\n" RESET_COLOR, hybridProblem.numOfFlowpipes());
		printf(BOLD_FONT "Please try smaller step sizes or larger Taylor model orders.\n" RESET_COLOR);
	}

	printf("Time cost of flowpipe construction:" BOLD_FONT " %lf" RESET_COLOR " seconds.\n\n", (double)(end - begin) / CLOCKS_PER_SEC);

	hybridProblem.bSafetyChecking = false;

	if(hybridProblem.bPlot)
	{
		if(hybridProblem.bDump)
		{
			printf("Preparing for plotting and dumping...\n");
			hybridProblem.prepareForDumping();
			printf("Done.\n");

			hybridProblem.plot_2D(false);

			char filename[NAME_SIZE+10];
			sprintf(filename, "%s%s.flow", outputDir, hybridProblem.outputFileName);
			FILE *fpDumping = fopen(filename, "w");

			if(fpDumping == NULL)
			{
				printf("Can not create the dumping file.\n");
				exit(1);
			}

			printf("Writing the flowpipe(s)...\n");
			hybridProblem.dump(fpDumping);
			printf("Done.\n");

			fclose(fpDumping);
		}
		else
		{
			printf("Preparing for plotting...\n");
			hybridProblem.prepareForPlotting();
			printf("Done.\n");

			hybridProblem.plot_2D(true);
		}
	}
	else
	{
		if(hybridProblem.bDump)
		{
			printf("Preparing for dumping...\n");
			hybridProblem.prepareForDumping();
			printf("Done.\n");

			char filename[NAME_SIZE+10];
			sprintf(filename, "%s%s.flow", outputDir, hybridProblem.outputFileName);
			FILE *fpDumping = fopen(filename, "w");

			if(fpDumping == NULL)
			{
				printf("Can not create the dumping file.\n");
				exit(1);
			}

			printf("Writing the flowpipe(s)...\n");
			hybridProblem.dump(fpDumping);
			printf("Done.\n");

			fclose(fpDumping);
		}
	}
}
#line 2635 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 502 "modelParser.y" /* yacc.c:1646  */
    {
	int mkres = mkdir(outputDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if(mkres < 0 && errno != EEXIST)
	{
		printf("Can not create the directory for output files.\n");
		exit(1);
	}

	mkres = mkdir(imageDir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	if(mkres < 0 && errno != EEXIST)
	{
		printf("Can not create the directory for images.\n");
		exit(1);
	}

	hybridProblem.bSafetyChecking = true;

	int result;

	clock_t begin, end;
	begin = clock();
	result = hybridProblem.run();
	end = clock();

	printf("\n");

	switch(result)
	{
	case COMPLETED_UNSAFE:
		printf("Computation completed: %ld flowpipe(s) computed.\n\n", hybridProblem.numOfFlowpipes());
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT RED_COLOR "UNSAFE\n\n" RESET_COLOR);
		break;
	case COMPLETED_SAFE:
		printf("Computation completed: %ld flowpipe(s) computed.\n\n", hybridProblem.numOfFlowpipes());
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT GREEN_COLOR "SAFE\n\n" RESET_COLOR);
		break;
	case COMPLETED_UNKNOWN:
		printf("Computation completed: %ld flowpipe(s) computed.\n\n", hybridProblem.numOfFlowpipes());
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT BLUE_COLOR "UNKNOWN\n\n" RESET_COLOR);
		break;
	case UNCOMPLETED_UNSAFE:
		printf(BOLD_FONT RED_COLOR "Computation not completed: %ld flowpipe(s) computed.\n" RESET_COLOR, hybridProblem.numOfFlowpipes());
		printf(BOLD_FONT "Please try smaller step sizes or larger Taylor model orders.\n\n" RESET_COLOR);
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT RED_COLOR "UNSAFE\n\n" RESET_COLOR);
		break;
	case UNCOMPLETED_SAFE:
		printf(BOLD_FONT RED_COLOR "Computation not completed: %ld flowpipe(s) computed.\n" RESET_COLOR, hybridProblem.numOfFlowpipes());
		printf(BOLD_FONT "Please try smaller step sizes or larger Taylor model orders.\n\n" RESET_COLOR);
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT GREEN_COLOR "SAFE\n\n" RESET_COLOR);
		break;
	case UNCOMPLETED_UNKNOWN:
		printf(BOLD_FONT RED_COLOR "Computation not completed: %ld flowpipe(s) computed.\n" RESET_COLOR, hybridProblem.numOfFlowpipes());
		printf(BOLD_FONT "Please try smaller step sizes or larger Taylor model orders.\n\n" RESET_COLOR);
		printf(BOLD_FONT "Result of the safety verification on the computed flowpipes: " RESET_COLOR);
		printf(BOLD_FONT BLUE_COLOR "UNKNOWN\n\n" RESET_COLOR);
		break;
	}

	printf("Total time cost:" BOLD_FONT " %lf" RESET_COLOR " seconds.\n\n", (double)(end - begin) / CLOCKS_PER_SEC);

	if(hybridProblem.bPlot)
	{
		if(hybridProblem.bDump)
		{
			printf("Preparing for plotting and dumping...\n");
			hybridProblem.prepareForDumping();
			printf("Done.\n");

			hybridProblem.plot_2D(false);

			char filename[NAME_SIZE+10];
			sprintf(filename, "%s%s.flow", outputDir, hybridProblem.outputFileName);
			FILE *fpDumping = fopen(filename, "w");

			if(fpDumping == NULL)
			{
				printf("Can not create the dumping file.\n");
				exit(1);
			}

			printf("Writing the flowpipe(s)...\n");
			hybridProblem.dump(fpDumping);
			printf("Done.\n");

			fclose(fpDumping);
		}
		else
		{
			printf("Preparing for plotting...\n");
			hybridProblem.prepareForPlotting();
			printf("Done.\n");

			hybridProblem.plot_2D(true);
		}
	}
	else
	{
		if(hybridProblem.bDump)
		{
			printf("Preparing for dumping...\n");
			hybridProblem.prepareForDumping();
			printf("Done.\n");

			char filename[NAME_SIZE+10];
			sprintf(filename, "%s%s.flow", outputDir, hybridProblem.outputFileName);
			FILE *fpDumping = fopen(filename, "w");

			if(fpDumping == NULL)
			{
				printf("Can not create the dumping file.\n");
				exit(1);
			}

			printf("Writing the flowpipe(s)...\n");
			hybridProblem.dump(fpDumping);
			printf("Done.\n");

			fclose(fpDumping);
		}
	}
}
#line 2766 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 630 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-9].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	continuousProblem.globalMaxOrder = (int)(yyvsp[-9].dblVal);

	flowstar::Interval I(-(yyvsp[-7].dblVal),(yyvsp[-7].dblVal));
	continuousProblem.cutoff_threshold = I;

	clock_t begin, end;
	int checkingResult;
	printf("Safety checking ...\n");
	begin = clock();
	checkingResult = continuousProblem.safetyChecking();
	end = clock();
	printf("Done.\n");
	printf("Time cost of safety verification:" BOLD_FONT " %lf" RESET_COLOR " seconds.\n\n", (double)(end - begin) / CLOCKS_PER_SEC);
	printf(BOLD_FONT "Result of safety verification: " RESET_COLOR);

	switch(checkingResult)
	{
	case UNSAFE:
		printf(BOLD_FONT RED_COLOR "UNSAFE\n\n" RESET_COLOR);
		break;
	case SAFE:
		printf(BOLD_FONT GREEN_COLOR "SAFE\n\n" RESET_COLOR);
		break;
	case UNKNOWN:
		printf(BOLD_FONT BLUE_COLOR "UNKNOWN\n\n" RESET_COLOR);
		break;
	}

	if(continuousProblem.bPlot)
	{
		continuousProblem.plot_2D(false);
	}
}
#line 2811 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 672 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-6].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	continuousProblem.globalMaxOrder = (int)(yyvsp[-8].dblVal);

	flowstar::Interval I(-(yyvsp[-6].dblVal),(yyvsp[-6].dblVal));
	continuousProblem.cutoff_threshold = I;

	if(continuousProblem.bPlot)
	{
		continuousProblem.plot_2D(false);
	}
}
#line 2833 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 691 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-13].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	continuousProblem.step = (yyvsp[-17].dblVal);
	continuousProblem.globalMaxOrder = (int)(yyvsp[-15].dblVal);

	flowstar::Interval I(-(yyvsp[-13].dblVal),(yyvsp[-13].dblVal));
	continuousProblem.cutoff_threshold = I;

	if(continuousProblem.bPlot)
	{
		continuousProblem.plot_2D(false);
	}
}
#line 2856 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 711 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-14].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	continuousProblem.step = (yyvsp[-18].dblVal);
	continuousProblem.globalMaxOrder = (int)(yyvsp[-16].dblVal);

	flowstar::Interval I(-(yyvsp[-14].dblVal),(yyvsp[-14].dblVal));
	continuousProblem.cutoff_threshold = I;

	clock_t begin, end;
	int checkingResult;
	printf("Safety checking ...\n");
	begin = clock();
	checkingResult = continuousProblem.safetyChecking();
	end = clock();
	printf("Done.\n");
	printf("Time cost of safety verification:" BOLD_FONT " %lf" RESET_COLOR " seconds.\n\n", (double)(end - begin) / CLOCKS_PER_SEC);
	printf(BOLD_FONT "Result of safety verification: " RESET_COLOR);

	switch(checkingResult)
	{
	case UNSAFE:
		printf(BOLD_FONT RED_COLOR "UNSAFE\n\n" RESET_COLOR);
		break;
	case SAFE:
		printf(BOLD_FONT GREEN_COLOR "SAFE\n\n" RESET_COLOR);
		break;
	case UNKNOWN:
		printf(BOLD_FONT BLUE_COLOR "UNKNOWN\n\n" RESET_COLOR);
		break;
	}

	if(continuousProblem.bPlot)
	{
		continuousProblem.plot_2D(false);
	}
}
#line 2902 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 754 "modelParser.y" /* yacc.c:1646  */
    {
	clock_t begin, end;

	generateNodeSeq(hybridProblem.traceNodes, hybridProblem.traceTree);

	int checkingResult;
	printf("Safety checking ...\n");
	begin = clock();
	checkingResult = hybridProblem.safetyChecking();
	end = clock();
	printf("Done.\n");
	printf("Time cost of safety verification:" BOLD_FONT " %lf" RESET_COLOR " seconds.\n\n", (double)(end - begin) / CLOCKS_PER_SEC);
	printf(BOLD_FONT "Result of safety verification: " RESET_COLOR);

	switch(checkingResult)
	{
	case UNSAFE:
		printf(BOLD_FONT RED_COLOR "UNSAFE\n\n" RESET_COLOR);
		break;
	case SAFE:
		printf(BOLD_FONT GREEN_COLOR "SAFE\n\n" RESET_COLOR);
		break;
	case UNKNOWN:
		printf(BOLD_FONT BLUE_COLOR "UNKNOWN\n\n" RESET_COLOR);
		break;
	}

	if(hybridProblem.bPlot)
	{
		hybridProblem.plot_2D(false);
	}
}
#line 2939 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 788 "modelParser.y" /* yacc.c:1646  */
    {
	if(hybridProblem.bPlot)
	{
		hybridProblem.plot_2D(false);
	}
}
#line 2950 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 796 "modelParser.y" /* yacc.c:1646  */
    {
	(yyvsp[-1].ptm)->getExpansion(parseResult.expansion);
	parseResult.remainder = (yyvsp[-1].ptm)->getRemainder();
	delete (yyvsp[-1].ptm);
}
#line 2960 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 803 "modelParser.y" /* yacc.c:1646  */
    {
	parseResult.remainder = (*(yyvsp[-1].pint));
	delete (yyvsp[-1].pint);
}
#line 2969 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 809 "modelParser.y" /* yacc.c:1646  */
    {
	parseResult.expansion = (*(yyvsp[-1].poly));
	delete (yyvsp[-1].poly);
}
#line 2978 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 815 "modelParser.y" /* yacc.c:1646  */
    {
	parseResult.strExpansion = (yyvsp[-1].p_ODE_String)->ode;
	parseResult.constant = (yyvsp[-1].p_ODE_String)->constant;
	delete (yyvsp[-1].p_ODE_String);
}
#line 2988 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 822 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::up_parseresult = (*(yyvsp[-1].pUpoly));
	delete (yyvsp[-1].pUpoly);
}
#line 2997 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 828 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::parsePolynomial.result = (*(yyvsp[-1].poly));
	delete (yyvsp[-1].poly);
}
#line 3006 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 834 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::parseExpression.expression = (*(yyvsp[-1].pExpression));
	delete (yyvsp[-1].pExpression);
}
#line 3015 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 840 "modelParser.y" /* yacc.c:1646  */
    {
	int rows = (yyvsp[-1].piMatrix)->size();
	int cols = 0;

	if(rows > 0)
	{
		cols = (*(yyvsp[-1].piMatrix))[0].size();

		flowstar::iMatrix matrix(rows, cols);

		for(int i=0; i<rows; ++i)
		{
			for(int j=0; j<cols; ++j)
			{
				matrix[i][j] = (*(yyvsp[-1].piMatrix))[i][j];
			}
		}

		flowstar::matrixParseSetting.result = matrix;
	}
	else
	{
		flowstar::iMatrix matrix;
		flowstar::matrixParseSetting.result = matrix;
	}

	delete (yyvsp[-1].piMatrix);
}
#line 3048 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 871 "modelParser.y" /* yacc.c:1646  */
    {
	strcpy(continuousProblem.outputFileName, (yyvsp[0].identifier)->c_str());
	strcpy(hybridProblem.outputFileName, (yyvsp[0].identifier)->c_str());
	continuousProblem.bDump = true;
	continuousProblem.bPlot = true;
	hybridProblem.bDump = true;
	hybridProblem.bPlot = true;
}
#line 3061 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 881 "modelParser.y" /* yacc.c:1646  */
    {
	strcpy(continuousProblem.outputFileName, (yyvsp[0].identifier)->c_str());
	strcpy(hybridProblem.outputFileName, (yyvsp[0].identifier)->c_str());
	continuousProblem.bDump = false;
	continuousProblem.bPlot = true;
	hybridProblem.bDump = false;
	hybridProblem.bPlot = true;
}
#line 3074 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 891 "modelParser.y" /* yacc.c:1646  */
    {
	strcpy(continuousProblem.outputFileName, (yyvsp[0].identifier)->c_str());
	strcpy(hybridProblem.outputFileName, (yyvsp[0].identifier)->c_str());
	continuousProblem.bDump = true;
	continuousProblem.bPlot = false;
	hybridProblem.bDump = true;
	hybridProblem.bPlot = false;
}
#line 3087 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 901 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.bDump = false;
	continuousProblem.bPlot = false;
	hybridProblem.bDump = false;
	hybridProblem.bPlot = false;
}
#line 3098 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 910 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 3105 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 915 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 3112 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 920 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.intVal) = 1;
}
#line 3120 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 925 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.intVal) = 0;
}
#line 3128 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 929 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.intVal) = 0;
}
#line 3136 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 936 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::domainVarNames = varlist.varNames; 
}
#line 3144 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 942 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 3151 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 947 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.flowpipesCompo.push_back(*(yyvsp[-3].tmVec));
	continuousProblem.domains.push_back(*(yyvsp[-2].intVec));
	continuousProblem.flowpipes_safety.push_back(SAFE);

	if((yyvsp[-1].intVal) == 1)
	{
		continuousProblem.flowpipes_contracted.push_back(true);
	}
	else
	{
		continuousProblem.flowpipes_contracted.push_back(false);
	}

	delete (yyvsp[-3].tmVec);
	delete (yyvsp[-2].intVec);
}
#line 3173 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 966 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.flowpipesCompo.push_back(*(yyvsp[-3].tmVec));
	continuousProblem.domains.push_back(*(yyvsp[-2].intVec));
	continuousProblem.flowpipes_safety.push_back(SAFE);

	if((yyvsp[-1].intVal) == 1)
	{
		continuousProblem.flowpipes_contracted.push_back(true);
	}
	else
	{
		continuousProblem.flowpipes_contracted.push_back(false);
	}

	delete (yyvsp[-3].tmVec);
	delete (yyvsp[-2].intVec);
}
#line 3195 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 987 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Flowpipe initialSet;

	initialSet.tmvPre = *(yyvsp[-2].tmVec);
	initialSet.domain = *(yyvsp[-1].intVec);

	continuousProblem.system.initialSets.push_back(initialSet);

	delete (yyvsp[-2].tmVec);
	delete (yyvsp[-1].intVec);
}
#line 3211 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 1000 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Flowpipe initialSet;

	initialSet.tmvPre = *(yyvsp[-2].tmVec);
	initialSet.domain = *(yyvsp[-1].intVec);

	continuousProblem.system.initialSets.push_back(initialSet);

	delete (yyvsp[-2].tmVec);
	delete (yyvsp[-1].intVec);
}
#line 3227 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 1014 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.flowpipesCompo.push_back(*(yyvsp[-1].tmVec));
	int num = continuousProblem.system.initialSets.size();

	for(int i=0; i<num; ++i)
	{
		continuousProblem.flowpipes_safety.push_back(SAFE);
	}

	delete (yyvsp[-1].tmVec);
}
#line 3243 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 1027 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.flowpipesCompo.push_back(*(yyvsp[-1].tmVec));

	int num = continuousProblem.system.initialSets.size();

	for(int i=0; i<num; ++i)
	{
		continuousProblem.flowpipes_safety.push_back(SAFE);
	}

	delete (yyvsp[-1].tmVec);
}
#line 3260 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 1043 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::TaylorModelVec tmvDummy;

	if((yyvsp[-9].dblVal) <= 0)
	{
		parseError("The order should be a positive number.", lineNum);
		exit(1);
	}

	if((yyvsp[-5].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval I(-(yyvsp[-5].dblVal),(yyvsp[-5].dblVal));
	mode_local_setting.cutoff_threshold = I;
	mode_local_setting.globalMaxOrder = (yyvsp[-9].dblVal);
	
	hybridProblem.declareMode(*(yyvsp[-13].identifier), tmvDummy, *(yyvsp[-2].vecConstraints), 0, mode_local_setting);

	delete (yyvsp[-13].identifier);
	delete (yyvsp[-2].vecConstraints);
}
#line 3289 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 1069 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::TaylorModelVec tmvDummy;

	if((yyvsp[-9].dblVal) <= 0)
	{
		parseError("The order should be a positive number.", lineNum);
		exit(1);
	}

	if((yyvsp[-5].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval I(-(yyvsp[-5].dblVal),(yyvsp[-5].dblVal));
	mode_local_setting.cutoff_threshold = I;
	mode_local_setting.globalMaxOrder = (yyvsp[-9].dblVal);

	hybridProblem.declareMode(*(yyvsp[-13].identifier), tmvDummy, *(yyvsp[-2].vecConstraints), 0, mode_local_setting);

	delete (yyvsp[-13].identifier);
	delete (yyvsp[-2].vecConstraints);
}
#line 3318 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 1096 "modelParser.y" /* yacc.c:1646  */
    {
	int id = hybridProblem.getIDForMode(*(yyvsp[-4].identifier));
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*(yyvsp[-4].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.modeIDs.push_back(id);
	hybridProblem.flowpipes.push_back(continuousProblem.flowpipesCompo);
	hybridProblem.domains.push_back(continuousProblem.domains);
	hybridProblem.flowpipes_safety.push_back(continuousProblem.flowpipes_safety);
	hybridProblem.flowpipes_contracted.push_back(continuousProblem.flowpipes_contracted);

	continuousProblem.flowpipesCompo.clear();
	continuousProblem.domains.clear();
	continuousProblem.flowpipes_contracted.clear();
	continuousProblem.flowpipes_safety.clear();
	continuousProblem.tmVarTab.clear();
	continuousProblem.tmVarNames.clear();
}
#line 3346 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 1121 "modelParser.y" /* yacc.c:1646  */
    {
	int id = hybridProblem.getIDForMode(*(yyvsp[-4].identifier));
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*(yyvsp[-4].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.modeIDs.push_back(id);
	hybridProblem.flowpipes.push_back(continuousProblem.flowpipesCompo);
	hybridProblem.domains.push_back(continuousProblem.domains);
	hybridProblem.flowpipes_safety.push_back(continuousProblem.flowpipes_safety);
	hybridProblem.flowpipes_contracted.push_back(continuousProblem.flowpipes_contracted);

	continuousProblem.flowpipesCompo.clear();
	continuousProblem.domains.clear();
	continuousProblem.flowpipes_contracted.clear();
	continuousProblem.flowpipes_safety.clear();
	continuousProblem.tmVarTab.clear();
	continuousProblem.tmVarNames.clear();
}
#line 3374 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 1147 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 3381 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 1151 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 3388 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 1156 "modelParser.y" /* yacc.c:1646  */
    {
	int id = hybridProblem.getIDForMode(*(yyvsp[0].identifier));
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	std::list<flowstar::TreeNode *>::iterator iter = (yyval.pNode)->children.begin();
	bool found = false;
	for(; iter!=(yyval.pNode)->children.end(); ++iter)
	{
		if((*iter)->jumpID == (yyvsp[-10].dblVal) && (*iter)->modeID == id)
		{
			(yyval.pNode) = *iter;
			found = true;
			break;
		}
	}

	if(!found)
	{
		flowstar::Interval I((yyvsp[-7].dblVal), (yyvsp[-5].dblVal));
		flowstar::TreeNode *tmp = new flowstar::TreeNode((int)(yyvsp[-10].dblVal), id, I);
		tmp->parent = (yyval.pNode);
		(yyval.pNode)->children.push_back(tmp);
		(yyval.pNode) = tmp;
	}

	delete (yyvsp[0].identifier);
}
#line 3426 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 1191 "modelParser.y" /* yacc.c:1646  */
    {
	int id = hybridProblem.getIDForMode(*(yyvsp[0].identifier));
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if(hybridProblem.traceTree == NULL)
	{
		flowstar::Interval intZero;
		hybridProblem.traceTree = new flowstar::TreeNode(0, id, intZero);
		(yyval.pNode) = hybridProblem.traceTree;
	}
	else
	{
		if(hybridProblem.traceTree->modeID == id)
		{
			(yyval.pNode) = hybridProblem.traceTree;
		}
		else
		{
			parseError("Invalid computation path.", lineNum);
			exit(1);
		}
	}

	delete (yyvsp[0].identifier);
}
#line 3462 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 1225 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.bPrint = true;
	hybridProblem.bPrint = true;
}
#line 3471 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 1231 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.bPrint = false;
	hybridProblem.bPrint = false;
}
#line 3480 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 1238 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.unsafeSet = *(yyvsp[-1].vecConstraints);
	delete (yyvsp[-1].vecConstraints);
}
#line 3489 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 1245 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 3496 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 1250 "modelParser.y" /* yacc.c:1646  */
    {
	int id = hybridProblem.getIDForMode(*(yyvsp[-3].identifier));
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*(yyvsp[-3].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.unsafeSet[id] = *(yyvsp[-1].vecConstraints);
	hybridProblem.bVecUnderCheck[id] = true;
	delete (yyvsp[-1].vecConstraints);
}
#line 3515 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 1265 "modelParser.y" /* yacc.c:1646  */
    {
	std::vector<flowstar::PolynomialConstraint> vecEmpty;
	for(int i=0; i<hybridProblem.modeNames.size(); ++i)
	{
		hybridProblem.unsafeSet.push_back(vecEmpty);
		hybridProblem.bVecUnderCheck.push_back(false);
	}
}
#line 3528 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 1276 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-2].poly)->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	(yyval.vecConstraints) = (yyvsp[-3].vecConstraints);
	flowstar::Interval B((yyvsp[0].dblVal));
	flowstar::PolynomialConstraint pc(*(yyvsp[-2].poly), B);
	(yyval.vecConstraints)->push_back(pc);

	delete (yyvsp[-2].poly);
}
#line 3547 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 1292 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-2].poly)->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	(yyval.vecConstraints) = (yyvsp[-3].vecConstraints);
	flowstar::Interval I(-1);
	(yyvsp[-2].poly)->mul_assign(I);

	flowstar::Interval B(-(yyvsp[0].dblVal));
	flowstar::PolynomialConstraint pc(*(yyvsp[-2].poly), B);
	(yyval.vecConstraints)->push_back(pc);

	delete (yyvsp[-2].poly);
}
#line 3569 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 1311 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-2].poly)->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	(yyval.vecConstraints) = (yyvsp[-3].vecConstraints);
	flowstar::Interval B((yyvsp[0].dblVal));
	flowstar::PolynomialConstraint pc1(*(yyvsp[-2].poly), B);
	(yyval.vecConstraints)->push_back(pc1);

	flowstar::Interval I(-1);
	(yyvsp[-2].poly)->mul_assign(I);
	flowstar::Interval mB(-(yyvsp[0].dblVal));
	flowstar::PolynomialConstraint pc2(*(yyvsp[-2].poly), mB);
	(yyval.vecConstraints)->push_back(pc2);

	delete (yyvsp[-2].poly);
}
#line 3594 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 1333 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-6].poly)->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	flowstar::PolynomialConstraint pc1(*(yyvsp[-6].poly), (yyvsp[-1].dblVal));
	(yyval.vecConstraints)->push_back(pc1);

	flowstar::Interval I(-1);
	(yyvsp[-6].poly)->mul_assign(I);
	flowstar::PolynomialConstraint pc2(*(yyvsp[-6].poly), -(yyvsp[-3].dblVal));
	(yyval.vecConstraints)->push_back(pc2);

	delete (yyvsp[-6].poly);
}
#line 3616 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 1352 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-2].poly)->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	int id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(yyval.vecConstraints) = (yyvsp[-3].vecConstraints);
	flowstar::PolynomialConstraint pc(*(yyvsp[-2].poly), range);
	(yyval.vecConstraints)->push_back(pc);

	delete (yyvsp[-2].poly);
	delete (yyvsp[0].identifier);
}
#line 3650 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 1383 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-2].poly)->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	int id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(yyval.vecConstraints) = (yyvsp[-3].vecConstraints);
	flowstar::Interval I(-1);
	(yyvsp[-2].poly)->mul_assign(I);
	range *= I;
	flowstar::PolynomialConstraint pc(*(yyvsp[-2].poly), range);
	(yyval.vecConstraints)->push_back(pc);

	delete (yyvsp[-2].poly);
	delete (yyvsp[0].identifier);
}
#line 3687 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 1417 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-2].poly)->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	int id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(yyval.vecConstraints) = (yyvsp[-3].vecConstraints);
	flowstar::PolynomialConstraint pc1(*(yyvsp[-2].poly), range);
	(yyval.vecConstraints)->push_back(pc1);

	flowstar::Interval I(-1);
	(yyvsp[-2].poly)->mul_assign(I);
	range *= I;
	flowstar::PolynomialConstraint pc2(*(yyvsp[-2].poly), range);
	(yyval.vecConstraints)->push_back(pc2);

	delete (yyvsp[-2].poly);
	delete (yyvsp[0].identifier);
}
#line 3727 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 1454 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-6].poly)->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	int id = continuousProblem.getIDForPar(*(yyvsp[-1].identifier));
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *(yyvsp[-1].identifier));
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*(yyvsp[-1].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::PolynomialConstraint pc1(*(yyvsp[-6].poly), range);
	(yyval.vecConstraints)->push_back(pc1);

	id = continuousProblem.getIDForPar(*(yyvsp[-3].identifier));

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *(yyvsp[-3].identifier));
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*(yyvsp[-3].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Interval I(-1);
	(yyvsp[-6].poly)->mul_assign(I);
	range *= I;
	flowstar::PolynomialConstraint pc2(*(yyvsp[-6].poly), range);
	(yyval.vecConstraints)->push_back(pc2);

	delete (yyvsp[-6].poly);
	delete (yyvsp[-3].identifier);
	delete (yyvsp[-1].identifier);
}
#line 3781 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 1504 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.vecConstraints) = new std::vector<flowstar::PolynomialConstraint>(0);
}
#line 3789 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 1748 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD;

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].tmVec);
}
#line 3814 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 1770 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LOW_DEGREE;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LOW_DEGREE;

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].tmVec);
}
#line 3839 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 1792 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = HIGH_DEGREE;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = HIGH_DEGREE;

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].tmVec);
}
#line 3864 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 1814 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].strVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].strVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR;

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].strVec);
}
#line 3889 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 1836 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(dyn_A, dyn_B, dyn_ti, dyn_tv, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTI;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(dyn_A, dyn_B, dyn_ti, dyn_tv, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTI;

		delete (yyvsp[-1].pFlowpipe);
	}
}
#line 3912 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 1856 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(dyn_A_t, dyn_B_t, dyn_ti_t, dyn_tv_t, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTV;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(dyn_A_t, dyn_B_t, dyn_ti_t, dyn_tv_t, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTV;

		delete (yyvsp[-1].pFlowpipe);
	}
}
#line 3935 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 1876 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD_SYMB;
		continuousProblem.max_remainder_queue = (yyvsp[-8].dblVal);
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD_SYMB;
		continuousProblem.max_remainder_queue = (yyvsp[-8].dblVal);

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].tmVec);
}
#line 3962 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 1900 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].strVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR_SYMB;
		continuousProblem.max_remainder_queue = (yyvsp[-8].dblVal);
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].strVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR_SYMB;
		continuousProblem.max_remainder_queue = (yyvsp[-8].dblVal);

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].strVec);
}
#line 3989 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 1924 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD;

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].tmVec);
}
#line 4014 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 1946 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LOW_DEGREE;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LOW_DEGREE;

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].tmVec);
}
#line 4039 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 1968 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = HIGH_DEGREE;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = HIGH_DEGREE;

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].tmVec);
}
#line 4064 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 1990 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].strVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].strVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR;

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].strVec);
}
#line 4089 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 2012 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(dyn_A, dyn_B, dyn_ti, dyn_tv, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTI;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(dyn_A, dyn_B, dyn_ti, dyn_tv, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTI;

		delete (yyvsp[-1].pFlowpipe);
	}
}
#line 4112 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 2032 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(dyn_A_t, dyn_B_t, dyn_ti_t, dyn_tv_t, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTV;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(dyn_A_t, dyn_B_t, dyn_ti_t, dyn_tv_t, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTV;

		delete (yyvsp[-1].pFlowpipe);
	}
}
#line 4135 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 2052 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD_SYMB;
		continuousProblem.max_remainder_queue = (yyvsp[-8].dblVal);
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].tmVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD_SYMB;
		continuousProblem.max_remainder_queue = (yyvsp[-8].dblVal);

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].tmVec);
}
#line 4162 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 2076 "modelParser.y" /* yacc.c:1646  */
    {
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*(yyvsp[-5].strVec), initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR_SYMB;
		continuousProblem.max_remainder_queue = (yyvsp[-8].dblVal);
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*(yyvsp[-1].pFlowpipe));
		flowstar::ContinuousSystem system(*(yyvsp[-5].strVec), vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR_SYMB;
		continuousProblem.max_remainder_queue = (yyvsp[-8].dblVal);

		delete (yyvsp[-1].pFlowpipe);
	}

	delete (yyvsp[-5].strVec);
}
#line 4189 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 2101 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-14].dblVal) < 0)
	{
		parseError("The maximum jump depth should be a nonnegative integer.", lineNum);
		exit(1);
	}

	hybridProblem.maxJumps = (int)(yyvsp[-14].dblVal);
}
#line 4203 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 2112 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-14].dblVal) < 0)
	{
		parseError("The maximum jump depth should be a nonnegative integer.", lineNum);
		exit(1);
	}

	hybridProblem.maxJumps = (int)(yyvsp[-14].dblVal);
}
#line 4217 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 2124 "modelParser.y" /* yacc.c:1646  */
    {
	int id = hybridProblem.getIDForMode(*(yyvsp[-3].identifier));
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*(yyvsp[-3].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Interval intZero;
	flowstar::Flowpipe initialSet(*(yyvsp[-1].intVec), intZero);
	hybridProblem.initialConfig(id, initialSet);

	int numVars = hybridProblem.stateVarNames.size();

	std::string tVar("local_t");
	hybridProblem.declareTMVar(tVar);
	continuousProblem.declareTMVar(tVar);

	char name[NAME_SIZE];

	for(int i=0; i<numVars; ++i)
	{
		sprintf(name, "%s%d", local_var_name, i+1);
		std::string tmVarName(name);
		hybridProblem.declareTMVar(tmVarName);
		continuousProblem.declareTMVar(tmVarName);
	}

	delete (yyvsp[-3].identifier);
	delete (yyvsp[-1].intVec);
}
#line 4255 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 2159 "modelParser.y" /* yacc.c:1646  */
    {
	int id = hybridProblem.getIDForMode(*(yyvsp[-5].identifier));
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*(yyvsp[-5].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Flowpipe initialSet(*(yyvsp[-2].tmVec), *(yyvsp[-1].intVec));
	hybridProblem.initialConfig(id, initialSet);

	delete (yyvsp[-2].tmVec);
	delete (yyvsp[-1].intVec);
}
#line 4276 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 2178 "modelParser.y" /* yacc.c:1646  */
    {
	hybridProblem.declareMode(*(yyvsp[-11].identifier), *(yyvsp[-6].tmVec), *(yyvsp[-2].vecConstraints), ONLY_PICARD, mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-6].tmVec);
	delete (yyvsp[-2].vecConstraints);
}
#line 4289 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 2188 "modelParser.y" /* yacc.c:1646  */
    {
	hybridProblem.declareMode(*(yyvsp[-11].identifier), *(yyvsp[-6].tmVec), *(yyvsp[-2].vecConstraints), ONLY_PICARD, mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-6].tmVec);
	delete (yyvsp[-2].vecConstraints);
}
#line 4302 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 2198 "modelParser.y" /* yacc.c:1646  */
    {
	hybridProblem.declareMode(*(yyvsp[-11].identifier), *(yyvsp[-6].tmVec), *(yyvsp[-2].vecConstraints), LOW_DEGREE, mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-6].tmVec);
	delete (yyvsp[-2].vecConstraints);
}
#line 4315 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 2208 "modelParser.y" /* yacc.c:1646  */
    {
	hybridProblem.declareMode(*(yyvsp[-11].identifier), *(yyvsp[-6].tmVec), *(yyvsp[-2].vecConstraints), LOW_DEGREE, mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-6].tmVec);
	delete (yyvsp[-2].vecConstraints);
}
#line 4328 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 2218 "modelParser.y" /* yacc.c:1646  */
    {
	hybridProblem.declareMode(*(yyvsp[-11].identifier), *(yyvsp[-6].tmVec), *(yyvsp[-2].vecConstraints), HIGH_DEGREE, mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-6].tmVec);
	delete (yyvsp[-2].vecConstraints);
}
#line 4341 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 2228 "modelParser.y" /* yacc.c:1646  */
    {
	hybridProblem.declareMode(*(yyvsp[-11].identifier), *(yyvsp[-6].tmVec), *(yyvsp[-2].vecConstraints), HIGH_DEGREE, mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-6].tmVec);
	delete (yyvsp[-2].vecConstraints);
}
#line 4354 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 2238 "modelParser.y" /* yacc.c:1646  */
    {
	hybridProblem.declareMode(*(yyvsp[-11].identifier), *(yyvsp[-6].strVec), *(yyvsp[-2].vecConstraints), NONPOLY_TAYLOR, mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-6].strVec);
	delete (yyvsp[-2].vecConstraints);
}
#line 4367 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 2248 "modelParser.y" /* yacc.c:1646  */
    {
	hybridProblem.declareMode(*(yyvsp[-11].identifier), *(yyvsp[-6].strVec), *(yyvsp[-2].vecConstraints), NONPOLY_TAYLOR, mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-6].strVec);
	delete (yyvsp[-2].vecConstraints);
}
#line 4380 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 2258 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::LTI_Dynamics lti_dyn(dyn_A, dyn_B);
	hybridProblem.declareMode(*(yyvsp[-11].identifier), lti_dyn, *(yyvsp[-2].vecConstraints), mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-2].vecConstraints);
}
#line 4393 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 2268 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::LTI_Dynamics lti_dyn(dyn_A, dyn_B);
	hybridProblem.declareMode(*(yyvsp[-11].identifier), lti_dyn, *(yyvsp[-2].vecConstraints), mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-2].vecConstraints);
}
#line 4406 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 2278 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::LTV_Dynamics ltv_dyn(dyn_A_t, dyn_B_t);
	hybridProblem.declareMode(*(yyvsp[-11].identifier), ltv_dyn, *(yyvsp[-2].vecConstraints), mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-2].vecConstraints);
}
#line 4419 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 2288 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::LTV_Dynamics ltv_dyn(dyn_A_t, dyn_B_t);
	hybridProblem.declareMode(*(yyvsp[-11].identifier), ltv_dyn, *(yyvsp[-2].vecConstraints), mode_local_setting);
	mode_local_setting.clear();

	delete (yyvsp[-11].identifier);
	delete (yyvsp[-2].vecConstraints);
}
#line 4432 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 2301 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 4439 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 2304 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 4446 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 2309 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].dblVal) <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	mode_local_setting.bAdaptiveSteps = false;
	mode_local_setting.step = (yyvsp[0].dblVal);
}
#line 4461 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 2321 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].dblVal) <= 0)
	{
		parseError("Remainder estimation should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval I(-(yyvsp[0].dblVal), (yyvsp[0].dblVal));

	for(int i=0; i<continuousProblem.stateVarNames.size(); ++i)
	{
		mode_local_setting.estimation.push_back(I);
	}
}
#line 4480 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 2337 "modelParser.y" /* yacc.c:1646  */
    {
	mode_local_setting.estimation = *(yyvsp[-1].intVec);
}
#line 4488 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 2342 "modelParser.y" /* yacc.c:1646  */
    {
	mode_local_setting.precondition = QR_PRE;
}
#line 4496 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 2347 "modelParser.y" /* yacc.c:1646  */
    {
	mode_local_setting.precondition = ID_PRE;
}
#line 4504 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 2352 "modelParser.y" /* yacc.c:1646  */
    {
	int order = (int)(yyvsp[0].dblVal);

	if(order <= 0)
	{
		parseError("Orders should be larger than zero.", lineNum);
		exit(1);
	}

	mode_local_setting.bAdaptiveOrders = false;
	mode_local_setting.orderType = UNIFORM;
	mode_local_setting.orders.push_back(order);
	mode_local_setting.globalMaxOrder = order;
}
#line 4523 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 2368 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval I(-(yyvsp[0].dblVal), (yyvsp[0].dblVal));
	mode_local_setting.cutoff_threshold = I;
}
#line 4538 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 2380 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-4].dblVal) <= 0 || (yyvsp[-1].dblVal) <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	if((yyvsp[-4].dblVal) > (yyvsp[-1].dblVal))
	{
		parseError("MIN step should be no larger than MAX step.", lineNum);
		exit(1);
	}

	mode_local_setting.bAdaptiveSteps = true;
	mode_local_setting.step = (yyvsp[-1].dblVal);
	mode_local_setting.miniStep = (yyvsp[-4].dblVal);
	mode_local_setting.bAdaptiveOrders = false;
}
#line 4561 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 2400 "modelParser.y" /* yacc.c:1646  */
    {
	int minOrder = (int)(yyvsp[-4].dblVal);
	int maxOrder = (int)(yyvsp[-1].dblVal);

	if(minOrder <= 0 || maxOrder <= 0)
	{
		parseError("Orders should be larger than zero.", lineNum);
		exit(1);
	}

	if(minOrder > maxOrder)
	{
		parseError("MAX order should be no smaller than MIN order.", lineNum);
		exit(1);
	}

	mode_local_setting.bAdaptiveSteps = false;
	mode_local_setting.bAdaptiveOrders = true;
	mode_local_setting.orderType = UNIFORM;
	mode_local_setting.orders.push_back(minOrder);
	mode_local_setting.maxOrders.push_back(maxOrder);
	mode_local_setting.globalMaxOrder = maxOrder;
}
#line 4589 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 2425 "modelParser.y" /* yacc.c:1646  */
    {
	mode_local_setting.bAdaptiveOrders = false;
	mode_local_setting.orderType = MULTI;
	mode_local_setting.orders = *(yyvsp[-1].iVec);

	for(int i=0; i<(yyvsp[-1].iVec)->size(); ++i)
	{
		if((*(yyvsp[-1].iVec))[i] <= 0)
		{
			parseError("Orders should be larger than zero.", lineNum);
			exit(1);
		}
	}

	int maxOrder = (*(yyvsp[-1].iVec))[0];
	for(int i=1; i<(yyvsp[-1].iVec)->size(); ++i)
	{
		if(maxOrder < (*(yyvsp[-1].iVec))[i])
		{
			maxOrder = (*(yyvsp[-1].iVec))[i];
		}
	}

	mode_local_setting.globalMaxOrder = maxOrder;
}
#line 4619 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 2452 "modelParser.y" /* yacc.c:1646  */
    {
	mode_local_setting.bAdaptiveSteps = false;
	mode_local_setting.bAdaptiveOrders = true;
	mode_local_setting.orderType = MULTI;
	mode_local_setting.orders = *(yyvsp[-7].iVec);
	mode_local_setting.maxOrders = *(yyvsp[-2].iVec);

	if((yyvsp[-7].iVec)->size() != (yyvsp[-2].iVec)->size())
	{
		parseError("Orders are not properly specified.", lineNum);
		exit(1);
	}

	for(int i=0; i<(yyvsp[-2].iVec)->size(); ++i)
	{
		if((*(yyvsp[-7].iVec))[i] <= 0 || (*(yyvsp[-2].iVec))[i] <= 0)
		{
			parseError("Orders should be larger than zero.", lineNum);
			exit(1);
		}

		if((*(yyvsp[-7].iVec))[i] > (*(yyvsp[-2].iVec))[i])
		{
			parseError("MAX order should be no smaller than MIN order.", lineNum);
			exit(1);
		}
	}

	int maxOrder = (*(yyvsp[-2].iVec))[0];
	for(int i=1; i<(yyvsp[-2].iVec)->size(); ++i)
	{
		if(maxOrder < (*(yyvsp[-2].iVec))[i])
		{
			maxOrder = (*(yyvsp[-2].iVec))[i];
		}
	}

	mode_local_setting.globalMaxOrder = maxOrder;
}
#line 4663 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 2492 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 4670 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 2497 "modelParser.y" /* yacc.c:1646  */
    {
	int startID = hybridProblem.getIDForMode(*(yyvsp[-15].identifier));
	if(startID < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*(yyvsp[-15].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	int endID = hybridProblem.getIDForMode(*(yyvsp[-12].identifier));
	if(endID < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*(yyvsp[-12].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if((yyvsp[-1].dVecVec)->size() > 0)
	{
		hybridProblem.declareTrans(startID, endID, *(yyvsp[-9].vecConstraints), *(yyvsp[-5].resetMap), PARA_AGGREG, *(yyvsp[-1].dVecVec));
	}
	else
	{
		std::vector<std::vector<double> > emptyVec;
		hybridProblem.declareTrans(startID, endID, *(yyvsp[-9].vecConstraints), *(yyvsp[-5].resetMap), PARA_AGGREG, emptyVec);
	}
}
#line 4704 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 2528 "modelParser.y" /* yacc.c:1646  */
    {
	int startID = hybridProblem.getIDForMode(*(yyvsp[-12].identifier));
	if(startID < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*(yyvsp[-12].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	int endID = hybridProblem.getIDForMode(*(yyvsp[-9].identifier));
	if(endID < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*(yyvsp[-9].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	std::vector<std::vector<double> > empty;
	hybridProblem.declareTrans(startID, endID, *(yyvsp[-6].vecConstraints), *(yyvsp[-2].resetMap), INTERVAL_AGGREG, empty);
}
#line 4731 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 2551 "modelParser.y" /* yacc.c:1646  */
    {
	hybridProblem.declareTrans();
}
#line 4739 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 2557 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.resetMap) = (yyvsp[-10].resetMap);

	int id = hybridProblem.getIDForStateVar(*(yyvsp[-9].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-9].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if((yyvsp[-3].dblVal) > (yyvsp[-1].dblVal))
	{
		parseError("Invalid remainder interval.", lineNum);
		exit(1);
	}

	flowstar::Interval I((yyvsp[-3].dblVal), (yyvsp[-1].dblVal));
	flowstar::TaylorModel tmTemp(*(yyvsp[-6].poly), I);
	(yyval.resetMap)->tmvReset.tms[id] = tmTemp;

	delete (yyvsp[-6].poly);
}
#line 4769 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 2584 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.resetMap) = (yyvsp[-4].resetMap);

	int id = hybridProblem.getIDForStateVar(*(yyvsp[-3].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-3].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Interval intZero;
	flowstar::TaylorModel tmTemp(*(yyvsp[0].poly), intZero);
	(yyval.resetMap)->tmvReset.tms[id] = tmTemp;

	delete (yyvsp[0].poly);
}
#line 4793 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 2604 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = hybridProblem.stateVarNames.size();

	flowstar::Matrix coefficients_identity_reset(numVars, numVars+1);

	for(int i=0; i<numVars; ++i)
	{
		coefficients_identity_reset.set(1, i, i+1);
	}

	flowstar::TaylorModelVec tmvReset(coefficients_identity_reset);

	(yyval.resetMap) = new flowstar::ResetMap(tmvReset);
}
#line 4812 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 2621 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.dVecVec)->push_back(*(yyvsp[0].dVec));
	delete (yyvsp[0].dVec);
}
#line 4821 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 2626 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.dVecVec) = new std::vector<std::vector<double> >(0);
}
#line 4829 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 2632 "modelParser.y" /* yacc.c:1646  */
    {
	int rangeDim = (yyvsp[-1].dVec)->size();

	if(rangeDim != hybridProblem.stateVarNames.size())
	{
		parseError("The vector dimension should be equivalent to the system dimension.", lineNum);
		exit(1);
	}

	(yyval.dVec) = new std::vector<double>(0);

	for(int i=0; i<rangeDim; ++i)
	{
		(yyval.dVec)->push_back(0);
	}

	bool bZero = true;
	for(int i=0; i<rangeDim; ++i)
	{
		if((*(yyvsp[-1].dVec))[i] < -THRESHOLD_LOW || (*(yyvsp[-1].dVec))[i] > THRESHOLD_LOW)
		{
			if(bZero)
			{
				bZero = false;
			}
		}

		(*(yyval.dVec))[i] = (*(yyvsp[-1].dVec))[i];
	}

	if(bZero)
	{
		parseError("A template vector should not be zero.", lineNum);
		exit(1);
	}

	delete (yyvsp[-1].dVec);
}
#line 4872 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 2673 "modelParser.y" /* yacc.c:1646  */
    {
	int id = hybridProblem.getIDForStateVar(*(yyvsp[-2].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-2].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(yyval.dVec) = (yyvsp[-4].dVec);
	(*(yyval.dVec))[id] = (yyvsp[0].dblVal);
	delete (yyvsp[-2].identifier);
}
#line 4892 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 2690 "modelParser.y" /* yacc.c:1646  */
    {
	int num = hybridProblem.stateVarNames.size();
	(yyval.dVec) = new std::vector<double>(num);

	int id = hybridProblem.getIDForStateVar(*(yyvsp[-2].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-2].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(*(yyval.dVec))[id] = (yyvsp[0].dblVal);
	delete (yyvsp[-2].identifier);
}
#line 4914 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 2710 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 4921 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 118:
#line 2715 "modelParser.y" /* yacc.c:1646  */
    {
	if(!continuousProblem.declareStateVar(*(yyvsp[0].identifier)))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s has already been declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.declareStateVar(*(yyvsp[0].identifier));
	delete (yyvsp[0].identifier);
}
#line 4938 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 2729 "modelParser.y" /* yacc.c:1646  */
    {
	if(!continuousProblem.declareStateVar(*(yyvsp[0].identifier)))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s has already been declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.declareStateVar(*(yyvsp[0].identifier));
	delete (yyvsp[0].identifier);
}
#line 4955 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 2746 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 4962 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 2751 "modelParser.y" /* yacc.c:1646  */
    {
	if(continuousProblem.getIDForStateVar(*(yyvsp[-2].identifier)) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*(yyvsp[-2].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		flowstar::Interval range((yyvsp[0].dblVal));

		if(!continuousProblem.declarePar(*(yyvsp[-2].identifier), range))
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Parameter %s has already been declared.", (*(yyvsp[-2].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		hybridProblem.declarePar(*(yyvsp[-2].identifier), range);
	}
}
#line 4990 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 2776 "modelParser.y" /* yacc.c:1646  */
    {
	if(continuousProblem.getIDForStateVar(*(yyvsp[-2].identifier)) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*(yyvsp[-2].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		flowstar::Interval range((yyvsp[0].dblVal));

		if(!continuousProblem.declarePar(*(yyvsp[-2].identifier), range))
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Parameter %s has already been declared.", (*(yyvsp[-2].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		hybridProblem.declarePar(*(yyvsp[-2].identifier), range);
	}
}
#line 5018 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 2803 "modelParser.y" /* yacc.c:1646  */
    {
	if(continuousProblem.getIDForStateVar(*(yyvsp[0].identifier)) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		if(continuousProblem.getIDForPar(*(yyvsp[0].identifier)) != -1)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "%s has already been declared as a parameter.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
		else
		{
			if(!continuousProblem.declareTIPar(*(yyvsp[0].identifier)))
			{
				char errMsg[MSG_SIZE];
				sprintf(errMsg, "Time-invariant uncertainty %s has already been declared.", (*(yyvsp[0].identifier)).c_str());
				parseError(errMsg, lineNum);
				exit(1);
			}
		}
	}
}
#line 5052 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 2834 "modelParser.y" /* yacc.c:1646  */
    {
	if(continuousProblem.getIDForStateVar(*(yyvsp[0].identifier)) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		if(continuousProblem.getIDForPar(*(yyvsp[0].identifier)) != -1)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "%s has already been declared as a parameter.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
		else
		{
			if(!continuousProblem.declareTIPar(*(yyvsp[0].identifier)))
			{
				char errMsg[MSG_SIZE];
				sprintf(errMsg, "Uncertainty %s has already been declared.", (*(yyvsp[0].identifier)).c_str());
				parseError(errMsg, lineNum);
				exit(1);
			}
		}
	}
}
#line 5086 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 125:
#line 2864 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 5093 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 126:
#line 2870 "modelParser.y" /* yacc.c:1646  */
    {
	if(continuousProblem.getIDForStateVar(*(yyvsp[0].identifier)) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		if(continuousProblem.getIDForPar(*(yyvsp[0].identifier)) != -1)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "%s has already been declared as a parameter.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
		else
		{
			if(!continuousProblem.declareTVPar(*(yyvsp[0].identifier)))
			{
				char errMsg[MSG_SIZE];
				sprintf(errMsg, "Uncertainty %s has already been declared.", (*(yyvsp[0].identifier)).c_str());
				parseError(errMsg, lineNum);
				exit(1);
			}
		}

//		hybridProblem.declareUnc(*$2, range);
	}
}
#line 5129 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 2903 "modelParser.y" /* yacc.c:1646  */
    {
	if(continuousProblem.getIDForStateVar(*(yyvsp[0].identifier)) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		if(continuousProblem.getIDForPar(*(yyvsp[0].identifier)) != -1)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "%s has already been declared as a parameter.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
		else
		{
			if(!continuousProblem.declareTVPar(*(yyvsp[0].identifier)))
			{
				char errMsg[MSG_SIZE];
				sprintf(errMsg, "Uncertainty %s has already been declared.", (*(yyvsp[0].identifier)).c_str());
				parseError(errMsg, lineNum);
				exit(1);
			}
		}

//		hybridProblem.declareUnc(*$1, range);
	}
}
#line 5165 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 2935 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 5172 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 2940 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-12].dblVal) <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	int order = (int)(yyvsp[-5].dblVal);

	if(order <= 0)
	{
		parseError("Orders should be larger than zero.", lineNum);
		exit(1);
	}

	continuousProblem.bAdaptiveSteps = false;
	continuousProblem.step = (yyvsp[-12].dblVal);
	continuousProblem.time = (yyvsp[-10].dblVal);
	continuousProblem.bAdaptiveOrders = false;
	continuousProblem.orderType = UNIFORM;
	continuousProblem.orders.push_back(order);
	continuousProblem.globalMaxOrder = order;

	hybridProblem.global_setting.bAdaptiveSteps = false;
	hybridProblem.global_setting.step = (yyvsp[-12].dblVal);
	hybridProblem.time = (yyvsp[-10].dblVal);
	hybridProblem.global_setting.bAdaptiveOrders = false;
	hybridProblem.global_setting.orderType = UNIFORM;
	hybridProblem.global_setting.orders.push_back(order);
	hybridProblem.global_setting.globalMaxOrder = order;

	if((yyvsp[-3].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-(yyvsp[-3].dblVal),(yyvsp[-3].dblVal));
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)(yyvsp[-1].dblVal);
}
#line 5220 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 2985 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-18].dblVal) <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	int minOrder = (int)(yyvsp[-9].dblVal);
	int maxOrder = (int)(yyvsp[-6].dblVal);

	if(minOrder <= 0 || maxOrder <= 0)
	{
		parseError("Orders should be larger than zero.", lineNum);
		exit(1);
	}

	if(minOrder > maxOrder)
	{
		parseError("MAX order should be no smaller than MIN order.", lineNum);
		exit(1);
	}

	continuousProblem.bAdaptiveSteps = false;
	continuousProblem.step = (yyvsp[-18].dblVal);
	continuousProblem.time = (yyvsp[-16].dblVal);
	continuousProblem.bAdaptiveOrders = true;
	continuousProblem.orderType = UNIFORM;
	continuousProblem.orders.push_back(minOrder);
	continuousProblem.maxOrders.push_back(maxOrder);
	continuousProblem.globalMaxOrder = maxOrder;

	hybridProblem.global_setting.bAdaptiveSteps = false;
	hybridProblem.global_setting.step = (yyvsp[-18].dblVal);
	hybridProblem.time = (yyvsp[-16].dblVal);
	hybridProblem.global_setting.bAdaptiveOrders = true;
	hybridProblem.global_setting.orderType = UNIFORM;
	hybridProblem.global_setting.orders.push_back(minOrder);
	hybridProblem.global_setting.maxOrders.push_back(maxOrder);
	hybridProblem.global_setting.globalMaxOrder = maxOrder;

	if((yyvsp[-3].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-(yyvsp[-3].dblVal),(yyvsp[-3].dblVal));
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)(yyvsp[-1].dblVal);
}
#line 5277 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 131:
#line 3039 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-14].dblVal) <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	continuousProblem.bAdaptiveSteps = false;
	continuousProblem.step = (yyvsp[-14].dblVal);
	continuousProblem.time = (yyvsp[-12].dblVal);
	continuousProblem.bAdaptiveOrders = false;
	continuousProblem.orderType = MULTI;
	continuousProblem.orders = *(yyvsp[-6].iVec);

	hybridProblem.global_setting.bAdaptiveSteps = false;
	hybridProblem.global_setting.step = (yyvsp[-14].dblVal);
	hybridProblem.time = (yyvsp[-12].dblVal);
	hybridProblem.global_setting.bAdaptiveOrders = false;
	hybridProblem.global_setting.orderType = MULTI;
	hybridProblem.global_setting.orders = *(yyvsp[-6].iVec);

	for(int i=0; i<(yyvsp[-6].iVec)->size(); ++i)
	{
		if((*(yyvsp[-6].iVec))[i] <= 0)
		{
			parseError("Orders should be larger than zero.", lineNum);
			exit(1);
		}
	}

	int maxOrder = (*(yyvsp[-6].iVec))[0];
	for(int i=1; i<(yyvsp[-6].iVec)->size(); ++i)
	{
		if(maxOrder < (*(yyvsp[-6].iVec))[i])
		{
			maxOrder = (*(yyvsp[-6].iVec))[i];
		}
	}

	continuousProblem.globalMaxOrder = maxOrder;
	hybridProblem.global_setting.globalMaxOrder = maxOrder;

	if((yyvsp[-3].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-(yyvsp[-3].dblVal),(yyvsp[-3].dblVal));
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)(yyvsp[-1].dblVal);

	delete (yyvsp[-6].iVec);
}
#line 5338 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 132:
#line 3097 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-22].dblVal) <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	continuousProblem.bAdaptiveSteps = false;
	continuousProblem.step = (yyvsp[-22].dblVal);
	continuousProblem.time = (yyvsp[-20].dblVal);
	continuousProblem.bAdaptiveOrders = true;
	continuousProblem.orderType = MULTI;
	continuousProblem.orders = *(yyvsp[-12].iVec);
	continuousProblem.maxOrders = *(yyvsp[-7].iVec);

	hybridProblem.global_setting.bAdaptiveSteps = false;
	hybridProblem.global_setting.step = (yyvsp[-22].dblVal);
	hybridProblem.time = (yyvsp[-20].dblVal);
	hybridProblem.global_setting.bAdaptiveOrders = true;
	hybridProblem.global_setting.orderType = MULTI;
	hybridProblem.global_setting.orders = *(yyvsp[-12].iVec);
	hybridProblem.global_setting.maxOrders = *(yyvsp[-7].iVec);

	if((yyvsp[-12].iVec)->size() != (yyvsp[-7].iVec)->size())
	{
		parseError("Orders are not properly specified.", lineNum);
		exit(1);
	}

	for(int i=0; i<(yyvsp[-7].iVec)->size(); ++i)
	{
		if((*(yyvsp[-12].iVec))[i] <= 0 || (*(yyvsp[-7].iVec))[i] <= 0)
		{
			parseError("Orders should be larger than zero.", lineNum);
			exit(1);
		}

		if((*(yyvsp[-12].iVec))[i] > (*(yyvsp[-7].iVec))[i])
		{
			parseError("MAX order should be no smaller than MIN order.", lineNum);
			exit(1);
		}
	}

	int maxOrder = (*(yyvsp[-7].iVec))[0];
	for(int i=1; i<(yyvsp[-7].iVec)->size(); ++i)
	{
		if(maxOrder < (*(yyvsp[-7].iVec))[i])
		{
			maxOrder = (*(yyvsp[-7].iVec))[i];
		}
	}

	continuousProblem.globalMaxOrder = maxOrder;
	hybridProblem.global_setting.globalMaxOrder = maxOrder;

	if((yyvsp[-3].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-(yyvsp[-3].dblVal),(yyvsp[-3].dblVal));
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)(yyvsp[-1].dblVal);

	delete (yyvsp[-12].iVec);
	delete (yyvsp[-7].iVec);
}
#line 5414 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 133:
#line 3170 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-16].dblVal) <= 0 || (yyvsp[-13].dblVal) <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	if((yyvsp[-16].dblVal) > (yyvsp[-13].dblVal))
	{
		parseError("MIN step should be no larger than MAX step.", lineNum);
		exit(1);
	}

	int order = (int)(yyvsp[-5].dblVal);

	if(order <= 0)
	{
		parseError("Orders should be larger than zero.", lineNum);
		exit(1);
	}

	continuousProblem.bAdaptiveSteps = true;
	continuousProblem.step = (yyvsp[-13].dblVal);
	continuousProblem.miniStep = (yyvsp[-16].dblVal);
	continuousProblem.time = (yyvsp[-10].dblVal);
	continuousProblem.bAdaptiveOrders = false;
	continuousProblem.orderType = UNIFORM;
	continuousProblem.orders.push_back(order);
	continuousProblem.globalMaxOrder = order;

	hybridProblem.global_setting.bAdaptiveSteps = true;
	hybridProblem.global_setting.step = (yyvsp[-13].dblVal);
	hybridProblem.global_setting.miniStep = (yyvsp[-16].dblVal);
	hybridProblem.time = (yyvsp[-10].dblVal);
	hybridProblem.global_setting.bAdaptiveOrders = false;
	hybridProblem.global_setting.orderType = UNIFORM;
	hybridProblem.global_setting.orders.push_back(order);
	hybridProblem.global_setting.globalMaxOrder = order;

	if((yyvsp[-3].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-(yyvsp[-3].dblVal),(yyvsp[-3].dblVal));
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)(yyvsp[-1].dblVal);
}
#line 5470 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 3223 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[-18].dblVal) <= 0 || (yyvsp[-15].dblVal) <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	if((yyvsp[-18].dblVal) > (yyvsp[-15].dblVal))
	{
		parseError("MIN step should be no larger than MAX step.", lineNum);
		exit(1);
	}

	for(int i=0; i<(yyvsp[-6].iVec)->size(); ++i)
	{
		if((*(yyvsp[-6].iVec))[i] <= 0)
		{
			parseError("Orders should be larger than zero.", lineNum);
			exit(1);
		}
	}

	continuousProblem.bAdaptiveSteps = true;
	continuousProblem.step = (yyvsp[-15].dblVal);
	continuousProblem.miniStep = (yyvsp[-18].dblVal);
	continuousProblem.time = (yyvsp[-12].dblVal);
	continuousProblem.bAdaptiveOrders = false;
	continuousProblem.orderType = MULTI;
	continuousProblem.orders = *(yyvsp[-6].iVec);

	hybridProblem.global_setting.bAdaptiveSteps = true;
	hybridProblem.global_setting.step = (yyvsp[-15].dblVal);
	hybridProblem.global_setting.miniStep = (yyvsp[-18].dblVal);
	hybridProblem.time = (yyvsp[-12].dblVal);
	hybridProblem.global_setting.bAdaptiveOrders = false;
	hybridProblem.global_setting.orderType = MULTI;
	hybridProblem.global_setting.orders = *(yyvsp[-6].iVec);

	int maxOrder = (*(yyvsp[-6].iVec))[0];
	for(int i=1; i<(yyvsp[-6].iVec)->size(); ++i)
	{
		if(maxOrder < (*(yyvsp[-6].iVec))[i])
		{
			maxOrder = (*(yyvsp[-6].iVec))[i];
		}
	}
	
	continuousProblem.globalMaxOrder = maxOrder;
	hybridProblem.global_setting.globalMaxOrder = maxOrder;

	if((yyvsp[-3].dblVal) <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-(yyvsp[-3].dblVal),(yyvsp[-3].dblVal));
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)(yyvsp[-1].dblVal);

	delete (yyvsp[-6].iVec);
}
#line 5539 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 135:
#line 3290 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].dblVal) <= 0)
	{
		parseError("Remainder estimation should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval I(-(yyvsp[0].dblVal), (yyvsp[0].dblVal));

	for(int i=0; i<continuousProblem.stateVarNames.size(); ++i)
	{
		continuousProblem.estimation.push_back(I);
		hybridProblem.global_setting.estimation.push_back(I);
	}
}
#line 5559 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 136:
#line 3307 "modelParser.y" /* yacc.c:1646  */
    {
	for(int i=0; i<(yyvsp[-1].intVec)->size(); ++i)
	{
		if((*(yyvsp[-1].intVec))[i].inf() >= (*(yyvsp[-1].intVec))[i].sup() - THRESHOLD_LOW)
		{
			parseError("Invalid remainder estimation.", lineNum);
			exit(1);
		}
	}

	continuousProblem.estimation = *(yyvsp[-1].intVec);
	hybridProblem.global_setting.estimation = *(yyvsp[-1].intVec);
	delete (yyvsp[-1].intVec);
}
#line 5578 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 137:
#line 3324 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.intVec) = (yyvsp[-8].intVec);
	int id = continuousProblem.getIDForStateVar(*(yyvsp[-6].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-6].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if((yyvsp[-3].dblVal) >= (yyvsp[-1].dblVal))
	{
		parseError("Invalid remainder estimation.", lineNum);
		exit(1);
	}

	flowstar::Interval I((yyvsp[-3].dblVal),(yyvsp[-1].dblVal));
	(*(yyval.intVec))[id] = I;
	delete (yyvsp[-6].identifier);
}
#line 5605 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 138:
#line 3348 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = continuousProblem.stateVarNames.size();
	(yyval.intVec) = new std::vector<flowstar::Interval>(numVars);

	int id = continuousProblem.getIDForStateVar(*(yyvsp[-6].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-6].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if((yyvsp[-3].dblVal) >= (yyvsp[-1].dblVal))
	{
		parseError("Invalid remainder estimation.", lineNum);
		exit(1);
	}

	flowstar::Interval I((yyvsp[-3].dblVal),(yyvsp[-1].dblVal));
	(*(yyval.intVec))[id] = I;
	delete (yyvsp[-6].identifier);
}
#line 5634 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 139:
#line 3375 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.iVec) = (yyvsp[-4].iVec);
	int id = continuousProblem.getIDForStateVar(*(yyvsp[-2].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-2].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(*(yyval.iVec))[id] = (int)(yyvsp[0].dblVal);
	delete (yyvsp[-2].identifier);
}
#line 5654 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 140:
#line 3392 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = continuousProblem.stateVarNames.size();
	(yyval.iVec) = new std::vector<int>(numVars);
	for(int i=0; i<numVars; ++i)
	{
		(*(yyval.iVec))[i] = 0;
	}

	int id = continuousProblem.getIDForStateVar(*(yyvsp[-2].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-2].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(*(yyval.iVec))[id] = (int)(yyvsp[0].dblVal);
	delete (yyvsp[-2].identifier);
}
#line 5680 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 3416 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.precondition = QR_PRE;
	hybridProblem.global_setting.precondition = QR_PRE;
}
#line 5689 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 142:
#line 3422 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.precondition = ID_PRE;
	hybridProblem.global_setting.precondition = ID_PRE;
}
#line 5698 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 3429 "modelParser.y" /* yacc.c:1646  */
    {
	int x = continuousProblem.getIDForStateVar(*(yyvsp[-2].identifier));
	int y = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	std::string tVar("t");

	if(x < 0)
	{
		if((yyvsp[-2].identifier)->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-2].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if((yyvsp[0].identifier)->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	continuousProblem.outputAxes.push_back(x);
	continuousProblem.outputAxes.push_back(y);
	continuousProblem.plotSetting = PLOT_INTERVAL;
	continuousProblem.plotFormat = PLOT_GNUPLOT;

	hybridProblem.outputAxes.push_back(x);
	hybridProblem.outputAxes.push_back(y);
	hybridProblem.plotSetting = PLOT_INTERVAL;
	hybridProblem.plotFormat = PLOT_GNUPLOT;

	delete (yyvsp[-2].identifier);
	delete (yyvsp[0].identifier);
}
#line 5751 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 144:
#line 3479 "modelParser.y" /* yacc.c:1646  */
    {
	int x = continuousProblem.getIDForStateVar(*(yyvsp[-2].identifier));
	int y = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	std::string tVar("t");

	if(x < 0)
	{
		if((yyvsp[-2].identifier)->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State Variable %s is not declared.", (*(yyvsp[-2].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if((yyvsp[0].identifier)->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	continuousProblem.outputAxes.push_back(x);
	continuousProblem.outputAxes.push_back(y);
	continuousProblem.plotSetting = PLOT_OCTAGON;
	continuousProblem.plotFormat = PLOT_GNUPLOT;

	hybridProblem.outputAxes.push_back(x);
	hybridProblem.outputAxes.push_back(y);
	hybridProblem.plotSetting = PLOT_OCTAGON;
	hybridProblem.plotFormat = PLOT_GNUPLOT;

	delete (yyvsp[-2].identifier);
	delete (yyvsp[0].identifier);
}
#line 5804 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 145:
#line 3529 "modelParser.y" /* yacc.c:1646  */
    {
	int x = continuousProblem.getIDForStateVar(*(yyvsp[-2].identifier));
	int y = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	std::string tVar("t");

	if(x < 0)
	{
		if((yyvsp[-2].identifier)->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-2].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if((yyvsp[0].identifier)->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	continuousProblem.outputAxes.push_back(x);
	continuousProblem.outputAxes.push_back(y);
	continuousProblem.plotSetting = PLOT_GRID;
	continuousProblem.numSections = (int)(yyvsp[-3].dblVal);
	continuousProblem.plotFormat = PLOT_GNUPLOT;

	hybridProblem.outputAxes.push_back(x);
	hybridProblem.outputAxes.push_back(y);
	hybridProblem.plotSetting = PLOT_GRID;
	hybridProblem.numSections = (int)(yyvsp[-3].dblVal);
	hybridProblem.plotFormat = PLOT_GNUPLOT;

	delete (yyvsp[-2].identifier);
	delete (yyvsp[0].identifier);
}
#line 5859 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 146:
#line 3581 "modelParser.y" /* yacc.c:1646  */
    {
	int x = continuousProblem.getIDForStateVar(*(yyvsp[-2].identifier));
	int y = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	std::string tVar("t");

	if(x < 0)
	{
		if((yyvsp[-2].identifier)->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-2].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if((yyvsp[0].identifier)->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	continuousProblem.outputAxes.push_back(x);
	continuousProblem.outputAxes.push_back(y);
	continuousProblem.plotSetting = PLOT_INTERVAL;
	continuousProblem.plotFormat = PLOT_MATLAB;

	hybridProblem.outputAxes.push_back(x);
	hybridProblem.outputAxes.push_back(y);
	hybridProblem.plotSetting = PLOT_INTERVAL;
	hybridProblem.plotFormat = PLOT_MATLAB;

	delete (yyvsp[-2].identifier);
	delete (yyvsp[0].identifier);
}
#line 5912 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 147:
#line 3631 "modelParser.y" /* yacc.c:1646  */
    {
	int x = continuousProblem.getIDForStateVar(*(yyvsp[-2].identifier));
	int y = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	std::string tVar("t");

	if(x < 0)
	{
		if((yyvsp[-2].identifier)->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State Variable %s is not declared.", (*(yyvsp[-2].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if((yyvsp[0].identifier)->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	continuousProblem.outputAxes.push_back(x);
	continuousProblem.outputAxes.push_back(y);
	continuousProblem.plotSetting = PLOT_OCTAGON;
	continuousProblem.plotFormat = PLOT_MATLAB;

	hybridProblem.outputAxes.push_back(x);
	hybridProblem.outputAxes.push_back(y);
	hybridProblem.plotSetting = PLOT_OCTAGON;
	hybridProblem.plotFormat = PLOT_MATLAB;

	delete (yyvsp[-2].identifier);
	delete (yyvsp[0].identifier);
}
#line 5965 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 148:
#line 3681 "modelParser.y" /* yacc.c:1646  */
    {
	int x = continuousProblem.getIDForStateVar(*(yyvsp[-2].identifier));
	int y = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	std::string tVar("t");

	if(x < 0)
	{
		if((yyvsp[-2].identifier)->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-2].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if((yyvsp[0].identifier)->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	continuousProblem.outputAxes.push_back(x);
	continuousProblem.outputAxes.push_back(y);
	continuousProblem.plotSetting = PLOT_GRID;
	continuousProblem.numSections = (int)(yyvsp[-3].dblVal);
	continuousProblem.plotFormat = PLOT_MATLAB;

	hybridProblem.outputAxes.push_back(x);
	hybridProblem.outputAxes.push_back(y);
	hybridProblem.plotSetting = PLOT_GRID;
	hybridProblem.numSections = (int)(yyvsp[-3].dblVal);
	hybridProblem.plotFormat = PLOT_MATLAB;

	delete (yyvsp[-2].identifier);
	delete (yyvsp[0].identifier);
}
#line 6020 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 149:
#line 3734 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pFlowpipe) = new flowstar::Flowpipe(*(yyvsp[-1].tmVec), *(yyvsp[0].intVec));

	delete (yyvsp[-1].tmVec);
	delete (yyvsp[0].intVec);
}
#line 6031 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 150:
#line 3742 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval intZero;
	(yyval.pFlowpipe) = new flowstar::Flowpipe(*(yyvsp[0].intVec), intZero);

	delete (yyvsp[0].intVec);
}
#line 6042 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 151:
#line 3750 "modelParser.y" /* yacc.c:1646  */
    {
	initialSets = *(yyvsp[0].pVecFlowpipe);
	(yyval.pFlowpipe) = NULL;

	delete (yyvsp[0].pVecFlowpipe);
}
#line 6053 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 152:
#line 3759 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval intZero;
	flowstar::Flowpipe flowpipe(*(yyvsp[-1].intVec), intZero);

	(*(yyval.pVecFlowpipe)).push_back(flowpipe);

	delete (yyvsp[-1].intVec);
}
#line 6066 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 153:
#line 3769 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pVecFlowpipe) = new std::vector<flowstar::Flowpipe>(1);

	flowstar::Interval intZero;
	flowstar::Flowpipe flowpipe(*(yyvsp[-1].intVec), intZero);
	(*(yyval.pVecFlowpipe))[0] = flowpipe;

	delete (yyvsp[-1].intVec);
}
#line 6080 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 154:
#line 3781 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 6087 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 155:
#line 3786 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));

	if(id != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
	
	if(id != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a parameter.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if(!continuousProblem.declareTMVar(*(yyvsp[0].identifier)))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "TM variable %s has already been declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.declareTMVar(*(yyvsp[0].identifier));
	delete (yyvsp[0].identifier);
}
#line 6124 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 156:
#line 3820 "modelParser.y" /* yacc.c:1646  */
    {
	std::string tVar("local_t");
	continuousProblem.declareTMVar(tVar);
	hybridProblem.declareTMVar(tVar);

	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));

	if(id != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
	
	if(id != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a parameter.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if(!continuousProblem.declareTMVar(*(yyvsp[0].identifier)))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "TM variable %s has already been declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.declareTMVar(*(yyvsp[0].identifier));
	delete (yyvsp[0].identifier);
}
#line 6165 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 157:
#line 3860 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 6172 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 158:
#line 3865 "modelParser.y" /* yacc.c:1646  */
    {
	if(!varlist.declareVar(*(yyvsp[0].identifier)))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Variable %s has already been declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	delete (yyvsp[0].identifier);
}
#line 6188 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 159:
#line 3878 "modelParser.y" /* yacc.c:1646  */
    {
	varlist.clear();

	std::string tVar("local_t");
	varlist.declareVar(tVar);

	if(!varlist.declareVar(*(yyvsp[0].identifier)))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Variable %s has already been declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	delete (yyvsp[0].identifier);
}
#line 6209 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 160:
#line 3898 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[-8].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-8].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if((yyvsp[-3].dblVal) > (yyvsp[-1].dblVal))
	{
		parseError("Invalid interval remainder.", lineNum);
		exit(1);
	}

	flowstar::Interval I((yyvsp[-3].dblVal),(yyvsp[-1].dblVal));
	flowstar::TaylorModel tmTemp(*(yyvsp[-6].poly), I);
	(yyval.tmVec) = (yyvsp[-9].tmVec);
	(yyval.tmVec)->tms[id] = tmTemp;

	delete (yyvsp[-8].identifier);
	delete (yyvsp[-6].poly);
}
#line 6239 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 161:
#line 3924 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::TaylorModel tmEmpty;
	(yyval.tmVec) = new flowstar::TaylorModelVec;

	for(int i=0; i<continuousProblem.stateVarNames.size(); ++i)
	{
		(yyval.tmVec)->tms.push_back(tmEmpty);
	}
}
#line 6253 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 3936 "modelParser.y" /* yacc.c:1646  */
    {
	if(continuousProblem.tmVarNames.size() == 0)
	{
		int id = varlist.getIDForVar(*(yyvsp[-6].identifier));

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Variable %s is not declared.", (*(yyvsp[-6].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		if((yyvsp[-3].dblVal) > (yyvsp[-1].dblVal))
		{
			parseError("Invalid interval.", lineNum);
			exit(1);
		}

		flowstar::Interval I((yyvsp[-3].dblVal),(yyvsp[-1].dblVal));
		(yyval.intVec) = (yyvsp[-7].intVec);
		(*(yyval.intVec))[id] = I;
	}
	else
	{
		int id = continuousProblem.getIDForTMVar(*(yyvsp[-6].identifier));

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "TM variable %s is not declared.", (*(yyvsp[-6].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		if((yyvsp[-3].dblVal) > (yyvsp[-1].dblVal))
		{
			parseError("Invalid interval.", lineNum);
			exit(1);
		}

		flowstar::Interval I((yyvsp[-3].dblVal),(yyvsp[-1].dblVal));
		(yyval.intVec) = (yyvsp[-7].intVec);
		(*(yyval.intVec))[id] = I;
	}

	delete (yyvsp[-6].identifier);
}
#line 6306 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 163:
#line 3986 "modelParser.y" /* yacc.c:1646  */
    {
	if(continuousProblem.tmVarNames.size() == 0)
	{
		(yyval.intVec) = new std::vector<flowstar::Interval>( varlist.varNames.size() );

		flowstar::Interval intZero;
		(*(yyval.intVec))[0] = intZero;

		int id = varlist.getIDForVar(*(yyvsp[-6].identifier));

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Variable %s is not declared.", (*(yyvsp[-6].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		if((yyvsp[-3].dblVal) > (yyvsp[-1].dblVal))
		{
			parseError("Invalid interval.", lineNum);
			exit(1);
		}

		flowstar::Interval I((yyvsp[-3].dblVal),(yyvsp[-1].dblVal));
		(*(yyval.intVec))[id] = I;
	}
	else
	{
		(yyval.intVec) = new std::vector<flowstar::Interval>( continuousProblem.tmVarNames.size() );

		flowstar::Interval intZero;
		(*(yyval.intVec))[0] = intZero;

		int id = continuousProblem.getIDForTMVar(*(yyvsp[-6].identifier));

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "TM variable %s is not declared.", (*(yyvsp[-6].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		if((yyvsp[-3].dblVal) > (yyvsp[-1].dblVal))
		{
			parseError("Invalid interval.", lineNum);
			exit(1);
		}

		flowstar::Interval I((yyvsp[-3].dblVal),(yyvsp[-1].dblVal));
		(*(yyval.intVec))[id] = I;
	}

	delete (yyvsp[-6].identifier);
}
#line 6367 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 164:
#line 4045 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[-6].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-6].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if((yyvsp[-3].dblVal) > (yyvsp[-1].dblVal))
	{
		parseError("Invalid interval.", lineNum);
		exit(1);
	}

	flowstar::Interval I((yyvsp[-3].dblVal),(yyvsp[-1].dblVal));
	(yyval.intVec) = (yyvsp[-7].intVec);
	(*(yyval.intVec))[id] = I;

	delete (yyvsp[-6].identifier);
}
#line 6395 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 165:
#line 4069 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = continuousProblem.stateVarNames.size();
	(yyval.intVec) = new std::vector<flowstar::Interval>(numVars);

	std::string tVar("local_t");
	continuousProblem.declareTMVar(tVar);

	char name[NAME_SIZE];

	for(int i=0; i<numVars; ++i)
	{
		sprintf(name, "%s%d", local_var_name, i+1);
		std::string tmVarName(name);
		continuousProblem.declareTMVar(tmVarName);
	}
}
#line 6416 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 166:
#line 4088 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.tmVec) = (yyvsp[-4].tmVec);

	int id = continuousProblem.getIDForStateVar(*(yyvsp[-3].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-3].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Interval intZero;
	flowstar::TaylorModel tmTemp(*(yyvsp[0].poly), intZero);
	(yyval.tmVec)->tms[id] = tmTemp;

	delete (yyvsp[-3].identifier);
	delete (yyvsp[0].poly);
}
#line 6441 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 167:
#line 4109 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = continuousProblem.stateVarNames.size();

	(yyval.tmVec) = new flowstar::TaylorModelVec;
	flowstar::TaylorModel tmTemp;
	flowstar::Interval intZero;

	for(int i=0; i<numVars; ++i)
	{
		(yyval.tmVec)->tms.push_back(tmTemp);
	}
}
#line 6458 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 4124 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.strVec) = (yyvsp[-4].strVec);

	int id = continuousProblem.getIDForStateVar(*(yyvsp[-3].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-3].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(*(yyval.strVec))[id] = (*(yyvsp[0].identifier));

	delete (yyvsp[-3].identifier);
	delete (yyvsp[0].identifier);
}
#line 6481 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 4143 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = continuousProblem.stateVarNames.size();
	(yyval.strVec) = new std::vector<std::string>;

	std::string empty;
	flowstar::Interval intZero;

	for(int i=0; i<numVars; ++i)
	{
		(yyval.strVec)->push_back(empty);
	}
}
#line 6498 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 170:
#line 4159 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 6505 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 171:
#line 4163 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 6512 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 172:
#line 4167 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 6519 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 173:
#line 4171 "modelParser.y" /* yacc.c:1646  */
    {
}
#line 6526 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 174:
#line 4177 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[-3].identifier));
	flowstar::Interval intZero;

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-3].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	for(int i=0; i<dyn_A_row.cols(); ++i)
	{
		dyn_A[id][i] = dyn_A_row[0][i];
		dyn_A_row[0][i] = intZero;
	}

	dyn_B[id][0] = dyn_B_row[0][0];
	dyn_B_row[0][0] = intZero;

	for(int i=0; i<dyn_ti_row.cols(); ++i)
	{
		dyn_ti[id][i] = dyn_ti_row[0][i];
		dyn_ti_row[0][i] = intZero;
	}

	for(int i=0; i<dyn_tv_row.cols(); ++i)
	{
		dyn_tv[id][i] = dyn_tv_row[0][i];
		dyn_tv_row[0][i] = intZero;
	}

	delete (yyvsp[-3].identifier);
}
#line 6566 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 4213 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = (int)continuousProblem.stateVarNames.size();
	int numTIPar = (int)continuousProblem.TI_Par_Names.size();
	int numTVPar = (int)continuousProblem.TV_Par_Names.size();

	flowstar::iMatrix im_A(numVars, numVars), im_B(numVars, 1), im_ti(numVars, numTIPar), im_tv(numVars, numTVPar);
	flowstar::iMatrix im_A_row(1, numVars), im_B_row(1,1), im_ti_row(1, numTIPar), im_tv_row(1, numTVPar);

	dyn_A = im_A;
	dyn_B = im_B;
	dyn_ti = im_ti;
	dyn_tv = im_tv;

	dyn_A_row = im_A_row;
	dyn_B_row = im_B_row;
	dyn_ti_row = im_ti_row;
	dyn_tv_row = im_tv_row;
}
#line 6589 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 4235 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[-3].identifier));
	flowstar::Interval intZero;

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-3].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	for(int i=0; i<dyn_A_row.cols(); ++i)
	{
		dyn_A[id][i] = dyn_A_row[0][i];
		dyn_A_row[0][i] = intZero;
	}

	dyn_B[id][0] = dyn_B_row[0][0];
	dyn_B_row[0][0] = intZero;
/*
	for(int i=0; i<dyn_ti_row.cols(); ++i)
	{
		dyn_ti[id][i] = dyn_ti_row[0][i];
		dyn_ti_row[0][i] = intZero;
	}

	for(int i=0; i<dyn_tv_row.cols(); ++i)
	{
		dyn_tv[id][i] = dyn_tv_row[0][i];
		dyn_tv_row[0][i] = intZero;
	}
*/
	delete (yyvsp[-3].identifier);
}
#line 6629 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 177:
#line 4271 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = (int)hybridProblem.stateVarNames.size();

	flowstar::iMatrix im_A(numVars, numVars), im_B(numVars, 1);
	flowstar::iMatrix im_A_row(1, numVars), im_B_row(1,1);

	dyn_A = im_A;
	dyn_B = im_B;

	dyn_A_row = im_A_row;
	dyn_B_row = im_B_row;
}
#line 6646 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 178:
#line 4288 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.maxNumSteps = -1;
}
#line 6654 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 179:
#line 4293 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.maxNumSteps = (yyvsp[-6].dblVal);
}
#line 6662 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 180:
#line 4298 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.maxNumSteps = -1;
}
#line 6670 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 181:
#line 4303 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.maxNumSteps = -1;
}
#line 6678 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 182:
#line 4308 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.maxNumSteps = (yyvsp[-6].dblVal);
}
#line 6686 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 183:
#line 4313 "modelParser.y" /* yacc.c:1646  */
    {
	continuousProblem.maxNumSteps = -1;
}
#line 6694 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 184:
#line 4320 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[-3].identifier));
	flowstar::UnivariatePolynomial polyZero;

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-3].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	for(int i=0; i<dyn_A_t_row.cols(); ++i)
	{
		dyn_A_t[id][i] = dyn_A_t_row[0][i];
		dyn_A_t_row[0][i] = polyZero;
	}

	dyn_B_t[id][0] = dyn_B_t_row[0][0];
	dyn_B_t_row[0][0] = polyZero;

	for(int i=0; i<dyn_ti_t_row.cols(); ++i)
	{
		dyn_ti_t[id][i] = dyn_ti_t_row[0][i];
		dyn_ti_t_row[0][i] = polyZero;
	}

	for(int i=0; i<dyn_tv_t_row.cols(); ++i)
	{
		dyn_tv_t[id][i] = dyn_tv_t_row[0][i];
		dyn_tv_t_row[0][i] = polyZero;
	}

	delete (yyvsp[-3].identifier);
}
#line 6734 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 185:
#line 4356 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = (int)continuousProblem.stateVarNames.size();
	int numTIPar = (int)continuousProblem.TI_Par_Names.size();
	int numTVPar = (int)continuousProblem.TV_Par_Names.size();

	flowstar::upMatrix upm_A_t(numVars, numVars), upm_B_t(numVars, 1), upm_ti_t(numVars, numTIPar), upm_tv_t(numVars, numTVPar);
	flowstar::upMatrix upm_A_t_row(1, numVars), upm_B_t_row(1,1), upm_ti_t_row(1, numTIPar), upm_tv_t_row(1, numTVPar);

	dyn_A_t = upm_A_t;
	dyn_B_t = upm_B_t;
	dyn_ti_t = upm_ti_t;
	dyn_tv_t = upm_tv_t;

	dyn_A_t_row = upm_A_t_row;
	dyn_B_t_row = upm_B_t_row;
	dyn_ti_t_row = upm_ti_t_row;
	dyn_tv_t_row = upm_tv_t_row;
}
#line 6757 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 186:
#line 4378 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[-3].identifier));
	flowstar::UnivariatePolynomial polyZero;

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-3].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	for(int i=0; i<dyn_A_t_row.cols(); ++i)
	{
		dyn_A_t[id][i] = dyn_A_t_row[0][i];
		dyn_A_t_row[0][i] = polyZero;
	}

	dyn_B_t[id][0] = dyn_B_t_row[0][0];
	dyn_B_t_row[0][0] = polyZero;

	delete (yyvsp[-3].identifier);
}
#line 6785 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 187:
#line 4402 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = (int)continuousProblem.stateVarNames.size();

	flowstar::upMatrix upm_A_t(numVars, numVars), upm_B_t(numVars, 1);
	flowstar::upMatrix upm_A_t_row(1, numVars), upm_B_t_row(1,1);

	dyn_A_t = upm_A_t;
	dyn_B_t = upm_B_t;

	dyn_A_t_row = upm_A_t_row;
	dyn_B_t_row = upm_B_t_row;
}
#line 6802 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 188:
#line 4430 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) += (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 6813 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 189:
#line 4438 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) -= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 6824 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 190:
#line 4446 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-1].poly); 
}
#line 6832 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 191:
#line 4451 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) *= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 6843 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 192:
#line 4459 "modelParser.y" /* yacc.c:1646  */
    {
	int exp = (int) (yyvsp[0].dblVal);

	if(exp == 0)
	{
		flowstar::Interval I(1);
		(yyval.poly) = new flowstar::Polynomial(I, continuousProblem.tmVarNames.size());
	}
	else
	{
		(yyval.poly) = new flowstar::Polynomial;
		(*(yyvsp[-2].poly)).pow(*(yyval.poly), exp);
	}

	delete (yyvsp[-2].poly);
}
#line 6864 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 193:
#line 4477 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I(-1);
	(yyval.poly) = (yyvsp[0].poly);
	(yyval.poly)->mul_assign(I);
}
#line 6874 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 194:
#line 4484 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForTMVar(*(yyvsp[0].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "TM variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	int numVars = continuousProblem.tmVarNames.size();
	flowstar::Interval I(1);

	std::vector<int> degrees;
	for(int i=0; i<numVars; ++i)
	{
		degrees.push_back(0);
	}

	degrees[id] = 1;
	flowstar::Monomial monomial(I, degrees);

	(yyval.poly) = new flowstar::Polynomial(monomial);
	delete (yyvsp[0].identifier);
}
#line 6905 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 195:
#line 4512 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = continuousProblem.tmVarNames.size();
	flowstar::Interval I((yyvsp[0].dblVal));
	(yyval.poly) = new flowstar::Polynomial(I, numVars);
}
#line 6915 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 196:
#line 4530 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) += (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 6926 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 197:
#line 4538 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) -= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 6937 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 198:
#line 4546 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-1].poly); 
}
#line 6945 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 199:
#line 4551 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) *= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 6956 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 200:
#line 4559 "modelParser.y" /* yacc.c:1646  */
    {
	int exp = (int) (yyvsp[0].dblVal);

	if(exp == 0)
	{
		flowstar::Interval I(1);
		(yyval.poly) = new flowstar::Polynomial(I, continuousProblem.stateVarNames.size()+1);
	}
	else
	{
		(yyval.poly) = new flowstar::Polynomial;
		(*(yyvsp[-2].poly)).pow(*(yyval.poly), exp);
	}

	delete (yyvsp[-2].poly);
}
#line 6977 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 201:
#line 4577 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I(-1);
	(yyval.poly) = (yyvsp[0].poly);
	(yyval.poly)->mul_assign(I);
}
#line 6987 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 202:
#line 4584 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));

	if(id >= 0)
	{
		flowstar::Interval range;
		continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));

		int numVars = continuousProblem.stateVarNames.size()+1;
		(yyval.poly) = new flowstar::Polynomial(range, numVars);
	}
	else
	{
		id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		int numVars = continuousProblem.stateVarNames.size()+1;
		flowstar::Interval I(1);

		std::vector<int> degrees;
		for(int i=0; i<numVars; ++i)
		{
			degrees.push_back(0);
		}

		degrees[id+1] = 1;
		flowstar::Monomial monomial(I, degrees);

		(yyval.poly) = new flowstar::Polynomial(monomial);
	}

	delete (yyvsp[0].identifier);
}
#line 7032 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 203:
#line 4626 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I((yyvsp[-3].dblVal), (yyvsp[-1].dblVal));
	(yyval.poly) = new flowstar::Polynomial(I, numVars);
}
#line 7042 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 204:
#line 4633 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I((yyvsp[0].dblVal));
	(yyval.poly) = new flowstar::Polynomial(I, numVars);
}
#line 7052 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 205:
#line 4643 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) += (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 7063 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 206:
#line 4651 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) -= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 7074 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 207:
#line 4659 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-1].poly); 
}
#line 7082 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 208:
#line 4664 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) *= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 7093 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 209:
#line 4672 "modelParser.y" /* yacc.c:1646  */
    {
	int exp = (int) (yyvsp[0].dblVal);

	if(exp == 0)
	{
		flowstar::Interval I(1);
		(yyval.poly) = new flowstar::Polynomial(I, continuousProblem.stateVarNames.size()+1);
	}
	else
	{
		(yyval.poly) = new flowstar::Polynomial;
		(*(yyvsp[-2].poly)).pow(*(yyval.poly), exp);
	}

	delete (yyvsp[-2].poly);
}
#line 7114 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 210:
#line 4690 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I(-1);
	(yyval.poly) = (yyvsp[0].poly);
	(yyval.poly)->mul_assign(I);
}
#line 7124 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 211:
#line 4697 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));

	if(id >= 0)
	{
		flowstar::Interval range;
		continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));

		int numVars = continuousProblem.stateVarNames.size()+1;
		(yyval.poly) = new flowstar::Polynomial(range, numVars);
	}
	else
	{
		id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		int numVars = continuousProblem.stateVarNames.size()+1;
		flowstar::Interval I(1);

		std::vector<int> degrees;
		for(int i=0; i<numVars; ++i)
		{
			degrees.push_back(0);
		}

		degrees[id+1] = 1;
		flowstar::Monomial monomial(I, degrees);

		(yyval.poly) = new flowstar::Polynomial(monomial);
	}

	delete (yyvsp[0].identifier);
}
#line 7169 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 212:
#line 4739 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I((yyvsp[0].dblVal));
	(yyval.poly) = new flowstar::Polynomial(I, numVars);
}
#line 7179 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 213:
#line 4750 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) += (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 7190 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 214:
#line 4758 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) -= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 7201 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 215:
#line 4766 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-1].poly); 
}
#line 7209 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 216:
#line 4771 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) *= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 7220 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 217:
#line 4779 "modelParser.y" /* yacc.c:1646  */
    {
	int exp = (int) (yyvsp[0].dblVal);

	if(exp == 0)
	{
		flowstar::Interval I(1);
		(yyval.poly) = new flowstar::Polynomial(I, continuousProblem.stateVarNames.size()+1);
	}
	else
	{
		(yyval.poly) = new flowstar::Polynomial;
		(*(yyvsp[-2].poly)).pow(*(yyval.poly), exp);
	}

	delete (yyvsp[-2].poly);
}
#line 7241 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 4797 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I(-1);
	(yyval.poly) = (yyvsp[0].poly);
	(yyval.poly)->mul_assign(I);
}
#line 7251 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 4804 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));

	if(id >= 0)
	{
		flowstar::Interval range;
		continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));

		int numVars = continuousProblem.stateVarNames.size()+1;
		(yyval.poly) = new flowstar::Polynomial(range, numVars);
	}
	else
	{
		id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		int numVars = continuousProblem.stateVarNames.size()+1;
		flowstar::Interval I(1);

		std::vector<int> degrees;
		for(int i=0; i<numVars; ++i)
		{
			degrees.push_back(0);
		}

		degrees[id+1] = 1;
		flowstar::Monomial monomial(I, degrees);

		(yyval.poly) = new flowstar::Polynomial(monomial);
	}

	delete (yyvsp[0].identifier);
}
#line 7296 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 220:
#line 4846 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I((yyvsp[0].dblVal));
	(yyval.poly) = new flowstar::Polynomial(I, numVars);
}
#line 7306 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 4858 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[-8].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-8].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if((yyvsp[-3].dblVal) > (yyvsp[-1].dblVal))
	{
		parseError("Invalid interval remainder.", lineNum);
		exit(1);
	}

	flowstar::Interval I((yyvsp[-3].dblVal),(yyvsp[-1].dblVal));
	flowstar::TaylorModel tmTemp(*(yyvsp[-6].poly), I);
	(yyval.tmVec) = (yyvsp[-9].tmVec);
	(yyval.tmVec)->tms[id] = tmTemp;

	delete (yyvsp[-8].identifier);
	delete (yyvsp[-6].poly);
}
#line 7336 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 222:
#line 4885 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::TaylorModel tmEmpty;
	(yyval.tmVec) = new flowstar::TaylorModelVec;

	for(int i=0; i<continuousProblem.stateVarNames.size(); ++i)
	{
		(yyval.tmVec)->tms.push_back(tmEmpty);
	}

	int id = continuousProblem.getIDForStateVar(*(yyvsp[-8].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[-8].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if((yyvsp[-3].dblVal) > (yyvsp[-1].dblVal))
	{
		parseError("Invalid interval remainder.", lineNum);
		exit(1);
	}

	flowstar::Interval I((yyvsp[-3].dblVal),(yyvsp[-1].dblVal));
	flowstar::TaylorModel tmTemp(*(yyvsp[-6].poly), I);

	(yyval.tmVec)->tms[id] = tmTemp;

	delete (yyvsp[-8].identifier);
	delete (yyvsp[-6].poly);
}
#line 7374 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 223:
#line 4930 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) += (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 7385 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 224:
#line 4938 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) -= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 7396 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 225:
#line 4946 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-1].poly); 
}
#line 7404 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 4951 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) *= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 7415 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 227:
#line 4959 "modelParser.y" /* yacc.c:1646  */
    {
	int exp = (int) (yyvsp[0].dblVal);

	if(exp == 0)
	{
		flowstar::Interval I(1);
		(yyval.poly) = new flowstar::Polynomial(I, continuousProblem.tmVarNames.size());
	}
	else
	{
		(yyval.poly) = new flowstar::Polynomial;
		(*(yyvsp[-2].poly)).pow(*(yyval.poly), exp);
	}

	delete (yyvsp[-2].poly);
}
#line 7436 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 228:
#line 4977 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I(-1);
	(yyval.poly) = (yyvsp[0].poly);
	(yyval.poly)->mul_assign(I);
}
#line 7446 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 229:
#line 4984 "modelParser.y" /* yacc.c:1646  */
    {
	if(continuousProblem.tmVarNames.size() == 0)
	{
		int id = varlist.getIDForVar(*(yyvsp[0].identifier));

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		int numVars = varlist.varNames.size();
		flowstar::Interval I(1);

		std::vector<int> degrees;
		for(int i=0; i<numVars; ++i)
		{
			degrees.push_back(0);
		}

		degrees[id] = 1;
		flowstar::Monomial monomial(I, degrees);

		(yyval.poly) = new flowstar::Polynomial(monomial);
	}
	else
	{
		int id = continuousProblem.getIDForTMVar(*(yyvsp[0].identifier));

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "TM variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		int numVars = continuousProblem.tmVarNames.size();
		flowstar::Interval I(1);

		std::vector<int> degrees;
		for(int i=0; i<numVars; ++i)
		{
			degrees.push_back(0);
		}

		degrees[id] = 1;
		flowstar::Monomial monomial(I, degrees);

		(yyval.poly) = new flowstar::Polynomial(monomial);
	}

	delete (yyvsp[0].identifier);
}
#line 7507 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 230:
#line 5042 "modelParser.y" /* yacc.c:1646  */
    {
	if(continuousProblem.tmVarNames.size() == 0)
	{
		int numVars = varlist.varNames.size();
		flowstar::Interval I((yyvsp[-3].dblVal), (yyvsp[-1].dblVal));
		(yyval.poly) = new flowstar::Polynomial(I, numVars);
	}
	else
	{
		int numVars = continuousProblem.tmVarNames.size();
		flowstar::Interval I((yyvsp[-3].dblVal), (yyvsp[-1].dblVal));
		(yyval.poly) = new flowstar::Polynomial(I, numVars);
	}
}
#line 7526 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 231:
#line 5058 "modelParser.y" /* yacc.c:1646  */
    {
	if(continuousProblem.tmVarNames.size() == 0)
	{
		int numVars = varlist.varNames.size();
		flowstar::Interval I((yyvsp[0].dblVal));
		(yyval.poly) = new flowstar::Polynomial(I, numVars);
	}
	else
	{
		int numVars = continuousProblem.tmVarNames.size();
		flowstar::Interval I((yyvsp[0].dblVal));
		(yyval.poly) = new flowstar::Polynomial(I, numVars);
	}
}
#line 7545 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 5094 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.ptm) = (yyvsp[-2].ptm);
	(yyvsp[-2].ptm)->add_assign(*(yyvsp[0].ptm));
	delete (yyvsp[0].ptm);
}
#line 7555 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 233:
#line 5101 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.ptm) = (yyvsp[-2].ptm);
	(yyvsp[-2].ptm)->sub_assign(*(yyvsp[0].ptm));
	delete (yyvsp[0].ptm);
}
#line 7565 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 234:
#line 5108 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.ptm) = (yyvsp[-2].ptm);

	flowstar::Interval intPoly1, intPoly2, intTrunc;

	(yyvsp[0].ptm)->polyRangeNormal(intPoly2, parseSetting.step_exp_table);
	(yyvsp[-2].ptm)->mul_insert_ctrunc_normal_assign(intPoly1, intTrunc, *(yyvsp[0].ptm), intPoly2, parseSetting.step_exp_table, parseSetting.order, parseSetting.cutoff_threshold);

	parseSetting.ranges.push_back(intPoly1);
	parseSetting.ranges.push_back(intPoly2);
	parseSetting.ranges.push_back(intTrunc);

	delete (yyvsp[0].ptm);
}
#line 7584 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 5124 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.ptm) = (yyvsp[-1].ptm);
}
#line 7592 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 236:
#line 5129 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::TaylorModel tmTemp;
	(yyvsp[0].ptm)->rec_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(yyval.ptm) = (yyvsp[-2].ptm);

	flowstar::Interval intPoly1, intPoly2, intTrunc;

	tmTemp.polyRangeNormal(intPoly2, parseSetting.step_exp_table);
	(yyvsp[-2].ptm)->mul_insert_ctrunc_normal_assign(intPoly1, intTrunc, tmTemp, intPoly2, parseSetting.step_exp_table, parseSetting.order, parseSetting.cutoff_threshold);

	parseSetting.ranges.push_back(intPoly1);
	parseSetting.ranges.push_back(intPoly2);
	parseSetting.ranges.push_back(intTrunc);

	delete (yyvsp[0].ptm);
}
#line 7614 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 237:
#line 5148 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::TaylorModel tmTemp;
	(yyvsp[-1].ptm)->exp_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	*(yyvsp[-1].ptm) = tmTemp;
	(yyval.ptm) = (yyvsp[-1].ptm);
}
#line 7626 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 238:
#line 5157 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::TaylorModel tmTemp;
	(yyvsp[-1].ptm)->sin_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	*(yyvsp[-1].ptm) = tmTemp;
	(yyval.ptm) = (yyvsp[-1].ptm);
}
#line 7638 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 239:
#line 5166 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::TaylorModel tmTemp;
	(yyvsp[-1].ptm)->cos_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	*(yyvsp[-1].ptm) = tmTemp;
	(yyval.ptm) = (yyvsp[-1].ptm);
}
#line 7650 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 240:
#line 5175 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::TaylorModel tmTemp;
	(yyvsp[-1].ptm)->log_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	*(yyvsp[-1].ptm) = tmTemp;
	(yyval.ptm) = (yyvsp[-1].ptm);
}
#line 7662 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 241:
#line 5184 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::TaylorModel tmTemp;
	(yyvsp[-1].ptm)->sqrt_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	*(yyvsp[-1].ptm) = tmTemp;
	(yyval.ptm) = (yyvsp[-1].ptm);
}
#line 7674 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 242:
#line 5193 "modelParser.y" /* yacc.c:1646  */
    {
	int exp = (int)(yyvsp[0].dblVal);

	if(exp == 0)
	{
		flowstar::Interval I(1);
		(yyval.ptm) = new flowstar::TaylorModel(I, continuousProblem.stateVarNames.size()+1);
	}
	else
	{
		flowstar::TaylorModel res = *(yyvsp[-2].ptm);
		flowstar::TaylorModel pow = *(yyvsp[-2].ptm);
		
		flowstar::Interval intPoly1, intPoly2, intTrunc;
		
		for(int degree = exp - 1; degree > 0;)
		{
			pow.polyRangeNormal(intPoly2, parseSetting.step_exp_table);
		
			if(degree & 1)
			{
				res.mul_insert_ctrunc_normal_assign(intPoly1, intTrunc, pow, intPoly2, parseSetting.step_exp_table, parseSetting.order, parseSetting.cutoff_threshold);

				parseSetting.ranges.push_back(intPoly1);
				parseSetting.ranges.push_back(intPoly2);
				parseSetting.ranges.push_back(intTrunc);
			}
			
			degree >>= 1;
			
			if(degree > 0)
			{
				pow.mul_insert_ctrunc_normal_assign(intPoly1, intTrunc, pow, intPoly2, parseSetting.step_exp_table, parseSetting.order, parseSetting.cutoff_threshold);

				parseSetting.ranges.push_back(intPoly1);
				parseSetting.ranges.push_back(intPoly2);
				parseSetting.ranges.push_back(intTrunc);
			}
		}
		
		(yyval.ptm) = new flowstar::TaylorModel(res);
	}

	delete (yyvsp[-2].ptm);
}
#line 7724 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 243:
#line 5240 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I(-1);
	(yyval.ptm) = (yyvsp[0].ptm);
	(yyval.ptm)->mul_assign(I);
}
#line 7734 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 244:
#line 5247 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(yyval.ptm) = new flowstar::TaylorModel;
	(*(yyval.ptm)) = parseSetting.flowpipe.tms[id];

	flowstar::Interval intTemp;
	(*(yyval.ptm)).expansion.ctrunc_normal(intTemp, parseSetting.step_exp_table, parseSetting.order);
	parseSetting.ranges.push_back(intTemp);
	(*(yyval.ptm)).remainder += intTemp;

	delete (yyvsp[0].identifier);
}
#line 7760 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 245:
#line 5270 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I((yyvsp[-3].dblVal), (yyvsp[-1].dblVal));
	(yyval.ptm) = new flowstar::TaylorModel(I, numVars);
}
#line 7770 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 246:
#line 5304 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pint) = (yyvsp[-2].pint);
	(*(yyval.pint)) += (*(yyvsp[0].pint));
	delete (yyvsp[0].pint);
}
#line 7780 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 5311 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pint) = (yyvsp[-2].pint);
	(*(yyval.pint)) -= (*(yyvsp[0].pint));
	delete (yyvsp[0].pint);
}
#line 7790 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 248:
#line 5318 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pint) = new flowstar::Interval;

	(*(yyval.pint)) = (*parseSetting.iterRange) * (*(yyvsp[0].pint));
	++parseSetting.iterRange;
	(*(yyval.pint)) += (*parseSetting.iterRange) * (*(yyvsp[-2].pint));
	(*(yyval.pint)) += (*(yyvsp[-2].pint)) * (*(yyvsp[0].pint));
	++parseSetting.iterRange;
	(*(yyval.pint)) += (*parseSetting.iterRange);
	++parseSetting.iterRange;

	delete (yyvsp[-2].pint);
	delete (yyvsp[0].pint);
}
#line 7809 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 249:
#line 5334 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pint) = (yyvsp[-1].pint);
}
#line 7817 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 5339 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval intTemp;
	rec_taylor_only_remainder(intTemp, *(yyvsp[0].pint), parseSetting.iterRange, parseSetting.order);

	(yyval.pint) = new flowstar::Interval;

	(*(yyval.pint)) = (*parseSetting.iterRange) * intTemp;
	++parseSetting.iterRange;
	(*(yyval.pint)) += (*parseSetting.iterRange) * (*(yyvsp[-2].pint));
	(*(yyval.pint)) += (*(yyvsp[-2].pint)) * intTemp;
	++parseSetting.iterRange;
	(*(yyval.pint)) += (*parseSetting.iterRange);
	++parseSetting.iterRange;

	delete (yyvsp[-2].pint);
	delete (yyvsp[0].pint);
}
#line 7839 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 251:
#line 5358 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval intTemp;
	exp_taylor_only_remainder(intTemp, *(yyvsp[-1].pint), parseSetting.iterRange, parseSetting.order);

	(*(yyvsp[-1].pint)) = intTemp;
	(yyval.pint) = (yyvsp[-1].pint);
}
#line 7851 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 5367 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval intTemp;
	sin_taylor_only_remainder(intTemp, *(yyvsp[-1].pint), parseSetting.iterRange, parseSetting.order);

	(*(yyvsp[-1].pint)) = intTemp;
	(yyval.pint) = (yyvsp[-1].pint);
}
#line 7863 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 5376 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval intTemp;
	cos_taylor_only_remainder(intTemp, *(yyvsp[-1].pint), parseSetting.iterRange, parseSetting.order);

	(*(yyvsp[-1].pint)) = intTemp;
	(yyval.pint) = (yyvsp[-1].pint);
}
#line 7875 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 254:
#line 5385 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval intTemp;
	log_taylor_only_remainder(intTemp, *(yyvsp[-1].pint), parseSetting.iterRange, parseSetting.order);

	(*(yyvsp[-1].pint)) = intTemp;
	(yyval.pint) = (yyvsp[-1].pint);
}
#line 7887 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 255:
#line 5394 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval intTemp;
	sqrt_taylor_only_remainder(intTemp, *(yyvsp[-1].pint), parseSetting.iterRange, parseSetting.order);

	(*(yyvsp[-1].pint)) = intTemp;
	(yyval.pint) = (yyvsp[-1].pint);
}
#line 7899 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 256:
#line 5403 "modelParser.y" /* yacc.c:1646  */
    {
	int exp = (int)(yyvsp[0].dblVal);

	if(exp == 0)
	{
		flowstar::Interval intZero;
		(*(yyvsp[-2].pint)) = intZero;
		(yyval.pint) = (yyvsp[-2].pint);
	}
	else
	{
		flowstar::Interval res(*(yyvsp[-2].pint));
		flowstar::Interval pow(*(yyvsp[-2].pint));
		
		flowstar::Interval intPoly1, intPoly2, intTrunc;
		
		for(int degree = exp - 1; degree > 0;)
		{
			if(degree & 1)
			{
				flowstar::Interval intTemp;
				intTemp = (*parseSetting.iterRange) * pow;
				++parseSetting.iterRange;
				intTemp += (*parseSetting.iterRange) * res;
				intTemp += pow * res;
				++parseSetting.iterRange;
				intTemp += (*parseSetting.iterRange);
				++parseSetting.iterRange;
			
				res = intTemp;
			}
			
			degree >>= 1;
			
			if(degree > 0)
			{
				flowstar::Interval intTemp;
				intTemp = (*parseSetting.iterRange) * pow;
				++parseSetting.iterRange;
				intTemp += (*parseSetting.iterRange) * pow;
				intTemp += pow * pow;
				++parseSetting.iterRange;
				intTemp += (*parseSetting.iterRange);
				++parseSetting.iterRange;
			
				pow = intTemp;
			}
		}
		
		(yyval.pint) = new flowstar::Interval(res);
	}

	delete (yyvsp[-2].pint);
}
#line 7958 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 257:
#line 5459 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pint) = (yyvsp[0].pint);
	(yyval.pint)->inv_assign();
}
#line 7967 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 258:
#line 5465 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(yyval.pint) = new flowstar::Interval;
	(*(yyval.pint)) = parseSetting.flowpipe.tms[id].getRemainder();
	
	(*(yyval.pint)) += (*parseSetting.iterRange);
	++parseSetting.iterRange;

	delete (yyvsp[0].identifier);
}
#line 7991 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 259:
#line 5486 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pint) = new flowstar::Interval;
}
#line 7999 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 260:
#line 5513 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) += (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 8010 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 261:
#line 5521 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) -= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 8021 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 262:
#line 5529 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) *= (*(yyvsp[0].poly));
	(yyval.poly)->nctrunc(parseSetting.order);
	(yyval.poly)->cutoff(parseSetting.cutoff_threshold);

	delete (yyvsp[0].poly);
}
#line 8034 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 263:
#line 5539 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-1].poly);
}
#line 8042 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 264:
#line 5544 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Polynomial polyTemp;
	(yyvsp[0].poly)->rec_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*(yyvsp[-2].poly)) *= polyTemp;
	(yyvsp[-2].poly)->nctrunc(parseSetting.order);
	(yyval.poly) = (yyvsp[-2].poly);
	(yyval.poly)->cutoff(parseSetting.cutoff_threshold);

	delete (yyvsp[0].poly);
}
#line 8058 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 265:
#line 5557 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Polynomial polyTemp;
	(yyvsp[-1].poly)->exp_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*(yyvsp[-1].poly)) = polyTemp;
	(yyval.poly) = (yyvsp[-1].poly);
}
#line 8070 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 266:
#line 5566 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Polynomial polyTemp;
	(yyvsp[-1].poly)->sin_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*(yyvsp[-1].poly)) = polyTemp;
	(yyval.poly) = (yyvsp[-1].poly);
}
#line 8082 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 267:
#line 5575 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Polynomial polyTemp;
	(yyvsp[-1].poly)->cos_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*(yyvsp[-1].poly)) = polyTemp;
	(yyval.poly) = (yyvsp[-1].poly);
}
#line 8094 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 268:
#line 5584 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Polynomial polyTemp;
	(yyvsp[-1].poly)->log_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*(yyvsp[-1].poly)) = polyTemp;
	(yyval.poly) = (yyvsp[-1].poly);
}
#line 8106 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 269:
#line 5593 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Polynomial polyTemp;
	(yyvsp[-1].poly)->sqrt_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*(yyvsp[-1].poly)) = polyTemp;
	(yyval.poly) = (yyvsp[-1].poly);
}
#line 8118 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 270:
#line 5602 "modelParser.y" /* yacc.c:1646  */
    {
	int exp = (int) (yyvsp[0].dblVal);
	
	(yyval.poly) = new flowstar::Polynomial;
	
	(*(yyvsp[-2].poly)).pow(*(yyval.poly), exp, parseSetting.order);
	(yyval.poly)->cutoff(parseSetting.cutoff_threshold);
}
#line 8131 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 271:
#line 5612 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[0].poly);
	(yyval.poly)->inv_assign();
}
#line 8140 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 272:
#line 5618 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(yyval.poly) = new flowstar::Polynomial;
	parseSetting.flowpipe.tms[id].getExpansion(*(yyval.poly));
	
	(*(yyval.poly)).nctrunc(parseSetting.order);
}
#line 8161 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 273:
#line 5636 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I((yyvsp[-3].dblVal), (yyvsp[-1].dblVal));
	(yyval.poly) = new flowstar::Polynomial(I, continuousProblem.stateVarNames.size()+1);
}
#line 8170 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 274:
#line 5664 "modelParser.y" /* yacc.c:1646  */
    {
	(*(yyvsp[-2].identifier)) += ' ';
	(*(yyvsp[-2].identifier)) += '+';
	(*(yyvsp[-2].identifier)) += ' ';
	(*(yyvsp[-2].identifier)) += (*(yyvsp[0].identifier));

	(yyval.identifier) = (yyvsp[-2].identifier);
	delete (yyvsp[0].identifier);
}
#line 8184 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 275:
#line 5675 "modelParser.y" /* yacc.c:1646  */
    {
	(*(yyvsp[-2].identifier)) += ' ';
	(*(yyvsp[-2].identifier)) += '-';
	(*(yyvsp[-2].identifier)) += ' ';
	(*(yyvsp[-2].identifier)) += (*(yyvsp[0].identifier));

	(yyval.identifier) = (yyvsp[-2].identifier);
	delete (yyvsp[0].identifier);
}
#line 8198 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 276:
#line 5686 "modelParser.y" /* yacc.c:1646  */
    {
	(*(yyvsp[-2].identifier)) += ' ';
	(*(yyvsp[-2].identifier)) += '*';
	(*(yyvsp[-2].identifier)) += ' ';
	(*(yyvsp[-2].identifier)) += (*(yyvsp[0].identifier));

	(yyval.identifier) = (yyvsp[-2].identifier);
	delete (yyvsp[0].identifier);
}
#line 8212 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 277:
#line 5697 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str;
	str += '(';
	str += (*(yyvsp[-1].identifier));
	str += ')';
	(*(yyvsp[-1].identifier)) = str;

	(yyval.identifier) = (yyvsp[-1].identifier);
}
#line 8226 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 278:
#line 5708 "modelParser.y" /* yacc.c:1646  */
    {
	(*(yyvsp[-2].identifier)) += ' ';
	(*(yyvsp[-2].identifier)) += '/';
	(*(yyvsp[-2].identifier)) += ' ';
	(*(yyvsp[-2].identifier)) += (*(yyvsp[0].identifier));

	(yyval.identifier) = (yyvsp[-2].identifier);
	delete (yyvsp[0].identifier);
}
#line 8240 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 279:
#line 5719 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str("exp");
	str += '(';
	str += (*(yyvsp[-1].identifier));
	str += ')';
	(*(yyvsp[-1].identifier)) = str;

	(yyval.identifier) = (yyvsp[-1].identifier);
}
#line 8254 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 280:
#line 5730 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str("sin");
	str += '(';
	str += (*(yyvsp[-1].identifier));
	str += ')';
	(*(yyvsp[-1].identifier)) = str;

	(yyval.identifier) = (yyvsp[-1].identifier);
}
#line 8268 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 281:
#line 5741 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str("cos");
	str += '(';
	str += (*(yyvsp[-1].identifier));
	str += ')';
	(*(yyvsp[-1].identifier)) = str;

	(yyval.identifier) = (yyvsp[-1].identifier);
}
#line 8282 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 282:
#line 5752 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str("log");
	str += '(';
	str += (*(yyvsp[-1].identifier));
	str += ')';
	(*(yyvsp[-1].identifier)) = str;

	(yyval.identifier) = (yyvsp[-1].identifier);
}
#line 8296 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 283:
#line 5763 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str("sqrt");
	str += '(';
	str += (*(yyvsp[-1].identifier));
	str += ')';
	(*(yyvsp[-1].identifier)) = str;

	(yyval.identifier) = (yyvsp[-1].identifier);
}
#line 8310 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 284:
#line 5774 "modelParser.y" /* yacc.c:1646  */
    {
	(*(yyvsp[-2].identifier)) += '^';

	char strNum[NUM_LENGTH];
	sprintf(strNum, "%d", (int)(yyvsp[0].dblVal));
	std::string num(strNum);
	(*(yyvsp[-2].identifier)) += num;

	(yyval.identifier) = (yyvsp[-2].identifier);
}
#line 8325 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 285:
#line 5786 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str;
	str += '-';
	str += (*(yyvsp[0].identifier));
	(*(yyvsp[0].identifier)) = str;

	(yyval.identifier) = (yyvsp[0].identifier);
}
#line 8338 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 286:
#line 5796 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));

	if(id >= 0)
	{
		flowstar::Interval range;
		continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));

		(yyval.identifier) = new std::string;
		range.toString(*(yyval.identifier));
	}
	else
	{
		id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		(yyval.identifier) = (yyvsp[0].identifier);
	}
}
#line 8369 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 287:
#line 5824 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.identifier) = new std::string;
	char strNum_lo[NUM_LENGTH], strNum_up[NUM_LENGTH];
	sprintf(strNum_lo, "%.20e", (yyvsp[-3].dblVal));
	sprintf(strNum_up, "%.20e", (yyvsp[-1].dblVal));

	std::string num_lo(strNum_lo);
	std::string num_up(strNum_up);

	(*(yyval.identifier)) += '[';
	(*(yyval.identifier)) += num_lo;
	(*(yyval.identifier)) += ' ';
	(*(yyval.identifier)) += ',';
	(*(yyval.identifier)) += ' ';
	(*(yyval.identifier)) += num_up;
	(*(yyval.identifier)) += ']';
}
#line 8391 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 288:
#line 5843 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.identifier) = new std::string;
	char strNum[NUM_LENGTH];
	sprintf(strNum, "%.20e", (yyvsp[0].dblVal));
	std::string num(strNum);

	(*(yyval.identifier)) += '[';
	(*(yyval.identifier)) += num;
	(*(yyval.identifier)) += ' ';
	(*(yyval.identifier)) += ',';
	(*(yyval.identifier)) += ' ';
	(*(yyval.identifier)) += num;
	(*(yyval.identifier)) += ']';
}
#line 8410 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 289:
#line 5877 "modelParser.y" /* yacc.c:1646  */
    {
	(yyvsp[-2].p_ODE_String)->ode += ' ';
	(yyvsp[-2].p_ODE_String)->ode += '+';
	(yyvsp[-2].p_ODE_String)->ode += ' ';
	(yyvsp[-2].p_ODE_String)->ode += (yyvsp[0].p_ODE_String)->ode;

	(yyval.p_ODE_String) = (yyvsp[-2].p_ODE_String);

	if(parseResult.bConstant)
	{
		(yyval.p_ODE_String)->constant = (yyvsp[-2].p_ODE_String)->constant + (yyvsp[0].p_ODE_String)->constant;
	}

	delete (yyvsp[0].p_ODE_String);
}
#line 8430 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 290:
#line 5894 "modelParser.y" /* yacc.c:1646  */
    {
	(yyvsp[-2].p_ODE_String)->ode += ' ';
	(yyvsp[-2].p_ODE_String)->ode += '-';
	(yyvsp[-2].p_ODE_String)->ode += ' ';
	(yyvsp[-2].p_ODE_String)->ode += (yyvsp[0].p_ODE_String)->ode;

	(yyval.p_ODE_String) = (yyvsp[-2].p_ODE_String);

	if(parseResult.bConstant)
	{
		(yyval.p_ODE_String)->constant = (yyvsp[-2].p_ODE_String)->constant - (yyvsp[0].p_ODE_String)->constant;
	}

	delete (yyvsp[0].p_ODE_String);
}
#line 8450 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 291:
#line 5911 "modelParser.y" /* yacc.c:1646  */
    {
	(yyvsp[-2].p_ODE_String)->ode += ' ';
	(yyvsp[-2].p_ODE_String)->ode += '*';
	(yyvsp[-2].p_ODE_String)->ode += ' ';
	(yyvsp[-2].p_ODE_String)->ode += (yyvsp[0].p_ODE_String)->ode;

	(yyval.p_ODE_String) = (yyvsp[-2].p_ODE_String);

	if(parseResult.bConstant)
	{
		(yyval.p_ODE_String)->constant = (yyvsp[-2].p_ODE_String)->constant * (yyvsp[0].p_ODE_String)->constant;
	}

	delete (yyvsp[0].p_ODE_String);
}
#line 8470 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 292:
#line 5928 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str;
	str += '(';
	str += (yyvsp[-1].p_ODE_String)->ode;
	str += ')';
	(yyvsp[-1].p_ODE_String)->ode = str;

	(yyval.p_ODE_String) = (yyvsp[-1].p_ODE_String);
}
#line 8484 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 293:
#line 5939 "modelParser.y" /* yacc.c:1646  */
    {
	(yyvsp[-2].p_ODE_String)->ode += ' ';
	(yyvsp[-2].p_ODE_String)->ode += '/';
	(yyvsp[-2].p_ODE_String)->ode += ' ';
	(yyvsp[-2].p_ODE_String)->ode += (yyvsp[0].p_ODE_String)->ode;

	(yyval.p_ODE_String) = (yyvsp[-2].p_ODE_String);

	if(parseResult.bConstant)
	{
		(yyval.p_ODE_String)->constant = (yyvsp[-2].p_ODE_String)->constant / (yyvsp[0].p_ODE_String)->constant;
	}

	delete (yyvsp[0].p_ODE_String);
}
#line 8504 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 294:
#line 5956 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str("exp");
	str += '(';
	str += (yyvsp[-1].p_ODE_String)->ode;
	str += ')';
	(yyvsp[-1].p_ODE_String)->ode = str;

	if(parseResult.bConstant)
	{
		(yyvsp[-1].p_ODE_String)->constant.exp_assign();
	}

	(yyval.p_ODE_String) = (yyvsp[-1].p_ODE_String);
}
#line 8523 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 295:
#line 5972 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str("sin");
	str += '(';
	str += (yyvsp[-1].p_ODE_String)->ode;
	str += ')';
	(yyvsp[-1].p_ODE_String)->ode = str;

	if(parseResult.bConstant)
	{
		(yyvsp[-1].p_ODE_String)->constant.sin_assign();
	}

	(yyval.p_ODE_String) = (yyvsp[-1].p_ODE_String);
}
#line 8542 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 296:
#line 5988 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str("cos");
	str += '(';
	str += (yyvsp[-1].p_ODE_String)->ode;
	str += ')';
	(yyvsp[-1].p_ODE_String)->ode = str;

	if(parseResult.bConstant)
	{
		(yyvsp[-1].p_ODE_String)->constant.cos_assign();
	}

	(yyval.p_ODE_String) = (yyvsp[-1].p_ODE_String);
}
#line 8561 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 297:
#line 6004 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str("log");
	str += '(';
	str += (yyvsp[-1].p_ODE_String)->ode;
	str += ')';
	(yyvsp[-1].p_ODE_String)->ode = str;

	if(parseResult.bConstant)
	{
		(yyvsp[-1].p_ODE_String)->constant.log_assign();
	}

	(yyval.p_ODE_String) = (yyvsp[-1].p_ODE_String);
}
#line 8580 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 298:
#line 6020 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str("sqrt");
	str += '(';
	str += (yyvsp[-1].p_ODE_String)->ode;
	str += ')';
	(yyvsp[-1].p_ODE_String)->ode = str;

	if(parseResult.bConstant)
	{
		(yyvsp[-1].p_ODE_String)->constant.sqrt_assign();
	}

	(yyval.p_ODE_String) = (yyvsp[-1].p_ODE_String);
}
#line 8599 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 299:
#line 6036 "modelParser.y" /* yacc.c:1646  */
    {
	(yyvsp[-2].p_ODE_String)->ode += '^';

	char strNum[NUM_LENGTH];
	sprintf(strNum, "%d", (int)(yyvsp[0].dblVal));
	std::string num(strNum);
	(yyvsp[-2].p_ODE_String)->ode += num;

	if(parseResult.bConstant)
	{
		(yyvsp[-2].p_ODE_String)->constant.pow_assign((int)(yyvsp[0].dblVal));
	}

	(yyval.p_ODE_String) = (yyvsp[-2].p_ODE_String);
}
#line 8619 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 300:
#line 6053 "modelParser.y" /* yacc.c:1646  */
    {
	std::string str;
	str += '-';
	str += (yyvsp[0].p_ODE_String)->ode;
	(yyvsp[0].p_ODE_String)->ode = str;

	if(parseResult.bConstant)
	{
		(yyvsp[0].p_ODE_String)->constant.inv_assign();
	}

	(yyval.p_ODE_String) = (yyvsp[0].p_ODE_String);
}
#line 8637 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 301:
#line 6068 "modelParser.y" /* yacc.c:1646  */
    {
	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Interval intZero;
	parseResult.bConstant = false;
	parseResult.constant = intZero;

	(yyval.p_ODE_String) = new ODE_String(*(yyvsp[0].identifier), intZero);
}
#line 8659 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 302:
#line 6087 "modelParser.y" /* yacc.c:1646  */
    {
	char strNum[NUM_LENGTH];
	sprintf(strNum, "%.20e", ((yyvsp[-3].dblVal)+(yyvsp[-1].dblVal))/2);

	std::string num(strNum), strOde;

	strOde += '[';
	strOde += num;
	strOde += ' ';
	strOde += ',';
	strOde += ' ';
	strOde += num;
	strOde += ']';

	flowstar::Interval I((yyvsp[-3].dblVal), (yyvsp[-1].dblVal));
	(yyval.p_ODE_String) = new ODE_String(strOde, I);
}
#line 8681 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 303:
#line 6106 "modelParser.y" /* yacc.c:1646  */
    {
	char strNum[NUM_LENGTH];
	sprintf(strNum, "%.20e", (yyvsp[0].dblVal));
	std::string num(strNum), strOde;

	strOde += '[';
	strOde += num;
	strOde += ' ';
	strOde += ',';
	strOde += ' ';
	strOde += num;
	strOde += ']';

	flowstar::Interval I((yyvsp[0].dblVal));
	(yyval.p_ODE_String) = new ODE_String(strOde, I);
}
#line 8702 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 304:
#line 6245 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTI_Term)->stateVar_id >= 0)
	{
		dyn_A_row[0][(yyvsp[0].p_LTI_Term)->stateVar_id] += (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTI_Term)->ti_par_id >= 0)
	{
		dyn_ti_row[0][(yyvsp[0].p_LTI_Term)->ti_par_id] += (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTI_Term)->tv_par_id >= 0)
	{
		dyn_tv_row[0][(yyvsp[0].p_LTI_Term)->tv_par_id] += (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else
	{
		dyn_B_row[0][0] += (yyvsp[0].p_LTI_Term)->coefficient;
	}
}
#line 8725 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 305:
#line 6265 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTI_Term)->stateVar_id >= 0)
	{
		dyn_A_row[0][(yyvsp[0].p_LTI_Term)->stateVar_id] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTI_Term)->ti_par_id >= 0)
	{
		dyn_ti_row[0][(yyvsp[0].p_LTI_Term)->ti_par_id] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTI_Term)->tv_par_id >= 0)
	{
		dyn_tv_row[0][(yyvsp[0].p_LTI_Term)->tv_par_id] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else
	{
		dyn_B_row[0][0] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
}
#line 8748 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 306:
#line 6285 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTI_Term)->stateVar_id >= 0)
	{
		dyn_A_row[0][(yyvsp[0].p_LTI_Term)->stateVar_id] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTI_Term)->ti_par_id >= 0)
	{
		dyn_ti_row[0][(yyvsp[0].p_LTI_Term)->ti_par_id] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTI_Term)->tv_par_id >= 0)
	{
		dyn_tv_row[0][(yyvsp[0].p_LTI_Term)->tv_par_id] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else
	{
		dyn_B_row[0][0] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
}
#line 8771 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 307:
#line 6305 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTI_Term)->stateVar_id >= 0)
	{
		dyn_A_row[0][(yyvsp[0].p_LTI_Term)->stateVar_id] = (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTI_Term)->ti_par_id >= 0)
	{
		dyn_ti_row[0][(yyvsp[0].p_LTI_Term)->ti_par_id] = (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTI_Term)->tv_par_id >= 0)
	{
		dyn_tv_row[0][(yyvsp[0].p_LTI_Term)->tv_par_id] = (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else
	{
		dyn_B_row[0][0] = (yyvsp[0].p_LTI_Term)->coefficient;
	}
}
#line 8794 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 308:
#line 6327 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTI_Term) = new LTI_Term;

	flowstar::Interval coe((yyvsp[-2].dblVal));
	
	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	if(id >= 0)				// a state variable
	{
		(yyval.p_LTI_Term)->coefficient = coe;
		(yyval.p_LTI_Term)->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));
			(yyval.p_LTI_Term)->coefficient = coe * range;
		}
		else
		{
			id = continuousProblem.getIDForTIPar(*(yyvsp[0].identifier));
			if(id >= 0)		// a time-invariant uncertainty
			{
				(yyval.p_LTI_Term)->coefficient = coe;
				(yyval.p_LTI_Term)->ti_par_id = id;
			}
			else
			{
				id = continuousProblem.getIDForTVPar(*(yyvsp[0].identifier));
				if(id >= 0)	// a time-varying uncertainty
				{
					(yyval.p_LTI_Term)->coefficient = coe;
					(yyval.p_LTI_Term)->tv_par_id = id;
				}
				else		// not defined
				{
					char errMsg[MSG_SIZE];
					sprintf(errMsg, "Symbol %s is not defined.", (*(yyvsp[0].identifier)).c_str());
					parseError(errMsg, lineNum);
					exit(1);
				}
			}
		}
	}

	delete (yyvsp[0].identifier);
}
#line 8848 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 309:
#line 6378 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTI_Term) = new LTI_Term;

	flowstar::Interval coe((yyvsp[-5].dblVal), (yyvsp[-3].dblVal));

	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	if(id >= 0)				// a state variable
	{
		(yyval.p_LTI_Term)->coefficient = coe;
		(yyval.p_LTI_Term)->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));
			(yyval.p_LTI_Term)->coefficient = coe * range;
		}
		else
		{
			id = continuousProblem.getIDForTIPar(*(yyvsp[0].identifier));
			if(id >= 0)		// a time-invariant uncertainty
			{
				(yyval.p_LTI_Term)->coefficient = coe;
				(yyval.p_LTI_Term)->ti_par_id = id;
			}
			else
			{
				id = continuousProblem.getIDForTVPar(*(yyvsp[0].identifier));
				if(id >= 0)	// a time-varying uncertainty
				{
					(yyval.p_LTI_Term)->coefficient = coe;
					(yyval.p_LTI_Term)->tv_par_id = id;
				}
				else		// not defined
				{
					char errMsg[MSG_SIZE];
					sprintf(errMsg, "Symbol %s is not defined.", (*(yyvsp[0].identifier)).c_str());
					parseError(errMsg, lineNum);
					exit(1);
				}
			}
		}
	}

	delete (yyvsp[0].identifier);
}
#line 8902 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 310:
#line 6429 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTI_Term) = new LTI_Term;

	flowstar::Interval coe(1);

	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	if(id >= 0)				// a state variable
	{
		(yyval.p_LTI_Term)->coefficient = coe;
		(yyval.p_LTI_Term)->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));
			(yyval.p_LTI_Term)->coefficient = range;
		}
		else
		{
			id = continuousProblem.getIDForTIPar(*(yyvsp[0].identifier));
			if(id >= 0)		// a time-invariant uncertainty
			{
				(yyval.p_LTI_Term)->coefficient = coe;
				(yyval.p_LTI_Term)->ti_par_id = id;
			}
			else
			{
				id = continuousProblem.getIDForTVPar(*(yyvsp[0].identifier));
				if(id >= 0)	// a time-varying uncertainty
				{
					(yyval.p_LTI_Term)->coefficient = coe;
					(yyval.p_LTI_Term)->tv_par_id = id;
				}
				else		// not defined
				{
					char errMsg[MSG_SIZE];
					sprintf(errMsg, "Symbol %s is not defined.", (*(yyvsp[0].identifier)).c_str());
					parseError(errMsg, lineNum);
					exit(1);
				}
			}
		}
	}

	delete (yyvsp[0].identifier);
}
#line 8956 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 311:
#line 6480 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTI_Term) = new LTI_Term((yyvsp[-3].dblVal), (yyvsp[-1].dblVal), -1, -1, -1);
}
#line 8964 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 312:
#line 6485 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTI_Term) = new LTI_Term((yyvsp[0].dblVal), (yyvsp[0].dblVal), -1, -1, -1);
}
#line 8972 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 313:
#line 6501 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTI_Term)->stateVar_id >= 0)
	{
		dyn_A_row[0][(yyvsp[0].p_LTI_Term)->stateVar_id] += (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else
	{
		dyn_B_row[0][0] += (yyvsp[0].p_LTI_Term)->coefficient;
	}
}
#line 8987 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 314:
#line 6513 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTI_Term)->stateVar_id >= 0)
	{
		dyn_A_row[0][(yyvsp[0].p_LTI_Term)->stateVar_id] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else
	{
		dyn_B_row[0][0] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
}
#line 9002 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 315:
#line 6525 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTI_Term)->stateVar_id >= 0)
	{
		dyn_A_row[0][(yyvsp[0].p_LTI_Term)->stateVar_id] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else
	{
		dyn_B_row[0][0] -= (yyvsp[0].p_LTI_Term)->coefficient;
	}
}
#line 9017 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 316:
#line 6537 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTI_Term)->stateVar_id >= 0)
	{
		dyn_A_row[0][(yyvsp[0].p_LTI_Term)->stateVar_id] = (yyvsp[0].p_LTI_Term)->coefficient;
	}
	else
	{
		dyn_B_row[0][0] = (yyvsp[0].p_LTI_Term)->coefficient;
	}
}
#line 9032 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 317:
#line 6551 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTI_Term) = new LTI_Term;

	flowstar::Interval coe((yyvsp[-2].dblVal));
	
	int id = hybridProblem.getIDForStateVar(*(yyvsp[0].identifier));
	if(id >= 0)				// a state variable
	{
		(yyval.p_LTI_Term)->coefficient = coe;
		(yyval.p_LTI_Term)->stateVar_id = id;
	}
	else
	{
		id = hybridProblem.getIDForPar(*(yyvsp[0].identifier));
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			hybridProblem.getRangeForPar(range, *(yyvsp[0].identifier));
			(yyval.p_LTI_Term)->coefficient = range;
		}
		else				// not defined
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Symbol %s is not defined.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	delete (yyvsp[0].identifier);
}
#line 9068 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 318:
#line 6584 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTI_Term) = new LTI_Term;

	flowstar::Interval coe((yyvsp[-5].dblVal), (yyvsp[-3].dblVal));

	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	if(id >= 0)				// a state variable
	{
		(yyval.p_LTI_Term)->coefficient = coe;
		(yyval.p_LTI_Term)->stateVar_id = id;
	}
	else
	{
		id = hybridProblem.getIDForPar(*(yyvsp[0].identifier));
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			hybridProblem.getRangeForPar(range, *(yyvsp[0].identifier));
			(yyval.p_LTI_Term)->coefficient = range;
		}
		else				// not defined
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Symbol %s is not defined.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	delete (yyvsp[0].identifier);
}
#line 9104 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 319:
#line 6617 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTI_Term) = new LTI_Term;

	flowstar::Interval coe(1);

	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	if(id >= 0)				// a state variable
	{
		(yyval.p_LTI_Term)->coefficient = coe;
		(yyval.p_LTI_Term)->stateVar_id = id;
	}
	else
	{
		id = hybridProblem.getIDForPar(*(yyvsp[0].identifier));
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			hybridProblem.getRangeForPar(range, *(yyvsp[0].identifier));
			(yyval.p_LTI_Term)->coefficient = range;
		}
		else				// not defined
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Symbol %s is not defined.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	delete (yyvsp[0].identifier);
}
#line 9140 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 320:
#line 6650 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTI_Term) = new LTI_Term((yyvsp[-3].dblVal), (yyvsp[-1].dblVal), -1, -1, -1);
}
#line 9148 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 321:
#line 6655 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTI_Term) = new LTI_Term((yyvsp[0].dblVal), (yyvsp[0].dblVal), -1, -1, -1);
}
#line 9156 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 322:
#line 6685 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTV_Term)->stateVar_id >= 0)
	{
		dyn_A_t_row[0][(yyvsp[0].p_LTV_Term)->stateVar_id] += (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTV_Term)->ti_par_id >= 0)
	{
		dyn_ti_t_row[0][(yyvsp[0].p_LTV_Term)->ti_par_id] += (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTV_Term)->tv_par_id >= 0)
	{
		dyn_tv_t_row[0][(yyvsp[0].p_LTV_Term)->tv_par_id] += (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] += (yyvsp[0].p_LTV_Term)->coefficient;
	}
}
#line 9179 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 323:
#line 6705 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTV_Term)->stateVar_id >= 0)
	{
		dyn_A_t_row[0][(yyvsp[0].p_LTV_Term)->stateVar_id] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTV_Term)->ti_par_id >= 0)
	{
		dyn_ti_t_row[0][(yyvsp[0].p_LTV_Term)->ti_par_id] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTV_Term)->tv_par_id >= 0)
	{
		dyn_tv_t_row[0][(yyvsp[0].p_LTV_Term)->tv_par_id] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
}
#line 9202 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 324:
#line 6725 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTV_Term)->stateVar_id >= 0)
	{
		dyn_A_t_row[0][(yyvsp[0].p_LTV_Term)->stateVar_id] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTV_Term)->ti_par_id >= 0)
	{
		dyn_ti_t_row[0][(yyvsp[0].p_LTV_Term)->ti_par_id] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTV_Term)->tv_par_id >= 0)
	{
		dyn_tv_t_row[0][(yyvsp[0].p_LTV_Term)->tv_par_id] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
}
#line 9225 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 325:
#line 6745 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTV_Term)->stateVar_id >= 0)
	{
		dyn_A_t_row[0][(yyvsp[0].p_LTV_Term)->stateVar_id] = (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTV_Term)->ti_par_id >= 0)
	{
		dyn_ti_t_row[0][(yyvsp[0].p_LTV_Term)->ti_par_id] = (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else if((yyvsp[0].p_LTV_Term)->tv_par_id >= 0)
	{
		dyn_tv_t_row[0][(yyvsp[0].p_LTV_Term)->tv_par_id] = (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] = (yyvsp[0].p_LTV_Term)->coefficient;
	}
}
#line 9248 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 326:
#line 6768 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTV_Term) = new LTV_Term;
	
	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	if(id >= 0)				// a state variable
	{
		(yyval.p_LTV_Term)->coefficient = *(yyvsp[-3].pUpoly);
		(yyval.p_LTV_Term)->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));
			(yyval.p_LTV_Term)->coefficient = (*(yyvsp[-3].pUpoly)) * range;
		}
		else
		{
			id = continuousProblem.getIDForTIPar(*(yyvsp[0].identifier));
			if(id >= 0)		// a time-invariant uncertainty
			{
				(yyval.p_LTV_Term)->coefficient = *(yyvsp[-3].pUpoly);
				(yyval.p_LTV_Term)->ti_par_id = id;
			}
			else
			{
				id = continuousProblem.getIDForTVPar(*(yyvsp[0].identifier));
				if(id >= 0)	// a time-varying uncertainty
				{
					(yyval.p_LTV_Term)->coefficient = *(yyvsp[-3].pUpoly);
					(yyval.p_LTV_Term)->tv_par_id = id;
				}
				else		// not defined
				{
					char errMsg[MSG_SIZE];
					sprintf(errMsg, "Symbol %s is not defined.", (*(yyvsp[0].identifier)).c_str());
					parseError(errMsg, lineNum);
					exit(1);
				}
			}
		}
	}

	delete (yyvsp[-3].pUpoly);
	delete (yyvsp[0].identifier);
}
#line 9301 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 327:
#line 6818 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTV_Term) = new LTV_Term;
	flowstar::Interval intOne(1);

	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	if(id >= 0)				// a state variable
	{
		(yyval.p_LTV_Term)->coefficient = intOne;
		(yyval.p_LTV_Term)->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));
			(yyval.p_LTV_Term)->coefficient = range;
		}
		else
		{
			id = continuousProblem.getIDForTIPar(*(yyvsp[0].identifier));
			if(id >= 0)		// a time-invariant uncertainty
			{
				(yyval.p_LTV_Term)->coefficient = intOne;
				(yyval.p_LTV_Term)->ti_par_id = id;
			}
			else
			{
				id = continuousProblem.getIDForTVPar(*(yyvsp[0].identifier));
				if(id >= 0)	// a time-varying uncertainty
				{
					(yyval.p_LTV_Term)->coefficient = intOne;
					(yyval.p_LTV_Term)->tv_par_id = id;
				}
				else		// not defined
				{
					char errMsg[MSG_SIZE];
					sprintf(errMsg, "Symbol %s is not defined.", (*(yyvsp[0].identifier)).c_str());
					parseError(errMsg, lineNum);
					exit(1);
				}
			}
		}
	}

	delete (yyvsp[0].identifier);
}
#line 9354 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 328:
#line 6868 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTV_Term) = new LTV_Term(*(yyvsp[-1].pUpoly), -1, -1, -1);

	delete (yyvsp[-1].pUpoly);
}
#line 9364 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 329:
#line 6880 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTV_Term)->stateVar_id >= 0)
	{
		dyn_A_t_row[0][(yyvsp[0].p_LTV_Term)->stateVar_id] += (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] += (yyvsp[0].p_LTV_Term)->coefficient;
	}
}
#line 9379 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 330:
#line 6892 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTV_Term)->stateVar_id >= 0)
	{
		dyn_A_t_row[0][(yyvsp[0].p_LTV_Term)->stateVar_id] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
}
#line 9394 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 331:
#line 6904 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTV_Term)->stateVar_id >= 0)
	{
		dyn_A_t_row[0][(yyvsp[0].p_LTV_Term)->stateVar_id] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] -= (yyvsp[0].p_LTV_Term)->coefficient;
	}
}
#line 9409 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 332:
#line 6916 "modelParser.y" /* yacc.c:1646  */
    {
	if((yyvsp[0].p_LTV_Term)->stateVar_id >= 0)
	{
		dyn_A_t_row[0][(yyvsp[0].p_LTV_Term)->stateVar_id] = (yyvsp[0].p_LTV_Term)->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] = (yyvsp[0].p_LTV_Term)->coefficient;
	}
}
#line 9424 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 333:
#line 6932 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTV_Term) = new LTV_Term;
	
	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	if(id >= 0)				// a state variable
	{
		(yyval.p_LTV_Term)->coefficient = *(yyvsp[-3].pUpoly);
		(yyval.p_LTV_Term)->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));
			(yyval.p_LTV_Term)->coefficient = (*(yyvsp[-3].pUpoly)) * range;
		}
		else		// not defined
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Symbol %s is not defined.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	delete (yyvsp[-3].pUpoly);
	delete (yyvsp[0].identifier);
}
#line 9459 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 334:
#line 6964 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTV_Term) = new LTV_Term;
	flowstar::Interval intOne(1);

	int id = continuousProblem.getIDForStateVar(*(yyvsp[0].identifier));
	if(id >= 0)				// a state variable
	{
		(yyval.p_LTV_Term)->coefficient = intOne;
		(yyval.p_LTV_Term)->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*(yyvsp[0].identifier));
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *(yyvsp[0].identifier));
			(yyval.p_LTV_Term)->coefficient = range;
		}
		else		// not defined
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Symbol %s is not defined.", (*(yyvsp[0].identifier)).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	delete (yyvsp[0].identifier);
}
#line 9494 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 335:
#line 6996 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.p_LTV_Term) = new LTV_Term(*(yyvsp[-1].pUpoly), -1, -1, -1);

	delete (yyvsp[-1].pUpoly);
}
#line 9504 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 336:
#line 7011 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pUpoly) = (yyvsp[-2].pUpoly);
	(*(yyval.pUpoly)) += (*(yyvsp[0].pUpoly));

	delete (yyvsp[0].pUpoly);
}
#line 9515 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 337:
#line 7019 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pUpoly) = (yyvsp[-2].pUpoly);
	(*(yyval.pUpoly)) -= (*(yyvsp[0].pUpoly));

	delete (yyvsp[0].pUpoly);
}
#line 9526 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 338:
#line 7027 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pUpoly) = (yyvsp[-1].pUpoly); 
}
#line 9534 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 339:
#line 7032 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pUpoly) = (yyvsp[-2].pUpoly);
	(*(yyval.pUpoly)) *= (*(yyvsp[0].pUpoly));

	delete (yyvsp[0].pUpoly);
}
#line 9545 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 340:
#line 7040 "modelParser.y" /* yacc.c:1646  */
    {
	int exp = (int) (yyvsp[0].dblVal);

	if(exp == 0)
	{
		(yyval.pUpoly) = new flowstar::UnivariatePolynomial(1);
	}
	else
	{
		(yyval.pUpoly) = new flowstar::UnivariatePolynomial;
		(*(yyvsp[-2].pUpoly)).pow(*(yyval.pUpoly), exp);
	}

	delete (yyvsp[-2].pUpoly);
}
#line 9565 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 341:
#line 7057 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I(-1);
	(yyval.pUpoly) = (yyvsp[0].pUpoly);
	(*(yyval.pUpoly)) *= I;
}
#line 9575 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 342:
#line 7064 "modelParser.y" /* yacc.c:1646  */
    {
	std::string tVar("t");
	if((yyvsp[0].identifier)->compare(tVar) == 0)
	{
		(yyval.pUpoly) = new flowstar::UnivariatePolynomial(1, 1);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "The time variable should be denoted by t.");
		parseError(errMsg, lineNum);
		exit(1);
	}
}
#line 9594 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 343:
#line 7080 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I((yyvsp[-3].dblVal), (yyvsp[-1].dblVal));
	(yyval.pUpoly) = new flowstar::UnivariatePolynomial(I);
}
#line 9603 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 344:
#line 7086 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pUpoly) = new flowstar::UnivariatePolynomial((yyvsp[0].dblVal));
}
#line 9611 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 345:
#line 7098 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) += (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 9622 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 346:
#line 7106 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) -= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 9633 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 347:
#line 7114 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-1].poly); 
}
#line 9641 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 348:
#line 7119 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.poly) = (yyvsp[-2].poly);
	(*(yyval.poly)) *= (*(yyvsp[0].poly));

	delete (yyvsp[0].poly);
}
#line 9652 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 349:
#line 7127 "modelParser.y" /* yacc.c:1646  */
    {
	int exp = (int) (yyvsp[0].dblVal);

	if(exp == 0)
	{
		flowstar::Interval I(1);
		(yyval.poly) = new flowstar::Polynomial(I, flowstar::parsePolynomial.variables.size());
	}
	else
	{
		(yyval.poly) = new flowstar::Polynomial;
		(*(yyvsp[-2].poly)).pow(*(yyval.poly), exp);
	}

	delete (yyvsp[-2].poly);
}
#line 9673 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 350:
#line 7145 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I(-1);
	(yyval.poly) = (yyvsp[0].poly);
	(yyval.poly)->mul_assign(I);
}
#line 9683 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 351:
#line 7152 "modelParser.y" /* yacc.c:1646  */
    {
	int id = flowstar::parsePolynomial.variables.getIDForVar(*(yyvsp[0].identifier));

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Variable %s is not declared.", (*(yyvsp[0].identifier)).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	int numVars = flowstar::parsePolynomial.variables.size();
	flowstar::Interval I(1);

	std::vector<int> degrees;
	for(int i=0; i<numVars; ++i)
	{
		degrees.push_back(0);
	}

	degrees[id] = 1;
	flowstar::Monomial monomial(I, degrees);

	(yyval.poly) = new flowstar::Polynomial(monomial);

	delete (yyvsp[0].identifier);
}
#line 9715 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 352:
#line 7181 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = flowstar::parsePolynomial.variables.size();
	flowstar::Interval I((yyvsp[-3].dblVal), (yyvsp[-1].dblVal));
	(yyval.poly) = new flowstar::Polynomial(I, numVars);
}
#line 9725 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 353:
#line 7188 "modelParser.y" /* yacc.c:1646  */
    {
	int numVars = flowstar::parsePolynomial.variables.size();
	flowstar::Interval I((yyvsp[0].dblVal));
	(yyval.poly) = new flowstar::Polynomial(I, numVars);
}
#line 9735 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 354:
#line 7208 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[-2].pExpression);
	(*(yyval.pExpression)) += (*(yyvsp[0].pExpression));

	delete (yyvsp[0].pExpression);
}
#line 9746 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 355:
#line 7216 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[-2].pExpression);
	(*(yyval.pExpression)) -= (*(yyvsp[0].pExpression));

	delete (yyvsp[0].pExpression);
}
#line 9757 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 356:
#line 7224 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[-1].pExpression); 
}
#line 9765 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 357:
#line 7229 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[-2].pExpression);
	(*(yyval.pExpression)) *= (*(yyvsp[0].pExpression));

	delete (yyvsp[0].pExpression);
}
#line 9776 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 358:
#line 7237 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[-2].pExpression);
	(yyval.pExpression)->pow_assign((int)(yyvsp[0].dblVal));
}
#line 9785 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 359:
#line 7243 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[0].pExpression);
	(yyval.pExpression)->inv_assign();
}
#line 9794 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 360:
#line 7249 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = new flowstar::Expression_AST(*(yyvsp[0].identifier), flowstar::parseExpression.variables, 0);

	delete (yyvsp[0].identifier);
}
#line 9804 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 361:
#line 7256 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I((yyvsp[-3].dblVal), (yyvsp[-1].dblVal));
	(yyval.pExpression) = new flowstar::Expression_AST(I);
}
#line 9813 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 362:
#line 7262 "modelParser.y" /* yacc.c:1646  */
    {
	flowstar::Interval I((yyvsp[0].dblVal));
	(yyval.pExpression) = new flowstar::Expression_AST(I);
}
#line 9822 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 363:
#line 7268 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[-2].pExpression);
	(*(yyval.pExpression)) /= (*(yyvsp[0].pExpression));

	delete (yyvsp[0].pExpression);
}
#line 9833 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 364:
#line 7276 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[-1].pExpression);
	(yyval.pExpression)->exp_assign();
}
#line 9842 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 365:
#line 7282 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[-1].pExpression);
	(yyval.pExpression)->sin_assign();
}
#line 9851 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 366:
#line 7288 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[-1].pExpression);
	(yyval.pExpression)->cos_assign();
}
#line 9860 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 367:
#line 7294 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[-1].pExpression);
	(yyval.pExpression)->log_assign();
}
#line 9869 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 368:
#line 7300 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.pExpression) = (yyvsp[-1].pExpression);
	(yyval.pExpression)->sqrt_assign();
}
#line 9878 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 369:
#line 7313 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.piMatrix) = (yyvsp[-2].piMatrix);
	(yyval.piMatrix)->push_back(*(yyvsp[0].intVec));
	delete (yyvsp[0].intVec);
}
#line 9888 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 370:
#line 7320 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.piMatrix) = new std::vector<std::vector<flowstar::Interval> >(0);
	(yyval.piMatrix)->push_back(*(yyvsp[0].intVec));
	delete (yyvsp[0].intVec);
}
#line 9898 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 371:
#line 7328 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.intVec) = (yyvsp[-2].intVec);
	(yyval.intVec)->push_back((yyvsp[0].dblVal));
}
#line 9907 "modelParser.tab.c" /* yacc.c:1646  */
    break;

  case 372:
#line 7334 "modelParser.y" /* yacc.c:1646  */
    {
	(yyval.intVec) = new std::vector<flowstar::Interval>(0);
	(yyval.intVec)->push_back((yyvsp[0].dblVal));
}
#line 9916 "modelParser.tab.c" /* yacc.c:1646  */
    break;


#line 9920 "modelParser.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 7342 "modelParser.y" /* yacc.c:1906  */


int yyerror(const char * what)
{
	fprintf(stderr, "Error line %d: %s\n", lineNum, what);
	err = true;
	return 1;
}

int yyerror(std::string what)
{
	std::cerr << "Error line "<< lineNum << " " << what << std::endl;
	err = true;
	return 1;
}
