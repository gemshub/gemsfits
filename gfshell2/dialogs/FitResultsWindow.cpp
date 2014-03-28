#include <QFileDialog>
#include <QCloseEvent>
#include "FitResultsWindow.h"
#include "ui_FitResultsWindow.h"


FitResultsWindow* FitResultsWindow::pDia = 0;

FitResultsWindow::FitResultsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FitResultsWindow)
{
    ui->setupUi(this);
    pDia = this;


    // set up 1 tab
    modelFitResults = new TMatrixModel( "fit-results", 2, this );
    tableFitResults = new TMatrixTable(ui->tabFitResults);
    //TMatrixDelegate *deleg = new TMatrixDelegate(  this);
    //tableFitResults->setItemDelegate(deleg);
    tableFitResults->setModel(modelFitResults);
    ui->horizontalLayout->addWidget(tableFitResults);

    setActions();

}


FitResultsWindow::~FitResultsWindow()
{
    delete ui;
}

void FitResultsWindow::closeEvent(QCloseEvent* ev)
{
       pDia = 0;
       ev->accept();
}

//  Connect all actions
void FitResultsWindow::setActions()
{
    connect( ui->actionOpen_File, SIGNAL( triggered()), this, SLOT(CmOpenFile()));
    connect( ui->actionSave_File, SIGNAL( triggered()), this, SLOT(CmSaveFile()));
    connect( ui->actionClose, SIGNAL( triggered()), this, SLOT(close()));
}

void FitResultsWindow::CmOpenFile()
{
  // select directory
    QString dir = QFileDialog::getExistingDirectory(this, "Select work/output  Directory (Only test!!!)",
     "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
  modelFitResults->matrixFromCsvFile( dir );
}

void FitResultsWindow::CmSaveFile()
{
    // select directory
      QString dir = QFileDialog::getExistingDirectory(this, "Select work/output  Directory (Only test!!!)",
       "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    modelFitResults->matrixToCsvFile( dir );
}
