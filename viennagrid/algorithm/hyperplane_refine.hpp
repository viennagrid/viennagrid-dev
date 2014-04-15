#ifndef VIENNAGRID_ALGORITHM_HYPERPLANE_REFINE_HPP
#define VIENNAGRID_ALGORITHM_HYPERPLANE_REFINE_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/algorithm/detail/numeric.hpp"
#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/refine.hpp"

namespace viennagrid
{
  namespace detail
  {
    /** @brief For internal use only. */
    template<typename SrcMeshT, typename DstMeshT, typename PointT, typename LineRefinementTagContainerT, typename LineRefinementVertexHandleContainerT, typename NumericConfigT>
    void mark_edges_for_hyperplane_refine(SrcMeshT const & src_mesh, DstMeshT & dst_mesh,
                              PointT const & hyperplane_point, PointT const & hyperplane_normal,
                              LineRefinementTagContainerT & line_refinement_tag_accessor,
                              LineRefinementVertexHandleContainerT & line_refinement_vertex_handle_accessor,
                              NumericConfigT numeric_config)
    {
      typedef typename viennagrid::result_of::coord<SrcMeshT>::type NumericType;
      typedef typename viennagrid::result_of::const_line_range<SrcMeshT>::type ConstLineRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;

      ConstLineRangeType lines(src_mesh);
      for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
      {
        NumericType distance0 = viennagrid::inner_prod( hyperplane_normal, viennagrid::point( viennagrid::vertices(*lit)[0] )-hyperplane_point );
        NumericType distance1 = viennagrid::inner_prod( hyperplane_normal, viennagrid::point( viennagrid::vertices(*lit)[1] )-hyperplane_point );

        NumericType tolerance = viennagrid::detail::relative_tolerance(numeric_config, viennagrid::volume(*lit));

        if (distance0 > distance1)
          std::swap(distance0, distance1);

        if (distance0 < -tolerance && distance1 > tolerance)
        {
          line_refinement_tag_accessor(*lit) = true;

          PointT const & pt0 = viennagrid::point( viennagrid::vertices(*lit)[0] );
          PointT const & pt1 = viennagrid::point( viennagrid::vertices(*lit)[1] );

          double qd = viennagrid::inner_prod( hyperplane_normal, pt0-hyperplane_point );
          double pd = viennagrid::inner_prod( hyperplane_normal, pt1-hyperplane_point );

          PointT new_pt = pt1 - (pd / (pd-qd)) * (pt1 - pt0);
          line_refinement_vertex_handle_accessor(*lit) = viennagrid::make_unique_vertex( dst_mesh, new_pt );
        }
        else
          line_refinement_tag_accessor(*lit) = false;
      }
    }
  }



  /** @brief Refines a mesh and a segmentation based on a hyperplane. All elements which intersects the hyperplane are refined in a way that they don't intersect the hyperplane afterwards.
   *
   * @param src_mesh            The input mesh
   * @param src_segmentation    The input segmentation
   * @param dst_mesh            The output mesh
   * @param dst_segmentation    The output segmentation
   * @param hyperplane_point    A point representing the hyperplane
   * @param hyperplane_normal   The normale vector representing the hyperplane
   * @param numeric_config      The numeric config
   */
  template<typename SrcMeshT, typename SrcSegmentationT, typename DstMeshT, typename DstSegmentationT, typename PointT, typename NumericConfigT>
  void hyperplane_refine(SrcMeshT const & src_mesh, SrcSegmentationT const & src_segmentation,
                         DstMeshT & dst_mesh, DstSegmentationT & dst_segmentation,
                         PointT const & hyperplane_point, PointT const & hyperplane_normal,
                         NumericConfigT numeric_config)
  {
    typedef typename viennagrid::result_of::cell_tag<SrcMeshT>::type CellTag;
    typedef typename viennagrid::result_of::vertex<SrcMeshT>::type VertexType;
    typedef typename viennagrid::result_of::line<SrcMeshT>::type LineType;
    typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type DstMeshVertexHandleType;
    typedef typename viennagrid::result_of::point<SrcMeshT>::type PointType;

    viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map( dst_mesh );

    std::deque<bool> line_refinement_tag_container;
    typename viennagrid::result_of::accessor<std::deque<bool>, LineType>::type line_refinement_tag_accessor(line_refinement_tag_container);

    std::deque<DstMeshVertexHandleType> line_refinement_vertex_handle_container;
    typename viennagrid::result_of::accessor<std::deque<DstMeshVertexHandleType>, LineType>::type line_refinement_vertex_handle_accessor(line_refinement_vertex_handle_container);

    detail::mark_edges_for_hyperplane_refine(src_mesh, dst_mesh, hyperplane_point, hyperplane_normal, line_refinement_tag_accessor, line_refinement_vertex_handle_accessor, numeric_config);


    viennagrid::simple_refine<CellTag>(src_mesh, src_segmentation,
                                       dst_mesh, dst_segmentation,
                                       vertex_map,
                                       line_refinement_tag_accessor,
                                       line_refinement_vertex_handle_accessor);
  }



  /** @brief Refines a mesh and a segmentation based on a hyperplane. All elements which intersects the hyperplane are refined in a way that they don't intersect the hyperplane afterwards.
   *
   * @param src_mesh            The input mesh
   * @param dst_mesh            The output mesh
   * @param hyperplane_point    A point representing the hyperplane
   * @param hyperplane_normal   The normale vector representing the hyperplane
   * @param numeric_config      The numeric config
   */
  template<typename SrcMeshT, typename DstMeshT, typename PointT, typename NumericConfigT>
  void hyperplane_refine(SrcMeshT const & src_mesh, DstMeshT & dst_mesh,
                         PointT const & hyperplane_point, PointT const & hyperplane_normal,
                         NumericConfigT numeric_config)
  {
    typedef typename viennagrid::result_of::cell_tag<SrcMeshT>::type CellTag;
    typedef typename viennagrid::result_of::vertex<SrcMeshT>::type VertexType;
    typedef typename viennagrid::result_of::line<SrcMeshT>::type LineType;

    typedef typename viennagrid::result_of::vertex_handle<DstMeshT>::type DstMeshVertexHandleType;

    viennagrid::vertex_copy_map<SrcMeshT, DstMeshT> vertex_map( dst_mesh );

    std::deque<bool> line_refinement_tag_container;
    typename viennagrid::result_of::accessor<std::deque<bool>, LineType>::type line_refinement_tag_accessor(line_refinement_tag_container);

    std::deque<DstMeshVertexHandleType> line_refinement_vertex_handle_container;
    typename viennagrid::result_of::accessor<std::deque<DstMeshVertexHandleType>, LineType>::type line_refinement_vertex_handle_accessor(line_refinement_vertex_handle_container);

    detail::mark_edges_for_hyperplane_refine(src_mesh, dst_mesh, hyperplane_point, hyperplane_normal, line_refinement_tag_accessor, line_refinement_vertex_handle_accessor, numeric_config);

    viennagrid::simple_refine<CellTag>(src_mesh, dst_mesh,
                                       vertex_map,
                                       line_refinement_tag_accessor,
                                       line_refinement_vertex_handle_accessor);
  }
}


#endif
