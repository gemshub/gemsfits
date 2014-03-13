#include "FITMainWindow.h"
#include "ui_FITMainWindow.h"
#include "ProjectSettingsDialog.h"
#include "fservice.h"
#include "f_ejdb.h"

//----------------------------------------------------------
// Working with EJDB


void FITMainWindow::closeEJDB()
{
  for( int ii; ii<rtEJ.size(); ii++ )
     rtEJ[ii].Close();
  EJDBFile.Close();
}

/// Connect project database
void FITMainWindow::openEJDB()
{
    // close old project data
    closeEJDB();

    // set up new ejdb file
    string ejdbPath  = projectSettings->value("ProjFolderPath", ".").toString().toUtf8().data();
    ejdbPath  += projectSettings->value("ProjDatabasePath", "/EJDB").toString().toUtf8().data();
    ejdbPath  += "/";
    ejdbPath  += projectSettings->value("ProjDatabaseName", "myprojdb1" ).toString().toUtf8().data();
    EJDBFile.ChangePath(ejdbPath);

    // change collections names
    string samlescolName = projectSettings->value("ExpSamplesDataColl", "experiments").toString().toUtf8().data();
    rtEJ[MDF_DATABASE].SetKeywd(samlescolName);
    rtEJ[MDF_DATABASE].Open();
    string testcolName = projectSettings->value("TaskCasesDataColl", "tests").toString().toUtf8().data();
    rtEJ[MDF_TASK].SetKeywd(testcolName);
    rtEJ[MDF_TASK].Open();
}

/// Set up data for current project
void FITMainWindow::loadNewProject()
{
    // Connect project database
    openEJDB();

    pLineTask->setText( projectSettings->value("ProjFileName", "undefined").toString());

    // update key list
    defineModuleKeysList( MDF_DATABASE );
    // load first record

}

// -----------------------------------------------------------
// Actions and commands

//  Connect all actions
void FITMainWindow::setActions()
{

 // Tasks
    connect( ui->action_DataBase_mode, SIGNAL( triggered()), this, SLOT(CmDBMode()));
    connect( ui->action_Task_Mode, SIGNAL( triggered()), this, SLOT(CmTaskMode()));
    connect( ui->action_New_Project, SIGNAL( triggered()), this, SLOT(CmNewProject()));
    connect( ui->action_Config_Project, SIGNAL( triggered()), this, SLOT(CmConfigProject()));
    connect( ui->action_Select_project, SIGNAL( triggered()), this, SLOT(CmSelectProject()));
    connect( ui->actionSelect_GEMS, SIGNAL( triggered()), this, SLOT(CmSelectGEMS()));
    connect( ui->action_Exit, SIGNAL( triggered()), this, SLOT(close()));

 // Help
    connect( ui->actionHelp, SIGNAL( triggered()), this, SLOT(CmHelp()));
    //connect( ui->actionHow_to, SIGNAL( triggered()), this, SLOT(CmHowto()));
    connect( ui->actionAbout, SIGNAL( triggered()), this, SLOT(CmHelpAbout()));
    connect( ui->actionAuthors, SIGNAL( triggered()), this, SLOT(CmHelpAuthors()));
    //connect( ui->actionThanks, SIGNAL( triggered()), this, SLOT(CmHelpThanks()));
    connect( ui->actionLicense, SIGNAL( triggered()), this, SLOT(CmHelpLicense()));
    //connect( action_Help_2, SIGNAL( triggered()), this, SLOT(CmHelp2()));
    connect( ui->action_Preferences, SIGNAL( triggered()), this, SLOT(CmSettingth()));
    //connect( ui->actionTutorial, SIGNAL( triggered()), this, SLOT(CmTutorial()));

// Record
    connect( ui->actionCreate_New , SIGNAL( triggered()), this, SLOT(CmCreate()));
    connect( ui->actionNew_Clone, SIGNAL( triggered()), this, SLOT(CmClone()));
    connect( ui->actionSave, SIGNAL( triggered()), this, SLOT(CmSave()));
    connect( ui->actionSave_As, SIGNAL( triggered()), this, SLOT(CmSaveAs()));
    connect( ui->actionDelete, SIGNAL( triggered()), this, SLOT(CmDelete()));
    connect( ui->actionNext, SIGNAL( triggered()), this, SLOT(CmNext()));
    connect( ui->actionPrevious, SIGNAL( triggered()), this, SLOT(CmPrevious()));
    //connect( ui->actionUpdate_Test, SIGNAL( triggered()), this, SLOT(CmFilter()));
    connect( ui->actionUpdate_Test, SIGNAL( triggered()), this, SLOT(CmUpdateTest()));


 // Record list
       connect( ui->actionBackup_to_JSON, SIGNAL( triggered()), this, SLOT(CmBackupJSON()));
       connect( ui->actionRestore_from_JSON, SIGNAL( triggered()), this, SLOT(CmRestoreJSON()));
       connect( ui->actionBackup_to_csv, SIGNAL( triggered()), this, SLOT(CmBackupCSV()));
       connect( ui->actionRestore_from_csv, SIGNAL( triggered()), this, SLOT(CmRestoreCSV()));
       connect( ui->actionBackup_to_TXT, SIGNAL( triggered()), this, SLOT(CmBackupTXT()));
       connect( ui->actionRestore_from_TXT, SIGNAL( triggered()), this, SLOT(CmRestoreTXT()));
       connect( ui->actionBackup_to_YAML, SIGNAL( triggered()), this, SLOT(CmBackupYAML()));
       connect( ui->actionRestore_from_YAML, SIGNAL( triggered()), this, SLOT(CmRestoreYAML()));

  //Calc
    connect( ui->action_Run_test, SIGNAL( triggered()), this, SLOT(CmRunTest()));
    connect( ui->action_Show_Results, SIGNAL( triggered()), this, SLOT(CmShowResults()));


   pLineTask = new QLineEdit( ui->toolBarTask );
   pLineTask->setEnabled( true );
   pLineTask->setFocusPolicy( Qt::ClickFocus );
   pLineTask->setReadOnly( true );
   pLineTask->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   ui->toolBarTask->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   ui->toolBarTask->addWidget( pLineTask ); // setStretchableWidget( pLine );

   pLineGEMS = new QLineEdit( ui->toolBarGems );
   pLineGEMS->setEnabled( true );
   pLineGEMS->setFocusPolicy( Qt::ClickFocus );
   pLineGEMS->setReadOnly( true );
   pLineGEMS->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   ui->toolBarGems->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   ui->toolBarGems->addWidget( pLineGEMS ); // setStretchableWidget( pLine );

}

//-------------------------------------------------------------------------------------
// Project menu

/// Select new GEMS3K files list and setup windows
void FITMainWindow::CmSelectGEMS( const string& fname_ )
{
  try
    {
       string fname = fname_;
       if( fname.empty() )
       { fname = gemsLstFile.GetPath();
         //Select files
         if( !gemsLstFile.ChooseFileOpen( this, fname, "Please, select GEMS3K lst file","*.lst" ))
           return;
       }

       // Creates TNode structure instance accessible trough the "node" pointer
       aNode.reset( new TNode() );

       // (1) Initialization of GEMS3K internal data by reading  files
       if( node()->GEM_init( fname.c_str() ) )
       {
           Error( gemsLstFile.Name(), "GEMS3K Init() error: \n"
                   "Some GEMS3K input files are corrupt or cannot be found.");
       }
       // setup icomp table
       setTableIComp();

       // setup phase list
       setListPhase();

       pLineGEMS->setText( trUtf8( gemsLstFile.Name().c_str() ) );

       setStatusText( "Selected new GEMS3K files" );
    }
    catch( TError& err )
    {
       setStatusText( err.title );
       addLinetoStatus( err.mess );
    }
}

void FITMainWindow::CmSelectProject( const string& fname_ )
{
   string fname=fname_;
 try
 {
   if( fname.empty())
   { //Select fit project files
      fname = fitTaskDir.GetPath();
     if( !fitTaskDir.ChooseFileOpen( this, fname, "Please, select a project file","*.pro *.conf" ))
        return;
   }

   //load project settings
   if (fname.empty())
     return;
   if( projectSettings )
       delete projectSettings;
   projectSettings = new QSettings(fname.c_str(), QSettings::IniFormat);
   if( !projectSettings->contains("GEMSFITSAPP") )
       Error( fname, "This is not a project file");
   projectSettings->setValue("ProjFolderPath", fitTaskDir.Dir().c_str() );
   projectSettings->setValue("ProjFileName", fitTaskDir.Name().c_str() );

   //connect to EJDB for project&update key list
   loadNewProject();
  }
   catch( TError& err )
   {
      setStatusText( err.title );
      addLinetoStatus( err.mess );
   }
}

void FITMainWindow::CmConfigProject()
{
  try
  {    //Select fit project files
    string fname = fitTaskDir.GetPath();
    if( !fitTaskDir.ChooseFileOpen( this, fname, "Please, select a project file","*.pro *.conf" ))
        return;

    //load project settings
   if (fname.empty())
     return;
   if( projectSettings )
       delete projectSettings;
   projectSettings = new QSettings(fname.c_str(), QSettings::IniFormat);
   if( !projectSettings->contains("GEMSFITSAPP") )
       Error( fname, "This is not a project file");
   projectSettings->setValue("ProjFolderPath", fitTaskDir.Dir().c_str() );
   projectSettings->setValue("ProjFileName", fitTaskDir.Name().c_str() );
   //

   // change settings
   ProjectSettingsDialog dlg(projectSettings);
   if( !dlg.exec() )
      return;

   //connect to EJDB for project&update key list
   loadNewProject();
  }
    catch( TError& err )
    {
       setStatusText( err.title );
       addLinetoStatus( err.mess );
    }
}

void FITMainWindow::CmNewProject()
{
  try
  {  // define new project
     ProjectSettingsDialog dlg;
      if( !dlg.exec() )
          return;

     // get new settings
     if( projectSettings )
        delete projectSettings;
      projectSettings = dlg.getNewSettings();

     //connect to EJDB for project&update key list
     loadNewProject();
  }
    catch( TError& err )
    {
       setStatusText( err.title );
       addLinetoStatus( err.mess );
    }
}


//-------------------------------------------------------------------------------------
// Help menu

void FITMainWindow::CmSettingth()
{
   // SettingsDialog dlg(this);
   // dlg.exec();
}


void FITMainWindow::CmHelp()
{
    if( currentMode == MDF_DATABASE )
       OpenHelp( GEMS_TDBAS_HTML );
    else
       OpenHelp( GEMS_MODES_HTML );
}

void FITMainWindow::CmHelpAbout()
{
    OpenHelp( GEMS_ABOUT_HTML );
}

void FITMainWindow::CmHelpAuthors()
{
    OpenHelp( GEMS_AUTHORS_HTML );
}


void FITMainWindow::CmHelpLicense()
{
    OpenHelp( GEMS_LICENSE_HTML );
}





