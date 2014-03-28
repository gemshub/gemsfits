#ifndef FITRESULTSWINDOW_H
#define FITRESULTSWINDOW_H

#include <QMainWindow>
#include "fmodel.h"

namespace Ui {
class FitResultsWindow;
}

class FitResultsWindow : public QMainWindow
{
    Q_OBJECT

    void setActions();

    void closeEvent(QCloseEvent* ev);

public slots:
    void CmOpenFile();
    void CmSaveFile();

public:
    explicit FitResultsWindow(QWidget *parent = 0);
    ~FitResultsWindow();

    static FitResultsWindow* pDia;

private:
    Ui::FitResultsWindow *ui;
    TMatrixModel *modelFitResults;
    TMatrixTable *tableFitResults;

};

#endif // FITRESULTSWINDOW_H
