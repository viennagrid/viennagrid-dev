# Tutorials:

ADD_EXECUTABLE(viennagrid3_playground           viennagrid3_playground.cpp)
target_link_libraries(viennagrid3_playground    viennagrid)

add_executable(accessor             accessor.cpp)
target_link_libraries(accessor      viennagrid)

ADD_EXECUTABLE(algorithms           algorithms.cpp)
target_link_libraries(algorithms    viennagrid)

add_executable(coboundary_iteration coboundary_iteration.cpp)
target_link_libraries(coboundary_iteration    viennagrid)

ADD_EXECUTABLE(mesh_setup           mesh_setup.cpp)
target_link_libraries(mesh_setup    viennagrid)

# add_executable(element_erase        element_erase.cpp)
# target_link_libraries(element_erase    viennagrid)

# ADD_EXECUTABLE(finite_volumes       finite_volumes.cpp)
# target_link_libraries(finite_volumes    viennagrid)

ADD_EXECUTABLE(io                   io.cpp)
target_link_libraries(io     viennagrid)

ADD_EXECUTABLE(iterators            iterators.cpp)
target_link_libraries(iterators     viennagrid)

ADD_EXECUTABLE(multi_segment        multi_segment.cpp)
target_link_libraries(multi_segment viennagrid)

add_executable(neighbor_iteration   neighbor_iteration.cpp)
target_link_libraries(neighbor_iteration viennagrid)

add_executable(named_segments       named_segments.cpp)
target_link_libraries(named_segments viennagrid)

add_executable(polygon              polygon.cpp)
target_link_libraries(polygon viennagrid)

add_executable(segments             segments.cpp)
target_link_libraries(segments viennagrid)

add_executable(interface            interface.cpp)
target_link_libraries(interface viennagrid)

add_executable(read_plc             read_plc.cpp)
target_link_libraries(read_plc viennagrid)

add_executable(dump_info            dump_info.cpp)
target_link_libraries(dump_info viennagrid)

add_executable(vmesh_creator        vmesh_creator.cpp)
target_link_libraries(vmesh_creator viennagrid)

add_executable(copy_elements        copy_elements.cpp)
target_link_libraries(copy_elements viennagrid)

add_executable(convert_to_mphtxt    convert_to_mphtxt.cpp)
target_link_libraries(convert_to_mphtxt viennagrid)

add_executable(hybrid    hybrid.cpp)
target_link_libraries(hybrid viennagrid)

add_executable(plc_test    plc_test.cpp)
target_link_libraries(plc_test viennagrid)




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
