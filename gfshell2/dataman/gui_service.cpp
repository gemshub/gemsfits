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


#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

#include "gui_service.h"
#include "f_file.h"

bool ChooseFileOpen(common::TAbstractFile* file, QWidget* par, std::string& template_path,
                    const char* title, const char *filter)
{
    if( file && template_path.find('/') == std::string::npos ) {
        template_path  = file->WorkDir()+template_path;
    }

    QString filt;
    if( filter )
        filt = QString("Text files (%1);;All files (*)").arg(filter);
    else
        filt = "All files (*)";

    QString fn = QFileDialog::getOpenFileName(par, title, template_path.c_str(), filt, 0,
                                              QFileDialog::DontConfirmOverwrite );
#ifdef buildWIN32
    std::replace( fn.begin(), fn.end(), '/', '\\');
#endif
    if ( !fn.isEmpty() ) {
        QFileInfo flinfo(fn);
        common::TAbstractFile::current_work_directory = flinfo.dir().path().toStdString()+"/";
        template_path = fn.toStdString();
        if(file)  {
            file->setMode(std::ios::in);
            file->ChangePath(template_path);
        }
        return true;
    }
    else {
        template_path = "";
        return false;
    }

}

QStringList ChooseListFilesOpen(QWidget* par, const std::string& dir_path,
                                const std::string& title, const std::string& filter)
{
    QString dir;
    if( dir_path.empty() ) {
        dir  = common::TAbstractFile::current_work_directory.c_str();
    }

    QString filt;
    if( !filter.empty() )
        filt = QString("Text files (%1);;All files (*)").arg(filter.c_str());
    else
        filt = "All files (*)";

    auto fileNames = QFileDialog::getOpenFileNames(par, title.c_str(), dir, filt);

    if ( !fileNames.isEmpty() ) {
        QFileInfo flinfo(fileNames[0]);
        common::TAbstractFile::current_work_directory = flinfo.dir().path().toStdString()+"/";
    }
   return fileNames;
}

bool ChooseFileSave(common::TAbstractFile* file, QWidget* par, std::string& template_path,
                    const char* title, const char *filter)
{
    if( file && template_path.find('/') == std::string::npos ) {
        template_path  = file->WorkDir()+template_path;
    }
    replace(template_path.begin(), template_path.end(),'\\', '/');

    QStringList filt;
    if( filter )
        filt += QString("Files (%1)").arg(filter);
    else
        filt += "Text files (*.txt)";

    filt += "All files (*)";

    QString selectedFilter;
    QString fn = QFileDialog::getSaveFileName(par, title, template_path.c_str(), filt.join( ";;" ),
                                              &selectedFilter, QFileDialog::DontConfirmOverwrite);

    if ( !fn.isEmpty() ) {
        QFileInfo flinfo(fn);
        common::TAbstractFile::current_work_directory = flinfo.dir().path().toStdString()+"/";
        template_path = fn.toStdString();
        if(file) {
            file->setMode(std::ios::out);
            file->ChangePath(template_path);
        }
        return true;
    }
    else {
        template_path = "";
        return false;
    }
}

// internal functions
bool removeDirectoryEntry(QDir dir)
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


//------------------------------------------------------------------
// service functions

void messageCritical(QWidget* par, const std::string& title, const std::string& mess)
{
    QString titl, spac, messag;
    titl = title.c_str(); spac = "\n\n"; messag = mess.c_str();

    QMessageBox::critical(par,
                      #ifdef __unix
                      #ifdef __APPLE__
                          "Title", titl.append(spac+=messag)
                      #else
                          titl, messag
                      #endif
                      #else
                          titl, messag
                      #endif
                          );
}

static int posx=0, posy=0;
// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget* par, const std::string& title, const std::string& mess, const std::string& s1,
                const std::string& s2,  const std::string& s3, bool i_mov)
{
    QString titl, spac, messag;
    titl = title.c_str(); spac = "\n\n"; messag = mess.c_str();

    QMessageBox qm( QMessageBox::Question,
                #ifdef __unix
                #ifdef __APPLE__
                    "Title", titl.append(spac+=messag),
                #else
                    titl, messag,
                #endif
                #else
                    titl, messag,
                #endif
                    QMessageBox::NoButton, par);

    QAbstractButton *yesButton = qm.addButton(s1.c_str(), QMessageBox::YesRole);
    QAbstractButton *noButton = qm.addButton(s2.c_str(), QMessageBox::NoRole);
    QAbstractButton *cancelButton = nullptr;
    if( !s3.empty() )
        cancelButton = qm.addButton(s3.c_str(), QMessageBox::RejectRole);
    if( i_mov )
        qm.move(posx, posy);
    qm.exec();
    if( i_mov )
    {
        posx = qm.x();
        posy = qm.y();
    }
    if (qm.clickedButton() == yesButton) {
        return VF3_1;
    }
    else if (qm.clickedButton() == noButton) {
        return VF3_2;
    }
    else if (qm.clickedButton() == cancelButton) {
        return VF3_3;
    }
    return VF3_3;
}

bool vfQuestion(QWidget* par, const std::string& title, const std::string& mess)
{
    QString titl, spac, messag;
    titl = title.c_str(); spac = "\n\n"; messag = mess.c_str();

    int rest = (QMessageBox::question(par,
                                  #ifdef __unix
                                  #ifdef __APPLE__
                                      "Title", titl.append(spac+=messag),
                                  #else
                                      titl, messag,
                                  #endif
                                  #else
                                      titl, messag,
                                  #endif
                                      QMessageBox::Yes | QMessageBox::No));
    return rest==QMessageBox::Yes;
}


//--------------------- End of gui_service.cpp ---------------------------

