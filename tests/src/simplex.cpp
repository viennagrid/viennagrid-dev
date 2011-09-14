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

#ifdef _MSC_VER      //Visual Studio complains about potentially dangerous things, which are perfectly legal in our context
  #pragma warning( disable : 4355 )     //use of this in member initializer list
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"

template <viennagrid::dim_type dim, 
          typename DomainT>
void print_elements(DomainT & domain)
{
  DomainT const & const_domain = domain;

  std::cout << "-- non-const --" << std::endl;
  typedef typename viennagrid::result_of::ncell_range<DomainT, dim>::type  ContainerT;
  typedef typename viennagrid::result_of::iterator<ContainerT>::type           ContainerTIterator;
  
  ContainerT elements = viennagrid::ncells<dim>(domain);
  for (ContainerTIterator it = elements.begin();
       it != elements.end();
       ++it)
  {
    std::cout << *it << std::endl; 
    //it->print();
  }
  
  std::cout << "-- const --" << std::endl;
  typedef typename viennagrid::result_of::const_ncell_range<DomainT, dim>::type   ConstContainerT;
  typedef typename viennagrid::result_of::iterator<ConstContainerT>::type             ConstContainerTIterator;
  
  ConstContainerT const_elements = viennagrid::ncells<dim>(const_domain);
  for (ConstContainerTIterator const_it = const_elements.begin();
       const_it != const_elements.end();
       ++const_it)
  {
    std::cout << *const_it << std::endl; 
    //const_it->print();
  }
}


void setup_domain(viennagrid::domain_t<viennagrid::config::triangular_2d> & d)
{
  typedef viennagrid::config::triangular_2d                           ConfigType;
  typedef ConfigType::cell_tag                                         CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                            CellTag::dim>::type   CellType;
  
  PointType p0(0.0, 0.0, 0.0);
  PointType p1(1.0, 0.0, 0.0);
  PointType p2(0.0, 1.0, 0.0);
  PointType p3(0.0, 0.0, 1.0);

  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);

  VertexType * vertices[4];
  
  std::cout << "Adding vertices to domain..." << std::endl;
  vertices[0] = d.push_back(v0); std::cout << vertices[0] << std::endl;
  vertices[1] = d.push_back(v1); std::cout << vertices[1] << std::endl;
  vertices[2] = d.push_back(v2); std::cout << vertices[2] << std::endl;
  vertices[3] = d.push_back(v3); std::cout << vertices[3] << std::endl;

  std::cout << "Adding cells to domain..." << std::endl;
  CellType simplex;
  
  simplex.vertices(vertices);
  d.push_back(simplex);
  
  vertices[0] = &(viennagrid::ncells<0>(d)[1]);
  vertices[1] = &(viennagrid::ncells<0>(d)[3]);
  vertices[2] = &(viennagrid::ncells<0>(d)[2]);
  simplex.vertices(vertices);
  d.push_back(simplex);
  
}

void setup_domain(viennagrid::domain_t<viennagrid::config::triangular_3d> & d)
{
  typedef viennagrid::config::triangular_3d                            ConfigType;
  typedef ConfigType::cell_tag                                         CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                            CellTag::dim>::type   CellType;

  PointType p0(0.0, 0.0, 0.0);
  PointType p1(1.0, 0.0, 0.0);
  PointType p2(0.0, 1.0, 0.0);
  PointType p3(0.0, 0.0, 1.0);

  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);

  VertexType * vertices[4];
  
  std::cout << "Adding vertices to domain..." << std::endl;
  vertices[0] = d.push_back(v0); std::cout << vertices[0] << std::endl;
  vertices[1] = d.push_back(v1); std::cout << vertices[1] << std::endl;
  vertices[2] = d.push_back(v2); std::cout << vertices[2] << std::endl;
  vertices[3] = d.push_back(v3); std::cout << vertices[3] << std::endl;

  std::cout << "Adding cells to domain..." << std::endl;
  CellType simplex;
  
  simplex.vertices(vertices);
  d.push_back(simplex);
  
  vertices[0] = &(viennagrid::ncells<0>(d)[1]);
  vertices[1] = &(viennagrid::ncells<0>(d)[3]);
  vertices[2] = &(viennagrid::ncells<0>(d)[2]);
  simplex.vertices(vertices);
  d.push_back(simplex);

}

void setup_domain(viennagrid::domain_t<viennagrid::config::tetrahedral_3d> & d)
{
  typedef viennagrid::config::tetrahedral_3d                           ConfigType;
  typedef ConfigType::cell_tag                                         CellTag;
  
  typedef viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  
  typedef viennagrid::result_of::ncell<ConfigType,
                                            CellTag::dim>::type   CellType;
                                            
  PointType p0(0.0, 0.0, 0.0);
  PointType p1(1.0, 0.0, 0.0);
  PointType p2(0.0, 1.0, 0.0);
  PointType p3(0.0, 0.0, 1.0);

  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);

  VertexType * vertices[4];
  
  std::cout << "Adding vertices to domain..." << std::endl;
  vertices[0] = d.push_back(v0); std::cout << vertices[0] << std::endl;
  vertices[1] = d.push_back(v1); std::cout << vertices[1] << std::endl;
  vertices[2] = d.push_back(v2); std::cout << vertices[2] << std::endl;
  vertices[3] = d.push_back(v3); std::cout << vertices[3] << std::endl;

  std::cout << "Adding cells to domain..." << std::endl;
  CellType simplex;
  
  simplex.vertices(vertices);
  d.push_back(simplex);
  
  vertices[0] = &(viennagrid::ncells<0>(d)[1]);
  vertices[1] = &(viennagrid::ncells<0>(d)[3]);
  vertices[2] = &(viennagrid::ncells<0>(d)[2]);
  vertices[3] = &(viennagrid::ncells<0>(d)[0]);
  simplex.vertices(vertices);
  d.push_back(simplex);
                                            
}



template <typename ConfigType>
void test()
{
  typedef typename viennagrid::result_of::domain<ConfigType>::type          Domain;
  typedef typename ConfigType::cell_tag           CellTag;
  typedef viennagrid::segment_t<ConfigType>       SegmentType;
  
  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 1>::type       EdgeType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                     CellTag::dim>::type   CellType;

  Domain domain;
  
  setup_domain(domain);
  

  //SegmentType seg;
  //seg.add(simplex);
  
  //std::cout << "Vertices in Segment: " << seg.size<0>() << std::endl;
  //std::cout << "Edges in Segment: "    << seg.size<1>() << std::endl;
  //std::cout << "Facets in Segment: "   << seg.size<CellTag::dim-1>() << std::endl;
  //std::cout << "Cells in Segment: "    << seg.size<CellTag::dim>() << std::endl;
  
  std::cout << "Printing vertices in domain:" << std::endl;
  print_elements<0>(domain);

  std::cout << "Printing edges in domain:" << std::endl;
  print_elements<1>(domain);

  std::cout << "Printing facets in domain:" << std::endl;
  print_elements<Domain::config_type::cell_tag::dim-1>(domain);

  std::cout << "Printing cells in domain:" << std::endl;
  print_elements<Domain::config_type::cell_tag::dim>(domain);
  
  //Test for const-iterators:
  std::cout << "Test for const iterator: " << std::endl;
  typedef typename viennagrid::result_of::const_ncell_range<Domain, 1>::type  EdgeContainer;
  EdgeContainer edges = viennagrid::ncells<1>(domain);
  
  //edges.begin()->print();
  std::cout << *(edges.begin()) << std::endl;
  
  typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type  VertexOnEdgeContainer;
  VertexOnEdgeContainer vertices_on_edge = viennagrid::ncells<0>(*(edges.begin()));
  
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
  test<viennagrid::config::triangular_2d>();
  std::cout << "* Testing triangular mesh in 3D: " << std::endl;
  test<viennagrid::config::triangular_3d>();
  std::cout << "* Testing tetrahedral mesh in 3D: " << std::endl;
  test<viennagrid::config::tetrahedral_3d>();
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
