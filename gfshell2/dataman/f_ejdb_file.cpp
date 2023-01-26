//-------------------------------------------------------------------
// $Id: f_ejdb_file.cpp 333 2014-03-13 13:23:32Z gemsfits $
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
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include "f_ejdb_file.h"
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

bool TAbstractFile::ChooseFileOpen(QWidget* par, std::string& path_,
                                   const char* title, const char *filter )
{
    if(Test())
        Close();

    std::string a_path;
    if( path_.find('/') == std::string::npos ) {
        a_path  = dir;
        a_path += path_;
    }
    else
        a_path = path_;

    QString filt;
    if( filter )
        filt = QString("Text files (%1);;All files (*)").arg(filter);
    else
        filt = "All files (*)";

    QString fn = QFileDialog::getOpenFileName(  par, title,
                                                a_path.c_str(), filt, 0,
                                                QFileDialog::DontConfirmOverwrite );
#ifdef buildWIN32
    std::replace( fn.begin(), fn.end(), '/', '\\');
#endif
    if ( !fn.isEmpty() ) {
        mode = std::ios::in;
        path_ = path = fn.toStdString();
        u_splitpath(path, dir, name, ext);
        return true;
    }
    else {
        path_ = path = "";
        return false;
    }

}

bool TAbstractFile::ChooseFileSave(QWidget* par, std::string& path_,
                                   const char* title, const char *filter)
{
    if(Test())
        Close();

    std::string a_path;
    if( path_.find('/') == std::string::npos )  {
        a_path  = dir;
        a_path += path_;
    }
    else
        a_path = path_;

    replace(a_path.begin(), a_path.end(),'\\', '/');

    QStringList filt;
    if( filter )
        filt += QString("Files (%1)").arg(filter);
    else
        filt += "Text files (*.txt)";

    filt += "All files (*)";

    QString selectedFilter;
    QString fn = QFileDialog::getSaveFileName( par, title,
                                               a_path.c_str(), filt.join( ";;" ), &selectedFilter,
                                               QFileDialog::DontConfirmOverwrite);

    if ( !fn.isEmpty() ) {
        mode = std::ios::out;
        path_ = path = fn.toStdString();
        u_splitpath(path, dir, name, ext);
        return true;
    }
    else {
        path_ = path = "";
        return false;
    }
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

// internal functions
bool removeDirectoryEntry( QDir dir )
{
    bool ok = dir.exists();
    if( ok ) {
        QFileInfoList entries = dir.entryInfoList( QDir::NoDotAndDotDot |
                                                   QDir::Dirs | QDir::Files );
        foreach ( QFileInfo entryInfo, entries ) {
            QString path = entryInfo.absoluteFilePath();
            if ( entryInfo.isDir() ) {
                if( !removeDirectoryEntry(QDir( path ))) {
                    ok = false;
                    break;
                }
            }
            else {
                QFile file( path );
                if( !file.remove() ) {
                    ok = false;
                    break;
                }
            }
        }
    }
    return ok;
}


//--------------------- End of f_ejdb_file.cpp ---------------------------

