#ifndef FITMAINWINDOW_H
#define FITMAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QLineEdit>
#include <QTableWidget>
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
    TFile  gemsLstFile; ///< Path to GEMS3K files
    TFile  fitTaskDir;  ///< Path to the database file

    auto_ptr<TNode> aNode;
    TNode* node() const
    {
      return aNode.get();
    }
    //TNode *node;
    bool contentsChanged; ///< Current record changed

    void setDefValues(int c, char** v);
    void setActions();
    void closeEJDB();
    void openEJDB();
    void loadNewProject();
    void resetMainWindow();
    bool MessageToSave();
    void RecSave( const string& recBsonText, const char* key=0 );

    void closeEvent( QCloseEvent* );
    void resizeEvent( QResizeEvent * event );
    void showEvent( QShowEvent * event );

private slots:
    void moveToolBar( int pos, int index );
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
       void CmFilter();
       void CmUpdateTest();
    // Record list
       void CmBackupJSON();
       void CmRestoreJSON();
       void CmBackupCSV(){}
       void CmRestoreCSV(){}
       void CmBackupTXT(){}
       void CmRestoreTXT(){}
       void CmBackupYAML(){}
       void CmRestoreYAML(){}
     //Calc
       void CmRunTest(){}
       void CmShowResults(){}


public:
    explicit FITMainWindow(int c, char** v, QWidget *parent = 0);
    ~FITMainWindow();

       const string& sysDir() const {
         return SysFITDir;
       }
       void setSysDir(const string& sysDir) {
               SysFITDir = sysDir;
           }
       const string& docDir() const {
         return LocalDocDir;
       }
       void setDocDir(const string& localDir) {
               LocalDocDir = localDir;
           }

    void GetHelp();
    void OpenHelp(const char* file, const char* item=0, int page =-1);


private:
    Ui::FITMainWindow *ui;
    QSettings *projectSettings; ///< Properties for current project

    QToolBar *toolTasks;   ///< Tool bar to switch task/database mode
    QLineEdit* pLineTask;  ///< Current Task name
    QLineEdit* pLineGEMS;  ///< Current CSD GEMS3K file lst name
    TKeyTable* keyTable;   ///< Curent collection EJDB keys list

    void setTableIComp();
    void setListPhase();
    void setStatusText( const string& text );
    void addLinetoStatus( const string& line );
    void defineModuleKeysList( int nRT );

};

extern FITMainWindow* pFitImp;

#endif // FITMAINWINDOW_H
