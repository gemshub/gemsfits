#include "statistics.h"

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

//stat->std_get_stat_param_txt();
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


    SD_of_residuals = sqrt((Res/degrees_of_freedom));
    //SD_of_residuals = sqrt( (sum_of_squares/(number_of_measurements-number_of_parameters)) );


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
            myStat <<"			parameter "<< i <<" : 	           " << optv_[i] << endl;

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

