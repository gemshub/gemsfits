#include "FITMainWindow.h"
#include "ui_FITMainWindow.h"
#include "HelpWindow.h"


FITMainWindow* pFitImp;

FITMainWindow::FITMainWindow(int c, char** v, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::FITMainWindow)
{
    ui->setupUi(this);

    toolTasks = new QToolBar(this);
    toolTasks->setObjectName(QString::fromUtf8("toolTasks"));
    //toolTasks->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolTasks->setMovable(false);
    addToolBar(Qt::LeftToolBarArea, toolTasks );
    toolTasks->setIconSize(QSize(40,40));
    toolTasks->addAction(ui->action_DataBase_mode);
    toolTasks->addAction(ui->action_Task_Mode);
    toolTasks->setWindowTitle("toolTasks");

    setActions();
}

FITMainWindow::~FITMainWindow()
{
    delete ui;
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

