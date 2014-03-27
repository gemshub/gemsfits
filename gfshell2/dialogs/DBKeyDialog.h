//-------------------------------------------------------------------
// $Id: DBKeyDialog.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of DBKeyDialog and DBKeyFilter classes
//
// Copyright (C) 2014  S.V.Dmytriyeva
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

#ifndef DBKeyDialog_included
#define DBKeyDialog_included

#include <QDialog>
#include "ui_DBKeyDialog.h"
#include "verror.h"

/// List Keys Dialog
class DBKeyDialog : public QDialog, public Ui::KeyDialogData
{
    Q_OBJECT

    bool multi;
    string keyFilter;
    int iRT;
    vector<string> old_sel;

    void SetList();

protected slots:
    void CmSelectAll();
    void CmClearAll();
    void CmFilter();
    void CmHelp();


public:

    DBKeyDialog(QWidget* win, int irt, const char* key = "*",
              const char* caption = 0, bool filter=true);
    DBKeyDialog(QWidget* win, int irt, const vector<string>& sel,
              const char* key = "*", const char* caption = 0 );

    virtual ~DBKeyDialog();


    vector<string> allSelectedKeys();
    string getKey();
};

//------------------------------------------------------------
/// Key Edit Dialog
class DBKeyFilter:   public QDialog
{
    Q_OBJECT

    QList<QLineEdit*> aEdit;
    QLineEdit *fullKey;

    int iRT;
    bool allowTemplates;

protected slots:
    void EvSetAll();
    void EvGetList();
    void CmOk();
    void CmHelp();
    void setKeyLine();

protected:
    string SetKeyString();

public:
    DBKeyFilter(QWidget* win, int iRt, const char* keyFilter,
              const char* caption, bool allowTemplates=true );

    string getFilter();
};


#endif // DBKeyDialog_included
