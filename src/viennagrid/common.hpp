#ifndef VIENNAGRID_COMMON_HPP
#define VIENNAGRID_COMMON_HPP

#include <vector>


struct reference_counted
{
  reference_counted() : retain_release_count(0), reference_counter(1) {}

  viennagrid_int retain_release_count;
  viennagrid_int reference_counter;
};


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
