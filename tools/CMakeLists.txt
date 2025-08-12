

# Generate Qt Help Project File gems3helpconfig.qhp
message(STATUS "Generate Qt Help Project File gfshelpconfig.qhp")
add_executable(gfshelpconfig-qhp-generator help.h help.cpp qhp-generator.cpp)
target_link_libraries(gfshelpconfig-qhp-generator GEMS3K::GEMS3K)
