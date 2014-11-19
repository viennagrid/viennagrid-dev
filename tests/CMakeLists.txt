
# tests with CPU backend
foreach(PROG
        angle boundary coboundary
        distance_1d distance_2d distance_3d distance_boundary
        hypercube interface named_segment point
        scale segment surface vtk_writer


#             refinement refinement2 refinement3 refinement-triangles
#             voronoi_hex voronoi_rect voronoi_tet voronoi_triangle voronoi_line
#             vtk_writer
#             serialization
#             typelist typemap


#           io simplex
            )
  add_executable(${PROG}-test src/${PROG}.cpp)
  target_link_libraries(${PROG}-test    viennagrid)
  add_test(${PROG} ${PROG}-test)
endforeach(PROG)

if (ENABLE_VIENNADATA)
  ADD_EXECUTABLE(external_linkage-test  src/external_1.cpp src/external_2.cpp)
  ADD_test(external_linkage external_linkage-test)
endif()

