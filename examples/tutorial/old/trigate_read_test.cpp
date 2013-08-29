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

#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/domain/accessor.hpp"

#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/netgen_reader.hpp"


int main()
{
  //typedef viennagrid::domain_t< my_domain_config >          Domain;
  typedef viennagrid::domain_t< viennagrid::config::tetrahedral_3d >     Domain;
  typedef viennagrid::result_of::domain_view<Domain>::type               Segment;
  typedef viennagrid::result_of::segmentation<Domain>::type              Segmentation;

  Domain domain;
  Segmentation segmentation(domain);
        
  viennagrid::io::netgen_reader my_netgen_reader;
  my_netgen_reader(domain, segmentation, "/export/florian/work/meshes/IUE/half-trigate.mesh");




  std::cout << viennagrid::vertices( segmentation[1] ).size() << std::endl;
  std::cout << viennagrid::lines( segmentation[1] ).size() << std::endl;
  std::cout << viennagrid::triangles( segmentation[1] ).size() << std::endl;
  

  viennagrid::io::vtk_writer<Domain> writer;
  writer(domain, segmentation, "trigate");

  
  return EXIT_SUCCESS;
}
