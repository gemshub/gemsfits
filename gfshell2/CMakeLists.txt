message(STATUS "Building shellfit3")
file(COPY "${CMAKE_SOURCE_DIR}/Resources" DESTINATION "${CMAKE_BINARY_DIR}/bin")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dataman)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dialogs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
find_package(Threads REQUIRED)

# Recursively collect all header files
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h)
file(GLOB COMMON_HEADER_FILES ${COMMON_DIR}/*.h)
#file(GLOB GEMS3K_HEADER_FILES ${GEMS3K_DIR}/*.h)

# Recursively collect all source files
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file(GLOB COMMON_SOURCE_FILES ${COMMON_DIR}/*.cpp)
#file(GLOB GEMS3K_SOURCE_FILES ${GEMS3K_DIR}/*.cpp)


file(GLOB QRC_FILES *.qrc)
set(RESOURCE ${QRC_FILES})

# Enable automatic creation of a module definition (.def) file for a SHARED library on Windows.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

if(Qt6_FOUND)

  qt_standard_project_setup()
  qt_add_resources(GUI_SRCS ${RESOURCE})

  # Create a library using the collected source files
  add_library( GUI_OBJECT OBJECT ${HEADER_FILES} ${SOURCE_FILES} ${GUI_SRCS}
                    ${COMMON_HEADER_FILES} ${COMMON_SOURCE_FILES}
                    ${GEMS3K_HEADER_FILES} ${GEMS3K_SOURCE_FILES} )

  # Create an executable named app
  qt_add_executable(shellfit3 $<TARGET_OBJECTS:GUI_OBJECT> )

else()

  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)
  set(CMAKE_AUTOUIC ON)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  qt_add_resources(GUI_SRCS ${RESOURCE})

  # Create a library using the collected source files
  add_library( GUI_OBJECT OBJECT ${HEADER_FILES} ${SOURCE_FILES} ${GUI_SRCS}
                 ${COMMON_HEADER_FILES} ${COMMON_SOURCE_FILES}
                 ${GEMS3K_HEADER_FILES} ${GEMS3K_SOURCE_FILES} )

  # Create an executable named app
  add_executable(shellfit3 $<TARGET_OBJECTS:GUI_OBJECT> )

endif()

# Link your application against Qt C++ library.
target_link_libraries(shellfit3 PRIVATE ${GUI_QT_TARGETS} )

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
     target_link_libraries(shellfit3 PRIVATE ${EJDB_WIN_LIB} yaml-cpp Threads::Threads)
else()
     target_link_libraries(shellfit3 PRIVATE ${EJDB_LIB} yaml-cpp Threads::Threads)
endif()

target_link_libraries(shellfit3 PRIVATE PUBLIC GEMS3K::GEMS3K)

if(Qt6_FOUND)

  set_target_properties(shellfit3 PROPERTIES
     WIN32_EXECUTABLE ON
     MACOSX_BUNDLE ON )

endif()

install(TARGETS shellfit3 DESTINATION bin)

