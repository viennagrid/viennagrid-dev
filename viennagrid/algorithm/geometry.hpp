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
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/algorithm/inner_prod.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/cross_prod.hpp"
#include "viennagrid/algorithm/detail/numeric.hpp"

/** @file viennagrid/algorithm/geometry.hpp
    @brief Contains various functions for computing geometric quantities
*/

namespace viennagrid
{

  namespace detail
  {
    /** @brief Implementation for calculating a normal vector of a line in 2D */
    template<typename PointAccessorT, typename ElementT>
    typename PointAccessorT::value_type normal_vector_impl(
      PointAccessorT const point_accessor,
      ElementT const & element,
      viennagrid::line_tag,
      viennagrid::dimension_tag<2>)
    {
      typedef typename PointAccessorT::value_type    PointType;

      PointType const & p0 = point_accessor( viennagrid::vertices(element)[0] );
      PointType const & p1 = point_accessor( viennagrid::vertices(element)[1] );

      PointType line = p1-p0;
      std::swap(line[0], line[1]);
      line[0] = -line[0];

      return line;
    }

    /** @brief Implementation for calculating a normal vector of a triangle in 3D */
    template<typename PointAccessorT, typename ElementT>
    typename PointAccessorT::value_type normal_vector_impl(
      PointAccessorT const point_accessor,
      ElementT const & element,
      viennagrid::triangle_tag,
      viennagrid::dimension_tag<3>)
    {
      typedef typename PointAccessorT::value_type    PointType;

      PointType const & p0 = point_accessor( viennagrid::vertices(element)[0] );
      PointType const & p1 = point_accessor( viennagrid::vertices(element)[1] );
      PointType const & p2 = point_accessor( viennagrid::vertices(element)[2] );

      return viennagrid::cross_prod( p1-p0, p2-p0 );
    }
  }



  /** @brief Calculates the normal vector of an element
   *
   * @param point_accessor          Point accessor for input points
   * @param element                 The input element
   */
  template<typename PointAccessorT, typename ElementT>
  typename PointAccessorT::value_type normal_vector(
    PointAccessorT const point_accessor,
    ElementT const & element )
  {
    typedef typename viennagrid::result_of::element_tag<ElementT>::type ElementTag;
    typedef typename PointAccessorT::value_type PointType;
    typedef viennagrid::dimension_tag< result_of::static_size<PointType>::value > DimensionTag;

    return detail::normal_vector_impl( point_accessor, element, ElementTag(), DimensionTag() );
  }

  /** @brief Calculates the normal vector of an element
   *
   * @param element                The input element
   */
  template<typename ElementT>
  typename viennagrid::result_of::point<ElementT>::type normal_vector( ElementT const & element )
  {
    return normal_vector( default_point_accessor(element), element );
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

    PointType lower_left;
    PointType upper_right;

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
  template<typename MeshT>
  std::pair<
    typename viennagrid::result_of::point<MeshT>::type,
    typename viennagrid::result_of::point<MeshT>::type
  > bounding_box( MeshT const & mesh )
  {
    typedef typename viennagrid::result_of::point<MeshT>::type      PointType;
    typedef typename viennagrid::result_of::coord<MeshT>::type      NumericType;

    PointType lower_left;
    PointType upper_right;

    std::fill( lower_left.begin(), lower_left.end(), std::numeric_limits<NumericType>::max() );
    std::fill( upper_right.begin(), upper_right.end(), - std::numeric_limits<NumericType>::max() );
    //std::fill( upper_right.begin(), upper_right.end(), std::numeric_limits<NumericType>::lowest() );    C++11

    typedef typename viennagrid::result_of::const_vertex_range<MeshT>::type ConstVertexRangeType;
    typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexIteratorType;

    ConstVertexRangeType vertices(mesh);
    for (ConstVertexIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      lower_left = viennagrid::min( lower_left, viennagrid::point(*vit) );
      upper_right = viennagrid::max( upper_right, viennagrid::point(*vit) );
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




//   template<typename IteratorT>
//   IteratorT circular_next(IteratorT it, IteratorT const & start_it, IteratorT const & end_it)
//   {
//     if (++it == end_it)
//       return start_it;
//     else
//       return it;
//   }
//
//   template<typename IteratorT>
//   IteratorT circular_prev(IteratorT it, IteratorT const & start_it, IteratorT const & end_it)
//   {
//     if (it == start_it)
//       it = end_it;
//
//     return --it;
//   }




//         template<typename point_iterator_type, typename polygon_tag_type, typename point, typename numeric_config>
//         bool is_inside( point_iterator_type const & it_start, point_iterator_type it_end, polygon_tag_type polygon_tag,
//                         point const & point_to_test,
//                         numeric_config nc)
//         {
//             typedef typename std::iterator_traits<point_iterator_type>::value_type PolygonPointType;
//             std::pair<PolygonPointType, PolygonPointType> poly_bounding_box = bounding_box(it_start, it_end);
//
//             PolygonPointType outer_point;
//             outer_point[0] = point_to_test[0];
//             outer_point[1] = poly_bounding_box.first[1] - 1;
//             bool is_inside = false;
//
//             point_iterator_type it_prev = it_end; --it_prev;
//             point_iterator_type it_cur = it_start;
//
//             for ( ;it_cur != it_end ; ++it_cur, it_prev = circular_next(it_prev, it_start, it_end) )
//             {
//                 PolygonPointType const & q0 = *it_prev;
//                 PolygonPointType const & q1 = *it_cur;
//
//                 // is inner point on polygon line?
//                 if ( point_line_intersect( point_to_test, q0, q1, interval::closed_open_tag(), nc ) )
//                     return !is_open(polygon_tag);
//
//
//                 // is current line on test line?
//                 if ( !detail::is_equal(nc, q0[0], point_to_test[0]) || !detail::is_equal(nc, q1[0], point_to_test[0]) )
//                 {
//                     if ( line_line_intersect( q0, q1, interval::open_open_tag(), point_to_test, outer_point, interval::open_open_tag(), nc ) )
//                     {
//                         is_inside = !is_inside;
//                     }
//                 }
//             }
//
//
//             // find point which is not on the testing line
//             point_iterator_type it = it_start;
//             while ( point_line_intersect( *it, point_to_test, outer_point, interval::open_open_tag(), nc ) )
//             {
//                 it = circular_prev(it, it_start, it_end);
//                 if (it == it_start)
//                     break;
//             }
//
//             // no point found -> no intersection
//             if ( point_line_intersect( *it, point_to_test, outer_point, interval::open_open_tag(), nc ) )
//                 return false;
//
//
//             point_iterator_type circular_start_it = it;
//             it_prev = it;
//             it = circular_next(it, it_start, it_end);
//
//             // iterating over all points
//             while (it != circular_start_it)
//             {
//                 // is point on testing line?
//                 if ( point_line_intersect( *it, point_to_test, outer_point, interval::open_open_tag(), nc ) )
//                 {
//                     // find next point which is not on testing line
//                     point_iterator_type it_next = circular_next(it, it_start, it_end);
//                     while ( point_line_intersect( *it_next, point_to_test, outer_point, interval::open_open_tag(), nc ) )
//                         it_next = circular_next(it_next, it_start, it_end);
//
//                     // check if the the lines/points are an ear
//                     if ( ((*it_prev)[0] - (*it)[0]) * ((*it_next)[0] - (*it)[0]) < 0 )
//                     {
//                         is_inside = !is_inside;
//                     }
//
//                     it_prev = it;
//                     it = it_next;
//                 }
//                 else
//                 {
//                     it_prev = it;
//                     it = circular_next(it, it_start, it_end);
//                 }
//             }
//
//             return is_inside;
//         }




  /** @brief Makes a vector orthogonal to a set of orthogonal vectors (Gram–Schmidt process step)
   *
   * @param it              The begin vector iterator of the orthogonal vector set
   * @param end             The end vector iterator of the orthogonal vector set
   * @param vec             The vector to orthogonalize
   */
  template<typename VectorIteratorT, typename VectorT>
  VectorT orthogonalize_one_vector( VectorIteratorT it, VectorIteratorT const & end, VectorT vec )
  {
    for (; it != end; ++it)
      vec -= viennagrid::inner_prod( vec, *it ) / viennagrid::inner_prod( *it, *it ) * (*it);

    return vec;
  }


  /** @brief Makes a set of vectors orthogonal (Gram–Schmidt process step)
   *
   * @param start           The begin vector iterator of the vector set to orthogonalize
   * @param end             The end vector iterator of the vector set to orthogonalize
   * @param nc              Numeric config
   */
  template<typename IteratorT, typename NumericConfigT>
  bool orthogonalize( IteratorT start, IteratorT end, NumericConfigT nc )
  {
    typedef typename std::iterator_traits<IteratorT>::value_type      vector_type;
    typedef typename viennagrid::result_of::coord<vector_type>::type  coord_type;

    for (IteratorT n = start; n != end; ++n)
    {
      *n = orthogonalize_one_vector(start, n, *n);

      if ( viennagrid::norm_1(*n) < detail::absolute_tolerance<coord_type>(nc) )
        return false;
    }

    return true;
  }


  /** @brief Class for projecting 3D points of a plane to 2D
   *
   * @tparam Point3DT                 3D point type
   */
  template<typename Point3DT>
  class plane_to_2d_projector
  {
  public:

    /** @brief Initializes the projection for a set of points in 3D which lay on a common plane. Returns true on success, false otherwise
    *
    * @param start                       The begin 3D point iterator
    * @param end                         The end 3D point iterator
    * @param nc                          Numeric config
    */
    template<typename PointIteratorT, typename NumericConfigT>
    bool init(PointIteratorT const & start, PointIteratorT const & end, NumericConfigT nc)
    {
      center = Point3DT();
      matrix[0] = Point3DT();
      matrix[1] = Point3DT();

      typedef typename std::iterator_traits<PointIteratorT>::value_type                    point_type;
      typedef typename viennagrid::result_of::coord<point_type>::type                      coord_type;
      typedef typename detail::result_of::numeric_type<NumericConfigT, coord_type>::type  numeric_type;

      if (start == end) return false; // too few points

      PointIteratorT pit;

      // calculating the center
      pit = start;
      unsigned int num_points = 1;
      center = *pit; ++pit;

      for (; pit != end; ++pit)
      {
        center += *pit;
        ++num_points;
      }

      if (num_points < 3) return false; // too few points
      center /= num_points;


      // setting up a map of vectors from the center to all points, sorted descending by the length of that vector
      typedef std::multimap<numeric_type, point_type, std::greater<numeric_type> > vector_map_type;
      vector_map_type sorted_vectors;
      pit = start;
      for (; pit != end; ++pit)
      {
        point_type vector = center - *pit;
        typename vector_map_type::iterator it = sorted_vectors.insert( std::make_pair( viennagrid::norm_2( vector ), vector ) );
        it->second /= it->first; // normalizing the vector
      }


      // finding 2 non-liner dependent vectors
      unsigned int projection_matrix_index = 0;
      typename vector_map_type::iterator it = sorted_vectors.begin();
      while (projection_matrix_index < 2)
      {
        if ( it->first < detail::absolute_tolerance<coord_type>(nc) )
          return false; // points are too close together

        // check linear dependency with other vectors in projection matrix
        unsigned int index = 0;
        unsigned int pm_index = 0;
        for (; index < projection_matrix_index; ++index, ++pm_index)
        {
          numeric_type angle_cos = viennagrid::inner_prod( it->second, matrix[pm_index] );
          if ( std::abs(angle_cos) > 1 - detail::absolute_tolerance<coord_type>(nc))
            break;
        }

        if ( index == projection_matrix_index)
        {
          matrix[pm_index] = it->second;
          ++projection_matrix_index;
        }

        ++it;
      }

      // orthogonalize vectors
      orthogonalize( matrix.begin(), matrix.end(), nc );

      // normalize vectors
      matrix[0] /= viennagrid::norm_2(matrix[0]);
      matrix[1] /= viennagrid::norm_2(matrix[1]);

      return true;
    }

    /** @brief Projects a single 3D points to 2D, returns the projected point
    *
    * @tparam Point2DT       2D point type
    * @param pt3d            The 3D point
    * @return                The projected point in 2D
    */
    template<typename Point2DT>
    Point2DT project(Point3DT const & pt3d) const
    {
      Point3DT tmp = pt3d - center;

      Point2DT out;
      out[0] = viennagrid::inner_prod( tmp, matrix[0] );
      out[1] = viennagrid::inner_prod( tmp, matrix[1] );

      return out;
    }

    /** @brief Projects a iterator range of 3D points to 2D
    *
    * @param start                       The begin 3D point iterator
    * @param end                         The end 3D point iterator
    * @param out                         The begin output 2D point iterator
    */
    template<typename PointIteratorT, typename OutPointIteratorT>
    void project(PointIteratorT start, const PointIteratorT & end,
                OutPointIteratorT out)
    {
      typedef typename std::iterator_traits<OutPointIteratorT>::value_type Point2DType;

      for ( ; start != end; ++start, ++out)
        *out = project<Point2DType>(*start);
    }



    /** @brief Performs an inverse projection of a point in 2D into the 3D plane
    *
    * @param pt2d            The 2D point
    * @return                The same point in the 3D plane
    */
    template<typename Point2DT>
    Point3DT unproject(Point2DT const & pt2d) const
    {
      return matrix[0] * pt2d[0] + matrix[1] * pt2d[1] + center;
    }

    /** @brief Performs an inverse projection on an iterator range of 2D points to the 3D plane
    *
    * @param start                       The begin 2D point iterator
    * @param end                         The end 2D point iterator
    * @param out                         The begin output 3D point iterator
    */
    template<typename PointIteratorT, typename OutPointIteratorT>
    void unproject(PointIteratorT start, const PointIteratorT & end,
                  OutPointIteratorT out)
    {
      typedef typename std::iterator_traits<OutPointIteratorT>::value_type Point2DType;

      for ( ; start != end; ++start, ++out)
        *out = unproject(*start);
    }

  private:
    Point3DT center;
    viennagrid::static_array<Point3DT, 2> matrix;
  };
}

#endif

