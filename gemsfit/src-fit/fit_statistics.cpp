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
 *	@file fit_statistics.cpp
 *
 *	@brief this source file contains the implementation of the Statistics class, 
 *	which evaluates the fitted parameters and performs statistcal analysis.   
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */

#include <fstream>

#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>

#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"    
#include <boost/random/normal_distribution.hpp>

#include "fit_statistics.h"


// Constructor
StdStatistics::StdStatistics( vector<SS_System_Properties*> *systems, double sum_of_squares_, int num_of_params_, int num_of_runs_ )
{

    number_of_measurements = 0;
    for( int i=0; i< (int) systems->size(); i++)
    {
        number_of_measurements += systems->at(i)->computed_values_v.size();
    }

    sum_of_squares 		= sum_of_squares_;

    num_of_runs		= num_of_runs_;

cout<<"pid : "<<pid<<" Statistics Constructor: sum of squares: "<<sum_of_squares<<endl;

    number_of_parameters   = num_of_params_;

cout<<"pid : "<<pid<<" Statistics Constructor: number_of_parameters: "<<number_of_parameters<<endl;

    /// Populate data members
    std_get_stat_param();

    /// Instantiate pointer to PlotFit class to print results
    Plot_Stat = new PlotFit();

}


// Destructor
StdStatistics::~StdStatistics()
{
    delete Plot_Stat;

}


void StdStatistics::std_get_stat_param( )
{
        string OptParamFile("SS_INPUT/SS_GEMSFIT_input.dat");
        int pos_start, pos_end, i;
        ifstream param_stream;
        vector<string> data;
        string line, allparam;
        string s1("<StatMCruns>");
        string s2("<StatSensitivity>");
        string s3("<StatMCbars>");
        string s4("<StatMCbool>");

        string hash("#");

        // Open configuration file for GEMSFIT run
        param_stream.open(OptParamFile.c_str());
        if( param_stream.fail() )
        {
            cout << "Opening of file "<<OptParamFile<<" failed !!"<<endl;
            exit(1);
        }
        while( getline(param_stream, line) )
        {
            data.push_back(line);
        }
        param_stream.close();
        for( i=0; i < (int) data.size(); i++ )
            allparam += data[i];


        // Number of Monte Carlo runs for confidence interval evaluation
        string nMC_s, sub_nMC;
        pos_start = allparam.find(s1);
        pos_end   = allparam.find(hash,pos_start);
        nMC_s = allparam.substr((pos_start+s1.length()),(pos_end-pos_start-s1.length()));
        istringstream nMC_ss(nMC_s);
            nMC_ss >> sub_nMC;
        num_of_MC_runs = atoi(sub_nMC.c_str());
cout<<"num_of_MC_runs = "<<num_of_MC_runs<<endl;


        // Number of evaluations points per parameter for sensitivity evaluation
        string nSens_s, sub_nSens;
        pos_start = allparam.find(s2);
        pos_end   = allparam.find(hash,pos_start);
        nSens_s = allparam.substr((pos_start+s2.length()),(pos_end-pos_start-s2.length()));
        istringstream nSens_ss(nSens_s);
            nSens_ss >> sub_nSens;
        sensitivity_points = atoi(sub_nSens.c_str());
cout<<"sensitivity_points = "<<sensitivity_points<<endl;


        // Number of bars for the histogram which displays the results from Monte Carlo confidence interval generation
        string nMCbars_s, sub_nMCbars;
        pos_start = allparam.find(s3);
        pos_end   = allparam.find(hash,pos_start);
        nMCbars_s = allparam.substr((pos_start+s3.length()),(pos_end-pos_start-s3.length()));
        istringstream nMCbars_ss(nMCbars_s);
            nMCbars_ss >> sub_nMCbars;
        MC_number_of_bars = atoi(sub_nMCbars.c_str());
cout<<"MC_number_of_bars = "<<MC_number_of_bars<<endl;


        // Perform Monte Carlo runs yes (1) / no (0)
        string nMCb_s, sub_nMCb;
        pos_start = allparam.find(s4);
        pos_end   = allparam.find(hash,pos_start);
        nMCb_s = allparam.substr((pos_start+s4.length()),(pos_end-pos_start-s4.length()));
        istringstream nMCb_ss(nMCb_s);
            nMCb_ss >> sub_nMCb;
        MCbool = atoi(sub_nMCb.c_str());
cout<<"MCbool = "<<MCbool<<endl;


        param_stream.close();
}



// Perform basic statistical analysis
void StdStatistics::std_basic_stat( std::vector<double> &optv_, std::vector<SS_System_Properties*> *systems )
{
    // Variable declarations
    int i;
    double mean = 0.;
    double ResSumSquares = 0., TotalSumSquares = 0.;


    // Degrees of freedom
    degrees_of_freedom = number_of_measurements-number_of_parameters;


    // Compute standard deviation of residuals
    SD_of_residuals = sqrt( (sum_of_squares/(number_of_measurements-number_of_parameters)) );


    // Compute R^2: coefficient of determination
    mean = ( accumulate(systems->at(0)->measured_values_v.begin(), systems->at(0)->measured_values_v.end(), 0) ) / systems->at(0)->measured_values_v.size();

    assert( systems->at(0)->computed_values_v.size() == systems->at(0)->measured_values_v.size() );


    for( i=0; i< (int) systems->at(0)->computed_values_v.size(); i++ )
    {
        ResSumSquares += pow( (systems->at(0)->measured_values_v[i] - systems->at(0)->computed_values_v[i]), 2);
        TotalSumSquares += pow( (systems->at(0)->measured_values_v[i] - mean), 2);
    }

    coeff_of_determination = 1 - ResSumSquares / TotalSumSquares;


    // Pearson Chi Square test
    Pearsons_chi_square = 0.;
    for( i=0;  i< (int) systems->at(0)->computed_values_v.size(); i++ )
    {
        Pearsons_chi_square += (systems->at(0)->computed_values_v[i] - systems->at(0)->measured_values_v[i])*(systems->at(0)->computed_values_v[i] - systems->at(0)->measured_values_v[i]) / systems->at(0)->measured_values_v[i];
    }


    // Reduced Chi Square
    reduced_chi_square = sum_of_squares / degrees_of_freedom;


    // Write first statistcs to file
    if( !pid )
    {

#ifdef BOOST_MPI
        ofstream myStat;
        ostringstream pb;
        pb << proc_id_boost;
        string out_fit("./output_GEMSFIT")
        out_fit += "_" + pb.str() + "/myFitStatistics.txt";
        myStat.open( out_fit.c_str(), ios::app );
#endif

#ifndef BOOST_MPI
        ofstream myStat;
        myStat.open("output_GEMSFIT/myFitStatistics.txt",ios::app);
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
            myStat <<"			parameter "<< i <<" :	           " << optv_[i] << endl;

        }
        myStat << endl;
        myStat.close();
    }

}


// Constructor
Statistics::Statistics( vector<System_Properties*> *systems, double sum_of_squares_, int num_of_params_, int num_of_runs_ )
{

	number_of_measurements = 0;
	for( int i=0; i< (int) systems->size(); i++)
	{
		number_of_measurements += systems->at(i)->computed_values_v.size();
	}

	sum_of_squares 		= sum_of_squares_;

	num_of_runs		= num_of_runs_;

cout<<"pid : "<<pid<<" Statistics Constructor: sum of squares: "<<sum_of_squares<<endl;
	
	number_of_parameters   = num_of_params_;

cout<<"pid : "<<pid<<" Statistics Constructor: number_of_parameters: "<<number_of_parameters<<endl;

	/// Populate data members
	get_stat_param();

	/// Instantiate pointer to PlotFit class to print results
	Plot_Stat = new PlotFit();

}


// Destructor
Statistics::~Statistics()
{
	delete Plot_Stat;

}


void Statistics::get_stat_param( )
{
		string OptParamFile("GEMSFIT_input.dat");
		int pos_start, pos_end, i;
	 	ifstream param_stream;
		vector<string> data;
		string line, allparam;
		string s1("<StatMCruns>");
		string s2("<StatSensitivity>");
		string s3("<StatMCbars>");
		string s4("<StatMCbool>");

		string hash("#");

		// Open configuration file for GEMSFIT run
		param_stream.open(OptParamFile.c_str());	
		if( param_stream.fail() )
		{
			cout << "Opening of file "<<OptParamFile<<" failed !!"<<endl;
			exit(1);
		}
		while( getline(param_stream, line) ) 
		{
			data.push_back(line);
		} 
		param_stream.close();
		for( i=0; i < (int) data.size(); i++ )
			allparam += data[i];


		// Number of Monte Carlo runs for confidence interval evaluation
		string nMC_s, sub_nMC;
		pos_start = allparam.find(s1);
		pos_end   = allparam.find(hash,pos_start);
		nMC_s = allparam.substr((pos_start+s1.length()),(pos_end-pos_start-s1.length()));
		istringstream nMC_ss(nMC_s);
	    	nMC_ss >> sub_nMC;
		num_of_MC_runs = atoi(sub_nMC.c_str());
cout<<"num_of_MC_runs = "<<num_of_MC_runs<<endl;


		// Number of evaluations points per parameter for sensitivity evaluation
		string nSens_s, sub_nSens;
		pos_start = allparam.find(s2);
		pos_end   = allparam.find(hash,pos_start);
		nSens_s = allparam.substr((pos_start+s2.length()),(pos_end-pos_start-s2.length()));
		istringstream nSens_ss(nSens_s);
	    	nSens_ss >> sub_nSens;
		sensitivity_points = atoi(sub_nSens.c_str());
cout<<"sensitivity_points = "<<sensitivity_points<<endl;


		// Number of bars for the histogram which displays the results from Monte Carlo confidence interval generation
		string nMCbars_s, sub_nMCbars;
		pos_start = allparam.find(s3);
		pos_end   = allparam.find(hash,pos_start);
		nMCbars_s = allparam.substr((pos_start+s3.length()),(pos_end-pos_start-s3.length()));
		istringstream nMCbars_ss(nMCbars_s);
	    	nMCbars_ss >> sub_nMCbars;
		MC_number_of_bars = atoi(sub_nMCbars.c_str());
cout<<"MC_number_of_bars = "<<MC_number_of_bars<<endl;


		// Perform Monte Carlo runs yes (1) / no (0)
		string nMCb_s, sub_nMCb;
		pos_start = allparam.find(s4);
		pos_end   = allparam.find(hash,pos_start);
		nMCb_s = allparam.substr((pos_start+s4.length()),(pos_end-pos_start-s4.length()));
		istringstream nMCb_ss(nMCb_s);
	    	nMCb_ss >> sub_nMCb;
		MCbool = atoi(sub_nMCb.c_str());
cout<<"MCbool = "<<MCbool<<endl;


		param_stream.close();
}



// Perform basic statistical analysis
void Statistics::basic_stat( std::vector<double> &optv_, std::vector<System_Properties*> *systems )
{
	// Variable declarations
	int i;
	double mean = 0.;
	double ResSumSquares = 0., TotalSumSquares = 0.;

	
	// Degrees of freedom
	degrees_of_freedom = number_of_measurements-number_of_parameters;


	// Compute standard deviation of residuals
 	SD_of_residuals = sqrt( (sum_of_squares/(number_of_measurements-number_of_parameters)) );


	// Compute R^2: coefficient of determination
	mean = ( accumulate(systems->at(0)->measured_values_v.begin(), systems->at(0)->measured_values_v.end(), 0) ) / systems->at(0)->measured_values_v.size();

	assert( systems->at(0)->computed_values_v.size() == systems->at(0)->measured_values_v.size() );


	for( i=0; i< (int) systems->at(0)->computed_values_v.size(); i++ )	
	{	
		ResSumSquares += pow( (systems->at(0)->measured_values_v[i] - systems->at(0)->computed_values_v[i]), 2);
		TotalSumSquares += pow( (systems->at(0)->measured_values_v[i] - mean), 2);
	}	

	coeff_of_determination = 1 - ResSumSquares / TotalSumSquares;


	// Pearson Chi Square test
	Pearsons_chi_square = 0.;
	for( i=0;  i< (int) systems->at(0)->computed_values_v.size(); i++ )
	{
		Pearsons_chi_square += (systems->at(0)->computed_values_v[i] - systems->at(0)->measured_values_v[i])*(systems->at(0)->computed_values_v[i] - systems->at(0)->measured_values_v[i]) / systems->at(0)->measured_values_v[i];	
	}

	
	// Reduced Chi Square
	reduced_chi_square = sum_of_squares / degrees_of_freedom;	


	// Write first statistcs to file
	if( !pid )
	{

#ifdef BOOST_MPI		
		ofstream myStat;
		ostringstream pb; 
		pb << proc_id_boost;
		string out_fit("./output_GEMSFIT")
		out_fit += "_" + pb.str() + "/myFitStatistics.txt";
		myStat.open( out_fit.c_str(), ios::app );
#endif

#ifndef BOOST_MPI
		ofstream myStat;
		myStat.open("output_GEMSFIT/myFitStatistics.txt",ios::app);
#endif
		myStat << " # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # " << endl;  		
		myStat << " - - - - - - - RESULTS FROM GEMSFIT PARAMETER REGRESSION - - - - - - - " << endl;  
		myStat << " # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # " << endl;  		
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
			myStat <<"			parameter "<< i <<" :	           " << optv_[i] << endl;

		}			
		myStat << endl;
		myStat.close();             
	}

}



// Plot residuals
void Statistics::plot_residuals( std::vector<double> &optv_, std::vector<System_Properties*> *systems )
{
		double temp, pres, mola, mean;	
		double m2 = 0., m3 = 0., m4 = 0.;
		double sqrtb1, b2, Y, beta2_sqrtb1, W2, delta, alpha, Z_sqrtb1, E_b2, Var_b2, x, sqrt_beta1_b2, A, Z_b2, K2, K2test;	
    	std::vector<double> p_row;
		p_row.resize(3);
		std::vector< std::vector<double> > plot_array;
		int len = 0;
		int i, j, code;
		double residual_sys;
		len = (int) systems->at(0)->computed_values_v.size();
		vector<double> residuals_v;
		double q;
		vector<double> percentiles_v;
		vector<double> quantiles_v;

		if( !pid )
		{
			// Set printfile bool to true in order to deactivate to parallelization of the loop over measurements within the ActMod_tsolmod_wrap() function
			systems->at(0)->printfile = true;
			residual_sys = 0.;	
			opti::ActMod_tsolmod_wrap( residual_sys, optv_, systems->at(0) );
		}	

		// In case later on parallelization of measurement over for loop is wished, set printfile bool to false
		systems->at(0)->printfile = false;
		
		bool stop;
		// check for values that want to be printed
		for( i=0; i<len; i++ )
		{
			temp = systems->at(0)->sysdata->temperature[i];
			pres = systems->at(0)->sysdata->pressure[i];
			mola = systems->at(0)->sysdata->molality_1[i];
			code = systems->at(0)->sysdata->code[i];
cout<<"i = "<<i<<", mola = "<<mola<<", pres = "<<pres<<", temp = "<<temp<<endl;
			stop = false;
			j = 0;
			while( j<(int)  Plot_Stat->plotting_info->print_temperatures.size() && stop == false )
			{
cout<<" in print loop ... "<<endl;
				if( temp == Plot_Stat->plotting_info->print_temperatures[j] && pres == Plot_Stat->plotting_info->print_pressures[j] && mola >= Plot_Stat->plotting_info->print_molalities[2*j] && mola <=  Plot_Stat->plotting_info->print_molalities[2*j+1] && code == Plot_Stat->plotting_info->print_code )
				{
					// add columns to each row
					// molalities	
 				    p_row[0] = mola;
					// residuals 		
					p_row[1] = systems->at(0)->computed_values_v[i] - systems->at(0)->measured_values_v[i];
					residuals_v.push_back( systems->at(0)->computed_values_v[i] - systems->at(0)->measured_values_v[i] );
					// residuals in percent deviation from measured values
					p_row[2] = ( systems->at(0)->computed_values_v[i] / systems->at(0)->measured_values_v[i] ) * 100 - 100;			

					stop = true;		
				}	
				j++;						
			}	
			if( stop )
			{	
				plot_array.push_back(p_row);
			}				
		}
		
		for( i=0; i< (int) plot_array.size(); i++ )		
			cout<<"Statistics residuals: plot_array["<<i<<"][0] = "<<plot_array[i][0]<<", plot_array["<<i<<"][2] = "<<plot_array[i][2]<<endl;

		if( !plot_array.size() )
		{
			cout << "None of the measurement data is covered by the data printing selection you specified !!! " << endl; 
			cout << " Check values of <PrintTemperatures>, <PrintPressures>, <PrintMolalities> and <PrintMeasValueCode> in GEMSFIT_input.dat ..." << endl; 
		}

		// Call printing function
		if( !pid && (int) plot_array.size() > 0 )
		{	
cout << "plot_residuals 293 " << endl;

			Plot_Stat->print_vectors_scatter( 1, systems, plot_array ); 
		}
cout << "plot_residuals 297 " << endl;

		cout<<"pid "<<pid<<" leaving ActivityModel::print_results ..."<<endl;

#ifdef USE_MPI
		MPI_Barrier(MPI_COMM_WORLD);
#endif
		
		// Prepare data for Q-Q Plot

		// Generate object containing normally distributed data with mean = 0 and standard deviation = SD_of_residuals					
		boost::math::normal dist(  0., SD_of_residuals );
		sort( residuals_v.begin(), residuals_v.end() );
		int N = (int) residuals_v.size();
		
		// Compute percentile
		for( i=0; i< N; i++ )
			percentiles_v.push_back( (i+1-0.5)/N );

		// Rank-based z-scores
		for( i=0; i< N; i++ )
			quantiles_v.push_back( boost::math::quantile( dist, percentiles_v[i] ) );

		// Generate Q-Q Plot (Quantile-Quantile Plot)
		print_qqplot( residuals_v, quantiles_v );


		// D'Agostino K square test for normality
		mean = ( accumulate(residuals_v.begin(), residuals_v.end(), 0) ) / N;


		for( i=0; i<N; i++ )
		{
			m2 += pow( (residuals_v[i] - mean), 2. ) / N;
			m3 += pow( (residuals_v[i] - mean), 3. ) / N;
			m4 += pow( (residuals_v[i] - mean), 4. ) / N;
		}

		sqrtb1 = m3 / pow( m2, (3./2.));
		b2     = m4 / pow( m2, 2.);
		
		// D'Agostino K square test: test of skewness
		Y             = sqrtb1 * sqrt( ((N+1.)*(N+3.))/(6*(N-2.)) );
		beta2_sqrtb1  = ( 3*(N*N + 27*N - 70)*(N + 1.)*(N + 3.)  ) / ( (N - 2.)*(N + 5.)*(N + 7.)*(N + 9.) );
		W2            = -1. + sqrt(2 * (beta2_sqrtb1 - 1.) );
		delta         = 1. / sqrt( log( sqrt(W2) ) ) ;
		alpha         = sqrt( 2. / (W2 - 1) );
		Z_sqrtb1      = delta * log( Y / alpha + sqrt( pow( (Y/alpha), 2 ) + 1 ) );

		// D'Agostino K square test: test of kurtosis
		// mean of b2		
		E_b2          = 3.*(N-1.)/(N+1.);
		// variance of b2
		Var_b2        = ( 24.*N*(N - 2.)*(N - 3.) ) / ( (N + 1.)*(N + 1.)*(N + 3.)*(N + 5.) );
		// standardized version of b2		
		x             = ( b2 - E_b2 ) / ( sqrt(Var_b2) );		
		// third standardized moment of b2
		sqrt_beta1_b2 = 6.*(N*N - 5.*N + 2.) / ( (N + 7.)*(N + 9.) ) * sqrt( 6. * (N + 3.)*(N + 5.) / ( N * (N - 2.) * (N - 3.) ) );
		A             = 6. + ( 8. / sqrt_beta1_b2 ) * ( 2. / sqrt_beta1_b2 + sqrt( 1 + 4./(sqrt_beta1_b2*sqrt_beta1_b2) ) );
		Z_b2          = ( (1.-2./(9.*A)) - pow( ((1.-2./A) / (1 + x*sqrt(2./(A - 4.))) ), (1./3.) ) ) / sqrt( 2./(9.*A) );

		// K square
		K2 = Z_sqrtb1*Z_sqrtb1 + Z_b2*Z_b2;

		// Create chi-squared distribution object
		boost::math::chi_squared chi_dist( 2 );
		
		// 1 - cumulative distribution function
		K2test = 1 - boost::math::cdf( chi_dist, K2 ); 

#ifdef BOOST_MPI		
		ofstream myStat;
		ostringstream pb; 
		pb << proc_id_boost;
		string out_fit("./output_GEMSFIT")
		out_fit += "_" + pb.str() + "/myFitStatistics.txt";
		myStat.open( out_fit.c_str(), ios::app );
#endif

#ifndef BOOST_MPI
		ofstream myStat;
		myStat.open("output_GEMSFIT/myFitStatistics.txt",ios::app);
#endif

		myStat << " D'Agostino K square test: "<<endl;		
		myStat << " K squared :  " << K2 << endl;
		myStat << " Probability that K squared follows Chi-squared distribution :  " << K2test << endl;
		myStat << endl;
		myStat.close();             


	
}



void Statistics::MC_confidence_interval( opti::ActivityModel* actmod, std::vector<double> &optv_, vector<System_Properties*> *systems, int &countit )
{

cout<<"pid : "<<pid<<" entered Statistics::MC_confidence_interval ..."<<endl;

	int i,ierr,n_param,id,imc;
	int j,k;
	double sum_of_squares_MC;
	double residual = 0.0;
	double residual_sys = 0.0;
	std::vector<double> scatter_v;
	std::vector<std::vector<double> > measured_values_backup;
	std::vector<std::vector<double> > computed_values_backup;

	scatter_v.resize(number_of_measurements);
	
	// Uncheck parallelization of for-loop within objective function. Parallelize the for loop over MC runs instead.
	for( i=0; i<(int) systems->size(); i++ ) 	
		systems->at(i)->MC_MPI = true;

cout<<"pid : "<<pid<<" entered Statistics::MC_confidence_interval | line 120"<<endl;
 
	// loop over systems and backup measurement values and computed values	
	for( j=0; j< (int) systems->size(); j++ )
	{
		// Store originals measurements and computed values
		computed_values_backup.push_back( systems->at(j)->computed_values_v );
		measured_values_backup.push_back( systems->at(j)->sysdata->val );
	}
	
	// Check if number of MC runs has zero modulo. If not increase num_of_MC_runs till it can be equally divided by the processes	
	num_of_MC_runs = num_of_MC_runs + p - (num_of_MC_runs % p);
	
	for( i=0; i<(int) optv_.size(); i++ )
		cout<<"optv_["<<i<<"] = "<<optv_[i]<<endl;
	n_param = (int) optv_.size();
cout<<"pid : "<<pid<<" entered Statistics::MC_confidence_interval | line 133 | num_of_MC_runs = "<<num_of_MC_runs<<", n_param = "<<n_param<<endl;


	// Allocate dynamic arrays to contain the fitted parameters	( MUST BE ALLOCATED WITH CONTINUOUS LOCATIONSIN MEMORY !!!!)
//	double **MC_fitted_parameters_all = opti::alloc_2d_double( num_of_MC_runs, n_param );
//	double **MC_fitted_parameters_pid = opti::alloc_2d_double( num_of_MC_runs/p, n_param );

	double*  MC_fitted_parameters_all_storage = (double *) malloc ( num_of_MC_runs * n_param * sizeof(double) );
	double** MC_fitted_parameters_all		  = (double **) malloc ( num_of_MC_runs * sizeof(double *) );
	for( i=0; i<num_of_MC_runs; i++ )
		MC_fitted_parameters_all[i] = &MC_fitted_parameters_all_storage[ i * n_param ]; 

	double*  MC_fitted_parameters_pid_storage = (double *) malloc ( num_of_MC_runs/p * n_param * sizeof(double) );
	double** MC_fitted_parameters_pid		  = (double **) malloc ( num_of_MC_runs/p * sizeof(double *) );
	for( i=0; i<(num_of_MC_runs/p); i++ )
		MC_fitted_parameters_pid[i] = &MC_fitted_parameters_pid_storage[ i * n_param ]; 
	



	double* scatter_all 	  = new double[ number_of_measurements * num_of_MC_runs ];
	double* scatter_pid_MCrun = new double[ number_of_measurements * num_of_MC_runs / p ];


	// Master creates normally distributed random numbers and scatters them to all processes
	// normal distribution with mean of 0.0 and standard deviation of SD_of_residuals
	if( !pid )
	{	
		// mersenne twister generator
		typedef boost::mt19937 RNGType; 
		RNGType rng;

		boost::normal_distribution<> rdist(0.0, SD_of_residuals); 
	 
		boost::variate_generator< RNGType, boost::normal_distribution<> > get_rand(rng, rdist);  
	 
		// for each MC run generate number_of_measurements random variables 
			ofstream myScatter_all;
			myScatter_all.open("output_GEMSFIT/myScatter_all.txt");		
		for( i=0; i<(number_of_measurements * num_of_MC_runs); i++ )
		{
			scatter_all[i] = get_rand();
			myScatter_all << " scatter_all["<<i<<"] : "<< scatter_all[i] <<endl;				
		}
			myScatter_all.close();
	}


	// Scatter the generated random numbers to all processes 
	int count = number_of_measurements * num_of_MC_runs / p;

#ifdef USE_MPI
	ierr = MPI_Scatter( &scatter_all[0], count, MPI_DOUBLE, &scatter_pid_MCrun[0], count, MPI_DOUBLE, 0, MPI_COMM_WORLD );
#endif

	if(!pid)
	{
#ifdef GEMSFIT_DEBUG
		for( i=0; i<number_of_measurements; i++ )
			cout<<" pid "<<pid<<", scatter_pid_MCrun["<<i<<"] = "<<scatter_pid_MCrun[i]<<endl;
#endif
	}	
cout<<" pid "<<pid<<", line 206"<<endl;


	id = 0;
	// Perform Monte Carlo runs
	for( imc=pid; imc<num_of_MC_runs; imc += p)
   	{
		sum_of_squares_MC = 0.;
	
		for( i=0; i<number_of_measurements; i++ )
		{
			int it = number_of_measurements * imc/p + i;
#ifdef GEMSFIT_DEBUG
			cout<<"pid : "<<pid<<" line 210, imc = "<<imc<<", i = "<<i<<", pid = "<<pid<<", imc/p = "<<imc/p<<", it = "<<it<<endl; 			
#endif
			scatter_v[i] = scatter_pid_MCrun[ (imc/p * number_of_measurements + i) ]; 
		}
#ifdef GEMSFIT_DEBUG
		cout<<"pid : "<<pid<<" scatter_v[0] = "<<scatter_v[0]<<endl;
#endif

		if( !pid )
		{
			ofstream myScatter_pid_0;
			myScatter_pid_0.open("output_GEMSFIT/myScatter_pid_0.txt",ios::app);		
			for( i=0; i<number_of_measurements; i++ )
			{
				myScatter_pid_0 << " scatter_v["<<i<<"] : "<<scatter_v[i]<<endl;				
			}
			myScatter_pid_0 << endl;
			myScatter_pid_0.close();
		}
		else if( pid==1 )
		{
			ofstream myScatter_pid_1;
			myScatter_pid_1.open("output_GEMSFIT/myScatter_pid_1.txt");		
			for( i=0; i<number_of_measurements; i++ )
			{
				myScatter_pid_1 << " scatter_v["<<i<<"] : "<<scatter_v[i]<<endl<<endl;				
			}
				myScatter_pid_1.close();
		}

		
		// loop over systems and add MC scatter to simulated measurements to simulate new set of experimental data		
		for( j=0; j< (int) systems->size(); j++ )
		{
			double* simulated_measurements = new double[systems->at(j)->sysdata->molality_1.size()];

			// Add scatter to the computed values vector of each system
			for( k=0; k< (int) systems->at(j)->sysdata->molality_1.size(); k++ )
			{
				simulated_measurements[k] = systems->at(j)->computed_values_v[k] + scatter_v[k];
			}

			// Copy the simulated experiments onto the measurement value vector
#ifdef GEMSFIT_DEBUG
			cout<<"pid : "<<pid<<" systems->at(j)->sysdata->val.size() = "<<systems->at(j)->sysdata->val.size()<<endl;
			cout<<"pid : "<<pid<<" line 260: systems->at(j)->sysdata->val[0] = "<<systems->at(j)->sysdata->val[0]<<endl;
			cout<<"pid : "<<pid<<" line 260: systems->at(j)->computed_values_v[0] = "<<systems->at(j)->computed_values_v[0]<<endl;
			cout<<"pid : "<<pid<<" line 261: scatter_v[0] = "<<scatter_v[0]<<endl;
#endif
			for( k=0; k< (int) systems->at(j)->sysdata->val.size(); k++)
			{	
				systems->at(j)->sysdata->val[k] = simulated_measurements[k];
#ifdef GEMSFIT_DEBUG
			cout<<"pid : "<<pid<<" line 266: updated systems->at(j)->sysdata->val[0] = "<<systems->at(j)->sysdata->val[0]<<endl;
#endif
			} 			
			delete[] simulated_measurements;
		}	
#ifdef GEMSFIT_DEBUG
			cout<<"pid : "<<pid<<" entered Statistics::MC_confidence_interval | line 170"<<endl;
#endif

		// perform optimization
		actmod->init_optim( optv_, systems, countit, sum_of_squares_MC); 
#ifdef GEMSFIT_DEBUG
			cout<<"pid : "<<pid<<" entered Statistics::MC_confidence_interval | line 174"<<endl;
#endif
		if( isnan(sum_of_squares) )
		{ 
			cout<<" sum of squares from objective function is NaN !!! "<<endl;
			cout<<" system measurement values (modified by Monte Carlo algorithm for confidence analysis): "<<endl;
			for( i=0; i< (int) systems->at(j)->sysdata->val.size(); i++)
				cout<<"pid : "<<pid<<" fit_statistics.cpp: systems->at(j)->sysdata->val["<<i<<"] = "<<systems->at(j)->sysdata->val[i]<<endl;
			exit(1);	
		}	

		// Store fitted parameters of each run in process specific array
		for( j=0; j<n_param; j++ )
		{
			MC_fitted_parameters_pid[ id ][ j ] = optv_[ j ];
#ifdef GEMSFIT_DEBUG
			cout<<"pid : "<<pid<<" MC_fitted_parameters_pid["<<id<<"]["<<j<<"] = "<<MC_fitted_parameters_pid[ id ][ j ]<<endl;			
#endif

//			MC_fitted_parameters_pid[ id * num_of_MC_runs/p + j ] = optv_[ j ];
//#ifdef GEMSFIT_DEBUG
//			cout<<"pid : "<<pid<<" MC_fitted_parameters_pid["<<id<<"]["<<j<<"] = "<<MC_fitted_parameters_pid[ id * num_of_MC_runs/p + j ]<<endl;			
//#endif
		}


		// loop over systems and subtract MC scatter to retain the original measurement data
		for( j=0; j< (int) systems->size(); j++ )
		{
			// Retain original computed values
			systems->at(j)->computed_values_v = computed_values_backup[j];		
			// Retain original measurements
			systems->at(j)->sysdata->val = measured_values_backup[j];		
		}	
	
	id++;
	}// end Monte Carlo for-loop
	

#ifdef USE_MPI
	// Collect all MC results in array MC_fitted_parameters_all 
	ierr = MPI_Gather( &MC_fitted_parameters_pid_storage[0], (n_param*num_of_MC_runs/p), MPI_DOUBLE, &MC_fitted_parameters_all_storage[0], (n_param*num_of_MC_runs/p), MPI_DOUBLE, 0, MPI_COMM_WORLD );
	cout<<"ierr = "<<ierr<<endl;
#endif

	// Analysis of results (plot histogram, evaluate confidence intervals, ...)
	if( !pid )
	{	
#ifdef GEMSFIT_DEBUG
		for( i=0; i<num_of_MC_runs/p; i++ ) // rows
			for( j=0; j<n_param; j++ ) // cols
				cout<<"pid "<<pid<<", MC_fitted_parameters_pid["<<i<<"]["<<j<<"] = "<<MC_fitted_parameters_pid[ i ][ j ]<<endl; 

		for( i=0; i<num_of_MC_runs; i++ ) // rows
			for( j=0; j<n_param; j++ ) // cols
				cout<<"pid "<<pid<<", MC_fitted_parameters_all["<<i<<"]["<<j<<"] = "<<MC_fitted_parameters_all[ i ][ j ]<<endl; 
#endif


		double StandardDeviation = 0.;
		arma::vec MCparams( num_of_MC_runs );

#ifdef BOOST_MPI		
		ofstream myStat;
		ostringstream pb; 
		pb << proc_id_boost;
		string out_fit("./output_GEMSFIT")
		out_fit += "_" + pb.str() + "/myFitStatistics.txt";
		myStat.open( out_fit.c_str(), ios::app );
#endif

#ifndef BOOST_MPI
		ofstream myStat;
		myStat.open("output_GEMSFIT/myFitStatistics.txt",ios::app);
#endif

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
			myStat <<"			parameter "<< j <<" :	           " << StandardDeviation << endl;

		}			
		myStat << endl;
		myStat.close();             

	}



	
	if( !pid )
	{ 
			
		// generate and plot histogram
		print_histogram( optv_, MC_fitted_parameters_all, num_of_MC_runs, MC_number_of_bars );

	}


	// Free dynamic memory

	//opti::free_2d_double( MC_fitted_parameters_pid );	
	//opti::free_2d_double( MC_fitted_parameters_all );
	//delete[] MC_fitted_parameters_pid;
	//delete[] MC_fitted_parameters_all;
	
	free (MC_fitted_parameters_pid[0]);
	free (MC_fitted_parameters_pid);

	free (MC_fitted_parameters_all[0]);
	free (MC_fitted_parameters_all);

	delete[] scatter_all;
	delete[] scatter_pid_MCrun;

	
#ifdef USE_MPI
	// To prevent confusion, wait for all threads 
	MPI_Barrier(MPI_COMM_WORLD);
#endif

}// end of function MC_confidence_interval



// perform sensitivity analysis of selected parameters
void Statistics::sensitivity_correlation( const std::vector<double> &optv_, std::vector<System_Properties*> *systems )
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
		


		// Generate systematic variations in the aIPc/aDCc values from the optimization vector and loop over them

		if( !pid )
		{

			// Set printfile bool to true in order to deactivate to parallelization of the loop over measurements within the ActMod_tsolmod_wrap() function
			systems->at(0)->printfile = true;

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
					opti::ActMod_tsolmod_wrap( residual_sys, opt_scan, systems->at(0) );
cout<<"residual_sys = "<<residual_sys<<endl;
					opt_scan_v[j] = opt_scan[i];
					ssr_param[j]  = residual_sys;			
				}
				// add rows with 'sensitivity_points' columns		
				array_ssr.push_back( opt_scan_v );
				array_ssr.push_back( ssr_param );
			}
			
			
			print_vectors_curve( optv_, array_ssr, sensitivity_points );


		len_meas = (int) systems->at(0)->sysdata->val.size();

		// Compute Jacobian matrix (= Sensitivity matrix)
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
			opti::ActMod_tsolmod_wrap( residual_sys, opt_scan, systems->at(0) );
cout<<"residual_sys = "<<residual_sys<<endl;
			computed_up = systems->at(0)->computed_values_v;			
		
			opt_scan[i] = optv_[i] - optv_[i]*delta;
			residual_sys = 0.; 
			opti::ActMod_tsolmod_wrap( residual_sys, opt_scan, systems->at(0) );
cout<<"residual_sys = "<<residual_sys<<endl;
			computed_lo = systems->at(0)->computed_values_v;			

			for( k=0; k< len_meas; k++ )
			{	
				SensitivityMatrix(k,i) 	 			  = ( computed_up[k] - computed_lo[k] ) / ( optv_[i]*delta*2 );
				DimensionlessScaledSensitivities(k,i) = SensitivityMatrix(k,i) * fabs( optv_[i] ) * sqrt( 1./systems->at(0)->sysdata->error[k] );
				CompositeScaledSensitivities(i)      += sqrt( DimensionlessScaledSensitivities(k,i)*DimensionlessScaledSensitivities(k,i)/len_meas );		
			}
		}
		

#ifdef BOOST_MPI		
		ofstream myStat;
		ostringstream pb; 
		pb << proc_id_boost;
		string out_fit("./output_GEMSFIT")
		out_fit += "_" + pb.str() + "/myFitStatistics.txt";
		myStat.open( out_fit.c_str(), ios::app );
#endif

#ifndef BOOST_MPI
		ofstream myStat;
		myStat.open("output_GEMSFIT/myFitStatistics.txt",ios::app);
#endif


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
		myStat << " Composite Scaled Sensitivities: "<<endl;
		for( i=0; i< (int) optv_.size(); i++ )
		{
			// Write sensitivities to file
			myStat <<"			parameter "<< i <<" :	           " << CompositeScaledSensitivities(i)	<< endl;	
		}
		myStat << endl;

		// Plot results with DISLIN
		print_sensitivity( (int) computed_up.size(), (int) optv_.size(), SensitivityMatrix , systems->at(0)->sysdata->molality_1 );


SensitivityMatrix.print("SensitivityMatrix:");

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
				myStat <<"									   parameter "<< i;
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
				myStat <<"									   parameter "<< i;
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


		// allocate synamic 4D memory for holding parameter-pair specific SSR with varying parameters
		int points_per_axis = 100;
		float* SSR_surface;
		SSR_surface = new float [ points_per_axis * points_per_axis ];		
		double* param_1_ray;
		double* param_2_ray;
		param_1_ray = new double [ points_per_axis ];		
		param_2_ray = new double [ points_per_axis ];		
		int param_pair[2];

		for( i=0; i< (int) optv_.size()-1; i++ )			// loop over
		{													// first parameter and
			for( k=i+1; k< (int) optv_.size(); k++ )		// loop over
			{												// second parameter and
				for( jj=0; jj<points_per_axis; jj++ )		// vary +- 25%
				{			
					for( ll=0; ll<points_per_axis; ll++ )	// vary +- 25%
					{
						j = jj * 5;
						l = ll * 5;			
						
						// parameter 1: add/subtract up to 25% of the original parameter value
						opt_scan[i] = optv_[i] + optv_[i] * ( 0.5 / points_per_axis * jj - 0.25 );
						param_1_ray[jj] = opt_scan[i];

						// parameter 2: add/subtract up to 25% of the original parameter value
						opt_scan[k] = optv_[k] + optv_[k] * ( 0.5 / points_per_axis * ll - 0.25 );
						param_2_ray[ll] = opt_scan[k];

						residual_sys = 0.; 
						opti::ActMod_tsolmod_wrap( residual_sys, opt_scan, systems->at(0) );

						// Compute and store values of sruface matrix
						SSR_surface[ points_per_axis*jj + ll ] 	 = residual_sys ;
					}	
				}
				// Print a contour plot for every parameter pair
				param_pair[0] = i;
				param_pair[1] = k;			

				print_SSR_contour( param_pair, (int) optv_.size(), points_per_axis, SSR_surface, param_1_ray, param_2_ray );				
			}
		}
		

		// free dynamicaly allocated memory
		delete[] SSR_surface;
		delete[] param_1_ray;
		delete[] param_2_ray;

		

		}// if( !pid )	 

//exit(1);//STOP

		// In case later on parallelization of measurement over for loop is wished, set printfile bool to false
		systems->at(0)->printfile = false;


}





