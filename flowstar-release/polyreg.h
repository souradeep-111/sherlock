#ifndef POLYREGRESSION
#define POLYREGRESSION

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include "odesim.h"
#include "TaylorModel.h"
#include "qr_solve.hpp"

using namespace std;
using namespace flowstar;

//class polyregssion{
//private:
//    //vector <vector <double> > data_x;//output//{{t0,x0,y0,z0};{t1,x1,y1,z1};}
//    int order;
//public:
//    //void simulator(vector <vector <double> >& x0_samples,double t0,double tf,double delta_t);
//
//    polyregssion (const DynamicSys & DynamicSys_ );
//    polyregssion(const odesim & odesim_);
//    polyregssion & operator = (const polyregssion & polyregssion_);
//    ~polyregssion();
//    EvalPolyRegssion()
//    
//};
int i4_choose ( int n, int k );
void getsubmono (const int m,const int k_order,vector<vector<int> > & v);
double mono_value ( int m, vector <int> & f,int max_order, double* table_ValueOrder);
void mono_generator (const int n,const int kmax, vector<vector<int> > & table );
void EvalPolyRegssion(TaylorModelVec & tmv_regression, vector<vector<int> > & mono_table, shared_ptr<vector <vector <double> > > data_x,shared_ptr<vector <vector <double> > > data_y, vector<double> & errorVec,int max_order);

void PolyRegssionSim(TaylorModelVec & tmv_regression, vector<vector<int> > & mono_table, const TaylorModelVec & initialSet, const int d, const DynamicSys & DynamicSys1, double tf, int sim_step,vector<double> & errorVec, int max_order);
#endif
