//-------------------------------------------------------------------
// $Id: CalcDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of CalcDialog class
//
// Copyright (C) 1996-2010  A.Rysin, S.Dmytriyeva
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

#ifndef CalcDialog_included
#define CalcDialog_included

#include <QDialog>
#include <QButtonGroup>

namespace Ui {
class CalcDialogData;
}

class CalcDialog : public QDialog
{
    Q_OBJECT

    Ui::CalcDialogData *ui;
    int  nObj;
    QButtonGroup *allButtons;
    
protected slots:
    virtual void setMode(int id);
    virtual void ok();
    virtual void unaryMode();
    
public:
    CalcDialog(QWidget* parent);
    virtual ~CalcDialog();

    QString fun(double val);

};

#endif // CalcDialog_included
