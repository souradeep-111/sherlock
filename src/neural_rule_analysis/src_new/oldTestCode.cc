#ifdef OLD_TEST_CODE

#include "mpfiWrapper.h"
#include "Polynomial.h"
#include "Box.h"
#include "Tiling.h"
#include "PolynomialApproximator.h"
#include "AffineArithmeticExpression.h"
using NeuralRuleAnalysis::Polynomial;
using NeuralRuleAnalysis::Box;
using NeuralRuleAnalysis::Monomial;
using NeuralRuleAnalysis::MpfiWrapper;
using NeuralRuleAnalysis::Tiling;
using NeuralRuleAnalysis::Tile;
using NeuralRuleAnalysis::PolynomialApproximator;
using NeuralRuleAnalysis::AffineArithmeticExpression;
using NeuralRuleAnalysis::AffineArithmeticNoiseSymbols;

void test1(){
    // Define a polynomial on 4 variables.
    Polynomial p;
    // p = 1.2*x1*x2^2 - x1*x3 + x2*x4^2 - 2.1 * x2 - 1.2 * x3 + 0.1 * x1 - 2.5
    Monomial m0, m1, m2, m3, m4, m5, m6,m7;
    m0.set_power(0, 1);
    m0.set_power(1,2);
    m0.set_coefficient(1.2);
    p.add_monomial(m0);
    m1.set_power(0, 1);
    m1.set_power(2,1);
    m1.set_coefficient(-1.0);
    p.add_monomial(m1);
    m2.set_power(1,1);
    m2.set_power(3,2);
    m2.set_coefficient(1.0);
    p.add_monomial(m2);
    m3.set_power(1,1);
    m3.set_power(2,2);
    m3.set_coefficient(1.0);
    p.add_monomial(m3);
    m4.set_power(1,1);
    m4.set_coefficient(-2.1);
    p.add_monomial(m4);
    //- 1.2 * x3 + 0.1 * x1 - 2.5
    m5.set_power(2,1);
    m5.set_coefficient(-1.2);
    m6.set_power(0,1);
    m6.set_coefficient(0.1);
    m7.set_coefficient(-2.5);
    p.add_monomial(m5);
    p.add_monomial(m6);
    p.add_monomial(m7);
    std::cout << "Created the polynomial: " << p << std::endl;
    // Now evaluate its derivative and partial derivatives
    std::map<int, double> x;
    x[0] = 1.0; x[1] = 2.0; x[2] = -1.0; x[3]= -1.5;
    auto res = p.linearize_around_point(x);
    std::cout << "Linearized around point:"<< x[0]<<","
              <<x[1] <<"," << x[2] <<"," << x[3] << std::endl;
    std::cout << res.second;
    for (int j = 0; j < 3; ++j){
        std::cout << "+ (" << res.first[j] << "* x_"<<j << ")";
    }
    std::cout << std::endl;

    // Now create a box
    std::vector<int> vars({0, 1, 2, 3});
    Box b(vars);
    b.set_dimensions(0, MpfiWrapper(-1.0, 1.0));
    b.set_dimensions(1, MpfiWrapper(-2.1, 2.1));
    b.set_dimensions(2, MpfiWrapper(-1.3, 1.2));
    b.set_dimensions(3, MpfiWrapper(-2.0, -1.0));
    std::cout << b << std::endl;
    MpfiWrapper res2 = p.interval_evaluation(b);
    std::cout << "Polynomial Interval:" << res2<< std::endl;

    // Now try with Affine Arithmetic
    AffineArithmeticNoiseSymbols env;
    AffineArithmeticExpression a = p.affine_arithmetic_evaluate(env, b);
    std::cout << "Affine arithmetic:" << a << std::endl;
    std::cout << "Interval:" << a.get_range() << std::endl;


}

void test2(){
    // Let us test tilings
    std::vector<int> vars({0,1,2});
    std::vector<double> lower({0.0, 0.0, 0.0});
    std::vector<double> upper({5.0, 5.0, 5.0});
    std::vector<int> num_subdivs({5,5,5});
    Tiling til(vars, lower, upper, num_subdivs);
    bool found_tile;
    Tile t0(til), t1(til), t2(til);
    t0.set_coordinates(0, 0, 3);
    t0.set_coordinates(1, 1, 3);
    t0.set_coordinates(2, 1, 5);

    t1.set_coordinates(0, 3, 5);
    t1.set_coordinates(1, 0, 5);
    t1.set_coordinates(2, 0, 3);

    t2.set_coordinates(0, 1, 3);
    t2.set_coordinates(1, 0, 1);
    t2.set_coordinates(2, 2, 3);

    til.insert_tile(t0);
    assert(til.tile_has_no_intersections(t1));
    til.insert_tile(t1);
    assert(!til.tile_has_no_intersections(t1));
    assert(til.tile_has_no_intersections(t2));
    til.insert_tile(t2);
    while(true) {
        auto p1 = til.find_empty_tile();
        found_tile = p1.first;
        Tile t4(p1.second);
        if (found_tile) {
            std::cout << "Found empty tile #2" << std::endl;
            for (int j = 0; j < 3; ++j) {
                std::cout << "x" << j << ": " << t4.get_lower(j)
                          << "," << t4.get_upper(j) << std::endl;
            }
            assert(til.tile_has_no_intersections(t4));
        } else {
            std::cout << "could not find empty tile." << std::endl;
            break;
        }
        til.insert_tile(t4);
    }
}

void test3(){
    Polynomial p;
    // p = 1.1 * x1^2 * x0^2 - 1.2 * x0 * x2 - 2.1 * x1 + 3.5* x2^2;
    Monomial m0, m1, m2, m3;
    m0.set_power(0, 2);
    m0.set_power(1, 2);
    m0.set_coefficient(1.1);
    m1.set_power(0, 1);
    m1.set_coefficient(-1.2);
    m2.set_coefficient(-2.1);
    m2.set_power(1,1);
    m2.set_power(2,1);
    m3.set_coefficient(3.5);
    m3.set_power(2,2);
    p.add_monomial(m0);
    p.add_monomial(m1);
    p.add_monomial(m2);
    p.add_monomial(m3);
    std::cout << "Created : " << p << std::endl;
    std::map<int, double> x;
    x[0] = 1.0; x[1] = -1.0;x[2] = 1.0;
    auto res = p.linearize_around_point(x);
    std::cout << "Linearized around point:"<< x[0]<<","
              <<x[1] << "," << x[2]<< std::endl;
    std::cout << res.second;
    for (int j = 0; j < 6; ++j){
        if (res.first.find(j) != res.first.end())
            std::cout << "+ (" << res.first[j] << "* x_"<<j << ")";
    }
    std::cout << std::endl;

}

void test4(){
    // Create polynomial x1^2 - x2^2 + x2 * x1
    Polynomial p;
    Monomial m0, m1, m2;
    m0.set_coefficient(1.0);
    m0.set_power(1, 2);
    m1.set_coefficient(-1.0);
    m1.set_power(2, 2);
    m2.set_coefficient(1.0);
    m2.set_power(1,1);
    m2.set_power(2,1);
    p.add_monomial(m0);
    p.add_monomial(m1);
    p.add_monomial(m2);
    std::cout << p << std::endl;
    // Set lower and upper bounds
    std::vector<double> lower({-1.0, -1.0});
    std::vector<double> upper({1.0, 1.0});
    std::vector<int> num_subdivs({200,200});
    double tol =0.1;
    PolynomialApproximator papprox(p, tol, lower, upper, num_subdivs);
    papprox.compute_polynomial_approximation();
    papprox.pretty_print_linearization();
    return;
}

#endif