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

#include <iostream>
#include <cstdlib>
#include <sys/time.h>
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


//
// Generate a single tetrahedron
//
template <typename DeviceType>
void setup_device(DeviceType & device)
{
  typedef typename DeviceType::config_type           Config;
  typedef typename Config::cell_tag                  CellTag;
  
  typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
  typedef typename viennagrid::result_of::ncell<Config, CellTag::topology_level>::type   CellType;
  
  
  //
  // Step 1: Set up vertices:
  //
  VertexType vertex;
  device.reserve_vertices(5);

  vertex.getPoint()[0] = 0;   // #0
  vertex.getPoint()[1] = 0;
  vertex.getPoint()[2] = 0;
  device.add(vertex);
  
  vertex.getPoint()[0] = 1;   // #1
  vertex.getPoint()[1] = 0;
  vertex.getPoint()[2] = 0;
  device.add(vertex);

  vertex.getPoint()[0] = 0;   // #2
  vertex.getPoint()[1] = 1;
  vertex.getPoint()[2] = 0;
  device.add(vertex);

  vertex.getPoint()[0] = 0;   // #3
  vertex.getPoint()[1] = 0;
  vertex.getPoint()[2] = 1;
  device.add(vertex);
  
  vertex.getPoint()[0] = 1;   // #4
  vertex.getPoint()[1] = 0;
  vertex.getPoint()[2] = 1;
  device.add(vertex);
  
  
  //
  // Step 2: Set up cells:
  //
  
  CellType cell;
  VertexType *vertices[4];
  device.reserve_cells(2);
  
  vertices[0] = &(device.vertex(0));
  vertices[1] = &(device.vertex(1));
  vertices[2] = &(device.vertex(2));
  vertices[3] = &(device.vertex(3));
  cell.setVertices(vertices);
  device.add(cell);
  
  vertices[0] = &(device.vertex(3));
  vertices[1] = &(device.vertex(1));
  vertices[2] = &(device.vertex(2));
  vertices[3] = &(device.vertex(4));
  cell.setVertices(vertices);
  device.add(cell);
  
}



int main(int argc, char *argv[])
{
  typedef viennagrid::config::tetrahedral_3d          Config;
  typedef viennagrid::domain<Config>                  DomainType;
  
  std::cout << "* main(): Creating device..." << std::endl;
  DomainType my_domain;
  
  //create device:
  //setup_device(my_domain);
  try
  {
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(my_domain, "../../examples/data/cube48.mesh");
  }
  catch (...)
  {
    std::cerr << "File-Reader failed. Aborting program..." << std::endl;
    return EXIT_FAILURE;
  }
  
  
  //set up dual grid info:
  viennagrid::write_voronoi_info<edge_len_key,
                                 edge_interface_area_key,
                                 box_volume_key>(my_domain);
  
  //output results:
  output_voronoi_info(my_domain);
  
  
  std::cout << std::endl;
  viennagrid::ncells<3>(my_domain)[0].print_short();
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
  my_vtk_writer.writeDomain(my_domain, "voronoi_tet");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;  
}

