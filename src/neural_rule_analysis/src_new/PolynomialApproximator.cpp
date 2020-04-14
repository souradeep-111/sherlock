//
// Created by Mac User on 1/17/18.
//
#include <cassert>
#include <sstream>
#include "PolynomialApproximator.h"

namespace NeuralRuleAnalysis{
    bool debug = false;
    bool use_affine_arithmetic = true;
    bool debug_milp = false;

    PolynomialApproximator::PolynomialApproximator(Polynomial const & p,
                                                   double tol,
                                                   std::vector<double> const & lower,
                                                   std::vector<double> const & upper,
                                                   std::vector<int> const & num_subdivs):_p(p), _til(p.get_used_vars(),
                                                                                              lower,
                                                                                              upper,
                                                                                              num_subdivs),
                                                                                         _sys_vars(p.get_used_vars()),
                                                                                         _tol(tol),
                                                                                         is_linear(false)
    {
      auto num_vars = (int) _sys_vars.size();
      for (int i = 0; i < num_vars; ++i){
        _dims_to_sys_vars[ _sys_vars[i] ] = i;
      }
    };

    MpfiWrapper
    PolynomialApproximator::evaluate_linearization_against_current_tile(Tile const & current_tile,
                                                                        std::map<int, double> const & lin_expr,
                                                                        double coeff) const {
        Polynomial new_poly(_p);
        Box current_box = _til.tile_to_box(current_tile);
        new_poly.subtract_linear_expr(lin_expr);
        MpfiWrapper intvl = use_affine_arithmetic?
                            new_poly.interval_evaluation_using_affine_arithmetic(current_box):
                            new_poly.interval_evaluation(current_box);
        intvl = intvl - coeff;
        return intvl;
    }

    void PolynomialApproximator::improve_current_tile( Tile & current_tile,
                                                              std::map<int, double> & lin,
                                                              double coeff) const {
        std::deque<int> extensions_to_try;
        for(int i= 1; i < 1+_til.get_num_vars(); ++i){
            extensions_to_try.push_back(i); // Move right along dimension i-1
            extensions_to_try.push_back(-i); // Move left along dimension i-1

        }

        while (!extensions_to_try.empty()){
            int cur_ext = extensions_to_try.front();
            extensions_to_try.pop_front();
            int dim; bool left;
            if (cur_ext < 0){
                dim = -cur_ext -1;
                left = true;
            } else {
                dim = cur_ext -1;
                left = false;
            }
            bool reinsert = false;
            int cur_lb = current_tile.get_lower(dim);
            int cur_ub = current_tile.get_upper(dim);
            // Move left along cur_ext
            Tile new_tile_0(current_tile);
            bool new_tile_set = false;
            if (left && cur_lb > 0) {
                new_tile_0.set_lower(dim, cur_lb - 1);
                new_tile_set = true;
            }

            if (!left && cur_ub < _til.get_num_subdivs_for_dim(dim)) {
                new_tile_0.set_upper(dim, cur_ub + 1);
                new_tile_set = true;
            }

            if (new_tile_set){
                if (_til.tile_has_no_intersections(new_tile_0)){
                    // Now compute error for this new tile
                    MpfiWrapper intvl = evaluate_linearization_against_current_tile(new_tile_0, lin, coeff);
                    if (intvl.lower()  > -_tol && intvl.upper() < _tol){
                        current_tile = new_tile_0;
                        reinsert=true;
                    }
                }
            }

            if (reinsert){
                extensions_to_try.push_back(cur_ext);
            }
        }

    }

    void PolynomialApproximator::compute_polynomial_approximation()  {
        // Algorithm:
        // While there is an empty tile,
        //    1: get that empty tile.
        //    2: convert the tile to a box.
        //    3: linearize around tile center coordinates.
        //    4: compute the overall linearization error over the box.
        //    5: If error is within bounds, expand the current tile and repeat 2-4
        //    6: Insert the current tile and its linearization + associated error bounds.

        // DEBUG
        if (debug){
            std::cout << "<Polynomial>" << std::endl;
            std::cout << _p << std::endl;
            std::cout << "</Polynomial>"<< std::endl;
        }

        auto tile_result = _til.find_empty_tile();
        bool found_tile = tile_result.first;
        while (found_tile){
            Tile current_tile = tile_result.second;
            // DEBUG
            if (debug){
                std::cout << "<CurrentTile>" << std::endl;
                std::cout << current_tile << std::endl;
                std::cout << "</CurrenTile>"<< std::endl;
            }

            Box current_box = _til.tile_to_box(current_tile);
            // DEBUG
            if (debug){
                std::cout << "<CurrentBox>" << std::endl;
                std::cout << current_box << std::endl;
                std::cout << "</CurrenBox>"<< std::endl;
            }
            std::map<int, double> center_pt = current_box.get_center();
            // DEBUG
            if (debug){
                std::cout << "<LinearizationPoint>" << std::endl;
                for (auto kv: center_pt){
                    std::cout << "\t" << "x_"<<kv.first << ":" << kv.second << std::endl;
                }
                std::cout << "</LinearizationPoint>" << std::endl;
            }

            auto lin_res = _p.linearize_around_point(center_pt);
            std::map<int, double> lin = lin_res.first;
            double coeff = lin_res.second;
            // DEBUG
            if (debug){
                std::cout << "<LinearExpr>" << std::endl;
                std::cout << "\t" << coeff ;
                for (auto kv: lin){
                    std::cout << "+ (" << kv.second << "* x_"<<kv.first << ")";
                }
                std::cout << std::endl;
                std::cout << "</LinearExpr>" << std::endl;
            }

            MpfiWrapper intvl = evaluate_linearization_against_current_tile(current_tile, lin, coeff);
            if (debug){
                std::cout << "<IntervalError>" << std::endl;
                std::cout << "\t" << intvl << std::endl;
                std::cout << "</IntervalError>" << std::endl;
            }


            if (intvl.lower() > -_tol && intvl.upper() < _tol){
                // Now we have to keep extending current tile and evaluate
                // DEBUG
                if (debug){
                    std::cout << "<ExtendingBox/>" << std::endl;
                }
                improve_current_tile(current_tile, lin, coeff);

                current_box = _til.tile_to_box(current_tile);
                intvl = evaluate_linearization_against_current_tile(current_tile, lin, coeff);
                // DEBUG
                if (debug){
                    std::cout << "<FinishedExtendingBox/>" << std::endl;
                    std::cout << "<FinalBox>" << std::endl;
                    std::cout << current_box << std::endl;
                    std::cout << "</FinalBox>"<< std::endl;
                    std::cout << "<LinearizationError>" << intvl << "</LinError>" << std::endl;
                }
            }
            // DEBUG
            if (debug){
                std::cout << "<AddingLinearPiece/>" << std::endl;
            }

            add_linear_piece(current_box, lin, coeff, intvl);
            _til.insert_tile(current_tile);
            tile_result = _til.find_empty_tile();
            found_tile = tile_result.first;
        }

    }

    void PolynomialApproximator::add_linear_piece(Box const & box, std::map<int, double> const & lin, double coeff, MpfiWrapper tol) {
        LinearPiece li(box, lin, coeff, tol);
        _lin_pieces.push_back(li);
    }

    void print_piece(LinearPiece const & l){
        std::cout << "<Box>" << std::endl;
        std::cout << l.b << std::endl;
        std::cout << "</Box>" << std::endl;
        std::cout << "<LinearExpr>" << std::endl;
        std::cout << "\t" << (l.tol_intvl+ l.const_term);
        for (auto kv: l.linear_expr){
            std::cout << "+ (" << kv.second << "* x_"<<kv.first << ")";
        }
        std::cout << std::endl;
        std::cout << "</LinearExpr>" << std::endl;
    }

    void PolynomialApproximator::pretty_print_linearization() const{
        int count = 1;
        for(auto const & l: _lin_pieces){
            std::cout << "Linear Piece #" << count << std::endl;
            print_piece(l);
            count ++;

        }
    }

    void PolynomialApproximator::collect_cutpoints(std::vector<std::set<double> > & dimensional_cut_points) const {
        for(auto const & l: _lin_pieces){
            Box const & b = l.b;
            for (int dims: _sys_vars){
                MpfiWrapper w = b.get_bounds_for_input_var(dims);
                dimensional_cut_points[dims].insert(w.lower());
                dimensional_cut_points[dims].insert(w.upper());
            }
        }

    }

    GRBVar PolynomialApproximator::encode_in_gurobi_model(std::vector<RangeToVariables> const &all_ranges,
                                                        GRBModel & model,
                                                        std::vector<GRBVar> const &input_variables,
                                                        int pa_id) const {

        stringstream ss0;
        ss0 << "z_"<<pa_id;
        GRBVar output_var = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS, ss0.str());


        if (is_linear){
            assert(_lin_pieces.size()== 1);
            LinearPiece const & l0 = (*_lin_pieces.begin());
            GRBLinExpr lExpr0;
            for (auto kv: l0.linear_expr) {
                lExpr0 += kv.second * input_variables[kv.first];
            }
            model.addConstr( output_var <= lExpr0 + l0.tol_intvl.upper() + l0.const_term);
            model.addConstr( output_var >= lExpr0 + l0.tol_intvl.lower() + l0.const_term);
            return output_var;
        }
        //   3.1 for each linear piece do
        //       3.1.1 Create a binary variable for each piece.
        //       3.1.2 Link the binary variable for each piece with the variables for the individual dimension
        //       3.1.3 Set the output variable constraint as a function of the inputs for the piece
        int count = 0;
        std::vector<GRBVar> allVars;
        GRBLinExpr sum_of_indicators;

        if (debug_milp){
            std::cout << " Polynomial Approximator #  " << pa_id << std::endl;
            std::cout << "Creating variable" << ss0.str() << std::endl;

        }

        for (LinearPiece const & l: _lin_pieces){
            Box const & b = l.b;
            std::stringstream ss;
            ss << "l_" << pa_id << "_" << count ;

            if (debug_milp){
                std::cout << " Considering encoding for piece # " << count << std::endl;
                print_piece(l);
                std::cout << "Creating binary variable for piece number # " << count << std::endl;
                std::cout << "Var: " << ss.str() << std::endl;
            }
            count++;
            GRBVar lij = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, ss.str());
            allVars.push_back(lij);
            sum_of_indicators +=  GRBLinExpr(lij);
            // First is that if lij == 1, then for each dimension
            //   at least one of the indicators must be one.
            for (int dim: _sys_vars){
                RangeToVariables const & rI = all_ranges[dim];
                std::vector<GRBVar> rel_vars;
                MpfiWrapper w = b.get_bounds_for_input_var(dim);
                if (debug_milp){
                    std::cout << "Bounds for dimension: " << dim << " are: " << w << std::endl;
                }
                rI.get_all_variables_in_range(w.lower(), w.upper(), rel_vars);
                GRBLinExpr expr;
                for (GRBVar v: rel_vars){
                    expr = expr + GRBLinExpr(v);
                }
                model.addConstr( expr >= lij);
            }

            // Next, if lij == 1, then the output of this piece must depend on the
            // input in a specific manner
            GRBLinExpr lExpr;
            MpfiWrapper bnd;
            for (auto kv: l.linear_expr){
               lExpr += kv.second * input_variables[kv.first];
                bnd  = bnd + (MpfiWrapper(kv.second) * _til.get_bound_for_var(kv.first));
            }

            double tol_ub = l.tol_intvl.upper() + l.const_term;
            double tol_lb = l.tol_intvl.lower() + l.const_term;
            bnd = bnd + l.tol_intvl;
            bnd = bnd + MpfiWrapper(l.const_term);

            double entire_box_ub = bnd.upper();
            double entire_box_lb = bnd.lower();
            if (debug_milp){
                std::cout << "The expression bounds inside the piece are: " << tol_lb << ", " << tol_ub << std::endl;
                std::cout << "The expression bounds for the entire box are:" << entire_box_lb << ", " << entire_box_ub << std::endl;
            }
            model.addConstr(output_var <= (lExpr + tol_ub * lij + entire_box_ub * (1.0 - lij)));
            model.addConstr(output_var >= lExpr + tol_lb * lij + entire_box_lb * (1.0 - lij));
        }

        model.addConstr(sum_of_indicators == 1);
        return output_var;
    }
};
