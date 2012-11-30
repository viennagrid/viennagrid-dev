#ifndef VIENNAGRID_ALGORITHM_CROSS_PROD_HPP
#define VIENNAGRID_ALGORITHM_CROSS_PROD_HPP

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

#include "viennagrid/forwards.hpp"
#include "viennagrid/traits/point.hpp"
#include "viennagrid/point.hpp"

/** @file cross_prod.hpp
    @brief Computes the cross product of vectors
*/

namespace viennagrid
{

  namespace detail
  {
    /** @brief Implementation of the cross product calculation */
    template <typename PointType,
              long dim = traits::dimension<PointType>::value>
    struct cross_prod_impl;
    
    
    //for compatibility in 1d:
    /** @brief Implementation of the cross product calculation in 1d for compatibility. Returns the zero-vector. */
    template <typename PointType>
    struct cross_prod_impl<PointType, 1>
    {
      static PointType apply(PointType const & p1,
                            PointType const & p2)
      {
        return PointType(0);
      }
    };
    
    //for compatibility in 2d:
    /** @brief Implementation of the cross product calculation in 2d for compatibility. Returns the zero-vector. */
    template <typename PointType>
    struct cross_prod_impl<PointType, 2>
    {
      static PointType apply(PointType,
                            PointType)
      {
        return PointType(0,0);
      }
    };
    
    /** @brief Implementation of the cross product calculation in 3d. */
    template <typename PointType>
    struct cross_prod_impl<PointType, 3>
    {
      static PointType apply(PointType const & p1,
                            PointType const & p2)
      {
        return PointType(p1[1]*p2[2]-p1[2]*p2[1], 
                        p1[2]*p2[0]-p1[0]*p2[2],
                        p1[0]*p2[1]-p1[1]*p2[0]);
      }
    };
  }
  
  /** @brief Dispatched cross product function having information about the coordinate systems. Transforms to Cartesian coordinates and forwards to calculation */
  template<typename PointType1, typename PointType2, typename CSystem1, typename CSystem2>
  PointType1
  cross_prod_impl(PointType1 const & p1, PointType2 const & p2, CSystem1 const &, CSystem2 const &)
  {
    typedef typename traits::value_type<PointType1>::type    value_type;
    typedef typename result_of::cartesian_point<PointType1>::type   CartesianPoint1;
    
    return detail::cross_prod_impl<CartesianPoint1>::apply(to_cartesian(p1), to_cartesian(p2));
  }

  /** @brief Dispatched cross product function having information about the coordinate systems. Points are already in Cartesian coordinates, thus forwarding to the worker function directly. */
  template<typename PointType1, typename PointType2, long d>
  PointType1
  cross_prod_impl(PointType1 const & p1, PointType2 const & p2, cartesian_cs<d>, cartesian_cs<d>)
  {
    return detail::cross_prod_impl<PointType1>::apply(p1, p2);
  }

  /** @brief Returns the cross product of two vectors. If the vectors have one or two components only, the zero vector is returned.
   * 
   * @param v1    The first vector given in some coordinate system
   * @param v2    The second vector given in another coordinate system
   */
  template<typename PointType1, typename PointType2>
  PointType1
  cross_prod(PointType1 const& v1, PointType2 const& v2)
  {
    return cross_prod_impl(v1,
                           v2,
                           typename traits::coordinate_system<PointType1>::type(),
                           typename traits::coordinate_system<PointType2>::type());
  }

} 

#endif
