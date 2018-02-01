#include "./headers/propagate_intervals.h"
using namespace std;
using namespace std::chrono;
datatype network_offset = -100;
datatype offset_in_constraint_comb = constr_comb_offset;

int main()
{
	// Simple range propagation
	char controller_file[] = "./network_files/neural_network_information" ;
	network_handler system_network(controller_file);

	vector< vector< datatype > > input_interval(2, vector< datatype >(2,0));
	input_interval[0][0] = 0;input_interval[0][1] = 10;
	input_interval[1][0] = 0;input_interval[1][1] = 10;

	vector< vector< datatype > > input_constraints;
	create_constraint_from_interval(input_constraints, input_interval);

	clock_t begin, end;
	begin = clock();

	vector< datatype > output_range(2,0);

 	system_network.return_interval_output(input_constraints, output_range, 1);
	//
	cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;


	end = clock();
	printf("time cost for Sherlock ------------------ %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

	begin = clock();

	vector< vector< vector< datatype > > > weights;
	vector< vector< datatype > > biases;

	system_network.return_network_information(weights, biases);
	datatype milp_ret;
	vector< datatype > counter_example;
	datatype upper_lim;
	int direction = -1;
	milp_ret = do_MILP_optimization(input_constraints, weights, biases, counter_example, direction);
	cout << "Min found = " << milp_ret << endl;

	direction = 1;
	milp_ret = do_MILP_optimization(input_constraints, weights, biases, counter_example, direction);
	cout << "Max found = " << milp_ret << endl;

	end = clock();
	printf("time cost for Monolithic MILP ---------------------- %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);


	// clock_t begin, end;
	// begin = clock();
	// find_limits_using_reluplex(input_interval, output_range);
	// end = clock();
	// printf("time cost for reluplex : %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
	// cout << "output_range = [" << output_range[0] << " , " << output_range[1] << " ]" << endl;

	// Just verifying the limits using Reluplex
	steady_clock::time_point t1 = steady_clock::now();

	vector< datatype > limits(2);
	limits[0] = -1e10;
	limits[1] = output_range[0] - MILP_tolerance;

	int check = check_limits_using_reluplex(limits, input_interval, -1, counter_example);
	if(!check)
	{
		cout << "lower limits verified .. " << endl;
	}
	else
	{
		cout << "lower limits false .. " << endl;
	}

	limits[0] = output_range[1] + MILP_tolerance;
	limits[1] = 1e10;

	check = check_limits_using_reluplex(limits, input_interval, 1, counter_example);
	if(!check)
	{
		cout << "upper limits verified .. " << endl;
	}
	else
	{
		cout << "Upper limits false .. " << endl;
	}

	steady_clock::time_point t2 = steady_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	cout << "Reluplex time =  " << time_span.count() << " seconds." << endl;


  return 0;
}
