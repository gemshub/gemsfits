
TEMPLATE	= app
LANGUAGE        = C++
TARGET		= fits_shell
VERSION         = 1.4.0

#CONFIG -= warn_on
#CONFIG += warn_off
CONFIG += thread
CONFIG += c++17

DEFINES         += OLD_EJDB # compile using ejdb1

#DEFINES         += Use_mt_mode
DEFINES         += IPMGEMPLUGIN
#DEFINES += NODEARRAYLEVEL
#DEFINES += USE_NLOHMANNJSON
#DEFINES += USE_THERMOFUN
#DEFINES += USE_THERMO_LOG

QT   += core gui widgets
QT   += svg printsupport concurrent
QT   += charts help

QMAKE_CXXFLAGS += -O3 -fno-inline-small-functions -DIW_STATIC
QMAKE_LFLAGS += -O3 -fno-inline-small-functions
QMAKE_CFLAGS += -DIW_STATIC

!win32 {
  DEFINES += __unix
  INCLUDEPATH   += "/usr/local/include/ejdb"
  INCLUDEPATH   += "/usr/local/include/ejdb2"
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
COMMON_CPP  =  ../common
GEMS3K_CPP   =  ../gems3k/GEMS3K

CHARTS_H     =  $$CHARTS_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS_H    =  $$DIALOGS_CPP
COMMON_H     =  $$COMMON_CPP
GEMS3K_H     =  $$GEMS3K_CPP


win32{
   EJDB_LIB_PATH =  $$EJDB_PATH/build-win32
}

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

include($$GEMS3K_CPP/gems3k.pri)
include($$CHARTS_CPP/charts.pri)
include($$DATAMAN_CPP/dataman.pri)
include($$DIALOGS_CPP/dialogs.pri)
include($$COMMON_CPP/common.pri)

contains(DEFINES, OLD_EJDB) {
CONFIG(release, debug|release): LIBS += -lejdb -lyaml-cpp
CONFIG(debug, debug|release): LIBS += -lejdb -lyaml-cpp
}
else
{
CONFIG(release, debug|release): LIBS += -lejdb2 -lyaml-cpp
CONFIG(debug, debug|release): LIBS += -lejdb2 -lyaml-cpp
}


