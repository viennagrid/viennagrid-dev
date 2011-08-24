cmake_minimum_required(VERSION 2.6)

PROJECT(ViennaGrid)


#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#specify include and source directory
INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("../ViennaData")      #ViennaData

#look for HDF5
find_package(HDF5 COMPONENTS CXX)
IF(HDF5_FOUND)
   INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIRS})
ENDIF(HDF5_FOUND)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   INCLUDE_DIRECTORIES("/opt/local/include") 
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

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

#list all source files here
IF(HDF5_FOUND)
   ADD_EXECUTABLE(test_tdr         examples/tests/test_tdr.cpp)
   TARGET_LINK_LIBRARIES(test_tdr  ${HDF5_LIBRARIES})
ENDIF(HDF5_FOUND)


