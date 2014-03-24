//-------------------------------------------------------------------
// $Id: f_ejdb_file.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TAbstractFile, TEJDB and TFile classes
//
// Copyright (C) 2014 S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include "f_ejdb_file.h"
#include "v_user.h"
#include "ejdb.h"

#include <unistd.h>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>

//----------------------------------------------------------
// TAbstractFile
//----------------------------------------------------------

TAbstractFile::TAbstractFile(const string& fName, const string& fExt,
                             const string& fDir, FileStatus aMode ):
    mode(aMode), isopened( false )
{
    dir = fDir;
    ext = fExt;
    name = fName;

    Makepath();
    makeKeyword();
}

TAbstractFile::TAbstractFile( const string& path, FileStatus aMode ):
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

bool TAbstractFile::ChooseFileOpen(QWidget* par, string& path_,
          const char* title, const char *filter )
{
    if( Test() )
      Close();

    string path;
    if( path_.find('/') == string::npos )
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
   if ( !fn.isEmpty() )
    {
        mode = ios::in;
        path_ = Path = fn.toUtf8().data();
        u_splitpath(Path, dir, name, ext);
        return true;
    }
    else
    {
        path_ = Path = "";
        return false;
    }

}

bool TAbstractFile::ChooseFileSave(QWidget* par, string& path_,
       const char* title, const char *filter)
{
    if( Test() )
      Close();

    string path;
     if( path_.find('/') == string::npos )
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
        mode = ios::out;
        path_ = Path = fn.toUtf8().data();
        u_splitpath(Path, dir, name, ext);
        return true;
    }
    else
    {
        path_ = Path = "";
        return false;
    }
}

void TAbstractFile::ChangePath( const string& path )
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

void TAbstractFile::ChangeName( const string& newname )
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
TEJDB::TEJDB( const string& path ):
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
    string key;
    if( name.empty() )
        return;

    string fname = name;

    key = string(fname, 0, 2);
    size_t npos = 0;
    size_t npos2 = fname.find("_", npos);
    while( npos2 != string::npos )
    {   npos = npos2+1;
        key += string(fname, npos, 2);
        npos2 = fname.find("_", npos);
    }
    key += string(fname, npos+2);

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
        cout << "EJDB open error" << endl;
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
    string clfile = dir + "/Changelog.txt";
    fstream ff( clfile.c_str(), ios::out);
    ff << "File " << name.c_str() << " created on " << curDate().c_str() << " " << curTime().c_str() << endl;
    ff << "<Version> = v0.1" << endl;
}


/// Read version from Changelog.txt file
void TEJDB::readVersion( )
{
    // open changelog.txt file
    string clfile = dir + "/Changelog.txt";
    fstream ff( clfile.c_str(), ios::in);
    string fbuf;
    size_t pos;
    version = "not versioned";

    while( ff.good() )
    {
      getline ( ff ,fbuf);
      pos = fbuf.find("<Version>");
      if( pos != string::npos )
      {
         pos = fbuf.find("=", pos+1);
         if( pos != string::npos )
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
TFile::TFile( const string& path, FileStatus aMode ):
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


//--------------------- End of f_ejdb_file.cpp ---------------------------

