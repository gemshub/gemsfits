/* 
*	 Copyright (C) 2012 by Ferdinand F. Hingerl (hingerl@hotmail.com)
*    Modified for fitting standard state properties (2012)
*    by G. Dan Miron (mirondanro@yahoo.com)
*
*	 This file is part of the thermodynamic fitting program GEMSFIT.
*
*    GEMSFIT is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    GEMSFIT is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU  General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with GEMSFIT.  If not, see <http://www.gnu.org/licenses/>.
*/


/**
 *	@file opt_vector.cpp
 *
 *	@brief this header file contains the implementations of the Opt_Vector class, 
 *	which creates and hosts the optimization vector.   
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */

#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <limits>
#include <iomanip>

using namespace std;

#include "system_properties.h"
#include "opt_vector.h"

// ============================ SS_Opt_Vector =========================== //

SS_Opt_Vector::SS_Opt_Vector( vector<SS_System_Properties*> vect_of_sys_props )
{
    make_opt_vector( vect_of_sys_props );
}

void SS_Opt_Vector::make_opt_vector( vector<SS_System_Properties*> vect_of_sys_props )
{

    unsigned int i,j;

    // Loop over systems - curetnly only one system
    for( i=0; i<vect_of_sys_props.size(); i++ )
    {
        cout<<" ... now in make_opt_vector"<<endl;

        if( i==0 )
        {
            // Loop over all species of each system
            for( j=0; j<vect_of_sys_props[i]->fit_species_ind.size(); j++ )
            {
                // store first guesses in opt vector
                opt.push_back(vect_of_sys_props[i]->sysprop->std_gibbs[j]);
                fit_species.push_back(vect_of_sys_props[i]->to_fit_species[j]);
            };

        } // all other systems
        else
        {
            // add only parameters to the optimization vector that do not show up in other systems

        };
    };

    //cout<<"opt.size() = "<<opt.size()<<endl;

    // Load initial guess vector, which will be used for normalization of the opt vector
    optv0 = opt;
    vect_of_sys_props[0]->sysprop->init_guesses = opt;

}


// Destructor
SS_Opt_Vector::~SS_Opt_Vector( )
{
    //
}

// ============================ END SS_Opt_Vector =========================== //

Opt_Vector::Opt_Vector( vector<System_Properties*> vect_of_sys_props )
{
	make_opt_vector( vect_of_sys_props );
}

void Opt_Vector::make_opt_vector( vector<System_Properties*> vect_of_sys_props )
{	
	
	unsigned int i,j;
	int row_in_aIPx, index_species1, index_species2, row;
	vector<string> spec1, spec2;
	// Loop over systems
	for( i=0; i<vect_of_sys_props.size(); i++ )
	{
		cout<<" ... now in make_opt_vector"<<endl;
	
		if( i==0 )
		{
			// Loop over aIPc_fit_* of each system
			for( j=0; j<vect_of_sys_props[i]->sysparam->aIPc_fit_val.size(); j++ )
			{
				// store first guesses of aIPc arrays in opt vector
				opt.push_back(vect_of_sys_props[i]->sysparam->aIPc_fit_val[j]);

				// store col of parameter which is at position j in opt vector
				fit_param_col.push_back( vect_of_sys_props[i]->sysparam->aIPc_fit_ind[j] % vect_of_sys_props[i]->sysparam->cols_aIPc );
				// store type of parameter which is at position j in opt vector
				row  = vect_of_sys_props[i]->sysparam->aIPc_fit_ind[j] / vect_of_sys_props[i]->sysparam->cols_aIPc;
				fit_param_type.push_back( vect_of_sys_props[i]->sysparam->aIPc[ (row * vect_of_sys_props[i]->sysparam->cols_aIPc) ] );	

				// store name of species that are position j in opt vector 
				row_in_aIPx = vect_of_sys_props[i]->sysparam->cols_aIPx * ( vect_of_sys_props[i]->sysparam->aIPc_fit_ind[j] / vect_of_sys_props[i]->sysparam->cols_aIPc );
				index_species1 = vect_of_sys_props[i]->sysparam->aIPx[row_in_aIPx];			
				index_species2 = vect_of_sys_props[i]->sysparam->aIPx[row_in_aIPx+1];									
				spec1.push_back(vect_of_sys_props[i]->species[index_species1]);
				spec2.push_back(vect_of_sys_props[i]->species[index_species2]);
			
				// current position j in opt vector equals to fitting value number k of system (counting from aIPc_fit_* through aDCc_fit_* values) 
				vect_of_sys_props[i]->fit_ind_opt.push_back(opt.size()-1);	// position in opt_vector 
				vect_of_sys_props[i]->fit_ind_sys.push_back(j);	// position in system specific aIPc_fit_* vector	
					
			};

			// Loop over aDCc_fit_* of each system
			for( j=0; j<vect_of_sys_props[i]->sysparam->aDCc_fit_val.size(); j++ )
			{
				// store first guesses of aIPc arrays in opt vector
				opt.push_back(vect_of_sys_props[i]->sysparam->aDCc_fit_val[j]); 

				// store col of parameter which is at position j in opt vector
				fit_param_col.push_back( vect_of_sys_props[i]->sysparam->aDCc_fit_ind[j] % vect_of_sys_props[i]->sysparam->cols_aDCc); 
				
				// store type of parameter which is at position j in opt vector
			    fit_param_type.push_back( 3 );	

				// store name of species that are position j in opt vector 
				index_species1 = vect_of_sys_props[i]->sysparam->aDCc_fit_ind[j] / vect_of_sys_props[i]->sysparam->cols_aDCc;
				spec1.push_back(vect_of_sys_props[i]->species[index_species1]);
				spec2.push_back(" -- ");

				// current position j in opt vector equals to fitting value number k of system (counting from aIPc_fit_* through aDCc_fit_* values) 
				vect_of_sys_props[i]->fit_ind_opt.push_back(opt.size()-1);	// position in opt_vector 
				vect_of_sys_props[i]->fit_ind_sys.push_back(vect_of_sys_props[i]->sysparam->aIPc_fit_val.size()+j);	// position in system specific aIPc_fit_* vector	
				
			};
		
		} // all other systems
		else
		{
			// add only parameters to the optimization vector that do not show up in other systems


		};	
		// Array that describes which position in the opt_vector describes which parameter in the system ()  	
		vect_of_sys_props[i]->fit_indices.push_back(vect_of_sys_props[i]->fit_ind_opt);
		vect_of_sys_props[i]->fit_indices.push_back(vect_of_sys_props[i]->fit_ind_sys);
	};
	// Array containing the species involved in opt vector
	fit_species.push_back(spec1);
	fit_species.push_back(spec2);

	//cout<<"opt.size() = "<<opt.size()<<endl;
	
	// Load initial guess vector, which will be used for normalization of the opt vector
	optv0 = opt;
	vect_of_sys_props[0]->sysparam->init_guesses = opt;
	 						
}


// Destructor
Opt_Vector::~Opt_Vector( )
{
	//
}
