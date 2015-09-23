#ifndef VIENNAGRID_POINT_HPP
#define VIENNAGRID_POINT_HPP

#include <iostream>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <vector>
#include <cmath>

#include "viennagrid/core/forwards.hpp"

namespace viennagrid
{

  class point : public std::vector<viennagrid_numeric>
  {
  public:

    point() {}
    point(std::vector<viennagrid_numeric> const & pt) : std::vector<viennagrid_numeric>(pt) {}
    point(size_type size_in) : std::vector<viennagrid_numeric>(size_in, 0) {}
    point(size_type size_in, viennagrid_numeric init_value) : std::vector<viennagrid_numeric>(size_in, init_value) {}
    point(size_type size_in, viennagrid_numeric * coords) : std::vector<viennagrid_numeric>(size_in)
    {
      assert(coords);
      std::copy(coords, coords+size(), &(*this)[0]);
    }

    template<typename PointT>
    point & operator+=(PointT const & other)
    {
      assert( size() == other.size() );
      iterator it = begin();
      typename PointT::const_iterator jt = other.begin();
      for (; it != end(); ++it, ++jt)
        *it += *jt;
      return *this;
    }

    template<typename PointT>
    point & operator-=(PointT const & other)
    {
      assert( size() == other.size() );
      iterator it = begin();
      typename PointT::const_iterator jt = other.begin();
      for (; it != end(); ++it, ++jt)
        *it -= *jt;
      return *this;
    }




    point operator-() const
    {
      point tmp(*this);
      for (iterator it = tmp.begin(); it != tmp.end(); ++it)
        *it = -*it;
      return tmp;
    }


    point & operator*=(value_type scalar)
    {
      iterator it = begin();
      for (; it != end(); ++it)
        *it *= scalar;
      return *this;
    }

    point & operator/=(value_type scalar)
    {
      return (*this *= (value_type(1)/scalar));
    }

    point & normalize()
    {
      viennagrid_numeric tmp = 0.0;
      for (iterator it = begin(); it != end(); ++it)
        tmp += *it * *it;
      tmp = std::sqrt(tmp);
      return operator/=(tmp);
    }


  private:
  };

  inline point make_point( point::value_type x )
  {
    point p(1);
    p[0] = x;
    return p;
  }

  inline point make_point( point::value_type x, point::value_type y )
  {
    point p(2);
    p[0] = x;
    p[1] = y;
    return p;
  }

  inline point make_point( point::value_type x, point::value_type y, point::value_type z )
  {
    point p(3);
    p[0] = x;
    p[1] = y;
    p[2] = z;
    return p;
  }




  inline point operator+(point const & lhs, point const & rhs)
  {
    point result(lhs);
    result += rhs;
    return result;
  }

  inline point operator-(point const & lhs, point const & rhs)
  {
    point result(lhs);
    result -= rhs;
    return result;
  }

  inline point operator*(point const & vector, point::value_type scalar)
  {
    point result(vector);
    result *= scalar;
    return result;
  }

  inline point operator*(point::value_type scalar, point const & vector)
  {
    point result(vector);
    result *= scalar;
    return result;
  }

  inline point operator/(point const & vector, point::value_type scalar)
  {
    point result(vector);
    result /= scalar;
    return result;
  }

  inline point operator/(point::value_type scalar, point const & vector)
  {
    point result(vector);
    result /= scalar;
    return result;
  }




  inline bool operator<(point const & p1, point const & p2)
  {
    for (std::size_t i = 0; i < p1.size(); ++i)
      if (p1[i] >= p2[i])
        return false;

    return true;
  }

  inline bool operator<=(point const & p1, point const & p2)
  {
    for (std::size_t i = 0; i < p1.size(); ++i)
      if (p1[i] > p2[i])
        return false;

    return true;
  }


  /** @brief Returns the pointwise minimum point of two points, i.e. result[i] = min(x[i], y[i]) for all i. */
  inline point min(point const & p1, point const & p2)
  {
    assert( p1.size() == p2.size() );
    point tmp(p1.size());
    for (std::size_t i = 0; i < p1.size(); ++i)
      tmp[i] = std::min(p1[i], p2[i]);
    return tmp;
  }

  /** @brief Returns the pointwise maximum point of two points, i.e. result[i] = max(x[i], y[i]) for all i. */
  inline point max(point const & p1, point const & p2)
  {
    assert( p1.size() == p2.size() );
    point tmp(p1.size());
    for (std::size_t i = 0; i < p1.size(); ++i)
      tmp[i] = std::max(p1[i], p2[i]);
    return tmp;
  }


  /** @brief Returns the cross product of two vectors. If the vectors have one or two components only, the zero vector is returned.
   *
   * @param v1    The first vector given in some coordinate system
   * @param v2    The second vector given in another coordinate system
   */
  inline point cross_prod(point const& v0, point const& v1)
  {
    assert(v0.size() == v1.size());
    assert(v0.size() == 3);

    point result(v0.size());
    THROW_ON_ERROR( viennagrid_cross_prod(&v0[0], &v1[0], &result[0]) );
    return result;
  }


  /** @brief Returns the cross product of two vectors. If the vectors have one or two components only, the zero vector is returned.
   *
   * @param v1    The first vector given in some coordinate system
   * @param v2    The second vector given in another coordinate system
   */
  inline viennagrid_numeric inner_prod(point const& v0, point const& v1)
  {
    assert(v0.size() == v1.size());

    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_inner_prod(v0.size(), &v0[0], &v1[0], &result) );
    return result;
  }


  /** @brief Convenience shortcut for the 1-norm of a vector. */
  inline viennagrid_numeric norm_1(point const & p)
  {
    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_norm_1(p.size(), &p[0], &result) );
    return result;
  }

  /** @brief Convenience shortcut for the 2-norm of a vector. */
  inline viennagrid_numeric norm_2(point const & p)
  {
    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_norm_2(p.size(), &p[0], &result) );
    return result;
  }
  inline viennagrid_numeric norm(point const & p)
  { return norm_2(p); }

  /** @brief Convenience shortcut for the 2-norm of a vector. */
  inline viennagrid_numeric norm_inf(point const & p)
  {
    viennagrid_numeric result;
    THROW_ON_ERROR( viennagrid_norm_inf(p.size(), &p[0], &result) );
    return result;
  }

  std::ostream& operator<< (std::ostream & os, point const & p);
  std::istream& operator>> (std::istream & is, point & pt);



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


  namespace result_of
  {
    template<>
    struct point< viennagrid::point >
    {
      typedef viennagrid::point type;
    };

    template<>
    struct coord< viennagrid::point >
    {
      typedef viennagrid::point::value_type type;
    };
  }

}

#endif
