add_executable(create_mesh             create_mesh.cpp)
target_link_libraries(create_mesh      viennagridpp)

add_executable(create_mesh_region             create_mesh_region.cpp)
target_link_libraries(create_mesh_region      viennagridpp)

add_executable(iteration             iteration.cpp)
target_link_libraries(iteration      viennagridpp)

add_executable(boundary_iteration             boundary_iteration.cpp)
target_link_libraries(boundary_iteration      viennagridpp)

add_executable(iteration_volume             iteration_volume.cpp)
target_link_libraries(iteration_volume      viennagridpp)

add_executable(create_mesh_test             create_mesh_test.cpp)
target_link_libraries(create_mesh_test      viennagridpp)
