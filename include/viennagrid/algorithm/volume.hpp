#ifndef VIENNAGRID_ALGORITHM_VOLUME_HPP
#define VIENNAGRID_ALGORITHM_VOLUME_HPP

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

#include "viennagrid/viennagrid.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"

/** @file viennagrid/algorithm/volume.hpp
    @brief Computes the volume of different cell types as well as meshs and segments
*/


namespace viennagrid
{
  /** @brief Returns the volume of an element */
  template <bool element_is_const>
  viennagrid_numeric volume(base_element<element_is_const> const & cell)
  {
    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_element_volume( cell.internal_mesh(), cell.id().internal(), &result) );
    return result;
  }

  /** @brief Returns the volume of a whole mesh */
  template<bool mesh_is_const>
  viennagrid_numeric volume(base_mesh<mesh_is_const> const & mesh_obj)
  {
    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_mesh_volume( mesh_obj.internal(), &result) );
    return result;
  }
} //namespace viennagrid
#endif
