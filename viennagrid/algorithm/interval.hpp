#ifndef VIENNAGRID_ALGORITHM_INTERVAL_HPP
#define VIENNAGRID_ALGORITHM_INTERVAL_HPP


#include "viennagrid/algorithm/detail/numeric.hpp"


namespace viennagrid
{
  struct open_tag {};
  struct closed_tag {};


  namespace detail
  {

    template<typename tag0, typename tag1>
    open_tag intersect_tag( tag0, tag1 )
    { return open_tag(); }

    inline closed_tag intersect_tag ( closed_tag, closed_tag )
    { return closed_tag(); }

    inline bool is_open(open_tag) { return true; }
    inline bool is_open(closed_tag) { return false; }

    inline bool is_closed(open_tag) { return false; }
    inline bool is_closed(closed_tag) { return true; }

    struct open_open_tag
    {
        typedef open_tag first_tag;
        typedef open_tag second_tag;
    };

    struct open_closed_tag
    {
        typedef open_tag first_tag;
        typedef closed_tag second_tag;
    };

    struct closed_open_tag
    {
        typedef closed_tag first_tag;
        typedef open_tag second_tag;
    };

    struct closed_closed_tag
    {
        typedef closed_tag first_tag;
        typedef closed_tag second_tag;
    };

    template<typename dual_tag>
    typename dual_tag::first_tag first_tag( dual_tag )
    { return typename dual_tag::first_tag(); }

    template<typename dual_tag>
    typename dual_tag::second_tag second_tag( dual_tag )
    { return typename dual_tag::second_tag(); }

    inline open_open_tag make_tag( open_tag, open_tag )
    { return open_open_tag(); }

    inline open_closed_tag make_tag( open_tag, closed_tag )
    { return open_closed_tag(); }

    inline closed_open_tag make_tag( closed_tag, open_tag )
    { return closed_open_tag(); }

    inline closed_closed_tag make_tag( closed_tag, closed_tag )
    { return closed_closed_tag(); }


    template<typename numeric_type>
    bool greater( numeric_type ref, numeric_type to_test, open_tag )
    { return ref < to_test; }

    template<typename numeric_type>
    bool greater( numeric_type ref, numeric_type to_test, closed_tag )
    { return ref <= to_test; }

    template<typename numeric_type>
    bool less( numeric_type ref, numeric_type to_test, open_tag )
    { return ref > to_test; }

    template<typename numeric_type>
    bool less( numeric_type ref, numeric_type to_test, closed_tag )
    { return ref >= to_test; }

  }


  template<typename numeric_type, typename first_tag_type, typename second_tag_type, typename numeric_config>
  bool point_in_interval( numeric_type first, first_tag_type first_tag,
                          numeric_type second, second_tag_type second_tag,
                          numeric_type to_test,
                          numeric_config nc )
  {
    if ( detail::is_equal(nc, first, to_test) )
        return is_closed(first_tag);

    if ( detail::is_equal(nc, second, to_test) )
        return is_closed(second_tag);

    return (first <= second) ?
        (greater(first, to_test, first_tag) && less(second, to_test, second_tag)) :
        (greater(second, to_test, second_tag) && less(first, to_test, first_tag));
  }

  template<typename numeric_type, typename dual_tag, typename numeric_config>
  bool point_in_interval( numeric_type first, numeric_type second, dual_tag tag, numeric_type to_test, numeric_config nc )
  {
      return point_in_interval(first, first_tag(tag), second, second_tag(tag), to_test, nc);
  }

  template<typename numeric_type, typename interval_0_tag_type, typename interval_1_tag_type, typename numeric_config>
  bool interval_intersect(numeric_type first_0, numeric_type second_0, interval_0_tag_type interval_0_tag,
                          numeric_type first_1, numeric_type second_1, interval_1_tag_type interval_1_tag,
                          numeric_config nc)
  {
      return point_in_interval( first_0, intersect_tag(first_tag(interval_0_tag), first_tag(interval_1_tag)), second_0, intersect_tag(second_tag(interval_0_tag), first_tag(interval_1_tag)), first_1, nc ) ||
          point_in_interval( first_0, intersect_tag(first_tag(interval_0_tag), second_tag(interval_1_tag)), second_0, intersect_tag(second_tag(interval_0_tag), second_tag(interval_1_tag)), second_1, nc ) ||
          point_in_interval( first_1, intersect_tag(first_tag(interval_1_tag), first_tag(interval_0_tag)), second_1, intersect_tag(second_tag(interval_1_tag), first_tag(interval_0_tag)), first_0, nc ) ||
          point_in_interval( first_1, intersect_tag(first_tag(interval_1_tag), second_tag(interval_0_tag)), second_1, intersect_tag(second_tag(interval_1_tag), second_tag(interval_0_tag)), second_0, nc ) ||
          ( (first_0 == first_1) && (second_0 == second_1) && (std::abs(second_0 - first_0) > 0) ) ||
          ( (first_0 == second_1) && (second_0 == first_1) && (std::abs(second_0 - first_0) > 0) );
  }





}



#endif
