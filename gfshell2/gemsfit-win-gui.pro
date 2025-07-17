
TEMPLATE	= app
LANGUAGE        = C++
TARGET		= fits_shell
VERSION         = 2.0.0

#CONFIG -= warn_on
#CONFIG += warn_off
CONFIG += thread
CONFIG += c++17

QT   += core gui widgets
QT   += svg printsupport concurrent
QT   += charts help
#QT   += network sql xml

DEFINES         += OLD_EJDB # compile using ejdb1
DEFINES         += IPMGEMPLUGIN
#DEFINES += NODEARRAYLEVEL
#DEFINES += USE_NLOHMANNJSON
#DEFINES += USE_THERMOFUN
#DEFINES += USE_THERMO_LOG

QMAKE_CXXFLAGS += -O3 -fno-inline-small-functions -DIW_STATIC
QMAKE_LFLAGS += -O3 -fno-inline-small-functions
QMAKE_CFLAGS += -DIW_STATIC


!win32 {
  DEFINES += __unix
}

win32 {
  DEFINES         += buildWIN32
}

RESOURCES      = ./fgui.qrc

CHARTS_CPP   =  ./charts
DATAMAN_CPP  =  ./dataman
DIALOGS_CPP  =  ./dialogs
COMMON_CPP  =  ../common
GEMS3K_CPP   =  ../standalone/GEMS3K

CHARTS_H     =  $$CHARTS_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS_H    =  $$DIALOGS_CPP
COMMON_H     =  $$COMMON_CPP
GEMS3K_H     =  $$GEMS3K_CPP


EJDB_PATH = ../../standalone/EJDB
win32{
   EJDB_LIB_PATH =  $$EJDB_PATH/build-win32
}


EJDB_BSON_H = $$EJDB_PATH/src/bson
EJDB_EJDB_H = $$EJDB_PATH/src/ejdb
EJDB_TCUTIL_H = $$EJDB_PATH/src/tcutil
#EJDB_GENERATED_H = $$EJDB_LIB_PATH/debug/src/generated
CONFIG(release, debug|release): EJDB_GENERATED_H = $$EJDB_LIB_PATH/release/src/generated
CONFIG(debug, debug|release): EJDB_GENERATED_H = $$EJDB_LIB_PATH/debug/src/generated

DEPENDPATH   += $$CHARTS_H
DEPENDPATH   += $$DATAMAN_H
DEPENDPATH   += $$DIALOGS_H
DEPENDPATH   += $$COMMON_H
DEPENDPATH   += $$GEMS3K_H
DEPENDPATH   += $$KEYS_H

INCLUDEPATH   += $CHARTS_H
INCLUDEPATH   += $$DATAMAN_H
INCLUDEPATH   += $$DIALOGS_H
INCLUDEPATH   += $$COMMON_H
INCLUDEPATH   += $$GEMS3K_H
INCLUDEPATH   += $$KEYS_H

MOC_DIR = tmp
UI_DIR        = $$MOC_DIR
OBJECTS_DIR   = obj


INCLUDEPATH += $$PWD/../gemsfit2/boost_win32
YAML_PATH = ./YAML
INCLUDEPATH += $$YAML_PATH/include

MOC_DIR = tmp
UI_DIR        = $$MOC_DIR
OBJECTS_DIR   = obj

include($$GEMS3K_CPP/gems3k.pri)
include($$CHARTS_CPP/charts.pri)
include($$DATAMAN_CPP/dataman.pri)
include($$DIALOGS_CPP/dialogs.pri)
include($$COMMON_CPP/common.pri)

contains(DEFINES, OLD_EJDB) {
CONFIG(release, debug|release): LIBS += -L$$PWD/../gemsfit2/tcejdb_win32/lib/ -ltcejdbdll
CONFIG(debug, debug|release): LIBS += -L$$PWD/../gemsfit2/tcejdb_win32/lib/ -ltcejdbdll
}
else
{  # must be changed
CONFIG(release, debug|release): LIBS += -L$$PWD/../gemsfit2/tcejdb_win32/lib/ -ltcejdbdll
CONFIG(debug, debug|release): LIBS += -L$$PWD/../gemsfit2/tcejdb_win32/lib/ -ltcejdbdll
}


#BOOST
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../gemsfit2/boost_win32/filesystem/build/gcc-mingw-4.8.2/release/link-static/threading-multi/ -lboost_filesystem-mgw48-mt-1_57
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../gemsfit2/boost_win32/filesystem/build/gcc-mingw-4.8.2/release/link-static/threading-multi/ -lboost_filesystem-mgw48-mt-1_57
