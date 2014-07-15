/********************************************************************************
** Form generated from reading UI file 'FITMainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FITMAINWINDOW_H
#define UI_FITMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FITMainWindow
{
public:
    QAction *action_DataBase_mode;
    QAction *action_Task_Mode;
    QAction *action_New_Project;
    QAction *action_Select_project;
    QAction *actionSelect_GEMS;
    QAction *action_Exit;
    QAction *actionCreate_New;
    QAction *action_Find;
    QAction *action_Update;
    QAction *action_Insert;
    QAction *actionDelete;
    QAction *actionUpdate_Test;
    QAction *actionBackup_to_JSON;
    QAction *actionRestore_from_JSON;
    QAction *actionBackup_to_csv;
    QAction *actionRestore_from_csv;
    QAction *actionBackup_to_TXT;
    QAction *actionRestore_from_TXT;
    QAction *actionBackup_to_YAML;
    QAction *actionRestore_from_YAML;
    QAction *action_Run_test;
    QAction *action_Show_Results;
    QAction *action_Preferences;
    QAction *actionHelp;
    QAction *actionAbout;
    QAction *actionAuthors;
    QAction *actionLicense;
    QAction *actionNext;
    QAction *actionPrevious;
    QAction *action_Config_Project;
    QAction *actionOverwrite;
    QAction *action_Delete_multiple_data;
    QAction *actionFits_View_Mode;
    QAction *actionSave_Results_to_Database;
    QAction *actionReset_Search;
    QAction *actionSave_Search;
    QAction *actionLoad_Search;
    QAction *actionCancel_gemsfit2_run;
    QAction *actionInsert_search_results_into_task_definition;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splV;
    QWidget *widgetList;
    QVBoxLayout *verticalLayout_2;
    QSplitter *splKey;
    QWidget *widgetKeys;
    QGridLayout *gridLayout;
    QToolButton *filterButton;
    QLineEdit *filterEdit;
    QWidget *widgetQuery;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_5;
    QTextEdit *queryEdit;
    QWidget *widgetRecord;
    QVBoxLayout *verticalLayout_6;
    QSplitter *splRecord;
    QWidget *widget;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_3;
    QTextEdit *recordEdit;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_8;
    QLabel *label_4;
    QTextEdit *statusEdit;
    QWidget *widgetGEMS;
    QVBoxLayout *verticalLayout;
    QSplitter *splGEMS;
    QWidget *widgetIC;
    QVBoxLayout *verticalLayout_4;
    QLabel *label;
    QTableWidget *tableIComp;
    QWidget *widgetPhase;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_2;
    QTreeWidget *listPhases;
    QMenuBar *menubar;
    QMenu *menuTask;
    QMenu *menuRecord;
    QMenu *menuRecod_List;
    QMenu *menu_Calc;
    QMenu *menu_Help;
    QStatusBar *statusbar;
    QToolBar *toolBar_2;
    QToolBar *toolBarTask;
    QToolBar *toolBarMenu;
    QToolBar *toolBarGems;

    void setupUi(QMainWindow *FITMainWindow)
    {
        if (FITMainWindow->objectName().isEmpty())
            FITMainWindow->setObjectName(QStringLiteral("FITMainWindow"));
        FITMainWindow->setWindowModality(Qt::NonModal);
        FITMainWindow->resize(931, 670);
        QIcon icon;
        icon.addFile(QStringLiteral(":/modules/Icons/FitSpaceModuleIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        FITMainWindow->setWindowIcon(icon);
        action_DataBase_mode = new QAction(FITMainWindow);
        action_DataBase_mode->setObjectName(QStringLiteral("action_DataBase_mode"));
        action_DataBase_mode->setCheckable(true);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/menu/Icons/DatabaseModeIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_DataBase_mode->setIcon(icon1);
        action_Task_Mode = new QAction(FITMainWindow);
        action_Task_Mode->setObjectName(QStringLiteral("action_Task_Mode"));
        action_Task_Mode->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/modules/Icons/DualThModuleIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Task_Mode->setIcon(icon2);
        action_New_Project = new QAction(FITMainWindow);
        action_New_Project->setObjectName(QStringLiteral("action_New_Project"));
        action_Select_project = new QAction(FITMainWindow);
        action_Select_project->setObjectName(QStringLiteral("action_Select_project"));
        actionSelect_GEMS = new QAction(FITMainWindow);
        actionSelect_GEMS->setObjectName(QStringLiteral("actionSelect_GEMS"));
        action_Exit = new QAction(FITMainWindow);
        action_Exit->setObjectName(QStringLiteral("action_Exit"));
        actionCreate_New = new QAction(FITMainWindow);
        actionCreate_New->setObjectName(QStringLiteral("actionCreate_New"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/menu/Icons/NewCreateRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCreate_New->setIcon(icon3);
        action_Find = new QAction(FITMainWindow);
        action_Find->setObjectName(QStringLiteral("action_Find"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/menu/Icons/SelectDisplayRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Find->setIcon(icon4);
        action_Update = new QAction(FITMainWindow);
        action_Update->setObjectName(QStringLiteral("action_Update"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/menu/Icons/SaveCurrentRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Update->setIcon(icon5);
        action_Insert = new QAction(FITMainWindow);
        action_Insert->setObjectName(QStringLiteral("action_Insert"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/menu/Icons/NewCloneRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Insert->setIcon(icon6);
        actionDelete = new QAction(FITMainWindow);
        actionDelete->setObjectName(QStringLiteral("actionDelete"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/menu/Icons/DeleteCurrentRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete->setIcon(icon7);
        actionUpdate_Test = new QAction(FITMainWindow);
        actionUpdate_Test->setObjectName(QStringLiteral("actionUpdate_Test"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/menu/Icons/CalculateBCCIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUpdate_Test->setIcon(icon8);
        actionBackup_to_JSON = new QAction(FITMainWindow);
        actionBackup_to_JSON->setObjectName(QStringLiteral("actionBackup_to_JSON"));
        actionRestore_from_JSON = new QAction(FITMainWindow);
        actionRestore_from_JSON->setObjectName(QStringLiteral("actionRestore_from_JSON"));
        actionBackup_to_csv = new QAction(FITMainWindow);
        actionBackup_to_csv->setObjectName(QStringLiteral("actionBackup_to_csv"));
        actionRestore_from_csv = new QAction(FITMainWindow);
        actionRestore_from_csv->setObjectName(QStringLiteral("actionRestore_from_csv"));
        actionBackup_to_TXT = new QAction(FITMainWindow);
        actionBackup_to_TXT->setObjectName(QStringLiteral("actionBackup_to_TXT"));
        actionRestore_from_TXT = new QAction(FITMainWindow);
        actionRestore_from_TXT->setObjectName(QStringLiteral("actionRestore_from_TXT"));
        actionBackup_to_YAML = new QAction(FITMainWindow);
        actionBackup_to_YAML->setObjectName(QStringLiteral("actionBackup_to_YAML"));
        actionRestore_from_YAML = new QAction(FITMainWindow);
        actionRestore_from_YAML->setObjectName(QStringLiteral("actionRestore_from_YAML"));
        action_Run_test = new QAction(FITMainWindow);
        action_Run_test->setObjectName(QStringLiteral("action_Run_test"));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/menu/Icons/CalculateRecordDataIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Run_test->setIcon(icon9);
        action_Show_Results = new QAction(FITMainWindow);
        action_Show_Results->setObjectName(QStringLiteral("action_Show_Results"));
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/menu/Icons/ShowGEMResultsIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Show_Results->setIcon(icon10);
        action_Preferences = new QAction(FITMainWindow);
        action_Preferences->setObjectName(QStringLiteral("action_Preferences"));
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/menu/Icons/RemakeRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Preferences->setIcon(icon11);
        actionHelp = new QAction(FITMainWindow);
        actionHelp->setObjectName(QStringLiteral("actionHelp"));
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/menu/Icons/ShowHelpWindowIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionHelp->setIcon(icon12);
        actionAbout = new QAction(FITMainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionAuthors = new QAction(FITMainWindow);
        actionAuthors->setObjectName(QStringLiteral("actionAuthors"));
        actionLicense = new QAction(FITMainWindow);
        actionLicense->setObjectName(QStringLiteral("actionLicense"));
        actionNext = new QAction(FITMainWindow);
        actionNext->setObjectName(QStringLiteral("actionNext"));
        QIcon icon13;
        icon13.addFile(QStringLiteral(":/menu/Icons/DisplayNextRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNext->setIcon(icon13);
        actionPrevious = new QAction(FITMainWindow);
        actionPrevious->setObjectName(QStringLiteral("actionPrevious"));
        QIcon icon14;
        icon14.addFile(QStringLiteral(":/menu/Icons/DisplayPrevRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPrevious->setIcon(icon14);
        action_Config_Project = new QAction(FITMainWindow);
        action_Config_Project->setObjectName(QStringLiteral("action_Config_Project"));
        QIcon icon15;
        icon15.addFile(QStringLiteral(":/modules/Icons/ModelingProjectIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Config_Project->setIcon(icon15);
        actionOverwrite = new QAction(FITMainWindow);
        actionOverwrite->setObjectName(QStringLiteral("actionOverwrite"));
        actionOverwrite->setCheckable(true);
        action_Delete_multiple_data = new QAction(FITMainWindow);
        action_Delete_multiple_data->setObjectName(QStringLiteral("action_Delete_multiple_data"));
        actionFits_View_Mode = new QAction(FITMainWindow);
        actionFits_View_Mode->setObjectName(QStringLiteral("actionFits_View_Mode"));
        actionFits_View_Mode->setIcon(icon);
        actionSave_Results_to_Database = new QAction(FITMainWindow);
        actionSave_Results_to_Database->setObjectName(QStringLiteral("actionSave_Results_to_Database"));
        actionSave_Results_to_Database->setIcon(icon1);
        actionReset_Search = new QAction(FITMainWindow);
        actionReset_Search->setObjectName(QStringLiteral("actionReset_Search"));
        actionSave_Search = new QAction(FITMainWindow);
        actionSave_Search->setObjectName(QStringLiteral("actionSave_Search"));
        actionLoad_Search = new QAction(FITMainWindow);
        actionLoad_Search->setObjectName(QStringLiteral("actionLoad_Search"));
        actionCancel_gemsfit2_run = new QAction(FITMainWindow);
        actionCancel_gemsfit2_run->setObjectName(QStringLiteral("actionCancel_gemsfit2_run"));
        actionInsert_search_results_into_task_definition = new QAction(FITMainWindow);
        actionInsert_search_results_into_task_definition->setObjectName(QStringLiteral("actionInsert_search_results_into_task_definition"));
        centralwidget = new QWidget(FITMainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        splV = new QSplitter(centralwidget);
        splV->setObjectName(QStringLiteral("splV"));
        splV->setOrientation(Qt::Horizontal);
        splV->setHandleWidth(1);
        splV->setChildrenCollapsible(false);
        widgetList = new QWidget(splV);
        widgetList->setObjectName(QStringLiteral("widgetList"));
        verticalLayout_2 = new QVBoxLayout(widgetList);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        splKey = new QSplitter(widgetList);
        splKey->setObjectName(QStringLiteral("splKey"));
        splKey->setOrientation(Qt::Vertical);
        splKey->setChildrenCollapsible(false);
        widgetKeys = new QWidget(splKey);
        widgetKeys->setObjectName(QStringLiteral("widgetKeys"));
        gridLayout = new QGridLayout(widgetKeys);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        filterButton = new QToolButton(widgetKeys);
        filterButton->setObjectName(QStringLiteral("filterButton"));
        filterButton->setIcon(icon4);

        gridLayout->addWidget(filterButton, 0, 0, 1, 1);

        filterEdit = new QLineEdit(widgetKeys);
        filterEdit->setObjectName(QStringLiteral("filterEdit"));

        gridLayout->addWidget(filterEdit, 0, 1, 1, 1);

        splKey->addWidget(widgetKeys);
        widgetQuery = new QWidget(splKey);
        widgetQuery->setObjectName(QStringLiteral("widgetQuery"));
        verticalLayout_3 = new QVBoxLayout(widgetQuery);
        verticalLayout_3->setSpacing(5);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 3, 0, 0);
        label_5 = new QLabel(widgetQuery);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout_3->addWidget(label_5);

        queryEdit = new QTextEdit(widgetQuery);
        queryEdit->setObjectName(QStringLiteral("queryEdit"));

        verticalLayout_3->addWidget(queryEdit);

        splKey->addWidget(widgetQuery);

        verticalLayout_2->addWidget(splKey);

        splV->addWidget(widgetList);
        widgetRecord = new QWidget(splV);
        widgetRecord->setObjectName(QStringLiteral("widgetRecord"));
        verticalLayout_6 = new QVBoxLayout(widgetRecord);
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        splRecord = new QSplitter(widgetRecord);
        splRecord->setObjectName(QStringLiteral("splRecord"));
        splRecord->setOrientation(Qt::Vertical);
        widget = new QWidget(splRecord);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout_7 = new QVBoxLayout(widget);
        verticalLayout_7->setSpacing(5);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 3, 0, 0);
        label_3 = new QLabel(widget);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout_7->addWidget(label_3);

        recordEdit = new QTextEdit(widget);
        recordEdit->setObjectName(QStringLiteral("recordEdit"));

        verticalLayout_7->addWidget(recordEdit);

        splRecord->addWidget(widget);
        widget_2 = new QWidget(splRecord);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        verticalLayout_8 = new QVBoxLayout(widget_2);
        verticalLayout_8->setSpacing(5);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(0, 3, 0, 0);
        label_4 = new QLabel(widget_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout_8->addWidget(label_4);

        statusEdit = new QTextEdit(widget_2);
        statusEdit->setObjectName(QStringLiteral("statusEdit"));
        statusEdit->setReadOnly(true);

        verticalLayout_8->addWidget(statusEdit);

        splRecord->addWidget(widget_2);

        verticalLayout_6->addWidget(splRecord);

        splV->addWidget(widgetRecord);
        widgetGEMS = new QWidget(splV);
        widgetGEMS->setObjectName(QStringLiteral("widgetGEMS"));
        verticalLayout = new QVBoxLayout(widgetGEMS);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        splGEMS = new QSplitter(widgetGEMS);
        splGEMS->setObjectName(QStringLiteral("splGEMS"));
        splGEMS->setOrientation(Qt::Vertical);
        splGEMS->setChildrenCollapsible(false);
        widgetIC = new QWidget(splGEMS);
        widgetIC->setObjectName(QStringLiteral("widgetIC"));
        verticalLayout_4 = new QVBoxLayout(widgetIC);
        verticalLayout_4->setSpacing(5);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 3, 0, 0);
        label = new QLabel(widgetIC);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_4->addWidget(label);

        tableIComp = new QTableWidget(widgetIC);
        if (tableIComp->columnCount() < 5)
            tableIComp->setColumnCount(5);
        if (tableIComp->rowCount() < 2)
            tableIComp->setRowCount(2);
        tableIComp->setObjectName(QStringLiteral("tableIComp"));
        tableIComp->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        tableIComp->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableIComp->setDragEnabled(true);
        tableIComp->setDefaultDropAction(Qt::CopyAction);
        tableIComp->setSelectionMode(QAbstractItemView::SingleSelection);
        tableIComp->setRowCount(2);
        tableIComp->setColumnCount(5);
        tableIComp->horizontalHeader()->setVisible(false);
        tableIComp->horizontalHeader()->setDefaultSectionSize(40);
        tableIComp->horizontalHeader()->setMinimumSectionSize(10);
        tableIComp->verticalHeader()->setVisible(false);
        tableIComp->verticalHeader()->setDefaultSectionSize(20);

        verticalLayout_4->addWidget(tableIComp);

        splGEMS->addWidget(widgetIC);
        widgetPhase = new QWidget(splGEMS);
        widgetPhase->setObjectName(QStringLiteral("widgetPhase"));
        verticalLayout_5 = new QVBoxLayout(widgetPhase);
        verticalLayout_5->setSpacing(5);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 3, 0, 0);
        label_2 = new QLabel(widgetPhase);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_5->addWidget(label_2);

        listPhases = new QTreeWidget(widgetPhase);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        listPhases->setHeaderItem(__qtreewidgetitem);
        listPhases->setObjectName(QStringLiteral("listPhases"));
        listPhases->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listPhases->setTabKeyNavigation(true);
        listPhases->setDragEnabled(true);
        listPhases->header()->setVisible(false);

        verticalLayout_5->addWidget(listPhases);

        splGEMS->addWidget(widgetPhase);

        verticalLayout->addWidget(splGEMS);

        splV->addWidget(widgetGEMS);

        horizontalLayout->addWidget(splV);

        FITMainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(FITMainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 931, 20));
        menuTask = new QMenu(menubar);
        menuTask->setObjectName(QStringLiteral("menuTask"));
        menuRecord = new QMenu(menubar);
        menuRecord->setObjectName(QStringLiteral("menuRecord"));
        menuRecod_List = new QMenu(menubar);
        menuRecod_List->setObjectName(QStringLiteral("menuRecod_List"));
        menu_Calc = new QMenu(menubar);
        menu_Calc->setObjectName(QStringLiteral("menu_Calc"));
        menu_Help = new QMenu(menubar);
        menu_Help->setObjectName(QStringLiteral("menu_Help"));
        FITMainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(FITMainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        FITMainWindow->setStatusBar(statusbar);
        toolBar_2 = new QToolBar(FITMainWindow);
        toolBar_2->setObjectName(QStringLiteral("toolBar_2"));
        toolBar_2->setMovable(false);
        FITMainWindow->addToolBar(Qt::TopToolBarArea, toolBar_2);
        toolBarTask = new QToolBar(FITMainWindow);
        toolBarTask->setObjectName(QStringLiteral("toolBarTask"));
        toolBarTask->setMovable(false);
        FITMainWindow->addToolBar(Qt::TopToolBarArea, toolBarTask);
        toolBarMenu = new QToolBar(FITMainWindow);
        toolBarMenu->setObjectName(QStringLiteral("toolBarMenu"));
        toolBarMenu->setMovable(false);
        FITMainWindow->addToolBar(Qt::TopToolBarArea, toolBarMenu);
        toolBarGems = new QToolBar(FITMainWindow);
        toolBarGems->setObjectName(QStringLiteral("toolBarGems"));
        toolBarGems->setMovable(false);
        FITMainWindow->addToolBar(Qt::TopToolBarArea, toolBarGems);

        menubar->addAction(menuTask->menuAction());
        menubar->addAction(menuRecord->menuAction());
        menubar->addAction(menuRecod_List->menuAction());
        menubar->addAction(menu_Calc->menuAction());
        menubar->addAction(menu_Help->menuAction());
        menuTask->addSeparator();
        menuTask->addAction(action_Select_project);
        menuTask->addAction(action_New_Project);
        menuTask->addAction(action_Config_Project);
        menuTask->addSeparator();
        menuTask->addAction(action_DataBase_mode);
        menuTask->addAction(action_Task_Mode);
        menuTask->addAction(actionFits_View_Mode);
        menuTask->addSeparator();
        menuTask->addAction(actionSelect_GEMS);
        menuTask->addSeparator();
        menuTask->addAction(action_Exit);
        menuRecord->addAction(actionCreate_New);
        menuRecord->addAction(actionUpdate_Test);
        menuRecord->addSeparator();
        menuRecord->addAction(action_Update);
        menuRecord->addAction(action_Insert);
        menuRecord->addSeparator();
        menuRecord->addAction(actionDelete);
        menuRecord->addSeparator();
        menuRecord->addAction(actionReset_Search);
        menuRecord->addAction(action_Find);
        menuRecord->addAction(actionSave_Search);
        menuRecord->addAction(actionLoad_Search);
        menuRecord->addSeparator();
        menuRecord->addAction(actionInsert_search_results_into_task_definition);
        menuRecod_List->addAction(actionOverwrite);
        menuRecod_List->addSeparator();
        menuRecod_List->addAction(actionBackup_to_JSON);
        menuRecod_List->addAction(actionRestore_from_JSON);
        menuRecod_List->addSeparator();
        menuRecod_List->addAction(actionRestore_from_csv);
        menuRecod_List->addAction(actionBackup_to_csv);
        menuRecod_List->addSeparator();
        menuRecod_List->addAction(actionRestore_from_YAML);
        menuRecod_List->addAction(actionBackup_to_YAML);
        menuRecod_List->addSeparator();
        menuRecod_List->addAction(actionRestore_from_TXT);
        menuRecod_List->addAction(actionBackup_to_TXT);
        menuRecod_List->addSeparator();
        menuRecod_List->addAction(action_Delete_multiple_data);
        menu_Calc->addAction(action_Run_test);
        menu_Calc->addAction(action_Show_Results);
        menu_Calc->addAction(actionCancel_gemsfit2_run);
        menu_Help->addAction(action_Preferences);
        menu_Help->addSeparator();
        menu_Help->addAction(actionHelp);
        menu_Help->addAction(actionAbout);
        menu_Help->addSeparator();
        menu_Help->addAction(actionAuthors);
        menu_Help->addAction(actionLicense);
        toolBarMenu->addAction(actionPrevious);
        toolBarMenu->addAction(actionNext);
        toolBarMenu->addAction(action_Find);
        toolBarMenu->addSeparator();
        toolBarMenu->addAction(actionCreate_New);
        toolBarMenu->addAction(actionUpdate_Test);
        toolBarMenu->addAction(action_Insert);
        toolBarMenu->addAction(action_Update);
        toolBarMenu->addAction(actionDelete);
        toolBarMenu->addSeparator();
        toolBarMenu->addAction(action_Run_test);
        toolBarMenu->addAction(action_Show_Results);
        toolBarMenu->addSeparator();
        toolBarMenu->addAction(actionHelp);

        retranslateUi(FITMainWindow);

        QMetaObject::connectSlotsByName(FITMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *FITMainWindow)
    {
        FITMainWindow->setWindowTitle(QApplication::translate("FITMainWindow", "GEMSFITS: Optimization of GEM Input Parameters", 0));
        action_DataBase_mode->setText(QApplication::translate("FITMainWindow", "&Database Mode", 0));
        action_Task_Mode->setText(QApplication::translate("FITMainWindow", "Fitting &Task Mode", 0));
        action_New_Project->setText(QApplication::translate("FITMainWindow", "&New project...", 0));
        action_Select_project->setText(QApplication::translate("FITMainWindow", "&Open project...", 0));
        actionSelect_GEMS->setText(QApplication::translate("FITMainWindow", "Select &GEMS3K files", 0));
        action_Exit->setText(QApplication::translate("FITMainWindow", "&Exit", 0));
        actionCreate_New->setText(QApplication::translate("FITMainWindow", "&Create (New)...", 0));
#ifndef QT_NO_TOOLTIP
        actionCreate_New->setToolTip(QApplication::translate("FITMainWindow", "Create New by template", 0));
#endif // QT_NO_TOOLTIP
        action_Find->setText(QApplication::translate("FITMainWindow", "Execute &search query", 0));
#ifndef QT_NO_TOOLTIP
        action_Find->setToolTip(QApplication::translate("FITMainWindow", "Find", 0));
#endif // QT_NO_TOOLTIP
        action_Update->setText(QApplication::translate("FITMainWindow", "&Update ...", 0));
#ifndef QT_NO_TOOLTIP
        action_Update->setToolTip(QApplication::translate("FITMainWindow", "Update current record", 0));
#endif // QT_NO_TOOLTIP
        action_Insert->setText(QApplication::translate("FITMainWindow", "&Insert ...", 0));
#ifndef QT_NO_TOOLTIP
        action_Insert->setToolTip(QApplication::translate("FITMainWindow", "Insert new record", 0));
#endif // QT_NO_TOOLTIP
        actionDelete->setText(QApplication::translate("FITMainWindow", "&Delete ...", 0));
#ifndef QT_NO_TOOLTIP
        actionDelete->setToolTip(QApplication::translate("FITMainWindow", "Delete current record", 0));
#endif // QT_NO_TOOLTIP
        actionUpdate_Test->setText(QApplication::translate("FITMainWindow", "&Test JSON format", 0));
        actionBackup_to_JSON->setText(QApplication::translate("FITMainWindow", "Backup data to &JSON file...", 0));
        actionRestore_from_JSON->setText(QApplication::translate("FITMainWindow", "Restore data from JSO&N file...", 0));
        actionBackup_to_csv->setText(QApplication::translate("FITMainWindow", "Export data to &CSV file (tbd)...", 0));
        actionRestore_from_csv->setText(QApplication::translate("FITMainWindow", "Import data from C&SV file...", 0));
        actionBackup_to_TXT->setText(QApplication::translate("FITMainWindow", "Export task to gemsfit2 input &TXT file...", 0));
        actionRestore_from_TXT->setText(QApplication::translate("FITMainWindow", "Import task from gemsfit2 input T&XT file...", 0));
        actionBackup_to_YAML->setText(QApplication::translate("FITMainWindow", "Export data to &YAML file (tbd)...", 0));
        actionRestore_from_YAML->setText(QApplication::translate("FITMainWindow", "Import data from Y&AML file (tbd)...", 0));
        action_Run_test->setText(QApplication::translate("FITMainWindow", "&Run gemsfit2 parameter fitting task", 0));
        action_Show_Results->setText(QApplication::translate("FITMainWindow", "&View parameter fitting task results... ", 0));
        action_Preferences->setText(QApplication::translate("FITMainWindow", "&Preferences ...", 0));
        actionHelp->setText(QApplication::translate("FITMainWindow", "Help - About GEMSFITS", 0));
        actionAbout->setText(QApplication::translate("FITMainWindow", "About gemsfit2 code", 0));
        actionAuthors->setText(QApplication::translate("FITMainWindow", "Authors", 0));
        actionLicense->setText(QApplication::translate("FITMainWindow", "License", 0));
        actionNext->setText(QApplication::translate("FITMainWindow", "Next", 0));
        actionPrevious->setText(QApplication::translate("FITMainWindow", "Previous", 0));
        action_Config_Project->setText(QApplication::translate("FITMainWindow", "&Reconfigure project...", 0));
        actionOverwrite->setText(QApplication::translate("FITMainWindow", "Overwrite", 0));
        action_Delete_multiple_data->setText(QApplication::translate("FITMainWindow", "&Delete multiple data records... ", 0));
        actionFits_View_Mode->setText(QApplication::translate("FITMainWindow", "Fit results &View Mode", 0));
        actionSave_Results_to_Database->setText(QApplication::translate("FITMainWindow", "Save Results to Database", 0));
        actionReset_Search->setText(QApplication::translate("FITMainWindow", "&Reset data search query ...", 0));
        actionSave_Search->setText(QApplication::translate("FITMainWindow", "Save search query to file ...", 0));
        actionLoad_Search->setText(QApplication::translate("FITMainWindow", "Load search query from file ...", 0));
        actionCancel_gemsfit2_run->setText(QApplication::translate("FITMainWindow", "Cancel gemsfit2 run", 0));
        actionInsert_search_results_into_task_definition->setText(QApplication::translate("FITMainWindow", "Insert record list into task definition ", 0));
        filterButton->setText(QApplication::translate("FITMainWindow", "...", 0));
        label_5->setText(QApplication::translate("FITMainWindow", "Search Query Editor", 0));
        label_3->setText(QApplication::translate("FITMainWindow", "Data or Task Editor", 0));
        label_4->setText(QApplication::translate("FITMainWindow", "Run Status", 0));
        label->setText(QApplication::translate("FITMainWindow", "  IComp (Elements)", 0));
        label_2->setText(QApplication::translate("FITMainWindow", "Phases/Components", 0));
        menuTask->setTitle(QApplication::translate("FITMainWindow", "&Project", 0));
        menuRecord->setTitle(QApplication::translate("FITMainWindow", "&Record", 0));
        menuRecod_List->setTitle(QApplication::translate("FITMainWindow", "&Database", 0));
        menu_Calc->setTitle(QApplication::translate("FITMainWindow", "&Calculate", 0));
        menu_Help->setTitle(QApplication::translate("FITMainWindow", "&Help", 0));
        toolBar_2->setWindowTitle(QApplication::translate("FITMainWindow", "toolBar", 0));
        toolBarTask->setWindowTitle(QApplication::translate("FITMainWindow", "toolBar_2", 0));
        toolBarMenu->setWindowTitle(QApplication::translate("FITMainWindow", "toolBar_3", 0));
        toolBarGems->setWindowTitle(QApplication::translate("FITMainWindow", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class FITMainWindow: public Ui_FITMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FITMAINWINDOW_H
