#include "sherlock_poly.h"
#include <dirent.h>

using namespace std;
using namespace std::chrono;

bool prove_property(
  map<uint32_t, pair< double, double > >  property_interval,
  string filename);

int main(int argc, char ** argv)
{

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

  map<uint32_t, pair< double, double > >  property_4_interval;
  clock_t begin, end;

  // Convert each property to an interval based query

  string source_filename = network_source_path + network_names[0] + ".onnx";
  cout << "Source file name - " << source_filename << endl;
  computation_graph CG;
	onnx_parser my_parser(source_filename);
	map<string, ParameterValues <uint32_t> > tensor_mapping;
	my_parser.build_graph(CG, tensor_mapping);
  vector< linear_inequality > inequalities;

  string input_name = "X";
  string output_name = "y_out";

  tensor_mapping[input_name].print() ;
  tensor_mapping[output_name].print();


  // CHECKING PROPERTY 2

  uint32_t x0, x1, x2, x3, x4;
  x0 = 1; x1 = 2; x2 = 3; x3 = 4; x4 = 5;
  double lower_limit_1, upper_limit_1, lower_limit_2, upper_limit_2;
  uint32_t no_of_divs = 20;
  lower_limit_1 = 0.3181818182; upper_limit_1 = 0.5;
  lower_limit_2 = 0.0833333333; upper_limit_2 = 0.1666666667;
  double delta_1, delta_2;
  delta_1 = (upper_limit_1 - lower_limit_1) / ((double)no_of_divs);
  delta_2 = (upper_limit_2 - lower_limit_2) / ((double)no_of_divs);

  bool status;

  // Get a list of all the properties
  property_4_interval[x0] = make_pair(-0.3035311561, -0.2985528119);
  property_4_interval[x1] = make_pair(-0.0095492966, 0.0095492966);
  property_4_interval[x2] = make_pair(0.0, 0.0);
  property_4_interval[x3] = make_pair(0.3181818182, 0.5);
  property_4_interval[x4] = make_pair(0.0833333333, 0.1666666667);

  // Output : node[911] >= 0

  sherlock_parameters.skip_invariant_addition = true;
  sherlock_parameters.verbosity = false;
  // sherlock_parameters.grad_search_point_verbosity = true;
  // sherlock_parameters.MILP_tolerance = 1e-2;



  for(auto each_network : network_names)
  {
    source_filename = network_source_path + each_network + ".onnx";
    cout << "Source file name - " << source_filename << endl;

    begin = clock();

    status = true;
    // Considering smaller intervals
    for(int dim_1 = 0; dim_1 < no_of_divs; dim_1++)
    {
      for(int dim_2 = 0; dim_2 < no_of_divs; dim_2++)
      {
        property_4_interval[x3] = make_pair(
          lower_limit_1 + (delta_1 * dim_1), lower_limit_1 + (delta_1*(dim_1 + 1))
        );
        property_4_interval[x4] = make_pair(
          lower_limit_2 + (delta_2 * dim_2), lower_limit_2 + (delta_2*(dim_2 + 1))
        );

        status = prove_property(property_4_interval, source_filename);

        if(!status)
          break;
        cout << "Sub division number - " << (dim_1 * no_of_divs + dim_2) << endl;
      }
      if(!status)
       break;
    }
    if(status)
      cout << "Property holds" << endl;
    else
      cout << "Property Could not be proved " << endl;

    end = clock();
    cout << "Time taken : " << ((double)(end - begin) / CLOCKS_PER_SEC) << endl;
    cout << " ------------------------------------------------ " << endl << endl;


  }



  cout << endl;


  return 0;
}


bool prove_property(map<uint32_t, pair< double, double > >  property_interval,
                    string filename)
{


  _point_ witness, output_vals;
  pair< double, double > output_range;
  map<string, ParameterValues <uint32_t> > tensor_mapping;

  uint32_t y0, y1, y2, y3, y4;
  y0 = 911; y1 = 912; y2 = 913; y3 = 914; y4 = 915;
  uint32_t x0, x1, x2, x3, x4;
  x0 = 1; x1 = 2; x2 = 3; x3 = 4; x4 = 5;
  double property_3_upper_bound = 0.0;

  bool status, status_1, status_2, status_3, status_4;
  region_constraints region;
  region.create_region_from_interval(property_interval);

  computation_graph CG;
  CG.clear();
  onnx_parser parser(filename);
  parser.build_graph(CG, tensor_mapping);

  uint32_t node_index_1 = y4 + 1;
  node node_x_1(node_index_1, "none");
  CG.add_new_node(node_index_1, node_x_1);
  CG.connect_node1_to_node2_with_weight(y0, node_index_1, 1.0);
  CG.connect_node1_to_node2_with_weight(y1, node_index_1, -1.0);
  CG.set_bias_of_node(node_index_1, 0.0);

  uint32_t node_index_2 = y4 + 2;
  node node_x_2(node_index_2, "none");
  CG.add_new_node(node_index_2, node_x_2);
  CG.connect_node1_to_node2_with_weight(y0, node_index_2, 1.0);
  CG.connect_node1_to_node2_with_weight(y2, node_index_2, -1.0);
  CG.set_bias_of_node(node_index_2, 0.0);

  uint32_t node_index_3 = y4 + 3;
  node node_x_3(node_index_3, "none");
  CG.add_new_node(node_index_3, node_x_3);
  CG.connect_node1_to_node2_with_weight(y0, node_index_3, 1.0);
  CG.connect_node1_to_node2_with_weight(y3, node_index_3, -1.0);
  CG.set_bias_of_node(node_index_3, 0.0);

  uint32_t node_index_4 = y4 + 4;
  node node_x_4(node_index_4, "none");
  CG.add_new_node(node_index_4, node_x_4);
  CG.connect_node1_to_node2_with_weight(y0, node_index_4, 1.0);
  CG.connect_node1_to_node2_with_weight(y4, node_index_4, -1.0);
  CG.set_bias_of_node(node_index_4, 0.0);



  status_1 = false; status_2 = false; status_3 = false; status_4 = false;

  sherlock sherlock_instance(CG);


  sherlock_parameters.no_of_random_restarts = 1e2;
  // PART 1
  status = sherlock_instance.gradient_driven_target(node_index_1, region, true,
                                                  property_3_upper_bound, witness);


  if(status)
  {
    status_1 = false;
    cout << "Property Fails" << endl;
    cout << " Input Witness - ";
    print_point(witness);
    CG.evaluate_graph(witness, output_vals);
    cout << " Output vals - " << (output_vals[node_index_1]) << endl;
    return false;
  }
  else
  {
    cout << "Attempting to prove property ....... " << endl;

    sherlock_instance.gradient_driven_optimization(node_index_1, region, true,  output_range.first,
                                                  witness);

    if(output_range.first <= property_3_upper_bound)
      status_1 = true;
    else
    {
      status_1 = false;
      cout << "Limit Computed - " << output_range.first << endl;
      cout << "Witness - " ;
      print_point(witness);
      return false;
    }
  }

  // PART 2
  status = sherlock_instance.gradient_driven_target(node_index_2, region, true,
                                                  property_3_upper_bound, witness);


  if(status)
  {
    status_2 = false;
    cout << "Property Fails" << endl;
    cout << " Input Witness - ";
    print_point(witness);
    CG.evaluate_graph(witness, output_vals);
    cout << " Output vals - " << (output_vals[node_index_2]) << endl;
    return false;
  }
  else
  {
    cout << "Attempting to prove property ....... " << endl;

    sherlock_instance.gradient_driven_optimization(node_index_2, region, true, output_range.first,
                                                  witness);

    if(output_range.first <= property_3_upper_bound)
      status_2 = true;
    else
    {
      status_2 = false;
      cout << "Limit Computed - " << output_range.first << endl;
      cout << "Witness - " ;
      print_point(witness);
      return false;
    }
  }



  // PART 3
  status = sherlock_instance.gradient_driven_target(node_index_3, region, true,
                                                  property_3_upper_bound, witness);


  if(status)
  {
    status_3 = false;
    cout << "Property Fails" << endl;
    cout << " Input Witness - ";
    print_point(witness);
    CG.evaluate_graph(witness, output_vals);
    cout << " Output vals - " << (output_vals[node_index_3]) << endl;
    return false;
  }
  else
  {
    cout << "Attempting to prove property ....... " << endl;

    sherlock_instance.gradient_driven_optimization(node_index_3, region, true, output_range.first,
                                                  witness);
    if(output_range.first <= property_3_upper_bound)
      status_3 = true;
    else
    {
      cout << "Limit Computed - " << output_range.first << endl;
      cout << "Witness - " ;
      print_point(witness);
      status_3 = false;
      return false;

    }
  }


  // PART 4
  status = sherlock_instance.gradient_driven_target(node_index_4, region, true,
                                                  property_3_upper_bound, witness);


  if(status)
  {
    status_4 = false;
    cout << "Property Fails" << endl;
    cout << " Input Witness - ";
    print_point(witness);
    CG.evaluate_graph(witness, output_vals);
    cout << " Output vals - " << (output_vals[node_index_4]) << endl;
    return false;
  }
  else
  {
    cout << "Attempting to prove property ....... " << endl;
    sherlock_instance.gradient_driven_optimization(node_index_4, region, true, output_range.first,
                                                  witness);

    if(output_range.first <= property_3_upper_bound)
      status_4 = true;
    else
    {
      status_4 = false;
      cout << "Limit Computed - " << output_range.first << endl;
      cout << "Witness - " ;
      print_point(witness);
      return false;
    }
  }

  if(status_1 && status_2 && status_3 && status_4)
    return true;
  else
    return false;




}
