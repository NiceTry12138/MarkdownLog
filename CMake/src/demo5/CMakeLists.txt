

cmake_minimum_required(VERSION 3.0)

project(CALC)

# 输出一般日志信息
message(STATUS "status: ${PROJECT_SOURCE_DIR}")
# 输出警告信息
message(WARNING "warning: ${PROJECT_SOURCE_DIR}")

set(TEMP "Hello")
set(TEMP ${TEMP} " World")
message(${TEMP})
