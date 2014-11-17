#ifndef VIENNAGRID_ITERATORS_H
#define VIENNAGRID_ITERATORS_H

namespace viennagrid
{
  template<typename IteratorT, typename FunctorT>
  class transform_iterator
  {
    template<typename OtherIteratorT, typename OtherFunctorT>
    friend class transform_iterator;

  public:
    typedef transform_iterator<IteratorT, FunctorT> self_type;

    typedef typename std::iterator_traits<IteratorT>::difference_type difference_type;
    typedef typename FunctorT::value_type value_type;
    typedef typename FunctorT::pointer pointer;
    typedef typename FunctorT::reference reference;
    typedef typename std::iterator_traits<IteratorT>::iterator_category iterator_category;



    transform_iterator(IteratorT iterator_, FunctorT functor_) : iterator(iterator_), functor(functor_) {}

    template<typename OtherIteratorT, typename OtherFunctorT>
    transform_iterator(transform_iterator<OtherIteratorT, OtherFunctorT> const & ti) : iterator(ti.iterator), functor(ti.functor) {}

    bool operator==(self_type const & rhs) const { return iterator == rhs.iterator; }
    bool operator!=(self_type const & rhs) const { return iterator != rhs.iterator; }

    reference operator*() { return functor(*iterator); }
    const reference operator*() const { return functor(*iterator); }

    self_type & operator++()
    {
      ++iterator;
      return *this;
    }

    self_type operator++(int)
    {
      self_type result = *this;
      ++result;
      return result;
    }

    self_type & operator+=(difference_type diff)
    {
      iterator += diff;
      return *this;
    }


    self_type & operator--()
    {
      --iterator;
      return *this;
    }

    self_type operator--(int)
    {
      self_type result = *this;
      --result;
      return result;
    }

    self_type & operator-=(difference_type diff)
    {
      iterator -= diff;
      return *this;
    }



    difference_type operator-(self_type const & rhs) const
    {
      return iterator - rhs.iterator;
    }


  private:
    IteratorT iterator;
    FunctorT functor;
  };




  template<typename IteratorT, typename FunctorT>
  class view_iterator
  {
    template<typename OtherIteratorT, typename OtherFunctorT>
    friend class view_iterator;

  public:
    typedef view_iterator<IteratorT, FunctorT> self_type;

    typedef typename IteratorT::difference_type difference_type;
    typedef typename IteratorT::value_type value_type;
    typedef typename IteratorT::pointer pointer;
    typedef typename IteratorT::reference reference;
    typedef typename IteratorT::iterator_category iterator_category;

    view_iterator(IteratorT iterator_, FunctorT functor_) : iterator(iterator_), end_iterator(iterator_), functor(functor_)
    {
      skip_invalid_forward();
    }

    view_iterator(IteratorT iterator_, IteratorT end_iterator_, FunctorT functor_) : iterator(iterator_), end_iterator(end_iterator_), functor(functor_)
    {
      skip_invalid_forward();
    }

    template<typename OtherIteratorT, typename OtherFunctorT>
    view_iterator(view_iterator<OtherIteratorT, OtherFunctorT> const & vi) : iterator(vi.iterator), end_iterator(vi.end_iterator), functor(vi.functor) {}

    bool operator==(self_type const & rhs) const { return iterator == rhs.iterator; }
    bool operator!=(self_type const & rhs) const { return iterator != rhs.iterator; }

    reference operator*() { return *iterator; }
    const reference operator*() const { return *iterator; }

    pointer operator->() { return iterator.operator->(); }
    const pointer operator->() const { return iterator.operator->(); }



    self_type & operator++()
    {
      ++iterator;
      skip_invalid_forward();
      return *this;
    }

    self_type operator++(int)
    {
      self_type result = *this;
      ++(*this);
      return result;
    }

    self_type & operator+=(difference_type diff)
    {
      for (difference_type i = 0; i != diff; ++i)
      {
        ++(*this);
        if (iterator == end_iterator)
          break;
      }

      return *this;
    }



    self_type & operator--()
    {
      --iterator;
      skip_invalid_backward();
      return *this;
    }

    self_type operator--(int)
    {
      self_type result = *this;
      --(*this);
      return result;
    }

    self_type & operator-=(difference_type diff)
    {
      for (difference_type i = 0; i != diff; ++i)
      {
        --(*this);
        if (iterator == end_iterator)
          break;
      }

      return *this;
    }



    difference_type operator-(self_type const & rhs) const
    {
      self_type tmp = rhs;
      difference_type count = 0;
      for (; tmp != *this; ++tmp)
        ++count;
      return count;
    }


  private:

    void skip_invalid_forward()
    {
      while (iterator != end_iterator && !functor(*iterator))
        ++iterator;
    }

    void skip_invalid_backward()
    {
      while (iterator != end_iterator && !functor(*iterator))
        --iterator;
    }

    IteratorT iterator;
    IteratorT end_iterator;
    FunctorT functor;
  };

}



#endif
