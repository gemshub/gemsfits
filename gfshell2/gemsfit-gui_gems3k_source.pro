
TEMPLATE	= app
LANGUAGE        = C++
TARGET		= fits_shell
VERSION         = 2.0.0

#CONFIG -= warn_on
#CONFIG += warn_off
CONFIG += thread
CONFIG += c++20

QT   += core gui widgets
QT   += svg printsupport concurrent
QT   += charts help

DEFINES         += OLD_EJDB # compile using ejdb1
DEFINES         += IPMGEMPLUGIN
#DEFINES += NODEARRAYLEVEL
#DEFINES += USE_NLOHMANNJSON
DEFINES += USE_THERMOFUN
DEFINES += USE_THERMO_LOG
DEFINES += USE_GEMS3K_SOURCE

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

CHARTS_H     =  $$CHARTS_CPP
DATAMAN_H    =  $$DATAMAN_CPP
DIALOGS_H    =  $$DIALOGS_CPP
COMMON_H     =  $$COMMON_CPP

win32{
   EJDB_LIB_PATH =  $$EJDB_PATH/build-win32
}

DEPENDPATH   += $$CHARTS_H
DEPENDPATH   += $$DIALOGS_H
DEPENDPATH   += $$COMMON_H
DEPENDPATH   += $$GEMS3K_H
DEPENDPATH   += $$KEYS_H

INCLUDEPATH   += $CHARTS_H
INCLUDEPATH   += $$DATAMAN_H
INCLUDEPATH   += $$DIALOGS_H
INCLUDEPATH   += $$COMMON_H
INCLUDEPATH   += $$KEYS_H

MOC_DIR = tmp
UI_DIR        = $$MOC_DIR
OBJECTS_DIR   = obj

include($$CHARTS_CPP/charts.pri)
include($$DATAMAN_CPP/dataman.pri)
include($$DIALOGS_CPP/dialogs.pri)
include($$COMMON_CPP/common.pri)

contains(DEFINES, OLD_EJDB) {
message("OLD_EJDB is defined")
CONFIG(release, debug|release): LIBS += -lejdb -lyaml-cpp
CONFIG(debug, debug|release): LIBS += -lejdb -lyaml-cpp
}
else
{
message("OLD_EJDB is NOT defined")
# CONFIG(release, debug|release): LIBS += -lejdb2 -lyaml-cpp
# CONFIG(debug, debug|release): LIBS += -lejdb2 -lyaml-cpp
}

contains(DEFINES, USE_GEMS3K_SOURCE) {
    message("USE_GEMS3K_SOURCE is defined")
    GEMS3K_CPP     =  ../GEMS3K/GEMS3K
    GEMS3K_H       =  $$GEMS3K_CPP
    DEPENDPATH     += $$GEMS3K_H
    INCLUDEPATH    += $$GEMS3K_H
    include($$GEMS3K_CPP/gems3k.pri)
} else {
    message("USE_GEMS3K_SOURCE is NOT defined")
    INCLUDEPATH   += "/usr/local/include/GEMS3K"
    DEPENDPATH    += "/usr/local/include/GEMS3K"
    LIBS          += -lGEMS3K
}

contains(DEFINES, USE_THERMOFUN) {
  LIBS += -lThermoFun -lChemicalFun
} ## end USE_THERMOFUN

