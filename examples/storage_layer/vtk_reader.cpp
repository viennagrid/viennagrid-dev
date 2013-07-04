#include <iostream>
#include <typeinfo>

//#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/domain/accessor.hpp"
#include "viennagrid/domain/segmentation.hpp"
#include "viennagrid/domain/element_creation.hpp"

#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"


class my_domain_config
{
  private:
    typedef viennagrid::point_t<double, viennagrid::cartesian_cs<2> > point_type;
    
  public:
    
    typedef viennagrid::config::result_of::full_domain_config< viennagrid::triangle_tag, point_type, viennagrid::storage::pointer_handle_tag >::type    type;
};


int main()
{
  typedef viennagrid::domain_t<my_domain_config>                  domain_type;
  typedef viennagrid::result_of::domain_view< domain_type >::type view_type;
  
  typedef viennagrid::result_of::point_type<domain_type>::type point_type;
  
  typedef viennagrid::result_of::cell_type< domain_type >::type     cell_type;
  typedef viennagrid::result_of::vertex< domain_type >::type     vertex_type;
  typedef viennagrid::result_of::vertex_handle< domain_type >::type     vertex_handle_type;
  typedef viennagrid::result_of::triangle_handle< domain_type >::type   triangle_handle_type;
  typedef viennagrid::result_of::triangle< domain_type >::type          triangle_type;
  
    
  typedef viennagrid::result_of::oriented_3d_hull_segmentation<domain_type>::type segmentation_type;
//   typedef viennagrid::result_of::segmentation<domain_type>::type segmentation_type;
  
  typedef segmentation_type::segment_type segment_type;
  typedef segmentation_type::segment_id_type segment_id_type;
  
  domain_type domain;
  segmentation_type segmentation(domain);
  
  
  viennagrid::io::vtk_reader<domain_type, segmentation_type> reader;
  
  std::deque<double> potential_point;
  std::deque<double> potential_cell;
  reader.register_vertex_scalar_accessor( viennagrid::accessor::dense_container_accessor<vertex_type>(potential_point), "potential_point" );
  reader.register_cell_scalar_accessor( viennagrid::accessor::dense_container_accessor<cell_type>(potential_cell), "potential_cell" );
  
  reader(domain, segmentation, "test_main.pvd");
  
  segment_type seg0 = segmentation[0];
  segment_type seg1 = segmentation[1];  
  
  
  viennagrid::io::vtk_writer<domain_type, segmentation_type> writer;
  
  writer.add_scalar_data_on_vertices( viennagrid::accessor::dense_container_accessor<vertex_type>(potential_point), "potential_point" );
  writer.add_scalar_data_on_vertices( seg0, reader.vertex_scalar_accessor("potential_point_segment", seg0), "potential_point_segment" );

  writer.add_scalar_data_on_cells( viennagrid::accessor::dense_container_accessor<cell_type>(potential_point), "potential_cell" );
  writer.add_scalar_data_on_cells( seg1, reader.cell_scalar_accessor("potential_cell_segment", seg1), "potential_cell_segment" );
  
  writer( domain, segmentation, "vtk_reader_test" );
  

    
    return 0;
}
