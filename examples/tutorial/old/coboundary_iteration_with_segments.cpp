#include <iostream>
#include <typeinfo>

#include <map>

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

#include "viennagrid/domain/domain.hpp"
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
    
    typedef viennagrid::result_of::segmentation<domain_type>::type segmentation_type;
    typedef viennagrid::result_of::segment<segmentation_type>::type segment_type;
    
    segmentation_type segmentation(domain);
    
    segment_type seg0 = segmentation.make_segment();
    
    
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
       
    // creates the tetrahedron within the domain, all boundary cell generation is done here implicit
    viennagrid::make_triangle( seg0, vh0, vh2, vh1 );
    viennagrid::make_triangle( seg0, vh0, vh3, vh2 );
    viennagrid::make_triangle( seg0, vh1, vh2, vh4 );
    

    
    typedef viennagrid::result_of::coboundary_range<segment_type, viennagrid::vertex_tag, viennagrid::triangle_tag>::type coboundary_range_type;
    coboundary_range_type coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(seg0, vh2);
    cout << "All triangles connected to vh4" << endl;
    std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    std::cout << "Adding another triangle to segment" << std::endl;
    viennagrid::make_triangle( seg0, vh2, vh3, vh4 );
    cout << endl;
    
    coboundary_range = viennagrid::coboundary_elements<viennagrid::vertex_tag, viennagrid::triangle_tag>(seg0, vh2);
    cout << "All triangles connected to vh4" << endl;
    std::copy( coboundary_range.begin(), coboundary_range.end(), std::ostream_iterator<triangle_type>(cout, "\n") );
    cout << endl;
    
    return 0;
}


