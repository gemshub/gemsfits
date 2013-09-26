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

// Constructor
statistics::statistics(TGfitTask *gfittask, double weighted_Tfun_sum_of_residuals_, int num_of_params_, int num_of_runs_ )
{

    number_of_measurements = 0;
    number_of_measurements += gfittask->computed_values_v.size();
    Tfun_sum_of_residuals = 0.0;
    sum_of_residuals = 0.0; weighted_sum_of_residuals = 0.0;


    weighted_Tfun_sum_of_residuals 		= weighted_Tfun_sum_of_residuals_;
    for (unsigned int i=0; i<gfittask->Tfun_residuals_v.size(); ++i)
    {
        Tfun_sum_of_residuals += gfittask->Tfun_residuals_v[i];

        sum_of_residuals += fabs(gfittask->residuals_v[i]);

        weighted_sum_of_residuals += fabs(gfittask->residuals_v[i])*gfittask->weights[i];

        weighted_residuals.push_back(gfittask->residuals_v[i]*gfittask->weights[i]);
    }

    num_of_runs		= num_of_runs_;

cout<<" Statistics Constructor: sum of squares: "<<weighted_Tfun_sum_of_residuals<<endl;

    number_of_ind_parameters   = num_of_params_;
    number_of_parameters = number_of_ind_parameters + gfittask->Opti->reactions.size() + gfittask->Opti->Lparams.size();

cout<<" Statistics Constructor: number_of_parameters: "<<number_of_parameters<<endl;

//    /// Populate data members
//    std_get_stat_param();

    get_stat_param_txt();
//stat->set_plotfit_vars_txt();

//    /// Instantiate pointer to PlotFit class to print results
//    Plot_Stat = new PlotFit();

}


// Destructor
statistics::~statistics()
{

}

// Perform basic statistical analysis
void statistics::basic_stat( std::vector<double> &optv_, TGfitTask *gfittask )
{
    // Variable declarations
    unsigned int i;
    double mean = 0.;
    double mean_res = 0.;
    double ResSumSquares = 0., TotalSumSquares = 0.;
    double Res = 0.;


    // Degrees of freedom
    degrees_of_freedom = number_of_measurements-number_of_parameters;



    // Compute standard deviation of residuals
    for (i=1; i<gfittask->Weighted_Tfun_residuals_v.size(); i++)
    {
        mean_res += gfittask->Weighted_Tfun_residuals_v[i];
    }

    mean_res = mean_res / number_of_measurements;

    for (i=1; i<gfittask->Weighted_Tfun_residuals_v.size(); i++)
    {
        Res += pow((gfittask->Weighted_Tfun_residuals_v[i]-mean_res),2);
    }


//    SD_of_residuals = sqrt((Res/degrees_of_freedom));
    SD_of_residuals = sqrt( (weighted_Tfun_sum_of_residuals/(number_of_measurements-number_of_parameters)) );

    // Modified by DM on 12.03.2012 due to errorneus calculation of R^2 when using log_solubility data.
//    // Compute R^2: coefficient of determination
//    mean = ( accumulate(gfittask->measured_values_v.begin(), gfittask->measured_values_v.end(), 0) ) / gfittask->measured_values_v.size();

    mean = 0;
    // Compute R^2: coefficient of determination
    for (i=0; i< gfittask->measured_values_v.size(); i++)
    {
        mean += gfittask->measured_values_v[i];
    }
    mean = mean / gfittask->measured_values_v.size();

    assert( gfittask->computed_values_v.size() == gfittask->measured_values_v.size() );


    for( i=0; i< gfittask->computed_values_v.size(); i++ )
    {
        ResSumSquares += pow( (gfittask->measured_values_v[i] - gfittask->computed_values_v[i]), 2);
        TotalSumSquares += pow( (gfittask->measured_values_v[i] - mean), 2);
    }

    coeff_of_determination = 1 - ResSumSquares / TotalSumSquares;


    // Pearson Chi Square test
    Pearsons_chi_square = 0.;
    for( i=0;  i< gfittask->computed_values_v.size(); i++ )
    {
        Pearsons_chi_square += (gfittask->computed_values_v[i] - gfittask->measured_values_v[i])*(gfittask->computed_values_v[i] - gfittask->measured_values_v[i]) / gfittask->measured_values_v[i];
    }


    // Reduced Chi Square
    reduced_chi_square = weighted_Tfun_sum_of_residuals / degrees_of_freedom;

    // Error variance
    error_variance = (weighted_Tfun_sum_of_residuals/(number_of_measurements-number_of_parameters));



    // Write first statistcs to file
#ifdef BOOST_MPI
        ofstream myStat;
        ostringstream pb;
        pb << proc_id_boost;
        string out_fit(gpf->OutputDirPath().c_str())
        out_fit += "_" + pb.str() + "/myFitStatistics.txt";
        myStat.open( out_fit.c_str(), ios::app );
#endif

#ifndef BOOST_MPI
        ofstream myStat;
        myStat.open(gpf->FITStatisticsFile().c_str(),ios::app);
#endif
        myStat << " # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # " << endl;
        myStat << " - - - - - - - RESULTS FROM GEMSFIT STANDARD G0 PARAMETER REGRESSION - - - - - - - " << endl;
        myStat << " # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # " << endl;
        myStat << endl;
        myStat << " References: " << endl;
        myStat << " [1] Mary C. Hill, Claire R. Tiedeman (2007) Effective Groundwater Model Calibration: With Analysis of Data, Sensitivities, Predictions, and Uncertainty. 480 pages."<<endl;
        myStat << " [2] Eileen P, Poeter & Marc C. Hill DOCUMENTATION OF UCODE, A Computer Code for Universal Inverse Modeling. http://inside.mines.edu/~epoeter/583/UCODEmanual_wrir98-4080.pdf" <<endl;
        myStat << endl;
                myStat << " Number of measurements :              	" << number_of_measurements     << endl;
        myStat << endl;
        myStat << " Number of parameters :                	" << number_of_parameters 	<< endl;
        myStat << endl;
                myStat << " Number of runs needed for regression :      " << num_of_runs 		<< endl;
        myStat << endl;
        myStat << " Degrees of freedom :                  	" << degrees_of_freedom	 	<< endl;
        myStat << endl;
        myStat << " Target function sum of residuals :                      	" << Tfun_sum_of_residuals 		<< endl;
        myStat << endl;
        myStat << " Weighted target function sum of residuals :                      	" << weighted_Tfun_sum_of_residuals 		<< endl;
        myStat << endl;
        myStat << " Sum of residuals:                   " << sum_of_residuals << endl;
        myStat << endl;
        myStat << " Weighted sum of residuals:                  " << weighted_sum_of_residuals << endl;
        myStat << endl;
        myStat << " Reduced_chi_square :                  	" << reduced_chi_square 	<< endl;
        myStat << endl;
        myStat << " Standard deviation of the target function : 	" << SD_of_residuals 		<< endl;
        myStat << endl;
                myStat << " Coefficient of determination R^2 :    	" << coeff_of_determination     << endl;
        myStat << endl;
        myStat << " Pearson's Chi Square test :           	" << Pearsons_chi_square   	<< endl;
        myStat << endl;
                myStat << " Best fit results for parameters from regression : "                         <<endl;
        for( i=0; i< optv_.size(); i++ ) // cols
        {
            // Print optimized parameter values to file
            if (gfittask->Opti->Ptype[i] =="G0")
            {
                myStat <<"          parameter G0 "<<gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i])<<" :           " << optv_[i] << endl;
//                cout /*<< gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i])*/ << endl;
            } else
            myStat <<"          parameter "<<gfittask->Opti->Ptype[i]<<" :          " << optv_[i] << endl;

        }

        if (gfittask->Opti->h_RDc)
        {
            for (i=0; i<gfittask->Opti->reactions.size(); ++i)
            {
                myStat <<"          Reac parameter "<<gfittask->Opti->reactions[i]->Dc_name <<" :           "<<gfittask->Opti->reactions[i]->std_gibbs<<endl;
            }
        }

        if (gfittask->Opti->h_Lp)
        {
            for (i=0; i<gfittask->Opti->Lparams.size(); ++i)
            {
                myStat <<"          Linked parameter "<<gfittask->Opti->Lparams[i]->name <<" :           "<<gfittask->Opti->Lparams[i]->EV<<endl;
            }
        }

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

        // Generate systematic variations in the G0 values from the optimization vector and loop over them

            // Set printfile bool to true in order to deactivate to parallelization of the loop over measurements within the StdState_gems3k_wrap() function
//            gfittask->printfile = true;

//            for( i=0; i< (int) optv_.size(); i++ )
//            {
//                for( j=0; j< sensitivity_points; j++ )
//                {
//                    // Copy original values into opt_scan
//                    opt_scan = optv_;

//                    // add/subtract up to 25% of the original parameter value
//                    opt_scan[i] = optv_[i] + optv_[i] * ( j - sensitivity_points/2  ) / 200;

//                    // compute sum of squared residuals
//                    residual_sys = 0.;
//                    gems3k_wrap( residual_sys, opt_scan, gfittask );
////cout<<"residual_sys = "<<residual_sys<<endl;
//                    opt_scan_v[j] = opt_scan[i];
//                    ssr_param[j]  = residual_sys;
//                }
//                // add rows with 'sensitivity_points' columns
//                array_ssr.push_back( opt_scan_v );
//                array_ssr.push_back( ssr_param );
//            }


//            print_vectors_curve( optv_, array_ssr, sensitivity_points );
        /// adding to statistics the reaction and linked paramaters
//        gfittask->Opti->h_Lp = false;
//        gfittask->Opti->h_RDc = false;
//        for (i = 0; i<gfittask->Opti->reactions.size(); ++i)
//        {
//            optv_.push_back(gfittask->Opti->reactions[i]->std_gibbs);
//            gfittask->Opti->Pindex.push_back(gfittask->Opti->reactions[i]->DcIndex);
//            gfittask->Opti->Ptype.push_back("G0");
//            gfittask->Opti->opt.push_back(gfittask->Opti->reactions[i]->std_gibbs);
////            gfittask->Opti->opt.push_back(gfittask->Opti->reactions[i]->IV);
//        }
//        for (i = 0; i<gfittask->Opti->Lparams.size(); ++i)
//        {
//            optv_.push_back(gfittask->Opti->Lparams[i]->EV);
////            gfittask->Opti->Pindex.push_back(gfittask->Opti->reactions[i]->DcIndex);
////            gfittask->Opti->Ptype.push_back("bIC");
////            gfittask->Opti->opt.push_back(gfittask->Opti->reactions[i]->std_gibbs);

//        }

        len_meas = (int) gfittask->Weighted_Tfun_residuals_v.size();

//        // Compute Jacobian matrix (= Sensitivity matrix)
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

                OnePercentScaledSensitivities(k,i) = (SensitivityMatrix(k,i)) * optv_[i] / 100;

                if (gfittask->Tfun->weight == "inverr")
                {
                    DimensionlessScaledSensitivities(k,i) = SensitivityMatrix(k,i) * fabs( optv_[i] ) * gfittask->weights[k];
                } else
                    if ((gfittask->Tfun->weight == "inverr2") || (gfittask->Tfun->weight == "inverr3"))
                    {
                        DimensionlessScaledSensitivities(k,i) = SensitivityMatrix(k,i) * fabs( optv_[i] ) * sqrt(gfittask->weights[k]);
                    } else  DimensionlessScaledSensitivities(k,i) = SensitivityMatrix(k,i) * fabs( optv_[i] ) * 1;
                CompositeScaledSensitivities(i)      += sqrt( DimensionlessScaledSensitivities(k,i)*DimensionlessScaledSensitivities(k,i)/len_meas );
            }
        }

        ofstream myStat;
        myStat.open(gpf->FITStatisticsFile().c_str(),ios::app);



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


        myStat << " Composite Scaled Sensitivities: "<<endl;
        myStat << " See ref. [1] section 4.3.4. If there is no weighting the weight is asumed 1 for all measured values. "<<endl;
        myStat << " Larger values indicate parameters for which observations provide more information. \n A value less than 1 or less than (or close to) 1% of the highest value, means that the parameter is poorly estimated. \n These parameters should be fixed during regresion. "<<endl;
        for( i=0; i< optv_.size(); i++ )
        {
            // Write sensitivities to file
            myStat <<"			parameter "<< i <<" "<<gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]) <<" :	           " << CompositeScaledSensitivities(i)	<< endl;
        }
        myStat << endl;

        // Plot results with DISLIN
//        print_sensitivity( (int) computed_up.size(), (int) optv_.size(), SensitivityMatrix , gfittask->sysdata->molality_1 );


//SensitivityMatrix.print("SensitivityMatrix:");

        for(i=0; i<gfittask->weights.size(); ++i)
        {
            WeightMatrix(i,i) = gfittask->weights[i];
        }

        // Get transpose of sensitivity matrix
        arma::mat SensitivityMatrix_T = arma::trans(SensitivityMatrix);

//SensitivityMatrix_T.print("SensitivityMatrix_T:");

        // Compute Fisher matrix (is a proxy for the Hessian matrix)
        arma::mat FisherMatrix = SensitivityMatrix_T * /*WeightMatrix **/ SensitivityMatrix;
        if ((gfittask->Tfun->weight == "inverr2") /*|| (gfittask->Tfun->weight == "inverr1")*/ )
        {
            FisherMatrix = SensitivityMatrix_T */* WeightMatrix **/ SensitivityMatrix;
        }

FisherMatrix.print("Fisher Matrix:");

        // Compute variance-covariance matrix
        arma::mat VarCovarMatrix = 1 * arma::inv( FisherMatrix, false );
        if (gfittask->Tfun->weight == "inverr2")
        {
            VarCovarMatrix = error_variance * arma::inv( FisherMatrix, false );
        } else VarCovarMatrix = 1 * arma::inv( FisherMatrix, false );


VarCovarMatrix.print("Variance Covariance Matrix:");

        // Correlation matrix
        arma::mat CorellationMatrix( (int) optv_.size(), (int) optv_.size() );

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
        CorellationMatrix.print("CorellationMatrix = ");

CorellationMatrix.print("Corellation Matrix:");

        // Print Variance-Covariance matrix to file
        myStat << " Variance-Covariance matrix: "<<endl;
        myStat << " See ref. [1] section 7.2. V(b) = s^2([X]trans*[X])^1; \n [W] - weight matrix is considered as 1 as there is no full weight matrix calculted, based on true experimental error (not provided for all experiments). \n X - sensitivity matrix " <<endl;
        myStat << " When the chosen weight is other than inverr2 (1/sigma^2) error variance (s^2) is assumed 1. \n For inverr2 the error variance is calculated as weighted_Tfun_sum_of_residuals/degrees of freedom." << endl;
        for( i=0; i<  optv_.size(); i++ )
        {
            if( i== 0 )
                myStat <<"								parameter "<< i;
            else
                myStat <<"			parameter "<< i;
        }
        myStat << endl;
        for( i=0; i<  optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i <<" : ";
            for( j=0; j<  optv_.size(); j++ )
            {
                if( j==0 )
                    myStat <<"			   " <<VarCovarMatrix(i,j);
                else
                    myStat <<"		    " <<VarCovarMatrix(i,j);
            }
            myStat << endl;
        }
        myStat << endl;

        // Print Parameter Standard Deviations to file
        myStat << " Parameter Standard Deviations/Errors: "<<endl;
        for( i=0; i< optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i <<" "<<gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[i]) <<" :	           " << ParameterStandardDeviation(i) << endl;
        }
        myStat << endl;


        double T, w;
        int DegreesOfFreedom = number_of_measurements - number_of_parameters;
        double alpha[] = { 0.5, 0.25, 0.1, 0.05, 0.01 };
        boost::math::students_t dist( DegreesOfFreedom );
        for( j=0; j< optv_.size(); j++ )
        {
            myStat <<"			parameter "<< j <<" "<<gfittask->NodT[0]->xCH_to_DC_name(gfittask->Opti->Pindex[j]) << " :	           ";
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
            myStat <<"			parameter "<< i <<" :	           " << CoefficientOfVariation(i) << endl;
        }
        myStat << endl;

        // Print Parameter Standard Deviations to file
        myStat << " Parameter t-statistic: "<<endl;
        for( i=0; i< optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i <<" :	           " << Parameter_t_Statistic(i) << endl;
        }
        myStat << endl;

        // Print Correlation matrix to file
        myStat << " Correlation matrix: "<<endl;
        for( i=0; i< optv_.size(); i++ )
        {
            if( i== 0 )
                myStat <<"          parameter "<< i;
            else
                myStat <<"parameter "<< i;
        }
        myStat << endl;
        for( i=0; i< optv_.size(); i++ )
        {
            myStat <<"parameter "<< i <<" : ";
            for( j=0; j< optv_.size(); j++ )
            {
                if( j==0 )
                    myStat <<"	   " <<CorellationMatrix(i,j);
                else
                    myStat <<"	   		" <<CorellationMatrix(i,j);
            }
            myStat << endl;
        }
        myStat << endl;

        myStat.close();


//        // allocate synamic 4D memory for holding parameter-pair specific SSR with varying parameters
//        int points_per_axis = 100;
//        float* SSR_surface;
//        SSR_surface = new float [ points_per_axis * points_per_axis ];
//        double* param_1_ray;
//        double* param_2_ray;
//        param_1_ray = new double [ points_per_axis ];
//        param_2_ray = new double [ points_per_axis ];
//        int param_pair[2];

//        for( i=0; i< (int) optv_.size()-1; i++ )			// loop over
//        {													// first parameter and
//            for( k=i+1; k< (int) optv_.size(); k++ )		// loop over
//            {												// second parameter and
//                for( jj=0; jj<points_per_axis; jj++ )		// vary +- 25%
//                {
//                    for( ll=0; ll<points_per_axis; ll++ )	// vary +- 25%
//                    {
//                        j = jj * 5;
//                        l = ll * 5;

//                        // parameter 1: add/subtract up to 25% of the original parameter value
//                        opt_scan[i] = optv_[i] + optv_[i] * ( 0.5 / points_per_axis * jj - 0.25 );
//                        param_1_ray[jj] = opt_scan[i];

//                        // parameter 2: add/subtract up to 25% of the original parameter value
//                        opt_scan[k] = optv_[k] + optv_[k] * ( 0.5 / points_per_axis * ll - 0.25 );
//                        param_2_ray[ll] = opt_scan[k];

//                        residual_sys = 0.;
//                        opti::StdState_gems3k_wrap( residual_sys, opt_scan, gfittask );

//                        // Compute and store values of sruface matrix
//                        SSR_surface[ points_per_axis*jj + ll ] 	 = residual_sys ;
//                    }
//                }
//                // Print a contour plot for every parameter pair
//                param_pair[0] = i;
//                param_pair[1] = k;

//                print_SSR_contour( param_pair, (int) optv_.size(), points_per_axis, SSR_surface, param_1_ray, param_2_ray );
//            }
//        }


//        // free dynamicaly allocated memory
//        delete[] SSR_surface;
//        delete[] param_1_ray;
//        delete[] param_2_ray;




//exit(1);//STOP

        // In case later on parallelization of measurement over for loop is wished, set printfile bool to false


}



