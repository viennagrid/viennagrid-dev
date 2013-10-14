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

namespace viennagrid
{
  namespace detail
  {
    /** @brief For internal use only. */
    template<typename MeshType, typename ScalarType, typename PointAccessorType>
    void scale_impl(MeshType& mesh_obj, ScalarType factor, PointAccessorType accessor)
    {
      typedef typename viennagrid::result_of::element<MeshType, viennagrid::vertex_tag>::type         VertexType;
      typedef typename viennagrid::result_of::element_range<MeshType, viennagrid::vertex_tag>::type   VertexContainer;
      typedef typename viennagrid::result_of::iterator<VertexContainer>::type                           VertexIterator;

      VertexContainer vertices = viennagrid::elements<VertexType>(mesh_obj);
      for ( VertexIterator vit = vertices.begin();
            vit != vertices.end();
            ++vit )
      {
        accessor(*vit) *= factor;
      }
    }
  } // detail


  /** @brief Scales the geometric points of a mesh/segment
   *
   * @param  mesh_or_segment          The mesh/segment which points are scaled
   * @param  factor                     The scale factor
   */
  template<typename MeshOrSegmentHandleT, typename ScalarType>
  void scale(MeshOrSegmentHandleT & mesh_or_segment, ScalarType factor)
  {
    viennagrid::detail::scale_impl(mesh_or_segment, factor, viennagrid::default_point_accessor(mesh_or_segment));
  }

} // viennagrid

#endif
