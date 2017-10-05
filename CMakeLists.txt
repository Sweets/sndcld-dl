cmake_minimum_required (VERSION 2.6)
project(sndcld)

set (CMAKE_CXX_STANDARD 11)

add_executable (sndcld sndcld.cpp)
target_link_libraries (sndcld curl)
target_link_libraries (sndcld curlpp)
target_link_libraries (sndcld json11)
