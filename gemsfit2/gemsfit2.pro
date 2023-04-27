##############################################################################################																							 
#    Copyright (C) 2013 by G. Dan Miron (mirondanro@yahoo.com)
#
#    GEMSFIT2 is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by			
#    the Free Software Foundation, either version 3 of the License, or						
#    (at your option) any later version.													
#																							 
#    GEMSFIT2 is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of							
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the							
#    GNU Lesser General Public License for more details.									
#																							 
#    You should have received a copy of the GNU Lesser General Public License				
#    along with GEMSFIT2.  If not, see <http://www.gnu.org/licenses/>.
#   
#    GEMSFIT2 uses the GEMS3K solver of geochemical equilibria
#    (C) 2012 by GEMS Developer Team (http://gems.web.psi.ch/GEMS3K)
#    GEMSFIT2 is available under GNU Lesser General Public License v.3
#
#    GEMSFIT2 uses EJDB as internal databsse (http://ejdb.org)
#																							 
#    This is the qmake project file for generation of static GEMSFIT2 application
#  	configure serial, release version:      qmake "CONFIG += serial release" gemsfit.pro 
#  	configure parallelized, debug version:	qmake "CONFIG += mpi debug" gemsfit.pro      
#
#    -init <path to <GEMS3K>-dat.lst>
#    -run  <path to GEMSFIT2 input config file>
##############################################################################################


TEMPLATE	= app
#LANGUAGE        = C++
TARGET		= gemsfit2
VERSION         = 2.3.0
# GEMS3K commit 66761a7

DEFINES         += IPMGEMPLUGIN
DEFINES         += useomp
DEFINES         += _MYNOZLIB
#DEFINES         += CHECK_LOAD
CONFIG          += c++17

#QMAKE_CXXFLAGS += -O3
#QMAKE_LFLAGS += -O3

CONFIG -= qt
CONFIG += warn_on
CONFIG += console
CONFIG += serial release

CONFIG( release,  debug|release ) {
        message( "Configuring for release build ..." )
        QMAKE_CFLAGS_RELEASE = -g -pedantic -fopenmp
        QMAKE_CXXFLAGS_RELEASE = -g -pedantic -fopenmp
}

CONFIG( debug,  debug|release ) {
        DEFINES += GEMSFIT_DEBUG
        QMAKE_CFLAGS_DEBUG = -g -fopenmp
        QMAKE_CXXFLAGS_DEBUG = -g -fopenmp
        message( "Configuring for debug build ..." )
}

CONFIG( mpi, serial|mpi ) {
        message( "Configuring for OPEN_MPI build ..." )
        QMAKE_CC	= mpicc
        QMAKE_CXX	= mpic++
        QMAKE_CFLAGS += $$system(mpicc --showme:compile) -fopenmp
        QMAKE_CFLAGS += $$system(mpicc --showme:compile)
        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions $$system(mpic++ --showme:compile) -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions -fopenmp
#        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions $$system(mpic++ --showme:compile) -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions
        LIBS +=  $$system(mpic++ --showme:link) -lnlopt -lm -lboost_filesystem -lboost_system -llapack -lblas -larmadillo -lpthread -lz -fopenmp -ljansson -lejdb
#        LIBS +=  $$system(mpic++ --showme:link) -lnlopt -lm -lboost_filesystem -lboost_system -llapack -lblas -larmadillo -lpthread -lz -liomp5 -ljansson -lejdb
}

#contains( CONFIG, serial ) {
CONFIG( serial, serial|mpi ) {
        message( "Configuring for serial build ..." )
        QMAKE_CC	= gcc
        QMAKE_CXX	= g++
        QMAKE_CFLAGS += -fopenmp
        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions -fopenmp
#        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions
        LIBS += -lnlopt -lm -lboost_filesystem  -lboost_system -llapack -lblas -larmadillo -lpthread -lz -fopenmp -ljansson -lejdb
#        LIBS += -lnlopt -lm -lboost_filesystem  -lboost_system -llapack -lblas -larmadillo -lpthread -lz -liomp5 -ljansson -lejdb
}

!macx-clang {
  INCLUDEPATH   += "/usr/local/include/ejdb"
}else{
  INCLUDEPATH   += "/usr/local/include"
#  INCLUDEPATH   += "/usr/local/Cellar/libiomp/20150701/include/libiomp"
  LIBPATH += "/usr/local/lib/"
#  LIBPATH +=  "/usr/local/Cellar//libiomp/20150701/lib"

#  INCLUDEPATH   += /usr/local/Cellar/jansson/2.7
#  INCLUDEPATH   +=  /usr/local/Cellar/gcc/5.2.0
}

FIT_CPP      =  ./src-fit
GEMS3K_CPP   =  ../../standalone/GEMS3K
KEYS_CPP     =  ../csvtoejdb/src-csvtoejdb
MUP_CPP      =  ./muparser/src

FIT_H        =   $$FIT_CPP
GEMS3K_H     =   $$GEMS3K_CPP
KEYS_H       =   $$KEYS_CPP
MUP_H        =   $$MUP_CPP

#EJDB_PATH = ../../standalone/EJDB

#win32{
#   EJDB_LIB_PATH =  $$EJDB_PATH/build-win32
#}
#unix{
#   EJDB_LIB_PATH =  $$EJDB_PATH/build
#}

#EJDB_BSON_H = $$EJDB_PATH/src/bson
#EJDB_EJDB_H = $$EJDB_PATH/src/ejdb
#EJDB_TCUTIL_H = $$EJDB_PATH/src/tcutil
#EJDB_GENERATED_H = $$EJDB_LIB_PATH/debug/src/generated
#CONFIG(release, debug|release): EJDB_GENERATED_H = $$EJDB_LIB_PATH/release/src/generated
#CONFIG(debug, debug|release): EJDB_GENERATED_H = $$EJDB_LIB_PATH/debug/src/generated


DEPENDPATH   += $$FIT_H
DEPENDPATH   += $$GEMS3K_H
DEPENDPATH   += $$KEYS_H
DEPENDPATH   += $$MUP_H

INCLUDEPATH  += $$FIT_H
INCLUDEPATH  += $$GEMS3K_H   
INCLUDEPATH  += $$KEYS_H
INCLUDEPATH  += $$MUP_H

#INCLUDEPATH   += $$EJDB_BSON_H
#INCLUDEPATH   += $$EJDB_EJDB_H
#INCLUDEPATH   += $$EJDB_GENERATED_H
#INCLUDEPATH   += $$EJDB_TCUTIL_H

OBJECTS_DIR       = obj

include($$FIT_CPP/fit.pri)
include($$GEMS3K_CPP/gems3k.pri)
include($$MUP_CPP/muparser.pri)
#include($$EJDB_CPP/tcejdb.pri)
#include($$EJDB_PATH/ejdb.pri)

#CONFIG(release, debug|release): LIBS += -L$$EJDB_LIB_PATH/release/src/ -lejdb
#CONFIG(debug, debug|release): LIBS += -L$$EJDB_LIB_PATH/debug/src/ -lejdb
