/// \file opt_vector.cpp
/// Implementation of reading adusted parameters and reaction constraints
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


#include "opt_vector.h"
#include "gemsfit_iofiles.h"

opti_vector::opti_vector( )
{
    // GEMSFIT logfile
    ofstream fout;
    fout.open(gpf->FITLogFile().c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    // call GEM_init to read GEMS3K input files
    TNode* node  = new TNode();
    // call GEM_init     --> read in input files
    if( (node->GEM_init( gpf->GEMS3LstFilePath().c_str() )) == 1 )
       {
            cout << gpf->GEMS3LstFilePath().c_str() << endl;
            cout<<" .. ERROR occurred while reading GEMS3K input files !!! ..."<<endl;
            exit(1);
       }
    this->h_RDc = false;
    this->h_Lp  = false;

    fout << "10. opt_vector.cpp line 49. Reading optimized parameters from the input file; " << endl;
    get_gems_fit_DCH_txt( node, this ); // reading DCH parameters
    get_gems_fit_DBR_txt( node, this ); // reading DBR parameters
    get_gems_fit_multi_txt( node, this ); // reading multi parameters

    // getting indexes of components in the DCH
    if (h_RDc)
    {
        fout << "10-1. opt_vector.cpp line 58. Getting the indexes of species involved in the reaction constraints; " << endl;
        get_RDc_indexes (node, this);
    }

    // getting indexes of liked paramater/ elements
    if (h_Lp)
    {
        get_Lp_indexes (node, this);
    }

}


void opti_vector::get_RDc_indexes (TNode *node, opti_vector *ov)
{
    int index_species;
    // maybe try to add MPI here??
    for (int j = 0; j < ov->reactions.size(); ++j )
    {
        index_species = node->DC_name_to_xCH( ov->reactions[j]->Dc_name.c_str() );
        if( index_species < 0 )
        {
            throw index_species;
        }
        else
        {
            ov->reactions[j]->DcIndex = index_species;
        }

        for (int i=0; i<ov->reactions[j]->rdc_species.size(); ++i )
        {
            // Get form GEMS the index of to_fit_species of interest
            try
            {
                index_species = node->DC_name_to_xCH( ov->reactions[j]->rdc_species[i].c_str() );
                if( index_species < 0 )
                {
                    throw index_species;
                    cout << "Error: Name of species: "<< ov->reactions[j]->rdc_species[i].c_str() << " doesn't exist in the database!"<<endl;
                }
                else
                {
                    ov->reactions[j]->rdc_species_ind.push_back(index_species);
                }
            }
            catch( long e )
            {
                cout<<endl;
                cout<<" Phase name in GEMSFIT chemical system file has no corresponding phase name in GEMS3K input file !!!! "<<endl;
                cout<<" Can not proceed ... Bailing out now ... "<<endl;
                cout<<endl;
                exit(1);
            }
        }
    }
}

void opti_vector::get_Lp_indexes (TNode *node, opti_vector *ov)
{
    int index_IC;
    // maybe try to add MPI here??
    for (int j = 0; j < ov->Lparams.size(); ++j )
    {
        index_IC = node->IC_name_to_xCH( ov->Lparams[j]->name.c_str() );
        if( index_IC < 0 )
        {
            throw index_IC;
        }
        else
        {
            ov->Lparams[j]->index = index_IC;
        }

        for (int i=0; i<ov->Lparams[j]->L_param.size(); ++i )
        {
            // Get form GEMS the index of to_fit_species of interest
            try
            {
                index_IC = node->IC_name_to_xCH( ov->Lparams[j]->L_param[i].c_str() );
                if( index_IC < 0 )
                {
                    throw index_IC;
                }
                else
                {
                    ov->Lparams[j]->L_param_ind.push_back(index_IC);
                }
            }
            catch( long e )
            {
                cout<<endl;
                cout<<" Phase name in GEMSFIT chemical system file has no corresponding phase name in GEMS3K input file !!!! "<<endl;
                cout<<" Can not proceed ... Bailing out now ... "<<endl;
                cout<<endl;
                exit(1);
            }
        }
    }
}
