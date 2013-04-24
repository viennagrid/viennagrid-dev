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

//NOTE: This test does not check run-time output at the moment and rather serves as compilation test.

#ifdef _MSC_VER      //Visual Studio complains about potentially dangerous things, which are perfectly legal in our context
  #pragma warning( disable : 4355 )     //use of this in member initializer list
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif


#include "viennagrid/forwards.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain/config.hpp"
#include "viennagrid/domain/coboundary_iteration.hpp"
#include "viennagrid/io/netgen_reader.hpp"

template <typename CellTypeOrTag, typename Domain>
void test(std::string infile)
{
  typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;
    
  //typedef typename viennagrid::result_of::domain<ConfigType>::type        Domain;
  //typedef typename ConfigType::cell_tag                                   CellTag;
  typedef typename viennagrid::result_of::geometric_view<Domain>::type       SegmentType;
  
  typedef typename viennagrid::result_of::point_type<Domain>::type          PointType;
  typedef typename viennagrid::result_of::element<Domain, viennagrid::vertex_tag>::type       VertexType;
  typedef typename viennagrid::result_of::element<Domain, viennagrid::line_tag>::type       EdgeType;
  typedef typename viennagrid::result_of::element<Domain,
                                                typename CellTag::facet_tag>::type FacetType;
  typedef typename viennagrid::result_of::element<Domain,
                                                CellTag>::type   CellType;
                                            
  typedef typename viennagrid::result_of::element_range<Domain, viennagrid::vertex_tag>::type       VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type    VertexIterator;
  typedef typename viennagrid::result_of::hook_iterator<VertexContainer>::type    VertexHandleIterator;
                                            
  typedef typename viennagrid::result_of::element_range<Domain, viennagrid::line_tag>::type       EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type      EdgeIterator;

  typedef typename viennagrid::result_of::element_range<Domain, typename CellTag::facet_tag>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                         FacetIterator;
  typedef typename viennagrid::result_of::hook_iterator<FacetContainer>::type                         FacetHandleIterator;

  typedef typename viennagrid::result_of::element_range<Domain, CellTag>::type     CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                          CellIterator;
  
  typedef typename viennagrid::result_of::element_range<EdgeType, viennagrid::vertex_tag>::type           VertexOnEdgeContainer;
  typedef typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type    VertexOnEdgeIterator;
  
  Domain domain;
  std::deque<SegmentType> segments;

  //read from file:
  try
  {
    viennagrid::io::netgen_reader<CellTag> my_netgen_reader;
    my_netgen_reader(domain, segments, infile);
  }
  catch (std::exception const & ex)
  {
    std::cout << "what(): " << ex.what() << std::endl;
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    exit(EXIT_FAILURE);
  }

  //
  // Test 1: Iterate over all edges of each vertex:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 2: Iteration over all edges adjacent to each vertex" << std::endl;
  std::cout << "*" << std::endl;
  VertexContainer vertices = viennagrid::elements<viennagrid::vertex_tag>(domain);
  for (VertexHandleIterator vit = vertices.hook_begin();
       vit != vertices.hook_end();
       ++vit)
  {
    const VertexType & vertex = viennagrid::dereference_hook(domain, *vit);
    std::cout << vertex << std::endl;
    typedef typename viennagrid::result_of::coboundary_range<Domain, viennagrid::line_tag>::type          EdgeOnVertexContainer;
    typedef typename viennagrid::result_of::iterator<EdgeOnVertexContainer>::type     EdgeOnVertexIterator;
    
    EdgeOnVertexContainer edges = viennagrid::coboundary_elements<viennagrid::line_tag>(domain, *vit);
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
  FacetContainer facets = viennagrid::elements<typename CellTag::facet_tag>(domain);
  for (FacetHandleIterator fit = facets.hook_begin();
       fit != facets.hook_end();
       ++fit)
  {
    const FacetType & facet = viennagrid::dereference_hook(domain, *fit);
    std::cout << facet << std::endl;
    typedef typename viennagrid::result_of::coboundary_range<Domain, CellTag>::type   CellOnFacetContainer;
    typedef typename viennagrid::result_of::iterator<CellOnFacetContainer>::type                          CellOnFacetIterator;
    
    CellOnFacetContainer cells = viennagrid::coboundary_elements<CellTag>(domain, *fit);
    for (CellOnFacetIterator eovit = cells.begin();
         eovit != cells.end();
         ++eovit)
    {
      std::cout << *eovit << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
  }

}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../../examples/data/";
  
  std::cout << "Testing 1d..." << std::endl;
  test<viennagrid::line_tag, viennagrid::config::line_1d_domain>(path + "line8.mesh");
  std::cout << "Testing 2d..." << std::endl;
  test<viennagrid::triangle_tag, viennagrid::config::triangular_2d_domain>(path + "square32.mesh");
  std::cout << "Testing 3d..." << std::endl;
  test<viennagrid::tetrahedron_tag, viennagrid::config::tetrahedral_3d_domain>(path + "cube48.mesh");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
