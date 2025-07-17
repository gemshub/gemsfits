//-------------------------------------------------------------------
// $Id: main.cpp 333 2014-03-13 13:23:32Z gems $
//
// Implementation of GEMSFITS GUI Main Window, function main()
//
// Copyright (C) 2014-2023  S.V.Dmytriyeva, G.D.Miron, D.A.Kulik
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

#include <iostream>
#include <QApplication>
#include <QSqlDatabase>
#include <QtWidgets>
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
    std::cout << "QSqlDatabase: available drivers:" <<
                 QSqlDatabase::drivers().join(QLatin1String(" ")).toLatin1().data() << std::endl;

    shMemory.setKey("gemsfits");
    if( shMemory.attach())
    {
        _isRunning = true;
    }
    else
    {
        _isRunning = false;
        if( !shMemory.create(10))
        {
            return;
        }

        setStyle(QStyleFactory::create("windows"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/modules/Icons/UnSpaceModuleIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
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

int main(int argc, char* argv[])
{
    TFITApp IntegApp(argc, argv);

    if(IntegApp.isRunning())
    {
        std::cerr << "gemsfits: Unable to create second instance." << std::endl;
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
        std::cerr << "gemsfits: " << err.mess.c_str() << std::endl;
        std::string s = err.title;
        s += ": ";
        s += err.mess;
        QMessageBox::critical(0, "FIT fatal error", s.c_str());
    }
    catch(...)
    {
        std::cerr << "gemsfits: Unknown exception: program aborted" << std::endl;
        return -1;
    }
    return 0;
}

//--------------------- End of main.cpp ---------------------------

