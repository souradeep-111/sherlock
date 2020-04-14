#include "odesim.h"


int DynamicSys::getN_P()const
{
    return N_P;

}


DynamicSys::DynamicSys()
{
    variables = nullptr;
    Pnums = nullptr;

    p_constant = nullptr;
    p_constant_part = nullptr;

    N_P = 0;
};

DynamicSys::DynamicSys(shared_ptr<Variables> vars)
{

    Pnums = make_shared<vector <Expression_AST> >();
    variables = vars;
    N_P = variables->size()-1;

    Pnums.reset(new vector<Expression_AST>(N_P));

/*
    Expression_AST zero_derivative;

    for(int i=1; i<variables->size(); ++i)
    {
        Pnums->push_back(zero_derivative);
    }
*/

    p_constant.reset(new vector<bool>(N_P));
    p_constant_part.reset(new vector<Interval>(N_P));


}


DynamicSys::~DynamicSys()
{
    variables.reset();
    Pnums.reset();

    p_constant.reset();
    p_constant_part.reset();
}

DynamicSys::DynamicSys(const DynamicSys & DynamicSys_)
{
	variables = DynamicSys_.variables;
	Pnums = DynamicSys_.Pnums;
	N_P = DynamicSys_.N_P;

	p_constant = DynamicSys_.p_constant;
	p_constant_part = DynamicSys_.p_constant_part;
}

DynamicSys & DynamicSys::operator = (const DynamicSys & DynamicSys_)
{
    if(this == &DynamicSys_)
        return *this;
    
    variables = DynamicSys_.variables;
    
    Pnums = DynamicSys_.Pnums;
    N_P = DynamicSys_.N_P;
    p_constant = DynamicSys_.p_constant;
    p_constant_part = DynamicSys_.p_constant_part;

    return *this;
}
bool DynamicSys::assignDerivative(const std::string & varName, const Expression_AST & derivative)
{
    int id = variables->getIDForVar(varName);
    
    if(id == -1)
    {
        return false;
    }
    else
    {
        (*Pnums)[id-1] = derivative;
        
		Interval I;
		if(derivative.isConstant(I))
		{
			(*p_constant)[id-1] = true;
			(*p_constant_part)[id-1] = I;
		}

        return true;
    }
}

void DynamicSys::operator() ( const DynamicSys::state_type &x , state_type &dxdt , double t )
{
    
    Real t_r;
    std::vector<Real> domain;
    domain.push_back(t_r);
    
    for (int i=0; i<N_P;i++){
        Real temp_r(x[i]);
        domain.push_back(temp_r);//put real type
    }
    for (int i=0; i<N_P;i++){
        Real result;
        Expression_AST expression_temp = (*Pnums)[i];
        expression_temp.evaluate(result, domain);
        dxdt[i] = result.getValue_RNDD();
        //dxdt[i] = 1;
        
    }

    
    
}

void odesim::simulator(vector <vector <double> >& x0_samples,double t0,double tf,double delta_t)
{
    DynamicSys::state_type x0; // initial conditions
    for(int i=0; i<x0_samples[0].size(); i++){
        vector <double> row;
        for (int j=0; j<x0_samples.size() ;j++){
            x0.push_back(x0_samples[j][i]);
            row.push_back(x0_samples[j][i]);
        }
        setInitial_state(row);
        row.clear();
    //integrate_adaptive( DynamicSys::stepper_type() , DynSys , x0 , t0 , tf , 0.1,*this );
    //integrate( DynSys , x0 , t0 , tf , 0.1, *this); //dt=0.1 is the initial step size,The actual step size will be adjusted during integration due to error control.
        integrate_const( runge_kutta_fehlberg78< DynamicSys::state_type >(),DynSys, x0 , t0 , tf , delta_t,*this);
    
        x0.clear();
    }
    
    
    
}
void odesim::operator() ( const DynamicSys::state_type &x , const double t)
{
    vector <double> row;
    //put time t into data
    row.push_back(t);
    
    //put intial condition into data
    for (int i = 0;i<boost::size( x ); i++ ){
        row.push_back(initial_state[i]);
    }
    setData_x(row);
    row.clear();
    //put state value into data
    for (int i=0 ;i <  boost::size( x ); i++ ){
        row.push_back(x[i]);
    }
    setData_y(row);
    row.clear();
    
    
}



odesim::odesim (const DynamicSys & DynamicSys_ )
{
    DynSys = DynamicSys_;
    initial_state = vector <double> (0);
    data_x = make_shared<vector <vector <double> > >(0);
    data_y = make_shared<vector <vector <double> > >(0);
}

odesim::odesim(const odesim & odesim_): DynSys(odesim_.DynSys), data_x(odesim_.data_x),data_y(odesim_.data_y), initial_state(odesim_.initial_state)
{
}
odesim & odesim::operator = (const odesim & odesim_)
{   
    if(this == &odesim_)
        return *this;
    
    DynSys = odesim_.DynSys;
    initial_state = odesim_.initial_state;
    data_x = odesim_.data_x;
    data_y = odesim_.data_y;
    return *this;
}

odesim::~odesim()
{
    data_x.reset();
    data_y.reset();
}


void odesim::setData_x(vector <double> &row)
{
    data_x->push_back(row);

}
void odesim::setData_y(vector <double> &row)
{
    data_y->push_back(row);
    
}
void odesim::setInitial_state(vector <double> & x0)
{
    initial_state=x0;
    
}
