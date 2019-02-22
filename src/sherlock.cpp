#include "sherlock.h"

using namespace std;

sherlock :: sherlock()
{
  // Pretty much does nothing
}

sherlock :: sherlock(computation_graph & CG)
{
  neural_network = CG;
}

void sherlock :: optimize_node(uint32_t node_index, double & optima_achived)
{
  uint32_t node_index;
}

void sherlock :: compute_output_range(region_constraints & input_region, pair < double, double >& output_range )
{

}

void sherlock :: compute_output_region(region_constraints & input_region, region_constraints & output_region)
{

}
