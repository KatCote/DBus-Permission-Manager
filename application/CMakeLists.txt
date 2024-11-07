cmake_minimum_required(VERSION 3.10)
project(Application)

set(CMAKE_CXX_STANDARD 17)

find_package(sdbus-c++ REQUIRED)

add_executable(Application src/main.cpp)

target_link_libraries(Application sdbus-c++)