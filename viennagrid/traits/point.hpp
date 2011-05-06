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

#ifndef VIENNAGRID_TRAITS_POINT_HPP
#define VIENNAGRID_TRAITS_POINT_HPP

#include "viennagrid/forwards.h"
#include "viennagrid/point.hpp"

namespace viennagrid 
{
  
  namespace traits
  {
    //
    //  Dimension retrieval
    //
    template <typename PointType>
    struct dimension;
    
    template <typename CoordType, viennagrid::dim_type d, typename CoordinateSystem>
    struct dimension< point<CoordType, d, CoordinateSystem> >
    {
      enum { value = d };
    };

    
    //
    //  Coordinate system retrieval
    //
    template <typename PointType>
    struct coordinate_system;
    
    template <typename CoordType, viennagrid::dim_type d, typename CoordinateSystem>
    struct coordinate_system< point<CoordType, d, CoordinateSystem> >
    {
      typedef CoordinateSystem    type;
    };

    
    //
    //  Size retrieval
    //
    template <typename PointType>
    struct size;

    template <typename CoordType, viennagrid::dim_type d, typename CoordinateSystem>
    struct size< point<CoordType, d, CoordinateSystem> >
    {
      enum { value = d };
    };

    
    //
    //  Value Type retrieval
    //
    template <typename PointType>
    struct value_type;

    template <typename CoordType, viennagrid::dim_type d, typename CoordinateSystem>
    struct value_type< point<CoordType, d, CoordinateSystem> >
    {
      typedef CoordType    type;
    };
    
  }
} 

#endif
