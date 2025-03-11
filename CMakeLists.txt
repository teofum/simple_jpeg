cmake_minimum_required(VERSION 3.30)
project(simple_jpeg)

set(CMAKE_CXX_STANDARD 23)

############################################################
# Options                                                  #
############################################################

option(SIMPLEJPEG_ENABLE_EXAMPLE "Enable example target" OFF)
option(SIMPLEJPEG_BUILD_SHARED_LIBS "Build shared lib for libjpeg-turbo" OFF)

############################################################
# Find libjpeg-turbo if installed                          #
############################################################
find_package(libjpeg-turbo REQUIRED)

############################################################
# Fetch and build libjpeg-turbo                            #
############################################################

# TODO make this work

#if (SIMPLEJPEG_BUILD_SHARED_LIBS)
#    set(LIBJPEGTURBO_STATIC_LIBRARY_FLAG OFF)
#    set(LIBJPEGTURBO_SHARED_LIBRARY_FLAG ON)
#else()
#    set(LIBJPEGTURBO_STATIC_LIBRARY_FLAG ON)
#    set(LIBJPEGTURBO_SHARED_LIBRARY_FLAG OFF)
#endif()
#
## Fetch and build
#include(ExternalProject)
#ExternalProject_Add(libjpeg-turbo
#        URL https://github.com/libjpeg-turbo/libjpeg-turbo/archive/refs/tags/3.1.0.zip
#        PREFIX ${CMAKE_CURRENT_LIST_DIR}/turbojpeg
#        CMAKE_ARGS
#            -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_LIST_DIR}/turbojpeg
#            -DCMAKE_BUILD_TYPE=Release
#            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
#            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
#            -DENABLE_STATIC=${LIBJPEGTURBO_STATIC_LIBRARY_FLAG}
#            -DENABLE_SHARED=${LIBJPEGTURBO_SHARED_LIBRARY_FLAG}
#            -DENABLE_TESTING=OFF
#)
#
## Wait for libjpeg-turbo build
#ExternalProject_Get_Property(libjpeg-turbo install_dir)
#
## Get the include/lib paths
#set(LIBJPEGTURBO_INCLUDE_DIR ${install_dir}/include)
#set(LIBJPEGTURBO_LIB_DIR ${install_dir}/lib)
#
## Set the paths of the libraries for specific platforms
## From https://github.com/mrdcvlsc/libjpeg-turbo-cmake-template/blob/main/CMakeLists.txt
#if (BUILD_SHARED_LIBS)
#    set(LIBJPEGTURBO_LIB "${LIBJPEGTURBO_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}turbojpeg${CMAKE_SHARED_LIBRARY_SUFFIX}")
#else()
#    set(LIBJPEGTURBO_LIB "${LIBJPEGTURBO_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}turbojpeg${CMAKE_STATIC_LIBRARY_SUFFIX}")
#endif()

############################################################
# Define simple_jpeg library                               #
############################################################

add_library(simple_jpeg STATIC simple_jpeg.cpp simple_jpeg.hpp)
target_include_directories(simple_jpeg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(simple_jpeg PUBLIC libjpeg-turbo::jpeg)

############################################################
# Example app                                              #
############################################################

if (SIMPLEJPEG_ENABLE_EXAMPLE)
    add_subdirectory(example)
endif ()