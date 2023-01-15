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

CPMAddPackage(
  NAME tomlplusplus
  VERSION 3.2.0
  GITHUB_REPOSITORY "marzer/tomlplusplus"
)

if(tomlplusplus_ADDED)
    add_library(tomlplusplus INTERFACE)
    target_include_directories(tomlplusplus INTERFACE ${tomlplusplus_SOURCE_DIR}/include/)
endif()

# --- Building ---
add_library (DSCSModLoader SHARED)

target_sources(DSCSModLoader
    PRIVATE
    "src/DSCSModLoader.cpp" 
    "src/squirrel.cpp" 
    "src/GameInterface.cpp" 
    "src/utils.cpp" 
    "src/ScriptExtension.cpp"
    "src/CoSave.cpp"
    "src/SquirrelAway.cpp" 
    "src/CoSaveExtension.cpp")

target_include_directories(
    DSCSModLoader
    PUBLIC
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(DSCSModLoader PRIVATE Boost::log_setup tomlplusplus)
