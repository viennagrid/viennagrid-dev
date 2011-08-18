/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_TOPOLOGY_LINE_HPP
#define VIENNAGRID_TOPOLOGY_LINE_HPP

#include "viennagrid/forwards.h"
#include "viennagrid/topology/point.hpp"

namespace viennagrid
{

  struct line_tag
  {
    enum { topology_level = 1 };
    static std::string name() { return "Line"; }
  };


  namespace topology
  {

    //Line:
    template <>
    struct subcell_desc<line_tag, 0>
    {
      typedef point_tag             element_tag;
      typedef full_handling_tag     handling_tag;

      enum{ num_elements = 2 };     //2 vertices
    };

  }  
  
  
}

#endif