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
#include "io_arrays.h"
#include "data_manager.h"
#include "fit_statistics.h"
#include "optimization.h"

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

//-------------------------------------------------------------------------------------------------

outField SS_Data_Manager_fields[8] =
{
    { "DatDB",  0, 0, 1, "# DatDB: Comment"},
    { "DatTable",  0, 0, 1, "# DatTable: Comment"},
    { "DatUsername",  0, 0, 1, "# DatUsername: Comment"},
    { "DatPasswd",  0, 0, 1, "# DatPasswd: Comment"},
    { "DatSource",  1, 0, 1, "# DatSource: Comment"},
    { "DatCSVfile",  0, 0, 1, "# DatCSVfile: Comment"},
    { "DatServer",  0, 0, 1, "# DatServer: Comment"},
    { "DatRDCTable",  0, 0, 1, "# DatRDCTable: Comment"}
};

typedef enum {  /// Field index into outField structure
    f_DatDB= 0,
    f_DatTable,
    f_DatUsername,
    f_DatPasswd,
    f_DatSource,
    f_DatCSVfile,
    f_DatServer,
    f_DatRDCTable
} SS_Data_Manager_FIELDS;

void SS_Data_Manager::define_db_specs( )
{
   datasource = 0;
   CSVfile = "DatCSVfile";
   DBname ="";
   tablename="";
   username="";
   passwd="";
   psql_server="";
   RDCtablename="";
}

void SS_Data_Manager::out_db_specs_txt( bool with_comments, bool brief_mode )
{
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::app );
    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

    TPrintArrays  prar(8, SS_Data_Manager_fields, ff);
    // define data must be written
    if( datasource == 0 )
    {
       prar.setAlws( f_DatCSVfile );
    }
    else if( datasource == 1 )
    {
        prar.setAlws( f_DatDB );
        prar.setAlws( f_DatTable );
        prar.setAlws( f_DatRDCTable );
        prar.setAlws( f_DatUsername );
        prar.setAlws( f_DatPasswd );
        prar.setAlws( f_DatServer );
    }

    prar.writeField(f_DatSource, (long int)datasource, with_comments, brief_mode  );
    prar.writeField(f_DatCSVfile, CSVfile, with_comments, brief_mode  );
    prar.writeField(f_DatDB, DBname, with_comments, brief_mode  );
    prar.writeField(f_DatTable, tablename, with_comments, brief_mode  );
    prar.writeField(f_DatUsername, username, with_comments, brief_mode  );
    prar.writeField(f_DatPasswd, passwd, with_comments, brief_mode  );
    prar.writeField(f_DatServer, psql_server, with_comments, brief_mode  );
    prar.writeField(f_DatRDCTable, RDCtablename, with_comments, brief_mode  );
}

void SS_Data_Manager::get_db_specs_txt( )
{
    // open file for reading
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::in );
    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

    TReadArrays  rdar(8, SS_Data_Manager_fields, ff);

    long int nfild = rdar.findNext();
    while( nfild >=0 )
        {
          switch( nfild )
          {
           case f_DatDB: rdar.readArray( "DatDB",  DBname );
                   break;
           case f_DatTable: rdar.readArray( "DatTable",  tablename );
                   break;
           case f_DatUsername: rdar.readArray( "DatUsername",  username);
                   break;
           case f_DatPasswd: rdar.readArray( "DatPasswd",  passwd );
                   break;
           case f_DatSource: rdar.readArray( "DatSource",  &datasource, 1);
                   break;
           case f_DatCSVfile: rdar.readArray( "DatCSVfile",  CSVfile);
              break;
           case f_DatServer: rdar.readArray( "DatServer",  psql_server );
              break;
           case f_DatRDCTable: rdar.readArray( "DatRDCTable",  RDCtablename );
              break;
          }
          nfild = rdar.findNext();
        }

    // define data must be read
    if( datasource == 0 )
    {
       rdar.setAlws( f_DatCSVfile );
    }
    else if( datasource == 1 )
    {
        rdar.setAlws( f_DatDB );
        rdar.setAlws( f_DatTable );
        rdar.setAlws( f_DatRDCTable );
        rdar.setAlws( f_DatUsername );
        rdar.setAlws( f_DatPasswd );
        rdar.setAlws( f_DatServer );
    }

    // testing read
        string ret = rdar.testRead();
        if( !ret.empty() )
         { ret += " - fields must be read from OptParamFile structure";
           Error( "Error", ret);
         }
}

//-------------------------------------------------------------------------------------------------

outField StdStatistics_fields[4] =
{
    { "StatMCruns",  0, 0, 1, "# StatMCruns: Comment"},
    { "StatSensitivity",  0, 0, 1, "# StatSensitivity: Comment"},
    { "StatMCbars",  0, 0, 1, "# StatMCbars: Comment"},
    { "StatMCbool",  0, 0, 1, "# StatMCbool: Comment"}
};

typedef enum {  /// Field index into outField structure
    f_StatMCruns= 0,
    f_StatSensitivity,
    f_StatMCbars,
    f_StatMCbool
} StdStatistics_FIELDS;


void StdStatistics::default_stat_param()
{
  num_of_MC_runs = 10;
  sensitivity_points = 50;
  MC_number_of_bars = 10;
  MCbool =  0;
}


void StdStatistics::out_stat_param_txt( bool with_comments, bool brief_mode )
{
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::app );
    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

    TPrintArrays  prar(4, StdStatistics_fields, ff);
    prar.writeField(f_StatMCruns, (long int)num_of_MC_runs, with_comments, brief_mode  );
    prar.writeField(f_StatSensitivity, (long int)sensitivity_points, with_comments, brief_mode  );
    prar.writeField(f_StatMCbars, (long int)MC_number_of_bars, with_comments, brief_mode  );
    prar.writeField(f_StatMCbool, (long int)MCbool, with_comments, brief_mode  );
}

void StdStatistics::std_get_stat_param_txt( )
{
    // open file for reading
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::in );
    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

    TReadArrays  rdar(4, StdStatistics_fields, ff);

    long int nfild = rdar.findNext();
    while( nfild >=0 )
        {
          switch( nfild )
          {
           case f_StatMCruns: rdar.readArray( "StatMCruns",  &num_of_MC_runs, 1 );
                   break;
           case f_StatSensitivity: rdar.readArray( "StatSensitivity",  &sensitivity_points, 1 );
                   break;
           case f_StatMCbars: rdar.readArray( "StatMCbars",  &MC_number_of_bars, 1);
                   break;
           case f_StatMCbool:
                   { int bb;
                    rdar.readArray( "StatMCbool",  &bb, 1 );
                    MCbool = (bool)bb;
                   }
                   break;
          }
          nfild = rdar.findNext();
        }

    // testing read
        string ret = rdar.testRead();
        if( !ret.empty() )
         { ret += " - fields must be read from OptParamFile structure";
           Error( "Error", ret);
         }
}


//-------------------------------------------------------------------------------------------------

outField StdStateProp_fields[24] =
{
    { "OptAlgo",  0, 0, 1, "# OptAlgo: Comment"},
    { "OptThreads",  0, 0, 1, "# OptThreads: Comment"},
    { "OptUpBounds",  0, 0, 1, "# OptUpBounds: Comment"},
    { "OptLoBounds",  0, 0, 1, "# OptLoBounds: Comment"},
    { "OptTolRel",  0, 0, 1, "# OptTolRel: Comment"},
    { "OptMaxEval",  0, 0, 1, "# OptMaxEval: Comment"},
    { "OptUpConstraints",  0, 0, 1, "# OptUpConstraints: Comment"},
    { "OptLoConstraints",  0, 0, 1, "# OptLoConstraints: Comment"},
    { "OptConstraints",  0, 0, 1, "# OptConstraints: Comment"},
    { "OptDoWhat",  0, 0, 1, "# OptDoWhat: Comment"},
    { "OptStatOnlyBestFitParam",  0, 0, 1, "# OptStatOnlyBestFitParam: Comment"},
    { "OptStatOnlySSR",  0, 0, 1, "# OptStatOnlySSR: Comment"},
    { "OptEqSolv",  0, 0, 1, "# OptEqSolv: Comment"},
    { "OptTolAbs",  0, 0, 1, "# OptTolAbs: Comment"},
    { "OptHybridTolRel",  0, 0, 1, "# OptHybridTolRel: Comment"},
    { "OptHybridTolAbs",  0, 0, 1, "# OptHybridTolAbs: Comment"},
    { "OptHybridMaxEval",  0, 0, 1, "# OptHybridMaxEval: Comment"},
    { "OptHybridMode",  0, 0, 1, "# OptHybridMode: Comment"},
    { "OptNmultistart",  0, 0, 1, "# OptNmultistart: Comment"},
    { "OptHybridAlgo",  0, 0, 1, "# OptHybridAlgo: Comment"},
    { "OptInitStep",  0, 0, 1, "# OptInitStep: Comment"},
    { "OptScaleParam",  0, 0, 1, "# OptScaleParam: Comment"},
    { "OptNormParam",  0, 0, 1, "# OptNormParam: Comment"},
    { "OptBoundPerc",  0, 0, 1, "# OptBoundPerc: Comment"}
};

typedef enum {  /// Field index into outField structure
    f_OptAlgo= 0,
    f_OptThreads,
    f_OptUpBounds,
    f_OptLoBounds,
    f_OptTolRel,
    f_OptMaxEval,
    f_OptUpConstraints,
    f_OptLoConstraints,
    f_OptConstraints,
    f_OptDoWhat,
    f_OptStatOnlyBestFitParam,
    f_OptStatOnlySSR,
    f_OptEqSolv,
    f_OptTolAbs,
    f_OptHybridTolRel,
    f_OptHybridTolAbs,
    f_OptHybridMaxEval,
    f_OptHybridMode,
    f_OptNmultistart,
    f_OptHybridAlgo,
    f_OptInitStep,
    f_OptScaleParam,
    f_OptNormParam,
    f_OptBoundPerc

} StdStateProp_FIELDS;

using namespace opti;

void StdStateProp::define_nlopt_param( )
{
    OptAlgo = "LN_BOBYQA";
    OptThreads = 1;
    OptBoundPerc = -0.1;
    OptUpBounds.push_back(100);
    OptUpBounds.push_back(100);
    OptLoBounds.push_back(-10);
    OptLoBounds.push_back(-10);
    OptTolRel = 1e-4;
    OptTolAbs = 1e-4;
    OptMaxEval = 5000;

    OptConstraints = 0;
    //??OptUpConstraints;
    //??OptLoConstraints;

     OptDoWhat = 0;
    //OptStatOnlyBestFitParam;
    //OptStatOnlySSR;
    //OptEquil;
    OptHybridMode = 0;
    OptHybridAlgo = "LN_COBYLA";
    OptHybridTolRel = 1e-4;
    OptHybridTolAbs = 1e-4;
    OptHybridMaxEval = 5000000;
    OptNmultistart = 0;
    OptInitStep = 0;
    //OptScaleParam;
    OptNormParam = 1;

}

void StdStateProp::out_nlopt_param_txt( bool with_comments, bool brief_mode )
{
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::app );
    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

    vector<double> OptUpConstraints_;
    vector<double> OptLoConstraints_;

    TPrintArrays  prar(24, StdStateProp_fields, ff);

    if( OptHybridMode )
    {
        prar.setAlws( f_OptHybridTolRel );
        prar.setAlws( f_OptHybridTolAbs );
        prar.setAlws( f_OptHybridMaxEval );
        prar.setAlws( f_OptHybridAlgo );
    }

    if( OptConstraints )
    {

        // !!!! What constraint up or low ?
        int i;

        for( i=0; i< (int) constraint_data_v.size(); i++)
        {
            OptUpConstraints_.push_back(constraint_data_v[i].Constraints);
        };
        for( i=0; i< (int) constraint_data_v.size(); i++)
        {
            OptLoConstraints_.push_back(constraint_data_v[i].Constraints);
        };
    }

    prar.writeField( f_OptAlgo,  OptAlgo, with_comments, brief_mode );
    prar.writeField( f_OptThreads,  (long int)OptThreads, with_comments, brief_mode);
    prar.writeField( f_OptBoundPerc,  OptBoundPerc, with_comments, brief_mode );
    prar.writeArray( f_OptUpBounds,  OptUpBounds, with_comments, brief_mode);
    prar.writeArray( f_OptLoBounds,  OptLoBounds, with_comments, brief_mode );
    prar.writeField( f_OptTolRel,  OptTolRel, with_comments, brief_mode);
    prar.writeField( f_OptTolAbs,  OptTolAbs, with_comments, brief_mode);
    prar.writeField( f_OptMaxEval,  (long int)OptMaxEval, with_comments, brief_mode);
    prar.writeField( f_OptConstraints,  (long int)OptConstraints, with_comments, brief_mode);
    prar.writeArray( f_OptUpConstraints,  OptUpConstraints_, with_comments, brief_mode);
    prar.writeArray( f_OptLoConstraints,  OptLoConstraints_, with_comments, brief_mode );
    prar.writeField( f_OptDoWhat,  (long int)OptDoWhat, with_comments, brief_mode);
    prar.writeArray( f_OptStatOnlyBestFitParam,  OptStatOnlyBestFitParam, with_comments, brief_mode);
    prar.writeField( f_OptStatOnlySSR,  OptStatOnlySSR, with_comments, brief_mode);
    prar.writeField( f_OptEqSolv,  (long int)OptEquil, with_comments, brief_mode);
    prar.writeField( f_OptHybridMode,  (long int)OptHybridMode, with_comments, brief_mode);
    prar.writeField( f_OptHybridAlgo,  OptHybridAlgo, with_comments, brief_mode );
    prar.writeField( f_OptHybridTolRel,  OptHybridTolRel, with_comments, brief_mode);
    prar.writeField( f_OptHybridTolAbs,  OptHybridTolAbs, with_comments, brief_mode);
    prar.writeField( f_OptHybridMaxEval,  (long int)OptHybridMaxEval, with_comments, brief_mode);
    prar.writeField( f_OptNmultistart,  (long int)OptNmultistart, with_comments, brief_mode);
    prar.writeField( f_OptInitStep,  OptInitStep, with_comments, brief_mode);
    prar.writeField( f_OptScaleParam,  OptScaleParam, with_comments, brief_mode);
    prar.writeField( f_OptNormParam,  (long int)OptNormParam, with_comments, brief_mode);

}

void StdStateProp::set_nlopt_param_txt( )
{
    // open file for reading
    int i;
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::in );
    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

    TReadArrays  rdar(24, StdStateProp_fields, ff);
    vector<double> OptUpConstraints_;
    vector<double> OptLoConstraints_;

    long int nfild = rdar.findNext();
    while( nfild >=0 )
        {
          switch( nfild )
          {
          case f_OptAlgo: rdar.readArray( "OptAlgo",  OptAlgo );
                   break;
          case f_OptThreads: rdar.readArray( "OptThreads",  &OptThreads, 1);
                  break;
          case f_OptBoundPerc: rdar.readArray( "OptBoundPerc",  &OptBoundPerc, 1 );
                  if( OptBoundPerc < 0.) OptBoundPerc = -1;
                 break;
          case f_OptUpBounds: rdar.readArray( "OptUpBounds",  OptUpBounds);
                   break;
          case f_OptLoBounds: rdar.readArray( "OptLoBounds",  OptLoBounds );
                   break;
          case f_OptTolRel: rdar.readArray( "OptTolRel",  &OptTolRel, 1);
                   break;
          case f_OptTolAbs: rdar.readArray( "OptTolAbs",  &OptTolAbs, 1);
                  break;
          case f_OptMaxEval: rdar.readArray( "OptMaxEval",  &OptMaxEval, 1);
                  break;
          case f_OptConstraints: rdar.readArray( "OptConstraints",  &OptConstraints, 1);
                  break;
          case f_OptUpConstraints: rdar.readArray( "OptUpConstraints",  OptUpConstraints_);
                   break;
          case f_OptLoConstraints: rdar.readArray( "OptLoConstraints",  OptLoConstraints_ );
                   break;
          case f_OptDoWhat: rdar.readArray( "OptDoWhat",  &OptDoWhat, 1);
                  break;
          case f_OptStatOnlyBestFitParam: rdar.readArray( "OptStatOnlyBestFitParam",  OptStatOnlyBestFitParam);
                   break;
          case f_OptStatOnlySSR: rdar.readArray( "OptStatOnlySSR",  &OptStatOnlySSR, 1);
                  break;
          case f_OptEqSolv:
                  {
                    int bb;
                    rdar.readArray( "OptEqSolv",  &bb, 1);
                     OptEquil = bb;
                   }  break;
          case f_OptHybridMode: rdar.readArray( "OptHybridMode",  &OptHybridMode, 1);
                  break;

          case f_OptHybridAlgo: rdar.readArray( "OptHybridAlgo",  OptHybridAlgo );
                   break;
          case f_OptHybridTolRel: rdar.readArray( "OptHybridTolRel",  &OptHybridTolRel, 1);
                  break;
          case f_OptHybridTolAbs: rdar.readArray( "OptHybridTolAbs",  &OptHybridTolAbs, 1);
                  break;
          case f_OptHybridMaxEval: rdar.readArray( "OptHybridMaxEval",  &OptHybridMaxEval, 1);
                  break;
          case f_OptNmultistart: rdar.readArray( "OptNmultistart",  &OptNmultistart, 1);
                  break;
          case f_OptInitStep: rdar.readArray( "OptInitStep",  &OptInitStep, 1);
                  break;
          case f_OptScaleParam: rdar.readArray( "OptScaleParam",  &OptScaleParam, 1);
                  break;
          case f_OptNormParam:{
                int bb;
                rdar.readArray( "OptNormParam",  &bb, 1);
                OptNormParam = bb;
                 }
                  break;
          }
          nfild = rdar.findNext();
        }

    if( OptBoundPerc > 0. )
    {
        OptUpBounds.resize( opt_vec.size() );
        OptLoBounds.resize( opt_vec.size() );
        for(int i=0; i<opt_vec.size(); i++ )
        {
            OptUpBounds[i] = opt_vec[i] + fabs( opt_vec[i]*OptBoundPerc/100. );
            OptLoBounds[i] = opt_vec[i] - fabs( opt_vec[i]*OptBoundPerc/100. );
        }
    }

    // Check if all lower bounds are smaller than the corresponding upper bounds
    if( OptLoBounds.size() != OptUpBounds.size() )
    {
        cout<<endl;
        cout<<"upper and lower bounds must have same number of elements !! "<<endl;
            cout<<"exiting now ..."<<endl;
            cout<<endl;
        exit(1);
    }
    for( i=0; i<OptLoBounds.size(); i++ )
    {
        if( OptLoBounds[i]>OptUpBounds[i] )
        {
            cout<<endl;
            cout<<"Inconsistence in bounds: OptLoBounds["<<i<<"] = "<<OptLoBounds[i]<<" has a bigger value than OptUpBounds["<<i<<"] = "<<OptUpBounds[i]<<endl;
            cout<<" !! exiting now ..."<<endl;
                cout<<endl;
            exit(1);
        }
    }

    if( OptConstraints )
    {
        my_constraint_data constrdata;
        int ii = 0;

        for( i=0; i< (int) OptUpConstraints_.size(); i++)
        {
            constrdata.id = i;
            constrdata.Constraints = OptUpConstraints_[i];
            constraint_data_v.push_back(constrdata);
            ii = i;
        };
        for( i=0; i< (int) OptLoConstraints_.size(); i++)
        {
            constrdata.id = ii + 1 + i;
            constrdata.Constraints = OptLoConstraints_[i];
            constraint_data_v.push_back(constrdata);
        };


        // Check if all lower constraints are smaller than the corresponding upper constraints
        if( OptUpConstraints_.size() != OptLoConstraints_.size() )
        {
            cout<<endl;
            cout<<"WARNING: upper and lower constraints do not have same number of elements !!"<<endl;
            cout<<"OptUpConstraints.size() = "<<OptUpConstraints_.size()<<" <-> OptLoConstraints.size() = "<<OptLoConstraints_.size()<<endl;
            cout<<"hope that's ok, proceeding now ..."<<endl;
            cout<<endl;
        }
        for( i=0; i<OptLoConstraints_.size(); i++ )
        {
            if( OptLoConstraints_[i] > OptUpConstraints_[i] )
            {
                cout<<endl;
                cout<<"Inconsistence in constrains: OptLoConstraints["<<i<<"] = "<<OptLoConstraints_[i]<<" has a bigger value than OptUpConstraints["<<i<<"] = "<<OptUpConstraints_[i]<<endl;
                cout<<" .... exiting now .... "<<endl;
                    cout<<endl;
                exit(1);
            };
        };
    }//enf if ( OptConstraints )

        if( OptHybridMode )
        {
            rdar.setAlws( f_OptHybridTolRel );
            rdar.setAlws( f_OptHybridTolAbs );
            rdar.setAlws( f_OptHybridMaxEval );
            rdar.setAlws( f_OptHybridAlgo );
        }

        // testing read
        string ret = rdar.testRead();
        if( !ret.empty() )
         { ret += " - fields must be read from OptParamFile structure";
           Error( "Error", ret);
         }
}


// ----------- End of  visor.cpp ----------------------------
