#ifndef VIENNAGRID_ALGORITHM_CIRCUMCENTER_HPP
#define VIENNAGRID_ALGORITHM_CIRCUMCENTER_HPP

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

/** @file viennagrid/algorithm/circumcenter.hpp
    @brief Computes the circumcenter of n-cells.
*/

namespace viennagrid
{
  /** @brief The public interface function for the calculation of the circumcenter.
   *
   * @param element       The element for which the circumcenter should be computed
   */
  template<typename ElementT>
  typename viennagrid::result_of::point< ElementT >::type circumcenter(ElementT const & element)
  {
    typename viennagrid::result_of::point< ElementT >::type point( viennagrid::geometric_dimension(element) );
    viennagrid_element_circumcenter( element.internal_mesh(), element.id().internal(), &point[0] );
    return point;
  }


} //namespace viennagrid
#endif
