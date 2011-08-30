/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
                       Institute for Analysis and Scientific Computing,
                       TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
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
    //  Reserve memory in a container
    //
    template <typename T>
    void resize(T & t, std::size_t new_size)
    {
      //by default, no need to do anything.
    }
    
    template <typename T, typename A>
    void resize(std::vector<T, A> & vec, std::size_t new_size)
    {
      vec.resize(new_size);
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
