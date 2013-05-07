#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <vector>
#include <math.h>
#include <string>
#include <fstream>
#include <iostream>
#include "gemsfit_iofiles.h"
using namespace std;


class optimization
{
public:

    // opt vector
    vector<double> optv;

    // Constructor
    optimization(opti_vector *ov );
    optimization(); // init mode constructor DS

    // Destructor
    virtual ~optimization();

    // configuration data for nlopt instance (get from SS_GEMSFIT_input.dat)
    /// name of the optimization algorithm from NLOPT library
    string OptAlgo;
    /// number of threads for parallel execution
//    int OptThreads;
    ///
    double OptTolRel;
    double OptTolAbs;
    int OptMaxEval;
//    int OptConstraints;
    int OptDoWhat;
//    int OptHybridMode;
//    double OptHybridTolRel;
//    double OptHybridTolAbs;
//    int OptHybridMaxEval;
//    int OptNmultistart;
//    string OptHybridAlgo;
//    double OptInitStep;
//    double OptScaleParam;
    bool OptNormParam;
    double OptBoundPerc;

//    vector<double> OptStatOnlyBestFitParam;
//    double OptStatOnlySSR;
//    bool OptEquil;

//    bool test;

    // optimization vector and bounds (get from SS_GEMSFIT_input.dat)
//    vector<double> optv;
    vector<double> OptUpBounds;
    vector<double> OptLoBounds;


    /// printing information (get from SS_GEMSFIT_input.dat)
//        PlotFit* Plot_ActMod;


    /// struct holding constraint values (retrieved from SS_GEMSFIT_input.dat)
    typedef struct
    {
        int id;
        double Constraints;
    } my_constraint_data;
    my_constraint_data* constraint_data;
    vector<my_constraint_data> constraint_data_v;


    // Populate nlopt instance
//    virtual void set_nlopt_param( );
    virtual void get_nlopt_param_txt( vector<double> optv );
    virtual void out_nlopt_param_txt(  bool with_comments, bool brief_mode );
    virtual void define_nlopt_param( );

    // Normalize init vector, bounds and constraints
    virtual void normalize_params( const vector<double> initguesses );

    // NLopt return codes
    virtual void print_return_message( const int result );

//    // initialize optimization
//    virtual void init_optim( vector<double> &optv_, /*TGfitTask* gfittask,*/ int &countit, double &sum_of_squares );




};

#endif // OPTIMIZATION_H
