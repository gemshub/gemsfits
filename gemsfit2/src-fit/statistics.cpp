#include "statistics.h"
#include "gemsfit_global_functions.h"
#include <armadillo>
#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/chi_squared.hpp>

// Constructor
statistics::statistics( TGfitTask *gfittask, double sum_of_squares_, int num_of_params_, int num_of_runs_ )
{

    number_of_measurements = 0;
    number_of_measurements += gfittask->computed_values_v.size();


    sum_of_squares 		= sum_of_squares_;

    num_of_runs		= num_of_runs_;

cout<<" Statistics Constructor: sum of squares: "<<sum_of_squares<<endl;

    number_of_parameters   = num_of_params_;

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
    int i;
    double mean = 0.;
    double mean_res = 0.;
    double ResSumSquares = 0., TotalSumSquares = 0.;
    double Res = 0.;


    // Degrees of freedom
    degrees_of_freedom = number_of_measurements-number_of_parameters;


    // Compute standard deviation of residuals
    for (i=1; i<gfittask->computed_residuals_v.size(); i++)
    {
        mean_res += gfittask->computed_residuals_v[i];
    }

    mean_res = mean_res / number_of_measurements;

    for (i=1; i<gfittask->computed_residuals_v.size(); i++)
    {
        Res += pow((gfittask->computed_residuals_v[i]-mean_res),2);
    }


//    SD_of_residuals = sqrt((Res/degrees_of_freedom));
    SD_of_residuals = sqrt( (sum_of_squares/(number_of_measurements-number_of_parameters)) );


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


    for( i=0; i< (int) gfittask->computed_values_v.size(); i++ )
    {
        ResSumSquares += pow( (gfittask->measured_values_v[i] - gfittask->computed_values_v[i]), 2);
        TotalSumSquares += pow( (gfittask->measured_values_v[i] - mean), 2);
    }

    coeff_of_determination = 1 - ResSumSquares / TotalSumSquares;


    // Pearson Chi Square test
    Pearsons_chi_square = 0.;
    for( i=0;  i< (int) gfittask->computed_values_v.size(); i++ )
    {
        Pearsons_chi_square += (gfittask->computed_values_v[i] - gfittask->measured_values_v[i])*(gfittask->computed_values_v[i] - gfittask->measured_values_v[i]) / gfittask->measured_values_v[i];
    }


    // Reduced Chi Square
    reduced_chi_square = sum_of_squares / degrees_of_freedom;


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
                myStat << " Number of measurements :              	" << number_of_measurements     << endl;
        myStat << endl;
        myStat << " Number of parameters :                	" << number_of_parameters 	<< endl;
        myStat << endl;
                myStat << " Number of runs needed for regression :      " << num_of_runs 		<< endl;
        myStat << endl;
        myStat << " Degrees of freedom :                  	" << degrees_of_freedom	 	<< endl;
        myStat << endl;
        myStat << " Sum of squares :                      	" << sum_of_squares 		<< endl;
        myStat << endl;
        myStat << " Reduced_chi_square :                  	" << reduced_chi_square 	<< endl;
        myStat << endl;
        myStat << " Standard deviation of the residuals : 	" << SD_of_residuals 		<< endl;
        myStat << endl;
                myStat << " Coefficient of determination R^2 :    	" << coeff_of_determination     << endl;
        myStat << endl;
        myStat << " Pearson's Chi Square test :           	" << Pearsons_chi_square   	<< endl;
        myStat << endl;
                myStat << " Best fit results for parameters from regression : "                         <<endl;
        for( i=0; i< (int) optv_.size(); i++ ) // cols
        {
            // Print optimized parameter values to file
            myStat <<"          parameter "<< i <<" : 	           " << optv_[i] << endl;

        }

        if (gfittask->Opti->h_RDc)
        {
            for (i=0; i<gfittask->Opti->reactions.size(); ++i)
            {
                myStat <<"          Reac parameter "<<i <<" :               "<<gfittask->Opti->reactions[i]->std_gibbs<<endl;
            }
        }
        // Reaction constrained parameter
//            myStat << " Reaction constrained parameter: "  <<endl;
//            for ( i=0; i < gfittask->reaction.size(); ++i )
//            {
//                myStat <<" " <<gfittask->reaction[i]->rdc_species[gfittask->reaction[i]->rdc_species.size()-1] <<" : " << gfittask->reaction[i]->std_gibbs <<endl;
//            }

        myStat << endl;
        myStat.close();
}


// perform sensitivity analysis of selected parameters
void statistics::sensitivity_correlation( const std::vector<double> &optv_, TGfitTask* gfittask )
{
        int i, j, k, l, jj, ll, len_meas;
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

            for( i=0; i< (int) optv_.size(); i++ )
            {
                for( j=0; j< sensitivity_points; j++ )
                {
                    // Copy original values into opt_scan
                    opt_scan = optv_;

                    // add/subtract up to 25% of the original parameter value
                    opt_scan[i] = optv_[i] + optv_[i] * ( j - sensitivity_points/2  ) / 200;

                    // compute sum of squared residuals
                    residual_sys = 0.;
                    gems3k_wrap( residual_sys, opt_scan, gfittask );
//cout<<"residual_sys = "<<residual_sys<<endl;
                    opt_scan_v[j] = opt_scan[i];
                    ssr_param[j]  = residual_sys;
                }
                // add rows with 'sensitivity_points' columns
                array_ssr.push_back( opt_scan_v );
                array_ssr.push_back( ssr_param );
            }


//            print_vectors_curve( optv_, array_ssr, sensitivity_points );

        len_meas = (int) gfittask->experiments.size();

//        // Compute Jacobian matrix (= Sensitivity matrix)
        arma::mat SensitivityMatrix( len_meas, (int) optv_.size() );
        arma::mat DimensionlessScaledSensitivities( len_meas, (int) optv_.size() );
        arma::vec CompositeScaledSensitivities = arma::zeros<arma::vec>( (int) optv_.size(), 1 );
        arma::vec ParameterStandardDeviation = arma::zeros<arma::vec>( (int) optv_.size(), 1 );
        arma::vec CoefficientOfVariation = arma::zeros<arma::vec>( (int) optv_.size(), 1 );
        arma::vec Parameter_t_Statistic = arma::zeros<arma::vec>( (int) optv_.size(), 1 );


        double delta = 0.001;
        for( i=0; i< (int) optv_.size(); i++ )
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
//                DimensionlessScaledSensitivities(k,i) = SensitivityMatrix(k,i) * fabs( optv_[i] ) * sqrt( 1.0 / gfittask->data_meas->allexp[k]->error_sol[0] );
//                CompositeScaledSensitivities(i)      += sqrt( DimensionlessScaledSensitivities(k,i)*DimensionlessScaledSensitivities(k,i)/len_meas );
            }
        }

        ofstream myStat;
        myStat.open(gpf->FITStatisticsFile().c_str(),ios::app);



/*		myStat << " Sensitivity matrix over each measurement point: "<<endl;
        for( i=0; i< (int) optv_.size(); i++ )
        {
            for( j=0; j< (int) computed_up.size(); j++ )
            {
                // Write sensitivities to file
                myStat <<" "<< i <<" "<< j <<" :	" << SensitivityMatrix(j,i)	<< endl;
            }
        }
*/
//        myStat << " Composite Scaled Sensitivities: "<<endl;
//        for( i=0; i< (int) optv_.size(); i++ )
//        {
//            // Write sensitivities to file
//            myStat <<"			parameter "<< i <<" :	           " << CompositeScaledSensitivities(i)	<< endl;
//        }
//        myStat << endl;

        // Plot results with DISLIN
//        print_sensitivity( (int) computed_up.size(), (int) optv_.size(), SensitivityMatrix , gfittask->sysdata->molality_1 );


//SensitivityMatrix.print("SensitivityMatrix:");

        // Get transpose of sensitivity matrix
        arma::mat SensitivityMatrix_T = arma::trans(SensitivityMatrix);

SensitivityMatrix_T.print("SensitivityMatrix_T:");

        // Compute Fisher matrix (is a proxy for the Hessian matrix)
        arma::mat FisherMatrix = SensitivityMatrix_T * SensitivityMatrix;

FisherMatrix.print("Fisher Matrix:");

        // Compute variance-covariance matrix
        arma::mat VarCovarMatrix = SD_of_residuals * arma::inv( FisherMatrix, false );

VarCovarMatrix.print("Variance Covariance Matrix:");

        // Correlation matrix
        arma::mat CorellationMatrix( (int) optv_.size(), (int) optv_.size() );

        for( i=0; i<(int) optv_.size(); i++ )
        {
            for( k=i; k<(int) optv_.size(); k++ )
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
        for( i=0; i< (int) optv_.size(); i++ )
        {
            if( i== 0 )
                myStat <<"								parameter "<< i;
            else
                myStat <<"			parameter "<< i;
        }
        myStat << endl;
        for( i=0; i< (int) optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i <<" : ";
            for( j=0; j< (int) optv_.size(); j++ )
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
        for( i=0; i< (int) optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i <<" :	           " << ParameterStandardDeviation(i) << endl;
        }
        myStat << endl;


        double T, w;
        int DegreesOfFreedom = number_of_measurements - number_of_parameters;
        double alpha[] = { 0.5, 0.25, 0.1, 0.05, 0.01 };
        boost::math::students_t dist( DegreesOfFreedom );
        for( j=0; j< (int) optv_.size(); j++ )
        {
            myStat <<"			parameter "<< j << " :	           ";
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
        for( i=0; i< (int) optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i <<" :	           " << CoefficientOfVariation(i) << endl;
        }
        myStat << endl;

        // Print Parameter Standard Deviations to file
        myStat << " Parameter t-statistic: "<<endl;
        for( i=0; i< (int) optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i <<" :	           " << Parameter_t_Statistic(i) << endl;
        }
        myStat << endl;

        // Print Correlation matrix to file
        myStat << " Correlation matrix: "<<endl;
        for( i=0; i< (int) optv_.size(); i++ )
        {
            if( i== 0 )
                myStat <<"								parameter "<< i;
            else
                myStat <<"			parameter "<< i;
        }
        myStat << endl;
        for( i=0; i< (int) optv_.size(); i++ )
        {
            myStat <<"			parameter "<< i <<" : ";
            for( j=0; j< (int) optv_.size(); j++ )
            {
                if( j==0 )
                    myStat <<"			   " <<CorellationMatrix(i,j);
                else
                    myStat <<"		   			" <<CorellationMatrix(i,j);
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



