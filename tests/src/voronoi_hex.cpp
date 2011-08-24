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

#include "viennagrid/config/others.hpp"
#include "voronoi_common.hpp"
#include "refinement-common.hpp"

template <typename DomainType, typename VertexType>
void add_plane(DomainType & device, VertexType & vertex)
{
  //
  vertex.getPoint()[0] = 0;   // #0
  vertex.getPoint()[1] = 0;
  device.add(vertex);
  
  vertex.getPoint()[0] = 2;   // #1
  vertex.getPoint()[1] = 0;
  device.add(vertex);

  vertex.getPoint()[0] = 1;   // #2
  vertex.getPoint()[1] = 1;
  device.add(vertex);

  vertex.getPoint()[0] = 0;   // #3
  vertex.getPoint()[1] = 2;
  device.add(vertex);
  
  vertex.getPoint()[0] = -1;   // #4
  vertex.getPoint()[1] = 1;
  device.add(vertex);
  
  vertex.getPoint()[0] = -2;   // #5
  vertex.getPoint()[1] = 0;
  device.add(vertex);
  
  vertex.getPoint()[0] = -1;   // #6
  vertex.getPoint()[1] = -1;
  device.add(vertex);
  
  vertex.getPoint()[0] = 0;   // #7
  vertex.getPoint()[1] = -2;
  device.add(vertex);
  
  vertex.getPoint()[0] = 1;   // #8
  vertex.getPoint()[1] = -1;
  device.add(vertex);
  
}


template <typename DomainType, typename CellType>
void add_cellplane(DomainType & device, CellType & cell, std::size_t offset)
{
  typedef typename DomainType::config_type           Config;
  typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;

  VertexType *vertices[8];
  
  vertices[0] = &(device.vertex(0 + offset));
  vertices[1] = &(device.vertex(8 + offset));
  vertices[2] = &(device.vertex(1 + offset));
  vertices[3] = &(device.vertex(2 + offset));
  vertices[4] = &(device.vertex(9 + offset));
  vertices[5] = &(device.vertex(17 + offset));
  vertices[6] = &(device.vertex(10 + offset));
  vertices[7] = &(device.vertex(11 + offset));
  cell.setVertices(vertices);
  device.add(cell);

  vertices[0] = &(device.vertex(0 + offset));
  vertices[1] = &(device.vertex(2 + offset));
  vertices[2] = &(device.vertex(3 + offset));
  vertices[3] = &(device.vertex(4 + offset));
  vertices[4] = &(device.vertex(9 + offset));
  vertices[5] = &(device.vertex(11 + offset));
  vertices[6] = &(device.vertex(12 + offset));
  vertices[7] = &(device.vertex(13 + offset));
  cell.setVertices(vertices);
  device.add(cell);

  vertices[0] = &(device.vertex(0 + offset));
  vertices[1] = &(device.vertex(4 + offset));
  vertices[2] = &(device.vertex(5 + offset));
  vertices[3] = &(device.vertex(6 + offset));
  vertices[4] = &(device.vertex(9 + offset));
  vertices[5] = &(device.vertex(13 + offset));
  vertices[6] = &(device.vertex(14 + offset));
  vertices[7] = &(device.vertex(15 + offset));
  cell.setVertices(vertices);
  device.add(cell);

  vertices[0] = &(device.vertex(0 + offset));
  vertices[1] = &(device.vertex(6 + offset));
  vertices[2] = &(device.vertex(7 + offset));
  vertices[3] = &(device.vertex(8 + offset));
  vertices[4] = &(device.vertex(9 + offset));
  vertices[5] = &(device.vertex(15 + offset));
  vertices[6] = &(device.vertex(16 + offset));
  vertices[7] = &(device.vertex(17 + offset));
  cell.setVertices(vertices);
  device.add(cell);
}

//
// Generate four rectangles
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
  device.reserve_vertices(27);

  vertex[2] = 0.0;
  add_plane(device, vertex);
  vertex[2] = 1.0;
  add_plane(device, vertex);
  vertex[2] = 2.0;
  add_plane(device, vertex);
  
  //
  // Step 2: Set up cells:
  //
  
  CellType cell;
  device.reserve_cells(8);
  add_cellplane(device, cell, 0);
  add_cellplane(device, cell, 9);
}




int main(int argc, char *argv[])
{
  typedef viennagrid::config::hexahedron_3d           Config;
  typedef viennagrid::domain<Config>   DeviceType;
  
  std::cout << "* main(): Creating device..." << std::endl;
  DeviceType device;
  
  //create device:
  setup_device(device);
  
  //set up dual grid info:
  viennagrid::write_voronoi_info<edge_len_key, edge_interface_area_key, box_volume_key>(device);
                                      
  //output results:
  output_voronoi_info(device);
  
  std::cout << std::endl;
  std::cout << "Circumcenter of cell #0: " << viennagrid::circumcenter(viennagrid::ncells<3>(device)[0]) << std::endl;
  std::cout << "Circumcenter of cell #1: " << viennagrid::circumcenter(viennagrid::ncells<3>(device)[1]) << std::endl;
  std::cout << "Circumcenter of cell #2: " << viennagrid::circumcenter(viennagrid::ncells<3>(device)[2]) << std::endl;
  std::cout << "Circumcenter of cell #3: " << viennagrid::circumcenter(viennagrid::ncells<3>(device)[3]) << std::endl;
  std::cout << "Circumcenter of cell #4: " << viennagrid::circumcenter(viennagrid::ncells<3>(device)[4]) << std::endl;
  std::cout << "Circumcenter of cell #5: " << viennagrid::circumcenter(viennagrid::ncells<3>(device)[5]) << std::endl;
  std::cout << "Circumcenter of cell #6: " << viennagrid::circumcenter(viennagrid::ncells<3>(device)[6]) << std::endl;
  std::cout << "Circumcenter of cell #7: " << viennagrid::circumcenter(viennagrid::ncells<3>(device)[7]) << std::endl;
  std::cout << std::endl;
  
  double voronoi_vol = voronoi_volume(device);  
  double domain_vol = viennagrid::volume(device);  
  
  if ( fabs(voronoi_vol - domain_vol) / domain_vol > 1e-10 )
  {
    std::cerr << "Mismatch of volumes: " << voronoi_vol << " vs " << domain_vol << std::endl;
    return EXIT_FAILURE;
  }
  else
    std::cout << "Volume check passed: " << voronoi_vol << " vs " << domain_vol << std::endl;
  
  
  //check dual box volume of vertex 9: must be 2
  double volume9 = viennadata::access<box_volume_key, double>()(viennagrid::ncells<0>(device)[9]);
  if ( fabs(volume9 - 2.0) > 1e-10 )
  {
    std::cerr << "Incorrect dual box volume at vertex #9: " << volume9 << std::endl;
    return EXIT_FAILURE;
  }
  else
    std::cout << "Dual volume check at vertex #9 passed." << std::endl;
  
  //write to vtk:
  viennagrid::io::vtk_writer<DeviceType> my_vtk_writer;
  my_vtk_writer.writeDomain(device, "voronoi_hex");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;  
}

