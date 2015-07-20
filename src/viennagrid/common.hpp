#ifndef VIENNAGRID_COMMON_HPP
#define VIENNAGRID_COMMON_HPP

#include <vector>

template<typename T>
void retain(T * ptr)
{
  ++(ptr->reference_counter);
}


template<typename T>
bool release(T * ptr)
{
  if (--(ptr->reference_counter) <= 0)
  {
    delete ptr;
    return false;
  }

  return true;
}

template<typename T, typename Alloc>
void shrink_to_fit( std::vector<T, Alloc> & v )
{
  std::vector<T, Alloc>(v).swap(v);
}

#endif
