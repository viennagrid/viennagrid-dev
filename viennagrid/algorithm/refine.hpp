#ifndef VIENNAGRID_ALGORITHM_REFINE_HPP
#define VIENNAGRID_ALGORITHM_REFINE_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/forwards.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/norm.hpp"

#include "viennagrid/mesh/element_creation.hpp"

#include "viennagrid/algorithm/detail/refine_tri.hpp"
#include "viennagrid/algorithm/detail/refine_tet.hpp"

/** @file viennagrid/algorithm/refine.hpp
    @brief Provides the routines for a refinement of a mesh
*/

namespace viennagrid
{
  namespace detail
  {
    /** @brief For internal use only */
    template <typename CellTagInT,
              typename WrappedMeshConfigInT,
              typename WrappedMeshConfigOutT,
              typename PointAccessorT,
              typename EdgeRefinementFlagAccessorT, typename VertexToVertexHandleAccessorT, typename RefinementVertexAccessorT>
    void refine_impl(mesh<WrappedMeshConfigInT> const & mesh_in,
                     mesh<WrappedMeshConfigOutT> & mesh_out,
                     PointAccessorT const point_accessor_in,
                     EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor,
                     VertexToVertexHandleAccessorT vertex_to_vertex_handle_accessor,
                     RefinementVertexAccessorT edge_to_vertex_handle_accessor)
    {
      typedef mesh<WrappedMeshConfigInT>       WrappedMeshConfigInType;

      typedef typename viennagrid::result_of::const_element_range<WrappedMeshConfigInType, vertex_tag>::type       VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                               VertexIterator;
      typedef typename viennagrid::result_of::const_element_range<WrappedMeshConfigInType, line_tag>::type         EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                 EdgeIterator;
      typedef typename viennagrid::result_of::const_element_range<WrappedMeshConfigInType, CellTagInT>::type       CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                 CellIterator;

      //
      // Step 2: Write old vertices to new mesh
      //
      VertexRange vertices = viennagrid::elements(mesh_in);
      for (VertexIterator vit  = vertices.begin();
                          vit != vertices.end();
                        ++vit)
      {
        vertex_to_vertex_handle_accessor( *vit ) = viennagrid::make_vertex( mesh_out, point_accessor_in(*vit) );
      }

      //
      // Step 3: Each tagged edge in old mesh results in a new vertex (temporarily store new vertex IDs on old mesh)
      //
      EdgeRange edges = viennagrid::elements(mesh_in);
      for (EdgeIterator eit = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        if ( edge_refinement_flag_accessor(*eit) )
        {
          edge_to_vertex_handle_accessor( *eit ) = viennagrid::make_vertex( mesh_out, viennagrid::centroid(point_accessor_in, *eit) );
        }
      }

      //
      // Step 4: Now write new cells to new mesh
      //
        CellRange cells = viennagrid::elements(mesh_in);
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
        {
          detail::element_refinement<CellTagInT>::apply(*cit, mesh_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
        }
    }




    /** @brief For internal use only */
    template <typename CellTagInT,
              typename WrappedMeshConfigInT,  typename WrappedSegmentationConfigInT,
              typename WrappedMeshConfigOutT, typename WrappedSegmentationConfigOutT,
              typename PointAccessorT,
              typename EdgeRefinementFlagAccessorT, typename VertexToVertexHandleAccessorT, typename RefinementVertexAccessorT>
    void refine_impl(mesh<WrappedMeshConfigInT> const & mesh_in,    segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
                     mesh<WrappedMeshConfigOutT>      & mesh_out,   segmentation<WrappedSegmentationConfigOutT>      & segmentation_out,
                     PointAccessorT const point_accessor_in,
                     EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor,
                     VertexToVertexHandleAccessorT vertex_to_vertex_handle_accessor,
                     RefinementVertexAccessorT edge_to_vertex_handle_accessor)
    {
      typedef mesh<WrappedMeshConfigInT>    WrappedMeshConfigInType;

      typedef segmentation<WrappedSegmentationConfigInT>      WrappedSegmentationConfigInType;
      typedef segmentation<WrappedSegmentationConfigOutT>     WrappedSegmentationConfigOutType;

      typedef typename viennagrid::result_of::segment_handle<WrappedSegmentationConfigInType>::type     SegmentHandleInType;
      typedef typename viennagrid::result_of::segment_handle<WrappedSegmentationConfigOutType>::type    SegmentHandleOutType;

      typedef typename viennagrid::result_of::const_element_range<WrappedMeshConfigInType, vertex_tag>::type     VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                             VertexIterator;
      typedef typename viennagrid::result_of::const_element_range<WrappedMeshConfigInType, line_tag>::type       EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                               EdgeIterator;
      typedef typename viennagrid::result_of::const_element_range<SegmentHandleInType, CellTagInT>::type    CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                               CellIterator;

      //
      // Step 2: Write old vertices to new mesh
      //
      VertexRange vertices = viennagrid::elements(mesh_in);
      for (VertexIterator vit  = vertices.begin();
                          vit != vertices.end();
                        ++vit)
      {
        vertex_to_vertex_handle_accessor( *vit ) = viennagrid::make_vertex( mesh_out, point_accessor_in(*vit) );
      }

      //
      // Step 3: Each tagged edge in old mesh results in a new vertex (temporarily store new vertex IDs on old mesh)
      //
      EdgeRange edges = viennagrid::elements(mesh_in);
      for (EdgeIterator eit = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        if ( edge_refinement_flag_accessor(*eit) )
        {
          edge_to_vertex_handle_accessor( *eit ) = viennagrid::make_vertex( mesh_out, viennagrid::centroid(point_accessor_in, *eit) );
        }
      }

      //
      // Step 4: Now write new cells to new mesh
      //

      // TODO: fix overlapping segments (with this implementation overlapping segments would result in multiple elements)
      for (typename WrappedSegmentationConfigInType::const_iterator sit = segmentation_in.begin(); sit != segmentation_in.end(); ++sit)
      {
        SegmentHandleInType  const & segment_in  = *sit;
        SegmentHandleOutType       & segment_out = segmentation_out( sit->id() );

        CellRange cells = viennagrid::elements(segment_in);
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
        {
          detail::element_refinement<CellTagInT>::apply(*cit, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
        }
      }
    }


  } //namespace detail



    /** @brief Ensures refinement fo the longest edge. If any edge is refined in a cell, then the longest edge is refined as well. */
    template <typename CellTagIn, typename WrappedMeshConfigInT, typename EdgeRefinementFlagAccessorT>
    void ensure_longest_edge_refinement(mesh<WrappedMeshConfigInT> const & mesh_in, EdgeRefinementFlagAccessorT edge_refinement_flag_accessor)
    {
      typedef mesh<WrappedMeshConfigInT>                                            MeshInType;

      typedef typename viennagrid::result_of::element<MeshInType, vertex_tag>::type     VertexType;
      typedef typename viennagrid::result_of::element<MeshInType, line_tag>::type       EdgeType;
      typedef typename viennagrid::result_of::element<MeshInType, CellTagIn>::type      CellType;

      typedef typename viennagrid::result_of::const_element_range<MeshInType, CellTagIn>::type    CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                             CellIterator;
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type         EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                       EdgeOnCellIterator;
      typedef typename viennagrid::result_of::const_element_range<EdgeType, vertex_tag>::type       VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                     VertexOnEdgeIterator;


      bool something_changed = true;
      //std::size_t iter_cnt = 0;

      while (something_changed)
      {
        something_changed = false;

        CellRange cells = viennagrid::elements<CellTagIn>(mesh_in);
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
        {
          //
          // check if any edges are tagged for refinement
          //
          bool has_refinement = false;
          EdgeOnCellRange edges_on_cell = viennagrid::elements<line_tag>(*cit);
          for (EdgeOnCellIterator eocit = edges_on_cell.begin();
                                  eocit != edges_on_cell.end();
                                ++eocit)
          {
            if ( edge_refinement_flag_accessor(*eocit) )
            {
              has_refinement = true;
              break;
            }
          }

          if (has_refinement)
          {
            //
            // Find longest edge
            //
            EdgeType const * longest_edge_ptr = NULL;
            double longest_edge_len = 0;

            for (EdgeOnCellIterator eocit = edges_on_cell.begin();
                                    eocit != edges_on_cell.end();
                                  ++eocit)
            {
              VertexOnEdgeRange vertices_on_edge = viennagrid::elements<vertex_tag>(*eocit);
              VertexOnEdgeIterator voeit = vertices_on_edge.begin();
              VertexType const & v0 = *voeit; ++voeit;
              VertexType const & v1 = *voeit;

              double len = viennagrid::norm( viennagrid::point(mesh_in, v0) - viennagrid::point(mesh_in, v1) );
              if (len > longest_edge_len)
              {
                longest_edge_len = len;
                longest_edge_ptr = &(*eocit);
              }
            }

            //tag longest edge:
            if ( !edge_refinement_flag_accessor(*longest_edge_ptr) )
            {
              edge_refinement_flag_accessor(*longest_edge_ptr) = true;
              something_changed = true;
            }
          }

        } //for cells


      } //while
    } //ensure_longest_edge_refinement

    /** @brief Transfers tags for refinement from the cell to edges */
    template <typename CellTagIn, typename WrappedMeshConfigInT, typename CellRefinementFlagAccessorT, typename EdgeRefinementFlagAccessorT>
    void cell_refinement_to_edge_refinement(mesh<WrappedMeshConfigInT> const & mesh_in,
                                            CellRefinementFlagAccessorT cell_refinement_flag, EdgeRefinementFlagAccessorT edge_refinement_flag_accessor)
    {
      typedef mesh<WrappedMeshConfigInT>                                                        MeshInType;

      typedef typename viennagrid::result_of::element<MeshInType, CellTagIn>::type                  CellType;

      typedef typename viennagrid::result_of::const_element_range<MeshInType, CellTagIn>::type      CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                               CellIterator;
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type           EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                         EdgeOnCellIterator;

      std::size_t cells_for_refinement = 0;
      CellRange cells = viennagrid::elements(mesh_in);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        if ( cell_refinement_flag(*cit) )
        {
          ++cells_for_refinement;

          EdgeOnCellRange edges_on_cell = viennagrid::elements(*cit);
          for (EdgeOnCellIterator eocit = edges_on_cell.begin();
                                  eocit != edges_on_cell.end();
                                ++eocit)
          {
            edge_refinement_flag_accessor(*eocit) = true;
          }
        }
      }

    }


    /** @brief Mark all edges for refinement -> unform refinement */
    template<typename WrappedMeshConfigInT, typename EdgeRefinementFlagAccessorT>
    void mark_all_edge_refinement(mesh<WrappedMeshConfigInT> const & mesh_in, EdgeRefinementFlagAccessorT edge_refinement_flag_accessor)
    {
      typedef mesh<WrappedMeshConfigInT>                                                                MeshInType;

      typedef typename viennagrid::result_of::const_element_range<MeshInType, viennagrid::line_tag>::type   LineRange;
      typedef typename viennagrid::result_of::iterator<LineRange>::type                                       LineIterator;

      LineRange lines = viennagrid::elements(mesh_in);
      for (LineIterator it = lines.begin(); it != lines.end(); ++it)
        edge_refinement_flag_accessor(*it) = true;
    }










  /** @brief Public interface for refinement of a mesh with explicit point accessor, edge refinement accessor and temporary accessors
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   * @param vertex_to_vertex_handle_accessor  Temporary accessor for vertex to vertex mapping
   * @param edge_to_vertex_handle_accessor    Temporary accessor for refined edge to vertex mapping
   */
  template <typename ElementTypeOrTagT, typename WrappedMeshConfigInT, typename WrappedMeshConfigOutT, typename PointAccessorType,
            typename EdgeRefinementFlagAccessorT, typename VertexToVertexHandleAccessor, typename RefinementVertexAccessor>
  void refine(mesh<WrappedMeshConfigInT> const & mesh_in,
              mesh<WrappedMeshConfigOutT> & mesh_out, PointAccessorType point_accessor_in,
              EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor,
              VertexToVertexHandleAccessor vertex_to_vertex_handle_accessor, RefinementVertexAccessor edge_to_vertex_handle_accessor)
  {
      typedef typename viennagrid::result_of::element_tag<ElementTypeOrTagT>::type CellTag;

      detail::refine_impl<CellTag>(mesh_in, mesh_out, point_accessor_in,
                                  edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
  }


  /** @brief Public interface for refinement of a mesh with explicit point accessor and edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedMeshConfigInT, typename WrappedMeshConfigOutT, typename PointAccessorType, typename EdgeRefinementFlagAccessorT>
  void refine(mesh<WrappedMeshConfigInT> const & mesh_in,
              mesh<WrappedMeshConfigOutT> & mesh_out,
              PointAccessorType point_accessor_in, EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor)
  {
    typedef mesh<WrappedMeshConfigOutT>                                             MeshOutType;

    typedef typename viennagrid::result_of::vertex<MeshOutType>::type                   VertexType;
    typedef typename viennagrid::result_of::line<MeshOutType>::type                     EdgeType;

    typedef typename viennagrid::result_of::vertex_handle<MeshOutType>::type            VertexHandleType;

    std::deque<VertexHandleType> vertex_refinement_vertex_handle;
    std::deque<VertexHandleType> edge_refinement_vertex_handle;

    refine<ElementTypeOrTagT>(mesh_in, mesh_out, point_accessor_in,
                    edge_refinement_flag_accessor,
                    viennagrid::make_accessor<VertexType>(vertex_refinement_vertex_handle),
                    viennagrid::make_accessor<EdgeType>(edge_refinement_vertex_handle));
  }

  /** @brief Public interface for refinement of a mesh with edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedMeshConfigInT, typename WrappedMeshConfigOutT, typename EdgeRefinementFlagAccessorT>
  void refine(mesh<WrappedMeshConfigInT> const & mesh_in,
              mesh<WrappedMeshConfigOutT> & mesh_out,
              EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor)
  {
    typedef mesh<WrappedMeshConfigOutT>                                             MeshOutType;

    typedef typename viennagrid::result_of::vertex_handle<MeshOutType>::type            VertexHandleType;

    std::deque<VertexHandleType> vertex_refinement_vertex_handle;
    std::deque<VertexHandleType> edge_refinement_vertex_handle;

    refine<ElementTypeOrTagT>(mesh_in, mesh_out, default_point_accessor(mesh_in), edge_refinement_flag_accessor);
  }



  /** @brief Public interface for refinement of a mesh with explicit point accessor and cell refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param point_accessor_in                 Point accessor for input points
   * @param cell_refinement_flag_accessor     Accessor storing flags if a cell is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedMeshConfigInT, typename WrappedMeshConfigOutT, typename PointAccessorType, typename CellRefinementFlagAccessorT>
  void element_refine(mesh<WrappedMeshConfigInT> const & mesh_in,
                      mesh<WrappedMeshConfigOutT> & mesh_out,
                      PointAccessorType point_accessor_in, CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    typedef mesh<WrappedMeshConfigOutT>                                             MeshOutType;

    typedef typename viennagrid::result_of::line<MeshOutType>::type                     EdgeType;

    std::deque<bool> edge_refinement_flag;

    edge_refinement_flag.resize( viennagrid::id_upper_bound<EdgeType>(mesh_in).get() );

    cell_refinement_to_edge_refinement<ElementTypeOrTagT>( mesh_in,
                                        cell_refinement_flag_accessor,
                                        viennagrid::make_accessor<EdgeType>(edge_refinement_flag));

    refine<ElementTypeOrTagT>(mesh_in, mesh_out, point_accessor_in,
                    viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  /** @brief Public interface for refinement of a mesh with cell refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param cell_refinement_flag_accessor     Accessor storing flags if a cell is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedMeshConfigInT, typename WrappedMeshConfigOutT, typename CellRefinementFlagAccessorT>
  void element_refine(mesh<WrappedMeshConfigInT> const & mesh_in,
                      mesh<WrappedMeshConfigOutT> & mesh_out,
                      CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    element_refine<ElementTypeOrTagT>(mesh_in, mesh_out, default_point_accessor(mesh_in), cell_refinement_flag_accessor);
  }

  /** @brief Public interface for refinement of cells of a mesh with cell refinement accessor. If there is more than one cell type this funcion will fail.
   *
   * @param mesh_in                         Input mesh
   * @param mesh_out                        Output refined mesh
   * @param cell_refinement_flag_accessor     Accessor storing flags if a cell is marked for refinement
   */
  template <typename WrappedMeshConfigInT, typename WrappedMeshConfigOutT, typename CellRefinementFlagAccessorT>
  void cell_refine(mesh<WrappedMeshConfigInT> const & mesh_in,
                   mesh<WrappedMeshConfigOutT> & mesh_out,
                   CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    typedef mesh<WrappedMeshConfigInT>                            MeshInType;
    typedef typename viennagrid::result_of::cell<MeshInType>::type    CellType;

    element_refine<CellType>(mesh_in, mesh_out, default_point_accessor(mesh_in), cell_refinement_flag_accessor);
  }




  /** @brief Public interface for uniform refinement of a mesh with explicit point accessor, edge refinement accessor and temporary accessors.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   * @param vertex_to_vertex_handle_accessor  Temporary accessor for vertex to vertex mapping
   * @param edge_to_vertex_handle_accessor    Temporary accessor for refined edge to vertex mapping
   */
  template <typename ElementTypeOrTagT, typename WrappedMeshConfigInT, typename WrappedMeshConfigOutT, typename PointAccessorType,
            typename EdgeRefinementFlagAccessorT,  typename VertexToVertexHandleAccessor, typename RefinementVertexAccessor>
  void refine_uniformly(mesh<WrappedMeshConfigInT> const & mesh_in,
                        mesh<WrappedMeshConfigOutT> & mesh_out,
                        PointAccessorType point_accessor_in,
                        EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor,
                        VertexToVertexHandleAccessor vertex_to_vertex_handle_accessor, RefinementVertexAccessor edge_to_vertex_handle_accessor)
  {
    mark_all_edge_refinement( mesh_in, edge_refinement_flag_accessor );
    refine<ElementTypeOrTagT>(mesh_in, mesh_out, point_accessor_in,
                          edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
  }

  /** @brief Public interface for uniform refinement of a mesh with explicit point accessor and edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedMeshConfigInT, typename WrappedMeshConfigOutT, typename PointAccessorType, typename EdgeRefinementFlagAccessorT>
  void refine_uniformly(mesh<WrappedMeshConfigInT> const & mesh_in,
                        mesh<WrappedMeshConfigOutT> & mesh_out,
                        PointAccessorType point_accessor_in,
                        EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor)
  {
    mark_all_edge_refinement( mesh_in, edge_refinement_flag_accessor );
    refine<ElementTypeOrTagT>(mesh_in, mesh_out, point_accessor_in, edge_refinement_flag_accessor);
  }

  /** @brief Public interface for uniform refinement of a mesh with edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedMeshConfigInT, typename WrappedMeshConfigOutT, typename EdgeRefinementFlagAccessorT>
  void refine_uniformly(mesh<WrappedMeshConfigInT> const & mesh_in,
                        mesh<WrappedMeshConfigOutT> & mesh_out,
                        EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor)
  {
    refine_uniformly<ElementTypeOrTagT>(mesh_in, mesh_out, default_point_accessor(mesh_in), edge_refinement_flag_accessor);
  }

  /** @brief Public interface for uniform refinement of a mesh.
   *
   * @tparam ElementTypeOrTagT              The element type/tag which elements are refined
   * @param mesh_in                         Input mesh
   * @param mesh_out                        Output refined mesh
   */
  template <typename ElementTypeOrTagT, typename WrappedMeshConfigInT, typename WrappedMeshConfigOutT>
  void refine_uniformly(mesh<WrappedMeshConfigInT> const & mesh_in,
                        mesh<WrappedMeshConfigOutT> & mesh_out)
  {
    typedef mesh<WrappedMeshConfigOutT>                             MeshOutType;
    typedef typename viennagrid::result_of::line<MeshOutType>::type     EdgeType;
    std::deque<bool> edge_refinement_flag;

    refine_uniformly<ElementTypeOrTagT>(mesh_in, mesh_out, viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  /** @brief Public interface for uniform refinement of cells of a mesh. Will fail if there is more than one cell type.
   *
   * @param mesh_in                         Input mesh
   * @param mesh_out                        Output refined mesh
   */
  template <typename WrappedMeshConfigInT, typename WrappedMeshConfigOutT>
  void cell_refine_uniformly(mesh<WrappedMeshConfigInT> const & mesh_in,
                             mesh<WrappedMeshConfigOutT> & mesh_out)
  {
    typedef mesh<WrappedMeshConfigInT>                            MeshInType;
    typedef typename viennagrid::result_of::cell<MeshInType>::type    CellType;

    refine_uniformly<CellType>(mesh_in, mesh_out);
  }






  /** @brief Public interface for refinement of a mesh with segmentation providing explicit point accessor, edge refinement accessor and temporary accessors
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param segmentation_in                   Input segmentation
   * @param mesh_out                          Output refined mesh
   * @param segmentation_out                  Output refined segmentation
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   * @param vertex_to_vertex_handle_accessor  Temporary accessor for vertex to vertex mapping
   * @param edge_to_vertex_handle_accessor    Temporary accessor for refined edge to vertex mapping
   */
  template <typename ElementTypeOrTagT,
            typename WrappedMeshConfigInT, typename WrappedSegmentationConfigInT,
            typename WrappedMeshConfigOutT, typename WrappedSegmentationConfigOutT,
            typename PointAccessorT,
            typename EdgeRefinementFlagAccessorT, typename VertexToVertexHandleAccessorT, typename RefinementVertexAccessorT>
  void refine(mesh<WrappedMeshConfigInT> const & mesh_in,  segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
              mesh<WrappedMeshConfigOutT> & mesh_out,      segmentation<WrappedSegmentationConfigOutT> & segmentation_out,
              PointAccessorT point_accessor_in,
              EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor, VertexToVertexHandleAccessorT vertex_to_vertex_handle_accessor, RefinementVertexAccessorT edge_to_vertex_handle_accessor)
  {
    typedef typename viennagrid::result_of::element_tag<ElementTypeOrTagT>::type CellTag;

    detail::refine_impl<CellTag>(mesh_in, segmentation_in,
                                 mesh_out, segmentation_out,
                                 point_accessor_in,
                                 edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
  }


  /** @brief Public interface for refinement of a mesh with segmentation providing explicit point accessor and edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param segmentation_in                   Input segmentation
   * @param mesh_out                          Output refined mesh
   * @param segmentation_out                  Output refined segmentation
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedMeshConfigInT,  typename WrappedSegmentationConfigInT,
            typename WrappedMeshConfigOutT, typename WrappedSegmentationConfigOutT,
            typename PointAccessorT,
            typename EdgeRefinementFlagAccessorT>
  void refine(mesh<WrappedMeshConfigInT> const & mesh_in,  segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
              mesh<WrappedMeshConfigOutT> & mesh_out,      segmentation<WrappedSegmentationConfigOutT> & segmentation_out,
              PointAccessorT point_accessor_in,
              EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor)
  {
    typedef mesh<WrappedMeshConfigOutT> MeshOutT;

    typedef typename viennagrid::result_of::vertex<MeshOutT>::type VertexType;
    typedef typename viennagrid::result_of::line<MeshOutT>::type EdgeType;

    typedef typename viennagrid::result_of::vertex_handle<MeshOutT>::type VertexHandleType;

    std::deque<VertexHandleType> vertex_refinement_vertex_handle;
    std::deque<VertexHandleType> edge_refinement_vertex_handle;

    refine<ElementTypeOrTagT>(mesh_in, segmentation_in,
                           mesh_out, segmentation_out,
                           point_accessor_in,
                           edge_refinement_flag_accessor,
                           viennagrid::make_accessor<VertexType>(vertex_refinement_vertex_handle),
                           viennagrid::make_accessor<EdgeType>(edge_refinement_vertex_handle));
  }

  /** @brief Public interface for refinement of a mesh with segmentation providing edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param segmentation_in                   Input segmentation
   * @param mesh_out                          Output refined mesh
   * @param segmentation_out                  Output refined segmentation
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedMeshConfigInT,  typename WrappedSegmentationConfigInT,
            typename WrappedMeshConfigOutT, typename WrappedSegmentationConfigOutT,
            typename EdgeRefinementFlagAccessorT>
  void refine(mesh<WrappedMeshConfigInT> const & mesh_in,  segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
              mesh<WrappedMeshConfigOutT> & mesh_out,      segmentation<WrappedSegmentationConfigOutT> & segmentation_out,
              EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor)
  {
    typedef mesh<WrappedMeshConfigOutT> MeshOutT;

    typedef typename viennagrid::result_of::vertex_handle<MeshOutT>::type VertexHandleType;

    std::deque<VertexHandleType> vertex_refinement_vertex_handle;
    std::deque<VertexHandleType> edge_refinement_vertex_handle;

    refine<ElementTypeOrTagT>(mesh_in, segmentation_in,
                           mesh_out, segmentation_out,
                           default_point_accessor(mesh_in),
                           edge_refinement_flag_accessor);
  }



  /** @brief Public interface for refinement of a mesh with segmentation providing explicit point accessor and cell refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param segmentation_in                   Input segmentation
   * @param mesh_out                          Output refined mesh
   * @param segmentation_out                  Output refined segmentation
   * @param point_accessor_in                 Point accessor for input points
   * @param cell_refinement_flag_accessor     Accessor storing flags if a cell is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedMeshConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedMeshConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename PointAccessorT,
            typename CellRefinementFlagAccessorT>
  void element_refine(mesh<WrappedMeshConfigInT> const & mesh_in,  segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
                      mesh<WrappedMeshConfigOutT> & mesh_out,      segmentation<WrappedSegmentationConfigOutT> & segmentation_out,
                      PointAccessorT point_accessor_in,
                      CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    typedef mesh<WrappedMeshConfigOutT> MeshOutT;

    typedef typename viennagrid::result_of::line<MeshOutT>::type EdgeType;

    std::deque<bool> edge_refinement_flag;

    edge_refinement_flag.resize( viennagrid::id_upper_bound<EdgeType>(mesh_in).get() );

    cell_refinement_to_edge_refinement<ElementTypeOrTagT>(
                                        mesh_in,
                                        cell_refinement_flag_accessor,
                                        viennagrid::make_accessor<EdgeType>(edge_refinement_flag));

    refine<ElementTypeOrTagT>(mesh_in, segmentation_in,
                           mesh_out, segmentation_out,
                           point_accessor_in,
                           viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  /** @brief Public interface for refinement of a mesh with segmentation providing cell refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param segmentation_in                   Input segmentation
   * @param mesh_out                          Output refined mesh
   * @param segmentation_out                  Output refined segmentation
   * @param cell_refinement_flag_accessor     Accessor storing flags if a cell is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedMeshConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedMeshConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename CellRefinementFlagAccessorT>
  void element_refine(mesh<WrappedMeshConfigInT> const & mesh_in,  segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
                      mesh<WrappedMeshConfigOutT> & mesh_out,      segmentation<WrappedSegmentationConfigOutT> & segmentation_out,
                      CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    element_refine<ElementTypeOrTagT>(mesh_in, segmentation_in,
                                   mesh_out, segmentation_out,
                                   default_point_accessor(mesh_in),
                                   cell_refinement_flag_accessor);
  }

  /** @brief Public interface for refinement of cells of a mesh with segmentation providing cell refinement accessor. Will fail if there is more than one cell type.
   *
   * @param mesh_in                           Input mesh
   * @param segmentation_in                   Input segmentation
   * @param mesh_out                          Output refined mesh
   * @param segmentation_out                  Output refined segmentation
   * @param cell_refinement_flag_accessor     Accessor storing flags if a cell is marked for refinement
   */
  template <typename WrappedMeshConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedMeshConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename CellRefinementFlagAccessorT>
  void cell_refine(mesh<WrappedMeshConfigInT> const & mesh_in,  segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
                   mesh<WrappedMeshConfigOutT> & mesh_out,      segmentation<WrappedSegmentationConfigOutT> & segmentation_out,
                   CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    typedef mesh<WrappedMeshConfigInT>                              MeshInType;
    typedef typename viennagrid::result_of::cell_tag<MeshInType>::type  CellTag;
    element_refine<CellTag>(mesh_in, segmentation_in,
                            mesh_out, segmentation_out,
                            default_point_accessor(mesh_in),
                            cell_refinement_flag_accessor);
  }






  /** @brief Public interface for uniform refinement of a mesh with segmentation providing explicit point accessor, edge refinement accessor and temporary accessors.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param segmentation_in                   Input segmentation
   * @param mesh_out                          Output refined mesh
   * @param segmentation_out                  Output refined segmentation
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   * @param vertex_to_vertex_handle_accessor  Temporary accessor for vertex to vertex mapping
   * @param edge_to_vertex_handle_accessor    Temporary accessor for refined edge to vertex mapping
   */
  template <typename ElementTypeOrTagT,
            typename WrappedMeshConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedMeshConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename PointAccessorT,
            typename EdgeRefinementFlagAccessorT, typename VertexToVertexHandleAccessorT, typename RefinementVertexAccessorT>
  void refine_uniformly(mesh<WrappedMeshConfigInT> const & mesh_in,  segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
                        mesh<WrappedMeshConfigOutT> & mesh_out,      segmentation<WrappedSegmentationConfigOutT> & segmentation_out,
                        PointAccessorT point_accessor_in,
                        EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor,
                        VertexToVertexHandleAccessorT vertex_to_vertex_handle_accessor,
                        RefinementVertexAccessorT edge_to_vertex_handle_accessor)
  {
    mark_all_edge_refinement( mesh_in, edge_refinement_flag_accessor );
    refine<ElementTypeOrTagT>(mesh_in, segmentation_in,
                           mesh_out, segmentation_out,
                           point_accessor_in,
                           edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
  }

  /** @brief Public interface for uniform refinement of a mesh with segmentation providing explicit point accessor and edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param segmentation_in                   Input segmentation
   * @param mesh_out                          Output refined mesh
   * @param segmentation_out                  Output refined segmentation
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedMeshConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedMeshConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename PointAccessorT,
            typename EdgeRefinementFlagAccessorT>
  void refine_uniformly(mesh<WrappedMeshConfigInT> const & mesh_in,  segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
                        mesh<WrappedMeshConfigOutT> & mesh_out,      segmentation<WrappedSegmentationConfigOutT> & segmentation_out,
                        PointAccessorT point_accessor_in,
                        EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor)
  {
    mark_all_edge_refinement( mesh_in, edge_refinement_flag_accessor );
    refine<ElementTypeOrTagT>(mesh_in, segmentation_in,
                           mesh_out, segmentation_out,
                           point_accessor_in,
                           edge_refinement_flag_accessor);
  }

  /** @brief Public interface for uniform refinement of a mesh with segmentation providing edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param segmentation_in                   Input segmentation
   * @param mesh_out                          Output refined mesh
   * @param segmentation_out                  Output refined segmentation
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedMeshConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedMeshConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename EdgeRefinementFlagAccessorT>
  void refine_uniformly(mesh<WrappedMeshConfigInT> const & mesh_in,  segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
                        mesh<WrappedMeshConfigOutT> & mesh_out,      segmentation<WrappedSegmentationConfigOutT> & segmentation_out,
                        EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor)
  {
    refine_uniformly<ElementTypeOrTagT>(mesh_in, segmentation_in, mesh_out, segmentation_out, default_point_accessor(mesh_in), edge_refinement_flag_accessor);
  }

  /** @brief Public interface for uniform refinement of a mesh with segmentation.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param segmentation_in                   Input segmentation
   * @param mesh_out                          Output refined mesh
   * @param segmentation_out                  Output refined segmentation
   */
  template <typename ElementTypeOrTagT,
            typename WrappedMeshConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedMeshConfigOutT,  typename WrappedSegmentationConfigOutT>
  void refine_uniformly(mesh<WrappedMeshConfigInT> const & mesh_in,  segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
                        mesh<WrappedMeshConfigOutT> & mesh_out,      segmentation<WrappedSegmentationConfigOutT> & segmentation_out)
  {
    typedef mesh<WrappedMeshConfigOutT>                           MeshOutType;
    typedef typename viennagrid::result_of::line<MeshOutType>::type   EdgeType;
    std::deque<bool> edge_refinement_flag;

    refine_uniformly<ElementTypeOrTagT>(mesh_in, segmentation_in, mesh_out, segmentation_out, viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  /** @brief Public interface for uniform refinement of cells of a mesh with segmentation. Will fail if there is more than one cell type.
   *
   * @param mesh_in                           Input mesh
   * @param segmentation_in                   Input segmentation
   * @param mesh_out                          Output refined mesh
   * @param segmentation_out                  Output refined segmentation
   */
  template <typename WrappedMeshConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedMeshConfigOutT,  typename WrappedSegmentationConfigOutT>
  void cell_refine_uniformly(mesh<WrappedMeshConfigInT> const & mesh_in,  segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
                             mesh<WrappedMeshConfigOutT> & mesh_out,      segmentation<WrappedSegmentationConfigOutT> & segmentation_out)
  {
    typedef mesh<WrappedMeshConfigInT>                            MeshInType;
    typedef typename viennagrid::result_of::cell<MeshInType>::type    CellType;
    refine_uniformly<CellType>(mesh_in, segmentation_in, mesh_out, segmentation_out);
  }


}

#endif
