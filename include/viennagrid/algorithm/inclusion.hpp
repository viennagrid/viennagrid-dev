#ifndef VIENNAGRID_ALGORITHM_INCLUSION_HPP
#define VIENNAGRID_ALGORITHM_INCLUSION_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <limits>
#include "viennagrid/viennagrid.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"
#include "viennagrid/algorithm/detail/numeric.hpp"

/** @file viennagrid/algorithm/inclusion.hpp
    @brief Tests for inclusion of a point inside an element, segment, or mesh.
*/

namespace viennagrid
{
  /** @brief Determines if a given point is inside an element. Returns true if the point is inside the element, false otherwise.
   *
   * @param element             The element to test
   * @param point               The point to test
   * @param numeric_config      The numeric config
   */
  template<typename ElementT, typename PointT, typename NumericConfigT>
  bool is_inside( ElementT const & element, PointT const & point, NumericConfigT numeric_config )
  {
    viennagrid_bool tmp;
    THROW_ON_ERROR( viennagrid_point_in_element(element.internal_mesh(), element.id().internal(), &point[0], numeric_config, &tmp) );
    return tmp == VIENNAGRID_TRUE;
  }



  /** @brief Determines if a given point is inside an element. Returns true if the point is inside the element, false otherwise.
   *
   * @param element             The element to test
   * @param point               The point to test
   */
  template<typename ElementT, typename PointT>
  bool is_inside( ElementT const & element, PointT const & point )
  {
    viennagrid_bool tmp;
    THROW_ON_ERROR( viennagrid_point_in_element(element.internal_mesh(), element.id().internal(), &point[0], 10.0*std::numeric_limits<viennagrid_numeric>::epsilon(), &tmp) );
    return tmp == VIENNAGRID_TRUE;
  }
}


#endif
