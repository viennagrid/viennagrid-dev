#ifndef VIENNAGRID_ALGORITHM_REFINE_HPP
#define VIENNAGRID_ALGORITHM_REFINE_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/core.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/norm.hpp"

// #include "viennagrid/mesh/element_creation.hpp"
// #include "viennagrid/mesh/mesh_operations.hpp"

#include "viennagrid/algorithm/detail/refine_tri.hpp"
#include "viennagrid/algorithm/detail/refine_tet.hpp"

#include "viennagrid/algorithm/detail/numeric.hpp"

/** @file viennagrid/algorithm/refine.hpp
    @brief Provides the routines for a refinement of a mesh
*/

namespace viennagrid
{
  namespace detail
  {

    template<typename ElementT, typename MeshT,
             typename ElementsVerticesHandleContainerT, typename VertexCopyMapT,
             typename EdgeRefinementFlagAccessor, typename EdgeToVertexHandleAccessor>
    void refine_single_element(ElementT const & element, MeshT const & mesh,
                               ElementsVerticesHandleContainerT & elements_vertices,
                               VertexCopyMapT & vertex_copy_map_,
                               EdgeRefinementFlagAccessor const & edge_refinement_flag_accessor,
                               EdgeToVertexHandleAccessor const & edge_to_vertex_handle_accessor)
    {
      if (element.tag().is_triangle())
        detail::element_refinement<viennagrid::triangle_tag>::apply(element, mesh, elements_vertices, vertex_copy_map_, edge_refinement_flag_accessor, edge_to_vertex_handle_accessor);
      else if (element.tag().is_tetrahedron())
        detail::element_refinement<viennagrid::tetrahedron_tag>::apply(element, mesh, elements_vertices, vertex_copy_map_, edge_refinement_flag_accessor, edge_to_vertex_handle_accessor);
      else
        assert(false);
    }

  }


  /** @brief Refines a mesh based on edge information. A bool accessor, indicating if an edge should be refined, and a vertex handle accessor, representing the new vertex of an edge to refine, are used for the refinement process.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param vertex_copy_map_                  A vertex copy map for identifying vertices
   * @param edge_refinement_flag_accessor     An accessor defining which edge should be refined. operator() takes an edge and returns a bool.
   * @param edge_to_vertex_handle_accessor    An accessor defining the vertex handle of an edge to refine. operator() takes an edge and returns a vertex handle.
   */
  template<bool mesh_is_const,
           typename VertexCopyMapT,
           typename EdgeRefinementFlagAccessorT, typename RefinementVertexAccessorT>
  void simple_refine(viennagrid::base_mesh<mesh_is_const> const & mesh_in,
                     viennagrid::mesh_t const & mesh_out,
                     viennagrid_int topologic_dimension,
                     VertexCopyMapT & vertex_copy_map_,
                     EdgeRefinementFlagAccessorT const & edge_refinement_flag_accessor,
                     RefinementVertexAccessorT const & edge_to_vertex_handle_accessor)
  {
    typedef base_mesh<mesh_is_const>       InputMeshType;
    typedef typename viennagrid::result_of::mesh_hierarchy<InputMeshType>::type InputMeshHierarchyType;
    typedef mesh_t      OutputMeshType;

    typedef typename viennagrid::result_of::element<InputMeshType>::type  ElementType;
    typedef typename viennagrid::result_of::const_element_range<InputMeshType>::type  ElementRange;
    typedef typename viennagrid::result_of::iterator<ElementRange>::type                                 ElementIterator;


    ElementRange cells(mesh_in, topologic_dimension);
    for (ElementIterator cit  = cells.begin();
                      cit != cells.end();
                    ++cit)
    {
      typedef typename viennagrid::result_of::element<OutputMeshType>::type OutputVertexType;
      typedef std::vector<OutputVertexType> VertexHandlesContainerType;
      typedef std::vector<VertexHandlesContainerType> ElementsContainerType;
      ElementsContainerType elements_vertices;

      detail::refine_single_element(*cit, mesh_out, elements_vertices, vertex_copy_map_, edge_refinement_flag_accessor, edge_to_vertex_handle_accessor);



      if (mesh_in.mesh_hierarchy() != mesh_out.mesh_hierarchy())
      {
        for (typename ElementsContainerType::iterator it = elements_vertices.begin();
              it != elements_vertices.end();
              ++it)
        {
          ElementType new_element = viennagrid::make_element( mesh_out, (*cit).tag(), it->begin(), it->end() );

          typedef typename viennagrid::result_of::region_range<InputMeshHierarchyType, ElementType>::type ElementRegionType;
          typedef typename viennagrid::result_of::iterator<ElementRegionType>::type ElementRegionIterator;

          ElementRegionType regions(*cit);
          for (ElementRegionIterator rit = regions.begin(); rit != regions.end(); ++rit)
            viennagrid::add( mesh_out.get_make_region((*rit).id()), new_element );
        }
      }
      else
      {
        std::vector< std::pair<ElementType, ElementType> > intersects;

        typedef typename viennagrid::result_of::const_element_range<ElementType, 1>::type BoundaryElementRangeType;
        typedef typename viennagrid::result_of::iterator<BoundaryElementRangeType>::type BoundaryElementRangeIterator;

        BoundaryElementRangeType lines(*cit);
        for (BoundaryElementRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
        {
          if (edge_refinement_flag_accessor.get(*lit))
            intersects.push_back( std::make_pair( edge_to_vertex_handle_accessor.get(*lit), *lit ) );
        }


        for (typename ElementsContainerType::iterator it = elements_vertices.begin();
              it != elements_vertices.end();
              ++it)
        {
          ElementType new_element = viennagrid::make_refined_element(mesh_out, *cit,
                                           (*cit).tag(),
                                           it->begin(),
                                           it->end(),
                                           intersects.begin(),
                                           intersects.end());

          typedef typename viennagrid::result_of::region_range<InputMeshHierarchyType, ElementType>::type ElementRegionType;
          typedef typename viennagrid::result_of::iterator<ElementRegionType>::type ElementRegionIterator;

          ElementRegionType regions(*cit);
          for (ElementRegionIterator rit = regions.begin(); rit != regions.end(); ++rit)
            viennagrid::add( *rit, new_element );
        }
      }

    }
  }





//   /** @brief Refines a mesh and a segmentation based on edge information. A bool accessor, indicating if an edge should be refined, and a vertex handle accessor, representing the new vertex of an edge to refine, are used for the refinement process.
//    *
//    * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
//    * @param mesh_in                           Input mesh
//    * @param segmentation_in                   Input segmentation
//    * @param mesh_out                          Output refined mesh
//    * @param segmentation_out                  Output refined segmentation
//    * @param vertex_copy_map_                  A vertex copy map for identifying vertices
//    * @param edge_refinement_flag_accessor     An accessor defining which edge should be refined. operator() takes an edge and returns a bool.
//    * @param edge_to_vertex_handle_accessor    An accessor defining the vertex handle of an edge to refine. operator() takes an edge and returns a vertex handle.
//    */
//   template<typename ElementTypeOrTagT,
//             typename WrappedMeshConfigInT,  typename WrappedSegmentationConfigInT,
//             typename WrappedMeshConfigOutT, typename WrappedSegmentationConfigOutT,
//             typename VertexCopyMapT,
//             typename EdgeRefinementFlagAccessorT, typename RefinementVertexAccessorT>
//   void simple_refine(mesh<WrappedMeshConfigInT>                 const & mesh_in,
//                      segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
//                      mesh<WrappedMeshConfigOutT>                      & mesh_out,
//                      segmentation<WrappedSegmentationConfigOutT>      & segmentation_out,
//                      VertexCopyMapT & vertex_copy_map_,
//                      EdgeRefinementFlagAccessorT const & edge_refinement_flag_accessor,
//                      RefinementVertexAccessorT const & edge_to_vertex_handle_accessor)
//   {
//     typedef mesh<WrappedMeshConfigInT>    InputMeshType;
//     typedef mesh<WrappedMeshConfigOutT>   OutputMeshType;
//
//     typedef segmentation<WrappedSegmentationConfigInT>      InputSegmentationType;
//
//     typedef typename viennagrid::result_of::const_element_range<InputMeshType, ElementTypeOrTagT>::type ElementRange;
//     typedef typename viennagrid::result_of::iterator<ElementRange>::type                                ElementIterator;
//
//     ElementRange cells(mesh_in);
//     for (ElementIterator cit  = cells.begin();
//                       cit != cells.end();
//                     ++cit)
//     {
//       typedef typename viennagrid::result_of::segment_id_range<InputSegmentationType, ElementTypeOrTagT>::type SegmentIDRangeType;
//
//       SegmentIDRangeType segment_ids = viennagrid::segment_ids( segmentation_in, *cit );
//
//       typedef typename viennagrid::result_of::vertex_handle<OutputMeshType>::type OutputVertexHandleType;
//       typedef typename viennagrid::result_of::handle<OutputMeshType, ElementTypeOrTagT>::type OutputCellHandleType;
//       typedef std::vector<OutputVertexHandleType> VertexHandlesContainerType;
//       typedef std::vector<VertexHandlesContainerType> ElementsContainerType;
//       ElementsContainerType elements_vertices;
//
//       detail::element_refinement<ElementTypeOrTagT>::apply(*cit, mesh_out, elements_vertices, vertex_copy_map_, edge_refinement_flag_accessor, edge_to_vertex_handle_accessor);
//
//       for (typename ElementsContainerType::iterator it = elements_vertices.begin();
//             it != elements_vertices.end();
//             ++it)
//       {
//         OutputCellHandleType new_cell = viennagrid::make_element<ElementTypeOrTagT>( mesh_out, it->begin(), it->end() );
//         viennagrid::add( segmentation_out, segment_ids.begin(), segment_ids.end(), new_cell );
//       }
//     }
//   }




  namespace detail
  {



    /** @brief For internal use only */
    template<bool mesh_is_const,
             typename PointAccessorT,
             typename VertexCopyMapT,
             typename EdgeRefinementFlagAccessorT, typename RefinementVertexAccessorT>
    void refine_impl(base_mesh<mesh_is_const> const & mesh_in,
                     mesh_t & mesh_out,
                     viennagrid_int topologic_dimension,
                     PointAccessorT const point_accessor_in,
                     VertexCopyMapT & vertex_copy_map_,
                     EdgeRefinementFlagAccessorT const & edge_refinement_flag_accessor,
                     RefinementVertexAccessorT & edge_to_vertex_handle_accessor)
    {
      typedef base_mesh<mesh_is_const>       InputMeshType;

      typedef typename viennagrid::result_of::const_element_range<InputMeshType, 1>::type                 EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                             EdgeIterator;

      //
      // Step 1: Each tagged edge in old mesh results in a new vertex (temporarily store new vertex IDs on old mesh)
      //
      EdgeRange edges(mesh_in);
      for (EdgeIterator eit = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        if ( edge_refinement_flag_accessor.get(*eit) )
        {
          edge_to_vertex_handle_accessor.set( *eit,
                                              viennagrid::make_vertex(
                                                mesh_out.mesh_hierarchy(),
                                                viennagrid::centroid(point_accessor_in, *eit)
                                              )
                                            );
        }
      }

      //
      // Step 2: Now write new cells to new mesh
      //
      simple_refine(mesh_in, mesh_out,
                    topologic_dimension,
                    vertex_copy_map_, edge_refinement_flag_accessor, edge_to_vertex_handle_accessor);
    }



//     /** @brief For internal use only */
//     template<typename CellTagT,
//               typename WrappedMeshConfigInT,  typename WrappedSegmentationConfigInT,
//               typename WrappedMeshConfigOutT, typename WrappedSegmentationConfigOutT,
//               typename PointAccessorT,
//               typename VertexCopyMapT,
//               typename EdgeRefinementFlagAccessorT, typename RefinementVertexAccessorT>
//     void refine_impl(mesh<WrappedMeshConfigInT>                 const & mesh_in,
//                      segmentation<WrappedSegmentationConfigInT> const & segmentation_in,
//                      mesh<WrappedMeshConfigOutT>                      & mesh_out,
//                      segmentation<WrappedSegmentationConfigOutT>      & segmentation_out,
//                      PointAccessorT const point_accessor_in,
//                      VertexCopyMapT & vertex_copy_map_,
//                      EdgeRefinementFlagAccessorT const & edge_refinement_flag_accessor,
//                      RefinementVertexAccessorT & edge_to_vertex_handle_accessor)
//     {
//       typedef mesh<WrappedMeshConfigInT>    InputMeshType;
//
//       typedef typename viennagrid::result_of::const_line_range<InputMeshType>::type       EdgeRange;
//       typedef typename viennagrid::result_of::iterator<EdgeRange>::type                               EdgeIterator;
//
//
//       //
//       // Step 1: Each tagged edge in old mesh results in a new vertex (temporarily store new vertex IDs on old mesh)
//       //
//       EdgeRange edges(mesh_in);
//       for (EdgeIterator eit = edges.begin();
//                         eit != edges.end();
//                       ++eit)
//       {
//         if ( edge_refinement_flag_accessor(*eit) )
//         {
//           edge_to_vertex_handle_accessor( *eit ) = viennagrid::make_vertex( mesh_out, viennagrid::centroid(point_accessor_in, *eit) );
//         }
//       }
//
//       //
//       // Step 2: Now write new cells to new mesh
//       //
//       simple_refine<CellTagT>( mesh_in, segmentation_in, mesh_out, segmentation_out, vertex_copy_map_, edge_refinement_flag_accessor, edge_to_vertex_handle_accessor );
//     }


  } //namespace detail



  /** @brief Ensures refinement of the longest edge of each cell. If any edge is tagged for refinement in a cell, then the longest edge is refined as well. */
  template<bool mesh_is_const, typename EdgeRefinementFlagAccessorT>
  void ensure_longest_edge_refinement(base_mesh<mesh_is_const> const & mesh_in,
                                      viennagrid_int topologic_dimension,
//                                       element_tag_t cell_tag_begin, element_tag_t cell_tag_end,
                                      EdgeRefinementFlagAccessorT edge_refinement_flag_accessor)
  {
    typedef base_mesh<mesh_is_const>                                            MeshInType;

    typedef typename viennagrid::result_of::element<MeshInType>::type    CellType;
    typedef typename viennagrid::result_of::element<MeshInType>::type     VertexType;
    typedef typename viennagrid::result_of::element<MeshInType>::type       EdgeType;

    typedef typename viennagrid::result_of::const_cell_range<MeshInType>::type    CellRange;
    typedef typename viennagrid::result_of::iterator<CellRange>::type                CellIterator;
    typedef typename viennagrid::result_of::const_element_range<CellType, 1>::type         EdgeOnCellRange;
    typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type          EdgeOnCellIterator;
    typedef typename viennagrid::result_of::const_vertex_range<EdgeType>::type       VertexOnEdgeRange;
    typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type        VertexOnEdgeIterator;


    bool something_changed = true;
    //std::size_t iter_cnt = 0;

    while (something_changed)
    {
      something_changed = false;

//       for (element_tag_t cell_tag = cell_tag_begin; cell_tag != cell_tag_end; ++cell_tag)
//       {
        CellRange cells = viennagrid::elements(mesh_in, topologic_dimension);
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

              double len = viennagrid::norm( viennagrid::get_point(mesh_in, v0) - viennagrid::get_point(mesh_in, v1) );
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
//       }


    } //while
  } //ensure_longest_edge_refinement

  /** @brief Transfers tags for refinement from the cell to edges */
  template<bool mesh_is_const, typename CellRefinementFlagAccessorT, typename EdgeRefinementFlagAccessorT>
  void cell_refinement_to_edge_refinement(base_mesh<mesh_is_const> const & mesh_in,
                                          viennagrid_int topologic_dimension,
                                          CellRefinementFlagAccessorT cell_refinement_flag, EdgeRefinementFlagAccessorT edge_refinement_flag_accessor)
  {
    typedef base_mesh<mesh_is_const>                                                        MeshInType;

    typedef typename viennagrid::result_of::element<MeshInType>::type                  ElementType;

    typedef typename viennagrid::result_of::const_element_range<MeshInType>::type      ElementRange;
    typedef typename viennagrid::result_of::iterator<ElementRange>::type               ElementIterator;
    typedef typename viennagrid::result_of::const_element_range<ElementType, 1>::type           EdgeOnElementRange;
    typedef typename viennagrid::result_of::iterator<EdgeOnElementRange>::type                         EdgeOnCellIterator;

    std::size_t cells_for_refinement = 0;

//     for (; cell_tag != cell_tag_end; ++cell_tag)
//     {
      ElementRange elements(mesh_in, topologic_dimension);
      for (ElementIterator eit  = elements.begin();
                          eit != elements.end();
                        ++eit)
      {
        if ( cell_refinement_flag.get(*eit) )
        {
          ++cells_for_refinement;

          EdgeOnElementRange edges_on_element(*eit);
          for (EdgeOnCellIterator eocit = edges_on_element.begin();
                                  eocit != edges_on_element.end();
                                ++eocit)
          {
            edge_refinement_flag_accessor.set(*eocit, true);
          }
        }
      }
//     }

  }


  /** @brief Mark all edges for refinement -> unform refinement */
  template<bool mesh_is_const, typename EdgeRefinementFlagAccessorT>
  void mark_all_edge_refinement(base_mesh<mesh_is_const> const & mesh_in, EdgeRefinementFlagAccessorT edge_refinement_flag_accessor)
  {
    typedef base_mesh<mesh_is_const>                                                                MeshInType;

    typedef typename viennagrid::result_of::const_element_range<MeshInType, 1>::type   LineRange;
    typedef typename viennagrid::result_of::iterator<LineRange>::type                                       LineIterator;

    LineRange lines(mesh_in);
    for (LineIterator it = lines.begin(); it != lines.end(); ++it)
      edge_refinement_flag_accessor.set(*it, true);
  }


  /** @brief Mark edges for refinement based on a hyperplane */
  template<typename SrcMeshT, typename DstMeshT, typename PointT, typename LineRefinementTagContainerT, typename LineRefinementVertexHandleContainerT, typename NumericConfigT>
  void mark_edges_for_hyperplane_refine(SrcMeshT const & src_mesh, DstMeshT & dst_mesh,
                            PointT const & hyperplane_point, PointT const & hyperplane_normal,
                            LineRefinementTagContainerT & line_refinement_tag_accessor,
                            LineRefinementVertexHandleContainerT & line_refinement_vertex_handle_accessor,
                            NumericConfigT numeric_config)
  {
    typedef typename viennagrid::result_of::coord<SrcMeshT>::type NumericType;
    typedef typename viennagrid::result_of::const_element_range<SrcMeshT, 1>::type ConstLineRangeType;
    typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineIteratorType;

    ConstLineRangeType lines(src_mesh);
    for (ConstLineIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
    {
      NumericType distance0 = viennagrid::inner_prod( hyperplane_normal, viennagrid::get_point( viennagrid::vertices(*lit)[0] )-hyperplane_point );
      NumericType distance1 = viennagrid::inner_prod( hyperplane_normal, viennagrid::get_point( viennagrid::vertices(*lit)[1] )-hyperplane_point );

      NumericType tolerance = viennagrid::detail::relative_tolerance(numeric_config, viennagrid::volume(*lit));

      if (distance0 > distance1)
        std::swap(distance0, distance1);

      if (distance0 < -tolerance && distance1 > tolerance)
      {
        line_refinement_tag_accessor.set(*lit, true);

        PointT pt0 = viennagrid::get_point( viennagrid::vertices(*lit)[0] );
        PointT pt1 = viennagrid::get_point( viennagrid::vertices(*lit)[1] );

        double qd = viennagrid::inner_prod( hyperplane_normal, pt0-hyperplane_point );
        double pd = viennagrid::inner_prod( hyperplane_normal, pt1-hyperplane_point );

        PointT new_pt = pt1 - (pd / (pd-qd)) * (pt1 - pt0);
        line_refinement_vertex_handle_accessor.set(*lit, viennagrid::make_vertex( dst_mesh, new_pt ));
      }
      else
        line_refinement_tag_accessor.set(*lit, false);
    }
  }







  /** @brief Public interface for refinement of a mesh with explicit point accessor, edge refinement accessor and temporary accessors
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param point_accessor_in                 Point accessor for input points
   * @param vertex_copy_map_                  A vertex copy map for identifying vertices
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   * @param edge_to_vertex_handle_accessor    Temporary accessor for refined edge to vertex mapping
   */
  template<bool mesh_is_const,
           typename PointAccessorType,
           typename VertexCopyMapT,
           typename EdgeRefinementFlagAccessorT, typename RefinementVertexAccessor>
  void refine(base_mesh<mesh_is_const> const & mesh_in,
              mesh_t & mesh_out,
              viennagrid_int topologic_dimension,
              PointAccessorType point_accessor_in,
              VertexCopyMapT & vertex_copy_map_,
              EdgeRefinementFlagAccessorT const & edge_refinement_flag_accessor,
              RefinementVertexAccessor & edge_to_vertex_handle_accessor)
  {
    detail::refine_impl(mesh_in, mesh_out,
                        topologic_dimension,
                        point_accessor_in,
                        vertex_copy_map_,
                        edge_refinement_flag_accessor, edge_to_vertex_handle_accessor);
  }


  /** @brief Public interface for refinement of a mesh with explicit point accessor and edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template<bool mesh_is_const,
           typename PointAccessorType,
           typename EdgeRefinementFlagAccessorT>
  void refine(base_mesh<mesh_is_const> const & mesh_in,
              mesh_t & mesh_out,
              viennagrid_int topologic_dimension,
              PointAccessorType point_accessor_in,
              EdgeRefinementFlagAccessorT const & edge_refinement_flag_accessor)
  {
    typedef mesh_t                                             MeshOutType;

    typedef typename viennagrid::result_of::element<MeshOutType>::type                     EdgeType;
    typedef typename viennagrid::result_of::element<MeshOutType>::type            VertexType;

    typename viennagrid::result_of::element_copy_map<>::type copy_map(mesh_out);
    std::deque<VertexType> edge_refinement_vertex_handle_container;
    typename viennagrid::result_of::accessor<std::deque<VertexType>, EdgeType>::type edge_refinement_vertex_handle_accessor(edge_refinement_vertex_handle_container);

    refine(mesh_in, mesh_out,
           topologic_dimension,
           point_accessor_in,
           copy_map,
           edge_refinement_flag_accessor,
           edge_refinement_vertex_handle_accessor);
  }

  /** @brief Public interface for refinement of a mesh with edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template<bool mesh_is_const,
           typename EdgeRefinementFlagAccessorT>
  void refine(base_mesh<mesh_is_const> const & mesh_in,
              mesh_t & mesh_out,
              viennagrid_int topologic_dimension,
              EdgeRefinementFlagAccessorT const & edge_refinement_flag_accessor)
  {
    refine(mesh_in, mesh_out, topologic_dimension, point_accessor(mesh_in), edge_refinement_flag_accessor);
  }



  /** @brief Public interface for refinement of a mesh with explicit point accessor and cell refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param point_accessor_in                 Point accessor for input points
   * @param cell_refinement_flag_accessor     Accessor storing flags if a cell is marked for refinement
   */
  template<bool mesh_is_const,
           typename PointAccessorType,
           typename CellRefinementFlagAccessorT>
  void element_refine(base_mesh<mesh_is_const> const & mesh_in,
                      mesh_t & mesh_out,
                      viennagrid_int topologic_dimension,
                      PointAccessorType point_accessor_in, CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    typedef base_mesh<mesh_is_const>                                             MeshOutType;

    typedef typename viennagrid::result_of::element<MeshOutType>::type                     EdgeType;

    std::deque<bool> edge_refinement_flag;

//     edge_refinement_flag.resize( static_cast<std::size_t>(viennagrid::id_upper_bound<EdgeType>(mesh_in).get()) );
    edge_refinement_flag.resize( static_cast<std::size_t>( viennagrid::elements<1>(mesh_in).size() ) );

    cell_refinement_to_edge_refinement(mesh_in,
                                       topologic_dimension,
                                       cell_refinement_flag_accessor,
                                       viennagrid::make_accessor<EdgeType>(edge_refinement_flag));

    refine(mesh_in, mesh_out, topologic_dimension,
           point_accessor_in, viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  /** @brief Public interface for refinement of a mesh with cell refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param cell_refinement_flag_accessor     Accessor storing flags if a cell is marked for refinement
   */
  template<bool mesh_is_const,
           typename CellRefinementFlagAccessorT>
  void element_refine(base_mesh<mesh_is_const> const & mesh_in,
                      mesh_t & mesh_out,
                      viennagrid_int topologic_dimension,
                      CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    element_refine(mesh_in, mesh_out, topologic_dimension,
                   point_accessor(mesh_in), cell_refinement_flag_accessor);
  }

  /** @brief Public interface for refinement of cells of a mesh with cell refinement accessor. If there is more than one cell type this funcion will fail.
   *
   * @param mesh_in                         Input mesh
   * @param mesh_out                        Output refined mesh
   * @param cell_refinement_flag_accessor     Accessor storing flags if a cell is marked for refinement
   */
  template<bool mesh_is_const,
            typename CellRefinementFlagAccessorT>
  void cell_refine(base_mesh<mesh_is_const> const & mesh_in,
                   mesh_t & mesh_out,
                   CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    element_refine(mesh_in, mesh_out,
                   viennagrid::cell_dimension(mesh_in),
                   point_accessor(mesh_in), cell_refinement_flag_accessor);
  }




//   /** @brief Public interface for uniform refinement of a mesh with explicit point accessor, edge refinement accessor and temporary accessors.
//    *
//    * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
//    * @param mesh_in                           Input mesh
//    * @param mesh_out                          Output refined mesh
//    * @param point_accessor_in                 Point accessor for input points
//    * @param vertex_copy_map_                  A vertex copy map for identifying vertices
//    * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
//    * @param edge_to_vertex_handle_accessor    Temporary accessor for refined edge to vertex mapping
//    */
//   template<typename ElementTypeOrTagT,
//            bool mesh_is_const,
//            typename PointAccessorType,
//            typename VertexCopyMapT,
//            typename EdgeRefinementFlagAccessorT, typename RefinementVertexAccessor>
//   void refine_uniformly(base_mesh<mesh_is_const> const & mesh_in,
//                         mesh_t & mesh_out,
//                         PointAccessorType point_accessor_in,
//                         VertexCopyMapT vertex_copy_map_,
//                         EdgeRefinementFlagAccessorT const & edge_refinement_flag_accessor,
//                         RefinementVertexAccessor edge_to_vertex_handle_accessor)
//   {
//     mark_all_edge_refinement( mesh_in, edge_refinement_flag_accessor );
//     refine<ElementTypeOrTagT>(mesh_in, mesh_out, point_accessor_in,
//                           vertex_copy_map_,
//                           edge_refinement_flag_accessor, edge_to_vertex_handle_accessor);
//   }

  /** @brief Public interface for uniform refinement of a mesh with explicit point accessor and edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template<bool mesh_is_const,
           typename PointAccessorType,
           typename EdgeRefinementFlagAccessorT>
  void refine_uniformly(base_mesh<mesh_is_const> const & mesh_in,
                        mesh_t & mesh_out,
                        viennagrid_int topologic_dimension,
//                         element_tag_t cell_tag_begin, element_tag_t cell_tag_end,
                        PointAccessorType point_accessor_in,
                        EdgeRefinementFlagAccessorT const & edge_refinement_flag_accessor)
  {
    mark_all_edge_refinement( mesh_in, edge_refinement_flag_accessor );
    refine(mesh_in, mesh_out, topologic_dimension, point_accessor_in, edge_refinement_flag_accessor);
  }

  /** @brief Public interface for uniform refinement of a mesh with edge refinement accessor.
   *
   * @tparam ElementTypeOrTagT                The element type/tag which elements are refined
   * @param mesh_in                           Input mesh
   * @param mesh_out                          Output refined mesh
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template<bool mesh_is_const,
           typename EdgeRefinementFlagAccessorT>
  void refine_uniformly(base_mesh<mesh_is_const> const & mesh_in,
                        mesh_t & mesh_out,
                        viennagrid_int topologic_dimension,
                        EdgeRefinementFlagAccessorT const & edge_refinement_flag_accessor)
  {
    refine_uniformly(mesh_in, mesh_out, topologic_dimension,
                     point_accessor(mesh_in), edge_refinement_flag_accessor);
  }

  /** @brief Public interface for uniform refinement of a mesh.
   *
   * @tparam ElementTypeOrTagT              The element type/tag which elements are refined
   * @param mesh_in                         Input mesh
   * @param mesh_out                        Output refined mesh
   */
  template<bool mesh_is_const>
  void refine_uniformly(base_mesh<mesh_is_const> const & mesh_in,
                        mesh_t & mesh_out,
                        viennagrid_int topologic_dimension)
  {
    typedef base_mesh<mesh_is_const>                            MeshOutType;
    typedef typename viennagrid::result_of::element<MeshOutType>::type     EdgeType;
    std::deque<bool> edge_refinement_flag;

    refine_uniformly(mesh_in, mesh_out, topologic_dimension,
                     viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  /** @brief Public interface for uniform refinement of cells of a mesh. Will fail if there is more than one cell type.
   *
   * @param mesh_in                         Input mesh
   * @param mesh_out                        Output refined mesh
   */
  template<bool mesh_is_const>
  void cell_refine_uniformly(base_mesh<mesh_is_const> const & mesh_in,
                             mesh_t & mesh_out)
  {
    refine_uniformly(mesh_in, mesh_out, viennagrid::cell_dimension(mesh_in));
  }

  /** @brief Refines a mesh based on a hyperplane. All elements which intersects the hyperplane are refined in a way that they don't intersect the hyperplane afterwards.
   *
   * @param src_mesh            The input mesh
   * @param dst_mesh            The output mesh
   * @param hyperplane_point    A point representing the hyperplane
   * @param hyperplane_normal   The normale vector representing the hyperplane
   * @param numeric_config      The numeric config
   */
  template<bool mesh_is_const, typename PointT, typename NumericConfigT>
  void hyperplane_refine(viennagrid::base_mesh<mesh_is_const> const & src_mesh, viennagrid::mesh_t const & dst_mesh,
                         PointT const & hyperplane_point, PointT const & hyperplane_normal,
                         NumericConfigT numeric_config)
  {
    typedef viennagrid::base_mesh<mesh_is_const> SrcMeshType;
    typedef viennagrid::mesh_t DstMeshType;

    typedef typename viennagrid::result_of::element<SrcMeshType>::type LineType;
    typedef typename viennagrid::result_of::element<DstMeshType>::type DstMeshVertexType;

    viennagrid::result_of::element_copy_map<>::type copy_map( dst_mesh );

    std::deque<bool> line_refinement_tag_container;
    typename viennagrid::result_of::accessor<std::deque<bool>, LineType>::type line_refinement_tag_accessor(line_refinement_tag_container);

    std::deque<DstMeshVertexType> line_refinement_vertex_handle_container;
    typename viennagrid::result_of::accessor<std::deque<DstMeshVertexType>, LineType>::type line_refinement_vertex_handle_accessor(line_refinement_vertex_handle_container);

    mark_edges_for_hyperplane_refine(src_mesh, dst_mesh, hyperplane_point, hyperplane_normal, line_refinement_tag_accessor, line_refinement_vertex_handle_accessor, numeric_config);

    viennagrid::simple_refine(src_mesh, dst_mesh,
                              viennagrid::cell_dimension(src_mesh),
                              copy_map,
                              line_refinement_tag_accessor,
                              line_refinement_vertex_handle_accessor);
  }








  template<typename MeshT>
  void refine_plc_lines(MeshT const & input_mesh,
                        viennagrid::mesh_t const & output_mesh,
                        viennagrid_numeric line_size)
  {
    typedef typename viennagrid::result_of::element<MeshT>::type ElementType;

    std::map<ElementType, std::vector<ElementType> > line_to_lines_mapping;

    typedef typename viennagrid::result_of::const_element_range<MeshT>::type ConstElementRangeType;
    typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementRangeIterator;
    typedef typename viennagrid::result_of::point<MeshT>::type PointType;

    typedef typename viennagrid::result_of::element_copy_map<>::type ElementCopyMapType;
    ElementCopyMapType copy_map( output_mesh );

    ConstElementRangeType lines(input_mesh, 1);
    for (ConstElementRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
    {
      ElementType v0 = viennagrid::vertices(*lit)[0];
      ElementType v1 = viennagrid::vertices(*lit)[1];

      ElementType nv0 = copy_map(v0);
      ElementType nv1 = copy_map(v1);

      PointType p0 = viennagrid::get_point(v0);
      PointType p1 = viennagrid::get_point(v1);

      std::vector<ElementType> new_lines;

      double current_line_size = viennagrid::norm_2(p0-p1);
      if (current_line_size < line_size)
      {
        new_lines.push_back( copy_map(*lit) );
      }
      else
      {
        int number_of_new_lines = static_cast<int>(current_line_size / line_size);
        assert(number_of_new_lines > 1);
        PointType offset = (p1-p0) / number_of_new_lines;

        std::vector<ElementType> vertices;
        vertices.push_back(nv0);
        for (int i = 1; i != number_of_new_lines; ++i)
          vertices.push_back( viennagrid::make_vertex(output_mesh, p0+i*offset) );
        vertices.push_back(nv1);

        for (std::size_t i = 0; i != vertices.size()-1; ++i)
          new_lines.push_back( viennagrid::make_line(output_mesh, vertices[i], vertices[i+1]) );
      }

      line_to_lines_mapping[*lit] = new_lines;
    }

    ConstElementRangeType plcs(input_mesh, 2);
    for (ConstElementRangeIterator pit = plcs.begin(); pit != plcs.end(); ++pit)
    {
      std::vector<ElementType> new_lines;

      typedef typename viennagrid::result_of::const_element_range<ElementType>::type ConstBoundaryElementRangeType;
      typedef typename viennagrid::result_of::iterator<ConstBoundaryElementRangeType>::type ConstBoundaryElementRangeIterator;

      ConstBoundaryElementRangeType boundary_lines(*pit, 1);
      for (ConstBoundaryElementRangeIterator blit = boundary_lines.begin(); blit != boundary_lines.end(); ++blit)
      {
        std::vector<ElementType> const & current_new_lines = line_to_lines_mapping[*blit];
        std::copy( current_new_lines.begin(), current_new_lines.end(), std::back_inserter(new_lines) );
      }

      viennagrid::make_plc( output_mesh, new_lines.begin(), new_lines.end() );
    }
  }

}

#endif
