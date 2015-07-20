add_executable(create_mesh             create_mesh.cpp)
target_link_libraries(create_mesh      viennagridpp)

add_executable(create_mesh_region             create_mesh_region.cpp)
target_link_libraries(create_mesh_region      viennagridpp)

add_executable(create_batch_mesh_region             create_batch_mesh_region.cpp)
target_link_libraries(create_batch_mesh_region      viennagridpp)

add_executable(create_mesh_region_boundary             create_mesh_region_boundary.cpp)
target_link_libraries(create_mesh_region_boundary      viennagridpp)



add_executable(iteration             iteration.cpp)
target_link_libraries(iteration      viennagridpp)

add_executable(boundary_iteration             boundary_iteration.cpp)
target_link_libraries(boundary_iteration      viennagridpp)

add_executable(iteration_volume             iteration_volume.cpp)
target_link_libraries(iteration_volume      viennagridpp)

add_executable(create_mesh_test             create_mesh_test.cpp)
target_link_libraries(create_mesh_test      viennagridpp)

add_executable(memory_benchmark             memory_benchmark.cpp)
target_link_libraries(memory_benchmark      viennagridpp)

