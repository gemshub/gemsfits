#ifndef _system_restart_h_
#define _system_restart_h_


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

#include "H5Cpp.h"

#ifndef H5_NO_NAMESPACE
    using namespace H5;
#endif


class SystemRestart
{

	public:
		// Constructor
		SystemRestart();

		// Destructor
		~SystemRestart();


	private:

		H5std_string FILE_NAME; 	//( "SDS.h5" );
		H5std_string DATASET_NAME; 	//( "IntArray" );




};




#endif
