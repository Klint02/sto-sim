cmake_minimum_required(VERSION 3.10.0)
project(miniproject VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sto-sim STATIC src/stosim.cpp)

target_include_directories(sto-sim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(sto-sim-miniproject main.cpp)

target_link_libraries(sto-sim-miniproject PRIVATE sto-sim)