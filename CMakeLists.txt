cmake_minimum_required(VERSION 3.26)
project(to_do_list_proj)
set(CMAKE_CXX_STANDARD 17)

set(ENV{OPENCV_DIR} "C:\\tools\\opencv\\build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(to_do_list_proj main.cpp)

target_link_libraries(to_do_list_proj ${OpenCV_LIBS})