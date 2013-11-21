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

#include <iostream>
#include <typeinfo>

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/accessor.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/mesh/element_creation.hpp"



int main()
{
  typedef viennagrid::triangular_3d_mesh                              MeshType;
  typedef viennagrid::result_of::point<MeshType>::type                PointType;

  typedef viennagrid::result_of::vertex_handle< MeshType >::type     VertexHandleType;
  typedef viennagrid::result_of::triangle_handle< MeshType >::type   TriangleHandleType;


  MeshType mesh;

  // create some vertices
  VertexHandleType vh0 = viennagrid::make_vertex( mesh, PointType(0, 0, 0) );
  VertexHandleType vh1 = viennagrid::make_vertex( mesh, PointType(1, 0, 0) );
  VertexHandleType vh2 = viennagrid::make_vertex( mesh, PointType(0, 1, 0) );
  VertexHandleType vh3 = viennagrid::make_vertex( mesh, PointType(0, 0, 1) );
  VertexHandleType vh4 = viennagrid::make_vertex( mesh, PointType(1, 0, 1) );
  VertexHandleType vh5 = viennagrid::make_vertex( mesh, PointType(0, 1, 1) );

  // create some triangles
  viennagrid::static_array<TriangleHandleType, 4> th;
  th[0] = viennagrid::make_triangle( mesh, vh0, vh1, vh2 );
  th[1] = viennagrid::make_triangle( mesh, vh1, vh2, vh3 );
  th[2] = viennagrid::make_triangle( mesh, vh3, vh4, vh5 );
  th[3] = viennagrid::make_triangle( mesh, vh0, vh4, vh5 );


  typedef viennagrid::result_of::vertex_range<MeshType>::type VertexRangeType;
  typedef viennagrid::result_of::iterator<VertexRangeType>::type VertexIteratorType;

  typedef viennagrid::result_of::cell_range<MeshType>::type CellRangeType;
  typedef viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;


  {
    MeshType another_mesh;

    // copy all element handles to another_mesh
    viennagrid::copy_element_handles( mesh, th.begin(), th.end(), another_mesh, -1.0 );

    // output all vertices
    std::cout << "Number of vertices in another_mesh: " << viennagrid::vertices(another_mesh).size() << std::endl;
    VertexRangeType vertices(another_mesh);
    for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      std::cout << *vit << std::endl;
    std::cout << std::endl;

    // output all cells
    std::cout << "Number of cells in another_mesh: " << viennagrid::cells(another_mesh).size() << std::endl;
    CellRangeType cells(another_mesh);
    for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      std::cout << *cit << std::endl;
    std::cout << std::endl;
  }


  {
    MeshType another_mesh;

    // copy some element handles to another_mesh
    viennagrid::copy_element_handles( mesh, th.begin()+2, th.begin()+4, another_mesh, -1.0 );

    // output all vertices
    std::cout << "Number of vertices in another_mesh: " << viennagrid::vertices(another_mesh).size() << std::endl;
    VertexRangeType vertices(another_mesh);
    for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      std::cout << *vit << std::endl;
    std::cout << std::endl;

    // output all cells
    std::cout << "Number of cells in another_mesh: " << viennagrid::cells(another_mesh).size() << std::endl;
    CellRangeType cells(another_mesh);
    for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      std::cout << *cit << std::endl;
    std::cout << std::endl;
  }



  {
    MeshType another_mesh;

    // copy all element to another_mesh using a cell range
    {
      CellRangeType cells(mesh);
      viennagrid::copy_elements( cells.begin()+1, cells.begin()+3, another_mesh, -1.0 );
    }

    // output all vertices
    std::cout << "Number of vertices in another_mesh: " << viennagrid::vertices(another_mesh).size() << std::endl;
    VertexRangeType vertices(another_mesh);
    for (VertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      std::cout << *vit << std::endl;
    std::cout << std::endl;

    // output all cells
    std::cout << "Number of cells in another_mesh: " << viennagrid::cells(another_mesh).size() << std::endl;
    CellRangeType cells(another_mesh);
    for (CellIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
      std::cout << *cit << std::endl;
    std::cout << std::endl;
  }



  return EXIT_SUCCESS;
}
