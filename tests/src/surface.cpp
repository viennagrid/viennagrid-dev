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
//#define VIENNAGRID_DEBUG_IO

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/config/others.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/opendx_writer.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/surface.hpp"


template <typename ConfigType, typename ReaderType>
void test(ReaderType & my_reader, std::string const & infile)
{

  typedef typename viennagrid::result_of::domain<ConfigType>::type        Domain;
  typedef typename viennagrid::result_of::segment<ConfigType>::type       SegmentType;
  typedef typename ConfigType::cell_tag                  CellTag;
  
  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                CellTag::topology_level>::type   CellType;

  typedef typename viennagrid::result_of::ncell_range<Domain, 0>::type   VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
  typedef typename viennagrid::result_of::ncell_range<SegmentType, 0>::type       SegmentVertexContainer;
  typedef typename viennagrid::result_of::iterator<SegmentVertexContainer>::type  SegmentVertexIterator;
      
  typedef typename viennagrid::result_of::ncell_range<Domain, 1>::type   EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::topology_level-1>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                         FacetIterator;

  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::topology_level>::type     CellRange;
  typedef typename viennagrid::result_of::iterator<CellRange>::type                              CellIterator;

  Domain domain;
  
  try
  {
    my_reader(domain, infile);
  }
  catch (std::exception const & ex)
  {
     std::cerr << ex.what() << std::endl;
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
     exit(EXIT_FAILURE);
  }

  std::cout << "Volume of domain: " << viennagrid::volume(domain) << std::endl;
  std::cout << "Surface of domain: " << viennagrid::surface(domain) << std::endl;
  
  CellRange cells = viennagrid::ncells(domain);
  for (CellIterator cit = cells.begin();
                    cit != cells.end();
                   ++cit)
  {
    //std::cout << *cit << std::endl;
    std::cout << "Surface: " << viennagrid::surface(*cit) << std::endl;
  }
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
  test<viennagrid::config::triangular_2d>(my_netgen_reader, path + "square32.mesh");
  
  std::cout << "*********** tetrahedral, 3d ***********" << std::endl;
  test<viennagrid::config::tetrahedral_3d>(my_netgen_reader, path + "cube48.mesh");

  
  std::cout << "*********** quadrilateral, 2d ***********" << std::endl;
  viennagrid::io::vtk_reader<Domain2d>  vtk_reader_2d;
  test<viennagrid::config::quadrilateral_2d>(vtk_reader_2d, path + "quadrilateral2.vtu");
  
  std::cout << "*********** hexahedral, 3d ***********" << std::endl;
  viennagrid::io::vtk_reader<Domain3d>  vtk_reader_3d;
  test<viennagrid::config::hexahedral_3d>(vtk_reader_3d, path + "hexahedron2.vtu");

  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
