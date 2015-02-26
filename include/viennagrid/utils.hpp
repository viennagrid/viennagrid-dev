#ifndef VIENNAGRID_UTILS_HPP
#define VIENNAGRID_UTILS_HPP

namespace viennagrid
{

  /** @brief Advances an iterator and returns it
    *
    * @param in          the input iterator
    * @param distance    the distance to advance
    * @return            the iterator, advanced by distance
    */
  template<typename IteratorT>
  IteratorT advance(IteratorT const & in, typename std::iterator_traits<IteratorT>::difference_type distance)
  {
    IteratorT out(in);
    std::advance(out, distance);
    return out;
  }


  template<typename T1, typename T2, typename T3>
  struct triple
  {
    typedef T1 first_type;
    typedef T2 second_type;
    typedef T3 third_type;

    triple() {}
    triple(first_type first_, second_type second_, third_type third_) : first(first_), second(second_), third(third_) {}

    bool operator<( triple<T1,T2,T3> const & rhs ) const
    {
      if (first < rhs.first)
        return true;
      if (rhs.first < first)
        return false;

      if (second < rhs.first)
        return true;
      if (rhs.second < second)
        return false;

      return third < rhs.third;
    }

    bool operator<=( triple<T1,T2,T3> const & rhs ) const
    { return !(rhs < *this); }

    bool operator>( triple<T1,T2,T3> const & rhs ) const
    { return rhs < *this; }

    bool operator>=( triple<T1,T2,T3> const & rhs ) const
    { return !(*this < rhs); }


    first_type   first;
    second_type  second;
    third_type   third;
  };

  template<typename T1, typename T2, typename T3>
  triple<T1,T2,T3> make_tripple(T1 first_, T2 second_, T3 third_)
  {
    return triple<T1,T2,T3>(first_, second_, third_);
  }







  template<typename IteratorT, typename ConstIteratorT>
  class base_range
  {
  public:
    typedef IteratorT iterator;
    typedef ConstIteratorT const_iterator;

    base_range(iterator begin_in, iterator end_in) : begin_(begin_in), end_(end_in) {}

    iterator begin() { return begin_; }
    iterator end() { return end_; }

    const_iterator cbegin() const { return begin_; }
    const_iterator cend() const { return end_; }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }

  protected:
    IteratorT begin_;
    IteratorT end_;
  };


}

#endif
