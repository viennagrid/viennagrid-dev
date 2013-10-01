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

#include "viennagrid/mesh/accessor.hpp"

#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/netgen_reader.hpp"


int main()
{
  //typedef viennagrid::mesh_t< my_mesh_config >          Mesh;
  typedef viennagrid::mesh_t< viennagrid::config::tetrahedral_3d >     Mesh;
  typedef viennagrid::result_of::mesh_view<Mesh>::type               Segment;
  typedef viennagrid::result_of::segmentation<Mesh>::type              Segmentation;

  Mesh mesh;
  Segmentation segmentation(mesh);
        
  viennagrid::io::netgen_reader my_netgen_reader;
  my_netgen_reader(mesh, segmentation, "/export/florian/work/meshes/IUE/half-trigate.mesh");




  std::cout << viennagrid::vertices( segmentation[1] ).size() << std::endl;
  std::cout << viennagrid::lines( segmentation[1] ).size() << std::endl;
  std::cout << viennagrid::triangles( segmentation[1] ).size() << std::endl;
  

  viennagrid::io::vtk_writer<Mesh> writer;
  writer(mesh, segmentation, "trigate");

  
  return EXIT_SUCCESS;
}
