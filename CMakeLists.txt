cmake_minimum_required(VERSION 2.6)

PROJECT(ViennaGrid)


    SET (CMAKE_C_COMPILER             "/usr/bin/clang")
    SET (CMAKE_C_FLAGS                "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
    SET (CMAKE_CXX_FLAGS                "-Wall -ferror-limit=1")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_AR      "/usr/bin/llvm-ar")
    SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
    SET (CMAKE_NM      "/usr/bin/llvm-nm")
    SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
    SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")


#if you don't want the full compiler output, remove the following line
#SET(CMAKE_VERBOSE_MAKEFILE ON)

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
ADD_EXECUTABLE(domain_setup        examples/tutorial/domain_setup.cpp)
ADD_EXECUTABLE(finite_volumes      examples/tutorial/finite_volumes.cpp)
#ADD_EXECUTABLE(io                  examples/tutorial/io.cpp)
#ADD_EXECUTABLE(iterators           examples/tutorial/iterators.cpp)
#ADD_EXECUTABLE(multi_segment       examples/tutorial/multi_segment.cpp)
#ADD_EXECUTABLE(slim_domain         examples/tutorial/slim_domain.cpp)
#ADD_EXECUTABLE(store_access_data   examples/tutorial/store_access_data.cpp)


ADD_EXECUTABLE(container_test                  examples/storage_layer/container_test.cpp)
ADD_EXECUTABLE(collection_test                 examples/storage_layer/collection_test.cpp)
ADD_EXECUTABLE(container_collection_test       examples/storage_layer/container_collection_test.cpp)
ADD_EXECUTABLE(container_collection_test_2     examples/storage_layer/container_collection_test_2.cpp)
#ADD_EXECUTABLE(element_test                    examples/storage_layer/element_test.cpp)
ADD_EXECUTABLE(element_test_2                  examples/storage_layer/element_test_2.cpp)
ADD_EXECUTABLE(element_test_3                  examples/storage_layer/element_test_3.cpp)
ADD_EXECUTABLE(domain_test                     examples/storage_layer/domain_test.cpp)
ADD_EXECUTABLE(domain_test_2                   examples/storage_layer/domain_test_2.cpp)
ADD_EXECUTABLE(domain_test_3                   examples/storage_layer/domain_test_3.cpp)
ADD_EXECUTABLE(domain_test_4                   examples/storage_layer/domain_test_4.cpp)
ADD_EXECUTABLE(domain_test_5                   examples/storage_layer/domain_test_5.cpp)