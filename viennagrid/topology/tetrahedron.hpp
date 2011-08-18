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


  namespace topology
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
  
  
  
  /** @brief Compares the lines (v1_1, v1_2) and (v2_1, v2_2) and returns true if the first is longer. 
   * 
   * Ensures that the result is the same no matter in which order the parameters are passed. 
   * If the two lines have equal length, the line with the larger vertex IDs involved is considered as longer.
   */
  template <typename VertexType>
  bool stable_line_is_longer(VertexType const * v1_1, VertexType const * v1_2,  
                             VertexType const * v2_1, VertexType const * v2_2)
  {
    typedef typename VertexType::config_type::numeric_type       ScalarType;
    
    VertexType const * v1_1_ptr = (v1_1->id() < v1_2->id()) ? v1_1 : v1_2; //v1_1 carries smaller ID
    VertexType const * v1_2_ptr = (v1_1->id() < v1_2->id()) ? v1_2 : v1_1; //v1_2 carries larger ID
    
    VertexType const * v2_1_ptr = (v2_1->id() < v2_2->id()) ? v2_1 : v2_2; //v2_1 carries smaller ID
    VertexType const * v2_2_ptr = (v2_1->id() < v2_2->id()) ? v2_2 : v2_1; //v2_2 carries larger ID
    
    ScalarType line1 = viennagrid::norm(v1_1->getPoint() - v1_2->getPoint());
    ScalarType line2 = viennagrid::norm(v2_1->getPoint() - v2_2->getPoint());
    
    if (line1 > line2)
    {
      return true;
    }
    else if (line1 == line2)
    {
      //compare IDs:
      if (v1_1_ptr->id() > v2_1_ptr->id())
      {
        return true;
      }
      else if (v1_1_ptr->id() == v2_1_ptr->id())
      {
        if (v1_2_ptr->id() > v2_2_ptr->id())
          return true;
        else if (v1_2_ptr->id() == v2_2_ptr->id())
          return false; //identical lines are compared!
      }
    }

    return false;
  }
  
  
  
  //
  // Refinement of a tetrahedron: A LOT of spaghetti-code to follow. 
  // Unfortunately, combinatorics forces us to consider that many cases of refinement.
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
      //std::cout << "tetrahedron::apply0()" << std::endl;
      typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;

      VertexType * vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];
      
      // Step 1: grab existing vertices:
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[3] = &(segment_out.get_domain().vertex(vocit->id()));

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
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 1>::type             EdgeType;

      VertexType * vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];
      //std::cout << "apply1()" << std::endl;
      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[3] = &(segment_out.get_domain().vertex(vocit->id()));
      

      //
      // Step 2: Bring vertices in correct order, such that refined edge is on {0,1}-edge
      //
      VertexType * ordered_vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements + 1];
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
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
      VertexType * cellvertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];
      
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


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
                         VertexType ** vertices
                        )
    {
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];

      cellvertices[0] = vertices[4];
      cellvertices[1] = vertices[1];
      cellvertices[2] = vertices[5];
      cellvertices[3] = vertices[3];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
      //double diag01_len = viennagrid::norm(vertices[0]->getPoint() - vertices[1]->getPoint());
      //double diag12_len = viennagrid::norm(vertices[2]->getPoint() - vertices[1]->getPoint());

      //if (diag01_len > diag12_len) //split edge 01, introduce line 42
      if (stable_line_is_longer(vertices[0], vertices[1],
                                vertices[2], vertices[1])) //split edge 01, introduce line 42
      {
        /*std::cout << "Norm " << vertices[0]->id() << vertices[1]->id() << ": " 
                             << viennagrid::norm(vertices[0]->getPoint() - vertices[1]->getPoint()) << std::endl;
        std::cout << "Norm " << vertices[2]->id() << vertices[1]->id() << ": " 
                             << viennagrid::norm(vertices[2]->getPoint() - vertices[1]->getPoint()) << std::endl;
        std::cout << "Splitting " << vertices[0]->id() << vertices[1]->id() << std::endl;*/
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
        /*std::cout << "Norm " << vertices[0]->id() << vertices[1]->id() << ": " 
                             << viennagrid::norm(vertices[0]->getPoint() - vertices[1]->getPoint()) << std::endl;
        std::cout << "Norm " << vertices[2]->id() << vertices[1]->id() << ": " 
                             << viennagrid::norm(vertices[2]->getPoint() - vertices[1]->getPoint()) << std::endl;
        std::cout << "Splitting " << vertices[2]->id() << vertices[1]->id() << std::endl;*/
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
     */
    template <typename CellType, typename DomainTypeOut, typename VertexType>
    static void apply2_2(CellType const & cell_in, 
                         DomainTypeOut & segment_out,
                         VertexType ** vertices
                        )
    {
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];

      cellvertices[0] = vertices[4];
      cellvertices[1] = vertices[1];
      cellvertices[2] = vertices[2];
      cellvertices[3] = vertices[5];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
      cellvertices[0] = vertices[4];
      cellvertices[1] = vertices[1];
      cellvertices[2] = vertices[5];
      cellvertices[3] = vertices[3];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
      cellvertices[0] = vertices[0];
      cellvertices[1] = vertices[4];
      cellvertices[2] = vertices[2];
      cellvertices[3] = vertices[5];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
      cellvertices[0] = vertices[0];
      cellvertices[1] = vertices[4];
      cellvertices[2] = vertices[5];
      cellvertices[3] = vertices[3];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
    }

    // Reorders the tetrahedron 
    template <typename CellType, typename DomainTypeOut>
    static void apply2(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 1>::type             EdgeType;

      VertexType * vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];
      //std::cout << "apply2()" << std::endl;
      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[3] = &(segment_out.get_domain().vertex(vocit->id()));
      

      //
      // Step 2: Bring vertices in correct order, such that refined edge is on {0,1}-edge
      //
      VertexType * ordered_vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements + 2];
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit; ++eocit;
      EdgeType const & e3 = *eocit; ++eocit;
      EdgeType const & e4 = *eocit; ++eocit;
      EdgeType const & e5 = *eocit;
      
      //with e0
      if (viennadata::access<refinement_key, bool>(refinement_key())(e0) == true)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e1) == true)
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true)
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true) 
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[3];
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
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true) 
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true) 
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          apply2_2(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[1];
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
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          
          apply2_2(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true) 
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          
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
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          apply2_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true) 
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          
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
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[0];
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
    }    



    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //
    // Refinement of a tetrahedron, bisecting three edges
    //
    
    // Case 1: The three edges have a common vertex. Orientation: (dots are in the background)
    /*
     *            3 ----------- 2
     *          /   \       .  /
     *         /      6 .     5
     *        /     .   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexType>
    static void apply3_1(CellType const & cell_in, 
                         DomainTypeOut & segment_out,
                         VertexType ** vertices
                        )
    {
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];

      cellvertices[0] = vertices[4];
      cellvertices[1] = vertices[1];
      cellvertices[2] = vertices[5];
      cellvertices[3] = vertices[6];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
      //double diag01_len = viennagrid::norm(vertices[0]->getPoint() - vertices[1]->getPoint());
      //double diag12_len = viennagrid::norm(vertices[2]->getPoint() - vertices[1]->getPoint());
      //double diag13_len = viennagrid::norm(vertices[3]->getPoint() - vertices[1]->getPoint());

      // Strategy: The two longest edges of the common vertex are split 'twice',
      //           i.e. two new edges start from the center of the two longest edges
      //if (diag01_len > diag12_len) //split edge 01 again, introduce line 42
      if (stable_line_is_longer(vertices[0], vertices[1],
                                vertices[1], vertices[2])) //split edge 01 again, introduce line 42
      {
        /*std::cout << "Norm " << vertices[0]->id() << vertices[1]->id() << ": " 
                             << viennagrid::norm(vertices[0]->getPoint() - vertices[1]->getPoint()) << std::endl;
        std::cout << "Norm " << vertices[2]->id() << vertices[1]->id() << ": " 
                             << viennagrid::norm(vertices[2]->getPoint() - vertices[1]->getPoint()) << std::endl;
        std::cout << "Splitting " << vertices[0]->id() << vertices[1]->id() << std::endl;*/
        //if (diag13_len > diag12_len) //split edge 13 again, introduce line 62
        if (stable_line_is_longer(vertices[1], vertices[3],
                                  vertices[1], vertices[2])) //split edge 13 again, introduce line 62
        {
          /*std::cout << "Norm " << vertices[1]->id() << vertices[3]->id() << ": " 
                                << viennagrid::norm(vertices[1]->getPoint() - vertices[3]->getPoint()) << std::endl;
          std::cout << "Norm " << vertices[2]->id() << vertices[1]->id() << ": " 
                                << viennagrid::norm(vertices[2]->getPoint() - vertices[1]->getPoint()) << std::endl;
          std::cout << "Splitting " << vertices[1]->id() << vertices[3]->id() << std::endl; */
          
          if (stable_line_is_longer(vertices[1], vertices[3],
                                    vertices[0], vertices[1])) //split edge 13 again, introduce line 60
          {
            cellvertices[0] = vertices[0];
            cellvertices[1] = vertices[6];
            cellvertices[2] = vertices[2];
            cellvertices[3] = vertices[3];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
            
            cellvertices[0] = vertices[0];
            cellvertices[1] = vertices[4];
            cellvertices[2] = vertices[2];
            cellvertices[3] = vertices[6];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
            
            cellvertices[0] = vertices[4];
            cellvertices[1] = vertices[5];
            cellvertices[2] = vertices[2];
            cellvertices[3] = vertices[6];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
          }
          else  //split edge 01 again, introduce line 43
          {
            //std::cout << "apply_3_1_1" << std::endl;
            cellvertices[0] = vertices[0];
            cellvertices[1] = vertices[4];
            cellvertices[2] = vertices[2];
            cellvertices[3] = vertices[3];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
            
            cellvertices[0] = vertices[3];
            cellvertices[1] = vertices[4];
            cellvertices[2] = vertices[2];
            cellvertices[3] = vertices[6];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
            
            cellvertices[0] = vertices[4];
            cellvertices[1] = vertices[5];
            cellvertices[2] = vertices[2];
            cellvertices[3] = vertices[6];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
          }
        }
        else //split edge 12 again, introduce lines 43 and 53
        {
          if (stable_line_is_longer(vertices[1], vertices[3],
                                    vertices[0], vertices[1])) //split edge 13 again, introduce line 60
          {
            assert(false && "diag13_len > diag01_len impossible!");
          }
          else  //split edge 01 again, introduce line 43
          {
            //std::cout << "apply_3_1_2" << std::endl;
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
            
            cellvertices[0] = vertices[4];
            cellvertices[1] = vertices[6];
            cellvertices[2] = vertices[5];
            cellvertices[3] = vertices[3];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
          }
        }
      }
      else //split edge 12, introduce line 05
      {
        if (stable_line_is_longer(vertices[1], vertices[3],
                                  vertices[1], vertices[2])) //split edge 13 again, introduce line 62
        {
          if (stable_line_is_longer(vertices[1], vertices[3],
                                    vertices[0], vertices[1])) //split edge 13 again, introduce line 60
          {
            //std::cout << "apply_3_1_3" << std::endl;
            cellvertices[0] = vertices[0];
            cellvertices[1] = vertices[4];
            cellvertices[2] = vertices[5];
            cellvertices[3] = vertices[6];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
            
            cellvertices[0] = vertices[0];
            cellvertices[1] = vertices[6];
            cellvertices[2] = vertices[5];
            cellvertices[3] = vertices[2];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
            
            cellvertices[0] = vertices[0];
            cellvertices[1] = vertices[6];
            cellvertices[2] = vertices[2];
            cellvertices[3] = vertices[3];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
          }
          else  //split edge 01 again, introduce line 43
          {
            assert(false && "diag13_len > diag01_len impossible!");
          }
        }
        else //split edge 12 again, introduce line 53
        {
          if (stable_line_is_longer(vertices[1], vertices[3],
                                    vertices[0], vertices[1])) //split edge 13 again, introduce line 60
          {
            cellvertices[0] = vertices[0];
            cellvertices[1] = vertices[4];
            cellvertices[2] = vertices[5];
            cellvertices[3] = vertices[6];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
            
            cellvertices[0] = vertices[0];
            cellvertices[1] = vertices[6];
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
          else  //split edge 01 again, introduce line 43
          {
            //std::cout << "apply_3_1_4" << std::endl;
            cellvertices[0] = vertices[0];
            cellvertices[1] = vertices[4];
            cellvertices[2] = vertices[5];
            cellvertices[3] = vertices[3];
            new_cell.setVertices(cellvertices);
            segment_out.add(new_cell);
            
            cellvertices[0] = vertices[4];
            cellvertices[1] = vertices[5];
            cellvertices[2] = vertices[3];
            cellvertices[3] = vertices[6];
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
      }
      
    }


    // Case 2: The three edges don't have any common vertex. Orientation: (dots are in the background)
    /*
     *            3 ----------- 2
     *          /   \       .  /
     *         /      \ .     5
     *        /     6   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexType>
    static void apply3_2(CellType const & cell_in, 
                         DomainTypeOut & segment_out,
                         VertexType ** vertices
                        )
    {
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];

      //std::cout << "apply_3_2" << std::endl;
      
      cellvertices[0] = vertices[0];
      cellvertices[1] = vertices[4];
      cellvertices[2] = vertices[6];
      cellvertices[3] = vertices[3];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
      cellvertices[0] = vertices[4];
      cellvertices[1] = vertices[5];
      cellvertices[2] = vertices[6];
      cellvertices[3] = vertices[3];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
      cellvertices[0] = vertices[4];
      cellvertices[1] = vertices[1];
      cellvertices[2] = vertices[5];
      cellvertices[3] = vertices[3];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
      cellvertices[0] = vertices[6];
      cellvertices[1] = vertices[5];
      cellvertices[2] = vertices[2];
      cellvertices[3] = vertices[3];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
    }


    // Case 3: The three edges don't have any common vertex. Orientation: (dots are in the background)
    /*
     *            3 ----------- 2
     *          /   \       .  /
     *         6      \ .     5
     *        /     .   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexType>
    static void apply3_3(CellType const & cell_in, 
                         DomainTypeOut & segment_out,
                         VertexType ** vertices
                        )
    {
      //std::cout << "Found!" << std::endl;
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];

      //double diag01_len = viennagrid::norm(vertices[0]->getPoint() - vertices[1]->getPoint());
      //double diag12_len = viennagrid::norm(vertices[1]->getPoint() - vertices[2]->getPoint());
      //double diag03_len = viennagrid::norm(vertices[0]->getPoint() - vertices[3]->getPoint());

      // Strategy: The two longest edges of the common vertex are split 'twice',
      //           i.e. two new edges start from the center of the two longest edges
      //if (diag01_len > diag03_len) //split edge 01 again, introduce line 43
      if (stable_line_is_longer(vertices[0], vertices[1],
                                vertices[0], vertices[3])) //split edge 01 again, introduce line 43
      {
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[1];
        cellvertices[2] = vertices[5];
        cellvertices[3] = vertices[3];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
          
        //if (diag01_len > diag12_len) //split edge 01 again, introduce line 42
        if (stable_line_is_longer(vertices[0], vertices[1],
                                  vertices[1], vertices[2])) //split edge 01 again, introduce line 42
        {
          //std::cout << "apply_3_3_1" << std::endl;
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[2];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[6];
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
        else //split edge 12 again, introduce line 50
        {
          //std::cout << "apply_3_3_2" << std::endl;
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[5];
          cellvertices[2] = vertices[2];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[6];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[3];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[6];
          cellvertices[1] = vertices[5];
          cellvertices[2] = vertices[2];
          cellvertices[3] = vertices[3];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
      }
      else  //split edge 03 again, introduce line 61
      {
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[1];
        cellvertices[2] = vertices[5];
        cellvertices[3] = vertices[6];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
          
        cellvertices[0] = vertices[6];
        cellvertices[1] = vertices[1];
        cellvertices[2] = vertices[5];
        cellvertices[3] = vertices[3];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
          
        cellvertices[0] = vertices[6];
        cellvertices[1] = vertices[5];
        cellvertices[2] = vertices[2];
        cellvertices[3] = vertices[3];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
          
        //if (diag01_len > diag12_len) //split edge 01 again, introduce line 42
        if (stable_line_is_longer(vertices[0], vertices[1],
                                  vertices[1], vertices[2])) //split edge 01 again, introduce line 42
        {
          //std::cout << "apply_3_3_3" << std::endl;
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[2];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[6];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[2];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
        else //split edge 12 again, introduce line 50
        {
          //std::cout << "apply_3_3_4" << std::endl;
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[5];
          cellvertices[2] = vertices[2];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
      }
      
    }

    // Case 4: The three edges don't have any common vertex. Orientation: (dots are in the background)
    /*
     *            3 ----6------ 2
     *          /   \       .  /
     *         /      \ .     5
     *        /     .   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexType>
    static void apply3_4(CellType const & cell_in, 
                         DomainTypeOut & segment_out,
                         VertexType ** vertices
                        )
    {
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];

      //double diag01_len = viennagrid::norm(vertices[0]->getPoint() - vertices[1]->getPoint());
      //double diag12_len = viennagrid::norm(vertices[1]->getPoint() - vertices[2]->getPoint());
      //double diag23_len = viennagrid::norm(vertices[2]->getPoint() - vertices[3]->getPoint());

      // Strategy: The two longest edges of the common vertex are split 'twice',
      //           i.e. two new edges start from the center of the two longest edges
      //if (diag01_len > diag12_len) //split edge 01 again, introduce line 42
      if (stable_line_is_longer(vertices[0], vertices[1],
                                vertices[1], vertices[2])) //split edge 01 again, introduce line 42
      {
        cellvertices[0] = vertices[0];
        cellvertices[1] = vertices[4];
        cellvertices[2] = vertices[2];
        cellvertices[3] = vertices[6];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
      
        cellvertices[0] = vertices[0];
        cellvertices[1] = vertices[4];
        cellvertices[2] = vertices[6];
        cellvertices[3] = vertices[3];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
        
        cellvertices[0] = vertices[4];
        cellvertices[1] = vertices[5];
        cellvertices[2] = vertices[2];
        cellvertices[3] = vertices[6];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
          
        //if (diag12_len > diag23_len) //split edge 12 again, introduce line 53
        if (stable_line_is_longer(vertices[1], vertices[2],
                                  vertices[2], vertices[3])) //split edge 12 again, introduce line 53
        {
          //std::cout << "apply_3_4_1" << std::endl;
          cellvertices[0] = vertices[4];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[3];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[4];
          cellvertices[1] = vertices[5];
          cellvertices[2] = vertices[6];
          cellvertices[3] = vertices[3];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
        else //split edge 23 again, introduce line 61
        {
          //std::cout << "apply_3_4_2" << std::endl;
          cellvertices[0] = vertices[4];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[6];
          cellvertices[3] = vertices[3];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[4];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
      }
      else //split edge 12, introduce line 50
      {
        //if (diag12_len > diag23_len) //split edge 12 again, introduce line 53
        if (stable_line_is_longer(vertices[1], vertices[2],
                                  vertices[2], vertices[3])) //split edge 12 again, introduce line 53
        {
          //std::cout << "apply_3_4_3" << std::endl;
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[3];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[5];
          cellvertices[2] = vertices[6];
          cellvertices[3] = vertices[3];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[5];
          cellvertices[2] = vertices[2];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
            
          cellvertices[0] = vertices[4];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[3];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
        else //split edge 23 again, introduce line 61
        {
          //std::cout << "apply_3_4_4" << std::endl;
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[6];
          cellvertices[3] = vertices[3];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[5];
          cellvertices[2] = vertices[2];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[4];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[4];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[6];
          cellvertices[3] = vertices[3];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);          
        }
      }
      
    }



    //
    // Refinement of a tetrahedron, bisecting three edges
    //
    template <typename CellType, typename DomainTypeOut>
    static void apply3(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 1>::type             EdgeType;

      VertexType * vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];
      //std::cout << "apply3()" << std::endl;
      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[3] = &(segment_out.get_domain().vertex(vocit->id()));
      

      //
      // Step 2: Bring vertices in correct order
      //
      VertexType * ordered_vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements + 3];
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit; ++eocit;
      EdgeType const & e3 = *eocit; ++eocit;
      EdgeType const & e4 = *eocit; ++eocit;
      EdgeType const & e5 = *eocit;
      
      //with e0
      if (viennadata::access<refinement_key, bool>(refinement_key())(e0) == true)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e1) == true)
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          
          if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
            apply3_1(cell_in, segment_out, ordered_vertices);
          }
          else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
            apply3_2(cell_in, segment_out, ordered_vertices);
          }
          else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
            apply3_4(cell_in, segment_out, ordered_vertices);
          }
          else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
            apply3_3(cell_in, segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true)
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          
          if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
            apply3_3(cell_in, segment_out, ordered_vertices);
          }
          else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
            apply3_2(cell_in, segment_out, ordered_vertices);
          }
          else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
            apply3_4(cell_in, segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true)
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          
          if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
            apply3_1(cell_in, segment_out, ordered_vertices);
          }
          else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
            apply3_4(cell_in, segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true)
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          
          if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
            apply3_3(cell_in, segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else //no second edge
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      } 
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e1) == true)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true)
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          
          if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
            apply3_4(cell_in, segment_out, ordered_vertices);
          }
          else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
            apply3_3(cell_in, segment_out, ordered_vertices);
          }
          else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
            apply3_2(cell_in, segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true)
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          
          if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
            apply3_3(cell_in, segment_out, ordered_vertices);
          }
          else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
            apply3_1(cell_in, segment_out, ordered_vertices);
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
            //make edges 4 and 5 the references
            ordered_vertices[0] = vertices[1];
            ordered_vertices[1] = vertices[3];
            ordered_vertices[2] = vertices[2];
            ordered_vertices[3] = vertices[0];
            ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
            ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
            
            apply3_4(cell_in, segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else //no second edge
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true)
        {
          //NOTE: edges 2 and 3 don't have a common vertex, therefore 'base facet' is chosen depending on the third edge
          
          if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true)
          {
            // take edges 2 and 4 as reference
            ordered_vertices[0] = vertices[0];
            ordered_vertices[1] = vertices[3];
            ordered_vertices[2] = vertices[1];
            ordered_vertices[3] = vertices[2];
            ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
            ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
            
            apply3_4(cell_in, segment_out, ordered_vertices);
          }
          else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true)
          {
            // take edges 5 and 3 as reference
            ordered_vertices[0] = vertices[3];
            ordered_vertices[1] = vertices[2];
            ordered_vertices[2] = vertices[1];
            ordered_vertices[3] = vertices[0];
            ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
            ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
            
            apply3_3(cell_in, segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true)
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
            apply3_1(cell_in, segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else //no second edge
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == true)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == true)
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == true)
          {
            ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
            apply3_2(cell_in, segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else //no second edge
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else //no first edge found
      {
        assert(false && "Logic error: No edge for refinement found!"); 
      }
      
    } //apply3()




    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    //
    // Refinement of a tetrahedron, bisecting four edges
    //
    
    
    // Case 1: The two edges have a common vertex. Orientation: (dots are in the background)
    /*
     *            3 -----6----- 2
     *          /   \      .   /
     *         7      \ .     /
     *        /     4   5    /
     *       /  .         \ /
     *     0 ------------- 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexType>
    static void apply4_1(CellType const & cell_in, 
                         DomainTypeOut & segment_out,
                         VertexType ** vertices
                        )
    {
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];

      //double diag03_len = viennagrid::norm(vertices[0]->getPoint() - vertices[3]->getPoint());
      //double diag13_len = viennagrid::norm(vertices[1]->getPoint() - vertices[3]->getPoint());
      //double diag23_len = viennagrid::norm(vertices[2]->getPoint() - vertices[3]->getPoint());

      //if (diag03_len > diag13_len) //split edge 03, introduce line 71
      if (stable_line_is_longer(vertices[0], vertices[3],
                                vertices[1], vertices[3])) //split edge 03, introduce line 71
      {
        cellvertices[0] = vertices[0];
        cellvertices[1] = vertices[1];
        cellvertices[2] = vertices[4];
        cellvertices[3] = vertices[7];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
          
        cellvertices[0] = vertices[7];
        cellvertices[1] = vertices[5];
        cellvertices[2] = vertices[6];
        cellvertices[3] = vertices[3];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
        
        //if (diag13_len > diag23_len) //split edge 13, introduce line 52
        if (stable_line_is_longer(vertices[1], vertices[3],
                                  vertices[2], vertices[3])) //split edge 13, introduce line 52
        {
          //std::cout << "apply_4_1_1" << std::endl;
          cellvertices[0] = vertices[7];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[4];
          cellvertices[3] = vertices[5];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);

          cellvertices[0] = vertices[7];
          cellvertices[1] = vertices[5];
          cellvertices[2] = vertices[4];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[1];
          cellvertices[1] = vertices[2];
          cellvertices[2] = vertices[4];
          cellvertices[3] = vertices[5];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[4];
          cellvertices[1] = vertices[5];
          cellvertices[2] = vertices[2];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
        else //split edge 23, introduce line 61
        {
          //std::cout << "apply_4_1_2" << std::endl;
          cellvertices[0] = vertices[7];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[6];
          cellvertices[3] = vertices[5];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[7];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[4];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);

          cellvertices[0] = vertices[1];
          cellvertices[1] = vertices[2];
          cellvertices[2] = vertices[4];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
      }
      else //split edge 13, introduce line 50
      {
        cellvertices[0] = vertices[0];
        cellvertices[1] = vertices[1];
        cellvertices[2] = vertices[4];
        cellvertices[3] = vertices[5];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
          
        cellvertices[0] = vertices[0];
        cellvertices[1] = vertices[5];
        cellvertices[2] = vertices[4];
        cellvertices[3] = vertices[7];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);

        cellvertices[0] = vertices[7];
        cellvertices[1] = vertices[5];
        cellvertices[2] = vertices[6];
        cellvertices[3] = vertices[3];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
        
        cellvertices[0] = vertices[7];
        cellvertices[1] = vertices[5];
        cellvertices[2] = vertices[4];
        cellvertices[3] = vertices[6];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);

        //if (diag13_len > diag23_len) //split edge 13 again, introduce line 52
        if (stable_line_is_longer(vertices[1], vertices[3],
                                  vertices[2], vertices[3])) //split edge 13 again, introduce line 52
        {
          //std::cout << "apply_4_1_3" << std::endl;
          cellvertices[0] = vertices[1];
          cellvertices[1] = vertices[2];
          cellvertices[2] = vertices[4];
          cellvertices[3] = vertices[5];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[4];
          cellvertices[1] = vertices[5];
          cellvertices[2] = vertices[2];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
        else //split edge 23, introduce line 61
        {
          //std::cout << "apply_4_1_4" << std::endl;
          cellvertices[0] = vertices[5];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[4];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);

          cellvertices[0] = vertices[4];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[2];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
      }
      
    }
    
    // Case 2: The two edges don't have any common vertex. Orientation: (dots are in the background)
    /*
     *            3 ----------- 2
     *          /   \      .   /
     *         6      \ .     4
     *        /     5   7    /
     *       /  .         \ /
     *     0 ------------- 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexType>
    static void apply4_2(CellType const & cell_in, 
                         DomainTypeOut & segment_out,
                         VertexType ** vertices
                        )
    {
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];

      //double diag02_len = viennagrid::norm(vertices[0]->getPoint() - vertices[2]->getPoint());
      //double diag03_len = viennagrid::norm(vertices[0]->getPoint() - vertices[3]->getPoint());
      //double diag12_len = viennagrid::norm(vertices[1]->getPoint() - vertices[2]->getPoint());
      //double diag13_len = viennagrid::norm(vertices[1]->getPoint() - vertices[3]->getPoint());

      //if (diag03_len > diag13_len) //split edge 03, introduce line 61
      if (stable_line_is_longer(vertices[0], vertices[3],
                                vertices[1], vertices[3])) //split edge 03, introduce line 61
      {
        //std::cout << "split!" << std::endl;
        //if (diag13_len > diag12_len) //split edge 13, introduce line 72
        if (stable_line_is_longer(vertices[1], vertices[3],
                                  vertices[1], vertices[2])) //split edge 13, introduce line 72
        {
          //if (diag02_len > diag03_len) //split edge 02, introduce line 53
          if (stable_line_is_longer(vertices[0], vertices[2],
                                    vertices[0], vertices[3])) //split edge 02, introduce line 53
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(vertices[0], vertices[2],
                                      vertices[1], vertices[2])) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_1_" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[1];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[4];
              cellvertices[1] = vertices[2];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[7];
              cellvertices[1] = vertices[2];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
            else //split edge 12, introduce line 40
            {
              assert( false && "Logic error: diag02 < diag12 not possible here!");
            }
          }
          else //split edge 03, introduce line 62
          {
            //std::cout << "split!" << std::endl;
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(vertices[0], vertices[2],
                                      vertices[1], vertices[2])) //split edge 02, introduce line 51
            {
              //std::cout << "split!" << std::endl;
              //std::cout << "apply_4_2_2" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[1];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[1];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[6];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[7];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[6];
              cellvertices[3] = vertices[2];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[4];
              cellvertices[1] = vertices[2];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
              //std::cout << "done!" << std::endl;
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_3" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[2];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[2];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
          }
        }
        else //split edge 12, introduce line 43
        {
          //if (diag02_len > diag03_len) //split edge 02, introduce line 53
          if (stable_line_is_longer(vertices[0], vertices[2],
                                    vertices[0], vertices[3])) //split edge 02, introduce line 53
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(vertices[0], vertices[2],
                                      vertices[1], vertices[2])) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_4" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[1];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[5];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[5];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_5" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[5];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
          }
          else //split edge 03, introduce line 62
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(vertices[0], vertices[2],
                                      vertices[1], vertices[2])) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_6" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[1];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[1];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[6];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[7];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[6];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[4];
              cellvertices[1] = vertices[2];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_7" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[5];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
          }
        }
      }
      else //split edge 13, introduce line 70
      {
        //if (diag13_len > diag12_len) //split edge 13, introduce line 72
        if (stable_line_is_longer(vertices[1], vertices[3],
                                  vertices[1], vertices[2])) //split edge 13, introduce line 72
        {
          //if (diag02_len > diag03_len) //split edge 02, introduce line 53
          if (stable_line_is_longer(vertices[0], vertices[2],
                                    vertices[0], vertices[3])) //split edge 02, introduce line 53
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(vertices[0], vertices[2],
                                      vertices[1], vertices[2])) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_8" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[1];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[5];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[5];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_9" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[5];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[5];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
          }
          else //split edge 03, introduce line 62
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(vertices[0], vertices[2],
                                      vertices[1], vertices[2])) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_10" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[1];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[4];
              cellvertices[1] = vertices[2];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[7];
              cellvertices[1] = vertices[2];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[7];
              cellvertices[1] = vertices[2];
              cellvertices[2] = vertices[6];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_11" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[2];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[2];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
          }
        }
        else //split edge 12, introduce line 43
        {
          //if (diag02_len > diag03_len) //split edge 02, introduce line 53
          if (stable_line_is_longer(vertices[0], vertices[2],
                                    vertices[0], vertices[3])) //split edge 02, introduce line 53
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(vertices[0], vertices[2],
                                      vertices[1], vertices[2])) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_12" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[1];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[7];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[4];
              cellvertices[1] = vertices[2];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_13" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[7];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[4];
              cellvertices[1] = vertices[2];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
          }
          else //split edge 03, introduce line 62
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(vertices[0], vertices[2],
                                      vertices[1], vertices[2])) //split edge 02, introduce line 51
            {
              //we have diag12_len > diag13_len > diag03_len > diag02_len alreday, hence this case is bogus!
              assert( false && "Logic error: diag02 > diag12 not possible here!");
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_14" << std::endl;
              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[1];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[7];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[0];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[5];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[7];
              cellvertices[2] = vertices[4];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[6];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[3];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);

              cellvertices[0] = vertices[5];
              cellvertices[1] = vertices[4];
              cellvertices[2] = vertices[2];
              cellvertices[3] = vertices[6];
              new_cell.setVertices(cellvertices);
              segment_out.add(new_cell);
            }
          }
        }
      }
    }
    
    template <typename CellType, typename DomainTypeOut>
    static void apply4(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 1>::type             EdgeType;

      VertexType * vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];
      //std::cout << "apply4()" << std::endl;
      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[3] = &(segment_out.get_domain().vertex(vocit->id()));
      

      //
      // Step 2: Bring vertices in correct order, such that refined edge is on {0,1}-edge
      //
      VertexType * ordered_vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements + 4];
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit; ++eocit;
      EdgeType const & e3 = *eocit; ++eocit;
      EdgeType const & e4 = *eocit; ++eocit;
      EdgeType const & e5 = *eocit;
      
      //with e0
      if (viennadata::access<refinement_key, bool>(refinement_key())(e0) == false)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e1) == false)
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == false)
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == false) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == false) 
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == false) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          apply4_2(cell_in, segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e1) == false)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == false)
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == false) 
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == false) 
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          
          apply4_2(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == false) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == false)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == false) 
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          
          apply4_2(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == false) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == false) 
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == false)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == false) 
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
        }
        else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == false) 
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == false)
      {
        if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == false) 
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
          ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
          ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
          ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
          
          apply4_1(cell_in, segment_out, ordered_vertices);
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
    }    



    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    
    //
    // Refinement of a tetrahedron, bisecting five edges
    //
    
    // Case 1 (no other cases): Only edge 01 is not refined. Orientation: (dots are in the background)
    /*
     *            3 -----8----- 2
     *          /   \      .   /
     *         6      \ .     /
     *        /     5   7    4
     *       /  .         \ /
     *     0 ------------- 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexType>
    static void apply5_1(CellType const & cell_in, 
                         DomainTypeOut & segment_out,
                         VertexType ** vertices
                        )
    {
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];

      cellvertices[0] = vertices[6];
      cellvertices[1] = vertices[7];
      cellvertices[2] = vertices[8];
      cellvertices[3] = vertices[3];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);

      cellvertices[0] = vertices[5];
      cellvertices[1] = vertices[4];
      cellvertices[2] = vertices[2];
      cellvertices[3] = vertices[8];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
      
      //double diag02_len = viennagrid::norm(vertices[0]->getPoint() - vertices[2]->getPoint());
      //double diag03_len = viennagrid::norm(vertices[0]->getPoint() - vertices[3]->getPoint());
      //double diag12_len = viennagrid::norm(vertices[1]->getPoint() - vertices[2]->getPoint());
      //double diag13_len = viennagrid::norm(vertices[1]->getPoint() - vertices[3]->getPoint());

      //if (diag03_len > diag13_len) //split edge 03, introduce line 61
      if (stable_line_is_longer(vertices[0], vertices[3],
                                vertices[1], vertices[3])) //split edge 03, introduce line 61
      {
        cellvertices[0] = vertices[6];
        cellvertices[1] = vertices[4];
        cellvertices[2] = vertices[5];
        cellvertices[3] = vertices[8];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
        
        cellvertices[0] = vertices[6];
        cellvertices[1] = vertices[7];
        cellvertices[2] = vertices[4];
        cellvertices[3] = vertices[8];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
        
        cellvertices[0] = vertices[1];
        cellvertices[1] = vertices[4];
        cellvertices[2] = vertices[6];
        cellvertices[3] = vertices[7];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
        
        //if (diag02_len > diag12_len) //split edge 02, introduce line 51
        if (stable_line_is_longer(vertices[0], vertices[2],
                                  vertices[1], vertices[2])) //split edge 02, introduce line 51
        {
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[1];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
        else //split edge 12, introduce line 40
        {
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[4];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
      }
      else  //split edge 13, introduce line 70
      {
        //if (diag02_len > diag12_len) //split edge 02, introduce line 51
        if (stable_line_is_longer(vertices[0], vertices[2],
                                  vertices[1], vertices[2])) //split edge 02, introduce line 51
        {
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[7];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[7];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[1];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[7];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[7];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[8];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[6];
          cellvertices[1] = vertices[7];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[8];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
        else //split edge 12, introduce line 40
        {
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[1];
          cellvertices[2] = vertices[4];
          cellvertices[3] = vertices[7];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[7];
          cellvertices[2] = vertices[4];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[0];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[6];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[6];
          cellvertices[1] = vertices[4];
          cellvertices[2] = vertices[5];
          cellvertices[3] = vertices[8];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
          
          cellvertices[0] = vertices[6];
          cellvertices[1] = vertices[7];
          cellvertices[2] = vertices[4];
          cellvertices[3] = vertices[8];
          new_cell.setVertices(cellvertices);
          segment_out.add(new_cell);
        }
      }
    }
    
    template <typename CellType, typename DomainTypeOut>
    static void apply5(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 1>::type             EdgeType;

      VertexType * vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];
      //std::cout << "apply5()" << std::endl;
      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[3] = &(segment_out.get_domain().vertex(vocit->id()));
      

      //
      // Step 2: Bring vertices in correct order, such that refined edge is on {0,1}-edge
      //
      VertexType * ordered_vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements + 5];
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit; ++eocit;
      EdgeType const & e3 = *eocit; ++eocit;
      EdgeType const & e4 = *eocit; ++eocit;
      EdgeType const & e5 = *eocit;
      
      if (viennadata::access<refinement_key, bool>(refinement_key())(e0) == false)
      {
        ordered_vertices[0] = vertices[0];
        ordered_vertices[1] = vertices[1];
        ordered_vertices[2] = vertices[2];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
        ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
        ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
        ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
        ordered_vertices[8] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
        
        apply5_1(cell_in, segment_out, ordered_vertices);
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e1) == false)
      {
        ordered_vertices[0] = vertices[2];
        ordered_vertices[1] = vertices[0];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
        ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
        ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
        ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
        ordered_vertices[8] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
        
        apply5_1(cell_in, segment_out, ordered_vertices);
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e2) == false)
      {
        ordered_vertices[0] = vertices[0];
        ordered_vertices[1] = vertices[3];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[2];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
        ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
        ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
        ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
        ordered_vertices[8] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
        
        apply5_1(cell_in, segment_out, ordered_vertices);
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e3) == false)
      {
        ordered_vertices[0] = vertices[1];
        ordered_vertices[1] = vertices[2];
        ordered_vertices[2] = vertices[0];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
        ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
        ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
        ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
        ordered_vertices[8] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
        
        apply5_1(cell_in, segment_out, ordered_vertices);
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e4) == false)
      {
        ordered_vertices[0] = vertices[1];
        ordered_vertices[1] = vertices[3];
        ordered_vertices[2] = vertices[2];
        ordered_vertices[3] = vertices[0];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e5)));
        ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
        ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
        ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
        ordered_vertices[8] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
        
        apply5_1(cell_in, segment_out, ordered_vertices);
      }
      else if (viennadata::access<refinement_key, bool>(refinement_key())(e5) == false)
      {
        ordered_vertices[0] = vertices[3];
        ordered_vertices[1] = vertices[2];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[0];
        ordered_vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e3)));
        ordered_vertices[5] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e4)));
        ordered_vertices[6] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
        ordered_vertices[7] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
        ordered_vertices[8] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
        
        apply5_1(cell_in, segment_out, ordered_vertices);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!"); 
      }
    }    




    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    //
    // Refinement of a tetrahedron, bisecting six edges: Split the cell into 8 tets
    //
    template <typename CellType, typename DomainTypeOut>
    static void apply6(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;

      VertexType * vertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements
                            + topology::subcell_desc<tetrahedron_tag, 1>::num_elements];
      //std::cout << "apply6()" << std::endl;
      
      //
      // Step 1: Get vertices on the new domain
      //
      
      //grab existing vertices:
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[3] = &(segment_out.get_domain().vertex(vocit->id()));

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
      VertexType * cellvertices[topology::subcell_desc<tetrahedron_tag, 0>::num_elements];
      
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
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type                 EdgeOnCellIterator;            
      
      //std::cout << "tetrahedron_tag::apply()" << std::endl;
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