//
// Created by Mac User on 1/11/18.
//
#include <cassert>
#include <ostream>
#include "Box.h"
namespace NeuralRuleAnalysis {

    Box::Box(std::vector<int> const &bvars) : _box_vars(bvars) {
        unsigned long n = _box_vars.size();
        for(unsigned long i = 0; i < n; ++i){
            MpfiWrapper tmp(0.0);
            bounds.push_back(tmp);
        }
    }

    Box::Box(Box const & b): _box_vars(b._box_vars){
        // Deep copy the bounds
        unsigned long n = _box_vars.size();
        for(unsigned long i = 0; i < n; ++i){
            MpfiWrapper tmp(b.bounds[i]);
            bounds.push_back(tmp);
        }
    }

    void Box::set_dimensions(int inp_var, MpfiWrapper const & what){
        int i = input_var_to_dimension(inp_var);
        bounds[i].set(what);
        return;
    }

    MpfiWrapper & Box::get_bounds_for_input_var(int inp_var){
        int i = input_var_to_dimension(inp_var);
        return bounds[i];
    }

    MpfiWrapper const & Box::get_bounds_for_input_var(int inp_var) const{
        int i = input_var_to_dimension(inp_var);
        return bounds[i];
    }

    std::map<int, double> Box::get_center() const{
        int n = (int) _box_vars.size();
        std::map<int, double> ret_map;
        for (int i = 0; i < n; ++i){
            int var_id = dimension_to_input_var(i);
            ret_map[var_id] = median(bounds[i]);
        }
        return ret_map;
    };

    int Box::input_var_to_dimension(int inp_var) const {
        unsigned long nvars = _box_vars.size();
        for (unsigned long i =0 ; i < nvars; ++i){
            if (_box_vars[i] == inp_var)
                return i;
        }
        // Not found
        assert(false);
        return -1;
    }

    int Box::dimension_to_input_var(int dim) const {
        unsigned long nvars = _box_vars.size();
        assert(dim >= 0);
        assert(dim < nvars);
        return _box_vars[dim];
    }

    ostream& operator<< (ostream & out, Box const & b){
        unsigned long nvars = b._box_vars.size();
        for (unsigned long i = 0; i < nvars; ++i){
            out << "x_" << b._box_vars[i] << ": " << b.bounds[i]<<std::endl;
        }
        return out;
    }



};
