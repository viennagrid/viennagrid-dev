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
#include "viennagrid/domain/geometric_domain.hpp"
#include "viennagrid/domain/config.hpp"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;


int main()
{
    
    //
    // First define the type of hook to use:
    //
    
    //typedef viennagrid::storage::pointer_hook_tag hook_tag;
    //typedef viennagrid::storage::iterator_hook_tag hook_tag;
    typedef viennagrid::storage::id_hook_tag hook_tag;
    
    //
    // typedefing and setting up the topological domain
    //
    
    typedef viennagrid::config::polygonal_2d_domain domain_type;
    domain_type domain;
    
    //
    // typedefs for the element types
    //    
    
    typedef viennagrid::result_of::point_type<domain_type>::type point_type;
     
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::vertex_tag>::type vertex_hook_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::line_tag>::type line_hook_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::polygon_tag>::type polygon_type;
    typedef viennagrid::result_of::element_hook<domain_type, viennagrid::polygon_tag>::type polygon_hook_type;
    
    
    viennagrid::storage::static_array<vertex_hook_type, 7> v;
    
    v[0] = viennagrid::create_element<vertex_type>( domain, point_type(0, 0) );
    v[1] = viennagrid::create_element<vertex_type>( domain, point_type(1, 0) );
    v[2] = viennagrid::create_element<vertex_type>( domain, point_type(2, 1) );
    v[3] = viennagrid::create_element<vertex_type>( domain, point_type(2, 2) );
    v[4] = viennagrid::create_element<vertex_type>( domain, point_type(1, 2) );
    v[5] = viennagrid::create_element<vertex_type>( domain, point_type(0, 1) );
    v[6] = viennagrid::create_element<vertex_type>( domain, point_type(0.5, 0.5) );
    
    
    viennagrid::create_element<polygon_type>( domain, v );
    
    std::cout << viennagrid::elements<viennagrid::polygon_tag>(domain)[0] << std::endl;
    

    return 0;
}


