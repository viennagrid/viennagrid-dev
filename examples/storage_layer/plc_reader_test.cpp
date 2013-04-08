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
#include "viennagrid/topology/plc.hpp"


#include "viennagrid/element/element_config.hpp"
#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/element/element_view.hpp"

#include "viennagrid/point.hpp"

#include "viennagrid/config/generic_config.hpp"

#include "viennagrid/domain/topologic_domain.hpp"
#include "viennagrid/domain/metainfo.hpp"
#include "viennagrid/domain/geometric_domain.hpp"
#include "viennagrid/domain/config.hpp"
#include "viennagrid/domain/element_creation.hpp"

#include "viennagrid/io/poly_reader.hpp"





int main()
{
    
    //
    // First define the type of hook to use:
    //
    
//     typedef viennagrid::storage::pointer_hook_tag hook_tag;
    //typedef viennagrid::storage::iterator_hook_tag hook_tag;
    typedef viennagrid::storage::id_hook_tag hook_tag;
    
    //
    // typedefing and setting up the topological domain
    //
    
    typedef viennagrid::result_of::geometric_domain_config<viennagrid::plc_tag, viennagrid::config::point_type_3d, viennagrid::storage::pointer_hook_tag>::type domain_config;
    
    //typedef viennagrid::config::polygonal_2d_domain domain_type;
    typedef viennagrid::result_of::geometric_domain< domain_config >::type domain_type;
    domain_type domain;
    
    
    
    
    //
    // typedefs for the element types
    //    
    
    typedef viennagrid::result_of::point_type<domain_type>::type point_type;
     
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::vertex_tag>::type vertex_hook_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::line_tag>::type line_hook_type;
    
//     typedef viennagrid::result_of::element<domain_type, viennagrid::polygon_tag>::type polygon_type;
//     typedef viennagrid::result_of::element_hook<domain_type, viennagrid::polygon_tag>::type polygon_hook_type;

    
    

    typedef viennagrid::result_of::element<domain_type, viennagrid::plc_tag>::type plc_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::plc_tag>::type plc_hook_type;
    
    viennagrid::io::poly_reader reader;
    
    reader(domain, "/export/florian/work/meshes/simple/test.poly");
    
    plc_type & plc = viennagrid::elements<plc_type>(domain)[0];
    
    

    typedef viennagrid::result_of::element_range<plc_type, vertex_type>::type vertex_on_plc_range;
    typedef viennagrid::result_of::iterator<vertex_on_plc_range>::type vertex_on_plc_iterator;
    
    
    viennagrid::tagging::result_of::element_tag<plc_type, viennagrid::loose_tag>::type loose_tag = viennagrid::tagging::create_element_tag<viennagrid::loose_tag>(plc);
    
    std::cout << "All vertices of the PLC" << std::endl;
    vertex_on_plc_range vertex_range = viennagrid::elements<vertex_type>( plc );
    for (vertex_on_plc_iterator it = vertex_range.begin(); it != vertex_range.end(); ++it)
        std::cout << *it << " " << viennagrid::point(domain, *it) << " is loose: " << viennagrid::tagging::is_tagged(*it, loose_tag) << std::endl;
    std::cout << std::endl;
    
    typedef viennagrid::result_of::element_range<plc_type, line_type>::type line_on_plc_range;
    typedef viennagrid::result_of::iterator<line_on_plc_range>::type line_on_plc_iterator;
    
    std::cout << "All lines of the PLC" << std::endl;
    line_on_plc_range line_range = viennagrid::elements<line_type>( plc );
    for (line_on_plc_iterator it = line_range.begin(); it != line_range.end(); ++it)
        std::cout << *it << " is loose: " << viennagrid::tagging::is_tagged(*it, loose_tag) << std::endl;
    std::cout << std::endl;
    
    std::cout << "All PLCs of the domain" << std::endl;
    typedef viennagrid::result_of::element_range< domain_type, plc_type >::type plc_range_type;
    plc_range_type plc_range = viennagrid::elements< plc_type >(domain);
    for ( plc_range_type::iterator it = plc_range.begin(); it != plc_range.end(); ++it)
        std::cout << *it << std::endl;
    std::cout << std::endl;
        
        
    typedef viennagrid::result_of::const_element_view<plc_type, vertex_type>::type vertex_view_type;
    typedef viennagrid::result_of::iterator<vertex_view_type>::type vertex_view_iterator;
    
    std::cout << "All loose points of the plc" << std::endl;
    vertex_view_type vertex_view = viennagrid::loose_elements<vertex_type>(plc);
    for (vertex_view_iterator it = vertex_view.begin(); it != vertex_view.end(); ++it)
        std::cout << *it << std::endl;
    std::cout << std::endl;
    
    

    typedef viennagrid::result_of::element_view<plc_type, line_type>::type line_view_type;

    std::cout << "All inner lines of the plc" << std::endl;
    viennagrid::tagging::result_of::element_tag<plc_type, viennagrid::bounding_tag>::type bounding_tag = viennagrid::tagging::create_element_tag<viennagrid::bounding_tag>(plc);
    line_view_type inner_lines = viennagrid::untagged_elements<line_type>(plc, bounding_tag);
    std::copy( inner_lines.begin(), inner_lines.end(), std::ostream_iterator<line_type>(std::cout, "\n") );
    std::cout << std::endl;
    
    
    std::cout << "All hole points of the plc" << std::endl;
    std::vector<point_type> & pts = viennagrid::hole_points<domain_type>(plc);
    std::copy( pts.begin(), pts.end(), std::ostream_iterator<point_type>(std::cout, "\n") );
    
    return 0;
}
