#ifndef ODESIM
#define ODESIM

#include "TaylorModel.h"
#include "expression.h"

#include <iostream>
//#include <boost/array.hpp>
#include <boost/numeric/odeint.hpp>



#include <vector>
#include <fstream>
#include <memory>

using namespace flowstar;
using namespace std;

using namespace boost::numeric::odeint;



class DynamicSys
{
public:
    shared_ptr<Variables> variables;
    shared_ptr<vector <Expression_AST> > Pnums;

    shared_ptr<vector<bool> > p_constant;
    shared_ptr<vector<Interval> > p_constant_part;

    int N_P;
    
public:
    typedef vector< double > state_type;//declare the size of state_type
    typedef controlled_runge_kutta< runge_kutta_cash_karp54< state_type > > stepper_type;
    DynamicSys();
    //DynamicSys (const vector<Expression_AST> & polys_ );
    //DynamicSys(const Variables & vars);
    DynamicSys(shared_ptr<Variables> vars);
    DynamicSys(const DynamicSys & DynamicSys_);
    ~DynamicSys();
    //void write_( const DynamicSys::state_type &x , const double t );
    int getN_P()const;
    bool assignDerivative(const std::string & varName, const Expression_AST & derivative);
    DynamicSys & operator = (const DynamicSys & DynamicSys_);
    void operator() ( const DynamicSys::state_type &x , state_type &dxdt , double t );
};

class odesim
{
private:
    DynamicSys DynSys;
    vector <double> initial_state;
public:
    shared_ptr<vector <vector <double> > > data_x;//output//{{t0,x0,y0,z0};{t1,x0,y0,z0};}
    shared_ptr<vector <vector <double> > > data_y;//output//{{x0,y0,z0};{x1,y1,z1};}
    void write_( const DynamicSys::state_type &x , const double t );
    void simulator(vector <vector <double> >& x0_samples,double t0,double tf,double delta_t);
    //odesim();
    odesim (const DynamicSys & DynamicSys_ );
    odesim(const odesim & odesim_);
    odesim & operator = (const odesim & odesim_);
    ~odesim();
    void operator() ( const DynamicSys::state_type &x , const double t);
    //const vector <vector <double> > getData_x()const;
    //void getData_x(vector <vector <double> > & data_x1)const;
    void setData_x(vector <double> &row);
    void setData_y(vector <double> &row);
    void setInitial_state(vector <double> & x0);
    
};
#endif
