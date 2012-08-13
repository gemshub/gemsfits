#ifndef _gemsfit_global_functions_h_
#define _gemsfit_global_functions_h_

#include <vector>
#include <math.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>

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


	
	// global functions for ActivityModel instance

		// TSolMod and GEMS3K wrapper functions		
			//	callback function for TSolMod wrapper
			double ActMod_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );

			//	TSolMod wrapper
			void ActMod_tsolmod_wrap( double &residual, const std::vector<double> &opt, System_Properties* sys );

			// Constraint function wrapper
			double ActMod_constraint_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );

			// callback function for GEMS3K wrapper
			double ActModEquil_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );

			// GEMS3K wrapper
			void ActMod_gems3k_wrap( double &residual, const std::vector<double> &opt, System_Properties* sys );


		// Generic functions for computing osmotic coefficient or apparent molar volumes from activity coefficient models
			// Osmotic coefficient
//			double Int_OsmCoeff();

			// Finite Difference of lnGam of electrolyte 'j' with respect to its molality 'm[j]';
			double FinDiff( double m_j, int j, SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons ); 	

			// Compute apparent molar volume of electrolyte using numerical derivationa and integration  of activity model
			double App_molar_volume( SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons );

			// Compute apparent molar volume of electrolyte using analytical solution for bulk excess volume of solution
			double Get_Vex_from_actmod( SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons );

			// Finite differences of mean lnGam with respect to pressure
			double FinDiffVol( double m_j, int j, SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons );


			// Numerical integration algorithm from from Numerical Recipes in C, 2nd Ed.
			double trapzd( const double lower_bound, const double upper_bound, int& n, long int& species, int select, SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons ); 
			double qsimp( const double lower_bound, const double upper_bound, long int& species, int select, SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons ); 			


			bool barrier( int NP_DC, int NPar, int MaxOrd, int NPcoef, int NComp, double Tk, double Pbar, double* aDCc, double* aIPc, long int* aIPx, double Xw );



}// namespace opti 





#endif


