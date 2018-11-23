cmake_minimum_required(VERSION 3.5)
project(Box_Filter)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(Box_Filter main.cpp)
find_package(OpenCV REQUIRED)

include_directories(Box_Filter ${OpenCV_INCLUDE_DIRS})
target_link_libraries(Box_Filter ${OpenCV_LIBS})
