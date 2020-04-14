#ifndef image_handler_h
#define image_handler_h

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <map>
#include <fstream>
#include "parsing_onnx.h"

using namespace std;

void read_image_as_point(
  string name,
  ParameterValues < uint32_t > & input_indices,
  map< uint32_t, double >& image_point
);

#endif
