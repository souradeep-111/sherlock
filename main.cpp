/*

Contributors to the tool :
Souradeep Dutta

email : souradeep.dutta@colorado.edu

LICENSE : Please see the license file, in the main directory

*/

#include "sherlock.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char ** argv)
{
	sherlock_parameters.thread_count = 3;


	// testing the data structures built on a very small network which can be analysed etc
	computation_graph sample_graph_a;
	test_network_2(sample_graph_a);

	//
	auto x = 5.0;
	auto y = -2.0;
	map< uint32_t, double > inputs;
	// inputs.insert(make_pair(1, x));
	// inputs.insert(make_pair(2, y));
	//
	map< uint32_t, double > outputs;
	map< uint32_t, double > gradient;
	// sample_graph_a.evaluate_graph(inputs, outputs);
	// inputs.clear();
	// inputs.insert(make_pair(1, x));
	// inputs.insert(make_pair(2, y));
	// gradient = sample_graph_a.return_gradient_wrt_inputs(7, inputs);
	// cout << "Value at x = " << x << "  and y = " << y << " is " << outputs[7] << endl;
	// cout << "Gradient = [ " << gradient[1] << " , " << gradient[2] <<" ] " << endl;

	sherlock sherlock_handler(sample_graph_a);
	map<uint32_t, pair< double, double > > interval;
	pair< double, double > output_range;
	interval[0] = make_pair(-5,5);
	interval[1] = make_pair(-5,5);
	region_constraints region;
	region.create_region_from_interval(interval);

	sherlock_handler.compute_output_range_by_sampling(region, 7, output_range, 1);
	cout << "Computed output range from random sampling = [" <<
	output_range.first << " , " << output_range.second << " ] " << endl;


	sherlock_handler.compute_output_range(7, region, output_range);
	cout << "Computed output range by Sherlock = [" <<
	output_range.first << " , " << output_range.second << " ] " << endl;


	computation_graph sample_graph_b;
	test_network_1(sample_graph_b);
	x = 5.0;
	y = -2.0;
	inputs.clear();
	inputs.insert(make_pair(1, x));
	inputs.insert(make_pair(2, y));

	outputs.clear();
	gradient.clear();
	sample_graph_b.evaluate_graph(inputs, outputs);
	inputs.clear();
	inputs.insert(make_pair(1, x));
	inputs.insert(make_pair(2, y));
	gradient = sample_graph_b.return_gradient_wrt_inputs(10, inputs);
	cout << "Value at x = " << x << "  and y = " << y << " is " << outputs[10] << endl;
	cout << "Gradient = [ " << gradient[1] << " , " << gradient[2] <<" ] " << endl;

	sherlock_handler.clear();
	sherlock_handler.set_computation_graph(sample_graph_b);
	interval.clear();
	interval[0] = make_pair(-1,1);
	interval[1] = make_pair(-1,1);
	region.create_region_from_interval(interval);
	sherlock_handler.compute_output_range_by_sampling(region, 10, output_range, 1000);
	cout << "Computed output range from random sampling = [" <<
	output_range.first << " , " << output_range.second << "]" << endl;

	sherlock_handler.compute_output_range(10, region, output_range);
	cout << "Computed output range by Sherlock = [" << output_range.first
	<< " , " << output_range.second << " ] " << endl;


  return 0;
}
