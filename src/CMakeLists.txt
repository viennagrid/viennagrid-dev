set(LIB_VIENNAGRID_SOURCES viennagrid/algorithm.cpp
                           viennagrid/io.cpp
                           viennagrid/dynamic_sizeof.cpp
                           viennagrid/element_buffer.cpp
                           viennagrid/element_handle_buffer.cpp
                           viennagrid/mesh.cpp
                           viennagrid/mesh_hierarchy.cpp
                           viennagrid/plc.cpp
                           viennagrid/quantity_field.cpp
                           viennagrid/region.cpp
                           viennagrid/serialize.cpp
                           viennagrid/viennagrid.cpp
                           viennagrid/io/netgen.cpp
                           viennagrid/io/vtk.cpp
                           viennagrid/algorithm/centroid.cpp
                           viennagrid/algorithm/circumcenter.cpp
                           viennagrid/algorithm/closest_points.cpp
                           viennagrid/algorithm/inclusion.cpp
                           viennagrid/algorithm/quantity_interpolate.cpp
                           viennagrid/algorithm/surface.cpp
                           viennagrid/algorithm/volume.cpp)

add_library(viennagrid SHARED ${LIB_VIENNAGRID_SOURCES})
install(TARGETS viennagrid DESTINATION lib)


set(LIB_VIENNAGRIDPP_SOURCES viennagridpp/mesh.cpp
                             viennagridpp/point.cpp
                             viennagridpp/region.cpp)

add_library(viennagridpp SHARED ${LIB_VIENNAGRIDPP_SOURCES})
target_link_libraries(viennagridpp viennagrid)
install(TARGETS viennagridpp DESTINATION lib)
