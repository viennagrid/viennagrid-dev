#ifndef VIENNAGRID_ALGORITHM_SCALE_HPP
#define VIENNAGRID_ALGORITHM_SCALE_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

/** @file viennagrid/algorithm/scale.hpp
    @brief Scale the geometric points of a mesh/segment
*/

#include "viennagrid/forwards.hpp"
#include "viennagrid/point.hpp"
#include "viennagrid/accessor.hpp"

namespace viennagrid
{
  namespace detail
  {
    /** @brief For internal use only. */
    template<typename MeshT, typename ScalarT, typename PointType, typename PointAccessorT>
    void scale_impl(MeshT& mesh_obj, ScalarT factor, PointType scale_center, PointAccessorT accessor)
    {
      typedef typename viennagrid::result_of::element<MeshT, viennagrid::vertex_tag>::type         VertexType;
      typedef typename viennagrid::result_of::element_range<MeshT, viennagrid::vertex_tag>::type   VertexContainer;
      typedef typename viennagrid::result_of::iterator<VertexContainer>::type                           VertexIterator;

      VertexContainer vertices = viennagrid::elements<VertexType>(mesh_obj);
      for ( VertexIterator vit = vertices.begin();
            vit != vertices.end();
            ++vit )
      {
        PointType translated_point = accessor(*vit) - scale_center;
        translated_point *= factor;
        translated_point += scale_center;
        accessor(*vit) = translated_point;
      }
    }
  } // detail


  /** @brief Scales the geometric points of a mesh.
   *
   * @param  mesh_obj      The mesh which points are scaled
   * @param  factor        The scale factor
   */
  template<typename MeshConfigType, typename ScalarT>
  void scale(viennagrid::mesh<MeshConfigType> & mesh_obj, ScalarT factor)
  {
    typedef viennagrid::mesh<MeshConfigType>   MeshType;
    typedef typename viennagrid::result_of::point<MeshType>::type   PointType;
    viennagrid::detail::scale_impl(mesh_obj, factor, PointType(0,0), viennagrid::default_point_accessor(mesh_obj));
  }

  /** @brief Scales the geometric points of a mesh/segment
   *
   * @param  mesh_obj          The mesh/segment which points are scaled
   * @param  factor            The scale factor
   * @param  scaling_center    Point c which is used as the center of contraction/expansion. The vectors (p - c) for each point p in the mesh then gets scaled by the factor.
   */
  template<typename MeshConfigType, typename ScalarT, typename PointType>
  void scale(viennagrid::mesh<MeshConfigType> & mesh_obj, ScalarT factor, PointType const & scaling_center)
  {
    viennagrid::detail::scale_impl(mesh_obj, factor, scaling_center, viennagrid::default_point_accessor(mesh_obj));
  }

} // viennagrid

#endif
