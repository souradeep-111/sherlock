#include "image_handler.h"

using namespace std;

void read_image_as_point(
  string name,
  ParameterValues < uint32_t > & input_indices,
  map< uint32_t, double >& image_point
)
{

  assert(!input_indices.data_stash.empty());

  ifstream file;
  file.open(name.c_str());
  image_point.clear();


  unsigned int count, i;
  double buffer;

  file >> buffer;
  count = (unsigned int) buffer;
  assert(count == input_indices.data_stash.size());

  i = 0;
  while(i < count)
  {
    file >> buffer;
    image_point[input_indices.data_stash[i]] = buffer;
    i++;
  }

}
