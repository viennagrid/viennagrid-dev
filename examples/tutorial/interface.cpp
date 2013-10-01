#include <iostream>
#include <typeinfo>

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/mesh/accessor.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/mesh/element_creation.hpp"

#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/interface.hpp"

int main()
{
  typedef viennagrid::triangular_3d_mesh                        mesh_type;
  typedef viennagrid::result_of::mesh_view< mesh_type >::type view_type;
  
  
  
  typedef viennagrid::result_of::vertex< mesh_type >::type     vertex_type;
  typedef viennagrid::result_of::line< mesh_type >::type       line_type;
  
  typedef viennagrid::result_of::vertex_handle< mesh_type >::type     vertex_handle_type;
  typedef viennagrid::result_of::triangle_handle< mesh_type >::type   triangle_handle_type;
  typedef viennagrid::result_of::triangle< mesh_type >::type          triangle_type;
   
  typedef viennagrid::result_of::oriented_3d_hull_segmentation<mesh_type>::type segmentation_type;
//   typedef viennagrid::result_of::segmentation<mesh_type>::type segmentation_type;
  
  typedef segmentation_type::segment_type segment_type;
  typedef segmentation_type::segment_id_type segment_id_type;
  
  mesh_type mesh;
  segmentation_type segmentation(mesh);
  
  
  vertex_handle_type vh0 = viennagrid::make_vertex( mesh );
  vertex_handle_type vh1 = viennagrid::make_vertex( mesh );
  vertex_handle_type vh2 = viennagrid::make_vertex( mesh );
  vertex_handle_type vh3 = viennagrid::make_vertex( mesh );
  vertex_handle_type vh4 = viennagrid::make_vertex( mesh );
  
  triangle_handle_type th0 = viennagrid::make_triangle( mesh, vh0, vh1, vh2 );
  triangle_handle_type th1 = viennagrid::make_triangle( mesh, vh0, vh2, vh3 );
  triangle_handle_type th2 = viennagrid::make_triangle( mesh, vh0, vh3, vh4 );
  triangle_handle_type th3 = viennagrid::make_triangle( mesh, vh0, vh4, vh1 );

  
  segment_type seg0 = segmentation.make_segment();
  segment_type seg1 = segmentation.make_segment(); 
  
    
  triangle_type & tri0 = viennagrid::dereference_handle(mesh, th0);
  triangle_type & tri1 = viennagrid::dereference_handle(mesh, th1);
  triangle_type & tri2 = viennagrid::dereference_handle(mesh, th2);
  triangle_type & tri3 = viennagrid::dereference_handle(mesh, th3);
  
  viennagrid::add( seg0, tri0 );
  viennagrid::add( seg0, tri1 );
  viennagrid::add( seg1, tri2 );
  viennagrid::add( seg1, tri3 );
  
  typedef viennagrid::result_of::element_range< mesh_type, viennagrid::line_tag >::type line_range;
  line_range lines = viennagrid::elements( mesh );
  for (line_range::iterator it = lines.begin(); it != lines.end(); ++it)
  {
    std::cout << *it << " INTERFACE? " << viennagrid::is_interface( seg0, seg1, *it ) << std::endl;
  }

  typedef viennagrid::result_of::element_range< mesh_type, viennagrid::vertex_tag >::type vertex_range;
  vertex_range vertices = viennagrid::elements( mesh );
  for (vertex_range::iterator it = vertices.begin(); it != vertices.end(); ++it)
  {
    std::cout << *it << " INTERFACE? " << viennagrid::is_interface( seg0, seg1, *it ) << std::endl;
  }
  
  
  return 0;
}
