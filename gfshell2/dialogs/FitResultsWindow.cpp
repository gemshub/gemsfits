//-------------------------------------------------------------------
// $Id: FitResultsWindow.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of GEMSFITS GUI Results Window
//
// Copyright (C) 2014-2023  S.V.Dmytriyeva, D.A.Kulik
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
#include <QLabel>
#include <QFileDialog>
#include <QCloseEvent>
#include <QPainter>
#include <QMessageBox>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#else
#include <QPrintDialog>
#include <QPrinter>
#endif

#include "FitResultsWindow.h"
#include "ui_FitResultsWindow.h"
#include "FITMainWindow.h"
#include "DialogFindFromPlot.h"
#include "f_database.h"
#include "gui_service.h"

FitResultsWindow* FitResultsWindow::pDia = 0;

FitResultsWindow::FitResultsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FitResultsWindow)
{
    ui->setupUi(this);
    pDia = this;
    TMatrixDelegate *deleg;
    TSortFilterProxyModel *proxyModel;

    // set up FIT_CSV_FILE
    modelFitResults = new TMatrixModel( "fit-results", 2, 'e', 12 );
    tableFitResults = new TMatrixTable(0/*ui->tabFitResults*/);
    proxyModel = new TSortFilterProxyModel();
    proxyModel->setSourceModel( modelFitResults );
    deleg = new TMatrixDelegate( 2, this);
    tableFitResults->setItemDelegate(deleg);
    tableFitResults->setModel(proxyModel/*modelFitResults*/);
    ui->verticalLayout_6->addWidget(tableFitResults);

    // set up FIT_SENS_FILE
    modelSensitivity = new TMatrixModel( "meas-data-sensitivity", 1 );
    proxyModel = new TSortFilterProxyModel();
    proxyModel->setSourceModel( modelSensitivity );
    tableSensitivity = new TMatrixTable(0/*ui->tabSensitivity*/);
    deleg = new TMatrixDelegate( 1, this);
    tableSensitivity->setItemDelegate(deleg);
    tableSensitivity->setModel(proxyModel/*modelSensitivity*/);
    ui->verticalLayout_7->addWidget(tableSensitivity);

    // set up FIT_PARAM_FILE
    modelFitParams = new TMatrixModel( "fit-params", 3 );
    proxyModel = new TSortFilterProxyModel();
    proxyModel->setSourceModel( modelFitParams );
    tableFitParams = new TMatrixTable(  /*ui->tabFitParams*/);
    deleg = new TMatrixDelegate( 3, this);
    tableFitParams->setItemDelegate(deleg);
    tableFitParams->setModel(proxyModel/*modelFitParams*/);
    ui->verticalLayout_4->addWidget(tableFitParams);

    // set up FIT_SENS_FILE
    modelQQplot = new TMatrixModel( "qq-plot-data", 1 );
    proxyModel = new TSortFilterProxyModel();
    proxyModel->setSourceModel( modelQQplot );
    tableQQplot = new TMatrixTable( 0 /*ui->tabQuantiles*/);
    deleg = new TMatrixDelegate( 1, this);
    tableQQplot->setItemDelegate(deleg);
    tableQQplot->setModel(proxyModel/*modelQQplot*/);
    ui->verticalLayout_8->addWidget(tableQQplot);

    // set up FIT_NFUN_FILE
    modelFitInverse = new TMatrixModel( "fit-inverse-results", 4, 'e', 6 );
    proxyModel = new TSortFilterProxyModel();
    proxyModel->setSourceModel( modelFitInverse );
    tableFitInverse = new TMatrixTable( 0 /*ui->tabInverse*/);
    deleg = new TMatrixDelegate( 4, this);
    tableFitInverse->setItemDelegate(deleg);
    tableFitInverse->setModel(proxyModel/*modelFitInverse*/);
    ui->verticalLayout_9->addWidget(tableFitInverse);

    // set up FIT_STAT_FILE
    modelFitStatistics = new TMatrixModel( "sum-statistics", 1 );
    proxyModel = new TSortFilterProxyModel();
    proxyModel->setSourceModel( modelFitStatistics );
    tableFitStatistics = new TMatrixTable( 0 /*ui->tabStatistics*/);
    deleg = new TMatrixDelegate( 1, this);
    tableFitStatistics->setItemDelegate(deleg);
    tableFitStatistics->setModel(proxyModel/*modelFitStatistics*/);
    ui->verticalLayout_2->addWidget(tableFitStatistics);

    // set up FIT_MC_FILE
    modelMCResults = new TMatrixModel( "mc-results", 0 );
    proxyModel = new TSortFilterProxyModel();
    proxyModel->setSourceModel( modelMCResults );
    tableMCResults = new TMatrixTable( 0 /*ui->tabMcResults*/);
    deleg = new TMatrixDelegate( 0, this);
    tableMCResults->setItemDelegate(deleg);
    tableMCResults->setModel(proxyModel/*modelMCResults*/);
    ui->verticalLayout_5->addWidget(tableMCResults);

    //    fFitStatistic = "sum-statistics";   // name file  FIT_STATISTIC
    fFitLogfile = "gemsfit2";  // name file  FIT_LOGFILE

    setActions();
}


FitResultsWindow::~FitResultsWindow()
{
    delete ui;
}

void FitResultsWindow::closeEvent(QCloseEvent* ev)
{
    //     foreach (GraphDialog*value, graphList)
    //       delete value;
    modelFitParams->CloseGraph();
    modelFitResults->CloseGraph();
    modelMCResults->CloseGraph();
    modelSensitivity->CloseGraph();
    modelQQplot->CloseGraph();
    modelFitInverse->CloseGraph();
    modelFitStatistics->CloseGraph();

    pDia = 0;
    ev->accept();
}

void FitResultsWindow::ShowResults( const std::string& key )
{
    pLineTask->setText( key.c_str() );
    readBsonRecord();
    show();
}

void FitResultsWindow::ShowResults( const std::string& key, const QString& dir )
{
    pLineTask->setText( key.c_str());
    readBsonRecord();
    CmOpenFile( dir );
    show();
}

TMatrixTable *FitResultsWindow::current_table() const
{
//    return dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
    return dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->childAt(10,10));
}


//  Connect all actions
void FitResultsWindow::setActions()
{
    connect( ui->actionOpen_File, SIGNAL( triggered()), this, SLOT(CmOpenFile()));
    connect( ui->actionSave_File, SIGNAL( triggered()), this, SLOT(CmSaveFile()));
    connect( ui->actionClose, SIGNAL( triggered()), this, SLOT(close()));

    connect( ui->action_Insert, SIGNAL( triggered()), this, SLOT(CmSaveBsonRecord()));
    connect( ui->actionUpdate, SIGNAL( triggered()), this, SLOT(CmSaveBsonRecord()));
    connect( ui->actionDelete, SIGNAL( triggered()), this, SLOT(CmDeleteRecord()));
    connect( ui->actionExport, SIGNAL( triggered()), this, SLOT(CmBackupJSON()));

    connect( ui->actionAbout_GEMSFITS, SIGNAL( triggered()), this, SLOT(CmAboutGEMSFITS()));
    connect( ui->actionAbout_Graph_Dialog, SIGNAL( triggered()), this, SLOT(CmAbout_Graph_Dialog()));
    connect( ui->actionAbout_Results_window, SIGNAL( triggered()), this, SLOT(CmAbout_Results_window()));

    connect( ui->actionPlot_Results, SIGNAL( triggered()), this, SLOT(CmPlotTable()));
    connect( ui->action_Find_from_Plot, SIGNAL( triggered()), this, SLOT(CmFindFromPlot()));
    connect( ui->actionPrint, SIGNAL( triggered()), this, SLOT(CmPrintTable()));

    // edit menu
    connect( ui->actionCalculator, SIGNAL( triggered()), this, SLOT(CmCalc()));
    connect( ui->actionSelect_column, SIGNAL( triggered()), this, SLOT(SelectColumn()));
    connect( ui->actionSelect_all, SIGNAL( triggered()), this, SLOT(SelectAll()));
    connect( ui->actionC_ut, SIGNAL( triggered()), this, SLOT(CutData()));
    connect( ui->actionClear, SIGNAL( triggered()), this, SLOT(ClearData()));
    connect( ui->action_Copy, SIGNAL( triggered()), this, SLOT(CopyData()));
    connect( ui->action_Paste, SIGNAL( triggered()), this, SLOT(PasteData()));
    connect( ui->actionPaste_transposed, SIGNAL( triggered()), this, SLOT(PasteTransposedData()));
    connect( ui->actionMark_Columns_as_X, SIGNAL( triggered()), this, SLOT(ToggleX()));
    connect( ui->actionMark_Columns_as_Y, SIGNAL( triggered()), this, SLOT(ToggleY()));

    QLabel * pTask = new QLabel( "   Task:   ", ui->toolBarKey );
    ui->toolBarKey->addWidget( pTask ); // setStretchableWidget( pLine );

    pLineTask = new QLineEdit( ui->toolBarKey );
    pLineTask->setEnabled( true );
    pLineTask->setFocusPolicy( Qt::ClickFocus );
    pLineTask->setReadOnly( true );
    pLineTask->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    ui->toolBarKey->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    ui->toolBarKey->addWidget( pLineTask ); // setStretchableWidget( pLine );

}

void FitResultsWindow::CmOpenFile( const QString& dir_ )
{
    QString dir;
    if( dir_.isEmpty() )
    {
        // select directory
        dir = QFileDialog::getExistingDirectory(this, "Select work/output  Directory (Only test!!!)",
                                                "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    }
    else
        dir = dir_;

    modelFitResults->matrixFromCsvFile( dir );
    modelFitParams->matrixFromCsvFile( dir );
    modelMCResults->matrixFromCsvFile( dir );
    modelSensitivity->matrixFromCsvFile( dir );
    modelQQplot->matrixFromCsvFile( dir );
    modelFitInverse->matrixFromCsvFile( dir );
    modelFitStatistics->matrixFromCsvFile( dir );
    editFiledsFromFile( dir );
}

void FitResultsWindow::CmSaveFile()
{
    // select directory
    QString dir = QFileDialog::getExistingDirectory(this, "Select work/output  Directory (Only test!!!)",
                                                    "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    modelFitResults->matrixToCsvFile( dir );
    modelFitParams->matrixToCsvFile( dir );
    modelMCResults->matrixToCsvFile( dir );
    modelSensitivity->matrixToCsvFile( dir );
    modelQQplot->matrixToCsvFile( dir );
    modelFitInverse->matrixToCsvFile( dir );
    modelFitStatistics->matrixToCsvFile( dir );
    editFiledsToFile( dir );
}

void FitResultsWindow::editFiledsFromFile( const QString& dir )
{
    QString valCsv ="";

    // read file  FIT_STATISTIC
    //    QString fpath = dir + "/" + fFitStatistic+".txt";
    //    QFile tmpStriam(fpath);
    //    if(tmpStriam.open( QIODevice::ReadOnly ))
    //    {
    //      valCsv = tmpStriam.readAll();
    //      tmpStriam.close();
    //    }
    //    else
    //      {
    //        cout << "error open file " << fpath.toStdString() << endl;
    //      }
    //    ui->textStatistic->setText(valCsv);

    // read file  FIT_LOGFILE
    valCsv ="";
    QString fpath = dir + "/" + fFitLogfile+".log";
    QFile tmpStriam1(fpath);
    if(tmpStriam1.open( QIODevice::ReadOnly ))
    {
        valCsv = tmpStriam1.readAll();
        tmpStriam1.close();
    }
    else
    {
        std::cout << "error open file " << fpath.toStdString() << std::endl;
    }
    ui->textEdit->setText(valCsv);

}

void FitResultsWindow::editFiledsToFile( const QString& dir )
{
    // write file  FIT_STATISTIC
    //    QString valCsv = ui->textStatistic->toPlainText();
    //    QString fpath = dir + "/" + fFitStatistic+".txt";
    //    QFile tmpStriam(fpath);
    //    if(tmpStriam.open( QIODevice::WriteOnly|QIODevice::Truncate))
    //    {
    //      tmpStriam.write(valCsv.toUtf8());
    //      tmpStriam.close();
    //    }
    // write file  FIT_LOGFILE
    QString valCsv = ui->textEdit->toPlainText();
    QString fpath = dir + "/" + fFitLogfile+".log";
    QFile tmpStriam1(fpath);
    if(tmpStriam1.open( QIODevice::WriteOnly|QIODevice::Truncate))
    {
        tmpStriam1.write(valCsv.toUtf8());
        tmpStriam1.close();
    }
}

void FitResultsWindow::CmSaveBsonRecord()
{
    std::string key = pLineTask->text().toStdString();
    try
    {
        if( key.empty() )
            return;
        rtEJ[MDF_FITS].setKey(key);

        common::JsonFree object;
        // added key to json record
        rtEJ[MDF_FITS].addKeyToJson(object);

        // added tableViews to json Record
        modelFitParams->matrixToBson(object);
        modelFitResults->matrixToBson(object);
        modelMCResults->matrixToBson(object);
        modelSensitivity->matrixToBson(object);
        modelQQplot->matrixToBson(object);
        modelFitInverse->matrixToBson(object);
        modelFitStatistics->matrixToBson(object);

        // added text bufers to json record
        std::string valCsv = ui->textEdit->toPlainText().toStdString();
        std::string name = fFitLogfile.toStdString();
        object[name] = valCsv;

        // json to string
        std::string recBsonText = object.dump();

        // save record
        rtEJ[ MDF_FITS ].setJson(recBsonText);
        if( rtEJ[ MDF_FITS ].findRecord(key))
            rtEJ[ MDF_FITS ].saveRecord(key);
        else
            rtEJ[ MDF_FITS ].insertRecord();

    }
    catch( TError& err )
    {
        std::cout << err.title << err.mess << std::endl;
    }
}


void FitResultsWindow::readBsonRecord()
{
    std::string key = pLineTask->text().toStdString();
    try
    {
        common::JsonFree object;
        if( key.empty() )
            return;

        // empty data
        if(rtEJ[MDF_FITS].findRecord(key))  {
            //read record
            rtEJ[MDF_FITS].getRecord(key);
            object = fromJsonString(rtEJ[MDF_FITS].getJson());
        }

        //  tableViews from json Record
        modelFitParams->matrixFromBson( (QSortFilterProxyModel *)tableFitParams->model(), object );
        modelFitResults->matrixFromBson( (QSortFilterProxyModel *)tableFitResults->model(), object );
        modelMCResults->matrixFromBson( (QSortFilterProxyModel *)tableMCResults->model(), object );
        modelSensitivity->matrixFromBson( (QSortFilterProxyModel *)tableSensitivity->model(), object );
        modelQQplot->matrixFromBson( (QSortFilterProxyModel *)tableQQplot->model(), object );
        modelFitInverse->matrixFromBson( (QSortFilterProxyModel *)tableFitInverse->model(), object );
        modelFitStatistics->matrixFromBson( (QSortFilterProxyModel *)tableFitStatistics->model(), object );
        std::string name = fFitLogfile.toStdString();
        std::string valCsv = object.value(name, "");
        ui->textEdit->setText(valCsv.c_str());
    }
    catch( TError& err )
    {
        std::cout << err.title << err.mess << std::endl;
    }
}

/// Export Data Records to json txt-file
void FitResultsWindow::CmBackupJSON()
{
    try
    {
        // select record list to unload
        std::vector<std::string> aKey = vfMultiKeys( this, "Please, mark records to be unloaded to JSON",
                                                     MDF_FITS, "*" );
        if( aKey.size() <1 )
            return;

        // open file to unloading
        std::string fname;
        common::TFile  outFile("", std::ios::out );
        if( !ChooseFileSave(&outFile, this, fname, "Please, give a file name for unloading records","*.json" ))
            return;
        outFile.Open();

        outFile.ff << "[\n";
        for(size_t i=0; i<aKey.size(); i++ )
        {
            rtEJ[ MDF_FITS ].getRecord(aKey[i]);
            std::string valDB =rtEJ[ MDF_FITS ].getJson();
            outFile.ff << valDB;
            if( i<aKey.size()-1)
                outFile.ff <<  ",";
            outFile.ff <<  "\n";
        }
        outFile.ff << "]";
        outFile.Close();
    }
    catch( TError& err )
    {
        std::cout << err.title << err.mess << std::endl;
    }
}

/// Delete current record
void FitResultsWindow::CmDeleteRecord()
{
    try
    {
        std::string key = pLineTask->text().toStdString();
        if( !rtEJ[ MDF_FITS ].findRecord(key) )
            return;

        if( vfQuestion( this, rtEJ[MDF_FITS].getKeywd(),
                        "Confirm deletion of data record keyed "+ key ))
            rtEJ[ MDF_FITS ].deleteRecord(key);
    }
    catch( TError& err )
    {
        std::cout << err.title << err.mess << std::endl;
    }
}

void FitResultsWindow::CmPlotTable()
{
    try
    {
        TMatrixTable *tableCurrent = current_table();
        if( !tableCurrent )
            return;

        std::string title = "Task ";
        title  += pLineTask->text().toStdString();
        QSortFilterProxyModel *pmodel = (QSortFilterProxyModel *)tableCurrent->model();
        ((TMatrixModel *)pmodel->sourceModel())->showGraphData(pmodel, title );

        //graphList.insert( graph_dlg );
    }
    catch( TError& err )
    {
        QMessageBox::critical( this, err.title.c_str(), err.mess.c_str() );
        std::cout << err.title << err.mess << std::endl;
    }
}

void FitResultsWindow::CmPrintTable()
{
    try
    {
        TMatrixTable *tableCurrent = current_table();
        if( !(tableCurrent && tableCurrent->model()->rowCount()>0) )
            return;

        QPrinter printer(QPrinter::HighResolution);
        QPrintDialog *dlg = new QPrintDialog(&printer, this);
        dlg->setWindowTitle(tr("Print table"));

        if (dlg->exec() != QDialog::Accepted)
            return;

        if( QPageLayout::Landscape != printer.pageLayout().orientation() )
            printer.setPageOrientation(QPageLayout::Landscape);

        QPainter painter;
        painter.begin(&printer);
        double xscale = printer.pageRect(QPrinter::DevicePixel).width()/double(tableCurrent->width());
        double yscale = printer.pageRect(QPrinter::DevicePixel).height()/double(tableCurrent->height());
        double scale = qMin(xscale, yscale);
        painter.translate(printer.paperRect(QPrinter::DevicePixel).x() + printer.pageRect(QPrinter::DevicePixel).width()/2,
                          printer.paperRect(QPrinter::DevicePixel).y() + printer.pageRect(QPrinter::DevicePixel).height()/2);
        painter.scale(scale, scale);
        painter.translate(-tableCurrent->width()/2, -tableCurrent->height()/2);

        tableCurrent->render(&painter);


        //QPainter painter(&printer);
        //tableCurrent->printTable(&painter, printer.pageRect());

    }
    catch( TError& err )
    {
        std::cout << err.title << err.mess << std::endl;
    }
}

void FitResultsWindow::CmAboutGEMSFITS()
{
    pFitImp->OpenHelp( GF_PREFS_HTML );
}

void FitResultsWindow::CmAbout_Graph_Dialog()
{
    pFitImp->OpenHelp( GF_PREFS_HTML );
}

void FitResultsWindow::CmAbout_Results_window()
{
    pFitImp->OpenHelp( GF_PREFS_HTML );
}

void FitResultsWindow::CmCalc()
{
    TMatrixTable *tableCurrent = current_table();
    if( tableCurrent && tableCurrent->model()->rowCount()>0 )
        tableCurrent->CmCalc();
}

void FitResultsWindow::SelectColumn()
{
    TMatrixTable *tableCurrent = current_table();
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
        tableCurrent->SelectColumn();
}

void FitResultsWindow::SelectAll()
{
    TMatrixTable *tableCurrent = current_table();
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
        tableCurrent->SelectAll();
}

void FitResultsWindow::CutData()

{
    TMatrixTable *tableCurrent = current_table();
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
        tableCurrent->CutData();
}

void FitResultsWindow::ClearData()
{
    TMatrixTable *tableCurrent = current_table();
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
        tableCurrent->ClearData();
}

void FitResultsWindow::CopyData()
{
    TMatrixTable *tableCurrent = current_table();
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
        tableCurrent->CopyData();
}

void FitResultsWindow::PasteData()
{
    TMatrixTable *tableCurrent = current_table();
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
        tableCurrent->PasteData();
}

void FitResultsWindow::PasteTransposedData()
{
    TMatrixTable *tableCurrent = current_table();
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
        tableCurrent->PasteTransposedData();
}

void FitResultsWindow::ToggleX()
{
    TMatrixTable *tableCurrent = current_table();
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
        tableCurrent->ToggleX();
}

void FitResultsWindow::ToggleY()
{
    TMatrixTable *tableCurrent = current_table();
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
        tableCurrent->ToggleY();
}

void FitResultsWindow::CmFindFromPlot()
{
    try
    {

        TMatrixTable *tableCurrent = current_table();
        if( !tableCurrent )
            return;

        QSortFilterProxyModel *pmodel = (QSortFilterProxyModel *)tableCurrent->model();
        TMatrixModel *matrmodel = (TMatrixModel *)pmodel->sourceModel();
        auto* grdata = matrmodel->getGraphData();
        if( !grdata )
            return;

        // define new project
        DialogFindFromPlot dlg( grdata, this);
        if( !dlg.exec() )
            return;

        //find data from dlg
        int xyndx[2];
        double reg[4];
        dlg.getData( xyndx, reg );

        //search by data
        int frstrow = matrmodel->findRow( xyndx, reg);
        if( frstrow >= 0 )
        {
            QModelIndex ind = pmodel->mapFromSource(matrmodel->index(frstrow, 0));
            tableCurrent->selectRow(ind.row());
        }

    }
    catch( TError& err )
    {
        std::cout << err.title << err.mess << std::endl;
    }
}

// ------------------------ end of FitResultsWindow.cpp ------------------------------------------
