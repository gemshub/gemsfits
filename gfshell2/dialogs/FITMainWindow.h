#ifndef FITMAINWINDOW_H
#define FITMAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include "verror.h"

namespace Ui {
class FITMainWindow;
}

class FITMainWindow : public QMainWindow
{
    Q_OBJECT

    string LocalDocDir;

    void setActions();

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
    void OpenHelp(const char* file, const char* item1, int page );



private:
    Ui::FITMainWindow *ui;
    QToolBar *toolTasks;
    QLineEdit* pLine;
};

extern FITMainWindow* pFitImp;

#endif // FITMAINWINDOW_H
