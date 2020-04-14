#include "sherlock_poly.h"
#include "sherlock.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char ** argv)
{
  // sherlock_parameters.verbosity = true;
  // sherlock_parameters.grad_search_point_verbosity = true;
  sherlock_parameters.find_extra_directions = true;
  sherlock_parameters.skip_invariant_addition = true;
  sherlock_parameters.MILP_tolerance = 1e-3;

  string prefix_name = "./network_files/sine_net/300n/";
  vector< string > network_names;
  // network_names.push_back("sine_net_150_2.onnx");
  // network_names.push_back("sine_net_100_3.onnx");
  // network_names.push_back("sine_net_75_4.onnx");
  // network_names.push_back("sine_net_60_5.onnx");
  // network_names.push_back("sine_net_50_6.onnx");
  // network_names.push_back("sine_net_30_10.onnx");
  // network_names.push_back("sine_net_25_12.onnx");
  // network_names.push_back("sine_net_20_15.onnx");
  // network_names.push_back("sine_net_15_20.onnx");
  // network_names.push_back("sine_net_12_25.onnx");
  // network_names.push_back("sine_net_10_30.onnx");
  // network_names.push_back("sine_net_6_50.onnx");
  // network_names.push_back("sine_net_5_60.onnx"); // Sherlock time out
  network_names.push_back("sine_net_4_75.onnx"); // Polytime out
  network_names.push_back("sine_net_3_100.onnx");
  network_names.push_back("sine_net_2_150.onnx");

  int number_of_files = 50;
  uint32_t output_index, sample_count = 1000;
  pair< double, double > output_range;
  string current_file_name;
  vector< uint32_t > input_node_indices, output_node_indices;
  map<uint32_t, pair< double, double > > interval;
  clock_t begin, end;
  computation_graph CG;
  map<string, ParameterValues <uint32_t> > tensor_mapping;


  for(auto network_name : network_names)
  {
    current_file_name = prefix_name + network_name;

    onnx_parser parser(current_file_name);
    CG.clear();
    tensor_mapping.clear();
    parser.build_graph(CG, tensor_mapping);


    // tensor_mapping["input.1"] --> 1,2
    // tensor_mapping["16"] --> 104

    input_node_indices = tensor_mapping["input"].data_stash;
    output_node_indices = tensor_mapping["output"].data_stash;

  	interval.clear();
  	for(auto input_index : input_node_indices)
      interval[input_index] = make_pair(0.0, 1.0);

  	region_constraints region;
  	region.create_region_from_interval(interval);

    output_index = output_node_indices[0];

    polyhedral_abstraction sherlock_poly;
    sherlock sherlock_instance(CG);

    cout << "   ----------------------------   " << endl;
    cout << "Network name - " <<  current_file_name << endl;

    sherlock_parameters.find_extra_directions = true;

    // begin = clock();
    // region_constraints output_poly;
    // set < uint32_t > node_indices;
    // node_indices.insert(output_index);
    // sherlock_poly.propagate_polyhedrons(CG, region, output_poly, node_indices);
    // output_poly.overapproximate_polyhedron_as_rectangle(interval);
    // output_range = interval[output_index];
    //
    // end = clock();
    //
    // cout << "Computed output range by polyhedral propagation = [" <<
  	// output_range.first << " , " << output_range.second << " ] " << endl;
    // printf("time cost =========> %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

    sherlock_instance.compute_output_range_by_sampling(region, output_index,
      output_range, sample_count);
      cout << "Computed output range by sampling = [" <<
      output_range.first << " , " << output_range.second << " ] " << endl;

    begin = clock();
    sherlock_instance.compute_output_range(output_index, region, output_range);
    end = clock();
    cout << "Computed output range by Sherlock = [" <<
    output_range.first << " , " << output_range.second << " ] " << endl;
    printf("time cost =========> %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);


    cout << "   ----------------------------   " << endl;
  }


}
