/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#ifdef _MSC_VER
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include "viennagrid/forwards.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/io/vtk_writer.hpp"

//
//    Let us construct the following input mesh:
//
//    5---------4---------3
//    | \       | \       |
//    |   \     |   \     |    y
//    |     \   |     \   |     ^
//    |       \ |       \ |     |
//    0---------1---------2     *--> x
//
//    Segment 1 | Segment 2
//


int main()
{
  //
  // Define the necessary types:
  //

  typedef viennagrid::thin_triangular_2d_mesh                  MeshType;
  typedef viennagrid::result_of::segmentation<MeshType>::type SegmentationType;
  typedef viennagrid::result_of::segment_handle<SegmentationType>::type SegmentHandleType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::vertex_handle<MeshType>::type    VertexHandleType;

  typedef viennagrid::result_of::cell<MeshType>::type             CellType;
  typedef viennagrid::result_of::cell_range<SegmentHandleType>::type      CellRange;
  typedef viennagrid::result_of::iterator<CellRange>::type          CellIterator;

  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << "-- ViennaGrid tutorial: Setup of a mesh with two segments -- " << std::endl;
  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << std::endl;

  //
  // Step 1: Instantiate the mesh and the segmentation and create 2 segments:
  //
  MeshType mesh;
  SegmentationType segmentation(mesh);

  SegmentHandleType seg0 = segmentation.make_segment();
  SegmentHandleType seg1 = segmentation.make_segment();

  //
  // Step 2: Add vertices to the mesh.
  //         Note that vertices with IDs are enumerated in the order they are pushed to the mesh.
  //
  VertexHandleType vh0 = viennagrid::make_vertex(mesh, PointType(0,0)); // id = 0
  VertexHandleType vh1 = viennagrid::make_vertex(mesh, PointType(1,0)); // id = 1
  VertexHandleType vh2 = viennagrid::make_vertex(mesh, PointType(2,0));
  VertexHandleType vh3 = viennagrid::make_vertex(mesh, PointType(2,1));
  VertexHandleType vh4 = viennagrid::make_vertex(mesh, PointType(1,1));
  VertexHandleType vh5 = viennagrid::make_vertex(mesh, PointType(0,1)); // id = 5

  //
  // Step 3: Fill the two segments with cells.
  //         To do so, each cell must be linked with the defining vertex handles from the mesh
  //

  // First triangle, use vertex handles
  viennagrid::static_array<VertexHandleType, 3> vertices;
  vertices[0] = vh0;
  vertices[1] = vh1;
  vertices[2] = vh5;
  // Note that vertices are rearranged internally if they are not supplied in mathematically positive order.

  viennagrid::make_element<CellType>(seg0, vertices.begin(), vertices.end()); // creates an element with vertex handles


  // Second triangle:
  viennagrid::make_triangle(seg0, vh1, vh4, vh5);  //use the shortcut function

  // Third triangle:
  viennagrid::make_triangle(seg1, vh1, vh2, vh4); // Note that we create in 'seg1' now.

  // Fourth triangle:
  viennagrid::make_triangle(seg1, vh2, vh3, vh4 );

  //
  // That's it. The mesh consisting of two segments is now set up.
  // If no segments are required, one can also directly write viennagrid::make_triangle(mesh, ...);
  //

  //
  // Step 4: Output the cells for each segment:
  //

  std::cout << "Cells in segment 0:" << std::endl;
  CellRange cells_seg0(seg0);
  for (CellIterator cit0 = cells_seg0.begin();
                    cit0 != cells_seg0.end();
                  ++cit0)
  {
    std::cout << *cit0 << std::endl;
  }
  std::cout << std::endl;

  std::cout << "Cells in segment 1:" << std::endl;
  CellRange cells_seg1(seg1);
  for (CellIterator cit1 = cells_seg1.begin();
                    cit1 != cells_seg1.end();
                  ++cit1)
  {
    std::cout << *cit1 << std::endl;
  }
  std::cout << std::endl;

  viennagrid::io::vtk_writer<MeshType> writer;
  writer( mesh, segmentation, "test" );



  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
