cmake_minimum_required(VERSION 3.0)
project(z10-infodisplay)

include(FindPkgConfig)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(LIBGTOP REQUIRED libgtop-2.0)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14") # Enable C++14 features
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") # Turn on warnings


add_executable(z10-infodisplay WIN32 src/display/Canvas.cpp src/main.cpp)

target_link_libraries(z10-infodisplay -lg15daemon_client -lgtop-2.0 -lglib-2.0)
target_include_directories(z10-infodisplay PUBLIC ${GLIB_INCLUDE_DIRS})
target_include_directories(z10-infodisplay PUBLIC ${LIBGTOP_INCLUDE_DIRS})
