# Name the project you can choose any name you want here
PROJECT(Exercise03) 

# Check requirements
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# set the compiler flags
SET(CMAKE_BUILD_TYPE debug)
SET(CMAKE_CXX_FLAGS "-Wall")

# Add path to additional packages (makes it easier to include common libraries)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

# Define where your executables should be put
set(EXECUTABLE_OUTPUT_PATH ${Exercise03_SOURCE_DIR})

# Add required packages (e.g. libraries)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)

IF ( APPLE)
    FIND_LIBRARY(GL_LIBRARY NAMES OpenGL)
ENDIF( APPLE)


# Set include directories containing used header files
INCLUDE_DIRECTORIES(
  ${Exercise03_SOURCE_DIR}/include/
)

# Set link directories for addiontional libraries
LINK_DIRECTORIES(
  ${Exercise03_SOURCE_DIR}/lib/
)

# Tell CMake to process the sub-directories
ADD_SUBDIRECTORY(src)
