#ifndef VIENNAGRID_TRAITS_CONTAINER_HPP
#define VIENNAGRID_TRAITS_CONTAINER_HPP

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


#include <vector>
#include <limits>
#include "viennagrid/forwards.h"

/** @file container.hpp
    @brief Provides a generic layer for container operations.
*/

namespace viennagrid 
{
  
  /** @brief A generic layer for accessing containers such as std::vector, std::deque, std::map, etc. */
  namespace traits
  {
    
    /** @brief  Reserve memory in a container. Default case: Do nothing, because the container might not have a reserve() function  */
    template <typename T>
    void reserve(T & t, std::size_t new_size)
    {
      //by default, no need to do anything.
    }
    
    /** @brief  Reserve memory in a vector. */
    template <typename T, typename A>
    void reserve(std::vector<T, A> & vec, std::size_t new_size)
    {
      vec.reserve(new_size);
    }


    /** @brief  Resize a container. Default case: Do nothing, because the container might not have a resize() function  */
    template <typename T>
    void resize(T & t, std::size_t new_size)
    {
      //by default, no need to do anything.
    }
    
    /** @brief  Resize a vector. */
    template <typename T, typename A>
    void resize(std::vector<T, A> & vec, std::size_t new_size)
    {
      vec.resize(new_size);
    }


    /** @brief Generic determination of the size of a container. Assuming a .size() member by default. Can be overloaded for user types */
    template <typename T>
    std::size_t size(T & t)
    {
      //by default, a member function .size() is expected:
      return t.size();
    }
    
    /** @brief Capacity of a container. Default case: 'infinite'*/
    template <typename T>
    std::size_t capacity(T & t)
    {
      return std::numeric_limits<std::size_t>::max();
    }
    
    /** @brief Capacity of a container. Overload for a std::vector */
    template <typename T, typename A>
    std::size_t capacity(std::vector<T, A> & vec, std::size_t new_size)
    {
      return vec.capacity();
    }


  }
} 

#endif
