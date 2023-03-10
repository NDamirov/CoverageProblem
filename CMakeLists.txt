cmake_minimum_required(VERSION 3.2.10)
project(ortools-test C CXX)

set (PROJECT_CXX_FLAGS "-std=c++17")
set (ORTOOLS_ROOT "or-tools/")
set (ORTOOLS_INCLUDE_DIRS "or-tools/include")
set (ORTOOLS_LIBRARIES "ortools::ortools")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${ORTOOLS_ROOT})
find_package(ortools REQUIRED)

include_directories(
    ${ORTOOLS_INCLUDE_DIRS}
)
add_definitions(-DUSE_GLOP)

# compiler flags
set (CMAKE_CXX_FLAGS "${PROJECT_CXX_FLAGS} ${CMAKE_CXX_FLAGS} -Wall -pedantic -g")

add_executable(test-app main.cpp)
target_link_libraries(test-app ${ORTOOLS_LIBRARIES})