#include <QFileDialog>
#include "ProjectSettingsDialog.h"
#include "ui_ProjectSettingsDialog.h"

ProjectSettingsDialog::ProjectSettingsDialog( QSettings *aSet, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProjectSettingsDialog), settings(aSet)
{
    ui->setupUi(this);
    if( settings ) //load old settings
    {
        ui->projDir->setText( settings->value("ProjectDir", ".").toString() );
        ui->projName->setText( settings->value("ProjectName", "project1").toString() );
        ui->ejdbDir->setText( settings->value("EJDBDir", "./EJDB").toString() );
        ui->ejdbName->setText( settings->value("EJDBName", "database").toString() );
        ui->experCollect->setText( settings->value("DataCollection", "experiments").toString() );
        ui->taskCollection->setText( settings->value("TaskCollection", "tests").toString() );
        ui->gemsDir->setText( settings->value("GEMSDir", "./GEMS").toString() );
        ui->projDir->setEnabled(false);
        ui->projName->setEnabled(false);
        ui->projDirButton->setEnabled(false);
    }

    QObject::connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(CmSave()));
    //QObject::connect( ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
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
    settings->setValue("GEMSFITSAPP",       "v0.1" );  // _FIT_version_stamp
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
    QString dir = QFileDialog::getExistingDirectory(this, "Select EJDB Directory",
     "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->ejdbDir->setText( dir );
}

void ProjectSettingsDialog::CmGEMSDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select GEMS Directory",
     "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->gemsDir->setText( dir );
}

