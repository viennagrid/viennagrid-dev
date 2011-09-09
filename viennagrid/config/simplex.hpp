#ifndef VIENNAGRID_CONFIG_SIMPLEX_HPP
#define VIENNAGRID_CONFIG_SIMPLEX_HPP

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


#include "viennagrid/topology/triangle.hpp"
#include "viennagrid/topology/tetrahedron.hpp"

namespace viennagrid
{
  namespace config
  {
    //1d geometry:
    struct line_1d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<1>           coordinate_system_tag;
      typedef viennagrid::simplex_tag<1>            cell_tag;
    };
    
    //2d geometry:
    struct line_2d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<2>           coordinate_system_tag;
      typedef viennagrid::simplex_tag<1>            cell_tag;
    };

    struct triangular_2d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<2>           coordinate_system_tag;
      typedef viennagrid::triangle_tag              cell_tag;
    };
    
    //3d geometry:
    struct line_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<3>           coordinate_system_tag;
      typedef viennagrid::simplex_tag<1>            cell_tag;
    };

    struct triangular_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<3>           coordinate_system_tag;
      typedef viennagrid::triangle_tag              cell_tag;
    };

    struct tetrahedral_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<3>           coordinate_system_tag;
      typedef viennagrid::tetrahedron_tag           cell_tag;
    };
  }
}


#endif
