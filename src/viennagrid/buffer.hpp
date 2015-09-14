#ifndef VIENNAGRID_BACKEND_BUFFER_HPP
#define VIENNAGRID_BACKEND_BUFFER_HPP


#include <iostream>
#include <vector>
#include <map>
#include <cassert>




template<typename IndexT, typename T, typename PointerT = T *, typename ConstPointerT = T const *>
struct dense_multibuffer
{
public:

  typedef T value_type;
  typedef PointerT pointer;
  typedef ConstPointerT const_pointer;
  typedef IndexT size_type;

  typedef pointer iterator;
  typedef const_pointer const_iterator;

  dense_multibuffer() {}

  iterator begin(size_type index) { return (values.empty() || values[index].empty()) ? 0 : &values[index][0]; }
  iterator end(size_type index) { return (values.empty() || values[index].empty()) ? 0 : &values[index][0] + values[index].size(); }

  const_iterator cbegin(size_type index) const { return (values.empty() || values[index].empty()) ? 0 : &values[index][0]; }
  const_iterator cend(size_type index) const { return (values.empty() || values[index].empty()) ? 0 : &values[index][0] + values[index].size(); }

  const_iterator begin(size_type index) const { return cbegin(index); }
  const_iterator end(size_type index) const { return cend(index); }

  size_type size(size_type index) const { return end(index)-begin(index); }


  pointer push_back(size_type count)
  {
    values.push_back( std::vector<value_type>() );
    values.back().resize(count);
    return &values.back()[0];
  }

  pointer resize(size_type index, size_type count)
  {
    values[index].resize(count);
    return begin(index);
  }

  void add(size_type index, value_type to_add)
  {
    size_type old_size = size(index);
    pointer p = resize(index, old_size+1);
    p[old_size] = to_add;
  }

  void clear()
  {
    values.clear();
  }


private:
  std::vector< std::vector<value_type> > values;
};










template<typename IndexT, typename T, typename PointerT = T *, typename ConstPointerT = T const *>
struct dense_packed_multibuffer
{
public:

  typedef T value_type;
  typedef PointerT pointer;
  typedef ConstPointerT const_pointer;
  typedef IndexT size_type;

  typedef pointer iterator;
  typedef const_pointer const_iterator;

  dense_packed_multibuffer() : offsets(1,0) {}
  ~dense_packed_multibuffer() {}

  iterator begin(size_type index) { return (values.empty() || index >= size()) ? 0 : &values[0] + offsets[index]; }
  iterator end(size_type index) { return (values.empty() || index >= size()) ? 0 : &values[0] + offsets[index+1]; }

  const_iterator cbegin(size_type index) const { return (values.empty() || index >= size()) ? 0 : &values[0] + offsets[index]; }
  const_iterator cend(size_type index) const { return (values.empty() || index >= size()) ? 0 : &values[0] + offsets[index+1]; }

  const_iterator begin(size_type index) const { return cbegin(index); }
  const_iterator end(size_type index) const { return cend(index); }

  size_type size(size_type index) const
  {
    assert( index < static_cast<size_type>(offsets.size()) );
    return end(index)-begin(index);
  }
  size_type size() const
  {
    return offsets.size()-1;
  }


  void push_back( viennagrid_int count, size_type const * new_offsets_, value_type const * new_values_ )
  {
    viennagrid_int old_count = offsets.size()-1;
    offsets.resize( old_count+count+1 );
    std::copy( new_offsets_, new_offsets_+count+1, offsets.begin()+old_count );

    viennagrid_int base = offsets[offsets.size()-count-1];
    for (std::size_t i = offsets.size()-count; i != offsets.size(); ++i)
      offsets[i] += base;

    viennagrid_int old_values_count = values.size();
    values.resize( values.size()+new_offsets_[count] );
    std::copy( new_values_, new_values_+new_offsets_[count], values.begin()+old_values_count );
  }

  pointer push_back(size_type count)
  {
    std::size_t start_index = values.size();
    values.resize( values.size()+count );
    offsets.push_back(values.size());
    return &values[0] + start_index;
  }

  void reserve(size_type offsets_count, size_type values_count)
  {
    values.reserve( offsets_count );
    offsets.reserve( values_count );
  }

  pointer resize(size_type index, size_type count)
  {
    assert( index < static_cast<size_type>(offsets.size()) );
    size_type old_count = size(index);

    if (count > old_count)
    {
      size_type count_increase = count-old_count;
      values.insert( values.begin() + offsets[index+1], count_increase, value_type() );
      for (size_type i = index+1; i != static_cast<size_type>(offsets.size()); ++i)
        offsets[i] += count_increase;
    }
    else if (count < old_count)
    {
      size_type count_decrease = old_count-count;
      values.erase( values.begin() + offsets[index] + count, values.begin() + offsets[index+1] );
      for (size_type i = index+1; i != static_cast<size_type>(offsets.size()); ++i)
        offsets[i] -= count_decrease;
    }

    return begin(index);
  }

  void erase(size_type index, size_type first, size_type last)
  {
    if (last <= first)
      return;

    size_type old_count = size(index);
    size_type to_delete_count = last-first;

    pointer ptr = begin(index);
    for (size_type i = last; i != old_count; ++i)
      *(ptr+i-to_delete_count) = (*ptr+i);

    resize(index, old_count-to_delete_count);
  }

  void add(size_type index, value_type to_add)
  {
    assert( index < static_cast<size_type>(offsets.size()) );
    size_type old_size = size(index);
    pointer p = resize(index, old_size+1);
    p[old_size] = to_add;
  }

  void clear()
  {
    offsets = std::vector<size_type>(1,0);
    values.clear();
  }

  size_type * offset_pointer() { return &offsets[0]; }
  value_type * values_pointer() { return &values[0]; }


  void shrink_to_fit()
  {
    std::vector<value_type>(values).swap(values);
    std::vector<size_type>(offsets).swap(offsets);
  }

  std::vector<value_type> const & get_values() const { return values; }
  std::vector<size_type> const & get_offsets() const { return offsets; }

private:
  std::vector<value_type> values;
  std::vector<size_type> offsets;
};




template<typename IndexT, typename T, typename PointerT = T *, typename ConstPointerT = T const *>
struct sparse_packed_multibuffer
{
public:

  typedef T value_type;
  typedef PointerT pointer;
  typedef ConstPointerT const_pointer;
  typedef IndexT size_type;

  typedef std::vector<value_type> ValueContainer;
  typedef std::map<size_type, typename ValueContainer::size_type> OffsetMap;

  sparse_packed_multibuffer() {}

  typedef pointer iterator;
  typedef const_pointer const_iterator;

  size_type value_index(size_type index) const
  {
    typename OffsetMap::const_iterator it = offsets.find(index);
    if (it == offsets.end())
      return -1;

    return it->second;
  }

  iterator begin(size_type index)
  {
    typename OffsetMap::iterator it = offsets.find(index);
    if (it == offsets.end())
      return 0;

    return &values[0] + it->second;
  }

  iterator end(size_type index)
  {
    typename OffsetMap::iterator it = offsets.find(index);
    if (it == offsets.end())
      return 0;
    ++it;
    if (it == offsets.end())
      return &values[0] + values.size();

    return &values[0] + it->second;
  }

  const_iterator cbegin(size_type index) const
  {
    typename OffsetMap::const_iterator it = offsets.find(index);
    if (it == offsets.end())
      return 0;

    return &values[0] + it->second;
  }

  const_iterator cend(size_type index) const
  {
    typename OffsetMap::const_iterator it = offsets.find(index);
    if (it == offsets.end())
      return 0;
    ++it;
    if (it == offsets.end())
      return &values[0] + values.size();

    return &values[0] + it->second;
  }

  const_iterator begin(size_type index) const { return cbegin(index); }
  const_iterator end(size_type index) const { return cend(index); }


  iterator values_begin() { return &values[0]; }
  iterator values_end() { return &values[0] + values.size(); }

  const_iterator values_cbegin() const { return &values[0]; }
  const_iterator values_cend() const { return &values[0] + values.size(); }

  const_iterator values_begin() const { return cbegin(); }
  const_iterator values_end() const { return cend(); }


  size_type size(size_type index) const { return end(index)-begin(index); }

  pointer insert(size_type index, size_type count)
  {
    std::size_t start_index = values.size();
    offsets[index] = start_index;
    values.resize( values.size()+count );
    return &values[0] + start_index;
  }

  pointer resize(size_type index, size_type count)
  {
    size_type old_count = size(index);
    size_type vi = value_index(index);

    if (count > old_count)
    {
      size_type count_increase = count-old_count;
      values.resize( values.size()+count_increase );
      for (iterator it = values_end()-1; it != begin(index)+old_count; --it)
        *it = *(it-count_increase);

      for (typename OffsetMap::iterator it = offsets.begin(); it != offsets.end(); ++it)
      {
        if (static_cast<size_type>(it->second) > vi)
          it->second += count_increase;
      }
    }
    else if (count < old_count)
    {
      size_type count_decrease = old_count-count;
      for (iterator it = begin(index)+count; it != values_end()-count_decrease-1; ++it)
        *it = *(it+count_decrease);
      values.resize( values.size()-count_decrease );

      for (typename OffsetMap::iterator it = offsets.begin(); it != offsets.end(); ++it)
      {
        if (static_cast<size_type>(it->second) > vi)
          it->second -= count_decrease;
      }
    }

    return begin(index);
  }

  void add(size_type index, value_type to_add)
  {
    size_type old_size = size(index);
    pointer p = resize(index, old_size+1);
    p[old_size] = to_add;
  }

  void clear()
  {
    offsets.clear();
    values.clear();
  }


  ValueContainer const & get_values() const { return values; }
  OffsetMap const & get_offsets() const { return offsets; }

private:
  ValueContainer values;
  OffsetMap offsets;
};

#endif
