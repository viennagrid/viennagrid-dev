#include <iostream>
#include <typeinfo>


#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/mesh/element_deletion.hpp"
#include "viennagrid/mesh/coboundary_iteration.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "viennagrid/algorithm/refine.hpp"




int main()
{
    typedef viennagrid::triangular_2d_mesh mesh_type;

    typedef viennagrid::result_of::point<mesh_type>::type point_type;

    typedef viennagrid::result_of::vertex_handle<mesh_type>::type vertex_handle_type;
    typedef viennagrid::result_of::vertex<mesh_type>::type vertex_type;

    typedef viennagrid::result_of::line<mesh_type>::type line_type;

    typedef viennagrid::result_of::triangle_handle<mesh_type>::type triangle_handle_type;
    typedef viennagrid::result_of::triangle<mesh_type>::type triangle_type;

    mesh_type mesh;

    vertex_handle_type v00 = viennagrid::make_vertex(mesh, point_type(0.0, 0.0));
    vertex_handle_type v10 = viennagrid::make_vertex(mesh, point_type(1.0, 0.0));
    vertex_handle_type v20 = viennagrid::make_vertex(mesh, point_type(2.0, 0.0));
    vertex_handle_type v01 = viennagrid::make_vertex(mesh, point_type(0.0, 1.0));
    vertex_handle_type v11 = viennagrid::make_vertex(mesh, point_type(1.0, 1.0));
    vertex_handle_type v21 = viennagrid::make_vertex(mesh, point_type(2.0, 1.0));
    vertex_handle_type v02 = viennagrid::make_vertex(mesh, point_type(0.0, 2.0));
    vertex_handle_type v12 = viennagrid::make_vertex(mesh, point_type(1.0, 2.0));
    vertex_handle_type v22 = viennagrid::make_vertex(mesh, point_type(2.0, 2.0));


    triangle_handle_type triangle_handle_0 = viennagrid::make_triangle(mesh, v00, v01, v11);
    triangle_handle_type triangle_handle_1 = viennagrid::make_triangle(mesh, v00, v10, v11);
    triangle_handle_type triangle_handle_2 = viennagrid::make_triangle(mesh, v10, v11, v20);
    triangle_handle_type triangle_handle_3 = viennagrid::make_triangle(mesh, v11, v20, v21);
    triangle_handle_type triangle_handle_4 = viennagrid::make_triangle(mesh, v11, v21, v22);
    triangle_handle_type triangle_handle_5 = viennagrid::make_triangle(mesh, v12, v11, v22);
    triangle_handle_type triangle_handle_6 = viennagrid::make_triangle(mesh, v02, v11, v12);
    triangle_handle_type triangle_handle_7 = viennagrid::make_triangle(mesh, v01, v11, v02);

    triangle_type & triangle = viennagrid::dereference_handle( mesh, triangle_handle_0 );

    typedef viennagrid::result_of::triangle_range<mesh_type>::type triangle_range_type;
    typedef viennagrid::result_of::iterator<triangle_range_type>::type triangle_range_iterator;

    std::cout << viennagrid::triangles(mesh).size() << std::endl;


    typedef viennagrid::result_of::mesh_view<mesh_type>::type mesh_view_type;
    mesh_view_type elements_to_erase = viennagrid::make_view(mesh);
    viennagrid::mark_erase_elements( mesh, elements_to_erase, v22 );
    viennagrid::mark_erase_elements( mesh, elements_to_erase, v00 );

    viennagrid::erase_elements(mesh, elements_to_erase);


//     typedef viennagrid::result_of::vertex_range<mesh_type>::type vertex_range;
//     vertex_range vertices = viennagrid::elements(mesh);
//     for (vertex_range::iterator it = vertices.begin(); it != vertices.end(); ++it)
//       std::cout << *it << std::endl;

//     typedef viennagrid::result_of::line_range<mesh_type>::type line_range;
//     line_range lines = viennagrid::elements(mesh);
//     for (line_range::iterator it = lines.begin(); it != lines.end(); ++it)
//       std::cout << *it << std::endl;



    mesh_type refined;
    viennagrid::refine_uniformly<viennagrid::triangle_tag>(mesh, refined);
//
//
//     std::cout << viennagrid::triangles(mesh).size() << std::endl;
//     std::cout << viennagrid::triangles(refined).size() << std::endl;

    {
        viennagrid::io::vtk_writer<viennagrid::triangular_2d_mesh> vtk_writer;
        vtk_writer(mesh, "triangle_mesh");
    }

    {
        viennagrid::io::vtk_writer<viennagrid::triangular_2d_mesh> vtk_writer;
        vtk_writer(refined, "triangle_mesh_refined");
    }

    return 0;
}
