#ifndef _fit_statistics_h_
#define _fit_statistics_h_

#include <cmath>
#include <vector>
#include <numeric>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <limits>
#include <iomanip>
#include <cassert>

#include <armadillo>

#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/chi_squared.hpp>


#include "gemsfit_global_functions.h"
//#include "system_properties.h"
#include "optimization.h"
//#include "plot_class.h"

using namespace std;


class Statistics : public PlotFit
{
	public:
		// Constructor
		Statistics( std::vector<System_Properties*> *systems, double sum_of_squares_, int num_of_params_, int num_of_runs_  );
		// Destructor
		virtual ~Statistics();
		
		// Parameters for instantiating the class
		double sum_of_squares;
		int number_of_parameters;
		int number_of_measurements;
		int sensitivity_points;
		int MC_number_of_bars;
		int num_of_runs;
		
		// Number of Monte Carlo evaluations for confidence interval evaluation
		bool MCbool;		
		int num_of_MC_runs;

		// Computed parameters
		double SD_of_residuals;
		double coeff_of_determination;
		double Pearsons_chi_square;
		double reduced_chi_square;
		double degrees_of_freedom;
		
		// Printing class
		PlotFit* Plot_Stat;

		// Read statistical input specifications from GEMSFIT_input.dat
		void get_stat_param( );

		// Perform basic statistical test (R^2, SD of residuals, ...)
		void basic_stat( std::vector<double> &optv_, std::vector<System_Properties*> *systems );

		// Plot residuals
		void plot_residuals( std::vector<double> &optv_, std::vector<System_Properties*> *systems );

		// Compute confidence intervals with Mone Carlo
		void MC_confidence_interval( opti::ActivityModel* actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit );

		// Perform sensitivity analysis of selected parameters
		void sensitivity_correlation( const std::vector<double> &optv_, std::vector<System_Properties*> *systems );

		// Compute Pearson's sample correlation coefficient
		//void correlation(  );

};





#endif


