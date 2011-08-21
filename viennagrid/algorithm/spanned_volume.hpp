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


/************** All the code for a generic Point-Object ********************/


#ifndef VIENNAGRID_POINT_GUARD
#define VIENNAGRID_POINT_GUARD

#include <math.h>
#include "viennagrid/forwards.h"
#include "viennagrid/algorithm/cross_prod.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/traits/point.hpp"

namespace viennagrid
{

    template <typename PointType,
              typename CoordinateSystem = typename traits::coordinate_system<PointType>::type,
              dim_type dim = traits::dimension<PointType>::value>
    struct spanned_volume_impl;
    
    template <typename PointType>
    struct spanned_volume_impl<PointType, cartesian_cs, 1>
    {
      typedef typename traits::value_type<PointType>::type    value_type;

      static value_type apply(PointType const & p1,
                              PointType const & p2)
      {
        return fabs(p2[0] - p1[0]);
      }
    };

    //in 2d:
    template <typename PointType>
    struct spanned_volume_impl<PointType, cartesian_cs, 2>
    {
      typedef typename traits::value_type<PointType>::type    value_type;
      
      static value_type apply(PointType const & p1,
                              PointType const & p2)
      {
        //a line
        return sqrt(   (p2[0] - p1[0]) * (p2[0] - p1[0])
                     + (p2[1] - p1[1]) * (p2[1] - p1[1])  );
      }
      
      static value_type apply(PointType const & A,
                              PointType const & B,
                              PointType const & C)
      {
        //a triangle:
        return fabs(  A[0] * (B[1] - C[1])
                    + B[0] * (C[1] - A[1])
                    + C[0] * (A[1] - B[1]) ) / 2.0;
      }
      
    };
    

    template <typename PointType>
    struct spanned_volume_impl<PointType, cartesian_cs, 3>
    {
      typedef typename traits::value_type<PointType>::type    value_type;
      
      static value_type apply(PointType const & p1,
                              PointType const & p2)
      {
        //a line
        return sqrt(   (p2[0] - p1[0]) * (p2[0] - p1[0])
                     + (p2[1] - p1[1]) * (p2[1] - p1[1])
                     + (p2[2] - p1[2]) * (p2[2] - p1[2]) );
      }
      
      static value_type apply(PointType const & p1,
                              PointType const & p2,
                              PointType const & p3)
      {
        PointType v1 = p2 - p1;
        PointType v2 = p3 - p1;

        PointType v3 = cross_prod(v1, v2);
        
        return norm(v3) / 2.0;
      }

      static value_type apply(PointType const & p1,
                              PointType const & p2,
                              PointType const & p3,
                              PointType const & p4)
      {
        PointType v1 = p2 - p1;
        PointType v2 = p3 - p1;
        PointType v3 = p4 - p1;
        
        return fabs(inner_prod(v1, cross_prod(v2, v3)) ) / 6.0; 
      }

    };
    
    
    template <typename PointType>
    typename traits::value_type<PointType>::type 
    spanned_volume(PointType const & p1, PointType const & p2)
    {
      return spanned_volume_impl<PointType>::apply(p1, p2);
                             
    }
    
    
    template <typename PointType>
    typename traits::value_type<PointType>::type 
    spanned_volume(PointType const & p1, PointType const & p2, PointType const & p3)
    {
      return spanned_volume_impl<PointType>::apply(p1, p2, p3);
                             
    }
    
    
    template <typename PointType>
    typename traits::value_type<PointType>::type 
    spanned_volume(PointType const & p1, PointType const & p2, PointType const & p3, PointType const & p4)
    {
      return spanned_volume_impl<PointType>::apply(p1, p2, p3, p4);
                             
    }
    
    
    
    
    
/*    

    // '2D-object in 3D'
    template <typename T>
    double spannedVolume(const point<T, three_dimensions_tag> & p1,
                        const point<T, three_dimensions_tag> & p2)
    {
      double c1 = p1.coords_[0] * p2.coords_[1] - p1.coords_[1] * p2.coords_[0];
      double c2 = - p1.coords_[0] * p2.coords_[2] + p1.coords_[2] * p2.coords_[0];
      double c3 = p1.coords_[1] * p2.coords_[2] - p1.coords_[2] * p2.coords_[1];

      return sqrt( c1 * c1 + c2 * c2 + c3 * c3 );
    }

    template <typename T>
    double spannedVolume(const point<T, three_dimensions_tag> & p1,
                        const point<T, three_dimensions_tag> & p2,
                        const point<T, three_dimensions_tag> & p3)
    {
      //return det|p1 p2 p3|
      return p1.coords_[0] * p2.coords_[1] * p3.coords_[2]
            + p2.coords_[0] * p3.coords_[1] * p1.coords_[2]
            + p3.coords_[0] * p1.coords_[1] * p2.coords_[2]
            - p1.coords_[2] * p2.coords_[1] * p3.coords_[0]
            - p2.coords_[2] * p3.coords_[1] * p1.coords_[0]
            - p3.coords_[2] * p1.coords_[1] * p2.coords_[0];
    }
*/
}
#endif
