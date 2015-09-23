#ifndef VIENNAGRID_ALGORITHM_GEOMETRY_HPP
#define VIENNAGRID_ALGORITHM_GEOMETRY_HPP

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
#include "viennagrid/core/point_accessor.hpp"
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/algorithm/detail/numeric.hpp"

/** @file viennagrid/algorithm/geometry.hpp
    @brief Contains various functions for computing geometric quantities
*/

namespace viennagrid
{

  namespace detail
  {
    /** @brief Implementation for calculating a normal vector of a vertex in 1D */
    template<typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point<PointAccessorT, ElementT>::type normal_vector_impl(
      PointAccessorT const point_accessor,
      ElementT const & element,
      viennagrid::vertex_tag,
      viennagrid::dimension_tag<1>)
    {
      typedef typename viennagrid::result_of::point<PointAccessorT, ElementT>::type    PointType;

      (void)point_accessor; (void)element;
      return PointType(1.0);
    }

    /** @brief Implementation for calculating a normal vector of a line in 2D */
    template<typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point<PointAccessorT, ElementT>::type normal_vector_impl(
      PointAccessorT const point_accessor,
      ElementT const & element,
      viennagrid::line_tag,
      viennagrid::dimension_tag<2>)
    {
      typedef typename viennagrid::result_of::point<PointAccessorT, ElementT>::type    PointType;

      PointType p0 = point_accessor.get( viennagrid::vertices(element)[0] );
      PointType p1 = point_accessor.get( viennagrid::vertices(element)[1] );

      PointType line = p1-p0;
      std::swap(line[0], line[1]);
      line[0] = -line[0];

      return line;
    }

    /** @brief Implementation for calculating a normal vector of a triangle in 3D */
    template<typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point<PointAccessorT, ElementT>::type normal_vector_impl(
      PointAccessorT const point_accessor,
      ElementT const & element,
      viennagrid::triangle_tag,
      viennagrid::dimension_tag<3>)
    {
      typedef typename viennagrid::result_of::point<PointAccessorT, ElementT>::type    PointType;

      PointType p0 = point_accessor.get( viennagrid::vertices(element)[0] );
      PointType p1 = point_accessor.get( viennagrid::vertices(element)[1] );
      PointType p2 = point_accessor.get( viennagrid::vertices(element)[2] );

      return viennagrid::cross_prod( p1-p0, p2-p0 );
    }

    /** @brief Implementation for calculating a normal vector of a quadrilateral in 3D.
    *
    *  Reuses the implementation for a triangle.
    */
    template<typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::point<PointAccessorT, ElementT>::type normal_vector_impl(
      PointAccessorT const point_accessor,
      ElementT const & element,
      viennagrid::quadrilateral_tag,
      viennagrid::dimension_tag<3>)
    {
      return normal_vector_impl(point_accessor, element, viennagrid::triangle_tag(), viennagrid::dimension_tag<3>());
    }

  }






  /** @brief Calculates the normal vector of an element
   *
   * @param point_accessor          Point accessor for input points
   * @param element                 The input element
   */
  template<typename PointAccessorT, typename ElementT>
  typename viennagrid::result_of::point<PointAccessorT, ElementT>::type normal_vector(
    PointAccessorT const point_accessor,
    ElementT const & element)
  {
    typedef typename viennagrid::result_of::point<PointAccessorT, ElementT>::type PointType;

    if (element.is_vertex())
      return detail::normal_vector_impl( point_accessor, element, vertex_tag(), dimension_tag<1>() );
    else if (element.is_line())
      return detail::normal_vector_impl( point_accessor, element, line_tag(), dimension_tag<2>() );
    else if (element.is_triangle())
      return detail::normal_vector_impl( point_accessor, element, triangle_tag(), dimension_tag<3>() );
    else if (element.is_quadrilateral())
      return detail::normal_vector_impl( point_accessor, element, quadrilateral_tag(), dimension_tag<3>() );

    assert(false);
    return PointType();
  }


  /** @brief Calculates the normal vector of an element
   *
   * @param element                The input element
   */
  template<typename ElementT>
  typename viennagrid::result_of::point<ElementT>::type normal_vector( ElementT const & element )
  {
    return normal_vector( root_mesh_point_accessor(), element );
  }



  /** @brief Calculates the determinant of a 1x1 matrix with the columns provided as 1D-points
   *
   * @param p0                The first column/point
   */
  template<typename PointT>
  typename viennagrid::result_of::coord<PointT>::type determinant( PointT const & p0)
  {
    return p0[0];
  }

  /** @brief Calculates the determinant of a 2x2 matrix with the columns provided as 2D-points
   *
   * @param p0                The first column
   * @param p1                The second column
   */
  template<typename PointT>
  typename viennagrid::result_of::coord<PointT>::type determinant( PointT const & p0, PointT const & p1 )
  {
    return p0[0]*p1[1] - p0[1]*p1[0];
  }

  /** @brief Calculates the determinant of a 3x3 matrix with the columns provided as 3D-points
   *
   * @param p0                The first column
   * @param p1                The second column
   * @param p2                The third column
   */
  template<typename PointT>
  typename viennagrid::result_of::coord<PointT>::type determinant( PointT const & p0, PointT const & p1, PointT const & p2 )
  {
    return p0[0]*p1[1]*p2[2] + p1[0]*p2[1]*p0[2] + p2[0]*p0[1]*p1[2] - p0[2]*p1[1]*p2[0] - p1[2]*p2[1]*p0[0] - p2[2]*p0[1]*p1[0];
  }


  /** @brief Calculates the bounding box of a point iterator range. A pair of points is return, where the first represents the coordinate-wise minimum and the second represents the coordinate-wise maximum.
   *
   * @param it                The start point iterator
   * @param it_end            The end point iterator
   */
  template<typename PointIteratorT>
  std::pair<
      typename std::iterator_traits<PointIteratorT>::value_type,
      typename std::iterator_traits<PointIteratorT>::value_type
  > bounding_box( PointIteratorT it, PointIteratorT const & it_end )
  {
    typedef typename std::iterator_traits<PointIteratorT>::value_type   PointType;
    typedef typename viennagrid::result_of::coord<PointType>::type      NumericType;

    if (it == it_end)
      return std::make_pair(PointType(),PointType());

    PointType lower_left( (*it).size() );
    PointType upper_right( (*it).size() );

    std::fill( lower_left.begin(), lower_left.end(), std::numeric_limits<NumericType>::max() );
    std::fill( upper_right.begin(), upper_right.end(), - std::numeric_limits<NumericType>::max() );
    //std::fill( upper_right.begin(), upper_right.end(), std::numeric_limits<NumericType>::lowest() );    C++11

    for (; it != it_end; ++it )
    {
      lower_left = viennagrid::min( lower_left, *it );
      upper_right = viennagrid::max( upper_right, *it );
    }

    return std::make_pair( lower_left, upper_right );
  }


  /** @brief Calculates the bounding box a mesh. A pair of points is return, where the first represents the coordinate-wise minimum and the second represents the coordinate-wise maximum.
   *
   * @param mesh              The input mesh
   */
  template<typename MeshElementT>
  std::pair<
    typename viennagrid::result_of::point<MeshElementT>::type,
    typename viennagrid::result_of::point<MeshElementT>::type
  > bounding_box( MeshElementT const & mesh )
  {
    typedef typename viennagrid::result_of::point<MeshElementT>::type      PointType;
    typedef typename viennagrid::result_of::coord<MeshElementT>::type      NumericType;

    PointType lower_left( viennagrid::geometric_dimension(mesh) );
    PointType upper_right( viennagrid::geometric_dimension(mesh) );

    std::fill( lower_left.begin(), lower_left.end(), std::numeric_limits<NumericType>::max() );
    std::fill( upper_right.begin(), upper_right.end(), - std::numeric_limits<NumericType>::max() );
    //std::fill( upper_right.begin(), upper_right.end(), std::numeric_limits<NumericType>::lowest() );    C++11

    typedef typename viennagrid::result_of::const_vertex_range<MeshElementT>::type ConstVertexRangeType;
    typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

    ConstVertexRangeType vertices(mesh);
    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      lower_left = viennagrid::min( lower_left, viennagrid::get_point(*vit) );
      upper_right = viennagrid::max( upper_right, viennagrid::get_point(*vit) );
    }

    return std::make_pair( lower_left, upper_right );
  }

  /** @brief Calculates the size of a mesh: ||bounding_box.min - bounding_box.max||
   *
   * @param mesh              The input mesh
   */
  template<typename MeshT>
  typename viennagrid::result_of::coord<MeshT>::type mesh_size( MeshT const & mesh )
  {
    std::pair<
      typename viennagrid::result_of::point<MeshT>::type,
      typename viennagrid::result_of::point<MeshT>::type
    > bb = bounding_box(mesh);

    return viennagrid::norm_2(bb.second - bb.first);
  }

  /** @brief Makes a vector orthogonal to a set of linearly independent orthogonal vectors (Gram–Schmidt process step)
   *
   * @param it              The begin vector iterator of the linearly independent orthogonal vector set
   * @param end             The end vector iterator of the linearly independent orthogonal vector set
   * @param vec             The vector to orthogonalize
   */
  template<typename VectorIteratorT, typename VectorT>
  VectorT orthogonalize_vector( VectorIteratorT it, VectorIteratorT const & end, VectorT vec )
  {
    for (; it != end; ++it)
      vec -= viennagrid::inner_prod( vec, *it ) / viennagrid::inner_prod( *it, *it ) * (*it);

    return vec;
  }


  /** @brief Makes a set of vectors orthogonal (Gram–Schmidt process step).
   *
   * If linearly dependent vectors are encountered, they are moved/swapped to the end of the sequence.
   *
   * @param start           The begin vector iterator of the vector set to orthogonalize
   * @param end             The end vector iterator of the vector set to orthogonalize
   * @param nc              Numeric config
   *
   * @return Number of linearly independent vectors found during the process
   */
  template<typename IteratorT, typename NumericConfigT>
  std::size_t orthogonalize( IteratorT start, IteratorT end, NumericConfigT nc )
  {
    typedef typename std::iterator_traits<IteratorT>::value_type      vector_type;
    typedef typename viennagrid::result_of::coord<vector_type>::type  coord_type;

    std::size_t count = 0;
    for (IteratorT n = start; n != end;)
    {
      *n = orthogonalize_vector(start, n, *n);

      if ( viennagrid::norm_1(*n) < detail::absolute_tolerance<coord_type>(nc) )
      {
        --end;
        std::swap(*n, *end);
      }
      else
      {
        ++n;
        ++count;
      }
    }

    return count;
  }



}

#endif

