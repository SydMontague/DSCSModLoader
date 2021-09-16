cmake_minimum_required (VERSION 3.8)

include_directories(../include/)
include_directories( ${Boost_INCLUDE_DIR} )

add_library (DSCSModLoader SHARED "DSCSModLoader.cpp" "DSCSModLoader.h" "squirrel.cpp")
target_link_libraries(DSCSModLoader PUBLIC ${Boost_LIBRARIES})