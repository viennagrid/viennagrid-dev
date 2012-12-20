# DO NOT CUSTOMIZE THIS FILE DIRECTLY!
# You can provide overrides/customizations in the file
# dashboardCustomInitialCache.cmake.

# include custom initial cache
get_filename_component(_VIENNAGRID_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${_VIENNAGRID_CMAKE_DIR}/dashboardCustomInitialCache.cmake OPTIONAL)

# COMPILER INFORMATION
######################

# Set to the C and CXX compiler
if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
   if(CTEST_BUILD_NAME MATCHES gcc4.4.5)
     set(CMAKE_C_COMPILER "/usr/bin/gcc-4.4")
     set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.4")
   elseif(CTEST_BUILD_NAME MATCHES gcc4.5.2)
     set(CMAKE_C_COMPILER "/usr/bin/gcc-4.5")
     set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.5")
   elseif(CTEST_BUILD_NAME MATCHES clang2.9)
      set(CMAKE_C_COMPILER "/usr/bin/clang")
      set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
   else()
     message(FATAL_ERROR
       "Can't determine comiler to use (CTEST_BUILD_NAME = ${CTEST_BUILD_NAME})")
   endif()
endif()
set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER}" CACHE FILEPATH "Path to the C compiler")
set(CMAKE_CXX_COMPILER "${CMAKE_CXX_COMPILER}" CACHE FILEPATH "Path to the C++ compiler")

# CONFIGURATION OPTIONS
#######################

option(ENABLE_DIST "Build the dist package" FALSE)
option(BUILD_DOXYGEN_DOCS "Build the doxygen docs" TRUE)
option(BUILD_EXAMPLES "Build the example applications" TRUE)
option(BUILD_MANUAL "Build the manual" TRUE)
option(BUILD_TESTING "Build the tests" FALSE)
