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
  
  typedef typename viennagrid::result_of::point<Config>::type                            PointType;
  typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
  typedef typename viennagrid::result_of::ncell<Config, CellTag::dim>::type   CellType;
  
  
  //
  // Step 1: Set up vertices:
  //
  device.push_back(PointType(0));       // #0
  device.push_back(PointType(0.25));    // #1
  device.push_back(PointType(0.5));     // #2
  device.push_back(PointType(0.75));    // #3
  device.push_back(PointType(1.0));     // #4
  
  //
  // Step 2: Set up cells:
  //
  
  CellType cell;
  VertexType *vertices[2];
  
  vertices[0] = &(viennagrid::ncells<0>(device)[0]);
  vertices[1] = &(viennagrid::ncells<0>(device)[1]);
  cell.vertices(vertices);
  device.push_back(cell);

  vertices[0] = &(viennagrid::ncells<0>(device)[1]);
  vertices[1] = &(viennagrid::ncells<0>(device)[2]);
  cell.vertices(vertices);
  device.push_back(cell);

  vertices[0] = &(viennagrid::ncells<0>(device)[2]);
  vertices[1] = &(viennagrid::ncells<0>(device)[3]);
  cell.vertices(vertices);
  device.push_back(cell);

  vertices[0] = &(viennagrid::ncells<0>(device)[3]);
  vertices[1] = &(viennagrid::ncells<0>(device)[4]);
  cell.vertices(vertices);
  device.push_back(cell);
  
}




int main(int argc, char *argv[])
{
  typedef viennagrid::config::line_1d                   Config;
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
  std::cout << viennagrid::ncells<1>(device)[0] << std::endl;
  std::cout << std::endl;
  std::cout << "Circumcenter of cell #0: " << viennagrid::circumcenter(viennagrid::ncells<1>(device)[0]) << std::endl;
  std::cout << "Circumcenter of cell #1: " << viennagrid::circumcenter(viennagrid::ncells<1>(device)[1]) << std::endl;
  std::cout << "Circumcenter of cell #2: " << viennagrid::circumcenter(viennagrid::ncells<1>(device)[2]) << std::endl;
  std::cout << "Circumcenter of cell #3: " << viennagrid::circumcenter(viennagrid::ncells<1>(device)[3]) << std::endl;
  std::cout << "-----------------------" << std::endl;
  std::cout << "Centroid of cell #0: " << viennagrid::centroid(viennagrid::ncells<1>(device)[0]) << std::endl;
  std::cout << "Centroid of cell #1: " << viennagrid::centroid(viennagrid::ncells<1>(device)[1]) << std::endl;
  std::cout << "Centroid of cell #2: " << viennagrid::centroid(viennagrid::ncells<1>(device)[2]) << std::endl;
  std::cout << "Centroid of cell #3: " << viennagrid::centroid(viennagrid::ncells<1>(device)[3]) << std::endl;
  
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
  my_vtk_writer(device, "voronoi_line");
  
  std::cout << "*******************************" << std::endl;
  std::cout << "* Test finished successfully! *" << std::endl;
  std::cout << "*******************************" << std::endl;
  
  return EXIT_SUCCESS;  
}

