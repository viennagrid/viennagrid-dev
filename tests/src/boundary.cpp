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

#include "viennagrid/forwards.hpp"
//#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain/config.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/volume.hpp"

template <typename CellTypeOrTag, typename Domain, typename ReaderType>
void test(ReaderType & my_reader, std::string const & infile, double reference_surface)
{
  //typedef typename viennagrid::result_of::domain< DomainConfig >::type Domain;  
  typedef typename viennagrid::result_of::geometric_view<Domain>::type Segment;
  
  typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;
  //typedef viennagrid::result_of::element<Domain, CellTag>::type CellType;

    
  //typedef typename viennagrid::result_of::domain<ConfigType>::type        Domain;
  //typedef typename ConfigType::cell_tag                                   CellTag;
  //typedef typename viennagrid::result_of::segment<ConfigType>::type       SegmentType;
  
  typedef typename viennagrid::result_of::point_type<Domain>::type          PointType;
  typedef typename viennagrid::result_of::element<Domain, viennagrid::vertex_tag>::type       VertexType;
  typedef typename viennagrid::result_of::element<Domain, viennagrid::line_tag>::type       EdgeType;
  typedef typename viennagrid::result_of::element<Domain,
                                                typename CellTag::facet_tag>::type FacetType;
  typedef typename viennagrid::result_of::element<Domain,
                                                CellTag>::type   CellType;
                                            
  typedef typename viennagrid::result_of::element_range<Domain, viennagrid::vertex_tag>::type       VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type    VertexIterator;
                                            
  typedef typename viennagrid::result_of::element_range<Domain, viennagrid::line_tag>::type       EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type      EdgeIterator;

  typedef typename viennagrid::result_of::element_range<Domain, typename CellTag::facet_tag>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                         FacetIterator;

  typedef typename viennagrid::result_of::element_range<Domain, CellTag>::type     CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                          CellIterator;
  
  typedef typename viennagrid::result_of::element_range<EdgeType, viennagrid::vertex_tag>::type           VertexOnEdgeContainer;
  typedef typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type    VertexOnEdgeIterator;
  
  Domain domain;
  std::vector<Segment> segments;

  //read from file:
  try
  {
    my_reader(domain, segments, infile);
  }
  catch (std::exception const & ex)
  {
    std::cout << "what(): " << ex.what() << std::endl;
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    exit(EXIT_FAILURE);
  }


  //
  // Test 1: Iterate over all cells of each facet:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 1: Iteration over all facets on the boundary:" << std::endl;
  std::cout << "*" << std::endl;
  double surface = 0;
  FacetContainer facets = viennagrid::elements<typename CellTag::facet_tag>(domain);
  for (FacetIterator fit = facets.begin();
       fit != facets.end();
       ++fit)
  {
    if (viennagrid::is_boundary<CellTag>(*fit, domain))
    {
      std::cout << *fit << std::endl;
      surface += viennagrid::volume(*fit, domain);
    }
  }

  std::cout << "Volume of surface elements: " << surface << std::endl;
  
  if ( fabs(reference_surface - surface) / reference_surface > 1e-6 )
  {
    std::cout << "Surface area incorrect! Should: " << reference_surface << ", is: " << surface << std::endl;
    exit(EXIT_FAILURE);
  }
  
  //
  // Test 2: Print all vertices on the boundary:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 2: Iteration over all vertices on the boundary" << std::endl;
  std::cout << "*" << std::endl;
  VertexContainer vertices = viennagrid::elements<viennagrid::vertex_tag>(domain);
  for (VertexIterator vit = vertices.begin();
       vit != vertices.end();
       ++vit)
  {
    if (viennagrid::is_boundary<CellTag>(*vit, domain))
      std::cout << *vit << std::endl;
  }
  
}

int main()
{
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../../examples/data/";
  
  //viennagrid::io::netgen_reader my_netgen_reader;

  std::cout << "*********** line, 1d ***********" << std::endl;
  typedef viennagrid::result_of::domain< viennagrid::config::line_1d >::type Line1DDomain;
  viennagrid::io::netgen_reader<viennagrid::line_tag> line_1d_reader;
  test<viennagrid::line_tag, viennagrid::config::line_1d_domain>(line_1d_reader, path + "line8.mesh", 2);
  
  
  std::cout << "*********** triangular, 2d ***********" << std::endl;
  viennagrid::io::netgen_reader<viennagrid::triangle_tag> triangle_2d_reader;
  test<viennagrid::triangle_tag, viennagrid::config::triangular_2d_domain>(triangle_2d_reader, path + "square8.mesh", 4);
  
  std::cout << "*********** tetrahedral, 3d ***********" << std::endl;
  viennagrid::io::netgen_reader<viennagrid::tetrahedron_tag> tetrahedra_3d_reader;
  test<viennagrid::tetrahedron_tag, viennagrid::config::tetrahedral_3d_domain>(tetrahedra_3d_reader, path + "cube48.mesh", 6);

  
  std::cout << "*********** quadrilateral, 2d ***********" << std::endl;
  viennagrid::io::vtk_reader<viennagrid::quadrilateral_tag, viennagrid::config::quadrilateral_2d_domain>  quadrilateral_2d_reader;
  test<viennagrid::quadrilateral_tag, viennagrid::config::quadrilateral_2d_domain>(quadrilateral_2d_reader, path + "quadrilateral2.vtu", 6);
  
  std::cout << "*********** hexahedral, 3d ***********" << std::endl;
  viennagrid::io::vtk_reader<viennagrid::hexahedron_tag, viennagrid::config::hexahedral_3d_domain>  hexahedron_3d_reader;
  test<viennagrid::hexahedron_tag, viennagrid::config::hexahedral_3d_domain>(hexahedron_3d_reader, path + "hexahedron2.vtu", 10);
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
