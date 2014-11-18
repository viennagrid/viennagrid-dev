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

#include <map>
#include <iterator>

using std::cout;
using std::endl;

#include "viennagrid/core.hpp"


int main()
{

  //
  // typedefing and setting up the mesh
  //

  typedef viennagrid::mesh_t MeshType;
  MeshType mesh(2, viennagrid::triangle_tag());

  //
  // typedefs for the element types
  //

  typedef viennagrid::result_of::vertex<MeshType>::type        VertexType;
  typedef viennagrid::result_of::triangle<MeshType>::type             TriangleType;


  //
  // Adding a tetrahedron
  //

  // Creating nine vertices within the mesh, vh is short vor vertex handle
  // In this example we don't need geometric information -> no points
  //
  //  0 --- 1 --- 2
  //  |   / | \   |
  //  |  /  |  \  |
  //  | /   |   \ |
  //  3 --- 4 --- 5
  //  | \   |   / |
  //  |  \  |  /  |
  //  |   | | /   |
  //  6 --- 7 --- 8
  //

  VertexType v0 = viennagrid::make_vertex( mesh );
  VertexType v1 = viennagrid::make_vertex( mesh );
  VertexType v2 = viennagrid::make_vertex( mesh );
  VertexType v3 = viennagrid::make_vertex( mesh );
  VertexType v4 = viennagrid::make_vertex( mesh );
  VertexType v5 = viennagrid::make_vertex( mesh );
  VertexType v6 = viennagrid::make_vertex( mesh );
  VertexType v7 = viennagrid::make_vertex( mesh );
  VertexType v8 = viennagrid::make_vertex( mesh );

  // Creating the triangles within the mesh, all boundary cell generation is done here implicit
  //
  //  X --- X --- X
  //  | 0 / | \ 3 |
  //  |  /  |  \  |
  //  | / 1 | 2 \ |
  //  X --- X --- X
  //  | \ 5 | 6 / |
  //  |  \  |  /  |
  //  | 4 | | / 7 |
  //  X --- X --- X
  //

  TriangleType t0 = viennagrid::make_triangle( mesh, v0, v1, v3 );
  TriangleType t1 = viennagrid::make_triangle( mesh, v1, v4, v3 );
  viennagrid::make_triangle( mesh, v1, v5, v4 );
  viennagrid::make_triangle( mesh, v1, v2, v5 );
  viennagrid::make_triangle( mesh, v3, v7, v6 );
  viennagrid::make_triangle( mesh, v3, v4, v7 );
  viennagrid::make_triangle( mesh, v4, v5, v7 );
  viennagrid::make_triangle( mesh, v5, v8, v7 );






  {
    typedef viennagrid::result_of::neighbor_range<MeshType, viennagrid::vertex_tag, viennagrid::triangle_tag>::type NeighborRangeType;

    // we now want to iterate over all neighbor triangles of triangle 1 which share a common vertex. These should be the triangles 0, 2, 3, 4, 5 and 6
    NeighborRangeType neighbor_range(mesh, t1);
    cout << "All triangles neighboring t1 using vertices (should be triangle 0, 2, 3, 4, 5 and 6)" << endl;
    std::copy( neighbor_range.begin(), neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;

    // we now want to iterate over all neighbor triangles of triangle 0 which share a common vertex. These should be the triangles 1, 2, 3, 4 and 5
    neighbor_range = NeighborRangeType(mesh, t0);
    cout << "All triangles neighboring t0 using vertices (should be triangle 1, 2, 3, 4 and 5)" << endl;
    std::copy( neighbor_range.begin(), neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;
  }


  {
    // now we do the same but with a constant mesh

    MeshType const & cmesh = mesh;
    typedef viennagrid::result_of::const_neighbor_range<MeshType, viennagrid::vertex_tag, viennagrid::triangle_tag>::type ConstNeighborRangeType;

    ConstNeighborRangeType const_neighbor_range(cmesh, t1); //viennagrid::neighbor_elements<viennagrid::triangle_tag, viennagrid::vertex_tag>(cmesh, th1);
    cout << "All triangles neighboring t1 using vertices and const ranges (should be triangle 0, 2, 3, 4, 5 and 6)" << endl;
    std::copy( const_neighbor_range.begin(), const_neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;

    const_neighbor_range = ConstNeighborRangeType(cmesh, t0); //viennagrid::neighbor_elements<viennagrid::triangle_tag, viennagrid::vertex_tag>(cmesh, th0);
    cout << "All triangles neighboring t0 using vertices and const ranges  (should be triangle 1, 2, 3, 4 and 5)" << endl;
    std::copy( const_neighbor_range.begin(), const_neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;
  }


  {
    typedef viennagrid::result_of::neighbor_range<MeshType, viennagrid::line_tag, viennagrid::triangle_tag>::type NeighborRangeType;

    // we now want to iterate over all neighbor triangles of triangle 1 which share a common edge. These should be the triangles 0, 2 and 5
    NeighborRangeType neighbor_range = NeighborRangeType(mesh, t1);
    cout << "All triangles neighboring t1 using lines (should be triangle 0, 2 and 5)" << endl;
    std::copy( neighbor_range.begin(), neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;

    // we now want to iterate over all neighbor triangles of triangle 0 which share a common edge. This should only be the triangle 1
    neighbor_range = NeighborRangeType(mesh, t0);
    cout << "All triangles neighboring t0 using lines (should only be triangle 1)" << endl;
    std::copy( neighbor_range.begin(), neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;
  }


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}


