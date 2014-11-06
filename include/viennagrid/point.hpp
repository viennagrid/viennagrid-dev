#ifndef VIENNAGRID_POINT_HPP
#define VIENNAGRID_POINT_HPP

#include <iostream>
#include "viennagrid/forwards.hpp"

namespace viennagrid
{

  template<bool is_const>
  class base_point_proxy
  {
  public:

    typedef std::size_t size_type;
    typedef viennagrid_numeric value_type;
    typedef typename result_of::const_nonconst<value_type *, is_const>::type pointer_type;
    typedef typename result_of::const_type<value_type *>::type const_pointer_type;
    typedef typename result_of::const_nonconst<value_type &, is_const>::type reference_type;
    typedef typename result_of::const_type<value_type &>::type const_reference_type;

    base_point_proxy(size_type size_in, pointer_type data_in) : size_(size_in), data_(data_in) {}

    size_type size() const { return size_; }

    reference_type operator[](size_type index) { return data_[index]; }
    const_reference_type operator[](size_type index) const { return data_[index]; }

    pointer_type data() { return data_; }
    const_pointer_type data() const { return data_; }

  private:

    const size_type size_;
    pointer_type data_;
  };


  template<bool is_const>
  std::ostream& operator << (std::ostream & os, base_point_proxy<is_const> const & p)
  {
    typedef typename base_point_proxy<is_const>::size_type      size_type;
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
    struct point< base_point_proxy<is_const> >
    {
      typedef base_point_proxy<is_const> type;
    };

    template<bool is_const>
    struct coord< base_point_proxy<is_const> >
    {
      typedef typename base_point_proxy<is_const>::value_type type;
    };
  }

}

#endif
