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

#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/element_creation.hpp"


int main()
{
    //
    // typedefing and setting up the topological mesh
    //

    typedef viennagrid::polygonal_2d_mesh mesh_type;
    mesh_type mesh;

    //
    // typedefs for the element types
    //

    typedef viennagrid::result_of::point<mesh_type>::type point_type;

    typedef viennagrid::result_of::handle<mesh_type, viennagrid::vertex_tag>::type vertex_handle_type;

    typedef viennagrid::result_of::element<mesh_type, viennagrid::polygon_tag>::type polygon_type;


    viennagrid::storage::static_array<vertex_handle_type, 7> handles;

    handles[0] = viennagrid::make_vertex( mesh, point_type(0, 0) );
    handles[1] = viennagrid::make_vertex( mesh, point_type(1, 0) );
    handles[2] = viennagrid::make_vertex( mesh, point_type(2, 1) );
    handles[3] = viennagrid::make_vertex( mesh, point_type(2, 2) );
    handles[4] = viennagrid::make_vertex( mesh, point_type(1, 2) );
    handles[5] = viennagrid::make_vertex( mesh, point_type(0, 1) );
    handles[6] = viennagrid::make_vertex( mesh, point_type(0.5, 0.5) );


    viennagrid::make_element<polygon_type>( mesh, handles.begin(), handles.end() );

    std::cout << viennagrid::elements<viennagrid::polygon_tag>(mesh)[0] << std::endl;


    return 0;
}


