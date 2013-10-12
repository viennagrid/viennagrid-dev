#ifndef VIENNAGRID_ALGORITHM_DISTANCE_HPP
#define VIENNAGRID_ALGORITHM_DISTANCE_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
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
#include <limits>

#include "viennagrid/forwards.hpp"
#include "viennagrid/topology/all.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/closest_points.hpp"

/** @file viennagrid/algorithm/distance.hpp
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
    template <typename PointType1, typename PointType2>
    typename viennagrid::result_of::coord<PointType1>::type
    distance_impl(PointType1 const & p1,
                  PointType2 const & p2)
    {
      return norm_2(p1 - p2);
    }

    template <typename PointType1, typename PointType2>
    typename viennagrid::result_of::coord<PointType1>::type
    distance_impl( std::pair<PointType1,PointType2> const & pts)
    {
      return distance_impl(pts.first, pts.second);
    }

    template <typename PointAccessorType, typename CoordType1, typename CoordinateSystem1, typename CoordType2, typename CoordinateSystem2>
    CoordType1
    distance_impl(PointAccessorType const,
                  point_t<CoordType1, CoordinateSystem1> const & p1,
                  point_t<CoordType2, CoordinateSystem2> const & p2)
    {
      return distance_impl(p1, p2);
    }

    template <typename PointAccessorType, typename PointType, typename WrappedConfigType>
    typename viennagrid::result_of::coord<PointType>::type
    distance_impl(PointAccessorType const accessor,
                  PointType const & p1,
                  element_t<viennagrid::vertex_tag, WrappedConfigType> const & v2)
    {
      return distance_impl(p1, accessor(v2));
    }

    template <typename PointAccessorType, typename PointType, typename WrappedConfigType>
    typename viennagrid::result_of::coord<typename PointAccessorType::value_type>::type
    distance_impl(PointAccessorType const accessor,
                  element_t<viennagrid::vertex_tag, WrappedConfigType> const & v1,
                  PointType const & p2)
    {
      return distance_impl(accessor(v1), p2);
    }

    // Distance between vertices: Use point distance
    template <typename PointAccessorType, typename PointType, typename WrappedConfigType1, typename WrappedConfigType2>
    typename result_of::coord<typename PointAccessorType::value_type>::type
    distance_impl(PointAccessorType const accessor,
                  element_t<viennagrid::vertex_tag, WrappedConfigType1> const & v1,
                  element_t<viennagrid::vertex_tag, WrappedConfigType2> const & v2)
    {
      return distance_impl(accessor(v1), accessor(v2));
    }



    //
    // Generic distance computation: Reuse closest_points()
    //
    template <typename PointAccessorType, typename Something1, typename Something2>
    typename result_of::coord<typename PointAccessorType::value_type>::type
    distance_impl(PointAccessorType const accessor,
                  Something1 const & el1,
                  Something2 const & el2)
    {
      //typedef typename result_of::point<ElementType1>::type   PointType;
      typedef typename PointAccessorType::value_type PointType;

      std::pair<PointType, PointType> points = closest_points(accessor, el1, el2);

      return distance_impl( points );
    }



    ////////////////////////////////// boundary distance //////////////////////////////////////


    template <typename PointType1, typename PointType2>
    typename viennagrid::result_of::coord<PointType1>::type
    boundary_distance_impl(PointType1 const & p1,
                  PointType2 const & p2)
    {
      return norm_2(p1 - p2);
    }

    template <typename PointType1, typename PointType2>
    typename viennagrid::result_of::coord<PointType1>::type
    boundary_distance_impl( std::pair<PointType1,PointType2> const & pts)
    {
      return boundary_distance_impl(pts.first, pts.second);
    }

    template <typename PointAccessorType, typename CoordType1, typename CoordinateSystem1, typename CoordType2, typename CoordinateSystem2>
    CoordType1
    boundary_distance_impl(PointAccessorType const,
                  point_t<CoordType1, CoordinateSystem1> const & p1,
                  point_t<CoordType2, CoordinateSystem2> const & p2)
    {
      return boundary_distance_impl(p1, p2);
    }

    template <typename PointAccessorType, typename PointType, typename WrappedConfigType>
    typename viennagrid::result_of::coord<PointType>::type
    boundary_distance_impl(PointAccessorType const accessor,
                  PointType const & p1,
                  element_t<viennagrid::vertex_tag, WrappedConfigType> const & v2)
    {
      return boundary_distance_impl(p1, accessor(v2));
    }

    template <typename PointAccessorType, typename PointType, typename WrappedConfigType>
    typename viennagrid::result_of::coord<typename PointAccessorType::value_type>::type
    boundary_distance_impl(PointAccessorType const accessor,
                  element_t<viennagrid::vertex_tag, WrappedConfigType> const & v1,
                  PointType const & p2)
    {
      return boundary_distance_impl(accessor(v1), p2);
    }

    // Distance between vertices: Use point distance
    template <typename PointAccessorType, typename PointType, typename WrappedConfigType1, typename WrappedConfigType2>
    typename result_of::coord<typename PointAccessorType::value_type>::type
    boundary_distance_impl(PointAccessorType const accessor,
                  element_t<viennagrid::vertex_tag, WrappedConfigType1> const & v1,
                  element_t<viennagrid::vertex_tag, WrappedConfigType2> const & v2)
    {
      return boundary_distance_impl(accessor(v1), accessor(v2));
    }



    //
    // Generic distance computation: Reuse closest_points()
    //
    template <typename PointAccessorType, typename Something1, typename Something2>
    typename result_of::coord<typename PointAccessorType::value_type>::type
    boundary_distance_impl(PointAccessorType const accessor,
                           Something1 const & el1,
                           Something2 const & el2)
    {
      //typedef typename result_of::point<ElementType1>::type      PointType;
      typedef typename PointAccessorType::value_type PointType;

      std::pair<PointType, PointType> points = closest_points_on_boundary(accessor, el1, el2);

      return boundary_distance_impl(points);
    }

  } //namespace detail

  //
  // The public interface functions
  //
  /** @brief Returns the distance between elements, segments and/or meshs */
  template <typename PointAccessorType, typename Something1, typename Something2>
  typename result_of::coord<Something1>::type
  distance(PointAccessorType const accessor,
           Something1 const & el1,
           Something2 const & el2)
  {
    return detail::distance_impl(accessor, el1, el2);
  }


  template <typename Something1, typename Something2>
  typename result_of::coord<Something1>::type
  distance(Something1 const & el1,
           Something2 const & el2)
  {
    return detail::distance_impl( default_point_accessor(el1), el1, el2 );
  }

  template <typename Something, typename CoordType, typename CoordinateSystem>
  typename result_of::coord<Something>::type
  distance(Something const & el1,
           point_t<CoordType, CoordinateSystem> const & el2)
  {
    return detail::distance_impl( default_point_accessor(el1), el1, el2 );
  }

  template <typename CoordType, typename CoordinateSystem, typename Something>
  typename result_of::coord<Something>::type
  distance(point_t<CoordType, CoordinateSystem> const & el1,
           Something const & el2)
  {
    return detail::distance_impl( default_point_accessor(el2), el1, el2 );
  }

  template <typename CoordType1, typename CoordinateSystem1, typename CoordType2, typename CoordinateSystem2>
  typename result_of::coord< point_t<CoordType1, CoordinateSystem1> >::type
  distance(point_t<CoordType1, CoordinateSystem1> const & el1,
           point_t<CoordType2, CoordinateSystem2> const & el2)
  {
    return detail::distance_impl( el1, el2 );
  }




  /** @brief Returns the distance between elements, segments and/or meshs */
  template <typename PointAccessorType, typename Something1, typename Something2>
  typename result_of::coord<Something1>::type
  boundary_distance(PointAccessorType const accessor,
                    Something1 const & el1,
                    Something2 const & el2)
  {
    return detail::boundary_distance_impl(accessor, el1, el2);
  }

  /** @brief Returns the distance between elements, segments and/or meshs */
  template <typename Something1, typename Something2>
  typename result_of::coord<Something1>::type
  boundary_distance(Something1 const & el1,
                    Something2 const & el2)
  {
    return detail::boundary_distance_impl( default_point_accessor(el1), el1, el2 );
  }

  template <typename Something, typename CoordType, typename CoordinateSystem>
  typename result_of::coord<Something>::type
  boundary_distance(Something const & el1,
                    point_t<CoordType, CoordinateSystem> const & el2)
  {
    return detail::boundary_distance_impl( default_point_accessor(el1), el1, el2 );
  }

  template <typename CoordType, typename CoordinateSystem, typename Something>
  typename result_of::coord<Something>::type
  boundary_distance(point_t<CoordType, CoordinateSystem> const & el1,
                    Something const & el2)
  {
    return detail::boundary_distance_impl( default_point_accessor(el2), el1, el2 );
  }


} //namespace viennagrid
#endif
