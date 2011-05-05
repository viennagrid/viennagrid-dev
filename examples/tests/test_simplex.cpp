/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Peter Lagger                       peter.lagger@ieee.org

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
   
   This is a test file for the ViennaGrid Library IO-System
======================================================================= */

//***********************************************
// Define the dimension
//***********************************************
//#define THREEDIM
#define TWODIM

//***********************************************
// Define the input-file format
//***********************************************

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"
//#include "viennagrid/segment.hpp"
//#include "viennagrid/boundary.hpp"

struct TestDomainConfig
{
  typedef double                                  numeric_type;
  #ifdef THREEDIM
  typedef viennagrid::three_dimensions_tag        dimension_tag;
  typedef viennagrid::tetrahedron_tag             cell_tag;
  #endif
  #ifdef TWODIM
  typedef viennagrid::two_dimensions_tag          dimension_tag;
  typedef viennagrid::triangle_tag                cell_tag;
  #endif
  #ifdef ONEDIM
  typedef viennagrid::one_dimension_tag           dimension_tag;
  typedef viennagrid::line_tag                    cell_tag;
  #endif
};

template <viennagrid::dim_type dim, 
          typename DomainT>
void print_elements(DomainT & domain)
{
  DomainT const & const_domain = domain;

  std::cout << "-- non-const --" << std::endl;
  typedef typename viennagrid::result_of::ncell_container<DomainT, dim>::type  ContainerT;
  typedef typename viennagrid::result_of::iterator<ContainerT>::type           ContainerTIterator;
  
  ContainerT elements = viennagrid::ncells<dim>(domain);
  for (ContainerTIterator it = elements.begin();
       it != elements.end();
       ++it)
  {
    //std::cout << *it << std::endl; 
    it->print();
  }
  
  std::cout << "-- const --" << std::endl;
  typedef typename viennagrid::result_of::const_ncell_container<DomainT, dim>::type   ConstContainerT;
  typedef typename viennagrid::result_of::iterator<ConstContainerT>::type             ConstContainerTIterator;
  
  ConstContainerT const_elements = viennagrid::ncells<dim>(const_domain);
  for (ConstContainerTIterator const_it = const_elements.begin();
       const_it != const_elements.end();
       ++const_it)
  {
    //std::cout << *it << std::endl; 
    const_it->print();
  }
}


void testNewDomain(std::string & infile, std::string & outfile)
{

  typedef viennagrid::domain<TestDomainConfig>        Domain;
  typedef TestDomainConfig::cell_tag                                         CellTag;
  typedef viennagrid::segment_t<TestDomainConfig>       SegmentType;
  
  //typedef viennagrid::TestDomainConfig::DimensionTag              DimensionTag;
  typedef viennagrid::result_of::point_type<TestDomainConfig>::type          PointType;
  typedef viennagrid::result_of::ncell_type<TestDomainConfig, 0>::type       VertexType;
  typedef viennagrid::result_of::ncell_type<TestDomainConfig, 1>::type       EdgeType;
  typedef viennagrid::result_of::ncell_type<TestDomainConfig,
                                            CellTag::topology_level>::type   CellType;

  Domain domain;
  
#ifdef THREEDIM
  PointType p0(0.0, 0.0, 0.0);
  PointType p1(1.0, 0.0, 0.0);
  PointType p2(0.0, 1.0, 0.0);
  PointType p3(0.0, 0.0, 1.0);
#else
  PointType p0(0.0, 0.0);
  PointType p1(1.0, 0.0);
  PointType p2(0.0, 1.0);
  PointType p3(1.0, 1.0);
#endif

  //domain.addSegment();
//  Segment & seg = domain.add();
  
  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);

  VertexType * vertices[4];
  
  std::cout << "Adding vertices to domain..." << std::endl;
  domain.reserve_vertices(4);
  vertices[0] = domain.add(v0); std::cout << vertices[0] << std::endl;
  vertices[1] = domain.add(v1); std::cout << vertices[1] << std::endl;
  vertices[2] = domain.add(v2); std::cout << vertices[2] << std::endl;
  vertices[3] = domain.add(v3); std::cout << vertices[3] << std::endl;

  std::cout << "Adding cells to domain..." << std::endl;
  CellType simplex;
  domain.reserve_cells(2);
  
  simplex.setVertices(vertices);
  domain.add(simplex);
  
  vertices[0] = &(domain.vertex(1));
  vertices[1] = &(domain.vertex(3));
  vertices[2] = &(domain.vertex(2));
  vertices[3] = &(domain.vertex(0));
  simplex.setVertices(vertices);
  domain.add(simplex);

  //SegmentType seg;
  //seg.add(simplex);
  
  //std::cout << "Vertices in Segment: " << seg.size<0>() << std::endl;
  //std::cout << "Edges in Segment: "    << seg.size<1>() << std::endl;
  //std::cout << "Facets in Segment: "   << seg.size<CellTag::topology_level-1>() << std::endl;
  //std::cout << "Cells in Segment: "    << seg.size<CellTag::topology_level>() << std::endl;
  
  std::cout << "Printing vertices in domain:" << std::endl;
  print_elements<0>(domain);

  std::cout << "Printing edges in domain:" << std::endl;
  print_elements<1>(domain);

  std::cout << "Printing facets in domain:" << std::endl;
  print_elements<Domain::config_type::cell_tag::topology_level-1>(domain);

  std::cout << "Printing cells in domain:" << std::endl;
  print_elements<Domain::config_type::cell_tag::topology_level>(domain);
  
  //Test for const-iterators:
  std::cout << "Test for const iterator: " << std::endl;
  typedef viennagrid::result_of::const_ncell_container<Domain, 1>::type  EdgeContainer;
  EdgeContainer edges = viennagrid::ncells<1>(domain);
  
  edges.begin()->print();
  
  typedef viennagrid::result_of::const_ncell_container<EdgeType, 0>::type  VertexOnEdgeContainer;
  VertexOnEdgeContainer vertices_on_edge = viennagrid::ncells<0>(*(edges.begin()));
  
  for (viennagrid::result_of::iterator<VertexOnEdgeContainer>::type voe_it = vertices_on_edge.begin();
       voe_it != vertices_on_edge.end();
       ++voe_it)
       voe_it->print();
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../applications/data/";
  
  std::string infile = path + "line8.sgf";
  std::string outfile = path + "out"; // without ending
  
  testNewDomain(infile, outfile);
  return EXIT_SUCCESS;
}
