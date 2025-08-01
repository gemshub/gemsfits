//-------------------------------------------------------------------
// $Id: HelpWindow.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of HelpBrowser, HelpWindow and  SearchWidget classes
//
// Copyright (C) 2012-2023  S.V.Dmytriyeva
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

#ifndef HelpWindow_included
#define HelpWindow_included

#include <QMainWindow>
#include <QTextBrowser>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
class QHelpEngineCore;
class QHelpEngine;
class QHelpContentWidget;
class QHelpIndexWidget;
class QHelpSearchEngine;
class QHelpSearchResultWidget;
class QLabel
QT_END_NAMESPACE;

class HelpBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    HelpBrowser(QHelpEngine *hEngine, QWidget *parent);

private:
    QVariant loadResource(int type, const QUrl &name);

    QHelpEngine *m_Engine;
};

class SearchWidget : public QWidget
{
    Q_OBJECT

    QHelpSearchEngine *srchEngine;
    QHelpSearchResultWidget *rsltWidget;

public:
    SearchWidget(QHelpSearchEngine *engine, QWidget *parent = 0);
    ~SearchWidget();

signals:
    void requestShowLink(const QUrl &url);

private slots:
    void search() const;
    void searchingStarted();
    void searchingFinished(int hits);
};

namespace Ui {
class HelpWindowData;
}

class HelpWindow : public QMainWindow
{
    Q_OBJECT

    Ui::HelpWindowData *ui;
    QHelpEngine *hEngine;
    QHelpContentWidget *wContents;
    QHelpIndexWidget *wIndex;
    HelpBrowser *wBrowser;
    QLineEdit *adressLine;
    QLineEdit *findLine;
    SearchWidget* srchWidget;

    QTabWidget *tab_;
    QWidget *tabContents;
    QWidget *tabIndex;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *lineIndex;
    QWidget *tabSearch;
    QVBoxLayout *verticalLayout_2;

public:
    static HelpWindow* pDia;

    HelpWindow(QWidget* parent = NULL);
    virtual ~HelpWindow();

    void showDocumentation(const char* file, const char* item1);

protected:
    QUrl showHelpForKeyword(const QString &id);
    void setActions();

protected slots:
    void loadResource( const QUrl &name );
    void filterIndices(const QString &filter);
    void showAddres(const QUrl &name);
    void syncContents();
    void helpAbout();
    void helpVersion();
    void helpPrint();
    void helpOnHelp();
    void actionFind();
    void actionFindNext();
    void actionFindPrevious();
    void actionZoomIn();
    void actionZoomOut();
    void showFind();
};

#endif // HelpWindow_included

