TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gemsfit
VERSION         = 1.0.1

DEFINES         += IPMGEMPLUGIN
#DEFINES         += NODEARRAYLEVEL

QMAKE_LFLAGS  +=

CONFIG -= qt
CONFIG += warn_on
CONFIG += console
CONFIG += serial release

CONFIG( release,  debug|release ) {
        message( "Configuring for release build ..." )
        QMAKE_CFLAGS_RELEASE = -O3
        QMAKE_CXXFLAGS_RELEASE = -O3
}

CONFIG( debug,  debug|release ) {
        DEFINES += GEMSFIT_DEBUG
        message( "Configuring for debug build ..." )
}

CONFIG( mpi, serial|mpi ) {
        message( "Configuring for OPEN_MPI build ..." )
        QMAKE_CC	= mpicc
        QMAKE_CXX	= mpic++
        QMAKE_CFLAGS += $$system(mpicc --showme:compile)
        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions $$system(mpic++ --showme:compile) -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions
        LIBS += -L/usr/lib/postgresql/9.1/lib/ -L/usr/local/dislin/ $$system(mpic++ --showme:link) -ldislin -lXm -lecpg -lnlopt -lm -lboost_filesystem -lboost_system -llapack -lblas -larmadillo -lpthread -lz
}

#contains( CONFIG, serial ) {
CONFIG( serial, serial|mpi ) {
        message( "Configuring for serial build ..." )
        QMAKE_CC	= gcc
        QMAKE_CXX	= g++
        QMAKE_CFLAGS +=
        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions
        LIBS += -L/usr/lib/postgresql/9.1/lib/ -L/usr/local/dislin/ -ldislin -lXm -lecpg -lnlopt -lm -lboost_filesystem -lboost_system -llapack -lblas -larmadillo -lpthread -lz
}

FIT_CPP      =  ./src-fit
GEMS3K_CPP   =  ../../standalone/GEMS3K

FIT_H        =   $$FIT_CPP
GEMS3K_H     =   $$GEMS3K_CPP

DEPENDPATH   += $$FIT_H
DEPENDPATH   += $$GEMS3K_H   

INCLUDEPATH  += $$FIT_H
INCLUDEPATH  += $$GEMS3K_H   
INCLUDEPATH  += . /usr/include/postgresql /home/parallels/bin/dislin

OBJECTS_DIR       = obj

include($$FIT_CPP/fit.pri)
include($$GEMS3K_CPP/gems3k.pri)

