%{
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
%}

%union
{
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
}


%token <dblVal> NUM
%token <identifier> IDENT
%token STATEVAR TMVAR TM EQ GEQ LEQ ASSIGN END
%token MODE INIT BELONGSTO
%token POLYODE1 POLYODE2 POLYODE3
%token VISUALIZE PARAAGGREG INTAGGREG TMAGGREG
%token OUTPUT NOOUTPUT
%token CONTINUOUS HYBRID
%token SETTING
%token FIXEDST FIXEDORD ADAPTIVEST ADAPTIVEORD ORDER
%token MIN MAX
%token REMEST
%token INTERVAL OCTAGON GRID PLOT
%token QRPRECOND IDPRECOND
%token TIME
%token MODES JUMPS INV GUARD RESET START MAXJMPS
%token PRINTON PRINTOFF UNSAFESET
%token CONTINUOUSFLOW HYBRIDFLOW
%token TAYLOR_PICARD TAYLOR_REMAINDER TAYLOR_POLYNOMIAL NONPOLY_CENTER
%token EXP SIN COS LOG SQRT
%token NPODE_TAYLOR CUTOFF PRECISION
%token GNUPLOT MATLAB COMPUTATIONPATHS
%token LTIODE LTV_ODE PAR UNC
%token UNIVARIATE_POLY MULTIVARIATE_POLY
%token TIME_INV TIME_VAR STEP
%token TRUE FALSE
%token LINEARCONTINUOUSFLOW
%token EXPRESSION
%token MATRIX


%type <poly> polynomial
%type <poly> ODEpolynomial
%type <poly> constraint_polynomial
%type <poly> reset_polynomial
%type <poly> interval_polynomial
%type <p_LTI_Term> lti_ode_rhs_term
%type <p_LTI_Term> lti_ode_hybrid_rhs_term
%type <p_LTV_Term> ltv_ode_rhs_term
%type <p_LTV_Term> ltv_ode_hybrid_rhs_term
%type <tmVec> ode
%type <iVec> orders
%type <pFlowpipe> init
%type <resetMap> reset
%type <dVec> real_valued_vector
%type <dVecVec> real_valued_vectors
%type <dVec> vector_components
%type <vecConstraints> polynomial_constraints
%type <tmVec> taylor_model
%type <tmVec> interval_taylor_model
%type <intVec> taylor_model_domain
%type <intVec> intervals
%type <intVec> remainders
%type <ptm> non_polynomial_rhs_picard
%type <pint> non_polynomial_rhs_remainder
%type <poly> non_polynomial_rhs_no_remainder
%type <identifier> non_polynomial_rhs_string
%type <p_ODE_String> non_polynomial_rhs_center
%type <strVec> npode
%type <pNode> computation_path
%type <pUpoly> univariate_polynomial
%type <pVecFlowpipe> set_of_intervals
%type <intVal> range_contracted
%type <poly> multivariate_polynomial
%type <pExpression> ode_expression
%type <piMatrix> matrix_matlab
%type <intVec> vector_matlab




%left GEQ LEQ EQ 
%left '+' '-'
%left '*' '/'
%nonassoc uminus
%right '^'

%start model

%%

model: CONTINUOUS '{' continuous '}'
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
|
CONTINUOUS '{' continuous '}' unsafe_continuous_env
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
|
HYBRID '{' hybrid '}'
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
|
HYBRID '{' hybrid '}' unsafe_hybrid_env
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
|
stateVarDecls plotting ORDER NUM CUTOFF NUM output_env unsafe_continuous CONTINUOUSFLOW '{' tmVarDecls continuous_flowpipes '}'
{
	if($4 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	continuousProblem.globalMaxOrder = (int)$4;

	flowstar::Interval I(-$6,$6);
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
|
stateVarDecls plotting ORDER NUM CUTOFF NUM output_env CONTINUOUSFLOW '{' tmVarDecls continuous_flowpipes '}'
{
	if($6 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	continuousProblem.globalMaxOrder = (int)$4;

	flowstar::Interval I(-$6,$6);
	continuousProblem.cutoff_threshold = I;

	if(continuousProblem.bPlot)
	{
		continuousProblem.plot_2D(false);
	}
}
|
stateVarDecls plotting STEP NUM ORDER NUM CUTOFF NUM output_env TIME_INV '{' TIParDeclList '}' INIT '{' init_flowpipes '}' LINEARCONTINUOUSFLOW '{' linear_flowpipes '}'
{
	if($8 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	continuousProblem.step = $4;
	continuousProblem.globalMaxOrder = (int)$6;

	flowstar::Interval I(-$8,$8);
	continuousProblem.cutoff_threshold = I;

	if(continuousProblem.bPlot)
	{
		continuousProblem.plot_2D(false);
	}
}
|
stateVarDecls plotting STEP NUM ORDER NUM CUTOFF NUM output_env unsafe_continuous TIME_INV '{' TIParDeclList '}' INIT '{' init_flowpipes '}' LINEARCONTINUOUSFLOW '{' linear_flowpipes '}'
{
	if($8 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	continuousProblem.step = $4;
	continuousProblem.globalMaxOrder = (int)$6;

	flowstar::Interval I(-$8,$8);
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
|
stateVarDecls modeDecls COMPUTATIONPATHS '{' computation_paths '}' plotting output_env unsafe_hybrid HYBRIDFLOW '{' hybrid_flowpipes '}'
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
|
stateVarDecls modeDecls COMPUTATIONPATHS '{' computation_paths '}' plotting output_env HYBRIDFLOW '{' hybrid_flowpipes '}'
{
	if(hybridProblem.bPlot)
	{
		hybridProblem.plot_2D(false);
	}
}
|
TAYLOR_PICARD '{' non_polynomial_rhs_picard '}'
{
	$3->getExpansion(parseResult.expansion);
	parseResult.remainder = $3->getRemainder();
	delete $3;
}
|
TAYLOR_REMAINDER '{' non_polynomial_rhs_remainder '}'
{
	parseResult.remainder = (*$3);
	delete $3;
}
|
TAYLOR_POLYNOMIAL '{' non_polynomial_rhs_no_remainder '}'
{
	parseResult.expansion = (*$3);
	delete $3;
}
|
NONPOLY_CENTER '{' non_polynomial_rhs_center '}'
{
	parseResult.strExpansion = $3->ode;
	parseResult.constant = $3->constant;
	delete $3;
}
|
UNIVARIATE_POLY '{' univariate_polynomial '}'
{
	flowstar::up_parseresult = (*$3);
	delete $3;
}
|
MULTIVARIATE_POLY '{' multivariate_polynomial '}'
{
	flowstar::parsePolynomial.result = (*$3);
	delete $3;
}
|
EXPRESSION '{' ode_expression '}'
{
	flowstar::parseExpression.expression = (*$3);
	delete $3;
}
|
MATRIX '{' matrix_matlab '}'
{
	int rows = $3->size();
	int cols = 0;

	if(rows > 0)
	{
		cols = (*$3)[0].size();

		flowstar::iMatrix matrix(rows, cols);

		for(int i=0; i<rows; ++i)
		{
			for(int j=0; j<cols; ++j)
			{
				matrix[i][j] = (*$3)[i][j];
			}
		}

		flowstar::matrixParseSetting.result = matrix;
	}
	else
	{
		flowstar::iMatrix matrix;
		flowstar::matrixParseSetting.result = matrix;
	}

	delete $3;
}
;

output_env: OUTPUT IDENT
{
	strcpy(continuousProblem.outputFileName, $2->c_str());
	strcpy(hybridProblem.outputFileName, $2->c_str());
	continuousProblem.bDump = true;
	continuousProblem.bPlot = true;
	hybridProblem.bDump = true;
	hybridProblem.bPlot = true;
}
|
PLOT OUTPUT IDENT
{
	strcpy(continuousProblem.outputFileName, $3->c_str());
	strcpy(hybridProblem.outputFileName, $3->c_str());
	continuousProblem.bDump = false;
	continuousProblem.bPlot = true;
	hybridProblem.bDump = false;
	hybridProblem.bPlot = true;
}
|
TM OUTPUT IDENT
{
	strcpy(continuousProblem.outputFileName, $3->c_str());
	strcpy(hybridProblem.outputFileName, $3->c_str());
	continuousProblem.bDump = true;
	continuousProblem.bPlot = false;
	hybridProblem.bDump = true;
	hybridProblem.bPlot = false;
}
|
NOOUTPUT
{
	continuousProblem.bDump = false;
	continuousProblem.bPlot = false;
	hybridProblem.bDump = false;
	hybridProblem.bPlot = false;
}
;

unsafe_continuous_env: unsafe_continuous
{
}
;

unsafe_hybrid_env: unsafe_hybrid
{
}
;

range_contracted: TRUE
{
	$$ = 1;
}
|
FALSE
{
	$$ = 0;
}
|
{
	$$ = 0;
}
;


init_flowpipes: tmVarDecls2 initial_flowpipes
{
	flowstar::domainVarNames = varlist.varNames; 
}
;

linear_flowpipes: tmVarDecls2 linear_continuous_flowpipes
{
}
;

continuous_flowpipes: continuous_flowpipes '{' interval_taylor_model taylor_model_domain range_contracted '}'
{
	continuousProblem.flowpipesCompo.push_back(*$3);
	continuousProblem.domains.push_back(*$4);
	continuousProblem.flowpipes_safety.push_back(SAFE);

	if($5 == 1)
	{
		continuousProblem.flowpipes_contracted.push_back(true);
	}
	else
	{
		continuousProblem.flowpipes_contracted.push_back(false);
	}

	delete $3;
	delete $4;
}
|
'{' interval_taylor_model taylor_model_domain range_contracted '}'
{
	continuousProblem.flowpipesCompo.push_back(*$2);
	continuousProblem.domains.push_back(*$3);
	continuousProblem.flowpipes_safety.push_back(SAFE);

	if($4 == 1)
	{
		continuousProblem.flowpipes_contracted.push_back(true);
	}
	else
	{
		continuousProblem.flowpipes_contracted.push_back(false);
	}

	delete $2;
	delete $3;
}
;


initial_flowpipes: initial_flowpipes '{' interval_taylor_model taylor_model_domain '}'
{
	flowstar::Flowpipe initialSet;

	initialSet.tmvPre = *$3;
	initialSet.domain = *$4;

	continuousProblem.system.initialSets.push_back(initialSet);

	delete $3;
	delete $4;
}
|
'{' interval_taylor_model taylor_model_domain '}'
{
	flowstar::Flowpipe initialSet;

	initialSet.tmvPre = *$2;
	initialSet.domain = *$3;

	continuousProblem.system.initialSets.push_back(initialSet);

	delete $2;
	delete $3;
}
;

linear_continuous_flowpipes: linear_continuous_flowpipes '{' interval_taylor_model '}'
{
	continuousProblem.flowpipesCompo.push_back(*$3);
	int num = continuousProblem.system.initialSets.size();

	for(int i=0; i<num; ++i)
	{
		continuousProblem.flowpipes_safety.push_back(SAFE);
	}

	delete $3;
}
|
'{' interval_taylor_model '}'
{
	continuousProblem.flowpipesCompo.push_back(*$2);

	int num = continuousProblem.system.initialSets.size();

	for(int i=0; i<num; ++i)
	{
		continuousProblem.flowpipes_safety.push_back(SAFE);
	}

	delete $2;
}
;


modeDecls: modeDecls IDENT '{' '{' ORDER NUM '}' '{' CUTOFF NUM '}' '{' polynomial_constraints '}' '}'
{
	flowstar::TaylorModelVec tmvDummy;

	if($6 <= 0)
	{
		parseError("The order should be a positive number.", lineNum);
		exit(1);
	}

	if($10 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval I(-$10,$10);
	mode_local_setting.cutoff_threshold = I;
	mode_local_setting.globalMaxOrder = $6;
	
	hybridProblem.declareMode(*$2, tmvDummy, *$13, 0, mode_local_setting);

	delete $2;
	delete $13;
}
|
IDENT '{' '{' ORDER NUM '}' '{' CUTOFF NUM '}' '{' polynomial_constraints '}' '}'
{
	flowstar::TaylorModelVec tmvDummy;

	if($5 <= 0)
	{
		parseError("The order should be a positive number.", lineNum);
		exit(1);
	}

	if($9 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval I(-$9,$9);
	mode_local_setting.cutoff_threshold = I;
	mode_local_setting.globalMaxOrder = $5;

	hybridProblem.declareMode(*$1, tmvDummy, *$12, 0, mode_local_setting);

	delete $1;
	delete $12;
}
;

hybrid_flowpipes: hybrid_flowpipes IDENT '{' tmVarDecls continuous_flowpipes '}'
{
	int id = hybridProblem.getIDForMode(*$2);
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*$2).c_str());
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
|
IDENT '{' tmVarDecls continuous_flowpipes '}'
{
	int id = hybridProblem.getIDForMode(*$1);
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*$1).c_str());
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
;

computation_paths: computation_paths computation_path ';'
{
}
|
computation_path ';'
{
}
;

computation_path: computation_path '(' NUM ',' '[' NUM ',' NUM ']' ')' '-' '>' IDENT
{
	int id = hybridProblem.getIDForMode(*$13);
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*$13).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	std::list<flowstar::TreeNode *>::iterator iter = $$->children.begin();
	bool found = false;
	for(; iter!=$$->children.end(); ++iter)
	{
		if((*iter)->jumpID == $3 && (*iter)->modeID == id)
		{
			$$ = *iter;
			found = true;
			break;
		}
	}

	if(!found)
	{
		flowstar::Interval I($6, $8);
		flowstar::TreeNode *tmp = new flowstar::TreeNode((int)$3, id, I);
		tmp->parent = $$;
		$$->children.push_back(tmp);
		$$ = tmp;
	}

	delete $13;
}
|
IDENT
{
	int id = hybridProblem.getIDForMode(*$1);
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if(hybridProblem.traceTree == NULL)
	{
		flowstar::Interval intZero;
		hybridProblem.traceTree = new flowstar::TreeNode(0, id, intZero);
		$$ = hybridProblem.traceTree;
	}
	else
	{
		if(hybridProblem.traceTree->modeID == id)
		{
			$$ = hybridProblem.traceTree;
		}
		else
		{
			parseError("Invalid computation path.", lineNum);
			exit(1);
		}
	}

	delete $1;
}
;

print: PRINTON
{
	continuousProblem.bPrint = true;
	hybridProblem.bPrint = true;
}
|
PRINTOFF
{
	continuousProblem.bPrint = false;
	hybridProblem.bPrint = false;
}
;

unsafe_continuous: UNSAFESET '{' polynomial_constraints '}'
{
	continuousProblem.unsafeSet = *$3;
	delete $3;
}
;

unsafe_hybrid: UNSAFESET '{' hybrid_constraints '}'
{
}
;

hybrid_constraints: hybrid_constraints IDENT '{' polynomial_constraints '}'
{
	int id = hybridProblem.getIDForMode(*$2);
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*$2).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.unsafeSet[id] = *$4;
	hybridProblem.bVecUnderCheck[id] = true;
	delete $4;
}
|
{
	std::vector<flowstar::PolynomialConstraint> vecEmpty;
	for(int i=0; i<hybridProblem.modeNames.size(); ++i)
	{
		hybridProblem.unsafeSet.push_back(vecEmpty);
		hybridProblem.bVecUnderCheck.push_back(false);
	}
}
;

polynomial_constraints: polynomial_constraints constraint_polynomial LEQ NUM
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::Interval B($4);
	flowstar::PolynomialConstraint pc(*$2, B);
	$$->push_back(pc);

	delete $2;
}
|
polynomial_constraints constraint_polynomial GEQ NUM
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::Interval I(-1);
	$2->mul_assign(I);

	flowstar::Interval B(-$4);
	flowstar::PolynomialConstraint pc(*$2, B);
	$$->push_back(pc);

	delete $2;
}
|
polynomial_constraints constraint_polynomial EQ NUM
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::Interval B($4);
	flowstar::PolynomialConstraint pc1(*$2, B);
	$$->push_back(pc1);

	flowstar::Interval I(-1);
	$2->mul_assign(I);
	flowstar::Interval mB(-$4);
	flowstar::PolynomialConstraint pc2(*$2, mB);
	$$->push_back(pc2);

	delete $2;
}
|
polynomial_constraints constraint_polynomial BELONGSTO '[' NUM ',' NUM ']'
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	flowstar::PolynomialConstraint pc1(*$2, $7);
	$$->push_back(pc1);

	flowstar::Interval I(-1);
	$2->mul_assign(I);
	flowstar::PolynomialConstraint pc2(*$2, -$5);
	$$->push_back(pc2);

	delete $2;
}
|
polynomial_constraints constraint_polynomial LEQ IDENT
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	int id = continuousProblem.getIDForPar(*$4);
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *$4);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*$4).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::PolynomialConstraint pc(*$2, range);
	$$->push_back(pc);

	delete $2;
	delete $4;
}
|
polynomial_constraints constraint_polynomial GEQ IDENT
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	int id = continuousProblem.getIDForPar(*$4);
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *$4);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*$4).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::Interval I(-1);
	$2->mul_assign(I);
	range *= I;
	flowstar::PolynomialConstraint pc(*$2, range);
	$$->push_back(pc);

	delete $2;
	delete $4;
}
|
polynomial_constraints constraint_polynomial EQ IDENT
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	int id = continuousProblem.getIDForPar(*$4);
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *$4);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*$4).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::PolynomialConstraint pc1(*$2, range);
	$$->push_back(pc1);

	flowstar::Interval I(-1);
	$2->mul_assign(I);
	range *= I;
	flowstar::PolynomialConstraint pc2(*$2, range);
	$$->push_back(pc2);

	delete $2;
	delete $4;
}
|
polynomial_constraints constraint_polynomial BELONGSTO '[' IDENT ',' IDENT ']'
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	int id = continuousProblem.getIDForPar(*$7);
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *$7);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*$7).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::PolynomialConstraint pc1(*$2, range);
	$$->push_back(pc1);

	id = continuousProblem.getIDForPar(*$5);

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *$5);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*$5).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Interval I(-1);
	$2->mul_assign(I);
	range *= I;
	flowstar::PolynomialConstraint pc2(*$2, range);
	$$->push_back(pc2);

	delete $2;
	delete $5;
	delete $7;
}
|
{
	$$ = new std::vector<flowstar::PolynomialConstraint>(0);
}
;


/*
linear_constraints: linear_constraints linear_polynomial LEQ NUM
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::Interval B($4);
	flowstar::PolynomialConstraint pc(*$2, B);
	$$->push_back(pc);

	delete $2;
}
|
linear_constraints linear_polynomial GEQ NUM
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::Interval I(-1);
	$2->mul_assign(I);

	flowstar::Interval B(-$4);
	flowstar::PolynomialConstraint pc(*$2, B);
	$$->push_back(pc);

	delete $2;
}
|
linear_constraints linear_polynomial EQ NUM
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::Interval B($4);
	flowstar::PolynomialConstraint pc1(*$2, B);
	$$->push_back(pc1);

	flowstar::Interval I(-1);
	$2->mul_assign(I);
	flowstar::Interval mB(-$4);
	flowstar::PolynomialConstraint pc2(*$2, mB);
	$$->push_back(pc2);

	delete $2;
}
|
linear_constraints linear_polynomial BELONGSTO '[' NUM ',' NUM ']'
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	flowstar::PolynomialConstraint pc1(*$2, $7);
	$$->push_back(pc1);

	flowstar::Interval I(-1);
	$2->mul_assign(I);
	flowstar::PolynomialConstraint pc2(*$2, -$5);
	$$->push_back(pc2);

	delete $2;
}
|
linear_constraints linear_polynomial LEQ IDENT
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}
	
	int id = continuousProblem.getIDForPar(*$4);
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *$4);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*$4).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::PolynomialConstraint pc(*$2, range);
	$$->push_back(pc);

	delete $2;
	delete $4;
}
|
linear_constraints linear_polynomial GEQ IDENT
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	int id = continuousProblem.getIDForPar(*$4);
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *$4);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*$4).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::Interval I(-1);
	$2->mul_assign(I);
	range *= I;
	flowstar::PolynomialConstraint pc(*$2, range);
	$$->push_back(pc);

	delete $2;
	delete $4;
}
|
linear_constraints linear_polynomial EQ IDENT
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}

	int id = continuousProblem.getIDForPar(*$4);
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *$4);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*$4).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	$$ = $1;
	flowstar::PolynomialConstraint pc1(*$2, range);
	$$->push_back(pc1);

	flowstar::Interval I(-1);
	$2->mul_assign(I);
	range *= I;
	flowstar::PolynomialConstraint pc2(*$2, range);
	$$->push_back(pc2);

	delete $2;
	delete $4;
}
|
linear_constraints linear_polynomial BELONGSTO '[' IDENT ',' IDENT ']'
{
	if($2->degree() < 1)
	{
		parseError("Invalid constraint.", lineNum);
		exit(1);
	}
	
	int id = continuousProblem.getIDForPar(*$7);
	flowstar::Interval range;

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *$7);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*$7).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::PolynomialConstraint pc1(*$2, range);
	$$->push_back(pc1);

	id = continuousProblem.getIDForPar(*$5);

	if(id >= 0)
	{
		continuousProblem.getRangeForPar(range, *$5);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Parameter %s is not declared.", (*$5).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Interval I(-1);
	$2->mul_assign(I);
	range *= I;
	flowstar::PolynomialConstraint pc2(*$2, range);
	$$->push_back(pc2);

	delete $2;
	delete $5;
	delete $7;
}
|
{
	$$ = new std::vector<flowstar::PolynomialConstraint>(0);
}
;
*/


continuous: stateVarDecls SETTING '{' settings print '}' POLYODE1 '{' ode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$9, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$13);
		flowstar::ContinuousSystem system(*$9, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD;

		delete $13;
	}

	delete $9;
}
|
stateVarDecls SETTING '{' settings print '}' POLYODE2 '{' ode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$9, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LOW_DEGREE;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$13);
		flowstar::ContinuousSystem system(*$9, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LOW_DEGREE;

		delete $13;
	}

	delete $9;
}
|
stateVarDecls SETTING '{' settings print '}' POLYODE3 '{' ode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$9, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = HIGH_DEGREE;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$13);
		flowstar::ContinuousSystem system(*$9, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = HIGH_DEGREE;

		delete $13;
	}

	delete $9;
}
|
stateVarDecls SETTING '{' settings print '}' NPODE_TAYLOR '{' npode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$9, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$13);
		flowstar::ContinuousSystem system(*$9, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR;

		delete $13;
	}

	delete $9;
}
|
stateVarDecls SETTING '{' settings print '}' LTIODE '{' lti_env '}' INIT '{' init '}'
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
		vecFpTemp.push_back(*$13);
		flowstar::ContinuousSystem system(dyn_A, dyn_B, dyn_ti, dyn_tv, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTI;

		delete $13;
	}
}
|
stateVarDecls SETTING '{' settings print '}' LTV_ODE '{' ltv_env '}' INIT '{' init '}'
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
		vecFpTemp.push_back(*$13);
		flowstar::ContinuousSystem system(dyn_A_t, dyn_B_t, dyn_ti_t, dyn_tv_t, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTV;

		delete $13;
	}
}
|
stateVarDecls SETTING '{' settings print '}' POLYODE1 '{' NUM '}' '{' ode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$12, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD_SYMB;
		continuousProblem.max_remainder_queue = $9;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$16);
		flowstar::ContinuousSystem system(*$12, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD_SYMB;
		continuousProblem.max_remainder_queue = $9;

		delete $16;
	}

	delete $12;
}
|
stateVarDecls SETTING '{' settings print '}' NPODE_TAYLOR '{' NUM '}' '{' npode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$12, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR_SYMB;
		continuousProblem.max_remainder_queue = $9;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$16);
		flowstar::ContinuousSystem system(*$12, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR_SYMB;
		continuousProblem.max_remainder_queue = $9;

		delete $16;
	}

	delete $12;
}
|
stateVarDecls parDecls SETTING '{' settings print '}' POLYODE1 '{' ode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$10, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$14);
		flowstar::ContinuousSystem system(*$10, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD;

		delete $14;
	}

	delete $10;
}
|
stateVarDecls parDecls SETTING '{' settings print '}' POLYODE2 '{' ode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$10, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LOW_DEGREE;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$14);
		flowstar::ContinuousSystem system(*$10, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LOW_DEGREE;

		delete $14;
	}

	delete $10;
}
|
stateVarDecls parDecls SETTING '{' settings print '}' POLYODE3 '{' ode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$10, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = HIGH_DEGREE;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$14);
		flowstar::ContinuousSystem system(*$10, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = HIGH_DEGREE;

		delete $14;
	}

	delete $10;
}
|
stateVarDecls parDecls SETTING '{' settings print '}' NPODE_TAYLOR '{' npode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$10, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$14);
		flowstar::ContinuousSystem system(*$10, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR;

		delete $14;
	}

	delete $10;
}
|
stateVarDecls parDecls SETTING '{' settings print '}' LTIODE '{' lti_env '}' INIT '{' init '}'
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
		vecFpTemp.push_back(*$14);
		flowstar::ContinuousSystem system(dyn_A, dyn_B, dyn_ti, dyn_tv, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTI;

		delete $14;
	}
}
|
stateVarDecls parDecls SETTING '{' settings print '}' LTV_ODE '{' ltv_env '}' INIT '{' init '}'
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
		vecFpTemp.push_back(*$14);
		flowstar::ContinuousSystem system(dyn_A_t, dyn_B_t, dyn_ti_t, dyn_tv_t, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = LTV;

		delete $14;
	}
}
|
stateVarDecls parDecls SETTING '{' settings print '}' POLYODE1 '{' NUM '}' '{' ode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$13, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD_SYMB;
		continuousProblem.max_remainder_queue = $10;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$17);
		flowstar::ContinuousSystem system(*$13, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = ONLY_PICARD_SYMB;
		continuousProblem.max_remainder_queue = $10;

		delete $17;
	}

	delete $13;
}
|
stateVarDecls parDecls SETTING '{' settings print '}' NPODE_TAYLOR '{' NUM '}' '{' npode '}' INIT '{' init '}'
{
	if(initialSets.size() > 0)
	{
		flowstar::ContinuousSystem system(*$13, initialSets);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR_SYMB;
		continuousProblem.max_remainder_queue = $10;
	}
	else
	{
		std::vector<flowstar::Flowpipe> vecFpTemp;
		vecFpTemp.push_back(*$17);
		flowstar::ContinuousSystem system(*$13, vecFpTemp);
		continuousProblem.system = system;
		continuousProblem.integrationScheme = NONPOLY_TAYLOR_SYMB;
		continuousProblem.max_remainder_queue = $10;

		delete $17;
	}

	delete $13;
}
;

hybrid: stateVarDecls SETTING '{' settings MAXJMPS NUM print '}' MODES '{' modes '}' JUMPS '{' jumps '}' INIT '{' hybrid_init '}'
{
	if($6 < 0)
	{
		parseError("The maximum jump depth should be a nonnegative integer.", lineNum);
		exit(1);
	}

	hybridProblem.maxJumps = (int)$6;
}
|
stateVarDecls parDecls SETTING '{' settings MAXJMPS NUM print '}' MODES '{' modes '}' JUMPS '{' jumps '}' INIT '{' hybrid_init '}'
{
	if($7 < 0)
	{
		parseError("The maximum jump depth should be a nonnegative integer.", lineNum);
		exit(1);
	}

	hybridProblem.maxJumps = (int)$7;
}
;

hybrid_init: IDENT '{' intervals '}'
{
	int id = hybridProblem.getIDForMode(*$1);
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Interval intZero;
	flowstar::Flowpipe initialSet(*$3, intZero);
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

	delete $1;
	delete $3;
}
|
IDENT '{' tmVarDecls taylor_model taylor_model_domain '}'
{
	int id = hybridProblem.getIDForMode(*$1);
	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Flowpipe initialSet(*$4, *$5);
	hybridProblem.initialConfig(id, initialSet);

	delete $4;
	delete $5;
}
;

modes: modes IDENT local_setting '{' POLYODE1 '{' ode '}' INV '{' polynomial_constraints '}' '}'
{
	hybridProblem.declareMode(*$2, *$7, *$11, ONLY_PICARD, mode_local_setting);
	mode_local_setting.clear();

	delete $2;
	delete $7;
	delete $11;
}
|
IDENT local_setting '{' POLYODE1 '{' ode '}' INV '{' polynomial_constraints '}' '}'
{
	hybridProblem.declareMode(*$1, *$6, *$10, ONLY_PICARD, mode_local_setting);
	mode_local_setting.clear();

	delete $1;
	delete $6;
	delete $10;
}
|
modes IDENT local_setting '{' POLYODE2 '{' ode '}' INV '{' polynomial_constraints '}' '}'
{
	hybridProblem.declareMode(*$2, *$7, *$11, LOW_DEGREE, mode_local_setting);
	mode_local_setting.clear();

	delete $2;
	delete $7;
	delete $11;
}
|
IDENT local_setting '{' POLYODE2 '{' ode '}' INV '{' polynomial_constraints '}' '}'
{
	hybridProblem.declareMode(*$1, *$6, *$10, LOW_DEGREE, mode_local_setting);
	mode_local_setting.clear();

	delete $1;
	delete $6;
	delete $10;
}
|
modes IDENT local_setting '{' POLYODE3 '{' ode '}' INV '{' polynomial_constraints '}' '}'
{
	hybridProblem.declareMode(*$2, *$7, *$11, HIGH_DEGREE, mode_local_setting);
	mode_local_setting.clear();

	delete $2;
	delete $7;
	delete $11;
}
|
IDENT local_setting '{' POLYODE3 '{' ode '}' INV '{' polynomial_constraints '}' '}'
{
	hybridProblem.declareMode(*$1, *$6, *$10, HIGH_DEGREE, mode_local_setting);
	mode_local_setting.clear();

	delete $1;
	delete $6;
	delete $10;
}
|
modes IDENT local_setting '{' NPODE_TAYLOR '{' npode '}' INV '{' polynomial_constraints '}' '}'
{
	hybridProblem.declareMode(*$2, *$7, *$11, NONPOLY_TAYLOR, mode_local_setting);
	mode_local_setting.clear();

	delete $2;
	delete $7;
	delete $11;
}
|
IDENT local_setting '{' NPODE_TAYLOR '{' npode '}' INV '{' polynomial_constraints '}' '}'
{
	hybridProblem.declareMode(*$1, *$6, *$10, NONPOLY_TAYLOR, mode_local_setting);
	mode_local_setting.clear();

	delete $1;
	delete $6;
	delete $10;
}
|
modes IDENT local_setting '{' LTIODE '{' lti_ode_hybrid '}' INV '{' polynomial_constraints '}' '}'
{
	flowstar::LTI_Dynamics lti_dyn(dyn_A, dyn_B);
	hybridProblem.declareMode(*$2, lti_dyn, *$11, mode_local_setting);
	mode_local_setting.clear();

	delete $2;
	delete $11;
}
|
IDENT local_setting '{' LTIODE '{' lti_ode_hybrid '}' INV '{' polynomial_constraints '}' '}'
{
	flowstar::LTI_Dynamics lti_dyn(dyn_A, dyn_B);
	hybridProblem.declareMode(*$1, lti_dyn, *$10, mode_local_setting);
	mode_local_setting.clear();

	delete $1;
	delete $10;
}
|
modes IDENT local_setting '{' LTV_ODE '{' ltv_ode_hybrid '}' INV '{' polynomial_constraints '}' '}'
{
	flowstar::LTV_Dynamics ltv_dyn(dyn_A_t, dyn_B_t);
	hybridProblem.declareMode(*$2, ltv_dyn, *$11, mode_local_setting);
	mode_local_setting.clear();

	delete $2;
	delete $11;
}
|
IDENT local_setting '{' LTV_ODE '{' ltv_ode_hybrid '}' INV '{' polynomial_constraints '}' '}'
{
	flowstar::LTV_Dynamics ltv_dyn(dyn_A_t, dyn_B_t);
	hybridProblem.declareMode(*$1, ltv_dyn, *$10, mode_local_setting);
	mode_local_setting.clear();

	delete $1;
	delete $10;
}
;



local_setting: SETTING '{' parameters  '}'
{
}
|
{
}
;

parameters: parameters FIXEDST NUM
{
	if($3 <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	mode_local_setting.bAdaptiveSteps = false;
	mode_local_setting.step = $3;
}
|
parameters REMEST NUM
{
	if($3 <= 0)
	{
		parseError("Remainder estimation should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval I(-$3, $3);

	for(int i=0; i<continuousProblem.stateVarNames.size(); ++i)
	{
		mode_local_setting.estimation.push_back(I);
	}
}
|
parameters REMEST '{' remainders '}'
{
	mode_local_setting.estimation = *$4;
}
|
parameters QRPRECOND
{
	mode_local_setting.precondition = QR_PRE;
}
|
parameters IDPRECOND
{
	mode_local_setting.precondition = ID_PRE;
}
|
parameters FIXEDORD NUM
{
	int order = (int)$3;

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
|
parameters CUTOFF NUM
{
	if($3 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval I(-$3, $3);
	mode_local_setting.cutoff_threshold = I;
}
|
parameters ADAPTIVEST '{' MIN NUM ',' MAX NUM '}'
{
	if($5 <= 0 || $8 <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	if($5 > $8)
	{
		parseError("MIN step should be no larger than MAX step.", lineNum);
		exit(1);
	}

	mode_local_setting.bAdaptiveSteps = true;
	mode_local_setting.step = $8;
	mode_local_setting.miniStep = $5;
	mode_local_setting.bAdaptiveOrders = false;
}
|
parameters ADAPTIVEORD '{' MIN NUM ',' MAX NUM '}'
{
	int minOrder = (int)$5;
	int maxOrder = (int)$8;

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
|
parameters FIXEDORD '{' orders '}'
{
	mode_local_setting.bAdaptiveOrders = false;
	mode_local_setting.orderType = MULTI;
	mode_local_setting.orders = *$4;

	for(int i=0; i<$4->size(); ++i)
	{
		if((*$4)[i] <= 0)
		{
			parseError("Orders should be larger than zero.", lineNum);
			exit(1);
		}
	}

	int maxOrder = (*$4)[0];
	for(int i=1; i<$4->size(); ++i)
	{
		if(maxOrder < (*$4)[i])
		{
			maxOrder = (*$4)[i];
		}
	}

	mode_local_setting.globalMaxOrder = maxOrder;
}
|
parameters ADAPTIVEORD '{' MIN '{' orders '}' ',' MAX '{' orders '}' '}'
{
	mode_local_setting.bAdaptiveSteps = false;
	mode_local_setting.bAdaptiveOrders = true;
	mode_local_setting.orderType = MULTI;
	mode_local_setting.orders = *$6;
	mode_local_setting.maxOrders = *$11;

	if($6->size() != $11->size())
	{
		parseError("Orders are not properly specified.", lineNum);
		exit(1);
	}

	for(int i=0; i<$11->size(); ++i)
	{
		if((*$6)[i] <= 0 || (*$11)[i] <= 0)
		{
			parseError("Orders should be larger than zero.", lineNum);
			exit(1);
		}

		if((*$6)[i] > (*$11)[i])
		{
			parseError("MAX order should be no smaller than MIN order.", lineNum);
			exit(1);
		}
	}

	int maxOrder = (*$11)[0];
	for(int i=1; i<$11->size(); ++i)
	{
		if(maxOrder < (*$11)[i])
		{
			maxOrder = (*$11)[i];
		}
	}

	mode_local_setting.globalMaxOrder = maxOrder;
}
|
{
}
;

jumps: jumps IDENT '-' '>' IDENT GUARD '{' polynomial_constraints '}' RESET '{' reset '}' PARAAGGREG '{' real_valued_vectors '}'
{
	int startID = hybridProblem.getIDForMode(*$2);
	if(startID < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*$2).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	int endID = hybridProblem.getIDForMode(*$5);
	if(endID < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*$5).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if($16->size() > 0)
	{
		hybridProblem.declareTrans(startID, endID, *$8, *$12, PARA_AGGREG, *$16);
	}
	else
	{
		std::vector<std::vector<double> > emptyVec;
		hybridProblem.declareTrans(startID, endID, *$8, *$12, PARA_AGGREG, emptyVec);
	}
}
|
jumps IDENT '-' '>' IDENT GUARD '{' polynomial_constraints '}' RESET '{' reset '}' INTAGGREG
{
	int startID = hybridProblem.getIDForMode(*$2);
	if(startID < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*$2).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	int endID = hybridProblem.getIDForMode(*$5);
	if(endID < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Mode %s has not been declared.", (*$5).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	std::vector<std::vector<double> > empty;
	hybridProblem.declareTrans(startID, endID, *$8, *$12, INTERVAL_AGGREG, empty);
}
|
{
	hybridProblem.declareTrans();
}
;

reset: reset IDENT '\'' ASSIGN reset_polynomial '+' '[' NUM ',' NUM ']'
{
	$$ = $1;

	int id = hybridProblem.getIDForStateVar(*$2);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$2).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if($8 > $10)
	{
		parseError("Invalid remainder interval.", lineNum);
		exit(1);
	}

	flowstar::Interval I($8, $10);
	flowstar::TaylorModel tmTemp(*$5, I);
	$$->tmvReset.tms[id] = tmTemp;

	delete $5;
}
|
reset IDENT '\'' ASSIGN reset_polynomial
{
	$$ = $1;

	int id = hybridProblem.getIDForStateVar(*$2);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$2).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Interval intZero;
	flowstar::TaylorModel tmTemp(*$5, intZero);
	$$->tmvReset.tms[id] = tmTemp;

	delete $5;
}
|
{
	int numVars = hybridProblem.stateVarNames.size();

	flowstar::Matrix coefficients_identity_reset(numVars, numVars+1);

	for(int i=0; i<numVars; ++i)
	{
		coefficients_identity_reset.set(1, i, i+1);
	}

	flowstar::TaylorModelVec tmvReset(coefficients_identity_reset);

	$$ = new flowstar::ResetMap(tmvReset);
}
;

real_valued_vectors: real_valued_vectors real_valued_vector
{
	$$->push_back(*$2);
	delete $2;
}
|
{
	$$ = new std::vector<std::vector<double> >(0);
}
;

real_valued_vector: '[' vector_components ']'
{
	int rangeDim = $2->size();

	if(rangeDim != hybridProblem.stateVarNames.size())
	{
		parseError("The vector dimension should be equivalent to the system dimension.", lineNum);
		exit(1);
	}

	$$ = new std::vector<double>(0);

	for(int i=0; i<rangeDim; ++i)
	{
		$$->push_back(0);
	}

	bool bZero = true;
	for(int i=0; i<rangeDim; ++i)
	{
		if((*$2)[i] < -THRESHOLD_LOW || (*$2)[i] > THRESHOLD_LOW)
		{
			if(bZero)
			{
				bZero = false;
			}
		}

		(*$$)[i] = (*$2)[i];
	}

	if(bZero)
	{
		parseError("A template vector should not be zero.", lineNum);
		exit(1);
	}

	delete $2;
}
;

vector_components: vector_components ',' IDENT ':' NUM
{
	int id = hybridProblem.getIDForStateVar(*$3);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$3).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	$$ = $1;
	(*$$)[id] = $5;
	delete $3;
}
|
IDENT ':' NUM
{
	int num = hybridProblem.stateVarNames.size();
	$$ = new std::vector<double>(num);

	int id = hybridProblem.getIDForStateVar(*$1);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(*$$)[id] = $3;
	delete $1;
}
;

stateVarDecls: STATEVAR stateIdDeclList
{
}
;

stateIdDeclList: stateIdDeclList ',' IDENT
{
	if(!continuousProblem.declareStateVar(*$3))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s has already been declared.", (*$3).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.declareStateVar(*$3);
	delete $3;
}
|
IDENT
{
	if(!continuousProblem.declareStateVar(*$1))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s has already been declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.declareStateVar(*$1);
	delete $1;
}
;



parDecls: PAR '{' parDeclList '}'
{
}
;

parDeclList: parDeclList IDENT EQ NUM
{
	if(continuousProblem.getIDForStateVar(*$2) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*$2).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		flowstar::Interval range($4);

		if(!continuousProblem.declarePar(*$2, range))
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Parameter %s has already been declared.", (*$2).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		hybridProblem.declarePar(*$2, range);
	}
}
|
IDENT EQ NUM
{
	if(continuousProblem.getIDForStateVar(*$1) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		flowstar::Interval range($3);

		if(!continuousProblem.declarePar(*$1, range))
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Parameter %s has already been declared.", (*$1).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		hybridProblem.declarePar(*$1, range);
	}
}
;


TIParDeclList: TIParDeclList ',' IDENT
{
	if(continuousProblem.getIDForStateVar(*$3) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*$3).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		if(continuousProblem.getIDForPar(*$3) != -1)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "%s has already been declared as a parameter.", (*$3).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
		else
		{
			if(!continuousProblem.declareTIPar(*$3))
			{
				char errMsg[MSG_SIZE];
				sprintf(errMsg, "Time-invariant uncertainty %s has already been declared.", (*$3).c_str());
				parseError(errMsg, lineNum);
				exit(1);
			}
		}
	}
}
|
IDENT
{
	if(continuousProblem.getIDForStateVar(*$1) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		if(continuousProblem.getIDForPar(*$1) != -1)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "%s has already been declared as a parameter.", (*$1).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
		else
		{
			if(!continuousProblem.declareTIPar(*$1))
			{
				char errMsg[MSG_SIZE];
				sprintf(errMsg, "Uncertainty %s has already been declared.", (*$1).c_str());
				parseError(errMsg, lineNum);
				exit(1);
			}
		}
	}
}
|
{
}
;


TVParDeclList: TVParDeclList ',' IDENT
{
	if(continuousProblem.getIDForStateVar(*$3) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*$3).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		if(continuousProblem.getIDForPar(*$3) != -1)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "%s has already been declared as a parameter.", (*$3).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
		else
		{
			if(!continuousProblem.declareTVPar(*$3))
			{
				char errMsg[MSG_SIZE];
				sprintf(errMsg, "Uncertainty %s has already been declared.", (*$3).c_str());
				parseError(errMsg, lineNum);
				exit(1);
			}
		}

//		hybridProblem.declareUnc(*$2, range);
	}
}
|
IDENT
{
	if(continuousProblem.getIDForStateVar(*$1) != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		if(continuousProblem.getIDForPar(*$1) != -1)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "%s has already been declared as a parameter.", (*$1).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
		else
		{
			if(!continuousProblem.declareTVPar(*$1))
			{
				char errMsg[MSG_SIZE];
				sprintf(errMsg, "Uncertainty %s has already been declared.", (*$1).c_str());
				parseError(errMsg, lineNum);
				exit(1);
			}
		}

//		hybridProblem.declareUnc(*$1, range);
	}
}
|
{
}
;

settings: FIXEDST NUM TIME NUM remainder_estimation precondition plotting FIXEDORD NUM CUTOFF NUM PRECISION NUM output_env
{
	if($2 <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	int order = (int)$9;

	if(order <= 0)
	{
		parseError("Orders should be larger than zero.", lineNum);
		exit(1);
	}

	continuousProblem.bAdaptiveSteps = false;
	continuousProblem.step = $2;
	continuousProblem.time = $4;
	continuousProblem.bAdaptiveOrders = false;
	continuousProblem.orderType = UNIFORM;
	continuousProblem.orders.push_back(order);
	continuousProblem.globalMaxOrder = order;

	hybridProblem.global_setting.bAdaptiveSteps = false;
	hybridProblem.global_setting.step = $2;
	hybridProblem.time = $4;
	hybridProblem.global_setting.bAdaptiveOrders = false;
	hybridProblem.global_setting.orderType = UNIFORM;
	hybridProblem.global_setting.orders.push_back(order);
	hybridProblem.global_setting.globalMaxOrder = order;

	if($11 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-$11,$11);
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)$13;
}
|
FIXEDST NUM TIME NUM remainder_estimation precondition plotting ADAPTIVEORD '{' MIN NUM ',' MAX NUM '}' CUTOFF NUM PRECISION NUM output_env
{
	if($2 <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	int minOrder = (int)$11;
	int maxOrder = (int)$14;

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
	continuousProblem.step = $2;
	continuousProblem.time = $4;
	continuousProblem.bAdaptiveOrders = true;
	continuousProblem.orderType = UNIFORM;
	continuousProblem.orders.push_back(minOrder);
	continuousProblem.maxOrders.push_back(maxOrder);
	continuousProblem.globalMaxOrder = maxOrder;

	hybridProblem.global_setting.bAdaptiveSteps = false;
	hybridProblem.global_setting.step = $2;
	hybridProblem.time = $4;
	hybridProblem.global_setting.bAdaptiveOrders = true;
	hybridProblem.global_setting.orderType = UNIFORM;
	hybridProblem.global_setting.orders.push_back(minOrder);
	hybridProblem.global_setting.maxOrders.push_back(maxOrder);
	hybridProblem.global_setting.globalMaxOrder = maxOrder;

	if($17 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-$17,$17);
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)$19;
}
|
FIXEDST NUM TIME NUM remainder_estimation precondition plotting FIXEDORD '{' orders '}' CUTOFF NUM PRECISION NUM output_env
{
	if($2 <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	continuousProblem.bAdaptiveSteps = false;
	continuousProblem.step = $2;
	continuousProblem.time = $4;
	continuousProblem.bAdaptiveOrders = false;
	continuousProblem.orderType = MULTI;
	continuousProblem.orders = *$10;

	hybridProblem.global_setting.bAdaptiveSteps = false;
	hybridProblem.global_setting.step = $2;
	hybridProblem.time = $4;
	hybridProblem.global_setting.bAdaptiveOrders = false;
	hybridProblem.global_setting.orderType = MULTI;
	hybridProblem.global_setting.orders = *$10;

	for(int i=0; i<$10->size(); ++i)
	{
		if((*$10)[i] <= 0)
		{
			parseError("Orders should be larger than zero.", lineNum);
			exit(1);
		}
	}

	int maxOrder = (*$10)[0];
	for(int i=1; i<$10->size(); ++i)
	{
		if(maxOrder < (*$10)[i])
		{
			maxOrder = (*$10)[i];
		}
	}

	continuousProblem.globalMaxOrder = maxOrder;
	hybridProblem.global_setting.globalMaxOrder = maxOrder;

	if($13 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-$13,$13);
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)$15;

	delete $10;
}
|
FIXEDST NUM TIME NUM remainder_estimation precondition plotting ADAPTIVEORD '{' MIN '{' orders '}' ',' MAX '{' orders '}' '}' CUTOFF NUM PRECISION NUM output_env
{
	if($2 <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	continuousProblem.bAdaptiveSteps = false;
	continuousProblem.step = $2;
	continuousProblem.time = $4;
	continuousProblem.bAdaptiveOrders = true;
	continuousProblem.orderType = MULTI;
	continuousProblem.orders = *$12;
	continuousProblem.maxOrders = *$17;

	hybridProblem.global_setting.bAdaptiveSteps = false;
	hybridProblem.global_setting.step = $2;
	hybridProblem.time = $4;
	hybridProblem.global_setting.bAdaptiveOrders = true;
	hybridProblem.global_setting.orderType = MULTI;
	hybridProblem.global_setting.orders = *$12;
	hybridProblem.global_setting.maxOrders = *$17;

	if($12->size() != $17->size())
	{
		parseError("Orders are not properly specified.", lineNum);
		exit(1);
	}

	for(int i=0; i<$17->size(); ++i)
	{
		if((*$12)[i] <= 0 || (*$17)[i] <= 0)
		{
			parseError("Orders should be larger than zero.", lineNum);
			exit(1);
		}

		if((*$12)[i] > (*$17)[i])
		{
			parseError("MAX order should be no smaller than MIN order.", lineNum);
			exit(1);
		}
	}

	int maxOrder = (*$17)[0];
	for(int i=1; i<$17->size(); ++i)
	{
		if(maxOrder < (*$17)[i])
		{
			maxOrder = (*$17)[i];
		}
	}

	continuousProblem.globalMaxOrder = maxOrder;
	hybridProblem.global_setting.globalMaxOrder = maxOrder;

	if($21 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-$21,$21);
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)$23;

	delete $12;
	delete $17;
}
|
ADAPTIVEST '{' MIN NUM ',' MAX NUM '}' TIME NUM remainder_estimation precondition plotting FIXEDORD NUM CUTOFF NUM PRECISION NUM output_env
{
	if($4 <= 0 || $7 <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	if($4 > $7)
	{
		parseError("MIN step should be no larger than MAX step.", lineNum);
		exit(1);
	}

	int order = (int)$15;

	if(order <= 0)
	{
		parseError("Orders should be larger than zero.", lineNum);
		exit(1);
	}

	continuousProblem.bAdaptiveSteps = true;
	continuousProblem.step = $7;
	continuousProblem.miniStep = $4;
	continuousProblem.time = $10;
	continuousProblem.bAdaptiveOrders = false;
	continuousProblem.orderType = UNIFORM;
	continuousProblem.orders.push_back(order);
	continuousProblem.globalMaxOrder = order;

	hybridProblem.global_setting.bAdaptiveSteps = true;
	hybridProblem.global_setting.step = $7;
	hybridProblem.global_setting.miniStep = $4;
	hybridProblem.time = $10;
	hybridProblem.global_setting.bAdaptiveOrders = false;
	hybridProblem.global_setting.orderType = UNIFORM;
	hybridProblem.global_setting.orders.push_back(order);
	hybridProblem.global_setting.globalMaxOrder = order;

	if($17 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-$17,$17);
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)$19;
}
|
ADAPTIVEST '{' MIN NUM ',' MAX NUM '}' TIME NUM remainder_estimation precondition plotting FIXEDORD '{' orders '}' CUTOFF NUM PRECISION NUM output_env
{
	if($4 <= 0 || $7 <= 0)
	{
		parseError("A time step-size should be larger than 0", lineNum);
		exit(1);
	}

	if($4 > $7)
	{
		parseError("MIN step should be no larger than MAX step.", lineNum);
		exit(1);
	}

	for(int i=0; i<$16->size(); ++i)
	{
		if((*$16)[i] <= 0)
		{
			parseError("Orders should be larger than zero.", lineNum);
			exit(1);
		}
	}

	continuousProblem.bAdaptiveSteps = true;
	continuousProblem.step = $7;
	continuousProblem.miniStep = $4;
	continuousProblem.time = $10;
	continuousProblem.bAdaptiveOrders = false;
	continuousProblem.orderType = MULTI;
	continuousProblem.orders = *$16;

	hybridProblem.global_setting.bAdaptiveSteps = true;
	hybridProblem.global_setting.step = $7;
	hybridProblem.global_setting.miniStep = $4;
	hybridProblem.time = $10;
	hybridProblem.global_setting.bAdaptiveOrders = false;
	hybridProblem.global_setting.orderType = MULTI;
	hybridProblem.global_setting.orders = *$16;

	int maxOrder = (*$16)[0];
	for(int i=1; i<$16->size(); ++i)
	{
		if(maxOrder < (*$16)[i])
		{
			maxOrder = (*$16)[i];
		}
	}
	
	continuousProblem.globalMaxOrder = maxOrder;
	hybridProblem.global_setting.globalMaxOrder = maxOrder;

	if($19 <= 0)
	{
		parseError("The cutoff threshold should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval cutoff_threshold(-$19,$19);
	continuousProblem.cutoff_threshold = cutoff_threshold;
	hybridProblem.global_setting.cutoff_threshold = cutoff_threshold;

	intervalNumPrecision = (int)$21;

	delete $16;
}
;

remainder_estimation: REMEST NUM
{
	if($2 <= 0)
	{
		parseError("Remainder estimation should be a positive number.", lineNum);
		exit(1);
	}

	flowstar::Interval I(-$2, $2);

	for(int i=0; i<continuousProblem.stateVarNames.size(); ++i)
	{
		continuousProblem.estimation.push_back(I);
		hybridProblem.global_setting.estimation.push_back(I);
	}
}
|
REMEST '{' remainders '}'
{
	for(int i=0; i<$3->size(); ++i)
	{
		if((*$3)[i].inf() >= (*$3)[i].sup() - THRESHOLD_LOW)
		{
			parseError("Invalid remainder estimation.", lineNum);
			exit(1);
		}
	}

	continuousProblem.estimation = *$3;
	hybridProblem.global_setting.estimation = *$3;
	delete $3;
}
;

remainders: remainders ',' IDENT ':' '[' NUM ',' NUM ']'
{
	$$ = $1;
	int id = continuousProblem.getIDForStateVar(*$3);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$3).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if($6 >= $8)
	{
		parseError("Invalid remainder estimation.", lineNum);
		exit(1);
	}

	flowstar::Interval I($6,$8);
	(*$$)[id] = I;
	delete $3;
}
|
IDENT ':' '[' NUM ',' NUM ']'
{
	int numVars = continuousProblem.stateVarNames.size();
	$$ = new std::vector<flowstar::Interval>(numVars);

	int id = continuousProblem.getIDForStateVar(*$1);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if($4 >= $6)
	{
		parseError("Invalid remainder estimation.", lineNum);
		exit(1);
	}

	flowstar::Interval I($4,$6);
	(*$$)[id] = I;
	delete $1;
}
;

orders: orders ',' IDENT ':' NUM
{
	$$ = $1;
	int id = continuousProblem.getIDForStateVar(*$3);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$3).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(*$$)[id] = (int)$5;
	delete $3;
}
|
IDENT ':' NUM
{
	int numVars = continuousProblem.stateVarNames.size();
	$$ = new std::vector<int>(numVars);
	for(int i=0; i<numVars; ++i)
	{
		(*$$)[i] = 0;
	}

	int id = continuousProblem.getIDForStateVar(*$1);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(*$$)[id] = (int)$3;
	delete $1;
}
;

precondition: QRPRECOND
{
	continuousProblem.precondition = QR_PRE;
	hybridProblem.global_setting.precondition = QR_PRE;
}
|
IDPRECOND
{
	continuousProblem.precondition = ID_PRE;
	hybridProblem.global_setting.precondition = ID_PRE;
}
;

plotting: GNUPLOT INTERVAL IDENT ',' IDENT
{
	int x = continuousProblem.getIDForStateVar(*$3);
	int y = continuousProblem.getIDForStateVar(*$5);
	std::string tVar("t");

	if(x < 0)
	{
		if($3->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$3).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if($5->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$5).c_str());
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

	delete $3;
	delete $5;
}
|
GNUPLOT OCTAGON IDENT ',' IDENT
{
	int x = continuousProblem.getIDForStateVar(*$3);
	int y = continuousProblem.getIDForStateVar(*$5);
	std::string tVar("t");

	if(x < 0)
	{
		if($3->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State Variable %s is not declared.", (*$3).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if($5->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$5).c_str());
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

	delete $3;
	delete $5;
}
|
GNUPLOT GRID NUM IDENT ',' IDENT
{
	int x = continuousProblem.getIDForStateVar(*$4);
	int y = continuousProblem.getIDForStateVar(*$6);
	std::string tVar("t");

	if(x < 0)
	{
		if($4->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$4).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if($6->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$6).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	continuousProblem.outputAxes.push_back(x);
	continuousProblem.outputAxes.push_back(y);
	continuousProblem.plotSetting = PLOT_GRID;
	continuousProblem.numSections = (int)$3;
	continuousProblem.plotFormat = PLOT_GNUPLOT;

	hybridProblem.outputAxes.push_back(x);
	hybridProblem.outputAxes.push_back(y);
	hybridProblem.plotSetting = PLOT_GRID;
	hybridProblem.numSections = (int)$3;
	hybridProblem.plotFormat = PLOT_GNUPLOT;

	delete $4;
	delete $6;
}
|
MATLAB INTERVAL IDENT ',' IDENT
{
	int x = continuousProblem.getIDForStateVar(*$3);
	int y = continuousProblem.getIDForStateVar(*$5);
	std::string tVar("t");

	if(x < 0)
	{
		if($3->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$3).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if($5->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$5).c_str());
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

	delete $3;
	delete $5;
}
|
MATLAB OCTAGON IDENT ',' IDENT
{
	int x = continuousProblem.getIDForStateVar(*$3);
	int y = continuousProblem.getIDForStateVar(*$5);
	std::string tVar("t");

	if(x < 0)
	{
		if($3->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State Variable %s is not declared.", (*$3).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if($5->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$5).c_str());
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

	delete $3;
	delete $5;
}
|
MATLAB GRID NUM IDENT ',' IDENT
{
	int x = continuousProblem.getIDForStateVar(*$4);
	int y = continuousProblem.getIDForStateVar(*$6);
	std::string tVar("t");

	if(x < 0)
	{
		if($4->compare(tVar) == 0)
		{
			x = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$4).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	if(y < 0)
	{
		if($6->compare(tVar) == 0)
		{
			y = -1;
		}
		else
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$6).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	continuousProblem.outputAxes.push_back(x);
	continuousProblem.outputAxes.push_back(y);
	continuousProblem.plotSetting = PLOT_GRID;
	continuousProblem.numSections = (int)$3;
	continuousProblem.plotFormat = PLOT_MATLAB;

	hybridProblem.outputAxes.push_back(x);
	hybridProblem.outputAxes.push_back(y);
	hybridProblem.plotSetting = PLOT_GRID;
	hybridProblem.numSections = (int)$3;
	hybridProblem.plotFormat = PLOT_MATLAB;

	delete $4;
	delete $6;
}
;

init: tmVarDecls taylor_model taylor_model_domain
{
	$$ = new flowstar::Flowpipe(*$2, *$3);

	delete $2;
	delete $3;
}
|
intervals
{
	flowstar::Interval intZero;
	$$ = new flowstar::Flowpipe(*$1, intZero);

	delete $1;
}
|
set_of_intervals
{
	initialSets = *$1;
	$$ = NULL;

	delete $1;
}
;

set_of_intervals: set_of_intervals '{' intervals '}'
{
	flowstar::Interval intZero;
	flowstar::Flowpipe flowpipe(*$3, intZero);

	(*$$).push_back(flowpipe);

	delete $3;
}
|
'{' intervals '}'
{
	$$ = new std::vector<flowstar::Flowpipe>(1);

	flowstar::Interval intZero;
	flowstar::Flowpipe flowpipe(*$2, intZero);
	(*$$)[0] = flowpipe;

	delete $2;
}
;

tmVarDecls: TMVAR tmIdDeclList
{
}
;

tmIdDeclList: tmIdDeclList ',' IDENT
{
	int id = continuousProblem.getIDForStateVar(*$3);

	if(id != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*$3).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	id = continuousProblem.getIDForPar(*$3);
	
	if(id != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a parameter.", (*$3).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if(!continuousProblem.declareTMVar(*$3))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "TM variable %s has already been declared.", (*$3).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.declareTMVar(*$3);
	delete $3;
}
|
IDENT
{
	std::string tVar("local_t");
	continuousProblem.declareTMVar(tVar);
	hybridProblem.declareTMVar(tVar);

	int id = continuousProblem.getIDForStateVar(*$1);

	if(id != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a state variable.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	id = continuousProblem.getIDForPar(*$1);
	
	if(id != -1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "%s has already been declared as a parameter.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if(!continuousProblem.declareTMVar(*$1))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "TM variable %s has already been declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	hybridProblem.declareTMVar(*$1);
	delete $1;
}
;


tmVarDecls2: TMVAR tmIdDeclList2
{
}
;

tmIdDeclList2: tmIdDeclList2 ',' IDENT
{
	if(!varlist.declareVar(*$3))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Variable %s has already been declared.", (*$3).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	delete $3;
}
|
IDENT
{
	varlist.clear();

	std::string tVar("local_t");
	varlist.declareVar(tVar);

	if(!varlist.declareVar(*$1))
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Variable %s has already been declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	delete $1;
}
;


taylor_model: taylor_model IDENT EQ polynomial '+' '[' NUM ',' NUM ']'
{
	int id = continuousProblem.getIDForStateVar(*$2);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$2).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if($7 > $9)
	{
		parseError("Invalid interval remainder.", lineNum);
		exit(1);
	}

	flowstar::Interval I($7,$9);
	flowstar::TaylorModel tmTemp(*$4, I);
	$$ = $1;
	$$->tms[id] = tmTemp;

	delete $2;
	delete $4;
}
|
{
	flowstar::TaylorModel tmEmpty;
	$$ = new flowstar::TaylorModelVec;

	for(int i=0; i<continuousProblem.stateVarNames.size(); ++i)
	{
		$$->tms.push_back(tmEmpty);
	}
}
;

taylor_model_domain: taylor_model_domain IDENT BELONGSTO '[' NUM ',' NUM ']'
{
	if(continuousProblem.tmVarNames.size() == 0)
	{
		int id = varlist.getIDForVar(*$2);

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Variable %s is not declared.", (*$2).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		if($5 > $7)
		{
			parseError("Invalid interval.", lineNum);
			exit(1);
		}

		flowstar::Interval I($5,$7);
		$$ = $1;
		(*$$)[id] = I;
	}
	else
	{
		int id = continuousProblem.getIDForTMVar(*$2);

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "TM variable %s is not declared.", (*$2).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		if($5 > $7)
		{
			parseError("Invalid interval.", lineNum);
			exit(1);
		}

		flowstar::Interval I($5,$7);
		$$ = $1;
		(*$$)[id] = I;
	}

	delete $2;
}
|
IDENT BELONGSTO '[' NUM ',' NUM ']'
{
	if(continuousProblem.tmVarNames.size() == 0)
	{
		$$ = new std::vector<flowstar::Interval>( varlist.varNames.size() );

		flowstar::Interval intZero;
		(*$$)[0] = intZero;

		int id = varlist.getIDForVar(*$1);

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Variable %s is not declared.", (*$1).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		if($4 > $6)
		{
			parseError("Invalid interval.", lineNum);
			exit(1);
		}

		flowstar::Interval I($4,$6);
		(*$$)[id] = I;
	}
	else
	{
		$$ = new std::vector<flowstar::Interval>( continuousProblem.tmVarNames.size() );

		flowstar::Interval intZero;
		(*$$)[0] = intZero;

		int id = continuousProblem.getIDForTMVar(*$1);

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "TM variable %s is not declared.", (*$1).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		if($4 > $6)
		{
			parseError("Invalid interval.", lineNum);
			exit(1);
		}

		flowstar::Interval I($4,$6);
		(*$$)[id] = I;
	}

	delete $1;
}
;

intervals: intervals IDENT BELONGSTO '[' NUM ',' NUM ']'
{
	int id = continuousProblem.getIDForStateVar(*$2);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$2).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if($5 > $7)
	{
		parseError("Invalid interval.", lineNum);
		exit(1);
	}

	flowstar::Interval I($5,$7);
	$$ = $1;
	(*$$)[id] = I;

	delete $2;
}
|
{
	int numVars = continuousProblem.stateVarNames.size();
	$$ = new std::vector<flowstar::Interval>(numVars);

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
;

ode: ode IDENT '\'' EQ ODEpolynomial
{
	$$ = $1;

	int id = continuousProblem.getIDForStateVar(*$2);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$2).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Interval intZero;
	flowstar::TaylorModel tmTemp(*$5, intZero);
	$$->tms[id] = tmTemp;

	delete $2;
	delete $5;
}
|
{
	int numVars = continuousProblem.stateVarNames.size();

	$$ = new flowstar::TaylorModelVec;
	flowstar::TaylorModel tmTemp;
	flowstar::Interval intZero;

	for(int i=0; i<numVars; ++i)
	{
		$$->tms.push_back(tmTemp);
	}
}
;

npode: npode IDENT '\'' EQ non_polynomial_rhs_string
{
	$$ = $1;

	int id = continuousProblem.getIDForStateVar(*$2);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$2).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	(*$$)[id] = (*$5);

	delete $2;
	delete $5;
}
|
{
	int numVars = continuousProblem.stateVarNames.size();
	$$ = new std::vector<std::string>;

	std::string empty;
	flowstar::Interval intZero;

	for(int i=0; i<numVars; ++i)
	{
		$$->push_back(empty);
	}
}
;


lti_env: TIME_INV '{' TIParDeclList '}' TIME_VAR '{' TVParDeclList '}' '{' lti_ode '}'
{
}
|
TIME_INV '{' TIParDeclList '}' '{' lti_ode '}'
{
}
|
TIME_VAR '{' TVParDeclList '}' '{' lti_ode '}'
{
}
|
lti_ode
{
}
;


lti_ode: lti_ode IDENT '\'' EQ lti_ode_rhs
{
	int id = continuousProblem.getIDForStateVar(*$2);
	flowstar::Interval intZero;

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$2).c_str());
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

	delete $2;
}
|
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
;


lti_ode_hybrid: lti_ode_hybrid IDENT '\'' EQ lti_ode_hybrid_rhs
{
	int id = continuousProblem.getIDForStateVar(*$2);
	flowstar::Interval intZero;

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$2).c_str());
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
	delete $2;
}
|
{
	int numVars = (int)hybridProblem.stateVarNames.size();

	flowstar::iMatrix im_A(numVars, numVars), im_B(numVars, 1);
	flowstar::iMatrix im_A_row(1, numVars), im_B_row(1,1);

	dyn_A = im_A;
	dyn_B = im_B;

	dyn_A_row = im_A_row;
	dyn_B_row = im_B_row;
}
;



ltv_env: TIME_INV '{' TIParDeclList '}' TIME_VAR '{' TVParDeclList '}' '{' ltv_ode '}'
{
	continuousProblem.maxNumSteps = -1;
}
|
TIME_INV '{' TIParDeclList '}' TIME_VAR NUM '{' TVParDeclList '}' '{' ltv_ode '}'
{
	continuousProblem.maxNumSteps = $6;
}
|
TIME_INV '{' TIParDeclList '}' '{' ltv_ode '}'
{
	continuousProblem.maxNumSteps = -1;
}
|
TIME_VAR '{' TVParDeclList '}' '{' ltv_ode '}'
{
	continuousProblem.maxNumSteps = -1;
}
|
TIME_VAR NUM '{' TVParDeclList '}' '{' ltv_ode '}'
{
	continuousProblem.maxNumSteps = $2;
}
|
ltv_ode
{
	continuousProblem.maxNumSteps = -1;
}
;


ltv_ode: ltv_ode IDENT '\'' EQ ltv_ode_rhs
{
	int id = continuousProblem.getIDForStateVar(*$2);
	flowstar::UnivariatePolynomial polyZero;

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$2).c_str());
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

	delete $2;
}
|
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
;


ltv_ode_hybrid: ltv_ode_hybrid IDENT '\'' EQ ltv_ode_hybrid_rhs
{
	int id = continuousProblem.getIDForStateVar(*$2);
	flowstar::UnivariatePolynomial polyZero;

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$2).c_str());
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

	delete $2;
}
|
{
	int numVars = (int)continuousProblem.stateVarNames.size();

	flowstar::upMatrix upm_A_t(numVars, numVars), upm_B_t(numVars, 1);
	flowstar::upMatrix upm_A_t_row(1, numVars), upm_B_t_row(1,1);

	dyn_A_t = upm_A_t;
	dyn_B_t = upm_B_t;

	dyn_A_t_row = upm_A_t_row;
	dyn_B_t_row = upm_B_t_row;
}
;













// only used in Taylor model declaration
polynomial: polynomial '+' polynomial
{
	$$ = $1;
	(*$$) += (*$3);

	delete $3;
}
|
polynomial '-' polynomial
{
	$$ = $1;
	(*$$) -= (*$3);

	delete $3;
}
|
'(' polynomial ')'
{
	$$ = $2; 
}
|
polynomial '*' polynomial
{
	$$ = $1;
	(*$$) *= (*$3);

	delete $3;
}
|
polynomial '^' NUM
{
	int exp = (int) $3;

	if(exp == 0)
	{
		flowstar::Interval I(1);
		$$ = new flowstar::Polynomial(I, continuousProblem.tmVarNames.size());
	}
	else
	{
		$$ = new flowstar::Polynomial;
		(*$1).pow(*$$, exp);
	}

	delete $1;
}
|
'-' polynomial %prec uminus
{
	flowstar::Interval I(-1);
	$$ = $2;
	$$->mul_assign(I);
}
|
IDENT
{
	int id = continuousProblem.getIDForTMVar(*$1);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "TM variable %s is not declared.", (*$1).c_str());
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

	$$ = new flowstar::Polynomial(monomial);
	delete $1;
}
|
NUM
{
	int numVars = continuousProblem.tmVarNames.size();
	flowstar::Interval I($1);
	$$ = new flowstar::Polynomial(I, numVars);
}
;











ODEpolynomial: ODEpolynomial '+' ODEpolynomial
{
	$$ = $1;
	(*$$) += (*$3);

	delete $3;
}
|
ODEpolynomial '-' ODEpolynomial
{
	$$ = $1;
	(*$$) -= (*$3);

	delete $3;
}
|
'(' ODEpolynomial ')'
{
	$$ = $2; 
}
|
ODEpolynomial '*' ODEpolynomial
{
	$$ = $1;
	(*$$) *= (*$3);

	delete $3;
}
|
ODEpolynomial '^' NUM
{
	int exp = (int) $3;

	if(exp == 0)
	{
		flowstar::Interval I(1);
		$$ = new flowstar::Polynomial(I, continuousProblem.stateVarNames.size()+1);
	}
	else
	{
		$$ = new flowstar::Polynomial;
		(*$1).pow(*$$, exp);
	}

	delete $1;
}
|
'-' ODEpolynomial %prec uminus
{
	flowstar::Interval I(-1);
	$$ = $2;
	$$->mul_assign(I);
}
|
IDENT
{
	int id = continuousProblem.getIDForPar(*$1);

	if(id >= 0)
	{
		flowstar::Interval range;
		continuousProblem.getRangeForPar(range, *$1);

		int numVars = continuousProblem.stateVarNames.size()+1;
		$$ = new flowstar::Polynomial(range, numVars);
	}
	else
	{
		id = continuousProblem.getIDForStateVar(*$1);

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
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

		$$ = new flowstar::Polynomial(monomial);
	}

	delete $1;
}
|
'[' NUM ',' NUM ']'
{
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I($2, $4);
	$$ = new flowstar::Polynomial(I, numVars);
}
|
NUM
{
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I($1);
	$$ = new flowstar::Polynomial(I, numVars);
}
;



constraint_polynomial: constraint_polynomial '+' constraint_polynomial
{
	$$ = $1;
	(*$$) += (*$3);

	delete $3;
}
|
constraint_polynomial '-' constraint_polynomial
{
	$$ = $1;
	(*$$) -= (*$3);

	delete $3;
}
|
'(' constraint_polynomial ')'
{
	$$ = $2; 
}
|
constraint_polynomial '*' constraint_polynomial
{
	$$ = $1;
	(*$$) *= (*$3);

	delete $3;
}
|
constraint_polynomial '^' NUM
{
	int exp = (int) $3;

	if(exp == 0)
	{
		flowstar::Interval I(1);
		$$ = new flowstar::Polynomial(I, continuousProblem.stateVarNames.size()+1);
	}
	else
	{
		$$ = new flowstar::Polynomial;
		(*$1).pow(*$$, exp);
	}

	delete $1;
}
|
'-' constraint_polynomial %prec uminus
{
	flowstar::Interval I(-1);
	$$ = $2;
	$$->mul_assign(I);
}
|
IDENT
{
	int id = continuousProblem.getIDForPar(*$1);

	if(id >= 0)
	{
		flowstar::Interval range;
		continuousProblem.getRangeForPar(range, *$1);

		int numVars = continuousProblem.stateVarNames.size()+1;
		$$ = new flowstar::Polynomial(range, numVars);
	}
	else
	{
		id = continuousProblem.getIDForStateVar(*$1);

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
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

		$$ = new flowstar::Polynomial(monomial);
	}

	delete $1;
}
|
NUM
{
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I($1);
	$$ = new flowstar::Polynomial(I, numVars);
}
;




reset_polynomial: reset_polynomial '+' reset_polynomial
{
	$$ = $1;
	(*$$) += (*$3);

	delete $3;
}
|
reset_polynomial '-' reset_polynomial
{
	$$ = $1;
	(*$$) -= (*$3);

	delete $3;
}
|
'(' reset_polynomial ')'
{
	$$ = $2; 
}
|
reset_polynomial '*' reset_polynomial
{
	$$ = $1;
	(*$$) *= (*$3);

	delete $3;
}
|
reset_polynomial '^' NUM
{
	int exp = (int) $3;

	if(exp == 0)
	{
		flowstar::Interval I(1);
		$$ = new flowstar::Polynomial(I, continuousProblem.stateVarNames.size()+1);
	}
	else
	{
		$$ = new flowstar::Polynomial;
		(*$1).pow(*$$, exp);
	}

	delete $1;
}
|
'-' reset_polynomial %prec uminus
{
	flowstar::Interval I(-1);
	$$ = $2;
	$$->mul_assign(I);
}
|
IDENT
{
	int id = continuousProblem.getIDForPar(*$1);

	if(id >= 0)
	{
		flowstar::Interval range;
		continuousProblem.getRangeForPar(range, *$1);

		int numVars = continuousProblem.stateVarNames.size()+1;
		$$ = new flowstar::Polynomial(range, numVars);
	}
	else
	{
		id = continuousProblem.getIDForStateVar(*$1);

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
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

		$$ = new flowstar::Polynomial(monomial);
	}

	delete $1;
}
|
NUM
{
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I($1);
	$$ = new flowstar::Polynomial(I, numVars);
}
;





interval_taylor_model: interval_taylor_model IDENT EQ interval_polynomial '+' '[' NUM ',' NUM ']'
{
	int id = continuousProblem.getIDForStateVar(*$2);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$2).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if($7 > $9)
	{
		parseError("Invalid interval remainder.", lineNum);
		exit(1);
	}

	flowstar::Interval I($7,$9);
	flowstar::TaylorModel tmTemp(*$4, I);
	$$ = $1;
	$$->tms[id] = tmTemp;

	delete $2;
	delete $4;
}
|
IDENT EQ interval_polynomial '+' '[' NUM ',' NUM ']'
{
	flowstar::TaylorModel tmEmpty;
	$$ = new flowstar::TaylorModelVec;

	for(int i=0; i<continuousProblem.stateVarNames.size(); ++i)
	{
		$$->tms.push_back(tmEmpty);
	}

	int id = continuousProblem.getIDForStateVar(*$1);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	if($6 > $8)
	{
		parseError("Invalid interval remainder.", lineNum);
		exit(1);
	}

	flowstar::Interval I($6,$8);
	flowstar::TaylorModel tmTemp(*$3, I);

	$$->tms[id] = tmTemp;

	delete $1;
	delete $3;
}
;










interval_polynomial: interval_polynomial '+' interval_polynomial
{
	$$ = $1;
	(*$$) += (*$3);

	delete $3;
}
|
interval_polynomial '-' interval_polynomial
{
	$$ = $1;
	(*$$) -= (*$3);

	delete $3;
}
|
'(' interval_polynomial ')'
{
	$$ = $2; 
}
|
interval_polynomial '*' interval_polynomial
{
	$$ = $1;
	(*$$) *= (*$3);

	delete $3;
}
|
interval_polynomial '^' NUM
{
	int exp = (int) $3;

	if(exp == 0)
	{
		flowstar::Interval I(1);
		$$ = new flowstar::Polynomial(I, continuousProblem.tmVarNames.size());
	}
	else
	{
		$$ = new flowstar::Polynomial;
		(*$1).pow(*$$, exp);
	}

	delete $1;
}
|
'-' interval_polynomial %prec uminus
{
	flowstar::Interval I(-1);
	$$ = $2;
	$$->mul_assign(I);
}
|
IDENT
{
	if(continuousProblem.tmVarNames.size() == 0)
	{
		int id = varlist.getIDForVar(*$1);

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Variable %s is not declared.", (*$1).c_str());
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

		$$ = new flowstar::Polynomial(monomial);
	}
	else
	{
		int id = continuousProblem.getIDForTMVar(*$1);

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "TM variable %s is not declared.", (*$1).c_str());
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

		$$ = new flowstar::Polynomial(monomial);
	}

	delete $1;
}
|
'[' NUM ',' NUM ']'
{
	if(continuousProblem.tmVarNames.size() == 0)
	{
		int numVars = varlist.varNames.size();
		flowstar::Interval I($2, $4);
		$$ = new flowstar::Polynomial(I, numVars);
	}
	else
	{
		int numVars = continuousProblem.tmVarNames.size();
		flowstar::Interval I($2, $4);
		$$ = new flowstar::Polynomial(I, numVars);
	}
}
|
NUM
{
	if(continuousProblem.tmVarNames.size() == 0)
	{
		int numVars = varlist.varNames.size();
		flowstar::Interval I($1);
		$$ = new flowstar::Polynomial(I, numVars);
	}
	else
	{
		int numVars = continuousProblem.tmVarNames.size();
		flowstar::Interval I($1);
		$$ = new flowstar::Polynomial(I, numVars);
	}
}
;




















non_polynomial_rhs_picard: non_polynomial_rhs_picard '+' non_polynomial_rhs_picard
{
	$$ = $1;
	$1->add_assign(*$3);
	delete $3;
}
|
non_polynomial_rhs_picard '-' non_polynomial_rhs_picard
{
	$$ = $1;
	$1->sub_assign(*$3);
	delete $3;
}
|
non_polynomial_rhs_picard '*' non_polynomial_rhs_picard
{
	$$ = $1;

	flowstar::Interval intPoly1, intPoly2, intTrunc;

	$3->polyRangeNormal(intPoly2, parseSetting.step_exp_table);
	$1->mul_insert_ctrunc_normal_assign(intPoly1, intTrunc, *$3, intPoly2, parseSetting.step_exp_table, parseSetting.order, parseSetting.cutoff_threshold);

	parseSetting.ranges.push_back(intPoly1);
	parseSetting.ranges.push_back(intPoly2);
	parseSetting.ranges.push_back(intTrunc);

	delete $3;
}
|
'(' non_polynomial_rhs_picard ')'
{
	$$ = $2;
}
|
non_polynomial_rhs_picard '/' non_polynomial_rhs_picard
{
	flowstar::TaylorModel tmTemp;
	$3->rec_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	$$ = $1;

	flowstar::Interval intPoly1, intPoly2, intTrunc;

	tmTemp.polyRangeNormal(intPoly2, parseSetting.step_exp_table);
	$1->mul_insert_ctrunc_normal_assign(intPoly1, intTrunc, tmTemp, intPoly2, parseSetting.step_exp_table, parseSetting.order, parseSetting.cutoff_threshold);

	parseSetting.ranges.push_back(intPoly1);
	parseSetting.ranges.push_back(intPoly2);
	parseSetting.ranges.push_back(intTrunc);

	delete $3;
}
|
EXP '(' non_polynomial_rhs_picard ')'
{
	flowstar::TaylorModel tmTemp;
	$3->exp_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	*$3 = tmTemp;
	$$ = $3;
}
|
SIN '(' non_polynomial_rhs_picard ')'
{
	flowstar::TaylorModel tmTemp;
	$3->sin_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	*$3 = tmTemp;
	$$ = $3;
}
|
COS '(' non_polynomial_rhs_picard ')'
{
	flowstar::TaylorModel tmTemp;
	$3->cos_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	*$3 = tmTemp;
	$$ = $3;
}
|
LOG '(' non_polynomial_rhs_picard ')'
{
	flowstar::TaylorModel tmTemp;
	$3->log_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	*$3 = tmTemp;
	$$ = $3;
}
|
SQRT '(' non_polynomial_rhs_picard ')'
{
	flowstar::TaylorModel tmTemp;
	$3->sqrt_taylor(tmTemp, parseSetting.ranges, parseSetting.step_exp_table, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	*$3 = tmTemp;
	$$ = $3;
}
|
non_polynomial_rhs_picard '^' NUM
{
	int exp = (int)$3;

	if(exp == 0)
	{
		flowstar::Interval I(1);
		$$ = new flowstar::TaylorModel(I, continuousProblem.stateVarNames.size()+1);
	}
	else
	{
		flowstar::TaylorModel res = *$1;
		flowstar::TaylorModel pow = *$1;
		
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
		
		$$ = new flowstar::TaylorModel(res);
	}

	delete $1;
}
|
'-' non_polynomial_rhs_picard %prec uminus
{
	flowstar::Interval I(-1);
	$$ = $2;
	$$->mul_assign(I);
}
|
IDENT
{
	int id = continuousProblem.getIDForStateVar(*$1);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	$$ = new flowstar::TaylorModel;
	(*$$) = parseSetting.flowpipe.tms[id];

	flowstar::Interval intTemp;
	(*$$).expansion.ctrunc_normal(intTemp, parseSetting.step_exp_table, parseSetting.order);
	parseSetting.ranges.push_back(intTemp);
	(*$$).remainder += intTemp;

	delete $1;
}
|
'[' NUM ',' NUM ']'
{
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I($2, $4);
	$$ = new flowstar::TaylorModel(I, numVars);
}
;



























non_polynomial_rhs_remainder: non_polynomial_rhs_remainder '+' non_polynomial_rhs_remainder
{
	$$ = $1;
	(*$$) += (*$3);
	delete $3;
}
|
non_polynomial_rhs_remainder '-' non_polynomial_rhs_remainder
{
	$$ = $1;
	(*$$) -= (*$3);
	delete $3;
}
|
non_polynomial_rhs_remainder '*' non_polynomial_rhs_remainder
{
	$$ = new flowstar::Interval;

	(*$$) = (*parseSetting.iterRange) * (*$3);
	++parseSetting.iterRange;
	(*$$) += (*parseSetting.iterRange) * (*$1);
	(*$$) += (*$1) * (*$3);
	++parseSetting.iterRange;
	(*$$) += (*parseSetting.iterRange);
	++parseSetting.iterRange;

	delete $1;
	delete $3;
}
|
'(' non_polynomial_rhs_remainder ')'
{
	$$ = $2;
}
|
non_polynomial_rhs_remainder '/' non_polynomial_rhs_remainder
{
	flowstar::Interval intTemp;
	rec_taylor_only_remainder(intTemp, *$3, parseSetting.iterRange, parseSetting.order);

	$$ = new flowstar::Interval;

	(*$$) = (*parseSetting.iterRange) * intTemp;
	++parseSetting.iterRange;
	(*$$) += (*parseSetting.iterRange) * (*$1);
	(*$$) += (*$1) * intTemp;
	++parseSetting.iterRange;
	(*$$) += (*parseSetting.iterRange);
	++parseSetting.iterRange;

	delete $1;
	delete $3;
}
|
EXP '(' non_polynomial_rhs_remainder ')'
{
	flowstar::Interval intTemp;
	exp_taylor_only_remainder(intTemp, *$3, parseSetting.iterRange, parseSetting.order);

	(*$3) = intTemp;
	$$ = $3;
}
|
SIN '(' non_polynomial_rhs_remainder ')'
{
	flowstar::Interval intTemp;
	sin_taylor_only_remainder(intTemp, *$3, parseSetting.iterRange, parseSetting.order);

	(*$3) = intTemp;
	$$ = $3;
}
|
COS '(' non_polynomial_rhs_remainder ')'
{
	flowstar::Interval intTemp;
	cos_taylor_only_remainder(intTemp, *$3, parseSetting.iterRange, parseSetting.order);

	(*$3) = intTemp;
	$$ = $3;
}
|
LOG '(' non_polynomial_rhs_remainder ')'
{
	flowstar::Interval intTemp;
	log_taylor_only_remainder(intTemp, *$3, parseSetting.iterRange, parseSetting.order);

	(*$3) = intTemp;
	$$ = $3;
}
|
SQRT '(' non_polynomial_rhs_remainder ')'
{
	flowstar::Interval intTemp;
	sqrt_taylor_only_remainder(intTemp, *$3, parseSetting.iterRange, parseSetting.order);

	(*$3) = intTemp;
	$$ = $3;
}
|
non_polynomial_rhs_remainder '^' NUM
{
	int exp = (int)$3;

	if(exp == 0)
	{
		flowstar::Interval intZero;
		(*$1) = intZero;
		$$ = $1;
	}
	else
	{
		flowstar::Interval res(*$1);
		flowstar::Interval pow(*$1);
		
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
		
		$$ = new flowstar::Interval(res);
	}

	delete $1;
}
|
'-' non_polynomial_rhs_remainder %prec uminus
{
	$$ = $2;
	$$->inv_assign();
}
|
IDENT
{
	int id = continuousProblem.getIDForStateVar(*$1);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	$$ = new flowstar::Interval;
	(*$$) = parseSetting.flowpipe.tms[id].getRemainder();
	
	(*$$) += (*parseSetting.iterRange);
	++parseSetting.iterRange;

	delete $1;
}
|
'[' NUM ',' NUM ']'
{
	$$ = new flowstar::Interval;
}
;






















non_polynomial_rhs_no_remainder: non_polynomial_rhs_no_remainder '+' non_polynomial_rhs_no_remainder
{
	$$ = $1;
	(*$$) += (*$3);

	delete $3;
}
|
non_polynomial_rhs_no_remainder '-' non_polynomial_rhs_no_remainder
{
	$$ = $1;
	(*$$) -= (*$3);

	delete $3;
}
|
non_polynomial_rhs_no_remainder '*' non_polynomial_rhs_no_remainder
{
	$$ = $1;
	(*$$) *= (*$3);
	$$->nctrunc(parseSetting.order);
	$$->cutoff(parseSetting.cutoff_threshold);

	delete $3;
}
|
'(' non_polynomial_rhs_no_remainder ')'
{
	$$ = $2;
}
|
non_polynomial_rhs_no_remainder '/' non_polynomial_rhs_no_remainder
{
	flowstar::Polynomial polyTemp;
	$3->rec_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*$1) *= polyTemp;
	$1->nctrunc(parseSetting.order);
	$$ = $1;
	$$->cutoff(parseSetting.cutoff_threshold);

	delete $3;
}
|
EXP '(' non_polynomial_rhs_no_remainder ')'
{
	flowstar::Polynomial polyTemp;
	$3->exp_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*$3) = polyTemp;
	$$ = $3;
}
|
SIN '(' non_polynomial_rhs_no_remainder ')'
{
	flowstar::Polynomial polyTemp;
	$3->sin_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*$3) = polyTemp;
	$$ = $3;
}
|
COS '(' non_polynomial_rhs_no_remainder ')'
{
	flowstar::Polynomial polyTemp;
	$3->cos_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*$3) = polyTemp;
	$$ = $3;
}
|
LOG '(' non_polynomial_rhs_no_remainder ')'
{
	flowstar::Polynomial polyTemp;
	$3->log_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*$3) = polyTemp;
	$$ = $3;
}
|
SQRT '(' non_polynomial_rhs_no_remainder ')'
{
	flowstar::Polynomial polyTemp;
	$3->sqrt_taylor(polyTemp, continuousProblem.stateVarNames.size()+1, parseSetting.order, parseSetting.cutoff_threshold);

	(*$3) = polyTemp;
	$$ = $3;
}
|
non_polynomial_rhs_no_remainder '^' NUM
{
	int exp = (int) $3;
	
	$$ = new flowstar::Polynomial;
	
	(*$1).pow(*$$, exp, parseSetting.order);
	$$->cutoff(parseSetting.cutoff_threshold);
}
|
'-' non_polynomial_rhs_no_remainder %prec uminus
{
	$$ = $2;
	$$->inv_assign();
}
|
IDENT
{
	int id = continuousProblem.getIDForStateVar(*$1);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	$$ = new flowstar::Polynomial;
	parseSetting.flowpipe.tms[id].getExpansion(*$$);
	
	(*$$).nctrunc(parseSetting.order);
}
|
'[' NUM ',' NUM ']'
{
	flowstar::Interval I($2, $4);
	$$ = new flowstar::Polynomial(I, continuousProblem.stateVarNames.size()+1);
}
;






















non_polynomial_rhs_string: non_polynomial_rhs_string '+' non_polynomial_rhs_string
{
	(*$1) += ' ';
	(*$1) += '+';
	(*$1) += ' ';
	(*$1) += (*$3);

	$$ = $1;
	delete $3;
}
|
non_polynomial_rhs_string '-' non_polynomial_rhs_string
{
	(*$1) += ' ';
	(*$1) += '-';
	(*$1) += ' ';
	(*$1) += (*$3);

	$$ = $1;
	delete $3;
}
|
non_polynomial_rhs_string '*' non_polynomial_rhs_string
{
	(*$1) += ' ';
	(*$1) += '*';
	(*$1) += ' ';
	(*$1) += (*$3);

	$$ = $1;
	delete $3;
}
|
'(' non_polynomial_rhs_string ')'
{
	std::string str;
	str += '(';
	str += (*$2);
	str += ')';
	(*$2) = str;

	$$ = $2;
}
|
non_polynomial_rhs_string '/' non_polynomial_rhs_string
{
	(*$1) += ' ';
	(*$1) += '/';
	(*$1) += ' ';
	(*$1) += (*$3);

	$$ = $1;
	delete $3;
}
|
EXP '(' non_polynomial_rhs_string ')'
{
	std::string str("exp");
	str += '(';
	str += (*$3);
	str += ')';
	(*$3) = str;

	$$ = $3;
}
|
SIN '(' non_polynomial_rhs_string ')'
{
	std::string str("sin");
	str += '(';
	str += (*$3);
	str += ')';
	(*$3) = str;

	$$ = $3;
}
|
COS '(' non_polynomial_rhs_string ')'
{
	std::string str("cos");
	str += '(';
	str += (*$3);
	str += ')';
	(*$3) = str;

	$$ = $3;
}
|
LOG '(' non_polynomial_rhs_string ')'
{
	std::string str("log");
	str += '(';
	str += (*$3);
	str += ')';
	(*$3) = str;

	$$ = $3;
}
|
SQRT '(' non_polynomial_rhs_string ')'
{
	std::string str("sqrt");
	str += '(';
	str += (*$3);
	str += ')';
	(*$3) = str;

	$$ = $3;
}
|
non_polynomial_rhs_string '^' NUM
{
	(*$1) += '^';

	char strNum[NUM_LENGTH];
	sprintf(strNum, "%d", (int)$3);
	std::string num(strNum);
	(*$1) += num;

	$$ = $1;
}
|
'-' non_polynomial_rhs_string %prec uminus
{
	std::string str;
	str += '-';
	str += (*$2);
	(*$2) = str;

	$$ = $2;
}
|
IDENT
{
	int id = continuousProblem.getIDForPar(*$1);

	if(id >= 0)
	{
		flowstar::Interval range;
		continuousProblem.getRangeForPar(range, *$1);

		$$ = new std::string;
		range.toString(*$$);
	}
	else
	{
		id = continuousProblem.getIDForStateVar(*$1);

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}

		$$ = $1;
	}
}
|
'[' NUM ',' NUM ']'
{
	$$ = new std::string;
	char strNum_lo[NUM_LENGTH], strNum_up[NUM_LENGTH];
	sprintf(strNum_lo, "%.20e", $2);
	sprintf(strNum_up, "%.20e", $4);

	std::string num_lo(strNum_lo);
	std::string num_up(strNum_up);

	(*$$) += '[';
	(*$$) += num_lo;
	(*$$) += ' ';
	(*$$) += ',';
	(*$$) += ' ';
	(*$$) += num_up;
	(*$$) += ']';
}
|
NUM
{
	$$ = new std::string;
	char strNum[NUM_LENGTH];
	sprintf(strNum, "%.20e", $1);
	std::string num(strNum);

	(*$$) += '[';
	(*$$) += num;
	(*$$) += ' ';
	(*$$) += ',';
	(*$$) += ' ';
	(*$$) += num;
	(*$$) += ']';
}
;


















non_polynomial_rhs_center: non_polynomial_rhs_center '+' non_polynomial_rhs_center
{
	$1->ode += ' ';
	$1->ode += '+';
	$1->ode += ' ';
	$1->ode += $3->ode;

	$$ = $1;

	if(parseResult.bConstant)
	{
		$$->constant = $1->constant + $3->constant;
	}

	delete $3;
}
|
non_polynomial_rhs_center '-' non_polynomial_rhs_center
{
	$1->ode += ' ';
	$1->ode += '-';
	$1->ode += ' ';
	$1->ode += $3->ode;

	$$ = $1;

	if(parseResult.bConstant)
	{
		$$->constant = $1->constant - $3->constant;
	}

	delete $3;
}
|
non_polynomial_rhs_center '*' non_polynomial_rhs_center
{
	$1->ode += ' ';
	$1->ode += '*';
	$1->ode += ' ';
	$1->ode += $3->ode;

	$$ = $1;

	if(parseResult.bConstant)
	{
		$$->constant = $1->constant * $3->constant;
	}

	delete $3;
}
|
'(' non_polynomial_rhs_center ')'
{
	std::string str;
	str += '(';
	str += $2->ode;
	str += ')';
	$2->ode = str;

	$$ = $2;
}
|
non_polynomial_rhs_center '/' non_polynomial_rhs_center
{
	$1->ode += ' ';
	$1->ode += '/';
	$1->ode += ' ';
	$1->ode += $3->ode;

	$$ = $1;

	if(parseResult.bConstant)
	{
		$$->constant = $1->constant / $3->constant;
	}

	delete $3;
}
|
EXP '(' non_polynomial_rhs_center ')'
{
	std::string str("exp");
	str += '(';
	str += $3->ode;
	str += ')';
	$3->ode = str;

	if(parseResult.bConstant)
	{
		$3->constant.exp_assign();
	}

	$$ = $3;
}
|
SIN '(' non_polynomial_rhs_center ')'
{
	std::string str("sin");
	str += '(';
	str += $3->ode;
	str += ')';
	$3->ode = str;

	if(parseResult.bConstant)
	{
		$3->constant.sin_assign();
	}

	$$ = $3;
}
|
COS '(' non_polynomial_rhs_center ')'
{
	std::string str("cos");
	str += '(';
	str += $3->ode;
	str += ')';
	$3->ode = str;

	if(parseResult.bConstant)
	{
		$3->constant.cos_assign();
	}

	$$ = $3;
}
|
LOG '(' non_polynomial_rhs_center ')'
{
	std::string str("log");
	str += '(';
	str += $3->ode;
	str += ')';
	$3->ode = str;

	if(parseResult.bConstant)
	{
		$3->constant.log_assign();
	}

	$$ = $3;
}
|
SQRT '(' non_polynomial_rhs_center ')'
{
	std::string str("sqrt");
	str += '(';
	str += $3->ode;
	str += ')';
	$3->ode = str;

	if(parseResult.bConstant)
	{
		$3->constant.sqrt_assign();
	}

	$$ = $3;
}
|
non_polynomial_rhs_center '^' NUM
{
	$1->ode += '^';

	char strNum[NUM_LENGTH];
	sprintf(strNum, "%d", (int)$3);
	std::string num(strNum);
	$1->ode += num;

	if(parseResult.bConstant)
	{
		$1->constant.pow_assign((int)$3);
	}

	$$ = $1;
}
|
'-' non_polynomial_rhs_center %prec uminus
{
	std::string str;
	str += '-';
	str += $2->ode;
	$2->ode = str;

	if(parseResult.bConstant)
	{
		$2->constant.inv_assign();
	}

	$$ = $2;
}
|
IDENT
{
	int id = continuousProblem.getIDForStateVar(*$1);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
		parseError(errMsg, lineNum);
		exit(1);
	}

	flowstar::Interval intZero;
	parseResult.bConstant = false;
	parseResult.constant = intZero;

	$$ = new ODE_String(*$1, intZero);
}
|
'[' NUM ',' NUM ']'
{
	char strNum[NUM_LENGTH];
	sprintf(strNum, "%.20e", ($2+$4)/2);

	std::string num(strNum), strOde;

	strOde += '[';
	strOde += num;
	strOde += ' ';
	strOde += ',';
	strOde += ' ';
	strOde += num;
	strOde += ']';

	flowstar::Interval I($2, $4);
	$$ = new ODE_String(strOde, I);
}
|
NUM
{
	char strNum[NUM_LENGTH];
	sprintf(strNum, "%.20e", $1);
	std::string num(strNum), strOde;

	strOde += '[';
	strOde += num;
	strOde += ' ';
	strOde += ',';
	strOde += ' ';
	strOde += num;
	strOde += ']';

	flowstar::Interval I($1);
	$$ = new ODE_String(strOde, I);
}
;














/*
linear_polynomial: linear_polynomial '+' linear_polynomial
{
	$$ = $1;
	(*$$) += (*$3);

	delete $3;
}
|
linear_polynomial '-' linear_polynomial
{
	$$ = $1;
	(*$$) -= (*$3);

	delete $3;
}
|
linear_polynomial '*' linear_polynomial
{
	if($1->degree() + $3->degree() > 1)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Polynomial is not linear.");
		parseError(errMsg, lineNum);
		exit(1);
	}
	else
	{
		$$ = $1;
		(*$$) *= (*$3);
		delete $3;
	}
}
|
'(' linear_polynomial ')'
{
	$$ = $2; 
}
|
'-' linear_polynomial %prec uminus
{
	flowstar::Interval I(-1);
	$$ = $2;
	$$->mul_assign(I);
}
|
IDENT
{
	int id = continuousProblem.getIDForPar(*$1);

	if(id >= 0)
	{
		flowstar::Interval range;
		continuousProblem.getRangeForPar(range, *$1);

		int numVars = continuousProblem.stateVarNames.size()+1;
		$$ = new flowstar::Polynomial(range, numVars);
	}
	else
	{
		id = continuousProblem.getIDForStateVar(*$1);

		if(id < 0)
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "State variable %s is not declared.", (*$1).c_str());
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

		$$ = new flowstar::Polynomial(monomial);
	}

	delete $1;
}
|
NUM
{
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I($1);
	$$ = new flowstar::Polynomial(I, numVars);
}
|
'[' NUM ',' NUM ']'
{
	int numVars = continuousProblem.stateVarNames.size()+1;
	flowstar::Interval I($2, $4);
	$$ = new flowstar::Polynomial(I, numVars);
}
;
*/




lti_ode_rhs: lti_ode_rhs '+' lti_ode_rhs_term
{
	if($3->stateVar_id >= 0)
	{
		dyn_A_row[0][$3->stateVar_id] += $3->coefficient;
	}
	else if($3->ti_par_id >= 0)
	{
		dyn_ti_row[0][$3->ti_par_id] += $3->coefficient;
	}
	else if($3->tv_par_id >= 0)
	{
		dyn_tv_row[0][$3->tv_par_id] += $3->coefficient;
	}
	else
	{
		dyn_B_row[0][0] += $3->coefficient;
	}
}
|
lti_ode_rhs '-' lti_ode_rhs_term
{
	if($3->stateVar_id >= 0)
	{
		dyn_A_row[0][$3->stateVar_id] -= $3->coefficient;
	}
	else if($3->ti_par_id >= 0)
	{
		dyn_ti_row[0][$3->ti_par_id] -= $3->coefficient;
	}
	else if($3->tv_par_id >= 0)
	{
		dyn_tv_row[0][$3->tv_par_id] -= $3->coefficient;
	}
	else
	{
		dyn_B_row[0][0] -= $3->coefficient;
	}
}
|
'-' lti_ode_rhs_term
{
	if($2->stateVar_id >= 0)
	{
		dyn_A_row[0][$2->stateVar_id] -= $2->coefficient;
	}
	else if($2->ti_par_id >= 0)
	{
		dyn_ti_row[0][$2->ti_par_id] -= $2->coefficient;
	}
	else if($2->tv_par_id >= 0)
	{
		dyn_tv_row[0][$2->tv_par_id] -= $2->coefficient;
	}
	else
	{
		dyn_B_row[0][0] -= $2->coefficient;
	}
}
|
lti_ode_rhs_term
{
	if($1->stateVar_id >= 0)
	{
		dyn_A_row[0][$1->stateVar_id] = $1->coefficient;
	}
	else if($1->ti_par_id >= 0)
	{
		dyn_ti_row[0][$1->ti_par_id] = $1->coefficient;
	}
	else if($1->tv_par_id >= 0)
	{
		dyn_tv_row[0][$1->tv_par_id] = $1->coefficient;
	}
	else
	{
		dyn_B_row[0][0] = $1->coefficient;
	}
}
;


lti_ode_rhs_term: NUM '*' IDENT
{
	$$ = new LTI_Term;

	flowstar::Interval coe($1);
	
	int id = continuousProblem.getIDForStateVar(*$3);
	if(id >= 0)				// a state variable
	{
		$$->coefficient = coe;
		$$->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*$3);
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *$3);
			$$->coefficient = coe * range;
		}
		else
		{
			id = continuousProblem.getIDForTIPar(*$3);
			if(id >= 0)		// a time-invariant uncertainty
			{
				$$->coefficient = coe;
				$$->ti_par_id = id;
			}
			else
			{
				id = continuousProblem.getIDForTVPar(*$3);
				if(id >= 0)	// a time-varying uncertainty
				{
					$$->coefficient = coe;
					$$->tv_par_id = id;
				}
				else		// not defined
				{
					char errMsg[MSG_SIZE];
					sprintf(errMsg, "Symbol %s is not defined.", (*$3).c_str());
					parseError(errMsg, lineNum);
					exit(1);
				}
			}
		}
	}

	delete $3;
}
|
'[' NUM ',' NUM ']' '*' IDENT
{
	$$ = new LTI_Term;

	flowstar::Interval coe($2, $4);

	int id = continuousProblem.getIDForStateVar(*$7);
	if(id >= 0)				// a state variable
	{
		$$->coefficient = coe;
		$$->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*$7);
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *$7);
			$$->coefficient = coe * range;
		}
		else
		{
			id = continuousProblem.getIDForTIPar(*$7);
			if(id >= 0)		// a time-invariant uncertainty
			{
				$$->coefficient = coe;
				$$->ti_par_id = id;
			}
			else
			{
				id = continuousProblem.getIDForTVPar(*$7);
				if(id >= 0)	// a time-varying uncertainty
				{
					$$->coefficient = coe;
					$$->tv_par_id = id;
				}
				else		// not defined
				{
					char errMsg[MSG_SIZE];
					sprintf(errMsg, "Symbol %s is not defined.", (*$7).c_str());
					parseError(errMsg, lineNum);
					exit(1);
				}
			}
		}
	}

	delete $7;
}
|
IDENT
{
	$$ = new LTI_Term;

	flowstar::Interval coe(1);

	int id = continuousProblem.getIDForStateVar(*$1);
	if(id >= 0)				// a state variable
	{
		$$->coefficient = coe;
		$$->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*$1);
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *$1);
			$$->coefficient = range;
		}
		else
		{
			id = continuousProblem.getIDForTIPar(*$1);
			if(id >= 0)		// a time-invariant uncertainty
			{
				$$->coefficient = coe;
				$$->ti_par_id = id;
			}
			else
			{
				id = continuousProblem.getIDForTVPar(*$1);
				if(id >= 0)	// a time-varying uncertainty
				{
					$$->coefficient = coe;
					$$->tv_par_id = id;
				}
				else		// not defined
				{
					char errMsg[MSG_SIZE];
					sprintf(errMsg, "Symbol %s is not defined.", (*$1).c_str());
					parseError(errMsg, lineNum);
					exit(1);
				}
			}
		}
	}

	delete $1;
}
|
'[' NUM ',' NUM ']'
{
	$$ = new LTI_Term($2, $4, -1, -1, -1);
}
|
NUM
{
	$$ = new LTI_Term($1, $1, -1, -1, -1);
}
;











lti_ode_hybrid_rhs: lti_ode_hybrid_rhs '+' lti_ode_hybrid_rhs_term
{
	if($3->stateVar_id >= 0)
	{
		dyn_A_row[0][$3->stateVar_id] += $3->coefficient;
	}
	else
	{
		dyn_B_row[0][0] += $3->coefficient;
	}
}
|
lti_ode_hybrid_rhs '-' lti_ode_hybrid_rhs_term
{
	if($3->stateVar_id >= 0)
	{
		dyn_A_row[0][$3->stateVar_id] -= $3->coefficient;
	}
	else
	{
		dyn_B_row[0][0] -= $3->coefficient;
	}
}
|
'-' lti_ode_hybrid_rhs_term
{
	if($2->stateVar_id >= 0)
	{
		dyn_A_row[0][$2->stateVar_id] -= $2->coefficient;
	}
	else
	{
		dyn_B_row[0][0] -= $2->coefficient;
	}
}
|
lti_ode_hybrid_rhs_term
{
	if($1->stateVar_id >= 0)
	{
		dyn_A_row[0][$1->stateVar_id] = $1->coefficient;
	}
	else
	{
		dyn_B_row[0][0] = $1->coefficient;
	}
}
;


lti_ode_hybrid_rhs_term: NUM '*' IDENT
{
	$$ = new LTI_Term;

	flowstar::Interval coe($1);
	
	int id = hybridProblem.getIDForStateVar(*$3);
	if(id >= 0)				// a state variable
	{
		$$->coefficient = coe;
		$$->stateVar_id = id;
	}
	else
	{
		id = hybridProblem.getIDForPar(*$3);
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			hybridProblem.getRangeForPar(range, *$3);
			$$->coefficient = range;
		}
		else				// not defined
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Symbol %s is not defined.", (*$3).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	delete $3;
}
|
'[' NUM ',' NUM ']' '*' IDENT
{
	$$ = new LTI_Term;

	flowstar::Interval coe($2, $4);

	int id = continuousProblem.getIDForStateVar(*$7);
	if(id >= 0)				// a state variable
	{
		$$->coefficient = coe;
		$$->stateVar_id = id;
	}
	else
	{
		id = hybridProblem.getIDForPar(*$7);
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			hybridProblem.getRangeForPar(range, *$7);
			$$->coefficient = range;
		}
		else				// not defined
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Symbol %s is not defined.", (*$7).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	delete $7;
}
|
IDENT
{
	$$ = new LTI_Term;

	flowstar::Interval coe(1);

	int id = continuousProblem.getIDForStateVar(*$1);
	if(id >= 0)				// a state variable
	{
		$$->coefficient = coe;
		$$->stateVar_id = id;
	}
	else
	{
		id = hybridProblem.getIDForPar(*$1);
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			hybridProblem.getRangeForPar(range, *$1);
			$$->coefficient = range;
		}
		else				// not defined
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Symbol %s is not defined.", (*$1).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	delete $1;
}
|
'[' NUM ',' NUM ']'
{
	$$ = new LTI_Term($2, $4, -1, -1, -1);
}
|
NUM
{
	$$ = new LTI_Term($1, $1, -1, -1, -1);
}
;

























ltv_ode_rhs: ltv_ode_rhs '+' ltv_ode_rhs_term
{
	if($3->stateVar_id >= 0)
	{
		dyn_A_t_row[0][$3->stateVar_id] += $3->coefficient;
	}
	else if($3->ti_par_id >= 0)
	{
		dyn_ti_t_row[0][$3->ti_par_id] += $3->coefficient;
	}
	else if($3->tv_par_id >= 0)
	{
		dyn_tv_t_row[0][$3->tv_par_id] += $3->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] += $3->coefficient;
	}
}
|
ltv_ode_rhs '-' ltv_ode_rhs_term
{
	if($3->stateVar_id >= 0)
	{
		dyn_A_t_row[0][$3->stateVar_id] -= $3->coefficient;
	}
	else if($3->ti_par_id >= 0)
	{
		dyn_ti_t_row[0][$3->ti_par_id] -= $3->coefficient;
	}
	else if($3->tv_par_id >= 0)
	{
		dyn_tv_t_row[0][$3->tv_par_id] -= $3->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] -= $3->coefficient;
	}
}
|
'-' ltv_ode_rhs_term
{
	if($2->stateVar_id >= 0)
	{
		dyn_A_t_row[0][$2->stateVar_id] -= $2->coefficient;
	}
	else if($2->ti_par_id >= 0)
	{
		dyn_ti_t_row[0][$2->ti_par_id] -= $2->coefficient;
	}
	else if($2->tv_par_id >= 0)
	{
		dyn_tv_t_row[0][$2->tv_par_id] -= $2->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] -= $2->coefficient;
	}
}
|
ltv_ode_rhs_term
{
	if($1->stateVar_id >= 0)
	{
		dyn_A_t_row[0][$1->stateVar_id] = $1->coefficient;
	}
	else if($1->ti_par_id >= 0)
	{
		dyn_ti_t_row[0][$1->ti_par_id] = $1->coefficient;
	}
	else if($1->tv_par_id >= 0)
	{
		dyn_tv_t_row[0][$1->tv_par_id] = $1->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] = $1->coefficient;
	}
}
;



ltv_ode_rhs_term: '(' univariate_polynomial ')' '*' IDENT
{
	$$ = new LTV_Term;
	
	int id = continuousProblem.getIDForStateVar(*$5);
	if(id >= 0)				// a state variable
	{
		$$->coefficient = *$2;
		$$->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*$5);
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *$5);
			$$->coefficient = (*$2) * range;
		}
		else
		{
			id = continuousProblem.getIDForTIPar(*$5);
			if(id >= 0)		// a time-invariant uncertainty
			{
				$$->coefficient = *$2;
				$$->ti_par_id = id;
			}
			else
			{
				id = continuousProblem.getIDForTVPar(*$5);
				if(id >= 0)	// a time-varying uncertainty
				{
					$$->coefficient = *$2;
					$$->tv_par_id = id;
				}
				else		// not defined
				{
					char errMsg[MSG_SIZE];
					sprintf(errMsg, "Symbol %s is not defined.", (*$5).c_str());
					parseError(errMsg, lineNum);
					exit(1);
				}
			}
		}
	}

	delete $2;
	delete $5;
}
|
IDENT
{
	$$ = new LTV_Term;
	flowstar::Interval intOne(1);

	int id = continuousProblem.getIDForStateVar(*$1);
	if(id >= 0)				// a state variable
	{
		$$->coefficient = intOne;
		$$->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*$1);
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *$1);
			$$->coefficient = range;
		}
		else
		{
			id = continuousProblem.getIDForTIPar(*$1);
			if(id >= 0)		// a time-invariant uncertainty
			{
				$$->coefficient = intOne;
				$$->ti_par_id = id;
			}
			else
			{
				id = continuousProblem.getIDForTVPar(*$1);
				if(id >= 0)	// a time-varying uncertainty
				{
					$$->coefficient = intOne;
					$$->tv_par_id = id;
				}
				else		// not defined
				{
					char errMsg[MSG_SIZE];
					sprintf(errMsg, "Symbol %s is not defined.", (*$1).c_str());
					parseError(errMsg, lineNum);
					exit(1);
				}
			}
		}
	}

	delete $1;
}
|
'(' univariate_polynomial ')'
{
	$$ = new LTV_Term(*$2, -1, -1, -1);

	delete $2;
}
;





ltv_ode_hybrid_rhs: ltv_ode_hybrid_rhs '+' ltv_ode_hybrid_rhs_term
{
	if($3->stateVar_id >= 0)
	{
		dyn_A_t_row[0][$3->stateVar_id] += $3->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] += $3->coefficient;
	}
}
|
ltv_ode_hybrid_rhs '-' ltv_ode_hybrid_rhs_term
{
	if($3->stateVar_id >= 0)
	{
		dyn_A_t_row[0][$3->stateVar_id] -= $3->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] -= $3->coefficient;
	}
}
|
'-' ltv_ode_hybrid_rhs_term
{
	if($2->stateVar_id >= 0)
	{
		dyn_A_t_row[0][$2->stateVar_id] -= $2->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] -= $2->coefficient;
	}
}
|
ltv_ode_hybrid_rhs_term
{
	if($1->stateVar_id >= 0)
	{
		dyn_A_t_row[0][$1->stateVar_id] = $1->coefficient;
	}
	else
	{
		dyn_B_t_row[0][0] = $1->coefficient;
	}
}
;




ltv_ode_hybrid_rhs_term: '(' univariate_polynomial ')' '*' IDENT
{
	$$ = new LTV_Term;
	
	int id = continuousProblem.getIDForStateVar(*$5);
	if(id >= 0)				// a state variable
	{
		$$->coefficient = *$2;
		$$->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*$5);
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *$5);
			$$->coefficient = (*$2) * range;
		}
		else		// not defined
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Symbol %s is not defined.", (*$5).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	delete $2;
	delete $5;
}
|
IDENT
{
	$$ = new LTV_Term;
	flowstar::Interval intOne(1);

	int id = continuousProblem.getIDForStateVar(*$1);
	if(id >= 0)				// a state variable
	{
		$$->coefficient = intOne;
		$$->stateVar_id = id;
	}
	else
	{
		id = continuousProblem.getIDForPar(*$1);
		if(id >= 0)			// a parameter
		{
			flowstar::Interval range;
			continuousProblem.getRangeForPar(range, *$1);
			$$->coefficient = range;
		}
		else		// not defined
		{
			char errMsg[MSG_SIZE];
			sprintf(errMsg, "Symbol %s is not defined.", (*$1).c_str());
			parseError(errMsg, lineNum);
			exit(1);
		}
	}

	delete $1;
}
|
'(' univariate_polynomial ')'
{
	$$ = new LTV_Term(*$2, -1, -1, -1);

	delete $2;
}
;








univariate_polynomial: univariate_polynomial '+' univariate_polynomial
{
	$$ = $1;
	(*$$) += (*$3);

	delete $3;
}
|
univariate_polynomial '-' univariate_polynomial
{
	$$ = $1;
	(*$$) -= (*$3);

	delete $3;
}
|
'(' univariate_polynomial ')'
{
	$$ = $2; 
}
|
univariate_polynomial '*' univariate_polynomial
{
	$$ = $1;
	(*$$) *= (*$3);

	delete $3;
}
|
univariate_polynomial '^' NUM
{
	int exp = (int) $3;

	if(exp == 0)
	{
		$$ = new flowstar::UnivariatePolynomial(1);
	}
	else
	{
		$$ = new flowstar::UnivariatePolynomial;
		(*$1).pow(*$$, exp);
	}

	delete $1;
}
|
'-' univariate_polynomial %prec uminus
{
	flowstar::Interval I(-1);
	$$ = $2;
	(*$$) *= I;
}
|
IDENT
{
	std::string tVar("t");
	if($1->compare(tVar) == 0)
	{
		$$ = new flowstar::UnivariatePolynomial(1, 1);
	}
	else
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "The time variable should be denoted by t.");
		parseError(errMsg, lineNum);
		exit(1);
	}
}
|
'[' NUM ',' NUM ']'
{
	flowstar::Interval I($2, $4);
	$$ = new flowstar::UnivariatePolynomial(I);
}
|
NUM
{
	$$ = new flowstar::UnivariatePolynomial($1);
}
;







multivariate_polynomial: multivariate_polynomial '+' multivariate_polynomial
{
	$$ = $1;
	(*$$) += (*$3);

	delete $3;
}
|
multivariate_polynomial '-' multivariate_polynomial
{
	$$ = $1;
	(*$$) -= (*$3);

	delete $3;
}
|
'(' multivariate_polynomial ')'
{
	$$ = $2; 
}
|
multivariate_polynomial '*' multivariate_polynomial
{
	$$ = $1;
	(*$$) *= (*$3);

	delete $3;
}
|
multivariate_polynomial '^' NUM
{
	int exp = (int) $3;

	if(exp == 0)
	{
		flowstar::Interval I(1);
		$$ = new flowstar::Polynomial(I, flowstar::parsePolynomial.variables.size());
	}
	else
	{
		$$ = new flowstar::Polynomial;
		(*$1).pow(*$$, exp);
	}

	delete $1;
}
|
'-' multivariate_polynomial %prec uminus
{
	flowstar::Interval I(-1);
	$$ = $2;
	$$->mul_assign(I);
}
|
IDENT
{
	int id = flowstar::parsePolynomial.variables.getIDForVar(*$1);

	if(id < 0)
	{
		char errMsg[MSG_SIZE];
		sprintf(errMsg, "Variable %s is not declared.", (*$1).c_str());
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

	$$ = new flowstar::Polynomial(monomial);

	delete $1;
}
|
'[' NUM ',' NUM ']'
{
	int numVars = flowstar::parsePolynomial.variables.size();
	flowstar::Interval I($2, $4);
	$$ = new flowstar::Polynomial(I, numVars);
}
|
NUM
{
	int numVars = flowstar::parsePolynomial.variables.size();
	flowstar::Interval I($1);
	$$ = new flowstar::Polynomial(I, numVars);
}
;













ode_expression: ode_expression '+' ode_expression
{
	$$ = $1;
	(*$$) += (*$3);

	delete $3;
}
|
ode_expression '-' ode_expression
{
	$$ = $1;
	(*$$) -= (*$3);

	delete $3;
}
|
'(' ode_expression ')'
{
	$$ = $2; 
}
|
ode_expression '*' ode_expression
{
	$$ = $1;
	(*$$) *= (*$3);

	delete $3;
}
|
ode_expression '^' NUM
{
	$$ = $1;
	$$->pow_assign((int)$3);
}
|
'-' ode_expression %prec uminus
{
	$$ = $2;
	$$->inv_assign();
}
|
IDENT
{
	$$ = new flowstar::Expression_AST(*$1, flowstar::parseExpression.variables, 0);

	delete $1;
}
|
'[' NUM ',' NUM ']'
{
	flowstar::Interval I($2, $4);
	$$ = new flowstar::Expression_AST(I);
}
|
NUM
{
	flowstar::Interval I($1);
	$$ = new flowstar::Expression_AST(I);
}
|
ode_expression '/' ode_expression
{
	$$ = $1;
	(*$$) /= (*$3);

	delete $3;
}
|
EXP '(' ode_expression ')'
{
	$$ = $3;
	$$->exp_assign();
}
|
SIN '(' ode_expression ')'
{
	$$ = $3;
	$$->sin_assign();
}
|
COS '(' ode_expression ')'
{
	$$ = $3;
	$$->cos_assign();
}
|
LOG '(' ode_expression ')'
{
	$$ = $3;
	$$->log_assign();
}
|
SQRT '(' ode_expression ')'
{
	$$ = $3;
	$$->sqrt_assign();
}
;







matrix_matlab: matrix_matlab ';' vector_matlab
{
	$$ = $1;
	$$->push_back(*$3);
	delete $3;
}
|
vector_matlab
{
	$$ = new std::vector<std::vector<flowstar::Interval> >(0);
	$$->push_back(*$1);
	delete $1;
}
;

vector_matlab: vector_matlab ',' NUM
{
	$$ = $1;
	$$->push_back($3);
}
|
NUM
{
	$$ = new std::vector<flowstar::Interval>(0);
	$$->push_back($1);
}
;



%%

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
