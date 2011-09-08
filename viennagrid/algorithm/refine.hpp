#ifndef VIENNAGRID_ALGORITHM_REFINE_HPP
#define VIENNAGRID_ALGORITHM_REFINE_HPP

/* =======================================================================
   Copyright (c) 2011, Institute for Microelectronics,
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

#include "viennagrid/forwards.h"
#include "viennagrid/domain.hpp"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennadata/api.hpp"

namespace viennagrid 
{
  namespace detail
  {
    //
    // If any edge is refined in a cell, then the longest edge is refined as well
    //
    template <typename ConfigTypeIn>
    void ensure_longest_edge_refinement(domain_t<ConfigTypeIn> const & domain_in)
    {
      typedef typename viennagrid::result_of::domain<ConfigTypeIn>::type                      DomainTypeIn;
      typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
      
      typedef typename viennagrid::result_of::ncell<ConfigTypeIn, 0>::type                                      VertexType;
      typedef typename viennagrid::result_of::ncell<ConfigTypeIn, 1>::type                                      EdgeType;
      typedef typename viennagrid::result_of::ncell<ConfigTypeIn, CellTagIn::dim>::type              CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, CellTagIn::dim>::type   CellRange;          
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
        
      
        CellRange cells = viennagrid::ncells<CellTagIn::dim>(domain_in);
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
        {
          //
          // check if any edges are tagged for refinement
          //
          bool has_refinement = false;
          EdgeOnCellRange edges_on_cell = viennagrid::ncells<1>(*cit);
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
              
              double len = viennagrid::norm(v0.point() - v1.point());
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
    
    
    template <typename ConfigTypeIn>
    void cell_refinement_to_edge_refinement(domain_t<ConfigTypeIn> const & domain_in)
    {
      typedef typename viennagrid::result_of::domain<ConfigTypeIn>::type                      DomainTypeIn;
      typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
      typedef typename viennagrid::result_of::ncell<ConfigTypeIn, CellTagIn::dim>::type              CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, CellTagIn::dim>::type   CellRange;          
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                          CellIterator;         
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type                               EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                                    EdgeOnCellIterator;            
      
      std::size_t cells_for_refinement = 0;
      CellRange cells = viennagrid::ncells<CellTagIn::dim>(domain_in);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(*cit) == true)
        {
          ++cells_for_refinement;
          
          EdgeOnCellRange edges_on_cell = viennagrid::ncells<1>(*cit);
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
    
    

    //
    // Adaptive refinement
    //
    template <typename ConfigTypeIn, typename ConfigTypeOut>
    void refine_impl(domain_t<ConfigTypeIn> const & domain_in,
                    domain_t<ConfigTypeOut> & domain_out,
                    adaptive_refinement_tag)
    {
      typedef domain_t<ConfigTypeIn>                      DomainTypeIn;
      typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
      typedef typename ConfigTypeIn::numeric_type       NumericType;

      typedef typename DomainTypeIn::segment_type                                                                    SegmentTypeIn; 
      typedef typename viennagrid::result_of::point<ConfigTypeIn>::type                                         PointType;
      typedef typename viennagrid::result_of::ncell<ConfigTypeIn, 0>::type                                      VertexType;
      typedef typename viennagrid::result_of::ncell<ConfigTypeIn, 1>::type                                      EdgeType;
      typedef typename viennagrid::result_of::ncell<ConfigTypeIn, CellTagIn::dim>::type              CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, 0>::type                           VertexRange;          
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                                        VertexIterator;         
      typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, 1>::type                           EdgeRange;          
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                          EdgeIterator;         
      typedef typename viennagrid::result_of::const_ncell_range<SegmentTypeIn, CellTagIn::dim>::type  CellRange;          
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                          CellIterator;         
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type                               VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type                                  VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                               VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                                  VertexOnEdgeIterator;            

      //
      // Step 1: Write tags from cells to edges:
      //
      cell_refinement_to_edge_refinement(domain_in);
      ensure_longest_edge_refinement(domain_in);
      
      //
      // Step 2: Write old vertices to new domain
      //
      std::size_t num_vertices = 0;
      VertexRange vertices = viennagrid::ncells<0>(domain_in);
      for (VertexIterator vit  = vertices.begin();
                          vit != vertices.end();
                        ++vit)
      {
        domain_out.push_back(*vit);
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
          VertexType v;
          v.point() = viennagrid::centroid(*eit);
          
          v.id(num_vertices);
          domain_out.push_back(v);
          viennadata::access<refinement_key, std::size_t>()(*eit) = num_vertices;
          ++num_vertices;
        }
      }
      //std::cout << "Number of vertices in new domain: " << num_vertices << std::endl;

      //
      // Step 4: Now write new cells to new domain
      //
      domain_out.segments().resize(domain_in.segments().size());
      for (std::size_t i=0; i<domain_in.segments().size(); ++i)
      {
        //std::cout << "Working on segment " << i << std::endl;
        CellRange cells = viennagrid::ncells<CellTagIn::dim>(domain_in.segments()[i]);
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
        {
          element_refinement<CellTagIn>::apply(*cit, domain_out.segments()[i]);
        }
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
    
    
    
    
    //
    // Uniform refinement
    //
    template <typename ConfigTypeIn, typename ConfigTypeOut>
    void refine_impl(domain_t<ConfigTypeIn> const & domain_in,
                    domain_t<ConfigTypeOut> & domain_out,
                    uniform_refinement_tag)
    {
      typedef domain_t<ConfigTypeIn>                    DomainTypeIn;
      typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
      typedef typename ConfigTypeIn::numeric_type       NumericType;

      typedef typename DomainTypeIn::segment_type                                                                    SegmentTypeIn; 
      typedef typename viennagrid::result_of::point<ConfigTypeIn>::type                                         PointType;
      typedef typename viennagrid::result_of::ncell<ConfigTypeIn, 0>::type                                      VertexType;
      typedef typename viennagrid::result_of::ncell<ConfigTypeIn, 1>::type                                      EdgeType;
      typedef typename viennagrid::result_of::ncell<ConfigTypeIn, CellTagIn::dim>::type              CellType;
      
      typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, 0>::type                           VertexRange;          
      typedef typename viennagrid::result_of::iterator<VertexRange>::type                                        VertexIterator;         
      typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, 1>::type                           EdgeRange;          
      typedef typename viennagrid::result_of::iterator<EdgeRange>::type                                          EdgeIterator;         
      typedef typename viennagrid::result_of::const_ncell_range<SegmentTypeIn, CellTagIn::dim>::type  CellRange;          
      typedef typename viennagrid::result_of::iterator<CellRange>::type                                          CellIterator;         
      typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                               VertexOnEdgeRange;
      typedef typename viennagrid::result_of::iterator<VertexOnEdgeRange>::type                                  VertexOnEdgeIterator;            
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type                               VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type                                  VertexOnCellIterator;            
      
      //
      // Step 1: Write old vertices to new domain
      //
      std::size_t num_vertices = 0;
      VertexRange vertices = viennagrid::ncells<0>(domain_in);
      for (VertexIterator vit  = vertices.begin();
                          vit != vertices.end();
                        ++vit)
      {
        domain_out.push_back(*vit);
        ++num_vertices;
      }

      //
      // Step 2: Each edge in old domain results in a new vertex (temporarily store new vertex IDs on old domain)
      //
      EdgeRange edges = viennagrid::ncells<1>(domain_in);
      for (EdgeIterator eit = edges.begin();
                        eit != edges.end();
                      ++eit)
      {
        VertexType v;
        v.point() = viennagrid::centroid(*eit);

        v.id(num_vertices);
        domain_out.push_back(v);
        viennadata::access<refinement_key, std::size_t>()(*eit) = num_vertices;
        viennadata::access<refinement_key, bool>()(*eit) = true;
        ++num_vertices;
      }
      //std::cout << "Number of vertices in new domain: " << num_vertices << std::endl;

      //
      // Step 3: Now write new cells to new domain
      //
      domain_out.segments().resize(domain_in.segments().size());
      for (std::size_t i=0; i<domain_in.segments().size(); ++i)
      {
        //std::cout << "Working on segment " << i << std::endl;
        std::size_t counter = 0;
        CellRange cells = viennagrid::ncells<CellTagIn::dim>(domain_in.segments()[i]);
        for (CellIterator cit  = cells.begin();
                          cit != cells.end();
                        ++cit)
        {
          element_refinement<CellTagIn>::apply(*cit, domain_out.segments()[i]);
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
  
  
  template <typename DomainType, typename RefinementTag>
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
  
  
  template <typename ConfigTypeIn, typename RefinementTag>
  refinement_proxy< domain_t<ConfigTypeIn>, RefinementTag >
  refine(domain_t<ConfigTypeIn> const & domain_in, RefinementTag const & tag)
  {
    return refinement_proxy< domain_t<ConfigTypeIn>, RefinementTag >(domain_in, tag);
  }
  

  template <typename ConfigTypeIn>
  refinement_proxy< domain_t<ConfigTypeIn>, uniform_refinement_tag >
  refine_uniformly(domain_t<ConfigTypeIn> const & domain_in)
  {
    return refinement_proxy< domain_t<ConfigTypeIn>, uniform_refinement_tag >(domain_in, uniform_refinement_tag());
  }
  
  template <typename ConfigTypeIn>
  refinement_proxy< domain_t<ConfigTypeIn>, adaptive_refinement_tag >
  refine_adaptively(domain_t<ConfigTypeIn> const & domain_in)
  {
    return refinement_proxy< domain_t<ConfigTypeIn>, adaptive_refinement_tag >(domain_in, adaptive_refinement_tag());
  }
  
} 

#endif
