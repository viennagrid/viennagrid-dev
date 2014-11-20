/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
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

#include "viennagrid/core.hpp"
#include "viennagrid/io/vtk_writer.hpp"

//
//    Let us construct the following input mesh:
//
//    5---------4---------3
//    | \       |         |
//    |   \     |         |    y
//    |     \   |         |     ^
//    |       \ |         |     |
//    0---------1---------2     *--> x
//
//    Segment 1 | Segment 2
//


int main()
{
  //
  // Define the necessary types:
  //

  typedef viennagrid::mesh_t MeshType;
  typedef viennagrid::result_of::region<MeshType>::type RegionType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::vertex<MeshType>::type           VertexType;

  typedef viennagrid::result_of::element_range<MeshType>::type      ElementRange;
  typedef viennagrid::result_of::iterator<ElementRange>::type          ElementIterator;

  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << "-- ViennaGrid tutorial: Setup of a mesh with two segments -- " << std::endl;
  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << std::endl;

  //
  // Step 1: Instantiate the mesh and the segmentation and create 2 segments:
  //
  MeshType mesh;

//   RegionType region0 = mesh.make_region();
//   RegionType region1 = mesh.make_region();

  //
  // Step 2: Add vertices to the mesh.
  //         Note that vertices with IDs are enumerated in the order they are pushed to the mesh.
  //
  VertexType v0 = viennagrid::make_vertex(mesh, 0, 0); // id = 0
  VertexType v1 = viennagrid::make_vertex(mesh, 1, 0); // id = 1
  VertexType v2 = viennagrid::make_vertex(mesh, 2, 0);
  VertexType v3 = viennagrid::make_vertex(mesh, 2, 1);
  VertexType v4 = viennagrid::make_vertex(mesh, 1, 1);
  VertexType v5 = viennagrid::make_vertex(mesh, 0, 1); // id = 5

  //
  // Step 3: Fill the two segments with cells.
  //         To do so, each cell must be linked with the defining vertex handles from the mesh
  //

  // First triangle, use vertex handles
  viennagrid::make_triangle(mesh, v0, v1, v5); // creates an element with vertex handles
  viennagrid::make_triangle(mesh, v1, v4, v5);  //use the shortcut function

  // Third triangle:
//   viennagrid::make_triangle(region1, v1, v2, v4); // Note that we create in 'seg1' now.
  // Fourth triangle:
  viennagrid::make_quadrilateral(mesh, v1, v2, v4, v3 );

  //
  // That's it. The mesh consisting of two segments is now set up.
  // If no segments are required, one can also directly write viennagrid::make_triangle(mesh, ...);
  //

  //
  // Step 4: Output the cells for each segment:
  //

  ElementRange cells(mesh, 2);
  std::cout << "Cells in mesh (count = " << cells.size() << "):" << std::endl;
  for (ElementIterator cit0 = cells.begin();
                      cit0 != cells.end();
                    ++cit0)
  {
    std::cout << *cit0 << std::endl;
  }
  std::cout << std::endl;


  viennagrid::io::vtk_writer<MeshType> writer;
  writer(mesh, "hybrid");


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
