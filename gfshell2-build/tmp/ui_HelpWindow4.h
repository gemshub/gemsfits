/********************************************************************************
** Form generated from reading UI file 'HelpWindow4.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HELPWINDOW4_H
#define UI_HELPWINDOW4_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HelpWindowData
{
public:
    QAction *actionHome;
    QAction *actionBack;
    QAction *actionForward;
    QAction *actionSync_with_Contents;
    QAction *action_About;
    QAction *action_Print;
    QAction *action_Quit;
    QAction *action_Find;
    QAction *actionFind_Next;
    QAction *actionFind_Previous;
    QAction *action_Case_sensetiv;
    QAction *action_Whole_words;
    QAction *actionVersion;
    QAction *actionZoom_In;
    QAction *actionZoom_Out;
    QAction *actionHelp_on_Help;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QMenuBar *menubar;
    QMenu *menuGo;
    QMenu *menu_Help;
    QMenu *menu_File;
    QMenu *menuFin_d;
    QMenu *menuView;
    QStatusBar *statusbar;
    QToolBar *toolBar;
    QToolBar *toolFind;
    QToolBar *toolAddress;

    void setupUi(QMainWindow *HelpWindowData)
    {
        if (HelpWindowData->objectName().isEmpty())
            HelpWindowData->setObjectName(QStringLiteral("HelpWindowData"));
        HelpWindowData->setWindowModality(Qt::NonModal);
        HelpWindowData->resize(983, 600);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(HelpWindowData->sizePolicy().hasHeightForWidth());
        HelpWindowData->setSizePolicy(sizePolicy);
        actionHome = new QAction(HelpWindowData);
        actionHome->setObjectName(QStringLiteral("actionHome"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/menu/Icons/HomeInHelpMenu.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionHome->setIcon(icon);
        actionBack = new QAction(HelpWindowData);
        actionBack->setObjectName(QStringLiteral("actionBack"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/menu/Icons/DisplayPrevRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionBack->setIcon(icon1);
        actionForward = new QAction(HelpWindowData);
        actionForward->setObjectName(QStringLiteral("actionForward"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/menu/Icons/DisplayNextRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionForward->setIcon(icon2);
        actionSync_with_Contents = new QAction(HelpWindowData);
        actionSync_with_Contents->setObjectName(QStringLiteral("actionSync_with_Contents"));
        action_About = new QAction(HelpWindowData);
        action_About->setObjectName(QStringLiteral("action_About"));
        action_Print = new QAction(HelpWindowData);
        action_Print->setObjectName(QStringLiteral("action_Print"));
        action_Quit = new QAction(HelpWindowData);
        action_Quit->setObjectName(QStringLiteral("action_Quit"));
        action_Find = new QAction(HelpWindowData);
        action_Find->setObjectName(QStringLiteral("action_Find"));
        actionFind_Next = new QAction(HelpWindowData);
        actionFind_Next->setObjectName(QStringLiteral("actionFind_Next"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/menu/Icons/SearchForwardIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFind_Next->setIcon(icon3);
        actionFind_Previous = new QAction(HelpWindowData);
        actionFind_Previous->setObjectName(QStringLiteral("actionFind_Previous"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/menu/Icons/SearchBackwardIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFind_Previous->setIcon(icon4);
        action_Case_sensetiv = new QAction(HelpWindowData);
        action_Case_sensetiv->setObjectName(QStringLiteral("action_Case_sensetiv"));
        action_Case_sensetiv->setCheckable(true);
        action_Whole_words = new QAction(HelpWindowData);
        action_Whole_words->setObjectName(QStringLiteral("action_Whole_words"));
        action_Whole_words->setCheckable(true);
        actionVersion = new QAction(HelpWindowData);
        actionVersion->setObjectName(QStringLiteral("actionVersion"));
        actionZoom_In = new QAction(HelpWindowData);
        actionZoom_In->setObjectName(QStringLiteral("actionZoom_In"));
        actionZoom_Out = new QAction(HelpWindowData);
        actionZoom_Out->setObjectName(QStringLiteral("actionZoom_Out"));
        actionHelp_on_Help = new QAction(HelpWindowData);
        actionHelp_on_Help->setObjectName(QStringLiteral("actionHelp_on_Help"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/menu/Icons/ShowHelpWindowIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionHelp_on_Help->setIcon(icon5);
        centralwidget = new QWidget(HelpWindowData);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        HelpWindowData->setCentralWidget(centralwidget);
        menubar = new QMenuBar(HelpWindowData);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 983, 20));
        menuGo = new QMenu(menubar);
        menuGo->setObjectName(QStringLiteral("menuGo"));
        menu_Help = new QMenu(menubar);
        menu_Help->setObjectName(QStringLiteral("menu_Help"));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        menuFin_d = new QMenu(menubar);
        menuFin_d->setObjectName(QStringLiteral("menuFin_d"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QStringLiteral("menuView"));
        HelpWindowData->setMenuBar(menubar);
        statusbar = new QStatusBar(HelpWindowData);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        HelpWindowData->setStatusBar(statusbar);
        toolBar = new QToolBar(HelpWindowData);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        HelpWindowData->addToolBar(Qt::TopToolBarArea, toolBar);
        toolFind = new QToolBar(HelpWindowData);
        toolFind->setObjectName(QStringLiteral("toolFind"));
        HelpWindowData->addToolBar(Qt::TopToolBarArea, toolFind);
        toolAddress = new QToolBar(HelpWindowData);
        toolAddress->setObjectName(QStringLiteral("toolAddress"));
        HelpWindowData->addToolBar(Qt::TopToolBarArea, toolAddress);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menuFin_d->menuAction());
        menubar->addAction(menuGo->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menu_Help->menuAction());
        menuGo->addAction(actionHome);
        menuGo->addAction(actionBack);
        menuGo->addAction(actionForward);
        menuGo->addAction(actionSync_with_Contents);
        menu_Help->addAction(action_About);
        menu_Help->addAction(actionVersion);
        menu_Help->addAction(actionHelp_on_Help);
        menu_File->addAction(action_Print);
        menu_File->addSeparator();
        menu_File->addAction(action_Quit);
        menuFin_d->addAction(action_Find);
        menuFin_d->addAction(actionFind_Next);
        menuFin_d->addAction(actionFind_Previous);
        menuFin_d->addSeparator();
        menuFin_d->addAction(action_Case_sensetiv);
        menuFin_d->addAction(action_Whole_words);
        menuView->addAction(actionZoom_In);
        menuView->addAction(actionZoom_Out);
        toolBar->addAction(actionBack);
        toolBar->addAction(actionForward);
        toolBar->addAction(actionHome);
        toolBar->addSeparator();
        toolBar->addAction(actionHelp_on_Help);

        retranslateUi(HelpWindowData);
        QObject::connect(action_Quit, SIGNAL(triggered()), HelpWindowData, SLOT(close()));

        QMetaObject::connectSlotsByName(HelpWindowData);
    } // setupUi

    void retranslateUi(QMainWindow *HelpWindowData)
    {
        HelpWindowData->setWindowTitle(QApplication::translate("HelpWindowData", "GEMSFITS Help Viewer", 0));
        actionHome->setText(QApplication::translate("HelpWindowData", "&Home", 0));
        actionBack->setText(QApplication::translate("HelpWindowData", "&Back", 0));
        actionForward->setText(QApplication::translate("HelpWindowData", "&Forward", 0));
        actionSync_with_Contents->setText(QApplication::translate("HelpWindowData", "&Sync with Contents", 0));
        action_About->setText(QApplication::translate("HelpWindowData", "&About GEM-Selektor...", 0));
        action_Print->setText(QApplication::translate("HelpWindowData", "&Print...", 0));
        action_Quit->setText(QApplication::translate("HelpWindowData", "&Quit", 0));
        action_Find->setText(QApplication::translate("HelpWindowData", "&Find...", 0));
        actionFind_Next->setText(QApplication::translate("HelpWindowData", "Find &Next", 0));
        actionFind_Next->setShortcut(QApplication::translate("HelpWindowData", "F3", 0));
        actionFind_Previous->setText(QApplication::translate("HelpWindowData", "Find &Previous", 0));
        actionFind_Previous->setShortcut(QApplication::translate("HelpWindowData", "Shift+F3", 0));
        action_Case_sensetiv->setText(QApplication::translate("HelpWindowData", "&Case Sensitive ", 0));
        action_Whole_words->setText(QApplication::translate("HelpWindowData", "&Whole Word", 0));
        actionVersion->setText(QApplication::translate("HelpWindowData", "Version...", 0));
        actionZoom_In->setText(QApplication::translate("HelpWindowData", "Zoom &In", 0));
        actionZoom_In->setShortcut(QApplication::translate("HelpWindowData", "+", 0));
        actionZoom_Out->setText(QApplication::translate("HelpWindowData", "Zoom &Out", 0));
        actionZoom_Out->setShortcut(QApplication::translate("HelpWindowData", "-", 0));
        actionHelp_on_Help->setText(QApplication::translate("HelpWindowData", "Help on Help", 0));
        menuGo->setTitle(QApplication::translate("HelpWindowData", "&Go", 0));
        menu_Help->setTitle(QApplication::translate("HelpWindowData", "&Help", 0));
        menu_File->setTitle(QApplication::translate("HelpWindowData", "&File", 0));
        menuFin_d->setTitle(QApplication::translate("HelpWindowData", "Fin&d", 0));
        menuView->setTitle(QApplication::translate("HelpWindowData", "&View", 0));
        toolBar->setWindowTitle(QApplication::translate("HelpWindowData", "toolBar", 0));
        toolFind->setWindowTitle(QApplication::translate("HelpWindowData", "toolBar_2", 0));
        toolAddress->setWindowTitle(QApplication::translate("HelpWindowData", "toolBar_3", 0));
    } // retranslateUi

};

namespace Ui {
    class HelpWindowData: public Ui_HelpWindowData {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HELPWINDOW4_H
