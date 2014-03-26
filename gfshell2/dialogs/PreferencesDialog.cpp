#include <QFileDialog>
#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"
#include "FITMainWindow.h"
#include "fservice.h"
#include "help.h"

PreferencesDialog::PreferencesDialog(QSettings *aSet,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog), settings(aSet)
{
    ui->setupUi(this);
    if( settings ) //load old settings
    {
        ui->resourcesEdit->setText( settings->value("ResourcesFolderPath", "../Resources/").toString() );
        ui->helpEdit->setText( settings->value("HelpFolderPath", "../Resources/help").toString() );
        ui->usersEdit->setText( settings->value("UserFolderPath", ".").toString() );
        ui->gemsfit2Edit->setText( settings->value("Gemsfit2ProgramPath", "gemsfit2").toString() );
        ui->commentsBox->setChecked( settings->value("PrintComments", false).toBool() );

        // load all template files
        QDir thisDir(ui->resourcesEdit->text()+"/data");
        if (thisDir.exists())
        {
            // thisDir.setFilter(QDir::Files|QDir::NoDot | QDir::NoDotDot);
            QStringList filters;
            filters << "*.dat";
            thisDir.setNameFilters(filters);
            QListIterator<QFileInfo> it(thisDir.entryInfoList());
            QFileInfo f;
            while ( it.hasNext() )
            {
                f = it.next();;
                ui->experimentsBox->addItem(f.fileName());
            }
         ui->experimentsBox->setCurrentText(settings->value("ExpTemplateFileName", "...").toString());
        }
    }

    QObject::connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(CmSave()));
    QObject::connect( ui->buttonBox, SIGNAL(helpRequested()), this, SLOT(CmHelp()));
    QObject::connect( ui->helpButton, SIGNAL(clicked()), this, SLOT(CmHelpFile()));
    QObject::connect( ui->gemsfit2Button, SIGNAL(clicked()), this, SLOT(CmGemsFit2File()));
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

    settings->setValue("ResourcesFolderPath",    ui->resourcesEdit->text() );
    settings->setValue("HelpFolderPath",    ui->helpEdit->text() );
    settings->setValue("UserFolderPath",    ui->usersEdit->text() );
    settings->setValue("Gemsfit2ProgramPath",    ui->gemsfit2Edit->text() );

    settings->setValue("ExpTemplateFileName", ui->experimentsBox->currentData(  Qt::DisplayRole ) );
    settings->setValue("PrintComments",  ui->commentsBox->isChecked() );

    settings->sync();
    accept();
}

void PreferencesDialog::CmProjectDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Project Directory",
     "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->usersEdit->setText( dir );
}

void PreferencesDialog::CmResourceDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Resource Directory",
     "",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->resourcesEdit->setText( dir );
}

void PreferencesDialog::CmHelpFile()
{
    QString projDir = ui->resourcesEdit->text();
    QString dir = QFileDialog::getExistingDirectory(this, "Select Help Directory",
     projDir,  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    ui->helpEdit->setText( dir );
}

void PreferencesDialog::CmGemsFit2File()
{
    QString dir = QFileDialog::getOpenFileName(  this,
       "Select gemsfite program", "", "All files (*)", 0, QFileDialog::DontConfirmOverwrite );
    ui->gemsfit2Edit->setText( dir );
}

void PreferencesDialog::CmGenerateHelp()
{
  try
    {
        QString phpdir =  ui->resourcesEdit->text()+"/doc/html/";
        QString qhpFile = phpdir +"gfshelpconfig.qhp";
        HelpConfigurator rr;
        if( rr.readDir(phpdir.toUtf8().data()) )
           rr.writeFile(qhpFile.toUtf8().data());

        if (!pFitImp->helpProcess)
             pFitImp->helpProcess = new QProcess();

       if (pFitImp->helpProcess->state() != QProcess::Running)
       {
        QString docPath =  phpdir;
        QString app;
#ifdef __unix
#ifdef __APPLE__
                    app += QLatin1String("qcollectiongenerator");
#else
                   //app = QLatin1String(getenv("HOME"));
                   //app += QLatin1String("/Gems3-app/qcollectiongenerator");
        app += QLatin1String("qcollectiongenerator");
#endif
#else    // windows
                    app += QLatin1String("qcollectiongenerator.exe");
#endif
       QStringList args;
       args << docPath + QLatin1String("gfshelpconfig.qhcp")
            << QLatin1String("-o")
            << docPath + QLatin1String("gfshelp.qhc");
                        ;
       pFitImp->helpProcess->start(app, args);
       cout << app.toUtf8().data() << endl;
       cout << args[2].toUtf8().data() << endl;

       if (!pFitImp->helpProcess->waitForStarted())
       {
           Error( "gemsfits", "Unable to launch qcollectiongenerator");
       }
     }
   }
   catch(TError& e)
   {
       cout <<  e.title <<  e.mess << endl;
   }
}


void PreferencesDialog::CmHelp()
{
    pFitImp->OpenHelp( GEMS_TDBAS_HTML );
}
