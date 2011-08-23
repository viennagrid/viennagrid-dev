/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
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

#ifndef VIENNAGRID_ALGORITHM_INNERPROD_HPP
#define VIENNAGRID_ALGORITHM_INNERPROD_HPP

#include "viennagrid/forwards.h"
#include "viennagrid/traits/point.hpp"

namespace viennagrid 
{

  template <typename PointType,
            dim_type dim = traits::dimension<PointType>::value,
            typename CoordinateSystem = typename traits::coordinate_system<PointType>::type>
  struct inner_prod_impl;

  template <typename PointType>
  struct inner_prod_impl<PointType, 1, cartesian_cs>
  {
    typedef typename traits::value_type<PointType>::type    value_type;

    static value_type apply(PointType const & p1,
                            PointType const & p2)
    {
      return p1[0] * p2[0];
    }
  };
  
  template <typename PointType>
  struct inner_prod_impl<PointType, 2, cartesian_cs>
  {
    typedef typename traits::value_type<PointType>::type    value_type;

    static value_type apply(PointType const & p1,
                            PointType const & p2)
    {
      return p1[0] * p2[0] + p1[1] * p2[1];
    }
  };
  
  template <typename PointType>
  struct inner_prod_impl<PointType, 3, cartesian_cs>
  {
    typedef typename traits::value_type<PointType>::type    value_type;

    static value_type apply(PointType const & p1,
                            PointType const & p2)
    {
      return p1[0] * p2[0] + p1[1] * p2[1] + p1[2] * p2[2];
    }
  };
  
  
  template<typename PointType>
  typename traits::value_type<PointType>::type
  inner_prod(PointType const& v1, PointType const& v2)
  {
    return inner_prod_impl<PointType>::apply(v1, v2);
  }

} 

#endif
