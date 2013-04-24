
# CASH/ solcash CASH.csv

TEMPLATE = app
CONFIG += console
CONFIG -= qt


CONFIG -= qt
CONFIG += warn_on
CONFIG += console
CONFIG += serial release

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

CONFIG( mpi, serial|mpi ) {
        message( "Configuring for OPEN_MPI build ..." )
        QMAKE_CC	= mpicc
        QMAKE_CXX	= mpic++
        QMAKE_CFLAGS += $$system(mpicc --showme:compile) -fopenmp
        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions $$system(mpic++ --showme:compile) -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions -fopenmp
        LIBS += -L/usr/lib/postgresql/9.1/lib/ -L/usr/local/dislin/ $$system(mpic++ --showme:link) -ldislin -lXm -lecpg -lnlopt -lm -lboost_filesystem -lboost_system -llapack -lblas -larmadillo -lpthread -lz -fopenmp -ljansson
}

#contains( CONFIG, serial ) {
CONFIG( serial, serial|mpi ) {
        message( "Configuring for serial build ..." )
        QMAKE_CC	= gcc
        QMAKE_CXX	= g++
        QMAKE_CFLAGS += -fopenmp
        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions -fopenmp
        LIBS += -L/usr/lib/postgresql/9.1/lib/ -L/usr/local/dislin/ -ldislin -lXm -lecpg -lnlopt -lm -lboost_filesystem -lboost_system -llapack -lblas -larmadillo -lpthread -lz -fopenmp -ljansson
}


QMAKE_CXXFLAGS += -std=gnu99
QMAKE_CFLAGS += -std=gnu99


EJDB_CPP   =  ../../gemsfit2/tcejdb
MAIN_CPP      =  ./main

EJDB_H   =  $$EJDB_CPP
MAIN_H        =   $$MAIN_CPP


DEPENDPATH   += $$EJDB_H
DEPENDPATH   += $$MAIN_H

INCLUDEPATH   += $$EJDB_H
INCLUDEPATH   += $$MAIN_H

OBJECTS_DIR       = obj

include($$EJDB_CPP/tcejdb.pri)
include($$MAIN_CPP/main.pri)

