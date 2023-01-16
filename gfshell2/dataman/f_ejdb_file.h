//-------------------------------------------------------------------
// $Id: f_ejdb_file.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of TAbstractFile, TEJDB and TFile classes
//
// Copyright (C) 2014-2023  S.V.Dmytriyeva
// Uses EJDB (https://ejdb.org),
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

#pragma once

#include <fstream>
#include <vector>
#include "verror.h"

#ifdef OLD_EJDB
#include <ejdb.h>
#else
#include <ejdb2/ejdb2.h>
#endif

class QWidget;
class QDir;
class QString;


//----------------------------------------------------------------------
// service functions

/// remove numbers!
const int VF3_1 = 6;
const int VF3_2 = 7;
const int VF3_3 = 2;

// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget* par, const std::string& title, const std::string& mess,
                const std::string& s1, const std::string& s2,  const std::string& s3="&Cancel",
                bool i_mov = false);

bool vfQuestion(QWidget* par, const std::string& title, const std::string& mess);

std::vector<std::string> vfMultiKeys(QWidget* par, const char* caption,
                                     int iRt, const char* key );

bool removeDirectoryEntry(QDir dir);

void removeComments(QString& valCsv);

//----------------------------------------------------------------------

// Mode enums
enum { MDF_DATABASE=0, MDF_TASK=1, MDF_FITS=2 };

typedef std::ios::openmode FileStatus;

/// Base class for file manipulation
class TAbstractFile
{
protected:

    FileStatus mode;
    std::string keywd;
    bool  is_opened;    // true, if file is opened
    std::string dir;
    std::string name;
    std::string ext;
    std::string path;

    virtual void makeKeyword();

public:

    TAbstractFile(const std::string& fName, const std::string& fExt,
                  const std::string& fDir, FileStatus mode = std::ios::in);
    TAbstractFile(const std::string& path, FileStatus mode = std::ios::in);
    virtual ~TAbstractFile() {}

    //--- Selectors
    const std::string& GetKeywd() const
    {  return keywd;  }
    bool Test() const
    {  return is_opened; }
    const std::string& Name() const
    {  return name;  }
    const std::string& Ext() const
    {  return ext;  }
    const std::string& Dir() const
    {  return dir;  }
    const std::string& getPath() const
    {  return path;  }
    FileStatus Mode() const
    {  return mode;  }
    void setMode(FileStatus aMode)
    {  mode = aMode;  }

    void Makepath();
    bool Exist();
    virtual void Open() = 0;
    virtual void Close() = 0;

    bool ChooseFileOpen(QWidget* par, std::string& path_,
                        const char* title, const char *filter);
    bool ChooseFileSave(QWidget* par, std::string& path_,
                        const char* title, const char *filter);
    virtual void ChangePath(const std::string& path);
    virtual void ChangeName(const std::string& name);
};


/// Class for EJDB file manipulation
class TEJDB: public TAbstractFile
{
    /// Version of EJDB
    std::string version;
    /// Number of usage EJDB database
    int numEJDB;

    void makeKeyword() override;

public:

#ifdef OLD_EJDB
    EJDB *ejDB;
#else
    EJDB ejDB ;
#endif

    TEJDB(const std::string& path);
    ~TEJDB();

    const std::string& Version() const
    {  return version;  }
    void readVersion();

    void Create();
    void Open() override;
    void Close() override;

};

extern TEJDB EJDBFile;

/// Class for text file manipulation
class TFile: public TAbstractFile
{
public:
    std::fstream ff;

    TFile(const std::string& path, FileStatus mode = std::ios::in|std::ios::out);
    ~TFile();

    void Open() override;
    void Close() override;
};
