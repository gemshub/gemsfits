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
VERSION         = 2.2.1
CONFIG          += c++17

DEFINES         += IPMGEMPLUGIN
DEFINES         += useomp
DEFINES         += _MYNOZLIB
DEFINES         += HAVE_STDINT_H

QMAKE_LFLAGS  +=

CONFIG -= qt
CONFIG += warn_on
CONFIG += console
#CONFIG += serial release

CONFIG( release,  debug|release ) {
        message( "Configuring for release build ..." )
        QMAKE_CFLAGS_RELEASE = -g -pedantic -O3
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
  DEFINES           += buildWIN32
  DEFINES           += HAVE_STDINT_H
  QMAKE_CXXFLAGS    += -D__USE_MINGW_ANSI_STDIO=1 -frounding-math -fopenmp -O3
  QMAKE_LFLAGS      *= -fopenmp -O3
}

win32 {
QMAKE_CXXFLAGS += -fopenmp -O3
QMAKE_LFLAGS += -fopenmp -O3
}

FIT_CPP      =  ./src-fit
GEMS3K_CPP   =  ../../standalone/GEMS3K
KEYS_CPP     =  ../csvtoejdb/src-csvtoejdb
MUP_CPP      =  ./muparser/src
JAN_CPP        =  ./jan_win32/src

FIT_H        =   $$FIT_CPP
GEMS3K_H     =   $$GEMS3K_CPP
KEYS_H       =   $$KEYS_CPP
PLATFORM_H   =   $$PLATFORM_CPP
MUP_H        =   $$MUP_CPP
JAN_H          =  $$JAN_CPP

DEPENDPATH   += $$FIT_H
DEPENDPATH   += $$GEMS3K_H
DEPENDPATH   += $$KEYS_H
DEPENDPATH   += $$MUP_H
DEPENDPATH     += $$JAN_H

INCLUDEPATH  += $$FIT_H
INCLUDEPATH  += $$GEMS3K_H   
INCLUDEPATH  += $$KEYS_H
INCLUDEPATH  += $$MUP_H
INCLUDEPATH    += $$JAN_H

INCLUDEPATH += ./boost_win32
INCLUDEPATH += ./armadillo_win32/include

OBJECTS_DIR       = obj

include($$FIT_CPP/fit.pri)
include($$GEMS3K_CPP/gems3k.pri)
include($$MUP_CPP/muparser.pri)
include($$JAN_CPP/jan.pri)

#TCEJDB
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/tcejdb_win32-64/bin/ -llibejdb
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/tcejdb_win32-64/bin/ -llibejdb
INCLUDEPATH += $$PWD/tcejdb_win32-64/include
DEPENDPATH += $$PWD/tcejdb_win32-64/include

#NLOPT
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/nlopt_win32-64/bin/ -llibnlopt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/nlopt_win32-64/bin/ -llibnlopt
INCLUDEPATH += $$PWD/nlopt_win32-64/include
DEPENDPATH += $$PWD/nlopt_win32-64/include

#ARMADILLO BLAS and LAPACK
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/armadillo_win32/examples/lib_win64/ -lblas_win64_MT
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/armadillo_win32/examples/lib_win64/ -lblas_win64_MT
#INCLUDEPATH += $$PWD/armadillo_win32/examples/lib_win64
#DEPENDPATH += $$PWD/armadillo_win32/examples/lib_win64

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/armadillo_win32/examples/lib_win64/ -llapack_win64_MT
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/armadillo_win32/examples/lib_win64/ -llapack_win64_MT
#INCLUDEPATH += $$PWD/armadillo_win32/examples/lib_win64
#DEPENDPATH += $$PWD/armadillo_win32/examples/lib_win64

#BOOST
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/boost_win32-64/lib/ -llibboost_filesystem-mt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/boost_win32-64/lib/ -llibboost_filesystem-mt
#INCLUDEPATH += $$PWD/boost_win32-64/include
#DEPENDPATH += $$PWD/boost_win32-64/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/boost_win32-64/lib -llibboost_system-mt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/boost_win32-64/lib -llibboost_system-mt
#INCLUDEPATH += $$PWD/boost_win32-64/include
#DEPENDPATH += $$PWD/boost_win32-64/include
