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
 *	@file print.cpp
 *
 *	@brief this header file contains implementetions of print class functions for printing the optimization results
 *
 *	@author G. Dan Miron
 *
 * 	@date 17.05.2013
 *
 */

#include "print.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <math.h>

/*
using namespace std;

ResPrint::ResPrint( optimization *opti)
{
    res_opti = opti;
}


void ResPrint::print_clear()
{
    experiment.clear();
    what1.clear();
    what2.clear();
    unit.clear();
    measured.clear();
    computed.clear();
    Weighted_Tfun_residual.clear();
    residuals_v.clear();

}

void ResPrint::set_print(string experiment_, string what1_, string what2_, string unit_, double measured_, double computed_, double Weighted_Tfun_residual_, double weight_)
{
    experiment.push_back(experiment_);
    what1.push_back(what1_);
    what2.push_back(what2_);
    unit.push_back(unit_);
    measured.push_back(measured_);
    computed.push_back(computed_);
    Weighted_Tfun_residual.push_back(Weighted_Tfun_residual_);
    residuals_v.push_back(measured_-computed_);
    weight.push_back(weight_);
}


void ResPrint::print_header(string function, string weight_, int size)
{
    stringstream ss;
    string sss;

    if (weight_ == "")
        gpf->fres << "experiment,,,unit,measured,computed," <<function;
    else gpf->fres << "experiment,,,unit,measured,computed," <<"weighted_"+function;
    for ( int i =0; i<size; ++i)
    {
        ss << i;
        sss = ss.str();
        gpf->fres <<",Sens_p"+sss;
        ss.str("");
    }
    gpf->fres << endl;

}

void ResPrint::print_result()
{
    double sum_res = 0.0, sum_res_wtfun = 0.0; // , sum_meas = 0.0;

    setprecision(12);
    scientific(gpf->fres);


    gpf->fres.setf(ios::fixed);
    for (unsigned int i=0; i<experiment.size(); ++i)
    {
        if (what1[i] == "")
        {
            gpf->fres<<experiment[i]<<","/*<<what1[i]*<<","<<what2[i]<<","<<unit[i]<<","<<measured[i]<<","<<computed[i]<<","<<Weighted_Tfun_residual[i];
        } else gpf->fres<<experiment[i]<<","<<what1[i]<<","<<what2[i]<<","<<unit[i]<<","<<measured[i]<<","<<computed[i]<<","<<Weighted_Tfun_residual[i];
        sum_res += (measured[i] - computed[i]);
        sum_res_wtfun +=Weighted_Tfun_residual[i];
        for (unsigned int j=0; j<sensitivity.size(); j++)
        {
            double abss = abs(sensitivity[j][i]);
            gpf->fres <<","<<log10(abss);
        }
        gpf->fres<<endl;
    }

    gpf->fres<< "sum of residuals = " << sum_res << endl;
    gpf->fres<< "minimized function value = " << sum_res_wtfun << endl;

//    double test2 = sum_res / residuals_v.size();
    // call GEM_init to read GEMS3K input files
    TNode* node  = new TNode();

    // call GEM_init     --> read in input files
    if( (node->GEM_init( gpf->GEMS3LstFilePath().c_str() )) == 1 )
    {
        cout<< gpf->GEMS3LstFilePath().c_str() << endl;
        cout<<" .. ERROR occurred while reading input files !!! ..."<<endl;
    }

    for(int i=0; i< (int) res_opti->optv.size(); i++ ) // cols
    {
        // Print optimized parameter values to file
        if (res_opti->Ptype[i] =="G0")
        {
            gpf->fres <<" parameter G0 "<<node->xCH_to_DC_name(res_opti->Pindex[i])<<" : " << res_opti->optv[i] << endl;
//            cout << node->xCH_to_DC_name(res_opti->Pindex[i]) << endl;
        } else
        gpf->fres <<"parameter "<<res_opti->Ptype[i]<<" : " << res_opti->optv[i] << endl;

    }

    if (res_opti->h_RDc)
    {
        for (unsigned int i=0; i<res_opti->reactions.size(); ++i)
        {
            gpf->fres <<"Reac parameter "<<res_opti->reactions[i]->Dc_name<<" :  "<<res_opti->reactions[i]->std_gibbs<<endl;
        }
    }

    if (res_opti->h_Lp)
    {
        for (unsigned int i=0; i<res_opti->Lparams.size(); ++i)
        {
            gpf->fres <<"Linked parameter "<<res_opti->Lparams[i]->name<<" :  "<<res_opti->Lparams[i]->EV<<endl;
        }
    }

    gpf->fres<<endl;
}
*/

//void ResPrint::print_result(string experiment, string what2, string unit, double measured, double computed, double residual)
//{
//    ofstream fout;
//    fout.open(path.c_str(), ios::app);
//    if( fout.fail() )
//    { cout<<"Output fileopen error"<<endl; /*exit(1);*/ }

//    fout<<experiment<<","/*<<what1*/<<","<<what2<<","<<unit<<","<<measured<<","<<computed<<","<<residual<<endl;
//}
