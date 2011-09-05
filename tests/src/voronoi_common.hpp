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
#include "viennagrid/algorithm/volume.hpp"


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
  
  viennagrid::voronoi_interface_area_key interface_key;
  viennagrid::voronoi_box_volume_key box_volume_key;
  
  std::cout << "-" << std::endl;
  std::cout << "- Vertex box volume information: " << std::endl;
  std::cout << "-" << std::endl;
  VertexContainer vertices = viennagrid::ncells<0>(d);
  for (VertexIterator vit  = vertices.begin();
                      vit != vertices.end();
                    ++vit)
  {
    std::cout << "Vertex " << counter++ << ": " << viennadata::access<viennagrid::voronoi_box_volume_key, double>(box_volume_key)(*vit) << std::endl;
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
    std::cout << "Length: "    << viennagrid::volume(*eit)            << std::endl;
    std::cout << "Interface: " << viennadata::access<viennagrid::voronoi_interface_area_key, double>(interface_key)(*eit) << std::endl;
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
  
  viennagrid::voronoi_box_volume_key box_volume_key;
  
  double boxed_volume = 0;
  VertexContainer vertices = viennagrid::ncells<0>(d);
  for (VertexIterator vit  = vertices.begin();
                      vit != vertices.end();
                    ++vit)
  {
    boxed_volume += viennadata::access<viennagrid::voronoi_box_volume_key, double>(box_volume_key)(*vit);
  }

  return boxed_volume;
}

