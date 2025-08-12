# Require a certain version of cmake
cmake_minimum_required(VERSION 3.16)

# Set the name of the project
project(gemsfits VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MACOSX_RPATH ON)

# Define variables with the GNU standard installation directories
include(GNUInstallDirs)

# Set the cmake module path of the project
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Use ccache to speed up repeated compilations
include(CCache)

# Ensure proper configuration if in a conda environment
include(CondaAware)

# Set the output directories of the built libraries and binaries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Define which types of libraries to build
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(BUILD_TOOLS "Build tools." OFF)
option(USE_THERMOFUN "Use ThermoFun" ON)
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  option(USE_OLD_EJDB "Compile using ejdb1" ON)
  option(BUILD_FIT "Build gem-fits" ON)
  option(BUILD_GUI "Build Qt GUI shell" ON)
else()
  option(USE_OLD_EJDB "Compile using ejdb1" ON)
  option(BUILD_FIT "Build gem-fits" ON)
  option(BUILD_GUI "Build Qt GUI shell" ON)

endif()

# Used into conda only
if(DEFINED ENV{CONDA_PREFIX})
   option(USE_SPDLOG_PRECOMPILED "Use spdlog in compiled version" ON)
else()
   option(USE_SPDLOG_PRECOMPILED "Use spdlog in compiled version" OFF)
endif()

# Set the default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    # The build type selection for the project
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type for ${PROJECT_NAME}." FORCE)
    # The build type options for the project
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release MinSizeRel RelWithDebInfo)
endif()

# Currently is not setup to produce a dynamic library using MSVC, only static
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(BUILD_SHARED_LIBS OFF)
else()
#    set(BUILD_STATIC_LIBS OFF)
endif()

if(BUILD_GUI)
   set(BUILD_STATIC_LIBS ON)
endif()

# Set libraries to be compiled with position independent code mode (i.e., fPIC option in GNU compilers)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find all gemsfits dependencies
include(gemsfitsFindDeps)

# Find Qt dependencies
find_package(Qt6 6.2 COMPONENTS Core Gui Widgets Charts Concurrent PrintSupport Svg Help Sql REQUIRED)
if(Qt6_FOUND)
  message(STATUS "Found Qt6 v${Qt6_VERSION}")
  set(GUI_QT_TARGETS Qt6::Widgets Qt6::Core Qt6::Gui Qt6::PrintSupport Qt6::Charts Qt6::Svg Qt6::Help Qt6::Sql)
  include_directories(${Qt6Core_INCLUDE_DIRS})
  include_directories(${Qt6Gui_INCLUDE_DIRS})
  include_directories(${Qt6Widgets_INCLUDE_DIRS})
  include_directories(${Qt6PrintSupport_INCLUDE_DIRS})
  include_directories(${Qt6Svg_INCLUDE_DIRS})
  include_directories(${Qt6Concurrent_INCLUDE_DIRS})
  include_directories(${Qt6Help_INCLUDE_DIRS})
  include_directories(${Qt6Sql_INCLUDE_DIRS})
endif()
qt_standard_project_setup(REQUIRES 6.8)

# Versioning
set(GEMSFITS_VERSION "${PROJECT_VERSION}")
find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE GEMSFITS_VERSION_HASH
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE GEMSFITS_GIT_BRANCH
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT GEMSFITS_VERSION_HASH)
        set(GEMSFITS_VERSION_HASH "---")
    endif()
    if(NOT GEMSFITS_GIT_BRANCH)
        set(GEMSFITS_GIT_BRANCH "---")
    endif()
else()
  set(GEMSFITS_VERSION_HASH "<unknown>")
  set(GEMSFITS_GIT_BRANCH "<unknown>")
endif()


# Set the list of compiler flags for GNU compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    add_compile_options( -pthread -Wall -Wno-misleading-indentation -Wno-ignored-attributes -Wno-variadic-macros -Wno-deprecated -Wno-unused-variable)
endif()

set(GEMSFITS_OSX "${CMAKE_SYSTEM_NAME}")
set(GEMSFITS_COMPILER_ID "${CMAKE_CXX_COMPILER_ID}")
set(GEMSFITS_COMPILER_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
set(GEMSFITS_QT_VERSION "${Qt6_VERSION}")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gemsfits_version.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/common/gemsfits_version.h"
  @ONLY
  )

# Set the list of compiler flags for Clang compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    add_compile_options(-mavx2 -Wall -Wno-ignored-attributes -Wno-pedantic -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for Intel compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
    add_compile_options(-Wall -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for MSVC compiler
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_compile_options(
        /D_SCL_SECURE_NO_WARNINGS
        /D_CRT_SECURE_NO_WARNINGS=1
        /MP4
        /EHsc
        /utf-8
        /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        /DNOMINMAX
    )
endif()

# Set the headers directory path
set(COMMON_DIR "${CMAKE_SOURCE_DIR}/common")

# Set the include directories
include_directories(${COMMON_DIR})

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
   include_directories("/usr/local/include")
   link_directories("/usr/local/lib")
endif()

# Set some necessary definitions
add_definitions(-DIPMGEMPLUGIN)
add_definitions(-Duseomp)

if(USE_OLD_EJDB MATCHES ON)
  add_definitions(-DOLD_EJDB)
  add_definitions(-DCHECK_LOAD)
  set(EJDB_LIB ejdb)

  if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(EJDB_DIR "${CMAKE_SOURCE_DIR}/gemsfit2/tcejdb_win32-64")
    include_directories("${EJDB_DIR}/include")
    link_directories("${EJDB_DIR}/lib")
    link_directories("${EJDB_DIR}/bin")
    set(EJDB_WIN_LIB libejdb)
  endif()

else()

  set(EJDB_LIB ejdb2)
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
      set(EJDB_DIR "${CMAKE_SOURCE_DIR}/gemsfit2/ejdb2_win32-64")
      include_directories("${EJDB_DIR}/include")
      link_directories("${EJDB_DIR}/lib")
      link_directories("${EJDB_DIR}/bin")
      set(EJDB_WIN_LIB libejdb2)
  endif()
endif()

if(USE_THERMOFUN MATCHES ON)
  add_definitions(-DUSE_THERMOFUN)
  add_definitions(-DUSE_THERMO_LOG)
endif()

# Compile gemsfits application
if(BUILD_FIT)
    add_subdirectory(gemsfit2)
endif()

# Compile gemsfits-gui application
if(BUILD_GUI)
    add_subdirectory(gfshell2)
endif()

# make service tools
if(BUILD_TOOLS)
   add_subdirectory(tools)
endif()

include(CPack) #should be last command

