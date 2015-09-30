#ifndef VIENNAGRID_ALGORITHM_SURFACE_HPP
#define VIENNAGRID_ALGORITHM_SURFACE_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "viennagrid/core/forwards.hpp"
#include "viennagrid/algorithm/volume.hpp"

/** @file viennagrid/algorithm/surface.hpp
    @brief Computes the surface of different cell types as well as meshs and regions
*/


namespace viennagrid
{
  /** @brief Returns the surface of an element */
  template <bool element_is_const>
  viennagrid_numeric surface(base_element<element_is_const> const & cell)
  {
    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_element_surface( cell.internal_mesh(), cell.id().internal(), &result) );
    return result;
  }

  /** @brief Returns the surface of a whole mesh */
  template<bool mesh_is_const>
  viennagrid_numeric surface(base_mesh<mesh_is_const> const & mesh_obj)
  {
    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_mesh_surface( mesh_obj.internal(), &result) );
    return result;
  }

  /** @brief Returns the surface of a whole mesh */
  template<bool region_is_const>
  viennagrid_numeric surface(base_region<region_is_const> const & region_obj)
  {
    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_region_surface( region_obj.internal_mesh(), region_obj.internal(), &result) );
    return result;
  }

} //namespace viennagrid
#endif
