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
#include <iterator>

#include <map>

using std::cout;
using std::endl;


#include "viennagridpp/core.hpp"

int main()
{

  //
  // typedefing and setting up the mesh
  //

  typedef viennagrid::mesh MeshType;
  MeshType mesh;


  //
  // typedefs for the element types
  //

  typedef viennagrid::result_of::element<MeshType>::type    VertexType;
  typedef viennagrid::result_of::element<MeshType>::type    TriangleType;



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

  viennagrid::make_triangle( mesh, v0, v1, v3 );
  viennagrid::make_triangle( mesh, v1, v4, v3 );
  viennagrid::make_triangle( mesh, v1, v5, v4 );
  viennagrid::make_triangle( mesh, v1, v2, v5 );
  viennagrid::make_triangle( mesh, v3, v7, v6 );
  viennagrid::make_triangle( mesh, v3, v4, v7 );
  viennagrid::make_triangle( mesh, v4, v5, v7 );
  viennagrid::make_triangle( mesh, v5, v8, v7 );


  // we now want to iterate over all co-boundary triangles of vertex 4, which should be triangle 1, 2, 5 and 6
  typedef viennagrid::result_of::coboundary_range<MeshType, 2>::type CoboundaryRangeType;
  CoboundaryRangeType coboundary_range(mesh, v4);
  cout << "All triangles connected to vh4 (should be triangle 1, 2, 5, and 6)" << endl;
    std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
  cout << endl;

  // we now want to iterate over all co-boundary triangles of vertex 4, which should only be triangle 3
  coboundary_range = CoboundaryRangeType(mesh, v2);
  cout << "All triangles connected to vh2 (should be triangle 3)" << endl;
  std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
  cout << endl;



  // now we do the same but with a constant mesh
  MeshType const & cmesh = mesh;
  typedef viennagrid::result_of::const_coboundary_range<MeshType, 2>::type ConstCoboundaryRangeType;

  ConstCoboundaryRangeType const_coboundary_range(cmesh, v4);
  cout << "All triangles connected to vh4 (should be triangle 1, 2, 5, and 6)" << endl;
  std::copy( const_coboundary_range.begin(), const_coboundary_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
  cout << endl;

  const_coboundary_range = ConstCoboundaryRangeType(cmesh, v2);
  cout << "All triangles connected to vh2 (should be triangle 3)" << endl;
  std::copy( const_coboundary_range.begin(), const_coboundary_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
  cout << endl;


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}


