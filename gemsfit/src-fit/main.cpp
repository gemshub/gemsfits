/*
*	 Copyright (C) 2012 by Ferdinand F. Hingerl (hingerl@hotmail.com)
*    Modified for fitting standard state properties (2012)
*    by G. Dan Miron (mirondanro@yahoo.com)
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
	@mainpage GEMSFIT - fitting thermodynamic data
	@ author Ferdinand F. Hingerl
    @ modified G. Dan Miron
 	
	GEMFSIT - fitting tool for thermodynamic data
*/

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <nlopt.hpp>
#include <ctime>

#ifdef USE_MPI
#include <mpi.h>
#endif

#include "system_properties.h"
#include "opt_vector.h"
#include "node.h"
#include "optimization.h"
#include "fit_statistics.h"
#include "gemsfit_global_functions.h"
#include "gemsfit_iofiles.h"


//#define BOOST_FILESYSTEM_VERSION 3
//#define BOOST_FILESYSTEM_NO_DEPRECATED
//#include <boost/filesystem.hpp>


using namespace std;

namespace bfs=boost::filesystem;

int countit = 0;
int count_r = 0;


void debug( System_Properties* , Opt_Vector, vector<System_Properties*> systems ); 



// ---- // ---- // ---- // MAIN FUNCTION FOR GEMSFIT PROGRAM // ---- // ---- // ---- //
 
int main( int argc, char *argv[] )
{
    gpf = new TGfitPath(argc, argv);

//    if( gpf->isInitMode() )
//      return 0;

    // Create output for log files and results directory
    if ( !bfs::exists( gpf->OutputDirPath() ) )
        bfs::create_directory(gpf->OutputDirPath() );
    if ( !bfs::exists( gpf->ResultDir() ) )
        bfs::create_directory(gpf->ResultDir());
	
    // empty output directory
    bfs::path fi(gpf->OutputDirPath() );
    if(!bfs::exists(fi) || !bfs::is_directory(fi))
    {
        std::cout<<"output_GEMSFIT could not be created or is not a directory. Exiting now ... "<<endl;
        exit(-1);
    }
    bfs::directory_iterator dir_iter(fi), dir_end;
    for(;dir_iter != dir_end; ++dir_iter)
    {
        bfs::remove(*dir_iter);
    }

	int ierr = 0;
	double elapsed_time = 0.0, sum_of_squares = 0.0;
    clockid_t startTime = clock();

cout<<"37"<<endl;

#ifdef USE_MPI
	// Initialize MPI environment
	if( MPI_Init(&argc, &argv) != MPI_SUCCESS ) 
	{
		fprintf( stderr,"MPI_Init failed.\n");
    }

cout<<"42"<<endl;
	// define MPI ranks and number of procceses	
	ierr = MPI_Comm_rank( MPI_COMM_WORLD, &pid );
	ierr = MPI_Comm_size( MPI_COMM_WORLD, &p );


cout<<"48"<<endl;
	// Get system time 
	ierr = MPI_Barrier( MPI_COMM_WORLD );		
	elapsed_time = - MPI_Wtime();
#endif	

int do_what = 1;

if (do_what) {


    // GEMSFIT logfile
    //const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
    ofstream fout;
    fout.open(gpf->FITLogFile().c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }
    fout << "1. main.cpp line 124. Creating new SS_System_Properties" << endl;

    // GEMSFIT results file for all test runs. Keeps a log of all runs. The file has to be deleted manually.
    string path_ = gpf->ResultDir()+"FIT_results.csv";
    ofstream fout_;
    fout_.open(path_.c_str(), ios::app);
    if( fout_.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }
    time_t now = time(0);
    char* dt = ctime(&now);
    fout_<<dt<<endl; // writes the date and time of the run in the result file

    // Reading in the data //
    SS_System_Properties* ss_newsys = new SS_System_Properties();

    fout << "10. main.cpp line 126. Finished reading input data and experimental data from the database." << endl;

    // Collect pointers to systems
    vector<SS_System_Properties*> ss_systems;
    ss_systems.push_back(ss_newsys);

    fout << "11. main.cpp line 134. Create optimization vector and pass the values to it." << endl;
    // Create optimization_vector class and pass pointers to systems
    SS_Opt_Vector optim( ss_systems );

    // Create instance of StdStateProp class derived from base class Optimization
    opti::StdStateProp gibbs( optim.opt );

    fout << "13. in main.cpp line 157. Preparing for optimization." << endl;
    if( optim.opt.size()>0 )
    {
        // 0 = optimization with full statistics, 1 = optimization with only basic statistics
        if( gibbs.OptDoWhat == 0 || gibbs.OptDoWhat == 1 )
        {
            // Prepare streaming file for live surveillance of master_counter, SSR and fitting parameters
            string path = gpf->OutputDirPath() ;
                   path += "ss_fitting_stream.out";
            ofstream fout;
            fout.open(path.c_str());
            if( fout.fail() )
            { cout<<"Output fileopen error"<<endl; exit(1); }
            fout << "##;##" << endl;
            fout << "counter;SSR";
            for( int i=0; i<optim.opt.size(); i++)
                fout << ";" << "param_" <<i+1;
            fout << endl;
            fout.close();

            if( !gibbs.OptNmultistart )
            {
                // GEMSFIT logfile
                //const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
                ofstream fout;
                fout.open(gpf->FITLogFile().c_str(), ios::app);
                if( fout.fail() )
                { cout<<"Output fileopen error"<<endl; exit(1); }

                // prepare optimization problem
                fout << "14. in main.cpp line 186. Preparing optimization problem init_optim()." << endl << endl;
                gibbs.init_optim( optim.opt, &ss_systems, countit, sum_of_squares );
            }
            else
            {
                // prepare multistart optimization problem
//                gibbs.init_optim_multistart( optim.opt, &ss_systems, countit, sum_of_squares );
            }


            fout_<<endl<<" back in main ..."<<endl;
            fout_.setf(ios::fixed);

            if (ss_newsys->fit_std) {
            for( int i=0; i< (int) optim.opt.size(); i++ )
                fout_<<"optim.opt["<<ss_newsys->to_fit_species[i]<<"] = "<<optim.opt[i]<<endl;
            fout_<<"pid : "<<pid<<", sum of squares = "<<sum_of_squares<<endl;
            fout_<<endl;
            }
            else
            {

            for( int i=0; i< (int) optim.opt.size(); i++ )
                cout<<"optim.opt["<<i<<"] = "<<optim.opt[i]<<endl;
            cout<<"pid : "<<pid<<", sum of squares = "<<sum_of_squares<<endl;
            cout<<endl;
            }


            fout_<<"Reference Year Tcelsius Pbars measured computed residuals" << endl;

            int k=0;
            for (int l=0; l<ss_systems[0]->data_meas->allexp.size(); ++l) {
                for (int m=0; m<ss_systems[0]->data_meas->allexp[l]->solubility.size(); ++m){
                fout_<< ss_systems[0]->data_meas->allexp[l]->reference<<";"<< ss_systems[0]->data_meas->allexp[l]->TC<<";"<< ss_systems[0]->data_meas->allexp[l]->PG<<";" << ss_systems[0]->data_meas->allexp[l]->name_elem[m] <<";"<<setprecision(15)<<ss_systems[0]->measured_values_v[k]<<";"<<ss_systems[0]->computed_values_v[k]<<";"<< fabs(ss_newsys->computed_residuals_v[k]) << endl;
                ++k;
                }

                if (ss_systems[0]->use_solidsolution){
                for (int m=0; m<ss_systems[0]->data_meas->allexp[l]->exp_phase.size(); ++m)
                {
                    for (int n=0; n<ss_systems[0]->data_meas->allexp[l]->exp_phase[m]->ph_name_elem.size(); ++n) {
                        fout_<< ss_systems[0]->data_meas->allexp[l]->reference<<";"<< ss_systems[0]->data_meas->allexp[l]->TC<<";"<< ss_systems[0]->data_meas->allexp[l]->PG<<";" << ss_systems[0]->data_meas->allexp[l]->exp_phase[m]->ph_name_elem[n]<<"/Si" <<";"<<setprecision(15)<<ss_systems[0]->measured_values_v[k]<<";"<<ss_systems[0]->computed_values_v[k]<<";"<< fabs(ss_newsys->computed_residuals_v[k]) << endl;
                        ++k;

                    }
                }
                }
            }


            for (int k=0; k<ss_systems[0]->measured_values_v.size(); ++k) {

             //    fout_<< ss_systems[0]->data_meas->allexp[k]->reference<<";"<< ss_systems[0]->data_meas->allexp[k]->TC<<";"<< ss_systems[0]->data_meas->allexp[k]->PG<<";"<<ss_systems[0]->measured_values_v[k]<<";"<<ss_systems[0]->computed_values_v[k]<<";"<< fabs(ss_newsys->computed_residuals_v[k]) << endl;
            }
            fout_<<endl<<endl;
            fout_.close();
            cout<<"pid : "<<pid<<" after print_results, back in main, sum_of_squares "<<sum_of_squares<<endl;
        }
    }
    else // Compute Residuals directly without fitting
    {

        // Directly call TSolMod wrapper
    //	opti::StdState_tsolmod_wrap( sum_of_squares, optim.opt, newsys );

    }

//    cout << ss_systems.at(0)->computed_residuals_v[1] << endl;
    // Perform statistical analysis: Instantiate object of Statistics class
    StdStatistics stat_gibbs( &ss_systems, sum_of_squares, (int) optim.opt.size(), countit );

//	cout<<"pid : "<<pid<<" back in main, sum_of_squares "<<sum_of_squares<<endl;

//	// perform basic statistical analysis
    stat_gibbs.std_basic_stat( optim.opt, &ss_systems );

//	// Print part of the resulting fit
//	elvis.print_results( optim.opt, &systems );


    countit = 0;

    // 0 = optimization with full statistics, 2 = only statistics
    if( gibbs.OptDoWhat == 0 || gibbs.OptDoWhat == 2 )
    {

//        if( stat_gibbs.MCbool == 1 )
//        {
//            cout << " ... performing Monte Carlo based confidence interval generation ... " << endl;
//            // Generate confidence intervals by Monte Carlo Simulation
//            // Create optimization_vector class and pass pointers to systems
//            //            SS_Opt_Vector optim( ss_systems );
//            //            // Create instance of StdStateProp class derived from base class Optimization
//            //            opti::StdStateProp gibbs( optim.opt );
//            stat_gibbs.SS_MC_confidence_interval( &gibbs, optim.opt, &ss_systems, countit );
//        }

//		// Plot residuals of minimization
//		stat_elvis.plot_residuals( optim.opt, &systems );

        // Plot results from sensitivity analysis
        stat_gibbs.SS_sensitivity_correlation( optim.opt, &ss_systems );

    }


    delete ss_newsys;
    fout.close();
}
else
{
	// Define first system
	System_Properties* newsys = new System_Properties();

	// Define further systems


	// Collect pointers to systems 
	vector<System_Properties*> systems;
        systems.push_back(newsys);	
		

	// Create optimization_vector class and pass pointers to systems
	Opt_Vector optim( systems );

	// Create instance of ActivityModel class (in future versions: choose between different derived classes from base class Optimization)
	opti::ActivityModel elvis( optim.opt );

	
	// DEBUG: print some variables	
	debug( newsys, optim, systems);


	if( optim.opt.size()>0 )
	{	
		// 0 = optimization with full statistics, 1 = optimization with only basic statistics 
		if( elvis.OptDoWhat == 0 || elvis.OptDoWhat == 1 )
		{
			// Prepare streaming file for live surveillance of master_counter, SSR and fitting parameters 
            string path = gpf->OutputDirPath();
                path += "fitting_stream.out";
			ofstream fout;
            fout.open(path.c_str());
			if( fout.fail() )
			{ cout<<"Output fileopen error"<<endl; exit(1); }
			fout << "##;##" << endl;
			fout << "counter;SSR";
			for( int i=0; i<optim.opt.size(); i++)
				fout << ";" << "param_" <<i+1;
			fout << endl;	
			fout.close();

			if( !elvis.OptNmultistart )
			{
				// prepare optimization problem	
				elvis.init_optim( optim.opt, &systems, countit, sum_of_squares );
			}
			else
			{
				// prepare multistart optimization problem	
				elvis.init_optim_multistart( optim.opt, &systems, countit, sum_of_squares );
			}

			cout<<endl<<" back in main ..."<<endl;
			for( int i=0; i< (int) optim.opt.size(); i++ )
				cout<<"optim.opt["<<i<<"] = "<<optim.opt[i]<<endl;
			cout<<"pid : "<<pid<<", sum of squares = "<<sum_of_squares<<endl;
			cout<<endl;



			cout<<"pid : "<<pid<<" after print_results, back in main, sum_of_squares "<<sum_of_squares<<endl;
		}
	}
	else // Compute Residuals directly without fitting
	{

		// Directly call TSolMod wrapper
		opti::ActMod_tsolmod_wrap( sum_of_squares, optim.opt, newsys );

	}	


	// Perform statistical analysis: Instantiate object of Statistics class
	Statistics stat_elvis( &systems, sum_of_squares, (int) optim.opt.size(), countit );

	cout<<"pid : "<<pid<<" back in main, sum_of_squares "<<sum_of_squares<<endl;

	// perform basic statistical analysis
	stat_elvis.basic_stat( optim.opt, &systems );

	// Print part of the resulting fit
	elvis.print_results( optim.opt, &systems );


	countit = 0;		

	// 0 = optimization with full statistics, 2 = only statistics
	if( elvis.OptDoWhat == 0 || elvis.OptDoWhat == 2 )
	{
		
		if( stat_elvis.MCbool == 1 )
		{
			cout << " ... performing Monte Carlo based confidence interval generation ... " << endl;
			// Generate confidence intervals by Monte Carlo Simulation
			stat_elvis.MC_confidence_interval( &elvis, optim.opt, &systems, countit );
		}

		// Plot residuals of minimization
		stat_elvis.plot_residuals( optim.opt, &systems );

		// Plot results from sensitivity analysis
		stat_elvis.sensitivity_correlation( optim.opt, &systems );

	}



	// Call destructor of ActivityModel class
	delete newsys;
}


#ifdef USE_MPI
	// Print computation time
	elapsed_time += MPI_Wtime();
	cout<<"pid "<<pid<<" : elapsed_time = "<<elapsed_time<<endl;
	
	// Destroy all MPI jobs
	ierr = MPI_Finalize();
#endif

    cout << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;

return 0;
}

// ---- // ---- // ---- // END OF MAIN FUNCTION FOR GEMSFIT PROGRAM // ---- // ---- // ---- //




void debug( System_Properties* cacl2, Opt_Vector optim, vector<System_Properties*> systems )
{
	// GEMSFIT logfile
    //const char path[200] = "output_GEMSFIT/GEMSFIT.log";
	ofstream fout;
    fout.open(gpf->FITLogFile().c_str(), ios::app);
	if( fout.fail() )
	{ cout<<"Output fileopen error"<<endl; exit(1); }


	fout<<"cacl2->sysdata->temperature[10] = "<<cacl2->sysdata->temperature[10]<<endl;
	fout<<"cacl2->sysparam->aIPc[10] = "<<cacl2->sysparam->aIPc[10]<<endl;
	fout<<"cacl2->phase_name = "<<cacl2->phase_name<<endl;

	fout<<"systems[0]->sysdata->temperature[10] = "<<systems[0]->sysdata->temperature[10]<<endl;
	fout<<"systems[0]->phase_name = "<<systems[0]->phase_name<<endl;

	fout<<"optim.opt.size()="<<optim.opt.size()<<endl;
	for( unsigned int i=0; i<optim.opt.size(); i++)
	{
		fout<<i<<endl;
		fout<<"optim.opt["<<i<<"] = "<<optim.opt[i]<<endl;
		fout<<"optim.fit_param_type["<<i<<"] = "<<optim.fit_param_type[i]<<endl;
		fout<<"optim.fit_param_col["<<i<<"] = "<<optim.fit_param_col[i]<<endl;
		fout<<"optim.fit_species[0]["<<i<<"] = "<<optim.fit_species[0][i]<<endl;
		fout<<"optim.fit_species[1]["<<i<<"] = "<<optim.fit_species[1][i]<<endl;	
		fout<<"cacl2->fit_indices[0]["<<i<<"] = "<<cacl2->fit_indices[0][i]<<endl;
		fout<<"cacl2->fit_indices[1]["<<i<<"] = "<<cacl2->fit_indices[1][i]<<endl;
	}

	fout.close();
}


