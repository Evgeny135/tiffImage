cmake_minimum_required(VERSION 3.25)
project(tiffImage)

set(CMAKE_CXX_STANDARD 14)

set(BOOST_ROOT $ENV{BOOST_ROOT})
set(BOOST_INCLUDEDIR $ENV{BOOST_INCLUDEDIR})
FIND_PACKAGE(Boost 1.83 COMPONENTS program_options REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(tiffImage
        EditTiffImage.cpp
        Matrix.h OpenAndSave.h EditMatrix.h RestoreParam.h)
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS})
if(Boost_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
    message(STATUS "Found BOOST ${Boost_VERSION_STRING}")
else()
    message(STATUS "WEBSOCKET BOOST Not Found")
endif()

