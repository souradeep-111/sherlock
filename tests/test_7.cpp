#include "sherlock_poly.h"
#include "sherlock.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char ** argv)
{
  sherlock_parameters.find_extra_directions = true;

  string prefix_name = "./network_files/sine_net/sine_net_const_width";
  int number_of_files = 50;
  uint32_t output_index, sample_count = 1000;
  pair< double, double > output_range;
  string current_file_name;
  vector< uint32_t > input_node_indices, output_node_indices;
  map<uint32_t, pair< double, double > > interval;
  clock_t begin, end;
  computation_graph CG;
  map<string, ParameterValues <uint32_t> > tensor_mapping;


  int i = 35; // 9 - time out
  while(i <= number_of_files)
  {
    current_file_name = prefix_name;
    current_file_name += ("_" + to_string(i) + "_20.onnx" );

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
      interval[input_index] = make_pair(0.0, 0.5);

  	region_constraints region;
  	region.create_region_from_interval(interval);

    output_index = output_node_indices[0];

    polyhedral_abstraction sherlock_poly;
    sherlock sherlock_instance(CG);

    cout << "   ----------------------------   " << endl;
    cout << "Network name - " <<  current_file_name << endl;

    sherlock_parameters.find_extra_directions = true;
    begin = clock();
    region_constraints output_poly;
    set < uint32_t > node_indices;
    node_indices.insert(output_index);
    sherlock_poly.propagate_polyhedrons(CG, region, output_poly, node_indices);
    output_poly.overapproximate_polyhedron_as_rectangle(interval);
    output_range = interval[output_index];

    // sherlock_instance.compute_output_range(output_index, region, output_range);
    end = clock();

    cout << "Computed output range by polyhedral propagation = [" <<
  	output_range.first << " , " << output_range.second << " ] " << endl;
    printf("time cost =========> %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

    sherlock_instance.compute_output_range_by_sampling(region, output_index,
      output_range, sample_count);
      cout << "Computed output range by sampling = [" <<
      output_range.first << " , " << output_range.second << " ] " << endl;

    // begin = clock();
    // sherlock_instance.compute_output_range(output_index, region, output_range);
    // end = clock();
    // cout << "Computed output range by Sherlock = [" <<
    // output_range.first << " , " << output_range.second << " ] " << endl;
    // printf("time cost =========> %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);




    cout << "   ----------------------------   " << endl;
    i++;
  }


}
