# Require a certain version of cmake
cmake_minimum_required(VERSION 3.9)

# Set the name of the project
project(gemsfits VERSION 2.0.0 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MACOSX_RPATH ON)

# Include the cmake variables with values for installation directories
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

# Set libraries to be compiled with position independent code mode (i.e., fPIC option in GNU compilers)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set the default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Define which types of libraries to build
option(USE_THERMOFUN "Use ThermoFun" OFF)
option(USE_OLD_EJDB "Compile using ejdb1" ON)
option(BUILD_GUI "Build Qt GUI shell" ON)

# Used into conda only
if(DEFINED ENV{CONDA_PREFIX})
   option(USE_SPDLOG_PRECOMPILED "Use spdlog in compiled version" ON)
else()
   option(USE_SPDLOG_PRECOMPILED "Use spdlog in compiled version" OFF)
endif()

# Set the list of compiler flags for GNU compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    add_compile_options( -pthread -Wall -Wno-misleading-indentation -Wno-ignored-attributes -Wno-variadic-macros -Wno-deprecated)
endif()

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
        /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        /DNOMINMAX
    )
endif()

# Set the headers directory path
set(COMMON_DIR "${CMAKE_SOURCE_DIR}/common")
set(GEMS3K_DIR "${CMAKE_SOURCE_DIR}/gems3k/GEMS3K")

# Set the include directories
include_directories(${COMMON_DIR})
include_directories(${GEMS3K_DIR})
include_directories("${CMAKE_SOURCE_DIR}/gems3k")
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    include_directories("/usr/local/include")
    link_directories("/usr/local/lib")
endif()

# Find all gemsfits dependencies
include(gemsfitsFindDeps)

# Set some necessary definitions
add_definitions(-DIPMGEMPLUGIN)
add_definitions(-Duseomp)

if(USE_OLD_EJDB MATCHES ON)
  add_definitions(-DOLD_EJDB)
  add_definitions(-DCHECK_LOAD)
endif()

if(USE_THERMOFUN MATCHES ON)
  add_definitions(-DUSE_THERMOFUN)
  add_definitions(-DUSE_THERMO_LOG)
endif()

# Compile gemsfits application
add_subdirectory(gemsfit2)

# Compile gemsfits-gui application
if(BUILD_GUI)
    add_subdirectory(gfshell2)
endif()

include(CPack) #should be last command
#cmake .. -DCMAKE_PREFIX_PATH=/home/sveta/Qt/6.5.0/gcc_64
