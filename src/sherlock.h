#ifndef sherlock_h
#define sherlock_h

#include < iostream >
#include < vector >
#include < queue >
#include

class sherlock
{
private:
  computation_graph neural_network;
  constraints_stack network_constraints;
public:
  sherlock();
  sherlock(computation_graph & CG);
  void optimize_node(uint32_t node_index, double & optima_achieved);
  void compute_output_range(region_constraints & input_region, pair< double, double >& output_range );
  void compute_output_region(region_constraints & input_region, region_constraints & output_region);

  void perform_gradient_search(uint32_t node_index, bool direction, region_constraints & region, double & val);
  void perform_gradient_search_with_random_restarts(uint32_t node_index, bool direction,
                                              region_constraints & region, double & val);
};

#endif
