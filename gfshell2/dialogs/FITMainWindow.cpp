
#include <QKeyEvent>
#include "FITMainWindow.h"
#include "ui_FITMainWindow.h"
#include "HelpWindow.h"

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
        break;
        //pVisorImp->openRecordKey(  currentRow(), currentColumn()  );
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
    currentMode (MDF_DATABASE), gemsLstFile(""), fitTaskDir(""),
    aNode(0), projectSettings(0)
{
    ui->setupUi(this);

    //set up main parameters
    setDefValues( c, v);

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
    connect(ui->splV, SIGNAL(splitterMoved( int , int  )),
                this, SLOT(moveToolBar( int , int  )));
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

/*
//------------------------------------------------------------------------------------
// Define list of Module keys using filter
void TVisorImp::defineModuleKeysList( int nRT )
{
  int ii, jj, kk, ln, colsz;
  string keyfld;
  QTableWidgetItem *item, *curItem=0;
  string oldKey = rt[nRT].UnpackKey();
  settedCureentKeyIntotbKeys = false;

  if(currentNrt != nRT)
    return;

  pFilterKey->setText(((TCModule*)aMod[nRT])->getFilter());

  // define tbKeys
  tbKeys->clear();
  tbKeys->setSortingEnabled ( false );
  tbKeys->setColumnCount( rt[nRT].KeyNumFlds());


  // get list or record keys
  string keyFilter = pFilterKey->text().toUtf8().data();
  vector<int> colSizes;
  vector<string> keyList;
  int nKeys = rt[nRT].GetKeyList( keyFilter.c_str(), keyList);

  for(jj=0; jj<rt[nRT].KeyNumFlds(); jj++)
   colSizes.push_back( 0/*rt[nRT].FldLen(jj)* );

  // define key list
  tbKeys->setRowCount(nKeys);

  for( ii=0; ii<nKeys; ii++ )
  {
      tbKeys->setRowHeight(ii, htF(ftString, 0)+2);
      for(jj=0, kk=0; jj<rt[nRT].KeyNumFlds(); jj++)
      {

          ln = rt[nRT].FldLen(jj);
          keyfld = string(keyList[ii], kk, ln);
          strip(keyfld);
          colsz = keyfld.length()+1;
          if( colsz > colSizes[jj])
              colSizes[jj] = colsz;
          kk += ln;
          item = new QTableWidgetItem(tr("%1").arg( keyfld.c_str()));
          tbKeys->setItem(ii, jj, item );
       }
      if( oldKey == keyList[ii] )
      {    curItem = tbKeys->item(ii,0);
           settedCureentKeyIntotbKeys = true;
      }

  }
  for(jj=0; jj<rt[nRT].KeyNumFlds(); jj++)
  {
      tbKeys->setColumnWidth(jj, wdF( ftString, colSizes[jj], eNo ) );
      item = new QTableWidgetItem(tr("%1").arg( jj+1));
      item->setToolTip( ((TCModule*)aMod[nRT])->GetFldHelp(jj));
      tbKeys->setHorizontalHeaderItem( jj, item );
  }

  tbKeys->setSortingEnabled ( true );
  if(curItem )
  {
    tbKeys->setCurrentItem( curItem );
    tbKeys->scrollToItem( curItem, QAbstractItemView::PositionAtCenter );
  }

  if( pVisor->ProfileMode == true && (nRT == RT_SYSEQ || nRT == RT_PROCES
    || nRT == RT_UNSPACE  || nRT > RT_GTDEMO ) )
  {
     tbKeys->hideColumn(0);
     tbKeys->hideColumn(1);
  }
  else
  {
     tbKeys->showColumn(0);
     tbKeys->showColumn(1);
  }

  rt[nRT].SetKey(oldKey.c_str());
}



void MainWindow::openPropertyList()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Open Property List"),
                               "", tr("Property List Files (*.plist)"));
    if (!fileName.isEmpty()) {
        QSettings *settings = new QSettings(fileName, QSettings::NativeFormat);
        setSettingsObject(settings);
        fallbacksAct->setEnabled(false);
    }
}
*/
