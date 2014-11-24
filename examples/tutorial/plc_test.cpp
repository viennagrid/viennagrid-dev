#include <iostream>
#include <typeinfo>

#include <map>
#include <iterator>

using std::cout;
using std::endl;


#include "viennagrid/core.hpp"

int main()
{

    //
    // First define the type of handle to use:
    //

    typedef viennagrid::mesh_t MeshType;
    MeshType mesh;

    //
    // typedefs for the element types
    //

    typedef viennagrid::result_of::point<MeshType>::type PointType;

    typedef viennagrid::result_of::element<MeshType, viennagrid::vertex_tag>::type VertexType;
    typedef viennagrid::result_of::element<MeshType, viennagrid::line_tag>::type LineType;


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

    lines.push_back( viennagrid::make_line( mesh, *(v.begin() + 9), *(v.begin() + 10) ) );

    typedef viennagrid::result_of::element<MeshType>::type PLCType;

    PLCType plc = viennagrid::make_element<viennagrid::plc_tag>( mesh, lines.begin(), lines.end() );
    viennagrid::add_hole_point( plc, viennagrid::make_point(10.5, 10.5) );
    viennagrid::add_hole_point( plc, viennagrid::make_point(0.5, 0.5) );

    typedef viennagrid::result_of::vertex_range<PLCType>::type VertexOnPLCRange;
    typedef viennagrid::result_of::iterator<VertexOnPLCRange>::type VertexOnPLCIterator;


    std::cout << "All vertices of the PLC" << std::endl;
    VertexOnPLCRange vertex_range( plc );
    for (VertexOnPLCIterator it = vertex_range.begin(); it != vertex_range.end(); ++it)
        std::cout << *it << " " << viennagrid::get_point(mesh, *it) << std::endl;
    std::cout << std::endl;

    typedef viennagrid::result_of::element_range<PLCType, 1>::type LineOnPLCRange;
    typedef viennagrid::result_of::iterator<LineOnPLCRange>::type LineOnPLCIterator;

    std::cout << "All lines of the PLC" << std::endl;
    LineOnPLCRange line_range( plc );
    for (LineOnPLCIterator it = line_range.begin(); it != line_range.end(); ++it)
        std::cout << *it << std::endl;
    std::cout << std::endl;

    std::cout << "All PLCs of the mesh" << std::endl;
    typedef viennagrid::result_of::element_range< MeshType, 2 >::type PLCRangeType;
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
