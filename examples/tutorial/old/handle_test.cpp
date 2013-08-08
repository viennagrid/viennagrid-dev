#include <iostream>
#include <typeinfo>

#include "viennagrid/config/default_configs.hpp"

// #include "viennagrid/domain/segmentation.hpp"
#include "viennagrid/domain/element_creation.hpp"


typedef viennagrid::point_t<double, viennagrid::cartesian_cs<3> > vector_type;

int main()
{
    
//     typedef viennagrid::config::tetrahedral_3d config;
    typedef viennagrid::tetrahedral_3d_domain   domain_type;
    domain_type domain;
        
    typedef viennagrid::result_of::point<domain_type>::type point_type;
    typedef viennagrid::result_of::vertex_handle<domain_type>::type vertex_handle_type;
    typedef viennagrid::result_of::tetrahedron<domain_type>::type tetrahedron_type;
    typedef viennagrid::result_of::tetrahedron_handle<domain_type>::type tetrahedron_handle_type;
    
    vertex_handle_type v0 = viennagrid::make_vertex(domain, point_type(0.0, 0.0, 0.0));
    vertex_handle_type v1 = viennagrid::make_vertex(domain, point_type(1.0, 0.0, 0.0));
    vertex_handle_type v2 = viennagrid::make_vertex(domain, point_type(0.0, 1.0, 0.0));
    vertex_handle_type v3 = viennagrid::make_vertex(domain, point_type(0.0, 0.0, 1.0));
    vertex_handle_type v4 = viennagrid::make_vertex(domain, point_type(0.0, 1.0, 1.0));
    
    tetrahedron_handle_type tet_handle0 = viennagrid::make_tetrahedron(domain, v0, v1, v2, v3);
    tetrahedron_handle_type tet_handle1 = viennagrid::make_tetrahedron(domain, v0, v1, v2, v4);
    std::cout << tet_handle0 << std::endl;
    std::cout << tet_handle1 << std::endl;
    
    tetrahedron_type & tet = viennagrid::dereference_handle(domain, tet_handle0);
    std::cout << tet << std::endl;
    
    tetrahedron_handle_type tet_handle2 = viennagrid::handle(domain, tet);
    std::cout << tet_handle2 << std::endl;
    
    tetrahedron_type & tet2 = viennagrid::dereference_handle(domain, tet_handle2);
    std::cout << tet2 << std::endl;
    
    
    
    typedef viennagrid::result_of::element<tetrahedron_type, viennagrid::line_tag>::type line_type;
    typedef viennagrid::result_of::handle<tetrahedron_type, viennagrid::line_tag>::type line_handle_type;
    
    line_type & line = viennagrid::lines(tet)[2];
    std::cout << viennagrid::lines(tet).handle_at(2) << std::endl;
    std::cout << viennagrid::dereference_handle(tet, viennagrid::lines(tet).handle_at(2)) << std::endl;
    
    line_handle_type line_handle = viennagrid::handle(tet, line);
    std::cout << line_handle << std::endl;
    std::cout << viennagrid::dereference_handle(tet, line_handle) << std::endl;
    
    
    return 0;
}
