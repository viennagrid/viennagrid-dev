PROJECT(ViennaGrid)

cmake_minimum_required(VERSION 2.4)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#specify include and source directory
INCLUDE_DIRECTORIES(".")

#add definitions, compiler switches, etc.
IF(DEFINED CMAKE_BUILD_TYPE)
 SET (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
ELSE()
 SET (CMAKE_BUILD_TYPE Release)
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
   #ADD_DEFINITIONS(-Wall -O0 -g)
   ADD_DEFINITIONS(-Wall -pedantic -O3)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#SET(CMAKE_CXX_FLAGS " -L../gtsio2/dist/local/native -lgtsio2")
SET(CMAKE_CXX_FLAGS " -L. -lgtsio2")

#list all source files here
ADD_EXECUTABLE(test_io    applications/tests/test_io.cpp)
ADD_EXECUTABLE(test_setup applications/tests/test_setup.cpp)


