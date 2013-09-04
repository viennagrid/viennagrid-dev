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
#include "viennagrid/topology/plc.hpp"


#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/element/element_view.hpp"

#include "viennagrid/point.hpp"

#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/domain_config.hpp"
#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/element_creation.hpp"

#include "viennagrid/io/poly_reader.hpp"




struct WrappedConfigType
{
  typedef viennagrid::storage::pointer_handle_tag handle_tag;
  typedef viennagrid::config::result_of::full_domain_config<viennagrid::plc_tag, viennagrid::config::point_type_3d, viennagrid::storage::pointer_handle_tag>::type type;
};


int main()
{
    
    //
    // First define the type of handle to use:
    //
    
//     typedef viennagrid::storage::pointer_handle_tag handle_tag;
    //typedef viennagrid::storage::iterator_handle_tag handle_tag;
//     typedef viennagrid::storage::id_handle_tag handle_tag;
    
    //
    // typedefing and setting up the topological domain
    //
    
//     typedef viennagrid::config::result_of::full_domain_config<viennagrid::plc_tag, viennagrid::config::point_type_3d, viennagrid::storage::pointer_handle_tag>::type domain_config;
    
    //typedef viennagrid::config::polygonal_2d_domain DomainType;
    typedef viennagrid::domain_t< WrappedConfigType > DomainType;
    DomainType domain;
    
    
    
    
    //
    // typedefs for the element types
    //    
    
    typedef viennagrid::result_of::point<DomainType>::type point_type;
     
    typedef viennagrid::result_of::element<DomainType, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::handle<DomainType, viennagrid::vertex_tag>::type vertex_handle_type;
    
    typedef viennagrid::result_of::element<DomainType, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::handle<DomainType, viennagrid::line_tag>::type line_handle_type;
    
//     typedef viennagrid::result_of::element<DomainType, viennagrid::polygon_tag>::type polygon_type;
//     typedef viennagrid::result_of::handle<DomainType, viennagrid::polygon_tag>::type polygon_handle_type;

    
    

    typedef viennagrid::result_of::element<DomainType, viennagrid::plc_tag>::type plc_type;
    typedef viennagrid::result_of::handle<DomainType, viennagrid::plc_tag>::type plc_handle_type;
    
    viennagrid::io::poly_reader reader;
    
    reader(domain, "/export/florian/work/meshes/simple/test.poly");
    
    plc_type & plc = viennagrid::elements<plc_type>(domain)[0];
    
    

    typedef viennagrid::result_of::element_range<plc_type, vertex_type>::type vertex_on_plc_range;
    typedef viennagrid::result_of::iterator<vertex_on_plc_range>::type vertex_on_plc_iterator;
    
    
//     viennagrid::tagging::result_of::element_tag<plc_type, viennagrid::loose_tag>::type loose_tag = viennagrid::tagging::make_element_tag<viennagrid::loose_tag>(plc);
    
    std::cout << "All vertices of the PLC" << std::endl;
    vertex_on_plc_range vertex_range = viennagrid::elements<vertex_type>( plc );
    for (vertex_on_plc_iterator it = vertex_range.begin(); it != vertex_range.end(); ++it)
        std::cout << *it << " " << viennagrid::point(domain, *it) << std::endl;
    std::cout << std::endl;
    
    typedef viennagrid::result_of::element_range<plc_type, line_type>::type line_on_plc_range;
    typedef viennagrid::result_of::iterator<line_on_plc_range>::type line_on_plc_iterator;
    
    std::cout << "All lines of the PLC" << std::endl;
    line_on_plc_range line_range = viennagrid::elements<line_type>( plc );
    for (line_on_plc_iterator it = line_range.begin(); it != line_range.end(); ++it)
        std::cout << *it << std::endl;
    std::cout << std::endl;
    
    std::cout << "All PLCs of the domain" << std::endl;
    typedef viennagrid::result_of::element_range< DomainType, plc_type >::type plc_range_type;
    plc_range_type plc_range = viennagrid::elements< plc_type >(domain);
    for ( plc_range_type::iterator it = plc_range.begin(); it != plc_range.end(); ++it)
        std::cout << *it << std::endl;
    std::cout << std::endl;
        
        
    typedef viennagrid::result_of::const_element_view<plc_type, vertex_type>::type vertex_view_type;
    typedef viennagrid::result_of::iterator<vertex_view_type>::type vertex_view_iterator;
    
//     std::cout << "All loose points of the plc" << std::endl;
//     vertex_view_type vertex_view = viennagrid::loose_elements<vertex_type>(plc);
//     for (vertex_view_iterator it = vertex_view.begin(); it != vertex_view.end(); ++it)
//         std::cout << *it << std::endl;
//     std::cout << std::endl;
    
    

    typedef viennagrid::result_of::element_view<plc_type, line_type>::type line_view_type;

//     std::cout << "All inner lines of the plc" << std::endl;
//     viennagrid::tagging::result_of::element_tag<plc_type, viennagrid::bounding_tag>::type bounding_tag = viennagrid::tagging::make_element_tag<viennagrid::bounding_tag>(plc);
//     line_view_type inner_lines = viennagrid::untagged_elements<line_type>(plc, bounding_tag);
//     std::copy( inner_lines.begin(), inner_lines.end(), std::ostream_iterator<line_type>(std::cout, "\n") );
//     std::cout << std::endl;
    
    
    std::cout << "All hole points of the plc" << std::endl;
    std::vector<point_type> & pts = plc.appendix(); //viennagrid::hole_points<DomainType>(plc);
    std::copy( pts.begin(), pts.end(), std::ostream_iterator<point_type>(std::cout, "\n") );
    
    return 0;
}
