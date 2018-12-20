#ifndef nodes_h
#define nodes_h

// Here is a list of capabilities I want from the objects here :
// There are two types of nodes, constant nodes, and computation nodes
// This is for the computation type nodes :
// internally it stores the input, output, and gradient
//  whenever it's queried it should return the outputs, and/or gradient when required
// and has pointers to the previous nodes it receives it's inputs from, and
// another set of forward pointers, Not required, but just in case it's needed.


#endif
