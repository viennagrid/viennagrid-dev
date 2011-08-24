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
#include <vector>


#include "voronoi_common.hpp"
#include "refinement-common.hpp"

//
// Generate a ring of six triangles:
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
  device.reserve_vertices(9);

  vertex.getPoint()[0] = 0;   // #0
  vertex.getPoint()[1] = 0;
  device.add(vertex);
  
  vertex.getPoint()[0] = 2;   // #1
  vertex.getPoint()[1] = 1;
  device.add(vertex);

  vertex.getPoint()[0] = 1;   // #2
  vertex.getPoint()[1] = 2;
  device.add(vertex);

  vertex.getPoint()[0] = -1;   // #3
  vertex.getPoint()[1] =  2;
  device.add(vertex);
  
  vertex.getPoint()[0] = -2;   // #4
  vertex.getPoint()[1] =  1;
  device.add(vertex);
  
  vertex.getPoint()[0] = -2;   // #5
  vertex.getPoint()[1] = -1;
  device.add(vertex);
  
  vertex.getPoint()[0] = -1;   // #6
  vertex.getPoint()[1] = -2;
  device.add(vertex);
  
  vertex.getPoint()[0] =  1;   // #7
  vertex.getPoint()[1] = -2;
  device.add(vertex);
  
  vertex.getPoint()[0] =  2;   // #8
  vertex.getPoint()[1] = -1;
  device.add(vertex);
  
  
  //
  // Step 2: Set up cells:
  //
  
  CellType cell;
  VertexType *vertices[3];
  device.reserve_cells(8);
  
  vertices[0] = &(device.vertex(8));
  vertices[1] = &(device.vertex(1));
  vertices[2] = &(device.vertex(0));
  cell.setVertices(vertices);
  device.add(cell);

  for (size_t i=1; i<8; ++i)
  {
    vertices[0] = &(device.vertex(i));
    vertices[1] = &(device.vertex(i+1));
    vertices[2] = &(device.vertex(0));
    cell.setVertices(vertices);
    device.add(cell);
  }

  
}

int main(int argc, char *argv[])
{
  typedef viennagrid::config::triangular_2d           Config;
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
  viennagrid::ncells<2>(device)[0].print_short();
  std::cout << std::endl;
  std::cout << "Circumcenter of first cell: " << viennagrid::circumcenter(device.cells(0)) << std::endl;
  std::cout << "Area of first cell: " << viennagrid::spanned_volume(device.vertex(0).getPoint(),
                                                                    device.vertex(1).getPoint(),
                                                                    device.vertex(8).getPoint()) << std::endl;

  double voronoi_vol = voronoi_volume(device);  
  double domain_vol = viennagrid::volume(device);  
  
  if ( fabs(voronoi_vol - domain_vol) / domain_vol > 1e-10 )
  {
    std::cerr << "Mismatch of volumes: " << voronoi_vol << " vs " << domain_vol << std::endl;
    return EXIT_FAILURE;
  }
  else
    std::cout << "Volume check passed: " << voronoi_vol << " vs " << domain_vol << std::endl;

  //write to vtk:
  viennagrid::io::vtk_writer<DeviceType> my_vtk_writer;
  my_vtk_writer.writeDomain(device, "voronoi_tri");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;  
}

