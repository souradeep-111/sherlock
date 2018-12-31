/*

Contributors to the tool :
Souradeep Dutta

email : souradeep.dutta@colorado.edu

LICENSE : Please see the license file, in the main directory

*/

#include "./src/propagate_intervals.h"
#include "./src/computation_graph.h"
// #include "./include/sherlock.h"


// #include <iterator >
#include <map>
#include <utility>

using namespace std;
using namespace std::chrono;

int main(int argc, char ** argv)
{


	// testing the data structures built on a very small network which can be analysed etc
	computation_graph sample_graph;

	// The two input nodes to the graph declared as constants
	node node_1(1, "constant");
	sample_graph.add_new_node(1, node_1);
	node node_2(2, "constant");
	sample_graph.add_new_node(2, node_2);

	// The internal nodes
	node node_3(3, "relu");
	sample_graph.add_new_node(3, node_3);
	node node_4(4, "relu");
	sample_graph.add_new_node(4, node_4);
	node node_5(5, "relu");
	sample_graph.add_new_node(5, node_5);
	node node_6(6, "relu");
	sample_graph.add_new_node(6, node_6);

	// The output node
	node node_7(7, "none");
	sample_graph.add_new_node(7, node_7);


	// First let's mark some of the nodes as inputs and outputs
	sample_graph.mark_node_as_input(1);
	sample_graph.mark_node_as_input(2);
	sample_graph.mark_node_as_output(7);

	// Now let's create the connections:

	// first layer connections and bias
	sample_graph.connect_node1_to_node2_with_weight(1,3,1.0);
	sample_graph.connect_node1_to_node2_with_weight(1,4,1.0);
	sample_graph.connect_node1_to_node2_with_weight(2,3,1.0);
	sample_graph.connect_node1_to_node2_with_weight(2,4,-1.0);
	sample_graph.set_bias_of_node(3, 0.0);
	sample_graph.set_bias_of_node(4, 0.0);

	sample_graph.connect_node1_to_node2_with_weight(3,5,1.0);
	sample_graph.connect_node1_to_node2_with_weight(3,6,0.0);
	sample_graph.connect_node1_to_node2_with_weight(4,5,0.0);
	sample_graph.connect_node1_to_node2_with_weight(4,6,1.0);
	sample_graph.set_bias_of_node(5,0.0);
	sample_graph.set_bias_of_node(6,0.0);

	sample_graph.connect_node1_to_node2_with_weight(5,7,0.5);
	sample_graph.connect_node1_to_node2_with_weight(6,7,0.5);
	sample_graph.set_bias_of_node(7, 0.0);

	auto x = 5.0;
	auto y = -2.0;
	map< uint32_t, double > inputs;
	inputs.insert(make_pair(1, x));
	inputs.insert(make_pair(2, y));

	map< uint32_t, double > outputs;
	map< uint32_t, double > gradient;
	sample_graph.evaluate_graph(inputs, outputs);
	inputs.clear();
	inputs.insert(make_pair(1, x));
	inputs.insert(make_pair(2, y));
	gradient = sample_graph.return_gradient_wrt_inputs(7, inputs);
	cout << "Value at x = " << x << "  and y = " << y << " is " << outputs[7] << endl;
	cout << "Gradient = [ " << gradient[1] << " , " << gradient[2] <<" ] " << endl;
	exit(0);


	int run_benchmark_no = -1;
	char key[] = "all";
	char key_2[] = "-";
	bool run_all = false;
	bool run_first_10 = false;
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
	else if(strcmp(key_2, argv[1]) == 0)
	{
		run_first_10 = true;
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

	if((run_benchmark_no == 0) || (run_all) || (run_first_10))
	{
		sherlock_parameters.MILP_tolerance = 1e-2;
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
	if ((run_benchmark_no == 1) || (run_all) || (run_first_10)) {
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
	if ((run_benchmark_no == 2) || (run_all) || (run_first_10)) {
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
	if ((run_benchmark_no == 3) || (run_all) || (run_first_10)) {
		// Simple range propagation
		sherlock_parameters.gradient_rate = 1e-4;
		sherlock_parameters.grad_scaling_factor = 2e1;
		sherlock_parameters.MILP_tolerance = 8e-2;
		sherlock_parameters.scale_factor_for_M = 1.3;
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
	if ((run_benchmark_no == 4) || (run_all) || (run_first_10)) {
		// Simple range propagation
		sherlock_parameters.gradient_rate = 1e-4;
		sherlock_parameters.grad_scaling_factor = 2e1;
		sherlock_parameters.MILP_tolerance = 8e-2; //8e-2
		sherlock_parameters.scale_factor_for_M = 1.3;
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
	if ((run_benchmark_no == 5) || (run_all) || (run_first_10)) {
		// Simple range propagation
		char benchmark_5_name[] = "./network_files/neural_network_information_5" ;
		sherlock_parameters.gradient_rate = 1e-4;
		sherlock_parameters.grad_scaling_factor = 2e1;
		sherlock_parameters.scale_factor_for_M = 1.3;
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
	if ((run_benchmark_no == 6) || (run_all) || (run_first_10)) {
		//	Simple range propagation
		sherlock_parameters.gradient_rate = 1e-4;
		sherlock_parameters.grad_scaling_factor = 2e1;
		sherlock_parameters.scale_factor_for_M = 1.3;
		sherlock_parameters.MILP_tolerance = 2e-2;
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
	if ((run_benchmark_no == 7) || (run_all) || (run_first_10)) {
		//	Simple range propagation
		sherlock_parameters.gradient_rate = 1e-4;
		sherlock_parameters.grad_scaling_factor = 2e1;
		sherlock_parameters.scale_factor_for_M = 1.3;
		sherlock_parameters.MILP_tolerance = 2e-2;
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
	if ((run_benchmark_no == 8) || (run_all) || (run_first_10)) {
		sherlock_parameters.gradient_rate = 1e-4;
		sherlock_parameters.grad_scaling_factor = 2e1;
		sherlock_parameters.scale_factor_for_M = 1.3;
		sherlock_parameters.MILP_tolerance = 2e-2;
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
	if ((run_benchmark_no == 9) || (run_all) || (run_first_10)) {
		sherlock_parameters.gradient_rate = 1e-4;
		sherlock_parameters.grad_scaling_factor = 2e1;
		sherlock_parameters.scale_factor_for_M = 1.3;
		sherlock_parameters.MILP_tolerance = 2e-2;
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
	if ((run_benchmark_no == 10) || (run_all) || (run_first_10)) {
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
	if((run_benchmark_no == 16) || (run_all))
	{
		char benchmark_16_name[] = "./network_files/neural_network_information_16" ;

		network_handler benchmark_16(benchmark_16_name);


		input_interval.clear();
		input_interval.resize(3);
		input_interval[0].push_back(0);input_interval[0].push_back(573);
		input_interval[1].push_back(0);input_interval[1].push_back(1);
		input_interval[2].push_back(0);input_interval[2].push_back(1);

		create_constraint_from_interval(input_constraints, input_interval);

		begin = clock();
		benchmark_16.return_interval_output(input_constraints, output_range, 1);
		cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;
		end = clock();
		printf("time cost for Sherlock benchmark 16 ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

		vector< vector< vector < datatype > > > weights;
		vector< vector< datatype > > biases;
		vector< datatype > extrema_point;
		benchmark_16.return_network_information(weights, biases);
		datatype max = do_MILP_optimization(input_constraints, weights, biases, extrema_point, 1);
		cout << "Max found by Naive method is : " << max << endl;
		cout << "Max point = [" << extrema_point[0] << " , " << extrema_point[1] << " , " << extrema_point[2] << " ] " << endl;

		datatype min = do_MILP_optimization(input_constraints, weights, biases, extrema_point, -1);
		cout << "Min found by Naive method is : " << min << endl;
		cout << "Min point = [" << extrema_point[0] << " , " << extrema_point[1] << " , " << extrema_point[2] << " ] " << endl;

	}



  return 0;
}
