//
// Created by Mac User on 1/19/18.
//

#include "neuralRuleAnalysisInterface.h"
#include "Box.h"
#include <string>
#include "RangeToVariables.h"

using NeuralRuleAnalysis::MpfiWrapper;
using NeuralRuleAnalysis::Box;
using NeuralRuleAnalysis::Monomial;
using NeuralRuleAnalysis::Polynomial;
using NeuralRuleAnalysis::RangeToVariables;
using NeuralRuleAnalysis::PolynomialApproximator;

#define TOL_THRESHOLD 1E-07

bool debug = false;

int total_degree(my_monomial_t const & m){
    int n = (int) m.second.size();
    int sum = 0;
    for(int k=0; k< n; ++k){
        sum = sum + m.second[k];
    }
    return sum;
}

Monomial convert_my_monomial( my_monomial_t mon){
    Monomial m( MpfiWrapper(mon.first));
    int n = (int) mon.second.size();
    for (int i=0; i< n; ++i ){
        if (mon.second[i] > 0){
            m.set_power(i, mon.second[i]);
        }
    }
    return m;
}

void decompose_input_poly(std::vector<my_monomial_t> &poly,
                          std::vector<Polynomial> & result,
                          std::map<int, double> & linear_part,
                          double & const_coeff){
    // 1. Iterate through each my_monomial_t
    //     1.1 Collect the used variables as a set
    //     1.2 If just one or zero variables, add to the linear part and continue
    //     1.3 Iterate through the polynomials so far.
    //        1.3.1 If any polynomial's used variables subsume, then add this monomial to that poly and continue outer loop.
    //     1.4 If monomial has not been added, add it as part of a fresh polynomial
    for (my_monomial_t const & mon: poly){
        Monomial m = convert_my_monomial(mon);
        std::set<int> used_vars;
        m.add_used_variables_to_set(used_vars);
        if (used_vars.size() == 0) {
            // Just a constant
            const_coeff += mon.first;
            continue;
        } else if (total_degree(mon) == 1){
            // Add to linear part
            assert(used_vars.size() == 1);
            int var_id = *(used_vars.begin());
            auto it = linear_part.find(var_id);
            double tmp = mon.first;
            if (it != linear_part.end())
                tmp = tmp + it -> second;
            linear_part[var_id] = tmp;
            continue;
        } else {
            // Iterate through polynomials so far
            bool subsumed  = false;
            for (Polynomial & p: result){
                std::vector<int> poly_used_vars = p.get_used_vars();
                if ( std::includes(poly_used_vars.begin(), poly_used_vars.end(),
                                  used_vars.begin(), used_vars.end()) ){
                    p.add_monomial(m);
                    subsumed = true;
                    break;
                }
            }
            if (!subsumed){
                Polynomial new_poly;
                new_poly.add_monomial(m);
                result.push_back(new_poly);
            }
        }
    }
}

void
piecewise_approximate_polynomial(int n, std::vector<my_monomial_t>  &poly, std::vector<double> const &lower_bounds,
                                 std::vector<double> const &upper_bounds, double tol,
                                 std::vector<int> const &num_subdivs, std::vector<PolynomialApproximator> &result) {

    std::map<int, double> linear_part;
    double const_coeff = 0.0;

    // Sort the polynomials so that the terms with higher total degree are placed up front.
    std::sort(poly.begin(), poly.end(), [](my_monomial_t const & a, my_monomial_t const & b){
        return total_degree(a) > total_degree(b);
    });
    // Now perform a decomposition
    std::vector<Polynomial> list_of_polys;

    decompose_input_poly(poly, list_of_polys, linear_part, const_coeff );
    if (debug){
        int dcount = 0;
        for (Polynomial & p: list_of_polys){
            std::cout << "Decomposed term #" << dcount++ <<" -> ";
            std::cout << p << std::endl;
        }
        std::cout << "Decomposed into: " << list_of_polys.size() << " polynomials." <<std::endl;
    }
    int n_polys = (int) list_of_polys.size();
    int count = 1;
    for (Polynomial & p : list_of_polys){
        if (debug)
            std::cout << "Working on poly #" << count << std::endl;
        count ++;
        // First prepare the dimensions corresponding to the used variables
        std::vector<int> vars = p.get_used_vars();
        std::vector<double> lb, ub;
        std::vector<int> num_sub;
        for (int var_id: vars){
            lb.push_back(lower_bounds[var_id]);
            ub.push_back(upper_bounds[var_id]);
            num_sub.push_back(num_subdivs[var_id]);
        }
        double new_tol= tol / (double) n_polys;
        PolynomialApproximator pa(p, new_tol, lb, ub, num_sub);
        pa.compute_polynomial_approximation();
        if (debug)
            std::cout << "Produced " << pa.num_linear_pieces() << " pieces in approximation" << std::endl;
        result.push_back(pa);
    }
    // It remains to handle the linear piece
    if (linear_part.size() > 0 || const_coeff >= TOL_THRESHOLD|| const_coeff < - TOL_THRESHOLD) {
        Polynomial linear_poly;
        for (auto const &kv: linear_part) {
            Monomial m(MpfiWrapper(kv.second));
            m.set_power(kv.first, 1);
            linear_poly.add_monomial(m);
        }
        MpfiWrapper cc(const_coeff);
        Monomial m0(cc);
        linear_poly.add_monomial(m0);
        vector<int> linear_vars = linear_poly.get_used_vars();
        Box input_box(linear_vars);
        std::vector<int> nsd_linear;
        std::vector<double> lb_linear;
        std::vector<double> ub_linear;
        for (int i: linear_vars) {
            input_box.set_dimensions(i, MpfiWrapper(lower_bounds[i], upper_bounds[i]));
            lb_linear.push_back(lower_bounds[i]);
            ub_linear.push_back(upper_bounds[i]);
            nsd_linear.push_back(num_subdivs[i]);
        }
        PolynomialApproximator pa_linear(linear_poly, tol, lb_linear, ub_linear, nsd_linear);
        pa_linear.add_linear_piece(input_box, linear_part, const_coeff, MpfiWrapper(0.0));
        pa_linear.set_linear(true);
        result.push_back(pa_linear);
    }
}


GRBVar encode_pwl_models_in_milp(int n, std::vector<PolynomialApproximator> const & decomposed_pwls,
                               std::vector<double> const &lower_bounds,
                               std::vector<double> const &upper_bounds,
                               GRBModel & model, std::vector<GRBVar> const & input_variables){
    // 1. First collect a list of intervals along each dimension that are involved in the pieces.
    // 2. Next create binary variables corresponding to these intervals.
    //    Add constraints that link the binary variables and the appropriate input dimensions
    // 3. Next, create the PWL model for each PolynomialApproximator
    //   3.1 for each linear piece do
    //       3.1.1 Create a binary variable for each piece.
    //       3.1.2 Link the binary variable for each piece with the variables for the individual dimension
    //       3.1.3 Set the output variable constraint as a function of the inputs for the piece
    // 4. Add the outputs of the polynomialapproximators to create the overall output variable for the entire PWL

    std::vector< std::set<double> > dimensional_cut_points;
    for (int i = 0; i < n; ++i){
        set<double> s;
        /* Insert upper and lower bounds for each dimension*/
        s.insert(lower_bounds[i]);
        s.insert(upper_bounds[i]);
        dimensional_cut_points.push_back(s);
    }

    for (PolynomialApproximator const & pa:decomposed_pwls){
        /* Iterate pieces and insert the lhs and rhs boundary into corresponding parts */
        pa.collect_cutpoints(dimensional_cut_points);
    }

    /* Next, create the appropriate variables and add the constraints */
    std::vector<RangeToVariables> all_ranges;
    for (int i = 0; i < n; ++ i){
        RangeToVariables rvI(i, lower_bounds[i], upper_bounds[i], dimensional_cut_points[i]);
        rvI.add_variables_and_constraints(model, input_variables);
        all_ranges.push_back(rvI);
    }

    /* Create model for each polynomial approximator */
    int count = 0;
    GRBLinExpr sum_of_individual_terms;
    for (PolynomialApproximator const & pa:decomposed_pwls){
        GRBVar v = pa.encode_in_gurobi_model(all_ranges, model, input_variables, count);
        count ++;
        sum_of_individual_terms = sum_of_individual_terms + GRBLinExpr(v);
    }
    /*- Add a final equality constraint -*/
    GRBVar retVar = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS, std::string("zz_pwl"));
    model.addConstr(retVar == sum_of_individual_terms);
    return retVar;
}