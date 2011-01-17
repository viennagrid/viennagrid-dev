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
#define THREEDIM
//#define TWODIM

//***********************************************
// Define the input-file format
//***********************************************

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/boundary.hpp"

struct TestDomainConfig
{
  typedef double                                 numeric_type;
  #ifdef THREEDIM
  typedef viennagrid::three_dimensions_tag         dimension_tag;
  typedef viennagrid::tetrahedron_tag             cell_tag;
  #endif
  #ifdef TWODIM
  typedef viennagrid::two_dimensions_tag           dimension_tag;
  typedef viennagrid::triangle_tag                cell_tag;
  #endif
  #ifdef ONEDIM
  typedef viennagrid::one_dimension_tag            dimension_tag;
  typedef viennagrid::line_tag                    cell_tag;
  #endif

  //multigrid:
  typedef viennagrid::full_multigrid_tag                       multigrid_tag;
  //typedef viennagrid::no_multigrid_tag             multigrid_tag;
};


void testNewDomain(std::string & infile, std::string & outfile)
{

  typedef viennagrid::domain<TestDomainConfig>        Domain;
  //typedef viennagrid::TestDomainConfig::DimensionTag              DimensionTag;
  typedef viennagrid::DomainTypes<TestDomainConfig>::point_type    PointType;
  typedef viennagrid::DomainTypes<TestDomainConfig>::vertex_type   VertexType;

  typedef viennagrid::DomainTypes<TestDomainConfig>::facet_type    FacetType;
  typedef viennagrid::DomainTypes<TestDomainConfig>::cell_type     CellType;
  typedef viennagrid::DomainTypes<TestDomainConfig>::segment_type  Segment;

  typedef viennagrid::IteratorTypes<Segment, 0>::ResultType                                         VertexIterator;
  typedef viennagrid::IteratorTypes<Segment, 1>::ResultType                                         EdgeIterator;
  typedef viennagrid::IteratorTypes<Segment, TestDomainConfig::cell_tag::topology_level-1>::ResultType   FacetIterator;
  typedef viennagrid::IteratorTypes<Segment, TestDomainConfig::cell_tag::topology_level>::ResultType     CellIterator;

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
  Segment & seg = domain.add();

  
  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);

  seg.reserveVertices(4);
  std::cout << "Adding vertices to segment:" << std::endl;
  seg.add<0>(0, v0);
  seg.add<0>(1, v1);
  seg.add<0>(2, v2);
  seg.add<0>(3, v3);

  seg.reserveCells(2);

  CellType simplex;
  
  VertexType * vertices[4];

  vertices[0] = seg.getVertexAddress(0);
  vertices[1] = seg.getVertexAddress(1);
  vertices[2] = seg.getVertexAddress(2);
  vertices[3] = seg.getVertexAddress(3);
  simplex.setVertices(vertices);
  seg.add<TestDomainConfig::cell_tag::topology_level>(0, simplex);

  vertices[0] = seg.getVertexAddress(1);
  vertices[1] = seg.getVertexAddress(3);
  vertices[2] = seg.getVertexAddress(2);
  vertices[3] = seg.getVertexAddress(0);
  simplex.setVertices(vertices);
  seg.add<TestDomainConfig::cell_tag::topology_level>(1, simplex);
  
  
  std::cout << "Vertices: " << std::endl;
  for (VertexIterator vit = seg.begin<0>();
        vit != seg.end<0>();
        ++vit)
      vit->print();

  std::cout << "Edges: " << std::endl;
  for (EdgeIterator eit = seg.begin<1>();
        eit != seg.end<1>();
        ++eit)
      eit->print();

  std::cout << "Facets: " << std::endl;
  for (FacetIterator fit = seg.begin<TestDomainConfig::cell_tag::topology_level-1>();
        fit != seg.end<TestDomainConfig::cell_tag::topology_level-1>();
        ++fit)
      fit->print();

  std::cout << "Cells: " << std::endl;
  for (CellIterator cit = seg.begin<TestDomainConfig::cell_tag::topology_level>();
        cit != seg.end<TestDomainConfig::cell_tag::topology_level>();
        ++cit)
      cit->print();

  
  
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
