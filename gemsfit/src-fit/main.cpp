#include <stdio.h> // ??
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <nlopt.hpp> // nonlinear optimization library http://openopt.org/nlopt ab initio

#ifdef USE_MPI
#include <mpi.h>
#endif

#include "system_properties.h"
#include "opt_vector.h"
#include "node.h"
#include "optimization.h"
#include "fit_statistics.h"
#include "gemsfit_global_functions.h"


#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>


using namespace std;

namespace bfs=boost::filesystem;

int countit = 0;

System_Specifications sysspec;


typedef struct
{
	double a,b,c,d;
}my_constraint_data;

void debug( System_Properties* , Opt_Vector, vector<System_Properties*> systems ); 



// ---- // ---- // ---- // MAIN FUNCTION FOR GEMSFIT PROGRAM // ---- // ---- // ---- //
 
int main( int argc, char *argv[] )
{
	// Create output directory
	if ( !bfs::exists( "output_GEMSFIT" ) )
		bfs::create_directory("output_GEMSFIT");
	
	// empty directory
	bfs::path fi("output_GEMSFIT");
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

cout<<"54"<<endl;
	// Create instance of ActivityModel class (in future versions: choose between different derived classes from base class Optimization)
	opti::ActivityModel elvis;


	// Define first system
	sysspec.meas_db_ 	 = "cacl2_meas_temp";
	sysspec.param_file_	 = "CaCl2-param.dat";
	System_Properties* newsys = new System_Properties( sysspec );
	 
	// Define further systems

		
	

	// Collect pointers to systems 
	vector<System_Properties*> systems;
        systems.push_back(newsys);	
		

	// Create optimization_vector class and pass pointers to systems
	Opt_Vector optim( systems );

	
	// DEBUG: print some variables	
	debug( newsys, optim, systems);


	if( optim.opt.size()>0 )
	{	
cout << " main 118 " << endl;	
		// 0 = optimization with full statistics, 1 = optimization with only basic statistics 
		if( elvis.OptDoWhat == 0 || elvis.OptDoWhat == 1 )
		{
			// Prepare streaming file for live surveillance of master_counter, SSR and fitting parameters 
			const char path[200] = "output_GEMSFIT/fitting_stream.out";
			ofstream fout;
			fout.open(path);						
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


			// Print part of the resulting fit
			elvis.print_results( optim.opt, &systems );

			cout<<"pid : "<<pid<<" after print_results, back in main, sum_of_squares "<<sum_of_squares<<endl;
		}
	}
	else
	{
cout << " main 140 " << endl;	

		// Directly call TSolMod wrapper
		opti::ActMod_tsolmod_wrap( sum_of_squares, optim.opt, newsys );

		// Print part of the resulting fit
		elvis.print_results( optim.opt, &systems );
	
	}	





	// Perform statistical analysis: Instantiate object of Statistics class
	Statistics stat_elvis( &systems, sum_of_squares, (int) optim.opt.size(), countit );

	cout<<"pid : "<<pid<<" back in main, sum_of_squares "<<sum_of_squares<<endl;

	// perform basic statistical analysis
	stat_elvis.basic_stat( optim.opt, &systems );

	countit = 0;		

	// 0 = optimization with full statistics, 2 = only statistics
	if( elvis.OptDoWhat == 0 || elvis.OptDoWhat == 2 )
	{
		
		if( stat_elvis.MCbool == 1 )
		{
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


#ifdef USE_MPI
	// Print computation time
	elapsed_time += MPI_Wtime();
	cout<<"pid "<<pid<<" : elapsed_time = "<<elapsed_time<<endl;
	
	// Destroy all MPI jobs
	ierr = MPI_Finalize();
#endif

return 0;
}

// ---- // ---- // ---- // END OF MAIN FUNCTION FOR GEMSFIT PROGRAM // ---- // ---- // ---- //




void debug( System_Properties* cacl2, Opt_Vector optim, vector<System_Properties*> systems )
{
	cout<<"cacl2->sysdata->temperature[10] = "<<cacl2->sysdata->temperature[10]<<endl;
	cout<<"cacl2->sysparam->aIPc[10] = "<<cacl2->sysparam->aIPc[10]<<endl;
	cout<<"cacl2->phase_name = "<<cacl2->phase_name<<endl;

	cout<<"systems[0]->sysdata->temperature[10] = "<<systems[0]->sysdata->temperature[10]<<endl;
	cout<<"systems[0]->phase_name = "<<systems[0]->phase_name<<endl;

	cout<<"optim.opt.size()="<<optim.opt.size()<<endl;
	for( unsigned int i=0; i<optim.opt.size(); i++)
	{
		cout<<i<<endl;
		cout<<"optim.opt["<<i<<"] = "<<optim.opt[i]<<endl;
		cout<<"optim.fit_param_type["<<i<<"] = "<<optim.fit_param_type[i]<<endl;
		cout<<"optim.fit_param_col["<<i<<"] = "<<optim.fit_param_col[i]<<endl;
		cout<<"optim.fit_species[0]["<<i<<"] = "<<optim.fit_species[0][i]<<endl;
		cout<<"optim.fit_species[1]["<<i<<"] = "<<optim.fit_species[1][i]<<endl;	
		cout<<"cacl2->fit_indices[0]["<<i<<"] = "<<cacl2->fit_indices[0][i]<<endl;
		cout<<"cacl2->fit_indices[1]["<<i<<"] = "<<cacl2->fit_indices[1][i]<<endl;
	};
}


