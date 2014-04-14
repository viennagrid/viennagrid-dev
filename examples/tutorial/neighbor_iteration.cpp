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

#include <map>

using std::cout;
using std::endl;


#include "viennagrid/forwards.hpp"

#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/id_generator.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"
#include "viennagrid/storage/range.hpp"


#include "viennagrid/topology/vertex.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"



#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/config/mesh_config.hpp"
#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/neighbor_iteration.hpp"
#include "viennagrid/mesh/element_creation.hpp"


int main()
{

  //
  // typedefing and setting up the mesh
  //

  typedef viennagrid::triangular_2d_mesh MeshType;
  MeshType mesh;

  //
  // typedefs for the element types
  //

  typedef viennagrid::result_of::vertex_handle<MeshType>::type        VertexHandleType;

  typedef viennagrid::result_of::triangle<MeshType>::type             TriangleType;
  typedef viennagrid::result_of::triangle_handle<MeshType>::type      TriangleHandleType;


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

  VertexHandleType vh0 = viennagrid::make_vertex( mesh );
  VertexHandleType vh1 = viennagrid::make_vertex( mesh );
  VertexHandleType vh2 = viennagrid::make_vertex( mesh );
  VertexHandleType vh3 = viennagrid::make_vertex( mesh );
  VertexHandleType vh4 = viennagrid::make_vertex( mesh );
  VertexHandleType vh5 = viennagrid::make_vertex( mesh );
  VertexHandleType vh6 = viennagrid::make_vertex( mesh );
  VertexHandleType vh7 = viennagrid::make_vertex( mesh );
  VertexHandleType vh8 = viennagrid::make_vertex( mesh );

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

  TriangleHandleType th0 = viennagrid::make_triangle( mesh, vh0, vh1, vh3 );
  TriangleHandleType th1 = viennagrid::make_triangle( mesh, vh1, vh4, vh3 );
  viennagrid::make_triangle( mesh, vh1, vh5, vh4 );
  viennagrid::make_triangle( mesh, vh1, vh2, vh5 );
  viennagrid::make_triangle( mesh, vh3, vh7, vh6 );
  viennagrid::make_triangle( mesh, vh3, vh4, vh7 );
  viennagrid::make_triangle( mesh, vh4, vh5, vh7 );
  viennagrid::make_triangle( mesh, vh5, vh8, vh7 );






  {
    typedef viennagrid::result_of::neighbor_range<MeshType, viennagrid::triangle_tag, viennagrid::vertex_tag>::type NeighborRangeType;

    // we now want to iterate over all neighbor triangles of triangle 1 which share a common vertex. These should be the triangles 0, 2, 3, 4, 5 and 6
    NeighborRangeType neighbor_range = NeighborRangeType(mesh, viennagrid::dereference_handle(mesh, th1));
    cout << "All triangles neighboring th1 using vertices (should be triangle 0, 2, 3, 4, 5 and 6)" << endl;
    std::copy( neighbor_range.begin(), neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;

    // we now want to iterate over all neighbor triangles of triangle 0 which share a common vertex. These should be the triangles 1, 2, 3, 4 and 5
    neighbor_range = NeighborRangeType(mesh, th0);
    cout << "All triangles neighboring th0 using vertices (should be triangle 1, 2, 3, 4 and 5)" << endl;
    std::copy( neighbor_range.begin(), neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;
  }


  {
    // now we do the same but with a constant mesh

    MeshType const & cmesh = mesh;
    typedef viennagrid::result_of::const_neighbor_range<MeshType, viennagrid::triangle_tag, viennagrid::vertex_tag>::type ConstNeighborRangeType;

    ConstNeighborRangeType const_neighbor_range(cmesh, viennagrid::dereference_handle(cmesh, th1)); //viennagrid::neighbor_elements<viennagrid::triangle_tag, viennagrid::vertex_tag>(cmesh, th1);
    cout << "All triangles neighboring th1 using vertices and const ranges (should be triangle 0, 2, 3, 4, 5 and 6)" << endl;
    std::copy( const_neighbor_range.begin(), const_neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;

    const_neighbor_range = ConstNeighborRangeType(cmesh, th0); //viennagrid::neighbor_elements<viennagrid::triangle_tag, viennagrid::vertex_tag>(cmesh, th0);
    cout << "All triangles neighboring th0 using vertices and const ranges  (should be triangle 1, 2, 3, 4 and 5)" << endl;
    std::copy( const_neighbor_range.begin(), const_neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;
  }


  {
    typedef viennagrid::result_of::neighbor_range<MeshType, viennagrid::triangle_tag, viennagrid::line_tag>::type NeighborRangeType;

    // we now want to iterate over all neighbor triangles of triangle 1 which share a common edge. These should be the triangles 0, 2 and 5
    NeighborRangeType neighbor_range = NeighborRangeType(mesh, th1);
    cout << "All triangles neighboring th1 using lines (should be triangle 0, 2 and 5)" << endl;
    std::copy( neighbor_range.begin(), neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;

    // we now want to iterate over all neighbor triangles of triangle 0 which share a common edge. This should only be the triangle 1
    neighbor_range = NeighborRangeType(mesh, th0);
    cout << "All triangles neighboring th0 using lines (should only be triangle 1)" << endl;
    std::copy( neighbor_range.begin(), neighbor_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;
  }


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}


