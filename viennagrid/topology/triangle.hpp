/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_TOPOLOGY_TRIANGLE_HPP
#define VIENNAGRID_TOPOLOGY_TRIANGLE_HPP

#include "viennagrid/forwards.h"
#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"

namespace viennagrid
{

  struct triangle_tag
  {
    enum{ topology_level = 2 };
    static std::string name() { return "Triangle"; }
  };

  
  namespace traits
  {
    //Triangle:
    template <>
    struct subcell_desc<triangle_tag, 0>
    {
      typedef point_tag             element_tag;
      typedef full_handling_tag     handling_tag;

      enum{ num_elements = 3 };     //3 vertices
    };

    template <>
    struct subcell_desc<triangle_tag, 1>
    {
      typedef line_tag              element_tag;
      typedef full_handling_tag     handling_tag;

      enum{ num_elements = 3 };     //3 edges

    };
  
    ///////////////////////////////// Filler for subcell elements ///////////////////////////////////
    

    //////// Triangle ////////
    template <>
    struct subcell_filler<triangle_tag, 1>
    {
      //fill edges:
      template <typename ElementType, typename Vertices, typename Orientations, typename Segment>
      static void fill(ElementType ** elements, Vertices ** vertices, Orientations * orientations, Segment & seg)
      {
        Vertices * edgevertices[2];
        ElementType edge;

        edgevertices[0] = vertices[0];
        edgevertices[1] = vertices[1];
        edge.setVertices(edgevertices);
        elements[0] = seg.add(edge, orientations);

        edgevertices[0] = vertices[0];
        edgevertices[1] = vertices[2];
        edge.setVertices(edgevertices);
        elements[1] = seg.add(edge, orientations + 1 );

        edgevertices[0] = vertices[1];
        edgevertices[1] = vertices[2];
        edge.setVertices(edgevertices);
        elements[2] = seg.add(edge, orientations + 2 );
      }
    };

  } //traits
  
  
  
  //
  // Uniform refinement of a triangle: Split the cell into 8 tets
  //
  template <>
  struct element_refinement<triangle_tag>
  {
    template <typename CellType, typename DomainTypeOut>
    static void apply(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_ncell_container<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_container<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;

      VertexType * vertices[traits::subcell_desc<triangle_tag, 0>::num_elements
                            + traits::subcell_desc<triangle_tag, 1>::num_elements];
      
      //
      // Step 1: Get vertices on the new domain
      //
      
      //grab existing vertices:
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->getID()));

      //add vertices from edge
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      vertices[3] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(*eocit))); ++eocit;
      vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(*eocit))); ++eocit;
      vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(*eocit)));
      
      //
      // Step 2: Add new cells to new domain:
      //
      CellType new_cell;
      VertexType * cellvertices[traits::subcell_desc<triangle_tag, 0>::num_elements];
      
      //0-3-4:
      cellvertices[0] = vertices[0];
      cellvertices[1] = vertices[3];
      cellvertices[2] = vertices[4];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);

      //3-1-5:
      cellvertices[0] = vertices[3];
      cellvertices[1] = vertices[1];
      cellvertices[2] = vertices[5];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);

      //4-5-2:
      cellvertices[0] = vertices[4];
      cellvertices[1] = vertices[5];
      cellvertices[2] = vertices[2];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);

      //4-3-5:
      cellvertices[0] = vertices[4];
      cellvertices[1] = vertices[3];
      cellvertices[2] = vertices[5];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
    } //apply()
    
  };
  
  
  
  
  
  
    
}

#endif