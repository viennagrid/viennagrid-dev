#ifndef VIENNAGRID_POINT_HPP
#define VIENNAGRID_POINT_HPP

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


#include <cmath>
#include <assert.h>
#include <stdexcept>
#include <cstddef>
#include <sstream>

#include "viennagrid/forwards.h"
#include "viennagrid/traits/point.hpp"

namespace viennagrid
{
  
  template <long d>
  struct dim_dispatcher;
  
  /** @brief A conversion facility for transformations between coordinate systems 
   * 
   * @tparam FromPointType   The source point type
   * @tparam ToPointType     The destination point type
   */
  template <typename FromPointType, 
            typename ToPointType,
            typename FromCoordinateSystem = typename traits::coordinate_system<FromPointType>::type,
            typename ToCoordinateSystem = typename traits::coordinate_system<ToPointType>::type 
           >
  class coordinate_converter
  {
    public:
      /** @brief By default, conversion is unknown, thus an error is forced when calling the functor */
      ToPointType operator()(FromPointType const & p_in)
      {
        typedef typename FromPointType::ERROR_COORDINATE_SYSTEM_UNKNOWN   type;
      }
  };
  
  
  /** @brief A functor for the transformation from two-dimensional Cartesian coordinates to polar coodinates */
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, cartesian_cs<2>, polar_cs>
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
  
  /** @brief A functor for the transformation from three-dimensional Cartesian coordinates to spherical coodinates */
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, cartesian_cs<3>, spherical_cs>
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
  
  /** @brief A functor for the transformation from three-dimensional Cartesian coordinates to cylindrical coodinates */
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, cartesian_cs<3>, cylindrical_cs>
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

  namespace result_of
  {
    /** @brief Convenience metafunction for the conversion to Cartesian coordinates */
    template <typename PointType>
    struct cartesian_point
    {
      typedef viennagrid::point_t<typename traits::value_type<PointType>::type,
                                  viennagrid::cartesian_cs<viennagrid::traits::static_size<PointType>::value>
                                 >                    type;
    };

  }
  
  /** @brief Helper function for the conversion to Cartesian coordinates. Should not be called directly. Use to_cartesian() instead. */
  template <typename PointType, typename CoordinateSystem>
  typename result_of::cartesian_point<PointType>::type
  to_cartesian_impl(PointType const & p, CoordinateSystem const &)
  {
    typedef typename result_of::cartesian_point<PointType>::type CartesianPointType;
    
    return coordinate_converter<PointType, CartesianPointType>()(p);
  }

  //public interface
  /** @brief Convenience function for converting a point to Cartesian coordinates.
   * 
   * @tparam PointType   A point type for which the traits::coordinate_system<> metafunction can deduce the coordinate system */
  template <typename PointType>
  typename result_of::cartesian_point<PointType>::type
  to_cartesian(PointType const & p)
  {
    return to_cartesian_impl(p, typename traits::coordinate_system<PointType>::type());
  }
  

  /** @brief A functor for the transformation from polar coordinates to two-dimensional Cartesian coordinates. */
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, polar_cs, cartesian_cs<2> >
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
  

  /** @brief A functor for the transformation from spherical coodinates to three-dimensional Cartesian coordinates. */
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, spherical_cs, cartesian_cs<3> >
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

  /** @brief A functor for the transformation from spherical coodinates to cylindrical coordinates. */
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

  
  /** @brief A functor for the transformation from cylindrical coodinates to three-dimensional Cartesian coordinates. */
  template <typename FromPointType, 
            typename ToPointType>
  class coordinate_converter<FromPointType, ToPointType, cylindrical_cs, cartesian_cs<3> >
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

  /** @brief A functor for the transformation from cylindrical coordinates to spherical coodinates. */
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
  template <long d>
  struct cartesian_cs 
  {
    enum { dim = d };
    
    /** @brief Returns the point p1 + p2 for a cartesian coordinate system */
    template <typename PointType>
    static PointType add(PointType const & p1, PointType const & p2)
    {
      assert(p1.size() == p2.size());
      
      PointType ret;
      typedef typename PointType::size_type      size_type;
      for (size_type i=0; i<ret.size(); ++i)
        ret[i] = p1[i] + p2[i];
      return ret;
    }

    /** @brief Implementation of p1 += p2 for a cartesian coordinate system */
    template <typename PointType>
    static void inplace_add(PointType & p1, PointType const & p2)
    {
      assert(p1.size() == p2.size());
      
      typedef typename PointType::size_type      size_type;
      for (size_type i=0; i<p1.size(); ++i)
        p1[i] += p2[i];
    }

    /** @brief Returns the point p1 - p2 for a cartesian coordinate system */
    template <typename PointType>
    static PointType subtract(PointType const & p1, PointType const & p2)
    {
      typedef typename PointType::size_type      size_type;
      assert(p1.size() == p2.size());
      
      PointType ret;
      for (size_type i=0; i<ret.size(); ++i)
        ret[i] = p1[i] - p2[i];
      return ret;
    }

    /** @brief Implementation of p1 -= p2 for a cartesian coordinate system */
    template <typename PointType>
    static void inplace_subtract(PointType & p1, PointType const & p2)
    {
      typedef typename PointType::size_type      size_type;
      assert(p1.size() == p2.size());
      
      for (size_type i=0; i<p1.size(); ++i)
        p1[i] -= p2[i];
    }
    
    /** @brief Implementation of p1 -= p2 for a cartesian coordinate system */
    template <typename PointType>
    static PointType & inplace_stretch(PointType & p1, typename traits::value_type<PointType>::type factor)
    {
      typedef typename PointType::size_type      size_type;
      for (size_type i=0; i<p1.size(); ++i)
        p1[i] *= factor;
      return p1;
    }
    
  };
  
  /** @brief Helper function for the transformation of any point to Cartesian coordinates. Should not be called directly - use to_cartesian() instead. */
  template <typename PointType, long d>
  PointType const &
  to_cartesian_impl(PointType const & p, cartesian_cs<d>)
  {
    return p;
  }

  /** @brief Helper function for the transformation of any point to Cartesian coordinates. Should not be called directly - use to_cartesian() instead. */
  template <typename PointType, long d>
  PointType &
  to_cartesian_impl(PointType & p, cartesian_cs<d>)
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
      static const int DIM = viennagrid::traits::static_size<PointType>::value;
      
      typedef point_t<typename PointType::value_type, 
                      cartesian_cs<DIM> 
                     >      CartesianPointType;
      
      assert(p1.size() == p2.size());
      
      coordinate_converter<PointType, CartesianPointType,
                          CSystem, cartesian_cs<DIM> >  cs_to_cartesian;
      coordinate_converter<CartesianPointType, PointType, 
                           cartesian_cs<DIM>, CSystem>  cartesian_to_cs;
      
      CartesianPointType cs1 = cs_to_cartesian(p1);
      CartesianPointType cs2 = cs_to_cartesian(p2);
      CartesianPointType cs_ret = cartesian_cs<DIM>::add(cs1, cs2);
      PointType ret = cartesian_to_cs(cs_ret);
      
      return ret;
    }

    /** @brief Implementation of p1 += p2 for a polar coordinate system */
    template <typename PointType>
    static void inplace_add(PointType & p1, PointType const & p2)
    {
      static const int DIM = viennagrid::traits::static_size<PointType>::value;
      
      typedef point_t<typename PointType::value_type, 
                      cartesian_cs<DIM> 
                     >      CartesianPointType;
      
      assert(p1.size() == p2.size());
      
      coordinate_converter<PointType, CartesianPointType, CSystem, cartesian_cs<DIM> >  cs_to_cartesian;
      coordinate_converter<CartesianPointType, PointType, cartesian_cs<DIM>, CSystem>  cartesian_to_cs;
      
      CartesianPointType cs1 = cs_to_cartesian(p1);
      CartesianPointType cs2 = cs_to_cartesian(p2);
      cartesian_cs<DIM>::inplace_add(cs1, cs2);
      p1 = cartesian_to_cs(cs1);
    }

    /** @brief Returns the point p1 - p2 for a polar coordinate system */
    template <typename PointType>
    static PointType subtract(PointType const & p1, PointType const & p2)
    {
      static const int DIM = viennagrid::traits::static_size<PointType>::value;
      
      typedef point_t<typename PointType::value_type, 
                      cartesian_cs<DIM> 
                     >      CartesianPointType;
      
      assert(p1.size() == p2.size());
      
      coordinate_converter<PointType, CartesianPointType, CSystem, cartesian_cs<DIM> >  cs_to_cartesian;
      coordinate_converter<CartesianPointType, PointType, cartesian_cs<DIM>, CSystem>  cartesian_to_cs;
      
      CartesianPointType cs1 = cs_to_cartesian(p1);
      CartesianPointType cs2 = cs_to_cartesian(p2);
      CartesianPointType cs_ret = cartesian_cs<DIM>::subtract(cs1, cs2);
      PointType ret = cartesian_to_cs(cs_ret);
      
      return ret;
    }

    /** @brief Implementation of p1 -= p2 for a polar coordinate system */
    template <typename PointType>
    static void inplace_subtract(PointType & p1, PointType const & p2)
    {
      static const int DIM = viennagrid::traits::static_size<PointType>::value;
      
      typedef point_t<typename PointType::value_type, 
                      cartesian_cs<DIM> 
                     >      CartesianPointType;
      
      assert(p1.size() == p2.size());
      
      coordinate_converter<PointType, CartesianPointType, CSystem, cartesian_cs<DIM> >  cs_to_cartesian;
      coordinate_converter<CartesianPointType, PointType, cartesian_cs<DIM>, CSystem>  cartesian_to_cs;
      
      CartesianPointType cs1 = cs_to_cartesian(p1);
      CartesianPointType cs2 = cs_to_cartesian(p2);
      cartesian_cs<DIM>::inplace_subtract(cs1, cs2);
      p1 = cartesian_to_cs(cs1);
    }
  };
  
  /** @brief Provides the basic operations in a polar coordinate system (r, phi) */
  struct polar_cs : public cs_base<polar_cs> 
  {
    enum { dim = 2 };
    
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
    enum { dim = 3 };
    
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
    enum { dim = 3 };
    
    /** @brief Implementation of p1 *= factor for a cylindrical coordinate system */
    template <typename PointType>
    static PointType & inplace_stretch(PointType & p1, typename traits::value_type<PointType>::type factor)
    {
      static const int DIM = viennagrid::traits::static_size<PointType>::value;
      
      typedef point_t<typename PointType::value_type, 
                      cartesian_cs<DIM> 
                     >      CartesianPointType;
      
      coordinate_converter<PointType, CartesianPointType, cylindrical_cs, cartesian_cs<DIM> >  cs_to_cartesian;
      coordinate_converter<CartesianPointType, PointType, cartesian_cs<DIM>, cylindrical_cs>  cartesian_to_cs;
      
      CartesianPointType cs1 = cs_to_cartesian(p1);
      cartesian_cs<DIM>::inplace_stretch(cs1, factor);
      p1 = cartesian_to_cs(cs1);
      return p1;
    }
  };
  
  
  
  
  /***************************** Point Type ************************/
  
  /** @brief An exception class for invalid point indices */
  class point_index_out_of_bounds_exception : public std::exception
  {
    public:
      point_index_out_of_bounds_exception(std::size_t i) : i_(i) {};
      
      virtual const char* what() const throw()
      {
        std::stringstream ss;
        ss << "Point index " << i_ << " out of bounds!";
        return ss.str().c_str();
      }
      
    private:
      std::size_t i_;
  };
  

  /** @brief A helper class for filling point coordinates with values. */
  template <typename CoordType, long d>
  struct point_filler
  {
    static void apply(CoordType * coords, CoordType x, CoordType y, CoordType z)
    {
      coords[0] = x;
      coords[1] = y;
      coords[2] = z;
      for (long i=3; i<d; ++i)
        coords[i] = 0;
    }
  };
  
  /** @brief A helper class for filling point coordinates with values. Specialization for one dimension */
  template <typename CoordType>
  struct point_filler<CoordType, 1>
  {
    static void apply(CoordType * coords, CoordType x, CoordType, CoordType)
    {
      coords[0] = x;
    }
  };
  
  /** @brief A helper class for filling point coordinates with values. Specialization for two dimensions */
  template <typename CoordType>
  struct point_filler<CoordType, 2>
  {
    static void apply(CoordType * coords, CoordType x, CoordType y, CoordType)
    {
      coords[0] = x;
      coords[1] = y;
    }
  };
  
  /** @brief A helper class for filling point coordinates with values. Specialization for three dimensions */
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
  template <typename CoordType, typename CoordinateSystem>
  class point_t
  {
    public:
      /** @brief Publish the underlying numberic type of coordinates */
      typedef CoordType       value_type;
      /** @brief STL-compatible typedef for the value returned by size() */
      typedef dim_type        size_type;
      
      /** @brief Publish the geometric dimension of the point */
      enum { dim = CoordinateSystem::dim };
      
      /** @brief Default constructor. Sets all entries to zero */
      point_t() 
      {
        point_filler<CoordType, dim>::apply(coords, 0, 0, 0);  //make sure that there is no bogus in the coords-array
      }
      
      /** @brief Convenience constructor, initializing the point components with values */
      point_t(CoordType x, CoordType y = 0, CoordType z = 0)
      {
        point_filler<CoordType, dim>::apply(coords, x, y, z);
      }
      
      /** @brief Constructor taking a point given in a different coordinate system */
      template <typename CoordType2, typename CoordinateSystem2>
      point_t(point_t<CoordType2, CoordinateSystem2> const & p2)
      {
        //std::cout << "Copy CTOR!" << std::endl;
        *this = coordinate_converter<point_t<CoordType2, CoordinateSystem2>, point_t>()(p2);
      }

      //explicit copy CTOR
      /** @brief Copy constructor. Copies the entries. */
      point_t(point_t const & other)
      {
        for (size_type i=0; i<size_type(dim); ++i)
          coords[i] = other.coords[i];
      }

      /** @brief Assignment operator for a point with the same coordinate system */
      point_t & operator=(point_t const & p2)
      {
        for (size_type i=0; i<size_type(dim); ++i)
          coords[i] = p2.coords[i];
        return *this;
      }
      
      
      /** @brief Assignment operator for a point given in a different coordinate system */
      template <typename CoordType2, typename CoordinateSystem2>
      point_t & operator=(point_t<CoordType2, CoordinateSystem2> const & p2)
      {
        //std::cout << "Assignment operator!" << std::endl;
        *this = coordinate_converter<point_t<CoordType2, CoordinateSystem2>, point_t>()(p2);
        return *this;
      }
      
      /** @brief Read/Write access to the coordinate at position 'index' */
      CoordType & operator[](size_type index) 
      {
        assert(index < dim);
        return coords[index]; 
      }
      
      /** @brief Read access to the coordinate at position 'index' */
      CoordType const & operator[](size_type index) const
      {
        assert(index < dim);
        return coords[index]; 
      }
      
      /** @brief STL-type checked read/write access to the coordinate at position 'index'. Throws an exception if 'index' is invalid. */
      CoordType at(size_type index)
      {
        if (index < 0 || index >= dim)
          throw point_index_out_of_bounds_exception(index);
        
        return coords[index];
      }

      /** @brief STL-type checked read access to the coordinate at position 'index'. Throws an exception if 'index' is invalid. */
      CoordType const & at(size_type index) const
      {
        if (index < 0 || index >= dim)
          throw point_index_out_of_bounds_exception(index);
        
        return coords[index];
      }
      
      /** @brief Returns the geometric dimension of the point */
      size_type size() const { return dim; }
      
      //
      // operators:
      //
      
      //with point:
      /** @brief Convenience overload for the addition of points. */
      point_t operator+(point_t const & other) const
      {
        return CoordinateSystem::add(*this, other);
      }

      /** @brief Convenience overload for the inplace addition of points. */
      point_t & operator+=(point_t const & other)
      {
        CoordinateSystem::inplace_add(*this, other);
        return *this;
      }

      /** @brief Convenience overload for the subtraction of points. */
      point_t operator-(point_t const & other) const
      {
        return CoordinateSystem::subtract(*this, other);
      }
      
      /** @brief Convenience overload for the inplace subtraction of points. */
      point_t & operator-=(point_t const & other)
      {
        CoordinateSystem::inplace_subtract(*this, other);
        return *this;
      }
      
      
      //with CoordType
      /** @brief Convenience overload for stretching the vector given by the point. The vector is stretched in the Euclidian space - thus coordinates are transformed appropriately if required */
      point_t & operator*=(CoordType factor)
      {
        CoordinateSystem::inplace_stretch(*this, factor);
        //for (size_type i=0; i<d; ++i)
        //  coords[i] *= factor;
        return *this;
      }
      
      /** @brief Convenience overload for stretching the vector given by the point. The vector is stretched in the Euclidian space - thus coordinates are transformed appropriately if required */
      point_t & operator/=(CoordType factor)
      {
        CoordinateSystem::inplace_stretch(*this, 1.0 / factor);
        //for (size_type i=0; i<d; ++i)
        //  coords[i] /= factor;
        return *this;
      }

      /** @brief Convenience overload for stretching the vector given by the point. The vector is stretched in the Euclidian space - thus coordinates are transformed appropriately if required */
      point_t operator*(CoordType factor) const
      {
        point_t ret(*this);
        return CoordinateSystem::inplace_stretch(ret, factor);
        //for (size_type i=0; i<d; ++i)
        //  ret[i] = coords[i] * factor;
        //return ret;
      }

      /** @brief Convenience overload for stretching the vector given by the point. The vector is stretched in the Euclidian space - thus coordinates are transformed appropriately if required */
      point_t operator/(CoordType factor) const
      {
        point_t ret(*this);
        return CoordinateSystem::inplace_stretch(ret, 1.0 / factor);
        //point_t ret;
        //for (size_type i=0; i<d; ++i)
        //  ret[i] = coords[i] / factor;
        //return ret;
      }

    private:
      CoordType coords[dim];
  };

  /** @brief Overload for stretching a vector from the left. */
  template <typename CoordType, typename CoordinateSystem>
  point_t<CoordType, CoordinateSystem>
  operator*(double val, point_t<CoordType, CoordinateSystem> const & p)
  {
    return p * val;
  }

/*  template <typename CoordType, long d, typename CoordinateSystem>
  point_t<CoordType, d, CoordinateSystem>
  operator*(CoordType val, point_t<CoordType, d, CoordinateSystem> const & p)
  {
    return p * val;
  }*/

  /** @brief Overload of the output stream operator. Allows simple printing to screen or streaming to file */
  template <typename CoordType, typename CoordinateSystem>
  std::ostream& operator << (std::ostream & os, point_t<CoordType, CoordinateSystem> const & p)
  {
    typedef typename point_t<CoordType, CoordinateSystem>::size_type      size_type;
    for (size_type i=0; i<static_cast<size_type>(CoordinateSystem::dim); ++i)
      os << p[i] << " ";
    return os;
  }
  

/** @brief This class provides a less-than comparison for points. Entries are checked for inequality, starting from the first and continuing only in the case of equality. */
  struct point_less
  {
    template <typename PointType>
    bool operator()(PointType const & p1, PointType const & p2) const
    {
      for (std::size_t i=0; i<p1.size(); ++i)
      {
        if (p1[i] < p2[i])
          return true;
        else if (p1[i] > p2[i])
          return false;
      }
      return false;
    }
  };

}
#endif
