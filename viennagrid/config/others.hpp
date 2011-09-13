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
    /** @brief Predefined configuration class for two-dimensional hypercube cells (quadrilaterals) in a 2d geometry using Cartesian coordinates. */
    struct quadrilateral_2d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<2>           coordinate_system_tag;
      typedef viennagrid::quadrilateral_tag         cell_tag;
    };

    /** @brief Predefined configuration class for two-dimensional hypercube cells (quadrilaterals) in a 3d geometry using Cartesian coordinates. */
    struct quadrilateral_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<3>           coordinate_system_tag;
      typedef viennagrid::quadrilateral_tag         cell_tag;
    };
    
    /** @brief Predefined configuration class for three-dimensional hypercube cells (hexahedra) in a 3d geometry using Cartesian coordinates. */
    struct hexahedral_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<3>           coordinate_system_tag;
      typedef viennagrid::hexahedron_tag            cell_tag;
    };
  }
}


#endif
