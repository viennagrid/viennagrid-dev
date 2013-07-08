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

//
// A check for the absence of external linkage (otherwise, library is not truly 'header-only')
//

#include <iostream>

//
// *** ViennaGrid
//

#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/inserter.hpp"
#include "viennagrid/storage/id_generator.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"
#include "viennagrid/storage/range.hpp"


#include "viennagrid/topology/vertex.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"


#include "viennagrid/element/element_key.hpp"
#include "viennagrid/element/element_orientation.hpp"

#include "viennagrid/config/element_config.hpp"
#include "viennagrid/config/topology_config.hpp"
#include "viennagrid/config/domain_config.hpp"
#include "viennagrid/config/default_configs.hpp"

#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/topology.hpp"

//algorithms:
#include "viennagrid/algorithm/angle.hpp"
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/interface.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/refine.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"
#include "viennagrid/algorithm/surface.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/voronoi.hpp"

//io:
#include "viennagrid/io/netgen_reader.hpp"
//#include "viennagrid/io/opendx_writer.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"


void other_func()
{
  viennagrid::tetrahedral_3d_domain  domain;
  
  std::cout << "--- Tetrahedral domain, 3d ---" << std::endl;
  std::cout << "Size<0>: " << viennagrid::elements<viennagrid::vertex_tag>(domain).size() << std::endl;
  std::cout << "Size<1>: " << viennagrid::elements<viennagrid::vertex_tag>(domain).size() << std::endl;
  std::cout << "Size<2>: " << viennagrid::elements<viennagrid::vertex_tag>(domain).size() << std::endl;
  std::cout << "Size<3>: " << viennagrid::elements<viennagrid::vertex_tag>(domain).size() << std::endl;
}
