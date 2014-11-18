#ifndef VIENNAGRID_POINT_HPP
#define VIENNAGRID_POINT_HPP

#include <iostream>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <vector>

#include "viennagrid/forwards.hpp"

namespace viennagrid
{

  class point_t : public std::vector<viennagrid_numeric>
  {
  public:

    point_t() {}
    point_t(size_type size_in) : std::vector<viennagrid_numeric>(size_in)
    { std::fill(begin(), end(), value_type(0)); }

    template<typename PointT>
    point_t & operator+=(PointT const & other)
    {
      assert( size() == other.size() );
      iterator it = begin();
      typename PointT::const_iterator jt = other.begin();
      for (; it != end(); ++it, ++jt)
        *it += *jt;
      return *this;
    }

    template<typename PointT>
    point_t & operator-=(PointT const & other)
    {
      assert( size() == other.size() );
      iterator it = begin();
      typename PointT::const_iterator jt = other.begin();
      for (; it != end(); ++it, ++jt)
        *it -= *jt;
      return *this;
    }



    point_t & operator*=(value_type scalar)
    {
      iterator it = begin();
      for (; it != end(); ++it)
        *it *= scalar;
      return *this;
    }

    point_t & operator/=(value_type scalar)
    {
      return (*this *= (value_type(1)/scalar));
    }


  private:
  };



  inline point_t make_point( point_t::value_type x )
  {
    point_t p(1);
    p[0] = x;
    return p;
  }

  inline point_t make_point( point_t::value_type x, point_t::value_type y )
  {
    point_t p(2);
    p[0] = x;
    p[1] = y;
    return p;
  }

  inline point_t make_point( point_t::value_type x, point_t::value_type y, point_t::value_type z )
  {
    point_t p(3);
    p[0] = x;
    p[1] = y;
    p[2] = z;
    return p;
  }




  inline point_t operator+(point_t const & lhs, point_t const & rhs)
  {
    point_t result(lhs);
    result += rhs;
    return result;
  }

  inline point_t operator-(point_t const & lhs, point_t const & rhs)
  {
    point_t result(lhs);
    result -= rhs;
    return result;
  }

  inline point_t operator*(point_t const & vector, point_t::value_type scalar)
  {
    point_t result(vector);
    result *= scalar;
    return result;
  }

  inline point_t operator*(point_t::value_type scalar, point_t const & vector)
  {
    point_t result(vector);
    result *= scalar;
    return result;
  }

  inline point_t operator/(point_t const & vector, point_t::value_type scalar)
  {
    point_t result(vector);
    result /= scalar;
    return result;
  }

  inline point_t operator/(point_t::value_type scalar, point_t const & vector)
  {
    point_t result(vector);
    result /= scalar;
    return result;
  }





  /** @brief Returns the pointwise minimum point of two points, i.e. result[i] = min(x[i], y[i]) for all i. */
  inline point_t min(point_t const & p1, point_t const & p2)
  {
    assert( p1.size() == p2.size() );
    point_t tmp(p1.size());
    for (std::size_t i = 0; i < p1.size(); ++i)
      tmp[i] = std::min(p1[i], p2[i]);
    return tmp;
  }

  /** @brief Returns the pointwise maximum point of two points, i.e. result[i] = max(x[i], y[i]) for all i. */
  inline point_t max(point_t const & p1, point_t const & p2)
  {
    assert( p1.size() == p2.size() );
    point_t tmp(p1.size());
    for (std::size_t i = 0; i < p1.size(); ++i)
      tmp[i] = std::max(p1[i], p2[i]);
    return tmp;
  }




  std::ostream& operator << (std::ostream & os, point_t const & p)
  {
    typedef point_t::size_type      size_type;
    os << "(";
    for (size_type i=0; i< p.size(); ++i)
      os << p[i] << (i == p.size()-1 ? "" :" ");
    os << ")";
    return os;
  }

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
    struct point< point_t >
    {
      typedef point_t type;
    };

    template<>
    struct coord< point_t >
    {
      typedef point_t::value_type type;
    };
  }

}

#endif
