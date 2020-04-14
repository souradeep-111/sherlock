#ifndef polyhedral_abstraction_h
#define polyhedral_abstraction_h

class polyhedral_abstraction
{
public:
  void optimize();
  void find_maximal_points();
  void find_intersection_points();
  void find_direction();
  void compute_polyhedron();
  void propagate_polyhedron();
};



// Function to optimize
// 1. Receive the neural network,
// 2. Get the direction of optimization
// 3. Add a node to the network with each of the directions
// 4. Get the point of maximization
// 5. Return the actual bounds as a vector


// Function to find the contact points and the current directions
// 1. Receive the current set of directions
// 2. Receive the neural network
// 3. Maximize the neural network output for each of the directions
// 4. Return the contact points for each of them returned by the above function

// Function to find the intersection point
// 1. Receive the current set of directions
// 2. Try to find an intersection among the individual directions
// 3. If it exists and is within the current region , return true, and the point
// 4. Else return false, and clear the container

// Function to learn the direction ---
// 1. Receive the good points as the contacts points
// 2. Receive the intersection points
// 3. Formulate the optimization problem
// 4. Get the result and return in a direction sense

// Function to compute the output polyhedron
// 1. Receive, sliced neural network
// 2. Incrementally add new directions which would bring improvements
// 3. Try this for a bunch of times and bound that.

// Function to propagate polyhedron
// 1. Receive the neural network to start with
// 2. Receive the indices for the input neurons
// 3. Receive the input polyhedron
// 4. For each of set of reachable neurons it computes a reachable polyhedron
// 5. Finally returns the outset range for the output neuron



#endif
