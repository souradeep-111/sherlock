#include "sherlock.h"
#include "sherlock_poly.h"
#include "sherlock_message.pb.h"
using namespace std;
using namespace std::chrono;

bool verbose_python_parsing = true;

int main(int argc, char ** argv)
{
  assert(argc == 2);
  string filename(argv[1]);
  string extension = ".sherlock_message";


  GOOGLE_PROTOBUF_VERIFY_VERSION;

  // Read the Sherlock message received as the command line argument
  __sherlock__ :: sherlock_message sherlock_problem;
  fstream input_file(filename, ios::in | ios :: binary);
  if( ! sherlock_problem.ParseFromIstream(& input_file))
  {
    cerr << "Failed to parse Sherlock Message " << endl;
  }

  // Create the network description
  __sherlock__::network network_description = sherlock_problem.network_description();

  // Name of the network, also figures out which file reader to call.
  string onnx_network_filename = network_description.onnx_filename();
  string sherlock_format_file = network_description.old_format_filename();
  computation_graph CG;
  vector< uint32_t > input_indices, output_indices;
  string input_tensor_name, output_tensor_name;
  map<string, ParameterValues <uint32_t> > tensor_mapping;
	map<uint32_t, pair< double, double > > input_interval;

  if(!sherlock_format_file.empty())
  {
    if(verbose_python_parsing)
      cout << "Name of the Sherlock format file - " << sherlock_format_file << endl;

    bool has_output_relu = network_description.has_output_relu_in_old_style();
  	create_computation_graph_from_file(sherlock_format_file, CG, has_output_relu,
  																		 input_indices, output_indices);

  }
  else if(!onnx_network_filename.empty())
  {
    if(verbose_python_parsing)
      cout << "Name of the onnx file - " << onnx_network_filename << endl;

    onnx_parser parser(onnx_network_filename);
    parser.build_graph(CG, tensor_mapping);
  }


  input_tensor_name = network_description.input_tensor_name();
  output_tensor_name = network_description.output_tensor_name();
  ParameterValues<uint32_t> input_tensor, output_tensor;
  input_tensor = tensor_mapping[input_tensor_name];
  output_tensor = tensor_mapping[output_tensor_name];

  if(verbose_python_parsing)
  {
    cout << "Input tensor name - " << input_tensor_name << endl;
    cout << "Output tensor name - " << output_tensor_name << endl;
  }

  input_indices.clear();
  for(int index = 0; index < input_tensor.data_stash.size(); index++)
    input_indices.push_back ( input_tensor.data_stash[index] );

  output_indices.clear();
  for(int index = 0; index < output_tensor.data_stash.size(); index++)
    output_indices.push_back( output_tensor.data_stash[index] );


  // Assert the interval constraints

  __sherlock__ :: interval input_interval_msg = sherlock_problem.input_interval();
  assert( input_interval_msg.limits_size() == input_indices.size());

  input_interval.clear();
  for(int index = 0; index < input_interval_msg.limits_size(); index++)
  {
    __sherlock__ :: mapping map = input_interval_msg.limits(index);
    uint32_t order_no = map.node_index();
    assert(order_no == index);
    uint32_t node_index_in_graph_constructed = input_indices[order_no];
    input_interval[node_index_in_graph_constructed] = make_pair(map.lower_limit(), map.upper_limit());
    if(verbose_python_parsing)
    {
      cout << "\t For input node - " << index << " interval - [ " << map.lower_limit() << " , "
      << map.upper_limit() << "]" << endl;
    }
  }


  region_constraints input_region;
  input_region.create_region_from_interval(input_interval);

  uint32_t new_node_index = output_indices.back() + 1;
  _point_ input_witness;
  uint32_t objective_node_index;
  // Print the objective function
  __sherlock__::objective objective_function = sherlock_problem.optimization_problem();
  bool sense;
  double optima;


  if(objective_function.direction() !=  __sherlock__::objective::MPC)
  {
    // Add a new node with the optimization objective
    sense = objective_function.direction();

    new_node_index++;
    objective_node_index = new_node_index;
    node  node_x(objective_node_index, "none");
    CG.add_new_node(objective_node_index, node_x);
    CG.mark_node_as_output(objective_node_index);
    CG.set_bias_of_node(objective_node_index, objective_function.constant());

    if(verbose_python_parsing)
    {
      cout << "The objective function is : " ;
      cout << objective_function.constant() << " + ";
    }

    for(int index = 0; index < objective_function.linear_terms_size(); index++)
    {
      __sherlock__ :: linear_combo term = objective_function.linear_terms(index);

      uint32_t order_no = term.node_index();
      uint32_t node_index = output_indices[order_no];

      double coeff = term.coefficient();

      CG.connect_node1_to_node2_with_weight(node_index, objective_node_index, coeff);


      if(verbose_python_parsing)
      cout << term.coefficient() << " node_" << order_no << " + ";
    }
  }
  else
  {
    if(verbose_python_parsing)
      cout << "Doing an MPC " << endl;

    assert(objective_function.direction() == __sherlock__ :: objective :: MPC);
    __sherlock__ :: MPC_details mpc_details = objective_function.mpc_structure();
    __sherlock__ :: point target_point = mpc_details.goal_state();

    _point_ target_state; // Note the mapping index here would in the order of output indices
    for(int index = 0; index < target_point.values_size(); index++ )
    {
      __sherlock__::value_binding value_pair = target_point.values(index);
      target_state[value_pair.node_index()] = value_pair.value();
    }

    vector< uint32_t > new_output_nodes;

    // Create a new set of nodes with the target as the bias terms
    for(auto each_pair : target_state)
    {
      uint32_t node_index_in_graph = output_indices[each_pair.first];

      new_node_index ++;
      uint32_t current_node_index = new_node_index;
      node node_x(current_node_index, "none");
      CG.add_new_node(current_node_index, node_x);
      CG.set_bias_of_node(current_node_index, - each_pair.second);
      CG.connect_node1_to_node2_with_weight(node_index_in_graph, current_node_index, 1.0);

      uint32_t diff_node_index = current_node_index;
      // Output[i] = Relu(x) + Relu(-x), 'x' is the difference

      new_node_index ++;
      uint32_t positive_diff_index = new_node_index;

      node positive_x(positive_diff_index, "relu");
      CG.add_new_node(positive_diff_index, positive_x);
      CG.set_bias_of_node(positive_diff_index, 0.0);
      CG.connect_node1_to_node2_with_weight(diff_node_index, positive_diff_index, 1.0);


      new_node_index ++;
      uint32_t negative_diff_index = new_node_index;
      node negative_x(negative_diff_index, "relu");
      CG.add_new_node(negative_diff_index, negative_x);
      CG.set_bias_of_node(negative_diff_index, 0.0);
      CG.connect_node1_to_node2_with_weight(diff_node_index, negative_diff_index, -1.0);


      new_node_index ++;
      uint32_t output_node_index = new_node_index;
      node mod_x(output_node_index, "none");
      CG.add_new_node(output_node_index, mod_x);
      CG.set_bias_of_node(output_node_index, 0.0);
      CG.connect_node1_to_node2_with_weight(positive_diff_index, output_node_index, 1.0);
      CG.connect_node1_to_node2_with_weight(negative_diff_index, output_node_index, 1.0);

      new_output_nodes.push_back(output_node_index);
    }



    new_node_index++;
    objective_node_index = new_node_index;
    node  objective_node(objective_node_index, "none");
    CG.add_new_node(objective_node_index, objective_node);
    CG.clear_output_nodes();
    CG.mark_node_as_output(objective_node_index);
    CG.set_bias_of_node(objective_node_index, 0.0);

    for(int index = 0; index < new_output_nodes.size(); index++)
      CG.connect_node1_to_node2_with_weight(new_output_nodes[index], objective_node_index, 1.0);


  }

  if(verbose_python_parsing)
    cout << "Sense is  " << sense << endl;

  if(objective_function.status_flag() == __sherlock__::objective::NOT_STARTED)
  {

    sherlock_parameters.verbosity = false;
    // Do the acutal optimization
    sherlock sherlock_handler(CG);

    // if MPC it's always minimize
    if(objective_function.direction() ==  __sherlock__::objective::MPC)
      sense = false;

    sherlock_handler.optimize_using_gradient(objective_node_index, input_region,
                                                  sense, optima, input_witness);

    // pair< double, double > range;
    // sherlock_handler.compute_output_range_by_sampling( input_region, objective_node_index, range, 1000);
    // cout << "Range - " << range.first << " --- " << range.second << endl;

    sherlock_problem.set_optima_val(optima);
    __sherlock__ :: point * Point = sherlock_problem.mutable_witness();

    for(auto each_term : input_witness)
    {
      assert(find(input_indices.begin(), input_indices.end(), each_term.first) != input_indices.end());
      __sherlock__ :: value_binding * some_binding = Point->add_values();
      some_binding->set_node_index(each_term.first);
      some_binding->set_value(each_term.second);
    }

    __sherlock__::objective * objective_details = sherlock_problem.mutable_optimization_problem();
    objective_details->set_status_flag(__sherlock__::objective::DONE);
  }

  // Change the output interval and see if you can write it back
  fstream output_file(filename, ios::out | ios::trunc | ios::binary);
  if (! sherlock_problem.SerializeToOstream(& output_file))
  {
    cerr << "Failed to write address book." << endl;
    return -1;
  }


  google::protobuf::ShutdownProtobufLibrary();



  return 0;
}
