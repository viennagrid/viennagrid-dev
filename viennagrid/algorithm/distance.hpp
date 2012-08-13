#ifndef VIENNAGRID_ALGORITHM_DISTANCE_HPP
#define VIENNAGRID_ALGORITHM_DISTANCE_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
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

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <limits>

#include "viennagrid/forwards.h"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/closest_points.hpp"

/** @file distance.hpp
    @brief Computes the (Cartesian) distance between different elements.
*/


namespace viennagrid
{
  namespace detail
  {
    
    //
    // Distance between points
    //
    
    // Distance between two points:
    template <typename CoordType, typename CoordinateSystem>
    CoordType
    distance_impl(point_t<CoordType, CoordinateSystem> const & p1,
                  point_t<CoordType, CoordinateSystem> const & p2)
    {
      return norm_2(p1 - p2);
    }

    template <typename CoordType, typename CoordinateSystem, typename ConfigType>
    CoordType
    distance_impl(point_t<CoordType, CoordinateSystem> const & p1,
                  element_t<ConfigType, point_tag> const & v2)
    {
      return norm_2(p1 - v2.point());
    }

    template <typename ConfigType, typename CoordType, typename CoordinateSystem>
    CoordType
    distance_impl(element_t<ConfigType, point_tag> const & v1,
                  point_t<CoordType, CoordinateSystem> const & p2)
    {
      return norm_2(v1.point() - p2);
    }
    
    // Distance between vertices: Use point distance
    template <typename ConfigType>
    typename ConfigType::numeric_type
    distance_impl(element_t<ConfigType, point_tag> const & v1,
                  element_t<ConfigType, point_tag> const & v2)
    {
      return norm_2(v1.point() - v2.point());
    }

    
    
    //
    // Generic distance computation: Reuse closest_points()
    //
    template <typename ElementType1, typename ElementType2>
    typename result_of::numeric<ElementType1>::type
    distance_impl(ElementType1 const & el1,
                  ElementType2 const & el2)
    {
      typedef typename result_of::point<ElementType1>::type   PointType;
      
      std::pair<PointType, PointType> points = closest_points(el1, el2);
      
      return norm_2(points.first - points.second);
    }
    
    
    
    ////////////////////////////////// boundary distance //////////////////////////////////////

    // Distance between two points:
    template <typename CoordType, typename CoordinateSystem>
    CoordType
    boundary_distance_impl(point_t<CoordType, CoordinateSystem> const & p1,
                           point_t<CoordType, CoordinateSystem> const & p2)
    {
      return norm_2(p1 - p2);
    }

    template <typename CoordType, typename CoordinateSystem, typename ConfigType>
    CoordType
    boundary_distance_impl(point_t<CoordType, CoordinateSystem> const & p1,
                           element_t<ConfigType, point_tag> const & v2)
    {
      return norm_2(p1 - v2.point());
    }

    template <typename ConfigType, typename CoordType, typename CoordinateSystem>
    CoordType
    boundary_distance_impl(element_t<ConfigType, point_tag> const & v1,
                           point_t<CoordType, CoordinateSystem> const & p2)
    {
      return norm_2(v1.point() - p2);
    }
    
    // Distance between vertices: Use point distance
    template <typename ConfigType>
    typename ConfigType::numeric_type
    boundary_distance_impl(element_t<ConfigType, point_tag> const & v1,
                           element_t<ConfigType, point_tag> const & v2)
    {
      return norm_2(v1.point() - v2.point());
    }

    
    
    //
    // Generic distance computation: Reuse closest_points()
    //
    template <typename ElementType1, typename ElementType2>
    typename result_of::numeric<ElementType1>::type
    boundary_distance_impl(ElementType1 const & el1,
                           ElementType2 const & el2)
    {
      typedef typename result_of::point<ElementType1>::type      PointType;
      
      std::pair<PointType, PointType> points = closest_points_on_boundary(el1, el2);
      
      return norm_2(points.first - points.second);
    }
    
    
  } //namespace detail
  
  //
  // The public interface functions
  //
  /** @brief Returns the distance between n-cells, segments and/or domains */
  template <typename ElementType1, typename ElementType2>
  typename result_of::numeric<ElementType1>::type
  distance(ElementType1 const & el1, ElementType2 const & el2)
  {
    return detail::distance_impl(el1, el2);
  }
  
  
  /** @brief Returns the distance between the boundary of n-cells, segments and/or domains */
  template <typename ElementType1, typename ElementType2>
  typename result_of::numeric<ElementType1>::type
  boundary_distance(ElementType1 const & el1, ElementType2 const & el2)
  {
    return detail::boundary_distance_impl(el1, el2);
  }
  
} //namespace viennagrid
#endif
