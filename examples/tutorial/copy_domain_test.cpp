#include <iostream>
#include <typeinfo>


#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/domain/element_creation.hpp"
#include "viennagrid/domain/element_deletion.hpp"
#include "viennagrid/domain/coboundary_iteration.hpp"
#include "viennagrid/io/vtk_writer.hpp"



struct config
{
  typedef viennagrid::storage::pointer_handle_tag handle_tag;
  //typedef viennagrid::storage::iterator_handle_tag handle_tag;
//   typedef viennagrid::storage::id_handle_tag          handle_tag;
  
  typedef viennagrid::config::result_of::full_domain_config< viennagrid::triangle_tag, viennagrid::config::point_type_2d, handle_tag >::type     type;
};

typedef viennagrid::domain_t<config> DomainType; 



void create_domain( DomainType & dom )
{
    typedef viennagrid::result_of::point<DomainType>::type point_type;

    typedef viennagrid::result_of::vertex_handle<DomainType>::type vertex_handle_type;
    typedef viennagrid::result_of::vertex<DomainType>::type vertex_type;

    typedef viennagrid::result_of::line<DomainType>::type line_type;

    typedef viennagrid::result_of::triangle_handle<DomainType>::type triangle_handle_type;
    typedef viennagrid::result_of::triangle<DomainType>::type triangle_type;

    DomainType domain;

    vertex_handle_type v00 = viennagrid::make_vertex(domain, point_type(0.0, 0.0));
    vertex_handle_type v10 = viennagrid::make_vertex(domain, point_type(1.0, 0.0));
    vertex_handle_type v20 = viennagrid::make_vertex(domain, point_type(2.0, 0.0));
    vertex_handle_type v01 = viennagrid::make_vertex(domain, point_type(0.0, 1.0));
    vertex_handle_type v11 = viennagrid::make_vertex(domain, point_type(1.0, 1.0));
    vertex_handle_type v21 = viennagrid::make_vertex(domain, point_type(2.0, 1.0));
    vertex_handle_type v02 = viennagrid::make_vertex(domain, point_type(0.0, 2.0));
    vertex_handle_type v12 = viennagrid::make_vertex(domain, point_type(1.0, 2.0));
    vertex_handle_type v22 = viennagrid::make_vertex(domain, point_type(2.0, 2.0));


    triangle_handle_type triangle_handle_0 = viennagrid::make_triangle(domain, v00, v01, v11);
    triangle_handle_type triangle_handle_1 = viennagrid::make_triangle(domain, v00, v10, v11);
    triangle_handle_type triangle_handle_2 = viennagrid::make_triangle(domain, v10, v11, v20);
    triangle_handle_type triangle_handle_3 = viennagrid::make_triangle(domain, v11, v20, v21);
    triangle_handle_type triangle_handle_4 = viennagrid::make_triangle(domain, v11, v21, v22);
    triangle_handle_type triangle_handle_5 = viennagrid::make_triangle(domain, v12, v11, v22);
    triangle_handle_type triangle_handle_6 = viennagrid::make_triangle(domain, v02, v11, v12);
    triangle_handle_type triangle_handle_7 = viennagrid::make_triangle(domain, v01, v11, v02);


    dom = domain;
}



int main()
{
    DomainType domain;
    create_domain(domain);

    typedef viennagrid::result_of::vertex_range<DomainType>::type VertexRangeType;
    VertexRangeType vertices = viennagrid::elements( domain );
    for (VertexRangeType::iterator it = vertices.begin(); it != vertices.end(); ++it)
      std::cout << *it << std::endl;
    std::cout << std::endl;
    
    typedef viennagrid::result_of::line_range<DomainType>::type LineRangeType;
    LineRangeType lines = viennagrid::elements( domain );
    for (LineRangeType::iterator it = lines.begin(); it != lines.end(); ++it)
      std::cout << *it << std::endl;
    std::cout << std::endl;
    

    typedef viennagrid::result_of::triangle_range<DomainType>::type TriangleRangeType;
    TriangleRangeType triangles = viennagrid::elements( domain );
    for (TriangleRangeType::iterator it = triangles.begin(); it != triangles.end(); ++it)
      std::cout << *it << std::endl;
    std::cout << std::endl;


    
    {
        viennagrid::io::vtk_writer<DomainType> vtk_writer;
        vtk_writer(domain, "copied_domain");
    }
    
    return 0;
}
