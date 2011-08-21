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
