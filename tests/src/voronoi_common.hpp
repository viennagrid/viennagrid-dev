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

template <typename DeviceType>
void output_voronoi_info(DeviceType const & d)
{
  typedef typename DeviceType::config_type           Config;
  typedef typename Config::cell_tag                  CellTag;
  typedef typename viennagrid::result_of::point<Config>::type                            PointType;
  typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
  typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
  typedef typename viennagrid::result_of::ncell<Config, CellTag::topology_level>::type   CellType;
  
  typedef typename viennagrid::result_of::const_ncell_range<DeviceType, 0>::type    VertexContainer;
  typedef typename viennagrid::result_of::iterator<VertexContainer>::type         VertexIterator;
  
  typedef typename viennagrid::result_of::const_ncell_range<DeviceType, 1>::type    EdgeContainer;
  typedef typename viennagrid::result_of::iterator<EdgeContainer>::type           EdgeIterator;
  
  long counter = 0;
  
  std::cout << "-" << std::endl;
  std::cout << "- Vertex box volume information: " << std::endl;
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
    std::cout << *eit << std::endl;
    std::cout << "Length: "    << viennadata::access<edge_len_key, double>()(*eit)            << std::endl;
    std::cout << "Interface: " << viennadata::access<edge_interface_area_key, double>()(*eit) << std::endl;
  }
  
}

    
//
// Test for Voronoi correctness: Volume of cells must equal volume of boxes
//
template <typename DomainType>
double voronoi_volume(DomainType const & d)
{
  typedef typename DomainType::config_type           Config;
  typedef typename Config::cell_tag                  CellTag;
  typedef typename viennagrid::result_of::point<Config>::type                            PointType;
  typedef typename viennagrid::result_of::ncell<Config, 0>::type                         VertexType;
  typedef typename viennagrid::result_of::ncell<Config, 1>::type                         EdgeType;
  typedef typename viennagrid::result_of::ncell<Config, 2>::type                         FacetType;
  typedef typename viennagrid::result_of::ncell<Config, CellTag::topology_level>::type   CellType;
  
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

  return boxed_volume;
}

