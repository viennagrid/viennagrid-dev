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
#ADD_EXECUTABLE(test_io          examples/tests/test_io.cpp)
#ADD_EXECUTABLE(test_simplex     examples/tests/test_simplex.cpp)
#ADD_EXECUTABLE(test_hypercube   examples/tests/test_hypercube.cpp)
#ADD_EXECUTABLE(test_coboundary  examples/tests/test_coboundary.cpp)
#ADD_EXECUTABLE(test_domain      examples/tests/test_domain.cpp)
#ADD_EXECUTABLE(test_segment     examples/tests/test_segment.cpp)
#ADD_EXECUTABLE(test_point       examples/tests/test_point.cpp)
#ADD_EXECUTABLE(test_refinement  examples/tests/test_refinement.cpp)
IF(HDF5_FOUND)
   ADD_EXECUTABLE(test_tdr         examples/tests/test_tdr.cpp)
   TARGET_LINK_LIBRARIES(test_tdr  ${HDF5_LIBRARIES})
ENDIF(HDF5_FOUND)

#ADD_EXECUTABLE(voronoi_rect      examples/tutorial/voronoi_rect.cpp)
#ADD_EXECUTABLE(voronoi_triangle  examples/tutorial/voronoi_triangle.cpp)
#ADD_EXECUTABLE(voronoi_tet       examples/tutorial/voronoi_tet.cpp)


