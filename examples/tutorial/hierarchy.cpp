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
#include "viennagrid/algorithm/refine.hpp"


int main()
{
  //
  // Define the necessary types:
  //

  typedef viennagrid::mesh_hierarchy_t MeshHierarchyType;
  typedef viennagrid::result_of::mesh<MeshHierarchyType>::type MeshType;
  typedef viennagrid::result_of::region<MeshType>::type RegionType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::element<MeshType>::type           ElementType;

  typedef viennagrid::result_of::element_range<MeshType>::type      ElementRange;
  typedef viennagrid::result_of::iterator<ElementRange>::type          ElementIterator;

  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << "-- ViennaGrid tutorial: Setup of a mesh with two segments -- " << std::endl;
  std::cout << "-------------------------------------------------------------- " << std::endl;
  std::cout << std::endl;

  //
  // Step 1: Instantiate the mesh and the segmentation and create 2 segments:
  //
  MeshHierarchyType hierarchy;
  MeshType mesh = hierarchy.root();
  MeshType refined_mesh = mesh.make_child();

  //
  // Step 2: Add vertices to the mesh.
  //         Note that vertices with IDs are enumerated in the order they are pushed to the mesh.
  //
  ElementType v0 = viennagrid::make_vertex(mesh, viennagrid::make_point(0, 0) ); // id = 0
  ElementType v1 = viennagrid::make_vertex(mesh, viennagrid::make_point(1, 0) ); // id = 1
  ElementType v2 = viennagrid::make_vertex(mesh, viennagrid::make_point(0, 1) );
  ElementType v3 = viennagrid::make_vertex(mesh, viennagrid::make_point(1, 1) );
//   ElementType v4 = viennagrid::make_vertex(mesh, 0.5, 0);
//   ElementType v5 = viennagrid::make_vertex(mesh, 0, 0.5); // id = 5

  //
  // Step 3: Fill the two segments with cells.
  //         To do so, each cell must be linked with the defining vertex handles from the mesh
  //

  // First triangle, use vertex handles
  /*ElementType tri0 =*/ viennagrid::make_triangle(mesh, v0, v1, v2); // creates an element with vertex handles
  /*ElementType tri1 =*/ viennagrid::make_triangle(mesh, v1, v2, v3);  //use the shortcut function






  ElementType line0 = viennagrid::make_line(mesh, v0, v1);
  ElementType line1 = viennagrid::make_line(mesh, v0, v2);



//   viennagrid::add(refined_mesh, tri1);


  typedef viennagrid::result_of::accessor_container< ElementType, bool, viennagrid::std_map_tag >::type LineRefinementContainerType;
  LineRefinementContainerType line_refinement_flag;

  viennagrid::result_of::field< LineRefinementContainerType, ElementType >::type line_refinement_field(line_refinement_flag);

  {
    typedef viennagrid::result_of::const_element_range<MeshType,2>::type ElementRangeType;
    typedef viennagrid::result_of::iterator<ElementRangeType>::type ElementRangeIterator;
    ElementRangeType lines(mesh);
    for (ElementRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
      line_refinement_field(*lit) = false;

    line_refinement_field(line0) = true;
    line_refinement_field(line1) = true;
  }



  viennagrid::refine(mesh, refined_mesh, 2, line_refinement_field);





//   std::vector<ElementType> refined_triangle_vertices(3);
//   refined_triangle_vertices[0] = v0;
//   refined_triangle_vertices[1] = v4;
//   refined_triangle_vertices[2] = v5;
//
//   std::vector< std::pair<ElementType, ElementType> > intersects;
//   intersects.push_back( std::make_pair(v4, line0) );
//   intersects.push_back( std::make_pair(v5, line1) );
//
//
//   ElementType tri3 = viennagrid::make_refined_element(refined_mesh, tri0, viennagrid::triangle_tag(),
//                                                       refined_triangle_vertices.begin(), refined_triangle_vertices.end(),
//                                                       intersects.begin(),
//                                                       intersects.end());
//
//
//   refined_triangle_vertices[0] = v4;
//   refined_triangle_vertices[1] = v1;
//   refined_triangle_vertices[2] = v5;
//
//   ElementType tri4 = viennagrid::make_refined_element(refined_mesh, tri0, viennagrid::triangle_tag(),
//                                                       refined_triangle_vertices.begin(), refined_triangle_vertices.end(),
//                                                       intersects.begin(),
//                                                       intersects.end());
//
//
//   refined_triangle_vertices[0] = v5;
//   refined_triangle_vertices[1] = v1;
//   refined_triangle_vertices[2] = v2;
//
//   ElementType tri5 = viennagrid::make_refined_element(refined_mesh, tri0, viennagrid::triangle_tag(),
//                                                       refined_triangle_vertices.begin(), refined_triangle_vertices.end(),
//                                                       intersects.begin(),
//                                                       intersects.end());

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
    std::cout << "  parent = " << viennagrid::parent(*cit0) << std::endl;
  }
  std::cout << std::endl;


  cells = ElementRange(refined_mesh, 2);
  std::cout << "Cells in refined mesh (count = " << cells.size() << "):" << std::endl;
  for (ElementIterator cit0 = cells.begin();
                      cit0 != cells.end();
                    ++cit0)
  {
    std::cout << *cit0 << std::endl;
    std::cout << "  parent: " << viennagrid::parent(*cit0) << std::endl;
  }
  std::cout << std::endl;





  ElementRange lines(mesh, 1);
  std::cout << "Lines in mesh (count = " << lines.size() << "):" << std::endl;
  for (ElementIterator cit0 = lines.begin();
                      cit0 != lines.end();
                    ++cit0)
  {
    std::cout << *cit0 << std::endl;
    std::cout << "  parent = " << viennagrid::parent(*cit0) << std::endl;
  }
  std::cout << std::endl;


  lines = ElementRange(refined_mesh, 1);
  std::cout << "Lines in refined mesh (count = " << lines.size() << "):" << std::endl;
  for (ElementIterator cit0 = lines.begin();
                      cit0 != lines.end();
                    ++cit0)
  {
    std::cout << *cit0 << std::endl;
    std::cout << "  parent: " << viennagrid::parent(*cit0) << std::endl;
  }
  std::cout << std::endl;




  viennagrid::io::vtk_writer<MeshType> writer;
  writer(mesh, "hierarchy_level0");
  writer(refined_mesh, "hierarchy_level1");


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
