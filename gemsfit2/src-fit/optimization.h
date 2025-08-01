/// \file optimization.h
/// Contains declaration of optimization class used in GEMSFIT for
/// storing configuration parameters for optimization algorithm as well as
/// the std::vector of optimized parameters
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
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "opt_vector.h"
#include "gemsfit_iofiles.h"

class optimization
{

private:

//    /// struct holding constraint values (retrieved from SS_GEMSFIT_input.dat)
    typedef struct
    {
        int id;
        double Constraints;
    } my_constraint_data;
    std::shared_ptr<my_constraint_data> constraint_data;
    std::vector<my_constraint_data> constraint_data_v;


    // Populate nlopt instance
////    virtual void set_nlopt_param( );
    void get_nlopt_param ( );
//    virtual void define_nlopt_param( );


public:

    typedef std::vector<double> vd;        // std::vector of double
    typedef std::vector<std::vector<double> > vvd;  // 2D std::vector of double
    typedef std::vector<std::string> vs;        // std::vector of std::string
    typedef std::vector<int> vi;           // vecotr of integer
    typedef std::vector<std::vector<int> > vvi; // 2D std::vector of integer


    // opt std::vector
    std::vector<double> optv, optv_0, optv0;

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
    std::string OptAlgo;
    std::vector<std::string> OptParameters;
    std::string AddOptParameters;
    std::string NFunParameters;
    std::string GEMSsys;
    int mode;
    /// number of threads for parallel execution
//    int OptThreads;
    ///
    double OptTolRel;
    double OptTolAbs;
    int OptMaxEval;
    int OptDoWhat;
    int OptPrcParamDigits=-1;
    int OptEquilibrium;
    int OptGemsSIA = -1;
    int OptTuckey;
    int OptTuckeyVal;
    int OptTitration = 0;
    int OptUserWeight;
    int OptMixedSumOfResiduals = -1;
    double OptInitStep;
    bool OptNormParam;
    double OptBoundPerc;
    double OptPerturbator;

    std::vector<double> OptUpBounds, UB;
    std::vector<double> OptLoBounds, LB;

    /// Normalize init std::vector, bounds and constraints
    void normalize_params( const std::vector<double> initguesses, bool NormBounds );

    /// NLopt return codes
    void print_return_message( const int result );

    void get_OptParameters ( );

    bool h_optNF; /// handle for checking if there are dynamic function in the DataTarget


    long int sizeOPs;     ///
    std::vector<std::shared_ptr<OptParameter>> optParam; ///
    std::vector<std::shared_ptr<OptParameter>> optNFParam;

    void OptParameterCreate ();

    void GetParameters ();

};

#endif // OPTIMIZATION_H
