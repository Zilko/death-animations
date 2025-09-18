cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(DeathAnimations VERSION 1.0.0)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

CPMAddPackage("gh:xiph/ogg#v1.3.6")
CPMAddPackage("gh:xiph/opus#v1.5.2")
CPMAddPackage(
    NAME opusfile
    GITHUB_REPOSITORY xiph/opusfile
    GIT_TAG 8890ea3
    OPTIONS "OP_DISABLE_DOCS ON" "OP_DISABLE_HTTP ON"
)

target_link_libraries(${PROJECT_NAME} ogg opus opusfile)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_BINARY_DIR}/_deps/ogg-src/include"
    "${CMAKE_BINARY_DIR}/_deps/opus-src/include"
    "${CMAKE_BINARY_DIR}/_deps/opusfile-src/include"
)

setup_geode_mod(${PROJECT_NAME})
