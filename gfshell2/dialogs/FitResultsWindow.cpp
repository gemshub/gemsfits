//-------------------------------------------------------------------
// $Id: FitResultsWindow.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of GEMSFITS GUI Results Window
//
// Copyright (C) 2014  S.V.Dmytriyeva, D.A.Kulik
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

#include <iostream>
#include <QLabel>
#include <QFileDialog>
#include <QCloseEvent>
#include <QSortFilterProxyModel>
#include "FitResultsWindow.h"
#include "FITMainWindow.h"
#include "ui_FitResultsWindow.h"
#include "DialogFindFromPlot.h"


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
    modelFitResults = new TMatrixModel( "fit-results", 2 );
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
    modelFitParams = new TMatrixModel( "fit-params", 3, 0 );
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
    modelFitInverse = new TMatrixModel( "fit-inverse-results", 4, 0 );
    proxyModel = new TSortFilterProxyModel();
    proxyModel->setSourceModel( modelFitInverse );
    tableFitInverse = new TMatrixTable( 0 /*ui->tabInverse*/);
    deleg = new TMatrixDelegate( 4, this);
    tableFitInverse->setItemDelegate(deleg);
    tableFitInverse->setModel(proxyModel/*modelFitInverse*/);
    ui->verticalLayout_9->addWidget(tableFitInverse);

    // set up FIT_MC_FILE
    modelMCResults = new TMatrixModel( "mc-results", 0, 0 );
    proxyModel = new TSortFilterProxyModel();
    proxyModel->setSourceModel( modelMCResults );
    tableMCResults = new TMatrixTable( 0 /*ui->tabMcResults*/);
    deleg = new TMatrixDelegate( 0, this);
    tableMCResults->setItemDelegate(deleg);
    tableMCResults->setModel(proxyModel/*modelMCResults*/);
    ui->verticalLayout_5->addWidget(tableMCResults);

    fFitStatistic = "sum-statistics";   // name file  FIT_STATISTIC
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

       pDia = 0;
       ev->accept();
}

void FitResultsWindow::ShowResults( const string& key )
{
   pLineTask->setText( trUtf8( key.c_str() ));
   readBsonRecord();
   show();
}

void FitResultsWindow::ShowResults( const string& key, const QString& dir )
{
   pLineTask->setText( trUtf8( key.c_str() ));
   readBsonRecord();
   CmOpenFile( dir );
   show();
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
   { // select directory
     dir = QFileDialog::getExistingDirectory(this, "Select work/output  Directory (Only test!!!)",
     "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
   } else
      dir = dir_;

  modelFitResults->matrixFromCsvFile( dir );
  modelFitParams->matrixFromCsvFile( dir );
  modelMCResults->matrixFromCsvFile( dir );
  modelSensitivity->matrixFromCsvFile( dir );
  modelQQplot->matrixFromCsvFile( dir );
  modelFitInverse->matrixFromCsvFile( dir );
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
    editFiledsToFile( dir );
}

void FitResultsWindow::editFiledsFromFile( const QString& dir )
{
    QString valCsv ="";

    // read file  FIT_STATISTIC
    QString fpath = dir + "/" + fFitStatistic+".txt";
    QFile tmpStriam(fpath);
    if(tmpStriam.open( QIODevice::ReadOnly ))
    {
      valCsv = tmpStriam.readAll();
      tmpStriam.close();
    }
    else
      {
        cout << "error open file " << fpath.toUtf8().data() << endl;
      }
    ui->textStatistic->setText(valCsv);

    // read file  FIT_LOGFILE
    valCsv ="";
    fpath = dir + "/" + fFitLogfile+".log";
    QFile tmpStriam1(fpath);
    if(tmpStriam1.open( QIODevice::ReadOnly ))
    {
      valCsv = tmpStriam1.readAll();
      tmpStriam1.close();
    }
    else
      {
        cout << "error open file " << fpath.toUtf8().data() << endl;
      }
    ui->textEdit->setText(valCsv);

}

void FitResultsWindow::editFiledsToFile( const QString& dir )
{
    // write file  FIT_STATISTIC
    QString valCsv = ui->textStatistic->toPlainText();
    QString fpath = dir + "/" + fFitStatistic+".txt";
    QFile tmpStriam(fpath);
    if(tmpStriam.open( QIODevice::WriteOnly|QIODevice::Truncate))
    {
      tmpStriam.write(valCsv.toUtf8());
      tmpStriam.close();
    }
    // write file  FIT_LOGFILE
    valCsv = ui->textEdit->toPlainText();
    fpath = dir + "/" + fFitLogfile+".log";
    QFile tmpStriam1(fpath);
    if(tmpStriam1.open( QIODevice::WriteOnly|QIODevice::Truncate))
    {
      tmpStriam1.write(valCsv.toUtf8());
      tmpStriam1.close();
    }
}

void FitResultsWindow::CmSaveBsonRecord()
{
    bson bsrec;
    string key = pLineTask->text().toUtf8().data();

  try
  {
    if( key.empty() )
      return;
    rtEJ[MDF_FITS ].SetKey(key.c_str());

    bson_init( &bsrec );
    // added key to bson record
    rtEJ[MDF_FITS ].putKeyToBson( &bsrec );

    // added tableViews to bson Record
    modelFitParams->matrixToBson( &bsrec );
    modelFitResults->matrixToBson( &bsrec );
    modelMCResults->matrixToBson( &bsrec );
    modelSensitivity->matrixToBson( &bsrec );
    modelQQplot->matrixToBson( &bsrec );
    modelFitInverse->matrixToBson( &bsrec );

    // added text bufers to bson record
    // write file  FIT_STATISTIC
    string valCsv = ui->textStatistic->toPlainText().toUtf8().data();
    string name = fFitStatistic.toUtf8().data();
    int iRet = bson_append_string( &bsrec, name.c_str(), valCsv.c_str() );
    ErrorIf( iRet == BSON_ERROR, name, "Error append string"+name );
    // write file  FIT_LOGFILE
    valCsv = ui->textEdit->toPlainText().toUtf8().data();
    name = fFitLogfile.toUtf8().data();
    iRet = bson_append_string( &bsrec, name.c_str(), valCsv.c_str() );
    ErrorIf( iRet == BSON_ERROR, name, "Error append string"+name );

    bson_finish( &bsrec );

    //set bson to string
    ParserJson pars;
    string recBsonText;
    pars.printBsonObjectToJson( recBsonText, bsrec.data );
    bson_destroy( &bsrec);

    // save record
    rtEJ[ MDF_FITS ].SetJson( recBsonText );
    if( rtEJ[ MDF_FITS ].Find( key.c_str() ))
        rtEJ[ MDF_FITS ].SaveRecord( key.c_str());
    else
        rtEJ[ MDF_FITS ].InsertRecord();

   }
    catch( TError& err )
    {
      cout << err.title << err.mess << endl;
    }
}


void FitResultsWindow::readBsonRecord()
{
    bson bsrec;
    string key = pLineTask->text().toUtf8().data();

  try
  {
    if( key.empty() )
      return;

    bson_init( &bsrec );

    // empty data
    if( rtEJ[ MDF_FITS ].Find( key.c_str() ))
    {  //read record
        rtEJ[ MDF_FITS ].Get( key.c_str() );
        string valDB =rtEJ[ MDF_FITS ].GetJson();

        //get bson bsrec;
        ParserJson pars;
        pars.setJsonText( valDB.substr( valDB.find_first_of('{')+1 ) );
        bson_init( &bsrec );
        pars.parseObject( &bsrec );
     }
     bson_finish( &bsrec );

        //  tableViews from bson Record
       modelFitParams->matrixFromBson( (QSortFilterProxyModel *)tableFitParams->model(), bsrec.data );
       modelFitResults->matrixFromBson( (QSortFilterProxyModel *)tableFitResults->model(), bsrec.data );
       modelMCResults->matrixFromBson( (QSortFilterProxyModel *)tableMCResults->model(), bsrec.data );
       modelSensitivity->matrixFromBson( (QSortFilterProxyModel *)tableSensitivity->model(), bsrec.data );
       modelQQplot->matrixFromBson( (QSortFilterProxyModel *)tableQQplot->model(),  bsrec.data );
       modelFitInverse->matrixFromBson( (QSortFilterProxyModel *)tableFitInverse->model(), bsrec.data );

       // added text bufers to bson record
       // write file  FIT_STATISTIC
       string name = fFitStatistic.toUtf8().data();
       string valCsv;
       if( !bson_find_string( bsrec.data, name.c_str(), valCsv ) )
           valCsv = "";
       ui->textStatistic->setText(valCsv.c_str());

       // write file  FIT_LOGFILE
       name = fFitLogfile.toUtf8().data();
       if( !bson_find_string( bsrec.data, name.c_str(), valCsv ) )
           valCsv = "";
       ui->textEdit->setText(valCsv.c_str());

    bson_destroy( &bsrec);
   }
    catch( TError& err )
    {
      cout << err.title << err.mess << endl;
    }
}

/// Export Data Records to json txt-file
void FitResultsWindow::CmBackupJSON()
{
    try
    {
       // select record list to unload
       vector<string> aKey = vfMultiKeys( this, "Please, mark records to be unloaded to JSON",
              MDF_FITS, "*" );
        if( aKey.size() <1 )
               return;

       // open file to unloading
        string fname;
        TFile  outFile("", ios::out );
        if( !outFile.ChooseFileSave( this, fname, "Please, give a file name for unloading records","*.json" ))
             return;
        outFile.Open();

        outFile.ff << "[\n";
        for(int i=0; i<aKey.size(); i++ )
        {
          rtEJ[ MDF_FITS ].Get( aKey[i].c_str() );
          string valDB =rtEJ[ MDF_FITS ].GetJson();
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
        cout << err.title << err.mess << endl;
    }
}

/// Delete current record
void FitResultsWindow::CmDeleteRecord()
{
    try
    {
        string key = pLineTask->text().toUtf8().data();
        if( !rtEJ[ MDF_FITS ].Find(key.c_str()) )
           return;

        if( vfQuestion( this, rtEJ[ MDF_FITS ].GetKeywd(),
               "Confirm deletion of data record keyed "+ key ))
             rtEJ[ MDF_FITS ].Del( key.c_str() );
    }
    catch( TError& err )
    {
        cout << err.title << err.mess << endl;
    }
}

void FitResultsWindow::CmPlotTable()
{
    try
    {
        TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
        if( !tableCurrent )
          return;

        string title = "Task ";
               title  += pLineTask->text().toUtf8().data();
        QSortFilterProxyModel *pmodel = (QSortFilterProxyModel *)tableCurrent->model();
        ((TMatrixModel *)pmodel->sourceModel())->showGraphData(pmodel, title );

        //graphList.insert( graph_dlg );
    }
    catch( TError& err )
    {
        cout << err.title << err.mess << endl;
    }
}

void FitResultsWindow::CmPrintTable()
{
    try
    {

    }
    catch( TError& err )
    {
        cout << err.title << err.mess << endl;
    }
}

/*
 *
 * void MainWindow::Print()
{
QPrinter printer(QPrinter::HighResolution);
QPrintDialog *dlg = new QPrintDialog(&printer, this);
dlg->setWindowTitle(tr("Print ACLs"));

if (dlg->exec() != QDialog::Accepted)
return;

PrintTableView *tempTableView = new PrintTableView();
tempTableView->setModel(model);
tempTableView->printTable(printer);
}

PrintTableView::PrintTableView(QWidget *parent) : QTableView (parent)
{

}

void PrintTableView::printTable(QPrinter &printer)
{
QString str;
QPainter painter;
painter.begin(&printer);
const int ItemSize = 256;
int rows = model()->rowCount(QModelIndex());
int columns = model()->columnCount(QModelIndex());
int sourceWidth = (columns+1) * ItemSize;
int sourceHeight = (rows+1) * ItemSize;
painter.save();

double xscale = printer.pageRect().width();
double yscale = printer.pageRect().height();
double scale = qMin(xscale, yscale);

painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
printer.paperRect().y() + printer.pageRect().height()/2);
painter.scale(scale, scale);
painter.translate(-sourceWidth/2, -sourceHeight/2);

QStyleOptionViewItem option = viewOptions();

float x = ItemSize /2;
for (int printRow = 0; printRow < rows; ++printRow) {
float y = ItemSize /2;
for (int column = 0; column < columns; ++column) {
option.rect = QRect(int(x), int(y), ItemSize, ItemSize);
itemDelegate()->paint(&painter, option, model()->index(printRow, column, QModelIndex()));
x = x + 256;
}
y = y + 256;
}
painter.restore();
painter.end();
}

QStyleOptionViewItem PrintTableView::viewOptions() const
{
QStyleOptionViewItem option = QAbstractItemView->viewOptions();
return option;
}
 * */

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
    TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
    if( tableCurrent && tableCurrent->model()->rowCount()>0 )
      tableCurrent->CmCalc();
}

void FitResultsWindow::SelectColumn()
{
    TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
      tableCurrent->SelectColumn();
}

void FitResultsWindow::SelectAll()
{
    TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
      tableCurrent->SelectAll();
}

void FitResultsWindow::CutData()

{
    TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
      tableCurrent->CutData();
}

void FitResultsWindow::ClearData()
{
    TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
      tableCurrent->ClearData();
}

void FitResultsWindow::CopyData()
{
    TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
      tableCurrent->CopyData();
}

void FitResultsWindow::PasteData()
{
    TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
      tableCurrent->PasteData();
}

void FitResultsWindow::PasteTransposedData()
{
    TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
      tableCurrent->PasteTransposedData();
}

void FitResultsWindow::ToggleX()
{
    TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
      tableCurrent->ToggleX();
}

void FitResultsWindow::ToggleY()
{
    TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
    if( tableCurrent && tableCurrent->model()->rowCount()>0)
      tableCurrent->ToggleY();
}

void FitResultsWindow::CmFindFromPlot()
{
  try
  {

     TMatrixTable *tableCurrent = dynamic_cast<TMatrixTable*>(ui->tabsResults->currentWidget()->focusWidget());
     if( !tableCurrent )
        return;

     QSortFilterProxyModel *pmodel = (QSortFilterProxyModel *)tableCurrent->model();
     TMatrixModel *matrmodel = (TMatrixModel *)pmodel->sourceModel();
     GraphData *grdata = matrmodel->getGraphData();
     //if( !grdata )
     //  return;

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
    { QModelIndex ind = pmodel->mapFromSource(matrmodel->index(frstrow, 0));
      tableCurrent->selectRow(ind.row());
    }

  }
    catch( TError& err )
    {
    cout << err.title << err.mess << endl;
    }
}

// ------------------------ end of FitResultsWindow.cpp ------------------------------------------
