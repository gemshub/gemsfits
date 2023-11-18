//-------------------------------------------------------------------
// $Id: FITMainWindow.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of GEMSFITS GUI Main Window (window part)
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
#include "FITMainWindow.h"
#include "ui_FITMainWindow.h"
#include "HelpWindow.h"
#include "FitResultsWindow.h"
#include "f_database.h"
#include "v_service.h"
#include "gui_service.h"
#include "keywords.h"
#include "json_view.h"
#include "v_yaml.h"

//--------------------------------------------------------------------------

void TKeyTable::keyPressEvent(QKeyEvent* e)
{
    QTableWidget::keyPressEvent(e);
    switch( e->key() )
    {
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
        pFitImp->openRecordKey(  currentRow(), currentColumn()  );
        break;
    }
}

//------------------------------------------------------------------------------

FITMainWindow* pFitImp;

void FITMainWindow::setDefValues(int /*c*/, char** /*v*/)
{
    // load main programm settingth
    mainSettings = new QSettings("gemsfits.ini", QSettings::IniFormat);
    getDataFromPreferences();
}

void FITMainWindow::getDataFromPreferences()
{
    if( !mainSettings)
        return;

    SysFITDir =  mainSettings->value("ResourcesFolderPath", "../Resources/").toString().toStdString();
    LocalDocDir =  mainSettings->value("HelpFolderPath", "../Resources/help").toString().toStdString();
    UserDir = mainSettings->value("UserFolderPath", ".").toString().toStdString();
    KeysLength = mainSettings->value("PrintComments", true).toBool();
    JsonDataShow = !mainSettings->value("ViewinYAMLFormat", false).toBool();
    EditorDataShow = mainSettings->value("ViewinModelEditor", true).toBool();

    QString program = mainSettings->value("Gemsfit2ProgramPath", "gemsfit2").toString();
    fitProcess->setProgram( program );

    // load experiment template text
    QString fname = SysFITDir.c_str();
    fname += "/data/" + mainSettings->value("ExpTemplateFileName", "...").toString();
    QFile tmpString(fname);
    if(tmpString.open( QIODevice::ReadOnly))
    {
        ExpTemplate = tmpString.readAll();
        tmpString.close();
    }

    // load experiment search text
    fname = SysFITDir.c_str();
    fname += "/templates/" + mainSettings->value("TemplateSearchFileName", "...").toString();
    QFile tmpString1(fname);
    if(tmpString1.open( QIODevice::ReadOnly))
    {
        SrchTemplate = tmpString1.readAll();
        tmpString1.close();
    }

    if( mainSettings->value("PrintGEMSFITMessages", true).toBool())
        connect( fitProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(showProcessMesage()) );
    else
        disconnect( fitProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(showProcessMesage()) );
}

FITMainWindow::FITMainWindow(int c, char** v, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::FITMainWindow),
    currentMode(MDF_TASK), gemsLstFile(""), fitTaskDir(""),
    aNode(0), contentsChanged(false), helpProcess(0),
    projectSettings(0),  mainSettings(0)
{
    ui->setupUi(this);

    // setup process
    fitProcess = new QProcess( this);
    //connect( fitProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(showProcessMesage()) );
    connect( fitProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT( runFinished(int,QProcess::ExitStatus)));
    //connect( fitProcess, SIGNAL(readyReadStandardError()), this, SLOT(ReadErr()) );

    //set up main parameters
    setDefValues( c, v);
    axisLabelFont = QFont("Courier New", 14);
    ui->actionCancel_gemsfit2_run->setEnabled(false);

    // define tool bar
    toolTasks = new QToolBar(this);
    toolTasks->setObjectName(QString::fromUtf8("toolTasks"));
    //toolTasks->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolTasks->setMovable(false);
    addToolBar(Qt::LeftToolBarArea, toolTasks );
    toolTasks->setIconSize(QSize(40,40));
    toolTasks->addAction(ui->action_DataBase_mode);
    toolTasks->addAction(ui->action_Task_Mode);
    toolTasks->addAction(ui->actionFits_View_Mode);
    toolTasks->setWindowTitle("toolTasks");

   // define edit tree view
   QStringList aHeaderData;
   aHeaderData << "key" << "value";
   json_tree.reset( new jsonui17::JsonView() );
   auto* json_model = new jsonui17::JsonModel("", aHeaderData, this );
   jsonui17::JsonDelegate* deleg_schema = new jsonui17::JsonDelegate();
   json_tree->setModel(json_model);
   json_tree->setItemDelegate(deleg_schema);
   json_tree->setColumnWidth( 0, 250 );
   json_tree->expandToDepth(2);
   json_tree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   ui->verticalLayout_7->addWidget(json_tree.get());

   recordEdit.reset( new QTextEdit());
   recordEdit->setObjectName("recordEdit");
   ui->verticalLayout_7->addWidget(recordEdit.get());

    // define main window
    keyTable = new TKeyTable( this );
    ui->gridLayout->addWidget( keyTable, 1, 0, 1, 2 );
    keyTable->setEditTriggers( QAbstractItemView::NoEditTriggers );
    keyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    keyTable->setSelectionBehavior ( QAbstractItemView::SelectRows );

    ui->splKey->setStretchFactor(0, 2);
    ui->splKey->setStretchFactor(1, 1);
    ui->splGEMS->setStretchFactor(0, 1);
    ui->splGEMS->setStretchFactor(1, 3);
    ui->splRecord->setStretchFactor(0, 4);
    ui->splRecord->setStretchFactor(1, 1);

    ui->splV->setStretchFactor(0, 1);
    ui->splV->setStretchFactor(1, 3);
    ui->splV->setStretchFactor(2, 2);

    // set up menu
    setActions();
    connect(ui->splV, SIGNAL(splitterMoved(int,int)), this, SLOT(moveToolBar(int,int)));
    connect(keyTable, SIGNAL(cellClicked(int,int)), this, SLOT(openRecordKey(int,int)));
    connect( recordEdit.get(), SIGNAL(undoAvailable(bool)),  this, SLOT(recEdited(bool)));
    connect( ui->filterEdit, SIGNAL(editingFinished()), this, SLOT(changeKeyList()) );

    // setup first lists
    CmDBMode();
}

FITMainWindow::~FITMainWindow()
{
    delete ui;
}

void FITMainWindow::closeEvent(QCloseEvent* /*e*/)
{
    //if( pVisor->CanClose() )
    {
        for(int ii=0; ii<10000; ii++);

        if( HelpWindow::pDia )
            delete HelpWindow::pDia;

        if( FitResultsWindow::pDia )
        {
            FitResultsWindow::pDia->close();
            //delete  FitResultsWindow::pDia;
        }
        //mdiArea->closeAllSubWindows();
        //if( mdiArea->subWindowList().count() > 0 )
        //    e->ignore();
        //else
        //    QWidget::closeEvent(e);
    }
}

void FITMainWindow::resizeEvent ( QResizeEvent * event )
{
    QMainWindow::resizeEvent(event);
    moveToolBar(1,1);
}

void FITMainWindow::showEvent ( QShowEvent * event )
{
    QMainWindow::showEvent(event);
    moveToolBar(1,1);
}

/*!
   Opens html <file> positioned on anchor <item> (as in "file:/myfile.html#item")

   NOTE: we should not specify parent parameter for modeless dialog
   because otherwise even that it allows work with parent window it will
   be always overlapping it. For modal windows (and thus modal help) we need parent
*/
void FITMainWindow::GetHelp( )
{
    (new HelpWindow(0));
    //HelpWindow::pDia->show();
}

void FITMainWindow::OpenHelp(const char* file, const char* item1, int page )
{
    if( HelpWindow::pDia )
    {
        std::cout << "OpenHelp: " << file << std::endl;
        if( item1 && page>=0 )
        {
            QString res = item1;
            res += QString("_%1").arg(page);
            std::string txt = res.toStdString();
            HelpWindow::pDia->showDocumentation( file, txt.c_str() );
        }
        else
            HelpWindow::pDia->showDocumentation( file, item1 );

        HelpWindow::pDia->show();
        HelpWindow::pDia->raise();
    }
    // old help assistantClient->showDocumentation( file, item);
}

void FITMainWindow::OpenResults( const std::string& key, const QString& dir )
{
    if( !FitResultsWindow::pDia )
    {
        ( new FitResultsWindow(0) );
    }
    if( !dir.isEmpty() )
        FitResultsWindow::pDia->ShowResults(key, dir);
    else
        FitResultsWindow::pDia->ShowResults(key );

    FitResultsWindow::pDia->raise();
}


void  FITMainWindow::moveToolBar( int , int )
{
    ui->toolBar_2->setFixedWidth(toolTasks->width());
    ui->toolBarTask->setFixedWidth(ui->splV->widget(0)->width());
    ui->toolBarMenu->setFixedWidth(ui->splV->widget(1)->width());
}

void FITMainWindow::setTableIComp()
{
    std::string valStr;
    QTableWidgetItem *item;

    DATACH  *dCH =  node()->pCSD();
    ui->tableIComp->clear();

    ui->tableIComp->setColumnCount(5);
    ui->tableIComp->setRowCount(dCH->nIC/5+1);

    for( int ii = 0; ii<dCH->nIC; ii++ )
    {
        valStr = std::string( dCH->ICNL[ii], 0,MaxICN );
        item = new QTableWidgetItem(tr("%1").arg( valStr.c_str()));
        ui->tableIComp->setItem(ii/5, ii%5, item );
    }
}

void FITMainWindow::setListPhase()
{
    int ii, jj,j;
    std::string valStr;

    DATACH  *dCH =  node()->pCSD();
    ui->listPhases->clear();

    for( ii = 0, jj=0; ii<dCH->nPH; ii++ )
    {
        valStr = std::string( dCH->PHNL[ii], 0,MaxPHN );
        QTreeWidgetItem *phase = new QTreeWidgetItem(ui->listPhases);
        phase->setText(0, valStr.c_str());

        for( j=0; j<dCH->nDCinPH[ii]; j++, jj++ )
        {
            valStr = std::string( dCH->DCNL[jj], 0, MaxDCN );
            QTreeWidgetItem *dcomp = new QTreeWidgetItem(phase);
            dcomp->setText(0, valStr.c_str());
        }
    }
}

void FITMainWindow::setStatusText( const std::string& text )
{
    QString vals = QString(text.c_str()) + "\n";
    ui->statusEdit->setPlainText( vals );
}

void FITMainWindow::addLinetoStatus( const std::string& line )
{
    //QString vals = ui->statusEdit->toPlainText();
    //vals += trUtf8(line.c_str()) + "\n";
    //ui->statusEdit->setPlainText( vals );
    ui->statusEdit->append( QString( line.c_str()) + "\n" );
}

void FITMainWindow::showProcessMesage( )
{
    QProcess *p = dynamic_cast<QProcess *>( sender() );
    if (p)
        ui->statusEdit->append( p->readAllStandardOutput() );
}

//----------------------------------------------------------
// Working with EJDB

void FITMainWindow::closeEJDB()
{
    rtEJ.Close();
}

/// Connect project database
void FITMainWindow::openEJDB()
{
    // close old project data
    closeEJDB();

    // set up new ejdb file
    std::string ejdbPath  = projectSettings->value("ProjFolderPath", ".").toString().toStdString();
    ejdbPath  += projectSettings->value("ProjDatabasePath", "/EJDB").toString().toStdString();
    ejdbPath  += "/";
    ejdbPath  += projectSettings->value("ProjDatabaseName", "myprojdb1" ).toString().toStdString();
    rtEJ.ChangePath(ejdbPath);

    // change collections names
    std::string samlescolName = projectSettings->value("ExpSamplesDataColl", "experiments").toString().toStdString();
    rtEJ[MDF_DATABASE].setKeywd(samlescolName);
    rtEJ[MDF_DATABASE].OpenDB();
    std::string testcolName = projectSettings->value("TaskCasesDataColl", "tests").toString().toStdString();
    rtEJ[MDF_TASK].setKeywd(testcolName);
    rtEJ[MDF_TASK].OpenDB();
    testcolName = projectSettings->value("FitsCasesDataColl", "fits").toString().toStdString();
    rtEJ[MDF_FITS].setKeywd(testcolName);
    rtEJ[MDF_FITS].OpenDB();
}

/// Set up data for current project
void FITMainWindow::loadNewProject()
{
    // clear old GEMS3K list windows
    ui->tableIComp->clear();
    ui->listPhases->clear();
    pLineGEMS->setText("");
    // clear all queries
    ui->queryEdit->setText("");
    rtEJ[MDF_DATABASE].setQuery("");
    rtEJ[MDF_TASK].setQuery("");
    rtEJ[MDF_FITS].setQuery("");

    // Connect project database
    openEJDB();

    pLineTask->setText( projectSettings->value("ProjFileName", "undefined").toString());
    // change gemsLstFile information
    std::string gemsname  = projectSettings->value("ProjFolderPath", ".").toString().toStdString();
    gemsname  += projectSettings->value("GEMS3KFilesPath", "/GEMS").toString().toStdString();
    gemsname += "/undef.lst";
    gemsLstFile.ChangePath( gemsname );
    gemsLstFile.ChangeName("");

    // empty status
    setStatusText( "" );
    // update key list, editor, filter
    resetMainWindow();
}

/// Set up data for Main window
void FITMainWindow::resetMainWindow()
{
    if(EditorDataShow) {
        recordEdit->hide();
        json_tree->show();
        ui->menuFin_d->setEnabled(false);
        ui->menuView->setEnabled(false);
        ui->actionFind_Previous->setEnabled(false);
        ui->actionFind_Next->setEnabled(false);
        ui->menu_Edit->setEnabled(true);
    }
    else {
       json_tree->hide();
       recordEdit->show();
       ui->menuFin_d->setEnabled(true);
       ui->menuView->setEnabled(true);
       ui->actionFind_Previous->setEnabled(true);
       ui->actionFind_Next->setEnabled(true);
       ui->menu_Edit->setEnabled(false);
    }
    // set up filter
    ui->filterEdit->setText("*");

    // update key list
    int curInd = defineModuleKeysList( currentMode );

    // load first record
    if( keyTable->rowCount() > 0 )
    {
        if( curInd >= 0 )
            openRecordKey( curInd, 0  );
        else
        {
            QTableWidgetItem *curItem = keyTable->item(0,0);
            keyTable->setCurrentItem( curItem );
            keyTable->scrollToItem( curItem );
            openRecordKey( 0, 0  );
        }
    }
    else
    {
        // loadTemplate( currentModule );
        CmCreate();
        if( !projectSettings )
            addLinetoStatus(  "Please, open or create a project first." );
    }

    // reset  ui->queryEdit
    ui->queryEdit->setText(rtEJ[currentMode].getLastQuery().c_str());
    if(  !rtEJ[currentMode].getLastQuery().empty() )
    {
        ui->action_Insert->setEnabled(false);
        ui->actionCreate_New->setEnabled(false);
        ui->actionRestore_from_csv->setEnabled(false);
        ui->actionRestore_from_JSON->setEnabled(false);
        ui->actionRestore_from_TXT->setEnabled(false);
        ui->actionRestore_from_YAML->setEnabled(false);
    }
    else
    {
        ui->action_Insert->setEnabled(true);
        ui->actionCreate_New->setEnabled(true);
        ui->actionRestore_from_JSON->setEnabled(true);
        ui->actionRestore_from_YAML->setEnabled(true);
        if( currentMode == MDF_DATABASE )
            ui->actionRestore_from_csv->setEnabled(true);
        else
            ui->actionRestore_from_TXT->setEnabled(true);
    }

}


void FITMainWindow::changeKeyList()
{
    defineModuleKeysList( currentMode );
}

/// Define list of Module keys using filter
int FITMainWindow::defineModuleKeysList( int nRT )
{
    int ii, kk, ln;
    int curInd = -1;
    std::string keyfld;
    QTableWidgetItem *item, *curItem=0;
    std::string oldKey = rtEJ[nRT].packKey();
    //settedCureentKeyIntotbKeys = false;

    if( currentMode != nRT)
        return curInd;

    // define tbKeys
    keyTable->clear();
    keyTable->setSortingEnabled ( false );
    keyTable->setColumnCount( rtEJ[nRT].keySize());

    // get list or record keys
    std::string keyFilter = ui->filterEdit->text().toStdString();
    if( keyFilter.empty() )
        keyFilter = ALLKEY;
    std::vector<std::string> keyList;
    int nKeys = rtEJ[nRT].getKeyList(keyFilter, keyList);

    std::vector<size_t> colSizes;
    for(size_t jj=0; jj<rtEJ[nRT].keySize(); jj++)
     colSizes.push_back( 0 );

    // define key list
    keyTable->setRowCount(nKeys);

    // set up table sizes
    QFontMetrics fm(keyTable->fontMetrics());
    int charWidth = fm.horizontalAdvance("B");
    int charHeight = fm.height();

    for( ii=0; ii<nKeys; ii++ )
    {
        keyTable->setRowHeight(ii, charHeight+2);
        for(size_t jj=0, kk=0; jj<rtEJ[nRT].keySize(); jj++)
        {
            ln = keyList[ii].find_first_of(':', kk);
            keyfld = std::string(keyList[ii], kk, ln-kk);
            strip(keyfld);
            if( keyfld.length()+1 > colSizes[jj])
                colSizes[jj] = keyfld.length()+1;
            kk = ln+1;
            item = new QTableWidgetItem(tr("%1").arg( keyfld.c_str()));
            keyTable->setItem(ii, jj, item );
        }
        if( oldKey == keyList[ii] )
        {    curItem = keyTable->item(ii,0);
            curInd = ii;
            //       settedCureentKeyIntotbKeys = true;
        }
    }
    for(size_t jj=0; jj<rtEJ[nRT].keySize(); jj++)
    {
        keyTable->setColumnWidth(jj, charWidth*colSizes[jj] );
        item = new QTableWidgetItem(tr("%1").arg( rtEJ[nRT].keyFieldName(jj) ));
        //item->setToolTip( ((TCModule*)aMod[nRT])->GetFldHelp(jj));
        keyTable->setHorizontalHeaderItem( jj, item );
    }

    keyTable->setSortingEnabled ( true );
    if(curItem )
    {
        keyTable->setCurrentItem( curItem );
        keyTable->scrollToItem( curItem, QAbstractItemView::PositionAtCenter );
    }

    rtEJ[nRT].setKey(oldKey);
    return curInd;
}

void FITMainWindow::set_record_edit(const std::string &json_text)
{
    std::string current_json;

     if(JsonDataShow) {
            current_json = json_text;
        }
        else{
        current_json = common::yaml::Json2Yaml(json_text);
    }
    json_tree->updateModelData(json_text);
    recordEdit->setText(current_json.c_str());
}

std::string FITMainWindow::get_record_edit()
{
    std::string editor_data;

    if(EditorDataShow) {
        editor_data = json_tree->saveToJson();
    }
    else  {
        editor_data = recordEdit->toPlainText().toStdString();
        if(!JsonDataShow) {
            editor_data = common::yaml::Yaml2Json(editor_data);
        }
    }
    return editor_data;
}


/// Save record structure to Data Base
void FITMainWindow::RecSave( const std::string& recBsonText, const std::string& key)
{
    rtEJ[ currentMode ].setJson(recBsonText);
    rtEJ[ currentMode ].saveRecord(key);
    //defineModuleKeysList( currentMode ); //?? need change key list if new record
    contentsChanged = false;
}

/// Delete record structure from Data Base
void FITMainWindow::RecDelete(const std::string& key )
{
    rtEJ[ currentMode ].deleteRecord(key);
    if( currentMode == MDF_TASK && rtEJ[ MDF_FITS ].findRecord(key) )
        rtEJ[ MDF_FITS ].deleteRecord( key );
}

/// Save solicitation
/// Returns true if user pressed 'save' or 'discard' and false on 'cancel'
bool FITMainWindow::MessageToSave()
{
    std::string key_str = rtEJ[ currentMode ].packKey();
    if( contentsChanged && key_str.find_first_of("*?") == std::string::npos )
    {
        int res = vfQuestion3(window(), key_str.c_str(),
                              "Data record has been changed!",
                              "Save changes", "Discard changes", "Cancel");
        if( res == VF3_3 )
        {
            // set current key as selected in table keys
            defineModuleKeysList( currentMode ); //?? may be function copy of this to only reset current line in table
            return false;
        }

        if( res == VF3_1 )
        {
            RecSave(get_record_edit(), key_str);
        }
    }
    contentsChanged = false;
    return true;
}

/// Change <SystemFiles> data in edit record
std::string FITMainWindow::makeSystemFileName( const std::string& path  )
{
    std::string name = path;
    if(projectSettings )
        name += projectSettings->value("GEMS3KFilesPath", "/GEMS").toString().toStdString();
    name += "/" + gemsLstFile.Name() + "." + gemsLstFile.Ext();
    return name;
}


/// Make template for task record
bool FITMainWindow::createTaskTemplate()
{
    if( gemsLstFile.Name().empty() )
        Error("Create task template", "Undefined System File name.");

    // create arguments std::string
    std::string newPath = makeSystemFileName( "." );
    QStringList cParameters;

    std::stringstream ss;
    ss << KeysLength;
    std::string sss = ss.str();
    ss.str("");

#ifdef buildWIN32
    std::replace( newPath.begin(), newPath.end(), '/', '\\');
#endif

    cParameters << "-initJ" << sss.c_str() << newPath.c_str() << "template.json";

    if( !runProcess( cParameters, fitTaskDir.Dir().c_str()) )
        Error("Run gemsfit -init", "Error starting gemsfit2 process. Check help->Preferences and set the correct path to gemsfit2 code.");

    int ret = fitProcess->waitForFinished();

    // read "template.dat" to json
    std::string path = fitTaskDir.Dir()+ "/template.json";

    std::ifstream my_file(path.c_str());
    if (!my_file.good())
    {
        std::ofstream outfile (path.c_str());
        outfile << "{}" << std::endl;
        outfile.close();
    }

    common::TFile  inFile(path, std::ios::in);
    readTXT( inFile );

    // set up EJDB path
    if( projectSettings )
    {
        std::string ejdbPath  = "..";
        ejdbPath  += projectSettings->value("ProjDatabasePath", "/EJDB").toString().toStdString();
        ejdbPath  += "/";
        ejdbPath  += projectSettings->value("ProjDatabaseName", "myprojdb1" ).toString().toStdString();
        std::string templ_json = get_record_edit();
        auto templ_obj = common::JsonFree::parse(templ_json);
        templ_obj[keys::DBPath[0]] = ejdbPath;
        templ_obj[keys::DBPath[1]] = ejdbPath;
        templ_obj[keys::DBColl[0]] = rtEJ[ MDF_DATABASE ].getKeywd();
        templ_obj[keys::DBColl[1]] = rtEJ[ MDF_DATABASE ].getKeywd();
        set_record_edit(templ_obj.dump());
    }
    return ret;
}


bool FITMainWindow::runProcess( const QStringList& cParameters, const QString& workDir )
{
    fitProcess->setArguments( cParameters);
    fitProcess->setWorkingDirectory( workDir );

    fitProcess->start();
    return fitProcess->waitForStarted();
}

void FITMainWindow::runFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->action_Run_test->setEnabled(true);
    ui->action_Show_Results->setEnabled(true);
    ui->actionCancel_gemsfit2_run->setEnabled(false);

    if( exitStatus==0 && exitCode==0 )
        addLinetoStatus( "Finished gemsfit2 task calculation o.k." );
    else
        addLinetoStatus( "Error in gemsfit2 task calculation..." );
}

/// Make list of Experiments keys using query
void FITMainWindow::defineModuleKeysList( std::string& samplelist )
{
    int ii, kk, ln;
    std::string keyfld;

    // get list or record keys
    std::string keyFilter = ALLKEY;
    std::vector<std::string> keyList;
    int nKeys = rtEJ[ MDF_DATABASE ].getKeyList(keyFilter, keyList);

    samplelist = " \"DataSelect\": {\n    \"samplelist\": [\n";
    for( ii=0; ii<nKeys; ii++ )
    {
        samplelist += "{ \"SA\": \"";
        kk=0;
        ln = keyList[ii].find_first_of(':', kk);
        keyfld = std::string(keyList[ii], kk, ln-kk);
        strip(keyfld);
        kk = ln+1;
        samplelist += keyfld + "\", \"DS\": \"";
        ln = keyList[ii].find_first_of(':', kk);
        keyfld = std::string(keyList[ii], kk, ln-kk);
        strip(keyfld);
        samplelist += keyfld;
        samplelist += "\", \"WT\": 1 }";
        if( ii != nKeys-1)
            samplelist += ",\n";
        else
            samplelist += "\n";
    }
    samplelist += "]\n } }";
}

//--------------- end of  FITMainWindow.cpp  -----------------------------
