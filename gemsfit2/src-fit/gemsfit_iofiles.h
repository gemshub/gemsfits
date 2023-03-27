/*
*	 Copyright (C) 2013 by Dmytriyeva S. (gems2.support@psi.ch)
*
*	 This file is part of the thermodynamic fitting program GEMSFIT2.
*
*    GEMSFIT2 is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    GEMSFIT2 is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU  Generstuff al Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with GEMSFIT2.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef _gemsfit_iofiles_h_
#define _gemsfit_iofiles_h_

#include <string>
#include "node.h"
//#include "opt_vector.h"
#include <sys/stat.h>
#include <ctime>

//using namespace std;



/**
* Writes the gemsfit input file with default parameters
* @author SD modified DM
* @param node
* @param _comment
* @param brief_mode
* @date 06.05.2013
*/
void out_gems_fit_txt( TNode* node, bool _comment, bool brief_mode );

///**
//* Reads the (to be adjusted) multi parameters form the input file
//* @author SD modified DM
//* @param node
//* @param op pointer to the opti_vector class that stores ifromation about the fitted parameters
//* @date 06.05.2013
//*/
//void get_gems_fit_multi_txt(TNode* node, opti_vector* op );

///**
//* Reads the (to be adjusted) DCH parameters form the input file
//* @author SD modified DM
//* @param node
//* @param op pointer to the opti_vector class that stores ifromation about the fitted parameters
//* @date 06.05.2013
//*/
//void get_gems_fit_DCH_txt(TNode* node , opti_vector *op);

///**
//* Reads the (to be adjusted) DBR parameters form the input file
//* @author SD modified DM
//* @param node
//* @param op pointer to the opti_vector class that stores ifromation about the fitted parameters
//* @date 06.05.2013
//*/
//void get_gems_fit_DBR_txt(TNode* node, opti_vector *op );

/// \enum MODES_GEMFIT codes with respect to gemFit calculations
enum MODES_GEMFIT {
 INIT_  = 0,   ///< GEMSFIT will generate a template input configuration file
 RUN_   = 1,   ///< Run GEMFIT calculation
 HELP_  = 2,    ///< Command line help
 INITJ_ = 3
};

class TGfitPath
{

    int argc;
    char** argv;

    int mode;  /// Mode of gemsfit running (-init, -run, ... )

    std::string optParamFilePath;      /// from command line
    std::string gems3LstFilePath;      ///  Full path from command line
    std::string gems3RecipeFilePath;      ///  Full path from command line

    std::string inputDir;
    std::string outputDir;

    std::string optParamFile;
    std::string fitFile;
    std::string fitnfun;
    std::string fitqq;
    std::string fitparam;
    std::string fitsens;
    std::string fitmc;
    std::string fitStatistics;
    std::string fitLogFile;

  //   void defaultCFG();
 //   void fromWinCFG();
public:

    std::ofstream flog;  // GEMSFIT logfile
    std::ofstream fres;  // GEMSFIT results file logging all test runs
    std::ofstream fnfres;
    std::ofstream fparam;
    std::ofstream fqq;
    std::ofstream fmc;
    std::ofstream fstat;
    std::ofstream fsens;
    std::ofstream ftmp;

    int KeysNdx;

    int sizeTP;

    TGfitPath(int argc, char* argv[]);
    ~TGfitPath();

    bool isInitMode() const { return (mode == INIT_); }
    bool isInitJMode() const { return (mode == INITJ_); }
    bool isRunMode() const { return (mode == RUN_); }
    bool isHelpMode() const { return (mode == HELP_); }

    std::string InputDirPath() const {
        return inputDir;
    }

    std::string OutputDirPath() const {
        return outputDir;
    }

//    std::string ResultDir() const {
//        return resultDir;
//    }

    std::string GEMS3LstFilePath() const {
        return gems3LstFilePath;
    }

    void setGEMS3LstFilePath( const std::string name)  {
        gems3LstFilePath = name;
    }

    std::string GEMS3RecipeFilePath() const {
        return gems3RecipeFilePath;
    }

    void setGEMS3RecipeFilePath( const char* name)  {
        gems3RecipeFilePath = name;
    }

    std::string OptParamFile() const {
        return optParamFile;
    }
    
    void OptParamFileRename( const char* name)  {
        optParamFile = optParamFilePath+"/";
        optParamFile +=name;
    }

    std::string FITFile() const {
        return fitFile;
    }

    std::string FITnfunFile() const {
        return fitnfun;
    }
    
    std::string FITStatisticsFile() {
        return fitStatistics;
    }

    std::string FITqqFile() {
        return fitqq;
    }

    std::string FITsensFile() {
        return fitsens;
    }

    std::string FITparamFile() {
        return fitparam;
    }

    std::string FITmcFile() {
        return fitmc;
    }
    
    std::string FITLogFile() const {
        return fitLogFile;
    }
    


  void deleteOutputDir( const char * dir);
  void makeOutputDir(const char *dir);


};

extern TGfitPath* gpf;

void bson_print_raw_txt( std::iostream& os, const char *data, int depth, int datatype );

#endif   // _gemsfit_iofiles_h_
