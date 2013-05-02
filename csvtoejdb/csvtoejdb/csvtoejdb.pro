
# CASH/ solcash CASH.csv

TEMPLATE        = app
CONFIG         += console
CONFIG         -= qt


CONFIG         -= qt
CONFIG         += warn_on
CONFIG         += console
CONFIG         += serial release


QMAKE_CC	= gcc
QMAKE_CXX	= g++
QMAKE_CXXFLAGS += -std=gnu99 -Wall -pedantic -fexceptions -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions -fopenmp
QMAKE_CFLAGS   += -std=gnu99 -fopenmp
LIBS           += -lboost_filesystem -lboost_system -lz -fopenmp -ljansson


EJDB_CPP        =  ../../gemsfit2/tcejdb
MAIN_CPP        =  ./main

EJDB_H          =  $$EJDB_CPP
MAIN_H          =   $$MAIN_CPP


DEPENDPATH     += $$EJDB_H
DEPENDPATH     += $$MAIN_H

INCLUDEPATH    += $$EJDB_H
INCLUDEPATH    += $$MAIN_H

OBJECTS_DIR     = obj

include($$EJDB_CPP/tcejdb.pri)
include($$MAIN_CPP/main.pri)

