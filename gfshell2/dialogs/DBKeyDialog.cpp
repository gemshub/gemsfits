//-------------------------------------------------------------------
// $Id: KeyDialog.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of KeyDialog class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
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

#include <QListWidget>


#include "DBKeyDialog.h"
#include "FITMainWindow.h"
#include "f_ejdb.h"

/*
DBKeyDialog::DBKeyDialog(QWidget* win, int irt, const char* key,
                     const char* caption, bool filter):
        QDialog( win),
        multi(false), iRt(irt)
{
 	setupUi(this);
	 
    old_sel.clear();
    //pList->setFont( pVisorImp->getCellFont() );
    QObject::connect(pList, SIGNAL(itemDoubleClicked ( QListWidgetItem *) ), this, SLOT(accept()));
    setWindowTitle( caption );
    vector<string> keyList;

    bool yesFilter = false;

    ErrorIf(!key, "DBKeyDialog", "Record Key is null");

    if( strpbrk(key, "*?") == 0 )
        keyFilter = "*";
    else
    {
        keyFilter = key;
        yesFilter = true;
    }

    string s = "Please, select one record key. Filter: ";
    s +=  keyFilter;
    pLabel->setText(s.c_str());

    s  = string(rtEJ[irt].PackKey());
    int n = rtEJ[irt].GetKeyList( keyFilter.c_str(), keyList);
    int sel = 0;

    for( int ii=0; ii<n; ii++ )
    {
        pList->addItem(keyList[ii].c_str());
        if( keyList[ii]==s )
            sel = ii;
    }

    pList->setSelectionMode(QAbstractItemView::SingleSelection); // pList->setMultiSelection(false);
    if( sel < 0 || sel > n )
	sel = 0;
    pList->setCurrentRow(sel); // pList->setSelected(sel, true);
    pButton3->hide();
    pButton2->hide();
    QObject::connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );


    pList->setFocus();

    if( !filter )
        pFilterButton->hide();
}

DBKeyDialog::DBKeyDialog(QWidget* win, int irt,
                     const vector<string>& sel,
                     const char* key, const char* caption):
        QDialog( win),
        multi(true), iRt(irt)
{
	setupUi(this);

    old_sel.clear();
    for(int ii=0; ii<sel.size(); ii++)
    {   if( strchr( sel[ii].c_str(), ':' ))  // key in packed form
        {
          rt[irt].SetKey( sel[ii].c_str() );
          old_sel.push_back(rt[irt].UnpackKey());
        }
        else
          old_sel.push_back( sel[ii] );
    }
    
    pList->setFont( pVisorImp->getCellFont() );
    pList->setSelectionMode(QAbstractItemView::MultiSelection);  // pList->setMultiSelection(true);
    setWindowTitle( caption );
    QObject::connect( bHelp, SIGNAL( clicked() ), this, SLOT( CmHelp() ) );

    ErrorIf(!key, "KeyDialog", "pkey is null");
    if( strpbrk(key, "*?") == 0 )
        keyFilter = "*";
    else
       keyFilter = key;

    SetList();
    pList->setFocus();

}


DBKeyDialog::~DBKeyDialog()
{}

void DBKeyDialog::CmHelp()
{
  pFitImp->OpenHelp( GEMS_SELECT_HTML );
}

void DBKeyDialog::languageChange()
{
    retranslateUi(this);
}

void DBKeyDialog::SetList()
{
    vector<string> keyList;

    string s = "Please, mark one or more record keys. Filter: ";
    s +=  keyFilter;
    pLabel->setText(s.c_str());

    int n = rt[iRt].GetKeyList( keyFilter.c_str(), keyList );

    for( int ii=0; ii<n; ii++ )
       pList->addItem(keyList[ii].c_str());

    if( multi )
    {  for(int jj=0; jj<old_sel.size(); jj++)
          for( int ii=0; ii<n; ii++ )
          {
            // comparing parts before '*' for overwrite dcomp, reacdc ....
            size_t pos = old_sel[jj].find('*');
            string str(old_sel[jj], 0, pos);
            if( keyList[ii].find(str) != string::npos )
            {
            	pList->item(ii)->setSelected( true); //pList->setSelected(ii, true);
              break;
            }
           }
      }
 }



// olvase unpaked keys?
string DBKeyDialog::getKey()
{
    int sel = pList->currentRow(); //pList->currentItem();
    if( sel != -1 )
    {
        ((TCModule*)aMod[iRt])->setFilter(keyFilter.c_str());
        string res;
        string s = pList->item(sel)->text().toUtf8().data();
        //string s = ss;
        int ln;
        for( int ii=0, jj=0; ii<rt[iRt].KeyNumFlds(); ii++)
        {
          //pos = strchr( s+jj, ":" );
          ln = rt[iRt].FldLen(ii);
          // if( pos) ln = min( ln, pos-s+jj );

          //res += string(s+jj, 0, ln);
          res += string(s, jj, ln);
          strip(res);
          res += ":";
          jj += ln;
        }
        return res;
    }
    return string();
}


void
DBKeyDialog::CmFilter()
{
    string str_name = "Template for ";
            str_name +=  rt[iRt].GetKeywd();
            str_name += " record key";
    KeyFilter dbFilter(this, iRt, keyFilter.c_str(), str_name.c_str() );
    if( dbFilter.exec() )
    {
        keyFilter = dbFilter.getFilter();
        pList->clear();
        SetList();
        pList->setCurrentRow(0);
     }
}

void
DBKeyDialog::CmSelectAll()
{
    // select all gstrings
    for( int ii=0; ii<pList->count(); ii++ )
        pList->item(ii)->setSelected( true);
}


void
DBKeyDialog::CmClearAll()
{
    pList->clearSelection();
}

/// returns selection array
///    array is empty if nothing is selected


vector<string>
DBKeyDialog::allSelectedKeys()
{
    vector<string> arr;

    if( !result() )
        return arr;

    for( int ii=0; ii<pList->count(); ii++ )
        if( pList->item(ii)->isSelected() )
        {
         string s = pList->item(ii)->text().toUtf8().data();;
         arr.push_back(s);
        }
    return arr;
}

//============================================================
// KeyEdit dialog
//============================================================

KeyFilter::KeyFilter(QWidget* win, int irt, const char* key,
                     const char* caption, bool allowTempl ):
        QDialog( win ),
        iRt(irt),
        allowTemplates(allowTempl)
{
    QLineEdit* pEdit;
    QLabel* pLabel;

    setWindowModality(Qt::WindowModal);
    setWindowTitle(caption);

    TEJDBKey dbKey( rt[irt].GetDBKey() );

    if( key )
        dbKey.SetKey(key);

    QGridLayout* editBox = new QGridLayout();
    int editLine = dynamic_cast<TCModule*>(aMod[iRt])->keyEditField();

    for( int ii=0; ii<dbKey.KeyNumFlds(); ii++)
    {
        aEdit.append( pEdit = new QLineEdit(this) );
        QString str = ((TCModule*)aMod[irt])->GetFldHelp(ii);
        pEdit->setToolTip( str);
        pEdit->setMaxLength( dbKey.FldLen(ii) );
        pEdit->setMaximumWidth( (dbKey.FldLen(ii)+2) * pVisorImp->getCharWidth() );
        pEdit->setMinimumWidth( (dbKey.FldLen(ii)+2) * pVisorImp->getCharWidth() );
        string s = dbKey.FldKey(ii);
        pEdit->setText( s.c_str() );
        connect( pEdit, SIGNAL(editingFinished ()), this, SLOT(setKeyLine()) );

        editBox->addWidget( pEdit, ii, 0, Qt::AlignRight);
        pLabel = new QLabel( str, this);
        editBox->addWidget( pLabel, ii, 1);

        if( !allowTemplates && ii < editLine )
            pEdit->setEnabled(false);
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
// qt3to4    btn->setAccel( Qt::Key_Escape );
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
    // setLayout(mainBox);
}

///  pVisor->SetStatus( aMod[iRt]->GetFldHelp(ii).c_str() );
void
KeyFilter::CmHelp()
{
   string dbName =  DBM;
   dbName +="_";
   dbName += string(aMod[iRt]->GetName());
   pVisorImp->OpenHelp(  GEMS_REKEY_HTML, dbName.c_str() );
}

void
KeyFilter::CmOk()
{
    if( allowTemplates || SetKeyString().find_first_of("*?") == string::npos )
    {
        accept();
        return;
    }

    vfMessage(this, "Key error", "No templates allowed!", vfErr);
}

string
KeyFilter::SetKeyString()
{
    TEJDBKey dbKey( rt[iRt].GetDBKey() );
    string Key;

    Key = "";
    for( int ii=0, jj=0; ii<aEdit.count(); ii++, jj=Key.length())
    {
        string s = aEdit[ii]->text().toUtf8().data();
        Key += s;
        strip(Key);
//Sveta 04/09/01 ????  if( Key.length()-jj < dbKey.FldLen(ii) )
            Key += ":";
    }

    return Key;
}

void
KeyFilter::setKeyLine()
{
    fullKey->setText(SetKeyString().c_str());
}

void
KeyFilter::EvSetAll()
{
    for( int ii=0; ii<aEdit.count(); ii++ )
        aEdit[ii]->setText("*");
    setKeyLine();
}

void
KeyFilter::EvGetList()
{
    KeyDialog dlg(this, iRt, SetKeyString().c_str(), "Key template", false);
    if( !dlg.exec() )
        return;

    TEJDBKey dbKey( rt[iRt].GetDBKey() );

    dbKey.SetKey(dlg.getKey().c_str());

    for( int ii=0; ii<dbKey.KeyNumFlds(); ii++)
    {
        aEdit[ii]->setText( dbKey.FldKey(ii) );
    }
    setKeyLine();
}

string
KeyFilter::getFilter()
{
    //  if( result )
    return SetKeyString();
}


*/
//--------------------- End of DBKeyDialog.cpp ---------------------------

