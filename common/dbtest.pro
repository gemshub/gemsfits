TEMPLATE = app
CONFIG += thread console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++17
#CONFIG += sanitizer sanitize_thread

DEFINES         += IPMGEMPLUGIN
DEFINES         += OLD_EJDB

!win32 {
  DEFINES += __unix
  INCLUDEPATH   += "/usr/local/include/ejdb"
  INCLUDEPATH   += "/usr/local/include/ejdb2"
#QMAKE_CFLAGS += pedantic -Wall -Wextra -Wwrite-strings -Werror
#QMAKE_CXXFLAGS += -ansi -pedantic -Wall -Wextra -Weffc++
}

macx-g++ {
  DEFINES += __APPLE__
}

macx-clang {
  DEFINES += __APPLE__
  INCLUDEPATH   += "/usr/local/include"
  DEPENDPATH   += "/usr/local/include"
  LIBPATH += "/usr/local/lib/"
}

win32 {
  INCLUDEPATH   += "C:\usr\local\include"
  DEPENDPATH   += "C:\usr\local\include"
  LIBPATH += "C:\usr\local\lib"
}

COMMON_CPP  =  .
COMMON_H     =  $$COMMON_CPP
DEPENDPATH   += $$COMMON_H
INCLUDEPATH   += $$COMMON_H

GEMS3K_CPP   =  ../../standalone/GEMS3K
COMMON_CPP  =  .
GEMS3K_H     =   $$GEMS3K_CPP
COMMON_H     =  $$COMMON_CPP

DEPENDPATH   += $$GEMS3K_H
DEPENDPATH   += $$COMMON_H

INCLUDEPATH  += $$GEMS3K_H
INCLUDEPATH   += $$COMMON_H

include($$COMMON_CPP/common.pri)
include($$GEMS3K_CPP/gems3k.pri)

OBJECTS_DIR       = obj

contains(DEFINES, OLD_EJDB) {
 LIBS += -lejdb -lyaml-cpp
}
else {
 LIBS += -lejdb2 -lyaml-cpp
}

SOURCES += \
       dbtest_main.cpp


