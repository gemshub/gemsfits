//-------------------------------------------------------------------
// $Id: PreferencesDialog.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of PreferencesDialog class
//
// Copyright (C) 2014  S.V.Dmytriyeva
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

#include <iostream>
#include <QFileDialog>
#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"
#include "FITMainWindow.h"
#include "help.h"

PreferencesDialog::PreferencesDialog(QSettings *aSet,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog), settings(aSet)
{
    ui->setupUi(this);
    if( settings ) //load old settings
    {
        ui->resourcesEdit->setText( settings->value("ResourcesFolderPath", "../Resources/").toString() );
        ui->helpEdit->setText( settings->value("HelpFolderPath", "../Resources/help").toString() );
        ui->usersEdit->setText( settings->value("UserFolderPath", ".").toString() );
        ui->gemsfit2Edit->setText( settings->value("Gemsfit2ProgramPath", "gemsfit2").toString() );
        ui->commentsBox->setChecked( settings->value("PrintComments", true).toBool() );
        ui->turnoff->setChecked( settings->value("PrintGEMSFITMessages", true).toBool() );
        ui->yamlBox->setChecked( settings->value("ViewinYAMLFormat", false).toBool() );
        ui->editorBox->setChecked( settings->value("ViewinModelEditor", true).toBool() );

        // load all template files
        QDir thisDir(ui->resourcesEdit->text()+"/data");
        if (thisDir.exists())
        {
            // thisDir.setFilter(QDir::Files|QDir::NoDot | QDir::NoDotDot);
            QStringList filters;
            filters << "*.dat";
            thisDir.setNameFilters(filters);
            QListIterator<QFileInfo> it(thisDir.entryInfoList());
            QFileInfo f;
            while ( it.hasNext() )
            {
                f = it.next();;
                ui->experimentsBox->addItem(f.fileName());
            }
            ui->experimentsBox->setCurrentText(settings->value("ExpTemplateFileName", "...").toString());
        }
    }

    // load all template files
    QDir thisDir(ui->resourcesEdit->text()+"/templates");
    if (thisDir.exists())
    {
        // thisDir.setFilter(QDir::Files|QDir::NoDot | QDir::NoDotDot);
        QStringList filters;
        filters << "*.dat";
        thisDir.setNameFilters(filters);
        QListIterator<QFileInfo> it(thisDir.entryInfoList());
        QFileInfo f;
        while ( it.hasNext() )
        {
            f = it.next();;
            ui->searchBox->addItem(f.fileName());
        }
        ui->searchBox->setCurrentText(settings->value("TemplateSearchFileName", "...").toString());
    }

    QObject::connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(CmSave()));
    QObject::connect( ui->buttonBox, SIGNAL(helpRequested()), this, SLOT(CmHelp()));
    QObject::connect( ui->helpButton, SIGNAL(clicked()), this, SLOT(CmHelpFile()));
    QObject::connect( ui->gemsfit2Button, SIGNAL(clicked()), this, SLOT(CmGemsFit2File()));
    QObject::connect( ui->usersButton, SIGNAL(clicked()), this, SLOT(CmProjectDir()));
    QObject::connect( ui->resourcesButton, SIGNAL(clicked()), this, SLOT(CmResourceDir()));
    QObject::connect( ui->pushButton, SIGNAL(clicked()), this, SLOT(CmGenerateHelp()));
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::CmSave()
{
    if( !settings )
        return;

    settings->setValue("ResourcesFolderPath",    ui->resourcesEdit->text() );
    settings->setValue("HelpFolderPath",    ui->helpEdit->text() );
    settings->setValue("UserFolderPath",    ui->usersEdit->text() );
    settings->setValue("Gemsfit2ProgramPath",    ui->gemsfit2Edit->text() );

    settings->setValue("ExpTemplateFileName", ui->experimentsBox->currentData(  Qt::DisplayRole ) );
    settings->setValue("TemplateSearchFileName", ui->searchBox->currentData(  Qt::DisplayRole ) );
    settings->setValue("PrintComments",  ui->commentsBox->isChecked() );
    settings->setValue("PrintGEMSFITMessages",  ui->turnoff->isChecked() );
    settings->setValue("ViewinYAMLFormat",  ui->yamlBox->isChecked() );
    settings->setValue("ViewinModelEditor",  ui->editorBox->isChecked() );

    settings->sync();
    accept();
}

void PreferencesDialog::CmProjectDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Project Directory",
                                                    "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->usersEdit->setText( dir );
}

void PreferencesDialog::CmResourceDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Resource Directory",
                                                    "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->resourcesEdit->setText( dir );
}

void PreferencesDialog::CmHelpFile()
{
    QString projDir = ui->resourcesEdit->text();
    QString dir = QFileDialog::getExistingDirectory(this, "Select Help Directory",
                                                    projDir,  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->helpEdit->setText( dir );
}

void PreferencesDialog::CmGemsFit2File()
{
    QString dir = QFileDialog::getOpenFileName(  this,
                                                 "Select gemsfite program", "", "All files (*)", 0, QFileDialog::DontConfirmOverwrite );
    ui->gemsfit2Edit->setText( dir );
}

void PreferencesDialog::CmGenerateHelp()
{
    try
    {
        QString phpdir =  ui->resourcesEdit->text()+"/doc/html/";
        QString qhpFile = phpdir +"gfshelpconfig.qhp";
        HelpConfigurator rr;
        if( rr.readDir(phpdir.toUtf8().data()) )
            rr.writeFile(qhpFile.toUtf8().data());

        if (!pFitImp->helpProcess)
            pFitImp->helpProcess = new QProcess();

        if (pFitImp->helpProcess->state() != QProcess::Running)
        {
            QString docPath =  phpdir;
            QString app;
#ifdef __unix
#ifdef __APPLE__
            app += QLatin1String("qcollectiongenerator");
#else
            //app = QLatin1String(getenv("HOME"));
            //app += QLatin1String("/Gems3-app/qcollectiongenerator");
            app += QLatin1String("qcollectiongenerator");
#endif
#else    // windows
            app += QLatin1String("qcollectiongenerator.exe");
#endif
            QStringList args;
            args << docPath + QLatin1String("gfshelpconfig.qhcp")
                 << QLatin1String("-o")
                 << docPath + QLatin1String("gfshelp.qhc");
            ;
            pFitImp->helpProcess->start(app, args);
            std::cout << app.toStdString() << std::endl;
            std::cout << args[2].toStdString() << std::endl;

            if (!pFitImp->helpProcess->waitForStarted())
            {
                Error( "gemsfits", "Unable to launch qcollectiongenerator");
            }
        }
    }
    catch(TError& e)
    {
        std::cout <<  e.title <<  e.mess << std::endl;
    }
}


void PreferencesDialog::CmHelp()
{
    pFitImp->OpenHelp( GF_PREFS_HTML );
}


// ------------------------ end of PreferencesDialog.cpp ------------------------------------------
