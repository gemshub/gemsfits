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
*    GNU  Generstuff al Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with GEMSFIT.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef _gemsfit_iofiles_h_
#define _gemsfit_iofiles_h_

#include <string>

/// \enum MODES_GEMFIT codes with respect to gemFit calculations
enum MODES_GEMFIT {
 INIT_  = 0,   ///< GEMSFIT will generate a template input configuration file
 RUN_   = 1   ///< Run GEMFIT calculation
};

class TGfitPath
{

    int argc;
    char** argv;

    int mode;  /// Mode of gemsfit running (-init, -run, ... )

    string optParamFilePath;      /// from command line
    string gems3LstFilePath;      ///  Full path from command line

    string inputDir;
    string outputDir;
    string resultDir;

    string optParamFile;
    string fitFile;
    string fitStatistics;
    string fitLogFile;

  //   void defaultCFG();
 //   void fromWinCFG();

public:

    TGfitPath(int argc, char* argv[]);
    ~TGfitPath();

    bool isInitMode() const { return (mode == INIT_); }
    bool isRunMode() const { return (mode == RUN_); }

    string InputDirPath() const {
        return inputDir;
    }

    string OutputDirPath() const {
        return outputDir;
    }

    string ResultDir() const {
        return resultDir;
    }

    string GEMS3LstFilePath() const {
        return gems3LstFilePath;
    }

    string OptParamFile() const {
        return optParamFile;
    }
    
    string FITFile() const {
        return fitFile;
    }
    
    string FITStatisticsFile() {
        return fitStatistics;
    }
    
    string FITLogFile() const {
        return fitLogFile;
    }
    

  void deleteOutputDir( const char * dir);
  void makeOutputDir(const char *dir);


};

extern TGfitPath* gpf;

#endif   // _gemsfit_iofiles_h_
