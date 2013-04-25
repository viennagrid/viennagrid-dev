#include <iostream>


#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/domain/segments.hpp"
#include "viennagrid/domain/element_creation.hpp"




int main()
{
    typedef viennagrid::config::tetrahedral_3d domain_config;
    
    
    typedef viennagrid::element_segment_info< viennagrid::tetrahedron_tag, viennagrid::tetrahedron_tag, viennagrid::disjunct_segmentation_tag, int > tetrahedron_segment_information;

    typedef viennameta::make_typemap< viennagrid::tetrahedron_tag, std::map<int, tetrahedron_segment_information> >::type meta_info_typemap;
    
            typedef viennameta::make_typemap<
                viennagrid::config::topology_config_tag, viennagrid::config::result_of::query_config<domain_config, viennagrid::config::topology_config_tag>::type,
                viennagrid::config::vector_type_tag, viennagrid::config::result_of::query_config<domain_config, viennagrid::config::vector_type_tag>::type,
                viennagrid::config::metainfo_typelist_tag, meta_info_typemap,
                viennagrid::config::metainfo_container_config_tag, viennagrid::config::result_of::query_config<domain_config, viennagrid::config::metainfo_container_config_tag>::type
            >::type new_domain_config;
    
    typedef viennagrid::result_of::domain<new_domain_config>::type domain_type;
    domain_type domain;
    
    typedef viennagrid::result_of::point_type<domain_type>::type point_type;
    typedef viennagrid::result_of::vertex_handle<domain_type>::type vertex_handle_type;
    typedef viennagrid::result_of::tetrahedron_handle<domain_type>::type tetrahedron_handle_type;
    
    vertex_handle_type v0 = viennagrid::create_vertex(domain, point_type(0.0, 0.0, 0.0));
    vertex_handle_type v1 = viennagrid::create_vertex(domain, point_type(1.0, 0.0, 0.0));
    vertex_handle_type v2 = viennagrid::create_vertex(domain, point_type(0.0, 1.0, 0.0));
    vertex_handle_type v3 = viennagrid::create_vertex(domain, point_type(0.0, 0.0, 1.0));
    
    tetrahedron_handle_type tet = viennagrid::create_tetrahedron(domain, v0, v1, v2, v3);
    
    
    tetrahedron_segment_information & tmp = viennagrid::look_up< std::map<int, tetrahedron_segment_information> >( domain, viennagrid::dereference_handle(domain, tet) )[0];
    tmp.segment_id = 1;
    
    std::cout << viennagrid::look_up< std::map<int, tetrahedron_segment_information> >( domain, viennagrid::dereference_handle(domain, tet) )[0].segment_id << std::endl;
}