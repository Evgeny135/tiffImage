cmake_minimum_required(VERSION 3.25)
project(tiffImage)

set(CMAKE_CXX_STANDARD 14)

set(BOOST_ROOT "C:/boost")
set(BOOST_INCLUDEDIR "C:/boost/include/boost-1_83/")

FIND_PACKAGE(Boost 1.83 COMPONENTS program_options REQUIRED)

add_executable(tiffImage
        EditTiffImage.cpp
        Matrix.h OpenAndSave.h EditMatrix.h)

if(Boost_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
    message(STATUS "Found BOOST ${Boost_VERSION_STRING}")
else()
    message(STATUS "WEBSOCKET BOOST Not Found")
endif()

