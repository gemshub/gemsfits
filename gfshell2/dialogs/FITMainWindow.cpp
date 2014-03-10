
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

FITMainWindow::FITMainWindow(int c, char** v, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::FITMainWindow),
    currentMode (MDF_DATABASE), LocalDocDir("HELP/")
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

