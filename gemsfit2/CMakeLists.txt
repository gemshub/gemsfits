# options
message(STATUS "Building gemsfit3")
file(COPY "${CMAKE_SOURCE_DIR}/Tests" DESTINATION "${CMAKE_BINARY_DIR}/bin")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/muparser/src)
find_package(Threads REQUIRED)

# Recursively collect all header files
file(GLOB COMMON_HEADER_FILES ${COMMON_DIR}/*.h)
file(GLOB GEMS3K_HEADER_FILES ${GEMS3K_DIR}/*.h)
file(GLOB FITS_HEADER_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/src-fit/*.h)
file(GLOB MUPARSER_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/muparser/src/*.h)

# Recursively collect all source files
file(GLOB COMMON_SOURCE_FILES ${COMMON_DIR}/*.cpp)
file(GLOB GEMS3K_SOURCE_FILES ${GEMS3K_DIR}/*.cpp)
file(GLOB FITS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src-fit/*.cpp)
file(GLOB MUPARSER_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/muparser/src/*.cpp)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

   add_library(gemsfit-static STATIC ${COMMON_HEADER_FILES} ${COMMON_SOURCE_FILES}
                                      ${GEMS3K_HEADER_FILES} ${GEMS3K_SOURCE_FILES}
                                      ${MUPARSER_HEADER_FILES} ${MUPARSER_SOURCE_FILES})
   target_link_libraries(gemsfit-static
                            PUBLIC nlopt
                            PUBLIC Boost::system
                            PUBLIC libejdb
                            PUBLIC yaml-cpp
                            PUBLIC armadillo)
   if(USE_THERMOFUN MATCHES ON)
         target_link_libraries(gemsfit-static PUBLIC ThermoFun::ThermoFun)
   else()
         if(USE_SPDLOG_PRECOMPILED)
              target_link_libraries(gemsfit-static PUBLIC spdlog::spdlog)
         endif()
   endif()

   add_executable(gemsfit3 ${FITS_HEADER_FILES} ${FITS_SOURCE_FILES})
   target_link_libraries(gemsfit3 gemsfit-static Threads::Threads)

else()
   # Create a library using the collected source files
   add_library( FITS_OBJECT OBJECT ${COMMON_HEADER_FILES} ${COMMON_SOURCE_FILES}
                    ${GEMS3K_HEADER_FILES} ${GEMS3K_SOURCE_FILES}
                    ${FITS_HEADER_FILES} ${FITS_SOURCE_FILES}
                    ${MUPARSER_HEADER_FILES} ${MUPARSER_SOURCE_FILES} )

   # Create an executable named app
   add_executable(gemsfit3 $<TARGET_OBJECTS:FITS_OBJECT>)
   target_link_libraries(gemsfit3 nlopt Boost::system armadillo ejdb yaml-cpp Threads::Threads)
   #target_link_libraries(gemsfit3 nlopt boost_system armadillo ejdb yaml-cpp openmp pthread)

  if(USE_THERMOFUN MATCHES ON)
     target_link_libraries(gemsfit3 ThermoFun::ThermoFun)
  else()
     if(USE_SPDLOG_PRECOMPILED)
       target_link_libraries(gemsfit3 spdlog::spdlog)
     endif()
  endif()

endif()

install(TARGETS gemsfit3 DESTINATION bin)

