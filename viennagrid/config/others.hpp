/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_CONFIG_OTHERS_HPP
#define VIENNAGRID_CONFIG_OTHERS_HPP

#include "viennagrid/topology/quadrilateral.hpp"
#include "viennagrid/topology/hexahedron.hpp"

namespace viennagrid
{
  namespace config
  {
    struct quadrilateral_2d
    {
      typedef double                                numeric_type;
      typedef viennagrid::two_dimensions_tag        dimension_tag;
      typedef viennagrid::quadrilateral_tag         cell_tag;
    };
    
    struct hexahedron_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::three_dimensions_tag      dimension_tag;
      typedef viennagrid::hexahedron_tag            cell_tag;
    };
  }
}


#endif
