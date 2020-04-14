#include <iostream>
#include <fstream>
#include "sherlock_message.pb.h"
using namespace std;

int main()
{

  GOOGLE_PROTOBUF_VERIFY_VERSION;
  sherlock :: sherlock_message message_sample;

  fstream input_file("sherlock_problem.sherlock_message", ios::in | ios::binary);

  if (! message_sample.ParseFromIstream(& input_file))
  {
     cerr << "Failed to parse Sherlock Message " << endl;
      return -1;
  }


  // Printing the network description

  sherlock::network network_descr = message_sample.network_description();

  // Name of the network, also figures out which file reader to call.
  string onnx_network_filename = network_descr.onnx_filename();
  string sherlock_format_file = network_descr.old_format_filename();


  if(!sherlock_format_file.empty())
    cout << "Name of the Sherlock format file - " << sherlock_format_file << endl;
  else if(!onnx_network_filename.empty())
    cout << "Name of the onnx file - " << onnx_network_filename << endl;

  // Printing the input node indices of the neural network
  cout << "Input node indices are  : ";
  for(int index = 0; index < network_descr.input_node_indices_size(); index++)
    cout << network_descr.input_node_indices(index) << " , ";
  cout << endl;

  // Printing the output node indices of the neural network
  vector< uint32_t > output_index;
  cout << "Output node indices are  : ";
  for(int index = 0; index < network_descr.output_node_indices_size(); index++)
  {
    cout << network_descr.output_node_indices(index) << " , ";
    output_index.push_back(network_descr.output_node_indices(index));
  }
  cout << endl;

  // Printing the input intervals
  cout << "The input intervals are  : " << endl;
  sherlock :: interval input_interval = message_sample.input_interval();
  for(int index = 0; index < input_interval.limits_size(); index++)
  {
    sherlock :: mapping map = input_interval.limits(index);
    cout << "\t" << map.node_index() << " - [" << map.lower_limit() << " , " << map.upper_limit() << "]" << endl;
  }


  // Printing the output intervals
  cout << "The output intervals are  : " << endl;
  sherlock :: interval output_interval = message_sample.output_interval();
  for(int index = 0; index < output_interval.limits_size(); index++)
  {
    sherlock :: mapping map = output_interval.limits(index);
    cout << "\t" << map.node_index() << " - [" << map.lower_limit() << " , " << map.upper_limit() << "]" << endl;
  }

  // Print the objective function
  sherlock::objective objective_function = message_sample.optimization_problem();
  cout << "Sense is - " << objective_function.direction() << endl;
  cout << "The objective function is : " ;
  for(int index = 0; index < objective_function.linear_terms_size(); index++)
  {
    sherlock :: linear_combo term = objective_function.linear_terms(index);
    cout << term.coefficient() << " node_" << term.node_index() << " + ";
  }
  cout << endl;



  // Modifying the output interval
  cout << "The output intervals being set. " << endl;
  sherlock::interval * modified_output_interval = message_sample.mutable_output_interval();
  for(int index = 0; index < output_index.size(); index++)
  {
    sherlock :: mapping * map = modified_output_interval->add_limits();
    map->set_node_index(output_index[index]);
    map->set_lower_limit(-100.0);
    map->set_upper_limit(100.0);
    // cout << "\t" << map.node_index() << " - [" << map.lower_limit() << " , " << map.upper_limit() << "]" << endl;
  }


  // Change the output interval and see if you can write it back
  fstream output_file("sherlock_answer.sherlock_message", ios::out | ios::trunc | ios::binary);
    if (!message_sample.SerializeToOstream(& output_file)) {
      cerr << "Failed to write address book." << endl;
      return -1;
    }


  google::protobuf::ShutdownProtobufLibrary();
  return 0;

}

// Compiling this code :
// g++ -c reading_sherlock_msg.cpp -o reading_sherlock_msg.o -std=c++17
// g++ -o runfile sherlock_message.pb.o reading_sherlock_msg.o -lprotobuf -std=c++17 -pthread
