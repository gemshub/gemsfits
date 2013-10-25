
# csvtoejdb.pro

TEMPLATE        = app
CONFIG         += console
CONFIG         -= qt

CONFIG         -= qt
CONFIG         += warn_on
CONFIG         += console
CONFIG         += serial release

CONFIG( release,  debug|release ) {
        message( "Configuring for release build ..." )
        QMAKE_CFLAGS_RELEASE = -g -pedantic
        QMAKE_CXXFLAGS_RELEASE = -g -pedantic
}

CONFIG( debug,  debug|release ) {
        DEFINES += GEMSFIT_DEBUG
        QMAKE_CFLAGS_DEBUG = -g
        QMAKE_CXXFLAGS_DEBUG = -g
        message( "Configuring for debug build ..." )
}

!win32 {
  PLATFORM_CPP       =  ../gemsfit2/tcejdb/nix
}
else {
  PLATFORM_CPP       =  ../gemsfit2/tcejdb/win32
}

!macx-clang {
 QMAKE_CXXFLAGS += -std=gnu99
 QMAKE_CFLAGS += -std=gnu99
}


QMAKE_CC	= gcc
QMAKE_CXX	= g++
QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions -fopenmp
QMAKE_CFLAGS   += -fopenmp
LIBS           += -lz -fopenmp -ljansson


EJDB_CPP        =  ../gemsfit2/tcejdb
SRC_CPP        =  ./src-csvtoejdb

EJDB_H          =  $$EJDB_CPP
SRC_H          =  $$SRC_CPP
PLATFORM_H   =  $$PLATFORM_CPP

DEPENDPATH     += $$EJDB_H
DEPENDPATH     += $$SRC_H
DEPENDPATH   += $$PLATFORM_H

INCLUDEPATH    += $$EJDB_H
INCLUDEPATH    += $$SRC_H
INCLUDEPATH  += $$PLATFORM_H

OBJECTS_DIR     = obj

include($$EJDB_CPP/tcejdb.pri)
include($$SRC_CPP/csvtoejdb.pri)

HEADERS +=

