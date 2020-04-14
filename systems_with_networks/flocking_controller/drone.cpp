#include "sherlock_poly.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char  ** argv)
{
  sherlock_parameters.thread_count = 1;
  sherlock_parameters.do_incremental_constant_search = true;
  sherlock_parameters.verbosity = false;
  sherlock_parameters.grad_search_point_verbosity = false;
  sherlock_parameters.time_verbosity = false;
  sherlock_parameters.skip_invariant_guarantees_in_binarization = true;
  sherlock_parameters.skip_invariant_addition = false;
  sherlock_parameters.MILP_M = 1e4;
  sherlock_parameters.verbose_onnx = false;
  sherlock_parameters.use_gurobi_internal_constraints = true;

  string onnx_file = "./systems_with_networks/flocking_controller/drone_controller.onnx";
  computation_graph CG;
  onnx_parser my_parser(onnx_file);
  map<string, ParameterValues < uint32_t > > tensor_mapping;
  my_parser.build_graph(CG, tensor_mapping);

  // Trying a sample range propagation through the controller
  map< uint32_t, pair< double, double > > interval;

  double delta = 0.1;
  interval[1] = make_pair(0, delta);
  interval[2] = make_pair(0, delta);
  interval[3] = make_pair(0, delta);
  interval[4] = make_pair(0, delta);
  interval[5] = make_pair(0, delta);
  interval[6] = make_pair(0, delta);
  interval[7] = make_pair(0, delta);
  interval[8] = make_pair(0, delta);

  region_constraints region;
  region.create_region_from_interval(interval);

  uint32_t output_index = 1009;
  pair<double, double > output_range;
  sherlock sherlock_instance(CG);
  // sherlock_instance.compute_output_range(output_index, region, output_range);
  // Expected Value : [-0.812381 , -0.783142 ]
  cout << "Computed output range by Sherlock = [" <<
	output_range.first << " , " << output_range.second << " ] " << endl;

  output_index = 1010;
  // sherlock_instance.compute_output_range(output_index, region, output_range);
  // Expected Value : [0.50932 , 0.511107 ]
  cout << "Computed output range by Sherlock = [" <<
	output_range.first << " , " << output_range.second << " ] " << endl;


  polyhedral_abstraction sherlock_poly;
  region_constraints output_polyhedron;
  set < uint32_t > output_indices;
  output_indices.insert(output_index);

  sherlock_poly.propagate_polyhedrons(CG, region, output_polyhedron, output_indices);

  cout << " ------ Output polyhedron computed ----- " << endl;
  output_polyhedron.print();


  // sherlock_instance.compute_output_range_by_sampling(region, output_index, output_range, 1000);
  // cout << "Computed output range by sampling = [" <<
	// output_range.first << " , " << output_range.second << " ] " << endl;

  // Plant Dynamics Here
  // x(k+1) = x(k) + dt * v(k)
  // v(k+1) = v(k) + dt * a(k)

  // Number of plants in the current neural network : 2
  // Order of inputs seems to be : [relative pos, velocity]



}
