/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
                       Institute for Analysis and Scientific Computing,
                       TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

//***********************************************
// Define the input-file format
//***********************************************

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/config/simplex.hpp"
//#include "viennagrid/segment.hpp"
//#include "viennagrid/boundary.hpp"
#include "viennagrid/io/vtk_writer.hpp"


template <viennagrid::dim_type dim, 
          typename SegmentT>
void print_elements(SegmentT & seg)
{
  SegmentT const & const_seg = seg;

  std::cout << "-- non-const --" << std::endl;
  typedef typename viennagrid::result_of::ncell_range<SegmentT, dim>::type  ContainerT;
  typedef typename viennagrid::result_of::iterator<ContainerT>::type           ContainerTIterator;
  
  ContainerT elements = viennagrid::ncells<dim>(seg);
  for (ContainerTIterator it = elements.begin();
       it != elements.end();
       ++it)
  {
    //std::cout << *it << std::endl; 
    it->print_short();
  }
  
  std::cout << "-- const --" << std::endl;
  typedef typename viennagrid::result_of::const_ncell_range<SegmentT, dim>::type   ConstContainerT;
  typedef typename viennagrid::result_of::iterator<ConstContainerT>::type             ConstContainerTIterator;
  
  ConstContainerT const_elements = viennagrid::ncells<dim>(const_seg);
  for (ConstContainerTIterator const_it = const_elements.begin();
       const_it != const_elements.end();
       ++const_it)
  {
    //std::cout << *it << std::endl; 
    const_it->print_short();
  }
}

template <viennagrid::dim_type dim_lower,  //lower dimensional elements
          viennagrid::dim_type dim_higher,  //coboundary for higher dimensional elements
          typename SegmentT>
void print_coelements(SegmentT & seg)
{
  typedef typename SegmentT::config_type    ConfigType;
  
  std::cout << "-- non-const --" << std::endl;
  typedef typename viennagrid::result_of::ncell_range<SegmentT, dim_lower>::type  ContainerT;
  typedef typename viennagrid::result_of::iterator<ContainerT>::type           ContainerTIterator;
  typedef typename viennagrid::result_of::ncell_type<ConfigType, dim_lower>::type      LowerElementType;

  typedef typename viennagrid::result_of::ncell_range<LowerElementType, dim_higher>::type  ContainerTHigher;
  typedef typename viennagrid::result_of::iterator<ContainerTHigher>::type           ContainerTHigherIterator;
  
  ContainerT elements = viennagrid::ncells<dim_lower>(seg);
  for (ContainerTIterator it = elements.begin();
       it != elements.end();
       ++it)
  {
    std::cout << "Lower level element: " << std::endl;
    it->print_short();
    
    std::cout << "Adjacent higher level elements: " << std::endl;
    ContainerTHigher elements2 = viennagrid::ncells<dim_higher>(*it, seg);
    for (ContainerTHigherIterator it2 = elements2.begin();
                                  it2 != elements2.end();
                                ++it2)
         it2->print_short();
    //std::cout << *it << std::endl; 
  }
  
  SegmentT const & const_seg = seg;

  std::cout << "-- const --" << std::endl;
  typedef typename viennagrid::result_of::const_ncell_range<SegmentT, dim_lower>::type   ConstContainerT;
  typedef typename viennagrid::result_of::iterator<ConstContainerT>::type             ConstContainerTIterator;

  typedef typename viennagrid::result_of::const_ncell_range<LowerElementType, dim_higher>::type  ConstContainerTHigher;
  typedef typename viennagrid::result_of::iterator<ConstContainerTHigher>::type           ConstContainerTHigherIterator;
  
  ConstContainerT const_elements = viennagrid::ncells<dim_lower>(const_seg);
  for (ConstContainerTIterator const_it = const_elements.begin();
       const_it != const_elements.end();
       ++const_it)
  {
    std::cout << "Lower level element: " << std::endl;
    const_it->print_short();
    
    std::cout << "Adjacent higher level elements: " << std::endl;
    ConstContainerTHigher const_elements2 = viennagrid::ncells<dim_higher>(*const_it, seg);
    for (ConstContainerTHigherIterator const_it2 = const_elements2.begin();
                                       const_it2 != const_elements2.end();
                                     ++const_it2)
         const_it2->print_short();
  }
}



//test for 3d case:
void test(viennagrid::config::tetrahedral_3d)
{
  typedef viennagrid::config::tetrahedral_3d          ConfigType;
  typedef viennagrid::domain<ConfigType>              Domain;
  typedef ConfigType::cell_tag                        CellTag;
  typedef viennagrid::segment_t<ConfigType>           SegmentType;
  
  //typedef viennagrid::TestDomainConfig::DimensionTag              DimensionTag;
  typedef viennagrid::result_of::point_type<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell_type<ConfigType, 0>::type       VertexType;
  typedef viennagrid::result_of::ncell_type<ConfigType, 1>::type       EdgeType;
  typedef viennagrid::result_of::ncell_type<ConfigType,
                                                     CellTag::topology_level>::type   CellType;

  Domain domain;
  
  PointType p0(0.0, 0.0, 0.0);
  PointType p1(0.5, 0.0, 0.0);
  PointType p2(1.0, 0.0, 0.0);
  PointType p3(0.0, 1.0, 0.0);
  PointType p4(0.5, 1.0, 0.0);
  PointType p5(1.0, 1.0, 0.0);
  PointType p6(0.5, 0.0, 1.0);

  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);
  VertexType v4(p4, 4);
  VertexType v5(p5, 5);
  VertexType v6(p6, 6);

  VertexType * vertices[7];
  
  std::cout << "Adding vertices to domain..." << std::endl;
  domain.reserve_vertices(7);
  vertices[0] = domain.add(v0); std::cout << vertices[0] << std::endl;
  vertices[1] = domain.add(v1); std::cout << vertices[1] << std::endl;
  vertices[2] = domain.add(v2); std::cout << vertices[2] << std::endl;
  vertices[3] = domain.add(v3); std::cout << vertices[3] << std::endl;
  vertices[4] = domain.add(v4); std::cout << vertices[4] << std::endl;
  vertices[5] = domain.add(v5); std::cout << vertices[5] << std::endl;
  vertices[6] = domain.add(v6); std::cout << vertices[6] << std::endl;
  
  std::cout << "Creating segments..." << std::endl;
  domain.create_segments(2);

  std::cout << "Adding cells to segments..." << std::endl;
  CellType simplex;
  domain.reserve_cells(4);
  
  std::cout << "Filling segment 0..." << std::endl;
  vertices[0] = &(domain.vertex(0));
  vertices[1] = &(domain.vertex(1));
  vertices[2] = &(domain.vertex(6));
  vertices[3] = &(domain.vertex(3));
  simplex.setVertices(vertices);
  domain.segment(0).add(simplex);
  
  vertices[0] = &(domain.vertex(1));
  vertices[1] = &(domain.vertex(6));
  vertices[2] = &(domain.vertex(3));
  vertices[3] = &(domain.vertex(4));
  simplex.setVertices(vertices);
  domain.segment(0).add(simplex);

  std::cout << "Filling segment 1..." << std::endl;
  vertices[0] = &(domain.vertex(1));
  vertices[1] = &(domain.vertex(2));
  vertices[2] = &(domain.vertex(6));
  vertices[3] = &(domain.vertex(4));
  simplex.setVertices(vertices);
  domain.segment(1).add(simplex);
  
  vertices[0] = &(domain.vertex(2));
  vertices[1] = &(domain.vertex(6));
  vertices[2] = &(domain.vertex(4));
  vertices[3] = &(domain.vertex(5));
  simplex.setVertices(vertices);
  domain.segment(1).add(simplex);
  
  std::cout << "ID of segment 0: " << domain.segment(0).id() << std::endl;
  std::cout << "ID of segment 1: " << domain.segment(1).id() << std::endl;
  
  std::cout << "Vertices in Segment 0: " << domain.segment(0).size<0>() << std::endl;
  std::cout << "Vertices in Segment 1: " << domain.segment(1).size<0>() << std::endl;
  //std::cout << "Edges in Segment: "    << seg.size<1>() << std::endl;
  //std::cout << "Facets in Segment: "   << seg.size<CellTag::topology_level-1>() << std::endl;
  std::cout << "Cells in Segment 0: "    << domain.segment(0).size<CellTag::topology_level>() << std::endl;
  std::cout << "Cells in Segment 1: "    << domain.segment(1).size<CellTag::topology_level>() << std::endl;
  
  std::cout << "Printing vertices in segment 0:" << std::endl;
  print_elements<0>(domain.segment(0));

  std::cout << "Printing vertices in segment 1:" << std::endl;
  print_elements<0>(domain.segment(1));
  
  std::cout << "Printing cells in segment 0:" << std::endl;
  print_elements<Domain::config_type::cell_tag::topology_level>(domain.segment(0));

  std::cout << "Printing cells in segment 1:" << std::endl;
  print_elements<Domain::config_type::cell_tag::topology_level>(domain.segment(1));
  
  viennagrid::io::vtk_writer<Domain> my_vtk_writer;
  my_vtk_writer.writeDomain(domain, "multi-segment-3d");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
}



//test for 2d case:
void test(viennagrid::config::triangular_2d)
{
  typedef viennagrid::config::triangular_2d           ConfigType;
  typedef viennagrid::domain<ConfigType>              Domain;
  typedef ConfigType::cell_tag                        CellTag;
  typedef viennagrid::segment_t<ConfigType>           SegmentType;
  
  //typedef viennagrid::TestDomainConfig::DimensionTag              DimensionTag;
  typedef viennagrid::result_of::point_type<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell_type<ConfigType, 0>::type       VertexType;
  typedef viennagrid::result_of::ncell_type<ConfigType, 1>::type       EdgeType;
  typedef viennagrid::result_of::ncell_type<ConfigType,
                                                     CellTag::topology_level>::type   CellType;

  Domain domain;
  
  PointType p0(0.0, 0.0);
  PointType p1(0.5, 0.0);
  PointType p2(1.0, 0.0);
  PointType p3(0.0, 1.0);
  PointType p4(0.5, 1.0);
  PointType p5(1.0, 1.0);

  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);
  VertexType v4(p4, 4);
  VertexType v5(p5, 5);

  VertexType * vertices[6];
  
  std::cout << "Adding vertices to domain..." << std::endl;
  domain.reserve_vertices(6);
  vertices[0] = domain.add(v0); std::cout << vertices[0] << std::endl;
  vertices[1] = domain.add(v1); std::cout << vertices[1] << std::endl;
  vertices[2] = domain.add(v2); std::cout << vertices[2] << std::endl;
  vertices[3] = domain.add(v3); std::cout << vertices[3] << std::endl;
  vertices[4] = domain.add(v4); std::cout << vertices[4] << std::endl;
  vertices[5] = domain.add(v5); std::cout << vertices[5] << std::endl;
  
  std::cout << "Creating segments..." << std::endl;
  domain.create_segments(2);

  std::cout << "Adding cells to segments..." << std::endl;
  CellType simplex;
  domain.reserve_cells(4);
  
  std::cout << "Filling segment 0..." << std::endl;
  vertices[0] = &(domain.vertex(0));
  vertices[1] = &(domain.vertex(1));
  vertices[2] = &(domain.vertex(3));
  simplex.setVertices(vertices);
  domain.segment(0).add(simplex);
  
  vertices[0] = &(domain.vertex(1));
  vertices[1] = &(domain.vertex(4));
  vertices[2] = &(domain.vertex(3));
  simplex.setVertices(vertices);
  domain.segment(0).add(simplex);

  std::cout << "Filling segment 1..." << std::endl;
  vertices[0] = &(domain.vertex(1));
  vertices[1] = &(domain.vertex(2));
  vertices[2] = &(domain.vertex(4));
  simplex.setVertices(vertices);
  domain.segment(1).add(simplex);
  
  vertices[0] = &(domain.vertex(2));
  vertices[1] = &(domain.vertex(5));
  vertices[2] = &(domain.vertex(4));
  simplex.setVertices(vertices);
  domain.segment(1).add(simplex);
  
  
  std::cout << "Vertices in Segment 0: " << domain.segment(0).size<0>() << std::endl;
  std::cout << "Vertices in Segment 1: " << domain.segment(1).size<0>() << std::endl;
  //std::cout << "Edges in Segment: "    << seg.size<1>() << std::endl;
  //std::cout << "Facets in Segment: "   << seg.size<CellTag::topology_level-1>() << std::endl;
  std::cout << "Cells in Segment 0: "    << domain.segment(0).size<CellTag::topology_level>() << std::endl;
  std::cout << "Cells in Segment 1: "    << domain.segment(1).size<CellTag::topology_level>() << std::endl;
  
  std::cout << "Printing vertices in segment 0:" << std::endl;
  print_elements<0>(domain.segment(0));

  std::cout << "Printing vertices in segment 1:" << std::endl;
  print_elements<0>(domain.segment(1));

  std::cout << "Printing edges in segment 0:" << std::endl;
  print_elements<1>(domain.segment(0));

  std::cout << "Printing edges in segment 1:" << std::endl;
  print_elements<1>(domain.segment(1));

  std::cout << "Printing cells in segment 0:" << std::endl;
  print_elements<Domain::config_type::cell_tag::topology_level>(domain.segment(0));

  std::cout << "Printing cells in segment 1:" << std::endl;
  print_elements<Domain::config_type::cell_tag::topology_level>(domain.segment(1));
  
  std::cout << "Printing co-boundary for vertices in segment 0:" << std::endl;
  print_coelements<0, 1>(domain.segment(0));
  
  std::cout << "Printing co-boundary for vertices in segment 1:" << std::endl;
  print_coelements<0, 1>(domain.segment(1));

  std::cout << "Printing co-boundary for vertices in segment 0:" << std::endl;
  print_coelements<0, 1>(domain.segment(0));
  
  viennagrid::io::vtk_writer<Domain> my_vtk_writer;
  my_vtk_writer.writeDomain(domain, "multi-segment-2d");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  test(viennagrid::config::triangular_2d());
  test(viennagrid::config::tetrahedral_3d());
  return EXIT_SUCCESS;
}
