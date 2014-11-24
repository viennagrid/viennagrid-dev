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



int main()
{
  typedef viennagrid::mesh_t                              MeshType;
  typedef viennagrid::result_of::point<MeshType>::type                PointType;

  typedef viennagrid::result_of::element< MeshType >::type     VertexType;
  typedef viennagrid::result_of::element< MeshType >::type     TriangleType;


  MeshType mesh;

  // create some vertices
  VertexType v0 = viennagrid::make_vertex( mesh, 0, 0, 0 );
  VertexType v1 = viennagrid::make_vertex( mesh, 1, 0, 0 );
  VertexType v2 = viennagrid::make_vertex( mesh, 0, 1, 0 );
  VertexType v3 = viennagrid::make_vertex( mesh, 0, 0, 1 );
  VertexType v4 = viennagrid::make_vertex( mesh, 1, 0, 1 );
  VertexType v5 = viennagrid::make_vertex( mesh, 0, 1, 1 );

  // create some triangles
  std::vector<TriangleType> t(4);

  t[0] = viennagrid::make_triangle( mesh, v0, v1, v2 );
  t[1] = viennagrid::make_triangle( mesh, v1, v2, v3 );
  t[2] = viennagrid::make_triangle( mesh, v3, v4, v5 );
  t[3] = viennagrid::make_triangle( mesh, v0, v4, v5 );


  typedef viennagrid::result_of::vertex_range<MeshType>::type VertexRangeType;
  typedef viennagrid::result_of::iterator<VertexRangeType>::type VertexIteratorType;

  typedef viennagrid::result_of::cell_range<MeshType>::type CellRangeType;
  typedef viennagrid::result_of::iterator<CellRangeType>::type CellIteratorType;


  {
    MeshType another_mesh;

    // copy all element handles to another_mesh
    viennagrid::copy_elements( t.begin(), t.end(), another_mesh );

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

      CellIteratorType begin = viennagrid::advance( cells.begin(), 1 );
      CellIteratorType end = viennagrid::advance( cells.begin(), 3 );

      viennagrid::copy_elements( begin, end, another_mesh );
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
