# options
message(STATUS "Building gem-fits")

if(USE_OLD_EJDB MATCHES ON)
   file(COPY "${CMAKE_SOURCE_DIR}/Tests" DESTINATION "${CMAKE_BINARY_DIR}/bin")
else()
   file(COPY "${CMAKE_SOURCE_DIR}/Tests2" DESTINATION "${CMAKE_BINARY_DIR}/bin")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/muparser/src)
find_package(Threads REQUIRED)

# Recursively collect all header files
file(GLOB COMMON_HEADER_FILES ${COMMON_DIR}/*.h)
file(GLOB FITS_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/src-fit/*.h)
file(GLOB MUPARSER_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/muparser/src/*.h)

# Recursively collect all source files
file(GLOB COMMON_SOURCE_FILES ${COMMON_DIR}/*.cpp)
file(GLOB FITS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src-fit/*.cpp)
file(GLOB MUPARSER_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/muparser/src/*.cpp)

set(GEMSFITS_SOURCES
      ${COMMON_HEADER_FILES} ${COMMON_SOURCE_FILES}
      ${FITS_HEADER_FILES} ${FITS_SOURCE_FILES}
      ${MUPARSER_HEADER_FILES} ${MUPARSER_SOURCE_FILES})


add_executable(gem-fits ${GEMSFITS_SOURCES})


if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
   set(ARMADILLO_DIR "${CMAKE_SOURCE_DIR}/gemsfit2/armadillo_win32")
   include_directories("${ARMADILLO_DIR}/include")
   link_directories("${ARMADILLO_DIR}/examples/lib_win64")

   #target_link_libraries(gem-fits nlopt blas_win64_MT lapack_win64_MT Threads::Threads)
   #target_link_libraries(gem-fits nlopt armadillo libblas liblapack Threads::Threads)
   #target_link_libraries(gem-fits GEMS3K::GEMS3K-static Boost::system ${EJDB_WIN_LIB} yaml-cpp)

   add_library(gemsfit-static STATIC ${COMMON_HEADER_FILES} ${COMMON_SOURCE_FILES}
                                      ${MUPARSER_HEADER_FILES} ${MUPARSER_SOURCE_FILES})
   target_link_libraries(gemsfit-static
                            PUBLIC GEMS3K::GEMS3K
                            PUBLIC Boost::system
                            PUBLIC ${EJDB_WIN_LIB}
                            PUBLIC yaml-cpp)

 add_executable(gem-fits ${FITS_HEADER_FILES} ${FITS_SOURCE_FILES})
 target_link_libraries(gem-fits gemsfit-static nlopt blas_win64_MT lapack_win64_MT Threads::Threads)

else()
   add_executable(gem-fits ${GEMSFITS_SOURCES})
   target_link_libraries(gem-fits  GEMS3K::GEMS3K nlopt Boost::system armadillo ${EJDB_LIB} yaml-cpp Threads::Threads)
   if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
     target_link_libraries(gem-fits omp)
   endif()
endif()

install(TARGETS gem-fits DESTINATION bin)

