#include <iostream>
#include <typeinfo>


#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/domain/element_deletion.hpp"
#include "viennagrid/domain/coboundary_iteration.hpp"
#include "viennagrid/io/vtk_writer.hpp"





int main()
{
    typedef viennagrid::config::triangular_2d_domain domain_type;
    
    typedef viennagrid::result_of::point_type<domain_type>::type point_type;
    
    typedef viennagrid::result_of::vertex_handle<domain_type>::type vertex_handle_type;
    typedef viennagrid::result_of::vertex<domain_type>::type vertex_type;
    
    typedef viennagrid::result_of::line<domain_type>::type line_type;
    
    typedef viennagrid::result_of::triangle_handle<domain_type>::type triangle_handle_type;
    typedef viennagrid::result_of::triangle<domain_type>::type triangle_type;

    domain_type domain;
    
    vertex_handle_type v00 = viennagrid::create_vertex(domain, point_type(0.0, 0.0));
    vertex_handle_type v10 = viennagrid::create_vertex(domain, point_type(1.0, 0.0));
    vertex_handle_type v20 = viennagrid::create_vertex(domain, point_type(2.0, 0.0));
    vertex_handle_type v01 = viennagrid::create_vertex(domain, point_type(0.0, 1.0));
    vertex_handle_type v11 = viennagrid::create_vertex(domain, point_type(1.0, 1.0));
    vertex_handle_type v21 = viennagrid::create_vertex(domain, point_type(2.0, 1.0));
    vertex_handle_type v02 = viennagrid::create_vertex(domain, point_type(0.0, 2.0));
    vertex_handle_type v12 = viennagrid::create_vertex(domain, point_type(1.0, 2.0));
    vertex_handle_type v22 = viennagrid::create_vertex(domain, point_type(2.0, 2.0));

    
    triangle_handle_type triangle_handle_0 = viennagrid::create_triangle(domain, v00, v01, v11);
    triangle_handle_type triangle_handle_1 = viennagrid::create_triangle(domain, v00, v10, v11);
    triangle_handle_type triangle_handle_2 = viennagrid::create_triangle(domain, v10, v11, v20);
    triangle_handle_type triangle_handle_3 = viennagrid::create_triangle(domain, v11, v20, v21);
    triangle_handle_type triangle_handle_4 = viennagrid::create_triangle(domain, v11, v21, v22);
    triangle_handle_type triangle_handle_5 = viennagrid::create_triangle(domain, v12, v11, v22);
    triangle_handle_type triangle_handle_6 = viennagrid::create_triangle(domain, v02, v11, v12);
    triangle_handle_type triangle_handle_7 = viennagrid::create_triangle(domain, v01, v11, v02);
    
    triangle_type & triangle = viennagrid::dereference_handle( domain, triangle_handle_0 );
    
    typedef viennagrid::result_of::triangle_range<domain_type>::type triangle_range_type;
    typedef viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;
    
    std::cout << viennagrid::triangles(domain).size() << std::endl;
    
    
    typedef viennagrid::result_of::domain_view<domain_type>::type domain_view_type;
    domain_view_type elements_to_erase = viennagrid::create_view<domain_view_type>(domain);
    viennagrid::mark_erase_elements( domain, elements_to_erase, v22 );
    viennagrid::mark_erase_elements( domain, elements_to_erase, v00 );
    
    viennagrid::erase_elements(domain, elements_to_erase);
    
//     viennagrid::erase_element(domain, v00);

    
    std::cout << viennagrid::triangles(domain).size() << std::endl;
    
    {
        viennagrid::io::vtk_writer<viennagrid::config::triangular_2d_domain, viennagrid::config::triangular_2d_cell> vtk_writer;
        vtk_writer(domain, "triangle_domain.vtu");
    }
    
    return 0;
}
