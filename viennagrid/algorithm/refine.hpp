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
#include "viennadata/api.hpp"

#include "viennagrid/refinement/triangle.hpp"
#include "viennagrid/refinement/tetrahedron.hpp"

/** @file refine.hpp
    @brief Provides the routines for a refinement of a domain
*/

namespace viennagrid 
{
  namespace detail
  {
    /** @brief Refinement requies vertex IDs. This class makes sure that they are available. */
//     template <typename DomainType,
//               typename VertexIDHandler = typename viennagrid::result_of::element_id_handler<typename DomainType::config_type,
//                                                                                             viennagrid::point_tag>::type>
//     struct refinement_vertex_id_requirement
//     {
//       typedef long        type;
//     };
//     
//     template <typename DomainType>
//     struct refinement_vertex_id_requirement< DomainType, pointer_id>
//     {
//       typedef typename DomainType::ERROR_VERTEX_IDs_FOR_REFINEMENT_REQUIRED   type;
//     };
    
    
    
    /** @brief Ensures refinement fo the longest edge. If any edge is refined in a cell, then the longest edge is refined as well. */
    template <typename CellTagIn, typename DomainTypeIn, typename EdgeRefinementFlagAccessor>
    void ensure_longest_edge_refinement(DomainTypeIn const & domain_in, EdgeRefinementFlagAccessor edge_refinement_flag_accessor)
    {
//       typedef typename viennagrid::resulf_of::cell_tag<DomainTypeIn>::type           CellTagIn;
      
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
        //std::cout << "Longest edge bisection, run " << ++iter_cnt << std::endl;
        
      
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
//       typedef typename viennagrid::resulf_of::cell_tag<DomainTypeIn>::type           CellTagIn;   
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
      
      //std::cout << "No. of cells tagged for refinement: " << cells_for_refinement << std::endl;
    }
    
    

    /** @brief Implementation of adaptive refinement. Responsible for all the book-keeping */
    template <typename CellTagIn, typename DomainTypeIn, typename DomainTypeOut, typename PointAccessorType,
              typename VertexToVertexHandleAccessor, typename CellRefinementFlagAccessor, typename EdgeRefinementFlagAccessor, typename RefinementVertexAccessor>
    void refine_impl(DomainTypeIn const & domain_in, DomainTypeOut & domain_out, PointAccessorType const point_accessor_in,
                     CellRefinementFlagAccessor const cell_refinement_flag, EdgeRefinementFlagAccessor edge_refinement_flag_accessor,
                     VertexToVertexHandleAccessor vertex_to_vertex_handle_accessor, RefinementVertexAccessor edge_to_vertex_handle_accessor)
    {
      //typedef domain_t<ConfigTypeIn>                      DomainTypeIn;
      //typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
      //typedef typename ConfigTypeIn::numeric_type       NumericType;
      typedef typename viennagrid::result_of::point_type<DomainTypeIn>::type PointType;
      
      typedef typename viennagrid::result_of::coord_type<PointType>::type NumericType;

      //typedef typename DomainTypeIn::segment_type                                                                    SegmentTypeIn; 
      //typedef typename viennagrid::result_of::point<ConfigTypeIn>::type                                         PointType;
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

      //typedef typename detail::refinement_vertex_id_requirement<domain_t<ConfigTypeIn> >::type   checked_type;
      
      //
      // Step 1: Write tags from cells to edges:
      //
//       cell_refinement_to_edge_refinement<CellTagIn>(domain_in, cell_refinement_flag, edge_refinement_flag_accessor);
//       ensure_longest_edge_refinement<CellTagIn>(domain_in, edge_refinement_flag_accessor);
      
      //
      // Step 2: Write old vertices to new domain
      //
      std::size_t num_vertices = 0;
      VertexRange vertices = viennagrid::elements(domain_in);
      for (VertexIterator vit  = vertices.begin();
                          vit != vertices.end();
                        ++vit)
      {
        //domain_out.push_back(*vit);
        VertexHandleType vh = viennagrid::create_vertex( domain_out, typename VertexType::id_type(num_vertices), point_accessor_in(*vit) );
//         viennagrid::push_element_noid( domain_out, *vit ).first;
//         viennagrid::point( domain_out, vh ) = viennagrid::point( domain_in, *vit );
        
        vertex_to_vertex_handle_accessor( *vit ) = vh;
        ++num_vertices;
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
//           VertexHandleType v = viennagrid::create_element<VertexType>( domain_out, typename VertexType::id_type(num_vertices) );
//           viennagrid::point(domain_out, v) = ;
          PointType tmp = viennagrid::centroid(point_accessor_in, *eit);
          VertexHandleType vh = viennagrid::create_vertex( domain_out, typename VertexType::id_type(num_vertices), tmp );

          //viennadata::access<refinement_key, std::size_t>()(*eit) = num_vertices;
//           viennadata::access<refinement_key, VertexIDTypeOut>()(*eit) = VertexIDTypeOut(num_vertices);
          edge_to_vertex_handle_accessor( *eit ) = vh;
          
          std::cout << "One edge marked for refinement: " << *eit << "  with Point " << tmp << std::endl;
          
          ++num_vertices;
        }
      }
      //std::cout << "Number of vertices in new domain: " << num_vertices << std::endl;

      //
      // Step 4: Now write new cells to new domain
      //
      //domain_out.segments().resize(domain_in.segments().size());
      //for (std::size_t i=0; i<domain_in.segments().size(); ++i)
      //{
        //std::cout << "Working on segment " << i << std::endl;
        CellRange cells = viennagrid::elements(domain_in);
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
        {
          element_refinement<CellTagIn>::apply(*cit, domain_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
        }
      //}

//       // Clean up refinement flags:
//       for (EdgeIterator eit = edges.begin();
//                         eit != edges.end();
//                       ++eit)
//       {
//         viennadata::erase<refinement_key, std::size_t>()(*eit);
//         viennadata::erase<refinement_key, bool>()(*eit);
//       }

    }
    
    
    
    
//     /** @brief Implementation of adaptive refinement. Responsible for all the book-keeping */
//     template <typename CellTagIn, typename DomainTypeIn, typename SegmentContainerTypeIn, typename DomainTypeOut, typename SegmentContainerTypeOut>
//     void refine_impl(DomainTypeIn const & domain_in, SegmentContainerTypeIn const & segments_in,
//                     DomainTypeOut & domain_out, SegmentContainerTypeOut & segments_out,
//                     local_refinement_tag)
//     {
//       //typedef domain_t<ConfigTypeIn>                      DomainTypeIn;
//       //typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
//       //typedef typename ConfigTypeIn::numeric_type       NumericType;
//       
//       typedef typename SegmentContainerTypeIn::value_type SegmentTypeIn;
//       typedef typename SegmentContainerTypeOut::value_type SegmentTypeOut;
//       
//       typedef typename viennagrid::result_of::point_type<DomainTypeIn>::type PointType;
//       
//       typedef typename viennagrid::result_of::coord_type<PointType>::type NumericType;
// 
//       //typedef typename DomainTypeIn::segment_type                                                                    SegmentTypeIn; 
//       //typedef typename viennagrid::result_of::point<ConfigTypeIn>::type                                         PointType;
//       typedef typename viennagrid::result_of::element<DomainTypeIn, vertex_tag>::type                                      VertexType;
// //       typedef typename VertexType::id_type                                      VertexIDType;
//       typedef typename viennagrid::result_of::handle<DomainTypeIn, vertex_tag>::type                                      VertexHandleType;
//       typedef typename viennagrid::result_of::element<DomainTypeIn, line_tag>::type                                      EdgeType;
//       typedef typename viennagrid::result_of::element<DomainTypeIn, CellTagIn>::type              CellType;
//       
//       typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, vertex_tag>::type                           VertexRange;          
//       typedef typename viennagrid::result_of::iterator<VertexRange>::type                                        VertexIterator;         
//       typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, line_tag>::type                           EdgeRange;          
//       typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                          EdgeIterator;         
//       typedef typename viennagrid::result_of::const_element_range<SegmentTypeIn, CellTagIn>::type  CellOnSegmentRange;          
//       typedef typename viennagrid::result_of::iterator<CellOnSegmentRange>::type                                          CellOnSegmentIterator;         
//       typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type                               VertexOnCellRange;
//       typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type                                  VertexOnCellIterator;            
//       typedef typename viennagrid::result_of::const_element_range<EdgeType, vertex_tag>::type                               VertexOnEdgeRange;
//       typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                                  VertexOnEdgeIterator;
//       
//       typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
//       typedef typename VertexTypeOut::id_type VertexIDTypeOut;
//       
//       for (typename SegmentContainerTypeIn::const_iterator it = segments_in.begin(); it != segments_in.end(); ++it)
//           segments_out.push_back( viennagrid::create_view<SegmentTypeOut>( domain_out ) );
//       
// //       segments_out.resize( segments_in.size() );
// //       for (unsigned int i = 0; i < segments_out.size(); ++i)
// //           segments_out[i] = viennagrid::create_view<SegmentTypeOut>( domain_out );
// 
//       //typedef typename detail::refinement_vertex_id_requirement<domain_t<ConfigTypeIn> >::type   checked_type;
//       
//       //
//       // Step 1: Write tags from cells to edges:
//       //
//       cell_refinement_to_edge_refinement<CellTagIn>(domain_in);
//       ensure_longest_edge_refinement<CellTagIn>(domain_in);
//       
//       //
//       // Step 2: Write old vertices to new domain
//       //
//       std::size_t num_vertices = 0;
//       VertexRange vertices = viennagrid::elements<vertex_tag>(domain_in);
//       for (VertexIterator vit  = vertices.begin();
//                           vit != vertices.end();
//                         ++vit)
//       {
//         //domain_out.push_back(*vit);
//         VertexHandleType vh = viennagrid::create_vertex( domain_out, VertexIDTypeOut(num_vertices), viennagrid::point( domain_in, *vit ) );
// //         viennagrid::push_element_noid( domain_out, *vit ).first;
// //         viennagrid::point( domain_out, vh ) = viennagrid::point( domain_in, *vit );
//         ++num_vertices;
//         
//         for (typename SegmentContainerTypeOut::iterator it = segments_out.begin(); it != segments_out.end(); ++it)
//             viennagrid::add_handle( *it, domain_out, vh );
//       }
// 
//       //
//       // Step 3: Each tagged edge in old domain results in a new vertex (temporarily store new vertex IDs on old domain)
//       //
//       EdgeRange edges = viennagrid::elements<line_tag>(domain_in);
//       for (EdgeIterator eit = edges.begin();
//                         eit != edges.end();
//                       ++eit)
//       {
//         if (viennadata::access<refinement_key, bool>()(*eit) == true)
//         {
// //           VertexHandleType v = viennagrid::create_element<VertexType>( domain_out, typename VertexType::id_type(num_vertices) );
// //           viennagrid::point(domain_out, v) = ;
//           VertexHandleType vh = viennagrid::create_vertex( domain_out, VertexIDTypeOut(num_vertices), viennagrid::centroid(domain_in, *eit) );
// 
//           //viennadata::access<refinement_key, std::size_t>()(*eit) = num_vertices;
//           viennadata::access<refinement_key, VertexIDTypeOut>()(*eit) = VertexIDTypeOut(num_vertices);
//           ++num_vertices;
//           
//         for (typename SegmentContainerTypeOut::iterator it = segments_out.begin(); it != segments_out.end(); ++it)
//             viennagrid::add_handle( *it, domain_out, vh );
//         }
//       }
//       //std::cout << "Number of vertices in new domain: " << num_vertices << std::endl;
// 
//       //
//       // Step 4: Now write new cells to new domain
//       //
//       //domain_out.segments().resize(domain_in.segments().size());
//       //for (std::size_t i=0; i<domain_in.segments().size(); ++i)
//       //{
//         //std::cout << "Working on segment " << i << std::endl;
//         
//       typename SegmentContainerTypeOut::iterator jt = segments_out.begin();
//       for (typename SegmentContainerTypeIn::const_iterator it = segments_in.begin(); it != segments_in.end(); ++it, ++jt)
//       {
//         
//         CellOnSegmentRange cells = viennagrid::elements<CellTagIn>( *it );
//         for (CellOnSegmentIterator cit  = cells.begin();
//                           cit != cells.end();
//                         ++cit)
//         {
//           element_refinement<CellTagIn>::apply(*cit, *jt);
//         }
//       }
//       //}
// 
//       // Clean up refinement flags:
//       for (EdgeIterator eit = edges.begin();
//                         eit != edges.end();
//                       ++eit)
//       {
//         viennadata::erase<refinement_key, std::size_t>()(*eit);
//         viennadata::erase<refinement_key, bool>()(*eit);
//       }
// 
//     }
    
    
    
    
//     /** @brief Implementation of uniform refinement. Responsible for all the book-keeping. */
//     template <typename CellTagIn, typename DomainTypeIn, typename DomainTypeOut, typename CellRefinementFlagAccessor, typename EdgeRefinementFlagAccessor, typename RefinementVertexAccessor>
//     void refine_impl(DomainTypeIn const & domain_in, DomainTypeOut & domain_out,
//                      CellRefinementFlagAccessor const & cell_refinement_flag, EdgeRefinementFlagAccessor & edge_to_vertex_handle_accessor, RefinementVertexAccessor & edge_to_vertex_handle_accessor,
//                      uniform_refinement_tag)
//     {
//       //typedef domain_t<ConfigTypeIn>                      DomainTypeIn;
//       //typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
//       //typedef typename ConfigTypeIn::numeric_type       NumericType;
//       typedef typename viennagrid::result_of::point_type<DomainTypeIn>::type PointType;
//       
//       typedef typename viennagrid::result_of::coord_type<PointType>::type NumericType;
// 
//       //typedef typename DomainTypeIn::segment_type                                                                    SegmentTypeIn; 
//       //typedef typename viennagrid::result_of::point<ConfigTypeIn>::type                                         PointType;
//       typedef typename viennagrid::result_of::element<DomainTypeIn, vertex_tag>::type                                      VertexType;
//       typedef typename viennagrid::result_of::handle<DomainTypeIn, vertex_tag>::type                                      VertexHandleType;
//       typedef typename viennagrid::result_of::element<DomainTypeIn, line_tag>::type                                      EdgeType;
//       typedef typename viennagrid::result_of::element<DomainTypeIn, CellTagIn>::type              CellType;
//       
//       typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, vertex_tag>::type                           VertexRange;          
//       typedef typename viennagrid::result_of::iterator<VertexRange>::type                                        VertexIterator;         
//       typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, line_tag>::type                           EdgeRange;          
//       typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                          EdgeIterator;         
//       typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, CellTagIn>::type  CellRange;          
//       typedef typename viennagrid::result_of::iterator<CellRange>::type                                          CellIterator;         
//       typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type                               VertexOnCellRange;
//       typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type                                  VertexOnCellIterator;            
//       typedef typename viennagrid::result_of::const_element_range<EdgeType, vertex_tag>::type                               VertexOnEdgeRange;
//       typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                                  VertexOnEdgeIterator;
//       
//       typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
//       typedef typename VertexTypeOut::id_type VertexIDTypeOut;
//       
//       //typedef typename detail::refinement_vertex_id_requirement<domain_t<ConfigTypeIn> >::type   checked_type;
//       
//       //
//       // Step 1: Write old vertices to new domain
//       //
//       std::size_t num_vertices = 0;
//       VertexRange vertices = viennagrid::elements<vertex_tag>(domain_in);
//       for (VertexIterator vit  = vertices.begin();
//                           vit != vertices.end();
//                         ++vit)
//       {
//         //domain_out.push_back(*vit);
// //         VertexHandleType vh = viennagrid::push_element_noid( domain_out, *vit ).first;
// //         viennagrid::point( domain_out, vh ) = viennagrid::point( domain_in, *vit );
//         VertexHandleType vh = viennagrid::create_vertex( domain_out, VertexIDTypeOut(num_vertices), viennagrid::point( domain_in, *vit ) );
//         ++num_vertices;
//       }
// 
//       //
//       // Step 2: Each edge in old domain results in a new vertex (temporarily store new vertex IDs on old domain)
//       //
//       EdgeRange edges = viennagrid::elements<line_tag>(domain_in);
//       for (EdgeIterator eit = edges.begin();
//                         eit != edges.end();
//                       ++eit)
//       {
// //           VertexHandleType v = viennagrid::create_element<VertexType>( domain_out, typename VertexType::id_type(num_vertices) );
// //           viennagrid::point(domain_out, v) = viennagrid::centroid(domain_in, *eit);
//           VertexHandleType vh = viennagrid::create_vertex( domain_out, VertexIDTypeOut(num_vertices), viennagrid::centroid(domain_in, *eit) );
// 
//           //viennadata::access<refinement_key, std::size_t>()(*eit) = num_vertices;
//           viennadata::access<refinement_key, VertexIDTypeOut>()(*eit) = VertexIDTypeOut(num_vertices);
//           viennadata::access<refinement_key, bool>()(*eit) = true;
//           ++num_vertices;
//       }
//       //std::cout << "Number of vertices in new domain: " << num_vertices << std::endl;
// 
//       //
//       // Step 3: Now write new cells to new domain
//       //
//       //domain_out.segments().resize(domain_in.segments().size());
//       //for (std::size_t i=0; i<domain_in.segments().size(); ++i)
//       {
//         //std::cout << "Working on segment " << i << std::endl;
//         std::size_t counter = 0;
//         CellRange cells = viennagrid::elements<CellTagIn>(domain_in);
//         for (CellIterator cit  = cells.begin();
//                           cit != cells.end();
//                         ++cit)
//         {
//           element_refinement<CellTagIn>::apply(*cit, domain_out);
//           ++counter;
//         }
//         //std::cout << "Refined cells in segment: " << counter << std::endl;
//       }
// 
//       // Clean up refinement flags:
//       for (EdgeIterator eit = edges.begin();
//                         eit != edges.end();
//                       ++eit)
//       {
//         viennadata::erase<refinement_key, std::size_t>()(*eit);
//         viennadata::erase<refinement_key, bool>()(*eit);
//       }
//     }
    
  } //namespace detail
  
  
  
  /** @brief Public interface for refinement of a domain. If local refinement is desired, cells or edges needs to be tagged using ViennaData with refinement_key.*/
  template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut,
            typename CellRefinementFlagAccessor, typename EdgeRefinementFlagAccessor,  typename VertexToVertexHandleAccessor, typename RefinementVertexAccessor>
  void refine(DomainTypeIn const & domain_in, DomainTypeOut & domain_out,
              CellRefinementFlagAccessor const cell_refinement_flag, EdgeRefinementFlagAccessor edge_refinement_flag_accessor,
              VertexToVertexHandleAccessor vertex_to_vertex_handle_accessor, RefinementVertexAccessor edge_to_vertex_handle_accessor)
  {
    typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;
    detail::refine_impl<CellTag>(domain_in, domain_out, accessor::default_point_accessor(domain_in), cell_refinement_flag, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor);
  }
  
  /** @brief Convenience overload for uniform refinement of a domain.  */
  template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut,
            typename CellRefinementFlagAccessor, typename EdgeRefinementFlagAccessor,  typename VertexToVertexHandleAccessor, typename RefinementVertexAccessor>
  void refine_uniformly(DomainTypeIn const & domain_in, DomainTypeOut & domain_out,
                        CellRefinementFlagAccessor cell_refinement_flag, EdgeRefinementFlagAccessor edge_refinement_flag_accessor,
                        VertexToVertexHandleAccessor vertex_to_vertex_handle_accessor, RefinementVertexAccessor edge_to_vertex_handle_accessor)
  {
    typedef typename viennagrid::result_of::const_element_range<DomainTypeIn, CellTypeOrTag>::type CellRange;
    typedef typename viennagrid::result_of::iterator<CellRange>::type CellIterator;

    
    
    CellRange cells = viennagrid::elements(domain_in);
    
    cell_refinement_flag.resize(cells.size());
    
    for (CellIterator it = cells.begin(); it != cells.end(); ++it)
      cell_refinement_flag(*it) = true;
    
    
    typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;
    detail::refine_impl<CellTag>(domain_in, domain_out, accessor::default_point_accessor(domain_in), cell_refinement_flag, edge_refinement_flag_accessor, edge_to_vertex_handle_accessor);
  }
//   
//   /** @brief Convenience overload for adaptive refinement of a domain. Cells or edges needs to be tagged using ViennaData with refinement_key. */
//   template <typename CellTypeOrTag, typename DomainTypeIn, typename DomainTypeOut>
//   void refine_locally(DomainTypeIn const & domain_in, DomainTypeOut & domain_out)
//   {
//       refine<CellTypeOrTag>( domain_in, domain_out, local_refinement_tag() );
//   }
//   
//   
//   
//   /** @brief Public interface for refinement of a domain. If local refinement is desired, cells or edges needs to be tagged using ViennaData with refinement_key.*/
//   template <typename CellTypeOrTag, typename DomainTypeIn, typename SegmentContainerTypeIn, typename DomainTypeOut, typename SegmentContainerTypeOut, typename RefinementTag>
//   void refine(DomainTypeIn const & domain_in, SegmentContainerTypeIn const & segments_in,
//               DomainTypeOut & domain_out, SegmentContainerTypeOut & segments_out,
//               RefinementTag const & tag)
//   {
//     typedef typename viennagrid::result_of::element_tag<CellTypeOrTag>::type CellTag;
//     detail::refine_impl<CellTag>(domain_in, segments_in, domain_out, segments_out, tag);
//   }

  
} 

#endif
