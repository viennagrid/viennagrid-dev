/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_TOPOLOGY_POINT_HPP
#define VIENNAGRID_TOPOLOGY_POINT_HPP

#include "viennagrid/forwards.h"

namespace viennagrid
{

  struct point_tag
  {
    enum { topology_level = 0 };
    static std::string name() { return "Point"; }
  };

  //no need to have a separate subcell_traits specialization here -> covered by default case
  
}

#endif