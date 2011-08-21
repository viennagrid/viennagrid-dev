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
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/iterators.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/algorithm/voronoi.hpp"



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
// Generate a ring of six triangles:
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


template <typename DeviceType>
void output_voronoi_info(DeviceType const & d)
{
  typedef typename DeviceType::config_type           Config;
  typedef typename Config::cell_tag                  CellTag;
  typedef typename viennagrid::result_of::point_type<Config>::type                            PointType;
  typedef typename viennagrid::result_of::ncell_type<Config, 0>::type                         VertexType;
  typedef typename viennagrid::result_of::ncell_type<Config, 1>::type                         EdgeType;
  typedef typename viennagrid::result_of::ncell_type<Config, CellTag::topology_level>::type   CellType;
  
  typedef typename viennagrid::result_of::const_ncell_range<DeviceType, 0>::type    VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type         VertexIterator;
  
  typedef typename viennagrid::result_of::const_ncell_range<DeviceType, 1>::type    EdgeContainer;
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
  }
  
}

    
//
// Test for Voronoi correctness: Volume of cells must equal volume of boxes
//
template <typename DomainType>
void voronoi_volume_test(DomainType const & d)
{
  typedef typename DomainType::config_type           Config;
  typedef typename Config::cell_tag                  CellTag;
  typedef typename viennagrid::result_of::point_type<Config>::type                            PointType;
  typedef typename viennagrid::result_of::ncell_type<Config, 0>::type                         VertexType;
  typedef typename viennagrid::result_of::ncell_type<Config, 1>::type                         EdgeType;
  typedef typename viennagrid::result_of::ncell_type<Config, 2>::type                         FacetType;
  typedef typename viennagrid::result_of::ncell_type<Config, CellTag::topology_level>::type   CellType;
  
  typedef typename viennagrid::result_of::const_ncell_range<DomainType, CellTag::topology_level>::type    CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;
  
  typedef typename viennagrid::result_of::const_ncell_range<DomainType, 0>::type                          VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type                                     VertexIterator;
  
  
  double boxed_volume = 0;
  VertexContainer vertices = viennagrid::ncells<0>(d);
  for (VertexIterator vit  = vertices.begin();
                      vit != vertices.end();
                    ++vit)
  {
    boxed_volume += viennadata::access<box_volume_key, double>()(*vit);
  }

  std::cout << "Volume due to boxes: " << boxed_volume << std::endl;
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
  device.cells(0).print_short();
  std::cout << std::endl;
  std::cout << "Circumcenter of first cell: " << viennagrid::circumcenter(device.cells(0)) << std::endl;
  std::cout << "Area of first cell: " << viennagrid::spanned_volume(device.vertex(0).getPoint(),
                                                                    device.vertex(1).getPoint(),
                                                                    device.vertex(8).getPoint()) << std::endl;
                                                                    
  voronoi_volume_test(device);  

  //write to vtk:
  viennagrid::io::vtk_writer<DeviceType> my_vtk_writer;
  my_vtk_writer.writeDomain(device, "voronoi");
  
}

