#ifndef VIENNAGRID_ALGORITHM_NORM_GUARD
#define VIENNAGRID_ALGORITHM_NORM_GUARD

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

#include <cmath>
#include "viennagrid/forwards.h"
#include "viennagrid/traits/point.hpp"
#include "viennagrid/point.hpp"

namespace viennagrid 
{
  
  namespace detail
  {

    // -----------------------------------------------------------------------------
    //
    // norm algorithm specialization hierarchy
    //
    template<typename Tag> 
    struct norm_impl 
    {
      template<typename PointType>
      typename traits::value_type<PointType>::type operator()(PointType const& p)
      {
          std::cerr << "ViennaGrid - Norm Error - this error type is not implemented" << std::endl;
          return 0.0;
      }
    };

    template<>
    struct norm_impl<viennagrid::one_tag>
    {
      template<typename PointType>
      typename traits::value_type<PointType>::type operator()(PointType const& p)
      {
          typename traits::value_type<PointType>::type result(0);
          for(std::size_t i = 0; i < traits::dynamic_size(p); i++)
            result += std::fabs(p[i]);
          return result;
      }
    };

    template<>
    struct norm_impl<viennagrid::two_tag>
    {
      template<typename PointType>
      typename traits::value_type<PointType>::type operator()(PointType const& p)
      {
          typename traits::value_type<PointType>::type result(0);
          for(std::size_t i = 0; i < traits::dynamic_size(p); i++)
            result += p[i]*p[i];
          return std::sqrt(result);   
      }
    };

    template<>
    struct norm_impl<viennagrid::inf_tag>
    {
      template<typename PointType>
      typename traits::value_type<PointType>::type operator()(PointType const& p)
      {
          typename traits::value_type<PointType>::type result(0);
          for(std::size_t i = 0; i < traits::dynamic_size(p); i++)
          {
            if(std::fabs(p[i]) > result)
                result = std::fabs(p[i]);
          }
          return result;
      }
    };
    
  } //namespace detail

  template<typename NormTag, typename PointType, typename CSystem>
  typename traits::value_type<PointType>::type
  norm_impl(PointType const & p, CSystem const &)
  {
    typedef typename traits::value_type<PointType>::type    value_type;
    typedef typename result_of::cartesian_point<PointType>::type   CartesianPoint1;
    
    return detail::norm_impl<NormTag>()(to_cartesian(p));
  }

  template<typename NormTag, typename PointType1>
  typename traits::value_type<PointType1>::type
  norm_impl(PointType1 const & p, cartesian_cs)
  {
    return detail::norm_impl<NormTag>()(p);
  }


  // -----------------------------------------------------------------------------
  //
  // norm algorithm generic interface functions
  //
  template<typename PointType, typename Tag>
  typename traits::value_type<PointType>::type
  norm(PointType const & p, Tag)
  {
    return norm_impl<Tag>(p, typename traits::coordinate_system<PointType>::type());
  }

  template<typename PointType>
  typename traits::value_type<PointType>::type
  norm(PointType const & p)
  {
    return norm_impl<viennagrid::two_tag>(p, typename traits::coordinate_system<PointType>::type());
  }


  //convenience shortcuts:
  template<typename PointType>
  typename traits::value_type<PointType>::type
  norm_1(PointType const & p)
  {
    return norm_impl<viennagrid::one_tag>(p, typename traits::coordinate_system<PointType>::type());
  }

  template<typename PointType>
  typename traits::value_type<PointType>::type
  norm_2(PointType const & p)
  {
    return norm_impl<viennagrid::two_tag>(p, typename traits::coordinate_system<PointType>::type());
  }

  template<typename PointType>
  typename traits::value_type<PointType>::type
  norm_inf(PointType const & p)
  {
    return norm_impl<viennagrid::inf_tag>(p, typename traits::coordinate_system<PointType>::type());
  }

} 

#endif

