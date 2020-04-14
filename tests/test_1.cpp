#include "sherlock_poly.h"
#include "sherlock.h"

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

  string prefix_name = "./network_files/previous_networks/neural_network_information";
  int number_of_files = 17;
  uint32_t output_index, sample_count = 1000;
  pair< double, double > output_range;
  string current_file_name;
  vector< uint32_t > input_node_indices, output_node_indices;
  map<uint32_t, pair< double, double > > interval;
  clock_t begin, end;

  int i = 0;
  while(i < number_of_files)
  {
    current_file_name = prefix_name;
    current_file_name += ("_" + to_string(i));
    computation_graph CG;
    create_computation_graph_from_file(current_file_name, CG, true,
                        input_node_indices, output_node_indices);

  	interval.clear();
  	for(auto input_index : input_node_indices)
    {
      if((i >= 0) && (i <= 5))
        interval[input_index] = make_pair(0.0, 10.0);
      else if((i == 6) || (i == 8) || (i == 9))
        interval[input_index] = make_pair(-0.5, 0.5);
      else if((i == 7) || ((i >= 10) && (i <= 15)))
        interval[input_index] = make_pair(-0.1,0.1);
    }

    if(i >= 11)
      sherlock_parameters.MILP_tolerance = 0.5;

  	region_constraints region;
  	region.create_region_from_interval(interval);

    output_index = output_node_indices[0];

    sherlock sherlock_instance(CG);
    begin = clock();
    sherlock_instance.compute_output_range(output_index, region, output_range);
    end = clock();

    cout << "   ----------------------------   " << endl;
    cout << "Network index - " <<  i << endl;
    cout << "Computed output range by Sherlock = [" <<
  	output_range.first << " , " << output_range.second << " ] " << endl;

    sherlock_instance.compute_output_range_by_sampling(region, output_index,
                                          output_range, sample_count);
		printf("time cost =========> %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

    cout << "Computed output range by sampling = [" <<
  	output_range.first << " , " << output_range.second << " ] " << endl;

    cout << "   ----------------------------   " << endl;




    i++;
  }


}
