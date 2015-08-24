cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
include ($ENV{VIENNAUTILSPATH}/cmake/vienna_build_common.cmake NO_POLICY_SCOPE)

vienna_build_setup_project(viennagrid viennautils)

#message("CMAKE_CURRENT_LIST_FILE: ${CMAKE_CURRENT_LIST_FILE} PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR} PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
#message("INCLUDE DIRECTORES: ${VIENNA_BUILD_GLOBAL_INCLUDES} BEFORE INCLUDES: ${VIENNA_BUILD_GLOBAL_BEFORE_INCLUDES}")

set(VERSION_MAJOR 3)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

add_definitions( -D_VERSION_MAJOR_=${VERSION_MAJOR} )
add_definitions( -D_VERSION_MINOR_=${VERSION_MINOR} )
add_definitions( -D_VERSION_PATCH_=${VERSION_PATCH} )

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Common options
################

find_package(Boost QUIET)
find_package(ViennaCL QUIET)

# Compile options
#################

if (VIENNA_BUILD_IS_MAIN_PROJECT)
  set(BUILD_SHARED_LIBS 1)
  include(ViennaGridCommon)

  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-variadic-macros -Wno-long-long -Wno-c99-extensions")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-variadic-macros")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
  endif()

  option(ENABLE_PEDANTIC_FLAGS "Enable pedantic compiler flags" ON)
  if(ENABLE_PEDANTIC_FLAGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
  endif()

  message( STATUS "BUILD TYPE = ${CMAKE_BUILD_TYPE}" )
  message( STATUS "CXX FLAGS = ${CMAKE_CXX_FLAGS}" )
  message( STATUS "C FLAGS = ${CMAKE_C_FLAGS}" )
  message( STATUS "RELEASE FLAGS = ${CMAKE_CXX_FLAGS_RELEASE}" )
  message( STATUS "DEBUG FLAGS = ${CMAKE_CXX_FLAGS_DEBUG}" )
endif ()

vienna_build_add_subprojects()

include_directories(include)
add_subdirectory(src)

include_directories(BEFORE ${PROJECT_SOURCE_DIR})

# Add visibility of headers
# Necessary for Qt-Creator usage.
################
file( GLOB_RECURSE MAKE_HEADERS_VISIBLE_SRC
    *.hpp *.h)
add_custom_target( MAKE_HEADERS_VISIBLE SOURCES ${MAKE_HEADERS_VISIBLE_SRC} )

# User options
##############

if (${VIENNA_BUILD_IS_MAIN_PROJECT})

option(BUILD_TESTING "Build the tests" ON)

message(STATUS "CXX FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "C FLAGS: ${CMAKE_C_FLAGS}")

cmake_dependent_option(ENABLE_DIST "Enable the the distribution tarball target" ON "BUILD_DOXYGEN_DOCS;BUILD_MANUAL" OFF)

mark_as_advanced(BUILD_DOXYGEN_DOCS BUILD_MANUAL ENABLE_DIST)

# Find prerequisites
####################

if(BUILD_DOXYGEN_DOCS)
   find_package(Doxygen REQUIRED)
endif()

if(BUILD_MANUAL)
   find_package(LATEX REQUIRED)
endif()


# Subdirectories
################

if(BUILD_EXAMPLES)
   add_subdirectory(examples)
endif()

if(BUILD_TESTING)
   include(CTest)
   add_subdirectory(tests)
endif()

add_subdirectory(doc)

# Install
#########

message(STATUS "Install dir: ${INSTALL_INCLUDE_DIR}")
install(DIRECTORY viennagrid ${CMAKE_CURRENT_SOURCE_DIR}/include/viennagrid
   DESTINATION ${INSTALL_INCLUDE_DIR} COMPONENT dev
   FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")


# Dist target
#############

if(ENABLE_DIST)
   file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/dist")

   set(DISTDIR "${PROJECT_BINARY_DIR}/dist/ViennaGrid-${VERSION}-all-arch")
   configure_file(cmake/CMakeLists.txt.in "${DISTDIR}/CMakeLists.txt" @ONLY)

   #
   # Full package:
   #
   add_custom_target(dist
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}"

      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/LICENSE" "${DISTDIR}/LICENSE"

      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/changelog" "${DISTDIR}/changelog"

      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/README" "${DISTDIR}/README"

      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/build"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/cmake/README" "${DISTDIR}/build/README"

      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/cmake"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/cmake/copymanual.cmake" "${DISTDIR}/cmake/copymanual.cmake"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/cmake/ViennaGridCommon.cmake" "${DISTDIR}/cmake/ViennaGridCommon.cmake"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/cmake/ViennaGridConfig.cmake.in" "${DISTDIR}/cmake/ViennaGridConfig.cmake.in"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/cmake/ViennaGridConfigVersion.cmake.in" "${DISTDIR}/cmake/ViennaGridConfigVersion.cmake.in"

      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/doc"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${CMAKE_CURRENT_BINARY_DIR}/doc/viennagrid.pdf" "${DISTDIR}/doc/viennagrid.pdf"
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
         "${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen"
         "${DISTDIR}/doc/doxygen"

      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" "${DISTDIR}/doc/CMakeLists.txt"

      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/doc/Doxyfile.in" "${DISTDIR}/doc/Doxyfile.in"

      COMMAND "${CMAKE_COMMAND}" -E copy_directory
         "${PROJECT_SOURCE_DIR}/examples" "${DISTDIR}/examples"

      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/tests"
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DISTDIR}/tests/src"
      COMMAND "${CMAKE_COMMAND}" -E copy_directory
         "${PROJECT_SOURCE_DIR}/tests/src" "${DISTDIR}/tests/src"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
         "${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt" "${DISTDIR}/tests/CMakeLists.txt"

      COMMAND "${CMAKE_COMMAND}" -E copy_directory
         "${PROJECT_SOURCE_DIR}/viennagrid" "${DISTDIR}/viennagrid"

      COMMAND "${CMAKE_COMMAND}" -E tar czf ViennaGrid-${VERSION}-all-arch.tar.gz
           ViennaGrid-${VERSION}-all-arch
      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/dist"
      COMMENT "Generating dist/ViennaGrid-${VERSION}-all-arch.tar.gz"
      VERBATIM)
   add_dependencies(dist apidoc manual)

endif()

endif (${VIENNA_BUILD_IS_MAIN_PROJECT})
