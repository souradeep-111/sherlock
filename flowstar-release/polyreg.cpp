#include "polyreg.h"

int i4_choose ( int n, int k )
{
  int i;
  int mn;
  int mx;
  int value;

  mn = k;
  if ( n - k < mn )
  {
    mn = n - k;
  }

  if ( mn < 0 )
  {
    value = 0;
  }
  else if ( mn == 0 )
  {
    value = 1;
  }
  else
  {
    mx = k;
    if ( mx < n - k )
    {
      mx = n - k;
    }
    value = mx + 1;

    for ( i = 2; i <= mn; i++ )
    {
      value = ( value * ( mx + i ) ) / i;
    }
  }

  return value;
}

void getsubmono (const int m,const int k_order,vector<vector<int> > & v){
    if((m-1)>0){
        
        for (int i=0; i<=k_order;i++){
            int res_order=k_order-i;
            vector<vector<int> > temp_v;
            getsubmono(m-1,res_order,temp_v);
            
            for(int j=0;j<temp_v.size();j++){
                temp_v[j].push_back(i);
            }
            for (int j=0;j<temp_v.size();j++){
                v.push_back(temp_v[j]);
            }
            
        }
    }
    else{
        vector <int> v1;
        
        v1.push_back( k_order);
        v.push_back(v1);
        
    }
    
}

void mono_generator (const int n,const int kmax, vector<vector<int> > & table ){
    //n: the number of variable
    //kmax: the maximum of monomial's order
    if(table.size() > 0){
        return;
    }
    for (int k=0; k<=kmax; k++){
        vector<vector<int> > temp_table;
        getsubmono(n,k,temp_table);
        
        for (int j=0;j<temp_table.size();j++){
            table.push_back(temp_table[j]);
        }
        temp_table.clear();
    }


}

double mono_value ( int m, vector <int> & f,int max_order, double* table_ValueOrder)

//****************************************************************************
//
//    MONO_VALUE evaluates a monomial.
//    input m: the number of variable
//    input f: the monimial order
//    input table_ValueOrder: the table for each varaible in ascending order
{
    double mult = 1.0;
    
    for(int j=0; j<m; j++){
        if(f[j] != 0){
            //mult *= value_order[j][expont[i][j]-1];
            int aa=0;
            for(int k=0;k<j;k++){ aa=aa+max_order;}
            
            mult *= table_ValueOrder[aa+(f[j]-1)];
            
        }
        
    }
    
    
    return mult;
}

void EvalPolyRegssion(TaylorModelVec & tmv_regression, vector<vector<int> > & mono_table, shared_ptr<vector <vector <double> > > data_x,shared_ptr<vector <vector <double> > > data_y, vector<double> & errorVec,int max_order)
{
	int n_var = (*data_x)[0].size();//include t
	//int max_order = 4;
    vector <vector <double>> pred_Y;
    //enumerate the number of monomials in n_var dimensions of degree up to max_order.
	int n_mono = i4_choose ( n_var + max_order, max_order );
    double *newA;
    int nc=data_x->size();
    int nr=n_mono;
    int size_newA=nc*nr;
    newA = new double[size_newA];
    for (int i=0; i<size_newA;i++){newA[i]=0.0;}
    
    //Construct monomial table
//    vector<vector<int> > mono_table;
    mono_generator (n_var, max_order, mono_table);

    //find max value for scaling
    vector <double> gamma;
    for (int j=0; j<n_var;j++){
        double maxval=(*data_x)[0][j];
        for(int i1=1; i1<nc;i1++){
            if((*data_x)[i1][j] > maxval){maxval=(*data_x)[i1][j];}
        }
        if(maxval<1.0){maxval=1.0;}
        gamma.push_back(maxval);
    }

    for (int j=0; j<nc; j++){
        
        ///Construct the table************
        int tol_order=max_order*n_var-1; //total number of all possible variable^order term
        double *table_ValueOrder;
        table_ValueOrder= new double[tol_order];
        
        int count=0;
        for(int i1=0; i1<n_var; i1++){
            double xPow = 1.0;
            
            for(int j1=0; j1<max_order; j1++){
                xPow *= ((*data_x)[j][i1]/gamma[i1]);// build up the power of x from term to term
                //value_order[i1][j1]=xPow;
                table_ValueOrder[count]=xPow;
                count+=1;
            }
        }
        ///*********************************
        
        for (int i=0; i<n_mono; i++){

            double resultx = mono_value ( n_var, mono_table[i], max_order, table_ValueOrder);
            newA[ i*nc + j ] = resultx;
        }
        delete[] table_ValueOrder;
        
    }


    for (int i_v=0; i_v<n_var-1; i_v++)
    {
        //find max value for scaling
        double gamma_y;
        double maxval=(*data_y)[0][i_v];
        for(int i1=1; i1<nc;i1++){
            if((*data_y)[i1][i_v] > maxval){maxval=(*data_y)[i1][i_v];}

            if(maxval<1.0){maxval=1.0;}
            gamma_y=maxval;
        }

        double *newb;
        newb = new double[nc];
        for(int i = 0; i < nc; i++){
            newb[i]=(*data_y)[i][i_v]/gamma_y;
        }  

       
        double *xc;
        xc = qr_solve ( nc, nr, newA, newb );

        
        
        //To check the error
        vector <double> pred_Ytemp;
        for (int i=0;i<nc;i++){
            double sum1=0;
            for (int j=0; j<nr; j++){
                sum1 = sum1+newA[ j*nc + i ]*xc[j];
                printf("xc_%d = %lf\n", i, xc[j]);
            }
            pred_Ytemp.push_back(sum1);
        }
        pred_Y.push_back(pred_Ytemp);

        double error=0.0;
        for (int i=0;i<nc;i++){
            error+=abs(pred_Ytemp[i]-newb[i])*gamma_y;
        
        }
        pred_Ytemp.clear();
        error = error/float(data_x->size());
        errorVec.push_back(error);



        TaylorModel tmTemp;

        for(int i=0; i<n_mono; ++i)
        {

            //transform coef xc back to coef in original scale
            xc[i]*=gamma_y;
            for(int j=0;j<n_var;j++){
                int xIndex=mono_table[i][j];
                if (xIndex != 0){
                    xc[i]/=pow(gamma[j],xIndex);
                }
            }



            if(xc[i] < -1e-10 || xc[i] > 1e-10)
            {
                Monomial monomial(xc[i], mono_table[i]);
                tmTemp.expansion.monomials.push_back(monomial);
            }
        }

        tmTemp.expansion.reorder();

        tmv_regression.tms.push_back(tmTemp);


        delete[] newb ; //freed memory

        delete[] xc;

    }
    delete[] newA ;

    

}

void PolyRegssionSim(TaylorModelVec & tmv_regression, vector<vector<int> > & mono_table, const TaylorModelVec & initialSet,const int d, const DynamicSys & DynamicSys1, double tf, int sim_step,vector<double> & errorVec, int max_order)
{
    // matrix to keep the samples, it is of the size d \times (2 * d + 1)
    rMatrix samples(d, 2*d+1);
    
    initialSet.get_samples(samples);

    //samples.output(stdout);
    // transfer rMatrix to vector of vectors <double> x0_samples
    int size1=d;
    int size2 =2*d+1;
    vector <vector <double> > x0_samples;
    vector <double> row;
    for (int i=0 ;i<size1; i++){
        for(int j=0; j<size2; j++){
            row.push_back( samples[i][j].getValue_RNDD());
            
        }
        x0_samples.push_back(row);
        row.clear();
    }

    double t0=0.0;
    double delta_t=tf/double(sim_step);
    odesim odesim1(DynamicSys1);
    
    odesim1.simulator(x0_samples,t0,tf,delta_t);
    EvalPolyRegssion(tmv_regression, mono_table,odesim1.data_x,odesim1.data_y,errorVec,max_order);
}






















