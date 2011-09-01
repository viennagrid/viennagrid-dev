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

#define VIENNAGRID_DEBUG_IO

#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/config/others.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/volume.hpp"

template <typename ConfigType, typename ReaderType>
void test(ReaderType & my_reader, std::string const & infile)
{

  typedef typename viennagrid::result_of::domain<ConfigType>::type        Domain;
  typedef typename ConfigType::cell_tag                                   CellTag;
  typedef typename viennagrid::result_of::segment<ConfigType>::type       SegmentType;
  
  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 1>::type       EdgeType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                CellTag::topology_level-1>::type FacetType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                CellTag::topology_level>::type   CellType;
                                            
  typedef typename viennagrid::result_of::ncell_range<Domain, 0>::type       VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type    VertexIterator;
                                            
  typedef typename viennagrid::result_of::ncell_range<Domain, 1>::type       EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type      EdgeIterator;

  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::topology_level-1>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                         FacetIterator;

  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::topology_level>::type     CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                          CellIterator;
  
  typedef typename viennagrid::result_of::ncell_range<EdgeType, 0>::type           VertexOnEdgeContainer;
  typedef typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type    VertexOnEdgeIterator;
  
  Domain domain;

  //read from file:
  try
  {
    my_reader(domain, infile);
  }
  catch (std::exception const & ex)
  {
    std::cout << "what(): " << ex.what() << std::endl;
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    exit(EXIT_FAILURE);
  }

  //viennagrid::detect_boundary();

  //
  // Test 1: Iterate over all edges of each vertex:
  //
  /*
  std::cout << "*" << std::endl;
  std::cout << "* Test 2: Iteration over all edges adjacent to each vertex" << std::endl;
  std::cout << "*" << std::endl;
  VertexContainer vertices = viennagrid::ncells<0>(domain);
  for (VertexIterator vit = vertices.begin();
       vit != vertices.end();
       ++vit)
  {
    std::cout << *vit << std::endl;
    typedef typename viennagrid::result_of::ncell_range<VertexType, 1>::type          EdgeOnVertexContainer;
    typedef typename viennagrid::result_of::iterator<EdgeOnVertexContainer>::type     EdgeOnVertexIterator;
    
    EdgeOnVertexContainer edges = viennagrid::ncells<1>(*vit, domain);
    for (EdgeOnVertexIterator eovit = edges.begin();
         eovit != edges.end();
         ++eovit)
    {
      std::cout << *eovit << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
  }*/

  //
  // Test 2: Iterate over all cells of each facet:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 2: Iteration over all facets on the boundary:" << std::endl;
  std::cout << "*" << std::endl;
  double surface = 0;
  FacetContainer facets = viennagrid::ncells<CellTag::topology_level-1>(domain);
  for (FacetIterator fit = facets.begin();
       fit != facets.end();
       ++fit)
  {
    if (viennagrid::on_boundary(*fit, domain))
    {
      std::cout << *fit << std::endl;
      surface += viennagrid::volume(*fit);
    }
  }

  std::cout << "Volume of surface elements: " << surface << std::endl;
}

int main()
{
  typedef viennagrid::result_of::domain<viennagrid::config::quadrilateral_2d>::type        Domain2d;
  typedef viennagrid::result_of::domain<viennagrid::config::hexahedral_3d>::type       Domain3d;
  
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../../examples/data/";
  
  viennagrid::io::netgen_reader my_netgen_reader;
  
  std::cout << "*********** triangular, 2d ***********" << std::endl;
  test<viennagrid::config::triangular_2d>(my_netgen_reader, path + "square8.mesh");
  
  /*std::cout << "*********** tetrahedral, 3d ***********" << std::endl;
  test<viennagrid::config::tetrahedral_3d>(my_netgen_reader, path + "cube48.mesh");

  
  std::cout << "*********** quadrilateral, 2d ***********" << std::endl;
  viennagrid::io::vtk_reader<Domain2d>  vtk_reader_2d;
  test<viennagrid::config::quadrilateral_2d>(vtk_reader_2d, path + "quadrilateral2.vtu");
  
  std::cout << "*********** hexahedral, 3d ***********" << std::endl;
  viennagrid::io::vtk_reader<Domain3d>  vtk_reader_3d;
  test<viennagrid::config::hexahedral_3d>(vtk_reader_3d, path + "hexahedron2.vtu");*/
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
