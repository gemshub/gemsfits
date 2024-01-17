//-------------------------------------------------------------------
// $Id: FITMainWindow.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of GEMSFITS GUI Main Window (menu part)
//
// Copyright (C) 2014  S.V.Dmytriyeva, D.A.Kulik
// Uses EJDB (https://ejdb.org),
//    yaml-cpp (https://code.google.com/p/yaml-cpp/)
//
// This file is part of the GEMSFITS GUI, which uses the
// Qt v.5 cross-platform App & UI framework (http://qt-project.org)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of LGPL v.3 license
//
// See http://gems.web.psi.ch/GEMSFIT for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <iostream>
#include <QFileDialog>
#include "FITMainWindow.h"
#include "ui_FITMainWindow.h"
#include "ProjectSettingsDialog.h"
#include "PreferencesDialog.h"
#include "DBKeyDialog.h"

#include "f_database.h"
#include "v_service.h"
#include "gui_service.h"
#include "keywords.h"
#include "v_yaml.h"
#include "json_view.h"

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
    connect( ui->actionUpdate_Test, SIGNAL( triggered()), this, SLOT(CmUpdateTest()));

    connect( ui->action_Find_query, SIGNAL( triggered()), this, SLOT(CmSearch()));
    connect( ui->actionReset_Search, SIGNAL( triggered()), this, SLOT(CmResetSearch()));
    connect( ui->actionSave_Search, SIGNAL( triggered()), this, SLOT(CmSaveSearch()));
    connect( ui->actionLoad_Search, SIGNAL( triggered()), this, SLOT(CmLoadSearch()));
    connect( ui->actionInsert_search_results_into_task_definition, SIGNAL( triggered()), this, SLOT(CmInsertSearch()));

    // Record list
    connect( ui->actionList, SIGNAL( triggered()), this, SLOT(CmKeysToTXT()));
    connect( ui->actionBackup_to_JSON, SIGNAL( triggered()), this, SLOT(CmBackupJSON()));
    connect( ui->actionRestore_from_JSON, SIGNAL( triggered()), this, SLOT(CmRestoreJSON()));
    connect( ui->actionBackup_to_csv, SIGNAL( triggered()), this, SLOT(CmBackupCSV()));
    connect( ui->actionRestore_from_csv, SIGNAL( triggered()), this, SLOT(CmRestoreCSV()));
    connect( ui->actionBackup_to_TXT, SIGNAL( triggered()), this, SLOT(CmBackupTXT()));
    connect( ui->actionRestore_from_TXT, SIGNAL( triggered()), this, SLOT(CmRestoreTXT()));
    connect( ui->actionBackup_to_YAML, SIGNAL( triggered()), this, SLOT(CmBackupYAML()));
    connect( ui->actionRestore_from_YAML, SIGNAL( triggered()), this, SLOT(CmRestoreYAML()));
    connect( ui->action_Delete_multiple_data, SIGNAL( triggered()), this, SLOT(CmDeleteList()));
    connect( ui->actionExport_TP_pairs_to_CSV_file, SIGNAL( triggered()), this, SLOT(CmTPpairsCSV()));

    //Calc
    connect( ui->action_Run_test, SIGNAL( triggered()), this, SLOT(CmRunTest()));
    connect( ui->action_Show_Results, SIGNAL( triggered()), this, SLOT(CmShowCalcResults()));
    connect( ui->actionFits_View_Mode, SIGNAL( triggered()), this, SLOT(CmShowFitResults()));
    connect( ui->actionCancel_gemsfit2_run, SIGNAL( triggered()), this, SLOT(CmCancelGemsfit()));

    // Edit
    QObject::connect(ui->actionAdd_One_Field, &QAction::triggered, json_tree.get(), &jsonui17::JsonView::CmAddObject);
    QObject::connect(ui->actionClone_Selected_Field, &QAction::triggered, json_tree.get(), &jsonui17::JsonView::CmCloneObject);
    QObject::connect(ui->action_Remove_Selected_Field, &QAction::triggered, json_tree.get(), &jsonui17::JsonView::CmDelObject);
    QObject::connect(ui->actionResize_Selected_List, &QAction::triggered, json_tree.get(), &jsonui17::JsonView::CmResizeArray);
    QObject::connect(ui->actionCalculator, &QAction::triggered, json_tree.get(), &jsonui17::JsonView::CmCalc);
    QObject::connect(ui->actionCopy_Field_Path, &QAction::triggered, json_tree.get(), &jsonui17::JsonView::CopyFieldPath);
    QObject::connect(ui->actionCopy_Field, &QAction::triggered, json_tree.get(), &jsonui17::JsonView::CopyField);
    QObject::connect(ui->actionPaste_Field_Value, &QAction::triggered, json_tree.get(), &jsonui17::JsonView::PasteField);
    // Find
    connect( ui->action_Find, SIGNAL( triggered()), this, SLOT(actionFind()));
    connect( ui->actionFind_Next, SIGNAL( triggered()), this, SLOT(actionFindNext()));
    connect( ui->actionFind_Previous, SIGNAL( triggered()), this, SLOT(actionFindPrevious()));
    connect( ui->actionZoom_In, SIGNAL( triggered()), this, SLOT(actionZoomIn()));
    connect( ui->actionZoom_Out, SIGNAL( triggered()), this, SLOT(actionZoomOut()));

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

    QLabel *label_2 = new QLabel(ui->toolBarMenu);
    label_2->setText("Find:");
    label_2->setToolTip("Find in text");

    ui->toolBarMenu->addWidget( label_2 );

    findLine = new QLineEdit(ui->toolBarMenu);
    findLine->setToolTip("Enter text to find");
    findLine->setEnabled( true );
    findLine->setFocusPolicy( Qt::ClickFocus );
    ui->toolBarMenu->addWidget( findLine );
    ui->toolBarMenu->addAction(ui->actionFind_Previous);
    ui->toolBarMenu->addAction(ui->actionFind_Next);

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
    ui->action_Show_Results->setEnabled(false);
    ui->actionFits_View_Mode->setEnabled(false);
    ui->action_Delete_multiple_data->setEnabled(true);
    ui->actionExport_TP_pairs_to_CSV_file->setEnabled(true);
    //   ui->menu_Calculate->setEnabled(false);

    // empty status
    setStatusText( "" );
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
    ui->actionExport_TP_pairs_to_CSV_file->setEnabled(false);
    ui->actionBackup_to_TXT->setEnabled(true);
    ui->actionRestore_from_TXT->setEnabled(true);
    ui->action_Run_test->setEnabled(true);
    ui->actionFits_View_Mode->setEnabled(true);
    //   ui->action_Show_Results->setEnabled(true);  // temporary
    ui->action_Show_Results->setEnabled(!lastCalcRecordKey.empty());
    //   ui->menu_Calculate->setEnabled(true);

    // empty status
    setStatusText( "" );
    // update key list, editor, filter
    resetMainWindow();

}

/// Select new GEMS3K files list and setup windows
void FITMainWindow::selectGEMS( const std::string& fname_ )
{
    try {
        // Creates TNode structure instance accessible trough the "node" pointer
        aNode.reset( new TNode() );
        std::string fname = fname_;
#ifdef buildWIN32
        std::replace( fname.begin(), fname.end(), '/', '\\');
#endif
        // (1) Initialization of GEMS3K internal data by reading  files
        if( node()->GEM_init( fname.c_str() ) )
        {
            Error( fname, "GEMS3K Init() error: \n"
                          "Some GEMS3K input files are corrupt or cannot be found. \n" );
        }
        // setup icomp table
        setTableIComp();

        // setup phase list
        setListPhase();

        pLineGEMS->setText(gemsLstFile.Name().c_str());
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Select new GEMS3K files list and setup windows
void FITMainWindow::CmSelectGEMS( const std::string& fname_ )
{
    try
    {
        std::string fname = fname_;
        if( fname.empty() )
        {
            fname = gemsLstFile.getPath();
            //Select files
            if( !ChooseFileOpen(&gemsLstFile, this, fname, "Please, select a GEMS3K lst file","*.lst" ))
                return;
        }

        selectGEMS( fname );

        // make new path std::string
        std::string newPath = makeSystemFileName("..");
        std::string edit_json = get_record_edit();
        auto templ_obj = common::JsonFree::parse(edit_json);
        templ_obj[keys::G3Ksys[0]] = newPath;
        templ_obj[keys::G3Ksys[1]] = newPath;
        //templ_obj["SystemFiles"] = newPath;
        set_record_edit(templ_obj.dump());
        contentsChanged = true;
        setStatusText( "GEMS3K input file set is selected" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

void FITMainWindow::CmSelectProject( const std::string& fname_ )
{
    std::string fname=fname_;
    try
    {
        if( fname.empty())
        { //Select fit project files
            fname = userDir();
            if( !ChooseFileOpen(&fitTaskDir, this, fname, "Please, select a GEMSFITS project file","*.pro *.conf" ))
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
        std::string fname = userDir();
        if( !ChooseFileOpen(&fitTaskDir, this, fname, "Please, select a GEMSFITS project file","*.pro *.conf" ))
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

std::string FITMainWindow::getRecordKey( int row )
{
    std::string currentKey ="";

    if( row <0 || row >= keyTable->rowCount())
        return "";

    for(int jj=0; jj<keyTable->columnCount(); jj++)
    {
        currentKey += keyTable->item(row, jj)->text().toStdString();
        strip(currentKey);
        currentKey +=":";
    }

    return currentKey;
}

void FITMainWindow::openRecordKey( int row, int    )
{
    std::string currentKey =getRecordKey( row );
    if( currentKey.empty() )
        return;
    CmShow( currentKey );
}

/// Show another record from DB (Without check)
void FITMainWindow::CmShow( const std::string& reckey )
{
    try
    {
        if( !MessageToSave() )
            return;

        // get key of record
        std::string str;
        if( reckey.empty() ) {
            //str = GetKeyofRecord( 0, "Select a document (data record) key ", KEY_OLD );
            //      if( str.empty() )
            return;
        }
        else {
            str = std::string(reckey);
        }
        rtEJ[ currentMode ].getRecord(str);
        set_record_edit(rtEJ[currentMode].getJson());
        contentsChanged = false;

        // load gems3k list
        common::TFile newgems( rtEJ[currentMode].getGems3kName() );
        if( newgems.Name() != gemsLstFile.Name() ) // new list
        {
            if( !newgems.Name().empty() )
            {   gemsLstFile.ChangeName( newgems.Name() );
                selectGEMS( gemsLstFile.getPath() );
            }
        }
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
    {
        row++;
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
    {
        row--;
        QTableWidgetItem *curItem = keyTable->item(row,0);
        keyTable->setCurrentItem( curItem );
        keyTable->scrollToItem( curItem );
        openRecordKey( row, 0  );
    }
}

void FITMainWindow::CmUpdateTest()
{
    try {
        std::string recBson= recordEdit->toPlainText().toStdString();
        if( JsonDataShow ) {
            fromJsonString(recBson);
            setStatusText( "Text in the editor is in valid JSON format" );
        }
        else {
            fromYamlString(recBson);
            setStatusText( "Text in the editor is in valid YAML format" );
        }
    }
    catch(TError& err) {
        setStatusText(err.title);
        addLinetoStatus(err.mess);
    }
}

/// Save current record to DB file as new
void FITMainWindow::CmUpdate()
{
    try
    {
        RecSave( get_record_edit(), rtEJ[currentMode].packKey() );
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
        rtEJ[currentMode].setJson(get_record_edit());
        rtEJ[currentMode].insertRecord();
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
        std::string strKey = rtEJ[currentMode].packKey();
        if( !vfQuestion(window(), rtEJ[currentMode].getKeywd(),
                        "Confirm deletion of data record keyed "+ strKey ))
            return;
        RecDelete(strKey);
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


/// Create new record
void FITMainWindow::CmCreate()
{
    try
    {
        if( currentMode == MDF_TASK )
            createTaskTemplate();
        else
            set_record_edit(ExpTemplate.toStdString());
        contentsChanged = false;
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/*{
     "expdataset" :   { '$in' : ["CH04D" ] }
}
*/


/// filtered records
void FITMainWindow::CmSearch()
{
    try
    {
        QString valQuery = ui->queryEdit->toPlainText();
        removeComments(valQuery);
        std::string filterText = valQuery.toStdString();

        if( currentMode == MDF_DATABASE )
        {
            rtEJ[MDF_DATABASE].setQuery(filterText);
            // reopen records
            rtEJ[MDF_DATABASE].CloseDB();
            rtEJ[MDF_DATABASE].OpenDB();
            resetMainWindow(); // need change key list& current record
            contentsChanged = false;
            setStatusText( "Record filtered" );
        }
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Load search from current temlate file
void FITMainWindow::CmResetSearch()
{
    try
    {
        if( currentMode == MDF_DATABASE )
            ui->queryEdit->setText(SrchTemplate);
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Save current search to txt file
void FITMainWindow::CmSaveSearch()
{
    try
    {
        // open file to unloading
        std::string fname= fitTaskDir.Dir()+ "/search1.txt";
        if( !ChooseFileSave( nullptr, this, fname, "Please, select file name to save search template","*.txt" ))
            return;

        QString filterText = ui->queryEdit->toPlainText();
        QFile tmpStriam(fname.c_str());
        if(tmpStriam.open( QIODevice::WriteOnly|QIODevice::Truncate))
        {
            tmpStriam.write(filterText.toUtf8());
            tmpStriam.close();
        }
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Load current search from txt file
void FITMainWindow::CmLoadSearch()
{
    try
    {
        // open file to unloading
        std::string fname = fitTaskDir.Dir();
        if( !ChooseFileOpen(nullptr, this, fname, "Please, select file name to load search template","*.txt" ))
            return;

        QString valStr;
        QFile tmpStriam(fname.c_str());
        if(tmpStriam.open( QIODevice::ReadOnly ))
        {
            valStr = tmpStriam.readAll();
            tmpStriam.close();
        }
        else
            Error( fname,  "error open file ");

        ui->queryEdit->setText(valStr);
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Insert search results into task definition
void FITMainWindow::CmInsertSearch()
{
    try
    {
        if( currentMode == MDF_TASK )
        {
            std::string samplelist;
            defineModuleKeysList(samplelist);
            //cout << samplelist << endl;
            // https://json.nlohmann.me/features/merge_patch/#patches
            auto bsrec = common::JsonFree::parse(get_record_edit());
            auto inprec = common::JsonFree::parse(samplelist);
            bsrec.merge_patch(inprec);
            //bson_merge( &bsrec, &inprec, true, &out );
            //show result
            set_record_edit(bsrec.dump());
            setStatusText( "List inserted" );
        }
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}


//-------------------------------------------------------------------------------------
// Run and show commands

/// Run gemsfit task
void FITMainWindow::CmRunTest()
{
    try
    {
        //if( !MessageToSave() )
        //    return;

        // create work directory
        QString workDir = QString(fitTaskDir.Dir().c_str()) + "/work";
        QDir dir(workDir);
        if( !dir.mkpath(workDir ) )
            Error( fitTaskDir.Dir(), "Error creating work directory");

        // empty work directory
        removeDirectoryEntry( dir );

        // Load curent record to json structure
        auto current_json = get_record_edit();
        common::JsonFree  jsrec = common::JsonFree::parse(current_json);

        //test current key
        lastCalcRecordKey = rtEJ[currentMode].getKeyFromJson(jsrec);
        //save record before run
        if( rtEJ[currentMode].findRecord(lastCalcRecordKey))
            rtEJ[currentMode].saveRecord(lastCalcRecordKey);
        else
            rtEJ[currentMode].insertRecord();

        // open file to unloading
        std::string fname = jsrec.value("taskid", "undefined");
        std::string fpath = "./" + fname + ".json";
        fname = fitTaskDir.Dir()+ "/work/" + fname + ".json";

        // save txt data
        std::fstream ff(fname.c_str(), std::ios::out );
        ff << current_json;
        ff.close();

#ifndef OLD_EJDB
        // disconnect from EJDB2
        closeEJDB();
#endif
        // start run
        // create arguments std::string
        std::stringstream ss;
        ss << KeysLength;
        std::string sss = ss.str();
        ss.str("");

        QStringList cParameters;
        cParameters << "-run" << sss.c_str() << fpath.c_str();

        if( !runProcess( cParameters, workDir) )
            Error("Run gemsfit -run", "Could not start a gemsfit2 process...");

        ui->action_Run_test->setEnabled(false);
        //ui->action_Show_Results->setEnabled(false);
        ui->actionCancel_gemsfit2_run->setEnabled(true);

        setStatusText( "Started a gemsfit2 task process..." );

    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Kill gemsfit task
void FITMainWindow::CmCancelGemsfit()
{
    try
    {
        fitProcess->kill();
        //ui->action_Run_test->setEnabled(false);
        //ui->action_Show_Results->setEnabled(false);
        //ui->actionCancel_gemsfit2_run->setEnabled(true);

        setStatusText( "Killed gemsfit2 run for the current task..." );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Show after Run gemsfit task
void FITMainWindow::CmShowCalcResults()
{
    try
    {
        //if( !MessageToSave() )
        //    return;

        // path to work directory
        QString workDir = QString(fitTaskDir.Dir().c_str()) + "/work/output";
        OpenResults( lastCalcRecordKey, workDir );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Show saved gemsfit results
void FITMainWindow::CmShowFitResults()
{
    try
    {
        //if( !MessageToSave() )
        //    return;

        // path to work directory
        std::string reckey = getRecordKey(  keyTable->currentRow()  );
        OpenResults( reckey );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}


//-------------------------------------------------------------------------------------
// Record list

void FITMainWindow::CmKeysToTXT()
{
    try
    {
        if( ! MessageToSave() )
        return;

        // get current filter
        std::string keyFilter = ui->filterEdit->text().toStdString();
        if( keyFilter.empty() )
            keyFilter = ALLKEY;

        // select record list to unload
        std::vector<std::string> aKey = vfMultiKeys( this, "Please, mark records to be listed", currentMode, keyFilter.c_str() );
        if( aKey.size() <1 )
                return;

        // open file to unloading
         std::string fname =  projectSettings->value("ProjFileName", "undefined").toString().toStdString();
                fname += ".";
                fname += rtEJ[ currentMode ].getKeywd();
                fname += ".txt";
         common::TFile  outFile("", std::ios::out );
         if( !ChooseFileSave(&outFile, this, fname, "Please, give a file name for unloading keys","*.txt" ))
              return;
         outFile.Open();

         for(size_t i=0; i<aKey.size(); i++ )  {
           outFile.ff << aKey[i] << "\n";
         }
         outFile.Close();

         changeKeyList();
         contentsChanged = false;
         setStatusText( "Keys exported to txt-file" );
     }
     catch( TError& err )
     {
         setStatusText( err.title );
         addLinetoStatus( err.mess );
     }
}


/// Export Data Records to json txt-file
void FITMainWindow::CmBackupJSON()
{
    try
    {
        if( !MessageToSave() )
            return;

        // get current filter
        std::string keyFilter = ui->filterEdit->text().toStdString();
        if( keyFilter.empty() )
            keyFilter = ALLKEY;

        // select record list to unload
        std::vector<std::string> aKey = vfMultiKeys( this, "Please, mark records to be unloaded to JSON",
                                                     currentMode, keyFilter.c_str() );
        if( aKey.size() <1 )
            return;

        // open file to unloading
        std::string fname =  projectSettings->value("ProjFileName", "undefined").toString().toStdString();
        fname += ".";
        fname += rtEJ[ currentMode ].getKeywd();
        fname += ".json";

        common::TFile  outFile("", std::ios::out );
        if( !ChooseFileSave(&outFile, this, fname, "Please, give a file name for unloading records","*.json" ))
            return;
        outFile.Open();

        outFile.ff << "[\n";
        for(size_t i=0; i<aKey.size(); i++ )
        {
            rtEJ[currentMode].getRecord(aKey[i]);
            std::string valDB = rtEJ[currentMode].getJson();
            auto obj_json = common::JsonFree::parse(valDB);
            outFile.ff << obj_json.dump();
            if( i<aKey.size()-1)
            {
                outFile.ff <<  ",";
            }
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
        std::string fname;
        common::TFile  inFile("", std::ios::in);
        if( !ChooseFileOpen(&inFile, this, fname, "Please, select file with unloaded records","*.json" ))
            return;
        inFile.Open();
        auto arr_json = common::JsonFree::parse(inFile.ff);

        for(const auto& object : arr_json)
        {
            rtEJ[currentMode].setJson( object->dump(true));
            if( ui->actionOverwrite->isChecked() )
                rtEJ[currentMode].saveRecord("");
            else
                rtEJ[currentMode].insertRecord();

        }
        setStatusText( "Records imported from json txt-file" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }

    changeKeyList();
    contentsChanged = false;
}

/// Export Data Records to YAML txt-file
void FITMainWindow::CmBackupYAML()
{
    try
    {
        if( !MessageToSave() )
            return;

        // get current filter
        std::string keyFilter = ui->filterEdit->text().toStdString();
        if( keyFilter.empty() )
            keyFilter = ALLKEY;

        // select record list to unload
        std::vector<std::string> aKey = vfMultiKeys( this, "Please, mark records to be unloaded to YAML",
                                                     currentMode, keyFilter.c_str() );
        if( aKey.size() <1 )
            return;

        // open file to unloading
        std::string fname =  projectSettings->value("ProjFileName", "undefined").toString().toStdString();
        fname += ".";
        fname += rtEJ[ currentMode ].getKeywd();
        fname += ".yaml";
        common::TFile  outFile("", std::ios::out );
        if( !ChooseFileSave(&outFile, this, fname, "Please, give a file name for unloading records","*.yaml" ))
            return;
        std::string json_array_str = "[\n";
        for(size_t i=0; i<aKey.size(); i++ )
        {
            rtEJ[ currentMode ].getRecord(aKey[i]);
            json_array_str +=  rtEJ[currentMode].getJson();
            if( i<aKey.size()-1) {
                json_array_str +=  ",";
            }
            json_array_str += "\n";
        }
        json_array_str += "]";

        auto yaml_str = common::yaml::Json2Yaml(json_array_str);
        outFile.Open();
        outFile.ff << yaml_str;
        outFile.Close();

        changeKeyList();
        contentsChanged = false;
        setStatusText( "Records exported to YAML txt-file" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Import Data Records from YAML txt-file
void FITMainWindow::CmRestoreYAML()
{
    try
    {
        if( !MessageToSave() )
            return;

        // open file to unloading
        std::string fname;
        common::TFile  inFile("", std::ios::in);
        if( !ChooseFileOpen(&inFile, this, fname, "Please, select file with unloaded records","*.yaml" ))
            return;
        auto yaml_str = inFile.ReadAll();
        auto jsonstr =  common::yaml::Yaml2Json(yaml_str);
        auto arr_json = common::JsonFree::parse(jsonstr);

        for(const auto& object : arr_json)
        {
            rtEJ[currentMode].setJson( object->dump(true));
            if( ui->actionOverwrite->isChecked() )
                rtEJ[currentMode].saveRecord("");
            else
                rtEJ[currentMode].insertRecord();

        }
        setStatusText( "Records imported from yaml txt-file" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }

    changeKeyList();
    contentsChanged = false;

}

/// Import Data Records experements from csv-file
void FITMainWindow::CmRestoreCSV()
{
    try
    {
        int ii, jj;
        QString valCsv ="";
        if( !MessageToSave() )
            return;

        // select files with data
        auto csv_files = ChooseListFilesOpen(this, "", "Please, select file with unloaded csv format","*.csv" );
        setStatusText( "Records importing from csv-file ..." );
        foreach (const QString &fpath, csv_files) {
            // read file
            std::cout << "Read file: " << fpath.toStdString() << std::endl;
            QFileInfo striam(fpath);
            addLinetoStatus( striam.fileName().toStdString());

            try {
                QFile tmpStriam(fpath);
                if(!tmpStriam.open( QIODevice::ReadOnly ))
                    Error( fpath.toStdString(), "File open error");
                valCsv = tmpStriam.readAll();
                tmpStriam.close();

                // split csv data
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
                const QStringList allrows = valCsv.split('\n', QString::KeepEmptyParts);
#else
                const QStringList allrows = valCsv.split('\n', Qt::KeepEmptyParts);
#endif
                std::vector<std::string> headline;
                std::vector<std::string> row;
                common::JsonFree exp;

                //get header
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
                QStringList cells = allrows[0].split(',', QString::KeepEmptyParts);
#else
                QStringList cells = allrows[0].split(',', Qt::KeepEmptyParts);
#endif
                for( ii=0; ii< cells.count(); ii++ )
                {
                    auto cell_ = cells[ii].remove('\"');
                    auto cellstr = cell_.remove('\r').toStdString();
                    headline.push_back( cellstr );
                }

                for( jj=1; jj< allrows.count(); jj++ )
                {
                    // get row
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
                    cells = allrows[jj].split(',', QString::KeepEmptyParts);
#else
                    cells = allrows[jj].split(',', Qt::KeepEmptyParts);
#endif
                    if( cells.count() < headline.size() )
                        continue;
                    row.clear();
                    for( ii=0; ii< cells.count(); ii++ )
                    {
                        auto cell_ = cells[ii].remove('\"');
                        auto cellstr = cell_.remove('\r').toStdString();
                        row.push_back( cellstr );
                    }

                    if (row.size()>0)
                    {
                        size_t found = row[0].find("STOP");
                        if (found != std::string::npos)
                        {
                            break;
                        }
                    }
                    // convert row to json
                    csvToBson( exp, headline, row );
                    std::string bsonVal = exp.dump();

                    //save results to EJDB
                    rtEJ[ currentMode ].setJson(bsonVal);
                    if( ui->actionOverwrite->isChecked() )
                        rtEJ[currentMode].saveRecord("");
                    else
                        rtEJ[currentMode].insertRecord();
                }
            }
            catch(TError& err) {
                addLinetoStatus(err.mess);
            }
        }
        addLinetoStatus( "Records imported from csv-file" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
    changeKeyList();
    contentsChanged = false;

}

/// Export Data Records from task configuration txt-file
void FITMainWindow::CmBackupTXT()
{
    try
    {
        if( !MessageToSave() )
            return;

        std::string projDir = fitTaskDir.Dir();
        QString dir = QFileDialog::getExistingDirectory(this, "Select Directory", projDir.c_str(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
        projDir = dir.toStdString();

        // Load curent record to json structure
        common::JsonFree  bsrec = common::JsonFree::parse(get_record_edit());

        std::string fname = bsrec.value("taskid", "undefined");
        fname = projDir + "/" + fname + ".json";
        // open file to unloading
        std::fstream ff(fname, std::ios::out );
        ff << bsrec.dump();;
        ff.close();

        changeKeyList();
        contentsChanged = false;
        setStatusText( "Records exported to json json-file" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}


/// Command Import Data Records from task configuration txt-file
void FITMainWindow::CmRestoreTXT()
{
    try
    {
        if( !MessageToSave() )
            return;

        // open file to unloading
        std::string fname;
        common::TFile  inFile("", std::ios::in);
        if( !ChooseFileOpen(&inFile, this, fname, "Please, select file with GEMSFIT2 specificatins file","*.json" ))
            return;

        readTXT(inFile);
        changeKeyList();
        contentsChanged = true;
        setStatusText( "Records imported from json txt-file" );
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

/// Import Data Record from task configuration txt-file
void FITMainWindow::readTXT( common::TFile& inFile )
{
    inFile.Open();

    std::ifstream t(inFile.getPath());
    std::string str, str2;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

    size_t found;

    // adds taskid and projectid fileds to the task configuration file
    found = str.find("{");
    str2 = "{\n     \"taskid\":   \""; str2 += inFile.Name(); str2 +="\", \n";
    str2 += "     \"projectid\":   \""; str2 += fitTaskDir.Name(); str2 +="\", ";
    str = str.substr(found+1, str.size());
    str2 += str;

    //show result
    set_record_edit(str2);
}

/// Delete the list of records
void FITMainWindow::CmDeleteList()
{
    try
    {
        if( !MessageToSave() )
            return;

        // get current filter
        std::string keyFilter = ui->filterEdit->text().toStdString();
        if( keyFilter.empty() )
            keyFilter = ALLKEY;

        // select record list to unload
        std::vector<std::string> aKey = vfMultiKeys( this, "Please, mark record keys to be deleted from database",
                                                     currentMode, keyFilter.c_str() );
        if( aKey.size() <1 )
            return;

        for(size_t i=0; i<aKey.size(); i++ )
            RecDelete( aKey[i].c_str() );
        rtEJ[currentMode].setKey(ALLKEY);
        //        changeKeyList();
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
    changeKeyList();
}

// write TP paris file
void FITMainWindow::CmTPpairsCSV()
{
    // Select all
    std::vector<double> TP[2], TP_pairs[2];
    bool isfound = false, isfound2 = false;

    common::SetReaded_f setfnc = [&TP](const std::string& bsdata)
    {
        auto object = fromJsonString(bsdata);
        if (object.contains("sT") ) {
            TP[0].push_back(object["sT"].to_double());
        }
        if (object.contains("sP") ) {
            TP[1].push_back(object["sP"].to_double());
        }
    };

    // execute setfnc for all records
    rtEJ[currentMode].selectQuery("", setfnc);

    // get distinct TP                                                          //D.2 getting the distinct T and P pairs
    for (size_t i=0; i<TP[0].size(); i++)
    {
        // check if TP pair is presnt more than once in the TP vector
        for (size_t j=0; j<TP[0].size(); j++)
        {
            if ((TP[0][i] == TP[0][j]) && (TP[1][i] == TP[1][j]) && (i != j))
            {
                isfound = true;
            }
        }
        // check if TP pair was added to the unique TP pairs container
        for (size_t j=0; j<TP_pairs[0].size(); ++j)
        {
            if ((TP[0][i] == TP_pairs[0][j]) && (TP[1][i] == TP_pairs[1][j]))
            {
                isfound2 = true;
            }
        }
        // add TP pair if it does not repeat itself or was not added already in the container
        if ((!isfound) || (!isfound2))
        {
            TP_pairs[0].push_back(TP[0][i]);
            TP_pairs[1].push_back(TP[1][i]);
        }
        isfound = false;
        isfound2 = false;
    }

    std::string fname;
    common::TFile  outFile("", std::ios::out );
    if( !ChooseFileSave(&outFile, this, fname, "Please, give a file name for exporting the P-T pairs ", "*.csv" ))
        return;
    outFile.Open();

    for (size_t i=0; i<TP_pairs[1].size(); ++i)
    {
        outFile.ff <<TP_pairs[1][i]<<";"<<TP_pairs[0][i]<<std::endl;
    }
    outFile.ff << TP_pairs[1].size() <<std::endl;
    outFile.Close();

    setStatusText( "P-T pairs of the experiments in the database were exported to the csv file" );
}

void FITMainWindow::actionFindNext()
{
    if( !findLine )
        return;

    QTextDocument::FindFlags flg;
    if(ui->action_Case_sensetiv->isChecked() )
        flg |=QTextDocument::FindCaseSensitively;

    if(ui->action_Whole_words->isChecked() )
        flg |=QTextDocument::FindWholeWords;

    recordEdit->find( findLine->text(), flg );
}

void FITMainWindow::actionFindPrevious()
{
    if( !findLine )
        return;

    QTextDocument::FindFlags flg = QTextDocument::FindBackward;
    if(ui->action_Case_sensetiv->isChecked() )
        flg |=QTextDocument::FindCaseSensitively;

    if(ui->action_Whole_words->isChecked() )
        flg |=QTextDocument::FindWholeWords;

    recordEdit->find( findLine->text(), flg );
}

void FITMainWindow::actionFind()
{
    actionFindNext();
}

void FITMainWindow::actionZoomIn()
{
    recordEdit->zoomIn(2);
}

void FITMainWindow::actionZoomOut()
{
    recordEdit->zoomOut(2);
}

//-------------------------------------------------------------------------------------
// Help menu

void FITMainWindow::CmSettingth()
{
    try
    {
        if( !MessageToSave() )
            return;

        // define new preferences
        PreferencesDialog dlg(mainSettings);
        if( !dlg.exec() )
            return;

        //get data from settings
        getDataFromPreferences();
        // update key list, editor, filter
        resetMainWindow();
    }
    catch( TError& err )
    {
        setStatusText( err.title );
        addLinetoStatus( err.mess );
    }
}

void FITMainWindow::CmHelp()
{
    if( currentMode == MDF_DATABASE )
        OpenHelp( GF_SABOUT_HTML );
    else
        OpenHelp( GF_TASKDB_HTML );
}

void FITMainWindow::CmHelpAbout()
{
    OpenHelp( GF_ABOUT_HTML );
}

void FITMainWindow::CmHelpAuthors()
{
    OpenHelp( GF_AUTHORS_HTML );
}


void FITMainWindow::CmHelpLicense()
{
    OpenHelp( GF_LICENSE_HTML );
}


//--------------- end of  FITMainMenu.cpp  -----------------------------


