message(STATUS "Building shellfit3")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dataman)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dialogs)

# Recursively collect all header files
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h)
file(GLOB COMMON_HEADER_FILES ${COMMON_DIR}/*.h)
file(GLOB GEMS3K_HEADER_FILES ${GEMS3K_DIR}/*.h)

# Recursively collect all source files
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file(GLOB COMMON_SOURCE_FILES ${COMMON_DIR}/*.cpp)
file(GLOB GEMS3K_SOURCE_FILES ${GEMS3K_DIR}/*.cpp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB QRC_FILES *.qrc)
set(RESOURCE ${QRC_FILES})
if(Qt6_FOUND)
  qt6_add_resources(GUI_SRCS ${RESOURCE})
else()
  qt5_add_resources(GUI_SRCS ${RESOURCE})
endif()

# Enable automatic creation of a module definition (.def) file for a SHARED library on Windows.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

# Create a library using the collected source files
add_library( GUI_OBJECT OBJECT ${HEADER_FILES} ${SOURCE_FILES} ${GUI_SRCS}
                    ${COMMON_HEADER_FILES} ${COMMON_SOURCE_FILES}
                    ${GEMS3K_HEADER_FILES} ${GEMS3K_SOURCE_FILES} )

# Create an executable named app
add_executable(shellfit3 $<TARGET_OBJECTS:GUI_OBJECT> )

# Link your application against Qt C++ library.
target_link_libraries(shellfit3 ${GUI_QT_TARGETS} )

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_link_libraries(shellfit3 ejdb yaml-cpp)
else()
    target_link_libraries(shellfit3 ejdb yaml-cpp pthread)
endif()

if(USE_THERMOFUN MATCHES ON)
  target_link_libraries(shellfit3 ThermoFun::ThermoFun)
else()
  if(USE_SPDLOG_PRECOMPILED)
      target_link_libraries(shellfit3 spdlog::spdlog)
  endif()
endif()

install(TARGETS shellfit3 DESTINATION bin)

