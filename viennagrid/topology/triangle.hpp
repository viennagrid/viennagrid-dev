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
#include "viennagrid/detail/element_iterators.hpp"
#include "viennagrid/algorithm/norm.hpp"

namespace viennagrid
{

  struct triangle_tag
  {
    enum{ topology_level = 2 };
    static std::string name() { return "Triangle"; }
  };

  
  namespace topology
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
    
    /*
     * No refinement. Just put same cell into new domain.
     */
    template <typename CellType, typename DomainTypeOut>
    static void apply0(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 0>::type            VertexOnCellContainer;
      typedef typename viennagrid::result_of::iterator<VertexOnCellContainer>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type            EdgeOnCellContainer;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellContainer>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::ncell_type<ConfigTypeIn, 0>::type             VertexType;

      VertexType * vertices[topology::subcell_desc<triangle_tag, 0>::num_elements];
      
      //
      // Step 1: Get vertices on the new domain
      //
      
      //grab existing vertices:
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->id()));

      //
      // Step 2: Add new cells to new domain:
      //
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<triangle_tag, 0>::num_elements];
      
      //0-3-2:
      cellvertices[0] = vertices[0];
      cellvertices[1] = vertices[1];
      cellvertices[2] = vertices[2];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);

    } //apply0()
    
    
    /* Refinement for one edge to be bisected. Orientation of vertices (established by rotating the triangle appropriately)
    *
    *           2
    *         /   \ 
    *        /     \ 
    *       0 - 3 - 1
    */
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

      VertexType * vertices[topology::subcell_desc<triangle_tag, 0>::num_elements + 1];
      
      //
      // Step 1: Get vertices on the new domain
      //
      
      //grab existing vertices:
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->id()));

      //add vertices from edge
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
      std::size_t offset = 0;
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit;
      
      if ( (viennadata::access<refinement_key, bool>(refinement_key())(e0) == true) )
      {
        vertices[3] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
        offset = 0;
      }
      else if ( (viennadata::access<refinement_key, bool>(refinement_key())(e1) == true) )
      {
        vertices[3] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
        offset = 2;
      }
      else if ( (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true) )
      {
        vertices[3] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
        offset = 1;
      }
      else
      {
        assert( (2 < 2) && "Logic error: Triangle does not have an edges for bisection!");
      }
      
      //
      // Step 2: Add new cells to new domain:
      //
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<triangle_tag, 0>::num_elements];
      
      //0-3-2:
      cellvertices[0] = vertices[(offset + 0) % topology::subcell_desc<triangle_tag, 0>::num_elements];
      cellvertices[1] = vertices[3];
      cellvertices[2] = vertices[(offset + 2) % topology::subcell_desc<triangle_tag, 0>::num_elements];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);

      //3-1-2:
      cellvertices[0] = vertices[3];
      cellvertices[1] = vertices[(offset + 1) % topology::subcell_desc<triangle_tag, 0>::num_elements];
      cellvertices[2] = vertices[(offset + 2) % topology::subcell_desc<triangle_tag, 0>::num_elements];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);
    } //apply1()
    

    /* Refinement for one edge to be bisected. Orientation of vertices:  (established by rotating the triangle appropriately)
     *
     *            2
     *          /   \
     *         /     4
     *        /       \
     *       0 -- 3 -- 1
    */
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

      VertexType * vertices[topology::subcell_desc<triangle_tag, 0>::num_elements + 2];
      
      //
      // Step 1: Get vertices on the new domain
      //
      
      //grab existing vertices:
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->id()));

      //Find rotation offset such that first two edges are to be refined
      EdgeOnCellContainer edges_on_cell = viennagrid::ncells<1>(cell_in);
      std::size_t offset = 0;
      
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit;
      
      if ( (viennadata::access<refinement_key, bool>(refinement_key())(e0) == true)
           && (viennadata::access<refinement_key, bool>(refinement_key())(e1) == true) )
      {
        vertices[3] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
        vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
        offset = 2;
      }
      else if ( (viennadata::access<refinement_key, bool>(refinement_key())(e0) == true)
           && (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true) )
      {
        vertices[3] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e0)));
        vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
        offset = 0;
      }
      else if ( (viennadata::access<refinement_key, bool>(refinement_key())(e1) == true)
           && (viennadata::access<refinement_key, bool>(refinement_key())(e2) == true) )
      {
        vertices[3] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e2)));
        vertices[4] = &(segment_out.get_domain().vertex(viennadata::access<refinement_key, std::size_t>(refinement_key())(e1)));
        offset = 1;
      }
      else
      {
        assert( (2 < 2) && "Logic error: Triangle does not have two edges for bisection!");
      }
      
      //
      // Step 2: Add new cells to new domain:
      //
      CellType new_cell;
      VertexType * cellvertices[topology::subcell_desc<triangle_tag, 0>::num_elements];
      
      //3-1-4:
      cellvertices[0] = vertices[3];
      cellvertices[1] = vertices[(offset + 1) % topology::subcell_desc<triangle_tag, 0>::num_elements];
      cellvertices[2] = vertices[4];
      new_cell.setVertices(cellvertices);
      segment_out.add(new_cell);

      //split second-longest edge
      VertexType & v0 = *(vertices[(offset + 0) % topology::subcell_desc<triangle_tag, 0>::num_elements]);
      VertexType & v1 = *(vertices[(offset + 1) % topology::subcell_desc<triangle_tag, 0>::num_elements]);
      VertexType & v2 = *(vertices[(offset + 2) % topology::subcell_desc<triangle_tag, 0>::num_elements]);
      double len_edge1 = viennagrid::norm(v1.getPoint() - v0.getPoint());
      double len_edge2 = viennagrid::norm(v2.getPoint() - v1.getPoint());
      
      if (len_edge1 > len_edge2) //split edge [v0, v1] again
      {
        //0-3-2:
        cellvertices[0] = vertices[(offset + 0) % topology::subcell_desc<triangle_tag, 0>::num_elements];
        cellvertices[1] = vertices[3];
        cellvertices[2] = vertices[(offset + 2) % topology::subcell_desc<triangle_tag, 0>::num_elements];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);

        //2-3-4:
        cellvertices[0] = vertices[(offset + 2) % topology::subcell_desc<triangle_tag, 0>::num_elements];
        cellvertices[1] = vertices[3];
        cellvertices[2] = vertices[4];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
      }
      else //split edge [v1, v2]
      {
        //0-3-4:
        cellvertices[0] = vertices[(offset + 0) % topology::subcell_desc<triangle_tag, 0>::num_elements];
        cellvertices[1] = vertices[3];
        cellvertices[2] = vertices[4];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);

        //0-4-2:
        cellvertices[0] = vertices[(offset + 0) % topology::subcell_desc<triangle_tag, 0>::num_elements];
        cellvertices[1] = vertices[4];
        cellvertices[2] = vertices[(offset + 2) % topology::subcell_desc<triangle_tag, 0>::num_elements];
        new_cell.setVertices(cellvertices);
        segment_out.add(new_cell);
      }
      
      
    } //apply2()
    


    
    //
    // Refinement for three refined edges
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

      VertexType * vertices[topology::subcell_desc<triangle_tag, 0>::num_elements
                            + topology::subcell_desc<triangle_tag, 1>::num_elements];
      
      //
      // Step 1: Get vertices on the new domain
      //
      
      //grab existing vertices:
      VertexOnCellContainer vertices_on_cell = viennagrid::ncells<0>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[1] = &(segment_out.get_domain().vertex(vocit->id())); ++vocit;
      vertices[2] = &(segment_out.get_domain().vertex(vocit->id()));

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
      VertexType * cellvertices[topology::subcell_desc<triangle_tag, 0>::num_elements];
      
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
      
    } //apply3()


    template <typename CellType, typename DomainTypeOut>
    static void apply(CellType const & cell_in, DomainTypeOut & segment_out)
    {
      typedef typename viennagrid::result_of::const_ncell_range<CellType, 1>::type            EdgeOnCellContainer;
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
        default: //nothing to do...
                break;
      }
    } //apply()
  };
  
  
  
  
  
  
    
}

#endif