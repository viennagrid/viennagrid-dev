#ifndef VIENNAGRID_POINT_HPP
#define VIENNAGRID_POINT_HPP

#include <iostream>
#include <cstring>
#include <cassert>
#include <algorithm>

#include "viennagrid/forwards.hpp"

namespace viennagrid
{

  template<bool is_const>
  class base_point
  {
  public:

    typedef unsigned char size_type;
    typedef viennagrid_numeric value_type;
    typedef typename result_of::const_nonconst<value_type *, is_const>::type pointer_type;
    typedef typename result_of::const_type<value_type *>::type const_pointer_type;
    typedef typename result_of::const_nonconst<value_type &, is_const>::type reference_type;
    typedef typename result_of::const_type<value_type &>::type const_reference_type;

    base_point(size_type size_in) : size_(size_in) { make(); }
    base_point(size_type size_in, pointer_type data_in) : data_(data_in), size_(size_in), data_owned_(false) {}

    ~base_point()
    {
      free();
    }

    template<bool other_is_const>
    base_point( base_point<other_is_const> const & op ) : data_(op.data()), size_(op.size())
    {
      make();
      *this = op;
    }

    base_point( base_point<is_const> const & op ) : data_(op.data_), size_(op.size_) {}

    base_point<is_const> operator=( base_point<is_const> const & op )
    {
      assert( size() == op.size() );
      memcpy(data_, op.data(), sizeof(value_type)*size());
      return *this;
    }

    size_type size() const { return size_; }

    reference_type operator[](size_type index) { return data_[index]; }
    const_reference_type operator[](size_type index) const { return data_[index]; }

    pointer_type data() { return data_; }
    const_pointer_type data() const { return data_; }


    typedef pointer_type iterator;
    typedef const_pointer_type const_iterator;

    iterator begin() { return data_; }
    iterator end() { return data_+size(); }

    const_iterator cbegin() const { return data_; }
    const_iterator cend() const { return data_+size(); }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }


    template<bool other_is_const>
    base_point<is_const> & operator+=(base_point<other_is_const> const & other)
    {
      assert( size() == other.size() );
      iterator it = begin();
      typename base_point<other_is_const>::const_iterator jt = other.begin();
      for (; it != end(); ++it, ++jt)
        *it += *jt;
      return *this;
    }

    template<bool other_is_const>
    base_point<is_const> & operator-=(base_point<other_is_const> const & other)
    {
      assert( size() == other.size() );
      iterator it = begin();
      typename base_point<other_is_const>::const_iterator jt = other.begin();
      for (; it != end(); ++it, ++jt)
        *it -= *jt;
      return *this;
    }



    template<bool other_is_const>
    base_point<is_const> & operator*=(value_type scalar)
    {
      iterator it = begin();
      for (; it != end(); ++it)
        *it *= scalar;
      return *this;
    }

    template<bool other_is_const>
    base_point<is_const> & operator/=(value_type scalar)
    {
      return (*this *= (value_type(1)/scalar));
    }


  private:

    void make()
    {
      data_ = new value_type[size()];
      data_owned_ = true;
      std::fill(begin(), end(), value_type(0));
    }

    void free()
    {
      if (data_owned_)
        delete[] data_;
    }

    pointer_type data_;
    size_type size_;
    bool data_owned_;
  };


  template<bool lhs_is_const, bool rhs_is_const>
  base_point<false> operator+(base_point<lhs_is_const> const & lhs, base_point<rhs_is_const> const & rhs)
  {
    base_point<false> result(lhs);
    result += rhs;
    return result;
  }

  template<bool lhs_is_const, bool rhs_is_const>
  base_point<false> operator-(base_point<lhs_is_const> const & lhs, base_point<rhs_is_const> const & rhs)
  {
    base_point<false> result(lhs);
    result -= rhs;
    return result;
  }

  template<bool is_const>
  base_point<false> operator*(base_point<is_const> const & vector, typename base_point<is_const>::value_type scalar)
  {
    base_point<false> result(vector);
    result *= scalar;
    return result;
  }

  template<bool is_const>
  base_point<false> operator*(typename base_point<is_const>::value_type scalar, base_point<is_const> const & vector)
  {
    base_point<false> result(vector);
    result *= scalar;
    return result;
  }

  template<bool is_const>
  base_point<false> operator/(base_point<is_const> const & vector, typename base_point<is_const>::value_type scalar)
  {
    base_point<false> result(vector);
    result /= scalar;
    return result;
  }

  template<bool is_const>
  base_point<false> operator/(typename base_point<is_const>::value_type scalar, base_point<is_const> const & vector)
  {
    base_point<false> result(vector);
    result /= scalar;
    return result;
  }



  template<bool is_const>
  std::ostream& operator << (std::ostream & os, base_point<is_const> const & p)
  {
    typedef typename base_point<is_const>::size_type      size_type;
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
    template<bool is_const>
    struct point< base_point<is_const> >
    {
      typedef base_point<is_const> type;
    };

    template<bool is_const>
    struct coord< base_point<is_const> >
    {
      typedef typename base_point<is_const>::value_type type;
    };
  }

}

#endif
