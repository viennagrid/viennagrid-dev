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

//   template<bool is_const>
  class point_t : public std::vector<viennagrid_numeric>
  {
  public:

//     typedef unsigned char size_type;
//     typedef viennagrid_numeric value_type;
//     typedef typename result_of::const_nonconst<value_type *, is_const>::type pointer_type;
//     typedef typename result_of::const_type<value_type *>::type const_pointer_type;
//     typedef typename result_of::const_nonconst<value_type &, is_const>::type reference_type;
//     typedef typename result_of::const_type<value_type &>::type const_reference_type;

    point_t() {}
    point_t(size_type size_in) : std::vector<viennagrid_numeric>(size_in)
    {
      std::fill(begin(), end(), value_type(0));
    }

//     base_point(size_type size_in, pointer_type data_in) : data_(data_in), size_(size_in), data_owned_(false) {}


//     reference_type operator[](size_type index) { return data_[index]; }
//     const_reference_type operator[](size_type index) const { return data_[index]; }

//     pointer_type data() { return data_; }
//     const_pointer_type data() const { return data_; }


//     typedef pointer_type iterator;
//     typedef const_pointer_type const_iterator;
//
//     iterator begin() { return data_; }
//     iterator end() { return data_+size(); }
//
//     const_iterator cbegin() const { return data_; }
//     const_iterator cend() const { return data_+size(); }
//
//     const_iterator begin() const { return cbegin(); }
//     const_iterator end() const { return cend(); }


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

//     void make();
// //     {
// //       data_ = new value_type[size()];
// //       data_owned_ = true;
// //       std::fill(begin(), end(), value_type(0));
// //     }
//
//     void free()
//     {
//       if (data_owned_)
//         delete[] data_;
//     }

//     pointer_type data_;
//     size_type size_;
//     bool data_owned_;
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


//   template<>
//   void point_t::make()
//   {
//     data_ = new value_type[size()];
//     data_owned_ = true;
//     std::fill(begin(), end(), value_type(0));
//   }
//
//   template<>
//   void base_point<true>::make()
//   {
//     data_ = new value_type[size()];
//     data_owned_ = true;
//   }


//   template<bool lhs_is_const, bool rhs_is_const>
  inline point_t operator+(point_t const & lhs, point_t const & rhs)
  {
    point_t result(lhs);
    result += rhs;
    return result;
  }

//   template<bool lhs_is_const, bool rhs_is_const>
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



//   template<bool is_const>
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
