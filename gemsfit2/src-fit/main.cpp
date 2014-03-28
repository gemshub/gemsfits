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
    @mainpage GEMSFIT2 - fitting thermodynamic data
    @ author G. Dan Miron
	GEMFSIT - fitting tool for thermodynamic data
*/

#define USE_MPI

//#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
//#include <cmath>
//#include <nlopt.hpp>
#include <ctime>
#include <fstream>

//#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>

#include "gemsfit_iofiles.h"
#include "gemsfit_task.h"
#include "statistics.h"
#include "gemsfit_global_functions.h"
#include <iomanip>

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>


using namespace std;

namespace bfs=boost::filesystem;

int countit = 0;


int generateConfig(); // Mode GEMSFIT to generate input configuration file

struct timeval start, end;



// ---- // ---- // ---- // MAIN FUNCTION FOR GEMSFIT PROGRAM // ---- // ---- // ---- //
 
int main( int argc, char *argv[] )
{
//    clockid_t startTime = clock();

    // benchmark code
    gettimeofday(&start, NULL);

    gpf = new TGfitPath(argc, argv);

    if (gpf->isHelpMode())
        return 0;

    // Create output for log files and results directory

    if( gpf->isRunMode() )
    {
    if ( access( gpf->OutputDirPath().c_str(), 0 ) != 0 )
//    if ( !bfs::exists( gpf->OutputDirPath() ) )
        mkdir(gpf->OutputDirPath().c_str(), 0775);
//        bfs::create_directory(gpf->OutputDirPath() );
    if ( access( gpf->OutputDirPath().c_str(), 0 ) != 0 )
//    if ( !bfs::exists( gpf->ResultDir() ) )
//        bfs::create_directory(gpf->ResultDir());
        mkdir(gpf->OutputDirPath().c_str(), 0775);
    }

    if( gpf->isInitMode() ) // Mode GEMSFIT to generate input configuration file
      return generateConfig();


    gpf->flog.open(gpf->FITLogFile().c_str(), ios::trunc);
    if( gpf->flog.fail() )
    { cout<<"Log fileopen error"<<endl; exit(1); }


   time_t now = time(0);
    char* dt = ctime(&now);
//    gpf->fres<< "# "<< dt; // writes the date and time of the begining of the run in the result file
//    gpf->flog<<dt<<endl;
//    gpf->fstat<<dt<<endl;

    // Reading in the data //
    gpf->flog << "01. main.cpp(132). Creating new TGfitTask" << endl;
    TGfitTask* gfittask = new TGfitTask();

//    cout << gfittask->NodT[0]->DenH2Ow(100000 *1, 25 + 273.15) << endl;

    // if optimization with statistics, without statistics or >2 only statistics
    if (gfittask->Opti->OptDoWhat < 2)
    {
        // ++++++++++ RUN OPTIMIZATION ++++++++++ //
        gfittask->run_optim();

    } else
    {
        vector<double> grad;
        Equil_objective_function_callback(gfittask->Opti->optv, grad, gfittask);
    }

    gpf->flog << endl <<"17. Back in main.cpp(149). Performing statistics ..."<< endl;

//        gfittask->test();

    countit = master_counter;

    // Perform statistical analysis: Instantiate object of Statistics class
    if (gfittask->Tfun->objfun.size() > 0)
    {
        statistics stat( gfittask, gfittask->weighted_Tfun_sum_of_residuals, (int) gfittask->Opti->optv.size(), countit );

        // perform basic statistical analysis
        stat.basic_stat( gfittask->Opti->optv, gfittask );

        if( gfittask->Opti->OptDoWhat != 1 )
        {
            stat.sensitivity_correlation( gfittask->Opti->optv, gfittask );
        }

        if(stat.MCbool > 0)  stat.MC_confidence_interval( gfittask->Opti->optv, gfittask );
        else
        {
            gpf->fmc.open(gpf->FITmcFile().c_str(), ios::trunc);
            if( gpf->fmc.fail() )
            { cout<<"Fit Monte Carlo fileopen error"<<endl; exit(1); }
            gpf->fmc << endl;
            gpf->fmc.close();
        }

        stat.print_param();
    }

//    gfittask->print->print_result();
    if (gfittask->Tfun->objfun.size() > 0)
        gfittask->print_global_results( );
    if (gfittask->Opti->h_nestfun)
    gfittask->print_nested_results();


    delete gfittask;

    gettimeofday(&end, NULL);

    double delta = ((end.tv_sec  - start.tv_sec) * 1000000u +
             end.tv_usec - start.tv_usec) / 1.e6;
    gpf->flog <<"18. main.cpp(184): finished in ";
    gpf->flog << delta << " seconds. GEMSFITS: End. Bye!" << endl;
    cout << delta << " seconds." << endl;
    cout << "GEMSFITS: End. Bye!" << endl;
   gpf->flog.close();
   gpf->fstat.close();
}


