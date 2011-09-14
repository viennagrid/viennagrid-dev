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
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"

//algorithms:
#include "viennagrid/algorithm/boundary.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/circumcenter.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"
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
#include "viennagrid/io/opendx_writer.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"

void other_func()
{
  viennagrid::domain_t<viennagrid::config::tetrahedral_3d>  domain;
  
  std::cout << "--- Tetrahedral domain, 3d ---" << std::endl;
  std::cout << "Size<0>: " << viennagrid::ncells<0>(domain).size() << std::endl;
  std::cout << "Size<1>: " << viennagrid::ncells<0>(domain).size() << std::endl;
  std::cout << "Size<2>: " << viennagrid::ncells<0>(domain).size() << std::endl;
  std::cout << "Size<3>: " << viennagrid::ncells<0>(domain).size() << std::endl;
}
