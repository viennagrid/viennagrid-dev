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
#define SGF
//#define VTK
//#define GTS

#include "viennagrid/domain.hpp"
#include "viennagrid/segment.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/opendx_writer.hpp"
//#include "viennagrid/io/gts_reader.hpp"
//#include "viennagrid/io/gts_writer.hpp"
#include "viennagrid/io/netgen_reader.hpp"


template <typename ConfigType, typename ReaderType>
void test(ReaderType & my_reader, std::string const & infile, std::string const & outfile)
{

  typedef typename viennagrid::result_of::domain<ConfigType>::type        Domain;
  typedef typename ConfigType::cell_tag                  CellTag;
  
  //typedef viennagrid::TestDomainConfig::DimensionTag              DimensionTag;
  typedef typename viennagrid::result_of::point<ConfigType>::type          PointType;
  typedef typename viennagrid::result_of::ncell<ConfigType, 0>::type       VertexType;
  typedef typename viennagrid::result_of::ncell<ConfigType,
                                                CellTag::topology_level>::type   CellType;
  //typedef viennagrid::DomainTypes<TestDomainConfig>::segment_type  Segment;

  typedef typename viennagrid::result_of::ncell_range<Domain, 0>::type   VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type        VertexIterator;
      
  typedef typename viennagrid::result_of::ncell_range<Domain, 1>::type   EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type          EdgeIterator;

  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::topology_level-1>::type   FacetContainer;
  typedef typename viennagrid::result_of::iterator<FacetContainer>::type                                 FacetIterator;

  typedef typename viennagrid::result_of::ncell_range<Domain, CellTag::topology_level>::type     CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                                  CellIterator;

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

  viennagrid::detectBoundary(domain);
  
  
  //write some dummy data:
  VertexContainer vertices = viennagrid::ncells<0>(domain);
  for (VertexIterator vit = vertices.begin();
      vit != vertices.end();
      ++vit)
  {
    viennadata::access<std::string, double>("vtk_data")(*vit) = vit->point()[0];
  }
  
  //test writers:
  viennagrid::io::vtk_writer<Domain> my_vtk_writer;
  viennagrid::io::add_scalar_data_on_vertices<std::string, double>(my_vtk_writer, "vtk_data", "data");
  viennagrid::io::add_scalar_data_on_vertices<std::string, double>(my_vtk_writer, "vtk_data_2", "data");
  my_vtk_writer(domain, outfile + ".vtu");
   
  viennagrid::io::opendx_writer my_dx_writer;
  my_dx_writer(domain, outfile + ".odx");

  return;
  
  //compilation tests:
  viennagrid::io::vtk_reader<Domain> my_vtk_reader;
  my_vtk_reader(domain, infile);
  
}


int main()
{
  typedef viennagrid::result_of::domain<viennagrid::config::triangular_2d>::type        Domain2d;
  typedef viennagrid::result_of::domain<viennagrid::config::tetrahedral_3d>::type       Domain3d;
  
  std::cout << "*****************" << std::endl;
  std::cout << "* Test started! *" << std::endl;
  std::cout << "*****************" << std::endl;
  
  std::string path = "../../examples/data/";
  
  #ifdef THREEDIM
  std::string infile = path + "cube384.mesh";
  #endif
  #ifdef TWODIM
  std::string infile = path + "square128.mesh";
  #endif
  #ifdef ONEDIM
  std::string infile = path + "line8.mesh";
  #endif
  std::string outfile = path + "out"; // without ending

  //Stage 1: Read from Netgen files, write to VTK
  viennagrid::io::netgen_reader my_netgen_reader;
  
  //test<viennagrid::config::line_1d>(my_netgen_reader, path + "line8.mesh", "io_1d");
  test<viennagrid::config::triangular_2d>(my_netgen_reader, path + "square128.mesh", "io_2d");
  test<viennagrid::config::tetrahedral_3d>(my_netgen_reader, path + "cube384.mesh", "io_3d");

  //Stage 2: Read VTK files, write to VTK
  //test<viennagrid::config::line_1d>("io_1d", "io_1d_2");
  
  viennagrid::io::vtk_reader<Domain2d>  vtk_reader_2d;
  //viennagrid::io::add_scalar_data_on_vertices<std::string, double>(vtk_reader_2d, "vtk_data_2", "vtk_data");
  test<viennagrid::config::triangular_2d>(vtk_reader_2d, "io_2d_0.vtu", "io_2d_2");
  
  viennagrid::io::vtk_reader<Domain3d>  vtk_reader_3d;
  //viennagrid::io::add_scalar_data_on_vertices<std::string, double>(vtk_reader_2d, "vtk_data_2", "vtk_data");
  test<viennagrid::config::tetrahedral_3d>(vtk_reader_3d, "io_3d_0.vtu", "io_3d_2");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;
}
