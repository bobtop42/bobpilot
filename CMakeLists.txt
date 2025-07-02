cmake_minimum_required(VERSION 4.1)

project(AP)

include_directories(${CMAKE_SOURCE_DIR}/ap/atc)
include_directories(${CMAKE_SOURCE_DIR}/io)
add_executable(app main.cpp)