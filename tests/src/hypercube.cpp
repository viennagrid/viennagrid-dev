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
// Define the dimension
//***********************************************
#define THREEDIM
//#define TWODIM

//***********************************************
// Define the input-file format
//***********************************************

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/config/others.hpp"
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
};


void testNewDomain(std::string & infile, std::string & outfile)
{

  typedef viennagrid::domain<TestDomainConfig>                               Domain;
  typedef TestDomainConfig::cell_tag                                         CellTag;
  //typedef viennagrid::TestDomainConfig::DimensionTag                       DimensionTag;
  typedef viennagrid::result_of::point_type<TestDomainConfig>::type          PointType;
  typedef viennagrid::result_of::ncell_type<TestDomainConfig, 0>::type       VertexType;
  typedef viennagrid::result_of::ncell_type<TestDomainConfig,
                                            CellTag::topology_level>::type   CellType;
  //typedef viennagrid::DomainTypes<TestDomainConfig>::segment_type          Segment;

  Domain domain;
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

  domain.reserve_vertices(12);
  std::cout << "Adding vertices to segment:" << std::endl;
  domain.add(v0);
  domain.add(v1);
  domain.add(v2);
  domain.add(v3);
  domain.add(v4);
  domain.add(v5);
  domain.add(v6);
  domain.add(v7);
  domain.add(v8);
  domain.add(v9);
  domain.add(v10);
  domain.add(v11);
  
  VertexType * vertices0[8];
  VertexType * vertices1[8];
  
  vertices0[0] = &(domain.vertex(0));
  vertices0[1] = &(domain.vertex(1));
  vertices0[2] = &(domain.vertex(2));
  vertices0[3] = &(domain.vertex(3));
  vertices0[4] = &(domain.vertex(4));
  vertices0[5] = &(domain.vertex(5));
  vertices0[6] = &(domain.vertex(6));
  vertices0[7] = &(domain.vertex(7));

  vertices1[0] = &(domain.vertex(1));
  vertices1[1] = &(domain.vertex(8));
  vertices1[2] = &(domain.vertex(9));
  vertices1[3] = &(domain.vertex(2));
  vertices1[4] = &(domain.vertex(5));
  vertices1[5] = &(domain.vertex(10));
  vertices1[6] = &(domain.vertex(11));
  vertices1[7] = &(domain.vertex(6));
  
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

  domain.reserve_vertices(6);
  std::cout << "Adding vertices to segment:" << std::endl;
  domain.add(v0);
  domain.add(v1);
  domain.add(v2);
  domain.add(v3);
  domain.add(v4);
  domain.add(v5);
  
  VertexType * vertices0[4];
  VertexType * vertices1[4];
  
  vertices0[0] = &(domain.vertex(0));
  vertices0[1] = &(domain.vertex(1));
  vertices0[2] = &(domain.vertex(4));
  vertices0[3] = &(domain.vertex(5));

  vertices1[0] = &(domain.vertex(1));
  vertices1[1] = &(domain.vertex(2));
  vertices1[2] = &(domain.vertex(3));
  vertices1[3] = &(domain.vertex(4));
  
#endif
  
  domain.reserve_cells(2);
  hypercube0.setVertices(vertices0);
  domain.add(hypercube0);
  
  hypercube1.setVertices(vertices1);  
  domain.add(hypercube1);
  
  std::cout << "Vertices: " << std::endl;
  typedef viennagrid::result_of::ncell_range<Domain, 0>::type   VertexContainer;
  typedef viennagrid::result_of::iterator<VertexContainer>::type    VertexIterator;
  VertexContainer vertices = viennagrid::ncells<0>(domain);
  for (VertexIterator vit = vertices.begin();
        vit != vertices.end();
        ++vit)
      vit->print();
  
  std::cout << "Edges: " << std::endl;
  typedef viennagrid::result_of::ncell_range<Domain, 1>::type   EdgeContainer;
  typedef viennagrid::result_of::iterator<EdgeContainer>::type      EdgeIterator;
  EdgeContainer edges = viennagrid::ncells<1>(domain);
  for (EdgeIterator eit = edges.begin();
        eit != edges.end();
        ++eit)
      eit->print();

  std::cout << "Facets: " << std::endl;
  typedef viennagrid::result_of::ncell_range<Domain, TestDomainConfig::cell_tag::topology_level-1>::type   FacetContainer;
  typedef viennagrid::result_of::iterator<FacetContainer>::type                                                FacetIterator;
  FacetContainer facets = viennagrid::ncells<TestDomainConfig::cell_tag::topology_level-1>(domain);
  for (FacetIterator fit = facets.begin();
        fit != facets.end();
        ++fit)
      fit->print();

  std::cout << "Cells: " << std::endl;
  typedef viennagrid::result_of::ncell_range<Domain, TestDomainConfig::cell_tag::topology_level>::type   CellContainer;
  typedef viennagrid::result_of::iterator<CellContainer>::type                                               CellIterator;
  CellContainer cells = viennagrid::ncells<TestDomainConfig::cell_tag::topology_level>(domain);
  for (CellIterator cit = cells.begin();
        cit != cells.end();
        ++cit)
      cit->print();

  viennagrid::io::vtk_writer<Domain> my_vtk_writer;
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
