cmake_minimum_required(VERSION 3.14)
project(Log LANGUAGES CXX)

# 类似加上-std=c++17
set(CMAKE_CXX_STANDARD 17)
# 要求编译器必须支持该标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 头文件库（logs/ 为公共头文件目录）
add_library(log_headers INTERFACE)
target_include_directories(log_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/logs)
