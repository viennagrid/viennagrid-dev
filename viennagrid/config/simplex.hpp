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

/** @file simplex.hpp
    @brief Provides default configuration classes for simplex domains
*/

namespace viennagrid
{
  namespace config
  {
    /** @brief Predefined configuration class for one-dimensional simplex cells (lines) in a 1d geometry using Cartesian coordinates. */
    struct line_1d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<1>           coordinate_system_tag;
      typedef viennagrid::simplex_tag<1>            cell_tag;
    };
    

    /** @brief Predefined configuration class for one-dimensional simplex cells (lines) in a 2d geometry using Cartesian coordinates. */
    struct line_2d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<2>           coordinate_system_tag;
      typedef viennagrid::simplex_tag<1>            cell_tag;
    };

    /** @brief Predefined configuration class for two-dimensional simplex cells (triangles) in a 2d geometry using Cartesian coordinates. */
    struct triangular_2d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<2>           coordinate_system_tag;
      typedef viennagrid::triangle_tag              cell_tag;
    };
    
    //3d geometry:
    /** @brief Predefined configuration class for one-dimensional simplex cells (lines) in a 3d geometry using Cartesian coordinates. */
    struct line_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<3>           coordinate_system_tag;
      typedef viennagrid::simplex_tag<1>            cell_tag;
    };

    /** @brief Predefined configuration class for two-dimensional simplex cells (triangles) in a 3d geometry using Cartesian coordinates. */
    struct triangular_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<3>           coordinate_system_tag;
      typedef viennagrid::triangle_tag              cell_tag;
    };

    /** @brief Predefined configuration class for three-dimensional simplex cells (tetrahedra) in a 3d geometry using Cartesian coordinates. */
    struct tetrahedral_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::cartesian_cs<3>           coordinate_system_tag;
      typedef viennagrid::tetrahedron_tag           cell_tag;
    };
  }
}


#endif
