#include "FITMainWindow.h"
#include "ui_FITMainWindow.h"
#include "ProjectSettingsDialog.h"
#include "DBKeyDialog.h"
#include "fservice.h"
#include "f_ejdb.h"

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
    connect( ui->action_Update, SIGNAL( triggered()), this, SLOT(CmUpdate()));
    connect( ui->action_Insert, SIGNAL( triggered()), this, SLOT(CmInsert()));
    connect( ui->actionDelete, SIGNAL( triggered()), this, SLOT(CmDelete()));
    connect( ui->actionNext, SIGNAL( triggered()), this, SLOT(CmNext()));
    connect( ui->actionPrevious, SIGNAL( triggered()), this, SLOT(CmPrevious()));
    connect( ui->action_Find, SIGNAL( triggered()), this, SLOT(CmFilter()));
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

/// Select experiments DB mode
void FITMainWindow::CmDBMode()
{
    if( !MessageToSave() )
         return;

   if( currentMode == MDF_DATABASE )
   {
       ui->action_DataBase_mode->setChecked(true);
       return;
   }
   ui->action_DataBase_mode->setChecked(true);
   ui->action_Task_Mode->setChecked(false);

   currentMode = MDF_DATABASE;

   // define actions
   ui->actionBackup_to_csv->setEnabled(true);
   ui->actionRestore_from_csv->setEnabled(true);
   ui->actionBackup_to_TXT->setEnabled(false);
   ui->actionRestore_from_TXT->setEnabled(false);
   ui->action_Run_test->setEnabled(false);
   ui->menu_Calc->setEnabled(false);

   // update key list, editor, filter
   resetMainWindow();

}

/// Select experiments DB mode
void FITMainWindow::CmTaskMode()
{
   if( !MessageToSave() )
         return;

   if( currentMode == MDF_TASK )
   {
       ui->action_Task_Mode->setChecked(true);
       return;
   }
   ui->action_DataBase_mode->setChecked(false);
   ui->action_Task_Mode->setChecked(true);

   currentMode = MDF_TASK;

   // define actions
   ui->actionBackup_to_csv->setEnabled(false);
   ui->actionRestore_from_csv->setEnabled(false);
   ui->actionBackup_to_TXT->setEnabled(true);
   ui->actionRestore_from_TXT->setEnabled(true);
   ui->action_Run_test->setEnabled(true);
   ui->menu_Calc->setEnabled(true);

   // update key list, editor, filter
   resetMainWindow();

}


/// Select new GEMS3K files list and setup windows
void FITMainWindow::CmSelectGEMS( const string& fname_ )
{
  try
    {
       string fname = fname_;
       if( fname.empty() )
       {
         if( projectSettings)
         {
             fname  = projectSettings->value("ProjFolderPath", ".").toString().toUtf8().data();
             fname  += projectSettings->value("GEMS3KFilesPath", "/GEMS").toString().toUtf8().data();
         }
         //Select files
         if( !gemsLstFile.ChooseFileOpen( this, fname, "Please, select a GEMS3K lst file","*.lst" ))
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

       setStatusText( "GEMS3K input file set is selected" );
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
     if( !fitTaskDir.ChooseFileOpen( this, fname, "Please, select a GEMSFITS project file","*.pro *.conf" ))
        return;
   }

   //load project settings
   if (fname.empty())
     return;
   if( projectSettings )
       delete projectSettings;
   projectSettings = new QSettings(fname.c_str(), QSettings::IniFormat);
   if( !projectSettings->contains("GEMSFITSAPP") )
       Error( fname, "This is not a GEMSFITS project file");
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
    if( !fitTaskDir.ChooseFileOpen( this, fname, "Please, select a GEMSFITS project file","*.pro *.conf" ))
        return;

    //load project settings
   if (fname.empty())
     return;
   if( projectSettings )
       delete projectSettings;
   projectSettings = new QSettings(fname.c_str(), QSettings::IniFormat);
   if( !projectSettings->contains("GEMSFITSAPP") )
       Error( fname, "This is not a GEMSFITS project file");
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
// Record menu

void FITMainWindow::openRecordKey( int row, int    )
{
    string currentKey ="";

    if( row >= keyTable->rowCount())
        return;

    for(int jj=0; jj<keyTable->columnCount(); jj++)
    {
        currentKey += keyTable->item( row, jj)->text().toUtf8().data();
        strip(currentKey);
        currentKey +=":";
     }

    CmShow( currentKey );
}

/// Show another record from DB (Without check)
void FITMainWindow::CmShow( const string& reckey )
{
 try
   {
      if( !MessageToSave() )
           return;

      // get key of record
      string str;
      if( reckey.empty() )
      { //str = GetKeyofRecord( 0, "Select a document (data record) key ", KEY_OLD );
        //      if( str.empty() )
                return;
       }
       else  str = string(reckey);
      rtEJ[ currentMode ].Get( str.c_str() );
      string valDB =rtEJ[ currentMode ].GetJson();
      ui->recordEdit->setText( trUtf8(valDB.c_str()));
      contentsChanged = false;

      // SetTitle();
      // pVisor->Update( true );
      // defineModuleKeysList( actwin->rtNumRecord() );
   }
   catch( TError& err )
        {
           setStatusText( err.title );
           addLinetoStatus( err.mess );
        }
}

void FITMainWindow::CmNext()
{

  int row = keyTable->currentRow();
  if( row < keyTable->rowCount()-1 )
   {  row++;
        QTableWidgetItem *curItem = keyTable->item(row,0);
        keyTable->setCurrentItem( curItem );
        keyTable->scrollToItem( curItem );
        openRecordKey( row, 0  );
   }
}

void FITMainWindow::CmPrevious()
{
       int row = keyTable->currentRow();
       if( row > 0  )
       {  row--;
          QTableWidgetItem *curItem = keyTable->item(row,0);
          keyTable->setCurrentItem( curItem );
          keyTable->scrollToItem( curItem );
          openRecordKey( row, 0  );
       }
}

void FITMainWindow::CmUpdateTest()
{
  try
    {
      string recBson = ui->recordEdit->toPlainText().toUtf8().data();
      rtEJ[ currentMode ].TestBson( recBson );
      setStatusText( "Text in the editor is in valid JSON format" );
    }
    catch( TError& err )
         {
           setStatusText( err.title );
           addLinetoStatus( err.mess );
         }
}

/// Save current record to DB file as new
void FITMainWindow::CmUpdate()
{
    try
    {
       string recBson = ui->recordEdit->toPlainText().toUtf8().data();
       RecSave( recBson, rtEJ[ currentMode ].PackKey() );
       setStatusText( "Record saved" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}


/// Save current record to DB file as new
void FITMainWindow::CmInsert()
{
    try
    {
        string recBsonText = ui->recordEdit->toPlainText().toUtf8().data();
        rtEJ[ currentMode ].SetJson( recBsonText );
        rtEJ[ currentMode ].InsertRecord();
        changeKeyList(); // need change key list insert new record
        contentsChanged = false;
        setStatusText( "Record inserted" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Delete current record
void FITMainWindow::CmDelete()
{
    try
    {
        string strKey = rtEJ[ currentMode ].PackKey();
        if( !vfQuestion(window(), rtEJ[ currentMode ].GetKeywd(),
               "Confirm deletion of data record keyed "+ strKey ))
            return;
        rtEJ[ currentMode ].Del( strKey.c_str() );
        changeKeyList();
        contentsChanged = false;
        setStatusText( "Record deleted" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}


string templRec = "  template must be     ";


/// Create new record
void FITMainWindow::CmCreate()
{
    try
    {
        ui->recordEdit->setText( trUtf8(templRec.c_str()));
        contentsChanged = true;
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Create new record
void FITMainWindow::CmFilter()
{
    try
    {
        string filterText = ui->queryEdit->toPlainText().toUtf8().data();
        // must be in future
        //rtEJ[ currentMode ].searchFilter();
        contentsChanged = false;
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

//-------------------------------------------------------------------------------------
// Record list

/// Export Data Records to json txt-file
void FITMainWindow::CmBackupJSON()
{
    try
    {
       if( !MessageToSave() )
           return;

       // get current filter
       string keyFilter = ui->filterEdit->text().toUtf8().data();
       if( keyFilter.empty() )
           keyFilter = ALLKEY;

       // select record list to unload
       vector<string> aKey = vfMultiKeys( this, "Please, mark records to be unloaded to JSON",
              currentMode, keyFilter.c_str() );
        if( aKey.size() <1 )
               return;

       // open file to unloading
        string fname;
        TFile  outFile("", ios::out );
        if( !outFile.ChooseFileSave( this, fname, "Please, give a file name for unloading records","*.json" ))
             return;
        outFile.Open();

        outFile.ff << "[\n";
        for(int i=0; i<aKey.size(); i++ )
        {
          rtEJ[ currentMode ].Get( aKey[i].c_str() );
          string valDB =rtEJ[ currentMode ].GetJson();
          outFile.ff << valDB;
          if( i<aKey.size()-1)
               outFile.ff <<  ",";
          outFile.ff <<  "\n";
        }
        outFile.ff << "]";
        outFile.Close();

        changeKeyList();
        contentsChanged = false;
        setStatusText( "Records exported to json txt-file" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Import Data Records from json txt-file
void FITMainWindow::CmRestoreJSON()
{
    try
    {
        if( !MessageToSave() )
          return;

        // open file to unloading
         string fname;
         TFile  inFile("", ios::in);
         if( !inFile.ChooseFileOpen( this, fname, "Please, select file with unloaded records","*.json" ))
              return;
         inFile.Open();


        // read bson records array from file
        ParserJson parserJson;
        char b;
        string objStr;

        while( !inFile.ff.eof() )
        {
          inFile.ff.get(b);
          if( b == jsBeginObject )
          {
            objStr =  parserJson.readObjectText(inFile.ff);
            objStr = "{" + objStr;
            rtEJ[ currentMode ].SetJson( objStr );
            if( ui->actionOverwrite->isChecked() )
               rtEJ[ currentMode ].SaveRecord(0);
            else
               rtEJ[ currentMode ].InsertRecord();
         }
        }

        changeKeyList();
        contentsChanged = false;
        setStatusText( "Records imported from json txt-file" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

void get_bson_from_gems_fit_txt( const string& fname, bson *obj );
/// Import Data Records from task configuration txt-file
void FITMainWindow::CmRestoreTXT()
{
    try
    {
        if( !MessageToSave() )
          return;

        // open file to unloading
         string fname;
         TFile  inFile("", ios::in);
         if( !inFile.ChooseFileOpen( this, fname, "Please, select file with GEMSFIT2 specificatins file","*.dat" ))
              return;
         inFile.Open();

        // read bson records array from file
         bson bsrec;
         bson_init( &bsrec );
         bson_append_string( &bsrec, "taskid", inFile.Name().c_str() );
         bson_append_string( &bsrec, "projectid", fitTaskDir.Name().c_str() );
         get_bson_from_gems_fit_txt( fname.c_str(), &bsrec );
         bson_finish( &bsrec );

         //set bson to string
         ParserJson pars;
         string bsonVal;
         pars.printBsonObjectToJson( bsonVal, &bsrec );

         //show result
         ui->recordEdit->setText( trUtf8(bsonVal.c_str()));

        changeKeyList();
        contentsChanged = false;
        setStatusText( "Records imported from json txt-file" );
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





