//-------------------------------------------------------------------
// $Id: help.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of HelpConfigurator class
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

#include <fstream>
#include <iostream>
using namespace std;

#include "help.h"

// read string untill "end"
void HelpConfigurator::u_getline(istream& is, QString& str, QString end )
{
    char ch;
    is.get(ch);
    while( !is.eof() && is.good()  )
    {
        str += ch;
        if( str.indexOf(end) > 0)
        {
          break;
        }
        is.get(ch);
    }
}

// read all reference from file
void HelpConfigurator::getHrefs( QString file, QString file_name)
{
   char ch;
   fstream f_in( file.toUtf8().data(), ios::in );
   QString ref;

   if( !f_in.good() )
   {
#ifdef IPMGEMPLUGIN
        cout << file.toUtf8().data() << " Fileopen error" << endl;
#else
        file += " Fileopen error";
        Error( "HelpConfigurator", file.toUtf8().data());
#endif
         return;
   }

   while( !f_in.eof() )
   {
       f_in.get(ch);
       if(  ch == '<' )
       {
           f_in.get(ch);
           if( ch == 'a' )  // <a name="keywd"></a>
           {
               ref = "<a";
               u_getline(f_in, ref, "</a>" );
               addNameToList(ref, file_name);
               continue;
           }
           if( ch == 'i')  // <img src="file" ... >
           {
               ref = "<i";
               u_getline(f_in, ref, ">" );
               addImgToList(ref );
               continue;
           }
       }
   }
}

// read all reference from file
void HelpConfigurator::addNameToList( QString ref, QString file_name )
{
    //QRegExp reg("(\\w+(?::\\w+)?)=(\"[^\"]+\"|'[^']+')");
    QString key;
    QString value;

    int indx = ref.indexOf("name=");
// cout << indx << "    " << ref.toUtf8().data() << endl;
    if(indx > -1 )
    {
      names.append(ref);
      key =ref.section("\"",1,1);
      value = file_name +"#"+key;
      links.insertMulti(key,QUrl(value));
// cout << value.toUtf8().data()<<endl;
    }
    else
        hrefs.append(ref);

}

// read all images from file
void HelpConfigurator::addImgToList( QString ref )
{
    QString value, rref, file_name;
    int indx = ref.indexOf("src=");
// cout << indx << "    " << ref.toUtf8().data(); // << endl;
    if(indx > -1 )   // Bugfixes by DK on 15.02.2012
    {
      rref = ref.mid(indx);
      value = rref.section("\"",1,1);
// cout << "+ " << value.toUtf8().data() << endl;
      file_name = value.section("/", -1);
// cout << "- " << file_name.toUtf8().data() << endl;
      images.append(file_name);
    }
    else
        others.append(ref);
}

// read and parse all *.html files from dir
int HelpConfigurator::readDir(const char *dir)
{
    path = dir;
    QDir thisDir(dir);
    if (!thisDir.isReadable())
    {
#ifdef IPMGEMPLUGIN
        cout <<  " GEMS DB directory is not readable" << endl;
#else
        Error( "HelpConfigurator", "GEMS DB directory is not readable");
#endif
        return 0;
    }

    thisDir .setFilter(QDir::Files);
    QStringList filters;
    filters << "*.html";
    thisDir.setNameFilters(filters);

    QFileInfoList files = thisDir.entryInfoList();
    if (files.empty())
        return 0;

    QListIterator<QFileInfo> it(files);
    QFileInfo f;
    while ( it.hasNext() )
    {
        f = it.next();
        if (f.isFile() )
        {
            getHrefs( f.filePath(), f.fileName() );
            addFileToList( f.fileName());
        }
    }
    return 1;
}

// Build *.qhp file for help system (gems3helpconfig.qhp by default)
int HelpConfigurator::writeFile(const char *file)
{
    fstream f_out( file, ios::out );
    if( !f_out.good() )
    {
#ifdef IPMGEMPLUGIN
        cout << file << " Fileopen error" << endl;
#else
        QString str = QString(file) + " Fileopen error";
        Error( "HelpConfigurator", str.toUtf8().data());
#endif
         return 0;
    }

    // Write head
    f_out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    f_out << "<QtHelpProject version=\"1.0\">" << endl;
    f_out << "    <namespace>gemsfits</namespace>" << endl;
    f_out << "    <virtualFolder>help</virtualFolder>" << endl;
    f_out << "    <filterSection>" << endl;

    writeContent( f_out);
    writeFiles( f_out );
    writeKeywords( f_out );

    // Write end
    f_out << "    </filterSection>" << endl;
    f_out << "   </QtHelpProject>" << endl;

    return 1;
}

void HelpConfigurator::writeFiles( fstream& f_out)
{
    f_out << "      <files>" << endl;
    for( int ii =0; ii<files.count(); ii++)
    {
      f_out << "        <file>" << files[ii].toUtf8().data() << "</file>" << endl;
    }
    images.sort();
    images.removeDuplicates();
    for( int ii =0; ii<images.count(); ii++)
    {
      f_out << "        <file>" << images[ii].toUtf8().data() << "</file>" << endl;
    }
    f_out << "      </files>" << endl;
}

void HelpConfigurator::writeKeywords( fstream& f_out)
{
    QStringList kwds;
    QList<QUrl> urls;

    //kwds = links.keys();
    //for(int ii=0; ii<links.count(); ii++ )
    // cout << kwds[ii].toUtf8().data() << endl;

    kwds = links.uniqueKeys();
    f_out << "    <keywords>" << endl;
    for( int ii =0; ii<kwds.count(); ii++)
    {
#ifndef IPMGEMPLUGIN
        // add only keywords for objects
        if( showObjectForKeyword(kwds[ii])<0)
          continue;
#endif
        urls = links.values( kwds[ii] );

      for( int jj=0; jj<urls.count(); jj++ )
      f_out << "        <keyword name=\"" << kwds[ii].toUtf8().data()
            << "\" ref=\"" << urls[jj].toString().toUtf8().data()  << "\"/>" << endl;
    }
    f_out << "    </keywords>" << endl;
}

void HelpConfigurator::writeContent( fstream& f_out)
{
    QString ref;
    QString contentfile = path;
            contentfile += "/gfshelpconfig.toc";
    fstream f_in( contentfile.toUtf8().data(), ios::in );
    if( !f_in.good() )
    {
#ifdef IPMGEMPLUGIN
        cout << contentfile.toUtf8().data() << " Fileopen error" << endl;
#else
        contentfile += " Fileopen error";
        Error( "HelpConfigurator", contentfile.toUtf8().data());
#endif
         return;
    }

    while( !f_in.eof() )
    {
      ref = "";
      u_getline(f_in, ref, ">" );
      f_out << ref.toUtf8().data();
    }
}

#ifndef IPMGEMPLUGIN

#include "v_object.h"

// Searches for reference <keyword> in the list of objects
//    <keyword> should be in format: objectlabel or objectlabel_<index>
int HelpConfigurator::showObjectForKeyword(const QString &keyword)
{
    int nO = -1;

    // finding full name, not subString
    nO = aObj.Find(keyword.toUtf8().data());
    if( nO < 0 )
    {
       QString kwInternal = keyword;
       int ndx = kwInternal.lastIndexOf('_');  // finding only keyword
       if(ndx > -1)
       {    kwInternal= kwInternal.remove(QRegExp("_[0-9]{1,3}")/*ndx*/);
            nO = aObj.Find(kwInternal.toUtf8().data());
       }
    }

    return nO;
}

#endif


// ----------------- End of help.cpp -----------------------

