#ifndef VIENNAGRID_ALGORITHM_EXTRACT_BOUNDARY_HPP
#define VIENNAGRID_ALGORITHM_EXTRACT_BOUNDARY_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/viennagrid.hpp"

/** @file viennagrid/algorithm/extract_boundary.hpp
    @brief Extraction of a hull/boundary of a mesh.
*/

namespace viennagrid
{

  /** @brief Extracts the hull of mesh using viennagrid::boundary, e.g. the triangular hull of a tetrahedral mesh.
   *
   * @tparam HullTypeOrTagT                The type or tag of the hull element
   * @param volume_mesh                    The input mesh
   * @param hull_mesh                      The output hull mesh
   */
  template<bool mesh_is_const>
  void extract_boundary(viennagrid::base_mesh<mesh_is_const> const & volume_mesh,
                        viennagrid::mesh const & hull_mesh,
                        viennagrid_dimension hull_dimension)
  {
    THROW_ON_ERROR( viennagrid_mesh_extract_boundary(volume_mesh.internal(), hull_mesh.internal(), hull_dimension) );
  }

}

#endif
