#ifndef VIENNAGRID_CONFIG_OTHERS_HPP
#define VIENNAGRID_CONFIG_OTHERS_HPP

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

    struct quadrilateral_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::three_dimensions_tag      dimension_tag;
      typedef viennagrid::quadrilateral_tag         cell_tag;
    };
    
    struct hexahedral_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::three_dimensions_tag      dimension_tag;
      typedef viennagrid::hexahedron_tag            cell_tag;
    };
  }
}


#endif
