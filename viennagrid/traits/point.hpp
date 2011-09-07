#ifndef VIENNAGRID_TRAITS_POINT_HPP
#define VIENNAGRID_TRAITS_POINT_HPP

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

#include "viennagrid/forwards.h"

namespace viennagrid 
{
  
  namespace traits
  {
    //
    //  Dimension retrieval
    //
    template <typename PointType>
    struct dimension;
    
    template <typename CoordType, long d, typename CoordinateSystem>
    struct dimension< point_t<CoordType, d, CoordinateSystem> >
    {
      enum { value = d };
    };

    
    //
    //  Coordinate system retrieval
    //
    template <typename PointType>
    struct coordinate_system
    {
      //by default, we don't know anything about the point type, so let's complain at compile time
      typedef typename PointType::ERROR_UNKNOWN_COORDINATE_SYSTEM_FOR_POINT_TYPE    type;
    };
    
    template <typename CoordType, long d, typename CoordinateSystem>
    struct coordinate_system< point_t<CoordType, d, CoordinateSystem> >
    {
      typedef CoordinateSystem    type;
    };

    
    //
    //  Size retrieval
    //
    template <typename PointType>
    struct static_size;

    template <typename CoordType, long d, typename CoordinateSystem>
    struct static_size< point_t<CoordType, d, CoordinateSystem> >
    {
      enum { value = d };
    };

    template <typename PointType>
    std::size_t dynamic_size(PointType const & p)
    {
      return p.size();
    }
    
    
    //
    //  Value Type retrieval
    //
    template <typename PointType>
    struct value_type;

    template <typename CoordType, long d, typename CoordinateSystem>
    struct value_type< point_t<CoordType, d, CoordinateSystem> >
    {
      typedef CoordType    type;
    };
    
  }
} 

#endif
