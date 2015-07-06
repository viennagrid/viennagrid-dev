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

#include <iostream>
#include <typeinfo>


#include "viennagrid/core.hpp"
// #include "viennagrid/mesh/element_creation.hpp"
// #include "viennagrid/mesh/element_deletion.hpp"
// #include "viennagrid/mesh/coboundary_iteration.hpp"
// #include "viennagrid/io/vtk_writer.hpp"



int main()
{
  typedef viennagrid::mesh MeshType;

  typedef viennagrid::result_of::point<MeshType>::type point_type;
  typedef viennagrid::result_of::vertex<MeshType>::type VertexHandleType;

  MeshType mesh(2, viennagrid::triangle_tag());

  VertexHandleType v00 = viennagrid::make_vertex(mesh, 0.0, 0.0);
  VertexHandleType v10 = viennagrid::make_vertex(mesh, 1.0, 0.0);
  VertexHandleType v20 = viennagrid::make_vertex(mesh, 2.0, 0.0);
  VertexHandleType v01 = viennagrid::make_vertex(mesh, 0.0, 1.0);
  VertexHandleType v11 = viennagrid::make_vertex(mesh, 1.0, 1.0);
  VertexHandleType v21 = viennagrid::make_vertex(mesh, 2.0, 1.0);
  VertexHandleType v02 = viennagrid::make_vertex(mesh, 0.0, 2.0);
  VertexHandleType v12 = viennagrid::make_vertex(mesh, 1.0, 2.0);
  VertexHandleType v22 = viennagrid::make_vertex(mesh, 2.0, 2.0);

  viennagrid::make_triangle(mesh, v00, v01, v11);
  viennagrid::make_triangle(mesh, v00, v10, v11);
  viennagrid::make_triangle(mesh, v10, v11, v20);
  viennagrid::make_triangle(mesh, v11, v20, v21);
  viennagrid::make_triangle(mesh, v11, v21, v22);
  viennagrid::make_triangle(mesh, v12, v11, v22);
  viennagrid::make_triangle(mesh, v02, v11, v12);
  viennagrid::make_triangle(mesh, v01, v11, v02);



  std::cout << "--------------------------------------------------" << std::endl;
  std::cout << "----------- Erasing from Mesh           ----------" << std::endl;
  std::cout << "--------------------------------------------------" << std::endl;


  std::cout << viennagrid::triangles(mesh).size() << std::endl;


  typedef viennagrid::result_of::mesh_view<MeshType>::type mesh_view_type;
  mesh_view_type elements_to_erase = viennagrid::make_view(mesh);
  viennagrid::mark_erase_elements( mesh, elements_to_erase, v22 );
  viennagrid::mark_erase_elements( mesh, elements_to_erase, v21 );

  viennagrid::erase_elements(mesh, elements_to_erase);

  std::cout << viennagrid::triangles(mesh).size() << std::endl;

  {
    viennagrid::io::vtk_writer<viennagrid::triangular_2d_mesh> vtk_writer;
    vtk_writer(mesh, "triangle_mesh");
  }



  std::cout << "--------------------------------------------------" << std::endl;
  std::cout << "----------- Erasing from View           ----------" << std::endl;
  std::cout << "--------------------------------------------------" << std::endl;


  typedef viennagrid::result_of::mesh_view<MeshType>::type MeshViewType;
  MeshViewType view = viennagrid::make_view(mesh);

  VertexHandleType new_vh = viennagrid::vertices(mesh).handle_at(0);

  viennagrid::add_single_handle( view, new_vh );

  typedef viennagrid::result_of::cell_range<MeshType>::type CellRangeType;
  typedef viennagrid::result_of::iterator<CellRangeType>::type CellRangeIterator;

  CellRangeType cells(mesh);
  for (CellRangeIterator cit = cells.begin(); cit != cells.end(); ++cit)
      viennagrid::add_single_handle( view, cit.handle() );

  std::cout << viennagrid::cells(view).size() << std::endl;
  viennagrid::erase_element(view, new_vh);
  std::cout << viennagrid::cells(view).size() << std::endl;

  {
    viennagrid::io::vtk_writer<MeshViewType> vtk_writer;
    vtk_writer(view, "triangle_view");
  }

  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
