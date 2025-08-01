# options
message(STATUS "Building gemsfit3")

if(USE_OLD_EJDB MATCHES ON)
   file(COPY "${CMAKE_SOURCE_DIR}/Tests" DESTINATION "${CMAKE_BINARY_DIR}/bin")
else()
   file(COPY "${CMAKE_SOURCE_DIR}/Tests2" DESTINATION "${CMAKE_BINARY_DIR}/bin")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/muparser/src)
find_package(Threads REQUIRED)

# Recursively collect all header files
file(GLOB COMMON_HEADER_FILES ${COMMON_DIR}/*.h)
#file(GLOB GEMS3K_HEADER_FILES ${GEMS3K_DIR}/*.h)
file(GLOB FITS_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/src-fit/*.h)
file(GLOB MUPARSER_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/muparser/src/*.h)

# Recursively collect all source files
file(GLOB COMMON_SOURCE_FILES ${COMMON_DIR}/*.cpp)
#file(GLOB GEMS3K_SOURCE_FILES ${GEMS3K_DIR}/*.cpp)
file(GLOB FITS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src-fit/*.cpp)
file(GLOB MUPARSER_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/muparser/src/*.cpp)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

   set(ARMADILLO_DIR "${CMAKE_SOURCE_DIR}/gemsfit2/armadillo_win32")
   include_directories("${ARMADILLO_DIR}/include")
   link_directories("${ARMADILLO_DIR}/examples/lib_win64")


   add_library(gemsfit-static STATIC ${COMMON_HEADER_FILES} ${COMMON_SOURCE_FILES}
                                      ${GEMS3K_HEADER_FILES} ${GEMS3K_SOURCE_FILES}
                                      ${MUPARSER_HEADER_FILES} ${MUPARSER_SOURCE_FILES})
   target_link_libraries(gemsfit-static
                            PUBLIC GEMS3K::GEMS3K
                            PUBLIC Boost::system
                            PUBLIC ${EJDB_WIN_LIB}
                            PUBLIC yaml-cpp)
   #if(USE_THERMOFUN MATCHES ON)
   #      target_link_libraries(gemsfit-static PUBLIC ThermoFun::ThermoFun)
   #else()
   #      if(USE_SPDLOG_PRECOMPILED)
   #           target_link_libraries(gemsfit-static PUBLIC spdlog::spdlog)
   #      endif()
   #endif()

   add_executable(gemsfit3 ${FITS_HEADER_FILES} ${FITS_SOURCE_FILES})
   target_link_libraries(gemsfit3 gemsfit-static nlopt blas_win64_MT lapack_win64_MT Threads::Threads)

else()
   # Create a library using the collected source files
   add_library( FITS_OBJECT OBJECT ${COMMON_HEADER_FILES} ${COMMON_SOURCE_FILES}
                    ${FITS_HEADER_FILES} ${FITS_SOURCE_FILES}
                    ${MUPARSER_HEADER_FILES} ${MUPARSER_SOURCE_FILES} )

   # Create an executable named app
   add_executable(gemsfit3 $<TARGET_OBJECTS:FITS_OBJECT>)
   target_link_libraries(gemsfit3  GEMS3K::GEMS3K nlopt Boost::system armadillo ${EJDB_LIB} yaml-cpp Threads::Threads)

  #if(USE_THERMOFUN MATCHES ON)
  #   target_link_libraries(gemsfit3 ThermoFun::ThermoFun)
  #else()
  #   if(USE_SPDLOG_PRECOMPILED)
  #     target_link_libraries(gemsfit3 spdlog::spdlog)
  #   endif()
  #endif()

endif()

install(TARGETS gemsfit3 DESTINATION bin)

