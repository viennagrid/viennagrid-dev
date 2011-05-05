/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
   
   This is a test file for the ViennaGrid Library IO-System
======================================================================= */

//***********************************************
// Define the dimension
//***********************************************
//#define THREEDIM
#define TWODIM
//#define ONEDIM

//***********************************************
// Define the input-file format
//***********************************************

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/topology/tetrahedron.hpp"
#include "viennagrid/topology/triangle.hpp"
//#include "viennagrid/segment.hpp"
//#include "viennagrid/boundary.hpp"
#include "viennagrid/io/sgf_reader.hpp"

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
                                            CellTag::topology_level-1>::type FacetType;
  typedef viennagrid::result_of::ncell_type<TestDomainConfig,
                                            CellTag::topology_level>::type   CellType;
                                            
  typedef viennagrid::result_of::ncell_container<Domain, 0>::type   VertexContainer;
  typedef viennagrid::result_of::iterator<VertexContainer>::type    VertexIterator;
                                            
  typedef viennagrid::result_of::ncell_container<Domain, 1>::type   EdgeContainer;
  typedef viennagrid::result_of::iterator<EdgeContainer>::type      EdgeIterator;

  typedef viennagrid::result_of::ncell_container<Domain, CellTag::topology_level-1>::type   FacetContainer;
  typedef viennagrid::result_of::iterator<FacetContainer>::type                             FacetIterator;

  typedef viennagrid::result_of::ncell_container<Domain, CellTag::topology_level>::type     CellContainer;
  typedef viennagrid::result_of::iterator<CellContainer>::type                              CellIterator;
  
  typedef viennagrid::result_of::ncell_container<EdgeType, 0>::type       VertexOnEdgeContainer;
  typedef viennagrid::result_of::iterator<VertexOnEdgeContainer>::type    VertexOnEdgeIterator;
  
  Domain domain;

  //read from file:
  try
  {
    viennagrid::io::sgf_reader my_sgf_reader;
#ifdef THREEDIM
    my_sgf_reader(domain, "../examples/data/cube6.sgf");
#endif
#ifdef TWODIM
    my_sgf_reader(domain, "../examples/data/square8.sgf");
#endif
#ifdef ONEDIM
    my_sgf_reader(domain, "../examples/data/line8.sgf");
#endif
  }
  catch (...)
  {
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    exit(EXIT_FAILURE);
  }

  //
  // Test 1: Iterate over all edges of each vertex:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 2: Iteration over all edges adjacent to each vertex" << std::endl;
  std::cout << "*" << std::endl;
  VertexContainer vertices = viennagrid::ncells<0>(domain);
  for (VertexIterator vit = vertices.begin();
       vit != vertices.end();
       ++vit)
  {
    vit->print_short();
    typedef viennagrid::result_of::ncell_container<VertexType, 1>::type      EdgeOnVertexContainer;
    typedef viennagrid::result_of::iterator<EdgeOnVertexContainer>::type     EdgeOnVertexIterator;
    
    EdgeOnVertexContainer edges = viennagrid::ncells<1>(*vit, domain);
    for (EdgeOnVertexIterator eovit = edges.begin();
         eovit != edges.end();
         ++eovit)
    {
      eovit->print_short();
    }
    std::cout << std::endl;
    std::cout << std::endl;
  }

  //
  // Test 2: Iterate over all cells of each facet:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 2: Iteration over all cells adjacent to each facet" << std::endl;
  std::cout << "*" << std::endl;
  FacetContainer facets = viennagrid::ncells<0>(domain);
  for (FacetIterator fit = facets.begin();
       fit != facets.end();
       ++fit)
  {
    fit->print_short();
    typedef viennagrid::result_of::const_ncell_container<FacetType, CellTag::topology_level>::type   CellOnFacetContainer;
    typedef viennagrid::result_of::iterator<CellOnFacetContainer>::type     CellOnFacetIterator;
    
    CellOnFacetContainer cells;
    cells = viennagrid::ncells<CellTag::topology_level>(*fit, domain);
    for (CellOnFacetIterator eovit = cells.begin();
         eovit != cells.end();
         ++eovit)
    {
      eovit->print_short();
    }
    std::cout << std::endl;
    std::cout << std::endl;
  }

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
