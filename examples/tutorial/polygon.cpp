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


#include "viennagrid/core.hpp"

// #include "viennagrid/storage/view.hpp"
// #include "viennagrid/storage/container_collection.hpp"
// #include "viennagrid/storage/inserter.hpp"
// #include "viennagrid/storage/id_generator.hpp"
// #include "viennagrid/storage/hidden_key_map.hpp"
// #include "viennagrid/storage/range.hpp"


// #include "viennagrid/topology/vertex.hpp"
// #include "viennagrid/topology/line.hpp"
// #include "viennagrid/topology/simplex.hpp"

// #include "viennagrid/element/element_key.hpp"
// #include "viennagrid/element/element_orientation.hpp"
// #include "viennagrid/point.hpp"

// #include "viennagrid/config/default_configs.hpp"

// #include "viennagrid/mesh/mesh.hpp"
// #include "viennagrid/mesh/element_creation.hpp"


int main()
{
  //
  // typedefing and setting up the topological mesh
  //

  typedef viennagrid::mesh_t MeshType;
  MeshType mesh(2, viennagrid::polygon_tag());

  //
  // typedefs for the element types
  //

  typedef viennagrid::result_of::point<MeshType>::type PointType;
  typedef viennagrid::result_of::vertex<MeshType>::type VertexType;
  typedef viennagrid::result_of::polygon<MeshType>::type PolygonType;


//   viennagrid::static_array<vertex_handle_type, 7> handles;
  std::vector<VertexType> vertices(7);

  vertices[0] = viennagrid::make_vertex( mesh, 0, 0 );
  vertices[1] = viennagrid::make_vertex( mesh, 1, 0 );
  vertices[2] = viennagrid::make_vertex( mesh, 2, 1 );
  vertices[3] = viennagrid::make_vertex( mesh, 2, 2 );
  vertices[4] = viennagrid::make_vertex( mesh, 1, 2 );
  vertices[5] = viennagrid::make_vertex( mesh, 0, 1 );
  vertices[6] = viennagrid::make_vertex( mesh, 0.5, 0.5 );


  viennagrid::make_element<viennagrid::polygon_tag>( mesh, vertices.begin(), vertices.end() );

  std::cout << viennagrid::elements<viennagrid::polygon_tag>(mesh)[0] << std::endl;

  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}


