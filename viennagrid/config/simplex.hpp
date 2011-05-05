/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_CONFIG_SIMPLEX_HPP
#define VIENNAGRID_CONFIG_SIMPLEX_HPP

#include "viennagrid/topology/triangle.hpp"
#include "viennagrid/topology/tetrahedron.hpp"

namespace viennagrid
{
  namespace config
  {
    //2d geometry:
    struct line_2d
    {
      typedef double                                numeric_type;
      typedef viennagrid::two_dimensions_tag        dimension_tag;
      typedef viennagrid::line_tag                  cell_tag;
    };

    struct triangular_2d
    {
      typedef double                                numeric_type;
      typedef viennagrid::two_dimensions_tag        dimension_tag;
      typedef viennagrid::triangle_tag              cell_tag;
    };
    
    struct triangular_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::three_dimensions_tag      dimension_tag;
      typedef viennagrid::triangle_tag              cell_tag;
    };

    struct tetrahedral_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::three_dimensions_tag      dimension_tag;
      typedef viennagrid::tetrahedron_tag           cell_tag;
    };
  }
}


#endif
