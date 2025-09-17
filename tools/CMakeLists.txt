

# Generate Qt Help Project File gems3helpconfig.qhp
message(STATUS "Generate Qt Help Project File gfshelpconfig.qhp")

file(GLOB GEMS3K_SOURCE_FILES ${GEMS3K_DIR}/*.cpp)
file(GLOB GEMS3K_HEADER_FILES ${GEMS3K_DIR}/*.h)

add_executable(gfshelpconfig-qhp-generator
                  help.h help.cpp qhp-generator.cpp
                  ${GEMS3K_HEADER_FILES} ${GEMS3K_SOURCE_FILES} )

#target_link_libraries(gfshelpconfig-qhp-generator GEMS3K::GEMS3K)
if(USE_THERMOFUN MATCHES ON)
    target_link_libraries(gfshelpconfig-qhp-generator ThermoFun::ThermoFun)
 else()
    if(USE_SPDLOG_PRECOMPILED)
      target_link_libraries(gfshelpconfig-qhp-generator spdlog::spdlog)
    endif()
endif()
