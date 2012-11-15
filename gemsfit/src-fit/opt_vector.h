/* 
*	 Copyright (C) 2012 by Ferdinand F. Hingerl (hingerl@hotmail.com)
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
 *	@file opt_vector.h
 *
 *	@brief this header file contains definitions of the Opt_Vector class, 
 *	which creates and hosts the optimization vector.   
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */

#ifndef _opt_vector_h_
#define _opt_vector_h_

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

/// The Opt_Vector creates and hosts the optimization vector.
// ============================ SS_Opt_Vector =========================== //

class SS_Opt_Vector
{
    public:
        /**
        * Constructor of the SS_Opt_Vector class.
        * Function creates and hosts the optimization vector of std state G0's.
        *
        * @param systems vector of pointers to instances of System_Properties structs
        * @author DM
        * @date 14.11.2012
        */
        SS_Opt_Vector( vector<SS_System_Properties*> systems );
        /**
        * Destructor of the Opt_Vector class.
        *
        * @author DM
        * @date 14.11.2012
        */
        virtual ~SS_Opt_Vector();

        /// vector containing optimization values
        vector<double> opt; // position 0 specie 0 etc.

        /// vector containing the first guesses for normalization of the opt vector
        vector<double> optv0;

        /// vector indicating which species is the respective position in the opt vector is referring to
        vector<string> fit_species;
//        vector<vector<string> >::iterator iter_spec1; vector<string>::iterator iter_spec2;
//        /// col of parameter which is at respective position in opt vector
//        vector<int> fit_param_col;
//        /// type of parameter which is at respective position in opt vector
//        vector<int> fit_param_type;

        /**
        * Function creates the opt, species and param_types vectors
        *
        * @param vect_of_sys_props vector of pointers to instances of System_Properties structs
        * @author DM
        * @date 14.11.2012
        */
        void make_opt_vector( vector<SS_System_Properties*> vect_of_sys_props );
};
// ============================ END SS_Opt_Vector =========================== //


class Opt_Vector
{
	public:
		/** 
		* Constructor of the Opt_Vector class. 
		* Function creates and hosts the optimization vector.  
		*
		* @param systems vector of pointers to instances of System_Properties structs
		* @author FFH
		* @date 09.04.2012
		*/	
		Opt_Vector( vector<System_Properties*> systems );
		/** 
		* Destructor of the Opt_Vector class. 
		*
		* @author FFH
		* @date 09.04.2012
		*/	
		virtual ~Opt_Vector();

		/// vector containing optimization values
		vector<double> opt;

		/// vector containing the first guesses for normalization of the opt vector	
		vector<double> optv0; 			

		/// vector indicating which species is the respective position in the opt vector is referring to
		vector<vector<string> > fit_species;  
		vector<vector<string> >::iterator iter_spec1; vector<string>::iterator iter_spec2;  
		/// col of parameter which is at respective position in opt vector
		vector<int> fit_param_col;
		/// type of parameter which is at respective position in opt vector
		vector<int> fit_param_type;

		/**  
		* Function creates the opt, species and param_types vectors  
		*
		* @param vect_of_sys_props vector of pointers to instances of System_Properties structs
		* @author FFH
		* @date 09.04.2012
		*/	
		void make_opt_vector( vector<System_Properties*> vect_of_sys_props );
};

#endif
