/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
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

#include "viennagrid/core.hpp"
#include "viennagrid/io/vtk_writer.hpp"


template <typename ElementTagT,
          typename RegionT>
void print_elements(RegionT & region)
{
  typedef typename viennagrid::result_of::mesh<RegionT>::type   MeshType;
//   typedef typename viennagrid::result_of::mesh<SegmentationType>::type   MeshType2;  //to check that one can deduce {mesh type} <-> {segmentation type}
//   typedef typename viennagrid::result_of::mesh<RegionT>::type           MeshType3;  //to check that one can deduce {mesh type} <-> {segment handle}

  MeshType mesh2 = region.mesh(); (void)mesh2;
  MeshType mesh3 = region.mesh(); (void)mesh3;
  RegionT const_seg = region;

  std::cout << "-- non-const --" << std::endl;
  typedef typename viennagrid::result_of::element_range<RegionT, ElementTagT>::type  ContainerT;
  typedef typename viennagrid::result_of::iterator<ContainerT>::type           ContainerTIterator;

  ContainerT elements(region);
  for (ContainerTIterator it = elements.begin();
       it != elements.end();
       ++it)
  {
    std::cout << *it << std::endl;
    //it->print_short();
  }

  std::cout << "-- const --" << std::endl;
  typedef typename viennagrid::result_of::const_element_range<RegionT, ElementTagT>::type   ConstContainerT;
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
inline void test_tetrahedral3d()
{
  typedef viennagrid::mesh_t                         MeshType;
  typedef viennagrid::result_of::cell_tag<MeshType>::type         CellTag;
  typedef viennagrid::result_of::region<MeshType>::type    RegionType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::vertex<MeshType>::type    VertexType;

  MeshType mesh(3, viennagrid::tetrahedron_tag());

  PointType p[7];

  p[0] = viennagrid::make_point(0.0, 0.0, 0.0);
  p[1] = viennagrid::make_point(0.5, 0.0, 0.0);
  p[2] = viennagrid::make_point(1.0, 0.0, 0.0);
  p[3] = viennagrid::make_point(0.0, 1.0, 0.0);
  p[4] = viennagrid::make_point(0.5, 1.0, 0.0);
  p[5] = viennagrid::make_point(1.0, 1.0, 0.0);
  p[6] = viennagrid::make_point(0.5, 0.0, 1.0);

  //upgrade to vertex:
  VertexType vh[7];

  std::cout << "Adding vertices to mesh..." << std::endl;
  for (int i = 0; i < 7; ++i)
    vh[i] = viennagrid::make_vertex( mesh, p[i] );


  std::cout << "Creating segments..." << std::endl;
  RegionType region0 = mesh.make_region();
  RegionType region1 = mesh.make_region();

  std::cout << "Adding cells to segments..." << std::endl;

  std::cout << "Filling segment 0..." << std::endl;
  viennagrid::make_tetrahedron( region0, vh[0], vh[1], vh[6], vh[3] );
  viennagrid::make_tetrahedron( region0, vh[1], vh[6], vh[3], vh[4] );


  std::cout << "Filling segment 1..." << std::endl;
  viennagrid::make_tetrahedron( region1, vh[1], vh[2], vh[6], vh[4] );
  viennagrid::make_tetrahedron( region1, vh[2], vh[6], vh[4], vh[5] );


  std::cout << "Vertices in Segment 0: " << viennagrid::vertices(region0).size() << std::endl;
  std::cout << "Vertices in Segment 1: " << viennagrid::vertices(region1).size() << std::endl;
  std::cout << "Edges in Segment 0: "    << viennagrid::lines(region0).size() << std::endl;
  std::cout << "Edges in Segment 1: "    << viennagrid::lines(region1).size() << std::endl;
  std::cout << "Cells in Segment 0: "    << viennagrid::cells(region0).size() << std::endl;
  std::cout << "Cells in Segment 1: "    << viennagrid::cells(region1).size() << std::endl;

  std::cout << "Printing vertices in segment 0:" << std::endl;
  print_elements<viennagrid::vertex_tag>(region0);

  std::cout << "Printing vertices in segment 1:" << std::endl;
  print_elements<viennagrid::vertex_tag>(region1);

  std::cout << "Printing cells in segment 0:" << std::endl;
  print_elements<CellTag>(region0);

  std::cout << "Printing cells in segment 1:" << std::endl;
  print_elements<CellTag>(region1);

  std::cout << "Test for direct operator[] access: " << std::endl;
  //viennagrid::ncells<0>(mesh.segments()[0])[0].print_short();
  std::cout << viennagrid::cells(region0)[0] << std::endl;

  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, "multi_segment_handle_tet");
}

//test for 3d hexahedral case:
inline void test_hexahedral3d()
{
  typedef viennagrid::mesh_t                         MeshType;
  typedef viennagrid::result_of::cell_tag<MeshType>::type         CellTag;
  typedef viennagrid::result_of::region<MeshType>::type    RegionType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::vertex<MeshType>::type    VertexType;

  MeshType mesh(3, viennagrid::hexahedron_tag());

  PointType p[12];

  p[0] = viennagrid::make_point(0.0, 0.0, 0.0);
  p[1] = viennagrid::make_point(0.5, 0.0, 0.0);
  p[2] = viennagrid::make_point(1.0, 0.0, 0.0);
  p[3] = viennagrid::make_point(1.0, 1.0, 0.0);
  p[4] = viennagrid::make_point(0.5, 1.0, 0.0);
  p[5] = viennagrid::make_point(0.0, 1.0, 0.0);
  p[6] = viennagrid::make_point(0.0, 0.0, 1.0);
  p[7] = viennagrid::make_point(0.5, 0.0, 1.0);
  p[8] = viennagrid::make_point(1.0, 0.0, 1.0);
  p[9] = viennagrid::make_point(1.0, 1.0, 1.0);
  p[10] = viennagrid::make_point(0.5, 1.0, 1.0);
  p[11] = viennagrid::make_point(0.0, 1.0, 1.0);

  //upgrade to vertex:
  VertexType vh[12];

  std::cout << "Adding vertices to mesh..." << std::endl;
  for (int i = 0; i < 12; ++i)
    vh[i] = viennagrid::make_vertex( mesh, p[i] );

  std::cout << "Creating segments..." << std::endl;
  RegionType seg0 = mesh.make_region();
  RegionType seg1 = mesh.make_region();

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
  my_vtk_writer(mesh, "multi_segment_hex");
}


//test for 2d triangular case:
inline void test_triangle2d()
{
  typedef viennagrid::mesh_t                         MeshType;
  typedef viennagrid::result_of::cell_tag<MeshType>::type         CellTag;
  typedef viennagrid::result_of::region<MeshType>::type    RegionType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::vertex<MeshType>::type    VertexType;

  MeshType mesh(2, viennagrid::triangle_tag());

  PointType p[6];

  p[0] = viennagrid::make_point(0.0, 0.0);
  p[1] = viennagrid::make_point(0.5, 0.0);
  p[2] = viennagrid::make_point(1.0, 0.0);
  p[3] = viennagrid::make_point(0.0, 1.0);
  p[4] = viennagrid::make_point(0.5, 1.0);
  p[5] = viennagrid::make_point(1.0, 1.0);

  //upgrade to vertex:
  VertexType vh[6];

  std::cout << "Adding vertices to mesh..." << std::endl;
  for (int i = 0; i < 6; ++i)
    vh[i] = viennagrid::make_vertex( mesh, p[i] );

  std::cout << "Creating segments..." << std::endl;
  RegionType region0 = mesh.make_region();
  RegionType region1 = mesh.make_region();

  std::cout << "Adding cells to segments..." << std::endl;

  std::cout << "Filling segment 0..." << std::endl;
  viennagrid::make_triangle( region0, vh[0], vh[1], vh[3] );
  viennagrid::make_triangle( region0, vh[1], vh[4], vh[3] );

  std::cout << "Filling segment 1..." << std::endl;
  viennagrid::make_triangle( region1, vh[1], vh[2], vh[4] );
  viennagrid::make_triangle( region1, vh[2], vh[5], vh[4] );



  std::cout << "Vertices in Segment 0: " << viennagrid::vertices(region0).size() << std::endl;
  std::cout << "Vertices in Segment 1: " << viennagrid::vertices(region1).size() << std::endl;
  std::cout << "Edges in Segment 0: "    << viennagrid::lines(region0).size() << std::endl;
  std::cout << "Edges in Segment 1: "    << viennagrid::lines(region1).size() << std::endl;
  std::cout << "Cells in Segment 0: "    << viennagrid::cells(region0).size() << std::endl;
  std::cout << "Cells in Segment 1: "    << viennagrid::cells(region1).size() << std::endl;

  std::cout << "Printing vertices in segment 0:" << std::endl;
  print_elements<viennagrid::vertex_tag>(region0);

  std::cout << "Printing vertices in segment 1:" << std::endl;
  print_elements<viennagrid::vertex_tag>(region1);

  std::cout << "Printing cells in segment 0:" << std::endl;
  print_elements<CellTag>(region0);

  std::cout << "Printing cells in segment 1:" << std::endl;
  print_elements<CellTag>(region1);

  std::cout << "Test for direct operator[] access: " << std::endl;
  //viennagrid::ncells<0>(mesh.segments()[0])[0].print_short();
  std::cout << viennagrid::cells(region0)[0] << std::endl;

  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, "multi_segment_handle_tri");
}

//test for 2d quadrilateral case:
inline void test_quadrilateral2d()
{
  typedef viennagrid::mesh_t                         MeshType;
  typedef viennagrid::result_of::cell_tag<MeshType>::type         CellTag;
  typedef viennagrid::result_of::region<MeshType>::type    RegionType;

  typedef viennagrid::result_of::point<MeshType>::type            PointType;
  typedef viennagrid::result_of::vertex<MeshType>::type    VertexType;

  MeshType mesh(2, viennagrid::quadrilateral_tag());

  PointType p[6];

  p[0] = viennagrid::make_point(0.0, 0.0);
  p[1] = viennagrid::make_point(0.5, 0.0);
  p[2] = viennagrid::make_point(1.0, 0.0);
  p[3] = viennagrid::make_point(0.0, 1.0);
  p[4] = viennagrid::make_point(0.5, 1.0);
  p[5] = viennagrid::make_point(1.0, 1.0);

  //upgrade to vertex:
  VertexType vh[6];

  std::cout << "Adding vertices to mesh..." << std::endl;
  for (int i = 0; i < 6; ++i)
    vh[i] = viennagrid::make_vertex( mesh, p[i] );

  std::cout << "Creating segments..." << std::endl;
  RegionType region0 = mesh.make_region();
  RegionType region1 = mesh.make_region();

  std::cout << "Adding cells to segments..." << std::endl;


  std::cout << "Filling segment 0..." << std::endl;
  viennagrid::make_quadrilateral( region0, vh[0], vh[1], vh[4], vh[5] );

  std::cout << "Filling segment 1..." << std::endl;
  viennagrid::make_quadrilateral( region1, vh[1], vh[2], vh[3], vh[4] );



  std::cout << "Vertices in Segment 0: " << viennagrid::vertices(region0).size() << std::endl;
  std::cout << "Vertices in Segment 1: " << viennagrid::vertices(region1).size() << std::endl;
  std::cout << "Edges in Segment 0: "    << viennagrid::lines(region0).size() << std::endl;
  std::cout << "Edges in Segment 1: "    << viennagrid::lines(region1).size() << std::endl;
  std::cout << "Cells in Segment 0: "    << viennagrid::cells(region0).size() << std::endl;
  std::cout << "Cells in Segment 1: "    << viennagrid::cells(region1).size() << std::endl;

  std::cout << "Printing vertices in segment 0:" << std::endl;
  print_elements<viennagrid::vertex_tag>(region0);

  std::cout << "Printing vertices in segment 1:" << std::endl;
  print_elements<viennagrid::vertex_tag>(region1);

  std::cout << "Printing cells in segment 0:" << std::endl;
  print_elements<CellTag>(region0);

  std::cout << "Printing cells in segment 1:" << std::endl;
  print_elements<CellTag>(region1);

  std::cout << "Test for direct operator[] access: " << std::endl;
  //viennagrid::ncells<0>(mesh.segments()[0])[0].print_short();
  std::cout << viennagrid::cells(region0)[0] << std::endl;

  viennagrid::io::vtk_writer<MeshType> my_vtk_writer;
  my_vtk_writer(mesh, "multi_segment_quad");
}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "--- Testing with triangular_2d ---" << std::endl;
  test_triangle2d();
  std::cout << "--- Testing with tetrahedral_3d ---" << std::endl;
  test_tetrahedral3d();

  std::cout << "--- Testing with quadrilateral_2d ---" << std::endl;
  test_quadrilateral2d();
  std::cout << "--- Testing with hexahedral_3d ---" << std::endl;
  test_hexahedral3d();

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  return EXIT_SUCCESS;
}
