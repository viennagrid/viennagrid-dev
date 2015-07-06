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

#include "viennagridpp/core.hpp"

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


inline void setup_triangular2d_mesh(viennagrid::mesh & mesh)
{
  typedef viennagrid::mesh                                          MeshType;

  typedef viennagrid::result_of::point<MeshType>::type              PointType;
  typedef viennagrid::result_of::vertex_handle<MeshType>::type      VertexHandleType;

  PointType p[4];

  p[0] = viennagrid::make_point(0.0, 0.0);
  p[1] = viennagrid::make_point(1.0, 0.0);
  p[2] = viennagrid::make_point(0.0, 1.0);
  p[3] = viennagrid::make_point(1.0, 1.0);

  //upgrade to vertex:
  VertexHandleType vh[4];

  std::cout << "Adding vertices to mesh..." << std::endl;
  for (int i = 0; i < 4; ++i)
    vh[i] = viennagrid::make_vertex( mesh, p[i] );

  std::cout << "Adding cells to mesh..." << std::endl;
  viennagrid::make_triangle( mesh, vh[1], vh[3], vh[2] );
}

inline void setup_triangular3d_mesh(viennagrid::mesh & mesh)
{
  typedef viennagrid::mesh                                          MeshType;

  typedef viennagrid::result_of::point<MeshType>::type              PointType;
  typedef viennagrid::result_of::vertex_handle<MeshType>::type      VertexHandleType;

  PointType p[4];

  p[0] = viennagrid::make_point(0.0, 0.0, 0.0);
  p[1] = viennagrid::make_point(1.0, 0.0, 0.0);
  p[2] = viennagrid::make_point(0.0, 1.0, 0.0);
  p[3] = viennagrid::make_point(0.0, 0.0, 1.0);

  //upgrade to vertex:
  VertexHandleType vh[4];

  std::cout << "Adding vertices to mesh..." << std::endl;
  for (int i = 0; i < 4; ++i)
    vh[i] = viennagrid::make_vertex( mesh, p[i] );

  std::cout << "Adding cells to mesh..." << std::endl;
  viennagrid::make_triangle( mesh, vh[1], vh[3], vh[2] );
}

inline void setup_mesh(viennagrid::tetrahedral_3d_mesh & mesh)
{
  typedef viennagrid::tetrahedral_3d_mesh MeshType;

  typedef viennagrid::result_of::point<MeshType>::type          PointType;
  typedef viennagrid::result_of::vertex_handle<MeshType>::type       VertexHandleType;

  PointType p[4];

  p[0] = PointType(0.0, 0.0, 0.0);
  p[0] = PointType(1.0, 0.0, 0.0);
  p[0] = PointType(0.0, 1.0, 0.0);
  p[0] = PointType(0.0, 0.0, 1.0);

  //upgrade to vertex:
  VertexHandleType vh[4];

  std::cout << "Adding vertices to mesh..." << std::endl;
  for (int i = 0; i < 4; ++i)
    vh[i] = viennagrid::make_vertex( mesh, p[i] );

  std::cout << "Adding cells to mesh..." << std::endl;
  viennagrid::make_tetrahedron( mesh, vh[1], vh[3], vh[2], vh[0] );
}



template <typename MeshType>
void test()
{
  typedef typename viennagrid::result_of::cell_tag<MeshType>::type   CellTag;
  typedef typename viennagrid::result_of::facet_tag<MeshType>::type   FacetTag;
  typedef typename viennagrid::result_of::line<MeshType>::type       EdgeType;

  MeshType mesh;
  setup_mesh(mesh);


  //SegmentType seg;
  //seg.add(simplex);

  //std::cout << "Vertices in Segment: " << seg.size<0>() << std::endl;
  //std::cout << "Edges in Segment: "    << seg.size<1>() << std::endl;
  //std::cout << "Facets in Segment: "   << seg.size<CellTag::dim-1>() << std::endl;
  //std::cout << "Cells in Segment: "    << seg.size<CellTag::dim>() << std::endl;

  std::cout << "Printing vertices in mesh:" << std::endl;
  print_elements<viennagrid::vertex_tag>(mesh);

  std::cout << "Printing edges in mesh:" << std::endl;
  print_elements<viennagrid::line_tag>(mesh);

  std::cout << "Printing facets in mesh:" << std::endl;
  print_elements<FacetTag>(mesh);

  std::cout << "Printing cells in mesh:" << std::endl;
  print_elements<CellTag>(mesh);

  //Test for const-iterators:
  std::cout << "Test for const iterator: " << std::endl;
  typedef typename viennagrid::result_of::const_line_range<MeshType>::type  EdgeContainer;
  EdgeContainer edges(mesh);

  //edges.begin()->print();
  std::cout << *(edges.begin()) << std::endl;

  typedef typename viennagrid::result_of::const_vertex_range<EdgeType>::type  VertexOnEdgeContainer;
  VertexOnEdgeContainer vertices_on_edge(*(edges.begin()));

  for (typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type voe_it = vertices_on_edge.begin();
       voe_it != vertices_on_edge.end();
       ++voe_it)
       std::cout << *voe_it << std::endl;

}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;

  std::cout << "* Testing triangular mesh in 2D: " << std::endl;
  test<viennagrid::triangular_2d_mesh>();
  std::cout << "* Testing triangular mesh in 3D: " << std::endl;
  test<viennagrid::triangular_3d_mesh>();
  std::cout << "* Testing tetrahedral mesh in 3D: " << std::endl;
  test<viennagrid::tetrahedral_3d_mesh>();

  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;

  return EXIT_SUCCESS;
}
