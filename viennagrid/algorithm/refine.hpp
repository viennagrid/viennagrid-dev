#ifndef VIENNAGRID_ALGORITHM_REFINE_HPP
#define VIENNAGRID_ALGORITHM_REFINE_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at

   (A list of additional contributors can be found in the PDF manual)

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
    /** @brief Implementation of adaptive refinement. Responsible for all the book-keeping */
    template <typename CellTagIn, typename DomainTypeIn, typename DomainTypeOut, typename PointAccessorType,
              typename VertexToVertexHandleAccessor, typename EdgeRefinementFlagAccessor, typename RefinementVertexAccessor>
    void refine_impl(DomainTypeIn const & domain_in, DomainTypeOut & domain_out, PointAccessorType const point_accessor_in,
                     EdgeRefinementFlagAccessor const edge_refinement_flag_accessor,
                     VertexToVertexHandleAccessor vertex_to_vertex_handle_accessor, RefinementVertexAccessor edge_to_vertex_handle_accessor)
    {
      typedef typename viennagrid::result_of::point<DomainTypeIn>::type PointType;

      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

      typedef typename viennagrid::result_of::element<DomainTypeIn, vertex_tag>::type                                      VertexType;
      typedef typename VertexType::id_type                                      VertexIDType;
      typedef typename viennagrid::result_of::handle<DomainTypeIn, vertex_tag>::type                                      VertexHandleType;
      typedef typename viennagrid::result_of::element<DomainTypeIn, line_tag>::type                                      EdgeType;
      typedef typename viennagrid::result_of::element<DomainTypeIn, CellTagIn>::type              CellType;

      typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, vertex_tag>::type                           VertexRange;
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                                        VertexIterator;
      typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, line_tag>::type                           EdgeRange;
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                          EdgeIterator;
      typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, CellTagIn>::type  CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                          CellIterator;
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type                               VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type                                  VertexOnCellIterator;
      typedef typename viennagrid::result_of::const_element_range<EdgeType, vertex_tag>::type                               VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                                  VertexOnEdgeIterator;

      typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
      typedef typename VertexTypeOut::id_type VertexIDTypeOut;

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
          element_refinement<CellTagIn>::apply(*cit, domain_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
        }
    }



  } //namespace detail



    /** @brief Ensures refinement fo the longest edge. If any edge is refined in a cell, then the longest edge is refined as well. */
    template <typename CellTagIn, typename DomainTypeIn, typename EdgeRefinementFlagAccessor>
    void ensure_longest_edge_refinement(DomainTypeIn const & domain_in, EdgeRefinementFlagAccessor edge_refinement_flag_accessor)
    {

      typedef typename viennagrid::result_of::element<DomainTypeIn, vertex_tag>::type                                      VertexType;
      typedef typename viennagrid::result_of::element<DomainTypeIn, line_tag>::type                                      EdgeType;
      typedef typename viennagrid::result_of::element<DomainTypeIn, CellTagIn>::type              CellType;

      typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, CellTagIn>::type   CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                          CellIterator;
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type                               EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                                    EdgeOnCellIterator;
      typedef typename viennagrid::result_of::const_element_range<EdgeType, vertex_tag>::type                               VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                                  VertexOnEdgeIterator;


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
    template <typename CellTagIn, typename DomainTypeIn, typename CellRefinementFlagAccessor, typename EdgeRefinementFlagAccessor>
    void cell_refinement_to_edge_refinement(DomainTypeIn const & domain_in,
                                            CellRefinementFlagAccessor cell_refinement_flag, EdgeRefinementFlagAccessor edge_refinement_flag_accessor)
    {
      typedef typename viennagrid::result_of::element<DomainTypeIn, CellTagIn>::type              CellType;

      typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, CellTagIn>::type   CellRange;
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                          CellIterator;
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type                               EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                                    EdgeOnCellIterator;

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


    template<typename DomainTypeIn, typename EdgeRefinementFlagAccessor>
    void mark_all_edge_refinement(DomainTypeIn const & domain_in, EdgeRefinementFlagAccessor edge_refinement_flag_accessor)
    {
      typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, viennagrid::line_tag>::type LineRange;
      typedef typename viennagrid::result_of::iterator<LineRange>::type LineIterator;

      LineRange lines = viennagrid::elements(domain_in);
      edge_refinement_flag_accessor.resize(lines.size());
      for (LineIterator it = lines.begin(); it != lines.end(); ++it)
        edge_refinement_flag_accessor(*it) = true;
    }




  /** @brief Public interface for refinement of a domain. If local refinement is desired, cells or edges needs to be tagged using ViennaData with refinement_key.*/
  template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut, typename PointAccessorType,
            typename EdgeRefinementFlagAccessor,  typename VertexToVertexHandleAccessor, typename RefinementVertexAccessor>
  void refine(DomainTypeIn const & domain_in, DomainTypeOut & domain_out, PointAccessorType point_accessor_in,
              EdgeRefinementFlagAccessor const edge_refinement_flag_accessor,
              VertexToVertexHandleAccessor vertex_to_vertex_handle_accessor, RefinementVertexAccessor edge_to_vertex_handle_accessor)
  {
      typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;

      detail::refine_impl<CellTag>(domain_in, domain_out, point_accessor_in,
                                  edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
  }


  /** @brief Public interface for refinement of a domain. If local refinement is desired, cells or edges needs to be tagged using ViennaData with refinement_key.*/
  template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut, typename PointAccessorType, typename EdgeRefinementFlagAccessor>
  void refine(DomainTypeIn const & domain_in, DomainTypeOut & domain_out, PointAccessorType point_accessor_in,
              EdgeRefinementFlagAccessor const edge_refinement_flag_accessor)
  {
    typedef typename viennagrid::result_of::vertex<DomainTypeOut>::type VertexType;
    typedef typename viennagrid::result_of::line<DomainTypeOut>::type EdgeType;

    typedef typename viennagrid::result_of::vertex_handle<DomainTypeOut>::type VertexHandleType;

    std::deque<VertexHandleType> vertex_refinement_vertex_handle;
    std::deque<VertexHandleType> edge_refinement_vertex_handle;

    refine<CellTypeOrTag>(domain_in, domain_out, point_accessor_in,
                    edge_refinement_flag_accessor,
                    viennagrid::make_accessor<VertexType>(vertex_refinement_vertex_handle),
                    viennagrid::make_accessor<EdgeType>(edge_refinement_vertex_handle));
  }

  /** @brief Public interface for refinement of a domain. If local refinement is desired, cells or edges needs to be tagged using ViennaData with refinement_key.*/
  template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut, typename EdgeRefinementFlagAccessor>
  void refine(DomainTypeIn const & domain_in, DomainTypeOut & domain_out, EdgeRefinementFlagAccessor const edge_refinement_flag_accessor)
  {
    typedef typename viennagrid::result_of::vertex<DomainTypeOut>::type VertexType;
    typedef typename viennagrid::result_of::line<DomainTypeOut>::type EdgeType;

    typedef typename viennagrid::result_of::vertex_handle<DomainTypeOut>::type VertexHandleType;

    std::deque<VertexHandleType> vertex_refinement_vertex_handle;
    std::deque<VertexHandleType> edge_refinement_vertex_handle;

    refine<CellTypeOrTag>(domain_in, domain_out, default_point_accessor(domain_in), edge_refinement_flag_accessor);
  }



  /** @brief Public interface for refinement of a domain. If local refinement is desired, cells or edges needs to be tagged using ViennaData with refinement_key.*/
  template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut, typename PointAccessorType, typename CellRefinementFlagAccessor>
  void refine_element(DomainTypeIn const & domain_in, DomainTypeOut & domain_out, PointAccessorType point_accessor_in,
              CellRefinementFlagAccessor const cell_refinement_flag_accessor)
  {
    typedef typename viennagrid::result_of::line<DomainTypeOut>::type EdgeType;
    typedef typename viennagrid::result_of::cell<DomainTypeOut>::type CellType;

    std::deque<bool> edge_refinement_flag;

    edge_refinement_flag.resize( viennagrid::max_id<EdgeType>(domain_in).get() );

    cell_refinement_to_edge_refinement<CellTypeOrTag>( domain_in,
                                        cell_refinement_flag_accessor,
                                        viennagrid::make_accessor<EdgeType>(edge_refinement_flag));

    refine<CellTypeOrTag>(domain_in, domain_out, point_accessor_in,
                    viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  /** @brief Public interface for refinement of a domain. If local refinement is desired, cells or edges needs to be tagged using ViennaData with refinement_key.*/
  template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut, typename CellRefinementFlagAccessor>
  void refine_element(DomainTypeIn const & domain_in, DomainTypeOut & domain_out, CellRefinementFlagAccessor const cell_refinement_flag_accessor)
  {
    refine_element<CellTypeOrTag>(domain_in, domain_out, default_point_accessor(domain_in), cell_refinement_flag_accessor);
  }






  /** @brief Convenience overload for uniform refinement of a domain.  */
  template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut, typename PointAccessorType,
            typename EdgeRefinementFlagAccessor,  typename VertexToVertexHandleAccessor, typename RefinementVertexAccessor>
  void refine_uniformly(DomainTypeIn const & domain_in, DomainTypeOut & domain_out, PointAccessorType point_accessor_in,
                        EdgeRefinementFlagAccessor const edge_refinement_flag_accessor,
                        VertexToVertexHandleAccessor vertex_to_vertex_handle_accessor, RefinementVertexAccessor edge_to_vertex_handle_accessor)
  {
    mark_all_edge_refinement( domain_in, edge_refinement_flag_accessor );
    refine<CellTypeOrTag>(domain_in, domain_out, point_accessor_in,
                          edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
  }

  template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut, typename PointAccessorType, typename EdgeRefinementFlagAccessor>
  void refine_uniformly(DomainTypeIn const & domain_in, DomainTypeOut & domain_out, PointAccessorType point_accessor_in,
                        EdgeRefinementFlagAccessor const edge_refinement_flag_accessor)
  {
    mark_all_edge_refinement( domain_in, edge_refinement_flag_accessor );
    refine<CellTypeOrTag>(domain_in, domain_out, point_accessor_in, edge_refinement_flag_accessor);
  }

  template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut, typename EdgeRefinementFlagAccessor>
  void refine_uniformly(DomainTypeIn const & domain_in, DomainTypeOut & domain_out, EdgeRefinementFlagAccessor const edge_refinement_flag_accessor)
  {
    refine_uniformly<CellTypeOrTag>(domain_in, domain_out, default_point_accessor(domain_in), edge_refinement_flag_accessor);
  }

  template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut>
  void refine_uniformly(DomainTypeIn const & domain_in, DomainTypeOut & domain_out)
  {
    typedef typename viennagrid::result_of::line<DomainTypeOut>::type EdgeType;
    std::deque<bool> edge_refinement_flag;

    refine_uniformly<CellTypeOrTag>(domain_in, domain_out, viennagrid::make_accessor<EdgeType>(edge_refinement_flag));
  }

  template <typename DomainTypeIn, typename DomainTypeOut>
  void refine_uniformly(DomainTypeIn const & domain_in, DomainTypeOut & domain_out)
  {
    typedef typename viennagrid::result_of::cell<DomainTypeIn>::type CellType;
    refine_uniformly<CellType>(domain_in, domain_out);
  }

}

#endif
