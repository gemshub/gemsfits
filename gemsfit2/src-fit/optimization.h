/// \file optimization.h
/// Contains declaration of optimization class used in GEMSFIT for
/// storing configuration parameters for optimization algorithm as well as
/// the vector of optimized parameters
//
/// \class optimization optimization.h
/// The optimization class, retrieves and stores configuration parameters
/// for optimization algorithm form the input file
//
// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT/>
//
// GEMSIFT2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT2 code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//



#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <vector>
#include <math.h>
#include <string>
#include <fstream>
#include <iostream>
#include "gemsfit_iofiles.h"
#include <jansson.h>
#include <sstream>
#include "opt_vector.h"
using namespace std;


class optimization
{

private:

//    /// struct holding constraint values (retrieved from SS_GEMSFIT_input.dat)
    typedef struct
    {
        int id;
        double Constraints;
    } my_constraint_data;
    my_constraint_data* constraint_data;
    vector<my_constraint_data> constraint_data_v;


    // Populate nlopt instance
////    virtual void set_nlopt_param( );
    void get_nlopt_param ( );
//    virtual void define_nlopt_param( );


public:

    typedef vector<double> vd;        // vector of double
    typedef vector<vector<double> > vvd;  // 2D vector of double
    typedef vector<string> vs;        // vector of string
    typedef vector<int> vi;           // vecotr of integer
    typedef vector<vector<int> > vvi; // 2D vector of integer


    // opt vector
    vector<double> optv, optv_0, optv0;

    // Constructor
    optimization( );
//    optimization( int i); // init mode constructor DS

//    bool h_RDc; /// handle for checking if there are reaction constraints in the input file


    // Destructor
     ~optimization();

//    /// Writes the NLopt parameters in the input tamplat file
//    virtual void out_nlopt_param_txt(  bool with_comments, bool brief_mode );

    // configuration data for nlopt instance (get from SS_GEMSFIT_input.dat)
    /// name of the optimization algorithm from NLOPT library
    string OptAlgo;
    vector<string> OptParameters;
    string NFunParameters;
    string GEMSsys;
    int mode;
    /// number of threads for parallel execution
//    int OptThreads;
    ///
    double OptTolRel;
    double OptTolAbs;
    int OptMaxEval;
    int OptDoWhat;
    int OptEquilibrium;
    int OptTuckey;
    int OptTuckeyVal;
    int OptTitration;
    int OptUserWeight;
    double OptInitStep;
    bool OptNormParam;
    double OptBoundPerc;
    double OptPerturbator;

    vector<double> OptUpBounds, UB;
    vector<double> OptLoBounds, LB;

    /// Normalize init vector, bounds and constraints
    void normalize_params( const vector<double> initguesses, bool NormBounds );

    /// NLopt return codes
    void print_return_message( const int result );

    void get_OptParameters ( );

    bool h_optNF; /// handle for checking if there are dynamic function in the DataTarget


    long int sizeOPs;     ///
    vector<OptParameter*> optParam; ///
    vector<OptParameter*> optNFParam;

    void OptParameterCreate ();

    void GetParameters ();

};

#endif // OPTIMIZATION_H
