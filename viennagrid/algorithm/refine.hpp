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
#include "viennadata/api.hpp"

namespace viennagrid 
{
  
  
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
    typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, CellTagIn::topology_level>::type              CellType;
    
    typedef typename viennagrid::result_of::const_ncell_container<DomainTypeIn, 0>::type                           VertexContainer;          
    typedef typename viennagrid::result_of::iterator<VertexContainer>::type                                        VertexIterator;         
    typedef typename viennagrid::result_of::const_ncell_container<DomainTypeIn, 1>::type                           EdgeContainer;          
    typedef typename viennagrid::result_of::iterator<EdgeContainer>::type                                          EdgeIterator;         
    typedef typename viennagrid::result_of::const_ncell_container<SegmentTypeIn, CellTagIn::topology_level>::type  CellContainer;          
    typedef typename viennagrid::result_of::iterator<CellContainer>::type                                          CellIterator;         
    typedef typename viennagrid::result_of::const_ncell_container<CellType, 0>::type                               VertexOnCellContainer;
    typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type                                  VertexOnCellIterator;            
    
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
      v.getPoint() = viennagrid::centroid(*eit);
      v.setID(num_vertices);
      domain_out.add(v);
      viennadata::access<refinement_key, std::size_t>(refinement_key())(*eit) = num_vertices;
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
      CellContainer cells = viennagrid::ncells<CellTagIn::topology_level>(domain_in.segment(i));
      for (CellIterator cit  = cells.begin();
                        cit != cells.end();
                      ++cit)
      {
        element_refinement<CellTagIn>::apply(*cit, domain_out.segment(i));
      }
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
