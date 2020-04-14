#ifndef _onnx_parser_h
#define _onnx_parser_h

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "onnx.pb.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <map>
#include "computation_graph.h"
#include <assert.h>
#include <stdint.h>
#include <bitset>

using namespace std;

// Roughly speaking this is the list of operators that has been implemented
// in Sherlock's Format

// The naming of the different operators here, are exactly in line with the documentation
// in ONNX format description. The webpage that might be useful here is the
// following : https://github.com/onnx/onnx/blob/master/docs/Operators.md

const string Gemm("Gemm");
const string Relu("Relu");
const string Conv("Conv");
const string AveragePool("AveragePool");
const string BatchNormalization("BatchNormalization");
const string MaxPool("MaxPool");
const string Constant("Constant");
const string Reshape("Reshape");
const string _Transpose_("Transpose");
// The above weird naming is because Eigen has a similar name
const string MatMul("MatMul");
const string Add("Add");
const string Flatten("Flatten");
const string Slice("Slice");
const string Concat("Concat");

typedef vector< uint32_t > _id_list_;
typedef vector< int > _shape_;
typedef vector< string > _name_list_;
typedef map<string, _id_list_ > string_to_id_list;
typedef map<string, _name_list_ > string_to_string_list;

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
} __FLOAT__;

int64_t convert_string(string input_string);


// This basically takes care of the row major formation of the way the
// tensor values are stored in the data stash
template <class T>
class ParameterValues{
public:
  vector<int> dimension_values;
  vector<T> data_stash;

  ParameterValues();
  void clear();
  void set_dimension(vector< int > dim_val);
  void set_data(vector< T > & data_val);
  uint32_t compute_row_major_index(vector<int> dimension_limits,
                                   vector<int> indices);
  vector<int> compute_indices(vector<int> dimension_limits,
                              int offset_amount);
  T get_value(vector<int> index);
  ParameterValues <T> get_sub_tensor(vector< int > dimension_indices_from_left);
  void push_sub_tensor(int dimension_from_left, ParameterValues<T> & p_value);

  void add_node_tensor(ParameterValues & p_value,
                       map< uint32_t , node> & node_id_to_node,
                       computation_graph & CG);

  void perform_convolution(ParameterValues < uint32_t > & image_input,
                           ParameterValues < double > & kernel_weights,
                           string autopad, vector<int> dilations, int group,
                           vector<int> kernel_shape, vector<int> padding_size, vector<int> strides,
                           uint32_t begin_val, uint32_t & end_val,
                           map<uint32_t, node > & node_id_to_node, computation_graph & CG);

  void perform_AveragePool(ParameterValues < uint32_t > & input_tensor,
                           int ceil_mode, int count_include_pad, vector< int > kernel_shape,
                           vector< int > padding_size, vector< int > strides,
                           uint32_t begin_val, uint32_t & end_val,
                           map<uint32_t, node >& node_id_to_node, computation_graph & CG);

  void perform_MaxPool(ParameterValues < uint32_t > & input_tensor,
                          int ceil_mode, int storage_order, vector< int > kernel_shape,
                          vector< int > padding_size, vector< int > strides,
                          vector< int > dilations, uint32_t begin_val, uint32_t & end_val,
                          map<uint32_t, node >& node_id_to_node, computation_graph & CG);

  void perform_simple_MaxPool(ParameterValues < uint32_t > & image_slice,
                           uint32_t begin_val, uint32_t & end_val,
                           map< uint32_t, node > & node_id_to_node,
                           computation_graph & CG);

  void perform_pairwise_Max(pair<uint32_t, uint32_t> & index_pair,
                            uint32_t begin_val, uint32_t & end_val,
                            map< uint32_t, node > & node_id_to_node,
                            computation_graph & CG);

  void perform_BatchNormalization(ParameterValues <uint32_t> & input_tensor,
                                  double bias, double scale,
                                  uint32_t begin_val, uint32_t & end_val,
                                  map<uint32_t, node> & node_id_to_node, computation_graph & CG);

  ParameterValues<T> & operator= (const ParameterValues<T> & p_values);

  void print();

  ParameterValues <T> return_flatten(int cut_axis);
};

class onnx_parser
{
private:
  string input_filename;
  uint32_t node_count;
public:
  onnx_parser();
  onnx_parser(string filename);
  void build_graph(computation_graph & CG,
                   map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes);

  bool read_graph_proto(onnx::GraphProto & graph_proto,
                        map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                        computation_graph & CG);

  void declare_nodes_for_shape( onnx::TensorShapeProto tensor_shape_proto,
                                bool input_nodes,
                                map< uint32_t, node >& node_id_to_node,
                                ParameterValues<uint32_t> & p_value,
                                computation_graph & CG);

  void parse_all_nodes(onnx::GraphProto & graph_proto,
                       vector < string > & initializer_names,
                       map < string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                       map < string, ParameterValues < double > > & parameters_map,
                       map<uint32_t, node > & node_id_to_node,
                       computation_graph & CG);

  bool read_tensor_proto(onnx::TensorProto & tensor_proto,
                         ParameterValues < double > & p_value);


  int read_node_proto(onnx::NodeProto & node_proto,
                      map < string, ParameterValues < uint32_t > > &  tensor_name_to_nodes,
                      map < string, ParameterValues < double > > & parameters_map,
                      map < uint32_t, node > & node_id_to_node,
                      computation_graph & CG);

  void implement_Transpose(onnx::NodeProto & node_proto,
                          map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                          map< string, ParameterValues < double > > & parameters_map,
                          map< uint32_t, node > & node_id_to_node,
                          computation_graph & CG);

  void implement_MatMul(onnx::NodeProto & node_proto,
                        map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                        map< string, ParameterValues < double > > & parameters_map,
                        map< uint32_t, node > & node_id_to_node,
                        computation_graph & CG);

  void implement_Add(onnx::NodeProto & node_proto,
                     map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                     map< string, ParameterValues < double > > & parameters_map,
                     map< uint32_t, node > & node_id_to_node,
                     computation_graph & CG);


  void implement_Gemm(onnx::NodeProto & node_proto,
                     map< string, ParameterValues < uint32_t > > & tensor_names_to_nodes,
                     map< string, ParameterValues < double > > & parameters_map,
                     map< uint32_t, node > & node_id_to_node,
                     computation_graph & CG);

   void implement_Relu(onnx::NodeProto & node_proto,
                      map< string, ParameterValues < uint32_t > > & tensor_names_to_nodes,
                      map< string, ParameterValues < double > > & parameters_map,
                      map< uint32_t, node > & node_id_to_node,
                      computation_graph & CG);

   void implement_Conv(onnx::NodeProto & node_proto,
                     map< string, ParameterValues < uint32_t > > & tensor_names_to_nodes,
                     map< string, ParameterValues < double > > & parameters_map,
                     map< uint32_t, node > & node_id_to_node,
                     computation_graph & CG);

   void implement_AveragePool(onnx::NodeProto & node_proto,
                      map< string, ParameterValues < uint32_t > > & tensor_names_to_nodes,
                      map< string, ParameterValues < double > > & parameters_map,
                      map< uint32_t, node > & node_id_to_node,
                      computation_graph & CG);

   void implement_BatchNormalization(onnx::NodeProto & node_proto,
                       map< string, ParameterValues < uint32_t > > & tensor_names_to_nodes,
                       map< string, ParameterValues < double > > & parameters_map,
                       map< uint32_t, node > & node_id_to_node,
                       computation_graph & CG);

   void implement_MaxPool(onnx :: NodeProto & node_proto,
                        map< string, ParameterValues <uint32_t > > & tensor_name_to_nodes,
                        map< string, ParameterValues <double > > & parameters_map,
                        map< uint32_t, node > & node_id_to_node,
                        computation_graph & CG);

  void implement_Constant(onnx:: NodeProto & node_proto,
                          map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                          map< string, ParameterValues < double > > & parameters_map,
                          map< uint32_t , node> & node_id_to_node,
                          computation_graph & CG);

  void implement_Reshape(onnx:: NodeProto & node_proto,
                         map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                         map< string, ParameterValues < double > > & parameters_map,
                         map< uint32_t , node> & node_id_to_node,
                         computation_graph & CG);

   void implement_Flatten(onnx::NodeProto & node_proto,
                       map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                       map< string, ParameterValues < double > > & parameters_map,
                       map< uint32_t, node > & node_id_to_node,
                       computation_graph & CG);
   void implement_Slice(onnx::NodeProto & node_proto,
                       map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                       map< string, ParameterValues < double > > & parameters_map,
                       map< uint32_t, node > & node_id_to_node,
                       computation_graph & CG);

   void implement_Concat(onnx::NodeProto & node_proto,
                        map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                        map< string, ParameterValues < double > > & parameters_map,
                        map< uint32_t, node > & node_id_to_node,
                        computation_graph & CG);

   void weight_times_nodes( double scaling_factor,
                            ParameterValues <double> weight_values,
                            ParameterValues <uint32_t> node_indices,
                            ParameterValues <uint32_t> & result,
                            map<uint32_t, node>& node_id_to_node,
                            computation_graph & CG);

   void nodes_times_weight( double scaling_factor,
                            ParameterValues <uint32_t> node_indices,
                            ParameterValues <double> weight_values,
                            ParameterValues <uint32_t> & result,
                            map<uint32_t, node>& node_id_to_node,
                            computation_graph & CG);

   void set_bias( ParameterValues <double> bias_values,
                  ParameterValues <uint32_t> node_indices,
                  computation_graph & CG);
};

template<typename T>
void transpose_matrix(ParameterValues <T> & p_value);

void make_padded_image_from(ParameterValues< uint32_t > & image_input,
                            string autopad, vector<int> padding_size,
                            ParameterValues< int > & padded_image);

void reshape_vector(vector< double > reshape_, vector< int > & result);
#endif
