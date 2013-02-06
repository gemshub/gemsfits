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
 *	@file gemsfit_global_functions.h
 *
 *	@brief this header file contains definitions of global functions needed during optimization. 
 *	Since the nlopt library needs a function pointer to the objective and constraint
 *  functions, these functions need to be globally accessible and can not be implemented 
 *  as member functions. This header lists mainly these implementations of the 
 *  respective callback functions.  
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */

#ifndef _gemsfit_global_functions_h_
#define _gemsfit_global_functions_h_

#include <vector>
#include <math.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <omp.h>


#include <unistd.h>    /* standard unix functions, like getpid()         */ 
#include <sys/types.h> /* various type definitions, like pid_t           */ 
#include <signal.h>    /* signal name macros, and the signal() prototype */ 


#ifdef USE_MPI
#include <mpi.h>
#endif

namespace dislin 
{ 
#include "dislin.h"
}

#include "node.h"
#include "system_properties.h"
//#include "opt_vector.h"
#include "optimization.h"

#include "gemsfit_global_variables.h"



namespace opti
{

	// Create arrays with continuous locations in memory
	int **alloc_2d_int(int rows, int cols);
	void free_2d_int (int **array_2d_int);

	double **alloc_2d_double(int rows, int cols);
	void free_2d_double (double **array_2d_double);

	float **alloc_2d_float(int rows, int cols);
	void free_2d_float (float **array_2d_float);


	// Create arrays with continuous locations in memory
	template < class T >
	T **alloc_2D_array( int rows, int cols )
	{
		T *data		= (T *)malloc(rows*cols*sizeof(int));
		T **array_2D	= (T **)malloc(rows*sizeof(int*));
		for( int i=0; i<rows; i++ )
		    array_2D[i] = &(data[cols*i]);
		return array_2D;
	}	

	template < class T >
	void free_2D_array( T **array_2D )
	{
	  free (array_2D[0]);
	  free (array_2D);
	}


	// Signal handler for brute force termination of GEMSFIT
	void catch_int( int sig_num );

    // global functions for StandardStateProp instance

        // callback function for GEMS3K wrapper
        /**
        * Callback function for the constraint functions referring to the GEMS3 solver. The arguments of this function must not be
        * modified since the nlopt library expects exactly this format.
        * @param opt             optimization vector
        * @param grad            vector holding function gradients (can be NULL)
        * @param obj_func_data   a general data object will holds all the data needed by the onbejctive function. The data will be cat to the necessary identity within the function.
        * @author DM
        * @date 16.11.2012
        */
        double StdStateEquil_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );

        // callback function for GEMS3K wrapper for fitting interaction parameters
        /**
        * Callback function for the constraint functions referring to the GEMS3 solver. The arguments of this function must not be
        * modified since the nlopt library expects exactly this format.
        * @param opt             optimization vector
        * @param grad            vector holding function gradients (can be NULL)
        * @param obj_func_data   a general data object will holds all the data needed by the onbejctive function. The data will be cat to the necessary identity within the function.
        * @author DM
        * @date 06.02.2013
        */
        double IntParamEquil_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );

        // GEMS3K wrapper
        /**
        * Call to the GEMS3K wrapper. The arguments of this function must not be
        * modified since the nlopt library expects exactly this format.
        * @param opt      optimization vector
        * @param residual sum of squared residuals being returned from the function
        * @param sys      data object that holds the data of the current System_Properties struct
        * @author DM
        * @date 19.11.2012
        */
        void StdState_gems3k_wrap( double &residual, const std::vector<double> &opt, SS_System_Properties *sys );


        // Constraint function wrapper
        /**
        * Callback function for the constraint functions referring to the TSolMod functions. The arguments of this function must not be
        * modified since the nlopt library expects exactly this format.
        * @param opt             optimization vector
        * @param grad            vector holding function gradients (can be NULL)
        * @param obj_func_data   a general data object will holds all the data needed by the obejctive function. The data will be cat to the necessary identity within the function.
        * @author DM
        * @date 16.11.2012
        */
        double StdState_constraint_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );

//======================================== END StandrardStateProp instance ========================================== //

        // global functions for ActivityModel instance

        // TSolMod and GEMS3K wrapper functions

			/** 
			* Callback function for TSolMod wrapper. The arguments of this function must not be
			* modified since the nlopt library expects exactly this format. 
			* @param opt             optimization vector
			* @param grad            vector holding function gradients (can be NULL)
			* @param obj_func_data   a general data object will holds all the data needed by the onbejctive function. The data will be cat to the necessary identity within the function. 
			* @author FFH
			* @date 09.04.2012
			*/	
			double ActMod_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );

			//	TSolMod wrapper
			/** 
			* Call to the TSolMod wrapper. The arguments of this function must not be
			* modified since the nlopt library expects exactly this format. 
			* @param opt      optimization vector
			* @param residual sum of squared residuals being returned from the function
			* @param sys      data object that holds the data of the current System_Properties struct 
			* @author FFH
			* @date 09.04.2012
			*/	
			void ActMod_tsolmod_wrap( double &residual, const std::vector<double> &opt, System_Properties* sys );

			// Constraint function wrapper
			/** 
			* Callback function for the constraint functions referring to the TSolMod functions. The arguments of this function must not be
			* modified since the nlopt library expects exactly this format. 
			* @param opt             optimization vector
			* @param grad            vector holding function gradients (can be NULL)
			* @param obj_func_data   a general data object will holds all the data needed by the onbejctive function. The data will be cat to the necessary identity within the function. 
			* @author FFH
			* @date 09.04.2012
			*/	
			double ActMod_constraint_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );

			// callback function for GEMS3K wrapper
			/** 
			* Callback function for the constraint functions referring to the GEMS3 solver. The arguments of this function must not be
			* modified since the nlopt library expects exactly this format. 
			* @param opt             optimization vector
			* @param grad            vector holding function gradients (can be NULL)
			* @param obj_func_data   a general data object will holds all the data needed by the onbejctive function. The data will be cat to the necessary identity within the function. 
			* @author FFH
			* @date 09.04.2012
			*/	
			double ActModEquil_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );

			// GEMS3K wrapper
			/** 
			* Call to the GEMS3K wrapper. The arguments of this function must not be
			* modified since the nlopt library expects exactly this format. 
			* @param opt      optimization vector
			* @param residual sum of squared residuals being returned from the function
			* @param sys      data object that holds the data of the current System_Properties struct 
			* @author FFH
			* @date 09.04.2012
			*/	
			void ActMod_gems3k_wrap( double &residual, const std::vector<double> &opt, System_Properties* sys );


			/** 
			* Compute osmotic coefficient contribution of a species using numerical derivationa and integration  of activity model 
			* @param sd             SolutionData object which is needed by TSolMod class of GEMS3K
			* @param aM				array containing the species concentrations
			* @param aZ				array containing the species charges
			* @param Rhow			array containing the water density and its derivatives with respect to temperature and pressure
			* @param Epsw			array containing the water permittivity and its derivatives with respect to temperature and pressure
			* @param node_pointer   pointer to node class
			* @author FFH
			* @date 16.05.2012
			*/	
			double Osm_coeff_Bjerrum( long int species_gamma_id, long int species_conc_id, SolutionData sd, double* aM, double* aZ, double* Rhow, double* Epsw );


		// Generic functions for computing osmotic coefficient or apparent molar volumes from activity coefficient models
			
			/** 
			* Compute forward finite difference of lnGam of electrolyte 'j' with respect to its molality 'm[j]' 
			* @param m_spec_current concentration of species j
			* @param species_id     index of species of interest
			* @param sd             SolutionData object which is needed by TSolMod class of GEMS3K
			* @param aM				array containing the species concentrations
			* @param aZ				array containing the species charges
			* @param Rhow			array containing the water density and its derivatives with respect to temperature and pressure
			* @param Epsw			array containing the water permittivity and its derivatives with respect to temperature and pressure
			* @param node_pointer   pointer to node class
			* @param StoicIons		array containing the stoichiometry of the investigated electrolyte	
			* @author FFH
			* @date 09.04.2012
			*/	
			double FinDiff( double m_spec_current, long int species_gamma_id, long int species_conc_id, SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw ); 	

			
			/** 
			* Compute apparent molar volume of electrolyte using numerical derivationa and integration  of activity model 
			* @param sd             SolutionData object which is needed by TSolMod class of GEMS3K
			* @param aM				array containing the species concentrations
			* @param aZ				array containing the species charges
			* @param Rhow			array containing the water density and its derivatives with respect to temperature and pressure
			* @param Epsw			array containing the water permittivity and its derivatives with respect to temperature and pressure
			* @param node_pointer   pointer to node class
			* @param StoicIons		array containing the stoichiometry of the investigated electrolyte	
			* @author FFH
			* @date 09.04.2012
			*/	
			double App_molar_volume( SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons );


			/** 
			* Compute apparent molar volume of electrolyte using analytical solution for bulk excess volume of solution 
			* @param sd             SolutionData object which is needed by TSolMod class of GEMS3K
			* @param aM				array containing the species concentrations
			* @param aZ				array containing the species charges
			* @param Rhow			array containing the water density and its derivatives with respect to temperature and pressure
			* @param Epsw			array containing the water permittivity and its derivatives with respect to temperature and pressure
			* @param node_pointer   pointer to node class
			* @param StoicIons		array containing the stoichiometry of the investigated electrolyte	
			* @author FFH
			* @date 09.04.2012
			*/				
			double Get_Vex_from_actmod( SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons );

			/** 
			* Finite differences of mean lnGam with respect to pressure 
			* @param m_j            concentration of species j
			* @param j              index of species of interest
			* @param sd             SolutionData object which is needed by TSolMod class of GEMS3K
			* @param aM				array containing the species concentrations
			* @param aZ				array containing the species charges
			* @param Rhow			array containing the water density and its derivatives with respect to temperature and pressure
			* @param Epsw			array containing the water permittivity and its derivatives with respect to temperature and pressure
			* @param node_pointer   pointer to node class
			* @param StoicIons		array containing the stoichiometry of the investigated electrolyte	
			* @author FFH
			* @date 09.04.2012
			*/	
			double FinDiffVol( double m_j, int j, SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons );

			/** 
			* Numerical integration routine (using trapezoidal rule) for apparent molar volume (integration with respect to pressure) or osmotic coefficients (integration with respect to electrolyte concentration). Numerical integration algorithm from from Numerical Recipes in C, 2nd Ed.
			* @param lower_bound    lower bound of pressure pressure/molality integration 
			* @param upper_bound    upper bound of pressure pressure/molality integration (equals system pressure or electrolyte concentration, repectively) 
			* @param n				number of trapezoidal points (set by calling function qsimp)
			* @param species        index of species of interest
			* @param select         select = 0: Finite differences over molality of electrolyte -> compute osmotic coefficient; select = 1: Finite differences over pressure -> compute apparent molar volume
			* @param sd             SolutionData object which is needed by TSolMod class of GEMS3K
			* @param aM				array containing the species concentrations
			* @param aZ				array containing the species charges
			* @param Rhow			array containing the water density and its derivatives with respect to temperature and pressure
			* @param Epsw			array containing the water permittivity and its derivatives with respect to temperature and pressure
			* @param node_pointer   pointer to node class
			* @param StoicIons		array containing the stoichiometry of the investigated electrolyte	
			* @author FFH
			* @date 09.04.2012
			*/	
			double trapzd( const double lower_bound, const double upper_bound, int& n, long int& species_gamma_id, long int& species_conc_id, int select, SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons ); 

			/** 
			* Numerical integration algorithm (Simpsons algorithm) from from Numerical Recipes in C, 2nd Ed.
			* @param lower_bound    lower bound of pressure pressure/molality integration 
			* @param upper_bound    upper bound of pressure pressure/molality integration (equals system pressure or electrolyte concentration, repectively) 
			* @param species        index of species of interest
			* @param select         select = 0: Finite differences over molality of electrolyte -> compute osmotic coefficient; select = 1: Finite differences over pressure -> compute apparent molar volume
			* @param sd             SolutionData object which is needed by TSolMod class of GEMS3K
			* @param aM				array containing the species concentrations
			* @param aZ				array containing the species charges
			* @param Rhow			array containing the water density and its derivatives with respect to temperature and pressure
			* @param Epsw			array containing the water permittivity and its derivatives with respect to temperature and pressure
			* @param node_pointer   pointer to node class
			* @param StoicIons		array containing the stoichiometry of the investigated electrolyte	
			* @author FFH
			* @date 09.04.2012
			*/	
			double qsimp( const double lower_bound, const double upper_bound, long int& species_gamma_id, long int& species_conc_id, int select, SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons ); 			

			/**
			* Barrier function to replace or complement constraint functions
			* @param NP_DC         number of parameters per dependent components
			* @param NPar          number of interaction parameters (rows of aIPc array)
			* @param MaxOrd        maximum order of interaction parameters
			* @param NPcoef        number of coefficients per interaction parameter
			* @param NComp         number of components
			* @param Tk			   system temperature
			* @param Pbar          system pressure
			* @param aDCc          array of component specific parameters (aDCc array)
			* @param aIPc          array of interaction parameters (aIPc array)
			* @param aIPx          array of indices of interaction parameter array (aIPx array)
			* @param Xw            mole fraction of water in the solution
			* 
			* @author FFH
			* @date 09.04.2012
			*/	
			bool barrier( int NP_DC, int NPar, int MaxOrd, int NPcoef, int NComp, double Tk, double Pbar, double* aDCc, double* aIPc, long int* aIPx, double Xw );


			// Compute Law-of-Mass-Action
			void LMA_basic( vector<double> CompMol, vector<double> MeasMol , SolutionData& sd, double* aM, double* aZ, double* RhoW, double* EpsW, vector<int> Stoic, vector<int> DC_id, double Ksp, int nH2O );
			
			// Law-of-Mass-Action callback function
			double LMA_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );



}// namespace opti 





#endif


