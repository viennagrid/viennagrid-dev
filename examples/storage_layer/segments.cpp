#include <iostream>
#include <typeinfo>

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/domain/accessor.hpp"
#include "viennagrid/domain/segmentation.hpp"
#include "viennagrid/domain/element_creation.hpp"




int main()
{
  typedef viennagrid::config::triangular_3d_domain domain_type;
  typedef viennagrid::config::triangular_3d_view view_type;
  
  
  
  typedef viennagrid::result_of::vertex_handle< domain_type >::type vertex_handle_type;
  typedef viennagrid::result_of::triangle_handle< domain_type >::type triangle_handle_type;
  typedef viennagrid::result_of::triangle< domain_type >::type triangle_type;
  
  
//   typedef int segment_id_type;
//   
//   typedef std::deque< viennagrid::segment_info_t< viennagrid::element_segment_mapping<segment_id_type, bool> > > segment_info_container_type;
//   typedef viennameta::make_typemap< triangle_type, segment_info_container_type >::type typemap;
  
  typedef viennagrid::result_of::oriented_3d_hull_segmentation<domain_type>::type segmentation_type;
//   typedef viennagrid::result_of::segmentation<domain_type>::type segmentation_type;
  
  typedef segmentation_type::segment_type segment_type;
  typedef segmentation_type::segment_id_type segment_id_type;
  
  domain_type domain;
  segmentation_type segmentation(domain);
  
  
  vertex_handle_type vh0 = viennagrid::create_vertex( domain );
  vertex_handle_type vh1 = viennagrid::create_vertex( domain );
  vertex_handle_type vh2 = viennagrid::create_vertex( domain );
  vertex_handle_type vh3 = viennagrid::create_vertex( domain );
  vertex_handle_type vh4 = viennagrid::create_vertex( domain );
  vertex_handle_type vh5 = viennagrid::create_vertex( domain );
  
  triangle_handle_type th0 = viennagrid::create_triangle( domain, vh0, vh1, vh2 );
  triangle_handle_type th1 = viennagrid::create_triangle( domain, vh1, vh2, vh3 );
  triangle_handle_type th2 = viennagrid::create_triangle( domain, vh3, vh4, vh5 );
  
  

  
  segment_type seg0 = segmentation.create_segment();
  segment_type seg1 = segmentation.create_segment( segment_id_type(4) );
  segment_type seg2 = segmentation.create_segment();
  
  for ( segmentation_type::iterator it = segmentation.begin(); it != segmentation.end(); ++it)
    std::cout << (*it).id() << std::endl;
  
  
  
  typedef std::deque< viennagrid::segment_info_t< viennagrid::element_segment_mapping<segment_id_type, bool> > > segment_info_container_type;
  segment_info_container_type container;
  
  viennagrid::accessor::dense_container_accessor_t< segment_info_container_type, triangle_type> acc(container);
  
  triangle_type & tri0 = viennagrid::dereference_handle(domain, th0);
  triangle_type & tri1 = viennagrid::dereference_handle(domain, th1);
  triangle_type & tri2 = viennagrid::dereference_handle(domain, th2);
  
  viennagrid::add_to_segment( seg0, tri0 );
  std::cout << viennagrid::is_in_segment( seg0, tri0 ) << std::endl;
  viennagrid::erase_from_segment( seg0, tri0 );
  std::cout << viennagrid::is_in_segment( seg0, tri0 ) << std::endl;
  
  
  viennagrid::add_to_segment( seg0, tri0 );
  viennagrid::add_to_segment( seg1, tri1 );
  viennagrid::add_to_segment( seg2, tri2 );
  
  std::cout << *viennagrid::element_segment_info( seg0, tri0 ) << std::endl;
  *viennagrid::element_segment_info( seg0, tri0 ) = true;
  std::cout << *viennagrid::element_segment_info( seg0, tri0 ) << std::endl;
  
  
  typedef viennagrid::result_of::element_range< segment_type, viennagrid::triangle_tag >::type range_type;\
  
  std::cout << "Triangles of Segment 0" << std::endl;
  range_type range = viennagrid::elements( seg0 );
  for (range_type::iterator it = range.begin(); it != range.end(); ++it)
    std::cout << *it << std::endl;
  
  std::cout << "Triangles of Segment 1" << std::endl;
  range = viennagrid::elements( seg1 );
  for (range_type::iterator it = range.begin(); it != range.end(); ++it)
    std::cout << *it << std::endl;
  
  std::cout << "Triangles of Segment 2" << std::endl;
  range = viennagrid::elements( seg2 );
  for (range_type::iterator it = range.begin(); it != range.end(); ++it)
    std::cout << *it << std::endl;
  
  
  
//   typedef std::deque< std::pair<segment_id_type, bool> > segment_info;
//   segment_info & segment_information = viennagrid::segment_information( segmentation, viennagrid::dereference_handle(domain, th0) );
  
  
//     {
//         typedef viennagrid::config::tetrahedral_3d config;
//         typedef viennagrid::result_of::domain<config>::type domain_type;
//         domain_type domain;
//         
//         typedef viennagrid::result_of::element_segment_info<domain_type, viennagrid::tetrahedron_tag>::type element_segment_info;
//         typedef viennagrid::result_of::segmentation<domain_type, viennagrid::tetrahedron_tag>::type segmentation_type;
//         segmentation_type segmentation;
//         
//         typedef viennagrid::result_of::point_type<domain_type>::type point_type;
//         typedef viennagrid::result_of::vertex_handle<domain_type>::type vertex_handle_type;
//         typedef viennagrid::result_of::tetrahedron_handle<domain_type>::type tetrahedron_handle_type;
//         
//         vertex_handle_type v0 = viennagrid::create_vertex(domain, point_type(0.0, 0.0, 0.0));
//         vertex_handle_type v1 = viennagrid::create_vertex(domain, point_type(1.0, 0.0, 0.0));
//         vertex_handle_type v2 = viennagrid::create_vertex(domain, point_type(0.0, 1.0, 0.0));
//         vertex_handle_type v3 = viennagrid::create_vertex(domain, point_type(0.0, 0.0, 1.0));
//         
//         tetrahedron_handle_type tet = viennagrid::create_tetrahedron(domain, v0, v1, v2, v3);
//         
//         segmentation.set_segment_info( viennagrid::dereference_handle(domain, tet), element_segment_info(1) );
//         
//         std::cout << segmentation.segment_info( viennagrid::dereference_handle(domain, tet) ).segment_id << std::endl;
//     }
// 
//     
//     {
//         typedef viennagrid::config::triangular_3d config;
//         typedef viennagrid::result_of::domain<config>::type domain_type;
//         domain_type domain;
//         
//         typedef viennagrid::result_of::element_segment_info<domain_type, viennagrid::triangle_tag>::type element_segment_info;
//         typedef viennagrid::result_of::segmentation<domain_type, viennagrid::triangle_tag>::type segmentation_type;
//         segmentation_type segmentation;
//         
//         typedef viennagrid::result_of::point_type<domain_type>::type point_type;
//         typedef viennagrid::result_of::vertex_handle<domain_type>::type vertex_handle_type;
//         typedef viennagrid::result_of::triangle_handle<domain_type>::type triangle_handle_type;
//         
//         vertex_handle_type v0 = viennagrid::create_vertex(domain, point_type(0.0, 0.0, 0.0));
//         vertex_handle_type v1 = viennagrid::create_vertex(domain, point_type(1.0, 0.0, 0.0));
//         vertex_handle_type v2 = viennagrid::create_vertex(domain, point_type(0.0, 1.0, 0.0));
//         
//         triangle_handle_type tri = viennagrid::create_triangle(domain, v0, v1, v2);
//         
//         segmentation.set_segment_info( viennagrid::dereference_handle(domain, tri), element_segment_info(1,2) );
//         
//         std::cout << segmentation.segment_info( viennagrid::dereference_handle(domain, tri) ).positive_orientation_segment_id << std::endl;
//         std::cout << segmentation.segment_info( viennagrid::dereference_handle(domain, tri) ).negative_orientation_segment_id << std::endl;
//     }
    
    return 0;
}