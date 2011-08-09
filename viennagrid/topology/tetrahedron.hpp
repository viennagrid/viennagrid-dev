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
  // Refinement of a tetrahedron
  //
  template <>
  struct element_refinement<tetrahedron_tag>
  {

    //
    // Not refining a tetrahedron at all
    //
    template <typename CellType, typename DomainTypeOut>
    static void apply0(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_ncell_container<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_container<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;

      VertexType * vertices[traits::subcell_desc<tetrahedron_tag, 0>::num_elements];
      
      // Step 1: grab existing vertices:
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[3] = &(segment_out.get_domain().vertex(vocit->getID()));

      // Step 2: Add new cells to new domain:
      CellType new_cell;
      new_cell.setVertices(vertices);
      segment_out.add(new_cell);

    }    

    //
    // Refinement of a tetrahedron, bisecting one edge
    //
    template <typename CellType, typename DomainTypeOut>
    static void apply1(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_ncell_container<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_container<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 1>::type             EdgeType;

      VertexType * vertices[traits::subcell_desc<tetrahedron_tag, 0>::num_elements];
      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[3] = &(segment_out.get_domain().vertex(vocit->getID()));
      

      //
      // Step 2: Bring vertices in correct order, such that refined edge is on {0,1}-edge
      //
      VertexType * ordered_vertices[traits::subcell_desc<tetrahedron_tag, 0>::num_elements + 1];
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
      std::size_t offset = 0;
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit; ++eocit;
      EdgeType const & e3 = *eocit; ++eocit;
      EdgeType const & e4 = *eocit; ++eocit;
      EdgeType const & e5 = *eocit;
      
      if (viennadata::access<refinement_key, bool>(refinement_key())(e0) == true)
      {
        ordered_vertices[0] = vertices[0];
        ordered_vertices[1] = vertices[1];
        ordered_vertices[2] = vertices[2];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e1) == true)
      {
        ordered_vertices[0] = vertices[2];
        ordered_vertices[1] = vertices[0];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true)
      {
        ordered_vertices[0] = vertices[0];
        ordered_vertices[1] = vertices[3];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[2];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true)
      {
        ordered_vertices[0] = vertices[1];
        ordered_vertices[1] = vertices[2];
        ordered_vertices[2] = vertices[0];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true)
      {
        ordered_vertices[0] = vertices[3];
        ordered_vertices[1] = vertices[1];
        ordered_vertices[2] = vertices[0];
        ordered_vertices[3] = vertices[2];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true)
      {
        ordered_vertices[0] = vertices[3];
        ordered_vertices[1] = vertices[2];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[0];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!"); 
      }
      
      
      //
      // Step 3: Write new cells to domain_out
      //
      CellType new_cell;
      VertexType * cellvertices[traits::subcell_desc<triangle_tag, 0>::num_elements];
      
      //cell containing vertex 0:
      cellvertices[0] = ordered_vertices[0];
      cellvertices[1] = ordered_vertices[4];
      cellvertices[2] = ordered_vertices[2];
      cellvertices[3] = ordered_vertices[3];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);

      //cell without vertex 0:
      cellvertices[0] = ordered_vertices[4];
      cellvertices[1] = ordered_vertices[1];
      cellvertices[2] = ordered_vertices[2];
      cellvertices[3] = ordered_vertices[3];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
    }    


    //
    // Refinement of a tetrahedron, bisecting two edges
    //
    
    // Case 1: The two edges have a common vertex. Orientation: (dots are in the background)
    /*
     *            3 ----------- 2
     *          /   \      .   /
     *         /      \ .     5
     *        /     .   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexType>
    static void apply2_1(CellType const & cell_in, 
                         DomainTypeOut & segment_out,
                         VertexType const * vertices
                        )
    {
      CellType new_cell;
      VertexType * cellvertices[traits::subcell_desc<tetrahedron_tag, 0>::num_elements];

      cellvertices[0] = vertices[4];
      cellvertices[1] = vertices[1];
      cellvertices[2] = vertices[5];
      cellvertices[3] = vertices[3];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
      double diag01_len = viennagrid::norm(vertices[0]->getPoint() - vertices[1]->getPoint());
      double diag12_len = viennagrid::norm(vertices[2]->getPoint() - vertices[1]->getPoint());

      if (diag01_len > diag12_len) //split edge 01, introduce line 42
      {
        cellvertices[0] = vertices[0];
        cellvertices[1] = vertices[4];
        cellvertices[2] = vertices[2];
        cellvertices[3] = vertices[3];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
        
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[5];
        cellvertices[2] = vertices[2];
        cellvertices[3] = vertices[3];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
      }
      else //split edge 12, introduce line 05
      {
        cellvertices[0] = vertices[0];
        cellvertices[1] = vertices[4];
        cellvertices[2] = vertices[5];
        cellvertices[3] = vertices[3];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
        
        cellvertices[0] = vertices[0];
        cellvertices[1] = vertices[5];
        cellvertices[2] = vertices[2];
        cellvertices[3] = vertices[3];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
      }
      
    }

    // Case 2: The two edges don't have any common vertex. Orientation: (dots are in the background)
    /*
     *            3 -----5----- 2
     *          /   \      .   /
     *         /      \ .     /
     *        /     .   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexType>
    static void apply2_2(CellType const & cell_in, 
                         DomainTypeOut & segment_out,
                         VertexType const * vertices
                        )
    {
      CellType new_cell;
      VertexType * cellvertices[traits::subcell_desc<tetrahedron_tag, 0>::num_elements];
      
      //TODO
      //cell containing vertex 0:
      /*cellvertices[0] = vertices[];
      cellvertices[1] = vertices[];
      cellvertices[2] = vertices[];
      cellvertices[3] = vertices[];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);*/

    }

    // Reorders the tetrahedron 
    template <typename CellType, typename DomainTypeOut>
    static void apply2(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_ncell_container<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_container<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 1>::type             EdgeType;

      VertexType * vertices[traits::subcell_desc<tetrahedron_tag, 0>::num_elements];
      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->getID())); ++vocit;
      vertices[3] = &(segment_out.get_domain().vertex(vocit->getID()));
      

      //
      // Step 2: Bring vertices in correct order, such that refined edge is on {0,1}-edge
      //
      VertexType * ordered_vertices[traits::subcell_desc<tetrahedron_tag, 0>::num_elements + 2];
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
      std::size_t offset = 0;
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit; ++eocit;
      EdgeType const & e3 = *eocit; ++eocit;
      EdgeType const & e4 = *eocit; ++eocit;
      EdgeType const & e5 = *eocit;
      
      /* TODO
      //with e0
      if (viennadata::access<refinement_key, bool>(refinement_key())(e0) == true)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e1) == true)
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true)
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          
          apply2_2(cell_in, segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e1) == true)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true)
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          apply2_2(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          
          apply2_2(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true) 
        {
          ordered_vertices[0] = vertices[];
          ordered_vertices[1] = vertices[];
          ordered_vertices[2] = vertices[];
          ordered_vertices[3] = vertices[];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!"); 
      }
      */
    }    

    //
    // Refinement of a tetrahedron, bisecting three edges
    //
    template <typename CellType, typename DomainTypeOut>
    static void apply3(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      assert(false && "Not yet implemented!");
    }    

    //
    // Refinement of a tetrahedron, bisecting four edges
    //
    template <typename CellType, typename DomainTypeOut>
    static void apply4(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      assert(false && "Not yet implemented!");
    }    

    
    //
    // Refinement of a tetrahedron, bisecting five edges
    //
    template <typename CellType, typename DomainTypeOut>
    static void apply5(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      assert(false && "Not yet implemented!");
    }    



    //
    // Refinement of a tetrahedron, bisecting six edges: Split the cell into 8 tets
    //
    template <typename CellType, typename DomainTypeOut>
    static void apply6(CellType const & cell_in, DomainTypeOut & segment_out)
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
      
    } //apply6()
    
    
    //
    // public interface function
    //
    template <typename CellType, typename DomainTypeOut>
    static void apply(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename viennagrid::result_of::const_ncell_container<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type                 EdgeOnCellIterator;            
      
      std::size_t edges_to_refine = 0;
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
      for (EdgeOnCellIterator eocit = edges_on_cell.begin();
                              eocit != edges_on_cell.end();
                            ++eocit)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(*eocit) == true)
          ++edges_to_refine;
      }
      
      switch (edges_to_refine)
      {
        case 0: apply0(cell_in, segment_out); break;
        case 1: apply1(cell_in, segment_out); break;
        case 2: apply2(cell_in, segment_out); break;
        case 3: apply3(cell_in, segment_out); break;
        case 4: apply4(cell_in, segment_out); break;
        case 5: apply5(cell_in, segment_out); break;
        case 6: apply6(cell_in, segment_out); break;
        default: //nothing to do...
                break;
      }
    } //apply()
    
  };
  
    
  
}

#endif