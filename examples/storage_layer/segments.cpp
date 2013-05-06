#include <iostream>
#include <typeinfo>

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/domain/segmentation.hpp"
#include "viennagrid/domain/element_creation.hpp"




int main()
{
    {
        typedef viennagrid::config::tetrahedral_3d config;
        typedef viennagrid::result_of::domain<config>::type domain_type;
        domain_type domain;
        
        typedef viennagrid::result_of::element_segment_info<domain_type, viennagrid::tetrahedron_tag>::type element_segment_info;
        typedef viennagrid::result_of::segmentation<domain_type, viennagrid::tetrahedron_tag>::type segmentation_type;
        segmentation_type segmentation;
        
        typedef viennagrid::result_of::point_type<domain_type>::type point_type;
        typedef viennagrid::result_of::vertex_handle<domain_type>::type vertex_handle_type;
        typedef viennagrid::result_of::tetrahedron_handle<domain_type>::type tetrahedron_handle_type;
        
        vertex_handle_type v0 = viennagrid::create_vertex(domain, point_type(0.0, 0.0, 0.0));
        vertex_handle_type v1 = viennagrid::create_vertex(domain, point_type(1.0, 0.0, 0.0));
        vertex_handle_type v2 = viennagrid::create_vertex(domain, point_type(0.0, 1.0, 0.0));
        vertex_handle_type v3 = viennagrid::create_vertex(domain, point_type(0.0, 0.0, 1.0));
        
        tetrahedron_handle_type tet = viennagrid::create_tetrahedron(domain, v0, v1, v2, v3);
        
        segmentation.set_segment_info( viennagrid::dereference_handle(domain, tet), element_segment_info(1) );
        
        std::cout << segmentation.segment_info( viennagrid::dereference_handle(domain, tet) ).segment_id << std::endl;
    }

    
    {
        typedef viennagrid::config::triangular_3d config;
        typedef viennagrid::result_of::domain<config>::type domain_type;
        domain_type domain;
        
        typedef viennagrid::result_of::element_segment_info<domain_type, viennagrid::triangle_tag>::type element_segment_info;
        typedef viennagrid::result_of::segmentation<domain_type, viennagrid::triangle_tag>::type segmentation_type;
        segmentation_type segmentation;
        
        typedef viennagrid::result_of::point_type<domain_type>::type point_type;
        typedef viennagrid::result_of::vertex_handle<domain_type>::type vertex_handle_type;
        typedef viennagrid::result_of::triangle_handle<domain_type>::type triangle_handle_type;
        
        vertex_handle_type v0 = viennagrid::create_vertex(domain, point_type(0.0, 0.0, 0.0));
        vertex_handle_type v1 = viennagrid::create_vertex(domain, point_type(1.0, 0.0, 0.0));
        vertex_handle_type v2 = viennagrid::create_vertex(domain, point_type(0.0, 1.0, 0.0));
        
        triangle_handle_type tri = viennagrid::create_triangle(domain, v0, v1, v2);
        
        segmentation.set_segment_info( viennagrid::dereference_handle(domain, tri), element_segment_info(1,2) );
        
        std::cout << segmentation.segment_info( viennagrid::dereference_handle(domain, tri) ).positive_orientation_segment_id << std::endl;
        std::cout << segmentation.segment_info( viennagrid::dereference_handle(domain, tri) ).negative_orientation_segment_id << std::endl;
    }
    
    return 0;
}