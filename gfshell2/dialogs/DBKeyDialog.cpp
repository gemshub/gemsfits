//-------------------------------------------------------------------
// $Id: DBKeyDialog.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of DBKeyDialog and DBKeyFilter classes
//
// Copyright (C) 2014  S.V.Dmytriyeva
// Uses EJDB (http://ejdb.org), yaml-cpp (https://code.google.com/p/yaml-cpp/)
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

#include <QListWidget>
#include "DBKeyDialog.h"
#include "ui_DBKeyDialog.h"
#include "f_database.h"
#include "gui_service.h"
#include <GEMS3K/v_service.h>

extern const char *DBM;

std::vector<std::string> vfMultiKeys(QWidget* par, const char* caption,
                                     int iRt, const char* key)
{
    std::vector<std::string> sel;
    DBKeyDialog dbk(par, iRt, sel, key, caption);
    dbk.exec();
    return dbk.allSelectedKeys();
}

std::vector<std::string> vfMultiKeysSet(QWidget* par, const char* caption,
                                        int iRt, const char* key, std::vector<std::string>& sel)
{
    DBKeyDialog dbk(par, iRt, sel, key, caption);
    dbk.exec();
    return dbk.allSelectedKeys();
}

//--------------------------------------------------------------------------------------------------------------


DBKeyDialog::DBKeyDialog(QWidget* win, int irt, const char* key,
                         const char* caption, bool filter):
    QDialog( win),
    ui(new Ui::KeyDialogData),
    multi(false), iRT(irt)
{
    ui->setupUi(this);

    old_sel.clear();
    //pList->setFont( pVisorImp->getCellFont() );
    QObject::connect(ui->pList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
    setWindowTitle( caption );
    std::vector<std::string> keyList;

    if(!key)
        Error( "DBKeyDialog", "Record Key is null");
    else if( strpbrk(key, "*?") == 0 )
        keyFilter = "*";
    else
        keyFilter = key;

    std::string s = "Please, select one record key. Filter: ";
    s +=  keyFilter;
    ui->pLabel->setText(s.c_str());

    s  = rtEJ[irt].packKey();
    int n = rtEJ[irt].getKeyList(keyFilter, keyList);
    int sel = 0;

    for( int ii=0; ii<n; ii++ )
    {
        ui->pList->addItem(keyList[ii].c_str());
        if( keyList[ii]==s )
            sel = ii;
    }

    ui->pList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->pList->setCurrentRow(sel);

    ui->pButton3->hide();
    ui->pButton2->hide();
    if( !filter )
        ui->pFilterButton->hide();
    set_connect();
    ui->pList->setFocus();
}

DBKeyDialog::DBKeyDialog(QWidget* win, int irt,
                         const std::vector<std::string>& sel,
                         const char* key, const char* caption):
    QDialog(win),
    ui(new Ui::KeyDialogData),
    multi(true), iRT(irt)
{
    ui->setupUi(this);

    old_sel.clear();
    for(size_t ii=0; ii<sel.size(); ii++)
        old_sel.push_back( sel[ii] );

    //pList->setFont( pVisorImp->getCellFont() );
    ui->pList->setSelectionMode(QAbstractItemView::MultiSelection);
    setWindowTitle( caption );

    if(!key)
        Error("KeyDialog", "pkey is null");
    else if( strpbrk(key, "*?") == 0 )
        keyFilter = "*";
    else
        keyFilter = key;

    SetList();
    set_connect();
    ui->pList->setFocus();
}

DBKeyDialog::~DBKeyDialog()
{
    delete ui;
}

void DBKeyDialog::set_connect()
{
    QObject::connect(ui->bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ));
    QObject::connect(ui->PushButton1, &QPushButton::clicked, this, qOverload<>(&QDialog::accept));
    QObject::connect(ui->PushButton3, &QPushButton::clicked, this, qOverload<>(&QDialog::reject));
    QObject::connect(ui->pFilterButton, &QPushButton::clicked, this, qOverload<>(&DBKeyDialog::CmFilter));
    QObject::connect(ui->pButton2, &QPushButton::clicked, this, qOverload<>(&DBKeyDialog::CmSelectAll));
    QObject::connect(ui->pButton3, &QPushButton::clicked, this, qOverload<>(&DBKeyDialog::CmClearAll));
}

void DBKeyDialog::CmHelp()
{
    ; // pFitImp->OpenHelp( GEMS_SELECT_HTML );
}


void DBKeyDialog::SetList()
{
    std::vector<std::string> keyList;

    std::string s = "Please, mark one or more record keys. Filter: ";
    s +=  keyFilter;
    ui->pLabel->setText(s.c_str());

    int n = rtEJ[iRT].getKeyList(keyFilter, keyList);

    for( int ii=0; ii<n; ii++ )
        ui->pList->addItem(keyList[ii].c_str());

    if( multi )
    {
        for(size_t jj=0; jj<old_sel.size(); jj++)
            for( int ii=0; ii<n; ii++ )
            {
                // comparing parts before '*' for overwrite dcomp, reacdc ....
                size_t pos = old_sel[jj].find('*');
                std::string str(old_sel[jj], 0, pos);
                if( keyList[ii].find(str) != std::string::npos )
                {
                    ui->pList->item(ii)->setSelected( true);
                    break;
                }
            }
    }
}


std::string DBKeyDialog::getKey()
{
    int sel = ui->pList->currentRow();
    if( sel != -1 )
    {
        /// !!!! ((TCModule*)aMod[iRt])->setFilter(keyFilter.c_str());
        std::string res = ui->pList->item(sel)->text().toStdString();
        return res;
    }
    return std::string();
}


void DBKeyDialog::CmFilter()
{
    std::string str_name = "Template for ";
    str_name +=  rtEJ[iRT].getKeywd();
    str_name += " record key";
    DBKeyFilter dbFilter(this, iRT, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        ui->pList->clear();
        SetList();
        ui->pList->setCurrentRow(0);
    }
}

void DBKeyDialog::CmSelectAll()
{
    // select all strings
    for( int ii=0; ii<ui->pList->count(); ii++ )
        ui->pList->item(ii)->setSelected( true);
}


void DBKeyDialog::CmClearAll()
{
    ui->pList->clearSelection();
}

/// returns selection array
///    array is empty if nothing is selected
std::vector<std::string> DBKeyDialog::allSelectedKeys()
{
    std::vector<std::string> arr;

    if( !result() )
        return arr;

    for( int ii=0; ii<ui->pList->count(); ii++ )
        if( ui->pList->item(ii)->isSelected() )
        {
            std::string s = ui->pList->item(ii)->text().toStdString();
            arr.push_back(s);
        }
    return arr;
}

//============================================================
// KeyEdit dialog
//============================================================

DBKeyFilter::DBKeyFilter(QWidget* win, int irt, const char* key,
                         const char* caption, bool allowTempl):
    QDialog(win),
    iRT(irt),
    allowTemplates(allowTempl)
{
    QLineEdit* pEdit;
    QLabel* pLabel;

    setWindowModality(Qt::WindowModal);
    setWindowTitle(caption);

    TEJDBKey dbKey(rtEJ[irt].getDBKey());
    if( key )
        dbKey.setKey(key);

    QGridLayout* editBox = new QGridLayout();
    for( size_t ii=0; ii<dbKey.keySize(); ii++)
    {
        aEdit.append( pEdit = new QLineEdit(this) );
        QString str = rtEJ[irt].keyFieldName(ii);
        pEdit->setToolTip( str );
        pEdit->setMaxLength( 100 );
        pEdit->setText( dbKey.keyField(ii).c_str() );
        connect( pEdit, SIGNAL(editingFinished()), this, SLOT(setKeyLine()) );

        editBox->addWidget( pEdit, ii, 0, Qt::AlignRight);
        pLabel = new QLabel( str, this);
        editBox->addWidget( pLabel, ii, 1);
    }
    aEdit[0]->setFocus();

    QHBoxLayout* buttonBox = new QHBoxLayout();
    QPushButton* btn;

    btn = new QPushButton(this);
    btn->setText("&Ok");
    btn->setDefault(true);
    connect( btn, SIGNAL(clicked()), this, SLOT(CmOk()) );
    buttonBox->addWidget( btn );

    btn = new QPushButton(this);
    btn->setText("&Reset");
    connect( btn, SIGNAL(clicked()), this, SLOT(EvSetAll()) );
    buttonBox->addWidget( btn );

    btn = new QPushButton(this);
    btn->setText("&From List");
    connect( btn, SIGNAL(clicked()), this, SLOT(EvGetList()) );
    buttonBox->addWidget( btn );

    buttonBox->addStretch();

    btn = new QPushButton(this);
    btn->setText("&Help");
    connect( btn, SIGNAL(clicked()), this, SLOT(CmHelp()) );
    buttonBox->addWidget( btn );

    btn = new QPushButton(this);
    btn->setText("&Cancel");
    connect( btn, SIGNAL(clicked()), this, SLOT(reject()) );
    buttonBox->addWidget( btn );

    fullKey = new QLineEdit(this);
    fullKey->setText(dbKey.packKey().c_str());
    fullKey->setFocusPolicy( Qt::ClickFocus );
    fullKey->setReadOnly( true );

    QFrame *line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line_3"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QVBoxLayout* mainBox = new QVBoxLayout(this);
    mainBox->addWidget(fullKey);
    mainBox->addWidget(line);
    mainBox->addLayout(editBox);

    line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainBox->addWidget(line);
    mainBox->addLayout(buttonBox);
}

void DBKeyFilter::CmHelp()
{
    std::string dbName =  DBM;
    dbName +="_";
    dbName += rtEJ[iRT].getKeywd();
    // pFitImp->OpenHelp(  GEMS_REKEY_HTML, dbName.c_str() );
}

void DBKeyFilter::CmOk()
{
    if( allowTemplates || SetKeyString().find_first_of("*?") == std::string::npos )
    {
        accept();
        return;
    }
    messageCritical(this, "Key error", "No templates allowed!");
}

std::string DBKeyFilter::SetKeyString()
{
    std::string Key = "";
    for( int ii=0; ii<aEdit.count(); ii++ )
    {
        std::string s = aEdit[ii]->text().toStdString();
        Key += s;
        strip(Key);
        Key += ":";
    }
    return Key;
}

void DBKeyFilter::setKeyLine()
{
    fullKey->setText(SetKeyString().c_str());
}

void DBKeyFilter::EvSetAll()
{
    for( int ii=0; ii<aEdit.count(); ii++ )
        aEdit[ii]->setText("*");
    setKeyLine();
}

void DBKeyFilter::EvGetList()
{
    DBKeyDialog dlg(this, iRT, SetKeyString().c_str(), "Key template", false);
    if( !dlg.exec() )
        return;

    TEJDBKey dbKey(rtEJ[iRT].getDBKey());
    dbKey.setKey(dlg.getKey());

    for(size_t ii=0; ii<dbKey.keySize(); ii++)
        aEdit[ii]->setText( dbKey.keyField(ii).c_str() );
    setKeyLine();
}

std::string DBKeyFilter::getFilter()
{
    return SetKeyString();
}

//--------------------- End of DBKeyDialog.cpp ---------------------------

