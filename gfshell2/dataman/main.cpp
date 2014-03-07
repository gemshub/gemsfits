//-------------------------------------------------------------------
// $Id: main.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation and Declaration of TIntegApp class, function main()
//
// Copyright (C) 1996-2001  S.Dmytriyeva, A.Rysin
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


#include <QApplication>
#include <QSqlDatabase>

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QApplication>
#include <QSqlDatabase>
#include <QSharedMemory>
#include <QMainWindow>
#include <QMessageBox>
#include <QWindowsStyle>
#endif

#include <iostream>
#include "FITMainWindow.h"

class TFITApp:  public QApplication
{
  int argc;
  char** argv;

  bool _isRunning;
  QSharedMemory shMemory;

public:
    TFITApp(int& c, char** v);
    void InitMainWindow();
    bool isRunning()
    { return _isRunning; }
};

TFITApp::TFITApp(int& c, char** v):
        QApplication( c, v),
      argc(c),
      argv(v)
{
    cout << "QSqlDatabase: available drivers:" <<
          QSqlDatabase::drivers().join(QLatin1String(" ")).toLatin1().data() << endl;

    shMemory.setKey("gemsfits");
    if( shMemory.attach())
    {
        _isRunning = true;
    }
    else
    {  _isRunning = false;
       if( !shMemory.create(10))
        {
           return;
         }

#if QT_VERSION >= 0x050000
#include <QtWidgets>
       setStyle(QStyleFactory::create("windows"));
#else
       setStyle( new QWindowsStyle() );
#endif

       QIcon icon;
       icon.addFile(QString::fromUtf8(":/Icons/gems50.png"), QSize(), QIcon::Normal, QIcon::Off);
       setWindowIcon(icon);
   }
}

void TFITApp::InitMainWindow()
{
    pFitImp = new FITMainWindow(argc, argv);
    pFitImp->show();
// init Help Window
    pFitImp->GetHelp();
}

int
main(int argc, char* argv[])
{
    TFITApp IntegApp(argc, argv);

    if(IntegApp.isRunning())
    {
       cerr << "gemsfits: Unable to create second instance." << endl;
       return -2;
    }
    try
    {
        IntegApp.InitMainWindow();
        int res = IntegApp.exec();
        delete pFitImp;

        return res;
    }
    catch(TError& err)
    {
        cerr << "gemsfits: " << err.mess.c_str() << endl;
        string s = err.title;
        s += ": ";
        s += err.mess;
        QMessageBox::critical(0, "FIT fatal error", s.c_str());
    }
    catch(...)
    {
        cerr << "gemsfits: Unknown exception: program aborted" << endl;
        return -1;
    }
    return 0;
}

//--------------------- End of main.cpp ---------------------------

