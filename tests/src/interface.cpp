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


#include "viennagrid/forwards.h"
#include "viennagrid/element.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/config/others.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/algorithm/interface.hpp"
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

  assert(domain.segments().size() > 1 && "Not enough segments!");
  
  SegmentType const & seg1 = domain.segments()[0];
  SegmentType const & seg2 = domain.segments()[1];

  //
  // Test 1: Iterate over all interface facets:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 1: Iteration over all facets on the boundary:" << std::endl;
  std::cout << "*" << std::endl;
  double surface = 0;
  FacetContainer facets = viennagrid::ncells<CellTag::topology_level-1>(domain);
  for (FacetIterator fit = facets.begin();
       fit != facets.end();
       ++fit)
  {
    if (viennagrid::is_interface(*fit, seg1, seg2))
    {
      std::cout << *fit << std::endl;
      surface += viennagrid::volume(*fit);
    }
  }

  std::cout << "Volume of interface elements: " << surface << std::endl;
  
  /*if ( fabs(reference_surface - surface) / reference_surface > 1e-6 )
  {
    std::cout << "Surface area incorrect! Should: " << reference_surface << ", is: " << surface << std::endl;
    exit(EXIT_FAILURE);
  }*/
  
  //
  // Test 2: Print all vertices on the interface:
  //
  std::cout << "*" << std::endl;
  std::cout << "* Test 2: Iteration over all vertices on the interface" << std::endl;
  std::cout << "*" << std::endl;
  bool vertices_found = false;
  VertexContainer vertices = viennagrid::ncells<0>(domain);
  for (VertexIterator vit = vertices.begin();
       vit != vertices.end();
       ++vit)
  {
    if (viennagrid::is_interface(*vit, seg1, seg2))
    {
      assert(vit->point()[0] == 0.5 && "Invalid point detected on interface!");
      std::cout << *vit << std::endl;
      vertices_found = true;
    }
  }
  
  if (!vertices_found)
  {
    std::cerr << "ERROR: No vertices found!" << std::endl;
    exit(EXIT_FAILURE);
  }
}

int main()
{
  typedef viennagrid::result_of::domain<viennagrid::config::triangular_2d>::type        DomainTri;
  typedef viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type       DomainTet;
  typedef viennagrid::result_of::domain<viennagrid::config::quadrilateral_2d>::type        DomainQuad;
  typedef viennagrid::result_of::domain<viennagrid::config::hexahedral_3d>::type       DomainHex;
  
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../../examples/data/";
  
  std::cout << "*********** triangular, 2d ***********" << std::endl;
  viennagrid::io::vtk_reader<DomainTri>  vtk_reader_tri;
  test<viennagrid::config::triangular_2d>(vtk_reader_tri, path + "multi_segment_tri_main.pvd");
  
  std::cout << "*********** tetrahedral, 3d ***********" << std::endl;
  viennagrid::io::vtk_reader<DomainTet>  vtk_reader_tet;
  test<viennagrid::config::tetrahedral_3d>(vtk_reader_tet, path + "multi_segment_tet_main.pvd");

  
  std::cout << "*********** quadrilateral, 2d ***********" << std::endl;
  viennagrid::io::vtk_reader<DomainQuad>  vtk_reader_quad;
  test<viennagrid::config::quadrilateral_2d>(vtk_reader_quad, path + "multi_segment_quad_main.pvd");
  
  std::cout << "*********** hexahedral, 3d ***********" << std::endl;
  viennagrid::io::vtk_reader<DomainHex>  vtk_reader_hex;
  test<viennagrid::config::hexahedral_3d>(vtk_reader_hex, path + "multi_segment_hex_main.pvd");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
