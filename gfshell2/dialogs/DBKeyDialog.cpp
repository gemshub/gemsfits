//-------------------------------------------------------------------
// $Id: DBKeyDialog.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of DBKeyDialog and DBKeyFilter classes
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

#include <QListWidget>
#include <QMessageBox>

#include "DBKeyDialog.h"
#include "FITMainWindow.h"
#include "f_ejdb.h"
#include "v_service.h"

//------------------------------------------------------------------
// service functions

void messageCritical(QWidget* par, const std::string& title, const std::string& mess )
{
    QString titl, spac, messag;
    titl = title.c_str(); spac = "\n\n"; messag = mess.c_str();

    QMessageBox::critical(par,
#ifdef __unix
#ifdef __APPLE__
        "Title", titl.append(spac+=messag)
#else
        titl, messag
#endif
#else
        titl, messag
#endif
                            );
}


static int posx=0, posy=0;
// returns VF3_1, VF3_2 or VF3_3
int vfQuestion3(QWidget* par, const std::string& title, const std::string& mess, const std::string& s1,
            const std::string& s2,  const std::string& s3, bool i_mov )
{
  QString titl, spac, messag;
  titl = title.c_str(); spac = "\n\n"; messag = mess.c_str();

   QMessageBox qm( QMessageBox::Question,
#ifdef __unix
#ifdef __APPLE__
         "Title", titl.append(spac+=messag),
#else
          titl, messag,
#endif
#else
          titl, messag,
#endif           
           QMessageBox::NoButton, par);

    QAbstractButton *yesButton = qm.addButton(s1.c_str(), QMessageBox::YesRole);
    QAbstractButton *noButton = qm.addButton(s2.c_str(), QMessageBox::NoRole);
    QAbstractButton *cancelButton = nullptr;
    if( !s3.empty() )
       cancelButton = qm.addButton(s3.c_str(), QMessageBox::RejectRole);
    if( i_mov )
        qm.move(posx, posy);
    qm.exec();
    if( i_mov )
    {
        posx = qm.x();
        posy = qm.y();
    }
    if (qm.clickedButton() == yesButton) {
        return VF3_1;
    }
    else if (qm.clickedButton() == noButton) {
        return VF3_2;
    }
    else if (qm.clickedButton() == cancelButton) {
        return VF3_3;
    }
    return VF3_3;
}

bool vfQuestion(QWidget* par, const std::string& title, const std::string& mess)
{
  QString titl, spac, messag;
  titl = title.c_str(); spac = "\n\n"; messag = mess.c_str();

  int rest = (QMessageBox::question(par,
#ifdef __unix
#ifdef __APPLE__
         "Title", titl.append(spac+=messag),
#else
         titl, messag,
#endif
#else
         titl, messag,
#endif
         QMessageBox::Yes | QMessageBox::No));
    return rest==QMessageBox::Yes;
}

std::vector<std::string> vfMultiKeys(QWidget* par, const char* caption,
                            int iRt, const char* key )
{
    std::vector<std::string> sel;
    DBKeyDialog dbk(par, iRt, sel, key, caption);
    dbk.exec();
    return dbk.allSelectedKeys();
}

std::vector<std::string> vfMultiKeysSet(QWidget* par, const char* caption,
                    int iRt, const char* key, std::vector<std::string>& sel )
{
    DBKeyDialog dbk(par, iRt, sel, key, caption);
    dbk.exec();
    return dbk.allSelectedKeys();
}

//--------------------------------------------------------------------------------------------------------------


DBKeyDialog::DBKeyDialog(QWidget* win, int irt, const char* key,
                     const char* caption, bool filter):
        QDialog( win),
        multi(false), iRT(irt)
{
 	setupUi(this);
	 
    old_sel.clear();
    //pList->setFont( pVisorImp->getCellFont() );
    QObject::connect(pList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
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
    pLabel->setText(s.c_str());

    s  = std::string(rtEJ[irt].PackKey());
    int n = rtEJ[irt].GetKeyList( keyFilter.c_str(), keyList);
    int sel = 0;

    for( int ii=0; ii<n; ii++ )
    {
        pList->addItem(keyList[ii].c_str());
        if( keyList[ii]==s )
            sel = ii;
    }

    pList->setSelectionMode(QAbstractItemView::SingleSelection);
    pList->setCurrentRow(sel);

    pButton3->hide();
    pButton2->hide();
    if( !filter )
      pFilterButton->hide();
    set_connect();
    pList->setFocus();
}

DBKeyDialog::DBKeyDialog(QWidget* win, int irt,
                     const std::vector<std::string>& sel,
                     const char* key, const char* caption):
        QDialog( win),
        multi(true), iRT(irt)
{
	setupUi(this);

    old_sel.clear();
    for(size_t ii=0; ii<sel.size(); ii++)
          old_sel.push_back( sel[ii] );

    //pList->setFont( pVisorImp->getCellFont() );
    pList->setSelectionMode(QAbstractItemView::MultiSelection);
    setWindowTitle( caption );

    if(!key)
       Error("KeyDialog", "pkey is null");
    else if( strpbrk(key, "*?") == 0 )
        keyFilter = "*";
    else
        keyFilter = key;

    SetList();
    set_connect();
    pList->setFocus();
}

DBKeyDialog::~DBKeyDialog()
{}

void DBKeyDialog::set_connect()
{
    QObject::connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );
    QObject::connect(PushButton1, &QPushButton::clicked, this, qOverload<>(&QDialog::accept));
    QObject::connect(PushButton3, &QPushButton::clicked, this, qOverload<>(&QDialog::reject));
    QObject::connect(pFilterButton, &QPushButton::clicked, this, qOverload<>(&DBKeyDialog::CmFilter));
    QObject::connect(pButton2, &QPushButton::clicked, this, qOverload<>(&DBKeyDialog::CmSelectAll));
    QObject::connect(pButton3, &QPushButton::clicked, this, qOverload<>(&DBKeyDialog::CmClearAll));
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
    pLabel->setText(s.c_str());

    int n = rtEJ[iRT].GetKeyList( keyFilter.c_str(), keyList );

    for( int ii=0; ii<n; ii++ )
       pList->addItem(keyList[ii].c_str());

    if( multi )
    {  for(size_t jj=0; jj<old_sel.size(); jj++)
          for( int ii=0; ii<n; ii++ )
          {
            // comparing parts before '*' for overwrite dcomp, reacdc ....
            size_t pos = old_sel[jj].find('*');
            std::string str(old_sel[jj], 0, pos);
            if( keyList[ii].find(str) != std::string::npos )
            {
              pList->item(ii)->setSelected( true);
              break;
            }
           }
    }
 }


std::string DBKeyDialog::getKey()
{
    int sel = pList->currentRow();
    if( sel != -1 )
    {
        /// !!!! ((TCModule*)aMod[iRt])->setFilter(keyFilter.c_str());
        std::string res = pList->item(sel)->text().toStdString();
        return res;
    }
    return std::string();
}


void DBKeyDialog::CmFilter()
{
    std::string str_name = "Template for ";
            str_name +=  rtEJ[iRT].GetKeywd();
            str_name += " record key";
    DBKeyFilter dbFilter(this, iRT, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        pList->clear();
        SetList();
        pList->setCurrentRow(0);
     }
}

void DBKeyDialog::CmSelectAll()
{
    // select all strings
    for( int ii=0; ii<pList->count(); ii++ )
        pList->item(ii)->setSelected( true);
}


void DBKeyDialog::CmClearAll()
{
    pList->clearSelection();
}

/// returns selection array
///    array is empty if nothing is selected
std::vector<std::string> DBKeyDialog::allSelectedKeys()
{
    std::vector<std::string> arr;

    if( !result() )
        return arr;

    for( int ii=0; ii<pList->count(); ii++ )
        if( pList->item(ii)->isSelected() )
        {
         std::string s = pList->item(ii)->text().toStdString();
         arr.push_back(s);
        }
    return arr;
}

//============================================================
// KeyEdit dialog
//============================================================

DBKeyFilter::DBKeyFilter(QWidget* win, int irt, const char* key,
                     const char* caption, bool allowTempl ):
        QDialog( win ),
        iRT(irt),
        allowTemplates(allowTempl)
{
    QLineEdit* pEdit;
    QLabel* pLabel;

    setWindowModality(Qt::WindowModal);
    setWindowTitle(caption);

    TEJDBKey dbKey( rtEJ[irt].GetDBKey() );
    if( key )
        dbKey.SetKey(key);

    QGridLayout* editBox = new QGridLayout();
    for( int ii=0; ii<dbKey.KeyNumFlds(); ii++)
    {
        aEdit.append( pEdit = new QLineEdit(this) );
        QString str = rtEJ[irt].FldKeyName(ii);
        pEdit->setToolTip( str );
        pEdit->setMaxLength( 100 );
        pEdit->setText( dbKey.FldKey(ii) );
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
    fullKey->setText(dbKey.PackKey());
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
   dbName += std::string(rtEJ[iRT].GetKeywd());
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

    TEJDBKey dbKey( rtEJ[iRT].GetDBKey() );
    dbKey.SetKey(dlg.getKey().c_str());

    for( int ii=0; ii<dbKey.KeyNumFlds(); ii++)
        aEdit[ii]->setText( dbKey.FldKey(ii) );
    setKeyLine();
}

std::string DBKeyFilter::getFilter()
{
    return SetKeyString();
}

//--------------------- End of DBKeyDialog.cpp ---------------------------

