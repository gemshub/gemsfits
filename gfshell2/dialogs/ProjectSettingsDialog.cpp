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
        QString fname = ui->projDir->text() + "/" + ui->projName->text() + ".ini";
        settings = new QSettings(fname, QSettings::IniFormat);
    }
    settings->setValue("ProjectDir", ui->projDir->text() );
    settings->setValue("ProjectName", ui->projName->text() );
    settings->setValue("EJDBDir", ui->ejdbDir->text() );
    settings->setValue("EJDBName", ui->ejdbName->text() );
    settings->setValue("DataCollection", ui->experCollect->text() );
    settings->setValue("TaskCollection", ui->taskCollection->text() );
    settings->setValue("GEMSDir", ui->gemsDir->text() );
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

