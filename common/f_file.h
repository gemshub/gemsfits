//-------------------------------------------------------------------
// $Id: f_ejdb_file.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of TAbstractFile and TFile classes
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

namespace common {

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
    /// Current work directory
    static std::string current_work_directory;

    TAbstractFile(const std::string& fName, const std::string& fExt,
                  const std::string& fDir, FileStatus mode = std::ios::in);
    TAbstractFile(const std::string& path, FileStatus mode = std::ios::in);
    virtual ~TAbstractFile() {}

    //--- Selectors
    const std::string& GetKeywd() const
    {  return keywd;  }
    const std::string& Name() const
    {  return name;  }
    const std::string& Ext() const
    {  return ext;  }
    const std::string& Dir() const
    {  return dir;  }
    const std::string& WorkDir() const
    {
        if(dir.empty())
            return current_work_directory;
        else
            return dir;
    }

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

    virtual void ChangePath(const std::string& path);
    virtual void ChangeName(const std::string& name);
};

/// Class for text file manipulation
class TFile: public TAbstractFile
{
public:
    std::fstream ff;

    TFile(const std::string& path, FileStatus mode = std::ios::in|std::ios::out);
    ~TFile();

    void Open() override;
    void Close() override;
    std::string ReadAll();
};

/// Class for database file manipulation
class TDataBaseFile: public TAbstractFile
{
public:

    TDataBaseFile(const std::string& path);
    ~TDataBaseFile();

    const std::string& Version() const
    {  return version;  }
    void readVersion();

    virtual void Create();

protected:
    /// Version of db
    std::string version;
    void makeKeyword() override;

};

} // namespace common
