/*
*	 Copyright (C) 2013 by Dmytriyeva S. (gems2.support@psi.ch)
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

#include <iostream>
using namespace std;
#include <cstdlib>

#include "gemsfit_iofiles.h"
#include "v_user.h"

// subfolder and file names default
const char *INPUT_DIR = "SS_INPUT/";
const char *OUTPUT_DIR = "output_GEMSFIT/";
const char *RESULT_DIR = "results_GEMSFIT/";
const char *OPT_PARAM_FILE = "SS_INPUT/SS_GEMSFIT_input.dat";
const char *FIT_CSV_FILE = "FIT.csv";
const char *FIT_STATISTIC = "MyFitStatistics.txt";
const char *FIT_LOGFILE = "SS_GEMSFIT.log";

//----------------------------------------------------------------
// TGfitPath  class implementation
//----------------------------------------------------------------

TGfitPath::TGfitPath(int c, char *v[]):
        argc(c), argv(v)
{
    optParamFilePath = "";
    gems3LstFilePath = "";

//#ifdef __unix
//        optParamFilePath = getenv("HOME");
//#else
        optParamFilePath = ".";
//#endif

    // parsing options -init, -run, -conf if given

    int iinit = 0;		// index of -init option
    int irun = 0;		// index of -run option
    int iconf = 0;		// index of -conf option

    for (int ii = 1; ii < argc; ii++)
    {
        if (strcmp(argv[ii], "-init") == 0 )
          iinit = ii;
        else if (strcmp(argv[ii], "-run") == 0 )
            irun = ii;
                 else if (strcmp(argv[ii], "-conf") == 0)
                     iconf = ii;
    }


    if (irun != 0)
    {
        if (argc <= irun + 1)
            Error("Wrong options", "Wrong argument for option -run");
        optParamFile = argv[irun + 1];
        mode = RUN_;
    }


   if (iinit != 0)
    {
        if (argc <= iinit + 1)
            Error("Wrong options", "Wrong argument for option -init");
        gems3LstFilePath = argv[iinit + 1];
        mode = INIT_;
    }

    if (iconf != 0)
    {
        if (argc <= iconf + 1)
            Error("Wrong options", "Wrong argument for option -conf");
        optParamFile = argv[iconf + 1];
    }

    if( optParamFile.empty() )
    {
        optParamFile = optParamFilePath;
        optParamFile += "/";
        optParamFile += OPT_PARAM_FILE;
    }
    else
    {
        string name;
        string ext;
        u_splitpath( optParamFile, optParamFilePath, name, ext );
    }

    string path = optParamFilePath;
    path += "/";

    // set up default pathes
    // later this pathes and filenames can be read from config file
    inputDir = path + INPUT_DIR;
    outputDir = path + OUTPUT_DIR;
    resultDir = path + RESULT_DIR;

    fitFile = resultDir+FIT_CSV_FILE;
    fitStatistics = outputDir+FIT_STATISTIC;
    fitLogFile = outputDir+FIT_LOGFILE;

    cout << "optParamFile = " << optParamFile << endl;
    cout << "fitFile = " << fitFile << endl;
    cout << "fitLogFile = " << fitLogFile << endl;
    cout << "gems3LstFilePath = " << gems3LstFilePath << endl;
 }

TGfitPath::~TGfitPath()
{}

void TGfitPath::deleteOutputDir(const char *dir)
{
}


void TGfitPath::makeOutputDir(const char *dir)
{
}

TGfitPath *gpf;

// ----------- End of  visor.cpp ----------------------------
