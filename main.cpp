	/*

Contributors to the tool :
Souradeep Dutta

email : souradeep.dutta@colorado.edu

LICENSE : Please see the license file, in the main directory

*/

#include "sherlock.h"
#include "sherlock_poly.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char ** argv)
{
	sherlock_parameters.thread_count = 1;
	sherlock_parameters.do_incremental_constant_search = true;
	sherlock_parameters.verbosity = false;
	sherlock_parameters.grad_search_point_verbosity = false;
	sherlock_parameters.time_verbosity = false;
	sherlock_parameters.skip_invariant_guarantees_in_binarization = true;
	sherlock_parameters.skip_invariant_addition = true;
	sherlock_parameters.MILP_M = 1e4;
	sherlock_parameters.verbose_onnx = false;
	sherlock_parameters.use_gurobi_internal_constraints = true;
	sherlock_parameters.find_extra_directions = true;

	string onnx_file = "./network_files/sample_network.onnx";
	// string onnx_file = "./network_files/cifar_network.onnx";
	string deep_2_neuron_file = "./network_files/simple_deep_2.onnx";
	string nnet_to_onnx = "./automating_run/ACASXU_experimental_v2a_1_1.onnx";

	computation_graph CG_1;
	onnx_parser my_parser_1(deep_2_neuron_file);
	map<string, ParameterValues <uint32_t> > tensor_mapping_1;
	my_parser_1.build_graph(CG_1, tensor_mapping_1);
	// for(auto each_pair : tensor_mapping_1)
	// {
	// 	cout << each_pair.first << endl;
	// 	each_pair.second.print();
	// }
	test_poly_abstr_simple(CG_1);

	exit(0);

	computation_graph CG_2;
	onnx_parser my_parser_2(nnet_to_onnx);
	map<string, ParameterValues <uint32_t> > tensor_mapping_2;
	my_parser_2.build_graph(CG_2, tensor_mapping_2);
	for(auto each_pair : tensor_mapping_2)
	{
		cout << each_pair.first << endl;
		each_pair.second.print();
	}


	exit(0);

	onnx_parser my_parser(onnx_file);
	computation_graph CG;
	map<string, ParameterValues < uint32_t > > tensor_mapping;
	my_parser.build_graph(CG, tensor_mapping);

	cout << "Checking outputs " << endl;
	map<uint32_t, double> in, out;
	read_image_as_point("image.txt", tensor_mapping["input.1"], in);
	CG.evaluate_graph(in, out);
	//
	// cout << "Printing graph outputs : " << endl;
	// for(auto output_pair : out)
	// {
	// 	cout << "At " << output_pair.first << " -- " << output_pair.second << endl;
	// }

	map<uint32_t, pair< double, double > > interval;
	interval.clear();
	for(auto input : in)
	{
		interval[input.first] = make_pair(input.second - 0.001, input.second + 0.001);
	}
	region_constraints region;
	region.create_region_from_interval(interval);

	uint32_t output_index = 25553; // 45-54
	pair <double, double > output_range;
	sherlock sherlock_handler(CG);
	// sherlock_handler.compute_output_range(output_index, region, output_range);
	// cout << "Computed output range by Sherlock = [" <<
	// output_range.first << " , " << output_range.second << " ] " << endl;


	// testing the data structures built on a very small network which can be analysed etc
	computation_graph sample_graph_a;
	test_network_sigmoid(sample_graph_a);

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



	sherlock_handler.set_computation_graph(sample_graph_a);

	interval.clear();
	interval[1] = make_pair(0,5);
	interval[2] = make_pair(0,5);
	region.create_region_from_interval(interval);

	sherlock_handler.compute_output_range_by_sampling(region, 7, output_range, 1000);
	cout << "Computed output range from random sampling = [" <<
	output_range.first << " , " << output_range.second << " ] " << endl;

	double max;
	sherlock_handler.optimize_node(7, true, region, max);
	double min;
	sherlock_handler.optimize_node(7, false, region, min);

	cout << "Result by optimization = [ " << min << " , " << max << " ] " << endl;

	sherlock_handler.compute_output_range(7, region, output_range);
	cout << "Computed output range by Sherlock = [" <<
	output_range.first << " , " << output_range.second << " ] " << endl;


	map<int, double > some_map;
	some_map[-1] = -1;
	some_map[1] = 1;
	some_map[2] = 1;
	linear_inequality my_inequality(some_map);
	vector< linear_inequality > collection;
	collection.push_back(my_inequality);

	sherlock_handler.optimize_constrained(7, true, region, collection, max);
	sherlock_handler.optimize_constrained(7, false, region, collection, min);
	cout << "Constrained output range computed by Sherlock = ["
	<< min << " , " << max << " ] " << endl;

	cout << "Starting selective binarization : " << endl;
	set< uint32_t > binarized_neurons;
	if(sherlock_handler.prove_bounds(7, output_range.second + 1, true,
		region, binarized_neurons))
	{
		cout << "Binarization sucessful " << endl;
		cout << "Neurons binarized : " ;
		for(auto each_neuron : binarized_neurons)
			cout << " " << each_neuron << " ";

		cout << endl;

	}


	computation_graph network_graph;
	string network_1_name = "./network_files/previous_networks/neural_network_information_5";
	vector<uint32_t> input_indices, output_indices;
	create_computation_graph_from_file(network_1_name, network_graph, true,
																		 input_indices, output_indices);
	sherlock_handler.set_computation_graph(network_graph);
	map<uint32_t, double> input_val;


	interval.clear();
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

	// sherlock_handler.optimize_node(output_indices[0], true, region, max);
	// cout << "Node count for max = " << sherlock_handler.nodes_explored << endl;
	// sherlock_handler.optimize_node(output_indices[0], false, region, min);
	// cout << "Node count for min = " << sherlock_handler.nodes_explored << endl;
	// cout << "Result by optimization = [ " << min << " , " << max << " ] " << endl;


	// sherlock_parameters.verbosity = true;
	// sherlock_parameters.grad_search_point_verbosity = true;
	// sherlock_handler.compute_output_range(output_indices[0], region, output_range);
	// cout << "Computed output range by Sherlock = [" <<
	// output_range.first << " , " << output_range.second << " ] " << endl;
	// cout << "Node count = " << sherlock_handler.nodes_explored << endl;

	if(sherlock_handler.prove_bounds(output_indices[0], output_range.second + 10, true,
																	 region, binarized_neurons))
	{
		cout << "Binarization sucessful " << endl;
		cout << "Neurons binarized : " ;
		for(auto each_neuron : binarized_neurons)
			cout << " " << each_neuron << " ";

		cout << endl;

	}

  return 0;
}
