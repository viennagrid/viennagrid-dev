#ifndef VIENNAGRID_TRAITS_POINT_HPP
#define VIENNAGRID_TRAITS_POINT_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/forwards.hpp"

/** @file viennagrid/traits/point.hpp
    @brief Provides a generic layer for point operations
*/

namespace viennagrid
{

  namespace traits
  {
    /** @brief  Returns the geometric dimension of a point */
    template <typename PointType>
    struct dimension;

    /** @brief  Returns the geometric dimension of a point. Specialization for a ViennaGrid point */
    template <typename CoordType, typename CoordinateSystem>
    struct dimension< spatial_point<CoordType, CoordinateSystem> >
    {
      static const int value = CoordinateSystem::dim;
    };


    /** @brief Returns the coordinate system of a point. Must be specialized for a user-provided point type */
    template <typename PointType>
    struct coordinate_system
    {
      //by default, we don't know anything about the point type, so let's complain at compile time
      typedef typename PointType::ERROR_UNKNOWN_COORDINATE_SYSTEM_FOR_POINT_TYPE    type;
    };

    /** @brief Returns the coordinate system of a point. Specialization for a ViennaGrid point. */
    template <typename CoordType, typename CoordinateSystem>
    struct coordinate_system< spatial_point<CoordType, CoordinateSystem> >
    {
      typedef CoordinateSystem    type;
    };


    /** @brief Returns the static (compile time) size of a point */
    template <typename PointType>
    struct static_size;

    /** @brief Returns the static (compile time) size of a point. Specialization for a ViennaGrid point. */
    template <typename CoordType, typename CoordinateSystem>
    struct static_size< spatial_point<CoordType, CoordinateSystem> >
    {
      static const int value = CoordinateSystem::dim;
    };

    /** @brief Returns the dynamic (run time) size of a point. Assumes a .size() member. Other cases must be provided with overloads */
    template <typename PointType>
    std::size_t dynamic_size(PointType const & p)
    {
      return p.size();
    }

  }
}

#endif
