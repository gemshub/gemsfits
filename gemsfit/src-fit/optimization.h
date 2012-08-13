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

	class Optimization : public PlotFit
	{
		public:
			// Constructor
			Optimization();
	
			// Destructor
			virtual ~Optimization();

			// nlopt instance
			nlopt::opt opt_actmod;

			// configuration data for nlopt instance (get from GEMSFIT_input.dat)
			string OptAlgo;
			int OptThreads;
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

			// optimization vector and bounds (get from GEMSFIT_input.dat)
			std::vector<double> optv; 
			std::vector<double> OptUpBounds;
			std::vector<double> OptLoBounds;

			// printing information (get from GEMSFIT_input.dat)
			PlotFit* Plot_ActMod;

/*			typedef struct 
			{
				std::vector<double> print_temperatures; 
				std::vector<double> print_pressures; 
				std::vector<double> print_molalities; 
				string print_filename;
				string print_format;
				int print_code;
				string print_xlabel;
				string print_ylabel;
				string print_head;
			} my_plotting_info;
			my_plotting_info* plotting_info;
*/

			// struct keeping the constraint data (get from GEMSFIT_input.dat)
/*			typedef struct 
			{    
			    int id;
		    	double OptUpConstraints;
    			double OptLoConstraints;
			} my_constraint_data;
*/			typedef struct 
			{    
			    int id;
		    	double Constraints;
			} my_constraint_data;
			my_constraint_data* constraint_data;
			std::vector<my_constraint_data> constraint_data_v;

			// Populate nlopt instance
			virtual void set_nlopt_param( );			

			// NLopt return codes
			virtual void print_return_message( const int result );

			// Constraint function
			virtual double constraint_function(const std::vector<double> &x, std::vector<double> &grad, void *data);

			// Initialize optimization object and assign constraints and bounds
			virtual void build_optim( nlopt::opt &opt_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares );
		
			// Initialize hybrid optimization object and assign constraints and bounds
			virtual void build_hybrid_optim( nlopt::opt &opt_hybrid_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares );

            virtual void init_optim( std::vector<double> &optv_, std::vector<System_Properties*> *sys, int &countit, double &sum_of_squares );

            // Initialize multistart optimization object and assign constraints and bounds
            virtual void init_optim_multistart( std::vector<double> &optv_, std::vector<System_Properties*> *sys, int &countit, double &sum_of_squares );

			// Print results to file
			virtual void print_results( std::vector<double> &optv_, std::vector<System_Properties*> *systems );

	};


	
	class ActivityModel : public Optimization 
	{
		public:
			// Constructor
			ActivityModel(); 

			// Destructor
			virtual ~ActivityModel();

			// nlopt instance
			nlopt::opt opt_actmod;

			// configuration data for nlopt instance (get from GEMSFIT_input.dat)
			string OptAlgo;
			int OptThreads;
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

/*			typedef struct 
			{
				std::vector<double> print_temperatures; 
				std::vector<double> print_pressures; 
				std::vector<double> print_molalities; 
				string print_filename;
				string print_format;
				int print_code;
				string print_xlabel;
				string print_ylabel;
				string print_head;
			} my_plotting_info;
			my_plotting_info* plotting_info;
*/

			// struct keeping the constraint data (get from GEMSFIT_input.dat)
/*			typedef struct 
			{    
			    int id;
		    	double OptUpConstraints;
    			double OptLoConstraints;
			} my_constraint_data;
*/			typedef struct 
			{    
			    int id;
		    	double Constraints;
			} my_constraint_data;
			my_constraint_data* constraint_data;
			vector<my_constraint_data> constraint_data_v;

			// Populate nlopt instance
			virtual void set_nlopt_param( );			

			// NLopt return codes
			virtual void print_return_message( const int result );

			// Constraint function
			virtual double constraint_function(const std::vector<double> &x, std::vector<double> &grad, void *data);

			// Initialize optimization object and assign constraints and bounds
			virtual void build_optim( nlopt::opt &opt_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares );
		
			// Initialize hybrid optimization object and assign constraints and bounds
			virtual void build_hybrid_optim( nlopt::opt &opt_hybrid_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares );

			virtual void init_optim( std::vector<double> &optv_, std::vector<System_Properties*> *sys, int &countit, double &sum_of_squares );			

            // Initialize multistart optimization object and assign constraints and bounds
            virtual void init_optim_multistart( std::vector<double> &optv_, std::vector<System_Properties*> *sys, int &countit, double &sum_of_squares );

			// Print results to file
			virtual void print_results( std::vector<double> &optv_, std::vector<System_Properties*> *sys );

	};


}// namespace opti 

#endif


