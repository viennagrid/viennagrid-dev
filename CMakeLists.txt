cmake_minimum_required(VERSION 2.6)

PROJECT(ViennaGrid)


#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#specify include and source directory
INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("../ViennaData")      #ViennaData


#add definitions, compiler switches, etc.
IF(DEFINED CMAKE_BUILD_TYPE)
 SET (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
ELSE()
 SET (CMAKE_BUILD_TYPE Release)
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
   ADD_DEFINITIONS(-Wall -O0 -g)
   #ADD_DEFINITIONS(-Wall -pedantic -O3)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#list all source files here
ADD_EXECUTABLE(test_io          examples/tests/test_io.cpp)
ADD_EXECUTABLE(test_simplex     examples/tests/test_simplex.cpp)
ADD_EXECUTABLE(test_hypercube   examples/tests/test_hypercube.cpp)
ADD_EXECUTABLE(test_coboundary  examples/tests/test_coboundary.cpp)
ADD_EXECUTABLE(test_domain      examples/tests/test_domain.cpp)
ADD_EXECUTABLE(test_segment     examples/tests/test_segment.cpp)
ADD_EXECUTABLE(test_point       examples/tests/test_point.cpp)
