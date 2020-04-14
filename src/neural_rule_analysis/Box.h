//
// Created by Mac User on 1/11/18.
//

#ifndef NEURAL_RULE_ANALYSIS_BOX_H
#define NEURAL_RULE_ANALYSIS_BOX_H

/* Class Box:
 * This is the basic class for storing and manipulating boxes
 * over a set of variables.
 */
#include <map>
#include <vector>
#include "mpfiWrapper.h"


namespace NeuralRuleAnalysis {
    class Box {
    private:
        std::vector<int> _box_vars; // Map from indices 0, 1..., _n_vars-1 to the actual input variables.
        std::vector<MpfiWrapper> bounds;

        int input_var_to_dimension(int inp_var) const; // Translate from input variable id to the corresponding id in the box.
        int dimension_to_input_var(int dim) const;
    public:
        // Initialize a box with variables in bvars and ranges include [0,0] along each dimension.
        Box(std::vector<int> const & bvars);
        // Copy constructor
        Box(Box const & what);
        // Set the bounds for a given variable.
        // Note: inp_var must belong to the vector _box_vars
        void set_dimensions(int inp_var, MpfiWrapper const & what);
        // Get the bounds for a given variable
        MpfiWrapper & get_bounds_for_input_var(int inp_var);
        MpfiWrapper const &  get_bounds_for_input_var(int inp_var) const;
        std::map<int, double> get_center() const;
        friend ostream& operator<< (ostream & out, Box const & b);
    };
};


#endif //NEURAL_RULE_ANALYSIS_BOX_H
