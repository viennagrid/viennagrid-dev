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
#include "viennagrid/domain/coboundary_iteration.hpp"



int main()
{
    
    //
    // First define the type of hook to use:
    //
    
    typedef viennagrid::storage::pointer_hook_tag hook_tag;
    //typedef viennagrid::storage::iterator_hook_tag hook_tag;
    //typedef viennagrid::storage::id_hook_tag hook_tag;
    
    
    
    
    
    typedef viennagrid::result_of::default_topologic_config<viennagrid::triangle_tag, hook_tag>::type toplological_config;

    
    
    
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
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::triangle_tag>::type triangle_hook_type;
    

    //
    // Adding a tetrahedron
    //

    // creates four vertices within the domain, vh is short vor vertex hook
    vertex_hook_type vh0 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh1 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh2 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh3 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh4 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh5 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh6 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh7 = viennagrid::create_element<vertex_type>( domain );
    vertex_hook_type vh8 = viennagrid::create_element<vertex_type>( domain );
    
    // creates a hook buffer for the vertex hooks of the tetdrahedron
    //std::vector<vertex_hook_type> hooks(4);
    viennagrid::storage::static_array<vertex_hook_type, 3> hooks;
    
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    hooks[0] = vh0; hooks[1] = vh1; hooks[2] = vh3;
    viennagrid::create_element<triangle_type>( domain, hooks );

    hooks[0] = vh1; hooks[1] = vh4; hooks[2] = vh3;
    viennagrid::create_element<triangle_type>( domain, hooks );
    
    hooks[0] = vh1; hooks[1] = vh5; hooks[2] = vh4;
    viennagrid::create_element<triangle_type>( domain, hooks );
    
    hooks[0] = vh1; hooks[1] = vh2; hooks[2] = vh5;
    viennagrid::create_element<triangle_type>( domain, hooks );
    
    hooks[0] = vh3; hooks[1] = vh7; hooks[2] = vh6;
    viennagrid::create_element<triangle_type>( domain, hooks );
    
    hooks[0] = vh3; hooks[1] = vh4; hooks[2] = vh7;
    viennagrid::create_element<triangle_type>( domain, hooks );
    
    hooks[0] = vh4; hooks[1] = vh5; hooks[2] = vh7;
    viennagrid::create_element<triangle_type>( domain, hooks );

    hooks[0] = vh5; hooks[1] = vh8; hooks[2] = vh7;
    viennagrid::create_element<triangle_type>( domain, hooks );



    
    

    
    typedef viennagrid::result_of::coboundary_range<domain_type, viennagrid::triangle_tag>::type coboundary_range_type;
    coboundary_range_type coboundary_range = viennagrid::coboundary_elements<viennagrid::triangle_tag>(domain, vh4);
    cout << "All triangles connected to vh4" << endl;
    std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    
    coboundary_range = viennagrid::coboundary_elements<viennagrid::triangle_tag>(domain, vh4);
    cout << "All triangles connected to vh4" << endl;
    std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    coboundary_range = viennagrid::coboundary_elements<viennagrid::triangle_tag>(domain, vh2);
    cout << "All triangles connected to vh2" << endl;
    std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;


    
    const domain_type & cdomain = domain;
    typedef viennagrid::result_of::const_coboundary_range<domain_type, viennagrid::triangle_tag>::type const_coboundary_range_type;
    
    const_coboundary_range_type const_coboundary_range = viennagrid::coboundary_elements<viennagrid::triangle_tag>(cdomain, vh4);
    cout << "All triangles connected to vh4" << endl;
    std::copy( const_coboundary_range.begin(), const_coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    const_coboundary_range = viennagrid::coboundary_elements<viennagrid::triangle_tag>(cdomain, vh4);
    cout << "All triangles connected to vh4" << endl;
    std::copy( const_coboundary_range.begin(), const_coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    const_coboundary_range = viennagrid::coboundary_elements<viennagrid::triangle_tag>(cdomain, vh2);
    cout << "All triangles connected to vh2" << endl;
    std::copy( const_coboundary_range.begin(), const_coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;



    return 0;
}


