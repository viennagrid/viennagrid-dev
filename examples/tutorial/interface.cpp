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
  typedef viennagrid::mesh_t                        MeshType;
  typedef viennagrid::result_of::region<MeshType>::type RegionType;

  typedef viennagrid::result_of::vertex< MeshType >::type     VertexType;
  typedef viennagrid::result_of::triangle< MeshType >::type          TriangleType;

  MeshType mesh;


  VertexType v0 = viennagrid::make_vertex( mesh );
  VertexType v1 = viennagrid::make_vertex( mesh );
  VertexType v2 = viennagrid::make_vertex( mesh );
  VertexType v3 = viennagrid::make_vertex( mesh );
  VertexType v4 = viennagrid::make_vertex( mesh );

  TriangleType t0 = viennagrid::make_triangle( mesh, v0, v1, v2 );
  TriangleType t1 = viennagrid::make_triangle( mesh, v0, v2, v3 );
  TriangleType t2 = viennagrid::make_triangle( mesh, v0, v3, v4 );
  TriangleType t3 = viennagrid::make_triangle( mesh, v0, v4, v1 );


  RegionType region0 = mesh.make_region();
  RegionType region1 = mesh.make_region();

  viennagrid::add( region0, t0 );
  viennagrid::add( region0, t1 );
  viennagrid::add( region1, t2 );
  viennagrid::add( region1, t3 );

  typedef viennagrid::result_of::line_range< MeshType >::type line_range;
  line_range lines( mesh );
  for (line_range::iterator it = lines.begin(); it != lines.end(); ++it)
  {
    std::cout << *it << " INTERFACE? " << viennagrid::is_interface( region0, region1, *it ) << std::endl;
  }

  typedef viennagrid::result_of::vertex_range< MeshType >::type vertex_range;
  vertex_range vertices( mesh );
  for (vertex_range::iterator it = vertices.begin(); it != vertices.end(); ++it)
  {
    std::cout << *it << " INTERFACE? " << viennagrid::is_interface( region0, region1, *it ) << std::endl;
  }


  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << " \\o/    Tutorial finished successfully!    \\o/ " << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  return EXIT_SUCCESS;
}
