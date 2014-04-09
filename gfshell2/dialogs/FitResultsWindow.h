//-------------------------------------------------------------------
// $Id: FitResultsWindow.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of GEMSFITS GUI Results Window
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

#ifndef FITRESULTSWINDOW_H
#define FITRESULTSWINDOW_H

#include <QMainWindow>
#include "fmodel.h"
#include "GraphDialog.h"


namespace Ui {
class FitResultsWindow;
}

class FitResultsWindow : public QMainWindow
{
    Q_OBJECT

    void setActions();
    void editFiledsFromFile( const QString& dir );
    void editFiledsToFile( const QString& dir );
    void readBsonRecord();

    void closeEvent(QCloseEvent* ev);

public slots:
    void CmOpenFile( const QString& dir_="" );
    void CmSaveFile();
    void CmSaveBsonRecord();
    void CmDeleteRecord();
    void CmBackupJSON();

    void CmAboutGEMSFITS();
    void CmAbout_Graph_Dialog();
    void CmAbout_Results_window();

    void CmPlotTable();
    void CmPrintTable();
    void CmFindFromPlot();

   void CmCalc();
   void SelectColumn();
   void SelectAll();
   void CutData();
   void ClearData();
   void CopyData();
   void PasteData();
   void PasteTransposedData();
   void ToggleX();
   void ToggleY();


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

    //QSet< GraphDialog*> graphList; /// internal list of opened graphics
};

#endif // FITRESULTSWINDOW_H
