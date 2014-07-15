/********************************************************************************
** Form generated from reading UI file 'FitResultsWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FITRESULTSWINDOW_H
#define UI_FITRESULTSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FitResultsWindow
{
public:
    QAction *actionOpen_File;
    QAction *actionSave_File;
    QAction *actionClose;
    QAction *action_Insert;
    QAction *actionUpdate;
    QAction *actionDelete;
    QAction *actionExport;
    QAction *action_Copy;
    QAction *action_Paste;
    QAction *actionPlot_Results;
    QAction *actionAbout_Results_window;
    QAction *actionAbout_Graph_Dialog;
    QAction *actionAbout_GEMSFITS;
    QAction *actionPrint;
    QAction *actionAdd_Columns;
    QAction *actionDelete_Columns;
    QAction *actionMark_Columns_as_X;
    QAction *actionMark_Columns_as_Y;
    QAction *actionClear;
    QAction *action_Find;
    QAction *actionCalculator;
    QAction *actionSelect_column;
    QAction *actionSelect_all;
    QAction *actionC_ut;
    QAction *actionPaste_transposed;
    QAction *action_Find_from_Plot;
    QAction *actionAbout_gemsfits2_Code;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabsResults;
    QWidget *tabFitParams;
    QVBoxLayout *verticalLayout_4;
    QWidget *tabFitResults;
    QVBoxLayout *verticalLayout_6;
    QWidget *tabStatistic;
    QVBoxLayout *verticalLayout_2;
    QWidget *tabSensitivity;
    QVBoxLayout *verticalLayout_7;
    QWidget *tabQuantiles;
    QVBoxLayout *verticalLayout_8;
    QWidget *tabMcResults;
    QVBoxLayout *verticalLayout_5;
    QWidget *tabInverse;
    QVBoxLayout *verticalLayout_9;
    QWidget *tabLog;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *textEdit;
    QWidget *tabPlotData;
    QMenuBar *menubar;
    QMenu *menuFiles;
    QMenu *menuTools;
    QMenu *menuHelp;
    QMenu *menuEdit;
    QMenu *menuDatabase;
    QStatusBar *statusbar;
    QToolBar *toolBar;
    QToolBar *toolBarKey;

    void setupUi(QMainWindow *FitResultsWindow)
    {
        if (FitResultsWindow->objectName().isEmpty())
            FitResultsWindow->setObjectName(QStringLiteral("FitResultsWindow"));
        FitResultsWindow->setWindowModality(Qt::NonModal);
        FitResultsWindow->resize(800, 600);
        QIcon icon;
        icon.addFile(QStringLiteral(":/modules/Icons/FitSpaceModuleIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        FitResultsWindow->setWindowIcon(icon);
        actionOpen_File = new QAction(FitResultsWindow);
        actionOpen_File->setObjectName(QStringLiteral("actionOpen_File"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/menu/Icons/ShowFilesIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen_File->setIcon(icon1);
        actionSave_File = new QAction(FitResultsWindow);
        actionSave_File->setObjectName(QStringLiteral("actionSave_File"));
        actionClose = new QAction(FitResultsWindow);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        action_Insert = new QAction(FitResultsWindow);
        action_Insert->setObjectName(QStringLiteral("action_Insert"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/menu/Icons/NewCloneRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Insert->setIcon(icon2);
        actionUpdate = new QAction(FitResultsWindow);
        actionUpdate->setObjectName(QStringLiteral("actionUpdate"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/menu/Icons/SaveCurrentRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUpdate->setIcon(icon3);
        actionDelete = new QAction(FitResultsWindow);
        actionDelete->setObjectName(QStringLiteral("actionDelete"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/menu/Icons/DeleteCurrentRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete->setIcon(icon4);
        actionExport = new QAction(FitResultsWindow);
        actionExport->setObjectName(QStringLiteral("actionExport"));
        action_Copy = new QAction(FitResultsWindow);
        action_Copy->setObjectName(QStringLiteral("action_Copy"));
        action_Paste = new QAction(FitResultsWindow);
        action_Paste->setObjectName(QStringLiteral("action_Paste"));
        actionPlot_Results = new QAction(FitResultsWindow);
        actionPlot_Results->setObjectName(QStringLiteral("actionPlot_Results"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/modules/Icons/RTparmModuleIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPlot_Results->setIcon(icon5);
        actionAbout_Results_window = new QAction(FitResultsWindow);
        actionAbout_Results_window->setObjectName(QStringLiteral("actionAbout_Results_window"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/menu/Icons/ShowHelpWindowIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout_Results_window->setIcon(icon6);
        actionAbout_Graph_Dialog = new QAction(FitResultsWindow);
        actionAbout_Graph_Dialog->setObjectName(QStringLiteral("actionAbout_Graph_Dialog"));
        actionAbout_GEMSFITS = new QAction(FitResultsWindow);
        actionAbout_GEMSFITS->setObjectName(QStringLiteral("actionAbout_GEMSFITS"));
        actionPrint = new QAction(FitResultsWindow);
        actionPrint->setObjectName(QStringLiteral("actionPrint"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/menu/Icons/PrintDataToFileIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPrint->setIcon(icon7);
        actionAdd_Columns = new QAction(FitResultsWindow);
        actionAdd_Columns->setObjectName(QStringLiteral("actionAdd_Columns"));
        actionDelete_Columns = new QAction(FitResultsWindow);
        actionDelete_Columns->setObjectName(QStringLiteral("actionDelete_Columns"));
        actionMark_Columns_as_X = new QAction(FitResultsWindow);
        actionMark_Columns_as_X->setObjectName(QStringLiteral("actionMark_Columns_as_X"));
        actionMark_Columns_as_Y = new QAction(FitResultsWindow);
        actionMark_Columns_as_Y->setObjectName(QStringLiteral("actionMark_Columns_as_Y"));
        actionClear = new QAction(FitResultsWindow);
        actionClear->setObjectName(QStringLiteral("actionClear"));
        action_Find = new QAction(FitResultsWindow);
        action_Find->setObjectName(QStringLiteral("action_Find"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/menu/Icons/SelectDisplayRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Find->setIcon(icon8);
        actionCalculator = new QAction(FitResultsWindow);
        actionCalculator->setObjectName(QStringLiteral("actionCalculator"));
        actionSelect_column = new QAction(FitResultsWindow);
        actionSelect_column->setObjectName(QStringLiteral("actionSelect_column"));
        actionSelect_all = new QAction(FitResultsWindow);
        actionSelect_all->setObjectName(QStringLiteral("actionSelect_all"));
        actionC_ut = new QAction(FitResultsWindow);
        actionC_ut->setObjectName(QStringLiteral("actionC_ut"));
        actionPaste_transposed = new QAction(FitResultsWindow);
        actionPaste_transposed->setObjectName(QStringLiteral("actionPaste_transposed"));
        action_Find_from_Plot = new QAction(FitResultsWindow);
        action_Find_from_Plot->setObjectName(QStringLiteral("action_Find_from_Plot"));
        actionAbout_gemsfits2_Code = new QAction(FitResultsWindow);
        actionAbout_gemsfits2_Code->setObjectName(QStringLiteral("actionAbout_gemsfits2_Code"));
        centralwidget = new QWidget(FitResultsWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabsResults = new QTabWidget(centralwidget);
        tabsResults->setObjectName(QStringLiteral("tabsResults"));
        tabFitParams = new QWidget();
        tabFitParams->setObjectName(QStringLiteral("tabFitParams"));
        verticalLayout_4 = new QVBoxLayout(tabFitParams);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        tabsResults->addTab(tabFitParams, QString());
        tabFitResults = new QWidget();
        tabFitResults->setObjectName(QStringLiteral("tabFitResults"));
        verticalLayout_6 = new QVBoxLayout(tabFitResults);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        tabsResults->addTab(tabFitResults, QString());
        tabStatistic = new QWidget();
        tabStatistic->setObjectName(QStringLiteral("tabStatistic"));
        verticalLayout_2 = new QVBoxLayout(tabStatistic);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        tabsResults->addTab(tabStatistic, QString());
        tabSensitivity = new QWidget();
        tabSensitivity->setObjectName(QStringLiteral("tabSensitivity"));
        verticalLayout_7 = new QVBoxLayout(tabSensitivity);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        tabsResults->addTab(tabSensitivity, QString());
        tabQuantiles = new QWidget();
        tabQuantiles->setObjectName(QStringLiteral("tabQuantiles"));
        verticalLayout_8 = new QVBoxLayout(tabQuantiles);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        tabsResults->addTab(tabQuantiles, QString());
        tabMcResults = new QWidget();
        tabMcResults->setObjectName(QStringLiteral("tabMcResults"));
        verticalLayout_5 = new QVBoxLayout(tabMcResults);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        tabsResults->addTab(tabMcResults, QString());
        tabInverse = new QWidget();
        tabInverse->setObjectName(QStringLiteral("tabInverse"));
        verticalLayout_9 = new QVBoxLayout(tabInverse);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        tabsResults->addTab(tabInverse, QString());
        tabLog = new QWidget();
        tabLog->setObjectName(QStringLiteral("tabLog"));
        verticalLayout_3 = new QVBoxLayout(tabLog);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        textEdit = new QTextEdit(tabLog);
        textEdit->setObjectName(QStringLiteral("textEdit"));

        verticalLayout_3->addWidget(textEdit);

        tabsResults->addTab(tabLog, QString());
        tabPlotData = new QWidget();
        tabPlotData->setObjectName(QStringLiteral("tabPlotData"));
        tabsResults->addTab(tabPlotData, QString());

        verticalLayout->addWidget(tabsResults);

        FitResultsWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(FitResultsWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 20));
        menuFiles = new QMenu(menubar);
        menuFiles->setObjectName(QStringLiteral("menuFiles"));
        menuTools = new QMenu(menubar);
        menuTools->setObjectName(QStringLiteral("menuTools"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuDatabase = new QMenu(menubar);
        menuDatabase->setObjectName(QStringLiteral("menuDatabase"));
        FitResultsWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(FitResultsWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        FitResultsWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(FitResultsWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        FitResultsWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        toolBarKey = new QToolBar(FitResultsWindow);
        toolBarKey->setObjectName(QStringLiteral("toolBarKey"));
        FitResultsWindow->addToolBar(Qt::TopToolBarArea, toolBarKey);

        menubar->addAction(menuFiles->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuDatabase->menuAction());
        menubar->addAction(menuTools->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFiles->addAction(actionOpen_File);
        menuFiles->addAction(actionSave_File);
        menuFiles->addAction(actionPrint);
        menuFiles->addSeparator();
        menuFiles->addAction(actionClose);
        menuTools->addAction(actionPlot_Results);
        menuTools->addSeparator();
        menuTools->addAction(action_Find_from_Plot);
        menuHelp->addAction(actionAbout_Results_window);
        menuHelp->addAction(actionAbout_Graph_Dialog);
        menuHelp->addAction(actionAbout_GEMSFITS);
        menuHelp->addAction(actionAbout_gemsfits2_Code);
        menuEdit->addSeparator();
        menuEdit->addAction(actionCalculator);
        menuEdit->addAction(actionMark_Columns_as_X);
        menuEdit->addAction(actionMark_Columns_as_Y);
        menuEdit->addSeparator();
        menuEdit->addAction(actionSelect_column);
        menuEdit->addAction(actionSelect_all);
        menuEdit->addSeparator();
        menuEdit->addAction(actionC_ut);
        menuEdit->addAction(action_Copy);
        menuEdit->addAction(action_Paste);
        menuEdit->addAction(actionPaste_transposed);
        menuEdit->addAction(actionClear);
        menuDatabase->addAction(action_Insert);
        menuDatabase->addSeparator();
        menuDatabase->addAction(actionUpdate);
        menuDatabase->addAction(actionDelete);
        menuDatabase->addSeparator();
        menuDatabase->addAction(actionExport);
        toolBar->addAction(actionOpen_File);
        toolBar->addSeparator();
        toolBar->addAction(action_Find);
        toolBar->addAction(action_Insert);
        toolBar->addAction(actionUpdate);
        toolBar->addAction(actionDelete);
        toolBar->addSeparator();
        toolBar->addAction(actionPlot_Results);
        toolBar->addSeparator();
        toolBar->addAction(actionAbout_Results_window);

        retranslateUi(FitResultsWindow);

        tabsResults->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(FitResultsWindow);
    } // setupUi

    void retranslateUi(QMainWindow *FitResultsWindow)
    {
        FitResultsWindow->setWindowTitle(QApplication::translate("FitResultsWindow", "GEMSFITS Results Viewer", 0));
        actionOpen_File->setText(QApplication::translate("FitResultsWindow", "Load gemsfit2 output files...", 0));
        actionSave_File->setText(QApplication::translate("FitResultsWindow", "Save results table to csv file...", 0));
        actionClose->setText(QApplication::translate("FitResultsWindow", "Close results viewer window", 0));
        action_Insert->setText(QApplication::translate("FitResultsWindow", "&Save gemsfit2 results to record", 0));
        actionUpdate->setText(QApplication::translate("FitResultsWindow", "&Update record with saved results", 0));
        actionDelete->setText(QApplication::translate("FitResultsWindow", "&Delete record with saved results", 0));
        actionExport->setText(QApplication::translate("FitResultsWindow", "&Export record to JSON file...", 0));
        action_Copy->setText(QApplication::translate("FitResultsWindow", "&Copy", 0));
        action_Paste->setText(QApplication::translate("FitResultsWindow", "&Paste", 0));
        actionPlot_Results->setText(QApplication::translate("FitResultsWindow", "Plot parameter optimization results...", 0));
        actionAbout_Results_window->setText(QApplication::translate("FitResultsWindow", "About Results Viewer", 0));
        actionAbout_Graph_Dialog->setText(QApplication::translate("FitResultsWindow", "About Graph Dialog", 0));
        actionAbout_GEMSFITS->setText(QApplication::translate("FitResultsWindow", "About GEMSFITS", 0));
        actionPrint->setText(QApplication::translate("FitResultsWindow", "Print", 0));
        actionAdd_Columns->setText(QApplication::translate("FitResultsWindow", "Add Columns...", 0));
        actionDelete_Columns->setText(QApplication::translate("FitResultsWindow", "Delete Columns...", 0));
        actionMark_Columns_as_X->setText(QApplication::translate("FitResultsWindow", "Mark columns as &X (toggle) ... ", 0));
        actionMark_Columns_as_Y->setText(QApplication::translate("FitResultsWindow", "Mark columns as Y (toggle)...", 0));
        actionClear->setText(QApplication::translate("FitResultsWindow", "Clear", 0));
        actionClear->setShortcut(QApplication::translate("FitResultsWindow", "Ctrl+Del", 0));
        action_Find->setText(QApplication::translate("FitResultsWindow", "&Find...", 0));
        actionCalculator->setText(QApplication::translate("FitResultsWindow", "Calculator ...", 0));
        actionSelect_column->setText(QApplication::translate("FitResultsWindow", "Select co&lumn", 0));
        actionSelect_all->setText(QApplication::translate("FitResultsWindow", "Select &all columns", 0));
        actionC_ut->setText(QApplication::translate("FitResultsWindow", "C&ut", 0));
        actionPaste_transposed->setText(QApplication::translate("FitResultsWindow", "Paste &transposed", 0));
        action_Find_from_Plot->setText(QApplication::translate("FitResultsWindow", "&Find a sample on the plot fragment...", 0));
        actionAbout_gemsfits2_Code->setText(QApplication::translate("FitResultsWindow", "About gemsfits2 code", 0));
        tabsResults->setTabText(tabsResults->indexOf(tabFitParams), QApplication::translate("FitResultsWindow", "Fitted Parameters", 0));
        tabsResults->setTabText(tabsResults->indexOf(tabFitResults), QApplication::translate("FitResultsWindow", "Fit for Samples", 0));
        tabsResults->setTabText(tabsResults->indexOf(tabStatistic), QApplication::translate("FitResultsWindow", "Sum. Statistics", 0));
        tabsResults->setTabText(tabsResults->indexOf(tabSensitivity), QApplication::translate("FitResultsWindow", "Sensitivity Data", 0));
        tabsResults->setTabText(tabsResults->indexOf(tabQuantiles), QApplication::translate("FitResultsWindow", "Quantile Resid.", 0));
        tabsResults->setTabText(tabsResults->indexOf(tabMcResults), QApplication::translate("FitResultsWindow", "MC Results", 0));
        tabsResults->setTabText(tabsResults->indexOf(tabInverse), QApplication::translate("FitResultsWindow", "Inv. Mod. Results", 0));
        tabsResults->setTabText(tabsResults->indexOf(tabLog), QApplication::translate("FitResultsWindow", "gemsfit2 log", 0));
        tabsResults->setTabText(tabsResults->indexOf(tabPlotData), QApplication::translate("FitResultsWindow", "Plot Setup", 0));
        menuFiles->setTitle(QApplication::translate("FitResultsWindow", "Files", 0));
        menuTools->setTitle(QApplication::translate("FitResultsWindow", "Tools", 0));
        menuHelp->setTitle(QApplication::translate("FitResultsWindow", "Help", 0));
        menuEdit->setTitle(QApplication::translate("FitResultsWindow", "Edit", 0));
        menuDatabase->setTitle(QApplication::translate("FitResultsWindow", "Database", 0));
        toolBar->setWindowTitle(QApplication::translate("FitResultsWindow", "toolBar", 0));
        toolBarKey->setWindowTitle(QApplication::translate("FitResultsWindow", "toolBar_2", 0));
    } // retranslateUi

};

namespace Ui {
    class FitResultsWindow: public Ui_FitResultsWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FITRESULTSWINDOW_H
