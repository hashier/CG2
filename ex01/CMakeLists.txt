# choose a title for yout project
PROJECT( ? ) 

# check for minimum requirements (do not change this)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

# set the compiler flags (do not change this)
SET(CMAKE_BUILD_TYPE debug)
SET(CMAKE_CXX_FLAGS "-Wall")

# set path to additional packages (makes it easier to include common libraries) (do not change this)
set(CMAKE_MODULE_PATH "./CMakeModules/")

# search for required packages (do not change this)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)

# setup the executable output path here

# setup your include directories here

# setup your link directories for additional libraries here

# setup subdirectories here, that shall be processed by CMake
