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

#include "viennagrid/config/others.hpp"
#include "voronoi_common.hpp"
#include "refinement-common.hpp"

//
// Generate four rectangles
//
template <typename DeviceType>
void setup_device(DeviceType & device)
{
  typedef typename DeviceType::config_type           Config;
  typedef typename Config::cell_tag                  CellTag;
  
  typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
  typedef typename viennagrid::result_of::ncell<Config, CellTag::dim>::type   CellType;
  
  
  //
  // Step 1: Set up vertices:
  //
  VertexType vertex;

  vertex.point()[0] = 0;   // #0
  vertex.point()[1] = 0;
  device.push_back(vertex);
  
  vertex.point()[0] = 2;   // #1
  vertex.point()[1] = 0;
  device.push_back(vertex);

  vertex.point()[0] = 1;   // #2
  vertex.point()[1] = 1;
  device.push_back(vertex);

  vertex.point()[0] = 0;   // #3
  vertex.point()[1] = 2;
  device.push_back(vertex);
  
  vertex.point()[0] = -1;   // #4
  vertex.point()[1] = 1;
  device.push_back(vertex);
  
  vertex.point()[0] = -2;   // #5
  vertex.point()[1] = 0;
  device.push_back(vertex);
  
  vertex.point()[0] = -1;   // #6
  vertex.point()[1] = -1;
  device.push_back(vertex);
  
  vertex.point()[0] = 0;   // #7
  vertex.point()[1] = -2;
  device.push_back(vertex);
  
  vertex.point()[0] = 1;   // #8
  vertex.point()[1] = -1;
  device.push_back(vertex);
  
  
  //
  // Step 2: Set up cells:
  //
  
  CellType cell;
  VertexType *vertices[4];
  
  vertices[0] = &(viennagrid::ncells<0>(device)[0]);
  vertices[1] = &(viennagrid::ncells<0>(device)[8]);
  vertices[2] = &(viennagrid::ncells<0>(device)[2]);
  vertices[3] = &(viennagrid::ncells<0>(device)[1]);
  cell.vertices(vertices);
  device.push_back(cell);

  vertices[0] = &(viennagrid::ncells<0>(device)[0]);
  vertices[1] = &(viennagrid::ncells<0>(device)[2]);
  vertices[2] = &(viennagrid::ncells<0>(device)[4]);
  vertices[3] = &(viennagrid::ncells<0>(device)[3]);
  cell.vertices(vertices);
  device.push_back(cell);

  vertices[0] = &(viennagrid::ncells<0>(device)[0]);
  vertices[1] = &(viennagrid::ncells<0>(device)[4]);
  vertices[2] = &(viennagrid::ncells<0>(device)[6]);
  vertices[3] = &(viennagrid::ncells<0>(device)[5]);
  cell.vertices(vertices);
  device.push_back(cell);

  vertices[0] = &(viennagrid::ncells<0>(device)[0]);
  vertices[1] = &(viennagrid::ncells<0>(device)[6]);
  vertices[2] = &(viennagrid::ncells<0>(device)[8]);
  vertices[3] = &(viennagrid::ncells<0>(device)[7]);
  cell.vertices(vertices);
  device.push_back(cell);

}




int main(int argc, char *argv[])
{
  typedef viennagrid::config::quadrilateral_2d           Config;
  typedef viennagrid::result_of::domain<Config>::type   DeviceType;
  
  std::cout << "* main(): Creating device..." << std::endl;
  DeviceType device;
  
  //create device:
  setup_device(device);
  
  viennagrid::voronoi_interface_area_key interface_key;
  viennagrid::voronoi_box_volume_key box_volume_key;
  
  //set up dual grid info:
  viennagrid::apply_voronoi(device, interface_key, box_volume_key);
                                      
  //output results:
  output_voronoi_info(device);
  
  std::cout << std::endl;
  std::cout << viennagrid::ncells<2>(device)[0] << std::endl;
  std::cout << std::endl;
  std::cout << "Circumcenter of cell #0: " << viennagrid::circumcenter(viennagrid::ncells<2>(device)[0]) << std::endl;
  std::cout << "Circumcenter of cell #1: " << viennagrid::circumcenter(viennagrid::ncells<2>(device)[1]) << std::endl;
  std::cout << "Circumcenter of cell #2: " << viennagrid::circumcenter(viennagrid::ncells<2>(device)[2]) << std::endl;
  std::cout << "Circumcenter of cell #3: " << viennagrid::circumcenter(viennagrid::ncells<2>(device)[3]) << std::endl;
  
  // Check Voronoi volumes:
  voronoi_volume_check(device);
  
  //write to vtk:
  viennagrid::io::vtk_writer<DeviceType> my_vtk_writer;
  my_vtk_writer(device, "voronoi_rect");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;  
}

