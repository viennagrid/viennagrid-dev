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


#ifndef VIENNAGRID_POINT_HPP
#define VIENNAGRID_POINT_HPP

#include <math.h>
#include <assert.h>
#include <stdexcept>
#include <cstddef>
#include <sstream>

#include "viennagrid/forwards.h"

namespace viennagrid
{
  
  /********************* CoordinateSystem *****************/
  
  /** @brief Provides the basic operations in a cartesian coordinate system */
  struct cartesian_cs 
  {
    /** @brief Returns the point p1 + p2 for a cartesian coordinate system */
    template <typename PointType>
    static PointType add(PointType const & p1, PointType const & p2)
    {
      assert(p1.size() == p2.size());
      
      PointType ret;
      for (dim_type i=0; i<ret.size(); ++i)
        ret[i] = p1[i] + p2[i];
      return ret;
    }

    /** @brief Implementation of p1 += p2 for a cartesian coordinate system */
    template <typename PointType>
    static void inplace_add(PointType & p1, PointType const & p2)
    {
      assert(p1.size() == p2.size());
      
      for (dim_type i=0; i<p1.size(); ++i)
        p1[i] += p2[i];
    }

    /** @brief Returns the point p1 - p2 for a cartesian coordinate system */
    template <typename PointType>
    static PointType subtract(PointType const & p1, PointType const & p2)
    {
      assert(p1.size() == p2.size());
      
      PointType ret;
      for (dim_type i=0; i<ret.size(); ++i)
        ret[i] = p1[i] - p2[i];
      return ret;
    }

    /** @brief Implementation of p1 -= p2 for a cartesian coordinate system */
    template <typename PointType>
    static void inplace_subtract(PointType & p1, PointType const & p2)
    {
      assert(p1.size() == p2.size());
      
      for (dim_type i=0; i<p1.size(); ++i)
        p1[i] -= p2[i];
    }
  };
  
  
  // Polar and spherical coordinate system not yet supported
  
  
  
  /***************************** Point Type ************************/
  
  class point_index_out_of_bounds_exception : public std::exception
  {
    public:
      point_index_out_of_bounds_exception(std::size_t i) {};
      
      virtual const char* what() const throw()
      {
        std::stringstream ss;
        ss << "Point index " << i_ << " out of bounds!";
        return ss.str().c_str();
      }
      
    private:
      std::size_t i_;
  };
  

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
  
  
  
  /** @brief Represents an arbitrary point in the underlying coordinate system. 
   * 
   * @tparam CoordType    Type of each coordinate entry (usually double or float)
   * @tparam d            Geometric dimension of the underlying coordinate system
   * @tparam CoordinateSystem   The underlying coordinate system of the point. 
   * 
   */
  template <typename CoordType, dim_type d, typename CoordinateSystem>
  class point
  {
    public:
      typedef CoordType       value_type;
      typedef std::size_t     size_type;
      
      point() 
      {
        point_filler<CoordType, d>::apply(coords, 0, 0, 0);  //make sure that there is no bogus in the coords-array
      }
      
      point(CoordType x, CoordType y = 0, CoordType z = 0)
      {
        point_filler<CoordType, d>::apply(coords, x, y, z);
      }
      
      CoordType & operator[](dim_type index) 
      {
        assert(index < d);
        return coords[index]; 
      }
      
      CoordType const & operator[](dim_type index) const
      {
        assert(index < d);
        return coords[index]; 
      }
      
      CoordType at(dim_type index)
      {
        if (index < 0 || index >= d)
          throw point_index_out_of_bounds_exception(index);
        
        return coords[index];
      }

      CoordType const & at(dim_type index) const
      {
        if (index < 0 || index >= d)
          throw point_index_out_of_bounds_exception(index);
        
        return coords[index];
      }
      
      /** @brief Returns the geometric dimension of the point */
      dim_type size() const { return d; }
      
      //
      // operators:
      //
      
      //with point:
      point operator+(point const & other) const
      {
        return CoordinateSystem::add(*this, other);
      }

      point & operator+=(point const & other)
      {
        CoordinateSystem::inplace_add(*this, other);
        return *this;
      }

      point operator-(point const & other) const
      {
        return CoordinateSystem::subtract(*this, other);
      }
      
      point & operator-=(point const & other)
      {
        CoordinateSystem::inplace_subtract(*this, other);
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
