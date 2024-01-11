//-------------------------------------------------------------------
// $Id: f_ejdb_file.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of TAbstractFile, TEJDB and TFile classes
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

#include <unistd.h>
#include <iostream>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>

#include "f_ejdb_file.h"
#include "v_service.h"
#include "v_user.h"
//#ifdef buildWIN32
//#include <tcejdb/ejdb.h>
//#else
#include "ejdb.h"
//#endif

//----------------------------------------------------------
// TAbstractFile
//----------------------------------------------------------

TAbstractFile::TAbstractFile(const std::string& fName, const std::string& fExt,
                             const std::string& fDir, FileStatus aMode ):
    mode(aMode), isopened( false )
{
    dir = fDir;
    ext = fExt;
    name = fName;

    Makepath();
    makeKeyword();
}

TAbstractFile::TAbstractFile( const std::string& path, FileStatus aMode ):
    mode(aMode), isopened( false ), Path(path)
{
    if( path.empty() )
      return;
    u_splitpath(Path, dir, name, ext);
    makeKeyword();
}


// For all files is name
void TAbstractFile::makeKeyword()
{
    Keywd = name;
}

/// Get full name of file
void TAbstractFile::Makepath()
{
    Path = u_makepath( dir, name, ext );
}

/// Check for existence of file
bool TAbstractFile::Exist()
{
    if( Test() )
        return true;  // file alredy open
    if( Path.empty() )
        Makepath();

    if( access( Path.c_str(), 0 ) )
        return false;
    else  return true;
}

bool TAbstractFile::ChooseFileOpen(QWidget* par, std::string& path_,
          const char* title, const char *filter )
{
    if( Test() )
      Close();

    std::string path;
    if( path_.find('/') == std::string::npos )
    {
           path  = dir; // userFITDir(); ???
           path += path_;
    }
    else   path = path_;

    QString filt;
    if( filter )
        filt = QString("Text files (%1);;All files (*)").arg(filter);
    else
        filt = "All files (*)";

    QString fn = QFileDialog::getOpenFileName(  par, title,
          path.c_str(), filt, 0,
          QFileDialog::DontConfirmOverwrite );
#ifdef buildWIN32
    std::replace( fn.begin(), fn.end(), '/', '\\');
#endif
   if ( !fn.isEmpty() )
    {
        mode = std::ios::in;
        path_ = Path = fn.toStdString();
        u_splitpath(Path, dir, name, ext);
        return true;
    }
    else
    {
        path_ = Path = "";
        return false;
    }

}

bool TAbstractFile::ChooseFileSave(QWidget* par, std::string& path_,
       const char* title, const char *filter)
{
    if( Test() )
      Close();

    std::string path;
     if( path_.find('/') == std::string::npos )
      {      path  = dir;  // userFitDir();
             path += path_;
      }
     else   path = path_;

    replace(path.begin(), path.end(),'\\', '/');

    QStringList filt;
    if( filter )
        filt += QString("Files (%1)").arg(filter);
    else
        filt += "Text files (*.txt)";

    filt += "All files (*)";

    QString selectedFilter;
    QString fn = QFileDialog::getSaveFileName( par, title,
         path.c_str(), filt.join( ";;" ), &selectedFilter,
         QFileDialog::DontConfirmOverwrite);

    if ( !fn.isEmpty() )
    {
        mode = std::ios::out;
        path_ = Path = fn.toStdString();
        u_splitpath(Path, dir, name, ext);
        return true;
    }
    else
    {
        path_ = Path = "";
        return false;
    }
}

void TAbstractFile::ChangePath( const std::string& path )
{
    if( Test() )
        Close();

    Path = path;
    if( path.empty() )
    {
      dir = name = ext = "";
    }
    else
    {
      u_splitpath(Path, dir, name, ext);
      makeKeyword();
    }
}

void TAbstractFile::ChangeName( const std::string& newname )
{
    if( Test() )
        Close();

    name = newname;
    Makepath();
    makeKeyword();
}


//-------------------------------------------------------------
// TEJDB, files that contain EJDB records
//-------------------------------------------------------------

TEJDB EJDBFile("");

/// Configurations from file path
TEJDB::TEJDB( const std::string& path ):
   TAbstractFile( path ), numEJDB(0),  ejDB(0)
{
     makeKeyword();
}


TEJDB::~TEJDB()
{
    if( Test() )
        Close();
}

/// Make keyword of DB internal file
void TEJDB::makeKeyword()
{
    std::string key;
    if( name.empty() )
        return;

    std::string fname = name;

    key = std::string(fname, 0, 2);
    size_t npos = 0;
    size_t npos2 = fname.find("_", npos);
    while( npos2 != std::string::npos )
    {   npos = npos2+1;
        key += std::string(fname, npos, 2);
        npos2 = fname.find("_", npos);
    }
    key += std::string(fname, npos+2);

    Keywd = key;
}

/// Open file in special mode
void TEJDB::Open()
{

    if( Test() )
    {
        numEJDB++;
    }
    else
    {

      // Test and open file  (name of ejdb must be take from nFile)
      ejDB = ejdbnew();
      if (!ejdbopen(ejDB, Path.c_str(), JBOWRITER | JBOCREAT ))
      {
        ejdbdel(ejDB);
        std::cout << "EJDB open error" << std::endl;
        ejDB = 0;
        isopened = false;
        Error( Path, "EJDB open error");
        return;
       }
      numEJDB++;
      isopened = true;
    }
}

/// Close EJ DataBase
void TEJDB::Close()
{
    numEJDB--;

    if( numEJDB <= 0 )
    { if(ejDB )
      { ejdbclose(ejDB);
        ejdbdel(ejDB);
        ejDB = 0;
      }
    isopened = false;
    }
}

/// Create PDB file
void TEJDB::Create()
{
    Open();
    Close();
    // make changelog.txt file
    std::string clfile = dir + "/Changelog.txt";
    std::fstream ff( clfile.c_str(), std::ios::out);
    ff << "File " << name.c_str() << " created on " << curDate().c_str() << " " << curTime().c_str() << std::endl;
    ff << "<Version> = v0.1" << std::endl;
}


/// Read version from Changelog.txt file
void TEJDB::readVersion( )
{
    // open changelog.txt file
    std::string clfile = dir + "/Changelog.txt";
    std::fstream ff( clfile.c_str(), std::ios::in);
    std::string fbuf;
    size_t pos;
    version = "not versioned";

    while( ff.good() )
    {
      getline ( ff ,fbuf);
      pos = fbuf.find("<Version>");
      if( pos != std::string::npos )
      {
         pos = fbuf.find("=", pos+1);
         if( pos != std::string::npos )
         {
           version = fbuf.substr(pos+1);
           strip( version);
           break;
         }
      }
    }
}

//-------------------------------------------------------------
// TFile, txt input/output files
//-------------------------------------------------------------

/// Configurations from file path
TFile::TFile( const std::string& path, FileStatus aMode ):
    TAbstractFile(path, aMode)
{ }


TFile::~TFile()
{
    if( Test() )
        Close();
}

/// Open file in special mode
void TFile::Open()
{
    if( Test() )
    {
       Close();
    }

    ff.open( Path.c_str(), mode );
    ErrorIf( !ff.good(), Path, "Error opening file" );
    isopened = true;
}

/// Close EJ DataBase
void TFile::Close()
{
    ff.close();
    isopened = false;
}

// internal functions
bool removeDirectoryEntry( QDir dir )
{
    bool ok = dir.exists();
    if ( ok )
    {
        QFileInfoList entries = dir.entryInfoList( QDir::NoDotAndDotDot |
                QDir::Dirs | QDir::Files );
        foreach ( QFileInfo entryInfo, entries )
        {
            QString path = entryInfo.absoluteFilePath();
            if ( entryInfo.isDir() )
            {
                if ( !removeDirectoryEntry( QDir( path ) ) )
                {
                    ok = false;
                    break;
                }
            }
            else
            {
                QFile file( path );
                if ( !file.remove() )
                {
                    ok = false;
                    break;
                }
            }
        }
    }
   return ok;
}


//--------------------- End of f_ejdb_file.cpp ---------------------------

