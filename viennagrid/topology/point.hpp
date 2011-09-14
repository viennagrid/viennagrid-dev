#ifndef VIENNAGRID_TOPOLOGY_POINT_HPP
#define VIENNAGRID_TOPOLOGY_POINT_HPP

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

#include "viennagrid/forwards.h"

/** @file viennagrid/topology/point.hpp
    @brief Provides the topological definition of a point (vertex)
*/

namespace viennagrid
{

  /** @brief Topological description of a 0-cell (vertex). */
  struct point_tag
  {
    enum { dim = 0 };
    static std::string name() { return "Point"; }
  };

  //no need to have a separate subelements specialization here -> covered by default case
  
}

#endif

