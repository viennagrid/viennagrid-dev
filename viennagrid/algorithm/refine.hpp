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
#include "viennagrid/domain.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennadata/api.hpp"

#include "viennagrid/refinement/tetrahedron.hpp"

/** @file refine.hpp
    @brief Provides the routines for a refinement of a domain
*/

template<class _vertex_type >
class line;
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
    template <typename CellTagIn, typename GeometricDomainTypeIn>
    void ensure_longest_edge_refinement(GeometricDomainTypeIn const & domain_in)
    {
      //typedef typename viennagrid::result_of::domain<ConfigTypeIn>::type                      DomainTypeIn;
      //typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
      
      typedef typename viennagrid::result_of::element<GeometricDomainTypeIn, vertex_tag>::type                                      VertexType;
      typedef typename viennagrid::result_of::element<GeometricDomainTypeIn, line_tag>::type                                      EdgeType;
      typedef typename viennagrid::result_of::element<GeometricDomainTypeIn, CellTagIn>::type              CellType;
      
      typedef typename viennagrid::result_of::const_element_range<GeometricDomainTypeIn, CellTagIn>::type   CellRange;          
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                          CellIterator;         
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type                               EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                                    EdgeOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                               VertexOnEdgeRange;
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
            if (viennadata::access<refinement_key, bool>(refinement_key())(*eocit) == true)
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
              VertexOnEdgeRange vertices_on_edge = viennagrid::ncells<0>(*eocit);
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
            if (viennadata::access<refinement_key, bool>(refinement_key())(*longest_edge_ptr) == false)
            {
              viennadata::access<refinement_key, bool>(refinement_key())(*longest_edge_ptr) = true;
              something_changed = true;
            }
          }
          
        } //for cells


      } //while
    } //ensure_longest_edge_refinement
    
    /** @brief Transfers tags for refinement from the cell to edges */
    template <typename CellTagIn, typename GeometricDomainTypeIn>
    void cell_refinement_to_edge_refinement(GeometricDomainTypeIn const & domain_in)
    {
      //typedef typename viennagrid::result_of::domain<ConfigTypeIn>::type                      DomainTypeIn;
      //typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
      typedef typename viennagrid::result_of::element<GeometricDomainTypeIn, CellTagIn>::type              CellType;
      
      typedef typename viennagrid::result_of::const_element_range<GeometricDomainTypeIn, CellTagIn>::type   CellRange;          
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                          CellIterator;         
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type                               EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                                    EdgeOnCellIterator;            
      
      std::size_t cells_for_refinement = 0;
      CellRange cells = viennagrid::elements<CellTagIn>(domain_in);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(*cit) == true)
        {
          ++cells_for_refinement;
          
          EdgeOnCellRange edges_on_cell = viennagrid::elements<line_tag>(*cit);
          for (EdgeOnCellIterator eocit = edges_on_cell.begin();
                                  eocit != edges_on_cell.end();
                                ++eocit)
          {
            viennadata::access<refinement_key, bool>(refinement_key())(*eocit) = true;
          }
        }
      }
      
      //std::cout << "No. of cells tagged for refinement: " << cells_for_refinement << std::endl;
    }
    
    

    /** @brief Implementation of adaptive refinement. Responsible for all the book-keeping */
    template <typename CellTagIn, typename GeometricDomainTypeIn, typename GeometricDomainTypeOut>
    void refine_impl(GeometricDomainTypeIn const & domain_in,
                    GeometricDomainTypeOut & domain_out,
                    local_refinement_tag)
    {
      //typedef domain_t<ConfigTypeIn>                      DomainTypeIn;
      //typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
      //typedef typename ConfigTypeIn::numeric_type       NumericType;
      typedef typename viennagrid::result_of::point_type<GeometricDomainTypeIn>::type PointType;
      
      typedef typename viennagrid::traits::value_type<PointType>::type NumericType;

      //typedef typename DomainTypeIn::segment_type                                                                    SegmentTypeIn; 
      //typedef typename viennagrid::result_of::point<ConfigTypeIn>::type                                         PointType;
      typedef typename viennagrid::result_of::element<GeometricDomainTypeIn, vertex_tag>::type                                      VertexType;
      typedef typename viennagrid::result_of::element_hook<GeometricDomainTypeIn, vertex_tag>::type                                      VertexHookType;
      typedef typename viennagrid::result_of::element<GeometricDomainTypeIn, line_tag>::type                                      EdgeType;
      typedef typename viennagrid::result_of::element<GeometricDomainTypeIn, CellTagIn>::type              CellType;
      
      typedef typename viennagrid::result_of::const_element_range<GeometricDomainTypeIn, vertex_tag>::type                           VertexRange;          
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                                        VertexIterator;         
      typedef typename viennagrid::result_of::const_element_range<GeometricDomainTypeIn, line_tag>::type                           EdgeRange;          
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                          EdgeIterator;         
      typedef typename viennagrid::result_of::const_element_range<GeometricDomainTypeIn, CellTagIn>::type  CellRange;          
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                          CellIterator;         
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type                               VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type                                  VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_element_range<EdgeType, vertex_tag>::type                               VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                                  VertexOnEdgeIterator;
      
      typedef typename viennagrid::result_of::element<GeometricDomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
      typedef typename VertexTypeOut::id_type VertexIDTypeOut;

      //typedef typename detail::refinement_vertex_id_requirement<domain_t<ConfigTypeIn> >::type   checked_type;
      
      //
      // Step 1: Write tags from cells to edges:
      //
      cell_refinement_to_edge_refinement<CellTagIn>(domain_in);
      ensure_longest_edge_refinement<CellTagIn>(domain_in);
      
      //
      // Step 2: Write old vertices to new domain
      //
      std::size_t num_vertices = 0;
      VertexRange vertices = viennagrid::elements<vertex_tag>(domain_in);
      for (VertexIterator vit  = vertices.begin();
                          vit != vertices.end();
                        ++vit)
      {
        //domain_out.push_back(*vit);
        VertexHookType vh = viennagrid::push_element_noid( domain_out, *vit ).first;
        viennagrid::point( domain_out, vh ) = viennagrid::point( domain_in, *vit );
        ++num_vertices;
      }

      //
      // Step 3: Each tagged edge in old domain results in a new vertex (temporarily store new vertex IDs on old domain)
      //
      EdgeRange edges = viennagrid::ncells<1>(domain_in);
      for (EdgeIterator eit = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        if (viennadata::access<refinement_key, bool>()(*eit) == true)
        {
          VertexHookType v = viennagrid::create_element<VertexType>( domain_out, typename VertexType::id_type(num_vertices) );
          viennagrid::point(domain_out, v) = viennagrid::centroid(*eit, domain_in);

          //viennadata::access<refinement_key, std::size_t>()(*eit) = num_vertices;
          viennadata::access<refinement_key, VertexIDTypeOut>()(*eit) = VertexIDTypeOut(num_vertices);
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
        CellRange cells = viennagrid::elements<CellTagIn>(domain_in);
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
        {
          element_refinement<CellTagIn>::apply(*cit, domain_out);
        }
      //}

      // Clean up refinement flags:
      for (EdgeIterator eit = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        viennadata::erase<refinement_key, std::size_t>()(*eit);
        viennadata::erase<refinement_key, bool>()(*eit);
      }

    }
    
    
    
    
    /** @brief Implementation of uniform refinement. Responsible for all the book-keeping. */
    template <typename CellTagIn, typename GeometricDomainTypeIn, typename GeometricDomainTypeOut>
    void refine_impl(GeometricDomainTypeIn const & domain_in,
                    GeometricDomainTypeOut & domain_out,
                    uniform_refinement_tag)
    {
      //typedef domain_t<ConfigTypeIn>                      DomainTypeIn;
      //typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
      //typedef typename ConfigTypeIn::numeric_type       NumericType;
      typedef typename viennagrid::result_of::point_type<GeometricDomainTypeIn>::type PointType;
      
      typedef typename viennagrid::traits::value_type<PointType>::type NumericType;

      //typedef typename DomainTypeIn::segment_type                                                                    SegmentTypeIn; 
      //typedef typename viennagrid::result_of::point<ConfigTypeIn>::type                                         PointType;
      typedef typename viennagrid::result_of::element<GeometricDomainTypeIn, vertex_tag>::type                                      VertexType;
      typedef typename viennagrid::result_of::element_hook<GeometricDomainTypeIn, vertex_tag>::type                                      VertexHookType;
      typedef typename viennagrid::result_of::element<GeometricDomainTypeIn, line_tag>::type                                      EdgeType;
      typedef typename viennagrid::result_of::element<GeometricDomainTypeIn, CellTagIn>::type              CellType;
      
      typedef typename viennagrid::result_of::const_element_range<GeometricDomainTypeIn, vertex_tag>::type                           VertexRange;          
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                                        VertexIterator;         
      typedef typename viennagrid::result_of::const_element_range<GeometricDomainTypeIn, line_tag>::type                           EdgeRange;          
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                          EdgeIterator;         
      typedef typename viennagrid::result_of::const_element_range<GeometricDomainTypeIn, CellTagIn>::type  CellRange;          
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                          CellIterator;         
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type                               VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type                                  VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_element_range<EdgeType, vertex_tag>::type                               VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                                  VertexOnEdgeIterator;
      
      typedef typename viennagrid::result_of::element<GeometricDomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
      typedef typename VertexTypeOut::id_type VertexIDTypeOut;
      
      //typedef typename detail::refinement_vertex_id_requirement<domain_t<ConfigTypeIn> >::type   checked_type;
      
      //
      // Step 1: Write old vertices to new domain
      //
      std::size_t num_vertices = 0;
      VertexRange vertices = viennagrid::elements<vertex_tag>(domain_in);
      for (VertexIterator vit  = vertices.begin();
                          vit != vertices.end();
                        ++vit)
      {
        //domain_out.push_back(*vit);
        VertexHookType vh = viennagrid::push_element_noid( domain_out, *vit ).first;
        viennagrid::point( domain_out, vh ) = viennagrid::point( domain_in, *vit );
        ++num_vertices;
      }

      //
      // Step 2: Each edge in old domain results in a new vertex (temporarily store new vertex IDs on old domain)
      //
      EdgeRange edges = viennagrid::elements<line_tag>(domain_in);
      for (EdgeIterator eit = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
          VertexHookType v = viennagrid::create_element<VertexType>( domain_out, typename VertexType::id_type(num_vertices) );
          viennagrid::point(domain_out, v) = viennagrid::centroid(*eit, domain_in);

          //viennadata::access<refinement_key, std::size_t>()(*eit) = num_vertices;
          viennadata::access<refinement_key, VertexIDTypeOut>()(*eit) = VertexIDTypeOut(num_vertices);
          viennadata::access<refinement_key, bool>()(*eit) = true;
          ++num_vertices;
      }
      //std::cout << "Number of vertices in new domain: " << num_vertices << std::endl;

      //
      // Step 3: Now write new cells to new domain
      //
      //domain_out.segments().resize(domain_in.segments().size());
      //for (std::size_t i=0; i<domain_in.segments().size(); ++i)
      {
        //std::cout << "Working on segment " << i << std::endl;
        std::size_t counter = 0;
        CellRange cells = viennagrid::elements<CellTagIn>(domain_in);
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
        {
          element_refinement<CellTagIn>::apply(*cit, domain_out);
          ++counter;
        }
        //std::cout << "Refined cells in segment: " << counter << std::endl;
      }

      // Clean up refinement flags:
      for (EdgeIterator eit = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        viennadata::erase<refinement_key, std::size_t>()(*eit);
        viennadata::erase<refinement_key, bool>()(*eit);
      }
    }
    
  } //namespace detail
  
  /** @brief A proxy class that is used to allow ' refined_domain = refine(domain); ' without temporary. */
  template <typename CellTag, typename DomainType, typename RefinementTag>
  class refinement_proxy
  {
    public:
      refinement_proxy(DomainType const & domain_in,
                       RefinementTag const & refinement_tag) : domain_in_(domain_in), tag_(refinement_tag) {}
      
      DomainType const & get() const 
      {
        return domain_in_;
      }
      
      RefinementTag const & tag() const { return tag_; }
      
    private:
      DomainType const & domain_in_;
      RefinementTag const & tag_;
  };
  
  
  /** @brief Public interface for refinement of a domain. If local refinement is desired, cells or edges needs to be tagged using ViennaData with refinement_key.*/
  template <typename CellTag, typename DomainTypeIn, typename RefinementTag>
  refinement_proxy< CellTag, DomainTypeIn, RefinementTag >
  refine(DomainTypeIn const & domain_in, RefinementTag const & tag)
  {
    //typedef typename detail::refinement_vertex_id_requirement<domain_t<ConfigTypeIn> >::type   checked_type;
    return refinement_proxy< CellTag, DomainTypeIn, RefinementTag >(domain_in, tag);
  }
  
  /** @brief Convenience overload for uniform refinement of a domain.  */
  template <typename CellTag, typename DomainTypeIn>
  refinement_proxy< CellTag, DomainTypeIn, uniform_refinement_tag >
  refine_uniformly(DomainTypeIn const & domain_in)
  {
    //typedef typename detail::refinement_vertex_id_requirement<domain_t<ConfigTypeIn> >::type   checked_type;
    return refinement_proxy< CellTag, DomainTypeIn, uniform_refinement_tag >(domain_in, uniform_refinement_tag());
  }
  
  /** @brief Convenience overload for adaptive refinement of a domain. Cells or edges needs to be tagged using ViennaData with refinement_key. */
  template <typename CellTag, typename DomainTypeIn>
  refinement_proxy< CellTag, DomainTypeIn, local_refinement_tag >
  refine_locally(DomainTypeIn const & domain_in)
  {
    //typedef typename detail::refinement_vertex_id_requirement<domain_t<ConfigTypeIn> >::type   checked_type;
    return refinement_proxy< CellTag, DomainTypeIn, local_refinement_tag >(domain_in, local_refinement_tag());
  }
  
} 

#endif
