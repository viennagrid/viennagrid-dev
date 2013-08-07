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

#ifdef _MSC_VER      //Visual Studio complains about potentially dangerous things, which are perfectly legal in our context
  #pragma warning( disable : 4355 )     //use of this in member initializer list
  #pragma warning( disable : 4503 )     //truncated name decoration
#endif

#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/config/default_configs.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/opendx_writer.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/surface.hpp"


template <typename DomainType, typename ReaderType>
void test(ReaderType & my_reader, std::string const & infile)
{
  typedef typename viennagrid::result_of::cell_tag<DomainType>::type              CellTag;
  typedef typename viennagrid::result_of::segmentation<DomainType>::type          SegmentationType;

  typedef typename viennagrid::result_of::cell_range<DomainType>::type            CellRange;
  typedef typename viennagrid::result_of::iterator<CellRange>::type               CellIterator;

  DomainType domain;
  SegmentationType segmentation(domain);
  
  try
  {
    my_reader(domain, segmentation, infile);
  }
  catch (std::exception const & ex)
  {
     std::cerr << ex.what() << std::endl;
     std::cerr << "File-Reader failed. Aborting program..." << std::endl;
     exit(EXIT_FAILURE);
  }

  std::cout << "Volume of domain: " << viennagrid::volume(domain) << std::endl;
  std::cout << "Surface of domain: " << viennagrid::surface(domain) << std::endl;
  
  CellRange cells = viennagrid::elements(domain);
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
//   typedef viennagrid::result_of::domain<viennagrid::config::quadrilateral_2d>::type        Domain2d;
//   typedef viennagrid::result_of::domain<viennagrid::config::hexahedral_3d>::type       Domain3d;
  
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../../examples/data/";
  
  viennagrid::io::netgen_reader my_netgen_reader;
  
  std::cout << "*********** triangular, 2d ***********" << std::endl;
  test<viennagrid::triangular_2d_domain>(my_netgen_reader, path + "square32.mesh");
  
  std::cout << "*********** tetrahedral, 3d ***********" << std::endl;
  test<viennagrid::tetrahedral_3d_domain>(my_netgen_reader, path + "cube48.mesh");

  
  std::cout << "*********** quadrilateral, 2d ***********" << std::endl;
  viennagrid::io::vtk_reader<viennagrid::quadrilateral_2d_domain>  vtk_reader_2d;
  test<viennagrid::quadrilateral_2d_domain>(vtk_reader_2d, path + "quadrilateral2.vtu");
  
  std::cout << "*********** hexahedral, 3d ***********" << std::endl;
  viennagrid::io::vtk_reader<viennagrid::hexahedral_3d_domain>  vtk_reader_3d;
  test<viennagrid::hexahedral_3d_domain 
  >(vtk_reader_3d, path + "hexahedron2.vtu");

  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
