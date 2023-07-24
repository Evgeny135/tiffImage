cmake_minimum_required(VERSION 3.25)
project(tiffImage)

set(CMAKE_CXX_STANDARD 14)

include_directories(EditTiffImage)

add_executable(tiffImage
        EditTiffImage/EditTiffImage.cpp
        EditTiffImage/Matrix.h EditTiffImage/OpenAndSave.h EditTiffImage/EditMatrix.h)
