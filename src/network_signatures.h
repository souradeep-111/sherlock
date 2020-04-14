#ifndef network_signatures_h
#define network_signatures_h

#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include "computation_graph.h"
#include "region_constraints.h"
#include <set>
#include <assert.h>
#include <time.h>

typedef map<uint32_t, bool> bit_vector;

class network_signatures
{
private:
  map< uint32_t , bit_vector > signatures;
public:
  network_signatures();
  void create_signature_for_graph( computation_graph & CG,
                                   region_constraints & input_region,
                                   uint32_t no_of_samples);

  vector< uint32_t > return_sample_indices();
  void return_bit_vector_for_sample(uint32_t index, bit_vector & b_vec);
  void clear();
  bool empty();
  void learn_constant_neurons(set < uint32_t > & on_list,
                              set < uint32_t > & off_list);

  void learn_pairwise_relationship(uint32_t trial_count,
                                   set< pair< uint32_t, uint32_t > >& same_sense_nodes,
                                   set< pair< uint32_t, uint32_t > >& opposite_sense_nodes);

  void learn_implies_relationship(uint32_t trial_count,
                                  set< pair< uint32_t, uint32_t > >& node_1_implies_node_2_true_sense,
                                  set< pair< uint32_t, uint32_t > >& node_1_implies_node_2_false_sense);

  void learn_constant_neurons_within_set(set<uint32_t> super_set,
                                         set<uint32_t> & on_list,
                                         set<uint32_t> & off_list);

};
uint32_t generate_random_int(uint32_t range);
uint32_t generate_random_int(uint32_t range, uint32_t seed);
uint32_t generate_random_int_from_set(set< uint32_t >& input_set, uint32_t seed);
#endif
