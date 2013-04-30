#include <iostream>
#include <typeinfo>

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/domain/segments.hpp"
#include "viennagrid/domain/element_creation.hpp"




int main()
{
    {
        typedef viennagrid::result_of::cell_segmented_domain_config< viennagrid::config::tetrahedral_3d >::type config;
        typedef viennagrid::result_of::domain<config>::type domain_type;
        domain_type domain;
        
        typedef viennagrid::result_of::point_type<domain_type>::type point_type;
        typedef viennagrid::result_of::vertex_handle<domain_type>::type vertex_handle_type;
        typedef viennagrid::result_of::tetrahedron_handle<domain_type>::type tetrahedron_handle_type;
        
        vertex_handle_type v0 = viennagrid::create_vertex(domain, point_type(0.0, 0.0, 0.0));
        vertex_handle_type v1 = viennagrid::create_vertex(domain, point_type(1.0, 0.0, 0.0));
        vertex_handle_type v2 = viennagrid::create_vertex(domain, point_type(0.0, 1.0, 0.0));
        vertex_handle_type v3 = viennagrid::create_vertex(domain, point_type(0.0, 0.0, 1.0));
        
        tetrahedron_handle_type tet = viennagrid::create_tetrahedron(domain, v0, v1, v2, v3);
        
        
        typedef viennagrid::result_of::cell_segment_info<domain_type>::type tetrahedron_segment_information;
        
        viennagrid::default_segment( domain, viennagrid::dereference_handle(domain, tet) ).segment_id = 1;
        
        std::cout << viennagrid::default_segment( domain, viennagrid::dereference_handle(domain, tet) ).segment_id << std::endl;    
    }
    
    {
        typedef viennagrid::result_of::cell_segmented_domain_config< viennagrid::config::triangular_3d >::type config;
        typedef viennagrid::result_of::domain<config>::type domain_type;
        domain_type domain;
        
        typedef viennagrid::result_of::point_type<domain_type>::type point_type;
        typedef viennagrid::result_of::vertex_handle<domain_type>::type vertex_handle_type;
        typedef viennagrid::result_of::triangle_handle<domain_type>::type triangle_handle_type;
        
        vertex_handle_type v0 = viennagrid::create_vertex(domain, point_type(0.0, 0.0, 0.0));
        vertex_handle_type v1 = viennagrid::create_vertex(domain, point_type(1.0, 0.0, 0.0));
        vertex_handle_type v2 = viennagrid::create_vertex(domain, point_type(0.0, 1.0, 0.0));
        
        triangle_handle_type tet = viennagrid::create_triangle(domain, v0, v1, v2);
        
        
        typedef viennagrid::result_of::cell_segment_info<domain_type>::type tetrahedron_segment_information;
        
        viennagrid::default_segment( domain, viennagrid::dereference_handle(domain, tet) ).positive_orientation_segment_id = 1;
        viennagrid::default_segment( domain, viennagrid::dereference_handle(domain, tet) ).negative_orientation_segment_id = 2;
        
        std::cout << viennagrid::default_segment( domain, viennagrid::dereference_handle(domain, tet) ).positive_orientation_segment_id << std::endl;
        std::cout << viennagrid::default_segment( domain, viennagrid::dereference_handle(domain, tet) ).negative_orientation_segment_id << std::endl;
    }
    
    return 0;
}