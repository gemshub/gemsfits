// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT/>
//
// GEMSIFT is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//


/**
    @mainpage GEMSFIT2 - fitting thermodynamic data
    @ author G. Dan Miron
	GEMFSIT - fitting tool for thermodynamic data
*/

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <nlopt.hpp>
#include <ctime>
#include <fstream>

#ifdef USE_MPI
#include <mpi.h>
#endif


#include "gemsfit_iofiles.h"
#include "gemsfit_task.h"


#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>


using namespace std;

namespace bfs=boost::filesystem;

int countit = 0;
int count_r = 0;


int generateConfig(); // Mode GEMSFIT to generate input configuration file


// ---- // ---- // ---- // MAIN FUNCTION FOR GEMSFIT PROGRAM // ---- // ---- // ---- //
 
int main( int argc, char *argv[] )
{
    gpf = new TGfitPath(argc, argv);


    if( gpf->isInitMode() ) // Mode GEMSFIT to generate input configuration file
      return generateConfig();

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


    // GEMSFIT logfile
    //const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
    ofstream fout;
    fout.open(gpf->FITLogFile().c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

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
    fout << "1. main.cpp line 138. Creating new TGfitTask" << endl;
    TGfitTask* gfittask = new TGfitTask();



//    // call GEM_init to read GEMS3K input files
//    TNode* node  = new TNode();

//    // call GEM_init     --> read in input files
//    if( (node->GEM_init( gpf->GEMS3LstFilePath().c_str() )) == 1 )
//       {
//            cout << gpf->GEMS3LstFilePath().c_str() << endl;

//           cout<<" .. ERROR occurred while reading GEMS3K input files !!! ..."<<endl;
//           return 1;
//       }

//    get_gems_fit_DCH_txt( node );
//    get_gems_fit_DBR_txt( node );
//    get_gems_fit_multi_txt( node );


    fout << "10. main.cpp line 126. Finished reading input data and experimental data from the database." << endl;

//    // Collect pointers to systems
//    vector<SS_System_Properties*> ss_systems;
//    ss_systems.push_back(ss_newsys);





}


