#ifndef VIENNAGRID_REFINEMENT_TETRAHEDRON_HPP
#define VIENNAGRID_REFINEMENT_TETRAHEDRON_HPP

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
#include "viennagrid/topology/vertex.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/topology/simplex.hpp"

/** @file refinement/tetrahedron.hpp
    @brief Provides refinement routines for tetrahedra
*/

namespace viennagrid
{

  
  /** @brief Compares the lines (v1_1, v1_2) and (v2_1, v2_2) and returns true if the first is longer. 
   * 
   * Ensures that the result is the same no matter in which order the parameters are passed. 
   * If the two lines have equal length, the line with the larger vertex IDs involved is considered as longer.
   */
  template <typename GeometricContainerType, typename VertexHandleType>
  bool stable_line_is_longer(GeometricContainerType const & domain,
                             VertexHandleType vh1_1, VertexHandleType vh1_2,  
                             VertexHandleType vh2_1, VertexHandleType vh2_2)
  {
      typedef typename viennagrid::storage::handle::result_of::value_type< VertexHandleType >::type VertexType;
      typedef typename viennagrid::result_of::point_type< GeometricContainerType >::type PointType;
      typedef typename viennagrid::result_of::coord_type< PointType >::type ScalarType;
      
      const VertexType & v1_1 = viennagrid::dereference_handle( domain, vh1_1 );
      const VertexType & v1_2 = viennagrid::dereference_handle( domain, vh1_2 );
      const VertexType & v2_1 = viennagrid::dereference_handle( domain, vh2_1 );
      const VertexType & v2_2 = viennagrid::dereference_handle( domain, vh2_2 );
      
      
    //typedef typename VertexType::config_type::numeric_type       ScalarType;
    
    const VertexType & v1_1_ptr = (v1_1.id() < v1_2.id()) ? v1_1 : v1_2; //v1_1 carries smaller ID
    const VertexType & v1_2_ptr = (v1_1.id() < v1_2.id()) ? v1_2 : v1_1; //v1_2 carries larger ID
    
    const VertexType & v2_1_ptr = (v2_1.id() < v2_2.id()) ? v2_1 : v2_2; //v2_1 carries smaller ID
    const VertexType & v2_2_ptr = (v2_1.id() < v2_2.id()) ? v2_2 : v2_1; //v2_2 carries larger ID
    
    ScalarType line1 = viennagrid::norm( viennagrid::point(domain, v1_1) - viennagrid::point(domain, v1_2) );
    ScalarType line2 = viennagrid::norm( viennagrid::point(domain, v2_1) - viennagrid::point(domain, v2_2) );

    
    if (line1 > line2)
    {
      return true;
    }
    else if (line1 == line2)
    {
      //compare IDs:
      if (v1_1_ptr.id() > v2_1_ptr.id())
      {
        return true;
      }
      else if (v1_1_ptr.id() == v2_1_ptr.id())
      {
        if (v1_2_ptr.id() > v2_2_ptr.id())
          return true;
        else if (v1_2_ptr.id() == v2_2_ptr.id())
          return false; //identical lines are compared!
      }
    }

    return false;
  }
  
  template<typename GeometricContainerType, typename VertexHandleContainer>
  bool stable_line_is_longer(GeometricContainerType const & domain, VertexHandleContainer vertices, unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3)
  {
      return stable_line_is_longer(domain,
                                *viennagrid::advance(vertices.begin(), i0), *viennagrid::advance(vertices.begin(), i1),
                                *viennagrid::advance(vertices.begin(), i2), *viennagrid::advance(vertices.begin(), i3));
  }
  
  
  template<typename CellType, typename GeometricDomainType, typename VertexHandleContainer>
  void create_refinement_cell(GeometricDomainType & domain, VertexHandleContainer vertex_handle_container, unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3)
  {
      typedef typename VertexHandleContainer::iterator VertexHandleIteratorType;
      typedef typename std::iterator_traits<VertexHandleIteratorType>::value_type VertexHandleType;
      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num > cellvertices;
      
      cellvertices[0] = *viennagrid::advance(vertex_handle_container.begin(), i0);
      cellvertices[1] = *viennagrid::advance(vertex_handle_container.begin(), i1);
      cellvertices[2] = *viennagrid::advance(vertex_handle_container.begin(), i2);
      cellvertices[3] = *viennagrid::advance(vertex_handle_container.begin(), i3);
      
      viennagrid::create_element<CellType>( domain, cellvertices.begin(), cellvertices.end() );
  }
  
  
  
  
  /** @brief Class specialization for the refinement of a tetrahedron: A LOT of spaghetti-code to follow. 
   * 
   * 
   * Unfortunately, combinatorics (or a lack of talent?) forces us to consider that many cases of refinement.
   */
  template <>
  struct element_refinement<tetrahedron_tag>
  {

    /** @brief Not refining a tetrahedron at all */
    template <typename CellType, typename DomainTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply0(CellType const & cell_in, DomainTypeOut & segment_out,
                       EdgeRefinementFlagAccessor const edge_refinement_flag_accessor, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const edge_to_vertex_handle_accessor)
    {
//       std::cout << "tetrahedron::apply0()" << std::endl;
      //typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type             VertexType;
      typedef typename viennagrid::result_of::handle<DomainTypeOut, vertex_tag>::type             VertexHandleType;
      
      typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
      typedef typename VertexTypeOut::id_type VertexIDTypeOut;
      
      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num > vertex_handles;
      
      // Step 1: grab existing vertices:
      VertexOnCellRange vertices_on_cell = viennagrid::elements<vertex_tag>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();

      
      vertex_handles[0] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertex_handles[1] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertex_handles[2] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertex_handles[3] = vertex_to_vertex_handle_accessor(*vocit);

      // Step 2: Add new cells to new domain:
      viennagrid::create_element<CellType>( segment_out, vertex_handles.begin(), vertex_handles.end() );

    }    

    /** @brief Refinement of a tetrahedron, bisecting one edge */
    template <typename CellType, typename DomainTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply1(CellType const & cell_in, DomainTypeOut & segment_out,
                       EdgeRefinementFlagAccessor const edge_refinement_flag_accessor, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const edge_to_vertex_handle_accessor)
    {
//       std::cout << "tetrahedron::apply1()" << std::endl;
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::element<CellType, line_tag>::type             EdgeType;
      
      typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type             VertexType;
      typedef typename viennagrid::result_of::handle<DomainTypeOut, vertex_tag>::type             VertexHandleType;
      
      typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
      typedef typename VertexTypeOut::id_type VertexIDTypeOut;


      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num > vertices;

      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellRange vertices_on_cell = viennagrid::elements(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();

      
      vertices[0] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[1] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[2] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[3] = vertex_to_vertex_handle_accessor(*vocit);
      
      //
      // Step 2: Bring vertices in correct order, such that refined edge is on {0,1}-edge
      //
      //VertexType * ordered_vertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num + 1];
      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num + 1 > ordered_vertices;
      
      EdgeOnCellRange edges_on_cell = viennagrid::elements(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit; ++eocit;
      EdgeType const & e3 = *eocit; ++eocit;
      EdgeType const & e4 = *eocit; ++eocit;
      EdgeType const & e5 = *eocit;
      
      if (edge_refinement_flag_accessor(e0) == true)
      {
        ordered_vertices[0] = vertices[0];
        ordered_vertices[1] = vertices[1];
        ordered_vertices[2] = vertices[2];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e0);
        
        //[)]);
      }
      else if (edge_refinement_flag_accessor(e1) == true)
      {
        ordered_vertices[0] = vertices[2];
        ordered_vertices[1] = vertices[0];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e1);
      }
      else if (edge_refinement_flag_accessor(e2) == true)
      {
        ordered_vertices[0] = vertices[0];
        ordered_vertices[1] = vertices[3];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[2];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e2);
      }
      else if (edge_refinement_flag_accessor(e3) == true)
      {
        ordered_vertices[0] = vertices[1];
        ordered_vertices[1] = vertices[2];
        ordered_vertices[2] = vertices[0];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e3);
      }
      else if (edge_refinement_flag_accessor(e4) == true)
      {
        ordered_vertices[0] = vertices[3];
        ordered_vertices[1] = vertices[1];
        ordered_vertices[2] = vertices[0];
        ordered_vertices[3] = vertices[2];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e4);
      }
      else if (edge_refinement_flag_accessor(e5) == true)
      {
        ordered_vertices[0] = vertices[3];
        ordered_vertices[1] = vertices[2];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[0];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e5);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!"); 
      }

      //
      // Step 3: Write new cells to domain_out
      //
      
      //cell containing vertex 0:      
      create_refinement_cell<CellType>( segment_out, ordered_vertices, 0, 4, 2, 3);
      
      //cell without vertex 0:
      create_refinement_cell<CellType>( segment_out, ordered_vertices, 4, 1, 2, 3);
    }    


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //
    // Refinement of a tetrahedron, bisecting two edges
    //
    
    /** @brief Refinement of a tetrahedron, bisecting two edges. Case 1: The two edges have a common vertex.
     *
     * Orientation: (dots are in the background)
     *
     *            3 ----------- 2
     *          /   \      .   /
     *         /      \ .     5
     *        /     .   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexHandleIteratorType>
    static void apply2_1(DomainTypeOut & segment_out,
                         VertexHandleIteratorType vertices
                        )
    {
      create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 5, 3);

      if (stable_line_is_longer(segment_out, vertices, 0, 1, 2, 1))
      {
            create_refinement_cell<CellType>( segment_out, vertices, 4, 5, 2, 3);
      }
      else //split edge 12, introduce line 05
      {
            create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 3);
            create_refinement_cell<CellType>( segment_out, vertices, 0, 5, 2, 3);
      }
      
    }

    /** @brief Refinement of a tetrahedron, bisecting two edges.  Case 2: The two edges don't have any common vertex.
     *
     *
     * Orientation: (dots are in the background)
     *
     *            3 -----5----- 2
     *          /   \      .   /
     *         /      \ .     /
     *        /     .   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexHandleIteratorType>
    static void apply2_2(DomainTypeOut & segment_out,
                         VertexHandleIteratorType vertices
                        )
    {
        create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 2, 5);
        create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 5, 3);
        create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 2, 5);
        create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 3);
    }

    /** @brief Entry function for the refinement of a tetrahedron by bisection of two edges. Reorders the tetrahedron to reduce complexity.
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply2(CellType const & cell_in, DomainTypeOut & segment_out,
                       EdgeRefinementFlagAccessor const edge_refinement_flag_accessor, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const edge_to_vertex_handle_accessor)
    {
//       std::cout << "tetrahedron::apply2()" << std::endl;
      //typedef typename CellType::config_type        ConfigTypeIn;
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::element<CellType, vertex_tag>::type             VertexType;
      typedef typename viennagrid::result_of::handle<CellType, vertex_tag>::type             VertexHandleType;
      typedef typename viennagrid::result_of::element<CellType, line_tag>::type             EdgeType;
      
      typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
      typedef typename VertexTypeOut::id_type VertexIDTypeOut;


      
      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num > vertices;
      
      //std::cout << "apply2()" << std::endl;
      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellRange vertices_on_cell = viennagrid::elements<vertex_tag>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[1] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[2] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[3] = vertex_to_vertex_handle_accessor(*vocit);      

      //
      // Step 2: Bring vertices in correct order, such that refined edge is on {0,1}-edge
      //
      //VertexType * ordered_vertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num + 2];
      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num + 2 > ordered_vertices;
      EdgeOnCellRange edges_on_cell = viennagrid::elements<line_tag>(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit; ++eocit;
      EdgeType const & e3 = *eocit; ++eocit;
      EdgeType const & e4 = *eocit; ++eocit;
      EdgeType const & e5 = *eocit;
      
      //with e0
      if (edge_refinement_flag_accessor(e0) == true)
      {
        if (edge_refinement_flag_accessor(e1) == true)
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e0);
          
          apply2_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e2) == true)
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e2);
          
          apply2_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e3) == true) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e3);
          
          apply2_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e4) == true) 
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e4);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e0);
          
          apply2_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e5) == true) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e5);
          
          apply2_2<CellType>(segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (edge_refinement_flag_accessor(e1) == true)
      {
        if (edge_refinement_flag_accessor(e2) == true)
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e1);
          
          apply2_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e3) == true) 
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e1);

          
          apply2_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e4) == true) 
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e4);
          
          apply2_2<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e5) == true) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e5);
          
          apply2_1<CellType>(segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (edge_refinement_flag_accessor(e2) == true)
      {
        if (edge_refinement_flag_accessor(e3) == true) 
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e3);
          
          apply2_2<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e4) == true) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e4);
          
          apply2_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e5) == true) 
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e5);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e2);
          
          apply2_1<CellType>(segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (edge_refinement_flag_accessor(e3) == true)
      {
        if (edge_refinement_flag_accessor(e4) == true) 
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e4);
          
          apply2_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e5) == true) 
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e5);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e3);
          
          apply2_1<CellType>(segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (edge_refinement_flag_accessor(e4) == true)
      {
        if (edge_refinement_flag_accessor(e5) == true) 
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e4);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e5);
          
          apply2_1<CellType>(segment_out, ordered_vertices);
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


    /** @brief Refinement of a tetrahedron, bisecting three edges.  Case 1: The three edges have a common vertex. 
     * 
     * Orientation: (dots are in the background)
     *
     *            3 ----------- 2
     *          /   \       .  /
     *         /      6 .     5
     *        /     .   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexHandleIteratorType>
    static void apply3_1(DomainTypeOut & segment_out,
                         VertexHandleIteratorType vertices
                        )
    {
      create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 5, 6);
      
      //double diag01_len = viennagrid::norm(vertices[0]->point() - vertices[1]->point());
      //double diag12_len = viennagrid::norm(vertices[2]->point() - vertices[1]->point());
      //double diag13_len = viennagrid::norm(vertices[3]->point() - vertices[1]->point());

      // Strategy: The two longest edges of the common vertex are split 'twice',
      //           i.e. two new edges start from the center of the two longest edges
      //if (diag01_len > diag12_len) //split edge 01 again, introduce line 42
      if (stable_line_is_longer(segment_out, vertices, 0, 1, 1, 2))
      {
        /*std::cout << "Norm " << vertices[0]->id() << vertices[1]->id() << ": " 
                             << viennagrid::norm(vertices[0]->point() - vertices[1]->point()) << std::endl;
        std::cout << "Norm " << vertices[2]->id() << vertices[1]->id() << ": " 
                             << viennagrid::norm(vertices[2]->point() - vertices[1]->point()) << std::endl;
        std::cout << "Splitting " << vertices[0]->id() << vertices[1]->id() << std::endl;*/
        //if (diag13_len > diag12_len) //split edge 13 again, introduce line 62
        if (stable_line_is_longer(segment_out, vertices, 1, 3, 1, 2))
        {
          /*std::cout << "Norm " << vertices[1]->id() << vertices[3]->id() << ": " 
                                << viennagrid::norm(vertices[1]->point() - vertices[3]->point()) << std::endl;
          std::cout << "Norm " << vertices[2]->id() << vertices[1]->id() << ": " 
                                << viennagrid::norm(vertices[2]->point() - vertices[1]->point()) << std::endl;
          std::cout << "Splitting " << vertices[1]->id() << vertices[3]->id() << std::endl; */
          
          if (stable_line_is_longer(segment_out, vertices, 1, 3, 0, 1))
          {
            create_refinement_cell<CellType>( segment_out, vertices, 0, 6, 2, 3);
            create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 2, 6);
            create_refinement_cell<CellType>( segment_out, vertices, 4, 5, 2, 6);
          }
          else  //split edge 01 again, introduce line 43
          {
            //std::cout << "apply_3_1_1" << std::endl;

            create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 2, 3);
            create_refinement_cell<CellType>( segment_out, vertices, 3, 4, 2, 6);
            create_refinement_cell<CellType>( segment_out, vertices, 4, 5, 2, 6);
          }
        }
        else //split edge 12 again, introduce lines 43 and 53
        {
          if (stable_line_is_longer(segment_out, vertices, 1, 3, 0, 1))
          {
            assert(false && "diag13_len > diag01_len impossible!");
          }
          else  //split edge 01 again, introduce line 43
          {
            //std::cout << "apply_3_1_2" << std::endl;
            create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 2, 3);
            create_refinement_cell<CellType>( segment_out, vertices, 4, 5, 2, 3);
            create_refinement_cell<CellType>( segment_out, vertices, 4, 6, 5, 3);
          }
        }
      }
      else //split edge 12, introduce line 05
      {
        if (stable_line_is_longer(segment_out, vertices, 1, 3, 1, 2)) //split edge 13 again, introduce line 62
        {
          if (stable_line_is_longer(segment_out, vertices, 1, 3, 0, 1)) //split edge 13 again, introduce line 60
          {
            //std::cout << "apply_3_1_3" << std::endl;
            create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
            create_refinement_cell<CellType>( segment_out, vertices, 0, 6, 5, 2);
            create_refinement_cell<CellType>( segment_out, vertices, 0, 6, 2, 3);
          }
          else  //split edge 01 again, introduce line 43
          {
            assert(false && "diag13_len > diag01_len impossible!");
          }
        }
        else //split edge 12 again, introduce line 53
        {
          if (stable_line_is_longer(segment_out, vertices, 1, 3, 0, 1)) //split edge 13 again, introduce line 60
          {
            create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
            create_refinement_cell<CellType>( segment_out, vertices, 0, 6, 5, 3);
            create_refinement_cell<CellType>( segment_out, vertices, 0, 5, 2, 3);
          }
          else  //split edge 01 again, introduce line 43
          {
            //std::cout << "apply_3_1_4" << std::endl;
            create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 3);
            create_refinement_cell<CellType>( segment_out, vertices, 4, 5, 3, 6);
            create_refinement_cell<CellType>( segment_out, vertices, 0, 5, 2, 3);
          }
        }
      }
      
    }


    /** @brief Refinement of a tetrahedron, bisecting three edges.  Case 2: The three edges don't have any common vertex.
     *
     *
     * Orientation: (dots are in the background)
     *
     *            3 ----------- 2
     *          /   \       .  /
     *         /      \ .     5
     *        /     6   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexHandleIteratorType>
    static void apply3_2(DomainTypeOut & segment_out,
                         VertexHandleIteratorType vertices
                        )
    {
      //std::cout << "apply_3_2" << std::endl;
      create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 6, 3);
      create_refinement_cell<CellType>( segment_out, vertices, 4, 5, 6, 3);
      create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 5, 3);
      create_refinement_cell<CellType>( segment_out, vertices, 6, 5, 2, 3);
    }


    /** @brief Refinement of a tetrahedron, bisecting three edges. Case 3: The three edges don't have any common vertex.
     *
     * Orientation: (dots are in the background)
     *
     *            3 ----------- 2
     *          /   \       .  /
     *         6      \ .     5
     *        /     .   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexHandleIteratorType>
    static void apply3_3(DomainTypeOut & segment_out,
                         VertexHandleIteratorType vertices
                        )
    {
      //std::cout << "Found!" << std::endl;

      //double diag01_len = viennagrid::norm(vertices[0]->point() - vertices[1]->point());
      //double diag12_len = viennagrid::norm(vertices[1]->point() - vertices[2]->point());
      //double diag03_len = viennagrid::norm(vertices[0]->point() - vertices[3]->point());

      // Strategy: The two longest edges of the common vertex are split 'twice',
      //           i.e. two new edges start from the center of the two longest edges
      //if (diag01_len > diag03_len) //split edge 01 again, introduce line 43
      if (stable_line_is_longer(segment_out, vertices, 0, 1, 0, 3)) //split edge 01 again, introduce line 43
      {
        create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 5, 3);
          
        //if (diag01_len > diag12_len) //split edge 01 again, introduce line 42
        if (stable_line_is_longer(segment_out, vertices, 0, 1, 1, 2)) //split edge 01 again, introduce line 42
        {
          //std::cout << "apply_3_3_1" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 2, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 6, 4, 2, 3);
          create_refinement_cell<CellType>( segment_out, vertices, 4, 5, 2, 3);
        }
        else //split edge 12 again, introduce line 50
        {
          //std::cout << "apply_3_3_2" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 0, 5, 2, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 6, 4, 5, 3);
          create_refinement_cell<CellType>( segment_out, vertices, 6, 5, 2, 3);
        }
      }
      else  //split edge 03 again, introduce line 61
      {
        create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 5, 6);
        create_refinement_cell<CellType>( segment_out, vertices, 6, 1, 5, 3);
        create_refinement_cell<CellType>( segment_out, vertices, 6, 5, 2, 3);
          
        //if (diag01_len > diag12_len) //split edge 01 again, introduce line 42
        if (stable_line_is_longer(segment_out, vertices, 0, 1, 1, 2)) //split edge 01 again, introduce line 42
        {
          //std::cout << "apply_3_3_3" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 2, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 6, 4, 5, 2);
        }
        else //split edge 12 again, introduce line 50
        {
          //std::cout << "apply_3_3_4" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 0, 5, 2, 6);
        }
      }
      
    }

    /** @brief Refinement of a tetrahedron, bisecting three edges. Case 4: The three edges don't have any common vertex.
     *
     * Orientation: (dots are in the background)
     *
     *            3 ----6------ 2
     *          /   \       .  /
     *         /      \ .     5
     *        /     .   \    /
     *       /  .         \ /
     *     0 ----- 4 ------ 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexHandleIteratorType>
    static void apply3_4(DomainTypeOut & segment_out,
                         VertexHandleIteratorType vertices
                        )
    {
//       CellType new_cell;
//       VertexType * cellvertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num];

      //double diag01_len = viennagrid::norm(vertices[0]->point() - vertices[1]->point());
      //double diag12_len = viennagrid::norm(vertices[1]->point() - vertices[2]->point());
      //double diag23_len = viennagrid::norm(vertices[2]->point() - vertices[3]->point());

      // Strategy: The two longest edges of the common vertex are split 'twice',
      //           i.e. two new edges start from the center of the two longest edges
      //if (diag01_len > diag12_len) //split edge 01 again, introduce line 42
      if (stable_line_is_longer(segment_out, vertices, 0, 1, 1, 2)) //split edge 01 again, introduce line 42
      {
        create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 2, 6);
        create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 6, 3);
        create_refinement_cell<CellType>( segment_out, vertices, 4, 5, 2, 6);
          
        //if (diag12_len > diag23_len) //split edge 12 again, introduce line 53
        if (stable_line_is_longer(segment_out, vertices, 1, 2, 2, 3)) //split edge 12 again, introduce line 53
        {
          //std::cout << "apply_3_4_1" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 5, 3);
          create_refinement_cell<CellType>( segment_out, vertices, 4, 5, 6, 3);
        }
        else //split edge 23 again, introduce line 61
        {
          //std::cout << "apply_3_4_2" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 6, 3);
          create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 5, 6);
        }
      }
      else //split edge 12, introduce line 50
      {
        //if (diag12_len > diag23_len) //split edge 12 again, introduce line 53
        if (stable_line_is_longer(segment_out, vertices, 1, 2, 2, 3)) //split edge 12 again, introduce line 53
        {
          //std::cout << "apply_3_4_3" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 3);
          create_refinement_cell<CellType>( segment_out, vertices, 0, 5, 6, 3);
          create_refinement_cell<CellType>( segment_out, vertices, 0, 5, 2, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 5, 3);
        }
        else //split edge 23 again, introduce line 61
        {
          //std::cout << "apply_3_4_4" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 6, 3);
          create_refinement_cell<CellType>( segment_out, vertices, 0, 5, 2, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 5, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 6, 3);
        }
      }
      
    }



    /** @brief Entry function for refinement of a tetrahedron with three edges for bisection. */
    template <typename CellType, typename DomainTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply3(CellType const & cell_in, DomainTypeOut & segment_out,
                       EdgeRefinementFlagAccessor const edge_refinement_flag_accessor, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const edge_to_vertex_handle_accessor)
    {
//       std::cout << "tetrahedron::apply3()" << std:4endl;
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::element<CellType, vertex_tag>::type             VertexType;
      typedef typename viennagrid::result_of::handle<CellType, vertex_tag>::type             VertexHandleType;
      typedef typename viennagrid::result_of::element<CellType, line_tag>::type             EdgeType;
      
      typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
      typedef typename VertexTypeOut::id_type VertexIDTypeOut;


      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num > vertices;
      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellRange vertices_on_cell = viennagrid::elements<vertex_tag>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertices[0] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[1] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[2] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[3] = vertex_to_vertex_handle_accessor(*vocit);  
      

      //
      // Step 2: Bring vertices in correct order
      //
      //VertexType * ordered_vertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num + 3];
      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num + 3 > ordered_vertices;
      EdgeOnCellRange edges_on_cell = viennagrid::elements<line_tag>(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit; ++eocit;
      EdgeType const & e3 = *eocit; ++eocit;
      EdgeType const & e4 = *eocit; ++eocit;
      EdgeType const & e5 = *eocit;
      
      //with e0
      if (edge_refinement_flag_accessor(e0) == true)
      {
        if (edge_refinement_flag_accessor(e1) == true)
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e0);
          
          if (edge_refinement_flag_accessor(e2) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e2);
            apply3_1<CellType>(segment_out, ordered_vertices);
          }
          else if (edge_refinement_flag_accessor(e3) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e3);
            apply3_2<CellType>(segment_out, ordered_vertices);
          }
          else if (edge_refinement_flag_accessor(e4) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e4);
            apply3_4<CellType>(segment_out, ordered_vertices);
          }
          else if (edge_refinement_flag_accessor(e5) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
            apply3_3<CellType>(segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else if (edge_refinement_flag_accessor(e2) == true)
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e2);
          
          if (edge_refinement_flag_accessor(e3) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e3);
            apply3_3<CellType>(segment_out, ordered_vertices);
          }
          else if (edge_refinement_flag_accessor(e4) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e4);
            apply3_2<CellType>(segment_out, ordered_vertices);
          }
          else if (edge_refinement_flag_accessor(e5) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
            apply3_4<CellType>(segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else if (edge_refinement_flag_accessor(e3) == true)
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e3);
          
          if (edge_refinement_flag_accessor(e4) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e4);
            apply3_1<CellType>(segment_out, ordered_vertices);
          }
          else if (edge_refinement_flag_accessor(e5) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
            apply3_4<CellType>(segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else if (edge_refinement_flag_accessor(e4) == true)
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e4);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e0);
          
          if (edge_refinement_flag_accessor(e5) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
            apply3_3<CellType>(segment_out, ordered_vertices);
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
      else if (edge_refinement_flag_accessor(e1) == true)
      {
        if (edge_refinement_flag_accessor(e2) == true)
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e1);
          
          if (edge_refinement_flag_accessor(e3) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e3);
            apply3_4<CellType>(segment_out, ordered_vertices);
          }
          else if (edge_refinement_flag_accessor(e4) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e4);
            apply3_3<CellType>(segment_out, ordered_vertices);
          }
          else if (edge_refinement_flag_accessor(e5) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
            apply3_2<CellType>(segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else if (edge_refinement_flag_accessor(e3) == true)
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e1);
          
          if (edge_refinement_flag_accessor(e4) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e4);
            apply3_3<CellType>(segment_out, ordered_vertices);
          }
          else if (edge_refinement_flag_accessor(e5) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
            apply3_1<CellType>(segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else if (edge_refinement_flag_accessor(e4) == true)
        {
          if (edge_refinement_flag_accessor(e5) == true)
          {
            //make edges 4 and 5 the references
            ordered_vertices[0] = vertices[1];
            ordered_vertices[1] = vertices[3];
            ordered_vertices[2] = vertices[2];
            ordered_vertices[3] = vertices[0];
            ordered_vertices[4] = edge_to_vertex_handle_accessor(e4);
            ordered_vertices[5] = edge_to_vertex_handle_accessor(e5);
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e1);
            
            apply3_4<CellType>(segment_out, ordered_vertices);
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
      else if (edge_refinement_flag_accessor(e2) == true)
      {
        if (edge_refinement_flag_accessor(e3) == true)
        {
          //NOTE: edges 2 and 3 don't have a common vertex, therefore 'base facet' is chosen depending on the third edge
          
          if (edge_refinement_flag_accessor(e4) == true)
          {
            // take edges 2 and 4 as reference
            ordered_vertices[0] = vertices[0];
            ordered_vertices[1] = vertices[3];
            ordered_vertices[2] = vertices[1];
            ordered_vertices[3] = vertices[2];
            ordered_vertices[4] = edge_to_vertex_handle_accessor(e2);
            ordered_vertices[5] = edge_to_vertex_handle_accessor(e4);
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e3);
            
            apply3_4<CellType>(segment_out, ordered_vertices);
          }
          else if (edge_refinement_flag_accessor(e5) == true)
          {
            // take edges 5 and 3 as reference
            ordered_vertices[0] = vertices[3];
            ordered_vertices[1] = vertices[2];
            ordered_vertices[2] = vertices[1];
            ordered_vertices[3] = vertices[0];
            ordered_vertices[4] = edge_to_vertex_handle_accessor(e5);
            ordered_vertices[5] = edge_to_vertex_handle_accessor(e3);
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e2);
            
            apply3_3<CellType>(segment_out, ordered_vertices);
          }
          else
          {
            assert(false && "Logic error: No edge for refinement found!"); 
          }
        }
        else if (edge_refinement_flag_accessor(e4) == true)
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e4);
          
          if (edge_refinement_flag_accessor(e5) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
            apply3_1<CellType>(segment_out, ordered_vertices);
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
      else if (edge_refinement_flag_accessor(e3) == true)
      {
        if (edge_refinement_flag_accessor(e4) == true)
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e4);
          
          if (edge_refinement_flag_accessor(e5) == true)
          {
            ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
            apply3_2<CellType>(segment_out, ordered_vertices);
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

    /** @brief Refinement of a tetrahedron, bisecting four edges. Case 1: The two edges have a common vertex.
     *
     * Orientation: (dots are in the background)
     *
     *            3 -----6----- 2
     *          /   \      .   /
     *         7      \ .     /
     *        /     4   5    /
     *       /  .         \ /
     *     0 ------------- 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexHandleIteratorType>
    static void apply4_1(DomainTypeOut & segment_out,
                         VertexHandleIteratorType vertices
                        )
    {
//       CellType new_cell;
//       VertexType * cellvertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num];

      //double diag03_len = viennagrid::norm(vertices[0]->point() - vertices[3]->point());
      //double diag13_len = viennagrid::norm(vertices[1]->point() - vertices[3]->point());
      //double diag23_len = viennagrid::norm(vertices[2]->point() - vertices[3]->point());

      
      
        
      
      //if (diag03_len > diag13_len) //split edge 03, introduce line 71
      if (stable_line_is_longer(segment_out, vertices, 0, 3, 1, 3)) //split edge 03, introduce line 71
      {
        create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 4, 7);
        create_refinement_cell<CellType>( segment_out, vertices, 7, 5, 6, 3);
        
        //if (diag13_len > diag23_len) //split edge 13, introduce line 52
        if (stable_line_is_longer(segment_out, vertices, 1, 3, 2, 3)) //split edge 13, introduce line 52
        {
          //std::cout << "apply_4_1_1" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 7, 1, 4, 5);
          create_refinement_cell<CellType>( segment_out, vertices, 7, 5, 4, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 1, 2, 4, 5);
          create_refinement_cell<CellType>( segment_out, vertices, 4, 5, 2, 6);
        }
        else //split edge 23, introduce line 61
        {
          //std::cout << "apply_4_1_2" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 7, 1, 6, 5);
          create_refinement_cell<CellType>( segment_out, vertices, 7, 1, 4, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 1, 2, 4, 6);
        }
      }
      else //split edge 13, introduce line 50
      {
        create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 4, 5);
        create_refinement_cell<CellType>( segment_out, vertices, 0, 5, 4, 7);
        create_refinement_cell<CellType>( segment_out, vertices, 7, 5, 6, 3);
        create_refinement_cell<CellType>( segment_out, vertices, 7, 5, 4, 6);

        //if (diag13_len > diag23_len) //split edge 13 again, introduce line 52
        if (stable_line_is_longer(segment_out, vertices, 1, 3, 2, 3)) //split edge 13 again, introduce line 52
        {
          //std::cout << "apply_4_1_3" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 1, 2, 4, 5);
          create_refinement_cell<CellType>( segment_out, vertices, 4, 5, 2, 6);
        }
        else //split edge 23, introduce line 61
        {
          //std::cout << "apply_4_1_4" << std::endl;
          create_refinement_cell<CellType>( segment_out, vertices, 5, 1, 4, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 4, 1, 2, 6);
        }
      }
      
    }
    
    /** @brief Refinement of a tetrahedron, bisecting four edges. Case 2: The two edges don't have any common vertex.
     *
     *
     * Orientation: (dots are in the background)
     *
     *            3 ----------- 2
     *          /   \      .   /
     *         6      \ .     4
     *        /     5   7    /
     *       /  .         \ /
     *     0 ------------- 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexHandleIteratorType>
    static void apply4_2(DomainTypeOut & segment_out,
                         VertexHandleIteratorType vertices
                        )
    {
//       CellType new_cell;
//       VertexType * cellvertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num];

      //double diag02_len = viennagrid::norm(vertices[0]->point() - vertices[2]->point());
      //double diag03_len = viennagrid::norm(vertices[0]->point() - vertices[3]->point());
      //double diag12_len = viennagrid::norm(vertices[1]->point() - vertices[2]->point());
      //double diag13_len = viennagrid::norm(vertices[1]->point() - vertices[3]->point());

      //if (diag03_len > diag13_len) //split edge 03, introduce line 61
      if (stable_line_is_longer(segment_out, vertices, 0, 3, 1, 3)) //split edge 03, introduce line 61
      {
        //std::cout << "split!" << std::endl;
        //if (diag13_len > diag12_len) //split edge 13, introduce line 72
        if (stable_line_is_longer(segment_out, vertices, 1, 3, 1, 2)) //split edge 13, introduce line 72
        {
          //if (diag02_len > diag03_len) //split edge 02, introduce line 53
          if (stable_line_is_longer(segment_out, vertices, 0, 2, 0, 3)) //split edge 02, introduce line 53
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(segment_out, vertices, 0, 2, 1, 2)) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_1_" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 1, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 4, 2, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 5, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 7, 2, 5, 3);
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
            if (stable_line_is_longer(segment_out, vertices, 0, 2, 1, 2)) //split edge 02, introduce line 51
            {
              //std::cout << "split!" << std::endl;
              //std::cout << "apply_4_2_2" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 6, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 7, 4, 6, 2);
              create_refinement_cell<CellType>( segment_out, vertices, 4, 2, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 2, 3);
              //std::cout << "done!" << std::endl;
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_3" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 4, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 1, 4, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 4, 2);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 4, 5, 2);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 2, 3);
            }
          }
        }
        else //split edge 12, introduce line 43
        {
          //if (diag02_len > diag03_len) //split edge 02, introduce line 53
          if (stable_line_is_longer(segment_out, vertices, 0, 2, 0, 3)) //split edge 02, introduce line 53
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(segment_out, vertices, 0, 2, 1, 2)) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_4" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 1, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 5, 4, 2, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 5, 7, 4, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 5, 3);
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_5" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 4, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 1, 4, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 4, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 4, 5, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 5, 4, 2, 3);
            }
          }
          else //split edge 03, introduce line 62
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(segment_out, vertices, 0, 2, 1, 2)) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_6" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 6, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 7, 4, 6, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 4, 2, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 4, 2, 3);
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_7" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 4, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 1, 4, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 4, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 5, 4, 2, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 4, 2, 3);
            }
          }
        }
      }
      else //split edge 13, introduce line 70
      {
        //if (diag13_len > diag12_len) //split edge 13, introduce line 72
        if (stable_line_is_longer(segment_out, vertices, 1, 3, 1, 2)) //split edge 13, introduce line 72
        {
          //if (diag02_len > diag03_len) //split edge 02, introduce line 53
          if (stable_line_is_longer(segment_out, vertices, 0, 2, 0, 3)) //split edge 02, introduce line 53
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(segment_out, vertices, 0, 2, 1, 2)) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_8" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 7, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 5, 4, 2, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 5, 7, 2, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 5, 3);
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_9" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 4, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 7, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 5, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 5, 4, 2, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 5, 7, 2, 3);
            }
          }
          else //split edge 03, introduce line 62
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(segment_out, vertices, 0, 2, 1, 2)) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_10" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 7, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 4, 2, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 7, 2, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 7, 2, 6, 3);
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_11" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 4, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 7, 4, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 4, 2);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 4, 5, 2);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 2, 3);
            }
          }
        }
        else //split edge 12, introduce line 43
        {
          //if (diag02_len > diag03_len) //split edge 02, introduce line 53
          if (stable_line_is_longer(segment_out, vertices, 0, 2, 0, 3)) //split edge 02, introduce line 53
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(segment_out, vertices, 0, 2, 1, 2)) //split edge 02, introduce line 51
            {
              //std::cout << "apply_4_2_12" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 7, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 5, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 7, 4, 5, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 4, 2, 5, 3);
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_13" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 4, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 7, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 5, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 7, 4, 5, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 4, 2, 5, 3);
            }
          }
          else //split edge 03, introduce line 62
          {
            //if (diag02_len > diag12_len) //split edge 02, introduce line 51
            if (stable_line_is_longer(segment_out, vertices, 0, 2, 1, 2)) //split edge 02, introduce line 51
            {
              //we have diag12_len > diag13_len > diag03_len > diag02_len alreday, hence this case is bogus!
              assert( false && "Logic error: diag02 > diag12 not possible here!");
            }
            else //split edge 12, introduce line 40
            {
              //std::cout << "apply_4_2_14" << std::endl;
              create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 4, 7);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 7, 4, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 4, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 6, 4, 2, 3);
              create_refinement_cell<CellType>( segment_out, vertices, 5, 4, 2, 6);
            }
          }
        }
      }
    }
    
    
    /** @brief Entry function for a refinement of a tetrahedron by a bisection of four edges. Reorders vertices to reduce complexity. */
    template <typename CellType, typename DomainTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply4(CellType const & cell_in, DomainTypeOut & segment_out,
                       EdgeRefinementFlagAccessor const edge_refinement_flag_accessor, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const edge_to_vertex_handle_accessor)
    {
//       std::cout << "tetrahedron::apply4()" << std::endl;
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::element<CellType, vertex_tag>::type             VertexType;
      typedef typename viennagrid::result_of::handle<CellType, vertex_tag>::type             VertexHandleType;
      typedef typename viennagrid::result_of::element<CellType, line_tag>::type             EdgeType;
      
      typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
      typedef typename VertexTypeOut::id_type VertexIDTypeOut;


      //VertexType * vertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num];
      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num > vertices;
      //std::cout << "apply4()" << std::endl;
      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellRange vertices_on_cell = viennagrid::elements<vertex_tag>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
//       vertices[0] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]); ++vocit;
//       vertices[1] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]); ++vocit;
//       vertices[2] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]); ++vocit;
//       vertices[3] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]);
      vertices[0] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[1] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[2] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[3] = vertex_to_vertex_handle_accessor(*vocit);  
      

      //
      // Step 2: Bring vertices in correct order, such that refined edge is on {0,1}-edge
      //
      //VertexType * ordered_vertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num + 4];
      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num + 4 > ordered_vertices;
      EdgeOnCellRange edges_on_cell = viennagrid::elements<line_tag>(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit; ++eocit;
      EdgeType const & e3 = *eocit; ++eocit;
      EdgeType const & e4 = *eocit; ++eocit;
      EdgeType const & e5 = *eocit;
      
      //with e0
      if (edge_refinement_flag_accessor(e0) == false)
      {
        if (edge_refinement_flag_accessor(e1) == false)
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e4);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e5);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e2) == false)
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e4);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e3);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e3) == false) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e4);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e2);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e4) == false) 
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e5);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e5) == false) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e4);
          
          apply4_2<CellType>(segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (edge_refinement_flag_accessor(e1) == false)
      {
        if (edge_refinement_flag_accessor(e2) == false)
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e5);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e4);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e3) == false) 
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e5);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e4);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e4) == false) 
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[3];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e2);
          
          apply4_2<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e5) == false) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e4);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e0);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (edge_refinement_flag_accessor(e2) == false)
      {
        if (edge_refinement_flag_accessor(e3) == false) 
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[0];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e5);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e4);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e0);
          
          apply4_2<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e4) == false) 
        {
          ordered_vertices[0] = vertices[0];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[2];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e5);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e1);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e5) == false) 
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[0];
          ordered_vertices[3] = vertices[1];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e4);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e3);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (edge_refinement_flag_accessor(e3) == false)
      {
        if (edge_refinement_flag_accessor(e4) == false) 
        {
          ordered_vertices[0] = vertices[2];
          ordered_vertices[1] = vertices[1];
          ordered_vertices[2] = vertices[3];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e5);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e1);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
        }
        else if (edge_refinement_flag_accessor(e5) == false) 
        {
          ordered_vertices[0] = vertices[3];
          ordered_vertices[1] = vertices[2];
          ordered_vertices[2] = vertices[1];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e4);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e0);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e2);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
        }
        else
        {
          assert(false && "Logic error: No edge for refinement found!"); 
        }
      }
      else if (edge_refinement_flag_accessor(e4) == false)
      {
        if (edge_refinement_flag_accessor(e5) == false) 
        {
          ordered_vertices[0] = vertices[1];
          ordered_vertices[1] = vertices[3];
          ordered_vertices[2] = vertices[2];
          ordered_vertices[3] = vertices[0];
          ordered_vertices[4] = edge_to_vertex_handle_accessor(e3);
          ordered_vertices[5] = edge_to_vertex_handle_accessor(e2);
          ordered_vertices[6] = edge_to_vertex_handle_accessor(e1);
          ordered_vertices[7] = edge_to_vertex_handle_accessor(e0);
          
          apply4_1<CellType>(segment_out, ordered_vertices);
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

    
    /** @brief Refinement of a tetrahedron, bisecting five edges. Case 1 (no other cases): Only edge 01 is not refined.
     *
     * Orientation: (dots are in the background)
     *
     *            3 -----8----- 2
     *          /   \      .   /
     *         6      \ .     /
     *        /     5   7    4
     *       /  .         \ /
     *     0 ------------- 1
     * 
     * 
     */
    template <typename CellType, typename DomainTypeOut, typename VertexHandleIteratorType>
    static void apply5_1(DomainTypeOut & segment_out,
                         VertexHandleIteratorType vertices
                        )
    {
//       CellType new_cell;
//       VertexType * cellvertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num];

      create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 8, 3);
      create_refinement_cell<CellType>( segment_out, vertices, 5, 4, 2, 8);
      

      //if (diag03_len > diag13_len) //split edge 03, introduce line 61
      if (stable_line_is_longer(segment_out, vertices, 0, 3, 1, 3)) //split edge 03, introduce line 61
      {
        create_refinement_cell<CellType>( segment_out, vertices, 6, 4, 5, 8);
        create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 4, 8);
        create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 6, 7);
        
        //if (diag02_len > diag12_len) //split edge 02, introduce line 51
        if (stable_line_is_longer(segment_out, vertices, 0, 2, 1, 2)) //split edge 02, introduce line 51
        {
          create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 5, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 5, 6);
        }
        else //split edge 12, introduce line 40
        {
          create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 4, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
        }
      }
      else  //split edge 13, introduce line 70
      {
        //if (diag02_len > diag12_len) //split edge 02, introduce line 51
        if (stable_line_is_longer(segment_out, vertices, 0, 2, 1, 2)) //split edge 02, introduce line 51
        {
          create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 5, 7);
          create_refinement_cell<CellType>( segment_out, vertices, 0, 7, 5, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 1, 4, 5, 7);
          create_refinement_cell<CellType>( segment_out, vertices, 7, 4, 5, 8);
          create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 5, 8);
        }
        else //split edge 12, introduce line 40
        {
          create_refinement_cell<CellType>( segment_out, vertices, 0, 1, 4, 7);
          create_refinement_cell<CellType>( segment_out, vertices, 0, 7, 4, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);
          create_refinement_cell<CellType>( segment_out, vertices, 6, 4, 5, 8);
          create_refinement_cell<CellType>( segment_out, vertices, 6, 7, 4, 8);
        }
      }
    }
    
    /** @brief Entry function for the refinement of a tetrahedron bisecting five edges. Reorders vertices to reduce complexity */
    template <typename CellType, typename DomainTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply5(CellType const & cell_in, DomainTypeOut & segment_out,
                       EdgeRefinementFlagAccessor const edge_refinement_flag_accessor, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const edge_to_vertex_handle_accessor)
    {
//       std::cout << "tetrahedron::apply5()" << std::endl;
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::element<CellType, vertex_tag>::type             VertexType;
      typedef typename viennagrid::result_of::handle<CellType, vertex_tag>::type             VertexHandleType;
      typedef typename viennagrid::result_of::element<CellType, line_tag>::type             EdgeType;
      
      typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
      typedef typename VertexTypeOut::id_type VertexIDTypeOut;

      
      //VertexType * vertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num];
      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num > vertices;
      //std::cout << "apply4()" << std::endl;
      
      //
      // Step 1: Get vertices from input cell
      //
      VertexOnCellRange vertices_on_cell = viennagrid::elements<vertex_tag>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
//       vertices[0] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]); ++vocit;
//       vertices[1] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]); ++vocit;
//       vertices[2] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]); ++vocit;
//       vertices[3] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]);
      vertices[0] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[1] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[2] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[3] = vertex_to_vertex_handle_accessor(*vocit);  
      

      //
      // Step 2: Bring vertices in correct order, such that refined edge is on {0,1}-edge
      //
      //VertexType * ordered_vertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num + 5];
      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num + 5 > ordered_vertices;
      EdgeOnCellRange edges_on_cell = viennagrid::elements<line_tag>(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit; ++eocit;
      EdgeType const & e3 = *eocit; ++eocit;
      EdgeType const & e4 = *eocit; ++eocit;
      EdgeType const & e5 = *eocit;
      
      if (edge_refinement_flag_accessor(e0) == false)
      {
        ordered_vertices[0] = vertices[0];
        ordered_vertices[1] = vertices[1];
        ordered_vertices[2] = vertices[2];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e3);
        ordered_vertices[5] = edge_to_vertex_handle_accessor(e1);
        ordered_vertices[6] = edge_to_vertex_handle_accessor(e2);
        ordered_vertices[7] = edge_to_vertex_handle_accessor(e4);
        ordered_vertices[8] = edge_to_vertex_handle_accessor(e5);
        
        apply5_1<CellType>(segment_out, ordered_vertices);
      }
      else if (edge_refinement_flag_accessor(e1) == false)
      {
        ordered_vertices[0] = vertices[2];
        ordered_vertices[1] = vertices[0];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e0);
        ordered_vertices[5] = edge_to_vertex_handle_accessor(e3);
        ordered_vertices[6] = edge_to_vertex_handle_accessor(e5);
        ordered_vertices[7] = edge_to_vertex_handle_accessor(e2);
        ordered_vertices[8] = edge_to_vertex_handle_accessor(e4);
        
        apply5_1<CellType>(segment_out, ordered_vertices);
      }
      else if (edge_refinement_flag_accessor(e2) == false)
      {
        ordered_vertices[0] = vertices[0];
        ordered_vertices[1] = vertices[3];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[2];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e4);
        ordered_vertices[5] = edge_to_vertex_handle_accessor(e0);
        ordered_vertices[6] = edge_to_vertex_handle_accessor(e1);
        ordered_vertices[7] = edge_to_vertex_handle_accessor(e5);
        ordered_vertices[8] = edge_to_vertex_handle_accessor(e3);
        
        apply5_1<CellType>(segment_out, ordered_vertices);
      }
      else if (edge_refinement_flag_accessor(e3) == false)
      {
        ordered_vertices[0] = vertices[1];
        ordered_vertices[1] = vertices[2];
        ordered_vertices[2] = vertices[0];
        ordered_vertices[3] = vertices[3];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e1);
        ordered_vertices[5] = edge_to_vertex_handle_accessor(e0);
        ordered_vertices[6] = edge_to_vertex_handle_accessor(e4);
        ordered_vertices[7] = edge_to_vertex_handle_accessor(e5);
        ordered_vertices[8] = edge_to_vertex_handle_accessor(e2);
        
        apply5_1<CellType>(segment_out, ordered_vertices);
      }
      else if (edge_refinement_flag_accessor(e4) == false)
      {
        ordered_vertices[0] = vertices[1];
        ordered_vertices[1] = vertices[3];
        ordered_vertices[2] = vertices[2];
        ordered_vertices[3] = vertices[0];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e5);
        ordered_vertices[5] = edge_to_vertex_handle_accessor(e3);
        ordered_vertices[6] = edge_to_vertex_handle_accessor(e0);
        ordered_vertices[7] = edge_to_vertex_handle_accessor(e2);
        ordered_vertices[8] = edge_to_vertex_handle_accessor(e1);
        
        apply5_1<CellType>(segment_out, ordered_vertices);
      }
      else if (edge_refinement_flag_accessor(e5) == false)
      {
        ordered_vertices[0] = vertices[3];
        ordered_vertices[1] = vertices[2];
        ordered_vertices[2] = vertices[1];
        ordered_vertices[3] = vertices[0];
        ordered_vertices[4] = edge_to_vertex_handle_accessor(e3);
        ordered_vertices[5] = edge_to_vertex_handle_accessor(e4);
        ordered_vertices[6] = edge_to_vertex_handle_accessor(e2);
        ordered_vertices[7] = edge_to_vertex_handle_accessor(e1);
        ordered_vertices[8] = edge_to_vertex_handle_accessor(e0);
        
        apply5_1<CellType>(segment_out, ordered_vertices);
      }
      else
      {
        assert(false && "Logic error: No edge for refinement found!"); 
      }
    }    




    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    /** @brief Refinement of a tetrahedron, bisecting six edges: Split the cell into 8 tets */
    template <typename CellType, typename DomainTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply6(CellType const & cell_in, DomainTypeOut & segment_out,
                       EdgeRefinementFlagAccessor const edge_refinement_flag_accessor, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const edge_to_vertex_handle_accessor)
    {
//       std::cout << "tetrahedron::apply6()" << std::endl;
      typedef typename viennagrid::result_of::const_element_range<CellType, vertex_tag>::type            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;            
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;            
      
      typedef typename viennagrid::result_of::element<CellType, vertex_tag>::type             VertexType;
      typedef typename viennagrid::result_of::handle<CellType, vertex_tag>::type             VertexHandleType;
      typedef typename viennagrid::result_of::element<CellType, line_tag>::type             EdgeType;
      
      typedef typename viennagrid::result_of::element<DomainTypeOut, vertex_tag>::type                                      VertexTypeOut;
      typedef typename VertexTypeOut::id_type VertexIDTypeOut;

      storage::static_array< VertexHandleType, element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num +
                                             element_topology::boundary_cells<tetrahedron_tag, line_tag>::num> vertices;
      
//       VertexType * vertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num
//                             + element_topology::boundary_cells<tetrahedron_tag, line_tag>::num];
      //std::cout << "apply6()" << std::endl;
      
      //
      // Step 1: Get vertices on the new domain
      //
      
      //grab existing vertices:
      VertexOnCellRange vertices_on_cell = viennagrid::elements<vertex_tag>(cell_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
//       vertices[0] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]); ++vocit;
//       vertices[1] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]); ++vocit;
//       vertices[2] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]); ++vocit;
//       vertices[3] = &(viennagrid::elements<vertex_tag>(segment_out.domain())[vocit->id()]);
      vertices[0] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[1] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[2] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertices[3] = vertex_to_vertex_handle_accessor(*vocit);  
      
      //add vertices from edge
      EdgeOnCellRange edges_on_cell = viennagrid::elements<line_tag>(cell_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      
      
      
      vertices[4] = edge_to_vertex_handle_accessor(*eocit); ++eocit;
      vertices[5] = edge_to_vertex_handle_accessor(*eocit); ++eocit;
      vertices[6] = edge_to_vertex_handle_accessor(*eocit); ++eocit;
      vertices[7] = edge_to_vertex_handle_accessor(*eocit); ++eocit;
      vertices[8] = edge_to_vertex_handle_accessor(*eocit); ++eocit;
      vertices[9] = edge_to_vertex_handle_accessor(*eocit);
      
      //
      // Step 2: Add new cells to new domain:
      //
//       CellType new_cell;
//       VertexType * cellvertices[element_topology::boundary_cells<tetrahedron_tag, vertex_tag>::num];
      
      //0-4-5-6:
      create_refinement_cell<CellType>( segment_out, vertices, 0, 4, 5, 6);

      //1-7-4-8:
      create_refinement_cell<CellType>( segment_out, vertices, 1, 7, 4, 8);

      //2-5-7-9:
      create_refinement_cell<CellType>( segment_out, vertices, 2, 5, 7, 9);

      //3-8-6-9:
      create_refinement_cell<CellType>( segment_out, vertices, 3, 8, 6, 9);
      
      double diag58 = viennagrid::norm( viennagrid::point(segment_out, vertices[5]) - viennagrid::point(segment_out, vertices[8]) );
      double diag67 = viennagrid::norm( viennagrid::point(segment_out, vertices[6]) - viennagrid::point(segment_out, vertices[7]) );
      double diag49 = viennagrid::norm( viennagrid::point(segment_out, vertices[4]) - viennagrid::point(segment_out, vertices[9]) );
      
      if ( (diag58 <= diag67) && (diag58 <= diag49) )  //diag58 is shortest: keep it, split others
      {
        //4-8-5-6:
        create_refinement_cell<CellType>( segment_out, vertices, 4, 8, 5, 6);
        
        //4-8-7-5:
        create_refinement_cell<CellType>( segment_out, vertices, 4, 8, 7, 5);

        //7-8-9-5:
        create_refinement_cell<CellType>( segment_out, vertices, 7, 8, 9, 5);

        //8-6-9-5:
        create_refinement_cell<CellType>( segment_out, vertices, 8, 6, 9, 5);
      }
      else if ( (diag67 <= diag58) && (diag67 <= diag49) ) //diag67 is shortest: keep it, split others
      {
        //4-7-6-8:
        create_refinement_cell<CellType>( segment_out, vertices, 4, 7, 6, 8);
        
        //4-7-5-6:
        create_refinement_cell<CellType>( segment_out, vertices, 4, 7, 5, 6);

        //7-9-6-8:
        create_refinement_cell<CellType>( segment_out, vertices, 7, 9, 6, 8);

        //7-9-5-6:
        create_refinement_cell<CellType>( segment_out, vertices, 7, 9, 5, 6);
      }
      else //keep shortest diagonal diag49
      {
        //4-9-6-8:
        create_refinement_cell<CellType>( segment_out, vertices, 4, 9, 6, 8);
        
        //4-9-5-6:
        create_refinement_cell<CellType>( segment_out, vertices, 4, 9, 5, 6);

        //4-7-9-8:
        create_refinement_cell<CellType>( segment_out, vertices, 4, 7, 9, 8);

        //4-7-5-9:
        create_refinement_cell<CellType>( segment_out, vertices, 4, 7, 5, 9);
      }
      
    } //apply6()
    
    
    //
    /** @brief Public entry function for the refinement of a tetrahedron.
     * 
     * @param cell_in       The tetrahedron to be refined
     * @param segment_out   The domain or segment the refined tetrahedra are written to
     */
    template <typename CellType, typename DomainTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply(CellType const & cell_in, DomainTypeOut & segment_out,
                      EdgeRefinementFlagAccessor const edge_refinement_flag_accessor, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const edge_to_vertex_handle_accessor)
    {
      typedef typename viennagrid::result_of::const_element_range<CellType, line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                 EdgeOnCellIterator;            
      
      //std::cout << "tetrahedron_tag::apply()" << std::endl;
      std::size_t edges_to_refine = 0;
      EdgeOnCellRange edges_on_cell = viennagrid::elements(cell_in);
      for (EdgeOnCellIterator eocit = edges_on_cell.begin();
                              eocit != edges_on_cell.end();
                            ++eocit)
      {
        if (edge_refinement_flag_accessor(*eocit) == true)
          ++edges_to_refine;
      }
      
      switch (edges_to_refine)
      {
        case 0: apply0(cell_in, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor); break;
        case 1: apply1(cell_in, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor); break;
        case 2: apply2(cell_in, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor); break;
        case 3: apply3(cell_in, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor); break;
        case 4: apply4(cell_in, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor); break;
        case 5: apply5(cell_in, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor); break;
        case 6: apply6(cell_in, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor); break;
        default: //nothing to do...
                break;
      }
    } //apply()
    
  };
  
    
  
}

#endif

