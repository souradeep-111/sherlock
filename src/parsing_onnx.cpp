#include "parsing_onnx.h"

using namespace std;

bool debug_onnx = false;

// Assumption : 0th element in the vector is the outer most index

template<class T>
ParameterValues<T> :: ParameterValues()
{
  dimension_values.clear();
  data_stash.clear();
}

template<class T>
void ParameterValues <T> :: clear()
{
  dimension_values.clear();
  data_stash.clear();
}

template<class T>
void ParameterValues <T> :: set_dimension(vector<int> dim_val)
{
  assert(! dim_val.empty());
  dimension_values = dim_val;
}

template<class T>
void ParameterValues <T> :: set_data(vector< T >& data_val)
{
  data_stash = data_val;
}

template<class T>
uint32_t ParameterValues <T> :: compute_row_major_index(vector<int> dimension_limits,
                                                     vector<int> indices)

{
  assert(indices.size() == dimension_limits.size());

  for(int i = 0; i < indices.size(); i++)
  {
    assert(indices[i] < dimension_limits[i]);
  }

  map<int, int> element_limits_count;
  // Computing the size limits for each dimension
  int product = 1;
  for(int i = 0; i < dimension_values.size(); i++)
  {
    element_limits_count[(dimension_limits.size()-1) - i] = product;
    product *= dimension_limits[(dimension_limits.size()-1) - i];

  }

  // Accessing the mentioned element
  uint32_t offset_amount = 0;
  for(int i = 0; i < indices.size(); i++)
  {
    offset_amount += (indices[i] * element_limits_count[i]);
  }

  return offset_amount;
}

template<class T>
vector<int> ParameterValues <T> :: compute_indices(vector<int> dimension_limits,
                                                int offset_amount)
{

  vector<int> indices;
  map<int, int> element_limits_count;

  // Computing the size limits for each dimension
  int product = 1;
  for(int i = 0; i < dimension_values.size(); i++)
  {
    element_limits_count[(dimension_limits.size()-1) - i] = product;
    product *= dimension_limits[(dimension_limits.size()-1) - i];
    // product *= dimension_limits[i]
  }

  indices.clear();
  int current_index = 0;
  for(int some_index = 0; some_index < dimension_limits.size(); some_index++ )
  {
    current_index = (int) (offset_amount/(element_limits_count[indices.size()])) ;
    offset_amount = offset_amount % (element_limits_count[indices.size()]) ;
    indices.push_back(current_index);
  }

  assert(indices.size() == dimension_limits.size());

  for(int i = 0; i < indices.size(); i++)
  {
    assert(indices[i] < dimension_limits[i]);
  }

  return indices;
}

template<class T>
T ParameterValues <T> :: get_value(vector<int> indices)
{
  uint32_t offset_amount = compute_row_major_index(dimension_values, indices);
  return data_stash[offset_amount];
}

template<class T>
ParameterValues<T> ParameterValues <T> ::get_sub_tensor(vector< int > dimension_indices_from_left)
{
  assert(dimension_indices_from_left.size() < dimension_values.size());
  ParameterValues <T> return_val;
  // int starting_index = dimension_values.size() - dimension_indices_from_left.size() - 1;
  int starting_index = dimension_indices_from_left.size();
  int no_of_elements_to_be_read = 1;

  vector<int> indices = dimension_indices_from_left;

  for(int index = starting_index; index < dimension_values.size(); index++)
  {
    indices.push_back(0);
    return_val.dimension_values.push_back(dimension_values[index]);
    no_of_elements_to_be_read *= dimension_values[index];
  }

  // int start_val = get_value(indices);
  int start_val = compute_row_major_index(dimension_values, indices);


  return_val.data_stash.clear();
  vector<int> temp_index(dimension_values.size(), 0);
  for(int index = start_val; index < (start_val + no_of_elements_to_be_read); index++)
  {
    return_val.data_stash.push_back(data_stash[index]);
  }

  return return_val;
}

template<class T>
void ParameterValues <T> ::push_sub_tensor(int dimension_from_left, ParameterValues<T> & p_value)
{
  assert(dimension_from_left < (dimension_values.size()-1));
  assert(dimension_from_left >= 0);
  assert(!p_value.data_stash.empty());


  dimension_values[dimension_from_left]++;

  for(T element : p_value.data_stash)
  {
    data_stash.push_back(element);
  }

  for(int index = 0; index < p_value.dimension_values.size(); index ++ )
  {
    dimension_values[dimension_from_left + 1 + index] = p_value.dimension_values[index];
  }
}

template<class T>
void ParameterValues <T> ::add_node_tensor(ParameterValues & p_value,
                                       map< uint32_t , node> & node_id_to_node,
                                       computation_graph & CG)
{
    assert(data_stash.size() == p_value.data_stash.size());
    assert( (this->dimension_values).size() == p_value.dimension_values.size());
    for(int each_dim = 0; each_dim < p_value.dimension_values.size(); each_dim++)
    {
      assert( (this->dimension_values)[each_dim] == p_value.dimension_values[each_dim] );
    }

    for(uint32_t index = 0; index < data_stash.size(); index++)
    {
      CG.connect_node1_to_node2_with_weight(p_value.data_stash[index], data_stash[index], 1.0);
    }
}

template<class T>
void ParameterValues <T> ::perform_convolution(ParameterValues < uint32_t > & image_input,
                                           ParameterValues < double > & kernel_weights,
                                           string autopad, vector<int> dilations, int group,
                                           vector<int> kernel_shape, vector<int> padding_size, vector<int> strides,
                                           uint32_t begin_val, uint32_t & end_val,
                                           map<uint32_t, node > & node_id_to_node, computation_graph & CG)
{

  uint32_t node_index, data_stash_index;
  uint32_t height_index, width_index;


  assert( image_input.dimension_values.size() == 2);
  assert( kernel_weights.dimension_values.size() == 2);
  assert( strides.size() == 2);
  assert(group == 1);

  if(!dilations.empty())
  {
    for(auto each_dilation : dilations)
    {
      // Most probably you don't need this assertion, but just that this
      // hasn't been tested for cases other than 1
      assert(each_dilation == 1);
    }
  }

  ParameterValues<int> paddded_image;
  vector<int> indices;
  bool create_node;
  if(dimension_values.empty() || data_stash.empty())
  {
    dimension_values.clear();
    data_stash.clear();
    create_node  = true;
    node_index = begin_val;
    // Computing the height :
    dimension_values.push_back( ((image_input.dimension_values[0] - kernel_weights.dimension_values[0] +
                               padding_size[0] + padding_size[1])/strides[0]) + 1 );

    // Computing the width :
    dimension_values.push_back( ((image_input.dimension_values[1] - kernel_weights.dimension_values[1] +
                              padding_size[2] + padding_size[3])/strides[1]) + 1 );

  }
  else
  {
    data_stash_index = 0;
    create_node = false;
  }

  // Assert that group = 1

  // Pad the image according to the padding size, and autopad parameter
  ParameterValues<int> padded_image;
  make_padded_image_from(image_input, autopad, padding_size, padded_image);
  ParameterValues <uint32_t> slice_of_image;

  height_index = 0;
  width_index = 0;

  int count = 0;

  while(/* height index can be moved down, or width index can be moved right */
          ( height_index <= (padded_image.dimension_values[0] - (kernel_weights.dimension_values[0] * dilations[0])) )  ||
          ( width_index <= (padded_image.dimension_values[1] - (kernel_weights.dimension_values[1] * dilations[1]))  )
       )
  {
    // Take a slice of the image input according to the kernel weights shape

    slice_of_image.dimension_values = kernel_weights.dimension_values;
    slice_of_image.data_stash.clear();
    for(int slice_height = 0;
        slice_height < ((slice_of_image.dimension_values[0]  - 1)* dilations[0] + 1);
        slice_height += dilations[0])
    {
      for(int slice_width = 0;
          slice_width < ((slice_of_image.dimension_values[1] - 1)* dilations[1] + 1);
          slice_width += dilations[1])
      {
        indices.clear();
        indices.push_back(slice_height + height_index);
        indices.push_back(slice_width + width_index);

        uint32_t node_index = image_input.get_value(indices);
        assert(node_index > 0);
        slice_of_image.data_stash.push_back(node_index);
      }
    }


    // Do the convolution operation, and
    //  if (create a node == true) : make a node and assign to it
    //  else { find the node you should be adding this thing to and do it}

    // if width coordinate can be moved move it.
    // else, if height index, can be moved, then reset width index, and
    // move height index

    if(create_node)
    {
      node_index ++ ;
      node node_x(node_index, "none");
      CG.add_new_node(node_index, node_x);

      node_id_to_node[node_index] = node_x;
      data_stash.push_back(node_index);
      end_val = node_index;
    }
    else
    {
      node_index = data_stash[data_stash_index];
      data_stash_index++;
    }

    for(int conv_index = 0; conv_index < slice_of_image.data_stash.size(); conv_index++)
    {
      if(slice_of_image.data_stash[conv_index] != -1)
      {
        CG.connect_node1_to_node2_with_weight(slice_of_image.data_stash[conv_index] ,
                                               node_index,
                                               kernel_weights.data_stash[conv_index]);
      }

    }

    if( (width_index + strides[1]) <= (padded_image.dimension_values[1] - ((kernel_weights.dimension_values[1] - 1)* dilations[1] + 1) ))
    {
      width_index += strides[1];
    }
    else if( (height_index + strides[0]) <= (padded_image.dimension_values[0] - ((kernel_weights.dimension_values[0] - 1)* dilations[0] + 1) ))
    {
      width_index = 0;
      height_index += strides[0];
    }
    else
    {
      break;
    }
  }

}

template<class T>
void ParameterValues <T> ::perform_AveragePool(ParameterValues < uint32_t > & input_tensor,
                                          int ceil_mode, int count_include_pad, vector< int > kernel_shape,
                                          vector< int > padding_size, vector< int > strides,
                                          uint32_t begin_val, uint32_t & end_val,
                                          map<uint32_t, node >& node_id_to_node, computation_graph & CG)
{


  assert(kernel_shape.size() == 2);
  dimension_values.clear();
  data_stash.clear();

  dimension_values.push_back(1); // Batch size is 1
  dimension_values.push_back(input_tensor.dimension_values[1]); // Channel count is same as the previous one
  for(int dim_index = 2; dim_index < input_tensor.dimension_values.size(); dim_index++)
  {
    int pad_size = padding_size[(dim_index - 2) * 2] + padding_size[(dim_index - 2) * 2 + 1];
    if(ceil_mode)
    {
      dimension_values.push_back(
        ceil(
               ((input_tensor.dimension_values[dim_index] + pad_size - kernel_shape[dim_index-2]) /
               strides[dim_index-2])
               + 1)
       );

    }
    else
    {
      dimension_values.push_back(
        floor(
               ((input_tensor.dimension_values[dim_index] + pad_size - kernel_shape[dim_index-2]) /
               strides[dim_index-2])
               + 1)
       );
    }
  }

  ParameterValues<int> padded_image;
  ParameterValues<uint32_t> current_image_channel;
  uint32_t output_image_height, output_image_width;
  uint32_t height_index, width_index;

  vector<int> indices;

  uint32_t node_index = begin_val;

  // Pad the image according to the padding size, and autopad parameter
  for(int channel_index = 0; channel_index < input_tensor.dimension_values[1]; channel_index++)
  {
    indices.clear();
    indices.push_back(0);
    indices.push_back(channel_index);
    current_image_channel = input_tensor.get_sub_tensor(indices);
    make_padded_image_from(current_image_channel,"NOTSET", padding_size, padded_image);

    ParameterValues <uint32_t> slice_of_image;
    height_index = 0;
    width_index = 0;
    output_image_height = 0;
    output_image_width = 0;

    while(/* height index can be moved down, or width index can be moved right */
            // ((height_index + strides[0]) < (padded_image.dimension_values[0] - kernel_shape[0]) )  ||
            // ((width_index + strides[1]) < (padded_image.dimension_values[1] - kernel_shape[1]))

            (output_image_height < dimension_values[0]) ||
            (output_image_width < dimension_values[1])
          )
    {
      // Take a slice of the image input according to the kernel weights shape
      slice_of_image.dimension_values = kernel_shape;
      slice_of_image.data_stash.clear();
      for(int slice_height = 0; slice_height < slice_of_image.dimension_values[0]; slice_height++)
      {
        for(int slice_width = 0; slice_width < slice_of_image.dimension_values[1]; slice_width++)
        {
          indices.clear();
          indices.push_back(slice_height + height_index);
          indices.push_back(slice_width + width_index);

          uint32_t node_id = current_image_channel.get_value(indices);
          slice_of_image.data_stash.push_back(node_id);
        }
      }


      // Do the convolution operation, and
      //  if (create a node == true) : make a node and assign to it
      //  else { find the node you should be adding this thing to and do it}

      // if width coordinate can be moved move it.
      // else, if height index, can be moved, then reset width index, and
      // move height index
      node_index ++ ;
      node node_x(node_index, "none");
      CG.add_new_node(node_index, node_x);

      node_id_to_node[node_index] = node_x;
      data_stash.push_back(node_index);
      end_val = node_index;

      int count= 0;
      for(int conv_index = 0; conv_index < slice_of_image.data_stash.size(); conv_index++)
      {
        if((!count_include_pad) && (slice_of_image.data_stash[conv_index] > 0))
        {
          count++;
        }
      }
      if(count_include_pad)
      {
        count = slice_of_image.data_stash.size();
      }


      for(int avg_index = 0; avg_index < slice_of_image.data_stash.size(); avg_index++)
      {
        if(slice_of_image.data_stash[avg_index] != -1)
        {
          CG.connect_node1_to_node2_with_weight(slice_of_image.data_stash[avg_index] ,
                                                 node_index, 1.0/((double)count) );
        }

      }

      CG.set_bias_of_node(node_index, 0.0);

      if( output_image_width < dimension_values[1] )
      {
        output_image_width++;
        assert((width_index + strides[1] ) <= (padded_image.dimension_values[1] - kernel_shape[1]));
        width_index += strides[1];
      }
      else if(output_image_height < dimension_values[0])
      {
        output_image_height++;
        assert((height_index + strides[0] ) <= (padded_image.dimension_values[0] - kernel_shape[0]));
        width_index = 0.0;
        height_index += strides[0];
      }
      else
      {
        break;
      }

    }


  }

}


template<class T>
void ParameterValues <T> ::perform_MaxPool(ParameterValues < uint32_t > & input_tensor,
                                          int ceil_mode, int storage_order, vector< int > kernel_shape,
                                          vector< int > padding_size, vector< int > strides,
                                          vector< int > dilations, uint32_t begin_val, uint32_t & end_val,
                                          map<uint32_t, node >& node_id_to_node, computation_graph & CG)
{

  // 2 dimensional kernel expected, but you can convert a 1d kernel into a 2d one very easily
  assert(kernel_shape.size() == 2);
  dimension_values.clear();
  data_stash.clear();

  if(!dilations.empty())
  {
    for(auto each_dilation : dilations)
    {
      // Most probably you don't need this assertion, but just that this
      // hasn't been tested for cases other than 1
      assert(each_dilation == 1);
    }
  }
  else
  {
    dilations.resize(kernel_shape.size());
    fill(dilations.begin(), dilations.end(), 1);
  }

  dimension_values.push_back(1); // Batch size is 1
  dimension_values.push_back(input_tensor.dimension_values[1]); // Channel count is same as the previous one
  for(int dim_index = 2; dim_index < input_tensor.dimension_values.size(); dim_index++)
  {
    int pad_size = padding_size[(dim_index - 2) * 2] + padding_size[(dim_index - 2) * 2 + 1];
    if(ceil_mode)
    {
      dimension_values.push_back(
        ceil(
               ((input_tensor.dimension_values[dim_index] + pad_size -
                 ((kernel_shape[dim_index-2] - 1) * dilations[dim_index-2] + 1)) /
               strides[dim_index-2])
               + 1)
       );
    }
    else
    {
      dimension_values.push_back(
        floor(
               ((input_tensor.dimension_values[dim_index] + pad_size -
                 ((kernel_shape[dim_index-2] - 1) * dilations[dim_index-2] + 1)) /
               strides[dim_index-2])
               + 1)
       );
    }

  }

  ParameterValues<int> padded_image;
  ParameterValues<uint32_t> current_image_channel;
  uint32_t output_image_height, output_image_width;
  uint32_t height_index, width_index;
  uint32_t starting_node_id, ending_node_id;

  vector<int> indices;

  uint32_t node_index = begin_val;

  for(int channel_index = 0; channel_index < input_tensor.dimension_values[1]; channel_index++)
  {

    indices.clear();
    indices.push_back(0);
    indices.push_back(channel_index);

    current_image_channel = input_tensor.get_sub_tensor(indices);

    // Pad the image according to the padding size, and autopad parameter
    make_padded_image_from(current_image_channel,"NOTSET", padding_size, padded_image);

    ParameterValues <uint32_t> slice_of_image;
    height_index = 0;
    width_index = 0;
    output_image_height = 0;
    output_image_width = 0;

    while(/* height index can be moved down, or width index can be moved right */
            // ((height_index + strides[0]) < (padded_image.dimension_values[0] - kernel_shape[0]) )  ||
            // ((width_index + strides[1]) < (padded_image.dimension_values[1] - kernel_shape[1]))

            (output_image_height < dimension_values[2]) ||
            (output_image_width < dimension_values[3])
          )
    {

      // Take a slice of the image input according to the kernel weights shape
      slice_of_image.dimension_values = kernel_shape;
      slice_of_image.data_stash.clear();
      for(int slice_height = 0;
          slice_height < ((slice_of_image.dimension_values[0]  - 1)* dilations[0] + 1);
          slice_height += dilations[0])
      {
        for(int slice_width = 0;
            slice_width < ((slice_of_image.dimension_values[1] - 1)* dilations[1] + 1);
            slice_width += dilations[1])
        {
          indices.clear();
          indices.push_back(slice_height + height_index);
          indices.push_back(slice_width + width_index);

          uint32_t node_id = current_image_channel.get_value(indices);

          slice_of_image.data_stash.push_back(node_id);
        }
      }


      // Do the max pooling operation, and
      //  if (create a node == true) : make a node and assign to it
      //  else { find the node you should be adding this thing to and do it}

      // if width coordinate can be moved move it.
      // else, if height index, can be moved, then reset width index, and
      // move height index


      starting_node_id = node_index;

      perform_simple_MaxPool(slice_of_image, starting_node_id, ending_node_id, node_id_to_node, CG);

      node_index = ending_node_id;
      data_stash.push_back(node_index);
      end_val = node_index;

      if( output_image_width < (dimension_values[3] - 1) )
      {
        output_image_width++;
        assert((width_index + strides[1] ) <=
               (padded_image.dimension_values[1] - ((kernel_shape[1] - 1)* dilations[1] + 1)) );
        width_index += strides[1];
      }
      else if(output_image_height < (dimension_values[2] - 1) )
      {
        output_image_height++;
        output_image_width = 0;
        assert((height_index + strides[0] ) <=
               (padded_image.dimension_values[0] - ((kernel_shape[0] - 1)* dilations[0] + 1)) );
        width_index = 0.0;
        height_index += strides[0];
      }
      else
      {
        break;
      }

    }

  }

}

template<class T>
void ParameterValues <T> :: perform_simple_MaxPool(ParameterValues < uint32_t > & image_slice,
                                                   uint32_t begin_val, uint32_t & end_val,
                                                   map< uint32_t, node > & node_id_to_node,
                                                   computation_graph & CG)
{

  queue< uint32_t > unprocessed_nodes;
  uint32_t node_index = begin_val;
  pair <uint32_t, uint32_t> pair_ids;
  // Copying all the node id's into the queue
  assert(!image_slice.data_stash.empty());
  assert(!image_slice.dimension_values.empty());
  for(auto index : image_slice.data_stash)
  {
    unprocessed_nodes.push(index);
  }


  while(unprocessed_nodes.size() > 1)
  {

      // Pop 2 nodes from the front
      pair_ids.first = unprocessed_nodes.front();
      unprocessed_nodes.pop();
      pair_ids.second = unprocessed_nodes.front();
      unprocessed_nodes.pop();

      // Compute max and push back the new node
      perform_pairwise_Max(pair_ids, node_index, node_index, node_id_to_node, CG);
      unprocessed_nodes.push(node_index);
  }

  assert(unprocessed_nodes.size() == 1);
  end_val = unprocessed_nodes.front();

}

template<class T>
void ParameterValues <T> :: perform_pairwise_Max  (pair<uint32_t, uint32_t> & index_pair,
                                                   uint32_t begin_val, uint32_t & end_val,
                                                   map< uint32_t, node > & node_id_to_node,
                                                   computation_graph & CG)
{
  uint32_t node_index = begin_val;

  node_index++;
  node node_a(node_index, "relu");
  CG.add_new_node(node_index, node_a);

  CG.connect_node1_to_node2_with_weight(index_pair.first, node_index, 1.0);
  CG.connect_node1_to_node2_with_weight(index_pair.second, node_index, -1.0);
  CG.set_bias_of_node(node_index, 0.0);

  node_index++;
  node node_b(node_index, "none");
  CG.add_new_node(node_index, node_b);

  CG.connect_node1_to_node2_with_weight(index_pair.second, node_index, 1.0);
  CG.connect_node1_to_node2_with_weight(node_a.get_node_number(), node_index, 1.0);
  CG.set_bias_of_node(node_index, 0.0);

  end_val = node_index;
}

template<class T>
void ParameterValues <T> ::perform_BatchNormalization(
                                          ParameterValues <uint32_t> & input_tensor,
                                          double bias, double scale,
                                          uint32_t begin_val, uint32_t & end_val,
                                          map< uint32_t, node> & node_id_to_node, computation_graph & CG)
{
  dimension_values.clear();
  data_stash.clear();

  assert(input_tensor.dimension_values.size() == 2);
  dimension_values = input_tensor.dimension_values;
  uint32_t node_index = begin_val;
  for(int data_index = 0; data_index < input_tensor.data_stash.size(); data_index++ )
  {
    node node_x(node_index, "none");
    CG.add_new_node(node_index, node_x);

    node_id_to_node[node_index] = node_x;
    data_stash.push_back(node_index);
    node_index++;
    end_val = node_index;

    CG.connect_node1_to_node2_with_weight(data_stash[data_index], node_index, scale);
    CG.set_bias_of_node(node_index, bias);

  }

}

template<class T>
ParameterValues<T>& ParameterValues <T> ::operator= (const ParameterValues<T> & p_values)
{
  this->dimension_values = p_values.dimension_values;
  this->data_stash = p_values.data_stash;
  return *this;
}

template<class T>
void ParameterValues <T>::print()
{
  cout << " ------------------------------ " << endl;
  cout << "\t Printing the parameters : " << endl;
  cout << "\t Dimension sizes : [ " ;
  for(auto dim_values : dimension_values )
    cout << dim_values << " , ";
  cout << "]" << endl;

  cout << "\t Content = " ;
  for(auto values : data_stash)
  {
    cout << values << " , ";
  }
  cout << endl;
  cout << " ------------------------------- " << endl;
  cout << endl;
}

template<class T>
ParameterValues<T> ParameterValues <T>::return_flatten(int cut_axis)
{

    assert(!dimension_values.empty());
    assert(!data_stash.empty());

    vector< int > target_dim(2, 0);
    if(cut_axis == 0)
    {
      target_dim[0] = 1;
      target_dim[1] = data_stash.size();
    }
    else
    {
      target_dim.clear();
      int size = 1;
      for(int index = 0; index < cut_axis; index++)
         size *= (dimension_values[index]);
      target_dim.push_back(size);

      size = 1;
      for(int index = cut_axis; index < dimension_values.size(); index++)
         size *= (dimension_values[index]);
      target_dim.push_back(size);
    }
    assert((target_dim[0] * target_dim[1]) == data_stash.size());

    ParameterValues <T> return_value;
    return_value.dimension_values.clear();
    return_value.data_stash.clear();

    return_value.data_stash.resize(data_stash.size(), 0);
    return_value.dimension_values = target_dim;

    vector< int > outer_axis_limits = vector<int>(dimension_values.begin(),
                                                  dimension_values.begin()+cut_axis);

    vector< int > inner_axis_limits = vector<int>(dimension_values.begin()+cut_axis,
                                                  dimension_values.end());

    vector< int > orig_indices, outer_orig_indices, inner_orig_indices;
    vector< int > target_indices;

    for(int data_index = 0; data_index < data_stash.size(); data_index++)
    {
      orig_indices = compute_indices(dimension_values ,data_index);
      outer_orig_indices = vector<int>(orig_indices.begin(), orig_indices.begin() + cut_axis);
      inner_orig_indices = vector<int>(orig_indices.begin() + cut_axis, orig_indices.end());

      int axis_0 = compute_row_major_index(outer_axis_limits, outer_orig_indices);
      int axis_1 = compute_row_major_index(inner_axis_limits, inner_orig_indices);

      target_indices.clear();
      target_indices.push_back(axis_0);
      target_indices.push_back(axis_1);

      int target_index = compute_row_major_index(target_dim, target_indices);
      return_value.data_stash[target_index] = data_stash[data_index];
    }

    return return_value;

}

onnx_parser :: onnx_parser()
{
  input_filename.clear();
  node_count = 0;
}

onnx_parser :: onnx_parser(string filename)
{
  assert(!filename.empty());
  ifstream f(filename.c_str());
  if(!f.good())
  {
    cout << "Data file does not exist, exiting..." << endl;
    exit(0);
  }

  input_filename = filename;
  node_count = 0;
}

void onnx_parser :: build_graph(computation_graph & CG,
                            map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes)
{

  fstream input(input_filename.c_str(), std::ios::in | std::ios::binary);
  string parameter_name;
  uint32_t node_index;

  onnx::ModelProto model_proto;
  bool isSuccess = model_proto.ParseFromIstream(& input);
  if(! isSuccess)
  {
    cout << "Could not read the onnx input file, exiting " << endl;
    exit(0);
  }

  if(!model_proto.has_graph())
  {
    cout << "No network graph found in the onnx file" << endl;
    exit(0);
  }

  // Okay, some kind of graph is present and the reading was successful
  if(sherlock_parameters.verbose_onnx)
  {
    cout << " ---------------------------------------- " << endl << endl;
    cout << "Version of the ONNX format is " << model_proto.ir_version() << endl;
    cout << "Number of operators used  : " << model_proto.opset_import_size() << endl;

    onnx::OperatorSetIdProto opset_id_proto = model_proto.opset_import(0);
    cout << "Opset id proto domain : " << model_proto.domain() << endl;
    cout << "Model version : " << model_proto.model_version() << endl;
  }

  onnx::GraphProto graph_proto = model_proto.graph();

  if(read_graph_proto(graph_proto, tensor_name_to_nodes ,CG))
  {
    if(sherlock_parameters.verbose_onnx)
    {
      cout << "Graph successfully read. " << endl;
    }
  }
  else
  {
    cout << "Error in reading Graph proto .. exiting " << endl;
    exit(0);
  }

  if(sherlock_parameters.verbose_onnx)
    cout << "Indices of the input nodes are : " << endl;

  for(int i = 0; i < graph_proto.input_size() ; i++)
  {
    onnx::ValueInfoProto value_info_proto = graph_proto.input(i);
    parameter_name = value_info_proto.name();

    if(tensor_name_to_nodes.find(parameter_name) == tensor_name_to_nodes.end())
      continue;

    if(sherlock_parameters.verbose_onnx)
    {
      cout << parameter_name << " -- [ ";
    }


    for(auto each_node_index : tensor_name_to_nodes[parameter_name].data_stash)
    {
      if(sherlock_parameters.verbose_onnx)
        cout << each_node_index << " , ";
    }

    if(sherlock_parameters.verbose_onnx)
      cout << " ] " << endl;

  }

  if(sherlock_parameters.verbose_onnx)
  {
    cout << "Number of output groups of the graph = " << graph_proto.output_size() << endl;
    cout << "Indices of the nodes are : " << endl;
  }

  for(int i = 0; i < graph_proto.output_size() ; i++)
  {
    onnx::ValueInfoProto value_info_proto = graph_proto.output(i);
    // cout << "Name : " << value_info_proto.name() << "  " << endl;
    parameter_name = value_info_proto.name();
    onnx::TypeProto type_proto = value_info_proto.type();
    onnx::TypeProto::Tensor tensor_proto = type_proto.tensor_type();
    onnx::TensorShapeProto tensor_shape_proto = tensor_proto.shape();
    assert(tensor_name_to_nodes.find(parameter_name) != tensor_name_to_nodes.end());

    if(sherlock_parameters.verbose_onnx)
    {cout << parameter_name << " -- [ ";}

    for(auto each_node_index : tensor_name_to_nodes[parameter_name].data_stash)
    {
      if(sherlock_parameters.verbose_onnx)
      {cout << each_node_index << " , ";}

      CG.mark_node_as_output(each_node_index);
    }
    if(sherlock_parameters.verbose_onnx)
    {
      cout << " ] " << endl;
      cout << " -------------------------------------- " << endl << endl;
    }

  }

}

bool onnx_parser :: read_graph_proto(onnx::GraphProto & graph_proto,
                           map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                           computation_graph & CG)
{

  // read the parameters and their dimensions
  // The actual values of  the parameters would be put in when you
  // read the corresponding initializers

  map< string, ParameterValues < double > > parameters_map;
  string parameter_name;
  map< uint32_t, node > node_id_to_node;

  // cout << "Name of the graph = " << graph_proto.name() << endl;
  // cout << "Documentation string for the graph = " << graph_proto.doc_string() << endl;
  // cout << "Number of inputs to the graph = " << graph_proto.input_size() << endl;
  // cout << "Inputs to the graph are = " << endl;

  // Reading the initializers and read the data stash from the initializers and
  // put them inside the parameters_name of the that you are building above
  int no_of_initializer_packages = graph_proto.initializer_size();
  int no_of_input_packages = graph_proto.input_size() - no_of_initializer_packages;

  vector< string > initializer_names;
  ParameterValues < double > p_weight_value;
  ParameterValues < uint32_t > p_nodes_value;

  // if(graph_proto.initializer_size() < 1)
  // {
  //   cout << "GraphProto reading failed due to initializer size " << endl;
  //   return false;
  // }

  for(int i = 0; i < graph_proto.initializer_size(); i++)
  {
    onnx::TensorProto tensor_proto;
    tensor_proto = graph_proto.initializer(i);
    parameter_name = tensor_proto.name();
    initializer_names.push_back(tensor_proto.name());
    p_weight_value.clear();
    read_tensor_proto(tensor_proto, p_weight_value);
    parameters_map[parameter_name] = p_weight_value;
  }

  // Reading the input parameter names of the graph and reading in the dimensions
  if(graph_proto.input_size() == 0)
  {
    cout << "GraphProto reading failed due to input size " << endl;
    return false;
  }

  for(int i = 0; i < graph_proto.input_size() ; i++)
  {
    onnx::ValueInfoProto value_info_proto = graph_proto.input(i);
    parameter_name = value_info_proto.name();
    if(find(initializer_names.begin(), initializer_names.end(), parameter_name) == initializer_names.end())
    {
      // For each input of the tensor space declare a node, and get the id
      // by calling the function from the parameter values guy
      onnx::TypeProto type_proto = value_info_proto.type();
      onnx::TypeProto::Tensor tensor_proto = type_proto.tensor_type();
      onnx::TensorShapeProto tensor_shape_proto = tensor_proto.shape();
      declare_nodes_for_shape(tensor_shape_proto, true, node_id_to_node, p_nodes_value, CG);
      tensor_name_to_nodes[parameter_name] = p_nodes_value;
    }

  }


  // Reading the nodes in the graph and creating everything :
  parse_all_nodes(graph_proto, initializer_names, tensor_name_to_nodes,
                  parameters_map, node_id_to_node, CG);

  return true;
}

void onnx_parser::declare_nodes_for_shape(
                                onnx::TensorShapeProto tensor_shape_proto,
                                bool input_nodes,
                                map< uint32_t, node >& node_id_to_node,
                                ParameterValues<uint32_t> & p_value,
                                computation_graph & CG)
{
  p_value.clear();
  int no_of_dimensions = tensor_shape_proto.dim_size();
  int number_of_input_nodes = 1;


  for(int dim_index = 0; dim_index < no_of_dimensions; dim_index ++)
  {
    onnx::TensorShapeProto::Dimension dim_proto = tensor_shape_proto.dim(dim_index);
    number_of_input_nodes *= ((int)dim_proto.dim_value());
    p_value.dimension_values.push_back((int)dim_proto.dim_value());
  }

  for(int input_node_index = 0; input_node_index < number_of_input_nodes; input_node_index ++)
  {
    node_count++;
    node node_x(node_count, "constant");
    CG.add_new_node(node_count, node_x);
    if(input_nodes)
    {
      CG.mark_node_as_input(node_count);
    }
    p_value.data_stash.push_back(node_count);
    node_id_to_node[node_count] = node_x;
    assert(node_count > 0);
  }


}

void onnx_parser::parse_all_nodes(onnx::GraphProto & graph_proto,
                                  vector < string > & initializer_names,
                                  map < string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                                  map < string, ParameterValues < double > > & parameters_map,
                                  map<uint32_t, node > & node_id_to_node,
                                  computation_graph & CG)
{

  /*
  string_to_id_list _input_to_id_map_, _output_to_id_map_;
  string_to_string_list _input_to_name_map_ , _output_to_name_map_;


  // Assign a node id to each node , and create a map between each tensor name
  // and the node id which produces that as input or takes it as an input

  map< uint32_t, onnx::NodeProto > node_id_to_node_proto;

  for(int node_index = 0; node_index < graph_proto.node_size(); node_index++)
  {
    onnx::NodeProto node_proto = graph_proto.node(node_index);

    for(int input_index = 0; input_index < node_proto.input_size(); input_index++)
    {
      string input_name = node_proto.input(input_index);
      if((initializer_names.find(input_name) != initializer_names.end())
         ||
        (graph_input_names.find(input_name) != graph_input_names.end()))
        {
          continue;
        }
        else
        {
          _input_to_id_map_[input_name].push_back(node_index);
        }

    }

    for(int output_index = 0; output_index < node_proto.output_size(); output_index++)
    {
      string output_name = node_proto.output(output_index);
      if(graph_output_names.find(output_name) != graph_output_names.end())
      {
        continue;
      }
      else
      {
        _output_to_id_map_[output_name].push_back(node_index);
      }
    }
  }
  */

  for(int node_index = 0; node_index < graph_proto.node_size(); node_index++)
  {
    onnx::NodeProto current_node = graph_proto.node(node_index);

    // // For each node, find the sherlock node group it takes it's input
    // // from and pass that to read_node_proto
    //
    // vector< onnx:NodeProto > input_nodes;
    //
    read_node_proto(current_node, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
  }

}

bool onnx_parser::read_tensor_proto(onnx::TensorProto & tensor_proto, ParameterValues < double > & p_value)
{
  vector< int > dimension_values;
  vector< double > data_stash;

  dimension_values.clear();
  for(int j = 0; j < tensor_proto.dims_size(); j++)
  {
    dimension_values.push_back(tensor_proto.dims(j));
  }


  string raw_data = tensor_proto.raw_data();
  if(!raw_data.empty())
  {
    if(tensor_proto.data_type() == onnx::TensorProto::FLOAT)
    {
      for(int j = 0; j < (raw_data.size()/4); j++)
      {
        __FLOAT__ var;
        string sample = raw_data.substr(j*4,4);
        var.raw.c_0 = sample.at(0);
        var.raw.c_1 = sample.at(1);
        var.raw.c_2 = sample.at(2);
        var.raw.c_3 = sample.at(3);
        data_stash.push_back(var.f);
      }
    }
    else if(tensor_proto.data_type() == onnx::TensorProto::INT64)
    {
      for(int j = 0; j < raw_data.size()/8; j++)
      {
        string sample = raw_data.substr(j*8,8);
        reverse(sample.begin(), sample.end());
        data_stash.push_back(convert_string(sample));
      }
    }
    else
    {
      cout << "Format is raw data, but the code expects it to be FLOAT, but it's not" << endl;
      cout << "Format = " << tensor_proto.data_type() << endl;
      assert(false);
    }
  }
  else if(tensor_proto.float_data_size() != 0)
  {
    data_stash.clear();
    for(int index = 0; index < tensor_proto.float_data_size(); index++)
    {
      data_stash.push_back((double)tensor_proto.float_data(index));
    }
  }
  else if(tensor_proto.double_data_size() != 0)
  {
    data_stash.clear();
    for(int index = 0; index < tensor_proto.double_data_size(); index++)
    {
      data_stash.push_back(tensor_proto.double_data(index));
    }
  }
  else
  {
    cout << "Error in reading tensor value proto " << endl;
    cout << "Tensor name = " << tensor_proto.name() << endl;
    return false;
  }

  p_value.dimension_values = dimension_values;
  p_value.data_stash = data_stash;

  // Khali haath laute toh bahut bara galti kiye beta
  assert(!data_stash.empty());
  assert(!dimension_values.empty());

}

int onnx_parser::read_node_proto(onnx::NodeProto & node_proto,
                                 map < string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                                 map < string, ParameterValues < double > > & parameters_map,
                                 map < uint32_t, node > & node_id_to_node,
                                 computation_graph & CG)
{
  string operator_name = node_proto.op_type();

  // For each node, try to figure out the name of the tensor for the inputs to the node
  // Next, use the nodes, and parameters map to compute the output of the node.
  // Update the maps : tensor_name_to_node_list, and tensor_name_to_shape

  if(operator_name.compare(Gemm) == 0)
  {
    if(debug_onnx){cout << "Implementing Gemm" << endl;}
    implement_Gemm(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Gemm" << endl;}
    return 1;
  }
  else if(operator_name.compare(Relu) == 0)
  {
    if(debug_onnx){cout << "Implementing Relu" << endl;}
    implement_Relu(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Relu" << endl;}
    return 1;
  }
  else if(operator_name.compare(Conv) == 0)
  {
    if(debug_onnx){cout << "Implementing Conv" << endl;}
    implement_Conv(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Conv" << endl;}
    return 1;
  }
  else if(operator_name.compare(AveragePool) == 0)
  {
    implement_AveragePool(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    return 1;
  }
  else if(operator_name.compare(BatchNormalization) == 0)
  {
    if(debug_onnx){cout << "Implementing Batch Norm" << endl;}
    implement_BatchNormalization(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Batch Norm" << endl;}
    return 1;
  }
  else if(operator_name.compare(MaxPool) == 0)
  {
    if(debug_onnx){cout << "Implementing MaxPool" << endl;}
    implement_MaxPool(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented MaxPool" << endl;}
    return 1;
  }
  else if(operator_name.compare(Constant) == 0)
  {
    if(debug_onnx){cout << "Implementing Constant" << endl;}
    implement_Constant(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Constant" << endl;}
    return 1;
  }
  else if(operator_name.compare(Reshape) == 0)
  {
    if(debug_onnx){cout << "Implementing Reshape" << endl;}
    implement_Reshape(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Reshape" << endl;}
    return 1;
  }
  else if(operator_name.compare(_Transpose_) == 0)
  {
    if(debug_onnx){cout << "Implementing Transpose" << endl;}
    implement_Transpose(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Transpose" << endl;}
    return 1;
  }
  else if(operator_name.compare(MatMul) == 0)
  {
    if(debug_onnx){cout << "Implementing Matmul" << endl;}
    implement_MatMul(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Matmul" << endl;}
    return 1;
  }
  else if(operator_name.compare(Add) == 0)
  {
    if(debug_onnx){cout << "Implementing Add" << endl;}
    implement_Add(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Add" << endl;}
    return 1;
  }
  else if(operator_name.compare(Flatten) == 0)
  {
    if(debug_onnx){cout << "Implementing Flatten" << endl;}
    implement_Flatten(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Flatten" << endl;}
    return 1;
  }
  else if(operator_name.compare(Slice) == 0)
  {
    if(debug_onnx){cout << "Implementing Slice" << endl;}
    implement_Slice(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Slice" << endl;}
    return 1;
  }
  else if(operator_name.compare(Concat) == 0)
  {
    if(debug_onnx){cout << "Implementing Concat" << endl;}
    implement_Concat(node_proto, tensor_name_to_nodes, parameters_map, node_id_to_node, CG);
    if(debug_onnx){cout << "Implemented Concat" << endl;}
    return 1;
  }
  else
  {
    cout << "Unrecognized operator type in reading node proto . Exiting... " << endl;
    cout << "The unrecognised operator name = " << operator_name << endl;
    exit(0);
  }

  return 0;


}


void onnx_parser::implement_Gemm(onnx::NodeProto & node_proto,
                                 map < string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                                 map < string, ParameterValues < double > > & parameters_map,
                                 map<uint32_t, node > & node_id_to_node,
                                 computation_graph & CG)
{
  // The naming of the different operators here, are excatly in line with the documentation
  // in ONNX format description. The webpage that might be useful here is the
  // following : https://github.com/onnx/onnx/blob/master/docs/Operators.md

  // List of attributes we are looking for here and their default values
  float alpha, beta;
  alpha = 1.0; beta = 1.0;
  int transA, transB;
  transA = 0; transB = 0;

  assert(node_proto.output_size() == 1);
  string node_output_name = node_proto.output(0);

  for(int attr_index = 0; attr_index < node_proto.attribute_size(); attr_index++)
  {
    onnx::AttributeProto attribute_proto = node_proto.attribute(attr_index);
    if(attribute_proto.name() == "alpha")
    {
      alpha = attribute_proto.f();
    }
    else if(attribute_proto.name() == "beta")
    {
      beta = attribute_proto.f();
    }
    else if(attribute_proto.name() == "transA")
    {
      transA = attribute_proto.i();
    }
    else if(attribute_proto.name() == "transB")
    {
      transB = attribute_proto.i();
    }

  }

  string _input_A_name = node_proto.input(0);
  string _input_B_name = node_proto.input(1);
  string _input_C_name = node_proto.input(2);

  ParameterValues< double > weight_val, bias_val;
  ParameterValues< uint32_t > nodes_list, result_nodes;
  assert(parameters_map.find(_input_C_name) != parameters_map.end());
  bias_val = parameters_map[_input_C_name];

  if( parameters_map.find(_input_A_name) != parameters_map.end() )
  {
    assert(parameters_map.find(_input_A_name) != parameters_map.end());
    weight_val = parameters_map[_input_A_name];
    assert(!weight_val.dimension_values.empty());
    assert(!weight_val.data_stash.empty());

    assert(tensor_name_to_nodes.find(_input_B_name) != tensor_name_to_nodes.end());
    nodes_list = tensor_name_to_nodes[_input_B_name];
    assert(!nodes_list.dimension_values.empty());
    assert(!nodes_list.data_stash.empty());

    if(transA)
    {
      transpose_matrix < double > (weight_val);
    }
    if(transB)
    {
      transpose_matrix < uint32_t > (nodes_list);
    }


    weight_times_nodes(alpha, weight_val, nodes_list, result_nodes, node_id_to_node, CG);
  }
  else if(parameters_map.find(_input_B_name) != parameters_map.end())
  {

    assert(parameters_map.find(_input_B_name) != parameters_map.end());
    weight_val = parameters_map[_input_B_name];
    assert(!weight_val.dimension_values.empty());
    assert(!weight_val.data_stash.empty());

    assert(tensor_name_to_nodes.find(_input_A_name) != tensor_name_to_nodes.end());
    nodes_list = tensor_name_to_nodes[_input_A_name];
    assert(!nodes_list.dimension_values.empty());
    assert(!nodes_list.data_stash.empty());

    if(transB)
    {
      transpose_matrix < double > (weight_val);
    }
    if(transA)
    {
      transpose_matrix < uint32_t > (nodes_list);
    }

    nodes_times_weight(alpha, nodes_list, weight_val, result_nodes, node_id_to_node, CG);

  }
  else
  {
    cout << "Woah, nothing was found in the list, of tensors and node,"<<
    " something is wrong in the onnx reading " << endl;
    exit(0);
  }

  for(double & each_bias_val : bias_val.data_stash)
  {
    each_bias_val *= beta;
  }

  set_bias(bias_val, result_nodes, CG);
  tensor_name_to_nodes[node_output_name] = result_nodes;


}


template<typename T>
void transpose_matrix(ParameterValues <T> & p_value)
{
  assert(p_value.dimension_values.size() == 2);
  assert(p_value.data_stash.size() > 0);

  vector<int> index(2);
  vector<int> transposed_index(2);
  vector<int> transposed_dim_values(2);
  transposed_dim_values[0] = p_value.dimension_values[1];
  transposed_dim_values[1] = p_value.dimension_values[0];

  T data_val;
  uint32_t dim_index;

  ParameterValues <T> transposed_matrix;
  transposed_matrix.dimension_values = transposed_dim_values;
  transposed_matrix.data_stash.resize(p_value.data_stash.size());

  assert(!transposed_matrix.data_stash.empty());

  for(int row_index = 0; row_index < p_value.dimension_values[0] ; row_index++)
  {
    for(int col_index = 0; col_index < p_value.dimension_values[1] ; col_index++)
    {
      index[0] = row_index;
      index[1] = col_index;
      data_val = p_value.get_value(index);

      transposed_index[0] = col_index;
      transposed_index[1] = row_index;
      dim_index = transposed_matrix.compute_row_major_index(transposed_matrix.dimension_values,
                                                            transposed_index);

      (transposed_matrix.data_stash)[dim_index] = data_val;
    }
  }

  p_value = transposed_matrix;

}

void onnx_parser::weight_times_nodes( double scaling_factor,
                       ParameterValues <double> weight_values,
                       ParameterValues <uint32_t> node_indices,
                       ParameterValues <uint32_t> & result,
                       map<uint32_t, node>& node_id_to_node,
                       computation_graph & CG)
{


  vector<int> shape_of_weight = weight_values.dimension_values;
  vector<int> shape_of_nodes = node_indices.dimension_values;
  vector<int> indices;
  uint32_t node_index;
  double weight_value;

  assert(shape_of_weight.size() == shape_of_nodes.size());
  assert(shape_of_weight[1] == shape_of_nodes[0]);
  vector<int> final_shape(2);
  final_shape[0] = shape_of_weight[0];
  final_shape[1] = shape_of_nodes[1];

  result.dimension_values.clear();
  result.data_stash.clear();
  result.dimension_values = final_shape;

  for(int row_index = 0; row_index < final_shape[0]; row_index ++)
  {
    for(int col_index = 0; col_index < final_shape[1]; col_index++)
    {
      node_count++;

      node node_x(node_count, "none");
      CG.add_new_node(node_count, node_x);
      result.data_stash.push_back(node_count);
      node_id_to_node[node_count] = node_x;

      for(int index = 0; index < shape_of_weight[1]; index++)
      {
        indices.clear();
        indices.push_back(index);
        indices.push_back(col_index);
        node_index = node_indices.get_value(indices);

        indices.clear();
        indices.push_back(row_index);
        indices.push_back(index);
        weight_value = weight_values.get_value(indices);
        weight_value *= scaling_factor;

        CG.connect_node1_to_node2_with_weight(node_index, node_count, weight_value);
      }

    }
  }


}

void onnx_parser::nodes_times_weight( double scaling_factor,
                       ParameterValues <uint32_t> node_indices,
                       ParameterValues <double> weight_values,
                       ParameterValues <uint32_t> & result,
                       map<uint32_t, node>& node_id_to_node,
                       computation_graph & CG)
{
  vector<int> shape_of_weight = weight_values.dimension_values;
  vector<int> shape_of_nodes = node_indices.dimension_values;
  vector<int> indices;
  uint32_t node_index;
  double weight_value;

  assert(shape_of_weight.size() == shape_of_nodes.size());
  assert(shape_of_nodes[1] == shape_of_weight[0]);
  vector<int> final_shape(2);
  final_shape[0] = shape_of_nodes[0];
  final_shape[1] = shape_of_weight[1];

  result.clear();
  result.dimension_values = final_shape;

  for(int row_index = 0; row_index < final_shape[0]; row_index ++)
  {
    for(int col_index = 0; col_index < final_shape[1]; col_index++)
    {
      node_count++;

      node node_x(node_count, "none");
      CG.add_new_node(node_count, node_x);
      result.data_stash.push_back(node_count);
      node_id_to_node[node_count] = node_x;

      for(int index = 0; index < shape_of_nodes[1]; index++)
      {
        indices.clear();
        indices.push_back(row_index);
        indices.push_back(index);
        node_index = node_indices.get_value(indices);

        indices.clear();
        indices.push_back(index);
        indices.push_back(col_index);
        weight_value = weight_values.get_value(indices);
        weight_value *= scaling_factor;

        CG.connect_node1_to_node2_with_weight(node_index, node_count, weight_value);

      }

    }
  }


}


void onnx_parser::set_bias( ParameterValues <double> bias_values,
                       ParameterValues <uint32_t> node_indices,
                       computation_graph & CG)
{


  vector<int> shape_of_bias = bias_values.dimension_values;
  vector<int> shape_of_nodes = node_indices.dimension_values;

  vector<int> indices;
  uint32_t node_index;
  double bias_value;

  // Making a special case for broadcastable biases when one of the dimensions is 1
  if((shape_of_bias.size() == 1) &&
    (node_indices.dimension_values.size() == 2) &&
    ((node_indices.dimension_values[0] * node_indices.dimension_values[1]) == shape_of_bias[0]))
  {
    if(node_indices.dimension_values[0] == 1)
    {
      vector<int> buffer;
      buffer.push_back(1);
      buffer.push_back(shape_of_bias[0]);
      shape_of_bias = buffer;
      bias_values.dimension_values = shape_of_bias;
    }
    else if(node_indices.dimension_values[1] == 1)
    {
      vector<int> buffer;
      buffer.push_back(shape_of_bias[0]);
      buffer.push_back(1);
      shape_of_bias = buffer;
      bias_values.dimension_values = shape_of_bias;
    }
  }


  assert(shape_of_bias.size() == shape_of_nodes.size());
  assert(shape_of_bias.size() == 2);

  assert(shape_of_bias[0] == shape_of_nodes[0]);
  assert(shape_of_bias[1] == shape_of_nodes[1]);

  for(int row_index = 0; row_index < shape_of_bias[0]; row_index ++)
  {
    for(int col_index = 0; col_index < shape_of_bias[1]; col_index++)
    {
      indices.clear();
      indices.push_back(row_index);
      indices.push_back(col_index);
      node_index = node_indices.get_value(indices);

      bias_value = bias_values.get_value(indices);

      CG.set_bias_of_node(node_index, bias_value);
    }
  }


}


void onnx_parser::implement_Relu(onnx::NodeProto & node_proto,
                                 map < string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                                 map < string, ParameterValues < double > > & parameters_map,
                                 map < uint32_t, node > & node_id_to_node,
                                 computation_graph & CG)
{

  assert(node_proto.input_size() == 1);
  string input_name = node_proto.input(0);

  assert(node_proto.output_size() == 1);
  string output_name = node_proto.output(0);


  ParameterValues < uint32_t > result;
  result.dimension_values.clear();
  result.data_stash.clear();

  result.dimension_values = tensor_name_to_nodes[input_name].dimension_values;

  if(debug_onnx)
  {
    cout << " ------------------------ " << endl;
    cout << "When starting Relu " << endl;
    cout << "Name of input tensor : " << input_name << endl;
    cout << "Size of the data : " << tensor_name_to_nodes[input_name].data_stash.size() << endl;
    cout << "Tensor sizes : [ " ;
    for(auto index : tensor_name_to_nodes[input_name].dimension_values)
    {
      cout << index << " , ";
    }
    cout << "]" << endl;
  }

  for(int input_index = 0; input_index < (tensor_name_to_nodes[input_name]).data_stash.size(); input_index++)
  {
    node_count++;
    node node_x(node_count, "relu");
    CG.add_new_node(node_count, node_x);

    uint32_t input_id = (tensor_name_to_nodes[input_name]).data_stash[input_index];
    CG.connect_node1_to_node2_with_weight(input_id, node_count, 1.0);
    CG.set_bias_of_node(node_count, 0.0);

    result.data_stash.push_back(node_count);
    node_id_to_node[node_count] = node_x;
  }

  tensor_name_to_nodes[output_name] = result;

  if(debug_onnx)
  {
    cout << "When finishing Relu " << endl;
    cout << "Name of output tensor : " << output_name << endl;
    cout << "Size of the data : " << tensor_name_to_nodes[output_name].data_stash.size() << endl;
    cout << "Tensor sizes : [ " ;
    for(auto index : tensor_name_to_nodes[output_name].dimension_values)
    {
      cout << index << " , ";
    }
    cout << "]" << endl;
    cout << " ------------------------ " << endl;

  }



}

void onnx_parser::implement_Conv(onnx::NodeProto & node_proto,
                                 map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                                 map< string, ParameterValues < double > > & parameters_map,
                                 map< uint32_t , node> & node_id_to_node,
                                 computation_graph & CG)
{
  // Attributes and their default values
  string autopad("NOTSET");
  vector< int > dilations, kernel_shape, padding_size, strides;
  int group = 1;

  ParameterValues <uint32_t> input_tensor;
  string input_tensor_name;
  ParameterValues <double> kernel_tensor;
  string kernel_tensor_name;
  ParameterValues <double> bias_tensor;
  string bias_tensor_name;
  ParameterValues <uint32_t> output_tensor;
  _shape_ size_post_convolution, size_pre_convolution;
  ParameterValues <double> bias_tensor_for_a_feature;

  // Read the attributes and get the correct parameters that you need to play with
  // assert(node_proto.attribute_size() == 6);
  for(int attr_index = 0; attr_index < node_proto.attribute_size(); attr_index++)
  {
    onnx::AttributeProto attribute_proto = node_proto.attribute(attr_index);
    string name = attribute_proto.name();
    if( name.compare(0, name.size(), "auto_pad") == 0)
    {
      autopad = attribute_proto.s();
    }
    else if(name.compare(0,name.size(),"dilations") == 0)
    {
      dilations.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      {
        dilations.push_back(attribute_proto.ints(index));
      }
    }
    else if(name.compare(0,name.size(),"group") == 0)
    {
      group = attribute_proto.i();
    }
    else if(name.compare(0,name.size(),"kernel_shape") == 0)
    {
      kernel_shape.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      {
        kernel_shape.push_back(attribute_proto.ints(index));
      }
    }
    else if(name.compare(0,name.size(),"pads") == 0)
    {
      padding_size.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      {
        padding_size.push_back(attribute_proto.ints(index));
      }
    }
    else if(name.compare(0,name.size(),"strides") == 0)
    {
      strides.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      {
        strides.push_back(attribute_proto.ints(index));
      }
    }
    else
    {
      cout << "Attribute name not recognised " << endl;
      cout << "Exiting... " << endl;
      exit(0);
    }
  }


  for(int input_index = 0; input_index < node_proto.input_size(); input_index++)
  {
    switch(input_index)
    {
      case 0:
        assert(tensor_name_to_nodes.find(node_proto.input(input_index)) != tensor_name_to_nodes.end());
        input_tensor_name = node_proto.input(input_index);
        input_tensor = tensor_name_to_nodes[input_tensor_name];
        assert(input_tensor.dimension_values.size() == 4);
        assert(input_tensor.dimension_values[0] == 1); // Batch size is 1
      break;
      case 1:
        assert(parameters_map.find(node_proto.input(input_index)) != parameters_map.end());
        kernel_tensor_name = node_proto.input(input_index);
        kernel_tensor = parameters_map[kernel_tensor_name];
        assert(kernel_tensor.dimension_values.size() == 4);
      break;
      case 2:
        assert(parameters_map.find(node_proto.input(input_index)) != parameters_map.end());
        bias_tensor_name = node_proto.input(input_index);
        bias_tensor = parameters_map[bias_tensor_name];
        if(bias_tensor.dimension_values.size() == 0)
        {
          bias_tensor.data_stash.clear();
          bias_tensor.dimension_values.clear();
          bias_tensor.dimension_values.push_back(kernel_tensor.dimension_values[0]);
          fill(bias_tensor.data_stash.begin(), bias_tensor.data_stash.end(), 0.0);
        }
      break;
      default :
      cout << "Error in reading Convolutional layer, does not match the imput type expected." << endl;
      cout << "Input type tried = " << node_proto.input(input_index) << endl;
      exit(0);
      break;
    }
  }


  // Compute the output tensor and add it to the table to tensor values
  output_tensor.dimension_values.clear();
  output_tensor.data_stash.clear();
  output_tensor.dimension_values.resize(input_tensor.dimension_values.size());
  fill(output_tensor.dimension_values.begin(), output_tensor.dimension_values.end(), 0);
  output_tensor.dimension_values[0] = 1; // Batch size is always set to 1
  size_post_convolution.clear();
  size_pre_convolution.clear();
  ParameterValues < uint32_t > convolution_output, convolution_output_for_single_input_channel;
  ParameterValues < uint32_t > image_channel;
  ParameterValues < double > kernel_channel;

  vector<int> partial_index_1(1,0);
  vector<int> partial_index_2(2,0);
  vector<int> partial_index_3(2,0);

  if(debug_onnx)
  {
    cout << "When starting Conv " << endl;
    cout << "Name of input tensor : " << input_tensor_name << endl;
    cout << "Size of the data : " << tensor_name_to_nodes[input_tensor_name].data_stash.size() << endl;
    cout << "Tensor sizes : [ " ;
    for(auto index : tensor_name_to_nodes[input_tensor_name].dimension_values)
    {
      cout << index << " , ";
    }
    cout << "]" << endl;
    cout << " ---------------------------- " << endl;
  }


  for(int output_feature_index = 0; output_feature_index < kernel_tensor.dimension_values[0]; output_feature_index++)
  {

    convolution_output.dimension_values.clear();
    convolution_output.data_stash.clear();

    for(int input_channel_index = 0;
        input_channel_index < input_tensor.dimension_values[1];
        input_channel_index++)
    {
      partial_index_2.clear();
      partial_index_2.push_back(0);
      partial_index_2.push_back(input_channel_index);
      image_channel = input_tensor.get_sub_tensor(partial_index_2);

      partial_index_3.clear();
      partial_index_3.push_back(output_feature_index);
      partial_index_3.push_back(input_channel_index);
      kernel_channel = kernel_tensor.get_sub_tensor(partial_index_3);


      convolution_output.perform_convolution(image_channel, kernel_channel,
                                             autopad, dilations, group, kernel_shape, padding_size, strides,
                                             node_count, node_count, node_id_to_node, CG);

    }

    // Add the bias value for this particular feature to all the
    // elements of the image computed for this feature map
    bias_tensor_for_a_feature.dimension_values = convolution_output.dimension_values;
    bias_tensor_for_a_feature.data_stash.resize(convolution_output.data_stash.size());
    fill(bias_tensor_for_a_feature.data_stash.begin(),
         bias_tensor_for_a_feature.data_stash.end(), bias_tensor.data_stash[output_feature_index]);

    set_bias( bias_tensor_for_a_feature, convolution_output, CG);

    // Push back the result into the output tensor
    output_tensor.push_sub_tensor(1, convolution_output);

    size_post_convolution = output_tensor.dimension_values;
    assert(size_post_convolution.size() == 4);
    if(! size_pre_convolution.empty()) // Checking the consistency of the convolution operation
    {
      assert(size_pre_convolution[0] == size_post_convolution[0]);
      assert( (size_pre_convolution[1] + 1) == size_post_convolution[1]);
      assert(size_pre_convolution[2] == size_post_convolution[2]);
      assert(size_pre_convolution[3] == size_post_convolution[3]);
    }
    size_pre_convolution = size_post_convolution;
  }

  assert(node_proto.output_size() == 1);
  tensor_name_to_nodes[node_proto.output(0)] = output_tensor;

  if(debug_onnx)
  {
    cout << "When finishing Conv " << endl;
    cout << "Name of input tensor : " << node_proto.output(0) << endl;
    cout << "Size of the data : " << tensor_name_to_nodes[node_proto.output(0)].data_stash.size() << endl;
    cout << "Tensor sizes : [ " ;
    for(auto index : tensor_name_to_nodes[node_proto.output(0)].dimension_values)
    {
      cout << index << " , ";
    }
    cout << "]" << endl;
    cout << " ------------------------ " << endl;
  }

}

void onnx_parser::implement_AveragePool(onnx::NodeProto & node_proto,
                                       map< string, ParameterValues <uint32_t> > & tensor_name_to_nodes,
                                       map< string, ParameterValues < double > > & parameters_map,
                                       map< uint32_t , node> & node_id_to_node,
                                       computation_graph & CG)
{
  string autopad;
  int ceil_mode = 0;
  int count_include_pad = 0;
  vector< int > kernel_shape, padding_size, strides;

  ParameterValues <uint32_t> input_tensor;
  string input_tensor_name;

  ParameterValues <uint32_t> output_tensor;
  _shape_ size_post_convolution, size_pre_convolution;


  for(int attr_index = 0; attr_index < node_proto.attribute_size(); attr_index++)
  {
    onnx::AttributeProto attribute_proto = node_proto.attribute(attr_index);
    string name = attribute_proto.name();

    if(name.compare(0,name.size(), "auto_pad") == 0)
    {
      autopad = attribute_proto.s();
      if(autopad != "NOTSET")
      {
        cout << "Autopad being used, it's a deprecated feature, exiting " << endl;
        exit(0);
      }
    }
    else if(name.compare(0,name.size(), "ceil_mode") == 0)
    {
      ceil_mode = attribute_proto.i();
    }
    else if(name.compare(0,name.size(), "count_include_pad") == 0)
    {
      count_include_pad = attribute_proto.i();
    }
    else if(name.compare(0,name.size(), "kernel_shape") == 0)
    {
      kernel_shape.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      {
        kernel_shape.push_back(attribute_proto.ints(index));
      }
    }
    else if(name.compare(0,name.size(), "pads") == 0)
    {
      padding_size.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      {
        padding_size.push_back(attribute_proto.ints(index));
      }
    }
    else if(name.compare(0,name.size(), "strides") == 0)
    {
      strides.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      {
        strides.push_back(attribute_proto.ints(index));
      }
    }
    else
    {
      cout << "Attribute name not recognised " << endl;
      cout << "Exiting... " << endl;
      exit(0);
    }

  }

  assert(node_proto.input_size() == 1);
  uint32_t input_index = 0;
  assert(tensor_name_to_nodes.find(node_proto.input(input_index)) != tensor_name_to_nodes.end());
  input_tensor_name = node_proto.input(input_index);
  input_tensor = tensor_name_to_nodes[input_tensor_name];
  assert(input_tensor.dimension_values.size() == 4);
  assert(input_tensor.dimension_values[0] == 1); // Batch size is 1


  // Compute the output tensor and add it to the table to tensor values
  output_tensor.dimension_values.clear();
  output_tensor.data_stash.clear();
  output_tensor.dimension_values.resize(input_tensor.dimension_values.size());
  fill(output_tensor.dimension_values.begin(), output_tensor.dimension_values.end(), 0);
  output_tensor.dimension_values[0] = 1; // Batch size is always set to 1

  output_tensor.perform_AveragePool(input_tensor, ceil_mode, count_include_pad, kernel_shape,
                                    padding_size, strides, node_count, node_count, node_id_to_node, CG);


  assert(node_proto.output_size() == 1);
  tensor_name_to_nodes[node_proto.output(0)] = output_tensor;
}

void onnx_parser::implement_BatchNormalization(onnx::NodeProto & node_proto,
                                                 map< string, ParameterValues<uint32_t> > & tensor_name_to_nodes,
                                                 map< string , ParameterValues <double> > & parameters_map,
                                                 map< uint32_t, node > & node_id_to_node,
                                                 computation_graph & CG)
{
  // ATTRIBUTES ARE BEING TOTALLY IGNORED HERE
  // Since, these are important for training purpose, and not for evaluation of the graph

  ParameterValues< uint32_t > input_tensor;
  string input_tensor_name;
  ParameterValues< double > scale_tensor;
  string scale_factor_name;
  ParameterValues< double > bias_tensor;
  string bias_tensor_name;
  ParameterValues< uint32_t > output_tensor;
  string output_tensor_name;

  // Only taking the first 3 inputs, and ignoring the rest
  input_tensor_name = node_proto.input(0);
  input_tensor = tensor_name_to_nodes[input_tensor_name];
  assert(input_tensor.dimension_values.size() == 4);
  assert(input_tensor.dimension_values[0] == 1);

  string scale_tensor_name = node_proto.input(1);
  assert(parameters_map.find(scale_tensor_name) != parameters_map.end());
  scale_tensor = parameters_map[scale_tensor_name];
  // Channel count matches the number of scale factors present
  assert(scale_tensor.dimension_values[0] == input_tensor.dimension_values[1]);


  bias_tensor_name = node_proto.input(2);
  assert(parameters_map.find(bias_tensor_name) != parameters_map.end());
  scale_tensor = parameters_map[bias_tensor_name];
  // Channel count matches the number of bias factors present
  assert(bias_tensor.dimension_values[0] == input_tensor.dimension_values[1]);

  double scale, bias;
  vector<int> indices;
  ParameterValues< uint32_t > image_channel;
  ParameterValues< uint32_t > transformed_image;

  for(int channel_index = 0; channel_index < input_tensor.dimension_values[1]; channel_index++)
  {
    indices.clear();
    indices.push_back(channel_index);
    image_channel = input_tensor.get_sub_tensor(indices);

    scale = scale_tensor.data_stash[channel_index];
    bias = bias_tensor.data_stash[channel_index];

    transformed_image.clear();
    transformed_image.perform_BatchNormalization(image_channel, bias, scale,
                                                 node_count, node_count, node_id_to_node, CG);

    output_tensor.push_sub_tensor(1, transformed_image);
  }

  tensor_name_to_nodes[node_proto.output(0)] = output_tensor;


}

void onnx_parser::implement_MaxPool(onnx :: NodeProto & node_proto,
                              map< string, ParameterValues <uint32_t > > & tensor_name_to_nodes,
                              map< string, ParameterValues <double > > & parameters_map,
                              map< uint32_t, node > & node_id_to_node,
                              computation_graph & CG)
{

  string autopad;
  int ceil_mode = 0;
  int storage_order = 0;
  vector< int > kernel_shape, padding_size, strides, dilations;

  ParameterValues <uint32_t> input_tensor;
  string input_tensor_name;

  ParameterValues <uint32_t> output_tensor;
  _shape_ size_post_convolution, size_pre_convolution;

  // Reading all the attributes for this operation

  for(int attr_index = 0; attr_index < node_proto.attribute_size(); attr_index++)
  {
    onnx::AttributeProto attribute_proto = node_proto.attribute(attr_index);
    string name = attribute_proto.name();

    if(name.compare(0,name.size(), "auto_pad") == 0)
    {
      autopad = attribute_proto.s();
      if(autopad != "NOTSET")
      {
        cout << "Autopad being used, it's a deprecated feature, exiting " << endl;
        exit(0);
      }
    }
    else if(name.compare(0,name.size(), "ceil_mode") == 0)
    {
      ceil_mode = attribute_proto.i();
    }
    else if(name.compare(0,name.size(), "storage_order") == 0)
    {
      storage_order = attribute_proto.i();
      assert(storage_order == 0);
    }
    else if(name.compare(0,name.size(), "kernel_shape") == 0)
    {
      kernel_shape.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      {
        kernel_shape.push_back(attribute_proto.ints(index));
      }

      //Only works for 2 dimensional kernels
      assert(kernel_shape.size() == 2);

    }
    else if(name.compare(0,name.size(), "pads") == 0)
    {
      padding_size.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      {
        padding_size.push_back(attribute_proto.ints(index));
      }
    }
    else if(name.compare(0,name.size(), "strides") == 0)
    {
      strides.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      {
        strides.push_back(attribute_proto.ints(index));
      }
    }
    else if(name.compare(0,name.size(),"dilations") == 0)
    {
      dilations.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      {
        dilations.push_back(attribute_proto.ints(index));
      }
    }
    else
    {
      cout << "Attribute name not recognised " << endl;
      cout << "Exiting... " << endl;
      exit(0);
    }

  }


  assert(node_proto.input_size() == 1);
  uint32_t input_index = 0;
  assert(tensor_name_to_nodes.find(node_proto.input(input_index)) != tensor_name_to_nodes.end());
  input_tensor_name = node_proto.input(input_index);
  input_tensor = tensor_name_to_nodes[input_tensor_name];
  assert(input_tensor.dimension_values.size() == 4);
  assert(input_tensor.dimension_values[0] == 1); // Batch size is 1

  if(debug_onnx)
  {
    cout << " ------------------------ " << endl;
    cout << "When starting Maxpool " << endl;
    cout << "Name of input tensor : " << input_tensor_name << endl;
    cout << "Size of the data : " << tensor_name_to_nodes[input_tensor_name].data_stash.size() << endl;
    cout << "Tensor sizes : [ " ;
    for(auto index : tensor_name_to_nodes[input_tensor_name].dimension_values)
    {
      cout << index << " , ";
    }
    cout << "]" << endl;
  }


  // Compute the output tensor and add it to the table to tensor values
  output_tensor.dimension_values.clear();
  output_tensor.data_stash.clear();
  output_tensor.dimension_values.resize(input_tensor.dimension_values.size());
  fill(output_tensor.dimension_values.begin(), output_tensor.dimension_values.end(), 0);
  output_tensor.dimension_values[0] = 1; // Batch size is always set to 1


  output_tensor.perform_MaxPool(input_tensor, ceil_mode, storage_order, kernel_shape,
                                padding_size, strides, dilations, node_count, node_count,
                                node_id_to_node, CG);

  assert(node_proto.output_size() == 1);
  tensor_name_to_nodes[node_proto.output(0)] = output_tensor;


  if(debug_onnx)
  {
    cout << "When finishing Maxpool " << endl;
    cout << "Name of output tensor : " << node_proto.output(0) << endl;
    cout << "Size of the data : " << tensor_name_to_nodes[node_proto.output(0)].data_stash.size() << endl;
    cout << "Tensor sizes : [ " ;
    for(auto index : tensor_name_to_nodes[node_proto.output(0)].dimension_values)
    {
      cout << index << " , ";
    }
    cout << "]" << endl;
    cout << " ------------------------ " << endl;
  }

}

void onnx_parser::implement_Constant(onnx:: NodeProto & node_proto,
                                     map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                                     map< string, ParameterValues < double > > & parameters_map,
                                     map< uint32_t , node> & node_id_to_node,
                                     computation_graph & CG)

{
  for(int attr_index = 0; attr_index < node_proto.attribute_size(); attr_index++)
  {
    onnx::AttributeProto attribute_proto = node_proto.attribute(attr_index);
    string name = attribute_proto.name();
    assert(name.compare(0, name.size(), "value") == 0);
    string tensor_name = node_proto.output(0);

    onnx::TensorProto T = attribute_proto.t();
    ParameterValues <double> tensor;
    read_tensor_proto(T, tensor);
    // tensor.print();
    parameters_map[tensor_name] = tensor;

  }
}

void onnx_parser::implement_Reshape(onnx:: NodeProto & node_proto,
                             map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                             map< string, ParameterValues < double > > & parameters_map,
                             map< uint32_t , node> & node_id_to_node,
                             computation_graph & CG)
{
  assert(node_proto.input_size() == 2);
  assert(node_proto.output_size() == 1);

  string input_variable_name = node_proto.input(0);
  string reshape_var = node_proto.input(1);

  string output_variable_name = node_proto.output(0);

  if(tensor_name_to_nodes.find(input_variable_name) != tensor_name_to_nodes.end())
  {
    ParameterValues < uint32_t > reshaped_nodes;
    reshaped_nodes.data_stash = tensor_name_to_nodes[input_variable_name].data_stash;
    reshaped_nodes.dimension_values = tensor_name_to_nodes[input_variable_name].dimension_values;
    reshape_vector(parameters_map[reshape_var].data_stash, reshaped_nodes.dimension_values);
    tensor_name_to_nodes[output_variable_name] = reshaped_nodes;
  }
  else if(parameters_map.find(input_variable_name) != parameters_map.end())
  {
    ParameterValues < double > reshaped_nodes;
    reshaped_nodes.data_stash = parameters_map[input_variable_name].data_stash;
    reshaped_nodes.dimension_values = parameters_map[input_variable_name].dimension_values;
    reshape_vector(parameters_map[reshape_var].data_stash, reshaped_nodes.dimension_values);
    parameters_map[output_variable_name] = reshaped_nodes;
  }
  else
  {
    cout << "Tensor Value being asked to reshape is in neither of the lists. " << endl;
    cout << "Name being searched for here : " << input_variable_name << endl;
    assert(false);
  }

}

void onnx_parser::implement_Transpose(onnx::NodeProto & node_proto,
                        map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                        map< string, ParameterValues < double > > & parameters_map,
                        map< uint32_t, node > & node_id_to_node,
                        computation_graph & CG)
{

  bool is_a_node_type;
  uint32_t input_index = 0;

  ParameterValues < uint32_t > input_tensor_nodes;
  ParameterValues < double > input_tensor_double;
  string input_tensor_name = node_proto.input(input_index);

  assert(node_proto.output_size() == 1);
  string output_tensor_name = node_proto.output(0);

  assert(node_proto.input_size() == 1);
  if(tensor_name_to_nodes.find(node_proto.input(input_index)) != tensor_name_to_nodes.end())
    is_a_node_type = true;
  else if(parameters_map.find(node_proto.input(input_index)) != parameters_map.end())
    is_a_node_type = false;
  else
  {
    cout << "Error in reading the Transpose node, input node name not found !" << endl;
    cout << "Input node name - " << node_proto.input(input_index) << " exiting.. " << endl;
    exit(0);
  }


  if(is_a_node_type)
  {
    input_tensor_nodes = tensor_name_to_nodes[input_tensor_name];
    // Making sure it's a 2 dimensional thing
    assert(input_tensor_nodes.dimension_values.size() == 2);
    ParameterValues < uint32_t > output_tensor;
    output_tensor.data_stash = input_tensor_nodes.data_stash;
    output_tensor.dimension_values =
    input_tensor_nodes.dimension_values;
    transpose_matrix< uint32_t > (output_tensor);
    tensor_name_to_nodes[output_tensor_name] = output_tensor;
  }
  else
  {
    input_tensor_double = parameters_map[input_tensor_name];
    // Making sure it's a 2 dimensional thing
    assert(input_tensor_double.dimension_values.size() == 2);
    ParameterValues < double > output_tensor;
    output_tensor.data_stash = input_tensor_double.data_stash;
    output_tensor.dimension_values =
    input_tensor_double.dimension_values;
    transpose_matrix< double > (output_tensor);
    parameters_map[output_tensor_name] = output_tensor;
  }

  return;

}


void onnx_parser::implement_MatMul(onnx::NodeProto & node_proto,
                      map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                      map< string, ParameterValues < double > > & parameters_map,
                      map< uint32_t, node > & node_id_to_node,
                      computation_graph & CG)
{
  // Aims to Y = A * B
  assert(node_proto.input_size() == 2);
  assert(node_proto.output_size() == 1);

  bool A_node_type = true;
  bool B_node_type = true;

  string _input_A_name_ = node_proto.input(0);
  string _input_B_name_ = node_proto.input(1);
  string _output_Y_name_ = node_proto.output(0);

  A_node_type = (tensor_name_to_nodes.find(_input_A_name_) != tensor_name_to_nodes.end())?
                true:false;

  B_node_type = (tensor_name_to_nodes.find(_input_B_name_) != tensor_name_to_nodes.end())?
                true:false;

  int strip_result_index = -1;

  if(A_node_type && !B_node_type)
  {

    ParameterValues < uint32_t > variable_A  = tensor_name_to_nodes[_input_A_name_];
    ParameterValues < double > variable_B = parameters_map[_input_B_name_];

    if((variable_A.dimension_values.size() == 1) &&
      (variable_B.dimension_values.size() == 2))
    {
      vector<int> new_dim;
      new_dim.push_back(1);
      new_dim.push_back(variable_A.dimension_values[0]);
      variable_A.dimension_values = new_dim;
      strip_result_index = 0;
    }
    else if((variable_B.dimension_values.size() == 1) &&
           (variable_A.dimension_values.size() == 2))
    {
      vector<int> new_dim;
      new_dim.push_back(variable_B.dimension_values[0]);
      new_dim.push_back(1);
      variable_B.dimension_values = new_dim;
      strip_result_index = 1;
    }
    else
    {
      cout <<"Sizes of variables A and B, are not what is expected " << endl;
    }

    // Make sure these are really just matrices you are Multiplying
    assert(variable_A.dimension_values.size() == variable_B.dimension_values.size());

    ParameterValues < uint32_t> result_nodes;
    nodes_times_weight(1.0, variable_A, variable_B, result_nodes, node_id_to_node, CG);

    // Set the biases to 0
    for(uint32_t index : result_nodes.data_stash)
      CG.set_bias_of_node(index, 0.0);

    if(strip_result_index >= 0)
    {
      result_nodes.dimension_values.erase(
        result_nodes.dimension_values.begin() + strip_result_index);
    }
    tensor_name_to_nodes[_output_Y_name_] = result_nodes;

  }
  else if(!A_node_type && B_node_type)
  {

    ParameterValues < double > variable_A  = parameters_map[_input_A_name_];
    ParameterValues < uint32_t > variable_B = tensor_name_to_nodes[_input_B_name_];

    if((variable_A.dimension_values.size() == 1) &&
      (variable_B.dimension_values.size() == 2))
    {
      vector<int> new_dim;
      new_dim.push_back(1);
      new_dim.push_back(variable_A.dimension_values[0]);
      variable_A.dimension_values = new_dim;
    }
    else if((variable_B.dimension_values.size() == 1) &&
           (variable_A.dimension_values.size() == 2))
    {
      vector<int> new_dim;
      new_dim.push_back(variable_B.dimension_values[0]);
      new_dim.push_back(1);
      variable_B.dimension_values = new_dim;
    }

    // Make sure these are really just matrices you are Multiplying
    assert(variable_A.dimension_values.size() == variable_B.dimension_values.size());

    ParameterValues < uint32_t> result_nodes;
    weight_times_nodes(1.0, variable_A, variable_B, result_nodes, node_id_to_node, CG);

    // Set the biases to 0
    for(uint32_t index : result_nodes.data_stash)
      CG.set_bias_of_node(index, 0.0);

    if(strip_result_index >= 0)
    {
      result_nodes.dimension_values.erase(
        result_nodes.dimension_values.begin() + strip_result_index);
    }
    tensor_name_to_nodes[_output_Y_name_] = result_nodes;
  }
  else if(A_node_type && B_node_type)
  {
    cout << "Both A and B are of type node, that cannot be done. " << endl;
    cout << "Error in type MatMul" << endl;
    cout << "Exiting... " << endl;
    exit(0);
  }
  else
  {
    cout <<"A name - " << _input_A_name_ << " B name - " << _input_B_name_ << endl;
    cout << "Both A and B are of type double hasn't implemented yet." << endl;
    cout << "Error in type MatMul" << endl;
    cout << "Exiting... " << endl;
    exit(0);
  }

}

void onnx_parser::implement_Add(onnx::NodeProto & node_proto,
                   map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                   map< string, ParameterValues < double > > & parameters_map,
                   map< uint32_t, node > & node_id_to_node,
                   computation_graph & CG)
{

  // Aims to C = A + B
  assert(node_proto.input_size() == 2);
  assert(node_proto.output_size() == 1);

  bool A_node_type = true;
  bool B_node_type = true;

  string _input_A_name_ = node_proto.input(0);
  string _input_B_name_ = node_proto.input(1);
  string _output_Y_name_ = node_proto.output(0);

  A_node_type = (tensor_name_to_nodes.find(_input_A_name_) != tensor_name_to_nodes.end())?
                true:false;

  B_node_type = (tensor_name_to_nodes.find(_input_B_name_) != tensor_name_to_nodes.end())?
                true:false;


  if(A_node_type && B_node_type)
  {
    ParameterValues < uint32_t > input_A = tensor_name_to_nodes[_input_A_name_];
    ParameterValues < uint32_t > input_B = tensor_name_to_nodes[_input_B_name_];

    ParameterValues < uint32_t > result_nodes;
    result_nodes.dimension_values.clear();
    result_nodes.data_stash.clear();

    for(int dim_index = 0; dim_index < input_A.dimension_values.size(); dim_index++)
      assert(input_A.dimension_values[dim_index] == input_B.dimension_values[dim_index]);

    result_nodes.dimension_values = input_A.dimension_values;
    for(int index = 0; index < input_A.data_stash.size(); index++)
    {
      node_count++;
      node node_x(node_count, "none");
      CG.add_new_node(node_count, node_x);
      result_nodes.data_stash.push_back(node_count);
      node_id_to_node[node_count] = node_x;

      CG.connect_node1_to_node2_with_weight(input_A.data_stash[index], node_count, 1.0);
      CG.connect_node1_to_node2_with_weight(input_B.data_stash[index], node_count, 1.0);
      CG.set_bias_of_node(node_count, 0.0);
    }
    tensor_name_to_nodes[_output_Y_name_] = result_nodes;
  }
  else if(!A_node_type && B_node_type)
  {
    ParameterValues < double > input_A = parameters_map[_input_A_name_];
    ParameterValues < uint32_t > input_B = tensor_name_to_nodes[_input_B_name_];

    ParameterValues < uint32_t > result_nodes;
    result_nodes.dimension_values.clear();
    result_nodes.data_stash.clear();

    for(int dim_index = 0; dim_index < input_A.dimension_values.size(); dim_index++)
      assert(input_A.dimension_values[dim_index] == input_B.dimension_values[dim_index]);

    result_nodes.dimension_values = input_A.dimension_values;
    for(int index = 0; index < input_B.data_stash.size(); index++)
    {
      node_count++;
      node node_x(node_count, "none");
      node_x.set_bias(input_A.data_stash[index]);

      CG.add_new_node(node_count, node_x);
      result_nodes.data_stash.push_back(node_count);
      node_id_to_node[node_count] = node_x;

      CG.connect_node1_to_node2_with_weight(input_B.data_stash[index], node_count, 1.0);
    }
    tensor_name_to_nodes[_output_Y_name_] = result_nodes;

  }
  else if(A_node_type && !B_node_type)
  {
    ParameterValues < uint32_t > input_A = tensor_name_to_nodes[_input_A_name_];
    ParameterValues < double > input_B = parameters_map[_input_B_name_];

    ParameterValues < uint32_t > result_nodes;
    result_nodes.dimension_values.clear();
    result_nodes.data_stash.clear();

    assert(input_A.data_stash.size() == input_B.data_stash.size());

    for(int dim_index = 0; dim_index < input_A.dimension_values.size(); dim_index++)
      if(dim_index < input_B.dimension_values.size())
        assert(input_A.dimension_values[dim_index] == input_B.dimension_values[dim_index]);

    result_nodes.dimension_values = input_A.dimension_values;
    for(int index = 0; index < input_B.data_stash.size(); index++)
    {
      node_count++;
      node node_x(node_count, "none");
      node_x.set_bias(input_B.data_stash[index]);

      CG.add_new_node(node_count, node_x);
      result_nodes.data_stash.push_back(node_count);
      node_id_to_node[node_count] = node_x;

      CG.connect_node1_to_node2_with_weight(input_A.data_stash[index], node_count, 1.0);
    }
    tensor_name_to_nodes[_output_Y_name_] = result_nodes;
  }
  else
  {
    ParameterValues < double > input_A = parameters_map[_input_A_name_];
    ParameterValues < double > input_B = parameters_map[_input_B_name_];

    ParameterValues < double > result_nodes;
    result_nodes.dimension_values.clear();
    result_nodes.data_stash.clear();

    for(int dim_index = 0; dim_index < input_A.dimension_values.size(); dim_index++)
      assert(input_A.dimension_values[dim_index] == input_B.dimension_values[dim_index]);

    for(int index = 0; index < input_B.data_stash.size(); index++)
    {
      result_nodes.data_stash.push_back(
        (input_A.data_stash[index] + input_B.data_stash[index]));
    }

    parameters_map[_output_Y_name_] = result_nodes;
  }

}

void onnx_parser::implement_Flatten(onnx::NodeProto & node_proto,
                    map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                    map< string, ParameterValues < double > > & parameters_map,
                    map< uint32_t, node > & node_id_to_node,
                    computation_graph & CG)
{

  assert(node_proto.attribute_size() == 1);
  assert(node_proto.input_size() == 1);
  assert(node_proto.output_size() == 1);

  onnx::AttributeProto attribute_proto = node_proto.attribute(0);
  assert(attribute_proto.name() == "axis");
  int cut_axis = attribute_proto.i();

  string input_name = node_proto.input(0);
  string output_name = node_proto.output(0);

  if(tensor_name_to_nodes.find(input_name) != tensor_name_to_nodes.end())
  {
    ParameterValues< uint32_t > input_tensor = tensor_name_to_nodes[input_name];
    ParameterValues< uint32_t > output_tensor = input_tensor.return_flatten(cut_axis);
    tensor_name_to_nodes[output_name] = output_tensor;
  }
  else if(parameters_map.find(input_name) != parameters_map.end())
  {
    ParameterValues< double > input_tensor = parameters_map[input_name];
    ParameterValues< double > output_tensor = input_tensor.return_flatten(cut_axis);
    parameters_map[output_name] = output_tensor;
  }
  else
  {
    cout << "Tensor being searched for in FLATTEN not found ! " << endl;
    cout << "Tensor name - " << input_name << " .. Exiting.. " << endl;
    assert(false);
  }
}

void onnx_parser::implement_Slice(onnx::NodeProto & node_proto,
                    map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                    map< string, ParameterValues < double > > & parameters_map,
                    map< uint32_t, node > & node_id_to_node,
                    computation_graph & CG)
{

  string name, output_name;

  assert(node_proto.attribute_size() == 3);
  assert(node_proto.input_size() == 1);
  assert(node_proto.output_size() == 1);

  name = node_proto.input(0);
  assert(tensor_name_to_nodes.find(name) != tensor_name_to_nodes.end());
  ParameterValues< uint32_t > data = tensor_name_to_nodes[name];



  vector< int > axes_values, ends, starts_values;

  for(int attr_index = 0; attr_index < node_proto.attribute_size(); attr_index++)
  {
    onnx::AttributeProto attribute_proto = node_proto.attribute(attr_index);
    name = attribute_proto.name();
    if(name.compare(0,name.size(), "axes") == 0)
    {
      axes_values.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      axes_values.push_back(attribute_proto.ints(index));
    }
    else if(name.compare(0,name.size(), "ends") == 0)
    {
      ends.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      ends.push_back(attribute_proto.ints(index));
    }
    else if(name.compare(0,name.size(), "starts") == 0)
    {
      starts_values.clear();
      for(int index = 0; index < attribute_proto.ints_size(); index++)
      starts_values.push_back(attribute_proto.ints(index));
    }
    else
    {
      cout << "Unrecognised attribute in implementing Slice, exiting..." << endl;
      exit(0);
    }
  }

  if(axes_values.empty())
  {
    for(int index = 0; index < starts_values.size(); index ++)
      axes_values.push_back(index);
  }


  assert(axes_values.size() == ends.size());
  assert(axes_values.size() == starts_values.size());

  ParameterValues < uint32_t > output_tensor;
  output_tensor.data_stash.clear();
  output_tensor.dimension_values.clear();
  vector< int > index;
  int element_count;
  index = starts_values;

  for( int axes_index = axes_values.size()-1; axes_index >= 0;
           axes_index--)
  {
    uint32_t current_dim = axes_values[axes_index];
    uint32_t start_val = starts_values[axes_index];
    uint32_t end_val = ends[axes_index];
    element_count = 0;
    while(index[current_dim] < end_val)
    {
      output_tensor.data_stash.push_back(data.get_value(index));
      index[current_dim]++;
      element_count++;
    }
    output_tensor.dimension_values.insert(output_tensor.dimension_values.begin(), element_count);
  }


  output_name = node_proto.output(0);
  tensor_name_to_nodes[output_name] = output_tensor;

}

void onnx_parser::implement_Concat(onnx::NodeProto & node_proto,
                    map< string, ParameterValues < uint32_t > > & tensor_name_to_nodes,
                    map< string, ParameterValues < double > > & parameters_map,
                    map< uint32_t, node > & node_id_to_node,
                    computation_graph & CG)
{
  string output_name, name;
  vector< string > input_tensor_names;
  int axis_index;

  assert(node_proto.attribute_size() == 1);
  assert(node_proto.output_size() == 1);

  output_name = node_proto.output(0);

  for(int input_index = 0; input_index < node_proto.input_size(); input_index++)
  {
    name = node_proto.input(input_index);
    input_tensor_names.push_back(name);
    assert(tensor_name_to_nodes.find(name) != tensor_name_to_nodes.end());
  }
  assert(input_tensor_names.size() > 1);

  onnx::AttributeProto attribute_proto = node_proto.attribute(0);
  name = attribute_proto.name();
  if(name.compare(0,name.size(), "axis") == 0)
  {
    axis_index = attribute_proto.i();
    if(axis_index < 0)
    {
      int limit = tensor_name_to_nodes[input_tensor_names[0]].dimension_values.size()-1;
      axis_index =  limit + axis_index;
    }
  }

  ParameterValues< uint32_t > output_tensor;
  output_tensor.clear();
  output_tensor = tensor_name_to_nodes[input_tensor_names[0]];

  for(int input_index = 1; input_index < input_tensor_names.size(); input_index++)
  {
    ParameterValues< uint32_t > new_tensor = tensor_name_to_nodes[input_tensor_names[input_index]];

    // Concatenate new tensor to output tensor
    ParameterValues < uint32_t > result_tensor;
    result_tensor.dimension_values = output_tensor.dimension_values;

    result_tensor.dimension_values[axis_index] = output_tensor.dimension_values[axis_index]
      + new_tensor.dimension_values[axis_index];


    uint32_t result_size = 1;
    for(auto dim : result_tensor.dimension_values)
      result_size *= dim;
    result_tensor.data_stash.resize(result_size);

    vector< int > indices;

    for(int data_index = 0; data_index < result_tensor.data_stash.size(); data_index++)
    {

      indices = result_tensor.compute_indices(result_tensor.dimension_values, data_index);

      if(indices[axis_index] < output_tensor.dimension_values[axis_index])
      // Already within the limits of the previous tensor
      {
        uint32_t val = output_tensor.get_value(indices);
        result_tensor.data_stash[data_index] = val;
      }
      else
      // Should be taken up from the new tensor
      {
        vector< int > adjusted_indices = indices;
        adjusted_indices[axis_index] -= output_tensor.dimension_values[axis_index];
        uint32_t val = new_tensor.get_value(adjusted_indices);
        result_tensor.data_stash[data_index] = val;
      }
    }
    output_tensor = result_tensor;
  }
  tensor_name_to_nodes[output_name] = output_tensor;

  // cout << "Output tensor name after concat - " << output_name << endl;
}

void reshape_vector(vector< double > reshape_, vector< int > & result)
{
  assert(!reshape_.empty());
  assert(!result.empty());

  vector<int> reshape;
  for(auto var : reshape_)
  {
    reshape.push_back((int)var);
  }

  vector< int > initial_shape = result;
  if(reshape.size() == result.size())
  {
    // Copy from the source for the 0 cases
     for(int i = 0; i < reshape.size(); i++)
     {
       if(reshape[i] == 0)
        result[i] = reshape[i];
     }
     int miss_dim = -1;
    // For final values as -1, detect the missing dimension
    for(int i = 0; i < reshape.size(); i++)
    {
      if(reshape[i] == -1)
      {
        miss_dim = i;
        break;
      }
    }
    int size_of_reshape = 1;
    int size_of_orig = 1;
    for(int i = 0; i < reshape.size(); i++)
    {
      size_of_orig *= result[i];
      size_of_reshape *= ((reshape[i] != (-1))?(reshape[i]):(1));
    }
     reshape[miss_dim] = (int) size_of_orig/size_of_reshape;
     result = reshape;
     return;
  }
  else
  {
    // if reshape has some -1 , than infer that dimension value
    int miss_dim = -1;
    for(int i = 0; i < reshape.size(); i++)
    {
      if(reshape[i] == -1)
      {
        miss_dim = i;
        break;
      }
    }
    int size_of_reshape = 1;
    for(int i = 0; i < reshape.size(); i++)
    {
      size_of_reshape *=
      ( ( reshape[i] != (-1)) ? (reshape[i]) : (1) );
    }

    int size_of_orig = 1;
    for(int i = 0; i < result.size(); i++)
    {
      size_of_orig *= result[i];
    }

    reshape[miss_dim] = (int) (size_of_orig / size_of_reshape);
    result = reshape;
    return;
  }



}

void make_padded_image_from(ParameterValues< uint32_t > & image_input,
                            string autopad, vector<int> padding_size,
                            ParameterValues< int > & padded_image)
{

  if(autopad != "NOTSET")
  {
    cout << "Auto pad not implemented yet in Sherlock" << endl;
    cout << "Exiting... " << endl;
    exit(0);
  }

  assert(image_input.dimension_values.size() == 2);
  assert((image_input.dimension_values.size() * 2) == padding_size.size());


  padded_image.dimension_values.clear();
  padded_image.data_stash.clear();

  padded_image.dimension_values = image_input.dimension_values;
  padded_image.dimension_values[0] += padding_size[0] + padding_size[1];
  padded_image.dimension_values[1] += padding_size[2] + padding_size[3];

  uint32_t original_image_height, original_image_width;

  for(int height_index = 0; height_index < padded_image.dimension_values[0]; height_index++)
  {
    for(int width_index = 0; width_index < padded_image.dimension_values[1]; width_index++)
    {
      if( ((height_index > (padding_size[0] - 1)) &&
          (height_index < (padded_image.dimension_values[0] - padding_size[1])))
         &&
         ((width_index > (padding_size[2] - 1)) &&
           (width_index < (padded_image.dimension_values[0] - padding_size[3])))
         )
      {
        original_image_height = height_index - padding_size[0];
        original_image_width = width_index - padding_size[2];
        vector<int> indices;
        indices.push_back(original_image_height);
        indices.push_back(original_image_width);

        int node_index = image_input.get_value(indices);
        padded_image.data_stash.push_back(node_index);
      }
      else
      {
        padded_image.data_stash.push_back(-1.0);
      }
    }
  }



}


int64_t convert_string(string input_string)
{
  assert(input_string.size() == 8);
  bool negative_number = false;
  bool first_time = true;
  int64_t ret_number = 0;
  int position_index = 0;

  // This thing here starts reading stuff starting from the MSB
  char ch = input_string.at(0);
  negative_number = ((ch & (1 << 7)) ? true : false);

  string local_repr("");
  for(int i = 0; i < 8 ; i++)
  {
    char c = input_string.at(i);
    for (int k = 7; k >= 0; --k)
    {
        if(c & (1 << k))
        {
          if(negative_number)
            local_repr.push_back('0');
          else
            local_repr.push_back('1');
        }
        else
        {
          if(negative_number)
            local_repr.push_back('1');
          else
            local_repr.push_back('0');
        }
    }
  }

  bitset<64> magnitude_bit(local_repr);
  int64_t magnitude_int = magnitude_bit.to_ullong();
  int64_t final_number;
  if(negative_number)
  {
    final_number = -(magnitude_int + (int64_t)1);
  }
  else
  {
    final_number = magnitude_int;
  }

  return final_number;
}

template class ParameterValues <uint32_t>;
template class ParameterValues <double>;
// THINGS YET TO DO :
// Make input and output Sherlock nodes out of the graph
