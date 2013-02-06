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
 *	@file system_properties.h
 *
 *	@brief this header file contains definitions of the System_Properties and SS_System_Properties class,
 *	which retrieves and stores data related to the chemical system setup.    
 *
 *	@author Ferdinand F. Hingerl, G. Dan Miron
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

//#include <boost/algorithm/string.hpp>
//#include <boost/algorithm/string/find.hpp>

#include "node.h"
#include "data_manager.h"

using namespace std;
//using namespace boost;

/// DM the (Standard State) SS_System_Properties class retrieves and stores data related to the chemical system setup.
class SS_System_Properties /*: public SS_Data_Manager*/
{
    public:
    /**
    * Constructor for the SS_System_Properties class.
    * Function reads parameters of the chemical system from SS_GEMSFIT_input.dat.
    *
    * @author DM
    * @date 30.10.2012
    */
    SS_System_Properties( );
    /**
    * Destructor for the SS_System_Properties class.
    *
    * @author DM
    * @date 30.10.2012
    */
    virtual ~SS_System_Properties( );

    typedef vector<double> vd;        // vector of double
    typedef vector<vector<double> > vvd;  // 2D vector of double
    typedef vector<string> vs;        // vector of string
    typedef vector<int> vi;           // vecotr of integer
    typedef vector<vector<int> > vvi; // 2D vector of integer

    /// name of the chemical system e.g. qtz_h2o_nacl-dat.lst (name of lst file exported by GEMS3K)
    string system_name;
    /// name of the measurement database
    string meas_db;
    /// the GEMSFIT configuration file (fixed to SS_GEMSFIT_input.dat)
    string param_file;

    /// 0 - not use weights; 1 - use weights // olny 1/sigma implemented !!! not working for solid solution
    bool weight_error;
    /// use solid solution measured composition
    bool use_solidsolution;
    /// 0 - fit interaction parameters; 1 - fit G0
    bool fit_std;
    /// 0 - molality; 1 - log_molality
    bool log_solubility; // in which form the solubility is compared


    /// name of species to be fitted.
    vs to_fit_species; // Dependent components in GEMS3K
    vi fit_species_ind; // indices of the fitted species in GEMS3K - read from node class after reading the experimental data.

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


    // Position of G0_fit parameters in opt vector
    vvi fit_indices;
    vi fit_ind_opt;
    vi fit_ind_sys;


    // Pointer to SS_Data_Manager class containing measurement data
    SS_Data_Manager* data_meas;

    /// Structure that holds the standard theromdynamic properties of the species to be fitted
    // implemented only for Gibbs energy
    struct std_prop
    {
        vd  std_gibbs;   // standard state Gibbs free energy of the species to be fitted at T=25 C and P=1 bar
        vvd std_gibbsTP; // standard state Gibbs free energy of the species to be fitted at T and P of the experiemnts [specie][TPpair]
        vvd dif_gibbs;   // difference dif_gibbs = std_gibbsTP-std_gibbs. This difference is set once after rertiving std_gibbs and std_gibbsTP form GEMS3K [specie][TPpair];

        /// normalize parameters flag
        bool NormParams;
        /// initial guess vector for normalization of bounds
        vd init_guesses;

    };
    std_prop* sysprop; // Pointer to std_prop struct that holds the stdandard state properties of the species to be fitted

    /**
    * Function reads standard state properties of the chemical system form GEMS3K input files
    *
    * @param sysprop		Structure that holds the standard theromdynamic properties of the species to be fitted
    * @author DM
    * @date 30.10.2012
    */
    void getsysprop( std_prop* sysprop );

    ///////////////// for fiting interaction parameters ////////////////////////

    /// name of the chemical phase
    string phase_name;
    /// name of species
    vs species;
    /// vector of charges of the dependent components
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
