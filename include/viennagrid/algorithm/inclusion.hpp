#ifndef VIENNAGRID_ALGORITHM_INCLUSION_HPP
#define VIENNAGRID_ALGORITHM_INCLUSION_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <limits>
#include "viennagrid/viennagrid.hpp"
#include "viennagrid/algorithm/spanned_volume.hpp"
#include "viennagrid/algorithm/detail/numeric.hpp"

/** @file viennagrid/algorithm/inclusion.hpp
    @brief Tests for inclusion of a point inside an element, segment, or mesh.
*/

namespace viennagrid
{
  namespace detail
  {
    // http://www.blackpawn.com/texts/pointinpoly/
    /** @brief Is inside implementation for triangle in arbitrary dimensions. */
    template<typename PointT, typename NumericConfigT>
    bool is_inside_triangle(PointT const & tp0, PointT const & tp1, PointT const & tp2,
                            PointT const & p,
                            NumericConfigT numeric_config)
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;

      PointT v0 = tp2-tp0;
      PointT v1 = tp1-tp0;
      PointT v2 = p-tp0;

      NumericType dot00 = viennagrid::inner_prod(v0, v0);
      NumericType dot01 = viennagrid::inner_prod(v0, v1);
      NumericType dot02 = viennagrid::inner_prod(v0, v2);
      NumericType dot11 = viennagrid::inner_prod(v1, v1);
      NumericType dot12 = viennagrid::inner_prod(v1, v2);

      NumericType denom = static_cast<NumericType>(1) / (dot00 * dot11 - dot01 * dot01);
      NumericType u = (dot11*dot02 - dot01*dot12) * denom;
      NumericType v = (dot00*dot12 - dot01*dot02) * denom;

      NumericType abs_eps = absolute_tolerance<NumericType>(numeric_config);
      return (u >= -abs_eps) && (v >= -abs_eps) && (u+v <= static_cast<NumericType>(1) + NumericType(2.0)*abs_eps );
    }

    /** @brief Is inside implementation for tetrahedron. */
    template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
    bool is_inside_impl(PointAccessorT const accessor,
                        ElementT const & element, viennagrid::triangle_tag,
                        PointT const & p,
                        NumericConfigT numeric_config )
    {
      return is_inside_triangle( accessor.get(viennagrid::vertices(element)[0]),
                                 accessor.get(viennagrid::vertices(element)[1]),
                                 accessor.get(viennagrid::vertices(element)[2]),
                                 p, numeric_config );
    }

    /** @brief Is inside implementation for tetrahedron. */
    template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
    bool is_inside_impl(PointAccessorT const accessor,
                        ElementT const & element, viennagrid::quadrilateral_tag,
                        PointT const & p,
                        NumericConfigT numeric_config )
    {
      return is_inside_triangle( accessor.get(viennagrid::vertices(element)[0]),
                                 accessor.get(viennagrid::vertices(element)[1]),
                                 accessor.get(viennagrid::vertices(element)[2]),
                                 p, numeric_config ) ||
             is_inside_triangle( accessor.get(viennagrid::vertices(element)[1]),
                                 accessor.get(viennagrid::vertices(element)[2]),
                                 accessor.get(viennagrid::vertices(element)[3]),
                                 p, numeric_config );
    }



    /** @brief Is inside implementation for tetrahedron. */
    template<typename PointT, typename NumericConfigT>
    bool is_inside_tetrahedron(PointT const & tp0, PointT const & tp1, PointT const & tp2, PointT const & tp3,
                               PointT const & p,
                               NumericConfigT numeric_config )
    {
      typedef typename viennagrid::result_of::coord<PointT>::type NumericType;

      NumericType denom = static_cast<NumericType>(1) / spanned_volume(tp0, tp1, tp2, tp3);

      NumericType A = spanned_volume(  p,tp1,tp2,tp3) * denom;
      NumericType B = spanned_volume(tp0,  p,tp2,tp3) * denom;
      NumericType C = spanned_volume(tp0,tp1,  p,tp3) * denom;
      NumericType D = spanned_volume(tp0,tp1,tp2,  p) * denom;

      NumericType abs_eps = absolute_tolerance<NumericType>(numeric_config);
      return (A >= -abs_eps) && (B >= -abs_eps) && (C >= -abs_eps) && (D >= -abs_eps) && (A+B+C+D <= static_cast<NumericType>(1) + NumericType(2.0)*abs_eps);
    }

    /** @brief Is inside implementation for tetrahedron. */
    template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
    bool is_inside_impl(PointAccessorT const accessor,
                        ElementT const & element, viennagrid::tetrahedron_tag,
                        PointT const & p,
                        NumericConfigT numeric_config )
    {
      return is_inside_tetrahedron( accessor.get(viennagrid::vertices(element)[0]),
                                    accessor.get(viennagrid::vertices(element)[1]),
                                    accessor.get(viennagrid::vertices(element)[2]),
                                    accessor.get(viennagrid::vertices(element)[3]),
                                    p, numeric_config );
    }


    /** @brief Is inside implementation for tetrahedron. */
    template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
    bool is_inside_impl( PointAccessorT const accessor,
                         ElementT const & element,
                         PointT const & p,
                         NumericConfigT numeric_config )
    {
      if (element.tag() == viennagrid::triangle_tag())
        return is_inside_impl(accessor, element, triangle_tag(), p, numeric_config);
      if (element.tag() == viennagrid::quadrilateral_tag())
        return is_inside_impl(accessor, element, quadrilateral_tag(), p, numeric_config);
      if (element.tag() == viennagrid::tetrahedron_tag())
        return is_inside_impl(accessor, element, tetrahedron_tag(), p, numeric_config);

      assert(false);
      return false;
    }
  }


  /** @brief Determines if a given point is inside an element. Returns true if the point is inside the element, false otherwise.
   *
   * @param accessor            The point accessor providing point information for geometric calculation
   * @param element             The element to test
   * @param point               The point to test
   * @param numeric_config      The numeric config
   */
  template<typename PointAccessorT, typename ElementT, typename PointT, typename NumericConfigT>
  bool is_inside( PointAccessorT const accessor, ElementT const & element,
                  PointT const & point,
                  NumericConfigT numeric_config )
  {
    return detail::is_inside_impl( accessor, element, point, numeric_config );
  }

  /** @brief Determines if a given point is inside an element. Returns true if the point is inside the element, false otherwise.
   *
   * @param element             The element to test
   * @param point               The point to test
   * @param numeric_config      The numeric config
   */
  template<typename ElementT, typename PointT, typename NumericConfigT>
  bool is_inside( ElementT const & element, PointT const & point, NumericConfigT numeric_config )
  {
    return is_inside( root_mesh_point_accessor(), element, point, numeric_config );
  }


  /** @brief Determines if a given point is inside an element. Returns true if the point is inside the element, false otherwise.
   *
   * @param element             The element to test
   * @param point               The point to test
   */
  template<typename ElementT, typename PointT>
  bool is_inside( ElementT const & element, PointT const & point )
  {
    typedef typename viennagrid::result_of::coord<PointT>::type NumericType;
    return is_inside( element, point, NumericType(10.0)*std::numeric_limits<NumericType>::epsilon() );
  }

}


#endif
