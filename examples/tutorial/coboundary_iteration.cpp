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


#include "viennagrid/topology/vertex.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"


#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/topology_config.hpp"

#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/domain/topology.hpp"
#include "viennagrid/domain/coboundary_iteration.hpp"
#include "viennagrid/domain/element_creation.hpp"


#include "viennagrid/algorithm/boundary.hpp"

int main()
{
    
    //
    // typedefing and setting up the topological domain
    //
    
    typedef viennagrid::triangular_2d_domain domain_type;
    domain_type domain;
    
    
    
    
    
    //
    // typedefs for the element types
    //    
     
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::element<domain_type, viennagrid::triangle_tag>::type triangle_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::triangle_tag>::type triangle_handle_type;
    

    //
    // Adding a tetrahedron
    //

    // creates four vertices within the domain, vh is short vor vertex handle
    vertex_handle_type vh0 = viennagrid::make_vertex( domain );
    vertex_handle_type vh1 = viennagrid::make_vertex( domain );
    vertex_handle_type vh2 = viennagrid::make_vertex( domain );
    vertex_handle_type vh3 = viennagrid::make_vertex( domain );
    vertex_handle_type vh4 = viennagrid::make_vertex( domain );
    vertex_handle_type vh5 = viennagrid::make_vertex( domain );
    vertex_handle_type vh6 = viennagrid::make_vertex( domain );
    vertex_handle_type vh7 = viennagrid::make_vertex( domain );
    vertex_handle_type vh8 = viennagrid::make_vertex( domain );
       
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    viennagrid::make_triangle( domain, vh0, vh1, vh3 );
    viennagrid::make_triangle( domain, vh1, vh4, vh3 );
    viennagrid::make_triangle( domain, vh1, vh5, vh4 );
    viennagrid::make_triangle( domain, vh1, vh2, vh5 );
    viennagrid::make_triangle( domain, vh3, vh7, vh6 );
    viennagrid::make_triangle( domain, vh3, vh4, vh7 );
    viennagrid::make_triangle( domain, vh4, vh5, vh7 );
    viennagrid::make_triangle( domain, vh5, vh8, vh7 );
    

    
    typedef viennagrid::result_of::coboundary_range<domain_type, viennagrid::vertex_tag, viennagrid::triangle_tag>::type coboundary_range_type;
    coboundary_range_type coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(domain, vh4);
    cout << "All triangles connected to vh4" << endl;
    std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    
    coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(domain, vh4);
    cout << "All triangles connected to vh4" << endl;
    std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(domain, vh2);
    cout << "All triangles connected to vh2" << endl;
    std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    
    
    const domain_type & cdomain = domain;
    typedef viennagrid::result_of::const_coboundary_range<domain_type, viennagrid::vertex_tag, viennagrid::triangle_tag>::type const_coboundary_range_type;
    
    const_coboundary_range_type const_coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(cdomain, vh4);
    cout << "All triangles connected to vh4" << endl;
    std::copy( const_coboundary_range.begin(), const_coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    const_coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(cdomain, vh4);
    cout << "All triangles connected to vh4" << endl;
    std::copy( const_coboundary_range.begin(), const_coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    const_coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(cdomain, vh2);
    cout << "All triangles connected to vh2" << endl;
    std::copy( const_coboundary_range.begin(), const_coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    

    return 0;
}


