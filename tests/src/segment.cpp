/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#ifdef _MSC_VER
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

//***********************************************
// Define the input-file format
//***********************************************

#include "viennagrid/forwards.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/mesh/element_creation.hpp"
#include "viennagrid/io/vtk_writer.hpp"


template <typename ElementTagT,
          typename SegmentT>
void print_elements(SegmentT & seg)
{
  SegmentT const & const_seg = seg;

  std::cout << "-- non-const --" << std::endl;
  typedef typename viennagrid::result_of::element_range<SegmentT, ElementTagT>::type  ContainerT;
  typedef typename viennagrid::result_of::iterator<ContainerT>::type           ContainerTIterator;

  ContainerT elements(seg);
  for (ContainerTIterator it = elements.begin();
       it != elements.end();
       ++it)
  {
    std::cout << *it << std::endl;
    //it->print_short();
  }

  std::cout << "-- const --" << std::endl;
  typedef typename viennagrid::result_of::const_element_range<SegmentT, ElementTagT>::type   ConstContainerT;
  typedef typename viennagrid::result_of::iterator<ConstContainerT>::type             ConstContainerTIterator;

  ConstContainerT const_elements(const_seg);
  for (ConstContainerTIterator const_it = const_elements.begin();
       const_it != const_elements.end();
       ++const_it)
  {
    std::cout << *const_it << std::endl;
    //const_it->print_short();
  }
}


//test for 3d tetrahedral case:
void test(viennagrid::tetrahedral_3d_mesh)
{
  typedef viennagrid::tetrahedral_3d_mesh                         MeshType;
  typedef viennagrid::result_of::cell_tag<MeshType>::type         CellTag;
  typedef viennagrid::result_of::segmentation<MeshType>::type     SegmentationType;
  typedef viennagrid::result_of::segment_handle<SegmentationType>::type    SegmentHandleType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::vertex_handle<MeshType>::type    VertexHandleType;

  MeshType mesh;
  SegmentationType segmentation(mesh);

  PointType p[7];

  p[0] = PointType(0.0, 0.0, 0.0);
  p[1] = PointType(0.5, 0.0, 0.0);
  p[2] = PointType(1.0, 0.0, 0.0);
  p[3] = PointType(0.0, 1.0, 0.0);
  p[4] = PointType(0.5, 1.0, 0.0);
  p[5] = PointType(1.0, 1.0, 0.0);
  p[6] = PointType(0.5, 0.0, 1.0);

  //upgrade to vertex:
  VertexHandleType vh[7];

  std::cout << "Adding vertices to mesh..." << std::endl;
  for (int i = 0; i < 7; ++i)
    vh[i] = viennagrid::make_vertex( mesh, p[i] );


  std::cout << "Creating segments..." << std::endl;
  SegmentHandleType seg0 = segmentation.make_segment();
  SegmentHandleType seg1 = segmentation.make_segment();

  std::cout << "Adding cells to segments..." << std::endl;

  std::cout << "Filling segment 0..." << std::endl;
  viennagrid::make_tetrahedron( seg0, vh[0], vh[1], vh[6], vh[3] );
  viennagrid::make_tetrahedron( seg0, vh[1], vh[6], vh[3], vh[4] );


  std::cout << "Filling segment 1..." << std::endl;
  viennagrid::make_tetrahedron( seg1, vh[1], vh[2], vh[6], vh[4] );
  viennagrid::make_tetrahedron( seg1, vh[2], vh[6], vh[4], vh[5] );


  std::cout << "Vertices in Segment 0: " << viennagrid::vertices(seg0).size() << std::endl;
  std::cout << "Vertices in Segment 1: " << viennagrid::vertices(seg1).size() << std::endl;
  std::cout << "Edges in Segment 0: "    << viennagrid::lines(seg0).size() << std::endl;
  std::cout << "Edges in Segment 1: "    << viennagrid::lines(seg1).size() << std::endl;
  std::cout << "Cells in Segment 0: "    << viennagrid::cells(seg0).size() << std::endl;
  std::cout << "Cells in Segment 1: "    << viennagrid::cells(seg1).size() << std::endl;

  std::cout << "Printing vertices in segment 0:" << std::endl;
  print_elements<viennagrid::vertex_tag>(seg0);

  std::cout << "Printing vertices in segment 1:" << std::endl;
  print_elements<viennagrid::vertex_tag>(seg1);

  std::cout << "Printing cells in segment 0:" << std::endl;
  print_elements<CellTag>(seg0);

  std::cout << "Printing cells in segment 1:" << std::endl;
  print_elements<CellTag>(seg1);

  std::cout << "Test for direct operator[] access: " << std::endl;
  //viennagrid::ncells<0>(mesh.segments()[0])[0].print_short();
  std::cout << viennagrid::cells(seg0)[0] << std::endl;

  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, segmentation, "multi_segment_handle_tet");
}

//test for 3d hexahedral case:
void test(viennagrid::hexahedral_3d_mesh)
{
  typedef viennagrid::hexahedral_3d_mesh                         MeshType;
  typedef viennagrid::result_of::cell_tag<MeshType>::type         CellTag;
  typedef viennagrid::result_of::segmentation<MeshType>::type     SegmentationType;
  typedef viennagrid::result_of::segment_handle<SegmentationType>::type    SegmentHandleType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::vertex_handle<MeshType>::type    VertexHandleType;

  MeshType mesh;
  SegmentationType segmentation(mesh);

  PointType p[12];

  p[0] = PointType(0.0, 0.0, 0.0);
  p[1] = PointType(0.5, 0.0, 0.0);
  p[2] = PointType(1.0, 0.0, 0.0);
  p[3] = PointType(1.0, 1.0, 0.0);
  p[4] = PointType(0.5, 1.0, 0.0);
  p[5] = PointType(0.0, 1.0, 0.0);
  p[6] = PointType(0.0, 0.0, 1.0);
  p[7] = PointType(0.5, 0.0, 1.0);
  p[8] = PointType(1.0, 0.0, 1.0);
  p[9] = PointType(1.0, 1.0, 1.0);
  p[10] = PointType(0.5, 1.0, 1.0);
  p[11] = PointType(0.0, 1.0, 1.0);

  //upgrade to vertex:
  VertexHandleType vh[12];

  std::cout << "Adding vertices to mesh..." << std::endl;
  for (int i = 0; i < 12; ++i)
    vh[i] = viennagrid::make_vertex( mesh, p[i] );

  std::cout << "Creating segments..." << std::endl;
  SegmentHandleType seg0 = segmentation.make_segment();
  SegmentHandleType seg1 = segmentation.make_segment();

  std::cout << "Adding cells to segments..." << std::endl;

  std::cout << "Filling segment 0..." << std::endl;
  viennagrid::make_hexahedron( seg0, vh[0], vh[1], vh[4], vh[5], vh[6], vh[7], vh[10], vh[11] );

  std::cout << "Filling segment 1..." << std::endl;
  viennagrid::make_hexahedron( seg1, vh[1], vh[2], vh[3], vh[4], vh[7], vh[8], vh[9], vh[10] );


  std::cout << "Vertices in Segment 0: " << viennagrid::vertices(seg0).size() << std::endl;
  std::cout << "Vertices in Segment 1: " << viennagrid::vertices(seg1).size() << std::endl;
  std::cout << "Edges in Segment 0: "    << viennagrid::lines(seg0).size() << std::endl;
  std::cout << "Edges in Segment 1: "    << viennagrid::lines(seg1).size() << std::endl;
  std::cout << "Cells in Segment 0: "    << viennagrid::cells(seg0).size() << std::endl;
  std::cout << "Cells in Segment 1: "    << viennagrid::cells(seg1).size() << std::endl;

  std::cout << "Printing vertices in segment 0:" << std::endl;
  print_elements<viennagrid::vertex_tag>(seg0);

  std::cout << "Printing vertices in segment 1:" << std::endl;
  print_elements<viennagrid::vertex_tag>(seg1);

  std::cout << "Printing cells in segment 0:" << std::endl;
  print_elements<CellTag>(seg0);

  std::cout << "Printing cells in segment 1:" << std::endl;
  print_elements<CellTag>(seg1);

  std::cout << "Test for direct operator[] access: " << std::endl;
  //viennagrid::ncells<0>(mesh.segments()[0])[0].print_short();
  std::cout << viennagrid::cells(seg0)[0] << std::endl;

  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, segmentation, "multi_segment_hex");
}


//test for 2d triangular case:
void test(viennagrid::triangular_2d_mesh)
{
  typedef viennagrid::triangular_2d_mesh                         MeshType;
  typedef viennagrid::result_of::cell_tag<MeshType>::type         CellTag;
  typedef viennagrid::result_of::segmentation<MeshType>::type     SegmentationType;
  typedef viennagrid::result_of::segment_handle<SegmentationType>::type    SegmentHandleType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::vertex_handle<MeshType>::type    VertexHandleType;

  MeshType mesh;
  SegmentationType segmentation(mesh);

  PointType p[6];

  p[0] = PointType(0.0, 0.0);
  p[1] = PointType(0.5, 0.0);
  p[2] = PointType(1.0, 0.0);
  p[3] = PointType(0.0, 1.0);
  p[4] = PointType(0.5, 1.0);
  p[5] = PointType(1.0, 1.0);

  //upgrade to vertex:
  VertexHandleType vh[6];

  std::cout << "Adding vertices to mesh..." << std::endl;
  for (int i = 0; i < 12; ++i)
    vh[i] = viennagrid::make_vertex( mesh, p[i] );

  std::cout << "Creating segments..." << std::endl;
  SegmentHandleType seg0 = segmentation.make_segment();
  SegmentHandleType seg1 = segmentation.make_segment();

  std::cout << "Adding cells to segments..." << std::endl;

  std::cout << "Filling segment 0..." << std::endl;
  viennagrid::make_triangle( seg0, vh[0], vh[1], vh[3] );
  viennagrid::make_triangle( seg0, vh[1], vh[4], vh[3] );

  std::cout << "Filling segment 1..." << std::endl;
  viennagrid::make_triangle( seg1, vh[1], vh[2], vh[4] );
  viennagrid::make_triangle( seg1, vh[2], vh[5], vh[4] );



  std::cout << "Vertices in Segment 0: " << viennagrid::vertices(seg0).size() << std::endl;
  std::cout << "Vertices in Segment 1: " << viennagrid::vertices(seg1).size() << std::endl;
  std::cout << "Edges in Segment 0: "    << viennagrid::lines(seg0).size() << std::endl;
  std::cout << "Edges in Segment 1: "    << viennagrid::lines(seg1).size() << std::endl;
  std::cout << "Cells in Segment 0: "    << viennagrid::cells(seg0).size() << std::endl;
  std::cout << "Cells in Segment 1: "    << viennagrid::cells(seg1).size() << std::endl;

  std::cout << "Printing vertices in segment 0:" << std::endl;
  print_elements<viennagrid::vertex_tag>(seg0);

  std::cout << "Printing vertices in segment 1:" << std::endl;
  print_elements<viennagrid::vertex_tag>(seg1);

  std::cout << "Printing cells in segment 0:" << std::endl;
  print_elements<CellTag>(seg0);

  std::cout << "Printing cells in segment 1:" << std::endl;
  print_elements<CellTag>(seg1);

  std::cout << "Test for direct operator[] access: " << std::endl;
  //viennagrid::ncells<0>(mesh.segments()[0])[0].print_short();
  std::cout << viennagrid::cells(seg0)[0] << std::endl;

  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, segmentation, "multi_segment_handle_tri");
}

//test for 2d quadrilateral case:
void test(viennagrid::quadrilateral_2d_mesh)
{
  typedef viennagrid::quadrilateral_2d_mesh                         MeshType;
  typedef viennagrid::result_of::cell_tag<MeshType>::type         CellTag;
  typedef viennagrid::result_of::segmentation<MeshType>::type     SegmentationType;
  typedef viennagrid::result_of::segment_handle<SegmentationType>::type    SegmentHandleType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::vertex_handle<MeshType>::type    VertexHandleType;

  MeshType mesh;
  SegmentationType segmentation(mesh);

  PointType p[6];

  p[0] = PointType(0.0, 0.0);
  p[1] = PointType(0.5, 0.0);
  p[2] = PointType(1.0, 0.0);
  p[3] = PointType(0.0, 1.0);
  p[4] = PointType(0.5, 1.0);
  p[5] = PointType(1.0, 1.0);

  //upgrade to vertex:
  VertexHandleType vh[6];

  std::cout << "Adding vertices to mesh..." << std::endl;
  for (int i = 0; i < 12; ++i)
    vh[i] = viennagrid::make_vertex( mesh, p[i] );

  std::cout << "Creating segments..." << std::endl;
  SegmentHandleType seg0 = segmentation.make_segment();
  SegmentHandleType seg1 = segmentation.make_segment();

  std::cout << "Adding cells to segments..." << std::endl;


  std::cout << "Filling segment 0..." << std::endl;
  viennagrid::make_quadrilateral( seg0, vh[0], vh[1], vh[4], vh[5] );

  std::cout << "Filling segment 1..." << std::endl;
  viennagrid::make_quadrilateral( seg1, vh[1], vh[2], vh[3], vh[4] );



  std::cout << "Vertices in Segment 0: " << viennagrid::vertices(seg0).size() << std::endl;
  std::cout << "Vertices in Segment 1: " << viennagrid::vertices(seg1).size() << std::endl;
  std::cout << "Edges in Segment 0: "    << viennagrid::lines(seg0).size() << std::endl;
  std::cout << "Edges in Segment 1: "    << viennagrid::lines(seg1).size() << std::endl;
  std::cout << "Cells in Segment 0: "    << viennagrid::cells(seg0).size() << std::endl;
  std::cout << "Cells in Segment 1: "    << viennagrid::cells(seg1).size() << std::endl;

  std::cout << "Printing vertices in segment 0:" << std::endl;
  print_elements<viennagrid::vertex_tag>(seg0);

  std::cout << "Printing vertices in segment 1:" << std::endl;
  print_elements<viennagrid::vertex_tag>(seg1);

  std::cout << "Printing cells in segment 0:" << std::endl;
  print_elements<CellTag>(seg0);

  std::cout << "Printing cells in segment 1:" << std::endl;
  print_elements<CellTag>(seg1);

  std::cout << "Test for direct operator[] access: " << std::endl;
  //viennagrid::ncells<0>(mesh.segments()[0])[0].print_short();
  std::cout << viennagrid::cells(seg0)[0] << std::endl;

  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, segmentation, "multi_segment_quad");
}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "--- Testing with triangular_2d ---" << std::endl;
  test(viennagrid::triangular_2d_mesh());
  std::cout << "--- Testing with tetrahedral_3d ---" << std::endl;
  test(viennagrid::tetrahedral_3d_mesh());

  std::cout << "--- Testing with quadrilateral_2d ---" << std::endl;
  test(viennagrid::quadrilateral_2d_mesh());
  std::cout << "--- Testing with hexahedral_3d ---" << std::endl;
  test(viennagrid::hexahedral_3d_mesh());

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
