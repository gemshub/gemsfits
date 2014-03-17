//-------------------------------------------------------------------
// $Id: KeyDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of KeyDialog class
//
// Copyright (C) 1996-2008 A.Rysin, S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
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
