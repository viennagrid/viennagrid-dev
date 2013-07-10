#ifndef VIENNAGRID_ALGORITHM_SPANNED_VOLUME_HPP
#define VIENNAGRID_ALGORITHM_SPANNED_VOLUME_HPP

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



//#include <math.h>
#include "viennagrid/forwards.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/traits/point.hpp"


/** @file spanned_volume.hpp
    @brief Computes the volume of n-simplices spanned by points
*/

namespace viennagrid
{

  namespace detail
  {
    template <typename PointType, long dim = traits::dimension<PointType>::value>
    struct signed_spanned_volume_impl;


    /** @brief Implementation of the volume spanned by two points in one dimension */
    template <typename PointType>
    struct signed_spanned_volume_impl<PointType, 1>
    {
      typedef typename result_of::coord_type<PointType>::type    value_type;

      static value_type apply(PointType const & p1,
                              PointType const & p2)
      {
        return p2[0] - p1[0];
      }
    };

    //in 2d:
    /** @brief Implementation of the volume of simplices spanned by points in two geometrical dimension */
    template <typename PointType>
    struct signed_spanned_volume_impl<PointType, 2>
    {
      typedef typename result_of::coord_type<PointType>::type    value_type;

      static value_type apply(PointType const & p1,
                              PointType const & p2)
      {
        //a line
        return sqrt(   (p2[0] - p1[0]) * (p2[0] - p1[0])
                     + (p2[1] - p1[1]) * (p2[1] - p1[1])  );
      }

      static value_type apply(PointType const & A,
                              PointType const & B,
                              PointType const & C)
      {
        //a triangle:
        return (  A[0] * (B[1] - C[1])
                    + B[0] * (C[1] - A[1])
                    + C[0] * (A[1] - B[1]) ) / 2.0;
      }

    };


    /** @brief Implementation of the volume of simplices spanned by points in three geometrical dimension */
    template <typename PointType>
    struct signed_spanned_volume_impl<PointType, 3>
    {
      typedef typename result_of::coord_type<PointType>::type    value_type;

      static value_type apply(PointType const & p1,
                              PointType const & p2)
      {
        //a line
        return sqrt(   (p2[0] - p1[0]) * (p2[0] - p1[0])
                     + (p2[1] - p1[1]) * (p2[1] - p1[1])
                     + (p2[2] - p1[2]) * (p2[2] - p1[2]) );
      }

      static value_type apply(PointType const & p1,
                              PointType const & p2,
                              PointType const & p3)
      {
        PointType v1 = p2 - p1;
        PointType v2 = p3 - p1;

        PointType v3 = cross_prod(v1, v2);

        return norm(v3) / 2.0;
      }

      static value_type apply(PointType const & p1,
                              PointType const & p2,
                              PointType const & p3,
                              PointType const & p4)
      {
        PointType v1 = p2 - p1;
        PointType v2 = p3 - p1;
        PointType v3 = p4 - p1;

        return (inner_prod(v1, cross_prod(v2, v3)) ) / 6.0;
      }

    };
  } //namespace detail






  //
  // Mixed coordinate systems:
  //
  /** @brief Dispatch facility for two points with possibly different coordinate systems */
  template<typename PointType1, typename PointType2, typename CSystem1, typename CSystem2>
  typename result_of::coord_type<PointType1>::type
  signed_spanned_volume_impl(PointType1 const & p1,
                      PointType2 const & p2,
                      CSystem1 const &,
                      CSystem2 const &)
  {
    typedef typename result_of::coord_type<PointType1>::type    value_type;
    typedef typename result_of::cartesian_point<PointType1>::type   CartesianPoint1;

    return detail::signed_spanned_volume_impl<CartesianPoint1>::apply(to_cartesian(p1), to_cartesian(p2));
  }

  /** @brief Dispatch facility for three points with possibly different coordinate systems */
  template<typename PointType1, typename PointType2, typename PointType3,
           typename CSystem1, typename CSystem2, typename CSystem3>
  typename result_of::coord_type<PointType1>::type
  signed_spanned_volume_impl(PointType1 const & p1,
                      PointType2 const & p2,
                      PointType3 const & p3,
                      CSystem1 const &,
                      CSystem2 const &,
                      CSystem3 const &)
  {
    typedef typename result_of::coord_type<PointType1>::type    value_type;
    typedef typename result_of::cartesian_point<PointType1>::type   CartesianPoint1;

    return detail::signed_spanned_volume_impl<CartesianPoint1>::apply(to_cartesian(p1), to_cartesian(p2), to_cartesian(p3));
  }

  /** @brief Dispatch facility for four points with possibly different coordinate systems */
  template<typename PointType1, typename PointType2, typename PointType3, typename PointType4,
           typename CSystem1, typename CSystem2, typename CSystem3, typename CSystem4>
  typename result_of::coord_type<PointType1>::type
  signed_spanned_volume_impl(PointType1 const & p1,
                      PointType2 const & p2,
                      PointType3 const & p3,
                      PointType4 const & p4,
                      CSystem1 const &,
                      CSystem2 const &,
                      CSystem3 const &,
                      CSystem4 const &)
  {
    typedef typename result_of::coord_type<PointType1>::type    value_type;
    typedef typename result_of::cartesian_point<PointType1>::type   CartesianPoint1;

    return detail::signed_spanned_volume_impl<CartesianPoint1>::apply(to_cartesian(p1), to_cartesian(p2), to_cartesian(p3), to_cartesian(p4));
  }

  //
  // All Cartesian:
  //
  /** @brief Dispatch facility for two points in Cartesian coordinates */
  template<typename PointType1, typename PointType2, long d>
  typename result_of::coord_type<PointType1>::type
  signed_spanned_volume_impl(PointType1 const & p1,
                      PointType2 const & p2,
                      cartesian_cs<d>,
                      cartesian_cs<d>)
  {
    return detail::signed_spanned_volume_impl<PointType1>::apply(p1, p2);
  }

  /** @brief Dispatch facility for three points in Cartesian coordinates */
  template <typename PointType1, typename PointType2, typename PointType3, long d>
  typename result_of::coord_type<PointType1>::type
  signed_spanned_volume_impl(PointType1 const & p1,
                      PointType2 const & p2,
                      PointType3 const & p3,
                      cartesian_cs<d>,
                      cartesian_cs<d>,
                      cartesian_cs<d>)
  {
    return detail::signed_spanned_volume_impl<PointType1>::apply(p1, p2, p3);
  }

  /** @brief Dispatch facility for four points in Cartesian coordinates */
  template <typename PointType1, typename PointType2, typename PointType3, typename PointType4, long d>
  typename result_of::coord_type<PointType1>::type
  signed_spanned_volume_impl(PointType1 const & p1,
                      PointType2 const & p2,
                      PointType3 const & p3,
                      PointType4 const & p4,
                      cartesian_cs<d>,
                      cartesian_cs<d>,
                      cartesian_cs<d>,
                      cartesian_cs<d>)
  {
    return detail::signed_spanned_volume_impl<PointType1>::apply(p1, p2, p3, p4);
  }



  //
  // public interface
  //
  /** @brief Returns the volume of the 1-simplex (line) spanned by the two points */
  template <typename PointType1, typename PointType2>
  typename result_of::coord_type<PointType1>::type
  signed_spanned_volume(PointType1 const & p1, PointType2 const & p2)
  {
    return signed_spanned_volume_impl(p1,
                               p2,
                               typename traits::coordinate_system<PointType1>::type(),
                               typename traits::coordinate_system<PointType2>::type());
  }


  /** @brief Returns the two-dimensional volume of the 2-simplex (triangle) spanned by the three points */
  template <typename PointType1, typename PointType2, typename PointType3>
  typename result_of::coord_type<PointType1>::type
  signed_spanned_volume(PointType1 const & p1, PointType2 const & p2, PointType3 const & p3)
  {
    return signed_spanned_volume_impl(p1,
                               p2,
                               p3,
                               typename traits::coordinate_system<PointType1>::type(),
                               typename traits::coordinate_system<PointType2>::type(),
                               typename traits::coordinate_system<PointType3>::type()
                              );

  }


  /** @brief Returns the three-dimensional volume of the 3-simplex (tetrahedron) spanned by the four points */
  template <typename PointType1, typename PointType2, typename PointType3, typename PointType4>
  typename result_of::coord_type<PointType1>::type
  signed_spanned_volume(PointType1 const & p1,
                  PointType2 const & p2,
                  PointType3 const & p3,
                  PointType4 const & p4)
  {
    return signed_spanned_volume_impl(p1,
                               p2,
                               p3,
                               p4,
                               typename traits::coordinate_system<PointType1>::type(),
                               typename traits::coordinate_system<PointType2>::type(),
                               typename traits::coordinate_system<PointType3>::type(),
                               typename traits::coordinate_system<PointType4>::type()
                              );
  }


  /** @brief Returns the volume of the 1-simplex (line) spanned by the two points */
  template <typename PointType1, typename PointType2>
  typename result_of::coord_type<PointType1>::type
  spanned_volume(PointType1 const & p1, PointType2 const & p2)
  {
    return std::abs(signed_spanned_volume(p1, p2));
  }


  /** @brief Returns the two-dimensional volume of the 2-simplex (triangle) spanned by the three points */
  template <typename PointType1, typename PointType2, typename PointType3>
  typename result_of::coord_type<PointType1>::type
  spanned_volume(PointType1 const & p1, PointType2 const & p2, PointType3 const & p3)
  {
    return std::abs(signed_spanned_volume(p1, p2, p3));
  }


  /** @brief Returns the three-dimensional volume of the 3-simplex (tetrahedron) spanned by the four points */
  template <typename PointType1, typename PointType2, typename PointType3, typename PointType4>
  typename result_of::coord_type<PointType1>::type
  spanned_volume(PointType1 const & p1,
                  PointType2 const & p2,
                  PointType3 const & p3,
                  PointType4 const & p4)
  {
    return std::abs(signed_spanned_volume(p1, p2, p3, p4));
  }

}
#endif
