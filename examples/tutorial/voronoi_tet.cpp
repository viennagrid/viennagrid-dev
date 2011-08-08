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
#include "viennagrid/io/netgen_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/iterators.hpp"
#include "viennagrid/config/simplex.hpp"
#include "viennagrid/algorithm/voronoi.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"


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
// Generate a single tetrahedron
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
  
  typedef typename viennagrid::result_of::const_ncell_container<DomainType, CellTag::topology_level>::type    CellContainer;
  typedef typename viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;
  
  typedef typename viennagrid::result_of::const_ncell_container<DomainType, 0>::type                          VertexContainer;
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
  typedef viennagrid::config::tetrahedral_3d          Config;
  typedef viennagrid::domain<Config>                  DomainType;
  
  std::cout << "* main(): Creating device..." << std::endl;
  DomainType my_domain;
  
  //create device:
  //setup_device(my_domain);
  try
  {
    viennagrid::io::netgen_reader my_netgen_reader;
    my_netgen_reader(my_domain, "../examples/data/cube48.mesh");
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
  my_domain.cells(0).print_short();
  std::cout << std::endl;
  std::cout << "Circumcenter of first cell: " << viennagrid::circumcenter(my_domain.cells(0)) << std::endl;

  voronoi_volume_test(my_domain);  
  
  //write to vtk:
  viennagrid::io::vtk_writer<DomainType> my_vtk_writer;
  my_vtk_writer.writeDomain(my_domain, "voronoi");
  
}

