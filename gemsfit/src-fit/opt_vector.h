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

class Opt_Vector  // optimization vector
{
	public:
		// Constructor
		Opt_Vector( vector<System_Properties*> systems ); // vector with pointer to the system properties
		// Destructor
		virtual ~Opt_Vector();

		// Vector containing x values
		vector<double> opt;

		// Vector containing the first guesses for normalization of the opt vector	
		vector<double> optv0; 			

		// Vectors containing information concerning the x values in the opt vector
		vector<vector<string> > fit_species;  
		vector<vector<string> >::iterator iter_spec1; vector<string>::iterator iter_spec2;  
		vector<int> fit_param_col;
		vector<int> fit_param_type;

		// Construction of opt, species and param_types vectors
		void make_opt_vector( vector<System_Properties*> vect_of_sys_props );
};

#endif
