#include <iostream>
#include <typeinfo>

#include <map>

using std::cout;
using std::endl;


#include "viennagrid/forwards.h"

#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/id_generator.hpp"


#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/triangle.hpp"
#include "viennagrid/topology/tetrahedron.hpp"


#include "viennagrid/detail/boundary_ncell_layer.hpp"
#include "viennagrid/point.hpp"


    


int main()
{
    //typedef viennagrid::storage::pointer_hook_tag hook_tag;
    //typedef viennagrid::storage::iterator_hook_tag hook_tag;
    typedef viennagrid::storage::id_hook_tag<int> hook_tag;
    
    
    
    
    
    typedef viennagrid::point_t< double, viennagrid::cartesian_cs<3> > point_type;
    

    typedef viennagrid::viennagrid_ng::vertex_t< point_type, int > vertex_type;
    
    
    
    typedef viennagrid::storage::result_of::hooked_container<
            vertex_type,
            viennagrid::storage::hooked_container_tag<
                viennagrid::storage::std_deque_tag,
                hook_tag
            >
        >::type vertex_container_type;
    

    typedef viennameta::static_pair<
        viennagrid::storage::result_of::view<
            vertex_container_type,
            viennagrid::storage::hooked_container_tag<
                //viennagrid::storage::std_deque_tag,
                viennagrid::storage::static_array_tag<2>,
                viennagrid::storage::no_hook_tag
            >
        >::type,
        viennameta::null_type
    > line_vertex_containers;
    
    typedef viennameta::make_typelist< line_vertex_containers >::type line_containers_typelist;
    typedef viennagrid::viennagrid_ng::element_t< viennagrid::line_tag, line_containers_typelist, int > line_type;
    
    typedef viennagrid::storage::result_of::hooked_container<
            line_type,
            viennagrid::storage::hooked_container_tag<
                viennagrid::storage::std_deque_tag,
                //viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag<int> >,
                hook_tag
            >
        >::type line_container_type;
    
    vertex_type v0;
    vertex_type v1;
    vertex_type v2;
    vertex_type v3;
    
    v0[0] = 0;
    v0[1] = 0;
    v0[2] = 0;
    //v0.id(0);

    v1[0] = 1;
    v1[1] = 0;
    v1[2] = 0;
    //v1.id(1);
    
    v2[0] = 0;
    v2[1] = 1;
    v2[2] = 0;
    //v2.id(2);
    
    v3[0] = 0;
    v3[1] = 0;
    v3[2] = 1;
    //v3.id(3);

    
//     line_type line;
//     line.container( viennagrid::dimension_tag<0>() ).set_hook(&v0, 0);
//     line.container( viennagrid::dimension_tag<0>() ).set_hook(&v1, 1);
    
    
//     typedef viennagrid::viennagrid_ng::result_of::container_of_dimension<line_type, 0>::type vertices_of_line_range_type;
//     vertices_of_line_range_type range = viennagrid::viennagrid_ng::ncells<0>(line);   
//     std::copy( range.begin(), range.end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    
    
    
    typedef viennameta::static_pair<
        viennagrid::storage::result_of::view<
            vertex_container_type,
            viennagrid::storage::hooked_container_tag<
                viennagrid::storage::static_array_tag<3>,
                viennagrid::storage::no_hook_tag
            >
        >::type,
        viennameta::null_type
    > triangle_vertex_containers;
    
    typedef viennameta::static_pair<
        viennagrid::storage::result_of::view<
            line_container_type,
            viennagrid::storage::hooked_container_tag<
                //viennagrid::storage::std_deque_tag,
                viennagrid::storage::static_array_tag<3>,
                viennagrid::storage::no_hook_tag
            >
        >::type,
        viennameta::null_type
    > triangle_line_containers;
    
    typedef viennameta::make_typelist< triangle_vertex_containers, triangle_line_containers >::type triangle_containers_typelist;
    typedef viennagrid::viennagrid_ng::element_t< viennagrid::triangle_tag, triangle_containers_typelist, int > triangle_type;
    
    typedef viennagrid::storage::result_of::hooked_container<
            triangle_type,
            viennagrid::storage::hooked_container_tag<
                viennagrid::storage::std_deque_tag,
                //viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag<int> >,
                hook_tag
            >
        >::type triangle_container_type;
    
    
    
    
    
    typedef viennameta::static_pair<
        viennagrid::storage::result_of::view<
            vertex_container_type,
            viennagrid::storage::hooked_container_tag<
                viennagrid::storage::static_array_tag<4>,
                viennagrid::storage::no_hook_tag
            >
        >::type,
        viennameta::null_type
    > tetrahedron_vertex_containers;
    
    typedef viennameta::static_pair<
        viennagrid::storage::result_of::view<
            line_container_type,
            viennagrid::storage::hooked_container_tag<
                viennagrid::storage::static_array_tag<6>,
                viennagrid::storage::no_hook_tag
            >
        >::type,
        viennameta::null_type
    > tetrahedron_line_containers;
    
    typedef viennameta::static_pair<
        viennagrid::storage::result_of::view<
            triangle_container_type,
            viennagrid::storage::hooked_container_tag<
                viennagrid::storage::static_array_tag<4>,
                viennagrid::storage::no_hook_tag
            >
        >::type,
        viennameta::null_type
    > tetrahedron_triangle_containers;
    
    typedef viennameta::make_typelist< tetrahedron_vertex_containers, tetrahedron_line_containers, tetrahedron_triangle_containers >::type tetrahedron_containers_typelist;
    typedef viennagrid::viennagrid_ng::element_t< viennagrid::tetrahedron_tag, tetrahedron_containers_typelist, int > tetrahedron_type;
    
    typedef viennagrid::storage::result_of::hooked_container<
            tetrahedron_type,
            viennagrid::storage::hooked_container_tag<
                viennagrid::storage::std_deque_tag,
                hook_tag
            >
        >::type tetrahedron_container_type;
        
        
        
        
        
    typedef viennagrid::storage::result_of::collection< 
        viennameta::make_typelist< vertex_container_type, line_container_type, triangle_container_type, tetrahedron_container_type >::type
    >::type domain_type;
    domain_type domain;
    
    typedef viennagrid::storage::result_of::continuous_id_generator< viennagrid::storage::container_collection::result_of::value_typelist<domain_type>::type >::type id_generator_type;
    typedef viennagrid::storage::result_of::physical_inserter<domain_type, id_generator_type&>::type inserter_type;

    id_generator_type id_generator;
    inserter_type inserter(domain, id_generator);
    
    
    inserter(v0);
    inserter(v1);
    inserter(v2);
    inserter(v3);
    
    

    
    tetrahedron_type tet;
    tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::container_collection::get<vertex_type>(domain).hook_at(0), 0);
    tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::container_collection::get<vertex_type>(domain).hook_at(1), 1);
    tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::container_collection::get<vertex_type>(domain).hook_at(2), 2);
    tet.container( viennagrid::dimension_tag<0>() ).set_hook( viennagrid::storage::container_collection::get<vertex_type>(domain).hook_at(3), 3);
    
    inserter(tet);
    

    
    std::copy( viennagrid::storage::container_collection::get<vertex_type>(domain).begin(), viennagrid::storage::container_collection::get<vertex_type>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::container_collection::get<line_type>(domain).begin(), viennagrid::storage::container_collection::get<line_type>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::container_collection::get<triangle_type>(domain).begin(), viennagrid::storage::container_collection::get<triangle_type>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    std::copy( viennagrid::storage::container_collection::get<tetrahedron_type>(domain).begin(), viennagrid::storage::container_collection::get<tetrahedron_type>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    

    

    return 0;
}


