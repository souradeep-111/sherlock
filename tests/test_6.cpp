#include "sherlock_poly.h"
#include "sherlock.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char ** argv)
{
  sherlock_parameters.find_extra_directions = true;

  string prefix_name = "./network_files/previous_networks/neural_network_information";
  int number_of_files = 17;
  uint32_t output_index, sample_count = 1000;
  pair< double, double > output_range;
  string current_file_name;
  vector< uint32_t > input_node_indices, output_node_indices;
  map<uint32_t, pair< double, double > > interval;
  clock_t begin, end;

  int i = 12; // 6, 10
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

    polyhedral_abstraction sherlock_poly;
    sherlock sherlock_instance(CG);

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

    cout << "   ----------------------------   " << endl;
    cout << "Network index - " <<  i << endl;
    cout << "Computed output range by polyhedral propagation = [" <<
  	output_range.first << " , " << output_range.second << " ] " << endl;
    printf("time cost =========> %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);


    sherlock_instance.compute_output_range_by_sampling(region, output_index,
                                          output_range, sample_count);

    // sherlock_instance.compute_output_range(output_index, region, output_range);
    end = clock();

    cout << "Computed output range by sampling = [" <<
  	output_range.first << " , " << output_range.second << " ] " << endl;

    cout << "   ----------------------------   " << endl;
    i++;
  }


}
