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

#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"
#include "viennagrid/point.hpp"

#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/topology_config.hpp"
#include "viennagrid/config/domain_config.hpp"
#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/domain/topology.hpp"
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/element_creation.hpp"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;


int main()
{
    //
    // typedefing and setting up the topological domain
    //
    
    typedef viennagrid::polygonal_2d_domain domain_type;
    domain_type domain;
    
    //
    // typedefs for the element types
    //    
    
    typedef viennagrid::result_of::point<domain_type>::type point_type;
     
    typedef viennagrid::result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::vertex_tag>::type vertex_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::line_tag>::type line_handle_type;
    
    typedef viennagrid::result_of::element<domain_type, viennagrid::polygon_tag>::type polygon_type;
    typedef viennagrid::result_of::handle<domain_type, viennagrid::polygon_tag>::type polygon_handle_type;
    
    
    viennagrid::storage::static_array<vertex_handle_type, 7> handles;
    
    handles[0] = viennagrid::make_vertex( domain, point_type(0, 0) );
    handles[1] = viennagrid::make_vertex( domain, point_type(1, 0) );
    handles[2] = viennagrid::make_vertex( domain, point_type(2, 1) );
    handles[3] = viennagrid::make_vertex( domain, point_type(2, 2) );
    handles[4] = viennagrid::make_vertex( domain, point_type(1, 2) );
    handles[5] = viennagrid::make_vertex( domain, point_type(0, 1) );
    handles[6] = viennagrid::make_vertex( domain, point_type(0.5, 0.5) );
    
    
    viennagrid::make_element<polygon_type>( domain, handles.begin(), handles.end() );
    
    std::cout << viennagrid::elements<viennagrid::polygon_tag>(domain)[0] << std::endl;
    

    return 0;
}


