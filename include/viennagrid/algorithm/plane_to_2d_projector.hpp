#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_PLANE_TO_2D_PROJECTOR_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_PLANE_TO_2D_PROJECTOR_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include <boost/array.hpp>
#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/algorithm/geometry.hpp"
#include "viennagrid/algorithm/detail/numeric.hpp"

namespace viennagrid
{

  /** @brief Class for projecting 3D points of a plane to 2D
   *
   * @tparam Point3DT                 3D point type
   */
  template<typename PointT>
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
      center = PointT(3);
      matrix[0] = PointT(3);
      matrix[1] = PointT(3);

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
    PointT project(PointT const & pt3d) const
    {
      PointT tmp = pt3d - center;

      PointT out(2);
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
//       typedef typename std::iterator_traits<OutPointIteratorT>::value_type Point2DType;

      for ( ; start != end; ++start, ++out)
        *out = project(*start);
    }



    /** @brief Performs an inverse projection of a point in 2D into the 3D plane
    *
    * @param pt2d            The 2D point
    * @return                The same point in the 3D plane
    */
    PointT unproject(PointT const & pt2d) const
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
      for ( ; start != end; ++start, ++out)
        *out = unproject(*start);
    }

  private:
    PointT center;
    boost::array<PointT, 2> matrix;
  };

}

#endif
