PROJECT(ViennaGridNG)

cmake_minimum_required(VERSION 2.6)


    SET (CMAKE_C_COMPILER             "/usr/bin/clang")
    SET (CMAKE_C_FLAGS                "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
    SET (CMAKE_CXX_FLAGS                "-Wall -ferror-limit=10")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_AR      "/usr/bin/llvm-ar")
    SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
    SET (CMAKE_NM      "/usr/bin/llvm-nm")
    SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
    SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")

# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "~/work/cmake/Modules")

set(LIBARY_PATH "../../..")

SET(VIENNADATA_INCLUDEDIR "${LIBARY_PATH}/ViennaData/")
SET(VIENNAGRID_INCLUDEDIR "${LIBARY_PATH}/ViennaGrid/")

find_package(VIENNADATA REQUIRED)
find_package(VIENNAGRID REQUIRED)

include_directories(${VIENNADATA_INCLUDE_DIRS})
include_directories(${VIENNAGRID_INCLUDE_DIRS})

set(BOOST_MIN_VERSION 1.42.0)
set(BOOST_LIBS)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

MESSAGE( STATUS "CMAKE_CXX_FLAGS_DEBUG:         " ${CMAKE_CXX_FLAGS_DEBUG} )
MESSAGE( STATUS "CMAKE_CXX_FLAGS_RELASEG        " ${CMAKE_CXX_FLAGS_RELEASE} )


add_executable(container_test container_test.cpp)
add_executable(collection_test collection_test.cpp)
add_executable(container_collection_test container_collection_test.cpp)
add_executable(container_collection_test_2 container_collection_test_2.cpp)
# add_executable(element_test element_test.cpp)
add_executable(element_test_2 element_test_2.cpp)
add_executable(element_test_3 element_test_3.cpp)
