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

#include <cmath>
#include <assert.h>
#include <stdexcept>
#include <cstddef>
#include <sstream>

#include "viennagrid/forwards.h"
#include "viennagrid/traits/point.hpp"

namespace viennagrid
{
  
  template <dim_type d>
  struct dim_dispatcher;
  
  //conversion facility:
  template <typename FromPointType, 
            typename ToPointType,
            typename FromCoordinateSystem = typename traits::coordinate_system<FromPointType>::type,
            typename ToCoordinateSystem = typename traits::coordinate_system<ToPointType>::type 
           >
  class coordinate_converter
  {
    public:
      ToPointType operator()(FromPointType const & p_in)
      {
        typedef typename FromPointType::ERROR_COORDINATE_SYSTEM_UNKNOWN   type;
      }
  };
  
  
  //
  // conversion from Cartesian coordinate system
  //
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, cartesian_cs, polar_cs>
  {
    public:
      ToPointType operator()(FromPointType const & p_in)
      {
        ToPointType ret;
        ret[0] = sqrt(p_in[0] * p_in[0] + p_in[1] * p_in[1]);
        ret[1] = atan2(p_in[1], p_in[0]);
        return ret;
      }
  };
  
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, cartesian_cs, spherical_cs>
  {
    public:
      ToPointType operator()(FromPointType const & p_in)
      {
        ToPointType ret;
        ret[0] = sqrt(p_in[0] * p_in[0] + p_in[1] * p_in[1] + p_in[2] * p_in[2]);
        ret[1] = (ret[0] != 0) ? acos(p_in[2] / ret[0]) : 0;
        ret[2] = atan2(p_in[1], p_in[0]);
        return ret;
      }
  };
  
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, cartesian_cs, cylindrical_cs>
  {
    public:
      ToPointType operator()(FromPointType const & p_in)
      {
        ToPointType ret;
        ret[0] = sqrt(p_in[0] * p_in[0] + p_in[1] * p_in[1]);
        ret[1] = atan2(p_in[1], p_in[0]);
        ret[2] = p_in[2];
        return ret;
      }
  };

  //
  // convenience function for conversion to cartesian coordinates
  //
  namespace result_of
  {
    template <typename PointType>
    struct cartesian_point
    {
      typedef viennagrid::point<typename traits::value_type<PointType>::type,
                                PointType::dim,
                                viennagrid::cartesian_cs>                    type;
    };
    
    /*template <typename PointType>
    struct cartesian_point<PointType, viennagrid::cartesian_cs>
    {
      typedef PointType         type;
    };*/

  }
  
  
  template <typename PointType, typename CoordinateSystem>
  typename result_of::cartesian_point<PointType>::type
  to_cartesian_impl(PointType const & p, CoordinateSystem const &)
  {
    typedef typename result_of::cartesian_point<PointType>::type CartesianPointType;
    
    return coordinate_converter<PointType, CartesianPointType>()(p);
  }

  //public interface
  template <typename PointType>
  typename result_of::cartesian_point<PointType>::type
  to_cartesian(PointType const & p)
  {
    return to_cartesian_impl(p, typename traits::coordinate_system<PointType>::type());
  }
  

  //
  // conversion from polar coordinate system
  //
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, polar_cs, cartesian_cs>
  {
    public:
      ToPointType operator()(FromPointType const & p_in)
      {
        ToPointType ret;
        ret[0] = p_in[0] * cos(p_in[1]);
        ret[1] = p_in[0] * sin(p_in[1]);
        return ret;
      }
  };
  

  //
  // conversion from spherical coordinate system (r, theta, phi)
  //
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, spherical_cs, cartesian_cs>
  {
    public:
      ToPointType operator()(FromPointType const & p_in)
      {
        ToPointType ret;
        ret[0] = p_in[0] * sin(p_in[1]) * cos(p_in[2]);
        ret[1] = p_in[0] * sin(p_in[1]) * sin(p_in[2]);
        ret[2] = p_in[0] * cos(p_in[1]);
        return ret;
      }
  };

  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, spherical_cs, cylindrical_cs>
  {
    public:
      ToPointType operator()(FromPointType const & p_in)
      {
        ToPointType ret;
        ret[0] = p_in[0] * sin(p_in[1]);                 //rho
        ret[1] = p_in[2];                                //phi
        ret[2] = p_in[0] * cos(p_in[1]);                 //z
        return ret;
      }
  };

  
  //
  // conversion from spherical cylindrical system (rho, phi, z)
  //
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, cylindrical_cs, cartesian_cs>
  {
    public:
      ToPointType operator()(FromPointType const & p_in)
      {
        ToPointType ret;
        ret[0] = p_in[0] * cos(p_in[1]);
        ret[1] = p_in[0] * sin(p_in[1]);
        ret[2] = p_in[2];
        return ret;
      }
  };

  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, cylindrical_cs, spherical_cs>
  {
    public:
      ToPointType operator()(FromPointType const & p_in)
      {
        ToPointType ret;
        ret[0] = sqrt(p_in[0] * p_in[0] + p_in[2] * p_in[2]);
        ret[1] = (ret[0] != 0) ? acos(p_in[2] / ret[0]) : 0;
        ret[2] = p_in[1];
        return ret;
      }
  };
  
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
    
    /** @brief Implementation of p1 -= p2 for a cartesian coordinate system */
    template <typename PointType>
    static PointType & inplace_stretch(PointType & p1, typename traits::value_type<PointType>::type factor)
    {
      for (dim_type i=0; i<p1.size(); ++i)
        p1[i] *= factor;
      return p1;
    }
    
  };
  
  template <typename PointType>
  PointType const &
  to_cartesian_impl(PointType const & p, cartesian_cs)
  {
    return p;
  }

  template <typename PointType>
  PointType &
  to_cartesian_impl(PointType & p, cartesian_cs)
  {
    return p;
  }

  
  
  /** @brief Common base for all non-cartesian coordinate systems */
  template <typename CSystem>
  struct cs_base
  {
    /** @brief Returns the point p1 + p2 for a polar coordinate system */
    template <typename PointType>
    static PointType add(PointType const & p1, PointType const & p2)
    {
      typedef point<typename PointType::value_type, PointType::dim, cartesian_cs>      CartesianPointType;
      
      assert(p1.size() == p2.size());
      
      coordinate_converter<PointType, CartesianPointType, CSystem, cartesian_cs>  cs_to_cartesian;
      coordinate_converter<CartesianPointType, PointType, cartesian_cs, CSystem>  cartesian_to_cs;
      
      CartesianPointType cs1 = cs_to_cartesian(p1);
      CartesianPointType cs2 = cs_to_cartesian(p2);
      CartesianPointType cs_ret = cartesian_cs::add(cs1, cs2);
      PointType ret = cartesian_to_cs(cs_ret);
      
      return ret;
    }

    /** @brief Implementation of p1 += p2 for a polar coordinate system */
    template <typename PointType>
    static void inplace_add(PointType & p1, PointType const & p2)
    {
      typedef point<typename PointType::value_type, PointType::dim, cartesian_cs>      CartesianPointType;
      
      assert(p1.size() == p2.size());
      
      coordinate_converter<PointType, CartesianPointType, CSystem, cartesian_cs>  cs_to_cartesian;
      coordinate_converter<CartesianPointType, PointType, cartesian_cs, CSystem>  cartesian_to_cs;
      
      CartesianPointType cs1 = cs_to_cartesian(p1);
      CartesianPointType cs2 = cs_to_cartesian(p2);
      cartesian_cs::inplace_add(cs1, cs2);
      p1 = cartesian_to_cs(cs1);
    }

    /** @brief Returns the point p1 - p2 for a polar coordinate system */
    template <typename PointType>
    static PointType subtract(PointType const & p1, PointType const & p2)
    {
      typedef point<typename PointType::value_type, PointType::dim, cartesian_cs>      CartesianPointType;
      
      assert(p1.size() == p2.size());
      
      coordinate_converter<PointType, CartesianPointType, CSystem, cartesian_cs>  cs_to_cartesian;
      coordinate_converter<CartesianPointType, PointType, cartesian_cs, CSystem>  cartesian_to_cs;
      
      CartesianPointType cs1 = cs_to_cartesian(p1);
      CartesianPointType cs2 = cs_to_cartesian(p2);
      CartesianPointType cs_ret = cartesian_cs::subtract(cs1, cs2);
      PointType ret = cartesian_to_cs(cs_ret);
      
      return ret;
    }

    /** @brief Implementation of p1 -= p2 for a polar coordinate system */
    template <typename PointType>
    static void inplace_subtract(PointType & p1, PointType const & p2)
    {
      typedef point<typename PointType::value_type, PointType::dim, cartesian_cs>      CartesianPointType;
      
      assert(p1.size() == p2.size());
      
      coordinate_converter<PointType, CartesianPointType, CSystem, cartesian_cs>  cs_to_cartesian;
      coordinate_converter<CartesianPointType, PointType, cartesian_cs, CSystem>  cartesian_to_cs;
      
      CartesianPointType cs1 = cs_to_cartesian(p1);
      CartesianPointType cs2 = cs_to_cartesian(p2);
      cartesian_cs::inplace_subtract(cs1, cs2);
      p1 = cartesian_to_cs(cs1);
    }
  };
  
  /** @brief Provides the basic operations in a polar coordinate system (r, phi) */
  struct polar_cs : public cs_base<polar_cs> 
  {
    template <typename PointType>
    static PointType & inplace_stretch(PointType & p1, typename traits::value_type<PointType>::type factor)
    {
      p1[0] *= factor;
      return p1;
    }
  };
  
  /** @brief Provides the basic operations in a spherical coordinate system (r, theta, phi) with theta denoting the elevation angle. */
  struct spherical_cs : public cs_base<spherical_cs> 
  {
    template <typename PointType>
    static PointType & inplace_stretch(PointType & p1, typename traits::value_type<PointType>::type factor)
    {
      p1[0] *= factor;
      return p1;
    }
  };

  /** @brief Provides the basic operations in a cylindrical coordinate system (rho, theta, z) */
  struct cylindrical_cs : public cs_base<cylindrical_cs> 
  {
    /** @brief Implementation of p1 *= factor for a cylindrical coordinate system */
    template <typename PointType>
    static PointType & inplace_stretch(PointType & p1, typename traits::value_type<PointType>::type factor)
    {
      typedef point<typename PointType::value_type, PointType::dim, cartesian_cs>      CartesianPointType;
      
      coordinate_converter<PointType, CartesianPointType, cylindrical_cs, cartesian_cs>  cs_to_cartesian;
      coordinate_converter<CartesianPointType, PointType, cartesian_cs, cylindrical_cs>  cartesian_to_cs;
      
      CartesianPointType cs1 = cs_to_cartesian(p1);
      cartesian_cs::inplace_stretch(cs1, factor);
      p1 = cartesian_to_cs(cs1);
      return p1;
    }
  };
  
  
  
  
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
      
      enum { dim = d };
      
      point() 
      {
        point_filler<CoordType, d>::apply(coords, 0, 0, 0);  //make sure that there is no bogus in the coords-array
      }
      
      point(CoordType x, CoordType y = 0, CoordType z = 0)
      {
        point_filler<CoordType, d>::apply(coords, x, y, z);
      }
      
      template <typename CoordType2, typename CoordinateSystem2>
      point(point<CoordType2, d, CoordinateSystem2> const & p2)
      {
        //std::cout << "Copy CTOR!" << std::endl;
        *this = coordinate_converter<point<CoordType2, d, CoordinateSystem2>, point>()(p2);
      }
      
      point & operator=(point const & p2)
      {
        for (size_type i=0; i<d; ++i)
          coords[i] = p2.coords[i];
        return *this;
      }
      
      
      template <typename CoordType2, typename CoordinateSystem2>
      point & operator=(point<CoordType2, d, CoordinateSystem2> const & p2)
      {
        //std::cout << "Assignment operator!" << std::endl;
        *this = coordinate_converter<point<CoordType2, d, CoordinateSystem2>, point>()(p2);
        return *this;
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
        CoordinateSystem::inplace_stretch(*this, factor);
        //for (dim_type i=0; i<d; ++i)
        //  coords[i] *= factor;
        return *this;
      }
      
      point & operator/=(CoordType factor)
      {
        CoordinateSystem::inplace_stretch(*this, 1.0 / factor);
        //for (dim_type i=0; i<d; ++i)
        //  coords[i] /= factor;
        return *this;
      }

      point operator*(CoordType factor) const
      {
        point ret(*this);
        return CoordinateSystem::inplace_stretch(ret, factor);
        //for (dim_type i=0; i<d; ++i)
        //  ret[i] = coords[i] * factor;
        //return ret;
      }

      point operator/(CoordType factor) const
      {
        point ret(*this);
        return CoordinateSystem::inplace_stretch(ret, 1.0 / factor);
        //point ret;
        //for (dim_type i=0; i<d; ++i)
        //  ret[i] = coords[i] / factor;
        //return ret;
      }

    private:
      CoordType coords[d];
  };

  
  template <typename CoordType, dim_type d, typename CoordinateSystem>
  point<CoordType, d, CoordinateSystem>
  operator*(double val, point<CoordType, d, CoordinateSystem> const & p)
  {
    return p * val;
  }

/*  template <typename CoordType, dim_type d, typename CoordinateSystem>
  point<CoordType, d, CoordinateSystem>
  operator*(CoordType val, point<CoordType, d, CoordinateSystem> const & p)
  {
    return p * val;
  }*/

  template <typename CoordType, dim_type d, typename CoordinateSystem>
  std::ostream& operator << (std::ostream & os, point<CoordType, d, CoordinateSystem> const & p)
  {
    for (dim_type i=0; i<d; ++i)
      os << p[i] << " ";
    return os;
  }
  
  
}
#endif
