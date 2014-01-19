// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT2 code for parameterization of thermodynamic
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

/**
 *	@file optimization.cpp
 *
 *	@brief this header file contains implementetions of optimization class functions for reading in
 *  and setting optimization specific input parameters
 *
 *	@author G. Dan Miron
 *
 * 	@date 17.05.2013
 *
 */


#include "optimization.h"

optimization::optimization()
{
    // GEMSFIT logfile
    //const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
//    ofstream fout;
//    fout.open(gpf->FITLogFile().c_str(), ios::app);
//    if( fout.fail() )
//    { cout<<"Output fileopen error"<<endl; exit(1); }

    constraint_data = new my_constraint_data;
    // assigining the vector that stores the optimized parameters
    optv = opt;

    OptUpBounds = UB;
    OptLoBounds = LB;

    gpf->fout << "11. optimization.cpp(54). Reading NLopt optimization settings from the input file; " << endl;
    get_nlopt_param_txt( optv );

    if (OptBoundPerc > 0.)
    {
        UB = OptUpBounds;
        LB = OptLoBounds;
    }
//    fout.close();
}

optimization::~optimization()
{
    delete constraint_data;

}

void optimization::normalize_params(const vector<double> initguesses , bool NormBounds)
{
    unsigned int i= 0;

    // GEMSFIT logfile
    //const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
//    ofstream fout;
//    fout.open(gpf->FITLogFile().c_str(), ios::app);
//    if( fout.fail() )
//    { cout<<"Output fileopen error"<<endl; exit(1); }

    gpf->fout << " ... performing parameter normalization ... " << endl;

    // Normalize init guess vector
    optv.resize( initguesses.size() );
    for(i=0; i<optv.size(); i++)
    {
        optv[i] = initguesses[i] / fabs(initguesses[i]);
    }

    // Normalize upper bounds vector
    if (!NormBounds)
    {
    for(i=0; i<OptUpBounds.size(); i++)
    {
        gpf->fout << "     Init guess ["<<i<<"] = " << initguesses[i] << endl;
        gpf->fout << "     Upper Bound old ["<<i<<"]= " << OptUpBounds[i] << endl;
        OptUpBounds[i] = OptUpBounds[i] / fabs(initguesses[i]);
        gpf->fout << "     Upper Bound new ["<<i<<"]= " << OptUpBounds[i] << endl;
    }

    // Normalize lower bounds vector
    for(i=0; i<OptLoBounds.size(); i++)
    {
        gpf->fout << "     Init guess ["<<i<<"] = " << initguesses[i] << endl;
        gpf->fout << "     Lower Bound old ["<<i<<"]= " << OptLoBounds[i] << endl;
        OptLoBounds[i] = OptLoBounds[i] / fabs(initguesses[i]);
        gpf->fout << "     Lower Bound new ["<<i<<"]= " << OptLoBounds[i] << endl;
    }

    // Normalize constraints vector
    for(i=0; i<constraint_data_v.size(); i++)
    {
        constraint_data_v[i].Constraints = constraint_data_v[i].Constraints / fabs(initguesses[i]);
    }
    }

//    fout.close();
}


// NLopt return codes
void optimization::print_return_message( const int result )
{
    switch (result)
    {
        case -5:
            cout<<" Halted because of a forced termination: the user called nlopt_force_stop(opt) on the optimization’s nlopt_opt object opt from the user’s objective function or constraints. "<<endl;
            break;
        case -4:
            cout<<" Halted because roundoff errors limited progress. (In this case, the optimization still typically returns a useful result.) "<<endl;
            break;
        case -3:
            cout<<" Ran out of memory. "<<endl;
            break;
        case -2:
            cout<<" Invalid arguments (e.g. lower bounds are bigger than upper bounds, an unknown algorithm was specified, etc.). "<<endl;
            break;
        case -1:
            cout<<" Generic failure code. "<<endl;
            break;
        case 1:
            cout<<" Generic success return value. "<<endl;
            break;
        case 2:
            cout<<" Optimization stopped because stopval was reached. "<<endl;
            break;
        case 3:
            cout<<" Optimization stopped because ftol_rel or ftol_abs was reached. "<<endl;
            break;
        case 4:
            cout<<" Optimization stopped because xtol_rel or xtol_abs was reached. "<<endl;
            break;
        case 5:
            cout<<" Optimization stopped because maxeval was reached. "<<endl;
            break;
        case 6:
            cout<<" Optimization stopped because maxtime was reached. "<<endl;
            break;
    }
}// end print_return_message
