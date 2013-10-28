/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
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
#include "viennagrid/topology/plc.hpp"


#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/element/element_view.hpp"

#include "viennagrid/point.hpp"

#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/mesh_config.hpp"
#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/element_creation.hpp"

#include "viennagrid/io/tetgen_poly_reader.hpp"



int main()
{

  //
  // typedefing and instancing the mesh type
  //

  typedef viennagrid::plc_3d_mesh MeshType;
  MeshType mesh;


  //
  // typedefs for the element types
  //

  typedef viennagrid::result_of::point<MeshType>::type point_type;

  typedef viennagrid::result_of::element<MeshType, viennagrid::vertex_tag>::type   vertex_type;
  typedef viennagrid::result_of::element<MeshType, viennagrid::line_tag>::type       line_type;
  typedef viennagrid::result_of::element<MeshType, viennagrid::plc_tag>::type         plc_type;

  //
  // Reading the PLC mesh
  //

  viennagrid::io::tetgen_poly_reader reader;
  reader(mesh, "../data/test.poly");


  // querying the first PLC in the mesh
  plc_type & plc = viennagrid::elements<plc_type>(mesh)[0];


  // printing all vertices of the first PLC
  typedef viennagrid::result_of::element_range<plc_type, vertex_type>::type vertex_on_plc_range;
  typedef viennagrid::result_of::iterator<vertex_on_plc_range>::type vertex_on_plc_iterator;

  std::cout << "All vertices of the PLC" << std::endl;
  vertex_on_plc_range vertex_range = viennagrid::elements<vertex_type>( plc );
  for (vertex_on_plc_iterator it = vertex_range.begin(); it != vertex_range.end(); ++it)
      std::cout << *it << " " << viennagrid::point(mesh, *it) << std::endl;
  std::cout << std::endl;

  // printing all lines of the first PLC
  typedef viennagrid::result_of::element_range<plc_type, line_type>::type line_on_plc_range;
  typedef viennagrid::result_of::iterator<line_on_plc_range>::type line_on_plc_iterator;

  std::cout << "All lines of the PLC" << std::endl;
  line_on_plc_range line_range = viennagrid::elements<line_type>( plc );
  for (line_on_plc_iterator it = line_range.begin(); it != line_range.end(); ++it)
      std::cout << *it << std::endl;
  std::cout << std::endl;

  // printing all PLC of the mesh
  std::cout << "All PLCs of the mesh" << std::endl;
  typedef viennagrid::result_of::element_range< MeshType, plc_type >::type plc_range_type;
  plc_range_type plc_range = viennagrid::elements< plc_type >(mesh);
  for ( plc_range_type::iterator it = plc_range.begin(); it != plc_range.end(); ++it)
      std::cout << *it << std::endl;
  std::cout << std::endl;

  // printing all hole points of the first PLC
  std::cout << "All hole points of the plc" << std::endl;
  std::vector<point_type> & pts = plc.appendix();
  std::copy( pts.begin(), pts.end(), std::ostream_iterator<point_type>(std::cout, "\n") );

    return 0;
}
