#ifndef _system_properties_h_
#define _system_properties_h_

#include <cmath>
#include <vector>
#include <string>
#include <sstream> // string stream
#include <iostream>
#include <fstream>
#include <cstdlib> // www.cplusplus.com/reference/clibrary/cstdlib/
#include <algorithm> // http://www.cplusplus.com/reference/algorithm/
#include <iterator> // http://www.cplusplus.com/reference/std/iterator/
#include <limits> // http://www.cplusplus.com/reference/std/limits/numeric_limits/  -its members returning or set to the different values that define the properties that type has in the specific platform in which it compiles.
#include <iomanip> // format output stream http://www.cprogramming.com/tutorial/iomanip.html

#include <boost/algorithm/string.hpp> // libraries for string http://www.boost.org/doc/libs/1_50_0/doc/html/string_algo.html
#include <boost/algorithm/string/find.hpp> // set of find algorithms http://www.boost.org/doc/libs/1_50_0/doc/html/string_algo/reference.html#header.boost.algorithm.string.find_hpp

#include "node.h" //???
#include "data_manager.h"//???

using namespace std;
using namespace boost;

struct System_Specifications // like a class for system sepcifi with two members strings
{
	string meas_db_;
	string param_file_;
};


class System_Properties : public Data_Manager // inherits the class Data_Manager
{
	public:
		// Constructor
		System_Properties( System_Specifications system_ );
		// Destructor		
		virtual ~System_Properties();
		
		typedef vector<double> vd; // typdefs 
		typedef vector<string> vs;
		typedef vector<int> vi;
		typedef vector<vector<int> > vvi;

		// System specifications
		string system_name;
		string phase_name;
		string meas_db;
		vs species;
		string param_file;
		vd charges;
		
		enum act_model
		{ 
		  ELVIS = 1, 
		  Pitzer, 
		  EUNIQUAC
		};
		act_model activity_model;
	
		// Printing Flag: if this flag is set to one, the result of the optimization will be printed to file (via optimization.cpp)
		bool printfile;

		// Monte Carlo flag: if true, then the MPI commands within the objective function call will not be executed. Instead, the loop over Monte Carlo runs is parallelized
		bool MC_MPI;

		// Store computed and actual measurement values for Monte Carlo confidence interval generation
		vd computed_values_v;
		vd measured_values_v;

		// Store computed residuals
		vd computed_residuals_v;

		// Position of aIPc_fit and aDCc_fit parameters in opt vector
		vvi fit_indices; vi fit_ind_opt; vi fit_ind_sys;		


		// Pointer to Data_Manager class containing measurement data
		Data_Manager* data_meas;


		// Struct with system specific measurement data
		struct parameters
		{
			// dimensionalities of the parameter arrays
			int cols_aIPc;
			int rows_aIPc; 
			int cols_aIPx;
			int cols_aDCc;
			
			// parameters vectors
			vector<double> aIPc;
			vector<double> aDCc; 
			vector<int> aIPx; 
			
			// vectors containing first guesses of aIPc/aDCc arrays
			vector<double> aIPc_fit_val;
			vector<double> aDCc_fit_val;

			// vectors containing the position indices of the values 
			// that will be fitted in the aIPc/aDCc arrays
			vector<int> aIPc_fit_ind;
			vector<int> aDCc_fit_ind;
		};
		
		parameters* sysparam; // pointer to struct parameters
		void getparam( parameters* sysparam );

};

#endif
