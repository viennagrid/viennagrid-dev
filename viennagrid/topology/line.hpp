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

      enum{ num_elements = 2 };     //2 vertices
    };

  }  
  
  
}

#endif