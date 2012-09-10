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
 *	@file system_properties.h
 *
 *	@brief this header file contains definitions of the System_Properties class, 
 *	which retrieves and stores data related to the chemical system setup.    
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */

#ifndef _system_properties_h_
#define _system_properties_h_

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

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find.hpp>

#include "node.h"
#include "data_manager.h"

using namespace std;
using namespace boost;

/// the System_Properties class retrieves and stores data related to the chemical system setup.
class System_Properties : public Data_Manager
{
	public:
		/** 
		* Constructor for the System_Properties class. 
		* Function reads parameters of the chemical system from GEMSFIT_input.dat.
		* Most of these parameters (such as parameter array dimensions) appear also in the GEMS3K input files and must therefore be identical. 
		* Examples for these parameters are the aIPc, aDCc and aIPx array. 
		*
		* @author FFH
		* @date 09.04.2012
		*/	
		System_Properties( );
		/** 
		* Destructor for the System_Properties class. 
		*
		* @author FFH
		* @date 09.04.2012
		*/	
		virtual ~System_Properties();
		
		typedef vector<double> vd;
		typedef vector<string> vs;
		typedef vector<int> vi;
		typedef vector<vector<int> > vvi;

		/// name of the chemical system
		string system_name;
		/// name of the chemical phase
		string phase_name;
		/// name of the measurement database
		string meas_db;
		/// name of species
		vs species;
		/// the GEMSFIT configuration file (fixed to GEMSFIT_input.dat)
		string param_file;
		// vector of charges of the dependent components
		vd charges;
		
		/// enumeration of possible activity models that can be fitted
		enum act_model
		{ 
		  ELVIS = 1, 
		  Pitzer, 
		  EUNIQUAC,
		  SIT
		};
		act_model activity_model;
	
		/// Printing Flag: if this flag is set to one, the result of the optimization will be printed to file (via optimization.cpp)
		bool printfile;

		/// Monte Carlo flag: if true, then the MPI commands within the objective function call will not be executed. Instead, the loop over Monte Carlo runs is parallelized
		bool MC_MPI;

		/// Computed values for Monte Carlo confidence interval generation
		vd computed_values_v;
		/// Measurement values for Monte Carlo confidence interval generation
		vd measured_values_v;

		/// Computed residuals
		vd computed_residuals_v;

		// Position of aIPc_fit and aDCc_fit parameters in opt vector
		vvi fit_indices; 
		vi fit_ind_opt; 
		vi fit_ind_sys;		


		// Pointer to Data_Manager class containing measurement data
		Data_Manager* data_meas;


		/// Struct with system specific measurement data
		struct parameters
		{
			/// number of columns of the interaction parameter array
			int cols_aIPc;
			/// number of rows of the interaction parameter array
			int rows_aIPc; 
			/// number of columns of the interaction parameter indices array
			int cols_aIPx;
			/// number of columns of the species-specific parameter array		
			int cols_aDCc;
			
			/// vector of interaction parameters
			vector<double> aIPc;
			/// vector of species-specific parameters
			vector<double> aDCc; 
			/// vector of interaction parameter indices vector
			vector<int> aIPx; 
			
			/// vectors of first guesses of aIPc arrays
			vector<double> aIPc_fit_val;
			/// vectors of first guesses of aDCc arrays
			vector<double> aDCc_fit_val;

			/// vectors contains the position indices of the values that will be fitted in the aIPc arrays
			vector<int> aIPc_fit_ind;
			/// vectors contains the position indices of the values that will be fitted in the aDCc arrays
			vector<int> aDCc_fit_ind;

			/// initial guess vector for normalization of bounds
			vd init_guesses;
			
			/// normalize parameters flag
			bool NormParams;

		};	
		parameters* sysparam;

		/** 
		* Function reads parameters of the chemical system
		*
		* @param sysparam		struct with system specific measurement data
		* @author FFH
		* @date 09.04.2012
		*/	
		void getparam( parameters* sysparam );

};

#endif
