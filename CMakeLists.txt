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
   ADD_DEFINITIONS(-Wall -pedantic -O0 -g)
   #ADD_DEFINITIONS(-Wall -pedantic -O3)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_EXECUTABLE(algorithms          examples/tutorial/algorithms.cpp)
ADD_EXECUTABLE(finite_volumes      examples/tutorial/finite_volumes.cpp)
ADD_EXECUTABLE(io                  examples/tutorial/io.cpp)
ADD_EXECUTABLE(iterators           examples/tutorial/iterators.cpp)
ADD_EXECUTABLE(multi_segment       examples/tutorial/multi_segment.cpp)
ADD_EXECUTABLE(slim_domain         examples/tutorial/slim_domain.cpp)
