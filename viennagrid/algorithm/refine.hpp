/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Josef Weinbub                   weinbub@iue.tuwien.ac.at
               Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_ALGORITHM_REFINE_HPP
#define VIENNAGRID_ALGORITHM_REFINE_HPP

#include "viennagrid/forwards.h"
#include "viennagrid/algorithm/centroid.hpp"
#include "viennagrid/algorithm/norm.hpp"
#include "viennadata/api.hpp"

namespace viennagrid 
{
  //
  // If any edge is refined in a cell, then the longest edge is refined as well
  //
  template <typename ConfigTypeIn>
  void ensure_longest_edge_refinement(domain<ConfigTypeIn> const & domain_in)
  {
    typedef domain<ConfigTypeIn>                      DomainTypeIn;
    typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
    
    typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type                                      VertexType;
    typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 1>::type                                      EdgeType;
    typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, CellTagIn::topology_level>::type              CellType;
    
    typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, CellTagIn::topology_level>::type   CellContainer;          
    typedef typename viennagrid::result_of::iterator<CellContainer>::type                                          CellIterator;         
    typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type                               EdgeOnCellContainer;
    typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type                                    EdgeOnCellIterator;            
    typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                               VertexOnEdgeContainer;
    typedef typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type                                  VertexOnEdgeIterator;            
    
    
    bool something_changed = true;
    //std::size_t iter_cnt = 0;
    
    while (something_changed)
    {
      something_changed = false;
      //std::cout << "Longest edge bisection, run " << ++iter_cnt << std::endl;
      
    
      CellContainer cells = viennagrid::ncells<CellTagIn::topology_level>(domain_in);
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        //
        // check if any edges are tagged for refinement
        //
        bool has_refinement = false;
        EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(*cit);
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
            VertexOnEdgeContainer vertices_on_edge = viennagrid::ncells<0>(*eocit);
            VertexOnEdgeIterator voeit = vertices_on_edge.begin();
            VertexType const & v0 = *voeit; ++voeit;
            VertexType const & v1 = *voeit;
            
            double len = viennagrid::norm(v0.getPoint() - v1.getPoint());
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
  void cell_refinement_to_edge_refinement(domain<ConfigTypeIn> const & domain_in)
  {
    typedef domain<ConfigTypeIn>                      DomainTypeIn;
    typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
    typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, CellTagIn::topology_level>::type              CellType;
    
    typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, CellTagIn::topology_level>::type   CellContainer;          
    typedef typename viennagrid::result_of::iterator<CellContainer>::type                                          CellIterator;         
    typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type                               EdgeOnCellContainer;
    typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type                                    EdgeOnCellIterator;            
    
    std::size_t cells_for_refinement = 0;
    CellContainer cells = viennagrid::ncells<CellTagIn::topology_level>(domain_in);
    for (CellIterator cit  = cells.begin();
                      cit != cells.end();
                    ++cit)
    {
      if (viennadata::access<refinement_key, bool>(refinement_key())(*cit) == true)
      {
        ++cells_for_refinement;
        
        EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(*cit);
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
  // Uniform refinement
  //
  template <typename ConfigTypeIn, typename ConfigTypeOut>
  void refine_impl(domain<ConfigTypeIn> const & domain_in,
                   domain<ConfigTypeOut> & domain_out,
                   adaptive_refinement_tag)
  {
    typedef domain<ConfigTypeIn>                      DomainTypeIn;
    typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
    typedef typename ConfigTypeIn::numeric_type       NumericType;

    typedef typename DomainTypeIn::segment_type                                                                    SegmentTypeIn; 
    typedef typename viennagrid::result_of::point_type<ConfigTypeIn>::type                                         PointType;
    typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type                                      VertexType;
    typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 1>::type                                      EdgeType;
    typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, CellTagIn::topology_level>::type              CellType;
    
    typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, 0>::type                           VertexContainer;          
    typedef typename viennagrid::result_of::iterator<VertexContainer>::type                                        VertexIterator;         
    typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, 1>::type                           EdgeContainer;          
    typedef typename viennagrid::result_of::iterator<EdgeContainer>::type                                          EdgeIterator;         
    typedef typename viennagrid::result_of::const_ncell_range<SegmentTypeIn, CellTagIn::topology_level>::type  CellContainer;          
    typedef typename viennagrid::result_of::iterator<CellContainer>::type                                          CellIterator;         
    typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type                               VertexOnCellContainer;
    typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type                                  VertexOnCellIterator;            
    typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                               VertexOnEdgeContainer;
    typedef typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type                                  VertexOnEdgeIterator;            


    //std::cout << "Refining domain adaptively..." << std::endl;
    
    
    //
    // Step 1: Write tags from cells to edges:
    //
    cell_refinement_to_edge_refinement(domain_in);
    ensure_longest_edge_refinement(domain_in);
    
    //
    // Step 2: Write old vertices to new domain
    //
    std::size_t num_vertices = 0;
    VertexContainer vertices = viennagrid::ncells<0>(domain_in);
    for (VertexIterator vit  = vertices.begin();
                        vit != vertices.end();
                      ++vit)
    {
      domain_out.add(*vit);
      ++num_vertices;
    }

    //
    // Step 3: Each tagged edge in old domain results in a new vertex (temporarily store new vertex IDs on old domain)
    //
    EdgeContainer edges = viennagrid::ncells<1>(domain_in);
    for (EdgeIterator eit = edges.begin();
                      eit != edges.end();
                    ++eit)
    {
      if (viennadata::access<refinement_key, bool>(refinement_key())(*eit) == true)
      {
        VertexType v;
        v.getPoint() = viennagrid::centroid(*eit);  //production value
        
        //Testing: Don't use midpoint:
        //VertexOnEdgeContainer vertices_on_edge = viennagrid::ncells<1>(*eit);
        //VertexOnEdgeIterator voeit = vertices_on_edge.begin();
        //VertexType const & v1 = *voeit; ++voeit;
        //VertexType const & v2 = *voeit;
        //double r = ((double)rand()/(double)RAND_MAX);
        //v.getPoint() = v1.getPoint() * r + v2.getPoint() * (1.0 - r);  //debug value
        
        v.id(num_vertices);
        domain_out.add(v);
        viennadata::access<refinement_key, std::size_t>(refinement_key())(*eit) = num_vertices;
        ++num_vertices;
      }
    }
    //std::cout << "Number of vertices in new domain: " << num_vertices << std::endl;

    //
    // Step 4: Now write new cells to new domain
    //
    domain_out.create_segments(domain_in.segment_size());
    for (std::size_t i=0; i<domain_in.segment_size(); ++i)
    {
      //std::cout << "Working on segment " << i << std::endl;
      CellContainer cells = viennagrid::ncells<CellTagIn::topology_level>(domain_in.segment(i));
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        element_refinement<CellTagIn>::apply(*cit, domain_out.segment(i));
      }
    }

  }
  
  
  
  
  //
  // Uniform refinement
  //
  template <typename ConfigTypeIn, typename ConfigTypeOut>
  void refine_impl(domain<ConfigTypeIn> const & domain_in,
                   domain<ConfigTypeOut> & domain_out,
                   uniform_refinement_tag)
  {
    typedef domain<ConfigTypeIn>                      DomainTypeIn;
    typedef typename ConfigTypeIn::cell_tag           CellTagIn;   
    typedef typename ConfigTypeIn::numeric_type       NumericType;

    typedef typename DomainTypeIn::segment_type                                                                    SegmentTypeIn; 
    typedef typename viennagrid::result_of::point_type<ConfigTypeIn>::type                                         PointType;
    typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type                                      VertexType;
    typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 1>::type                                      EdgeType;
    typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, CellTagIn::topology_level>::type              CellType;
    
    typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, 0>::type                           VertexContainer;          
    typedef typename viennagrid::result_of::iterator<VertexContainer>::type                                        VertexIterator;         
    typedef typename viennagrid::result_of::const_ncell_range<DomainTypeIn, 1>::type                           EdgeContainer;          
    typedef typename viennagrid::result_of::iterator<EdgeContainer>::type                                          EdgeIterator;         
    typedef typename viennagrid::result_of::const_ncell_range<SegmentTypeIn, CellTagIn::topology_level>::type  CellContainer;          
    typedef typename viennagrid::result_of::iterator<CellContainer>::type                                          CellIterator;         
    typedef typename viennagrid::result_of::const_ncell_range<EdgeType, 0>::type                               VertexOnEdgeContainer;
    typedef typename viennagrid::result_of::iterator<VertexOnEdgeContainer>::type                                  VertexOnEdgeIterator;            
    typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type                               VertexOnCellContainer;
    typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type                                  VertexOnCellIterator;            
    
    std::cout << "Refining domain uniformly..." << std::endl;
    
    //
    // Step 1: Write old vertices to new domain
    //
    std::size_t num_vertices = 0;
    VertexContainer vertices = viennagrid::ncells<0>(domain_in);
    for (VertexIterator vit  = vertices.begin();
                        vit != vertices.end();
                      ++vit)
    {
      domain_out.add(*vit);
      ++num_vertices;
    }

    //
    // Step 2: Each edge in old domain results in a new vertex (temporarily store new vertex IDs on old domain)
    //
    EdgeContainer edges = viennagrid::ncells<1>(domain_in);
    for (EdgeIterator eit = edges.begin();
                      eit != edges.end();
                    ++eit)
    {
      VertexType v;
      v.getPoint() = viennagrid::centroid(*eit);  //production value

      //Testing: Don't use midpoint:
      //VertexOnEdgeContainer vertices_on_edge = viennagrid::ncells<1>(*eit);
      //VertexOnEdgeIterator voeit = vertices_on_edge.begin();
      //VertexType const & v1 = *voeit; ++voeit;
      //VertexType const & v2 = *voeit;
      //double r=((double)rand()/(double)RAND_MAX);
      //v.getPoint() = v1.getPoint() * r + v2.getPoint() * (1.0 - r);  //debug value
      
      v.id(num_vertices);
      domain_out.add(v);
      viennadata::access<refinement_key, std::size_t>(refinement_key())(*eit) = num_vertices;
      viennadata::access<refinement_key, bool>(refinement_key())(*eit) = true;
      ++num_vertices;
    }
    std::cout << "Number of vertices in new domain: " << num_vertices << std::endl;

    //
    // Step 3: Now write new cells to new domain
    //
    domain_out.create_segments(domain_in.segment_size());
    for (std::size_t i=0; i<domain_in.segment_size(); ++i)
    {
      std::cout << "Working on segment " << i << std::endl;
      std::size_t counter = 0;
      CellContainer cells = viennagrid::ncells<CellTagIn::topology_level>(domain_in.segment(i));
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        element_refinement<CellTagIn>::apply(*cit, domain_out.segment(i));
        ++counter;
      }
      std::cout << "Refined cells in segment: " << counter << std::endl;
    }

  }
  
  
  
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
  refinement_proxy< domain<ConfigTypeIn>, RefinementTag >
  refine(domain<ConfigTypeIn> const & domain_in, RefinementTag const & tag)
  {
    return refinement_proxy< domain<ConfigTypeIn>, RefinementTag >(domain_in, tag);
  }
  

} 

#endif
