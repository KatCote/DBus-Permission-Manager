cmake_minimum_required(VERSION 3.10)
project(PermissionsService)

set(CMAKE_CXX_STANDARD 17)

find_package(sdbus-c++ REQUIRED)

add_executable(DBus-Permission-Manager src/permissions_service.cpp)
target_link_libraries(DBus-Permission-Manager sdbus-c++)