/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Josef Weinbub                   weinbub@iue.tuwien.ac.at
               Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_ALGORITHM_CROSS_PROD_HPP
#define VIENNAGRID_ALGORITHM_CROSS_PROD_HPP

#include "viennagrid/forwards.h"
#include "viennagrid/traits/point.hpp"

namespace viennagrid
{

  
  template <typename PointType,
            dim_type dim = traits::dimension<PointType>::value,
            typename CoordinateSystem = typename traits::coordinate_system<PointType>::type>
  struct cross_prod_impl;
  
  
  /*
  template<typename VectorT>
  VectorT cross_prod_impl(VectorT const& v1, VectorT const& v2)
  {
    return VectorT(v1[1]*v2[2]-v1[2]*v2[1], 
                    v1[2]*v2[0]-v1[0]*v2[2],
                    v1[0]*v2[1]-v1[1]*v2[0]);
  }

  template<typename VectorT>
  VectorT cross_prod(VectorT const& v1, VectorT const& v2)
  {
    return VectorT(v1[1]*v2[2]-v1[2]*v2[1], 
                    v1[2]*v2[0]-v1[0]*v2[2],
                    v1[0]*v2[1]-v1[1]*v2[0]);
  } */

  //for compatibility in 2d:
  template <typename PointType>
  struct cross_prod_impl<PointType, 2, cartesian_cs>
  {
    static PointType apply(PointType const & p1,
                           PointType const & p2)
    {
      return PointType(0,0);
    }
  };
  
  template <typename PointType>
  struct cross_prod_impl<PointType, 3, cartesian_cs>
  {
    static PointType apply(PointType const & p1,
                           PointType const & p2)
    {
      return PointType(p1[1]*p2[2]-p1[2]*p2[1], 
                       p1[2]*p2[0]-p1[0]*p2[2],
                       p1[0]*p2[1]-p1[1]*p2[0]);
    }
  };
  
  
  template<typename PointType>
  PointType
  cross_prod(PointType const& v1, PointType const& v2)
  {
    return cross_prod_impl<PointType>::apply(v1, v2);
  }

} 

#endif
