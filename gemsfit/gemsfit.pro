##############################################################################################																							 
#	 Copyright (C) 2012 by Ferdinand F. Hingerl (hingerl@hotmail.com)						
#	 This file is part of the thermodynamic fitting program GEMSFIT
#        developed by FFH as part of his PhD project at ETHZ and PSI						 
#																							 
#    GEMSFIT is free software: you can redistribute it and/or modify						 
#    it under the terms of the GNU Lesser General Public License as published by			
#    the Free Software Foundation, either version 3 of the License, or						
#    (at your option) any later version.													
#																							 
#    GEMSFIT is distributed in the hope that it will be useful,								 
#    but WITHOUT ANY WARRANTY; without even the implied warranty of							
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the							
#    GNU Lesser General Public License for more details.									
#																							 
#    You should have received a copy of the GNU Lesser General Public License				
#    along with GEMSFIT.  If not, see <http://www.gnu.org/licenses/>.	
#   
#    GEMSFIT uses the GEMS3K solver of geochemical equilibria 
#    (C) 2012 by GEMS Developer Team (http://gems.web.psi.ch/GEMS3K)
#    GEMSFIT is available under GNU Lesser General Public License v.3	
#
#    GEMSFIT uses Postgresql ...
#
#    GESMFIT uses Dislin ...
#
#    GEMSFIT uses ...				
#																							 
#   This is the qmake project file for generation of static GEMSFIT application 					
#  	configure serial, release version:      qmake "CONFIG += serial release" gemsfit.pro 
#  	configure parallelized, debug version:	qmake "CONFIG += mpi debug" gemsfit.pro      
#   for optional debug output add DEFINES += GEMSFIT_DEBUG                                   
##############################################################################################


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

FIT_CPP      =  ./src-fit
GEMS3K_CPP   =  ../../standalone/GEMS3K

FIT_H        =   $$FIT_CPP
GEMS3K_H     =   $$GEMS3K_CPP

DEPENDPATH   += $$FIT_H
DEPENDPATH   += $$GEMS3K_H   

INCLUDEPATH  += $$FIT_H
INCLUDEPATH  += $$GEMS3K_H   
INCLUDEPATH  += . /usr/include/postgresql /usr/local/dislin/

OBJECTS_DIR       = obj

include($$FIT_CPP/fit.pri)
include($$GEMS3K_CPP/gems3k.pri)
