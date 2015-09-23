#ifndef VIENNAGRID_ALGORITHM_CENTROID_HPP
#define VIENNAGRID_ALGORITHM_CENTROID_HPP

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
#include "viennagrid/algorithm/volume.hpp"    //for mesh/region centroid

/** @file viennagrid/algorithm/centroid.hpp
    @brief Computes the centroid (center of mass) for different cell types
*/

namespace viennagrid
{

  /** @brief The public interface function for the computation of a centroid of an element
   *
   * @param element    The element for which the centroid should be computed
   */
  template <bool element_is_const>
  typename viennagrid::result_of::point< base_element<element_is_const> >::type centroid(base_element<element_is_const> const & element)
  {
    point result( geometric_dimension(element) );
    THROW_ON_ERROR( viennagrid_element_centroid(element.internal_mesh(), element.id().internal(), &result[0]) );
    return result;
  }


  /** @brief The public interface function for the computation of a centroid of a mesh with explicit point accessor. Cells are used for centroid calculation, will fail if there is more than one cell type.
   *
   * @param  mesh_obj           The mesh which centroid is to be calculated
   */
  template<bool mesh_is_const>
  typename viennagrid::result_of::point< base_mesh<mesh_is_const> >::type centroid(base_mesh<mesh_is_const> const & mesh_obj)
  {
    point result( geometric_dimension(mesh_obj) );
    THROW_ON_ERROR( viennagrid_mesh_centroid(mesh_obj.internal(), &result[0]) );
    return result;
  }





} //namespace viennagrid
#endif
