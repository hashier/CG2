# choose a title for yout project
PROJECT( exe01 )

# check for minimum requirements (do not change this)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

# set the compiler flags (do not change this)
SET(CMAKE_BUILD_TYPE debug)
SET(CMAKE_CXX_FLAGS "-Wall")

# set path to additional packages (makes it easier to include common libraries) (do not change this)
set(CMAKE_MODULE_PATH "./CMakeModules/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

# search for required packages (do not change this)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)


# setup the executable output path here
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


# setup your include directories here
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )


# setup your link directories for additional libraries here
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    )


# setup subdirectories here, that shall be processed by CMake
add_subdirectory(src)

