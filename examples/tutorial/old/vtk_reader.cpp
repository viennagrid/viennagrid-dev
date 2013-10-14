#include <iostream>
#include <typeinfo>

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/mesh/accessor.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/mesh/element_creation.hpp"



#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"


class my_mesh_config
{
  private:
    typedef viennagrid::point_t<double, viennagrid::cartesian_cs<2> > point_type;

  public:

    typedef viennagrid::config::result_of::full_mesh_config< viennagrid::triangle_tag, point_type, viennagrid::storage::pointer_handle_tag >::type    type;
};


int main()
{
  //typedef viennagrid::mesh<my_mesh_config>                  mesh_type;
  typedef viennagrid::triangular_2d_mesh                        mesh_type;
  typedef viennagrid::result_of::mesh_view< mesh_type >::type view_type;

  typedef viennagrid::result_of::point<mesh_type>::type point_type;

  typedef viennagrid::result_of::cell< mesh_type >::type     cell_type;
  typedef viennagrid::result_of::vertex< mesh_type >::type     vertex_type;
  typedef viennagrid::result_of::vertex_handle< mesh_type >::type     vertex_handle_type;
  typedef viennagrid::result_of::triangle_handle< mesh_type >::type   triangle_handle_type;
  typedef viennagrid::result_of::triangle< mesh_type >::type          triangle_type;


  typedef viennagrid::result_of::oriented_3d_hull_segmentation<mesh_type>::type segmentation_type;
//   typedef viennagrid::result_of::segmentation<mesh_type>::type segmentation_type;

  typedef segmentation_type::segment_handle_type segment_handle_type;
  typedef segmentation_type::segment_id_type segment_id_type;

  mesh_type mesh;
  segmentation_type segmentation(mesh);


  viennagrid::io::vtk_reader<mesh_type, segmentation_type> reader;

  std::deque<double> potential_point;
  std::deque<double> potential_cell;
  reader.register_vertex_scalar( viennagrid::make_accessor<vertex_type>(potential_point), "potential_point" );
  reader.register_cell_scalar( viennagrid::make_accessor<cell_type>(potential_cell), "potential_cell" );

  reader(mesh, segmentation, "test_main.pvd");

  segment_handle_type seg0 = segmentation[0];
  segment_handle_type seg1 = segmentation[1];


  viennagrid::io::vtk_writer<mesh_type, segmentation_type> writer;

  writer.add_scalar_data_on_vertices( viennagrid::make_accessor<vertex_type>(potential_point), "potential_point" );
  writer.add_scalar_data_on_vertices( seg0, reader.vertex_scalar_field("potential_point_segment", seg0), "potential_point_segment" );

  writer.add_scalar_data_on_cells( viennagrid::make_accessor<cell_type>(potential_point), "potential_cell" );
  writer.add_scalar_data_on_cells( seg1, reader.cell_scalar_field("potential_cell_segment", seg1), "potential_cell_segment" );

  writer( mesh, segmentation, "vtk_reader_test" );



    return 0;
}
