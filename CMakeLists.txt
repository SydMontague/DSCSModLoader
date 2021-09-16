cmake_minimum_required (VERSION 3.20)

project ("DSCSModLoader")

set(CMAKE_CXX_STANDARD 20)

# Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.66 REQUIRED COMPONENTS log)

add_subdirectory ("DSCSModLoader")
