
PROJECT(viennagrid)

cmake_minimum_required(VERSION 2.6)

#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)

#specify include directory
INCLUDE_DIRECTORIES("..")
INCLUDE_DIRECTORIES("../../ViennaData/")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   INCLUDE_DIRECTORIES("/opt/local/include") 
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#add definitions, compiler switches, etc.
#ADD_DEFINITIONS(-Wall -pedantic -O3)
ADD_DEFINITIONS(-Wall -pedantic -O0 -g)


#-----------------------------------------------------------------------------
# Configure Dart testing support.  This should be done before any
# MESSAGE(FATAL_ERROR ...) commands are invoked.
INCLUDE(CTest)
ENABLE_TESTING()
IF(BUILD_TESTING)
  SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
  MARK_AS_ADVANCED(BUILDNAME)
ENDIF(BUILD_TESTING)

#-----------------------------------------------------------------------------
# add the source files which should be tested without the trailing *.cpp
SET(PROGS coboundary domain hypercube io segment simplex refinement refinement2 refinement3)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
FOREACH(PROG ${PROGS})
   ADD_EXECUTABLE(${PROG} ../src/${PROG}.cpp)
   ADD_test(${PROG} ${PROG})
ENDFOREACH(PROG)
#-----------------------------------------------------------------------------
