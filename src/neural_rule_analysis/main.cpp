#include <iostream>
#include <vector>
#include <random>
#include "neuralRuleAnalysisInterface.h"
#include <gurobi_c++.h>
#include <string>

std::vector<my_monomial_t> create_test_polynomial(){
    std::vector<my_monomial_t> poly;
    // // Create the polynomial 0.1 x1^2 - 0.1 x2^2 + 0.06 x1 * x2
    // std::vector<int> m1(2,0);
    // m1[0] = 2;
    // poly.push_back(std::make_pair(0.1, m1));
    // std::vector<int> m2(2,0);
    // m2[1] = 2;
    // poly.push_back(std::make_pair(-0.1, m2));
    // std::vector<int> m3(2,0);
    // m3[1] = 1;
    // m3[0] = 1;
    // poly.push_back(std::make_pair(0.06, m3));
    // return poly;

    // Create the polynomial 0.1 x1^2 - 0.1 x2^2
    std::vector<int> m1(2,0);
    poly.push_back(std::make_pair(-0.665025, m1));

    std::vector<int> m2(2,0);
    m2[0] = 1;
    poly.push_back(std::make_pair(-10.121, m2));

    std::vector<int> m3(2,0);
    m3[1] = 1;
    poly.push_back(std::make_pair(-4.95552, m3));

    std::vector<int> m4(2,0);
    m4[0] = 2;
    poly.push_back(std::make_pair(0.157891, m4));

    std::vector<int> m5(2,0);
    m5[0] = 1;
    m5[1] = 1;
    poly.push_back(std::make_pair(0.169693, m5));

    std::vector<int> m6(2,0);
    m6[1] = 2;
    poly.push_back(std::make_pair(0.0421063, m6));


    return poly;
}

std::vector<my_monomial_t> create_random_polynomial_and_test(int n, int d, int num_monomials){
    std::vector< my_monomial_t > random_poly;
    std::default_random_engine gen;
    std::uniform_int_distribution<int> coeffs_distrib(-10, 10);
    std::uniform_int_distribution<int> uniform_dist(0, n-1);
    std::uniform_int_distribution<int> power_dist(1,d);
    for (int i=0; i < num_monomials; ++i){
        // Generate a random coefficient between -10 and 10
        int max_deg = power_dist(gen);
        double c = (double) coeffs_distrib(gen)/4.0+0.15;
        std::vector<int> pows(n, 0);
        for (int j =0; j < max_deg; ++j){
            int var_id = uniform_dist(gen);
            pows[var_id] += 1;
        }
        random_poly.push_back(std::make_pair(c, pows));

    }
    std::cout << "---poly---"<<std::endl;
    for (auto & pa: random_poly) {
        double c = pa.first;
        std::vector<int> & pows = pa.second;
        std::cout << c << " ";
        for (int k = 0; k < n; ++k) {
            if (pows[k] > 0) {
                std::cout << "x_" << k;
                if (pows[k] > 1)
                    std::cout << "^" << pows[k] << " ";
                else
                    std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "---poly---"<<std::endl;
    return random_poly;
}

int test1(){
    int n = 50; // Number of input variables is 50
    auto rpoly = create_random_polynomial_and_test(n, 3, 50); // create a random polynomial over 50 terms with degree <= 3
    std::vector<double> lb(n, -0.5); // Lower bound is simply -0.2
    std::vector<double> ub(n, 0.5); // Upper bound along each dimension is simply 0.2

    std::vector<int> nsubs(n, 20); // Choose 20 subdivisions along each axis
    std::vector<PolynomialApproximator> res; // This is the vector that will store ther results
    double tol = 0.1;
    // Call the piecewise approximation. This will decompose the polynomial into many terms.
    // Each term will involve a small number of variables out of the total 150
    piecewise_approximate_polynomial(n, rpoly,lb, ub, tol, nsubs, res);

    for (PolynomialApproximator & pa: res){
        std::cout << "------ Polynomial Approximation ------ " << std::endl;
        std::cout << "Polynomial is : " << pa.get_polynomial() << std::endl;
        pa.pretty_print_linearization();
    }
    return 1;
}

int main() {
    int n = 2;
    std::vector<my_monomial_t> rpoly = create_test_polynomial();
    std::vector<double> lb(2, -0.5); // Lower bound is simply -0.2
    std::vector<double> ub(2, 0.5); // Upper bound along each dimension is simply 0.2
    std::vector<int> nsubs(2, 20); // Choose 20 subdivisions along each axis
    std::vector<PolynomialApproximator> res; // This is the vector that will store ther results
    lb[0] = 0.355; lb[1] = -1.105;
    ub[0] = 0.722; ub[1] = -0.694;

    // Change tolerance to 0.02 to make things feasible.
    double tol = 0.01;

    piecewise_approximate_polynomial(n, rpoly,lb, ub, tol, nsubs, res);
    for (PolynomialApproximator & pa: res){
        std::cout << "------ Polynomial Approximation ------ " << std::endl;
        std::cout << "Polynomial is : " << pa.get_polynomial() << std::endl;
        pa.pretty_print_linearization();
    }

    GRBEnv env;
    GRBModel model(env);
    // model.set(GRB_DoubleParam_IntFeasTol, 1e-9);

    GRBVar x1 = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS, std::string("x1"));
    GRBVar x2 = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS, std::string("x2"));
    std::vector<GRBVar> all_inputs({x1, x2});
    GRBVar PWL_output = encode_pwl_models_in_milp(2, res, lb, ub, model, all_inputs);
    GRBLinExpr objective_expr;
    objective_expr = 0;
    double data = 1.0;
    objective_expr.addTerms(& data, & PWL_output, 1);
    model.setObjective(objective_expr, GRB_MAXIMIZE);
    model.optimize();

    string s = "./Gurobi_file_created/Linear_program.lp";
    model.write(s);

    if(model.get(GRB_IntAttr_Status) == GRB_OPTIMAL)
    {
      cout << "Optima computed = " << model.get(GRB_DoubleAttr_ObjVal) << endl;
    }
    else if(model.get(GRB_IntAttr_Status) == GRB_INF_OR_UNBD)
    {
      model.set(GRB_IntParam_DualReductions, 0);
      model.update();
      model.optimize();
      if( model.get(GRB_IntAttr_Status) == GRB_OPTIMAL )
      {
        cout << "Optima computed  after dual reductions = " << model.get(GRB_DoubleAttr_ObjVal) << endl;
      }
      else
      {
        cout << "Model deemed infeasible, printing the infeasible constraint to file " <<
        ": ./Gurobi_file_created/infeasible_model.ilp " << endl;
        model.computeIIS();
        model.write("./Gurobi_file_created/infeasible_model.ilp");
      }
    }

    return 1;


}
