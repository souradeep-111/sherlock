#include "sherlock_poly.h"
#include <dirent.h>

using namespace std;
using namespace std::chrono;


int main(int argc, char ** argv)
{
  clock_t begin, end;

  // sherlock_parameters.skip_invariant_addition = true;

  string network_source_path = "./Marabou/onnx/acasxu/";

  // LIST OF NETWORKS IN THE SOURCE FILE
  // Generating all the file names
  vector< string > network_names;
  string prefix_name = "ACASXU_experimental_v2a";

  network_names.clear();
  int limit_1 = 5, limit_2 = 9;
  for(int index_1 = 1; index_1 <= limit_1; index_1 ++)
  {
    for(int index_2 = 1; index_2 <= limit_2; index_2 ++)
    {
      string network_name;
      network_name = prefix_name;
      network_name += ("_" + to_string(index_1) + "_" + to_string(index_2));
      network_names.push_back(network_name);
    }
  }

  map<uint32_t, pair< double, double > > property_1_interval;



  // Convert each property to an interval based query

  string source_filename = network_source_path + network_names[0] + ".onnx";
  cout << "Source file name - " << source_filename << endl;
  computation_graph CG;
	onnx_parser my_parser(source_filename);
	map<string, ParameterValues <uint32_t> > tensor_mapping;
	my_parser.build_graph(CG, tensor_mapping);

  string input_name = "X";
  string output_name = "y_out";

  tensor_mapping[input_name].print() ;
  tensor_mapping[output_name].print();
  uint32_t node_index = 911;

  // CHECKING PROPERTY 1

  _point_ witness, output_vals;
  // Get a list of all the properties
  property_1_interval[1] = make_pair(0.6, 0.6798577687);
  property_1_interval[2] = make_pair(-0.5, 0.5);
  property_1_interval[3] = make_pair(-0.5, 0.5);
  property_1_interval[4] = make_pair(0.45, 0.5);
  property_1_interval[5] = make_pair(-0.5, -0.45);
  double property_1_output_low = 3.9911256459;

  // Output : node[911] >= 0

  sherlock_parameters.skip_invariant_addition = false;


  region_constraints region;
  region.create_region_from_interval(property_1_interval);
  pair< double, double > output_range;

  for(auto each_network : network_names)
  {
    source_filename = network_source_path + each_network + ".onnx";
    cout << "Source file name - " << source_filename << endl;
    CG.clear();
  	onnx_parser parser(source_filename);
  	parser.build_graph(CG, tensor_mapping);
    sherlock sherlock_instance(CG);


    begin = clock();
    bool status = sherlock_instance.gradient_driven_target(node_index, region, false, property_1_output_low, witness);
    end = clock();

    if(status)
    {
      cout << "Property Fails" << endl;
      cout << " Input Witness - ";
      print_point(witness);
      CG.evaluate_graph(witness, output_vals);
      cout << " Output vals - " << (output_vals[node_index]) << endl;
      cout << "Time taken : " << ((double)(end - begin) / CLOCKS_PER_SEC) << endl;
    }
    else
    {
      begin = clock();
      sherlock_instance.optimize_using_gradient(node_index, region, false, output_range.first);
      end = clock();

      if(output_range.first > property_1_output_low)
        cout << "Property holds " << endl;
      else
        cout << "Unable to prove property" << endl;
      cout << "Time taken : " << ((double)(end - begin) / CLOCKS_PER_SEC) << endl;
    }

    cout << " ------------------------------------------------ " << endl << endl;

  }



  cout << endl;


  return 0;
}
