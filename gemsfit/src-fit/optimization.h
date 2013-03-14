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
 *	@file optimization.h
 *
 *	@brief this header file contains definitions of the Optimization class as well as the derived ActivityModel class. 
 * Member funtions read configuration parameters needed by the optimization algorithm from the GEMSFIT input file.     
 * This file contains all function declarations needed for the optimization such as assignment of bounds and constraints, initialization of the nlopt object as well as thje functions dealing with the actual parameter optimization.    
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */


#ifndef _optimization_h_
#define _optimization_h_

#include <vector>
#include <nlopt.hpp>
#include <math.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>

#ifdef USE_MPI
#include <mpi.h>
#endif

#ifdef USE_BOOST_MPI
#include <boost/mpi.hpp>
namespace bmpi = boost::mpi;
#endif

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "system_properties.h"
#include "opt_vector.h"
#include "plot_class.h"
#include "gemsfit_global_variables.h"


namespace opti
{
	/// the Optimization class provides a generic framework for initializiating and performing parameter optimization with the nlopt library.  
	class Optimization : public PlotFit
	{
		public:
			/**
			* Constructor of the Optimization class. Create pointer to constraint_data and PlotFit object. Read parameters needed by nlopt library from GEMSFIT configuration file.
			* @param vec_opt       optimization vector
			* @author FFH
			* @date 09.04.2012
			*/				
			Optimization( const vector<double> vec_opt );
	
			/**
			* Destructor of the Optimization class. Delete pointer to constraint_data and PlotFit object.
			* @author FFH
			* @date 09.04.2012
			*/				
			virtual ~Optimization();

			/// instance of the nlopt optmization object
			nlopt::opt opt_actmod;

			// configuration data for nlopt instance (get from GEMSFIT_input.dat)
			/// name of the optimizaiton algorithm
			string OptAlgo;
			/// number of computing threads ofr parallel computation
			int OptThreads;
			/// stopping criteria: relative tolerance on function value
			double OptTolRel;
			/// stopping criteria: absolute tolerance on function value.
			double OptTolAbs;
			/// stopping criteria: maximuma number of evaluations
			int OptMaxEval; 		
			/// 
			int OptConstraints;
			/// perform optimization and statistics (0), only optimization with basic Statistics (1), only Statistics (2) with initial guesses as best fit parametters
			int OptDoWhat;
			/// run hybrid optimization yes (1) / no (0)
			int OptHybridMode;
			/// hybrid stopping criterion -> specify relative tolerance (default = 1e-04) of function value
			double OptHybridTolRel;
			/// hybrid stopping criterion -> specify absolute tolerance (default = 1e-04) of function value
			double OptHybridTolAbs;
			/// max number of hybrid evaluations
			int OptHybridMaxEval; 		
			/// number of multistart runs (deactivate with 0)
			int OptNmultistart;
			/// hybrid algorithm GN_ISRES, LN_COBYLA or LN_BOBYQA
			string OptHybridAlgo;
			/// initial stepsize for local minimizers (factor will be multiplied to all optimization parameters); 0 => use default
			double OptInitStep;
			/// initial stepsize in percent (this percentage will be applied to all parameters)
			double OptScaleParam;
			/// normalize bounds/constraints/fitting parameters with initial guess vector => yes=1, no=0
			bool OptNormParam;
			/// percentage deviation used to generate bounds from initial guess vector
			double OptBoundPerc;

			// optimization vector and bounds (get from GEMSFIT_input.dat)
			/// optimization vector
			std::vector<double> optv;
			/// vector of upper bounds 
			std::vector<double> OptUpBounds;
			/// vector of lower bounds
			std::vector<double> OptLoBounds;

			// printing information (get from GEMSFIT_input.dat)
			PlotFit* Plot_ActMod;

			/// struct holding constraint values (retrieved from GEMSFIT_input.dat)
			typedef struct 
			{   
				/// index of constraint
			    int id;
				/// value of constraint
		    	double Constraints;
			} my_constraint_data;
			my_constraint_data* constraint_data;
			std::vector<my_constraint_data> constraint_data_v;

			/**
			* populate nlopt instance: set bounds, constraints, stopping criteria
			*
			* @author FFH
			* @date 09.04.2012
			*/	
			virtual void set_nlopt_param( );			

			/**
			* normalize init vector, bounds and constraints 		
			*
			* @param initguesses         vector containing initial guesses
			* @author FFH
			* @date 09.04.2012
			*/	
			virtual void normalize_params( const vector<double> initguesses );

			/**
			* print message associated with nlopt return code
			*
			* @param result              nlopt return code
			* @author FFH
			* @date 09.04.2012
			*/	
			virtual void print_return_message( const int result );

			/**
			* implementation of the constraint function
			*
			* @param x					optimization vector
			* @param grad				gradient vector (can be NULL)
			* @param data				general data that are needed by the constraint function
			* @author FFH
			* @date 09.04.2012
			*/	
			virtual double constraint_function(const std::vector<double> &x, std::vector<double> &grad, void *data);

			/**
			* initialize optimization object and assign constraints and bounds.
			* performs the optimization.
			* @param opt_actmod			nlopt optimization object
			* @param optv_              optimization vector
			* @param systems            data object that holds the data of the current System_Properties struct 
			* @param countit			global counter of runs over the measurement data 
			* @param sum_of_squares		sum of squared residuals
			* @author FFH
			* @date 09.04.2012
			*/				
			virtual void build_optim( nlopt::opt &opt_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares );
		
			/**
			* initialize hybrid optimization object and assign constraints and bounds.
			* performs the optimization.
			* @param opt_hybrid_actmod	nlopt hybrid optimization object
			* @param optv_              optimization vector
			* @param systems            data object that holds the data of the current System_Properties struct 
			* @param countit			global counter of runs over the measurement data 
			* @param sum_of_squares		sum of squared residuals
			* @author FFH
			* @date 09.04.2012
			*/				
			virtual void build_hybrid_optim( nlopt::opt &opt_hybrid_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares );

			/**
			* initialize optimization object and call build_optim(...)
			*
			* @param optv_              optimization vector
			* @param sys                data object that holds the data of the current System_Properties struct 
			* @param countit			global counter of runs over the measurement data 
			* @param sum_of_squares		sum of squared residuals
			* @author FFH
			* @date 09.04.2012
			*/				
            virtual void init_optim( std::vector<double> &optv_, std::vector<System_Properties*> *sys, int &countit, double &sum_of_squares );

			/**
			* initialize multistart optimization object, generate several initial guesses and call build_optim(...).
			*
			* @param optv_              optimization vector
			* @param sys                data object that holds the data of the current System_Properties struct 
			* @param countit			global counter of runs over the measurement data 
			* @param sum_of_squares		sum of squared residuals
			* @author FFH
			* @date 09.04.2012
			*/				
            virtual void init_optim_multistart( std::vector<double> &optv_, std::vector<System_Properties*> *sys, int &countit, double &sum_of_squares );

			/**
			* print results of the optimization to file
			*
			* @param optv_              optimization vector
			* @param systems            data object that holds the data of the current System_Properties struct 
			* @author FFH
			* @date 09.04.2012
			*/				
			virtual void print_results( std::vector<double> &optv_, std::vector<System_Properties*> *systems );

	};

    // ============================ StdStateProp =========================== //

    /// the StdStateProp class provides a generic framework for initializiating and performing std G0 (at 25 deg C and 1 bar) optimization with the nlopt library.
    class StdStateProp : public Optimization
    {
    public:
        // opt vector
        vector<double> opt_vec;

        // Constructor
        StdStateProp( const vector<double> vec_opt );

        // Destructor
        virtual ~StdStateProp();

        // nlopt instance
        nlopt::opt stdstate;

        // configuration data for nlopt instance (get from SS_GEMSFIT_input.dat)
        /// name of the optimization algorithm from NLOPT library
        string OptAlgo;
        /// number of threads for parallel execution
        int OptThreads;
        ///
        double OptTolRel;
        double OptTolAbs;
        int OptMaxEval;
        int OptConstraints;
        int OptDoWhat;
        int OptHybridMode;
        double OptHybridTolRel;
        double OptHybridTolAbs;
        int OptHybridMaxEval;
        int OptNmultistart;
        string OptHybridAlgo;
        double OptInitStep;
        double OptScaleParam;
        bool OptNormParam;
        double OptBoundPerc;

        vector<double> OptStatOnlyBestFitParam;
        double OptStatOnlySSR;
        bool OptEquil;

        bool test;

        // optimization vector and bounds (get from SS_GEMSFIT_input.dat)
        std::vector<double> optv;
        std::vector<double> OptUpBounds;
        std::vector<double> OptLoBounds;

        // printing information (get from SS_GEMSFIT_input.dat)
//        PlotFit* Plot_ActMod;


        /// struct holding constraint values (retrieved from SS_GEMSFIT_input.dat)
        typedef struct
        {
            int id;
            double Constraints;
        } my_constraint_data;
        my_constraint_data* constraint_data;
        vector<my_constraint_data> constraint_data_v;

        // Populate nlopt instance
        virtual void set_nlopt_param( );
        virtual void set_nlopt_param_txt( );
        virtual void out_nlopt_param_txt(  bool with_comments, bool brief_mode );
        virtual void define_nlopt_param( );

        // Normalize init vector, bounds and constraints
        virtual void normalize_params( const vector<double> initguesses );

        // NLopt return codes
        virtual void print_return_message( const int result );

        // Constraint function
        virtual double constraint_function(const std::vector<double> &x, std::vector<double> &grad, void *data);

        // Initialize optimization object and assign constraints and bounds
        virtual void build_optim( nlopt::opt &stdstate, std::vector<double> &optv_, std::vector<SS_System_Properties*> *systems, int &countit, double &sum_of_squares );

        // Initialize hybrid optimization object and assign constraints and bounds
        virtual void build_hybrid_optim( nlopt::opt &opt_hybrid_actmod, std::vector<double> &optv_, std::vector<SS_System_Properties*> *systems, int &countit, double &sum_of_squares );

        // initialize optimization
        virtual void init_optim( std::vector<double> &optv_, std::vector<SS_System_Properties*> *sys, int &countit, double &sum_of_squares );

//      // Initialize multistart optimization object and assign constraints and bounds
//      virtual void init_optim_multistart( std::vector<double> &optv_, std::vector<SS_System_Properties*> *sys, int &countit, double &sum_of_squares );

        // Print results to file
//      virtual void print_results( std::vector<double> &optv_, std::vector<SS_System_Properties*> *sys );
    };


    // ============================ END StdStateProp =========================== //

	/// the ActivityModel class provides a generic framework for initializiating and performing parameter optimization of TSolMod derived classes with the nlopt library.  
	class ActivityModel : public Optimization 
	{
		public:
			// opt vector			
			vector<double> opt_vec;

			// Constructor
			ActivityModel( const vector<double> vec_opt ); 

			// Destructor
			virtual ~ActivityModel();

			// nlopt instance
			nlopt::opt opt_actmod;

			// configuration data for nlopt instance (get from GEMSFIT_input.dat)
			/// name of the optimization algorithm from NLOPT library
			string OptAlgo;
			/// number of threads for parallel execution
			int OptThreads;
			/// 
			double OptTolRel;
			double OptTolAbs;
			int OptMaxEval; 		
			int OptConstraints;
			int OptDoWhat;
			int OptHybridMode;
			double OptHybridTolRel;
			double OptHybridTolAbs;
			int OptHybridMaxEval; 		
			int OptNmultistart;
			string OptHybridAlgo;
			double OptInitStep;
			double OptScaleParam;
			bool OptNormParam;
			double OptBoundPerc;
			
			vector<double> OptStatOnlyBestFitParam;
			double OptStatOnlySSR;
			bool OptEquil;

			bool test;

			// optimization vector and bounds (get from GEMSFIT_input.dat)
			std::vector<double> optv; 
			std::vector<double> OptUpBounds;
			std::vector<double> OptLoBounds;

			// printing information (get from GEMSFIT_input.dat)
			PlotFit* Plot_ActMod;


			/// struct holding constraint values (retrieved from GEMSFIT_input.dat)
			typedef struct 
			{    
			    int id;
		    	double Constraints;
			} my_constraint_data;
			my_constraint_data* constraint_data;
			vector<my_constraint_data> constraint_data_v;

			// Populate nlopt instance
			virtual void set_nlopt_param( );			

			// Normalize init vector, bounds and constraints
			virtual void normalize_params( const vector<double> initguesses );

			// NLopt return codes
			virtual void print_return_message( const int result );

			// Constraint function
			virtual double constraint_function(const std::vector<double> &x, std::vector<double> &grad, void *data);

			// Initialize optimization object and assign constraints and bounds
			virtual void build_optim( nlopt::opt &opt_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares );
		
			// Initialize hybrid optimization object and assign constraints and bounds
			virtual void build_hybrid_optim( nlopt::opt &opt_hybrid_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares );

			// initialize optimization 
			virtual void init_optim( std::vector<double> &optv_, std::vector<System_Properties*> *sys, int &countit, double &sum_of_squares );			

            // Initialize multistart optimization object and assign constraints and bounds
            virtual void init_optim_multistart( std::vector<double> &optv_, std::vector<System_Properties*> *sys, int &countit, double &sum_of_squares );

			// Print results to file
            virtual void print_results( std::vector<double> &optv_, std::vector<System_Properties*> *sys );

	};


}// namespace opti 

#endif


