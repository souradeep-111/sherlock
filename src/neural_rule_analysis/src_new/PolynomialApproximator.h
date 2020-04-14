//
// Created by Mac User on 1/17/18.
//

#ifndef NEURAL_RULE_ANALYSIS_POLYNOMIALAPPROXIMATOR_H
#define NEURAL_RULE_ANALYSIS_POLYNOMIALAPPROXIMATOR_H
#include <vector>
#include <map>
#include <iostream>
#include <gurobi_c++.h>

#include "mpfiWrapper.h"
#include "Box.h"
#include "Tiling.h"
#include "Polynomial.h"
#include "RangeToVariables.h"



namespace NeuralRuleAnalysis {
    struct LinearPiece{
        Box b;
        std::map<int, double> linear_expr; // Map system variables to the coefficients
        double const_term; // The constant term
        MpfiWrapper tol_intvl;

        LinearPiece(Box const & bi, std::map<int, double> const & li, double c, MpfiWrapper tol):b(bi), linear_expr(li),
                                                                                                 const_term(c), tol_intvl(tol)
        {};



    };

    class PolynomialApproximator {
    protected:
        Polynomial _p; // The polynomial of interest
        Tiling _til; // Maintain the tiling information
        std::vector<int> _sys_vars; // A list of system variables corresponding to the dimensions of the tiles.
        std::map<int, int> _dims_to_sys_vars;
        std::vector<LinearPiece> _lin_pieces; // The current linear pieces
        double _tol; // The desired error tolerance.
        bool is_linear;
        MpfiWrapper evaluate_linearization_against_current_tile( Tile const & current_tile,
                                                                 std::map<int, double> const & lin_expr,
                                                                 double coeff) const ;

        void improve_current_tile( Tile & current_tile, std::map<int, double> & lin, double coeff) const;

    public:

        PolynomialApproximator(Polynomial const & p, double tol,
                               std::vector<double> const & lower,
                               std::vector<double> const & upper,
                               std::vector<int> const & num_subdivs);

        PolynomialApproximator(PolynomialApproximator const & p ) = default;

        std::vector<LinearPiece> const & get_linear_pieces() const { return _lin_pieces; };
        std::vector<LinearPiece> & get_linear_pieces() { return _lin_pieces; };
        void set_tolerance(double t){ _tol = t; };

        int get_system_variable_for_dimension(int j) const{
            return _sys_vars[j];
        }
        int get_dimension_for_system_variable(int var_id) const{
            auto it = _dims_to_sys_vars.find(var_id);
            if (it != _dims_to_sys_vars.end())
                return it -> second;
            return (-1); // signal that the dimension cannot be found, after all.
        }
        void compute_polynomial_approximation();
        void add_linear_piece(Box const & box, std::map<int, double> const & lin, double coeff, MpfiWrapper tol);
        void pretty_print_linearization() const;
        int num_linear_pieces() const {
            return (int) _lin_pieces.size();
        }

        Polynomial const & get_polynomial() const { return _p; }

        void collect_cutpoints(std::vector< std::set<double> > & dimensional_cut_points) const;

        GRBVar encode_in_gurobi_model(std::vector<RangeToVariables> const &  all_ranges, GRBModel  & model,
                                    std::vector<GRBVar> const & input_variables, int pa_id) const;

        void set_linear(bool what){ is_linear = what; };
    };
};


#endif //NEURAL_RULE_ANALYSIS_POLYNOMIALAPPROXIMATOR_H
