#include <iostream>
#include <typeinfo>


#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/mesh/element_deletion.hpp"
#include "viennagrid/mesh/coboundary_iteration.hpp"
#include "viennagrid/io/vtk_writer.hpp"



struct config
{
  typedef viennagrid::storage::pointer_handle_tag handle_tag;
  //typedef viennagrid::storage::iterator_handle_tag handle_tag;
//   typedef viennagrid::storage::id_handle_tag          handle_tag;

  typedef viennagrid::config::result_of::full_mesh_config< viennagrid::triangle_tag, viennagrid::config::point_type_2d, handle_tag >::type     type;
};

typedef viennagrid::mesh_t<config> MeshType;



void create_mesh( MeshType & dom )
{
    typedef viennagrid::result_of::point<MeshType>::type point_type;

    typedef viennagrid::result_of::vertex_handle<MeshType>::type vertex_handle_type;
    typedef viennagrid::result_of::vertex<MeshType>::type vertex_type;

    typedef viennagrid::result_of::line<MeshType>::type line_type;

    typedef viennagrid::result_of::triangle_handle<MeshType>::type triangle_handle_type;
    typedef viennagrid::result_of::triangle<MeshType>::type triangle_type;

    MeshType mesh;

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


    dom = mesh;
}



int main()
{
    MeshType mesh;
    create_mesh(mesh);

    typedef viennagrid::result_of::vertex_range<MeshType>::type VertexRangeType;
    VertexRangeType vertices = viennagrid::elements( mesh );
    for (VertexRangeType::iterator it = vertices.begin(); it != vertices.end(); ++it)
      std::cout << *it << std::endl;
    std::cout << std::endl;

    typedef viennagrid::result_of::line_range<MeshType>::type LineRangeType;
    LineRangeType lines = viennagrid::elements( mesh );
    for (LineRangeType::iterator it = lines.begin(); it != lines.end(); ++it)
      std::cout << *it << std::endl;
    std::cout << std::endl;


    typedef viennagrid::result_of::triangle_range<MeshType>::type TriangleRangeType;
    TriangleRangeType triangles = viennagrid::elements( mesh );
    for (TriangleRangeType::iterator it = triangles.begin(); it != triangles.end(); ++it)
      std::cout << *it << std::endl;
    std::cout << std::endl;



    {
        viennagrid::io::vtk_writer<MeshType> vtk_writer;
        vtk_writer(mesh, "copied_mesh");
    }

    return 0;
}
