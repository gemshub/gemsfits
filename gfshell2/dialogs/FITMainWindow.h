//-------------------------------------------------------------------
// $Id: FITMainWindow.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of GEMSFITS GUI Main Window
//
// Copyright (C) 2014  S.V.Dmytriyeva, D.A.Kulik
// Uses Qwt (http://qwt.sourceforge.net), EJDB (http://ejdb.org),
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

#ifndef FITMAINWINDOW_H
#define FITMAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QLineEdit>
#include <QTableWidget>
#include <QProcess>
#include "verror.h"
#include "f_ejdb_file.h"
#include "node.h"

namespace Ui {
class FITMainWindow;
}

class TKeyTable: public QTableWidget
{
        Q_OBJECT

       void keyPressEvent(QKeyEvent* e);

  public:
       TKeyTable( QWidget * parent  ):
           QTableWidget(parent)
       {}
};

class FITMainWindow : public QMainWindow
{
    Q_OBJECT

    int currentMode;    ///< DataBase or Task mode

    string SysFITDir;   ///< Path to resources directory
    string LocalDocDir; ///< Path to help directory
    string UserDir;     ///< Path to User directory
    bool useComments;   ///< Write TXT files with comments
    QString ExpTemplate; ///< Current template for experiments record
    QString SrchTemplate; ///< Current template for experiments search

    TFile  gemsLstFile; ///< Path to GEMS3K files
    TFile  fitTaskDir;  ///< Path to the database file

    QFont	axisLabelFont; // used in graphics

    string lastCalcRecordKey;  ///< Last calculated record key
    auto_ptr<TNode> aNode;
    TNode* node() const
    {
      return aNode.get();
    }
    //TNode *node;
    bool contentsChanged; ///< Current record changed

    void setDefValues(int c, char** v);
    void getDataFromPreferences();
    void setActions();
    void closeEJDB();
    void openEJDB();
    void loadNewProject();
    void resetMainWindow();
    bool MessageToSave();
    void RecSave( const string& recBsonText, const char* key=0 );
    void RecDelete( const char* key );
    void changeEditeRecord(const string& tagname, const string& newValue);
    bool runProcess( const QStringList& cParameters, const QString& workDir );
    void selectGEMS( const string& fname );
    string makeSystemFileName(const string& path );
    bool createTaskTemplate();
    void readTXT( TFile& inFile );
    string getRecordKey( int row );
    void defineModuleKeysList( string& samplelist );


    void closeEvent( QCloseEvent* );
    void resizeEvent( QResizeEvent * event );
    void showEvent( QShowEvent * event );

private slots:
    void moveToolBar( int pos, int index );
    void showProcessMesage( );
    void runFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void changeKeyList();
    void recEdited()
    {
        contentsChanged = true;
    }

public slots:
     void openRecordKey(  int row, int column  );

    // Tasks
       void CmDBMode();
       void CmTaskMode();
       void CmConfigProject();
       void CmNewProject();
       void CmSelectProject( const string& fname_="" );
       void CmSelectGEMS( const string& fname_="" );
    // Help
       void CmHelp();
       void CmHelpAbout();
       void CmHelpAuthors();
       void CmHelpLicense();
       //void CmHelp2();
       void CmSettingth();
   // Record
       void CmShow( const string& reckey="" );
       void CmCreate();
       void CmUpdate();
       void CmInsert();
       void CmDelete();
       void CmNext();
       void CmPrevious();
       //void CmFilter();
       void CmUpdateTest();

       void CmSearch();
       void CmResetSearch();
       void CmSaveSearch();
       void CmLoadSearch();
       void CmInsertSearch();

       // Record list
       void CmBackupJSON();
       void CmRestoreJSON();
       void CmBackupCSV(){}
       void CmRestoreCSV();
       void CmBackupTXT();
       void CmRestoreTXT();
       void CmBackupYAML(){}
       void CmRestoreYAML(){}
       void CmDeleteList();
     //Calc
       void CmRunTest();
       void CmShowCalcResults();
       void CmShowFitResults();
       void CmCancelGemsfit();

public:
    explicit FITMainWindow(int c, char** v, QWidget *parent = 0);
    ~FITMainWindow();

       const string& sysDir() const {
         return SysFITDir;
       }
       const string& userDir() const {
         return UserDir;
       }
       const string& docDir() const {
         return LocalDocDir;
       }
       const QFont& getAxisLabelFont() const
           {  return axisLabelFont; }
       void setAxisLabelFont(const QFont& newAxisLabelFont)
           {  axisLabelFont = newAxisLabelFont;  }

    QProcess*  helpProcess;
    void GetHelp();
    void OpenHelp(const char* file, const char* item=0, int page =-1);
    void OpenResults(  const string& key, const QString& dir = "");

private:
    Ui::FITMainWindow *ui;
    QSettings *projectSettings; ///< Properties for current project
    QSettings *mainSettings; ///< Properties for gemsfit-gui program

    QToolBar *toolTasks;   ///< Tool bar to switch task/database mode
    QLineEdit* pLineTask;  ///< Current Task name
    QLineEdit* pLineGEMS;  ///< Current CSD GEMS3K file lst name
    TKeyTable* keyTable;   ///< Curent collection EJDB keys list
    QProcess*  fitProcess;


    void setTableIComp();
    void setListPhase();
    void setStatusText( const string& text );
    void addLinetoStatus( const string& line );
    int defineModuleKeysList( int nRT );

};

extern FITMainWindow* pFitImp;

extern const char *GF_INDEX_HTML; // = "gemsfit-index";
extern const char *GF_PREFS_HTML; // = "gemsfit-prefs";
extern const char *GF_EXPDB_HTML; // = "howto-csvtoejdb";
extern const char *GF_TASKDB_HTML; //= "gemsfit-input-file";
extern const char *GF_ABOUT_HTML; // = "gemsfit-about";
extern const char *GF_AUTHORS_HTML; // = "gemsfit-about#AUTHORS";
extern const char *GF_LICENSE_HTML; // = "gemsfit-about#LICENSE";
extern const char *DBM; // = "DBM";

#endif // FITMAINWINDOW_H
