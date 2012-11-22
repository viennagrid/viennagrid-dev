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




int main()
{
    
    //
    // First define the type of hook to use:
    //
    
    typedef viennagrid::storage::pointer_hook_tag hook_tag;
    //typedef viennagrid::storage::iterator_hook_tag hook_tag;
    //typedef viennagrid::storage::id_hook_tag hook_tag;
    
    
    
    
    
    typedef viennagrid::result_of::default_topologic_config<viennagrid::tetrahedron_tag, hook_tag>::type toplological_config;

    
    
    
    //
    // typedefing and setting up the topological domain
    //
    
    typedef viennagrid::result_of::topologic_domain<toplological_config>::type domain_type;
    domain_type domain = viennagrid::create_topologic_domain<domain_type>();
    
    //
    // typedefs for the element types
    //    
     
    typedef viennagrid::result_of::ncell<domain_type, 0>::type vertex_type;
    typedef viennagrid::result_of::ncell_hook<domain_type, 0>::type vertex_hook_type;
    
    typedef viennagrid::result_of::ncell<domain_type, 1>::type line_type;
    typedef viennagrid::result_of::ncell_hook<domain_type, 1>::type line_hook_type;
    
    typedef viennagrid::result_of::ncell<domain_type, 2>::type triangle_type;
    typedef viennagrid::result_of::ncell_hook<domain_type, 2>::type triangle_hook_type;
    
    typedef viennagrid::result_of::ncell<domain_type, 3>::type tetrahedron_type;
    typedef viennagrid::result_of::ncell_hook<domain_type, 3>::type tetrahedron_hook_type;
    
    
    
    //
    // typedefs for the view
    //   
    
    // only tetrahedrons are stored in the view
    typedef viennameta::make_typelist< tetrahedron_type >::type view_types;
    
    // typedefing the view
    typedef viennagrid::result_of::topologic_view<domain_type, view_types>::type domain_view_type;
    
    // creating two views of the domain
    domain_view_type domain_view_1 = viennagrid::create_topologic_view<domain_view_type>(domain);
    domain_view_type domain_view_2 = viennagrid::create_topologic_view<domain_view_type>(domain);

    
    //
    // filling the view/domain
    //   
    
    std::vector<vertex_hook_type> hooks(4);
    
    // pushing a tetrahedron to domain_view_1
    hooks[0] = viennagrid::create_element<vertex_type>( domain );
    hooks[1] = viennagrid::create_element<vertex_type>( domain );
    hooks[2] = viennagrid::create_element<vertex_type>( domain );
    hooks[3] = viennagrid::create_element<vertex_type>( domain );
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    viennagrid::create_element<tetrahedron_type>( domain_view_1, hooks );
    
    
    // pushing a tetrahedron to domain_view_2
    hooks[0] = viennagrid::create_element<vertex_type>( domain );
    hooks[1] = viennagrid::create_element<vertex_type>( domain );
    hooks[2] = viennagrid::create_element<vertex_type>( domain );
    hooks[3] = viennagrid::create_element<vertex_type>( domain );
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    viennagrid::create_element<tetrahedron_type>( domain_view_2, hooks );
    
    
    
    //
    // output
    //  
    
    cout << "All vertices of the domain" << endl;
    std::copy( viennagrid::ncells<3>(domain).begin(), viennagrid::ncells<3>(domain).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    cout << "All vertices of the domain view 1" << endl;
    std::copy( viennagrid::ncells<3>(domain_view_1).begin(), viennagrid::ncells<3>(domain_view_1).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;
    
    cout << "All vertices of the domain view 2" << endl;
    std::copy( viennagrid::ncells<3>(domain_view_2).begin(), viennagrid::ncells<3>(domain_view_2).end(), std::ostream_iterator<tetrahedron_type>(cout, "\n") );
    cout << endl;

    

    return 0;
}


