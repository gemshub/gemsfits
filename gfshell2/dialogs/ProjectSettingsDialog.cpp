#include <QFileDialog>
#include "ProjectSettingsDialog.h"
#include "ui_ProjectSettingsDialog.h"

#include "FITMainWindow.h"
#include "fservice.h"
#include "f_ejdb_file.h"
extern const char *_FIT_version_stamp;

ProjectSettingsDialog::ProjectSettingsDialog( QSettings *aSet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProjectSettingsDialog), settings(aSet)
{
    ui->setupUi(this);
    if( settings ) //load old settings
    {
        ui->projDir->setText( settings->value("ProjFolderPath", ".").toString() );
        ui->projName->setText( settings->value("ProjFileName", "myproj1").toString() );
        ui->ejdbDir->setText( settings->value("ProjDatabasePath", "/EJDB").toString() );
        ui->ejdbName->setText( settings->value("ProjDatabaseName", "myprojdb1").toString() );
        ui->experCollect->setText( settings->value("ExpSamplesDataColl", "experiments").toString() );
        ui->taskCollection->setText( settings->value("TaskCasesDataColl", "tests").toString() );
        ui->fitCollection->setText( settings->value("FitsCasesDataColl", "fits").toString() );
        ui->gemsDir->setText( settings->value("GEMS3KFilesPath", "/GEMS").toString() );
        ui->projDir->setEnabled(false);
        ui->projName->setEnabled(false);
        ui->projDirButton->setEnabled(false);
    }

    QObject::connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(CmSave()));
    QObject::connect( ui->buttonBox, SIGNAL(helpRequested()), this, SLOT(CmHelp()));
    QObject::connect( ui->projDirButton, SIGNAL(clicked()), this, SLOT(CmProjectDir()));
    QObject::connect( ui->ejdbDirButton, SIGNAL(clicked()), this, SLOT(CmEJDBDir()));
    QObject::connect( ui->gemsDirButton, SIGNAL(clicked()), this, SLOT(CmGEMSDir()));

}

ProjectSettingsDialog::~ProjectSettingsDialog()
{
    delete ui;
}

void ProjectSettingsDialog::CmSave()
{
    if( !settings )
    {
        QString fname = ui->projDir->text() + "/" + ui->projName->text() + ".pro";
        settings = new QSettings(fname, QSettings::IniFormat);
    }
    settings->setValue("ProjFolderPath",    ui->projDir->text() );
    settings->setValue("ProjFileName",      ui->projName->text() );
    settings->setValue("ProjDatabasePath",  ui->ejdbDir->text() );
    settings->setValue("ProjDatabaseName",  ui->ejdbName->text() );
    settings->setValue("ExpSamplesDataColl", ui->experCollect->text() );
    settings->setValue("TaskCasesDataColl", ui->taskCollection->text() );
    settings->setValue("FitsCasesDataColl", ui->fitCollection->text() );
    settings->setValue("GEMS3KFilesPath",   ui->gemsDir->text() );
    settings->setValue("GEMSFITSAPP",       _FIT_version_stamp );
    settings->sync();

    // create directories, if not exists
    QDir dir(ui->projDir->text());
    if( dir.mkpath(ui->projDir->text()) )
    {
       dir.mkpath(ui->projDir->text()+ui->gemsDir->text());
       dir.mkpath(ui->projDir->text()+ui->ejdbDir->text());
       dir.mkpath(ui->projDir->text()+"/work");
       dir.mkpath(ui->projDir->text()+"/dbimport");
    }
    accept();
}

void ProjectSettingsDialog::CmProjectDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Project Directory",
    pFitImp->userDir().c_str(),  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->projDir->setText( dir );
}

void ProjectSettingsDialog::CmEJDBDir()
{
    QString projDir = ui->projDir->text();
//    QString dir = QFileDialog::getExistingDirectory(this, "Select EJDB Directory",
//     projDir,  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    string fname = ui->projDir->text().toUtf8().data();
    //Select files
    TFile file("");
    if( !file.ChooseFileSave( this, fname, "Select EJDB Directory", "" ))
        return;
    QString dir( trUtf8( file.Dir().c_str() ));
    dir = dir.remove(projDir);
    ui->ejdbDir->setText( dir );
    ui->ejdbName->setText( trUtf8( file.Name().c_str() ) );
}

void ProjectSettingsDialog::CmGEMSDir()
{
    QString projDir = ui->projDir->text();
    QString dir = QFileDialog::getExistingDirectory(this, "Select GEMS Directory",
     projDir,  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    
    
    dir = dir.remove(projDir);
    ui->gemsDir->setText( dir );
}

void ProjectSettingsDialog::CmHelp()
{
    pFitImp->OpenHelp( GF_INDEX_HTML );
}
