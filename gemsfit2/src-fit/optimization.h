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
using namespace std;


class optimization : public opti_vector
{

private:

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
    virtual void define_nlopt_param( );

public:

    // opt vector
    vector<double> optv;

    // Constructor
    optimization( );
    optimization( int i); // init mode constructor DS

    // Destructor
    virtual ~optimization();

    /// Writes the NLopt parameters in the input tamplat file
    virtual void out_nlopt_param_txt(  bool with_comments, bool brief_mode );

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
    double OptInitStep;
//    double OptScaleParam;
    bool OptNormParam;
    double OptBoundPerc;
    double OptPerturbator;

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

    /// Normalize init vector, bounds and constraints
    virtual void normalize_params( const vector<double> initguesses );

    /// NLopt return codes
    virtual void print_return_message( const int result );


};

#endif // OPTIMIZATION_H
