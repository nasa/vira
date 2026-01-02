cmake_minimum_required(VERSION 3.16)
project(vira_packaging_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(vira REQUIRED)

add_executable(packaging_test main.cpp)
target_link_libraries(packaging_test PRIVATE vira::vira)

enable_testing()
add_test(NAME packaging_test COMMAND packaging_test)