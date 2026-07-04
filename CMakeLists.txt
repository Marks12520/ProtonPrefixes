cmake_minimum_required(VERSION 4.2)
project(ProtonPrefixes)

set(CMAKE_CXX_STANDARD 26)

add_executable(ProtonPrefixes main.cpp
        colored-cout.h)

target_link_libraries(ProtonPrefixes PRIVATE cpr)

