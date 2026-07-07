cmake_minimum_required(VERSION 4.2)
project(ProtonPrefixes)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CPR_USE_SYSTEM_CURL ON)

add_subdirectory(external/json)
add_subdirectory(external/cpr)

add_executable(ProtonPrefixes
        src/main.cpp
        src/colored-cout.h
        src/utils.cpp
        src/utils.h
        src/names.cpp
        src/names.h)

target_link_libraries(ProtonPrefixes PRIVATE cpr::cpr nlohmann_json::nlohmann_json)
