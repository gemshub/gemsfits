
TEMPLATE	= app
LANGUAGE        = C++
TARGET		= gemsfits
VERSION         = 1.4.0

DEFINES         += Use_mt_mode
#DEFINES         += NODEARRAYLEVEL
DEFINES         += IPMGEMPLUGIN
DEFINES         += _MYNOZLIB
DEFINES         += USEBSON

#CONFIG -= warn_on
#CONFIG += warn_off
CONFIG += thread
CONFIG += c++17

QT   += core gui widgets
QT   += svg printsupport concurrent
QT   += charts help

QMAKE_CXXFLAGS += -O3
QMAKE_LFLAGS += -O3

!win32 {
  DEFINES += __unix
  INCLUDEPATH   += "/usr/local/include/ejdb"
}

macx-g++ {
  DEFINES += __APPLE__
}


macx-clang {
  DEFINES += __APPLE__
  INCLUDEPATH   += "/usr/local/include"
}
else {
}


RESOURCES      = ./fgui.qrc

CHARTS_CPP   =  ./charts
DATAMAN_CPP  =  ./dataman
DIALOGS_CPP  =  ./dialogs
GEMS3K_CPP   =  ../../standalone/GEMS3K
KEYS_CPP     =  ../csvtoejdb/src-csvtoejdb

CHARTS_H     =  $$CHARTS_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS_H    =  $$DIALOGS_CPP
GEMS3K_H     =  $$GEMS3K_CPP
KEYS_H       =   $$KEYS_CPP

#EJDB_PATH = ../../standalone/EJDB
#YAML_PATH = $$PWD/YAML

win32{
   EJDB_LIB_PATH =  $$EJDB_PATH/build-win32
}
#unix{
#   EJDB_LIB_PATH =  $$EJDB_PATH/build
#}
#app{
#   EJDB_LIB_PATH =  $$EJDB_PATH/build
#}

#YAML_LIB_PATH =  $$YAML_PATH/build
#YAML_H =  $$YAML_PATH/include
#YAML_H2 =  $$YAML_PATH/src

#EJDB_BSON_H = $$EJDB_PATH/src/bson
#EJDB_EJDB_H = $$EJDB_PATH/src/ejdb
#EJDB_TCUTIL_H = $$EJDB_PATH/src/tcutil
#EJDB_GENERATED_H = $$EJDB_LIB_PATH/debug/src/generated
#CONFIG(release, debug|release): EJDB_GENERATED_H = $$EJDB_LIB_PATH/release/src/generated
#CONFIG(debug, debug|release): EJDB_GENERATED_H = $$EJDB_LIB_PATH/debug/src/generated

DEPENDPATH   += $$CHARTS_H
DEPENDPATH   += $$DATAMAN_H
DEPENDPATH   += $$DIALOGS_H
DEPENDPATH   += $$GEMS3K_H
DEPENDPATH   += $$KEYS_H

INCLUDEPATH   += $CHARTS_H
INCLUDEPATH   += $$DATAMAN_H
INCLUDEPATH   += $$DIALOGS_H
INCLUDEPATH   += $$GEMS3K_H
INCLUDEPATH   += $$KEYS_H
#INCLUDEPATH   += $$EJDB_BSON_H
#INCLUDEPATH   += $$EJDB_EJDB_H
#INCLUDEPATH   += $$EJDB_GENERATED_H
#INCLUDEPATH   += $$EJDB_TCUTIL_H
#INCLUDEPATH   += $$YAML_H
#INCLUDEPATH   += $$YAML_H2

MOC_DIR = tmp
UI_DIR        = $$MOC_DIR
OBJECTS_DIR   = obj

include($$DIALOGS_CPP/dialogs.pri)
include($$GEMS3K_CPP/gems3k.pri)
include($$CHARTS_CPP/charts.pri)
include($$DATAMAN_CPP/dataman.pri)
#include($$EJDB_PATH/ejdb.pri)

CONFIG(release, debug|release): LIBS += -lejdb -lyaml-cpp
CONFIG(debug, debug|release): LIBS += -lejdb -lyaml-cpp

#CONFIG(release, debug|release): LIBS += -L$$YAML_LIB_PATH/release/ -lyaml-cpp
#CONFIG(debug, debug|release): LIBS += -L$$YAML_LIB_PATH/debug/ -lyaml-cpp

#CONFIG(release, debug|release): LIBS += -L$$EJDB_LIB_PATH/release/src/ -lejdb
#CONFIG(debug, debug|release): LIBS += -L$$EJDB_LIB_PATH/debug/src/ -lejdb

