cmake_minimum_required(VERSION 3.0)

project(ExampleProject VERSION 1.2.3)

message(STATUS "------------------------------------")

message(STATUS "Project name: ${PROJECT_NAME}")
message(STATUS "Project version: ${PROJECT_VERSION} (Major: ${PROJECT_VERSION_MAJOR}, Minor: ${PROJECT_VERSION_MINOR}, Patch: ${PROJECT_VERSION_PATCH})")

message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Current source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Current binary directory: ${CMAKE_CURRENT_BINARY_DIR}")

message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")

message(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "System version: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Pointer size: ${CMAKE_SIZEOF_VOID_P}")

if(WIN32)
    message(STATUS "Building for Windows")
elseif(UNIX)
    message(STATUS "Building for Unix or Unix-like OS")
    if(APPLE)
        message(STATUS "Building for macOS")
    endif()
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

message(STATUS "------------------------------------")