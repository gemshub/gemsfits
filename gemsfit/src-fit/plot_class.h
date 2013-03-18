/* 
*	 Copyright (C) 2012 by Ferdinand F. Hingerl (hingerl@hotmail.com)
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
 *	@file plot_class.h
 *
 *	@brief this header file contains definitions of the PlotFit class, 
 *	which ia a wrapper for the dislin librarby and contains functions for data plotting.   
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */

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


/// class mainly wrapping DISLIN plotting library. It contains graph and functions for data plotting.
class PlotFit 
{
	public:
		/** 
		* Constructor for the PlotFit class. 
		* Function reads plotting parameters and flags from GEMSFIT_input.dat,
		*
		* @author FFH
		* @date 09.04.2012
		*/	
		PlotFit();
        PlotFit(int);

		/** 
		* Destructor for the PlotFit class. 
		*
		* @author FFH
		* @date 09.04.2012
		*/	
		~PlotFit();

		/// system containing printing information (retrieved from GEMSFIT_input.dat)
		typedef struct 
		{
			/// temperatures to be plotted
			std::vector<double> print_temperatures; 
			/// pressures to be plotted
			std::vector<double> print_pressures; 
			/// molality ranges to be plotted
			std::vector<double> print_molalities;
			/// name of output file (computed versus measured data) 
			string print_filename;
			/// format of output file 
			string print_format;
			/// which kind of measurement data should be plotted 
			int print_code;
			/// x label 
			string print_xlabel;
			/// y label 
			string print_ylabel;
			/// headline of plot
			string print_head;
		} my_plotting_info;
		my_plotting_info* plotting_info;


		/** 
		* Function populates member variables of the Plot_Fit class with data form the GEMSFIT configuration file.  
		*
		* @author FFH
		* @date 09.04.2012
		*/	
		void set_plotfit_vars();
	
        /**
        * Function populates member variables of the Plot_Fit class with data form the GEMSFIT configuration file.
        *
        * @author DS
        * @date 15.03.2013
        */
        void set_plotfit_vars_txt();
        void out_plotfit_vars_txt( bool with_comments, bool brief_mode );
        void define_plotfit_vars( );

		/** 
		* Function prints Q-Q Plot for residuals 
		*
		* @param residuals_v 		vector contains the residuals at each measurement data point
		* @param quantiles_v 		vector contains the quantiles of the computed data points
		* @author FFH
		* @date 09.04.2012
		*/	
		void print_qqplot( const std::vector<double> &residuals_v, const std::vector<double> &quantiles_v );

		/**  
		* Generic function for printing vectors (wrapper for CURVE function from DISLIN) 
		*
		* @param id           id can be used as flag to access specific code parts in the function
		* @param systems 	  vector of pointers to instances of System_Properties structs
		* @param plot_array   array prepared for plotting 
		* @author FFH
		* @date 09.04.2012
		*/	
		void print_vectors_curve( const int &id, const std::vector<System_Properties*> *systems, const std::vector< std::vector<double> > &plot_array );

		/**  
		* generic function for printing vectors as scatter plots (wrapper for QPLSCA function from DISLIN) 
		*
		* @param id           id can be used as flag to access specific code parts in the function
		* @param systems 	  vector of pointers to instances of System_Properties structs
		* @param plot_array   array prepared for plotting 
		* @author FFH
		* @date 09.04.2012
		*/	
		void print_vectors_scatter( const int &id, const std::vector<System_Properties*> *systems, const std::vector< std::vector<double> > &plot_array );
		
		/**  
		* wrapper for CURVE function from DISLIN for plotting results from sensitivity analysis 
		*
		* @param optv_        			optimization vector
		* @param sensitivity_points 	number of sensitivity point to plot
		* @param plot_array   			data array prepared for plotting 
		* @author FFH
		* @date 09.04.2012
		*/	
		void print_vectors_curve( const std::vector<double> &optv_, const std::vector< std::vector<double> > &plot_array, double sensitivity_points );

		/**  
		* generate and plot histogram of results from Monte Carlo confidence bound generation
		*
		* @param optv_        				optimization vector
		* @param MC_fitted_parameters_all  	array contains fitted parameters at all generated mesurement data
		* @param MC_number_of_bars   		number of bars in the Monte Carlo plot
		* @param MC_runs					number of Monte Carlo runs		
		* @author FFH
		* @date 09.04.2012
		*/	
		void print_histogram( std::vector<double> &optv_, double** MC_fitted_parameters_all, int MC_runs, int MC_number_of_bars );

		/**  
		* plot sensitivity gradients
		*
		* @param rows        				number of rows in the sensitivity plot
		* @param cols        				number of columns in the sensitivity plot
		* @param SensitiMatrix		  		matrix contains the sensitivity data 
		* @param molality   				vector of molal concentrations of species
		* @author FFH
		* @date 09.04.2012
		*/	
		void print_sensitivity( const int rows, const int cols, const arma::mat &SensitiMatrix, std::vector< double > &molality );

		/**  
		* plot SSR contours for varying values of parameter pairs
		*
		* @param param_pair        			indizes of dependent component pairs 
		* @param num_param        			number of parameters in the sensitivity plot
		* @param points_per_axis  			number of points per axis in the sensitivity plot 
		* @param param_1_ray   				array containing the parameter for the x-axis
		* @param param_2_ray   			    array containing the parameter for the y-axis
		* @param SSR_matrix					matrix conatains sum of squared residuals
		* @author FFH
		* @date 09.04.2012
		*/	
		void print_SSR_contour( const int param_pair[2], const int num_param, const int points_per_axis, float* SSR_matrix, double* param_1_ray, double* param_2_ray );

};





#endif



