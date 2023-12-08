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


#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>

#ifdef __unix
#include <unistd.h>
#include <sys/time.h>
#else
#ifdef __MINGW32__
#include <sys/time.h>
#endif
#ifdef _MSC_VER
#include <io.h>
#include <time.h>
#include <chrono>
#include <Windows.h>
#include <direct.h>

int gettimeofday(struct timeval* tp, struct timezone* tzp) {
  namespace sc = std::chrono;
  sc::system_clock::duration d = sc::system_clock::now().time_since_epoch();
  sc::seconds s = sc::duration_cast<sc::seconds>(d);
  tp->tv_sec = s.count();
  tp->tv_usec = sc::duration_cast<sc::microseconds>(d - s).count();

  return 0;
}
#endif
#endif


#include "gemsfit_iofiles.h"
#include "gemsfit_task.h"
#include "statistics.h"
#include "gemsfit_global_functions.h"
#include <iomanip>

#ifdef buildWIN32
#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost_win32/boost/filesystem.hpp>
#else
#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#endif

//using namespace std;
namespace bfs=boost::filesystem;

#include "io_template.h"
extern std::vector<io_formats::outField> DataCH_dynamic_fields;

int generateConfig(); // Mode GEMSFIT to generate input configuration file
int generateJConfig();

// ---- // ---- // ---- // MAIN FUNCTION FOR GEMSFIT PROGRAM // ---- // ---- // ---- //
 
// CSH-solid-solution/work
// -run 1 ./01-initial-values.json


// CSH-solid-solution
// -initJ 1 GEMS/TCSH-dat.lst

int main( int argc, char *argv[] )
{
    int countit = 0;
    struct timeval start, end_;
    // benchmark code
    gettimeofday(&start, nullptr);

    // Some changes in GEMS3k to read CH files without V0
    DataCH_dynamic_fields[f_V0].alws = 0;

    std::shared_ptr<TGfitPath> gpf_shared = std::make_shared<TGfitPath>(argc, argv);
    gpf = gpf_shared.get();

    // cleaning work folder
    remove( gpf->FITFile().c_str() );
    remove( gpf->FITLogFile().c_str() );
    remove( gpf->FITmcFile().c_str() );
    remove( gpf->FITnfunFile().c_str() );
    remove( gpf->FITparamFile().c_str() );
    remove( gpf->FITqqFile().c_str() );
    remove( gpf->FITsensFile().c_str() );
    remove( gpf->FITStatisticsFile().c_str() );

    if (gpf->isHelpMode())
        return 0;

    // Create output for log files and results directory

    if( gpf->isRunMode() )
    {
    if ( access( gpf->OutputDirPath().c_str(), 0 ) != 0 )
#ifdef buildWIN32
        mkdir(gpf->OutputDirPath().c_str());
#else
        mkdir(gpf->OutputDirPath().c_str(), 0775);
#endif
    }

//    if( gpf->isInitMode() ) // Mode GEMSFIT to generate input configuration file
//      return generateConfig();

    if( gpf->isInitJMode() ) // Mode GEMSFIT to generate input configuration file
      return generateJConfig();

    gpf->flog.open(gpf->FITLogFile().c_str(), std::ios::trunc);
    if( gpf->flog.fail() )
    { std::cout<<"Log fileopen error"<<std::endl; exit(1); }

//    time_t now = time(0);
//    char* dt = ctime(&now);

    // Reading in the data //
    gpf->flog << "01. main.cpp(108). Creating new TGfitTask" << std::endl;
    std::shared_ptr<TGfitTask> gfittask = std::make_shared<TGfitTask>();

    // if optimization with statistics, without statistics or >2 only statistics
    if (gfittask->Opti->OptDoWhat > 0)
    {
        // ++++++++++ RUN OPTIMIZATION ++++++++++ //
        gfittask->run_optim();
    } else
    {
        std::vector<double> grad;
        if (gfittask->Opti->h_optNF) // if nested function
        {
            // CHECK NESTED pH to use acid or base
            gfittask->nestedpH ();
        }
        Equil_objective_function_callback(gfittask->Opti->optv, grad, gfittask.get());
    }

    gpf->flog << std::endl <<"15. Back in main.cpp(122). Performing statistics ..."<< std::endl;

    countit = master_counter;

    // Perform statistical analysis: Instantiate object of Statistics class
    if ( gfittask->Tfun->objfun.size() > 0)
    {
        statistics stat( gfittask.get(), gfittask->weighted_Tfun_sum_of_residuals, (int) gfittask->Opti->optv.size(), countit );

        // perform basic statistical analysis
        stat.basic_stat( gfittask->Opti->optv, gfittask.get() );

        if( gfittask->Opti->OptDoWhat != 2 )
        {
            stat.sensitivity_correlation( gfittask->Opti->optv, gfittask.get() );
        }

        if(stat.MCbool > 0)  stat.MC_confidence_interval( gfittask->Opti->optv, gfittask.get() );
        stat.print_param();
        gfittask->print_global_results( );
    }   
    if (gfittask->Opti->h_optNF)
    gfittask->print_nested_results();

    //delete gfittask;

    gettimeofday(&end_, nullptr);

    double delta = ((end_.tv_sec  - start.tv_sec) * 1000000u +
             end_.tv_usec - start.tv_usec) / 1.e6;
    gpf->flog <<"16. main.cpp(152): finished in ";
    gpf->flog << delta << " seconds. GEMSFITS: End. Bye!" << std::endl;
    std::cout << delta << " seconds." << std::endl;
    std::cout << "GEMSFITS: End. Bye!" << std::endl;
    gpf->flog.close();
    gpf->fstat.close();
}


