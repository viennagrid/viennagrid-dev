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

#include <iostream>
#include <vector>


#include "viennagrid/domain.hpp"
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/iterators.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/algorithm/voronoi.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"

#include "voronoi_common.hpp"
#include "refinement-common.hpp"


int main(int argc, char *argv[])
{
  typedef viennagrid::config::tetrahedral_3d          Config;
  typedef viennagrid::result_of::domain<Config>::type                  DomainType;
  
  std::cout << "* main(): Creating device..." << std::endl;
  DomainType my_domain;

  std::string path = "../../examples/data/";
  
  //create device:
  try
  {
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(my_domain, path + "cube48.mesh");
  }
  catch (...)
  {
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    return EXIT_FAILURE;
  }
  
  //set up dual grid info:
  viennagrid::apply_voronoi(my_domain);
  
  //output results:
  output_voronoi_info(my_domain);
  
  
  std::cout << std::endl;
  std::cout << viennagrid::ncells<3>(my_domain)[0] << std::endl;
  std::cout << std::endl;
  std::cout << "Circumcenter of first cell: " << viennagrid::circumcenter(viennagrid::ncells<3>(my_domain)[0]) << std::endl;

  double voronoi_vol = voronoi_volume(my_domain);
  double domain_vol = viennagrid::volume(my_domain);  
  
  if ( fabs(voronoi_vol - domain_vol) / domain_vol > 1e-10 )
  {
    std::cerr << "Mismatch of volumes: " << voronoi_vol << " vs " << domain_vol << std::endl;
    return EXIT_FAILURE;
  }
  else
    std::cout << "Volume check passed: " << voronoi_vol << " vs " << domain_vol << std::endl;
  
  //write to vtk:
  viennagrid::io::vtk_writer<DomainType> my_vtk_writer;
  my_vtk_writer(my_domain, "voronoi_tet");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;  
}

