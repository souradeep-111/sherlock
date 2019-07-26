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
	sherlock_parameters.thread_count = 1;
	sherlock sherlock_handler;
	computation_graph network_graph;
	region_constraints region;
	pair<double, double> output_range;
	string network_1_name = "./network_files/neural_network_information_7";
	vector<uint32_t> input_indices, output_indices;
	create_computation_graph_from_file(network_1_name, network_graph, true,
																		 input_indices, output_indices);
	sherlock_handler.set_computation_graph(network_graph);
	map<uint32_t, double> input_val;

	map<uint32_t, pair< double, double > > interval;
	for(auto index : input_indices)
	{
		interval[index] = make_pair(-0.5, 0.1);
		input_val[index] = 5;
	}
	region.create_region_from_interval(interval);

	sherlock_handler.compute_output_range_by_sampling(region, output_indices[0],
																										output_range, 1000);
	cout << "Computed output range from random sampling = [" <<
	output_range.first << " , " << output_range.second << " ] " << endl;

	sherlock_handler.compute_output_range(output_indices[0], region, output_range);
	cout << "Computed output range by Sherlock = [" <<
	output_range.first << " , " << output_range.second << " ] " << endl;

  return 0;
}
