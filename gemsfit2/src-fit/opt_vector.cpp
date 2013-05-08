/// \file opt_vector.cpp
/// Implementation of reading adusted parameters and reaction constraints
//
// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT/>
//
// GEMSIFT is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//


#include "opt_vector.h"
#include "gemsfit_iofiles.h"

opti_vector::opti_vector( )
{
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

    get_gems_fit_DCH_txt( node, this ); // reading DCH parameters
    get_gems_fit_DBR_txt( node, this ); // reading DBR parameters
    get_gems_fit_multi_txt( node, this ); // reading multi parameters

    // getting indexes of components in the DCH

    if (h_RDc)
    {
        get_RDc_indexes (node, this);
    }

}


void opti_vector::get_RDc_indexes (TNode *node, opti_vector *ov)
{
    int index_species;
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
