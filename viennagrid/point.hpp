/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */


/************** All the code for a generic Point-Object ********************/


#ifndef VIENNAGRID_POINT_HPP
#define VIENNAGRID_POINT_HPP

#include <math.h>
#include "viennagrid/forwards.h"

namespace viennagrid
{
  /***************************** Point Type ************************/

  template <typename CoordType, dim_type d>
  struct point_filler;
  
  template <typename CoordType>
  struct point_filler<CoordType, 1>
  {
    static void apply(CoordType * coords, CoordType x, CoordType y, CoordType z)
    {
      coords[0] = x;
    }
  };
  
  template <typename CoordType>
  struct point_filler<CoordType, 2>
  {
    static void apply(CoordType * coords, CoordType x, CoordType y, CoordType z)
    {
      coords[0] = x;
      coords[1] = y;
    }
  };
  
  template <typename CoordType>
  struct point_filler<CoordType, 3>
  {
    static void apply(CoordType * coords, CoordType x, CoordType y, CoordType z)
    {
      coords[0] = x;
      coords[1] = y;
      coords[2] = z;
    }
  };
  
  
  
  
  template <typename CoordType, dim_type d, typename CoordinateSystem>
  class point
  {
    public:
      typedef CoordType       value_type;
      
      point() {}
      
      point(CoordType x, CoordType y = 0, CoordType z = 0)
      {
        point_filler<CoordType, d>::apply(coords, x, y, z);
      }
      
      CoordType & operator[](dim_type index) { return coords[index]; }
      
      CoordType const & operator[](dim_type index) const { return coords[index]; }
      
      CoordType at(dim_type index)
      {
        if (index < 0 || index >= d)
          throw "out of bounds!";
        
        return coords[index];
      }

      CoordType const & at(dim_type index) const
      {
        if (index < 0 || index >= d)
          throw "out of bounds!";
        
        return coords[index];
      }
      
      dim_type size() const { return d; }
      
      //
      // operators:
      //
      
      //with point:
      point operator+(point const & other) const
      {
        point ret;
        for (dim_type i=0; i<d; ++i)
          ret[i] = coords[i] + other[i];
        return ret;
      }

      point & operator+=(point const & other)
      {
        for (dim_type i=0; i<d; ++i)
          coords[i] += other[i];
        return *this;
      }

      point operator-(point const & other) const
      {
        point ret;
        for (dim_type i=0; i<d; ++i)
          ret[i] = coords[i] - other[i];
        return ret;
      }
      
      point & operator-=(point const & other)
      {
        for (dim_type i=0; i<d; ++i)
          coords[i] -= other[i];
        return *this;
      }
      
      
      //with CoordType
      point & operator*=(CoordType factor)
      {
        for (dim_type i=0; i<d; ++i)
          coords[i] *= factor;
        return *this;
      }
      
      point & operator/=(CoordType factor)
      {
        for (dim_type i=0; i<d; ++i)
          coords[i] /= factor;
        return *this;
      }

      point operator*(CoordType factor) const
      {
        point ret;
        for (dim_type i=0; i<d; ++i)
          ret[i] = coords[i] * factor;
        return ret;
      }

      point operator/(CoordType factor) const
      {
        point ret;
        for (dim_type i=0; i<d; ++i)
          ret[i] = coords[i] / factor;
        return ret;
      }

    private:
      CoordType coords[d];
  };

  
  template <typename CoordType, dim_type d, typename CoordinateSystem>
  std::ostream& operator << (std::ostream & os, point<CoordType, d, CoordinateSystem> const & p)
  {
    for (dim_type i=0; i<d; ++i)
      os << p[i] << " ";
    return os;
  }
  
  
}
#endif
