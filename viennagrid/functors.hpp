#ifndef VIENNAGRID_FUNCTORS_HPP
#define VIENNAGRID_FUNCTORS_HPP

/** @file viennagrid/functors.hpp
    @brief A collection of general functors
*/

namespace viennagrid
{
  /** @brief A trivial functor which always returns true */
  struct true_functor
  {
    typedef bool result_type;

    template<typename ValueT>
    bool operator()(ValueT const &) const
    { return true; }
  };
}


#endif
