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

#include "viennagrid/domain/element_creation.hpp"

#include "viennagrid/refinement/triangle.hpp"
#include "viennagrid/refinement/tetrahedron.hpp"

/** @file refine.hpp
    @brief Provides the routines for a refinement of a domain
*/

namespace viennagrid
{
  namespace detail
  {
    /** @brief For internal use only */
    template <typename CellTagInT,
              typename WrappedDomainConfigInT,
              typename WrappedDomainConfigOutT,
              typename PointAccessorT,
              typename EdgeRefinementFlagAccessorT, typename VertexToVertexHandleAccessorT, typename RefinementVertexAccessorT>
    void refine_impl(domain_t<WrappedDomainConfigInT> const & domain_in,
                     domain_t<WrappedDomainConfigOutT> & domain_out,
                     PointAccessorT const point_accessor_in,
                     EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor,
                     VertexToVertexHandleAccessorT vertex_to_vertex_handle_accessor,
                     RefinementVertexAccessorT edge_to_vertex_handle_accessor)
    {
      typedef domain_t<WrappedDomainConfigInT>       WrappedDomainConfigInType;
      typedef domain_t<WrappedDomainConfigOutT>      WrappedDomainConfigOutType;
      
      typedef typename viennagrid::result_of::point<WrappedDomainConfigInType>::type     PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type        NumericType;

      typedef typename viennagrid::result_of::element<WrappedDomainConfigInType, vertex_tag>::type       VertexType;
      typedef typename VertexType::id_type                                                  VertexIDType;
      typedef typename viennagrid::result_of::handle<WrappedDomainConfigInType, vertex_tag>::type        VertexHandleType;
      typedef typename viennagrid::result_of::element<WrappedDomainConfigInType, line_tag>::type         EdgeType;
      typedef typename viennagrid::result_of::element<WrappedDomainConfigInType, CellTagInT>::type       CellType;

      typedef typename viennagrid::result_of::const_element_range<WrappedDomainConfigInType, vertex_tag>::type       VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                               VertexIterator;
      typedef typename viennagrid::result_of::const_element_range<WrappedDomainConfigInType, line_tag>::type         EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                 EdgeIterator;
      typedef typename viennagrid::result_of::const_element_range<WrappedDomainConfigInType, CellTagInT>::type       CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                 CellIterator;
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type           VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type                         VertexOnCellIterator;
      typedef typename viennagrid::result_of::const_element_range<EdgeType, vertex_tag>::type           VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                         VertexOnEdgeIterator;

      typedef typename viennagrid::result_of::element<WrappedDomainConfigOutType, vertex_tag>::type      VertexTypeOut;
      typedef typename VertexTypeOut::id_type                                               VertexIDTypeOut;

      //
      // Step 2: Write old vertices to new domain
      //
      VertexRange vertices = viennagrid::elements(domain_in);
      for (VertexIterator vit  = vertices.begin();
                          vit != vertices.end();
                        ++vit)
      {
        vertex_to_vertex_handle_accessor( *vit ) = viennagrid::make_vertex( domain_out, point_accessor_in(*vit) );
      }

      //
      // Step 3: Each tagged edge in old domain results in a new vertex (temporarily store new vertex IDs on old domain)
      //
      EdgeRange edges = viennagrid::elements(domain_in);
      for (EdgeIterator eit = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        if ( edge_refinement_flag_accessor(*eit) )
        {
          edge_to_vertex_handle_accessor( *eit ) = viennagrid::make_vertex( domain_out, viennagrid::centroid(point_accessor_in, *eit) );
        }
      }

      //
      // Step 4: Now write new cells to new domain
      //
        CellRange cells = viennagrid::elements(domain_in);
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
        {
          element_refinement<CellTagInT>::apply(*cit, domain_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
        }
    }




    /** @brief For internal use only */
    template <typename CellTagInT,
              typename WrappedDomainConfigInT,  typename WrappedSegmentationConfigInT,
              typename WrappedDomainConfigOutT, typename WrappedSegmentationConfigOutT,
              typename PointAccessorT,
              typename EdgeRefinementFlagAccessorT, typename VertexToVertexHandleAccessorT, typename RefinementVertexAccessorT>
    void refine_impl(domain_t<WrappedDomainConfigInT> const & domain_in,    segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
                     domain_t<WrappedDomainConfigOutT>      & domain_out,   segmentation_t<WrappedSegmentationConfigOutT>      & segmentation_out,
                     PointAccessorT const point_accessor_in,
                     EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor,
                     VertexToVertexHandleAccessorT vertex_to_vertex_handle_accessor,
                     RefinementVertexAccessorT edge_to_vertex_handle_accessor)
    {
      typedef domain_t<WrappedDomainConfigInT>    WrappedDomainConfigInType;
      typedef domain_t<WrappedDomainConfigOutT>   WrappedDomainConfigOutType;

      typedef segmentation_t<WrappedSegmentationConfigInT>      WrappedSegmentationConfigInType;
      typedef segmentation_t<WrappedSegmentationConfigOutT>     WrappedSegmentationConfigOutType;

      typedef typename viennagrid::result_of::segment<WrappedSegmentationConfigInType>::type     SegmentInType;
      typedef typename viennagrid::result_of::segment<WrappedSegmentationConfigOutType>::type    SegmentOutType;

      
      typedef typename viennagrid::result_of::point<WrappedDomainConfigInType>::type       PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type          NumericType;

      typedef typename viennagrid::result_of::element<WrappedDomainConfigInType, vertex_tag>::type         VertexType;
      typedef typename VertexType::id_type                                                    VertexIDType;
      typedef typename viennagrid::result_of::handle<WrappedDomainConfigInType, vertex_tag>::type          VertexHandleType;
      typedef typename viennagrid::result_of::element<WrappedDomainConfigInType, line_tag>::type           EdgeType;
      typedef typename viennagrid::result_of::element<WrappedDomainConfigInType, CellTagInT>::type         CellType;

      typedef typename viennagrid::result_of::const_element_range<WrappedDomainConfigInType, vertex_tag>::type     VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                             VertexIterator;
      typedef typename viennagrid::result_of::const_element_range<WrappedDomainConfigInType, line_tag>::type       EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                               EdgeIterator;
      typedef typename viennagrid::result_of::const_element_range<SegmentInType, CellTagInT>::type    CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                               CellIterator;
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type         VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type                       VertexOnCellIterator;
      typedef typename viennagrid::result_of::const_element_range<EdgeType, vertex_tag>::type         VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                       VertexOnEdgeIterator;

      typedef typename viennagrid::result_of::element<WrappedDomainConfigOutType, vertex_tag>::type    VertexTypeOut;
      typedef typename VertexTypeOut::id_type                                             VertexIDTypeOut;

      //
      // Step 2: Write old vertices to new domain
      //
      VertexRange vertices = viennagrid::elements(domain_in);
      for (VertexIterator vit  = vertices.begin();
                          vit != vertices.end();
                        ++vit)
      {
        vertex_to_vertex_handle_accessor( *vit ) = viennagrid::make_vertex( domain_out, point_accessor_in(*vit) );
      }

      //
      // Step 3: Each tagged edge in old domain results in a new vertex (temporarily store new vertex IDs on old domain)
      //
      EdgeRange edges = viennagrid::elements(domain_in);
      for (EdgeIterator eit = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        if ( edge_refinement_flag_accessor(*eit) )
        {
          edge_to_vertex_handle_accessor( *eit ) = viennagrid::make_vertex( domain_out, viennagrid::centroid(point_accessor_in, *eit) );
        }
      }

      //
      // Step 4: Now write new cells to new domain
      //

      // TODO: fix overlapping segments (with this implementation overlapping segments would result in multiple elements)
      for (typename WrappedSegmentationConfigInType::const_iterator sit = segmentation_in.begin(); sit != segmentation_in.end(); ++sit)
      {
        SegmentInType  const & segment_in  = *sit;
        SegmentOutType       & segment_out = segmentation_out( sit->id() );
        
        CellRange cells = viennagrid::elements(segment_in);
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
        {
          element_refinement<CellTagInT>::apply(*cit, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
        }
      }
    }


  } //namespace detail



    /** @brief Ensures refinement fo the longest edge. If any edge is refined in a cell, then the longest edge is refined as well. */
    template <typename CellTagIn, typename WrappedDomainConfigInT, typename EdgeRefinementFlagAccessor>
    void ensure_longest_edge_refinement(domain_t<WrappedDomainConfigInT> const & domain_in, EdgeRefinementFlagAccessor edge_refinement_flag_accessor)
    {
      typedef domain_t<WrappedDomainConfigInT>                                            DomainInType;
      
      typedef typename viennagrid::result_of::element<DomainInType, vertex_tag>::type     VertexType;
      typedef typename viennagrid::result_of::element<DomainInType, line_tag>::type       EdgeType;
      typedef typename viennagrid::result_of::element<DomainInType, CellTagIn>::type      CellType;

      typedef typename viennagrid::result_of::const_element_range<DomainInType, CellTagIn>::type    CellRange;
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

        CellRange cells = viennagrid::elements<CellTagIn>(domain_in);
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

              double len = viennagrid::norm( viennagrid::point(domain_in, v0) - viennagrid::point(domain_in, v1) );
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
    template <typename CellTagIn, typename WrappedDomainConfigInT, typename CellRefinementFlagAccessor, typename EdgeRefinementFlagAccessor>
    void cell_refinement_to_edge_refinement(domain_t<WrappedDomainConfigInT> const & domain_in,
                                            CellRefinementFlagAccessor cell_refinement_flag, EdgeRefinementFlagAccessor edge_refinement_flag_accessor)
    {
      typedef domain_t<WrappedDomainConfigInT>                                                        DomainInType;
      
      typedef typename viennagrid::result_of::element<DomainInType, CellTagIn>::type                  CellType;

      typedef typename viennagrid::result_of::const_element_range<DomainInType, CellTagIn>::type      CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                               CellIterator;
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type           EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                         EdgeOnCellIterator;

      std::size_t cells_for_refinement = 0;
      CellRange cells = viennagrid::elements(domain_in);
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
    template<typename WrappedDomainConfigInT, typename EdgeRefinementFlagAccessor>
    void mark_all_edge_refinement(domain_t<WrappedDomainConfigInT> const & domain_in, EdgeRefinementFlagAccessor edge_refinement_flag_accessor)
    {
      typedef domain_t<WrappedDomainConfigInT>                                                                DomainInType;
      
      typedef typename viennagrid::result_of::const_element_range<DomainInType, viennagrid::line_tag>::type   LineRange;
      typedef typename viennagrid::result_of::iterator<LineRange>::type                                       LineIterator;

      LineRange lines = viennagrid::elements(domain_in);
      for (LineIterator it = lines.begin(); it != lines.end(); ++it)
        edge_refinement_flag_accessor(*it) = true;
    }








    

  /** @brief Public interface for refinement of a domain with explicit point accessor, edge refinement accessor and temporary accessors
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param domain_out                        Output refined domain
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   * @param vertex_to_vertex_handle_accessor  Temporary accessor for vertex to vertex mapping
   * @param edge_to_vertex_handle_accessor    Temporary accessor for refined edge to vertex mapping
   */
  template <typename ElementTypeOrTagT, typename WrappedDomainConfigInT, typename WrappedDomainConfigOutT, typename PointAccessorType,
            typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename RefinementVertexAccessor>
  void refine(domain_t<WrappedDomainConfigInT> const & domain_in,
              domain_t<WrappedDomainConfigOutT> & domain_out, PointAccessorType point_accessor_in,
              EdgeRefinementFlagAccessor const edge_refinement_flag_accessor,
              VertexToVertexHandleAccessor vertex_to_vertex_handle_accessor, RefinementVertexAccessor edge_to_vertex_handle_accessor)
  {
      typedef typename viennagrid::result_of::element_tag<ElementTypeOrTagT>::type CellTag;

      detail::refine_impl<CellTag>(domain_in, domain_out, point_accessor_in,
                                  edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
  }


  /** @brief Public interface for refinement of a domain with explicit point accessor and edge refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param domain_out                        Output refined domain
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedDomainConfigInT, typename WrappedDomainConfigOutT, typename PointAccessorType, typename EdgeRefinementFlagAccessor>
  void refine(domain_t<WrappedDomainConfigInT> const & domain_in,
              domain_t<WrappedDomainConfigOutT> & domain_out,
              PointAccessorType point_accessor_in, EdgeRefinementFlagAccessor const edge_refinement_flag_accessor)
  {
    typedef domain_t<WrappedDomainConfigInT>                                              DomainInType;
    typedef domain_t<WrappedDomainConfigOutT>                                             DomainOutType;
    
    typedef typename viennagrid::result_of::vertex<DomainOutType>::type                   VertexType;
    typedef typename viennagrid::result_of::line<DomainOutType>::type                     EdgeType;

    typedef typename viennagrid::result_of::vertex_handle<DomainOutType>::type            VertexHandleType;

    std::deque<VertexHandleType> vertex_refinement_vertex_handle;
    std::deque<VertexHandleType> edge_refinement_vertex_handle;

    refine<ElementTypeOrTagT>(domain_in, domain_out, point_accessor_in,
                    edge_refinement_flag_accessor,
                    viennagrid::make_accessor<VertexType>(vertex_refinement_vertex_handle),
                    viennagrid::make_accessor<EdgeType>(edge_refinement_vertex_handle));
  }

  /** @brief Public interface for refinement of a domain with edge refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param domain_out                        Output refined domain
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedDomainConfigInT, typename WrappedDomainConfigOutT, typename EdgeRefinementFlagAccessor>
  void refine(domain_t<WrappedDomainConfigInT> const & domain_in,
              domain_t<WrappedDomainConfigOutT> & domain_out,
              EdgeRefinementFlagAccessor const edge_refinement_flag_accessor)
  {
    typedef domain_t<WrappedDomainConfigInT>                                              DomainInType;
    typedef domain_t<WrappedDomainConfigOutT>                                             DomainOutType;
    
    typedef typename viennagrid::result_of::vertex<DomainOutType>::type                   VertexType;
    typedef typename viennagrid::result_of::line<DomainOutType>::type                     EdgeType;

    typedef typename viennagrid::result_of::vertex_handle<DomainOutType>::type            VertexHandleType;

    std::deque<VertexHandleType> vertex_refinement_vertex_handle;
    std::deque<VertexHandleType> edge_refinement_vertex_handle;

    refine<ElementTypeOrTagT>(domain_in, domain_out, default_point_accessor(domain_in), edge_refinement_flag_accessor);
  }



  /** @brief Public interface for refinement of a domain with explicit point accessor and cell refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param domain_out                        Output refined domain
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedDomainConfigInT, typename WrappedDomainConfigOutT, typename PointAccessorType, typename CellRefinementFlagAccessor>
  void element_refine(domain_t<WrappedDomainConfigInT> const & domain_in,
                      domain_t<WrappedDomainConfigOutT> & domain_out,
                      PointAccessorType point_accessor_in, CellRefinementFlagAccessor const cell_refinement_flag_accessor)
  {
    typedef domain_t<WrappedDomainConfigInT>                                              DomainInType;
    typedef domain_t<WrappedDomainConfigOutT>                                             DomainOutType;
    
    typedef typename viennagrid::result_of::cell<DomainOutType>::type                     CellType;
    typedef typename viennagrid::result_of::line<DomainOutType>::type                     EdgeType;

    std::deque<bool> edge_refinement_flag;

    edge_refinement_flag.resize( viennagrid::id_upper_bound<EdgeType>(domain_in).get() );

    cell_refinement_to_edge_refinement<ElementTypeOrTagT>( domain_in,
                                        cell_refinement_flag_accessor,
                                        viennagrid::make_accessor<EdgeType>(edge_refinement_flag));

    refine<ElementTypeOrTagT>(domain_in, domain_out, point_accessor_in,
                    viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  /** @brief Public interface for refinement of a domain with cell refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param domain_out                        Output refined domain
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedDomainConfigInT, typename WrappedDomainConfigOutT, typename CellRefinementFlagAccessor>
  void element_refine(domain_t<WrappedDomainConfigInT> const & domain_in,
                      domain_t<WrappedDomainConfigOutT> & domain_out,
                      CellRefinementFlagAccessor const cell_refinement_flag_accessor)
  {
    element_refine<ElementTypeOrTagT>(domain_in, domain_out, default_point_accessor(domain_in), cell_refinement_flag_accessor);
  }

  /** @brief Public interface for refinement of cells of a domain with cell refinement accessor. If there is more than one cell type this funcion will fail.
   * 
   * @param domain_in                         Input domain
   * @param domain_out                        Output refined domain
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename WrappedDomainConfigInT, typename WrappedDomainConfigOutT, typename CellRefinementFlagAccessor>
  void cell_refine(domain_t<WrappedDomainConfigInT> const & domain_in,
                   domain_t<WrappedDomainConfigOutT> & domain_out,
                   CellRefinementFlagAccessor const cell_refinement_flag_accessor)
  {
    typedef domain_t<WrappedDomainConfigInT>                            DomainInType;
    typedef typename viennagrid::result_of::cell<DomainInType>::type    CellType;
    
    element_refine<CellType>(domain_in, domain_out, default_point_accessor(domain_in), cell_refinement_flag_accessor);
  }




  /** @brief Public interface for uniform refinement of a domain with explicit point accessor, edge refinement accessor and temporary accessors.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param domain_out                        Output refined domain
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   * @param vertex_to_vertex_handle_accessor  Temporary accessor for vertex to vertex mapping
   * @param edge_to_vertex_handle_accessor    Temporary accessor for refined edge to vertex mapping
   */
  template <typename ElementTypeOrTagT, typename WrappedDomainConfigInT, typename WrappedDomainConfigOutT, typename PointAccessorType,
            typename EdgeRefinementFlagAccessor,  typename VertexToVertexHandleAccessor, typename RefinementVertexAccessor>
  void refine_uniformly(domain_t<WrappedDomainConfigInT> const & domain_in,
                        domain_t<WrappedDomainConfigOutT> & domain_out,
                        PointAccessorType point_accessor_in,
                        EdgeRefinementFlagAccessor const edge_refinement_flag_accessor,
                        VertexToVertexHandleAccessor vertex_to_vertex_handle_accessor, RefinementVertexAccessor edge_to_vertex_handle_accessor)
  {
    mark_all_edge_refinement( domain_in, edge_refinement_flag_accessor );
    refine<ElementTypeOrTagT>(domain_in, domain_out, point_accessor_in,
                          edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
  }

  /** @brief Public interface for uniform refinement of a domain with explicit point accessor and edge refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param domain_out                        Output refined domain
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedDomainConfigInT, typename WrappedDomainConfigOutT, typename PointAccessorType, typename EdgeRefinementFlagAccessor>
  void refine_uniformly(domain_t<WrappedDomainConfigInT> const & domain_in,
                        domain_t<WrappedDomainConfigOutT> & domain_out,
                        PointAccessorType point_accessor_in,
                        EdgeRefinementFlagAccessor const edge_refinement_flag_accessor)
  {
    mark_all_edge_refinement( domain_in, edge_refinement_flag_accessor );
    refine<ElementTypeOrTagT>(domain_in, domain_out, point_accessor_in, edge_refinement_flag_accessor);
  }

  /** @brief Public interface for uniform refinement of a domain with edge refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param domain_out                        Output refined domain
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT, typename WrappedDomainConfigInT, typename WrappedDomainConfigOutT, typename EdgeRefinementFlagAccessor>
  void refine_uniformly(domain_t<WrappedDomainConfigInT> const & domain_in,
                        domain_t<WrappedDomainConfigOutT> & domain_out,
                        EdgeRefinementFlagAccessor const edge_refinement_flag_accessor)
  {
    refine_uniformly<ElementTypeOrTagT>(domain_in, domain_out, default_point_accessor(domain_in), edge_refinement_flag_accessor);
  }

  /** @brief Public interface for uniform refinement of a domain.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param domain_out                        Output refined domain
   */
  template <typename ElementTypeOrTagT, typename WrappedDomainConfigInT, typename WrappedDomainConfigOutT>
  void refine_uniformly(domain_t<WrappedDomainConfigInT> const & domain_in,
                        domain_t<WrappedDomainConfigOutT> & domain_out)
  {
    typedef domain_t<WrappedDomainConfigOutT>                             DomainOutType;
    typedef typename viennagrid::result_of::line<DomainOutType>::type     EdgeType;
    std::deque<bool> edge_refinement_flag;

    refine_uniformly<ElementTypeOrTagT>(domain_in, domain_out, viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  /** @brief Public interface for uniform refinement of cells of a domain. Will fail if there is more than one cell type.
   * 
   * @param domain_in                         Input domain
   * @param domain_out                        Output refined domain
   */
  template <typename WrappedDomainConfigInT, typename WrappedDomainConfigOutT>
  void cell_refine_uniformly(domain_t<WrappedDomainConfigInT> const & domain_in,
                             domain_t<WrappedDomainConfigOutT> & domain_out)
  {
    typedef domain_t<WrappedDomainConfigInT>                            DomainInType;
    typedef typename viennagrid::result_of::cell<DomainInType>::type    CellType;
    
    refine_uniformly<CellType>(domain_in, domain_out);
  }






  /** @brief Public interface for refinement of a domain with segmentation providing explicit point accessor, edge refinement accessor and temporary accessors
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param segmentation_in                   Input segmentation
   * @param domain_out                        Output refined domain
   * @param segmentation_out                  Output refined segmentation
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   * @param vertex_to_vertex_handle_accessor  Temporary accessor for vertex to vertex mapping
   * @param edge_to_vertex_handle_accessor    Temporary accessor for refined edge to vertex mapping
   */
  template <typename ElementTypeOrTagT,
            typename WrappedDomainConfigInT, typename WrappedSegmentationConfigInT,
            typename WrappedDomainConfigOutT, typename WrappedSegmentationConfigOutT,
            typename PointAccessorT,
            typename EdgeRefinementFlagAccessorT, typename VertexToVertexHandleAccessorT, typename RefinementVertexAccessorT>
  void refine(domain_t<WrappedDomainConfigInT> const & domain_in,  segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
              domain_t<WrappedDomainConfigOutT> & domain_out,      segmentation_t<WrappedSegmentationConfigOutT> & segmentation_out,
              PointAccessorT point_accessor_in,
              EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor, VertexToVertexHandleAccessorT vertex_to_vertex_handle_accessor, RefinementVertexAccessorT edge_to_vertex_handle_accessor)
  {
      typedef typename viennagrid::result_of::element_tag<ElementTypeOrTagT>::type CellTag;

      detail::refine_impl<CellTag>(domain_in, segmentation_in,
                                   domain_out, segmentation_out,
                                   point_accessor_in,
                                   edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
  }


  /** @brief Public interface for refinement of a domain with segmentation providing explicit point accessor and edge refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param segmentation_in                   Input segmentation
   * @param domain_out                        Output refined domain
   * @param segmentation_out                  Output refined segmentation
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedDomainConfigInT,  typename WrappedSegmentationConfigInT,
            typename WrappedDomainConfigOutT, typename WrappedSegmentationConfigOutT,
            typename PointAccessorT,
            typename EdgeRefinementFlagAccessorT>
  void refine(domain_t<WrappedDomainConfigInT> const & domain_in,  segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
              domain_t<WrappedDomainConfigOutT> & domain_out,      segmentation_t<WrappedSegmentationConfigOutT> & segmentation_out,
              PointAccessorT point_accessor_in,
              EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor)
  {
    typedef domain_t<WrappedDomainConfigInT> DomainInT;
    typedef domain_t<WrappedDomainConfigOutT> DomainOutT;
    
    typedef typename viennagrid::result_of::vertex<DomainOutT>::type VertexType;
    typedef typename viennagrid::result_of::line<DomainOutT>::type EdgeType;

    typedef typename viennagrid::result_of::vertex_handle<DomainOutT>::type VertexHandleType;

    std::deque<VertexHandleType> vertex_refinement_vertex_handle;
    std::deque<VertexHandleType> edge_refinement_vertex_handle;

    refine<ElementTypeOrTagT>(domain_in, segmentation_in,
                           domain_out, segmentation_out,
                           point_accessor_in,
                           edge_refinement_flag_accessor,
                           viennagrid::make_accessor<VertexType>(vertex_refinement_vertex_handle),
                           viennagrid::make_accessor<EdgeType>(edge_refinement_vertex_handle));
  }

  /** @brief Public interface for refinement of a domain with segmentation providing edge refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param segmentation_in                   Input segmentation
   * @param domain_out                        Output refined domain
   * @param segmentation_out                  Output refined segmentation
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedDomainConfigInT,  typename WrappedSegmentationConfigInT,
            typename WrappedDomainConfigOutT, typename WrappedSegmentationConfigOutT,
            typename EdgeRefinementFlagAccessor>
  void refine(domain_t<WrappedDomainConfigInT> const & domain_in,  segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
              domain_t<WrappedDomainConfigOutT> & domain_out,      segmentation_t<WrappedSegmentationConfigOutT> & segmentation_out,
              EdgeRefinementFlagAccessor const edge_refinement_flag_accessor)
  {
    typedef domain_t<WrappedDomainConfigInT> DomainInT;
    typedef domain_t<WrappedDomainConfigOutT> DomainOutT;
    
    typedef typename viennagrid::result_of::vertex<DomainOutT>::type VertexType;
    typedef typename viennagrid::result_of::line<DomainOutT>::type EdgeType;

    typedef typename viennagrid::result_of::vertex_handle<DomainOutT>::type VertexHandleType;

    std::deque<VertexHandleType> vertex_refinement_vertex_handle;
    std::deque<VertexHandleType> edge_refinement_vertex_handle;

    refine<ElementTypeOrTagT>(domain_in, segmentation_in,
                           domain_out, segmentation_out,
                           default_point_accessor(domain_in),
                           edge_refinement_flag_accessor);
  }



  /** @brief Public interface for refinement of a domain with segmentation providing explicit point accessor and cell refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param segmentation_in                   Input segmentation
   * @param domain_out                        Output refined domain
   * @param segmentation_out                  Output refined segmentation
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedDomainConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedDomainConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename PointAccessorT,
            typename CellRefinementFlagAccessorT>
  void element_refine(domain_t<WrappedDomainConfigInT> const & domain_in,  segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
                      domain_t<WrappedDomainConfigOutT> & domain_out,      segmentation_t<WrappedSegmentationConfigOutT> & segmentation_out,
                      PointAccessorT point_accessor_in,
                      CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    typedef domain_t<WrappedDomainConfigInT> DomainInT;
    typedef domain_t<WrappedDomainConfigOutT> DomainOutT;
    
    typedef typename viennagrid::result_of::line<DomainOutT>::type EdgeType;
    typedef typename viennagrid::result_of::cell<DomainOutT>::type CellType;

    std::deque<bool> edge_refinement_flag;

    edge_refinement_flag.resize( viennagrid::id_upper_bound<EdgeType>(domain_in).get() );

    cell_refinement_to_edge_refinement<ElementTypeOrTagT>(
                                        domain_in,
                                        cell_refinement_flag_accessor,
                                        viennagrid::make_accessor<EdgeType>(edge_refinement_flag));

    refine<ElementTypeOrTagT>(domain_in, segmentation_in,
                           domain_out, segmentation_out,
                           point_accessor_in,
                           viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  /** @brief Public interface for refinement of a domain with segmentation providing cell refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param segmentation_in                   Input segmentation
   * @param domain_out                        Output refined domain
   * @param segmentation_out                  Output refined segmentation
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedDomainConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedDomainConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename CellRefinementFlagAccessorT>
  void element_refine(domain_t<WrappedDomainConfigInT> const & domain_in,  segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
                      domain_t<WrappedDomainConfigOutT> & domain_out,      segmentation_t<WrappedSegmentationConfigOutT> & segmentation_out,
                      CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    element_refine<ElementTypeOrTagT>(domain_in, segmentation_in,
                                   domain_out, segmentation_out,
                                   default_point_accessor(domain_in),
                                   cell_refinement_flag_accessor);
  }

  /** @brief Public interface for refinement of cells of a domain with segmentation providing cell refinement accessor. Will fail if there is more than one cell type.
   * 
   * @param domain_in                         Input domain
   * @param segmentation_in                   Input segmentation
   * @param domain_out                        Output refined domain
   * @param segmentation_out                  Output refined segmentation
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename WrappedDomainConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedDomainConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename CellRefinementFlagAccessorT>
  void cell_refine(domain_t<WrappedDomainConfigInT> const & domain_in,  segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
                   domain_t<WrappedDomainConfigOutT> & domain_out,      segmentation_t<WrappedSegmentationConfigOutT> & segmentation_out,
                   CellRefinementFlagAccessorT const cell_refinement_flag_accessor)
  {
    typedef domain_t<WrappedDomainConfigInT>                              DomainInType;
    typedef typename viennagrid::result_of::cell_tag<DomainInType>::type  CellTag;
    element_refine<CellTag>(domain_in, segmentation_in,
                            domain_out, segmentation_out,
                            default_point_accessor(domain_in),
                            cell_refinement_flag_accessor);
  }






  /** @brief Public interface for uniform refinement of a domain with segmentation providing explicit point accessor, edge refinement accessor and temporary accessors.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param segmentation_in                   Input segmentation
   * @param domain_out                        Output refined domain
   * @param segmentation_out                  Output refined segmentation
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   * @param vertex_to_vertex_handle_accessor  Temporary accessor for vertex to vertex mapping
   * @param edge_to_vertex_handle_accessor    Temporary accessor for refined edge to vertex mapping
   */
  template <typename ElementTypeOrTagT,
            typename WrappedDomainConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedDomainConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename PointAccessorT,
            typename EdgeRefinementFlagAccessorT, typename VertexToVertexHandleAccessorT, typename RefinementVertexAccessorT>
  void refine_uniformly(domain_t<WrappedDomainConfigInT> const & domain_in,  segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
                        domain_t<WrappedDomainConfigOutT> & domain_out,      segmentation_t<WrappedSegmentationConfigOutT> & segmentation_out,
                        PointAccessorT point_accessor_in,
                        EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor,
                        VertexToVertexHandleAccessorT vertex_to_vertex_handle_accessor,
                        RefinementVertexAccessorT edge_to_vertex_handle_accessor)
  {
    mark_all_edge_refinement( domain_in, edge_refinement_flag_accessor );
    refine<ElementTypeOrTagT>(domain_in, segmentation_in,
                           domain_out, segmentation_out,
                           point_accessor_in,
                           edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
  }

  /** @brief Public interface for uniform refinement of a domain with segmentation providing explicit point accessor and edge refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param segmentation_in                   Input segmentation
   * @param domain_out                        Output refined domain
   * @param segmentation_out                  Output refined segmentation
   * @param point_accessor_in                 Point accessor for input points
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedDomainConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedDomainConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename PointAccessorT,
            typename EdgeRefinementFlagAccessorT>
  void refine_uniformly(domain_t<WrappedDomainConfigInT> const & domain_in,  segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
                        domain_t<WrappedDomainConfigOutT> & domain_out,      segmentation_t<WrappedSegmentationConfigOutT> & segmentation_out,
                        PointAccessorT point_accessor_in,
                        EdgeRefinementFlagAccessorT const edge_refinement_flag_accessor)
  {
    mark_all_edge_refinement( domain_in, edge_refinement_flag_accessor );
    refine<ElementTypeOrTagT>(domain_in, segmentation_in,
                           domain_out, segmentation_out,
                           point_accessor_in,
                           edge_refinement_flag_accessor);
  }

  /** @brief Public interface for uniform refinement of a domain with segmentation providing edge refinement accessor.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param segmentation_in                   Input segmentation
   * @param domain_out                        Output refined domain
   * @param segmentation_out                  Output refined segmentation
   * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
   */
  template <typename ElementTypeOrTagT,
            typename WrappedDomainConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedDomainConfigOutT,  typename WrappedSegmentationConfigOutT,
            typename EdgeRefinementFlagAccessor>
  void refine_uniformly(domain_t<WrappedDomainConfigInT> const & domain_in,  segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
                        domain_t<WrappedDomainConfigOutT> & domain_out,      segmentation_t<WrappedSegmentationConfigOutT> & segmentation_out,
                        EdgeRefinementFlagAccessor const edge_refinement_flag_accessor)
  {
    refine_uniformly<ElementTypeOrTagT>(domain_in, domain_out, default_point_accessor(domain_in), edge_refinement_flag_accessor);
  }

  /** @brief Public interface for uniform refinement of a domain with segmentation.
   * 
   * @param ElementTypeOrTagT                 The element type/tag which elements are refined
   * @param domain_in                         Input domain
   * @param segmentation_in                   Input segmentation
   * @param domain_out                        Output refined domain
   * @param segmentation_out                  Output refined segmentation
   */
  template <typename ElementTypeOrTagT,
            typename WrappedDomainConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedDomainConfigOutT,  typename WrappedSegmentationConfigOutT>
  void refine_uniformly(domain_t<WrappedDomainConfigInT> const & domain_in,  segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
                        domain_t<WrappedDomainConfigOutT> & domain_out,      segmentation_t<WrappedSegmentationConfigOutT> & segmentation_out)
  {
    typedef domain_t<WrappedDomainConfigOutT>                           DomainOutType;
    typedef typename viennagrid::result_of::line<DomainOutType>::type   EdgeType;
    std::deque<bool> edge_refinement_flag;

    refine_uniformly<ElementTypeOrTagT>(domain_in, domain_out, viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  /** @brief Public interface for uniform refinement of cells of a domain with segmentation. Will fail if there is more than one cell type.
   * 
   * @param domain_in                         Input domain
   * @param segmentation_in                   Input segmentation
   * @param domain_out                        Output refined domain
   * @param segmentation_out                  Output refined segmentation
   */
  template <typename WrappedDomainConfigInT,   typename WrappedSegmentationConfigInT,
            typename WrappedDomainConfigOutT,  typename WrappedSegmentationConfigOutT>
  void cell_refine_uniformly(domain_t<WrappedDomainConfigInT> const & domain_in,  segmentation_t<WrappedSegmentationConfigInT> const & segmentation_in,
                             domain_t<WrappedDomainConfigOutT> & domain_out,      segmentation_t<WrappedSegmentationConfigOutT> & segmentation_out)
  {
    typedef domain_t<WrappedDomainConfigInT>                            DomainInType;
    typedef typename viennagrid::result_of::cell<DomainInType>::type    CellType;
    refine_uniformly<CellType>(domain_in, domain_out);
  }

  
}

#endif
