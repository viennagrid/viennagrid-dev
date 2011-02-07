/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


#ifndef VIENNAGRID_DOMAIN_CONFIGS_HPP
#define VIENNAGRID_DOMAIN_CONFIGS_HPP

#include "viennagrid/celltags.hpp"

namespace viennagrid
{
  namespace config
  {
    //2d geometry:
    struct triangular_2d
    {
      typedef double                                numeric_type;
      typedef viennagrid::two_dimensions_tag        dimension_tag;
      typedef viennagrid::triangle_tag              cell_tag;
      typedef viennagrid::no_multigrid_tag          multigrid_tag;
    };

    struct quadrilateral_2d
    {
      typedef double                                numeric_type;
      typedef viennagrid::two_dimensions_tag        dimension_tag;
      typedef viennagrid::quadrilateral_tag         cell_tag;
      typedef viennagrid::no_multigrid_tag          multigrid_tag;
    };
    
    //3d geometry:
    struct triangular_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::three_dimensions_tag      dimension_tag;
      typedef viennagrid::triangle_tag              cell_tag;
      typedef viennagrid::no_multigrid_tag          multigrid_tag;
    };

    struct tetrahedral_3d
    {
      typedef double                                numeric_type;
      typedef viennagrid::three_dimensions_tag      dimension_tag;
      typedef viennagrid::tetrahedron_tag           cell_tag;
      typedef viennagrid::no_multigrid_tag          multigrid_tag;
    };
  }
}


#endif
