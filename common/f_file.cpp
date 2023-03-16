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
#include <iostream>
#include "f_file.h"
#include "v_service.h"

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

    Makepath();
    makeKeyword();
}

TAbstractFile::TAbstractFile(const std::string& apath, FileStatus aMode):
    mode(aMode), is_opened(false), path(apath)
{
    if( path.empty() )
        return;
    u_splitpath(path, dir, name, ext);
    makeKeyword();
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
    if(Test())
        return true;  // file alredy open
    if(path.empty())
        Makepath();

    if(access(path.c_str(), 0 ))
        return false;
    else
        return true;
}

void TAbstractFile::ChangePath( const std::string& a_path )
{
    if(Test())
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

void TAbstractFile::ChangeName( const std::string& newname )
{
    if(Test())
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
    if(Test()) {
        Close();
    }
}

/// Open file in special mode
void TFile::Open()
{
    if(Test()) {
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

//--------------------- End of f_file.cpp ---------------------------

