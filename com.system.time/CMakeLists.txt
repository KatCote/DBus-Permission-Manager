cmake_minimum_required(VERSION 3.10)
project(TimeService)

set(CMAKE_CXX_STANDARD 17)

find_package(sdbus-c++ REQUIRED)

add_executable(DBus-Time-Service src/time_service.cpp)

target_link_libraries(DBus-Time-Service sdbus-c++)