set(LIB_VIENNAGRID_SOURCES viennagrid/algorithm.cpp
                           viennagrid/io.cpp
                           viennagrid/mesh.cpp
                           viennagrid/mesh_hierarchy.cpp
                           viennagrid/plc.cpp
                           viennagrid/quantity_field.cpp
                           viennagrid/region.cpp
                           viennagrid/serialize.cpp
                           viennagrid/viennagrid.cpp
                           viennagrid/io/netgen.cpp)

add_library(viennagrid SHARED ${LIB_VIENNAGRID_SOURCES})
install(TARGETS viennagrid DESTINATION lib)


set(LIB_VIENNAGRIDPP_SOURCES viennagridpp/mesh.cpp
                             viennagridpp/point.cpp
                             viennagridpp/region.cpp)

add_library(viennagridpp SHARED ${LIB_VIENNAGRIDPP_SOURCES})
target_link_libraries(viennagridpp viennagrid)
install(TARGETS viennagridpp DESTINATION lib)
