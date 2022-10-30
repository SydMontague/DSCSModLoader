# --- Project Setup ---
cmake_minimum_required (VERSION 3.21)

project ("DSCSModLoader")

set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)

# --- Sanity Checks ---
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(FATAL_ERROR "This project must not be built in Debug mode!")
endif()

# --- Dependencies ---
CPMAddPackage(
  NAME Boost
  VERSION 1.80.0
  GITHUB_REPOSITORY "boostorg/boost"
  GIT_TAG "boost-1.80.0"
)

# --- Building ---
add_library (DSCSModLoader SHARED)

target_sources(DSCSModLoader
    PRIVATE
    "src/DSCSModLoader.cpp" 
    "src/squirrel.cpp" 
    "src/GameInterface.cpp" 
    "src/utils.cpp" 
    "src/ScriptExtension.cpp" 
)

target_include_directories(
    DSCSModLoader
    PUBLIC
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(DSCSModLoader PRIVATE Boost::log_setup)
