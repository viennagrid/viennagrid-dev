#ifndef VIENNAGRID_ALGORITHM_DISTANCE_HPP
#define VIENNAGRID_ALGORITHM_DISTANCE_HPP

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
#include <limits>

#include "viennagrid/viennagrid.hpp"
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
    inline viennagrid::result_of::coord<point>::type distance_impl(point const & p1,
                                                                   point const & p2)
    {
      return norm_2(p1 - p2);
    }

    inline viennagrid::result_of::coord<point>::type distance_impl( std::pair<point, point> const & pts )
    {
      return distance_impl(pts.first, pts.second);
    }

    template <typename PointAccessorT>
    inline viennagrid::result_of::coord<point>::type distance_impl(PointAccessorT const,
                                                                   point const & p1,
                                                                   point const & p2)
    {
      return distance_impl(p1, p2);
    }

    template <typename PointAccessorT, bool element_is_const>
    viennagrid::result_of::coord<point>::type
    distance_impl(PointAccessorT const & accessor,
                  point const & p1,
                  base_element<element_is_const> const & el2)
    {
      typedef typename viennagrid::result_of::point<PointAccessorT, base_element<element_is_const> >::type PointT;
      std::pair<PointT, PointT> points = closest_points(accessor, p1, el2);

      return distance_impl(points);
    }

    template <typename PointAccessorT, bool element_is_const>
    typename viennagrid::result_of::coord<PointAccessorT, base_element<element_is_const> >::type
    distance_impl(PointAccessorT const & accessor,
                  base_element<element_is_const> const & el1,
                  point const & p2)
    {
      typedef typename viennagrid::result_of::point<PointAccessorT, base_element<element_is_const> >::type PointT;
      std::pair<PointT, PointT> points = closest_points(accessor, el1, p2);


      return distance_impl(points);
    }

    // Distance between vertices: Use point distance
//     template <typename PointAccessorT, bool element1_is_const, bool element2_is_const>
//     typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type
//     distance_impl(PointAccessorT const & accessor,
//                   base_element<element1_is_const> const & v1,
//                   base_element<element2_is_const> const & v2)
//     {
//       return distance_impl(accessor(v1), accessor(v2));
//     }



    //
    // Generic distance computation: Reuse closest_points()
    //

    template <typename PointAccessorT, bool element1_is_const, bool element2_is_const>
    typename viennagrid::result_of::coord< PointAccessorT, base_element<element1_is_const> >::type
    distance_impl(PointAccessorT const & accessor,
                  base_element<element1_is_const> const & el1,
                  base_element<element2_is_const> const & el2)
    {
      if (el1.is_vertex() && el2.is_vertex())
        return distance_impl(accessor.get(el1), accessor.get(el2));
      if (el1.is_vertex())
        return distance_impl(accessor, accessor.get(el1), el2);
      if (el2.is_vertex())
        return distance_impl(accessor, el1, accessor.get(el2));

      //typedef typename result_of::point<ElementType1>::type      PointT;
      typedef typename viennagrid::result_of::point<PointAccessorT, base_element<element1_is_const> >::type PointT;
      std::pair<PointT, PointT> points = closest_points(accessor, el1, el2);

      return distance_impl(points);
    }

//     template <typename PointAccessorT, typename SomethingT1, typename SomethingT2>
//     typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type
//     distance_impl(PointAccessorT const & accessor,
//                   SomethingT1 const & el1,
//                   SomethingT2 const & el2)
//     {
//       //typedef typename result_of::point<ElementType1>::type   PointT;
//       typedef typename PointAccessorT::value_type PointT;
//
//       std::pair<PointT, PointT> points = closest_points(accessor, el1, el2);
//
//       return distance_impl( points );
//     }



    ////////////////////////////////// boundary distance //////////////////////////////////////


    inline viennagrid::result_of::coord<point>::type boundary_distance_impl(point const & p1,
                                                                            point const & p2)
    {
      return norm_2(p1 - p2);
    }

    inline viennagrid::result_of::coord<point>::type boundary_distance_impl( std::pair<point, point> const & pts )
    {
      return boundary_distance_impl(pts.first, pts.second);
    }

    template <typename PointAccessorT>
    typename viennagrid::result_of::coord<point>::type boundary_distance_impl(PointAccessorT const,
                                                                              point const & p1,
                                                                              point const & p2)
    {
      return boundary_distance_impl(p1, p2);
    }

    template <typename PointAccessorT, bool element_is_const>
    typename viennagrid::result_of::coord<point>::type boundary_distance_impl(PointAccessorT const & accessor,
                                                                              point const & p1,
                                                                              base_element<element_is_const> const & v2)
    {
      return boundary_distance_impl(p1, accessor(v2));
    }

    template <typename PointAccessorT, bool element_is_const>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type
    boundary_distance_impl(PointAccessorT const & accessor,
                  base_element<element_is_const> const & v1,
                  point const & p2)
    {
      return boundary_distance_impl(accessor(v1), p2);
    }

//     // Distance between vertices: Use point distance
//     template <typename PointAccessorT, bool element1_is_const, bool element2_is_const>
//     typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type
//     boundary_distance_impl(PointAccessorT const & accessor,
//                   base_element<element1_is_const> const & v1,
//                   base_element<element2_is_const> const & v2)
//     {
//       return boundary_distance_impl(accessor(v1), accessor(v2));
//     }



    //
    // Generic distance computation: Reuse closest_points()
    //
    template <typename PointAccessorT, typename SomethingT1, typename SomethingT2>
    typename viennagrid::result_of::coord<PointAccessorT, SomethingT1>::type
    boundary_distance_impl(PointAccessorT const & accessor,
                           SomethingT1 const & el1,
                           SomethingT2 const & el2)
    {
      //typedef typename result_of::point<ElementType1>::type      PointT;
      typedef typename viennagrid::result_of::point<PointAccessorT, SomethingT1>::type PointT;

      std::pair<PointT, PointT> points = closest_points_on_boundary(accessor, el1, el2);

      return boundary_distance_impl(points);
    }


    template <typename PointAccessorT, bool element1_is_const, bool element2_is_const>
    typename viennagrid::result_of::coord< PointAccessorT, base_element<element1_is_const> >::type
    boundary_distance_impl(PointAccessorT const & accessor,
                           base_element<element1_is_const> const & el1,
                           base_element<element2_is_const> const & el2)
    {
      if (el1.is_vertex() && el2.is_vertex())
        return boundary_distance_impl(accessor, accessor.get(el1), accessor.get(el2));
      if (el1.is_vertex())
        return boundary_distance_impl(accessor, accessor.get(el1), el2);
      if (el2.is_vertex())
        return boundary_distance_impl(accessor, el1, accessor.get(el2));

      //typedef typename result_of::point<ElementType1>::type      PointT;
      typedef typename PointAccessorT::value_type PointT;

      std::pair<PointT, PointT> points = closest_points_on_boundary(accessor, el1, el2);

      return boundary_distance_impl(points);
    }

  } //namespace detail

  //
  // The public interface functions
  //
  /** @brief Returns the distance between elements, segments and/or meshs using the point accessor provided */
  template <typename PointAccessorT, typename SomethingT1, typename SomethingT2>
  typename viennagrid::result_of::coord<SomethingT1>::type
  distance(PointAccessorT const & accessor,
           SomethingT1 const & el1,
           SomethingT2 const & el2)
  {
    return detail::distance_impl(accessor, el1, el2);
  }


  /** @brief Returns the distance between elements, segments and/or meshs using the default point accessor*/
  template <typename SomethingT1, typename SomethingT2>
  typename viennagrid::result_of::coord<SomethingT1>::type
  distance(SomethingT1 const & el1,
           SomethingT2 const & el2)
  {
    return detail::distance_impl( root_mesh_point_accessor(), el1, el2 );
  }

  /** @brief Returns the distance between a point and an element/segment/meshs using the default point accessor*/
  template <typename SomethingT>
  typename viennagrid::result_of::coord<SomethingT>::type distance(SomethingT const & el1,
                                                                   point const & el2)
  {
    return detail::distance_impl( root_mesh_point_accessor(), el1, el2 );
  }

  /** @brief Returns the distance between a point and an element/segment/meshs using the default point accessor*/
  template <typename SomethingT>
  typename viennagrid::result_of::coord<SomethingT>::type distance(point const & el1,
                                                                   SomethingT const & el2)
  {
    return detail::distance_impl( root_mesh_point_accessor(), el1, el2 );
  }

  /** @brief Returns the Euclidian distance between two points */
  inline viennagrid::result_of::coord< point >::type distance(point const & el1,
                                                              point const & el2)
  {
    return detail::distance_impl( el1, el2 );
  }


  /** @brief Returns the Euclidian distance between the boundary of a segment and a line */
  template<bool element_is_const, bool mesh_region_is_const>
  typename result_of::coord< base_region<mesh_region_is_const> >::type distance(base_element<element_is_const> const & line,
                                                                                base_region<mesh_region_is_const> const & region)
  {
    typedef base_region<mesh_region_is_const> RegionType;
    typedef typename result_of::const_element_range<RegionType>::type ConstLineRangeType;
    typedef typename result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;

    typedef typename result_of::coord<RegionType>::type CoordType;

    ConstLineRangeType lines(region, 1);
    if (lines.empty())
      return -1;

    ConstLineIteratorType lit = lines.begin();
    CoordType min_distance = distance(line, *(lit++));

    for (; lit != lines.end(); ++lit)
    {
      CoordType current_distance = distance(line, *lit);
      if (current_distance < min_distance)
        min_distance = current_distance;
    }

    return min_distance;
  }

  template<bool mesh_region_is_const, bool element_is_const>
  typename result_of::coord< base_region<mesh_region_is_const> >::type distance(base_region<mesh_region_is_const> const & region,
                                                                                base_element<element_is_const> const & line)
  {
    return distance( line, region );
  }





  /** @brief Returns the distance between elements, segments and/or meshs
    *
    * @param accessor     Accessor functor for obtaining a point from a vertex
    * @param el1          The first element/mesh/vertex
    * @param el2          The second element/mesh/vertex
    */
  template <typename PointAccessorT, typename SomethingT1, typename SomethingT2>
  typename viennagrid::result_of::coord<SomethingT1>::type
  boundary_distance(PointAccessorT const & accessor,
                    SomethingT1 const & el1,
                    SomethingT2 const & el2)
  {
    return detail::boundary_distance_impl(accessor, el1, el2);
  }

  /** @brief Returns the distance between elements, segments and/or meshs */
  template <typename SomethingT1, typename SomethingT2>
  typename viennagrid::result_of::coord<SomethingT1>::type
  boundary_distance(SomethingT1 const & el1,
                    SomethingT2 const & el2)
  {
    return detail::boundary_distance_impl( root_mesh_point_accessor(), el1, el2 );
  }

  /** @brief Returns the distance between a point and an element, segment and/or mesh */
  template <typename SomethingT>
  typename viennagrid::result_of::coord<SomethingT>::type
  boundary_distance(SomethingT const & el1,
                    point const & el2)
  {
    return detail::boundary_distance_impl( root_mesh_point_accessor(), el1, el2 );
  }

  /** @brief Returns the distance between a point and an element, segment and/or mesh */
  template <typename SomethingT>
  typename viennagrid::result_of::coord<SomethingT>::type
  boundary_distance(point const & el1,
                    SomethingT const & el2)
  {
    return detail::boundary_distance_impl( root_mesh_point_accessor(), el1, el2 );
  }


} //namespace viennagrid
#endif
