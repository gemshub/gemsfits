
#include <QKeyEvent>
#include "FITMainWindow.h"
#include "ui_FITMainWindow.h"
#include "HelpWindow.h"
#include "f_ejdb.h"
#include "fservice.h"

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
   SysFITDir = "../Resources/";
   LocalDocDir = SysFITDir+ "help/";

   // may be gemsLstFile from arguments c,v
   // may be fitTaskDir from arguments c,v
}


FITMainWindow::FITMainWindow(int c, char** v, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::FITMainWindow),
    currentMode (MDF_TASK), gemsLstFile(""), fitTaskDir(""),
    aNode(0), contentsChanged(false), projectSettings(0)
{
    ui->setupUi(this);

    // define tool bar
    toolTasks = new QToolBar(this);
    toolTasks->setObjectName(QString::fromUtf8("toolTasks"));
    //toolTasks->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolTasks->setMovable(false);
    addToolBar(Qt::LeftToolBarArea, toolTasks );
    toolTasks->setIconSize(QSize(40,40));
    toolTasks->addAction(ui->action_DataBase_mode);
    toolTasks->addAction(ui->action_Task_Mode);
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
    QString vals = ui->statusEdit->toPlainText();
    vals += trUtf8(line.c_str()) + "\n";
    ui->statusEdit->setPlainText( vals );
    //QString vals = trUtf8(line.c_str()) + "\n";
    //ui->statusEdit->textCursor().movePosition(QTextCursor::End);
    //ui->statusEdit->insertPlainText(vals);
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
}

/// Set up data for current project
void FITMainWindow::loadNewProject()
{
    // Connect project database
    openEJDB();

    pLineTask->setText( projectSettings->value("ProjFileName", "undefined").toString());

    // update key list, editor, filter
    resetMainWindow();
}

/// Set up data for Main window
void FITMainWindow::resetMainWindow()
{
    // set up filter
    ui->filterEdit->setText("*");

    // update key list
    defineModuleKeysList( currentMode );

    // load first record
    if( keyTable->rowCount() > 0 )
    {  QTableWidgetItem *curItem = keyTable->item(0,0);
       keyTable->setCurrentItem( curItem );
       keyTable->scrollToItem( curItem );
       openRecordKey( 0, 0  );
    } else
      {
        // loadTemplate( currentModule );
        ui->recordEdit->setText( trUtf8("must be template"));
        contentsChanged = false;
    }

    // reset  ui->queryEdit
    ui->queryEdit->setText( trUtf8(""));
}


void FITMainWindow::changeKeyList()
{
   defineModuleKeysList( currentMode );
}

/// Define list of Module keys using filter
void FITMainWindow::defineModuleKeysList( int nRT )
{
  int ii, jj, kk, ln;
  string keyfld;
  QTableWidgetItem *item, *curItem=0;
  string oldKey = rtEJ[nRT].PackKey();
  //settedCureentKeyIntotbKeys = false;

  if( currentMode != nRT)
    return;

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
}


/// Save record structure to Data Base
void FITMainWindow::RecSave( const string& recBsonText, const char* key )
{
    rtEJ[ currentMode ].SetJson( recBsonText );
    rtEJ[ currentMode ].SaveRecord( key );
    //defineModuleKeysList( currentMode ); //?? need change key list if new record
    contentsChanged = false;
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
