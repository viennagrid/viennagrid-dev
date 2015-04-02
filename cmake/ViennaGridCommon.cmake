
include(CMakeDependentOption)

# Installation directories
##########################

set(INSTALL_INCLUDE_DIR include CACHE PATH
   "Installation directory for headers")

if(WIN32 AND NOT CYGWIN)
   set(DEF_INSTALL_CMAKE_DIR CMake)
else()
   set(DEF_INSTALL_CMAKE_DIR lib/cmake/viennagrid)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
   "Installation directory for CMake files")

if(NOT IS_ABSOLUTE "${INSTALL_CMAKE_DIR}")
   set(INSTALL_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR}")
endif()
file(RELATIVE_PATH CONF_REL_INSTALL_PREFIX "${INSTALL_CMAKE_DIR}"
   "${CMAKE_INSTALL_PREFIX}")
if(NOT IS_ABSOLUTE "${INSTALL_INCLUDE_DIR}")
   set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")
endif()
file(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")

# User options
##############

option(BUILD_DOXYGEN_DOCS "Build the Doxygen-generated API docs" OFF)

option(BUILD_MANUAL "Build the PDF manual" OFF)

option(BUILD_EXAMPLES "Build example programs" ON)

option(ENABLE_VIENNADATA "Enable ViennaData for advanced accessors" OFF)

mark_as_advanced(ENABLE_PEDANTIC_FLAGS)

# Set high warning level on GCC
if(ENABLE_PEDANTIC_FLAGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
endif()

if(ENABLE_VIENNADATA)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVIENNAGRID_WITH_VIENNADATA")
endif()


# Export
########

configure_file(cmake/ViennaGridConfig.cmake.in
   ${PROJECT_BINARY_DIR}/ViennaGridConfig.cmake @ONLY)

configure_file(cmake/ViennaGridConfigVersion.cmake.in
   ${PROJECT_BINARY_DIR}/ViennaGridConfigVersion.cmake @ONLY)

if (CMAKE_MINOR_VERSION GREATER 6)  # export(PACKAGE ...) introduced with CMake 2.8.0
  export(PACKAGE ViennaGrid)
endif()


# Install
#########

install(FILES
   ${PROJECT_BINARY_DIR}/ViennaGridConfig.cmake
   ${PROJECT_BINARY_DIR}/ViennaGridConfigVersion.cmake
   DESTINATION ${INSTALL_CMAKE_DIR} COMPONENT dev)
