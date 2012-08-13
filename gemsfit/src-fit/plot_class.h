#ifndef _gemsfit_plot_class_h_
#define _gemsfit_plot_class_h_

#include <vector>
#include <string>
#include <algorithm>

#include "system_properties.h"
#include "opt_vector.h"
#include "gemsfit_global_variables.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <armadillo>

namespace dislin
{ 
#include "dislin.h"
}


class PlotFit // class mainly wrapping DISLIN plotting library
{
	public:
		// Constructor
		PlotFit();

		// Destructor
		~PlotFit();

		// printing information (get from GEMSFIT_input.dat)
		typedef struct 
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


		// Populate member variables
		void set_plotfit_vars();
	
		// Print Q-Q Plot for residuals
		void print_qqplot( const std::vector<double> &residuals_v, const std::vector<double> &quantiles_v );

		// Print vectors: wrapper for CURVE function from DISLIN
		void print_vectors_curve( const int &id, const std::vector<System_Properties*> *systems, const std::vector< std::vector<double> > &plot_array );

		// Print vectors: wrapper for QPLSCA function from DISLIN
		void print_vectors_scatter( const int &id, const std::vector<System_Properties*> *systems, const std::vector< std::vector<double> > &plot_array );
		
		// Print vectors: wrapper for CURVE function from DISLIN for plotting results from sensitivity analysis
		void print_vectors_curve( const std::vector<double> &optv_, const std::vector< std::vector<double> > &plot_array, double sensitivity_points );

		// Generate and plot histogram
		void print_histogram( std::vector<double> &optv_, double** MC_fitted_parameters_all, int MC_runs, int MC_number_of_bars );

		// Plot sensitivity gradients
		void print_sensitivity( const int rows, const int cols, const arma::mat &SensitiMatrix, std::vector< double > &molality );

		// Plot SSR contours for varying values of parameter pairs
		void print_SSR_contour( const int param_pair[2], const int num_param, const int points_per_axis, float* SSR_matrix, double* param_1_ray, double* param_2_ray );

};










#endif



