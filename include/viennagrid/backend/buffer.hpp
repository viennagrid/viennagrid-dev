#ifndef VIENNAGRID_BACKEND_BUFFER_HPP
#define VIENNAGRID_BACKEND_BUFFER_HPP


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

  dense_multibuffer() : offsets(1,0) {}

  iterator begin(size_type index) { return values.empty() ? 0 : &values[0] + offsets[index]; }
  iterator end(size_type index) { return values.empty() ? 0 : &values[0] + offsets[index+1]; }

  const_iterator cbegin(size_type index) const { return values.empty() ? 0 : &values[0] + offsets[index]; }
  const_iterator cend(size_type index) const { return values.empty() ? 0 : &values[0] + offsets[index+1]; }

  const_iterator begin(size_type index) const { return cbegin(index); }
  const_iterator end(size_type index) const { return cend(index); }


  iterator values_begin() { return &values[0]; }
  iterator values_end() { return &values[0] + values.size(); }

  const_iterator values_cbegin() const { return &values[0]; }
  const_iterator values_cend() const { return &values[0] + values.size(); }

  const_iterator values_begin() const { return cbegin(); }
  const_iterator values_end() const { return cend(); }


  size_type size(size_type index) const { return end(index)-begin(index); }


  pointer push_back(size_type count)
  {
    std::size_t start_index = values.size();
    values.resize( values.size()+count );
    offsets.push_back(values.size());
    return &values[0] + start_index;
  }

  pointer resize(size_type index, size_type count)
  {
    size_type old_count = size(index);

    if (count > old_count)
    {
      size_type count_increase = count-old_count;
      values.resize( values.size()+count_increase );
      for (iterator it = values_end()-1; it != begin(index)+old_count; --it)
        *it = *(it-count_increase);
      for (size_type i = index+1; i != offsets.size(); ++i)
        offsets[i] += count_increase;
    }
    else if (count < old_count)
    {
      size_type count_decrease = old_count-count;
      for (iterator it = begin(index)+count; it != values_end()-count_decrease-1; ++it)
        *it = *(it+count_decrease);
      values.resize( values.size()-count_decrease );
      for (size_type i = index+1; i != offsets.size(); ++i)
        offsets[i] -= count_decrease;
    }

    return begin(index);
  }

  void push_back(size_type index, value_type to_add)
  {
    size_type old_size = size(index);
    pointer p = resize(index, old_size+1);
    p[old_size] = to_add;
  }

  void clear()
  {
    offsets = std::vector<size_type>(1,0);
    values.clear();
  }

  void print() const
  {
    for (std::size_t i = 0; i != offsets.size()-1; ++i)
    {
      std::cout << "Entry " << i << " [size=" << size(i) << "] :";
      for (const_iterator it = begin(i); it != end(i); ++it)
        std::cout << *it << " ";
      std::cout << std::endl;
    }
  }

private:
  std::vector<value_type> values;
  std::vector< typename std::vector<value_type>::size_type > offsets;
};




template<typename IndexT, typename T, typename PointerT = T *, typename ConstPointerT = T const *>
struct sparse_multibuffer
{
public:

  typedef T value_type;
  typedef PointerT pointer;
  typedef ConstPointerT const_pointer;
  typedef IndexT size_type;

  typedef std::vector<value_type> ValueContainer;
  typedef std::map<size_type, typename ValueContainer::size_type> OffsetMap;

  sparse_multibuffer() {}

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
        if (it->second > vi)
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
        if (it->second > vi)
          it->second -= count_decrease;
      }
    }

    return begin(index);
  }

  void push_back(size_type index, value_type to_add)
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

  void print() const
  {
    for (typename OffsetMap::const_iterator it = offsets.begin(); it != offsets.end(); ++it)
    {
      std::cout << "Entry " << it->first << " [size=" << size(it->first) << "] :";
      for (const_iterator jt = begin(it->first); jt != end(it->first); ++jt)
        std::cout << *jt << " ";
      std::cout << std::endl;
    }
  }

private:
  ValueContainer values;
  OffsetMap offsets;
};

#endif
