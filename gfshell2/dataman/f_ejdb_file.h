//-------------------------------------------------------------------
// $Id: f_ejdb_file.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of TAbstractFile, TEJDB and TFile classes
//
// Copyright (C) 2014  S.V.Dmytriyeva
// Uses Qwt (http://qwt.sourceforge.net), EJDB (http://ejdb.org),
//    yaml-cpp (https://code.google.com/p/yaml-cpp/)
//
// This file is part of the GEMSFITS GUI, which uses the
// Qt v.5 cross-platform App & UI framework (http://qt-project.org)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of LGPL v.3 license
//
// See http://gems.web.psi.ch/GEMSFIT for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef _v_ejdb_file_h_
#define _v_ejdb_file_h_

#include <fstream>
#include <vector>
#ifndef __APPLE__
#include <auto_ptr.h>   // deprecated
#endif

#include "verror.h"
class QWidget;
class EJDB;
class QDir;

//----------------------------------------------------------------------
// service functions

/// remove numbers!
const int VF3_1 = 6;
const int VF3_2 = 7;
const int VF3_3 = 2;

// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget* par, const string& title, const string& mess,
                const string& s1, const string& s2,  const string& s3="&Cancel",
                bool i_mov = false);

bool vfQuestion(QWidget* par, const string& title, const string& mess);

vector<string> vfMultiKeys(QWidget* par, const char* caption,
                            int iRt, const char* key );

bool removeDirectoryEntry( QDir dir );

//----------------------------------------------------------------------

// Mode enums
enum { MDF_DATABASE=0, MDF_TASK=1, MDF_FITS=2 };

typedef ios::openmode FileStatus;


/// Base class for file manipulation
class TAbstractFile
{
protected:

    FileStatus mode;
    string Keywd;
    bool  isopened;    // true, if file is opened
    string dir;
    string name;
    string ext;
    string Path;

    virtual void makeKeyword();
    //virtual void write( fstream& );
    //virtual void read( fstream& );

public:

    //  TFile();
    TAbstractFile(const string& fName,
          const string& fExt, const string& fDir, FileStatus mode = ios::in );
    TAbstractFile( const string& path, FileStatus mode = ios::in );
    //TFile(fstream& fcfg);
    virtual ~TAbstractFile() {}

    //void toCFG(fstream& fcfg);

    //--- Selectors
    const string& GetKeywd() const
    {
        return Keywd;
    }
    bool Test() const
    {
        return isopened;
    }
    const string& Name() const
    {
        return name;
    }
    const string& Ext() const
    {
        return ext;
    }
    const string& Dir() const
    {
        return dir;
    }
    const string& GetPath() const
    {
        return Path;
    }
    FileStatus Mode() const
    {
        return mode;
    }
    void setMode( FileStatus aMode )
    {
       mode = aMode;
    }

    //--- Manipulation

    void Makepath();
    bool Exist();
    virtual void Open() = 0;
    virtual void Close() = 0;

    bool ChooseFileOpen( QWidget* par, string& path_,
              const char* title, const char *filter );
    bool ChooseFileSave( QWidget* par, string& path_,
           const char* title, const char *filter);
    virtual void ChangePath( const string& path );
    virtual void ChangeName( const string& name );
};


/// Class for EJDB file manipulation
class TEJDB: public TAbstractFile
{
    string version; ///< Version of EJDB
    int numEJDB;    ///< Number of usage EJDB database

protected:

    virtual void makeKeyword();

public:
    EJDB *ejDB ;

    TEJDB( const string& path );
    virtual ~TEJDB();

    const string& Version() const
    {
        return version;
    }
    void readVersion();

    //--- Manipulation
    void Create();
    virtual void Open();
    virtual void Close();

};

extern TEJDB EJDBFile;

/// Class for text file manipulation
class TFile: public TAbstractFile
{
protected:

public:
    fstream ff;

    TFile( const string& path, FileStatus mode = ios::in|ios::out );
    virtual ~TFile();

    //--- Manipulation
    virtual void Open();
    virtual void Close();
};


#endif // _f_ejdb_file_h
