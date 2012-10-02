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

#include "viennagrid/storage/collection.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"







template<typename container_collection_type, typename inserter_type>
struct domain_type
{
    
    domain_type() : container_collection(), inserter(container_collection) {}
    
    
    template<typename element_type>
    void push_back( const element_type & element)
    {
        inserter( element );
    }
    
    
    container_collection_type container_collection;
    inserter_type inserter;
};
    






int main()
{
    //typedef viennagrid::config::tetrahedral_3d element_config;
    typedef viennagrid::config::triangular_3d element_config;
    typedef viennagrid::result_of::point< element_config >::type point_type;
    //typedef viennagrid::result_of::ncell< element_config, 3 >::type tetrahedron_type;
    typedef viennagrid::result_of::ncell< element_config, 2 >::type triangle_type;
    typedef viennagrid::result_of::ncell< element_config, 1 >::type line_type;
    typedef viennagrid::result_of::ncell< element_config, 0 >::type vertex_type;

    
    typedef viennagrid::storage::result_of::container_collection<
        //tetrahedron_type::required_types,
        triangle_type::required_types,
        viennagrid::storage::container_collection::default_container_config
    >::type domain_container_collection_type;
    
    typedef viennagrid::storage::result_of::recursive_inserter< domain_container_collection_type >::type domain_inserter_type;
    
    domain_type<domain_container_collection_type, domain_inserter_type> domain;
    
    
    //domain_type domain;

    
    vertex_type vertex;
    
    vertex.point() = point_type(0.0f, 0.0f, 0.0f);
    domain.push_back( vertex );

    vertex.point() = point_type(10.0f, 0.0f, 0.0f);
    domain.push_back( vertex );

    vertex.point() = point_type(0.0f, 10.0f, 0.0f);
    domain.push_back( vertex );
    
    vertex.point() = point_type(0.0f, 10.0f, 10.0f);
    domain.push_back( vertex );

    
    
    
    vertex_type * vertices[4];
    vertices[0] = &viennagrid::storage::container_collection::get<vertex_type>( domain.container_collection )[0];
    vertices[1] = &viennagrid::storage::container_collection::get<vertex_type>( domain.container_collection )[1];
    vertices[2] = &viennagrid::storage::container_collection::get<vertex_type>( domain.container_collection )[2];
    //vertices[3] = &viennagrid::storage::container_collection::get<vertex_type>( domain.container_collection )[3];
    
    tetrahedron_type tetrahedron;
    tetrahedron.vertices(vertices);
    
    domain.push_back( tetrahedron );
    
    cout << "Vertices:" << endl;
    std::copy( viennagrid::storage::container_collection::get<vertex_type>(domain.container_collection).begin(), viennagrid::storage::container_collection::get<vertex_type>(domain.container_collection).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << "\n\n";

    cout << "Lines:" << endl;
    std::copy( viennagrid::storage::container_collection::get<line_type>(domain.container_collection).begin(), viennagrid::storage::container_collection::get<line_type>(domain.container_collection).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << "\n\n";
    
    cout << "Triangles:" << endl;
    std::copy( viennagrid::storage::container_collection::get<triangle_type>(domain.container_collection).begin(), viennagrid::storage::container_collection::get<triangle_type>(domain.container_collection).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << "\n\n";
    
//     cout << "Tetrahedrons:" << endl;
//     std::copy( viennagrid::storage::container_collection::get<tetrahedron_type>(domain.container_collection).begin(), viennagrid::storage::container_collection::get<triangle_type>(domain.container_collection).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
//     cout << "\n\n";
    
    return 0;
}
