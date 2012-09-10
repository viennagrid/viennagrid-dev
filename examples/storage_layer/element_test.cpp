#include <algorithm>
#include <iostream>
#include <iterator>

#include <vector>
#include <deque>

#include <typeinfo>

using std::cout;
using std::endl;



#include "viennagrid/point.hpp"
#include "viennagrid/element.hpp"
#include "viennagrid/detail/element_iterators.hpp"
#include "viennagrid/config/simplex.hpp"

#include "viennagrid/collection.hpp"





template<typename typelist>
struct add_deque
{};

template<typename head, typename tail>
struct add_deque< Loki::Typelist<head, tail> >
{
    typedef Loki::Typelist< std::deque<head>, typename add_deque<tail>::type > type;
};

template<>
struct add_deque<Loki::NullType>
{
    typedef Loki::NullType type;
};




int main()
{
    typedef viennagrid::config::triangular_3d element_config;
    typedef viennagrid::result_of::point< element_config >::type point_type;
    typedef viennagrid::result_of::ncell< element_config, 2 >::type triangle_type;
    typedef viennagrid::result_of::ncell< element_config, 1 >::type line_type;
    typedef viennagrid::result_of::ncell< element_config, 0 >::type vertex_type;
    
    //typedef LOKI_TYPELIST_3(std::deque<vertex_type>, std::deque<line_type>, std::deque<triangle_type>) collection_config;
    typedef add_deque< triangle_type::required_elements >::type collection_config;
    
    viennagrid::collection::result_of<collection_config>::type domain;

    
    vertex_type vertex;
    
    vertex.point() = point_type(0.0f, 0.0f);
    domain.push_back( vertex );

    vertex.point() = point_type(10.0f, 0.0f);
    domain.push_back( vertex );

    vertex.point() = point_type(0.0f, 10.0f);
    domain.push_back( vertex );

    
    
    
    vertex_type * vertices[3];
    vertices[0] = &viennagrid::collection::get<vertex_type>( domain )[0];
    vertices[1] = &viennagrid::collection::get<vertex_type>( domain )[1];
    vertices[2] = &viennagrid::collection::get<vertex_type>( domain )[2];
    
    triangle_type triangle;
    triangle.vertices(vertices);
    
    domain.push_back( triangle );
    
    cout << "Vertices:" << endl;
    std::copy( viennagrid::collection::get<vertex_type>(domain).begin(), viennagrid::collection::get<vertex_type>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << "\n\n";

    cout << "Lines:" << endl;
    std::copy( viennagrid::collection::get<line_type>(domain).begin(), viennagrid::collection::get<line_type>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << "\n\n";
    
    cout << "Triangles:" << endl;
    std::copy( viennagrid::collection::get<triangle_type>(domain).begin(), viennagrid::collection::get<triangle_type>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << "\n\n";
    

    
    return 0;
}
