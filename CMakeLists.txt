cmake_minimum_required(VERSION 3.25)
project(tiffImage)

set(CMAKE_CXX_STANDARD 14)
#
#find_package(Boost 1.82.0 REQUIRED COMPONENTS program_options)
#
#include_directories(${Boost_INCLUDE_DIRS})
#
#target_link_libraries(tiffImage ${Boost_LIBRARIES})

add_executable(tiffImage
        EditTiffImage.cpp
        Matrix.h OpenAndSave.h EditMatrix.h)
