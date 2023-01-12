
TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gemsfits
VERSION         = 1.2.1

DEFINES         += Use_mt_mode
#DEFINES         += NODEARRAYLEVEL
DEFINES         += IPMGEMPLUGIN
DEFINES         += _MYNOZLIB
DEFINES         += USEBSON

CONFIG -= warn_on
CONFIG += warn_off
CONFIG += thread
CONFIG += c++11
QT += network
QT += sql
QT += xml
QT += svg

QMAKE_CXXFLAGS += -O3
QMAKE_LFLAGS += -O3

lessThan( QT_MAJOR_VERSION, 5 ): CONFIG += help
greaterThan( QT_MAJOR_VERSION, 4 ): QT += widgets printsupport help concurrent


!win32 {
  DEFINES += __unix
}

win32 {
  DEFINES         += buildWIN32
}

macx-g++ {
  DEFINES += __APPLE__
}

macx-clang {
  DEFINES += __APPLE__
}

RESOURCES      = ./fgui.qrc
#QWT6_CPP       =  ./QWT
DATAMAN_CPP    =  ./dataman
DIALOGS_CPP   =  ./dialogs
GEMS3K_CPP     =  ../../standalone/GEMS3K
KEYS_CPP     =  ../csvtoejdb/src-csvtoejdb

#QWT6_H       =  $$QWT6_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS_H    =  $$DIALOGS_CPP
GEMS3K_H     =  $$GEMS3K_CPP
KEYS_H       =   $$KEYS_CPP

#DEPENDPATH   += $$QWT6_H
DEPENDPATH   += $$DATAMAN_H
DEPENDPATH   += $$DIALOGS_H
DEPENDPATH   += $$GEMS3K_H
DEPENDPATH   += $$KEYS_H

INCLUDEPATH   += $$QWT6_H
INCLUDEPATH   += $$DATAMAN_H
INCLUDEPATH   += $$DIALOGS_H
INCLUDEPATH   += $$GEMS3K_H
INCLUDEPATH   += $$KEYS_H


INCLUDEPATH += $$PWD/../gemsfit2/boost_win32
YAML_PATH = ./YAML
INCLUDEPATH += $$YAML_PATH/include

MOC_DIR = tmp
UI_DIR        = $$MOC_DIR
OBJECTS_DIR   = obj

include($$DIALOGS_CPP/dialogs.pri)
include($$GEMS3K_CPP/gems3k.pri)
#include($$QWT6_CPP/qwt.pri)
include($$DATAMAN_CPP/dataman.pri)
include($$YAML_PATH/yaml.pri)

#TCEJDB
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../gemsfit2/tcejdb_win32-64/bin/ -llibejdb
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../gemsfit2/tcejdb_win32-64/bin/ -llibejdb
INCLUDEPATH += $$PWD/../gemsfit2/tcejdb_win32-64/include/ejdb
DEPENDPATH += $$PWD/../gemsfit2/tcejdb_win32-64/include/ejdb

#BOOST
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../gemsfit2/boost_win32-64/lib/ -llibboost_filesystem-mt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../gemsfit2/boost_win32-64/lib/ -llibboost_filesystem-mt
#INCLUDEPATH += $$PWD/../gemsfit2/boost_win32-64/include
#DEPENDPATH += $$PWD/../gemsfit2/boost_win32-64/include


