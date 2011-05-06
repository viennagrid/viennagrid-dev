/* =======================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
       ViennaSHE - The Vienna Spherical Harmonics Expansion Simulator
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at
               Markus Bina                        bina@iue.tuwien.ac.at

   license:    To be discussed, see file LICENSE in the ViennaSHE base directory
======================================================================= */

#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <vector>


#include "viennagrid/domain.hpp"
#include "viennagrid/io/sgf_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/iterators.hpp"
#include "viennagrid/algorithm/voronoi.hpp"
#include "viennagrid/config/others.hpp"



struct edge_len_key {};              //edge lengths
struct box_volume_key {};            //box volume associated with an edge or vertex
struct edge_interface_area_key {};   //box volume associated with an edge

namespace viennadata
{
  namespace config
  {
    template <>
    struct key_dispatch<edge_len_key>
    {
      typedef type_key_dispatch_tag    tag;
    };

    template <>
    struct key_dispatch<box_volume_key>
    {
      typedef type_key_dispatch_tag    tag;
    };
    
    template <>
    struct key_dispatch<edge_interface_area_key>
    {
      typedef type_key_dispatch_tag    tag;
    };
  }
}


//
// Generate four rectangles
//
template <typename DeviceType>
void setup_device(DeviceType & device)
{
  typedef typename DeviceType::config_type           Config;
  typedef typename Config::cell_tag                  CellTag;
  
  typedef typename viennagrid::result_of::ncell_type<Config, 0>::type                         VertexType;
  typedef typename viennagrid::result_of::ncell_type<Config, CellTag::topology_level>::type   CellType;
  
  
  //
  // Step 1: Set up vertices:
  //
  VertexType vertex;
  device.reserve_vertices(9);

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
  
  
  //
  // Step 2: Set up cells:
  //
  
  CellType cell;
  VertexType *vertices[4];
  device.reserve_cells(4);
  
  vertices[0] = &(device.vertex(0));
  vertices[1] = &(device.vertex(8));
  vertices[2] = &(device.vertex(1));
  vertices[3] = &(device.vertex(2));
  cell.setVertices(vertices);
  device.add(cell);

  vertices[0] = &(device.vertex(0));
  vertices[1] = &(device.vertex(2));
  vertices[2] = &(device.vertex(3));
  vertices[3] = &(device.vertex(4));
  cell.setVertices(vertices);
  device.add(cell);

  vertices[0] = &(device.vertex(0));
  vertices[1] = &(device.vertex(4));
  vertices[2] = &(device.vertex(5));
  vertices[3] = &(device.vertex(6));
  cell.setVertices(vertices);
  device.add(cell);

  vertices[0] = &(device.vertex(0));
  vertices[1] = &(device.vertex(6));
  vertices[2] = &(device.vertex(7));
  vertices[3] = &(device.vertex(8));
  cell.setVertices(vertices);
  device.add(cell);

}


template <typename DeviceType>
void output_voronoi_info(DeviceType const & d)
{
  typedef typename DeviceType::config_type           Config;
  typedef typename Config::cell_tag                  CellTag;
  typedef typename viennagrid::result_of::point_type<Config>::type                            PointType;
  typedef typename viennagrid::result_of::ncell_type<Config, 0>::type                         VertexType;
  typedef typename viennagrid::result_of::ncell_type<Config, 1>::type                         EdgeType;
  typedef typename viennagrid::result_of::ncell_type<Config, CellTag::topology_level>::type   CellType;
  
  typedef typename viennagrid::result_of::const_ncell_container<DeviceType, 0>::type    VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type         VertexIterator;
  
  typedef typename viennagrid::result_of::const_ncell_container<DeviceType, 1>::type    EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type           EdgeIterator;
  
  long counter = 0;
  
  std::cout << "-" << std::endl;
  std::cout << "- Vertex Information: " << std::endl;
  std::cout << "-" << std::endl;
  VertexContainer vertices = viennagrid::ncells<0>(d);
  for (VertexIterator vit  = vertices.begin();
                      vit != vertices.end();
                    ++vit)
  {
    std::cout << "Vertex " << counter++ << ": " << viennadata::access<box_volume_key, double>()(*vit) << std::endl;
  }
  
  std::cout << "-" << std::endl;
  std::cout << "- Edge Information: " << std::endl;
  std::cout << "-" << std::endl;
  counter = 0;
  EdgeContainer edges = viennagrid::ncells<1>(d);
  for (EdgeIterator eit  = edges.begin();
                    eit != edges.end();
                  ++eit)
  {
    eit->print_short();
    std::cout << "Length: "    << viennadata::access<edge_len_key, double>()(*eit)            << std::endl;
    std::cout << "Interface: " << viennadata::access<edge_interface_area_key, double>()(*eit) << std::endl;
    std::cout << "midpoint: " << viennagrid::circumcenter(*eit) << std::endl;
  }
  
}




int main(int argc, char *argv[])
{
  typedef viennagrid::config::quadrilateral_2d           Config;
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
  device.cells(0).print_short();
  std::cout << std::endl;
  std::cout << "Circumcenter of cell #0: " << viennagrid::circumcenter(device.cells(0)) << std::endl;
  std::cout << "Circumcenter of cell #1: " << viennagrid::circumcenter(device.cells(1)) << std::endl;
  std::cout << "Circumcenter of cell #2: " << viennagrid::circumcenter(device.cells(2)) << std::endl;
  std::cout << "Circumcenter of cell #3: " << viennagrid::circumcenter(device.cells(3)) << std::endl;
                                                                         
  //write to vtk:
  viennagrid::io::vtk_writer<DeviceType> my_vtk_writer;
  my_vtk_writer.writeDomain(device, "voronoi");
  
}

