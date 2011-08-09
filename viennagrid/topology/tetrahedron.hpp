/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaGrid - The Vienna Grid Library
                             -----------------

   authors:    Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaGrid base directory
======================================================================= */

#ifndef VIENNAGRID_TOPOLOGY_TETRAHEDRON_HPP
#define VIENNAGRID_TOPOLOGY_TETRAHEDRON_HPP

#include "viennagrid/forwards.h"
#include "viennagrid/topology/point.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/triangle.hpp"
#include "viennadata/api.hpp"
#include "viennagrid/algorithm/norm.hpp"

namespace viennagrid
{


  struct tetrahedron_tag
  {
    enum{ topology_level = 3 };
    static std::string name() { return "Tetrahedron"; }
  };


  namespace traits
  {

    //Tetrahedron:
    template <>
    struct subcell_desc<tetrahedron_tag, 0>
    {
      typedef point_tag             element_tag;
      typedef full_handling_tag     handling_tag;

      enum{ num_elements = 4 };     //4 vertices
    };

    template <>
    struct subcell_desc<tetrahedron_tag, 1>
    {
      typedef line_tag              element_tag;
      typedef full_handling_tag     handling_tag;

      enum{ num_elements = 6 };     //6 edges
    };

    template <>
    struct subcell_desc<tetrahedron_tag, 2>
    {
      typedef triangle_tag          element_tag;
      typedef full_handling_tag     handling_tag;

      enum{ num_elements = 4 };     //4 facets
    };
    
    //////// Tetrahedron ////////
    template <>
    struct subcell_filler<tetrahedron_tag, 1>
    {
      //fill edges:
      template <typename ElementType, typename Vertices, typename Orientations, typename Segment>
      static void fill(ElementType ** elements, Vertices ** vertices, Orientations * orientations, Segment & seg)
      {
        Vertices * edgevertices[2];
        ElementType edge;

        //fill edges according to orientation and ordering induced by k-tuple-metafunction:
        edgevertices[0] = vertices[0];
        edgevertices[1] = vertices[1];
        edge.setVertices(edgevertices);
        elements[0] = seg.add(edge, orientations);

        edgevertices[0] = vertices[0];
        edgevertices[1] = vertices[2];
        edge.setVertices(edgevertices);
        elements[1] = seg.add(edge, orientations + 1);

        edgevertices[0] = vertices[0];
        edgevertices[1] = vertices[3];
        edge.setVertices(edgevertices);
        elements[2] = seg.add(edge, orientations + 2);

        edgevertices[0] = vertices[1];
        edgevertices[1] = vertices[2];
        edge.setVertices(edgevertices);
        elements[3] = seg.add(edge, orientations + 3);

        edgevertices[0] = vertices[1];
        edgevertices[1] = vertices[3];
        edge.setVertices(edgevertices);
        elements[4] = seg.add(edge, orientations + 4);

        edgevertices[0] = vertices[2];
        edgevertices[1] = vertices[3];
        edge.setVertices(edgevertices);
        elements[5] = seg.add(edge, orientations + 5);
      }
    };
    
    template <>
    struct subcell_filler<tetrahedron_tag, 2>
    {
      //fill facets:
      template <typename ElementType, typename Vertices, typename Orientations, typename Segment>
      static void fill(ElementType ** elements, Vertices ** vertices, Orientations * orientations, Segment & seg)
      {
        Vertices * facetvertices[3];
        ElementType facet;

        //fill edges according to orientation and ordering induced by k-tuple-metafunction:

        facetvertices[0] = vertices[0];
        facetvertices[1] = vertices[1];
        facetvertices[2] = vertices[2];
        facet.setVertices(facetvertices);
        elements[0] = seg.add(facet, orientations );
        //this new facet must be initialized too:
        elements[0]->fill(seg);

        facetvertices[0] = vertices[0];
        facetvertices[1] = vertices[1];
        facetvertices[2] = vertices[3];
        facet.setVertices(facetvertices);
        elements[1] = seg.add(facet, orientations + 1 );
        elements[1]->fill(seg);

        facetvertices[0] = vertices[0];
        facetvertices[1] = vertices[2];
        facetvertices[2] = vertices[3];
        facet.setVertices(facetvertices);
        elements[2] = seg.add(facet, orientations + 2 );
        elements[2]->fill(seg);

        facetvertices[0] = vertices[1];
        facetvertices[1] = vertices[2];
        facetvertices[2] = vertices[3];
        facet.setVertices(facetvertices);
        elements[3] = seg.add(facet, orientations + 3 );
        elements[3]->fill(seg);

      }
    };

  } //traits
  
  
  
  //
  // Uniform refinement of a tetrahedron: Split the cell into 8 tets
  //
  template <>
  struct element_refinement<tetrahedron_tag>
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

      VertexType * vertices[traits::subcell_desc<tetrahedron_tag, 0>::num_elements
                            + traits::subcell_desc<tetrahedron_tag, 1>::num_elements];
      
      //
      // Step 1: Get vertices on the new domain
      //
      
      //grab existing vertices:
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[3] = &(segment_out.get_domain().vertex(vocit->getID()));

      //add vertices from edge
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(*eocit))); ++eocit;
      vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(*eocit))); ++eocit;
      vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(*eocit))); ++eocit;
      vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(*eocit))); ++eocit;
      vertices[8] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(*eocit))); ++eocit;
      vertices[9] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(*eocit)));
      
      //
      // Step 2: Add new cells to new domain:
      //
      CellType new_cell;
      VertexType * cellvertices[traits::subcell_desc<tetrahedron_tag, 0>::num_elements];
      
      //0-4-5-6:
      cellvertices[0] = vertices[0];
      cellvertices[1] = vertices[4];
      cellvertices[2] = vertices[5];
      cellvertices[3] = vertices[6];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);

      //1-7-4-8:
      cellvertices[0] = vertices[1];
      cellvertices[1] = vertices[7];
      cellvertices[2] = vertices[4];
      cellvertices[3] = vertices[8];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);

      //2-5-7-9:
      cellvertices[0] = vertices[2];
      cellvertices[1] = vertices[5];
      cellvertices[2] = vertices[7];
      cellvertices[3] = vertices[9];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);

      //3-8-6-9:
      cellvertices[0] = vertices[3];
      cellvertices[1] = vertices[8];
      cellvertices[2] = vertices[6];
      cellvertices[3] = vertices[9];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
      double diag58 = viennagrid::norm(vertices[5]->getPoint() - vertices[8]->getPoint());
      double diag67 = viennagrid::norm(vertices[6]->getPoint() - vertices[7]->getPoint());
      double diag49 = viennagrid::norm(vertices[4]->getPoint() - vertices[9]->getPoint());
      
      if ( (diag58 <= diag67) && (diag58 <= diag49) )  //diag58 is shortest: keep it, split others
      {
        //4-8-5-6:
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[8];
        cellvertices[2] = vertices[5];
        cellvertices[3] = vertices[6];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
        
        //4-8-7-5:
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[8];
        cellvertices[2] = vertices[7];
        cellvertices[3] = vertices[5];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);

        //7-8-9-5:
        cellvertices[0] = vertices[7];
        cellvertices[1] = vertices[8];
        cellvertices[2] = vertices[9];
        cellvertices[3] = vertices[5];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);

        //8-6-9-5:
        cellvertices[0] = vertices[8];
        cellvertices[1] = vertices[6];
        cellvertices[2] = vertices[9];
        cellvertices[3] = vertices[5];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
      }
      else if ( (diag67 <= diag58) && (diag67 <= diag49) ) //diag67 is shortest: keep it, split others
      {
        //4-7-6-8:
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[7];
        cellvertices[2] = vertices[6];
        cellvertices[3] = vertices[8];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
        
        //4-7-5-6:
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[7];
        cellvertices[2] = vertices[5];
        cellvertices[3] = vertices[6];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);

        //7-9-6-8:
        cellvertices[0] = vertices[7];
        cellvertices[1] = vertices[9];
        cellvertices[2] = vertices[6];
        cellvertices[3] = vertices[8];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);

        //7-9-5-6:
        cellvertices[0] = vertices[7];
        cellvertices[1] = vertices[9];
        cellvertices[2] = vertices[5];
        cellvertices[3] = vertices[6];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
      }
      else //keep shortest diagonal diag49
      {
        //4-9-6-8:
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[9];
        cellvertices[2] = vertices[6];
        cellvertices[3] = vertices[8];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
        
        //4-9-5-6:
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[9];
        cellvertices[2] = vertices[5];
        cellvertices[3] = vertices[6];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);

        //4-7-9-8:
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[7];
        cellvertices[2] = vertices[9];
        cellvertices[3] = vertices[8];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);

        //4-7-5-9:
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[7];
        cellvertices[2] = vertices[5];
        cellvertices[3] = vertices[9];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
      }
      
    } //apply()
    
  };
  
    
  
}

#endif