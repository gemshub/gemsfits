//-------------------------------------------------------------------
// $Id: FITMainWindow.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of GEMSFITS GUI Main Window (window part)
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

#include <QKeyEvent>
#include <QProcess>
#include <QtGlobal>
#include <QTextStream>

#include "FITMainWindow.h"
#include "ui_FITMainWindow.h"
#include "HelpWindow.h"
#include "FitResultsWindow.h"
#include "f_ejdb.h"

//--------------------------------------------------------------------------

void TKeyTable::keyPressEvent(QKeyEvent* e)
{
    QTableWidget::keyPressEvent(e);
    switch( e->key() )
    {
      case Qt::Key_Up:
      case Qt::Key_Down:
      case Qt::Key_PageUp:
      case Qt::Key_PageDown:
        pFitImp->openRecordKey(  currentRow(), currentColumn()  );
        break;
    }
}

//------------------------------------------------------------------------------

FITMainWindow* pFitImp;

void FITMainWindow::setDefValues(int c, char** v)
{
   // load main programm settingth
   mainSettings = new QSettings("gemsfits.ini", QSettings::IniFormat);
   getDataFromPreferences();
}

void FITMainWindow::getDataFromPreferences()
{
  if( !mainSettings)
   return;

  SysFITDir =  mainSettings->value("ResourcesFolderPath", "../Resources/").toString().toUtf8().data();
  LocalDocDir =  mainSettings->value("HelpFolderPath", "../Resources/help").toString().toUtf8().data();
  UserDir = mainSettings->value("UserFolderPath", ".").toString().toUtf8().data();
  useComments = mainSettings->value("PrintComments", false).toBool();

  QString program = mainSettings->value("Gemsfit2ProgramPath", "gemsfit2").toString();
  fitProcess->setProgram( program );

  // load experiment template text
  QString fname = SysFITDir.c_str();
          fname += "/data/" + mainSettings->value("ExpTemplateFileName", "...").toString();
  QFile tmpString(fname);
  if(tmpString.open( QIODevice::ReadOnly))
  {
    ExpTemplate = tmpString.readAll();
    tmpString.close();
  }

  // load experiment search text
  fname = SysFITDir.c_str();
          fname += "/templates/" + mainSettings->value("TemplateSearchFileName", "...").toString();
  QFile tmpString1(fname);
  if(tmpString1.open( QIODevice::ReadOnly))
  {
    SrchTemplate = tmpString1.readAll();
    tmpString1.close();
  }

  if( mainSettings->value("PrintGEMSFITMessages", true).toBool())
   connect( fitProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(showProcessMesage()) );
  else
   disconnect( fitProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(showProcessMesage()) );
}

FITMainWindow::FITMainWindow(int c, char** v, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::FITMainWindow),
    currentMode (MDF_TASK), gemsLstFile(""), fitTaskDir(""),
    aNode(0), contentsChanged(false), projectSettings(0),
    mainSettings(0), helpProcess(0)
{
    ui->setupUi(this);
    axisLabelFont = QFont("Courier New", 14);
    ui->actionCancel_gemsfit2_run->setEnabled(false);

    // define tool bar
    toolTasks = new QToolBar(this);
    toolTasks->setObjectName(QString::fromUtf8("toolTasks"));
    //toolTasks->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolTasks->setMovable(false);
    addToolBar(Qt::LeftToolBarArea, toolTasks );
    toolTasks->setIconSize(QSize(40,40));
    toolTasks->addAction(ui->action_DataBase_mode);
    toolTasks->addAction(ui->action_Task_Mode);
    toolTasks->addAction(ui->actionFits_View_Mode);
    toolTasks->setWindowTitle("toolTasks");

    // define main window
    keyTable = new TKeyTable( this );
    ui->gridLayout->addWidget( keyTable, 1, 0, 1, 2 );
    keyTable->setEditTriggers( QAbstractItemView::NoEditTriggers );
    keyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    keyTable->setSelectionBehavior ( QAbstractItemView::SelectRows );

    ui->splKey->setStretchFactor(0, 2);
    ui->splKey->setStretchFactor(1, 1);
    ui->splGEMS->setStretchFactor(0, 1);
    ui->splGEMS->setStretchFactor(1, 3);
    ui->splRecord->setStretchFactor(0, 4);
    ui->splRecord->setStretchFactor(1, 1);

    ui->splV->setStretchFactor(0, 1);
    ui->splV->setStretchFactor(1, 3);
    ui->splV->setStretchFactor(2, 2);

    // set up menu
    setActions();
    connect(ui->splV, SIGNAL(splitterMoved( int , int  )), this, SLOT(moveToolBar( int , int  )));
    connect( keyTable, SIGNAL(cellClicked ( int , int  ) ), this, SLOT(openRecordKey( int, int )));
    connect( ui->recordEdit, SIGNAL(textChanged()),  this, SLOT(recEdited()));
    connect( ui->filterEdit, SIGNAL( editingFinished ()), this, SLOT(changeKeyList()) );

    // setup process
     fitProcess = new QProcess( this);
     //connect( fitProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(showProcessMesage()) );
     connect( fitProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT( runFinished(int,QProcess::ExitStatus)));
     //connect( fitProcess, SIGNAL(readyReadStandardError()), this, SLOT(ReadErr()) );


   //set up main parameters
    setDefValues( c, v);
   // setup first lists
    CmDBMode();


}

FITMainWindow::~FITMainWindow()
{
    delete ui;
}

void FITMainWindow::closeEvent(QCloseEvent* e)
{
    //if( pVisor->CanClose() )
    {
       for(int ii=0; ii<10000; ii++);

       if( HelpWindow::pDia )
          delete HelpWindow::pDia;

       if( FitResultsWindow::pDia )
       {
           FitResultsWindow::pDia->close();
           //delete  FitResultsWindow::pDia;
       }
       //mdiArea->closeAllSubWindows();
       //if( mdiArea->subWindowList().count() > 0 )
       //    e->ignore();
       //else
       //    QWidget::closeEvent(e);
    }
}

void FITMainWindow::resizeEvent ( QResizeEvent * event )
{
    QMainWindow::resizeEvent(event);
    moveToolBar(1,1);
}

void FITMainWindow::showEvent ( QShowEvent * event )
{
    QMainWindow::showEvent(event);
    moveToolBar(1,1);
}

/*!
   Opens html <file> positioned on anchor <item> (as in "file:/myfile.html#item")

   NOTE: we should not specify parent parameter for modeless dialog
   because otherwise even that it allows work with parent window it will
   be always overlapping it. For modal windows (and thus modal help) we need parent
*/
void FITMainWindow::GetHelp( )
{
        (new HelpWindow(  0  ));
        //HelpWindow::pDia->show();
}

void FITMainWindow::OpenHelp(const char* file, const char* item1, int page )
{
    if( HelpWindow::pDia )
    {
cout << "OpenHelp: " << file << endl;
        if( item1 && page>=0 )
       {
          QString res = item1;
          res += QString("_%1").arg(page);
          string txt = res.toUtf8().data();
          HelpWindow::pDia->showDocumentation( file, txt.c_str() );
        }
        else
          HelpWindow::pDia->showDocumentation( file, item1 );

       HelpWindow::pDia->show();
       HelpWindow::pDia->raise();
    }
   // old help assistantClient->showDocumentation( file, item);
}

void FITMainWindow::OpenResults( const string& key, const QString& dir )
{
    if( !FitResultsWindow::pDia )
    {
        ( new FitResultsWindow(0) );
    }
    if( !dir.isEmpty() )
      FitResultsWindow::pDia->ShowResults(key, dir);
    else
      FitResultsWindow::pDia->ShowResults(key );

    FitResultsWindow::pDia->raise();
}


void  FITMainWindow::moveToolBar( int , int )
{
   ui->toolBar_2->setFixedWidth(toolTasks->width());
   ui->toolBarTask->setFixedWidth(ui->splV->widget(0)->width());
   ui->toolBarMenu->setFixedWidth(ui->splV->widget(1)->width());
}

void FITMainWindow::setTableIComp()
{
   string valStr;
   QTableWidgetItem *item;

   DATACH  *dCH =  node()->pCSD();
   ui->tableIComp->clear();

   ui->tableIComp->setColumnCount(5);
   ui->tableIComp->setRowCount(dCH->nIC/5+1);

   for( int ii = 0; ii<dCH->nIC; ii++ )
   {
     valStr = string( dCH->ICNL[ii], 0,MaxICN );
     item = new QTableWidgetItem(tr("%1").arg( valStr.c_str()));
     ui->tableIComp->setItem(ii/5, ii%5, item );
   }
}

void FITMainWindow::setListPhase()
{
   int ii, jj,j;
   string valStr;

   DATACH  *dCH =  node()->pCSD();
   ui->listPhases->clear();

   for( ii = 0, jj=0; ii<dCH->nPH; ii++ )
   {
     valStr = string( dCH->PHNL[ii], 0,MaxPHN );
     QTreeWidgetItem *phase = new QTreeWidgetItem(ui->listPhases);
     phase->setText(0, trUtf8(valStr.c_str()));

     for( j=0; j<dCH->nDCinPH[ii]; j++, jj++ )
     {
         valStr = string( dCH->DCNL[jj], 0, MaxDCN );
         QTreeWidgetItem *dcomp = new QTreeWidgetItem(phase);
         dcomp->setText(0, trUtf8(valStr.c_str()));
     }
   }
}

void FITMainWindow::setStatusText( const string& text )
{
  QString vals = trUtf8(text.c_str()) + "\n";
  ui->statusEdit->setPlainText( vals );
}

void FITMainWindow::addLinetoStatus( const string& line )
{
    //QString vals = ui->statusEdit->toPlainText();
    //vals += trUtf8(line.c_str()) + "\n";
    //ui->statusEdit->setPlainText( vals );
    ui->statusEdit->append( trUtf8( line.c_str()) + "\n" );
}

void FITMainWindow::showProcessMesage( )
{
    QProcess *p = dynamic_cast<QProcess *>( sender() );
    if (p)
        ui->statusEdit->append( p->readAllStandardOutput() );

 }

//----------------------------------------------------------
// Working with EJDB

void FITMainWindow::closeEJDB()
{
  for( int ii; ii<rtEJ.size(); ii++ )
     rtEJ[ii].Close();
  EJDBFile.Close();
}

/// Connect project database
void FITMainWindow::openEJDB()
{
    // close old project data
    closeEJDB();

    // set up new ejdb file
    string ejdbPath  = projectSettings->value("ProjFolderPath", ".").toString().toUtf8().data();
    ejdbPath  += projectSettings->value("ProjDatabasePath", "/EJDB").toString().toUtf8().data();
    ejdbPath  += "/";
    ejdbPath  += projectSettings->value("ProjDatabaseName", "myprojdb1" ).toString().toUtf8().data();
    EJDBFile.ChangePath(ejdbPath);

    // change collections names
    string samlescolName = projectSettings->value("ExpSamplesDataColl", "experiments").toString().toUtf8().data();
    rtEJ[MDF_DATABASE].SetKeywd(samlescolName);
    rtEJ[MDF_DATABASE].Open();
    string testcolName = projectSettings->value("TaskCasesDataColl", "tests").toString().toUtf8().data();
    rtEJ[MDF_TASK].SetKeywd(testcolName);
    rtEJ[MDF_TASK].Open();
    testcolName = projectSettings->value("FitsCasesDataColl", "fits").toString().toUtf8().data();
    rtEJ[MDF_FITS].SetKeywd(testcolName);
    rtEJ[MDF_FITS].Open();
}

/// Set up data for current project
void FITMainWindow::loadNewProject()
{
    // clear old GEMS3K list windows
    ui->tableIComp->clear();
    ui->listPhases->clear();
    pLineGEMS->setText( trUtf8( "" ) );
    // clear all queries
    ui->queryEdit->setText(trUtf8( "" ) );
    rtEJ[MDF_DATABASE].SetQueryJson("");
    rtEJ[MDF_TASK].SetQueryJson("");
    rtEJ[MDF_FITS].SetQueryJson("");

    // Connect project database
    openEJDB();

    pLineTask->setText( projectSettings->value("ProjFileName", "undefined").toString());
    // change gemsLstFile information
    string gemsname  = projectSettings->value("ProjFolderPath", ".").toString().toUtf8().data();
    gemsname  += projectSettings->value("GEMS3KFilesPath", "/GEMS").toString().toUtf8().data();
    gemsname += "/undef.lst";
    gemsLstFile.ChangePath( gemsname );
    gemsLstFile.ChangeName("");

    // update key list, editor, filter
    resetMainWindow();
}

/// Set up data for Main window
void FITMainWindow::resetMainWindow()
{
    // set up filter
    ui->filterEdit->setText("*");

    // update key list
    int curInd = defineModuleKeysList( currentMode );

    // load first record
    if( keyTable->rowCount() > 0 )
    {
       if( curInd >= 0 )
         openRecordKey( curInd, 0  );
       else
       { QTableWidgetItem *curItem = keyTable->item(0,0);
         keyTable->setCurrentItem( curItem );
         keyTable->scrollToItem( curItem );
         openRecordKey( 0, 0  );
       }
    } else
      {
        // loadTemplate( currentModule );
        CmCreate();
    }

    // reset  ui->queryEdit
    ui->queryEdit->setText( trUtf8( rtEJ[currentMode].GetLastQuery().c_str()));
    if(  !rtEJ[currentMode].GetLastQuery().empty() )
    {
      ui->action_Insert->setEnabled(false);
      ui->actionCreate_New->setEnabled(false);
      ui->actionRestore_from_csv->setEnabled(false);
      ui->actionRestore_from_JSON->setEnabled(false);
      ui->actionRestore_from_TXT->setEnabled(false);
      ui->actionRestore_from_YAML->setEnabled(false);
    }
    else
    {
      ui->action_Insert->setEnabled(true);
      ui->actionCreate_New->setEnabled(true);
      ui->actionRestore_from_JSON->setEnabled(true);
      ui->actionRestore_from_YAML->setEnabled(true);
      if( currentMode == MDF_DATABASE )
        ui->actionRestore_from_csv->setEnabled(true);
      else
        ui->actionRestore_from_TXT->setEnabled(true);
    }

}


void FITMainWindow::changeKeyList()
{
   defineModuleKeysList( currentMode );
}

/// Define list of Module keys using filter
int FITMainWindow::defineModuleKeysList( int nRT )
{
  int ii, jj, kk, ln;
  int curInd = -1;
  string keyfld;
  QTableWidgetItem *item, *curItem=0;
  string oldKey = rtEJ[nRT].PackKey();
  //settedCureentKeyIntotbKeys = false;

  if( currentMode != nRT)
    return curInd;

  // define tbKeys
  keyTable->clear();
  keyTable->setSortingEnabled ( false );
  keyTable->setColumnCount( rtEJ[nRT].KeyNumFlds());

  // get list or record keys
  string keyFilter = ui->filterEdit->text().toUtf8().data();
  if( keyFilter.empty() )
      keyFilter = ALLKEY;
  vector<string> keyList;
  int nKeys = rtEJ[nRT].GetKeyList( keyFilter.c_str(), keyList);

  //vector<int> colSizes;
  //for(jj=0; jj<rtEJ[nRT].KeyNumFlds(); jj++)
  // colSizes.push_back( 0 );

  // define key list
  keyTable->setRowCount(nKeys);

  // set up table sizes
  QFontMetrics fm(keyTable->fontMetrics());
  int charWidth = fm.width("5");
  int charHeight = fm.height();

  for( ii=0; ii<nKeys; ii++ )
  {
      keyTable->setRowHeight(ii, charHeight+2);
      for(jj=0, kk=0; jj<rtEJ[nRT].KeyNumFlds(); jj++)
      {
          ln = keyList[ii].find_first_of(':', kk);
          keyfld = string(keyList[ii], kk, ln-kk);
          strip(keyfld);
          //colsz = keyfld.length()+1;
          //if( colsz > colSizes[jj])
          //    colSizes[jj] = colsz;
          kk = ln+1;
          item = new QTableWidgetItem(tr("%1").arg( keyfld.c_str()));
          keyTable->setItem(ii, jj, item );
       }
      if( oldKey == keyList[ii] )
      {    curItem = keyTable->item(ii,0);
           curInd = ii;
    //       settedCureentKeyIntotbKeys = true;
      }
  }
  for(jj=0; jj<rtEJ[nRT].KeyNumFlds(); jj++)
  {
      //keyTable->setColumnWidth(jj, charWidth*colSizes[jj] );
      item = new QTableWidgetItem(tr("%1").arg( rtEJ[nRT].FldKeyName(jj) ));
      //item->setToolTip( ((TCModule*)aMod[nRT])->GetFldHelp(jj));
      keyTable->setHorizontalHeaderItem( jj, item );
  }

  keyTable->setSortingEnabled ( true );
  if(curItem )
  {
    keyTable->setCurrentItem( curItem );
    keyTable->scrollToItem( curItem, QAbstractItemView::PositionAtCenter );
  }

  rtEJ[nRT].SetKey(oldKey.c_str());
  return curInd;
}


/// Save record structure to Data Base
void FITMainWindow::RecSave( const string& recBsonText, const char* key )
{
    rtEJ[ currentMode ].SetJson( recBsonText );
    rtEJ[ currentMode ].SaveRecord( key );
    //defineModuleKeysList( currentMode ); //?? need change key list if new record
    contentsChanged = false;
}

/// Delete record structure from Data Base
void FITMainWindow::RecDelete( const char* key )
{
    rtEJ[ currentMode ].Del( key );
    if( currentMode == MDF_TASK && rtEJ[ MDF_FITS ].Find(key) )
         rtEJ[ MDF_FITS ].Del( key );
}

/// Save solicitation
/// Returns true if user pressed 'save' or 'discard' and false on 'cancel'
bool FITMainWindow::MessageToSave()
{
    string key_str = rtEJ[ currentMode ].PackKey();
    if( contentsChanged && key_str.find_first_of("*?") == string::npos )
    {
        int res = vfQuestion3(window(), key_str.c_str(),
                       "Data record has been changed!",
               "Save changes", "Discard changes", "Cancel");
        if( res == VF3_3 )
        {
            // set current key as selected in table keys
            defineModuleKeysList( currentMode ); //?? may be function copy of this to only reset current line in table
            return false;
        }

        if( res == VF3_1 )
        {
            string recBson = ui->recordEdit->toPlainText().toUtf8().data();
            RecSave( recBson, key_str.c_str() );
         }
    }
    contentsChanged = false;
    return true;
}

/// Change <SystemFiles> data in edit record
string FITMainWindow::makeSystemFileName( const string& path  )
{
   string name = path;
   if(projectSettings )
          name += projectSettings->value("GEMS3KFilesPath", "/GEMS").toString().toUtf8().data();
   name += "/" + gemsLstFile.Name() + "." + gemsLstFile.Ext();
   return name;
}

/// Change <SystemFiles> or <DataDB> data in edit record
void FITMainWindow::changeEditeRecord(const string& tagname, const string& newValue)
{
   // get value string
   string valueStr = ui->recordEdit->toPlainText().toUtf8().data();

   // delete old path string
   size_t found = valueStr.find( tagname );
   if (found != string::npos)
   {
       // change value
       found += 12;
       size_t  found1 =  valueStr.find("\"", found );
       size_t  found2 =  valueStr.find("\"", found1+1 );
       valueStr.replace( found1+1, found2-found1-1, newValue);
       ui->recordEdit->setText( trUtf8( valueStr.c_str() ));
       contentsChanged = true;
   }
}


/// Make template for task record
bool FITMainWindow::createTaskTemplate()
{
    if( gemsLstFile.Name().empty() )
       Error("Create task template", "Undefined System File name.");

    // create arguments string
    string newPath = makeSystemFileName( "." );
    QStringList cParameters;
    cParameters << "-init" << newPath.c_str() << "template.dat";

    if( !runProcess( cParameters, fitTaskDir.Dir().c_str()) )
       Error("Run gemsfit -init", "Error started process.");

    int ret = fitProcess->waitForFinished();

    // read "template.dat" to bson
    string path = fitTaskDir.Dir()+ "/template.dat";
    TFile  inFile(path, ios::in);
    readTXT( inFile );

    // set up EJDB path
    if( projectSettings )
     { string ejdbPath  = "..";
       ejdbPath  += projectSettings->value("ProjDatabasePath", "/EJDB").toString().toUtf8().data();
       ejdbPath  += "/";
       ejdbPath  += projectSettings->value("ProjDatabaseName", "myprojdb1" ).toString().toUtf8().data();
       changeEditeRecord( "DataDB", ejdbPath);
    }
    return ret;
}


bool FITMainWindow::runProcess( const QStringList& cParameters, const QString& workDir )
{
    fitProcess->setArguments( cParameters);
    fitProcess->setWorkingDirectory( workDir );

    fitProcess->start();
    return fitProcess->waitForStarted();
}

void FITMainWindow::runFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->action_Run_test->setEnabled(true);
    ui->action_Show_Results->setEnabled(true);
    ui->actionCancel_gemsfit2_run->setEnabled(false);

    if( exitStatus==0 && exitCode==0 )
        addLinetoStatus( "gemsfit2 calculation o.k." );
    else
        addLinetoStatus( "Error in gemsfit2 run." );
}

/// Make list of Experiments keys using query
void FITMainWindow::defineModuleKeysList( string& samplelist )
{
  int ii, kk, ln;
  string keyfld;

  // get list or record keys
  string keyFilter = ALLKEY;
  vector<string> keyList;
  int nKeys = rtEJ[ MDF_DATABASE ].GetKeyList( keyFilter.c_str(), keyList);

  samplelist = " \"DataSelect\": {\n    \"samplelist\": [\n";
  for( ii=0; ii<nKeys; ii++ )
  {
      samplelist += "{ \"SA\": \"";
      kk=0;
      ln = keyList[ii].find_first_of(':', kk);
      keyfld = string(keyList[ii], kk, ln-kk);
      strip(keyfld);
      kk = ln+1;
      samplelist += keyfld + "\", \"DS\": \"";
      ln = keyList[ii].find_first_of(':', kk);
      keyfld = string(keyList[ii], kk, ln-kk);
      strip(keyfld);
      samplelist += keyfld;
      samplelist += "\", \"WT\": 1 }";
      if( ii != nKeys-1)
          samplelist += ",\n";
      else
          samplelist += "\n";
  }
  samplelist += "]\n } }";
}

//--------------- end of  FITMainWindow.cpp  -----------------------------
