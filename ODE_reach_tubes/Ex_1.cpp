#include "compute_flowpipes.h"

using namespace std;
using namespace flowstar;

int main()
{
	Variables stateVars;

	/*
	 * Declaration of the state variables.
	 * The first one should always be the local time variable which can be
	 * viewed as a preserved variable. It is only used internally by the library.
	 */

	double time_step = 0.2;
	int no_of_flowpipes = 10;
	int no_of_steps = 30;

	int polynomial_degree_for_controller = 2;

	stateVars.declareVar("t");
	stateVars.declareVar("x");
	stateVars.declareVar("y");
	stateVars.declareVar("u");

	int domainDim = 4;

	Expression_AST deriv_x("y - x^3 ", stateVars);
	Expression_AST deriv_y("u", stateVars);
	Expression_AST deriv_u("0", stateVars);

	ODE plant(stateVars);
	plant.assignDerivative("x", deriv_x);
	plant.assignDerivative("y", deriv_y);
	plant.assignDerivative("u", deriv_u);


	/*
	 * Specify the parameters for reachability computation.
	 */
	Continuous_Reachability_Setting crs;

	// step size
	crs.setFixedStepsize(time_step/((double) no_of_flowpipes));

	// Taylor model order
	crs.setFixedOrder(4);

	// precision
	crs.setPrecision(100);

	// cutoff threshold
	Interval cutoff(-1e-10,1e-10);
	crs.setCutoff(cutoff);

	/*
	 * A remainder estimation is a vector of intervals such that
	 * the i-th component is the estimation for the i-th state variable.
	 */
	Interval E(-0.01,0.01);
	std::vector<Interval> estimation;
	estimation.push_back(E);	// estimation for the 1st variable
	estimation.push_back(E);	// estimation for the 2nd variable
	estimation.push_back(E);	// estimation for the 3rd variable
	crs.setRemainderEstimation(estimation);

	// call this function whenever a parameter is set or changed
	crs.prepareForReachability();

	string controller_file = "systems_with_networks/Ex_12/neural_network_controller" ;
	computation_graph controller_graph;
	// Create the input indices
	vector< uint32_t > controller_input_indices;
	// Create the output indices
	vector< uint32_t > controller_output_indices;
	// create_computation_graph_from_file(controller_file, controller_graph, true,
	// 			controller_input_indices, controller_output_indices);


	vector< double > scale, offset;
	// In the right order
	scale.push_back(1);
	offset.push_back(-4);
	read_controller_graph(controller_file, controller_graph, true, scale, offset,
		controller_input_indices, controller_output_indices);


	cout << "No of inputs - " << controller_input_indices.size() << endl;
	cout << "Indices are - " ;
	for(auto index : controller_input_indices)
		cout << index << " ";
	cout << endl;

	cout << "No of outputs - " << controller_output_indices.size() << endl;
	cout << "Indices are - " ;
	for(auto index : controller_output_indices)
		cout << index << " ";
	cout << endl;

	map< uint32_t, double > input_points, output_points;
	input_points[1] = 0.5;
	input_points[2] = 0.5;
	controller_graph.evaluate_graph(input_points, output_points);
	cout << "Output value - " << output_points[60] << endl;

	/*
	 * Initial set can be a box which is represented by a vector of intervals.
	 * The i-th component denotes the initial set of the i-th state variable.
	 */
	Interval init_x(0.355,0.722), init_y(-1.105, -0.694), init_u, intZero;
	std::vector<Interval> X0;
	X0.push_back(init_x);
	X0.push_back(init_y);
	X0.push_back(init_u);

	pair<int, int> plot_dimensions = make_pair(0, 1);
	string filename_to_save = "./Plots/Ex_1.m";

	// translate the initial set to a flowpipe
	Flowpipe initial_set(X0, intZero);

	// the flowpipe that keeps the overapproximation at the end of a time horizon
	Flowpipe fp_last;

	// the symbolic remainder
	Symbolic_Remainder symb_rem(initial_set);

	std::list<Flowpipe> result;
	std::list<Flowpipe> flowpipes_end;

	flowpipes_end.push_back(initial_set);

	std:: chrono::duration< double > time_span;
	std :: chrono :: steady_clock::time_point start_time;
	std :: chrono :: steady_clock::time_point end_time;

	map< string, double > timing_information;

	std :: chrono :: steady_clock::time_point t1 = std :: chrono :: steady_clock::now();


	compute_flowpipes_for_n_steps(
		X0, no_of_steps, no_of_flowpipes, polynomial_degree_for_controller,
		plant, crs, controller_graph, controller_input_indices,
		controller_output_indices, result,  filename_to_save,
		plot_dimensions, timing_information);


	std :: chrono :: steady_clock::time_point t2 = std::chrono::steady_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	cout << "Total execution time =  " << time_span.count() << " seconds." << endl;

	cout << "\% of time spent in regression = " << timing_information["time_in_regression"] * 100.0 << " \% " << endl;
	cout << "\% of  time spent in PWL construction = " << timing_information["time_in_pwl_construction"] * 100.0<< " \% "  << endl;
	cout << "\% of  time spent in Sherlock = " << timing_information["time_in_sherlock"] * 100.0 << " \% " <<  endl;
	cout << "\% of  time spent in Flowstar = " << timing_information["time_in_flowstar"] * 100.0 << " \% " << endl;
	cout << "Max Linear Pieces = " << timing_information["max_linear_pieces"] << endl;
	cout << "Max Error encountered = " << timing_information["max_error_encountered"] << endl;

	// plot the flowpipes in the x-y plane
	// FILE *fp = fopen("./Plots/Ex_1.m", "w");
	// plot_2D_interval_MATLAB(fp, "x", "y", stateVars, result);
	// // plot_2D_box_gnuplot(fp, "gnuplot", result, "x", "y", stateVars, cutoff);
	// fclose(fp);


	// max_difference and max_linear_pieces are global variables defined inside compute_flowpipes.h
	save_final_results_to_file(true, 1 ,4,0.02,2, timing_information["max_error_encountered"], time_span.count(),timing_information["time_in_regression"] * 100.0
	,timing_information["time_in_pwl_construction"] * 100.0, timing_information["time_in_sherlock"] * 100.0,
	timing_information["time_in_flowstar"] * 100.0, timing_information["max_linear_pieces"]);

	return 0;
}
