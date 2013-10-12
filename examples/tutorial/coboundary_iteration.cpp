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


#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/mesh_config.hpp"

#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/coboundary_iteration.hpp"
#include "viennagrid/mesh/element_creation.hpp"


#include "viennagrid/algorithm/boundary.hpp"

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

    typedef viennagrid::result_of::vertex_handle<MeshType>::type    VertexHandleType;
    typedef viennagrid::result_of::triangle<MeshType>::type         TriangleType;



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

    viennagrid::make_triangle( mesh, vh0, vh1, vh3 );
    viennagrid::make_triangle( mesh, vh1, vh4, vh3 );
    viennagrid::make_triangle( mesh, vh1, vh5, vh4 );
    viennagrid::make_triangle( mesh, vh1, vh2, vh5 );
    viennagrid::make_triangle( mesh, vh3, vh7, vh6 );
    viennagrid::make_triangle( mesh, vh3, vh4, vh7 );
    viennagrid::make_triangle( mesh, vh4, vh5, vh7 );
    viennagrid::make_triangle( mesh, vh5, vh8, vh7 );


    // we now want to iterate over all co-boundary triangles of vertex 4, which should be triangle 1, 2, 5 and 6
    typedef viennagrid::result_of::coboundary_range<MeshType, viennagrid::vertex_tag, viennagrid::triangle_tag>::type coboundary_range_type;
    coboundary_range_type coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(mesh, vh4);
    cout << "All triangles connected to vh4 (should be triangle 1, 2, 5, and 6)" << endl;
    std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;

    // we now want to iterate over all co-boundary triangles of vertex 4, which should only be triangle 3
    coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(mesh, vh2);
    cout << "All triangles connected to vh2 (should be triangle 3)" << endl;
    std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;



    // now we do the same but with a constant mesh
    const MeshType & cmesh = mesh;
    typedef viennagrid::result_of::const_coboundary_range<MeshType, viennagrid::vertex_tag, viennagrid::triangle_tag>::type const_coboundary_range_type;

    const_coboundary_range_type const_coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(cmesh, vh4);
    cout << "All triangles connected to vh4 (should be triangle 1, 2, 5, and 6)" << endl;
    std::copy( const_coboundary_range.begin(), const_coboundary_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;

    const_coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(cmesh, vh2);
    cout << "All triangles connected to vh2 (should be triangle 3)" << endl;
    std::copy( const_coboundary_range.begin(), const_coboundary_range.end(), std::ostream_iterator<TriangleType>(cout, "\n") );
    cout << endl;

    return 0;
}


