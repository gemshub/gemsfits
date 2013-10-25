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
#include <fstream>

using namespace std;

ResPrint::ResPrint(string path_, optimization *opti)
{
    path = path_;
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
}


void ResPrint::print_header(string function, string weight_)
{
    ofstream fout;
    fout.open(path.c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; /*exit(1);*/ }

    if (weight_ == "")
        fout << "experiment,,,unit,measured,computed," <<function<<endl;
    else fout << "experiment,,,unit,measured,computed," <<"weighted_"+function<<endl;

}

void ResPrint::print_result()
{
    ofstream fout;
    fout.open(path.c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; /*exit(1);*/ }

    fout.setf(ios::fixed);
    for (unsigned int i=0; i<experiment.size(); ++i)
    {
        if (what1[i] == "")
        {
            fout<<experiment[i]<<","/*<<what1[i]*/<<","<<what2[i]<<","<<unit[i]<<","<<measured[i]<<","<<computed[i]<<","<<Weighted_Tfun_residual[i]<<endl;
        } else fout<<experiment[i]<<","<<what1[i]<<","<<what2[i]<<","<<unit[i]<<","<<measured[i]<<","<<computed[i]<<","<<Weighted_Tfun_residual[i]<<endl;
    }


    // call GEM_init to read GEMS3K input files
    TNode* node  = new TNode();

    // call GEM_init     --> read in input files
    if( (node->GEM_init( gpf->GEMS3LstFilePath().c_str() )) == 1 )
    {
        cout<<" .. ERROR occurred while reading input files !!! ..."<<endl;
    }

    for(int i=0; i< (int) res_opti->optv.size(); i++ ) // cols
    {
        // Print optimized parameter values to file
        if (res_opti->Ptype[i] =="G0")
        {
            fout <<" parameter G0 "<<node->xCH_to_DC_name(res_opti->Pindex[i])<<" : " << res_opti->optv[i] << endl;
//            cout << node->xCH_to_DC_name(res_opti->Pindex[i]) << endl;
        } else
        fout <<"parameter "<<res_opti->Ptype[i]<<" : " << res_opti->optv[i] << endl;

    }

    if (res_opti->h_RDc)
    {
        for (unsigned int i=0; i<res_opti->reactions.size(); ++i)
        {
            fout <<"Reac parameter "<<res_opti->reactions[i]->Dc_name<<" :  "<<res_opti->reactions[i]->std_gibbs<<endl;
        }
    }

    if (res_opti->h_Lp)
    {
        for (unsigned int i=0; i<res_opti->Lparams.size(); ++i)
        {
            fout <<"Linked parameter "<<res_opti->Lparams[i]->name<<" :  "<<res_opti->Lparams[i]->EV<<endl;
        }
    }

    fout<<endl;
}

//void ResPrint::print_result(string experiment, string what2, string unit, double measured, double computed, double residual)
//{
//    ofstream fout;
//    fout.open(path.c_str(), ios::app);
//    if( fout.fail() )
//    { cout<<"Output fileopen error"<<endl; /*exit(1);*/ }

//    fout<<experiment<<","/*<<what1*/<<","<<what2<<","<<unit<<","<<measured<<","<<computed<<","<<residual<<endl;
//}
