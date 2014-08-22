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
#include "keywords.h"


using namespace std;


class statistics
{

    private:

    struct parameters /// structure holding the target function information
    {
        string Ptype;
        string Pfittype;
        string Pname;
        double Ival;
        double Fval;
        double CSS;
        double mc95;
        double mcSTDEV;
        double mcMEAN;
        vector<double> mcconfi;
        vector<double> correl;
    };

    vector<parameters*> fitparam;

    struct objfunstat
    {
        vector<double> scatter, scatter2;
        vector<double> percentiles;
        vector<double> quantiles;
        vector<double> orderd_res;
        double stdev_res;
        double norm_stdev_res;
        double mean_res;
        double mean_meas;
        double norm_mean_res;
        double R2;
        double min_res, max_res;
        double SumWTFun;
        int nr_pos_res, nr_neg_res;
        int nr;
        bool isComputed;

        struct expdataset
        {
            string name;
            vector<double> scatter;
            vector<double> residuals;
            vector<double> norm_residuals;
            vector<double> measured_value;
        };
        vector<expdataset> exp_dataset;
    };

    vector<objfunstat*> objfun_stat;

    double Weighted_mean_res;
    double means_res;
    double Weighted_max_res;
    double max_res;
    double Weighted_min_res;
    double min_res;
    int pos_residuals, neg_residuals;
    /// Sum of residuals of the weighted target function, the same as Tfun_sum_of_residuals with the weights are 1
    double Weighted_Tfun_sum_of_residuals;
    /// Sum of residuals of the target function with no weight
    double          Tfun_sum_of_residuals;
    /// Weighted absolute sum of residals (measured - calculated), the same as Abs_sum_of_residuals when the weights are 1
    double  Weighted_Abs_sum_of_residuals;
    /// Absolute sum of residuals
    double           Abs_sum_of_residuals;

    vector <double> weighted_residuals; /// residuals weighted with the weighting shceeme selected during fitting
    vector <double> measured_norm_residuals;  /// in %, residuals normalized to measured value * 100
    /// number of independent fitted parameters
    int number_of_ind_parameters;
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
    /// used in calculation of the sensitivities by central diferences
    double perturbator;

    // Number of Monte Carlo evaluations for confidence interval evaluation
    /// Number of Monte Carlo runs for confidence interval evaluation
    int num_of_MC_runs;

    // Computed parameters
    /// standard deviation of residuals
    double     SD_of_residuals;
    /// standard deviation of the absolute values of residuals
    double Abs_SD_of_residuals;
    /// standard deviation of the weighted residuals (same as SD_of_residuals if weights are 1)
    double Weighted_SD_of_residuals;
    /// standard deviation of target function residuals
    double  Weighted_TF_SD_of_residuals;
    /// R^2: coefficient of determination
    double coeff_of_determination;
    /// result from Pearson's Chi Squared test
    double Pearsons_chi_square;
    /// reduces Chi Squared
    double reduced_chi_square;
    /// Error variance
    double error_variance;
    /// degrees of freedom
    double degrees_of_freedom;


    /**
    * Read statistical input specifications from GEMSFIT_input.dat
    *
    * @author SD
    * @date 14.03.2013
    */
    void get_stat_param_txt( );
    void default_stat_param();


    public:


        /**
        * Constructor for the Statistics class. It is derived from the PlotFit class.
        * Function reads statistical parameters and flags from GEMSFIT_input.dat,
        * and instantiates a pointer to the PlotFit class to print results.
        *
        * @author DM, FFH
        * @param systems vector of pointers to instances of System_Properties structs
        * @param weighted_Tfun_sum_of_residuals_ the sum of squares which result from the performed regression
        * @param num_of_params_ the number of variables which were fitted (length of optimization vector)
        * @param num_of_runs_ the number of runs needed for the parameter regression
        * @date 21.11.2012
        */
        statistics( TGfitTask *system, double weighted_Tfun_sum_of_residuals_, int num_of_params_, int num_of_runs_  );
        statistics(); // init mode SD

        /// int indicating if Monte Carlo runs for confidence interval analysis should be performed (=1 || =2) or not (=0)
        int MCbool;

        /**
        * Destructor of the Statistics class.
        * Function Deletes the pointer to the PlotFit class and frees the respective memory.
        *
        * @author DM, FFH
        * @date 21.11.2012
        */
        virtual ~statistics();

        /**
        * Write statistical input specifications to GEMSFIT_input.dat template file
        *
        * @author SD
        * @date 14.03.2013
        */
        void out_stat_param_txt( bool with_comments, bool brief_mode );

        /**
        * Perform basic statistical test (R^2, SD of residuals, ...)
        *
        * @author DM, FFH
        * @date 21.11.2012
        * @param optv_ optimization vector
        * @param systems vector of pointers to instances of System_Properties structs
        */

        void basic_stat(std::vector<double> &optv_, TGfitTask *gfittask );

        /**
        * Compute confidence intervals using Mone Carlo.
        * @author DM
        * @param actmod   pointer to instance of ActivityModel object.
        * @param optv_    optimization vector
        * @param systems  vector of pointers to instances of System_Properties structs
        * @param countit  global counter of runs over measurement data
        * @date 05.12.2012
        */
        void MC_confidence_interval(std::vector<double> &optv_, TGfitTask* gfittask);

        /**
        * Perform sensitivity analysis of selected parameters.
        * @author DM
        * @param optv_    optimization vector
        * @param systems  vector of pointers to instances of System_Properties structs
        * @date 06.12.2012
        */
        void sensitivity_correlation(vector<double> &optv_, TGfitTask* gfittask );


        void print_param ();

//		// Compute Pearson's sample correlation coefficient
//		//void correlation(  );

};

#endif // STATISTICS_H
