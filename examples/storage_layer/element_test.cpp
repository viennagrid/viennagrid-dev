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
#include "viennagrid/storage/id_generator.hpp"
#include "viennagrid/storage/algorithm.hpp"



#include "viennagrid/detail/element_key.hpp"






template<typename ElementTypeList, typename ContainerConfig = viennagrid::storage::container_collection::default_container_config>
struct domain_t
{
//public:
    typedef ElementTypeList element_typelist;
    typedef ContainerConfig container_config;
    
//protected:    
    
    typedef typename viennagrid::storage::result_of::container_collection< element_typelist, container_config >::type container_collection_type;
    typedef typename viennagrid::storage::result_of::continuous_id_generator< element_typelist >::type id_generator_type;
    typedef typename viennagrid::storage::result_of::physical_inserter<container_collection_type, id_generator_type&>::type inserter_type;
    
//public:
    
    domain_t() : id_generator(), container_collection(), inserter(container_collection, id_generator) {}
    
    
    template<typename element_type>
    void push_back( const element_type & element)
    {
        inserter( element );
    }
    
    
//protected:
    id_generator_type id_generator;
    container_collection_type container_collection;
    inserter_type inserter;
};




template<typename DomainType,
    typename ElementTypeList = typename DomainType::element_typelist,
    typename ContainerConfig = viennagrid::storage::container_collection::default_container_config>
struct segment_t
{
    typedef ElementTypeList element_typelist;
    typedef ContainerConfig container_config;
    
    // TODO: element_typelist beachten!
    
    
    typedef typename viennagrid::storage::result_of::view_collection< typename DomainType::container_collection_type, container_config>::type container_collection_type;
    
//     typedef typename viennagrid::storage::view::reference_typelist_from_container_typelist_using_reference_config<
//         typename viennagrid::storage::container_collection::result_of::container_typelist<typename DomainType::container_collection_type>::type,
//         reference_config>::type element_reference_typelist;
//     
//     typedef typename viennagrid::storage::result_of::container_collection<
//         element_reference_typelist,
//         container_config>::type container_collection_type;
    typedef typename viennagrid::storage::result_of::recursive_inserter<container_collection_type, typename DomainType::inserter_type>::type inserter_type;
    
    segment_t(DomainType & domain, bool copy = true) : container_collection(), inserter(container_collection, domain.inserter)
    {
        if (copy)
            viennagrid::storage::container_collection::handle(domain.container_collection, container_collection);        
    }
    
    template<typename element_type>
    void push_back( const element_type & element)
    {
        inserter( element );
    }
    
//protected:
    container_collection_type container_collection;
    inserter_type inserter;
};



namespace viennagrid
{
    
    template<typename element_type, typename domain_type>
    typename viennagrid::storage::result_of::container_of<typename domain_type::container_collection_type, element_type>::type & elements( domain_type & domain )
    {
        return viennagrid::storage::container_collection::get<element_type>(domain.container_collection);
    }

    template<typename element_type, typename domain_type>
    const typename viennagrid::storage::result_of::container_of<typename domain_type::container_collection_type, element_type>::type & elements( const domain_type & domain )
    {
        return viennagrid::storage::container_collection::get<element_type>(domain.container_collection);
    }

}



int main()
{
    typedef viennagrid::config::tetrahedral_3d element_config;
    typedef viennagrid::result_of::point< element_config >::type point_type;
    typedef viennagrid::result_of::ncell< element_config, 3 >::type tetrahedron_type;
    typedef viennagrid::result_of::ncell< element_config, 2 >::type triangle_type;
    typedef viennagrid::result_of::ncell< element_config, 1 >::type line_type;
    typedef viennagrid::result_of::ncell< element_config, 0 >::type vertex_type;
    
    
    
    
    typedef viennameta::make_typemap<
        vertex_type,            viennagrid::storage::handleed_container_tag< viennagrid::storage::std_deque_tag, viennagrid::storage::pointer_handle_tag>,
        tetrahedron_type,       viennagrid::storage::handleed_container_tag< viennagrid::storage::std_deque_tag, viennagrid::storage::pointer_handle_tag>,
        triangle_type,          viennagrid::storage::handleed_container_tag< viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag<int> >, viennagrid::storage::pointer_handle_tag>,
        line_type,              viennagrid::storage::handleed_container_tag< viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag<int> >, viennagrid::storage::pointer_handle_tag>
    >::type container_config;
    
    
    
    
    typedef domain_t<tetrahedron_type::required_types, container_config> domain_type;
    domain_type domain;

    
    
    
    vertex_type vertex;
    
    vertex.point() = point_type(0.0f, 0.0f, 0.0f);
    domain.push_back( vertex );

    vertex.point() = point_type(10.0f, 0.0f, 0.0f);
    domain.push_back( vertex );

    vertex.point() = point_type(0.0f, 10.0f, 0.0f);
    domain.push_back( vertex );
    
    vertex.point() = point_type(0.0f, 10.0f, 10.0f);
    domain.push_back( vertex );

    vertex.point() = point_type(0.0f, 10.0f, -10.0f);
    domain.push_back( vertex );
    
    vertex.point() = point_type(-10.0f, 0.0f, 0.0f);
    domain.push_back( vertex );
    
    
    tetrahedron_type tetrahedron;
    vertex_type * vertices[4];
    
    
    vertices[0] = &viennagrid::elements<vertex_type>(domain)[0];
    vertices[1] = &viennagrid::elements<vertex_type>(domain)[1];
    vertices[2] = &viennagrid::elements<vertex_type>(domain)[2];
    vertices[3] = &viennagrid::elements<vertex_type>(domain)[3];
    
    tetrahedron.vertices(vertices);    
    domain.push_back( tetrahedron );

    
    vertices[0] = &viennagrid::elements<vertex_type>(domain)[0];
    vertices[1] = &viennagrid::elements<vertex_type>(domain)[1];
    vertices[2] = &viennagrid::elements<vertex_type>(domain)[2];
    vertices[3] = &viennagrid::elements<vertex_type>(domain)[4];
    
    tetrahedron.vertices(vertices);
    domain.push_back( tetrahedron );

    
    
    cout << "In Domain" << endl;
    cout << "Vertices:" << endl;
    std::copy( viennagrid::elements<vertex_type>(domain).begin(), viennagrid::elements<vertex_type>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << "\n\n";

    cout << "Lines:" << endl;
    std::copy( viennagrid::elements<line_type>(domain).begin(), viennagrid::elements<line_type>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << "\n\n";
    
    cout << "Triangles:" << endl;
    std::copy( viennagrid::elements<triangle_type>(domain).begin(), viennagrid::elements<triangle_type>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << "\n\n";
    
    cout << "Tetrahedrons:" << endl;
    std::copy( viennagrid::elements<tetrahedron_type>(domain).begin(), viennagrid::elements<tetrahedron_type>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << "\n\n";
    
    
    
    
    
    
    typedef viennameta::make_typemap<
        vertex_type,            viennagrid::storage::handleed_container_tag< viennagrid::storage::std_deque_tag, viennagrid::storage::no_handle_tag>,
        tetrahedron_type,       viennagrid::storage::handleed_container_tag< viennagrid::storage::std_deque_tag, viennagrid::storage::no_handle_tag>,
        triangle_type,          viennagrid::storage::handleed_container_tag< viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag<int> >, viennagrid::storage::no_handle_tag>,
        line_type,              viennagrid::storage::handleed_container_tag< viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag<int> >, viennagrid::storage::no_handle_tag>
    >::type view_container_config;
    
//     typedef VIENNAMETA_MAKE_TYPEMAP_4(  vertex_type *, viennagrid::storage::view_tag< viennagrid::storage::std_deque_tag >,
//                                         tetrahedron_type *,  viennagrid::storage::view_tag< viennagrid::storage::std_deque_tag >,
//                                         
//                                         //triangle_type, viennagrid::storage::std_deque_tag,
//                                         triangle_type *, viennagrid::storage::view_tag< viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag<int> > >,
//                                         
//                                         //line_type, viennagrid::storage::std_deque_tag
//                                         line_type *, viennagrid::storage::view_tag< viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag<int> > >
//     ) view_container_config;

    
    cout << "New Segment" << endl;
    typedef segment_t<domain_type, tetrahedron_type::required_types, view_container_config> segment_type;
    segment_type seg0(domain, true);
    segment_type seg1(domain, true);
    
    
    
    vertices[0] = &viennagrid::elements<vertex_type>(domain)[2];
    vertices[1] = &viennagrid::elements<vertex_type>(domain)[3];
    vertices[2] = &viennagrid::elements<vertex_type>(domain)[4];
    vertices[3] = &viennagrid::elements<vertex_type>(domain)[5];
    
    tetrahedron.vertices(vertices);
    seg0.push_back(tetrahedron);

    cout << "In Domain" << endl;
    cout << "Vertices:" << endl;
    std::copy( viennagrid::elements<vertex_type>(domain).begin(), viennagrid::elements<vertex_type>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << "\n\n";

    cout << "Lines:" << endl;
    std::copy( viennagrid::elements<line_type>(domain).begin(), viennagrid::elements<line_type>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << "\n\n";
    
    cout << "Triangles:" << endl;
    std::copy( viennagrid::elements<triangle_type>(domain).begin(), viennagrid::elements<triangle_type>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << "\n\n";
    
    cout << "Tetrahedrons:" << endl;
    std::copy( viennagrid::elements<tetrahedron_type>(domain).begin(), viennagrid::elements<tetrahedron_type>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << "\n\n";
    
    
    cout << "In seg0" << endl;
    cout << "Vertices:" << endl;
    std::copy( viennagrid::elements<vertex_type>(seg0).begin(), viennagrid::elements<vertex_type>(seg0).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << "\n\n";

    cout << "Lines:" << endl;
    std::copy( viennagrid::elements<line_type>(seg0).begin(), viennagrid::elements<line_type>(seg0).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << "\n\n";
    
    cout << "Triangles:" << endl;
    std::copy( viennagrid::elements<triangle_type>(seg0).begin(), viennagrid::elements<triangle_type>(seg0).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << "\n\n";
    
    cout << "Tetrahedrons:" << endl;
    std::copy( viennagrid::elements<tetrahedron_type>(seg0).begin(), viennagrid::elements<tetrahedron_type>(seg0).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << "\n\n";
    
    
    cout << "In seg1" << endl;
    cout << "Vertices:" << endl;
    std::copy( viennagrid::elements<vertex_type>(seg1).begin(), viennagrid::elements<vertex_type>(seg1).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << "\n\n";

    cout << "Lines:" << endl;
    std::copy( viennagrid::elements<line_type>(seg1).begin(), viennagrid::elements<line_type>(seg1).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << "\n\n";
    
    cout << "Triangles:" << endl;
    std::copy( viennagrid::elements<triangle_type>(seg1).begin(), viennagrid::elements<triangle_type>(seg1).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << "\n\n";
    
    cout << "Tetrahedrons:" << endl;
    std::copy( viennagrid::elements<tetrahedron_type>(seg1).begin(), viennagrid::elements<tetrahedron_type>(seg1).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << "\n\n";
    
    return 0;
}
