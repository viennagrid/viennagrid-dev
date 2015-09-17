# Tutorials:

# ADD_EXECUTABLE(viennagrid3_playground           viennagrid3_playground.cpp)
# target_link_libraries(viennagrid3_playground    viennagrid)

add_executable(accessor             accessor.cpp)
target_link_libraries(accessor      viennagridpp)

ADD_EXECUTABLE(algorithms           algorithms.cpp)
target_link_libraries(algorithms    viennagridpp)

add_executable(coboundary_iteration coboundary_iteration.cpp)
target_link_libraries(coboundary_iteration    viennagridpp)

ADD_EXECUTABLE(mesh_setup           mesh_setup.cpp)
target_link_libraries(mesh_setup    viennagridpp)

ADD_EXECUTABLE(mesh_setup_c         mesh_setup.c)
target_link_libraries(mesh_setup_c  viennagrid)

# add_executable(element_erase        element_erase.cpp)
# target_link_libraries(element_erase    viennagrid)

# ADD_EXECUTABLE(finite_volumes       finite_volumes.cpp)
# target_link_libraries(finite_volumes    viennagrid)

if (Boost_FOUND AND ViennaCL_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  include_directories(${ViennaCL_INCLUDE_DIRS})
  ADD_EXECUTABLE(finite_elements       finite_elements.cpp)
  target_link_libraries(finite_elements    viennagridpp)
endif()

ADD_EXECUTABLE(io                   io.cpp)
target_link_libraries(io     viennagridpp)

ADD_EXECUTABLE(iterators            iterators.cpp)
target_link_libraries(iterators     viennagridpp)

ADD_EXECUTABLE(multi_region        multi_region.cpp)
target_link_libraries(multi_region viennagridpp)

add_executable(neighbor_iteration   neighbor_iteration.cpp)
target_link_libraries(neighbor_iteration viennagridpp)

add_executable(named_regions       named_regions.cpp)
target_link_libraries(named_regions viennagridpp)

add_executable(polygon              polygon.cpp)
target_link_libraries(polygon viennagridpp)

add_executable(regions             regions.cpp)
target_link_libraries(regions viennagridpp)

add_executable(interface            interface.cpp)
target_link_libraries(interface viennagridpp)

add_executable(read_plc             read_plc.cpp)
target_link_libraries(read_plc viennagridpp)

add_executable(dump_info            dump_info.cpp)
target_link_libraries(dump_info viennagridpp)

add_executable(vmesh_creator        vmesh_creator.cpp)
target_link_libraries(vmesh_creator viennagridpp)

add_executable(copy_elements        copy_elements.cpp)
target_link_libraries(copy_elements viennagridpp)

add_executable(convert_to_mphtxt    convert_to_mphtxt.cpp)
target_link_libraries(convert_to_mphtxt viennagridpp)

add_executable(hybrid    hybrid.cpp)
target_link_libraries(hybrid viennagridpp)

add_executable(hybrid_3d    hybrid_3d.cpp)
target_link_libraries(hybrid_3d viennagridpp)

add_executable(hierarchy    hierarchy.cpp)
target_link_libraries(hierarchy viennagridpp)

add_executable(plc_test    plc_test.cpp)
target_link_libraries(plc_test viennagridpp)

add_executable(serialize    serialize.cpp)
target_link_libraries(serialize viennagridpp)

add_executable(point_ntree    point_ntree.cpp)
target_link_libraries(point_ntree viennagridpp)

add_executable(netgen_c_reader    netgen_c_reader.cpp)
target_link_libraries(netgen_c_reader viennagridpp)

# Old (TODO cleanup, merge to tests)
#add_executable(collection_test              old/collection_test.cpp)
#add_executable(container_collection_test_2  old/container_collection_test_2.cpp)
#add_executable(container_test               old/container_test.cpp)
#add_executable(copy_mesh_test             old/copy_mesh_test.cpp)
#add_executable(element_test_3               old/element_test_3.cpp)
#add_executable(element_test_4               old/element_test_4.cpp)
#add_executable(element_erase_refine_test    old/element_erase_refine_test.cpp)
#add_executable(mesh_test                  old/mesh_test.cpp)
#add_executable(mesh_test_2                old/mesh_test_2.cpp)
#add_executable(mesh_test_3                old/mesh_test_3.cpp)
#add_executable(mesh_test_4                old/mesh_test_4.cpp)
#add_executable(mesh_test_5                old/mesh_test_5.cpp)
#add_executable(trigate_read_test            old/trigate_read_test.cpp)
# add_executable(thin_mesh            thin_mesh.cpp)

#add_executable(plc_reader_test              old/plc_reader_test.cpp)
#add_executable(handle_test                  old/handle_test.cpp)
#if (ENABLE_VIENNADATA)
#  add_executable(accessor_test              old/accessor_test.cpp)
#endif()
#add_executable(coboundary_iteration_with_segments     old/coboundary_iteration_with_segments.cpp)
# ADD_EXECUTABLE(slim_mesh                  old/slim_mesh.cpp)
# ADD_EXECUTABLE(store_access_data            old/store_access_data.cpp)
#add_executable(vtk_reader                   old/vtk_reader.cpp)
#add_executable(vtk_writer                   old/vtk_writer.cpp)
#add_executable(point                        old/point.cpp)
