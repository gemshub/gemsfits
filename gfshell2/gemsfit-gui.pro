
TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gemsfits
#VERSION         = 1.0.0

DEFINES         += Use_mt_mode
#DEFINES         += NODEARRAYLEVEL
DEFINES         += IPMGEMPLUGIN
DEFINES         += _MYNOZLIB

CONFIG -= warn_on
CONFIG += warn_off
CONFIG += thread
QT += network
QT += sql
QT += xml
QT += svg

lessThan( QT_MAJOR_VERSION, 5 ): CONFIG += help
greaterThan( QT_MAJOR_VERSION, 4 ): QT += widgets printsupport help concurrent


!win32 {
  DEFINES += __unix
  PLATFORM_CPP       =  ../gemsfit2/tcejdb/nix
}

win32 {
  PLATFORM_CPP       =  ../gemsfit2/tcejdb/win32
}

macx-g++ {
  DEFINES += __APPLE__
}


macx-clang {
  DEFINES += __APPLE__
}
else {
 QMAKE_CXXFLAGS += -std=gnu99
 QMAKE_CFLAGS += -std=gnu99
}

RESOURCES      = ./fgui.qrc

QWT6_CPP       =  ./QWT
DATAMAN_CPP    =  ./dataman
DIALOGS_CPP   =  ./dialogs
EJDB_CPP       =  ../gemsfit2/tcejdb
GEMS3K_CPP     =  ../../standalone/GEMS3K

QWT6_H       =  $$QWT6_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS_H    =  $$DIALOGS_CPP
EJDB_H       =  $$EJDB_CPP
GEMS3K_H     =  $$GEMS3K_CPP
PLATFORM_H     =  $$PLATFORM_CPP


DEPENDPATH   += $$QWT6_H
DEPENDPATH   += $$DATAMAN_H
DEPENDPATH   += $$DIALOGS_H
DEPENDPATH   += $$EJDB_H
DEPENDPATH   += $$GEMS3K_H
DEPENDPATH   += $$PLATFORM_H

INCLUDEPATH   += $$QWT6_H
INCLUDEPATH   += $$DATAMAN_H
INCLUDEPATH   += $$DIALOGS_H
INCLUDEPATH   += $$EJDB_H
INCLUDEPATH   += $$GEMS3K_H
INCLUDEPATH   += $$PLATFORM_H

MOC_DIR = tmp
UI_DIR        = $$MOC_DIR
OBJECTS_DIR   = obj

#include($$DATAMAN_CPP/dataman.pri)
include($$DIALOGS_CPP/dialogs.pri)
include($$GEMS3K_CPP/gems3k.pri)
include($$QWT6_CPP/qwt.pri)
include($$EJDB_CPP/tcejdb.pri)
include($$DATAMAN_CPP/dataman.pri)


