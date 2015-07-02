#ifndef VIENNAGRID_COMMON_HPP
#define VIENNAGRID_COMMON_HPP

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

#endif
