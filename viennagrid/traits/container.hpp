/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Josef Weinbub                   weinbub@iue.tuwien.ac.at
               Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_TRAITS_CONTAINER_HPP
#define VIENNAGRID_TRAITS_CONTAINER_HPP


#include <vector>
#include <limits>
#include "viennagrid/forwards.h"


namespace viennagrid 
{
  
  namespace traits
  {
    //
    //  Reserve memory in a container
    //
    template <typename T>
    void reserve(T & t, std::size_t new_size)
    {
      //by default, no need to do anything.
    }
    
    template <typename T, typename A>
    void reserve(std::vector<T, A> & vec, std::size_t new_size)
    {
      vec.reserve(new_size);
    }



    //
    //  Capacity
    //
    template <typename T>
    std::size_t capacity(T & t)
    {
      return std::numeric_limits<std::size_t>::max();
    }
    
    template <typename T, typename A>
    std::size_t capacity(std::vector<T, A> & vec, std::size_t new_size)
    {
      return vec.capacity();
    }

  }
} 

#endif
