#include <iostream>
#include <typeinfo>

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/mesh/accessor.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/mesh/element_creation.hpp"



int main()
{
  typedef viennagrid::triangular_3d_mesh                        MeshType;

  typedef viennagrid::result_of::vertex_handle< MeshType >::type     VertexHandleType;
  typedef viennagrid::result_of::vertex< MeshType >::type            VertexType;
  typedef viennagrid::result_of::triangle_handle< MeshType >::type   TriangleHandleType;
  typedef viennagrid::result_of::triangle< MeshType >::type          TriangleType;


  // triangular hull segmentation, each triangular face has a segment for its positive orientation and its negative orientation
  typedef viennagrid::result_of::oriented_3d_hull_segmentation<MeshType>::type SegmentationType;

  // defining the segment type and segment id type
  typedef viennagrid::result_of::segment_handle<SegmentationType>::type SegmentHandleType;
  typedef viennagrid::result_of::segment_id<SegmentationType>::type SegmentIDType;


  MeshType mesh;
  SegmentationType segmentation(mesh);

  // create some vertices
  VertexHandleType vh0 = viennagrid::make_vertex( mesh );
  VertexHandleType vh1 = viennagrid::make_vertex( mesh );
  VertexHandleType vh2 = viennagrid::make_vertex( mesh );
  VertexHandleType vh3 = viennagrid::make_vertex( mesh );
  VertexHandleType vh4 = viennagrid::make_vertex( mesh );
  VertexHandleType vh5 = viennagrid::make_vertex( mesh );

  // create some triangles
  TriangleHandleType th0 = viennagrid::make_triangle( mesh, vh0, vh1, vh2 );
  TriangleHandleType th1 = viennagrid::make_triangle( mesh, vh1, vh2, vh3 );
  TriangleHandleType th2 = viennagrid::make_triangle( mesh, vh3, vh4, vh5 );
  /*TriangleHandleType th3 =*/ viennagrid::make_triangle( mesh, vh0, vh4, vh5 );



  // create a segment, segment id will be 0 because of first segment
  SegmentHandleType seg0 = segmentation.make_segment();

  // create a segment, segment id will be 4 because explicit segment_id
  SegmentHandleType seg1 = segmentation[ SegmentIDType(4) ];

  // create a segment, segment id will be 5 because highest segment id was 4
  SegmentHandleType seg2 = segmentation.make_segment();

  // create some other segments (ID would be 6, 7, 8)
  SegmentHandleType seg3 = segmentation.make_segment();
  SegmentHandleType seg4 = segmentation.make_segment();
  SegmentHandleType seg5 = segmentation.make_segment();

  // print the segments
  std::cout << "Segments in Segmentation" << std::endl;
  for ( SegmentationType::iterator it = segmentation.begin(); it != segmentation.end(); ++it)
    std::cout << (*it).id() << std::endl;
  std::cout << std::endl;


  TriangleType & tri0 = viennagrid::dereference_handle(mesh, th0);
  TriangleType & tri1 = viennagrid::dereference_handle(mesh, th1);
  TriangleType & tri2 = viennagrid::dereference_handle(mesh, th2);

  VertexType & v0 = viennagrid::dereference_handle(mesh, vh0);

  // add triangle tr0 to seg0
  std::cout << "Adding element tri0 to seg0" << std::endl;
  viennagrid::add( seg0, tri0 );

  std::cout << "Adding element tri0 to seg1" << std::endl;
  viennagrid::add( seg1, tri0 );

  std::cout << "Triangle 0 in Segment 0: " << viennagrid::is_in_segment( seg0, tri0 ) << std::endl;
  std::cout << "Vertex 0 in Segment 0: " << viennagrid::is_in_segment( seg0, v0 ) << std::endl;
  std::cout << "Numer of cells in segmentation: " << viennagrid::cells( segmentation ).size() << std::endl;

  std::cout << "Erasing element tri0 from seg0" << std::endl;
  // erase triangle tr0 from seg0
  viennagrid::erase( seg0, tri0 );
  std::cout << "Triangle 0 in Segment 0: " <<  viennagrid::is_in_segment( seg0, tri0 ) << std::endl;
  std::cout << "Vertex 0 in Segment 0: " << viennagrid::is_in_segment( seg0, v0 ) << std::endl;
  std::cout << "Numer of cells in segmentation: " << viennagrid::cells( segmentation ).size() << std::endl;

  std::cout << "Erasing element tri0 from seg1" << std::endl;
  // erase triangle tr0 from seg0
  viennagrid::erase( seg1, tri0 );
  std::cout << "Triangle 0 in Segment 0: " <<  viennagrid::is_in_segment( seg0, tri0 ) << std::endl;
  std::cout << "Vertex 0 in Segment 0: " << viennagrid::is_in_segment( seg0, v0 ) << std::endl;
  std::cout << "Numer of cells in segmentation: " << viennagrid::cells( segmentation ).size() << std::endl;


  // add triangle tr0 to seg0, tri1 to seg1, tri2 to seg2
  viennagrid::add( seg0, tri0 );
  viennagrid::add( seg1, tri1 );
  viennagrid::add( seg2, tri2 );


  viennagrid::add( seg3, tri0 );
  viennagrid::add( seg4, tri0 );
  viennagrid::add( seg5, tri0 );


  // print all cells of the mesh (should be four)
  typedef viennagrid::result_of::cell_range<MeshType>::type MeshCellRange;
  typedef viennagrid::result_of::iterator<MeshCellRange>::type MeshCellIterator;

  std::cout << std::endl;
  std::cout << "All cells in the mesh:" << std::endl;
  MeshCellRange mesh_cells = viennagrid::elements( mesh );
  for (MeshCellIterator cit = mesh_cells.begin(); cit != mesh_cells.end(); ++cit)
    std::cout << *cit << std::endl;
  std::cout << std::endl;

  // print all cells of the segmentation (should be three)
  typedef viennagrid::result_of::cell_range<SegmentationType>::type SegmentationCellRange;
  typedef viennagrid::result_of::iterator<SegmentationCellRange>::type SegmentationCellIterator;

  std::cout << std::endl;
  std::cout << "All cells in the segmentation:" << std::endl;
  SegmentationCellRange segmentation_cells = viennagrid::elements( segmentation );
  for (SegmentationCellIterator cit = segmentation_cells.begin(); cit != segmentation_cells.end(); ++cit)
    std::cout << *cit << std::endl;
  std::cout << std::endl;





  typedef viennagrid::result_of::segment_id_range<SegmentationType, TriangleType>::type SegmentIDRange;
  SegmentIDRange segment_ids = viennagrid::segment_ids( segmentation, tri0 );
  std::cout << "Segments for tr0:" << std::endl;
  for (SegmentIDRange::iterator it = segment_ids.begin(); it != segment_ids.end(); ++it)
    std::cout << *it << std::endl;


  // setting and querying additional segment information for tri0
  std::cout << "Triangle 0 in Segment 0: " <<  *viennagrid::segment_element_info( seg0, tri0 ) << std::endl;
  *viennagrid::segment_element_info( seg0, tri0 ) = true;
  std::cout << "Triangle 0 in Segment 0: " <<  *viennagrid::segment_element_info( seg0, tri0 ) << std::endl;
  std::cout << std::endl;

  // printing all triangles from all segments
  typedef viennagrid::result_of::element_range< SegmentHandleType, viennagrid::triangle_tag >::type range_type;

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

  std::cout << std::endl;

  // Adding additional triangles to seg2 and printing them
  viennagrid::make_triangle( seg2, vh1, vh4, vh5 );
  viennagrid::make_triangle( seg2, vh2, vh4, vh5 );

  std::cout << "Triangles of Segment 2, added 2 additional" << std::endl;
  range = viennagrid::elements( seg2 );
  for (range_type::iterator it = range.begin(); it != range.end(); ++it)
    std::cout << *it << std::endl;
  std::cout << std::endl;

  // Printing vertices from seg2, each vertex should only be printed once
  typedef viennagrid::result_of::vertex_range<SegmentHandleType>::type vertex_range;
  std::cout << "Vertices of Segment 2" << std::endl;
  vertex_range vertices = viennagrid::elements( seg2 );
  for (vertex_range::iterator it = vertices.begin(); it != vertices.end(); ++it)
    std::cout << *it << std::endl;

    return 0;
}
