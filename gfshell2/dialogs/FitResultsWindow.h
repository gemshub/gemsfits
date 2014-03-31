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
    void editFiledsFromFile( const QString& dir );
    void editFiledsToFile( const QString& dir );

    void closeEvent(QCloseEvent* ev);

public slots:
    void CmOpenFile( const QString& dir_="" );
    void CmSaveFile();

public:
    static FitResultsWindow* pDia;

    explicit FitResultsWindow(QWidget *parent = 0);
    ~FitResultsWindow();

    void ShowResults( const string& key );
    void ShowResults( const string& key, const QString& dir );

private:
    Ui::FitResultsWindow *ui;

    QLineEdit* pLineTask;  ///< Current Task key

    TMatrixModel *modelFitResults;  // model reading fit-results.csv file
    TMatrixTable *tableFitResults;

    TMatrixModel *modelFitParams;  // model reading fit-params.csv file
    TMatrixTable *tableFitParams;

    TMatrixModel *modelMCResults;  // model reading mc-results.csv file
    TMatrixTable *tableMCResults;

    TMatrixModel *modelSensitivity; // model reading meas-data-sensitivity.csv file
    TMatrixTable *tableSensitivity;

    TMatrixModel *modelQQplot;    // model reading qq-plot-data.csv file
    TMatrixTable *tableQQplot;

    TMatrixModel *modelFitInverse;   // model reading fit-inverse-results.csv file
    TMatrixTable *tableFitInverse;

    QString fFitStatistic;   // name file  FIT_STATISTIC
    QString fFitLogfile;  // name file  FIT_LOGFILE

};

#endif // FITRESULTSWINDOW_H
