/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <iostream>
#include <typeinfo>

#include <map>
#include <iterator>

using std::cout;
using std::endl;

#ifdef _MSC_VER
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif


#include "viennagrid/core.hpp"
#include "viennagrid/io/tetgen_poly_reader.hpp"



int main()
{

  //
  // typedefing and instancing the mesh type
  //

  typedef viennagrid::mesh_t MeshType;
  MeshType mesh(3, viennagrid::plc_tag());


  //
  // typedefs for the element types
  //

  typedef viennagrid::result_of::point<MeshType>::type point_type;

  typedef viennagrid::result_of::vertex<MeshType>::type   VertexType;
  typedef viennagrid::result_of::line<MeshType>::type       LineType;
  typedef viennagrid::result_of::plc<MeshType>::type         PLCType;

  //
  // Reading the PLC mesh
  //

  viennagrid::io::tetgen_poly_reader reader;
  reader(mesh, "../data/cube_with_tunnel.poly");


  // querying the first PLC in the mesh
  PLCType plc = viennagrid::plcs(mesh)[0];


  // printing all vertices of the first PLC
  typedef viennagrid::result_of::vertex_range<PLCType>::type vertex_on_plc_range;
  typedef viennagrid::result_of::iterator<vertex_on_plc_range>::type vertex_on_plc_iterator;

  std::cout << "All vertices of the PLC" << std::endl;
  vertex_on_plc_range vertex_range( plc );
  for (vertex_on_plc_iterator it = vertex_range.begin(); it != vertex_range.end(); ++it)
      std::cout << *it << " " << viennagrid::get_point(mesh, *it) << std::endl;
  std::cout << std::endl;

  // printing all lines of the first PLC
  typedef viennagrid::result_of::line_range<PLCType>::type line_on_plc_range;
  typedef viennagrid::result_of::iterator<line_on_plc_range>::type line_on_plc_iterator;

  std::cout << "All lines of the PLC" << std::endl;
  line_on_plc_range line_range( plc );
  for (line_on_plc_iterator it = line_range.begin(); it != line_range.end(); ++it)
      std::cout << *it << std::endl;
  std::cout << std::endl;

  // printing all PLC of the mesh
  std::cout << "All PLCs of the mesh" << std::endl;
  typedef viennagrid::result_of::plc_range< MeshType >::type PLCRangeTYpe;
  PLCRangeTYpe plc_range(mesh);
  for ( PLCRangeTYpe::iterator it = plc_range.begin(); it != plc_range.end(); ++it)
      std::cout << *it << std::endl;
  std::cout << std::endl;

  // printing all hole points of the first PLC
  std::cout << "All hole points of the plc" << std::endl;
  std::vector<point_type> pts = viennagrid::hole_points(plc);
  std::copy( pts.begin(), pts.end(), std::ostream_iterator<point_type>(std::cout, "\n") );

  return 0;
}
