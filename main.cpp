/*

Contributors to the tool : 
Souradeep Dutta

email : souradeep.dutta@colorado.edu

LICENSE : Please see the license file, in the main directory

*/

#include "propagate_intervals.h"
using namespace std;
using namespace std::chrono;

int main(int argc, char ** argv)
{
	int run_benchmark_no = -1;
	char key[] = "all";
	bool run_all = false;
	if(argc != 2)
	{
		cout << "Wrong number of command line arguments : " << endl;
		cout << "Please pass the benchmark number to run it. " << endl;
		cout << "Exiting... " << endl;
		exit(0);
	}
	else if(strcmp(key, argv[1]) == 0)
	{
		run_all = true;
	}
	else
	{
		sscanf(argv[1], "%d", &run_benchmark_no);
	}


	vector< vector< datatype > > input_interval(2, vector< datatype >(2,0));
	vector< vector< datatype > > input_constraints;
	clock_t begin, end;
	vector< datatype > output_range(2,0);
	// Simple range propagation
	// sherlock_parameters.verbosity = true;
	// sherlock_parameters.grad_search_point_verbosity = true;
	sherlock_parameters.time_verbosity = true;

	if((run_benchmark_no == 0) || (run_all))
	{
		char benchmark_0_name[] = "./network_files/neural_network_information_0" ;
		network_handler benchmark_0(benchmark_0_name);

		input_interval[0][0] = 0;input_interval[0][1] = 10;
		input_interval[1][0] = 0;input_interval[1][1] = 10;

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();


	 	benchmark_0.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 0 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if ((run_benchmark_no == 1) || (run_all)) {
		// Simple range propagation
		char benchmark_1_name[] = "./network_files/neural_network_information_1" ;
		sherlock_parameters.MILP_tolerance = 1e-2;
		network_handler benchmark_1(benchmark_1_name);

		input_interval[0][0] = 0;input_interval[0][1] = 10;
		input_interval[1][0] = 0;input_interval[1][1] = 10;

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_1.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 1 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);


	}
	if ((run_benchmark_no == 2) || (run_all)) {
		// Simple range propagation
		char benchmark_2_name[] = "./network_files/neural_network_information_2" ;
		sherlock_parameters.gradient_rate = 1e-4;
		sherlock_parameters.grad_scaling_factor = 2e1;
		sherlock_parameters.MILP_tolerance = 8e-2;
		sherlock_parameters.scale_factor_for_M = 1.3;

		network_handler benchmark_2(benchmark_2_name);

		input_interval[0][0] = 0;input_interval[0][1] = 10;
		input_interval[1][0] = 0;input_interval[1][1] = 10;

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_2.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 2 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
	}
	if ((run_benchmark_no == 3) || (run_all)) {
		// Simple range propagation
		char benchmark_3_name[] = "./network_files/neural_network_information_3" ;

		network_handler benchmark_3(benchmark_3_name);

		input_interval[0][0] = 0;input_interval[0][1] = 10;
		input_interval[1][0] = 0;input_interval[1][1] = 10;

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_3.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 3 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
	}
	if ((run_benchmark_no == 4) || (run_all)) {
		// Simple range propagation
		char benchmark_4_name[] = "./network_files/neural_network_information_4" ;

		network_handler benchmark_4(benchmark_4_name);

		input_interval[0][0] = 0;input_interval[0][1] = 10;
		input_interval[1][0] = 0;input_interval[1][1] = 10;

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_4.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 4 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if ((run_benchmark_no == 5) || (run_all)) {
		// Simple range propagation
		char benchmark_5_name[] = "./network_files/neural_network_information_5" ;
		sherlock_parameters.MILP_tolerance = 2e-2;

		network_handler benchmark_5(benchmark_5_name);

		input_interval.clear();
		input_interval.resize(3);
		input_interval[0].push_back(-0.5);input_interval[0].push_back(0.5);;
		input_interval[1].push_back(-0.5);input_interval[1].push_back(0.5);;
		input_interval[2].push_back(-0.5);input_interval[2].push_back(0.5);;

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_5.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 5 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if ((run_benchmark_no == 6) || (run_all)) {
		//	Simple range propagation
		char benchmark_6_name[] = "./network_files/neural_network_information_6" ;

		network_handler benchmark_6(benchmark_6_name);

		input_interval.clear();
		input_interval.resize(3);
		input_interval[0].push_back(-0.5);input_interval[0].push_back(0.5);;
		input_interval[1].push_back(-0.5);input_interval[1].push_back(0.5);;
		input_interval[2].push_back(-0.5);input_interval[2].push_back(0.5);;

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_6.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 6 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if ((run_benchmark_no == 7) || (run_all)) {
		//	Simple range propagation
		char benchmark_7_name[] = "./network_files/neural_network_information_7" ;
		network_handler benchmark_7(benchmark_7_name);

		input_interval.clear();
		input_interval.resize(4);
		input_interval[0].push_back(-0.1);input_interval[0].push_back(0.1);
		input_interval[1].push_back(-0.1);input_interval[1].push_back(0.1);
		input_interval[2].push_back(-0.1);input_interval[2].push_back(0.1);
		input_interval[3].push_back(-0.1);input_interval[3].push_back(0.1);

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_7.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 7 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if ((run_benchmark_no == 8) || (run_all)) {
		char benchmark_8_name[] = "./network_files/neural_network_information_8" ;
		network_handler benchmark_8(benchmark_8_name);

		input_interval.clear();
		input_interval.resize(3);
		input_interval[0].push_back(-0.5);input_interval[0].push_back(0.5);
		input_interval[1].push_back(-0.5);input_interval[1].push_back(0.5);
		input_interval[2].push_back(-0.5);input_interval[2].push_back(0.5);

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_8.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 8 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if ((run_benchmark_no == 9) || (run_all)) {
		char benchmark_9_name[] = "./network_files/neural_network_information_9" ;
		network_handler benchmark_9(benchmark_9_name);

		input_interval.clear();
		input_interval.resize(3);
		input_interval[0].push_back(-0.5);input_interval[0].push_back(0.5);
		input_interval[1].push_back(-0.5);input_interval[1].push_back(0.5);
		input_interval[2].push_back(-0.5);input_interval[2].push_back(0.5);

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_9.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 9 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if ((run_benchmark_no == 10) || (run_all)) {
		char benchmark_10_name[] = "./network_files/neural_network_information_10" ;
		sherlock_parameters.gradient_rate = 1e-6;
		sherlock_parameters.grad_scaling_factor = 2e1;
		sherlock_parameters.grad_switch_count = 1e2;
		sherlock_parameters.grad_termination_limit = 1e-6;
		sherlock_parameters.MILP_e_tolerance = 1e-15;
		sherlock_parameters.scale_factor_for_M = 1.5;
		sherlock_parameters.MILP_tolerance = 5e-2;

		network_handler benchmark_10(benchmark_10_name);


		input_interval.clear();
		input_interval.resize(3);
		input_interval[0].push_back(-0.1);input_interval[0].push_back(0.1);
		input_interval[1].push_back(-0.1);input_interval[1].push_back(0.1);
		input_interval[2].push_back(-0.1);input_interval[2].push_back(0.1);

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_10.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 10 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if ((run_benchmark_no == 11) || (run_all)) {
		char benchmark_11_name[] = "./network_files/neural_network_information_11" ;
		sherlock_parameters.gradient_rate = 1e-4;
		sherlock_parameters.grad_scaling_factor = 5e1;
		sherlock_parameters.grad_switch_count = 1e3;
		sherlock_parameters.grad_termination_limit = 1e-7;
		sherlock_parameters.MILP_tolerance = 5e-1;
		sherlock_parameters.MILP_e_tolerance = 1e-10;
		sherlock_parameters.scale_factor_for_M = 1;
		sherlock_parameters.do_LP_certificate = true;
		sherlock_parameters.LP_tolerance_limit = 1e-2;

		network_handler benchmark_11(benchmark_11_name);

		input_interval.clear();
		input_interval.resize(3);
		input_interval[0].push_back(-0.1);input_interval[0].push_back(0.1);
		input_interval[1].push_back(-0.1);input_interval[1].push_back(0.1);
		input_interval[2].push_back(-0.1);input_interval[2].push_back(0.1);

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_11.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 11 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if ((run_benchmark_no == 12) || (run_all)) {
		char benchmark_12_name[] = "./network_files/neural_network_information_12" ;
		network_handler benchmark_12(benchmark_12_name);
		sherlock_parameters.gradient_rate = 1e-2;
		sherlock_parameters.grad_scaling_factor = 5e1;
		sherlock_parameters.grad_switch_count = 1e3;
		sherlock_parameters.grad_termination_limit = 1e-7;
		sherlock_parameters.MILP_tolerance = 5e-1;
		sherlock_parameters.MILP_e_tolerance = 1e-10;
		sherlock_parameters.scale_factor_for_M = 1;
		sherlock_parameters.do_LP_certificate = true;
		sherlock_parameters.LP_tolerance_limit = 1e-2;


		input_interval.clear();
		input_interval.resize(3);
		input_interval[0].push_back(-0.1);input_interval[0].push_back(0.1);
		input_interval[1].push_back(-0.1);input_interval[1].push_back(0.1);
		input_interval[2].push_back(-0.1);input_interval[2].push_back(0.1);

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_12.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 12 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if ((run_benchmark_no == 13) || (run_all)) {
		char benchmark_13_name[] = "./network_files/neural_network_information_13" ;
		sherlock_parameters.gradient_rate = 1e-2;
		sherlock_parameters.grad_scaling_factor = 5e1;
		sherlock_parameters.grad_switch_count = 1e3;
		sherlock_parameters.grad_termination_limit = 1e-7;
		sherlock_parameters.MILP_tolerance = 5e-1;
		sherlock_parameters.MILP_e_tolerance = 1e-10;
		sherlock_parameters.scale_factor_for_M = 1;
		sherlock_parameters.LP_tolerance_limit = 1e-2;
		sherlock_parameters.do_LP_certificate = true;

		network_handler benchmark_13(benchmark_13_name);



		input_interval.clear();
		input_interval.resize(3);
		input_interval[0].push_back(-0.1);input_interval[0].push_back(0.1);
		input_interval[1].push_back(-0.1);input_interval[1].push_back(0.1);
		input_interval[2].push_back(-0.1);input_interval[2].push_back(0.1);

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_13.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 13 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if ((run_benchmark_no == 14) || (run_all)) {
		char benchmark_14_name[] = "./network_files/neural_network_information_14" ;
		sherlock_parameters.gradient_rate = 1e-2;
		sherlock_parameters.grad_scaling_factor = 5e1;
		sherlock_parameters.grad_switch_count = 1e3;
		sherlock_parameters.grad_termination_limit = 1e-7;
		sherlock_parameters.MILP_tolerance = 5e-1;
		sherlock_parameters.MILP_e_tolerance = 1e-10;
		sherlock_parameters.scale_factor_for_M = 1;
		sherlock_parameters.do_LP_certificate = true;
		sherlock_parameters.LP_tolerance_limit = 1e-2;

		network_handler benchmark_14(benchmark_14_name);

		input_interval.clear();
		input_interval.resize(3);
		input_interval[0].push_back(-0.1);input_interval[0].push_back(0.1);
		input_interval[1].push_back(-0.1);input_interval[1].push_back(0.1);
		input_interval[2].push_back(-0.1);input_interval[2].push_back(0.1);

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_14.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 14 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	}
	if((run_benchmark_no == 15) || (run_all))
	{
		char benchmark_15_name[] = "./network_files/neural_network_information_15" ;
		sherlock_parameters.gradient_rate = 1e-2;
		sherlock_parameters.grad_scaling_factor = 5e1;
		sherlock_parameters.grad_switch_count = 1e3;
		sherlock_parameters.grad_termination_limit = 1e-7;
		sherlock_parameters.MILP_tolerance = 5e-1;
		sherlock_parameters.MILP_e_tolerance = 1e-10;
		sherlock_parameters.scale_factor_for_M = 1;
		sherlock_parameters.do_LP_certificate = true;

		network_handler benchmark_15(benchmark_15_name);


		input_interval.clear();
		input_interval.resize(3);
		input_interval[0].push_back(-0.1);input_interval[0].push_back(0.1);
		input_interval[1].push_back(-0.1);input_interval[1].push_back(0.1);
		input_interval[2].push_back(-0.1);input_interval[2].push_back(0.1);

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_15.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 15 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
	}
	


  return 0;
}
