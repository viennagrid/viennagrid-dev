#include <iostream>
#include <typeinfo>

#include <map>
#include <boost/concept_check.hpp>

using std::cout;
using std::endl;


#include "viennagrid/forwards.hpp"

#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/id_generator.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"
#include "viennagrid/storage/range.hpp"


#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"


#include "viennagrid/element/element_config.hpp"
#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/config/generic_config.hpp"

#include "viennagrid/domain/topologic_domain.hpp"
#include "viennagrid/domain/metainfo.hpp"
#include "viennagrid/domain/element_creation.hpp"




int main()
{
    
    //
    // First define the type of hook to use:
    //
    
    //typedef viennagrid::storage::pointer_hook_tag hook_tag;
    //typedef viennagrid::storage::iterator_hook_tag hook_tag;
    typedef viennagrid::storage::id_hook_tag hook_tag;
    
    
    
    
    
    typedef viennagrid::result_of::default_topologic_config<viennagrid::tetrahedron_tag, hook_tag>::type toplological_config;

    
    
    
    //
    // typedefing and setting up the topological domain
    //
    
    typedef viennagrid::result_of::topologic_domain<toplological_config>::type domain_type;
    domain_type domain = viennagrid::create_topologic_domain<domain_type>();
    
    //
    // typedefs for the element types
    //    
     
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::vertex_tag>::type vertex_hook_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::element<domain_type, viennagrid::triangle_tag>::type triangle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::tetrahedron_tag>::type tetrahedron_hook_type;
    

    //
    // Adding a tetrahedron
    //

    // creates four vertices within the domain, vh is short vor vertex hook
    vertex_hook_type vh0 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh1 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh2 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh3 = viennagrid::create_element<vertex_type>( domain );
    
    // creates a hook buffer for the vertex hooks of the tetdrahedron
    //std::vector<vertex_hook_type> hooks(4);
    viennagrid::storage::static_array<vertex_hook_type, 4> hooks;
    hooks[0] = vh0; hooks[1] = vh1; hooks[2] = vh2; hooks[3] = vh3;
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    viennagrid::create_element<tetrahedron_type>( domain, hooks );

    
    //
    // display the domain content
    //
    
    cout << "  USING ncells<dim>()" << endl;
    
//     cout << "All vertices of the domain" << endl;
//     std::copy( viennagrid::ncells<0>(domain).begin(), viennagrid::ncells<0>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
//     cout << endl;
//     
//     cout << "All lines of the domain" << endl;
//     std::copy( viennagrid::ncells<1>(domain).begin(), viennagrid::ncells<1>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
//     cout << endl;
//     
//     cout << "All triangles of the domain" << endl;
//     std::copy( viennagrid::ncells<2>(domain).begin(), viennagrid::ncells<2>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
//     cout << endl;
//     
//     cout << "All tetraherons of the domain" << endl;
//     std::copy( viennagrid::ncells<3>(domain).begin(), viennagrid::ncells<3>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
//     cout << endl;
    
    
    cout << "  USING elements<tag>()" << endl;
    
    cout << "All vertices of the domain" << endl;
    std::copy( viennagrid::elements<viennagrid::vertex_tag>(domain).begin(), viennagrid::elements<viennagrid::vertex_tag>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << endl;
    
    cout << "All lines of the domain" << endl;
    std::copy( viennagrid::elements<viennagrid::line_tag>(domain).begin(), viennagrid::elements<viennagrid::line_tag>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    cout << "All triangles of the domain" << endl;
    std::copy( viennagrid::elements<viennagrid::triangle_tag>(domain).begin(), viennagrid::elements<viennagrid::triangle_tag>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    cout << "All tetraherons of the domain" << endl;
    std::copy( viennagrid::elements<viennagrid::tetrahedron_tag>(domain).begin(), viennagrid::elements<viennagrid::tetrahedron_tag>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    
    cout << "  USING elements<type>()" << endl;
    
    cout << "All vertices of the domain" << endl;
    std::copy( viennagrid::elements<vertex_type>(domain).begin(), viennagrid::elements<vertex_type>(domain).end(), std::ostream_iterator<vertex_type>(cout, "\n") );
    cout << endl;
    
    cout << "All lines of the domain" << endl;
    std::copy( viennagrid::elements<line_type>(domain).begin(), viennagrid::elements<line_type>(domain).end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;
    
    cout << "All triangles of the domain" << endl;
    std::copy( viennagrid::elements<triangle_type>(domain).begin(), viennagrid::elements<triangle_type>(domain).end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    cout << "All tetraherons of the domain" << endl;
    std::copy( viennagrid::elements<tetrahedron_type>(domain).begin(), viennagrid::elements<tetrahedron_type>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    
    //
    // doing some boundary cell iteration
    //
    
    const tetrahedron_type & test_tet = *viennagrid::elements<viennagrid::tetrahedron_tag>(domain).begin();
    const triangle_type & test_tri = *viennagrid::elements<viennagrid::triangle_tag>(domain).begin();
    
    
    
//     typedef viennagrid::result_of::const_ncell_range<tetrahedron_type, 2>::type tetrahedron_triangle_range;
//     typedef viennagrid::result_of::const_iterator<tetrahedron_triangle_range>::type tetrahedron_triangle_iterator;
//     
//     cout << "All triangles of the first tetdrahedron in the domain" << endl;
//     tetrahedron_triangle_range tri_range = viennagrid::elements<viennagrid::triangle_tag>(test_tet);
//     for (tetrahedron_triangle_iterator it = tri_range.begin(); it != tri_range.end(); ++it)
//         cout << *it << endl;
//     cout << endl;
//     
//     cout << "Once more with std::copy" << endl;
//     std::copy( tri_range.begin(), tri_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
//     cout << endl;
    
    
    
    
    
    typedef viennagrid::result_of::const_element_range<tetrahedron_type, viennagrid::triangle_tag>::type tetrahedron_triangle_range_2;
    typedef viennagrid::result_of::const_iterator<tetrahedron_triangle_range_2>::type tetrahedron_triangle_iterator_2;
    
    cout << "All triangles of the first tetdrahedron in the domain" << endl;
    tetrahedron_triangle_range_2 tri_range_2 = viennagrid::elements<viennagrid::triangle_tag>(test_tet);
    for (tetrahedron_triangle_iterator_2 it = tri_range_2.begin(); it != tri_range_2.end(); ++it)
        cout << *it << endl;
    cout << endl;
    
    cout << "Once more with std::copy" << endl;
    std::copy( tri_range_2.begin(), tri_range_2.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    
    
    
    
    typedef viennagrid::result_of::const_element_range<triangle_type, viennagrid::line_tag>::type triangle_line_range;
    typedef viennagrid::result_of::const_iterator<triangle_line_range>::type triangle_line_iterator;

    cout << "All lines of the first triangle in the domain" << endl;
    triangle_line_range lin_range = viennagrid::elements<viennagrid::line_tag>(test_tri);
    for (triangle_line_iterator it = lin_range.begin(); it != lin_range.end(); ++it)
        cout << *it << endl;
    cout << endl;
    
    cout << "Once more with std::copy" << endl;
    std::copy( lin_range.begin(), lin_range.end(), std::ostream_iterator<line_type>(cout, "\n") );
    cout << endl;

    return 0;
}


