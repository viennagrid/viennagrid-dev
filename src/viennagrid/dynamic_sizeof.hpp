#ifndef VIENNAUTILS_DYNAMIC_SIZEOF_HPP
#define VIENNAUTILS_DYNAMIC_SIZEOF_HPP

#include <utility>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include "boost/container/flat_map.hpp"


#include <ostream>

class viennagrid_mesh_hierarchy_;


namespace viennautils
{

  class indentation
  {
  public:

    indentation(int count) : count_(count), c_(' ') {}
    indentation(int count, char c) : count_(count), c_(c) {}

    std::ostream& operator<< (std::ostream & os)
    {
      for (int i = 0; i != count_; ++i)
        os << c_;
      return os;
    }

  private:
    int count_;
    char c_;
  };

  template<typename T>
  long dynamic_sizeof(T const & t);

  template<typename T>
  long total_sizeof(T const & t);


  namespace detail
  {
    template<typename T>
    struct dynamic_sizeof_impl;


    template<typename T>
    struct dynamic_sizeof_impl<T*>
    {
      static long size(T*) { return 0; }
    };



    template<typename T, int N>
    struct dynamic_sizeof_impl<T[N]>
    {
      static long size(T const * v)
      {
        long size = 0;
        for (int i = 0; i != N; ++i)
          size += dynamic_sizeof(v[i]);
        return size;
      }
    };

    template<typename T, int N>
    struct dynamic_sizeof_impl<T const[N]>
    {
      static long size(T const * v)
      {
        long size = 0;
        for (int i = 0; i != N; ++i)
          size += dynamic_sizeof(v[i]);
        return size;
      }
    };


    template<>
    struct dynamic_sizeof_impl<char>
    {
      static long size(char) { return 0; }
    };

    template<>
    struct dynamic_sizeof_impl<unsigned char>
    {
      static long size(unsigned char) { return 0; }
    };

    template<>
    struct dynamic_sizeof_impl<short>
    {
      static long size(short) { return 0; }
    };

    template<>
    struct dynamic_sizeof_impl<unsigned short>
    {
      static long size(unsigned short) { return 0; }
    };

    template<>
    struct dynamic_sizeof_impl<int>
    {
      static long size(int) { return 0; }
    };

    template<>
    struct dynamic_sizeof_impl<unsigned long>
    {
      static long size(unsigned long) { return 0; }
    };


    template<>
    struct dynamic_sizeof_impl<float>
    {
      static long size(float) { return 0; }
    };

    template<>
    struct dynamic_sizeof_impl<double>
    {
      static long size(double) { return 0; }
    };


    template<>
    struct dynamic_sizeof_impl<std::string>
    {
      static long size(std::string const & s) { return sizeof(char) * s.capacity(); }
//       static long size(std::string const & s) { return sizeof(char) * s.size(); }
    };

    template<typename T, typename U>
    struct dynamic_sizeof_impl< std::pair<T,U> >
    {
      static long size(std::pair<T,U> const & p) { return dynamic_sizeof(p.first) + dynamic_sizeof(p.second); }
    };

    template<typename T, typename Alloc>
    struct dynamic_sizeof_impl< std::vector<T, Alloc> >
    {
      static long size(std::vector<T, Alloc> const & v)
      {
        long size = v.capacity()*sizeof(T);
//         long size = v.size()*sizeof(T);
        for (typename std::vector<T, Alloc>::size_type i = 0; i != v.size(); ++i)
          size += dynamic_sizeof(v[i]);
        return size;
      }
    };

    template<typename T, typename Alloc>
    struct dynamic_sizeof_impl< std::deque<T, Alloc> >
    {
      static long size(std::deque<T, Alloc> const & d)
      {
        long size = d.size()*sizeof(T);
        for (typename std::deque<T, Alloc>::size_type i = 0; i != d.size(); ++i)
          size += dynamic_sizeof(d[i]);
        return size;
      }
    };



    template<typename T, typename Compare, typename Alloc>
    struct dynamic_sizeof_impl< std::set<T,Compare,Alloc> >
    {
      static long size(std::set<T,Compare,Alloc> const & s)
      {
        long size = 0;
        for (typename std::set<T,Compare,Alloc>::const_iterator it = s.begin(); it != s.end(); ++it)
          size += total_sizeof(*it);
        return size;
      }
    };

    template<typename Key, typename T, typename Compare, typename Alloc>
    struct dynamic_sizeof_impl< std::map<Key,T,Compare,Alloc> >
    {
      static long size(std::map<Key,T,Compare,Alloc> const & m)
      {
        long size = 0;
        for (typename std::map<Key,T,Compare,Alloc>::const_iterator it = m.begin(); it != m.end(); ++it)
          size += total_sizeof(*it) + sizeof(std::_Rb_tree_node_base);
        return size;
      }
    };

    template<typename Key, typename T, typename Compare, typename Alloc>
    struct dynamic_sizeof_impl< boost::container::flat_map<Key,T,Compare,Alloc> >
    {
      static long size(boost::container::flat_map<Key,T,Compare,Alloc> const & m)
      {
        long size = m.capacity()*(sizeof(T)+sizeof(Key));
        for (typename boost::container::flat_map<Key,T,Compare,Alloc>::const_iterator it = m.begin(); it != m.end(); ++it)
          size += dynamic_sizeof(*it);
        return size;
      }
    };



    template<>
    struct dynamic_sizeof_impl<viennagrid_mesh_hierarchy_>
    {
      static long size(viennagrid_mesh_hierarchy_ const & mh);
    };
  }



  template<typename T>
  long dynamic_sizeof(T const & t)
  {
    return detail::dynamic_sizeof_impl<T>::size(t);
  }

  template<typename T>
  long total_sizeof(T const & t)
  {
    return sizeof(T) + dynamic_sizeof(t);
  }



}

#endif
