//-------------------------------------------------------------------
// $Id: CalcDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of CalcDialog class
//
// Copyright (C) 1996-2023  A.Rysin, S.Dmytriyeva
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

#include "CalcDialog.h"
#include "ui_CalcDialog4.h"
#include "v_detail.h"

const double IPNC_DBL_MAX = 1e37;
const double IPNC_DBL_MIN = 1e-37;
const double IPNC_DBL_MAX_10_EXP = 37.;
const double IPNC_DBL_MIN_10_EXP = -37.;
const double IPNC_DBL_MAX_EXP = 85.195648;
const double IPNC_DBL_MIN_EXP = -85.195648;

CalcDialog::CalcDialog(QWidget* parent):
    QDialog(parent),
    ui(new Ui::CalcDialogData)
{
    ui->setupUi(this);
    
    allButtons = new QButtonGroup( ui->pButtonBox );
    allButtons->addButton(ui->pAssignBtn, 0);
    allButtons->addButton(ui->PushButton2, 1);
    allButtons->addButton(ui->PushButton3, 2);
    allButtons->addButton(ui->PushButton4, 3);
    allButtons->addButton(ui->PushButton5, 4);
    allButtons->addButton(ui->PushButton6, 5);
    allButtons->addButton(ui->PushButton7, 6);
    allButtons->addButton(ui->PushButton8, 7);
    allButtons->addButton(ui->PushButton9, 8);
    allButtons->addButton(ui->PushButton10, 9);
    allButtons->addButton(ui->PushButton11, 10);
    allButtons->addButton(ui->PushButton12, 11);
    allButtons->addButton(ui->PushButton13, 12);
    allButtons->addButton(ui->PushButton14, 13);
    allButtons->addButton(ui->PushButton15, 14);
    allButtons->addButton(ui->pClearBtn, 15);
    ui->pAssignBtn->setChecked( true);

    QObject::connect( allButtons, SIGNAL(buttonClicked(int)), this, SLOT(setMode(int)));
    QObject::connect(ui->PushButton1_3, SIGNAL(clicked()), this, SLOT(ok()));
    QObject::connect(ui->PushButton1_2, SIGNAL(clicked()), this, SLOT(reject()));
}

CalcDialog::~CalcDialog()
{
    delete ui;
}

void CalcDialog::unaryMode()
{
    ui->pValue->setEnabled(false);
}

void CalcDialog::setMode( int id )
{
    if( id <= 5)
        ui->pValue->setEnabled(true);
    else
        ui->pValue->setEnabled(false);
}

void CalcDialog::ok()
{
    accept();
}

QString CalcDialog::fun(double val)
{
    int ii, pos;
    double ret = val;

    ii = allButtons->checkedId();
    double val2=0.;
    if( ii <= 5 )
    {
        QString str = ui->pValue->currentText();
        pos = str.indexOf('(');
        if( pos >= 0 )
            str.truncate(pos);
        if( (str.indexOf("---") != -1) || str[0] == '`' || str.isEmpty() )
            val2 = DOUBLE_EMPTY;
        else
            val2 = str.toDouble();  /// check for error
    }
    switch( ii )
    {
    case 0:
        ret = val2;
        break;
    case 1:
        ret = val + val2;
        break;
    case 2:
        ret = val - val2;
        break;
    case 3:
        ret = val * val2;
        break;
    case 4:
        if( fabs(val2) >=  IPNC_DBL_MIN ) // Attempt of zerodivide!
            ret = val / val2;
        break;
    case 5:
        if( !(fabs(val)<IPNC_DBL_MIN || fabs(val)>IPNC_DBL_MAX
              || fabs(val2) < IPNC_DBL_MIN_10_EXP
              || fabs(val2) > IPNC_DBL_MAX_10_EXP) ) // Attempt of pow() argument out of range
            ret = pow(val, val2);
        break;
    case 6:
        if( fabs(val) >= IPNC_DBL_MIN ) // Attempt of zerodivide!
            ret = 1/val;
        break;
    case 7:
        if( val >= IPNC_DBL_MIN ) // Attempt of sqrt() argument <= 0
            ret = sqrt(val);
        break;
    case 8:
        if( val >= IPNC_DBL_MIN )
            ret = log10(val);
        break;
    case 9:
        if( val >= IPNC_DBL_MIN )
            ret = log(val);
        break;
    case 10:
        if( !( fabs(val) < IPNC_DBL_MIN_10_EXP
               || fabs(val) > IPNC_DBL_MAX_10_EXP) ) // Attempt of pow() argument out of range
            ret = pow(10.,val);
        break;
    case 11:
        if( !(val < IPNC_DBL_MIN_EXP || val > IPNC_DBL_MAX_EXP) ) // Attempt of exp() argument out of range
            ret =  exp(val);
        break;
    case 12:
        ret = sin(val);
        break;
    case 13:
        ret = cos(val);
        break;
    case 14:
        ret = tan(val);
        break;
    case 15:
        ret = DOUBLE_EMPTY; //No Data value
    }

    QString retstr;
    if( ret == DOUBLE_EMPTY )
        retstr = "";//---";
    else
        retstr = QString::number(  ret, 'g', 8 );//QVariant(ret).toString();
    return retstr;
}

//--------------------- End of CalcDialog.cpp ---------------------------
