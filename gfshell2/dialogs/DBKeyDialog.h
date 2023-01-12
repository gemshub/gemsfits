//-------------------------------------------------------------------
// $Id: DBKeyDialog.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of DBKeyDialog and DBKeyFilter classes
//
// Copyright (C) 2014  S.V.Dmytriyeva
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

#ifndef DBKeyDialog_included
#define DBKeyDialog_included

#include <string>
#include <vector>
#include <QDialog>
#include <QLineEdit>

namespace Ui {
class KeyDialogData;
}

/// List Keys Dialog
class DBKeyDialog : public QDialog
{
    Q_OBJECT

    Ui::KeyDialogData *ui;
    bool multi;
    std::string keyFilter;
    int iRT;
    std::vector<std::string> old_sel;

    void SetList();

protected slots:
    void CmSelectAll();
    void CmClearAll();
    void CmFilter();
    void CmHelp();

public:

    DBKeyDialog(QWidget* win, int irt, const char* key = "*",
                const char* caption = 0, bool filter=true);
    DBKeyDialog(QWidget* win, int irt, const std::vector<std::string>& sel,
                const char* key = "*", const char* caption = 0);

    virtual ~DBKeyDialog();


    std::vector<std::string> allSelectedKeys();
    std::string getKey();
    void set_connect();
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
    std::string SetKeyString();

public:
    DBKeyFilter(QWidget* win, int iRt, const char* keyFilter,
                const char* caption, bool allowTemplates=true );

    std::string getFilter();
};


#endif // DBKeyDialog_included
