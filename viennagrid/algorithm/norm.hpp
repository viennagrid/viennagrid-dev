#ifndef VIENNAGRID_ALGORITHM_NORM_GUARD
#define VIENNAGRID_ALGORITHM_NORM_GUARD

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
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
#include "viennagrid/forwards.hpp"
#include "viennagrid/traits/point.hpp"
#include "viennagrid/point.hpp"

/** @file norm.hpp
    @brief Computes the computation of norms for vectors
*/

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
      typename result_of::coord_type<PointType>::type operator()(PointType const&)
      {
          std::cerr << "ViennaGrid - Norm Error - this error type is not implemented" << std::endl;
          return 0.0;
      }
    };

    /** @brief Implementation of the 1-norm */
    template<>
    struct norm_impl<viennagrid::one_tag>
    {
      template<typename PointType>
      typename result_of::coord_type<PointType>::type operator()(PointType const& p)
      {
          typename result_of::coord_type<PointType>::type result(0);
          for(std::size_t i = 0; i < traits::dynamic_size(p); i++)
            result += std::abs(p[i]);
          return result;
      }
    };

    /** @brief Implementation of the 2-norm */
    template<>
    struct norm_impl<viennagrid::two_tag>
    {
      template<typename PointType>
      typename result_of::coord_type<PointType>::type operator()(PointType const& p)
      {
          typename result_of::coord_type<PointType>::type result(0);
          for(std::size_t i = 0; i < traits::dynamic_size(p); i++)
            result += p[i]*p[i];
          return std::sqrt(result);
      }
    };

    /** @brief Implementation of the inf-norm */
    template<>
    struct norm_impl<viennagrid::inf_tag>
    {
      template<typename PointType>
      typename result_of::coord_type<PointType>::type operator()(PointType const& p)
      {
          typename result_of::coord_type<PointType>::type result(0);
          for(std::size_t i = 0; i < traits::dynamic_size(p); i++)
          {
            if(std::abs(p[i]) > result)
                result = std::abs(p[i]);
          }
          return result;
      }
    };

  } //namespace detail

  /** @brief Dispatch for a point that needs coordinate conversion */
  template<typename NormTag, typename PointType, typename CSystem>
  typename result_of::coord_type<PointType>::type
  norm_impl(PointType const & p, CSystem const &)
  {
    typedef typename result_of::coord_type<PointType>::type    value_type;
    typedef typename result_of::cartesian_point<PointType>::type   CartesianPoint1;

    return detail::norm_impl<NormTag>()(to_cartesian(p));
  }

  /** @brief Dispatch for a point that does not need coordinate conversion */
  template<typename NormTag, typename PointType1, long d>
  typename result_of::coord_type<PointType1>::type
  norm_impl(PointType1 const & p, cartesian_cs<d>)
  {
    return detail::norm_impl<NormTag>()(p);
  }


  // -----------------------------------------------------------------------------
  //
  // norm algorithm generic interface functions
  //
  /** @brief Returns the norm of a point. The type of the norm is specified by a tag. Supported tags are one_tag, two_tag and inf_tag.
   *
   * @param p    The vector (point) for which the norm should be computed.
   */
  template<typename PointType, typename Tag>
  typename result_of::coord_type<PointType>::type
  norm(PointType const & p, Tag)
  {
    return norm_impl<Tag>(p, typename traits::coordinate_system<PointType>::type());
  }

  /** @brief Returns the 2-norm of a point. Result is such as if the point were transformed to Cartesian coordinates first */
  template<typename PointType>
  typename result_of::coord_type<PointType>::type
  norm(PointType const & p)
  {
    return norm_impl<viennagrid::two_tag>(p, typename traits::coordinate_system<PointType>::type());
  }


  //convenience shortcuts:
  /** @brief Convenience shortcut for the 1-norm of a vector. */
  template<typename PointType>
  typename result_of::coord_type<PointType>::type
  norm_1(PointType const & p)
  {
    return norm_impl<viennagrid::one_tag>(p, typename traits::coordinate_system<PointType>::type());
  }

  /** @brief Convenience shortcut for the 2-norm of a vector. */
  template<typename PointType>
  typename result_of::coord_type<PointType>::type
  norm_2(PointType const & p)
  {
    return norm_impl<viennagrid::two_tag>(p, typename traits::coordinate_system<PointType>::type());
  }

  /** @brief Convenience shortcut for the inf-norm of a vector. */
  template<typename PointType>
  typename result_of::coord_type<PointType>::type
  norm_inf(PointType const & p)
  {
    return norm_impl<viennagrid::inf_tag>(p, typename traits::coordinate_system<PointType>::type());
  }

}

#endif

