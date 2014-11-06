#ifndef VIENNAGRID_ALGORITHM_INTERFACE_HPP
#define VIENNAGRID_ALGORITHM_INTERFACE_HPP

#include "viennagrid/core.hpp"

namespace viennagrid
{
  template<typename SomethingFirstT, typename SomethingSecondT, typename ElementT>
  bool is_interface( SomethingFirstT first, SomethingSecondT second, ElementT element )
  {
    return is_boundary(first, element) && is_boundary(second, element);
  }
}


#endif
