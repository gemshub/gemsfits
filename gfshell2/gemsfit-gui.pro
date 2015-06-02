
TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gemsfits
#VERSION         = 1.0.0

DEFINES         += Use_mt_mode
#DEFINES         += NODEARRAYLEVEL
DEFINES         += IPMGEMPLUGIN
DEFINES         += _MYNOZLIB
DEFINES         += USEBSON

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
GEMS3K_CPP     =  ../../standalone/GEMS3K
KEYS_CPP     =  ../csvtoejdb/src-csvtoejdb

QWT6_H       =  $$QWT6_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS_H    =  $$DIALOGS_CPP
GEMS3K_H     =  $$GEMS3K_CPP
KEYS_H       =   $$KEYS_CPP

EJDB_PATH = ../../standalone/EJDB

win32{
   EJDB_LIB_PATH =  $$EJDB_PATH/build-win32
}
unix{
   EJDB_LIB_PATH =  $$EJDB_PATH/build
}

EJDB_BSON_H = $$EJDB_PATH/src/bson
EJDB_EJDB_H = $$EJDB_PATH/src/ejdb
EJDB_TCUTIL_H = $$EJDB_PATH/src/tcutil
#EJDB_GENERATED_H = $$EJDB_LIB_PATH/debug/src/generated
CONFIG(release, debug|release): EJDB_GENERATED_H = $$EJDB_LIB_PATH/release/src/generated
CONFIG(debug, debug|release): EJDB_GENERATED_H = $$EJDB_LIB_PATH/debug/src/generated

DEPENDPATH   += $$QWT6_H
DEPENDPATH   += $$DATAMAN_H
DEPENDPATH   += $$DIALOGS_H
DEPENDPATH   += $$GEMS3K_H
DEPENDPATH   += $$KEYS_H

INCLUDEPATH   += $$QWT6_H
INCLUDEPATH   += $$DATAMAN_H
INCLUDEPATH   += $$DIALOGS_H
INCLUDEPATH   += $$GEMS3K_H
INCLUDEPATH   += $$KEYS_H
INCLUDEPATH   += $$EJDB_BSON_H
INCLUDEPATH   += $$EJDB_EJDB_H
INCLUDEPATH   += $$EJDB_GENERATED_H
INCLUDEPATH   += $$EJDB_TCUTIL_H


MOC_DIR = tmp
UI_DIR        = $$MOC_DIR
OBJECTS_DIR   = obj

#include($$DATAMAN_CPP/dataman.pri)
include($$DIALOGS_CPP/dialogs.pri)
include($$GEMS3K_CPP/gems3k.pri)
include($$QWT6_CPP/qwt.pri)
include($$DATAMAN_CPP/dataman.pri)
include($$EJDB_PATH/ejdb.pri)

CONFIG(release, debug|release): LIBS += -L$$EJDB_LIB_PATH/release/src/ -lejdb
CONFIG(debug, debug|release): LIBS += -L$$EJDB_LIB_PATH/debug/src/ -lejdb
