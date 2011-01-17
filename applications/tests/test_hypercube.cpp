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
#include "viennagrid/io/vtk_writer.hpp"


struct TestDomainConfig
{
  typedef double                                  numeric_type;
  #ifdef THREEDIM
  typedef viennagrid::three_dimensions_tag        dimension_tag;
  typedef viennagrid::hexahedron_tag              cell_tag;
  #endif
  #ifdef TWODIM
  typedef viennagrid::two_dimensions_tag          dimension_tag;
  typedef viennagrid::quadrilateral_tag           cell_tag;
  #endif
  #ifdef ONEDIM
  typedef viennagrid::one_dimension_tag           dimension_tag;
  typedef viennagrid::line_tag                    cell_tag;
  #endif

  //multigrid:
  //typedef viennagrid::full_multigrid_tag                       multigrid_tag;
  typedef viennagrid::no_multigrid_tag             multigrid_tag;
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

  typedef viennagrid::IteratorTypes<Segment, 0>::result_type                                         VertexIterator;
  typedef viennagrid::IteratorTypes<Segment, 1>::result_type                                         EdgeIterator;
  typedef viennagrid::IteratorTypes<Segment, TestDomainConfig::cell_tag::topology_level-1>::result_type   FacetIterator;
  typedef viennagrid::IteratorTypes<Segment, TestDomainConfig::cell_tag::topology_level>::result_type     CellIterator;

  Domain domain;
  Segment & seg = domain.add();
  seg.reserveCells(2);
  CellType hypercube0;
  CellType hypercube1;

#ifdef THREEDIM
  PointType p0(0.0, 0.0, 0.0);
  PointType p1(1.0, 0.0, 0.0);
  PointType p2(1.0, 1.0, 0.0);
  PointType p3(0.0, 1.0, 0.0);
  PointType p4(0.0, 0.0, 1.0);
  PointType p5(1.0, 0.0, 1.0);
  PointType p6(1.0, 1.0, 1.0);
  PointType p7(0.0, 1.0, 1.0);
  PointType p8(2.0, 0.0, 0.0);
  PointType p9(2.0, 1.0, 0.0);
  PointType p10(2.0, 0.0, 1.0);
  PointType p11(2.0, 1.0, 1.0);
  
  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);
  VertexType v4(p4, 4);
  VertexType v5(p5, 5);
  VertexType v6(p6, 6);
  VertexType v7(p7, 7);
  VertexType v8(p8, 8);
  VertexType v9(p9, 9);
  VertexType v10(p10, 10);
  VertexType v11(p11, 11);

  seg.reserveVertices(12);
  std::cout << "Adding vertices to segment:" << std::endl;
  seg.add<0>(0, v0);
  seg.add<0>(1, v1);
  seg.add<0>(2, v2);
  seg.add<0>(3, v3);
  seg.add<0>(4, v4);
  seg.add<0>(5, v5);
  seg.add<0>(6, v6);
  seg.add<0>(7, v7);
  seg.add<0>(8, v8);
  seg.add<0>(9, v9);
  seg.add<0>(10, v10);
  seg.add<0>(11, v11);
  
  VertexType * vertices0[8];
  VertexType * vertices1[8];
  
  vertices0[0] = seg.getVertexAddress(0);
  vertices0[1] = seg.getVertexAddress(1);
  vertices0[2] = seg.getVertexAddress(2);
  vertices0[3] = seg.getVertexAddress(3);
  vertices0[4] = seg.getVertexAddress(4);
  vertices0[5] = seg.getVertexAddress(5);
  vertices0[6] = seg.getVertexAddress(6);
  vertices0[7] = seg.getVertexAddress(7);

  vertices1[0] = seg.getVertexAddress(1);
  vertices1[1] = seg.getVertexAddress(8);
  vertices1[2] = seg.getVertexAddress(9);
  vertices1[3] = seg.getVertexAddress(2);
  vertices1[4] = seg.getVertexAddress(5);
  vertices1[5] = seg.getVertexAddress(10);
  vertices1[6] = seg.getVertexAddress(11);
  vertices1[7] = seg.getVertexAddress(6);
  
#else
  
  PointType p0(0.0, 0.0);
  PointType p1(1.0, 0.0);
  PointType p2(2.0, 0.0);
  PointType p3(2.0, 1.0);
  PointType p4(1.0, 1.0);
  PointType p5(0.0, 1.0);
  
  //upgrade to vertex:
  VertexType v0(p0, 0);
  VertexType v1(p1, 1);
  VertexType v2(p2, 2);
  VertexType v3(p3, 3);
  VertexType v4(p4, 4);
  VertexType v5(p5, 5);

  seg.reserveVertices(6);
  std::cout << "Adding vertices to segment:" << std::endl;
  seg.add<0>(0, v0);
  seg.add<0>(1, v1);
  seg.add<0>(2, v2);
  seg.add<0>(3, v3);
  seg.add<0>(4, v4);
  seg.add<0>(5, v5);
  
  VertexType * vertices0[4];
  VertexType * vertices1[4];
  
  vertices0[0] = seg.getVertexAddress(0);
  vertices0[1] = seg.getVertexAddress(1);
  vertices0[2] = seg.getVertexAddress(4);
  vertices0[3] = seg.getVertexAddress(5);

  vertices1[0] = seg.getVertexAddress(1);
  vertices1[1] = seg.getVertexAddress(2);
  vertices1[2] = seg.getVertexAddress(3);
  vertices1[3] = seg.getVertexAddress(4);
  
#endif
  
  hypercube0.setVertices(vertices0);
  seg.add<TestDomainConfig::cell_tag::topology_level>(0, hypercube0);
  
  hypercube1.setVertices(vertices1);  
  seg.add<TestDomainConfig::cell_tag::topology_level>(1, hypercube1);
  
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

  viennagrid::io::Vtk_writer<Domain> my_vtk_writer;
  my_vtk_writer.writeDomain(domain, outfile + ".vtk");
  
  
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
