FILE(GLOB_RECURSE LIB_VIENNAGRID_SOURCES viennagrid/*.cpp)
FILE(GLOB_RECURSE LIB_VIENNAGRIDPP_SOURCES viennagridpp/*.cpp)

add_library(viennagrid SHARED ${LIB_VIENNAGRID_SOURCES})
install(TARGETS viennagrid DESTINATION lib)

add_library(viennagridpp SHARED ${LIB_VIENNAGRIDPP_SOURCES})
target_link_libraries(viennagridpp viennagrid)
install(TARGETS viennagridpp DESTINATION lib)
