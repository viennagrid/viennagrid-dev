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

#ifndef VIENNAGRID_NORM_GUARD
#define VIENNAGRID_NORM_GUARD

#include <cmath>
#include "viennagrid/forwards.h"

namespace viennagrid {


// -----------------------------------------------------------------------------
//
// norm algorithm specialization hierarchy
//
template<typename Tag> 
struct norm_impl 
{
   template<typename VectorT>
   typename VectorT::value_type operator()(VectorT const& vector)
   {
      std::cerr << "ViennaGrid - Norm Error - this error type is not implemented" << std::endl;
      return 0.0;
   }
};

template<>
struct norm_impl<viennagrid::one_tag>
{
   template<typename VectorT>
   typename VectorT::value_type operator()(VectorT const& vector)
   {
      typename VectorT::value_type result(0);
      for(std::size_t i = 0; i < traits::size<VectorT>::value; i++)       // TODO replace with generic size
         result += std::fabs(vector[i]);
      return result;
   }
};

template<>
struct norm_impl<viennagrid::two_tag>
{
   template<typename VectorT>
   typename VectorT::value_type operator()(VectorT const& vector)
   {
      typename VectorT::value_type result(0);
      for(std::size_t i = 0; i < traits::size<VectorT>::value; i++)       // TODO replace with generic size
         result += vector[i]*vector[i];
      return std::sqrt(result);   
   }
};

template<>
struct norm_impl<viennagrid::inf_tag>
{
   template<typename VectorT>
   typename VectorT::value_type operator()(VectorT const& vector)
   {
      typename VectorT::value_type result(0);
      for(std::size_t i = 0; i < traits::size<VectorT>::value; i++)       // TODO replace with generic size
      {
         if(std::fabs(vector[i]) > result)
            result = std::fabs(vector[i]);
      }
      return result;
   }
};

// -----------------------------------------------------------------------------
//
// norm algorithm generic interface functions
//
template<typename VectorT, typename Tag>
typename VectorT::value_type  norm(VectorT const& vector, Tag const& tag)
{
   return norm_impl<Tag>()(vector);
}

template<typename VectorT>
typename VectorT::value_type  norm(VectorT const& vector)
{
   return norm_impl<viennagrid::two_tag>()(vector);
}

} 

#endif

