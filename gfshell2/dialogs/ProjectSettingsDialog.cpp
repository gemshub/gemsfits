#include <QFileDialog>
#include "ProjectSettingsDialog.h"
#include "ui_ProjectSettingsDialog.h"

#include "FITMainWindow.h"
#include "fservice.h"
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
    settings->setValue("GEMS3KFilesPath",   ui->gemsDir->text() );
    settings->setValue("GEMSFITSAPP",       _FIT_version_stamp );
    settings->sync();
    accept();
}

void ProjectSettingsDialog::CmProjectDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Project Directory",
     "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->projDir->setText( dir );
}

void ProjectSettingsDialog::CmEJDBDir()
{
    QString projDir = ui->projDir->text();
    QString dir = QFileDialog::getExistingDirectory(this, "Select EJDB Directory",
     projDir,  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    dir = dir.remove(projDir);
    ui->ejdbDir->setText( dir );
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
    pFitImp->OpenHelp( GEMS_TDBAS_HTML );
}
