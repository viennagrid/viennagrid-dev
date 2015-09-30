#ifndef VIENNAGRID_ALGORITHM_GEOMETRIC_TRANSFORM_HPP
#define VIENNAGRID_ALGORITHM_GEOMETRIC_TRANSFORM_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/core/forwards.hpp"
#include "viennagrid/mesh/mesh.hpp"

/** @file viennagrid/algorithm/geometric_transform.hpp
    @brief Provides geometric transformation routines (e.g. scale()) for a mesh.
*/

namespace viennagrid
{
  /** @brief Function for scaling a mesh, uses scale_functor
   *
   * @param mesh                    The input mesh
   * @param factor                  The scaling factor
   * @param scaling_center          The center of the scaling operation
   */
  inline void scale(viennagrid::mesh const & mesh, viennagrid_numeric factor, viennagrid::point const & scaling_center)
  {
    THROW_ON_ERROR( viennagrid_mesh_scale(mesh.internal(), factor, &scaling_center[0]) );
  }

  /** @brief Function for scaling a mesh, uses scale_functor. Scaling center is the origin.
   *
   * @param mesh                    The input mesh
   * @param factor                  The scaling factor
   */
  inline void scale(viennagrid::mesh const & mesh, viennagrid_numeric factor)
  {
    THROW_ON_ERROR( viennagrid_mesh_scale(mesh.internal(), factor, NULL) );
  }

  /** @brief Function for an affine transformation of a mesh, uses affine_transform_functor
   *
   * @param mesh                    The input mesh
   * @param matrix                  The matrix representing the linear transformation part, row major layout. Attention! There are no out-of boundary checks, the user is responsible to provide a suitable matrix pointer.
   * @param translation             The translation vector
   */
  inline void affine_transform(viennagrid::mesh const & mesh,
                               viennagrid_numeric const * matrix,
                               viennagrid::point translation )
  {
    THROW_ON_ERROR( viennagrid_mesh_affine_transform(mesh.internal(), translation.size(), matrix, &translation[0]) );
  }

}

#endif
