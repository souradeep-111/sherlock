//
// Created by Mac User on 1/24/18.
//
#include <gurobi_c++.h>
#include "RangeToVariables.h"
#include <sstream>
#include <tuple>

#define INTER_BOUND_TOL 1E-07

namespace NeuralRuleAnalysis {
    extern bool debug_milp;

    void RangeToVariables::add_variables_and_constraints(GRBModel &mdl, std::vector<GRBVar> const &variables) {
        double cur_bound = _lb;
        int count = 0;
        GRBLinExpr e;

        for (double next_bound: _cuts) {
            if (next_bound - cur_bound >= INTER_BOUND_TOL) {
                /* add a new variable to represent the gap between
                 * cur_bound - INTER_BOUND_TOL and next_bound + INTER_BOUND_TOL */
                std::stringstream ss;
                ss << "w_" << _dim << "_" << count;
                count++;

                GRBVar wij = mdl.addVar(0.0, 1.0, 0.0, GRB_BINARY, ss.str());
                e = e + GRBLinExpr(wij);
                if (debug_milp) {
                    std::cout << "Created binary variable: " << ss.str() << " as indicator var.  dimension:"
                              << _dim << " Range:" << cur_bound << "," << next_bound << std::endl;
                }
                // Now add inequalities
                GRBVar xi = variables[_dim];
                // Now add constraint
                // xi <= wij * next_bound  + (1 - wij) * ub
                mdl.addConstr(xi <= wij * (next_bound + INTER_BOUND_TOL) + (1.0 - wij) * _ub);
                if (debug_milp) {
                    std::cout << "adding constraint: xi <=  " << ss.str() << " * "
                              << next_bound + INTER_BOUND_TOL << " + (1 - " << ss.str() << ") *"
                              << _ub << std::endl;
                }
                // xi >= wij * cur_bound + (1 - wij) * lb
                mdl.addConstr(xi >= wij * (cur_bound - INTER_BOUND_TOL) + (1.0 - wij) * _lb);
                if (debug_milp) {
                    std::cout << "adding constraint: xi >=  " << ss.str() << " * "
                              << cur_bound - INTER_BOUND_TOL << " + (1 - " << ss.str() << ") *"
                              << _lb << std::endl;
                }
                _vars.push_back(std::make_tuple(cur_bound, next_bound, wij, ss.str()));
                cur_bound = next_bound;
            }
            // mdl.addConstr(e == 1); //exactly one of the indicators can be active at all times.

        }
        mdl.addConstr(e == 1); //exactly one of the indicators can be active at all times.


    }

    void
    RangeToVariables::get_all_variables_in_range(double a, double b, std::vector<GRBVar> &res) const {
        double l, u;
        GRBVar var;
        std::string s;
        if (debug_milp){
            std::cout << " Getting all variables for dimension " << _dim << " in range: " << a << "," << b << std::endl;
        }

        for (std::tuple<double, double, GRBVar, std::string> const &tups: _vars) {
            std::tie(l, u, var,s) = tups;
            if (l >= a && u <= b) {
                res.push_back(var);
                if (debug_milp) {
                    std::cout << " \t  +" << s << std::endl;
                }
            }
        }
        assert(res.size() > 0);
    }
}
