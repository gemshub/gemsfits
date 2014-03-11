#ifndef FITMAINWINDOW_H
#define FITMAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTableWidget>
#include "verror.h"

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

// Mode enums
enum { MDF_DATABASE=0, MDF_TASK=1 };

class FITMainWindow : public QMainWindow
{
    Q_OBJECT

    int currentMode;    ///< DataBase or Task mode

    string LocalDocDir; ///< Path to help directory

    void setActions();
    void closeEvent( QCloseEvent* );
    void resizeEvent( QResizeEvent * event );
    void showEvent( QShowEvent * event );

private slots:
    void moveToolBar( int pos, int index );

public slots:
    // Tasks
       void CmDBMode(){}
       void CmTaskMode(){}
       void CmNewProject(){}
       void CmSelectProject(){}
       void CmSelectGEMS(){}
    // Help
       void CmHelp();
       void CmHowto();
       void CmHelpAbout();
       void CmHelpAuthors();
       void CmHelpThanks();
       void CmHelpLicense();
       //void CmHelp2();
       void CmSettingth();
       void CmTutorial();
   // Record
       void CmCreate(){}
       void CmClone(){}
       void CmSave(){}
       void CmSaveAs(){}
       void CmDelete(){}
       void CmNext(){}
       void CmPrevious(){}
       //void CmFilter();
       void CmUpdateTest(){}
    // Record list
       void CmBackupJSON(){}
       void CmRestoreJSON(){}
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
    QToolBar *toolTasks;   ///< Tool bar to switch task/database mode
    QLineEdit* pLineTask;  ///< Current Task name
    QLineEdit* pLineGEMS;  ///< Current CSD GEMS3K file lst name
    TKeyTable* keyTable;   ///< Curent collection EJDB keys list
};

extern FITMainWindow* pFitImp;

#endif // FITMAINWINDOW_H
