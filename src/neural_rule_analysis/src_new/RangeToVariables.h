//
// Created by Mac User on 1/24/18.
//

#ifndef NEURAL_RULE_ANALYSIS_RANGETOVARIABLES_H
#define NEURAL_RULE_ANALYSIS_RANGETOVARIABLES_H
#include <set>
#include <vector>
#include <gurobi_c++.h>
#include <cassert>

namespace NeuralRuleAnalysis {
    class RangeToVariables {
    protected:
        int _dim;
        double _lb;
        double _ub;
        std::set<double> _cuts;
        std::vector< std::tuple< double, double, GRBVar, std::string> >  _vars;
    public:
        RangeToVariables(int id, double lb, double ub, std::set<double> const & what ):_dim(id),
                                                                                       _lb(lb),
                                                                                       _ub(ub),
                                                                                       _cuts(what){};

        RangeToVariables(RangeToVariables const & what) = default;

        void add_variables_and_constraints(GRBModel & mdl, std::vector<GRBVar> const & variables);

        void get_all_variables_in_range( double a, double b, std::vector<GRBVar> & res) const;

    };
};

#endif //NEURAL_RULE_ANALYSIS_RANGETOVARIABLES_H
