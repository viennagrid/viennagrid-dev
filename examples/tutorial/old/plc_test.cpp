#include <iostream>
#include <typeinfo>

#include <map>
#include <iterator>

using std::cout;
using std::endl;


#include "viennagrid/core.hpp"

// #include "viennagrid/storage/view.hpp"
// #include "viennagrid/storage/container_collection.hpp"
// #include "viennagrid/storage/inserter.hpp"
// #include "viennagrid/storage/id_generator.hpp"
// #include "viennagrid/storage/hidden_key_map.hpp"
// #include "viennagrid/storage/range.hpp"
//
//
// #include "viennagrid/topology/vertex.hpp"
// #include "viennagrid/topology/line.hpp"
// #include "viennagrid/topology/simplex.hpp"
// #include "viennagrid/topology/plc.hpp"
//
// #include "viennagrid/config/element_config.hpp"
// #include "viennagrid/config/mesh_config.hpp"
// #include "viennagrid/config/default_configs.hpp"
//
// #include "viennagrid/element/element_key.hpp"
// #include "viennagrid/element/element_orientation.hpp"
// #include "viennagrid/element/element_view.hpp"
//
// #include "viennagrid/point.hpp"
//
// #include "viennagrid/mesh/mesh.hpp"
// #include "viennagrid/mesh/element_creation.hpp"







// struct WrappedConfigType
// {
//   typedef viennagrid::storage::pointer_handle_tag handle_tag;
//   typedef viennagrid::config::result_of::full_mesh_config<viennagrid::plc_tag, viennagrid::config::point_type_2d, viennagrid::storage::pointer_handle_tag>::type type;
// };


int main()
{

    //
    // First define the type of handle to use:
    //

//     typedef viennagrid::storage::pointer_handle_tag handle_tag;
    //typedef viennagrid::storage::iterator_handle_tag handle_tag;
//     typedef viennagrid::storage::id_handle_tag handle_tag;

    //
    // typedefing and setting up the topological mesh
    //

//     typedef viennagrid::config::result_of::full_mesh_config<viennagrid::plc_tag, viennagrid::config::point_type_2d, viennagrid::storage::pointer_handle_tag>::type mesh_config;

    //typedef viennagrid::config::polygonal_2d_mesh MeshType;
    typedef viennagrid::mesh_t MeshType;
    MeshType mesh(2, viennagrid::plc_tag());

    //
    // typedefs for the element types
    //

    typedef viennagrid::result_of::point<MeshType>::type PointType;

    typedef viennagrid::result_of::element<MeshType, viennagrid::vertex_tag>::type VertexType;
//     typedef viennagrid::result_of::handle<MeshType, viennagrid::vertex_tag>::type vertex_handle_type;

    typedef viennagrid::result_of::element<MeshType, viennagrid::line_tag>::type LineType;
//     typedef viennagrid::result_of::handle<MeshType, viennagrid::line_tag>::type line_handle_type;

//     typedef viennagrid::result_of::element<MeshType, viennagrid::polygon_tag>::type polygon_type;
//     typedef viennagrid::result_of::handle<MeshType, viennagrid::polygon_tag>::type polygon_handle_type;


    std::vector<VertexType> v;

    v.push_back( viennagrid::make_vertex( mesh, 0, 0 ) );
    v.push_back( viennagrid::make_vertex( mesh, 10, 0 ) );
    v.push_back( viennagrid::make_vertex( mesh, 20, 10 ) );
    v.push_back( viennagrid::make_vertex( mesh, 20, 20 ) );
    v.push_back( viennagrid::make_vertex( mesh, 10, 20 ) );
    v.push_back( viennagrid::make_vertex( mesh, 0, 10 ) );
    v.push_back( viennagrid::make_vertex( mesh, 5, 5 ) );

    v.push_back( viennagrid::make_vertex( mesh, 10, 10 ) );
    v.push_back( viennagrid::make_vertex( mesh, 12, 10 ) );
    v.push_back( viennagrid::make_vertex( mesh, 10, 12 ) );

    v.push_back( viennagrid::make_vertex( mesh, 8, 10 ) );

//     v.push_back( viennagrid::make_vertex( mesh, 15, 15 ) );


    std::vector<LineType> lines;

    {
        std::vector<VertexType>::iterator start = v.begin();
        std::vector<VertexType>::iterator end = v.begin() + 7;

        std::vector<VertexType>::iterator it1 = start;
        std::vector<VertexType>::iterator it2 = it1; ++it2;
        for (; it2 != end; ++it1, ++it2)
            lines.push_back( viennagrid::make_line(mesh, *it1, *it2) );
        lines.push_back( viennagrid::make_line(mesh, *it1, *start) );
    }


    {
        std::vector<VertexType>::iterator start = v.begin() + 7;
        std::vector<VertexType>::iterator end = v.begin() + 10;

        std::vector<VertexType>::iterator it1 = start;
        std::vector<VertexType>::iterator it2 = it1; ++it2;
        for (; it2 != end; ++it1, ++it2)
            lines.push_back( viennagrid::make_line(mesh, *it1, *it2) );
        lines.push_back( viennagrid::make_line(mesh, *it1, *start) );
    }


//     polygon_handle_type bounding_poly = viennagrid::make_element<polygon_type>( mesh, v.begin(), v.begin() + 7 );

//     polygon_handle_type hole_poly = viennagrid::make_element<polygon_type>( mesh, v.begin() + 7, v.begin() + 10 );

//     std::vector<line_handle_type> lines( viennagrid::elements<line_type>( viennagrid::dereference_handle(mesh, hole_poly) ).size() );
//     std::copy( viennagrid::elements<line_type>( viennagrid::dereference_handle(mesh, hole_poly)  ).handle_begin(), viennagrid::elements<line_type>( viennagrid::dereference_handle(mesh, hole_poly)  ).handle_end(), lines.begin() );
    lines.push_back( viennagrid::make_line( mesh, *(v.begin() + 9), *(v.begin() + 10) ) );

//     VertexType point = v[11];

//     std::cout << bounding_poly << std::endl;
//     std::cout << viennagrid::dereference_handle(mesh, bounding_poly) << std::endl;
//     std::cout << hole_poly << std::endl;
//     std::cout << viennagrid::dereference_handle(mesh, hole_poly) << std::endl;


    typedef viennagrid::result_of::plc<MeshType>::type PLCType;
//     typedef viennagrid::result_of::handle<MeshType, viennagrid::plc_tag>::type plc_handle_type;

    std::vector<PointType> hole_points;
    hole_points.push_back( viennagrid::make_point(10.5, 10.5) );

    PLCType plc = viennagrid::make_element<viennagrid::plc_tag>(  mesh,
//                                                                     &bounding_poly, &bounding_poly + 1,
//                                                                     bounding_poly,
//                                                                     &hole_poly, &hole_poly+1,
                                                                    lines.begin(), lines.end()
//                                                                     &point, &point + 1,
//                                                                     hole_points.begin(), hole_points.end()
                                                                 );

    viennagrid::add_hole_point( plc, viennagrid::make_point(10.5, 10.5) );
    viennagrid::add_hole_point( plc, viennagrid::make_point(0.5, 0.5) );


//     plc_type & plc = viennagrid::dereference_hxandle(mesh, plc_handle);

    //std::cout << plc.get_bounding_polygon() << std::endl;

    typedef viennagrid::result_of::vertex_range<PLCType>::type VertexOnPLCRange;
    typedef viennagrid::result_of::iterator<VertexOnPLCRange>::type VertexOnPLCIterator;


//     viennagrid::tagging::result_of::element_tag<plc_type, viennagrid::loose_tag>::type loose_tag = viennagrid::tagging::make_element_tag<viennagrid::loose_tag>(plc);

    std::cout << "All vertices of the PLC" << std::endl;
    VertexOnPLCRange vertex_range( plc );
    for (VertexOnPLCIterator it = vertex_range.begin(); it != vertex_range.end(); ++it)
        std::cout << *it << " " << viennagrid::get_point(mesh, *it) << std::endl;
    std::cout << std::endl;

    typedef viennagrid::result_of::line_range<PLCType>::type LineOnPLCRange;
    typedef viennagrid::result_of::iterator<LineOnPLCRange>::type LineOnPLCIterator;

    std::cout << "All lines of the PLC" << std::endl;
    LineOnPLCRange line_range( plc );
    for (LineOnPLCIterator it = line_range.begin(); it != line_range.end(); ++it)
        std::cout << *it << std::endl;
    std::cout << std::endl;

    std::cout << "All PLCs of the mesh" << std::endl;
    typedef viennagrid::result_of::plc_range< MeshType >::type PLCRangeType;
    PLCRangeType plc_range(mesh);
    for ( PLCRangeType::iterator it = plc_range.begin(); it != plc_range.end(); ++it)
        std::cout << *it << std::endl;
    std::cout << std::endl;


//     typedef viennagrid::result_of::const_element_view<plc_type, vertex_type>::type vertex_view_type;
//     typedef viennagrid::result_of::iterator<vertex_view_type>::type vertex_view_iterator;
//
//     std::cout << "All loose points of the plc" << std::endl;
//     vertex_view_type vertex_view = viennagrid::loose_elements<vertex_type>(plc);
//     for (vertex_view_iterator it = vertex_view.begin(); it != vertex_view.end(); ++it)
//         std::cout << *it << std::endl;
//     std::cout << std::endl;



//     typedef viennagrid::result_of::element_view<plc_type, line_type>::type line_view_type;
//
//     std::cout << "All inner lines of the plc" << std::endl;
//     viennagrid::tagging::result_of::element_tag<plc_type, viennagrid::bounding_tag>::type bounding_tag = viennagrid::tagging::make_element_tag<viennagrid::bounding_tag>(plc);
//     line_view_type inner_lines = viennagrid::untagged_elements<line_type>(plc, bounding_tag);
//     std::copy( inner_lines.begin(), inner_lines.end(), std::ostream_iterator<line_type>(std::cout, "\n") );
//     std::cout << std::endl;


    std::cout << "All hole points of the plc" << std::endl;
    std::vector<PointType> pts = viennagrid::hole_points(plc);// viennagrid::hole_points<MeshType>(plc);
    std::copy( pts.begin(), pts.end(), std::ostream_iterator<PointType>(std::cout, "\n") );


    return 0;
}
