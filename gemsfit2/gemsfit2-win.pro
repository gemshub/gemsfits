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
VERSION         = 2.0.0

CONFIG -= qt
CONFIG += warn_on
CONFIG += console
#CONFIG += sanitaze sanitaze_thread
#CONFIG += serial release
CONFIG += c++17

# check settengs
DEFINES         += useomp
DEFINES += CHECK_LOAD # to generate print for initial data after read input configuration.
DEFINES += OLD_EJDB # compile using ejdb1
#DEFINES += OVERFLOW_EXCEPT  #compile with nan inf exceptions

DEFINES += IPMGEMPLUGIN
#DEFINES += NODEARRAYLEVEL
#DEFINES += USE_NLOHMANNJSON
#DEFINES += USE_THERMOFUN
#DEFINES += USE_THERMO_LOG


CONFIG( release,  debug|release ) {
        message( "Configuring for release build ..." )
        QMAKE_CFLAGS_RELEASE = -g -pedantic
        QMAKE_CXXFLAGS_RELEASE = -g -pedantic
}

CONFIG( debug,  debug|release ) {
        DEFINES += GEMSFIT_DEBUG
        QMAKE_CFLAGS_DEBUG = -g
        QMAKE_CXXFLAGS_DEBUG = -g
        LIBS += -fopenmp
        message( "Configuring for debug build ..." )
}

CONFIG( mpi, serial|mpi ) {
        message( "Configuring for OPEN_MPI build ..." )
        QMAKE_CC	= mpicc
        QMAKE_CXX	= mpic++
        QMAKE_CFLAGS += $$system(mpicc --showme:compile) -fopenmp
        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions $$system(mpic++ --showme:compile) -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions -fopenmp
        LIBS +=  $$system(mpic++ --showme:link) -lm  -lpthread -lz -fopenmp
}

#contains( CONFIG, serial ) {
CONFIG( serial, serial|mpi ) {
        message( "Configuring for serial build ..." )
        QMAKE_CC	= gcc
        QMAKE_CXX	= g++
        QMAKE_CFLAGS += -fopenmp
        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions -fopenmp
        LIBS += -lm   -lpthread -lz -fopenmp
}

!win32 {
  PLATFORM_CPP       =  ./tcejdb/nix
}
else {
  PLATFORM_CPP       =  ./tcejdb_win32/win32
  DEFINES           += buildWIN32
  DEFINES           += HAVE_STDINT_H
  QMAKE_CXXFLAGS    += -D__USE_MINGW_ANSI_STDIO=1 -frounding-math -fopenmp
  QMAKE_LFLAGS      *= -fopenmp
}

!macx-clang {
  QMAKE_CXXFLAGS += -std=gnu99
  QMAKE_CFLAGS += -std=gnu99
}

win32 {
QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS *= -fopenmp
}

FIT_CPP      =  ./src-fit
GEMS3K_CPP   =  ../standalone/GEMS3K
MUP_CPP      =  ./muparser/src
COMMON_CPP  =  ../common

FIT_H        =   $$FIT_CPP
GEMS3K_H     =   $$GEMS3K_CPP
MUP_H        =   $$MUP_CPP
COMMON_H     =   $$COMMON_CPP

DEPENDPATH   += $$FIT_H
DEPENDPATH   += $$GEMS3K_H
DEPENDPATH   += $$KEYS_H
DEPENDPATH   += $$MUP_H
DEPENDPATH   += $$COMMON_H

INCLUDEPATH  += $$FIT_H
INCLUDEPATH  += $$GEMS3K_H
INCLUDEPATH  += $$KEYS_H
INCLUDEPATH  += $$MUP_H
INCLUDEPATH   += $$COMMON_H

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

INCLUDEPATH   += $$EJDB_BSON_H
INCLUDEPATH   += $$EJDB_EJDB_H
INCLUDEPATH   += $$EJDB_GENERATED_H
INCLUDEPATH   += $$EJDB_TCUTIL_H

INCLUDEPATH += ./boost_win32
INCLUDEPATH += ./armadillo_win32/include

OBJECTS_DIR       = obj

include($$COMMON_CPP/common.pri)
include($$FIT_CPP/fit.pri)
include($$GEMS3K_CPP/gems3k.pri)
include($$MUP_CPP/muparser.pri)

#TCEJDB
contains(DEFINES, OLD_EJDB) {
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/tcejdb_win32/lib/ -ltcejdbdll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/tcejdb_win32/lib/ -ltcejdbdll
INCLUDEPATH += $$PWD/tcejdb_win32/include
DEPENDPATH += $$PWD/tcejdb_win32/include
}
else
{
}

#NLOPT
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/nlopt_win32/ -llibnlopt-0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/nlopt_win32/ -llibnlopt-0
INCLUDEPATH += $$PWD/nlopt_win32
DEPENDPATH += $$PWD/nlopt_win32

#BOOST
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/boost_win32/filesystem/build/gcc-mingw-4.8.2/release/link-static/threading-multi/ -lboost_filesystem-mgw48-mt-1_57
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/boost_win32/filesystem/build/gcc-mingw-4.8.2/release/link-static/threading-multi/ -lboost_filesystem-mgw48-mt-1_57
#INCLUDEPATH += $$PWD/boost_win32/filesystem/build/gcc-mingw-4.8.2/release/link-static/threading-multi
#DEPENDPATH += $$PWD/boost_win32/filesystem/build/gcc-mingw-4.8.2/release/link-static/threading-multi

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/boost_win32/system/build/gcc-mingw-4.8.2/release/link-static/threading-multi/ -lboost_system-mgw48-mt-1_57
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/boost_win32/system/build/gcc-mingw-4.8.2/release/link-static/threading-multi/ -lboost_system-mgw48-mt-1_57
INCLUDEPATH += $$PWD/boost_win32/system/build/gcc-mingw-4.8.2/release/link-static/threading-multi
DEPENDPATH += $$PWD/boost_win32/system/build/gcc-mingw-4.8.2/release/link-static/threading-multi

#ARMADILLO BLAS and LAPACK
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/armadillo_win32/examples/lib_win32/ -llibblas
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/armadillo_win32/examples/lib_win32/ -llibblas
INCLUDEPATH += $$PWD/armadillo_win32/examples/lib_win32
DEPENDPATH += $$PWD/armadillo_win32/examples/lib_win32

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/armadillo_win32/examples/lib_win32/ -lliblapack
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/armadillo_win32/examples/lib_win32/ -lliblapack
INCLUDEPATH += $$PWD/armadillo_win32/examples/lib_win32
DEPENDPATH += $$PWD/armadillo_win32/examples/lib_win32
