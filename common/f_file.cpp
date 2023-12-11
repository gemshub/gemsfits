//-------------------------------------------------------------------
//
// Implementation of TAbstractFile, TEJDB and TFile classes
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

#include <unistd.h>
#include <iomanip>
#include <chrono>
#include <string>
#include <sstream>
#include "f_file.h"
#include "v_service.h"

namespace common {

std::string TAbstractFile::current_work_directory;

static std::string current_time_and_date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%d/%m/%Y %H:%M");
    return ss.str();
}

//----------------------------------------------------------
// TAbstractFile
//----------------------------------------------------------

TAbstractFile::TAbstractFile(const std::string& fName, const std::string& fExt,
                             const std::string& fDir, FileStatus aMode):
    mode(aMode), is_opened(false)
{
    dir = fDir;
    ext = fExt;
    name = fName;
    keywd = name;
    Makepath();
}

TAbstractFile::TAbstractFile(const std::string& apath, FileStatus aMode):
    mode(aMode), is_opened(false), path(apath)
{
    if( path.empty() )
        return;
    u_splitpath(path, dir, name, ext);
    keywd = name;
}

// For all files is name
void TAbstractFile::makeKeyword()
{
    keywd = name;
}

/// Get full name of file
void TAbstractFile::Makepath()
{
    path = u_makepath( dir, name, ext );
}

/// Check for existence of file
bool TAbstractFile::Exist()
{
    if(is_opened)
        return true;  // file alredy open
    if(path.empty())
        Makepath();

    if(access(path.c_str(), 0 ))
        return false;
    else
        return true;
}

void TAbstractFile::ChangePath(const std::string& a_path)
{
    if(is_opened)
        Close();

    path = a_path;
    if(path.empty()) {
        dir = name = ext = "";
    }
    else {
        u_splitpath(path, dir, name, ext);
        makeKeyword();
    }
}

void TAbstractFile::ChangeName(const std::string& newname)
{
    if(is_opened)
        Close();

    name = newname;
    Makepath();
    makeKeyword();
}


//-------------------------------------------------------------
// TFile, txt input/output files
//-------------------------------------------------------------

/// Configurations from file path
TFile::TFile( const std::string& path, FileStatus aMode ):
    TAbstractFile(path, aMode)
{}


TFile::~TFile()
{
    if(is_opened) {
        Close();
    }
}

/// Open file in special mode
void TFile::Open()
{
    if(is_opened) {
        Close();
    }

    ff.open( path.c_str(), mode );
    ErrorIf( !ff.good(), path, "Error opening file" );
    is_opened = true;
}

/// Close EJ DataBase
void TFile::Close()
{
    ff.close();
    is_opened = false;
}

//-------------------------------------------------------------
// TDataBaseFile
//-------------------------------------------------------------

// Configurations from file path
TDataBaseFile::TDataBaseFile(const std::string& path):TAbstractFile(path)
{
    makeKeyword();
}


TDataBaseFile::~TDataBaseFile()
{}

/// Make keyword of DB internal file
void TDataBaseFile::makeKeyword()
{
    std::string key;
    if( name.empty() )
        return;

    std::string fname = name;

    key = std::string(fname, 0, 2);
    size_t npos = 0;
    size_t npos2 = fname.find("_", npos);
    while( npos2 != std::string::npos ) {
        npos = npos2+1;
        key += std::string(fname, npos, 2);
        npos2 = fname.find("_", npos);
    }
    key += std::string(fname, npos+2);

    keywd = key;
}

/// Create PDB file
void TDataBaseFile::Create()
{
    Open();
    Close();
    // make changelog.txt file
    std::string clfile = dir + "/Changelog.txt";
    std::fstream ff( clfile.c_str(), std::ios::out);
    ff << "File " << name << " created on " << current_time_and_date() << std::endl;
    ff << "<Version> = v0.1" << std::endl;
}


/// Read version from Changelog.txt file
void TDataBaseFile::readVersion()
{
    // open changelog.txt file
    std::string clfile = dir + "/Changelog.txt";
    std::fstream ff( clfile.c_str(), std::ios::in);
    std::string fbuf;
    size_t pos;
    version = "not versioned";

    while(ff.good()) {
        getline(ff, fbuf);
        pos = fbuf.find("<Version>");
        if( pos != std::string::npos ) {
            pos = fbuf.find("=", pos+1);
            if( pos != std::string::npos ) {
                version = fbuf.substr(pos+1);
                strip( version);
                break;
            }
        }
    }
}

} // namespace common

//--------------------- End of f_file.cpp ---------------------------

