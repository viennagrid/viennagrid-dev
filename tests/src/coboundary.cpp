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
//#define ONEDIM

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
#include "viennagrid/io/netgen_reader.hpp"

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

  typedef viennagrid::result_of::domain<TestDomainConfig>::type        Domain;
  typedef TestDomainConfig::cell_tag                                         CellTag;
  typedef viennagrid::segment_t<TestDomainConfig>       SegmentType;
  
  //typedef viennagrid::TestDomainConfig::DimensionTag              DimensionTag;
  typedef viennagrid::result_of::point<TestDomainConfig>::type          PointType;
  typedef viennagrid::result_of::ncell<TestDomainConfig, 0>::type       VertexType;
  typedef viennagrid::result_of::ncell<TestDomainConfig, 1>::type       EdgeType;
  typedef viennagrid::result_of::ncell<TestDomainConfig,
                                            CellTag::topology_level-1>::type FacetType;
  typedef viennagrid::result_of::ncell<TestDomainConfig,
                                            CellTag::topology_level>::type   CellType;
                                            
  typedef viennagrid::result_of::ncell_range<Domain, 0>::type   VertexContainer;
  typedef viennagrid::result_of::iterator<VertexContainer>::type    VertexIterator;
                                            
  typedef viennagrid::result_of::ncell_range<Domain, 1>::type   EdgeContainer;
  typedef viennagrid::result_of::iterator<EdgeContainer>::type      EdgeIterator;

  typedef viennagrid::result_of::ncell_range<Domain, CellTag::topology_level-1>::type   FacetContainer;
  typedef viennagrid::result_of::iterator<FacetContainer>::type                             FacetIterator;

  typedef viennagrid::result_of::ncell_range<Domain, CellTag::topology_level>::type     CellContainer;
  typedef viennagrid::result_of::iterator<CellContainer>::type                              CellIterator;
  
  typedef viennagrid::result_of::ncell_range<EdgeType, 0>::type       VertexOnEdgeContainer;
  typedef viennagrid::result_of::iterator<VertexOnEdgeContainer>::type    VertexOnEdgeIterator;
  
  Domain domain;

  //read from file:
  try
  {
    viennagrid::io::netgen_reader my_netgen_reader;
#ifdef THREEDIM
    my_netgen_reader(domain, "../../examples/data/cube6.mesh");
#endif
#ifdef TWODIM
    my_netgen_reader(domain, "../../examples/data/square8.mesh");
#endif
#ifdef ONEDIM
    my_netgen_reader(domain, "../../examples/data/line8.mesh");
#endif
  }
  catch (std::exception const & ex)
  {
    std::cout << ex.what() << std::endl;
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
    std::cout << *vit << std::endl;
    typedef viennagrid::result_of::ncell_range<VertexType, 1>::type      EdgeOnVertexContainer;
    typedef viennagrid::result_of::iterator<EdgeOnVertexContainer>::type     EdgeOnVertexIterator;
    
    EdgeOnVertexContainer edges = viennagrid::ncells<1>(*vit, domain);
    for (EdgeOnVertexIterator eovit = edges.begin();
         eovit != edges.end();
         ++eovit)
    {
      std::cout << *eovit << std::endl;
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
  FacetContainer facets = viennagrid::ncells<CellTag::topology_level-1>(domain);
  for (FacetIterator fit = facets.begin();
       fit != facets.end();
       ++fit)
  {
    std::cout << *fit << std::endl;
    typedef viennagrid::result_of::const_ncell_range<FacetType, CellTag::topology_level>::type   CellOnFacetContainer;
    typedef viennagrid::result_of::iterator<CellOnFacetContainer>::type     CellOnFacetIterator;
    
    CellOnFacetContainer cells;
    cells = viennagrid::ncells<CellTag::topology_level>(*fit, domain);
    for (CellOnFacetIterator eovit = cells.begin();
         eovit != cells.end();
         ++eovit)
    {
      std::cout << *eovit << std::endl;
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
