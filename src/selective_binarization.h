#ifndef _selective_binarization_h
#define _selective_binarization_h

#include "generate_constraints.h"

using namespace std;

class selective_binarization : public constraints_stack
{
private:
  set < uint32_t > binarize;
  set < uint32_t > dont_touch;
public:
  selective_binarization();

  void set_binary_variables_excluding(set<uint32_t>& do_not_touch);

  bool optimize_binarization(
                 uint32_t node_index, bool direction,
                 map< uint32_t, double >& neuron_value,
                 map< uint32_t, double >& binary_values,
                 double & result);

  bool perform_binarization(uint32_t node_index, double bound,
                                      bool direction, region_constraints & input_region,
                                      computation_graph & neural_network,
                                      set< uint32_t > & binarized_neurons);

  void add_variables_to_binarize(map< uint32_t, double > current_assignment,
                                 map< uint32_t, double > current_boolean_state,
                                 set < uint32_t >& binarize_variables );

  void use_layered_binarization_strategy(computation_graph & neural_network,
                                         map< uint32_t, double > current_boolean_state,
                                         set< uint32_t > & binarize_variables);

};

#endif
