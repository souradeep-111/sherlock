#include <iostream>
#include <sstream>
#include <iomanip>
#include <fcntl.h>
#include <fstream>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "onnx.pb.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

int read_onnx_graph(onnx::GraphProto graph_proto);
void read_attribute(onnx::AttributeProto attr_proto);
void read_initializer(onnx::TensorProto tensor_proto);
typedef union {

    float f;
    struct
    {
        // Order is important.
        // Here the members of the union data structure
        // use the same memory (32 bits).
        // The ordering is taken
        // from the LSB to the MSB.
        char c_0 : 8;
        char c_1 : 8;
        char c_2 : 8;
        char c_3 : 8;
    } raw;
} myfloat;

// Function to convert a binary array
// to the corresponding integer
unsigned int convertToInt(vector<unsigned int > arr, int low, int high)
{
    unsigned f = 0, i;
    for (i = high; i >= low; i--) {
        f = f + arr[i] * pow(2, high - i);
    }
    return f;
};


int main()
{

  onnx::ModelProto model_proto;
  string filename("sample_network.onnx");
  std::fstream input(filename.c_str(), std::ios::in | std::ios::binary);
  bool isSuccess = model_proto.ParseFromIstream(& input);
  cout << "Reading success flag : " << isSuccess << endl;
  if(!isSuccess)
  {
    cout << "Could not read the Onnx input file, exiting " << endl;
    return -1;
  }

  if(!model_proto.has_graph())
  {
    cout << "No network graph found in the onnx file " << endl;
    return -1;
  }
  else
  {
    cout << "Version of the ONNX format is " << model_proto.ir_version() << endl;
    cout << "Number of operators used = " << model_proto.opset_import_size() << endl;
    onnx::OperatorSetIdProto opset_id_proto = model_proto.opset_import(0);
    cout << "Opset id proto domain = " << opset_id_proto.domain() << endl;
    cout << "Opset id proto version = " << opset_id_proto.version() << endl;

    cout << "Producer name = " << model_proto.producer_name() << endl;
    cout << "Producer version = " << model_proto.producer_version() << endl;
    cout << "Domain = " << model_proto.domain() << endl;
    cout << "Model version = " << model_proto.model_version() << endl;
    onnx::GraphProto graph_proto = model_proto.graph();
    if(read_onnx_graph(graph_proto) < 0)
    {
      cout << "Could not parse Onnx graph " << endl;
      return -2;
    }
    else
    {
      cout << "Onnx graph successfully read ! " << endl;
    }
  }

  return 0;
}

int read_onnx_graph(onnx::GraphProto graph_proto)
{
  cout << "Name of the graph = " << graph_proto.name() << endl;
  cout << "Doc string for the graph = "<< graph_proto.doc_string() << endl;

  cout << "Number of inputs to the graph = " << graph_proto.input_size() << endl;
  cout << "Inputs to the graph are : " << endl;
  for(int i = 0; i < graph_proto.input_size() ; i++)
  {
    onnx::ValueInfoProto value_info_proto = graph_proto.input(i);
    cout << "Name : " << value_info_proto.name() << "  " << endl;
    cout << "\t Document string : " << value_info_proto.doc_string() << endl;
    onnx::TypeProto type_proto = value_info_proto.type();
    onnx::TypeProto::Tensor tensor_proto = type_proto.tensor_type();
    cout << "\t Element type = " << tensor_proto.elem_type() << endl;
    onnx::TensorShapeProto tensor_shape_proto = tensor_proto.shape();
    cout << "\t No of dimensions = " << tensor_shape_proto.dim_size() << endl;
    cout << "\t";
    for(int j =0;j < tensor_shape_proto.dim_size() ; j++)
    {
      onnx::TensorShapeProto::Dimension dim_proto = tensor_shape_proto.dim(j);
      cout << " " << dim_proto.dim_value() << " ";
    }
    cout << endl;


  }
  cout << endl;
  cout << "Number of outputs of the graph = " << graph_proto.output_size() << endl;

  cout << "Number of nodes in the graph = " << graph_proto.node_size() << endl;
  for(int i = 0; i < graph_proto.node_size();i++)
  {
    cout << " --------------------------------------------- " << endl;
    onnx::NodeProto node_proto = graph_proto.node(i);
    cout << "Node proto name = " << node_proto.name() << endl;
    cout << "\t No of inputs to the node = " << node_proto.input_size() << endl;
    cout << "\t Inputs are : " ;
    for(int j = 0; j < node_proto.input_size() ; j++)
    {
      cout << node_proto.input(j) << " " ;
    }
    cout << endl;

    cout << "\t No of outputs of the node = " << node_proto.output_size() << endl;
    cout << "\t Outputs are : ";
    for(int j = 0; j < node_proto.output_size() ; j++)
    {
      cout << node_proto.output(j) << " " ;
    }
    cout << endl;

    cout << "\t Operator to execute at this node : " << node_proto.op_type() << endl;
    cout << "\t Doc string = " << node_proto.doc_string() << endl << endl;

    cout << "\t Number of attributes : " << node_proto.attribute_size() << endl;
    for(int j = 0; j < node_proto.attribute_size();j++)
    {
      onnx::AttributeProto attribute_proto  = node_proto.attribute(j);
      read_attribute(attribute_proto);
    }

  }

  cout <<  "No of value info for the graph : " << graph_proto.value_info_size() << endl;

  for(int i =0; i < graph_proto.value_info_size(); i++)
  {
    onnx::ValueInfoProto value_info_proto = graph_proto.value_info(i);
    cout << "Name : " << value_info_proto.name() << endl;
  }

  cout << "Reading the initializers of the Graph Proto " << endl;
  cout << "Number  of intializers = " << graph_proto.initializer_size() << endl;
  for(int i = 0; i < graph_proto.initializer_size(); i++)
  {
    onnx::TensorProto tensor_proto;
    tensor_proto = graph_proto.initializer(i);
    read_initializer(tensor_proto);
  }

  return 0;
}

void read_attribute(onnx::AttributeProto attr_proto)
{
  cout << "Printing Attribute details " << endl;
  cout << "Attribute name : " << attr_proto.name() << endl;
  cout << "Documentation : " << attr_proto.doc_string() << endl;
  cout << "Attribute type : " << attr_proto.type() << endl;
  cout << "Attribute value : " << attr_proto.f() << endl;

}

void read_initializer(onnx::TensorProto tensor_proto)
{
  // Print the shape of the tensor
  cout << "Tensor Name : " << tensor_proto.name() << endl;

  // Print the data type of the tensor
  cout << "Data type of the tensor : " << tensor_proto.data_type() << endl;

  // Printing the dimensions
  cout << "Number of dimensions : " << tensor_proto.dims_size() << endl;
  cout << "\t [ " ;
  for(int j = 0; j < tensor_proto.dims_size(); j++)
  {
    cout << tensor_proto.dims(j) << " , " ;
  }
  cout << "]" << endl;

  cout << "Size of Float data = " << tensor_proto.float_data_size() << endl;
  cout << "Size of Double data = " << tensor_proto.double_data_size() << endl;
  string raw_data = tensor_proto.raw_data();

  if(tensor_proto.data_type() == onnx::TensorProto::FLOAT)
  {
    cout << "Data stash of the current attribute : " << endl;
    cout << "[ ";
    for(int j = 0; j < (raw_data.size()/4); j++)
    {
      myfloat var;
      string sample = raw_data.substr(j*4,4);
      var.raw.c_0 = sample.at(0);
      var.raw.c_1 = sample.at(1);
      var.raw.c_2 = sample.at(2);
      var.raw.c_3 = sample.at(3);
      cout << var.f << " , ";

      // cout << sample << endl;
      // unsigned int buffer  = stoi(sample, nullptr, 2);
      // ieee.push_back(buffer);
      //
      // myfloat var;
      // unsigned f = convertToInt(ieee, 9, 31);
      // var.raw.mantissa = f ;
      // f = convertToInt(ieee, 1, 8);
      // var.raw.exponent = f;
      // var.raw.sign = ieee[0];
      // cout << var.f << " , ";

    }
    cout << "]" << endl;

  }
}
