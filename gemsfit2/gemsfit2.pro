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
VERSION         = 2.3.2
# GEMS3K commit 66761a7

CONFIG -= qt
CONFIG += warn_on
CONFIG += console
CONFIG += sanitaze sanitaze_thread
CONFIG += serial release
CONFIG += c++20

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

QMAKE_CXXFLAGS += -O3
QMAKE_LFLAGS += -O3

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
        LIBS +=  $$system(mpic++ --showme:link) -lnlopt -lm -lboost_system -llapack -lblas -larmadillo -lpthread -lz -fopenmp
#        LIBS +=  $$system(mpic++ --showme:link) -lnlopt -lm  -lboost_system -llapack -lblas -larmadillo -lpthread -lz -liomp5
}

#contains( CONFIG, serial ) {
CONFIG( serial, serial|mpi ) {
        message( "Configuring for serial build ..." )
        QMAKE_CC	= gcc
        QMAKE_CXX	= g++
        QMAKE_CFLAGS += -fopenmp
        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions -fopenmp
#        QMAKE_CXXFLAGS += -Wall -pedantic -fexceptions -Wl,-rpath -Wl,/usr/lib -Wl,-Bsymbolic-functions
        LIBS += -lnlopt -lm  -lboost_system -llapack -lblas -larmadillo -lpthread -lz -fopenmp
#        LIBS += -lnlopt -lm -lboost_system -llapack -lblas -larmadillo -lpthread -lz -liomp5
}

!macx-clang {
  INCLUDEPATH   += "/usr/local/include/ejdb"
  INCLUDEPATH   += "/usr/local/include/ejdb2"
}else{
  INCLUDEPATH   += "/usr/local/include"
  LIBPATH += "/usr/local/lib/"
}

FIT_CPP      =  ./src-fit
#GEMS3K_CPP   =  ../standalone/GEMS3K
MUP_CPP      =  ./muparser/src
COMMON_CPP  =  ../common

FIT_H        =   $$FIT_CPP
#GEMS3K_H     =   $$GEMS3K_CPP
MUP_H        =   $$MUP_CPP
COMMON_H     =   $$COMMON_CPP

DEPENDPATH   += $$FIT_H
#DEPENDPATH   += $$GEMS3K_H
DEPENDPATH   += $$KEYS_H
DEPENDPATH   += $$MUP_H
DEPENDPATH   += $$COMMON_H

INCLUDEPATH  += $$FIT_H
#INCLUDEPATH  += $$GEMS3K_H
INCLUDEPATH  += $$KEYS_H
INCLUDEPATH  += $$MUP_H
INCLUDEPATH   += $$COMMON_H

OBJECTS_DIR       = obj

include($$COMMON_CPP/common.pri)
include($$FIT_CPP/fit.pri)
#include($$GEMS3K_CPP/gems3k.pri)
include($$MUP_CPP/muparser.pri)

contains(DEFINES, OLD_EJDB) {
CONFIG(release, debug|release): LIBS += -lejdb -lyaml-cpp
CONFIG(debug, debug|release): LIBS += -lejdb -lyaml-cpp
}
else
{
CONFIG(release, debug|release): LIBS += -lejdb2 -lyaml-cpp
CONFIG(debug, debug|release): LIBS += -lejdb2 -lyaml-cpp
}

LIBS += -lGEMS3K
contains(DEFINES, USE_THERMOFUN) {
  LIBS += -lThermoFun -lChemicalFun
} ## end USE_THERMOFUN

