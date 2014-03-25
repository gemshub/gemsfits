#include <QFileDialog>
#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"
#include "FITMainWindow.h"
#include "fservice.h"

PreferencesDialog::PreferencesDialog(QSettings *aSet,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog), settings(aSet)
{
    ui->setupUi(this);
    if( settings ) //load old settings
    {
       // ui->projDir->setText( settings->value("ProjFolderPath", ".").toString() );
       // ui->projName->setText( settings->value("ProjFileName", "myproj1").toString() );

    }

    QObject::connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(CmSave()));
    QObject::connect( ui->buttonBox, SIGNAL(helpRequested()), this, SLOT(CmHelp()));
    QObject::connect( ui->helpButton, SIGNAL(clicked()), this, SLOT(CmHelpFile()));
    QObject::connect( ui->usersButton, SIGNAL(clicked()), this, SLOT(CmProjectDir()));
    QObject::connect( ui->resourcesButton, SIGNAL(clicked()), this, SLOT(CmResourceDir()));
    QObject::connect( ui->pushButton, SIGNAL(clicked()), this, SLOT(CmGenerateHelp()));
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::CmSave()
{
    if( !settings )
       return;

    //settings->setValue("ProjFolderPath",    ui->projDir->text() );
    //settings->setValue("ProjFileName",      ui->projName->text() );
    //settings->setValue("ProjDatabasePath",  ui->ejdbDir->text() );
    //settings->setValue("ProjDatabaseName",  ui->ejdbName->text() );
    //settings->setValue("ExpSamplesDataColl", ui->experCollect->text() );
    settings->sync();
    accept();
}

void PreferencesDialog::CmProjectDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Project Directory",
     "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->usersEdit->setText( dir );
}


void PreferencesDialog::CmHelp()
{
    pFitImp->OpenHelp( GEMS_TDBAS_HTML );
}
