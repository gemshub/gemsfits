//-------------------------------------------------------------------
// $Id: help.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of HelpConfigurator class
//
// Copyright (C) 2010-2014  S.V.Dmytriyeva
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
//-----------------------------------------------------------------

#ifndef __help_h
#define __help_h

#include <QMap>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QDir>

class HelpConfigurator
{
  QString path;
  QMap<QString, QUrl> links;
  QStringList images;
  QStringList files;

  void getHrefs( QString file, QString file_name);
  void u_getline(istream& is, QString& str, QString end );
  void addNameToList( QString ref, QString file_name );
  void addImgToList(  QString ref );
  void addFileToList(  QString file )
  {
      files.append(file);
  }

  void writeFiles( fstream& f_out);
  void writeKeywords( fstream& f_out);
  void writeContent( fstream& f_out);

#ifndef IPMGEMPLUGIN
  int showObjectForKeyword(const QString &keyword);
#endif

public:
  HelpConfigurator(){}

  int readDir(const char *dir);
  int writeFile(const char *file);

protected:
// only for test
 QStringList hrefs;
 QStringList names;
 QStringList others;

};

#endif  //__help_h
