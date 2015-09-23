#ifndef VIENNAGRID_COMMON_HPP
#define VIENNAGRID_COMMON_HPP

#include "viennagrid/viennagrid.h"
#include <vector>
#include <algorithm>
#include <cstring>



namespace viennautils
{
  namespace detail
  {
    template<typename T>
    struct dynamic_sizeof_impl;
  }
}



struct reference_counted
{
  reference_counted() : retain_release_count(0), reference_counter(1) {}

  viennagrid_int retain_release_count;
  viennagrid_int reference_counter;
};


template<typename T>
void retain(T * ptr)
{
  if (ptr)
    ++(ptr->reference_counter);
}


template<typename T>
bool release(T * ptr)
{
  if (ptr)
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









template<typename T, typename SizeT>
bool unsorted_arrays_equal(T const * lhs, SizeT lhs_size,
                           T const * rhs, SizeT rhs_size)
{
  if (lhs_size != rhs_size)
    return false;

  std::vector<T> lhs_sorted(lhs_size);
  std::vector<T> rhs_sorted(rhs_size);

  std::copy( lhs, lhs+lhs_size, lhs_sorted.begin() );
  std::copy( rhs, rhs+rhs_size, rhs_sorted.begin() );

  std::sort( lhs_sorted.begin(), lhs_sorted.end() );
  std::sort( rhs_sorted.begin(), rhs_sorted.end() );

  return lhs_sorted == rhs_sorted;
}







template<typename T, typename SizeT>
class short_vector
{
public:

  typedef SizeT size_type;

  typedef T * pointer;
  typedef T const * const_pointer;

  typedef pointer iterator;
  typedef const_pointer const_iterator;


  iterator begin() { return values_; }
  iterator end() { return values_+size(); }

  const_iterator cbegin() const { return values_; }
  const_iterator cend() const { return values_+size(); }

  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }



  short_vector() : values_(0), size_(0) {}
  short_vector(SizeT size_in) : values_(size_in ? new T[size_in] : 0), size_(size_in) {}

  short_vector(short_vector const & sv) : values_(0), size_(sv.size())
  {
    if (!sv.empty())
    {
      values_ = new T[size()];
      memcpy( values_, &sv[0], sizeof(T)*size() );
    }
  }

  short_vector & operator=(short_vector const & sv)
  {
    if (values_)
      delete[] values_;

    size_ = sv.size();
    if (!sv.empty())
    {
      values_ = new T[size()];
      memcpy( values_, &sv[0], sizeof(T)*size() );
    }

    return *this;
  }

  ~short_vector()
  {
    if (values_)
      delete[] values_;
  }

  bool empty() const { return size() == 0; }

  SizeT size() const { return size_; }
  void resize(SizeT new_size)
  {
    if (new_size == size_)
      return;

    T * new_values = new T[new_size];
    memcpy( new_values, values_, sizeof(T)*(std::min(size_, new_size)) );
    delete[] values_;

    values_ = new_values;
    size_ = new_size;
  }

  void push_back(T const & t)
  {
    resize( size()+1 );
    (*this)[size()-1] =  t;
  }

  T const & operator[](SizeT index) const { return values_[index]; }
  T & operator[](SizeT index) { return values_[index]; }

  T const & front() const { return values_[0]; }
  T & front() { return values_[0]; }


private:
  T * values_;
  SizeT size_;
};


template<typename T, typename SizeT>
bool operator<( short_vector<T,SizeT> const & lhs, short_vector<T,SizeT> const & rhs )
{
  return std::lexicographical_compare( lhs.begin(), lhs.end(), rhs.begin(), rhs.end() );
}


template<typename T, typename SizeT>
bool operator==( short_vector<T,SizeT> const & lhs, short_vector<T,SizeT> const & rhs )
{
  if (lhs.size() != rhs.size())
    return false;

  return std::equal( lhs.begin(), lhs.end(), rhs.begin() );

  return true;
}






inline viennagrid_dimension TOPODIM(viennagrid_element_id element_id) { return viennagrid_topological_dimension_from_element_id(element_id); }
inline viennagrid_element_id INDEX(viennagrid_element_id element_id) { return viennagrid_index_from_element_id(element_id); }
inline viennagrid_element_id ID(viennagrid_dimension td, viennagrid_element_id ei) { return viennagrid_compose_element_id(td, ei); }

#define RETURN_ON_ERROR( FUNCTION_CALL ) { viennagrid_error err = FUNCTION_CALL; if (err != VIENNAGRID_SUCCESS) return err; }




#endif
