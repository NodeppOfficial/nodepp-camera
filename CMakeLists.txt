cmake_minimum_required(VERSION 3.6)
project(nodepp-camera VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(NODEPP_CAMERA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Search for Nodepp Core
if (NOT TARGET nodepp)
    find_package(nodepp REQUIRED)
    if(NOT nodepp_FOUND)
        message(FATAL_ERROR "Nodepp not found. Please install nodepp.")
    endif()
endif()

find_package(Threads REQUIRED)

find_package(LibUSB-1.0 REQUIRED)

# Search for LibUVC
find_library(LIBUVC_LIBRARY NAMES uvc libuvc REQUIRED)
if(NOT LIBUVC_LIBRARY)
    message(FATAL_ERROR "LibUVC library file not found. Please install libuvc-dev or equivalent files.")
endif()

find_path(LIBUVC_INCLUDE_DIR NAMES libuvc.h REQUIRED
    HINTS /usr/include /usr/local/include
    PATH_SUFFIXES libuvc
)
if(NOT LIBUVC_INCLUDE_DIR)
    message(FATAL_ERROR "LibUVC header files (libuvc.h) not found.")
endif()

# interface-target
add_library(nodepp-camera INTERFACE)

# Combine all necessary include directories
target_include_directories(nodepp-camera INTERFACE
    ${NODEPP_CAMERA_INCLUDE_DIR}
    ${LIBUVC_INCLUDE_DIR}
    ${LIBUSB_1_0_INCLUDE_DIRS}
)

# Link against all dependencies (using robust targets/variables)
target_link_libraries(nodepp-camera INTERFACE
    nodepp 
    ${LIBUVC_LIBRARY}
    ${LIBUSB_1_0_LIBRARIES}
    Threads::Threads
)