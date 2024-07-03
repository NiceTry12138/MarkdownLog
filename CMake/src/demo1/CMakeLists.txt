cmake_minimum_required(VERSION 3.0)

project(CALC)

set(SRC_FILES add.c;div.c;main.c;mult.c;sub.c)
# set(CMAKE_CXX_STANDARD 17)

set(HOME ./build)
set(EXECUTABLE_OUTPUT_PATH ${HOME}/bin)

file(GLOB MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB MAIN_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# message(${MAIN_SRC})
# message(${MAIN_HEAD})

message(${PROJECT_SOURCE_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})

FOREACH(item ${MAIN_SRC})
    message(${item})
ENDFOREACH()

add_executable(app ${SRC_FILES})