/*
*	 Copyright (C) 2013 by G. Dan Miron (mirondanro@yahoo.com)
*
*	 This file is part of the thermodynamic fitting program GEMSFIT2.
*
*    GEMSFIT2 is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    GEMSFIT2 is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU  General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with GEMSFIT2.  If not, see <http://www.gnu.org/licenses/>.
*/


/**
 *	@file statistics.h
 *
 *	@brief this header file contains definitions of the Statistics class,
 *	which evaluates the fitted parameters and performs statistcal analysis.
 *
 *	@author Ferdinand F. Hingerl & G. Dan Miron
 *
 * 	@date 21.05.2013
 *
 */


#ifndef STATISTICS_H
#define STATISTICS_H

#include "gemsfit_task.h"


using namespace std;


class statistics
{
    public:
        /**
        * Constructor for the Statistics class. It is derived from the PlotFit class.
        * Function reads statistical parameters and flags from GEMSFIT_input.dat,
        * and instantiates a pointer to the PlotFit class to print results.
        *
        * @author DM, FFH
        * @param systems vector of pointers to instances of System_Properties structs
        * @param sum_of_squares_ the sum of squares which result from the performed regression
        * @param num_of_params_ the number of variables which were fitted (length of optimization vector)
        * @param num_of_runs_ the number of runs needed for the parameter regression
        * @date 21.11.2012
        */
        statistics( TGfitTask *system, double sum_of_squares_, int num_of_params_, int num_of_runs_  );
        statistics(); // init mode SD

        /**
        * Destructor of the Statistics class.
        * Function Deletes the pointer to the PlotFit class and frees the respective memory.
        *
        * @author DM, FFH
        * @date 21.11.2012
        */
        virtual ~statistics();

        // Parameters for instantiating the class
        /// sum of squares from fit
        double sum_of_squares;
        /// number of parameters of the last regression (length of optimization vector)
        int number_of_parameters;
        /// number of measurements invoilved in the parameter regression
        int number_of_measurements;
        /// Number of evaluations points per parameter for sensitivity evaluation
        int sensitivity_points;
        /// number of bars which will be used to generate the Histogram from the Monte Carlo runs (for generation of confidence intervals)
        int MC_number_of_bars;
        /// number of runs that will be performed during the sensitivity analysis
        int num_of_runs;

        // Number of Monte Carlo evaluations for confidence interval evaluation
        /// bool indicating if Monte Carlo runs for confidence interval analysis should be performed (=1) or not (=0)
        bool MCbool;
        /// Number of Monte Carlo runs for confidence interval evaluation
        int num_of_MC_runs;

        // Computed parameters
        /// standard deviation of residuals
        double SD_of_residuals;
        /// R^2: coefficient of determination
        double coeff_of_determination;
        /// result from Pearson's Chi Squared test
        double Pearsons_chi_square;
        /// reduces Chi Squared
        double reduced_chi_square;
        /// degrees of freedom
        double degrees_of_freedom;

//        // Printing class
//        PlotFit* Plot_Stat;

//        /**
//        * Read statistical input specifications from GEMSFIT_input.dat
//        *
//        * @author FFH
//        * @date 09.04.2012
//        */
//        void std_get_stat_param( );

        /**
        * Read statistical input specifications from GEMSFIT_input.dat
        *
        * @author SD
        * @date 14.03.2013
        */
        void get_stat_param_txt( );
        void out_stat_param_txt( bool with_comments, bool brief_mode );
        void default_stat_param();

        /**
        * Perform basic statistical test (R^2, SD of residuals, ...)
        *
        * @author DM, FFH
        * @date 21.11.2012
        * @param optv_ optimization vector
        * @param systems vector of pointers to instances of System_Properties structs
        */

        void basic_stat(std::vector<double> &optv_, TGfitTask *gfittask );

//		/**
//		* Plot residuals of parameter regression.
//		*
//		* @author FFH
//		* @param optv_ optimization vector
//		* @param systems vector of pointers to instances of System_Properties structs
//		* @date 09.04.2012
//		*/
//		void SS_plot_residuals( std::vector<double> &optv_, std::vector<SS_System_Properties*> *systems );

//        /**
//        * Compute confidence intervals using Mone Carlo.
//        * @author DM
//        * @param actmod   pointer to instance of ActivityModel object.
//        * @param optv_    optimization vector
//        * @param systems  vector of pointers to instances of System_Properties structs
//        * @param countit  global counter of runs over measurement data
//        * @date 05.12.2012
//        */
//        void SS_MC_confidence_interval( opti::StdStateProp* gibbs, std::vector<double> &optv_, std::vector<SS_System_Properties*> *systems, int &countit );

        /**
        * Perform sensitivity analysis of selected parameters.
        * @author DM
        * @param optv_    optimization vector
        * @param systems  vector of pointers to instances of System_Properties structs
        * @date 06.12.2012
        */
        void sensitivity_correlation( const std::vector<double> &optv_, TGfitTask* gfittask );

//		// Compute Pearson's sample correlation coefficient
//		//void correlation(  );

};

#endif // STATISTICS_H
