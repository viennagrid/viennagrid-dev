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
//     typedef typename std::iterator_traits<IteratorT>::difference_type;
    IteratorT out(in);
    std::advance(out, distance);
    return out;
  }





  /** @brief A trivial functor which always returns true */
  struct true_functor
  {
    typedef bool result_type;

    template<typename ValueT>
    bool operator()(ValueT) const
    { return true; }
  };






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
