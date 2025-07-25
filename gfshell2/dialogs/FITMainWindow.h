//-------------------------------------------------------------------
// $Id: FITMainWindow.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of GEMSFITS GUI Main Window
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

#ifndef FITMAINWINDOW_H
#define FITMAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QLineEdit>
#include <QTableWidget>
#include <QProcess>
#include "f_file.h"
#include <GEMS3K/node.h>

namespace Ui {
class FITMainWindow;
}

class QTextEdit;

namespace jsonui17 {
class JsonView;
}

extern const char *HELP_SRC_DIR;
extern const char *DATA_TEMPLATES;
extern const char *SEARCH_TEMPLATES;


class TKeyTable: public QTableWidget
{
    Q_OBJECT

    void keyPressEvent(QKeyEvent* e);

public:
    TKeyTable(QWidget * parent):
        QTableWidget(parent)
    {}
};

class FITMainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::FITMainWindow *ui;

    int currentMode;    ///< DataBase or Task mode

    std::string SysFITDir;   ///< Path to resources directory
    std::string UserDir;     ///< Path to User directory
    std::string WorkDir;     ///< Path to last used directory
    std::string LocalDocDir; ///< Path to help directory
    QString GemsfitApplication; ///< Path to gemsfit application

    bool KeysLength;   ///< Write TXT files with comments
    bool JsonDataShow;   ///< Write edited data in Json format
    bool EditorDataShow;   ///< Write edited data in Json editor
    QString ExpTemplate; ///< Current template for experiments record
    QString SrchTemplate; ///< Current template for experiments search

    common::TFile  gemsLstFile; ///< Path to GEMS3K files
    common::TFile  fitTaskDir;  ///< Path to the database file

    QFont	axisLabelFont; // used in graphics

    std::string lastCalcRecordKey;  ///< Last calculated record key
    std::shared_ptr<TNode> aNode;
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
    void RecSave(const std::string& recBsonText, const std::string& key="");
    void RecDelete(const std::string& key);
    bool runProcess(const QStringList& cParameters, const QString& workDir);
    void selectGEMS(const std::string& fname_);
    std::string makeSystemFileName(const std::string& path);
    bool createTaskTemplate();
    void readTXT(common::TFile& inFile);
    std::string getRecordKey(int row);
    void defineModuleKeysList( std::string& samplelist);


    void closeEvent(QCloseEvent*);
    void resizeEvent(QResizeEvent* event);
    void showEvent(QShowEvent* event);

private slots:
    void moveToolBar(int pos, int index);
    void showProcessMesage();
    void runFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void changeKeyList();
    void objectChanged()
    {
        contentsChanged = true;
    }
    void recEdited(bool edited)
    {
        if(edited)
            contentsChanged = true;
    }

public slots:
    void openRecordKey(int row, int column);

    // Tasks
    void CmDBMode();
    void CmTaskMode();
    void CmConfigProject();
    void CmNewProject();
    void CmSelectProject(const std::string& fname_="");
    void CmSelectGEMS(const std::string& fname_="");
    // Help
    void CmHelp();
    void CmHelpAbout();
    void CmHelpAuthors();
    void CmHelpLicense();
    //void CmHelp2();
    void CmSettingth();
    // Record
    void CmShow(const std::string& reckey="");
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
    void CmKeysToTXT();
    void CmBackupJSON();
    void CmRestoreJSON();
    void CmBackupCSV(){}
    void CmRestoreCSV();
    void CmBackupTXT();
    void CmRestoreTXT();
    void CmBackupYAML();
    void CmRestoreYAML();
    void CmDeleteList();
    void CmTPpairsCSV();
    //Calc
    void CmRunTest();
    void CmShowCalcResults();
    void CmShowFitResults();
    void CmCancelGemsfit();
    //Find
    void actionFind();
    void actionFindNext();
    void actionFindPrevious();
    void actionZoomIn();
    void actionZoomOut();

public:
    explicit FITMainWindow(int c, char** v, QWidget *parent = 0);
    ~FITMainWindow();

    const std::string& sysDir() const {
        return SysFITDir;
    }
    const std::string& userDir() const {
        return UserDir;
    }
    const std::string& docDir() const {
        return LocalDocDir;
    }
    const QString& fitApp() const {
        return GemsfitApplication;
    }
    const QFont& getAxisLabelFont() const
    {  return axisLabelFont; }
    void setAxisLabelFont(const QFont& newAxisLabelFont)
    {  axisLabelFont = newAxisLabelFont;  }

    QProcess*  helpProcess;
    void GetHelp();
    void OpenHelp(const char* file, const char* item=0, int page =-1);
    void OpenResults(const std::string& key, const QString& dir = "");

private:
    QSettings *projectSettings; ///< Properties for current project
    QSettings *mainSettings; ///< Properties for gemsfit-gui program

    QToolBar *toolTasks;   ///< Tool bar to switch task/database mode
    QLineEdit* pLineTask;  ///< Current Task name
    QLineEdit* pLineGEMS;  ///< Current CSD GEMS3K file lst name
    TKeyTable* keyTable;   ///< Curent collection EJDB keys list
    QProcess*  fitProcess;
    QLineEdit *findLine = nullptr;
    /// Tree view editor widget
    QScopedPointer<jsonui17::JsonView>  json_tree;
    QScopedPointer<QTextEdit> recordEdit;

    void setTableIComp();
    void setListPhase();
    void setStatusText( const std::string& text );
    void addLinetoStatus( const std::string& line );
    int defineModuleKeysList( int nRT );

    void set_record_edit(const std::string& json_text);
    std::string get_record_edit();

};

extern FITMainWindow* pFitImp;

extern const char *GF_INDEX_HTML; // = "gemsfit-index";
extern const char *GF_PREFS_HTML; // = "gemsfit-prefs";
extern const char *GF_SABOUT_HTML; // = "gfshell-about";
extern const char *GF_EXPDB_HTML; // = "howto-csvtoejdb";
extern const char *GF_TASKDB_HTML; //= "gemsfit-input-file";
extern const char *GF_ABOUT_HTML; // = "gemsfit-about";
extern const char *GF_AUTHORS_HTML; // = "gemsfit-about#AUTHORS";
extern const char *GF_LICENSE_HTML; // = "gemsfit-about#LICENSE";
extern const char *DBM; // = "DBM";

#endif // FITMAINWINDOW_H
