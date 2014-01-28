// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT2 code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT/>
//
// GEMSIFT2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT2 code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//

/**
 *	@file statistics.cpp
 *
 *	@brief this header file contains implementation of the statistics class and the functions for statistical calculations.
 *
 *	@author G. Dan Miron
 *
 * 	@date 17.05.2013
 *
 */


#include "statistics.h"
#include "gemsfit_global_functions.h"
#include <armadillo>
#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/chi_squared.hpp>


#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"
#include <boost/random/normal_distribution.hpp>

// Constructor
statistics::statistics(TGfitTask *gfittask, double Weighted_Tfun_sum_of_residuals_, int num_of_params_, int num_of_runs_ )
{
    double mean_res = 0.0, Abs_mean_res = 0.0, Weighted_TF_mean_res = 0.0;
    unsigned int i;

//    gfittask->test();

    number_of_measurements = 0;
    number_of_measurements += gfittask->computed_values_v.size();
    Tfun_sum_of_residuals = 0.0;
    Abs_sum_of_residuals = 0.0; Weighted_Abs_sum_of_residuals = 0.0;
    min_res =0.0; max_res = 0.0; Weighted_min_res = 0.0; Weighted_max_res = 0.0; neg_residuals = 0; pos_residuals = 0;

    // this is the minimized value during oprimization
    Weighted_Tfun_sum_of_residuals 		= Weighted_Tfun_sum_of_residuals_;

    //
    for (unsigned i=0; i<number_of_measurements; ++i)
    {
        Tfun_sum_of_residuals += gfittask->Tfun_residuals_v[i];                         // sum residuals of the minimized function without weight
         Abs_sum_of_residuals += fabs(gfittask->residuals_v[i]);                        // sum of the absolute value of residuals
Weighted_Abs_sum_of_residuals += fabs(gfittask->residuals_v[i])*gfittask->weights[i];   // sum of the absolute value of residuals * weight

        if (gfittask->weights[i] != 1)
        weighted_residuals.push_back(gfittask->residuals_v[i]*gfittask->weights[i]);
        else weighted_residuals.push_back(gfittask->residuals_v[i]);
        measured_norm_residuals.push_back(gfittask->residuals_v[i] / gfittask->measured_values_v[i] * 100);

        // getting the samlest and largest values of residuals and weighted residuals
        // counting the number of negative and positivie residuals
        if (min_res > gfittask->residuals_v[i]) min_res = gfittask->residuals_v[i];
        if (Weighted_min_res > weighted_residuals[i]) Weighted_min_res = weighted_residuals[i];
        if (max_res < gfittask->residuals_v[i]) max_res = gfittask->residuals_v[i];
        if (Weighted_max_res < weighted_residuals[i]) Weighted_max_res = weighted_residuals[i];
        if (gfittask->residuals_v[i] < 0) ++neg_residuals;
        if (gfittask->residuals_v[i] >= 0) ++pos_residuals;
    }

    num_of_runs		= num_of_runs_;

cout<<" Statistics Constructor: sum of squares: "<<Weighted_Tfun_sum_of_residuals<<endl;

    number_of_ind_parameters   = num_of_params_;
    number_of_parameters = number_of_ind_parameters + gfittask->Opti->reactions.size() + gfittask->Opti->Lparams.size();

cout<<" Statistics Constructor: number_of_parameters: "<<number_of_parameters<<endl;

    get_stat_param_txt();
//stat->set_plotfit_vars_txt();

//    /// Instantiate pointer to PlotFit class to print results
//    Plot_Stat = new PlotFit();

    // Degrees of freedom
    degrees_of_freedom = number_of_measurements-number_of_parameters;

    // Compute standard deviation of residuals
    for (unsigned i=0; i<number_of_measurements; i++)
    {
Weighted_TF_mean_res += gfittask->Weighted_Tfun_residuals_v[i];
        Abs_mean_res += fabs(gfittask->residuals_v[i]);
   Weighted_mean_res += weighted_residuals[i];
            mean_res += gfittask->residuals_v[i];

    }
// copute means
 Weighted_TF_mean_res = Weighted_TF_mean_res / number_of_measurements;  // mean of residuals that summ up to give the minimized value
         Abs_mean_res = Abs_mean_res / number_of_measurements;          // mean of the absolute value of residuals
    Weighted_mean_res = Weighted_mean_res / number_of_measurements;     // mean of residuals*weight (same as means_res if weight is 1)
             mean_res = mean_res / number_of_measurements;              // mean of residuals

 Weighted_TF_SD_of_residuals = 0.0;
         Abs_SD_of_residuals = 0.0;
    Weighted_SD_of_residuals = 0.0;
             SD_of_residuals = 0.0;

    for (unsigned i=0; i<number_of_measurements; i++)
    {
        Weighted_TF_SD_of_residuals += pow((gfittask->Weighted_Tfun_residuals_v[i]-Weighted_TF_mean_res),2);
                Abs_SD_of_residuals += pow((fabs(gfittask->residuals_v[i])-Abs_mean_res),2);
           Weighted_SD_of_residuals += pow((weighted_residuals[i]-Weighted_mean_res),2);
                    SD_of_residuals += pow((gfittask->residuals_v[i]-mean_res),2);
    }

    Weighted_TF_SD_of_residuals = sqrt((Weighted_TF_SD_of_residuals/degrees_of_freedom));   // standard deviation of residuals that summ up to give the minimized value
            Abs_SD_of_residuals = sqrt((Abs_SD_of_residuals/degrees_of_freedom));           // standard deviation of the absolute value of residuals
       Weighted_SD_of_residuals = sqrt((Weighted_SD_of_residuals/degrees_of_freedom));      // standard deviation of residuals*weight (same as means_res if weight is 1)
                SD_of_residuals = sqrt((SD_of_residuals/degrees_of_freedom));               // standard deviation of residuals
}


// Destructor
statistics::~statistics()
{

}

// Perform basic statistical analysis
void statistics::basic_stat( std::vector<double> &optv_, TGfitTask *gfittask )
{
    // Variable declarations
    int i;
    double mean = 0.;
    double ResSumSquares = 0., TotalSumSquares = 0.;
//    double Nom_CC = 0.0, Dnom_CC = 0.0;
    double Correlation_coef = 0.0;
//    double Res = 0.;
    double m2 = 0., m3 = 0., m4 = 0.;
    double sqrtb1, b2, Y, beta2_sqrtb1, W2, delta, alpha, Z_sqrtb1, E_b2, Var_b2, x, sqrt_beta1_b2, A, Z_b2, K2, K2test;
    vector<double> percentiles_v;
    vector<double> quantiles_v;
//    vector<double> abs_residuals_v;



    // Compute R^2: coefficient of determination
        mean = 0;
    for (unsigned i=0; i< number_of_measurements; i++)
    {
        mean += gfittask->measured_values_v[i];
    }
    mean = mean / gfittask->measured_values_v.size();

    assert( gfittask->computed_values_v.size() == gfittask->measured_values_v.size() );


    for(unsigned i=0; i< number_of_measurements; i++ )
    {
        ResSumSquares += pow( (gfittask->measured_values_v[i] - gfittask->computed_values_v[i]), 2);
        TotalSumSquares += pow( (gfittask->measured_values_v[i] - mean), 2);
    }
    coeff_of_determination = 1 - ResSumSquares / TotalSumSquares;




    // Pearson Chi Square test
    Pearsons_chi_square = 0.;
    for(unsigned i=0;  i< number_of_measurements; i++ )
    {
        if (gfittask->measured_values_v[i] != 0)
        Pearsons_chi_square += (abs(gfittask->computed_values_v[i]) - abs(gfittask->measured_values_v[i]))*(abs(gfittask->computed_values_v[i]) - abs(gfittask->measured_values_v[i])) / abs(gfittask->measured_values_v[i]);
    }

    // Reduced Chi Square
    reduced_chi_square = Pearsons_chi_square / degrees_of_freedom;

    // Error variance
    error_variance = Weighted_Tfun_sum_of_residuals/degrees_of_freedom;

    // Generate object containing normally distributed data with mean = 0 and standard deviation = SD_of_residuals
    boost::math::normal dist(  0., SD_of_residuals );

    sort( gfittask->residuals_v.begin(), gfittask->residuals_v.end() );
    sort(weighted_residuals.begin(), weighted_residuals.end());

    int N = (int) gfittask->residuals_v.size();

    // Compute percentile
    for( i=0; i< N; i++ )
        percentiles_v.push_back( (i+1-0.5)/N );

    // Compute Correlation coeficient
    arma::vec e0 = arma::zeros<arma::vec>( N, 1 );
    arma::vec tau = arma::zeros<arma::vec>( N, 1 );
    for( i=0; i< N; i++ )
    {
        e0(i) = weighted_residuals[i]-Weighted_mean_res;
        tau(i) = percentiles_v[i];
    }

    double sum1=0.0, sum2=0.0, sum3=0.0;
    for (i=0; i<N; i++)
    {
        sum1 += e0(i)*tau(i);
        sum2 += e0(i)*e0(i);
        sum3 += tau(i)*tau(i);
    }
    Correlation_coef = (sum1*sum1)/(sum2*sum3);

    // Rank-based z-scores
    for( i=0; i< N; i++ )
        quantiles_v.push_back( boost::math::quantile( dist, percentiles_v[i] ) );

    // Generate Q-Q Plot (Quantile-Quantile Plot)
    ofstream myqq;
    string path = gpf->ResultDir() + "qqplot.csv";
    myqq.open(path.c_str(),ios::app);
    myqq << "residuals,quantiles" << endl;
    for (i=0; i<N; ++i)
    {
        myqq <<gfittask->residuals_v[i]<<","<<quantiles_v[i]<< endl;
    }
    myqq.close();

    // D'Agostino K square test for normality
    mean = 0;
    for (i=0; i< gfittask->residuals_v.size(); i++)
    {
        mean += weighted_residuals[i];
    }
    mean = mean / gfittask->residuals_v.size();

    for( i=0; i<N; i++ )
    {
        m2 += pow( (weighted_residuals[i] - mean), 2. ) / N;
        m3 += pow( (weighted_residuals[i] - mean), 3. ) / N;
        m4 += pow( (weighted_residuals[i] - mean), 4. ) / N;
    }

    sqrtb1 = m3 / pow( m2, (3./2.));
    b2     = (m4 / pow( m2, 2.))-3;

    // D'Agostino K square test: test of skewness
    Y             = sqrtb1 * sqrt( ((N+1.)*(N+3.))/(6*(N-2.)) );
    beta2_sqrtb1  = ( 36*(N*N + 2*N - 5)*(N - 7.)  ) / ( (N - 2.)*(N + 5.)*(N + 7.)*(N + 9.) );
    W2            = -1. + sqrt(2 * (beta2_sqrtb1 + 2.) );
if( W2 < 1.)  // workaround to suppress nan() and zdiv crash
    W2 = 1.001;
    delta         = 1. / sqrt( log( sqrt(W2) ) ) ;
    alpha         = sqrt( 2. / (W2 - 1) );
    Z_sqrtb1      = delta * log( Y / alpha + sqrt( pow( (Y/alpha), 2 ) + 1 ) );

    // D'Agostino K square test: test of kurtosis
    // mean of b2
    E_b2          = -6./(N+1.);
    // variance of b2
    Var_b2        = ( 24.*N*(N - 2.)*(N - 3.) ) / ( (N + 1.)*(N + 1.)*(N + 3.)*(N + 5.) );
    // standardized version of b2
    x             = ( b2 - E_b2 ) / ( sqrt(Var_b2) );
    // third standardized moment of b2
    sqrt_beta1_b2 = 6.*(N*N - 5.*N + 2.) / ( (N + 7.)*(N + 9.) ) * sqrt(( 6. * (N + 3.)*(N + 5.) )/ ( N * (N - 2.) * (N - 3.) ) );
    A             = 6. + ( 8. / sqrt_beta1_b2 ) * ( 2. / sqrt_beta1_b2 + sqrt( 1 + 4./(sqrt_beta1_b2*sqrt_beta1_b2) ) );
    Z_b2          = ( (1.-2./(9.*A)) - pow( ((1.-2./A) / (1 + x*sqrt(2./(A - 4.))) ), (1./3.) ) ) / sqrt( 2./(9.*A) );

    // K square
    K2 = Z_sqrtb1*Z_sqrtb1 + Z_b2*Z_b2;


    // Create chi-squared distribution object
    boost::math::chi_squared chi_dist( 2 );
    // 1 - cumulative distribution function
    K2test = 1 - boost::math::cdf( chi_dist, K2 );


    // Write first statistcs to file
        ofstream myStat;
        myStat.open(gpf->FITStatisticsFile().c_str(),ios::app);
        myStat << "#####################################################################################" << endl;
        myStat << "#>>>>>>>>>>>>>>>>>>>> Summary and statistics from Gemsfits run >>>>>>>>>>>>>>>>>>>>>#" << endl;
        myStat << "#####################################################################################" << endl;
        myStat << endl;
        myStat << " Best fit results for parameters from regression : "                         <<endl;
        for( i=0; i< optv_.size(); i++ ) // cols
        {
            // Print optimized parameter values to file
            if (gfittask->Opti->Ptype[i] == "G0")
            {
                myStat <<"          parameter G0 "<<
                         gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i])
                       <<" : " << optv_[i] << endl;
//                cout /*<< gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i])*/ << endl;
            } else
              myStat <<"            parameter " << gfittask->Opti->Ptype[i] << " : "
                     << optv_[i] << endl;
        }
        if (gfittask->Opti->h_RDc)
        {
            for (i=0; i<gfittask->Opti->reactions.size(); ++i)
            {
                myStat <<"          Reac parameter "<<gfittask->Opti->reactions[i]->Dc_name <<" : "<<gfittask->Opti->reactions[i]->std_gibbs<<endl;
            }
        }
        if (gfittask->Opti->h_Lp)
        {
            for (i=0; i<gfittask->Opti->Lparams.size(); ++i)
            {
                myStat <<"          Linked parameter "<<gfittask->Opti->Lparams[i]->name <<" : "<<gfittask->Opti->Lparams[i]->EV<<endl;
            }
        }
        myStat << endl;
        myStat << endl;
        myStat << " Basic statistics:" << endl;
        myStat << " Number of measurements :                                                " << number_of_measurements     << endl;
        myStat << endl;
        myStat << " Number of parameters :                                              	" << number_of_parameters 	<< endl;
        myStat << endl;
        myStat << " Number of runs needed for regression :                                  " << num_of_runs 		<< endl;
        myStat << endl;
        myStat << " Degrees of freedom :                                                    " << degrees_of_freedom	 	<< endl;
        myStat << endl;

//        myStat << " - - - - - - - RESIDUAL STATISTICS - - - - - - - " << endl;
        myStat << " Value of the minimized function sum :                                   " << Weighted_Tfun_sum_of_residuals 		<< endl;
        myStat << endl;
//        myStat << " Target function sum  :                                                	" << Tfun_sum_of_residuals 		<< endl;
//        myStat << endl;
//        myStat << " Sum of absolute values of residuals:                                    " << Abs_sum_of_residuals << endl;
//        myStat << endl;
        myStat << " Sum of absolute values of residuals:                                    " << Weighted_Abs_sum_of_residuals << endl;
        myStat << endl;
//        myStat << " Standard deviation of the residuals :                               	" << SD_of_residuals 		<< endl;
//        myStat << endl;
        myStat << " Standard deviation of the residuals :                                   " << Weighted_SD_of_residuals 		<< endl;
        myStat << endl;
//        myStat << " Standard deviation of the weighted target function values :             " << Weighted_TF_SD_of_residuals 		<< endl;
//        myStat << endl;
//        myStat << " Standard deviation of the absolute values of the residuals :            " << Abs_SD_of_residuals 		<< endl;
//        myStat << endl;
        myStat << " Average residual :                                                      " << Weighted_mean_res 		<< endl;
        myStat << endl;
        myStat << " Maximum residual :                                                      " << Weighted_max_res 		<< endl;
        myStat << endl;
        myStat << " Minimum residual :                                                      " << Weighted_min_res 		<< endl;
        myStat << endl;
        myStat << " Number of positive residuals :                                          " << pos_residuals 		<< endl;
        myStat << endl;
        myStat << " Number of negative residuals :                                          " << neg_residuals 		<< endl;
        myStat << endl;


        myStat << " Coefficient of determination R^2 :                                  	" << coeff_of_determination     << endl;
        myStat << endl;
        myStat << " Correlation coefficient (Brockwell and Davis, 1987, p304) RN^2:         " << Correlation_coef     << endl;
        myStat << endl;
        myStat << " Pearson's Chi Square test :                                             " << Pearsons_chi_square   	<< endl;
        myStat << endl;
        myStat << " Reduced_chi_square :                                                	" << reduced_chi_square 	<< endl;
        myStat << endl;
        myStat << " Error variance :                                                     	" << error_variance 	<< endl;
        myStat << endl;

        myStat << " D'Agostino K square test, K squared :                                   " << K2 << endl;
        myStat << endl;
        myStat << " Probability that K squared follows Chi-squared distribution :           " << K2test << endl;
        myStat << endl;
        myStat << endl;
        myStat.close();
}


// perform sensitivity analysis of selected parameters
void statistics::sensitivity_correlation( vector<double> &optv_, TGfitTask* gfittask )
{
        unsigned int i, j, k, len_meas;
        double residual_sys;

        std::vector<double> computed_up;
        std::vector<double> computed_lo;
        std::vector<double> opt_scan;
        std::vector<double> ssr_param;
        std::vector<double> opt_scan_v;

        opt_scan.resize( optv_.size() );
        ssr_param.resize( sensitivity_points );
        opt_scan_v.resize( sensitivity_points );
        std::vector< std::vector< double> > array_ssr;

        len_meas = (int) gfittask->Weighted_Tfun_residuals_v.size();

        // Compute Jacobian matrix (= Sensitivity matrix)
        arma::mat SensitivityMatrix( len_meas, (int) optv_.size() );
        arma::mat DimensionlessScaledSensitivities( len_meas, (int) optv_.size() );
        arma::mat OnePercentScaledSensitivities( len_meas, (int) optv_.size() );
        arma::mat WeightMatrix = arma::zeros<arma::mat>(len_meas, len_meas);
        arma::vec CompositeScaledSensitivities = arma::zeros<arma::vec>( (int) optv_.size(), 1 );
        arma::vec ParameterStandardDeviation = arma::zeros<arma::vec>( (int) optv_.size(), 1 );
        arma::vec CoefficientOfVariation = arma::zeros<arma::vec>( (int) optv_.size(), 1 );
        arma::vec Parameter_t_Statistic = arma::zeros<arma::vec>( (int) optv_.size(), 1 );


        double delta = perturbator;
        for( i=0; i< optv_.size(); i++ )
        {
            opt_scan = optv_;
            vector<double> sens;

            // Central finite differences:
            opt_scan[i] = optv_[i] + optv_[i]*delta;
            residual_sys = 0.;
            gems3k_wrap( residual_sys, opt_scan, gfittask );
//cout<<"residual_sys = "<<residual_sys<<endl;
            computed_up = gfittask->computed_values_v;

            opt_scan[i] = optv_[i] - optv_[i]*delta;
            residual_sys = 0.;
            gems3k_wrap( residual_sys, opt_scan, gfittask );
//cout<<"residual_sys = "<<residual_sys<<endl;
            computed_lo = gfittask->computed_values_v;

            for( k=0; k< len_meas; k++ )
            {
                SensitivityMatrix(k,i) 	 			  = ( computed_up[k] - computed_lo[k] ) / ( optv_[i]*delta*2 );
                sens.push_back(SensitivityMatrix(k,i));

                OnePercentScaledSensitivities(k,i) = (SensitivityMatrix(k,i)) * optv_[i] / 100;

                if (gfittask->Tfun->weight == keys::inverr2) // the weight matrix is added (weighting with 1/error)
                {
                    DimensionlessScaledSensitivities(k,i) = SensitivityMatrix(k,i) * fabs( optv_[i] ) * gfittask->weights[k];
                } else
                    if ((gfittask->Tfun->weight == keys::inverr) || (gfittask->Tfun->weight == keys::inverr3) || (gfittask->Tfun->weight == keys::inverr_norm)) // no re
                    {
                        DimensionlessScaledSensitivities(k,i) = SensitivityMatrix(k,i) * fabs( optv_[i] ) * sqrt(gfittask->weights[k]);
                    }
                    else  DimensionlessScaledSensitivities(k,i) = SensitivityMatrix(k,i) * fabs( optv_[i] ) * 1;
                CompositeScaledSensitivities(i)      += sqrt( DimensionlessScaledSensitivities(k,i)*DimensionlessScaledSensitivities(k,i)/len_meas );
            }
            gfittask->print->sensitivity.push_back(sens);
        }

        gems3k_wrap( residual_sys, optv_, gfittask );

        ofstream myStat;
        myStat.open(gpf->FITStatisticsFile().c_str(),ios::app);

/*
        myStat << " Sensitivity matrix over each parameter [j] measurement point [i]: "<<endl;
        myStat << " Calculated using central diferences, see ref. [1] section 4.3 "<<endl;
        for( i=0; i<  computed_up.size();  i++ )
        {
            for( j=0; j<  optv_.size(); j++ )
            {
                // Write sensitivities to file
                myStat <<" ["<< j <<"] ["<< i <<"] : " << SensitivityMatrix(i,j) <<" ";
            }

            myStat << endl;
        }
*/

        myStat << " Composite Scaled Sensitivities: "<<endl;
        {
            // Write sensitivities to file
            for( i=0; i< optv_.size(); i++ )
            {
                myStat <<"			parameter "<< i;
                         if (gfittask->Opti->Ptype[i] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
                         else myStat << "  " << gfittask->Opti->Ptype[i];
                myStat <<" :	     " << CompositeScaledSensitivities(i)	<< endl;
            }
        }
        myStat << endl;

//SensitivityMatrix.print("SensitivityMatrix:");

        for(i=0; i<gfittask->weights.size(); ++i)
        {
            WeightMatrix(i,i) = gfittask->weights[i];
        }

        // Get transpose of sensitivity matrix
        arma::mat SensitivityMatrix_T = arma::trans(SensitivityMatrix);

//SensitivityMatrix_T.print("SensitivityMatrix_T:");

        // Compute Fisher matrix (is a proxy for the Hessian matrix)
        arma::mat FisherMatrix = SensitivityMatrix_T * WeightMatrix * SensitivityMatrix;
        if ((gfittask->Tfun->weight == "inverr2") /*|| (gfittask->Tfun->weight == "inverr1")*/ )
        {
            FisherMatrix = SensitivityMatrix_T * sqrt(WeightMatrix) * SensitivityMatrix;
        }
        if ((gfittask->Tfun->weight == keys::inverr3) || (gfittask->Tfun->weight == keys::inverr) || (gfittask->Tfun->weight == keys::inverr_norm))
        {
            FisherMatrix = SensitivityMatrix_T * SensitivityMatrix;
        }
        FisherMatrix.print("Fisher Matrix:");

        // Compute variance-covariance matrix
        arma::mat VarCovarMatrix = 1 * arma::inv( FisherMatrix, false );
        if (gfittask->Tfun->weight == "inverr2")
        {
            VarCovarMatrix = error_variance * arma::inv( FisherMatrix, false );
        } else VarCovarMatrix = 1 * arma::inv( FisherMatrix, false );
        VarCovarMatrix.print("Variance Covariance Matrix:");

        // Print Variance-Covariance matrix to file
        myStat << " Variance-Covariance matrix: "<<endl;
        for( i=0; i< (int) optv_.size(); i++ )
        {
            if( i== 0 )
            {
                myStat <<"                                                      ";
                if (gfittask->Opti->Ptype[i] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
                else myStat << "  " << gfittask->Opti->Ptype[i];
            }
            else
            {
                myStat <<"              ";
                if (gfittask->Opti->Ptype[i] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
                else myStat << "  " << gfittask->Opti->Ptype[i];
            }
        }
        myStat << endl;
        for( i=0; i<  optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i;
                     if (gfittask->Opti->Ptype[i] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
                     else myStat << "  " << gfittask->Opti->Ptype[i];
            for( j=0; j<  optv_.size(); j++ )
            {
                if( j==0 )
                    myStat <<"          " <<VarCovarMatrix(i,j);
                else
                    myStat <<"          " <<VarCovarMatrix(i,j);
            }
            myStat << endl;
        }
        myStat << endl;

        // Correlation matrix
        arma::mat CorellationMatrix( (int) optv_.size(), (int) optv_.size() );
        CorellationMatrix.print("CorellationMatrix = ");

        for( i=0; i< optv_.size(); i++ )
        {
            for( k=i; k< optv_.size(); k++ )
            {
                // 			 			 Covar(i,k)          / sqrt( Var(k,k)            * Var(i,i)            )
                CorellationMatrix(i,k) = VarCovarMatrix(i,k) / sqrt( VarCovarMatrix(k,k) * VarCovarMatrix(i,i) );
                CorellationMatrix(k,i) = CorellationMatrix(i,k);
            }
            ParameterStandardDeviation(i) = sqrt(VarCovarMatrix(i,i));
            CoefficientOfVariation(i) 	  = ParameterStandardDeviation(i) / optv_[i];
            Parameter_t_Statistic(i) 	  = 1. / CoefficientOfVariation(i);
        }

        // Print Parameter Standard Deviations to file
        myStat << " Parameter Standard Deviation/Error: "<<endl;
        for( i=0; i< optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i;
                     if (gfittask->Opti->Ptype[i] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
                     else myStat << "  " << gfittask->Opti->Ptype[i];
                 myStat <<" :	   " << ParameterStandardDeviation(i)/sqrt(degrees_of_freedom) << endl;
        }
        myStat << endl;


        double T, w;
        int DegreesOfFreedom = number_of_measurements - number_of_parameters;
        double alpha[] = { 0.5, 0.25, 0.1, 0.05, 0.01 };
        boost::math::students_t dist( DegreesOfFreedom );
        for( j=0; j< optv_.size(); j++ )
        {
            myStat <<"			parameter "<< j;
                     if (gfittask->Opti->Ptype[j] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[j]);
                     else myStat << "  " << gfittask->Opti->Ptype[j];
            myStat << " :	           ";
            myStat << endl;
            myStat <<    "___________________________________________________________________" << endl;
            myStat <<    "Confidence       T           Interval          Lower          Upper" << endl;
            myStat <<    " Value (%)     Value          Width            Limit          Limit" << endl;
            myStat <<    "___________________________________________________________________" << endl;

            for( i = 0; i < int (sizeof(alpha)/sizeof(alpha[0])); ++i)
            {
                // Confidence value:
                myStat << fixed << setprecision(3) << setw(10) << right << 100 * (1-alpha[i]);
                // calculate T:
                T = boost::math::quantile(boost::math::complement(dist, alpha[i] / 2));
                // Print T:
                myStat << fixed << setprecision(3) << setw(10) << right << T;
                // Calculate width of interval (one sided):
                w = T * ParameterStandardDeviation(j) / sqrt(double( number_of_measurements ));
                // Print width:
                if(w < 0.01)
                    myStat << scientific << setprecision(3) << setw(17) << right << w;
                else
                    myStat << fixed << setprecision(3) << setw(17) << right << w;
                // Print Limits:
                myStat << fixed << setprecision(5) << setw(15) << right << optv_[j] - w;
                myStat << fixed << setprecision(5) << setw(15) << right << optv_[j] + w << endl;
            }
            myStat << endl;

        }

        // Print Parameter Standard Deviations to file
        myStat << " Coefficient of Variation: "<<endl;
        for( i=0; i< optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i;
                     if (gfittask->Opti->Ptype[i] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
                     else myStat << "  " << gfittask->Opti->Ptype[i];
            myStat <<" :	           " << CoefficientOfVariation(i) << endl;
        }
        myStat << endl;

        // Print Parameter Standard Deviations to file
        myStat << " Parameter t-statistic: "<<endl;
        for( i=0; i< optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i;
                     if (gfittask->Opti->Ptype[i] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
                     else myStat << "  " << gfittask->Opti->Ptype[i];
            myStat <<" :	           " << Parameter_t_Statistic(i) << endl;
        }
        myStat << endl;

        // Print Correlation matrix to file
        myStat << " Correlation matrix: "<<endl;
        for( i=0; i< (int) optv_.size(); i++ )
        {
            if( i== 0 )
            {
                myStat <<"                              ";
                if (gfittask->Opti->Ptype[i] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
                else myStat << "  " << gfittask->Opti->Ptype[i];
            }
            else
            {
                myStat <<"                  ";
                if (gfittask->Opti->Ptype[i] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
                else myStat << "  " << gfittask->Opti->Ptype[i];
            }
        }
        myStat << endl;
        for( i=0; i< optv_.size(); i++ )
        {
            myStat <<"parameter "<< i;
            if (gfittask->Opti->Ptype[i] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]);
            else myStat << "  " << gfittask->Opti->Ptype[i];
            myStat<<" : ";
            for( j=0; j< optv_.size(); j++ )
            {
                if( j==0 )
                    myStat <<"          " <<CorellationMatrix(i,j);
                else
                    myStat <<"	   		" <<CorellationMatrix(i,j);
            }
            myStat << endl;
        }
        myStat << endl;

        myStat.close();

}


void statistics::MC_confidence_interval( std::vector<double> &optv_, TGfitTask* gfittask, int &countit )
{

//cout<<"pid : "<<pid<<" entered Statistics::MC_confidence_interval ..."<<endl;

    int i,n_param,id,imc;
    int j, p_=1, pid_=0; // k
    double sum_of_squares_MC;
    std::vector<double> scatter_v, MC_computed_v;
    std::vector<double> optv_backup;
    std::vector<std::vector<double> > measured_values_backup;
    std::vector<std::vector<double> > computed_values_backup;

    int n_Rparam = gfittask->Opti->reactions.size();

    scatter_v.resize(number_of_measurements);
    MC_computed_v.resize(number_of_measurements);

    // loop over systems and backup measurement values and computed values
        // Store originals measurements and computed values
        computed_values_backup.push_back( gfittask->computed_values_v );
        measured_values_backup.push_back( gfittask->measured_values_v );
        optv_backup = optv_;

    n_param = (int) optv_.size();

    double*  MC_fitted_parameters_all_storage = (double *) malloc ( num_of_MC_runs * n_param * sizeof(double) );
    double** MC_fitted_parameters_all		  = (double **) malloc ( num_of_MC_runs * sizeof(double *) );
    for( i=0; i<num_of_MC_runs; i++ )
        MC_fitted_parameters_all[i] = &MC_fitted_parameters_all_storage[ i * n_param ];

    double*  MCR_fitted_parameters_all_storage = (double *) malloc ( num_of_MC_runs * n_Rparam * sizeof(double) );
    double** MCR_fitted_parameters_all		  = (double **) malloc ( num_of_MC_runs * sizeof(double *) );
    for( i=0; i<num_of_MC_runs; i++ )
        MCR_fitted_parameters_all[i] = &MCR_fitted_parameters_all_storage[ i * n_Rparam ];

    double* scatter_all 	  = new double[ number_of_measurements * num_of_MC_runs ];

        // mersenne twister generator
        typedef boost::mt19937 RNGType;
        RNGType rng;

        boost::normal_distribution<> rdist(0.0, SD_of_residuals);

        boost::variate_generator< RNGType, boost::normal_distribution<> > get_rand(rng, rdist);

        // for each MC run generate number_of_measurements random variables
            ofstream myScatter_all;
            string path = gpf->ResultDir() + "myScatter_all.txt";
            myScatter_all.open(path.c_str());
        for( i=0; i<(number_of_measurements * num_of_MC_runs); i++ )
        {
            scatter_all[i] = get_rand();
            myScatter_all << " scatter_all["<<i<<"] : "<< scatter_all[i] <<endl;
        }
            myScatter_all.close();


pid_ = 0;
    id = 0;
    // Perform Monte Carlo runs
    for( imc=pid_; imc<num_of_MC_runs; ++imc /*+= p*/)
    {
        sum_of_squares_MC = 0.;

        for( i=0; i<number_of_measurements; i++ )
        {
            scatter_v[i] = scatter_all[ (imc/p_ * number_of_measurements + i) ];
        }


        ofstream myScatter_pid_0;
        myScatter_pid_0.open("output_GEMSFIT/myScatter_pid_0.txt",ios::app);
        for( i=0; i<number_of_measurements; i++ )
        {
            myScatter_pid_0 << " scatter_v["<<i<<"] : "<<scatter_v[i]<<endl;
        }
        myScatter_pid_0 << endl;
        myScatter_pid_0.close();



        // loop over systems and add MC scatter to calculated values to simulate new set of experimental data
//        double* simulated_measurements = new double[gfittask->computed_values_v.size()];

        if (MCbool == 1) // the new simulated values are from scatter + computed values
            for (i=0; i<number_of_measurements; i++)
            {
                MC_computed_v[i] = scatter_v[i] + gfittask->computed_values_v[i];
            }
        else
            if (MCbool == 2) // the new simulated values are from scatter + measured values
            {
                for (i=0; i<number_of_measurements; i++)
                {
                    MC_computed_v[i] = scatter_v[i] + gfittask->measured_values_v[i];
                }
            }

        gfittask->add_MC_scatter(MC_computed_v);


//        delete[] simulated_measurements;


        // perform optimization
//            gfittask->Opti->OptLoBounds = gfittask->Opti->LB;
//            gfittask->Opti->OptUpBounds = gfittask->Opti->UB;
        optv_ = optv_backup;

        gfittask->Ainit_optim( optv_ ); //, sum_of_squares_MC);


        // Store fitted parameters of each run in process specific array
        for( j=0; j<n_param; j++ )
        {
            MC_fitted_parameters_all[ id ][ j ] = optv_[ j ];

        }

        // Store reaction parameters
        for ( j=0; j<n_Rparam; ++j)
        {
            MCR_fitted_parameters_all[ id ][ j ] = gfittask->Opti->reactions[j]->std_gibbs;
        }


        // loop over systems and subtract MC scatter to retain the original measurement data
        // Retain original computed values
        gfittask->computed_values_v = computed_values_backup[0];
        // Retain original measurements
        gfittask->measured_values_v = measured_values_backup[0];
        // puts back the original mesured values
        gfittask->add_MC_scatter(measured_values_backup[0]);

        id++;
    }// end Monte Carlo for-loop

        double StandardDeviation = 0.;
        arma::vec MCparams( num_of_MC_runs );
        arma::vec MCRparams( num_of_MC_runs );

        ofstream myStat;
        myStat.open(gpf->FITStatisticsFile().c_str(),ios::app);

        myStat << " Confidence intervals of MC parameters : "<<endl;
        myStat << " -> standard deviation of parameters generated during Monte Carlo runs "<<endl;
        for( j=0; j<n_param; j++ ) // cols
        {
            for( i=0; i<num_of_MC_runs; i++ ) // rows
            {
                MCparams(i) = MC_fitted_parameters_all[ i ][ j ];
            }
            // compute standard deviation of generated parameters
            StandardDeviation = arma::stddev( MCparams, 0 );

            // Print Standard Deviations of parameters generated during MC runs to file
            myStat <<"			parameter "<< j;
            if (gfittask->Opti->Ptype[j] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[j]);
            else myStat << "  " << gfittask->Opti->Ptype[j];
            myStat <<" :	           " << StandardDeviation << endl;

        }

        for( j=0; j<n_Rparam; j++ ) // cols
        {
            for( i=0; i<num_of_MC_runs; i++ ) // rows
            {
                MCRparams(i) = MCR_fitted_parameters_all[ i ][ j ];
            }
            // compute standard deviation of generated parameters
            StandardDeviation = arma::stddev( MCRparams, 0 );

            // Print Standard Deviations of parameters generated during MC runs to file
            myStat <<"			parameter "<< j;
            if (gfittask->Opti->Ptype[j] == "G0") myStat << "  " << gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[j]);
            else myStat << "  " << gfittask->Opti->Ptype[j];
            myStat <<" :	           " << StandardDeviation << endl;

        }
        myStat << endl;
        myStat.close();

    free (MC_fitted_parameters_all[0]);
    free (MC_fitted_parameters_all);

    delete[] scatter_all;

}// end of function MC_confidence_interval

