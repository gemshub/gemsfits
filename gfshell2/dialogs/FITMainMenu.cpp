#include "FITMainWindow.h"
#include "ui_FITMainWindow.h"


// -----------------------------------------------------------
// Actions and commands

//  Connect all actions
void FITMainWindow::setActions()
{

 // Tasks
    connect( ui->action_DataBase_mode, SIGNAL( triggered()), this, SLOT(CmIComp()));
    connect( ui->action_Task_Mode, SIGNAL( triggered()), this, SLOT(CmDComp()));
    connect( ui->action_New_Project, SIGNAL( triggered()), this, SLOT(CmReacDC()));
    connect( ui->action_Select_project, SIGNAL( triggered()), this, SLOT(CmReacDC()));
    connect( ui->actionSelect_GEMS, SIGNAL( triggered()), this, SLOT(CmRTparm()));
    connect( ui->action_Exit, SIGNAL( triggered()), this, SLOT(close()));

 // Help
    connect( ui->actionHelp, SIGNAL( triggered()), this, SLOT(CmHelp()));
    connect( ui->actionHow_to, SIGNAL( triggered()), this, SLOT(CmHowto()));
    connect( ui->actionAbout, SIGNAL( triggered()), this, SLOT(CmHelpAbout()));
    connect( ui->actionAuthors, SIGNAL( triggered()), this, SLOT(CmHelpAuthors()));
    connect( ui->actionThanks, SIGNAL( triggered()), this, SLOT(CmHelpThanks()));
    connect( ui->actionLicense, SIGNAL( triggered()), this, SLOT(CmHelpLicense()));
    //connect( action_Help_2, SIGNAL( triggered()), this, SLOT(CmHelp2()));
    connect( ui->action_Preferences, SIGNAL( triggered()), this, SLOT(CmSettingth()));
    connect( ui->actionTutorial, SIGNAL( triggered()), this, SLOT(CmTutorial()));

// Record
    connect( ui->actionCreate_New , SIGNAL( triggered()), this, SLOT(CmCreate()));
    connect( ui->actionNew_Clone, SIGNAL( triggered()), this, SLOT(CmClone()));
    connect( ui->actionSave, SIGNAL( triggered()), this, SLOT(CmSave()));
    connect( ui->actionSave_As, SIGNAL( triggered()), this, SLOT(CmSaveAs()));
    connect( ui->actionDelete, SIGNAL( triggered()), this, SLOT(CmDelete()));
    connect( ui->actionNext, SIGNAL( triggered()), this, SLOT(CmNext()));
    connect( ui->actionPrevious, SIGNAL( triggered()), this, SLOT(CmPrevious()));
    connect( ui->actionUpdate_Test, SIGNAL( triggered()), this, SLOT(CmFilter()));


 // Record list
       connect( ui->actionBackup_to_JSON, SIGNAL( triggered()), this, SLOT(CmCopyList()));
       connect( ui->actionRestore_from_JSON, SIGNAL( triggered()), this, SLOT(CmRenameList()));
       connect( ui->actionBackup_to_csv, SIGNAL( triggered()), this, SLOT(CmTransferList()));
       connect( ui->actionRestore_from_csv, SIGNAL( triggered()), this, SLOT(CmDeleteList()));
       connect( ui->actionBackup_to_TXT, SIGNAL( triggered()), this, SLOT(CmKeysToTXT()));
       connect( ui->actionRestore_from_TXT, SIGNAL( triggered()), this, SLOT(CmExport()));
       connect( ui->actionBackup_to_YAML, SIGNAL( triggered()), this, SLOT(CmImport()));
       connect( ui->actionRestore_from_YAML, SIGNAL( triggered()), this, SLOT(CmBackupEJDB()));

  //Calc
    connect( ui->action_Run_test, SIGNAL( triggered()), this, SLOT(CmRunBCC()));
    connect( ui->action_Show_Results, SIGNAL( triggered()), this, SLOT(CmRunIPM()));


   pLine = new QLineEdit( ui->toolBar );
   pLine->setEnabled( true );
   pLine->setFocusPolicy( Qt::ClickFocus );
   pLine->setReadOnly( true );
   pLine->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   ui->toolBar->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   ui->toolBar->addWidget( pLine ); // setStretchableWidget( pLine );

}




