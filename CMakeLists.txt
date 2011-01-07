PROJECT(ViennaMesh)

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

SET(CMAKE_CXX_FLAGS " -L/home/markus/work/she/ViennaMesh/gtsio2/dist/local/native -lgtsio2")

#list all source files here
#ADD_EXECUTABLE(tut1 examples/tutorial/tut1.cpp)
#ADD_EXECUTABLE(tut2 examples/tutorial/tut2.cpp)
#ADD_EXECUTABLE(tut3 examples/tutorial/tut3.cpp)
#ADD_EXECUTABLE(tut4 examples/tutorial/tut4.cpp)
#ADD_EXECUTABLE(tut5 examples/tutorial/tut5.cpp)

#ADD_EXECUTABLE(vectorbench examples/benchmarks/vector.cpp)
#ADD_EXECUTABLE(sparsebench examples/benchmarks/sparse.cpp)
ADD_EXECUTABLE(test1 applications/tests/test1.cpp)


