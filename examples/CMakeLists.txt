set(EXAMPLES_TESTDATA
   data/cube3072.mesh
   data/cube384.mesh
   data/cube48.mesh
   data/cube6.mesh
   data/hexahedron2.vtu
   data/interconnect3d.mesh
   data/line8.mesh
   data/lines_3d.vtu
   data/multi_segment_hex_0.vtu
   data/multi_segment_hex_1.vtu
   data/multi_segment_hex_main.pvd
   data/multi_segment_quad_0.vtu
   data/multi_segment_quad_1.vtu
   data/multi_segment_quad_main.pvd
   data/multi_segment_tet_0.vtu
   data/multi_segment_tet_1.vtu
   data/multi_segment_tet_main.pvd
   data/multi_segment_tri_0.vtu
   data/multi_segment_tri_1.vtu
   data/multi_segment_tri_main.pvd
   data/opendx_visualization.net
   data/quadrilateral2.vtu
   data/square128.mesh
   data/square32.mesh
   data/square8.mesh
   data/sshape2d.mesh
   data/sshape3d.mesh
   data/tets_with_data_0.vtu
   data/tets_with_data_1.vtu
   data/tets_with_data_main.pvd
   data/twocubes.mesh
   )
foreach(f IN LISTS EXAMPLES_TESTDATA)
   configure_file(${f} "${CMAKE_CURRENT_BINARY_DIR}/${f}" COPYONLY)
endforeach()

add_subdirectory(tutorial)
