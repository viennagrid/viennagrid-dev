
# tests with CPU backend
foreach(PROG boundary coboundary 
             distance_1d distance_2d distance_3d distance_boundary
             domain hypercube interface io point
             refinement refinement2 refinement3 refinement-triangles
             segment simplex surface
             voronoi_hex voronoi_rect voronoi_tet voronoi_triangle voronoi_line
             vtk_writer serialization)
   add_executable(${PROG}-test src/${PROG}.cpp)
   add_test(${PROG} ${PROG}-test)
endforeach(PROG)

ADD_EXECUTABLE(external_linkage  src/external_1.cpp
                                 src/external_2.cpp)
ADD_test(external_linkage external_linkage)


