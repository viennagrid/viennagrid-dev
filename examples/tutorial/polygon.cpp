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

using std::cout;
using std::endl;

#include "viennagrid/viennagrid.hpp"

int main()
{
  //
  // typedefing and setting up the topological mesh
  //

  typedef viennagrid::mesh                                 MeshType;
  MeshType mesh;

  //
  // typedefs for the element types
  //

  typedef viennagrid::result_of::element<MeshType>::type   VertexType;


//   viennagrid::static_array<vertex_handle_type, 7> handles;
  std::vector<VertexType> vertices(7);

  vertices[0] = viennagrid::make_vertex( mesh, viennagrid::make_point(0, 0) );
  vertices[1] = viennagrid::make_vertex( mesh, viennagrid::make_point(1, 0) );
  vertices[2] = viennagrid::make_vertex( mesh, viennagrid::make_point(2, 1) );
  vertices[3] = viennagrid::make_vertex( mesh, viennagrid::make_point(2, 2) );
  vertices[4] = viennagrid::make_vertex( mesh, viennagrid::make_point(1, 2) );
  vertices[5] = viennagrid::make_vertex( mesh, viennagrid::make_point(0, 1) );
  vertices[6] = viennagrid::make_vertex( mesh, viennagrid::make_point(0.5, 0.5) );


  viennagrid::make_element<viennagrid::polygon_tag>( mesh, vertices.begin(), vertices.end() );

  std::cout << viennagrid::elements<2>(mesh)[0] << std::endl;

  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}


