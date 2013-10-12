#ifndef VIENNAGRID_TOPOLOGY_TRIANGLE_HPP
#define VIENNAGRID_TOPOLOGY_TRIANGLE_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/forwards.hpp"
#include "viennagrid/topology/vertex.hpp"
#include "viennagrid/topology/line.hpp"
#include "viennagrid/algorithm/norm.hpp"

/** @file viennagrid/refinement/triangle.hpp
    @brief Provides refinement routines for a triangle
*/

namespace viennagrid
{


  template<typename ElementType, typename MeshType, typename VertexHandleContainer>
  void make_refinement_element(MeshType & mesh, VertexHandleContainer vertex_handle_container, unsigned int i0, unsigned int i1, unsigned int i2)
  {
    typedef typename VertexHandleContainer::iterator VertexHandleIteratorType;
    typedef typename std::iterator_traits<VertexHandleIteratorType>::value_type VertexHandleType;
    storage::static_array< VertexHandleType, boundary_elements<triangle_tag, vertex_tag>::num > cellvertices;

    cellvertices[0] = *viennagrid::advance(vertex_handle_container.begin(), i0);
    cellvertices[1] = *viennagrid::advance(vertex_handle_container.begin(), i1);
    cellvertices[2] = *viennagrid::advance(vertex_handle_container.begin(), i2);

    viennagrid::make_element<ElementType>( mesh, cellvertices.begin(), cellvertices.end() );
  }


  /** @brief Specialization of the refinement class for a triangle */
  template <>
  struct element_refinement<triangle_tag>
  {

    /** @brief No refinement. Just put same cell into new mesh. */
    template <typename ElementType, typename MeshTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply0(ElementType const & element_in, MeshTypeOut & segment_out,
                       EdgeRefinementFlagAccessor const, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const)
    {
      typedef typename viennagrid::result_of::const_element_range<ElementType, viennagrid::vertex_tag>::type            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;

      typedef typename viennagrid::result_of::handle<MeshTypeOut, viennagrid::vertex_tag>::type             VertexHandleType;

      storage::static_array<VertexHandleType, boundary_elements<triangle_tag, vertex_tag>::num> vertex_handles;

      //
      // Step 1: Get vertices on the new mesh
      //

      //grab existing vertices:
      VertexOnCellRange vertices_on_cell = viennagrid::elements<viennagrid::vertex_tag>(element_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertex_handles[0] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertex_handles[1] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertex_handles[2] = vertex_to_vertex_handle_accessor(*vocit);

      //
      // Step 2: Add new cells to new mesh:
      //
      viennagrid::make_element<ElementType>( segment_out, vertex_handles.begin(), vertex_handles.end() );

    } //apply0()


    /** @brief Refinement for one edge to be bisected.
     *
     * Orientation of vertices (established by rotating the triangle appropriately)
     *
     *           2
     *         /   \
     *        /     \
     *       0 - 3 - 1
     */
    template <typename ElementType, typename MeshTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply1(ElementType const & element_in, MeshTypeOut & segment_out,
                       EdgeRefinementFlagAccessor const edge_refinement_flag_accessor, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const edge_to_vertex_handle_accessor)
    {
      typedef typename viennagrid::result_of::const_element_range<ElementType, viennagrid::vertex_tag>::type            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
      typedef typename viennagrid::result_of::const_element_range<ElementType, viennagrid::line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;

      typedef typename viennagrid::result_of::handle<MeshTypeOut, viennagrid::vertex_tag>::type             VertexHandleType;
      typedef typename viennagrid::result_of::element<MeshTypeOut, viennagrid::line_tag>::type             EdgeType;

      const unsigned int num_vertices = boundary_elements<triangle_tag, vertex_tag>::num;
      storage::static_array<VertexHandleType, num_vertices+1> vertex_handles;


      //
      // Step 1: Get vertices on the new mesh
      //

      //grab existing vertices:
      VertexOnCellRange vertices_on_cell = viennagrid::elements(element_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertex_handles[0] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertex_handles[1] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertex_handles[2] = vertex_to_vertex_handle_accessor(*vocit);

      //add vertices from edge
      EdgeOnCellRange edges_on_cell = viennagrid::elements(element_in);
      std::size_t offset = 0;
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit;

      if ( edge_refinement_flag_accessor(e0) )
      {
        vertex_handles[3] = edge_to_vertex_handle_accessor(e0);
        offset = 0;
      }
      else if ( edge_refinement_flag_accessor(e1) )
      {
        vertex_handles[3] = edge_to_vertex_handle_accessor(e1);
        offset = 2;
      }
      else if ( edge_refinement_flag_accessor(e2) )
      {
        vertex_handles[3] = edge_to_vertex_handle_accessor(e2);
        offset = 1;
      }
      else
      {
        assert( (2 < 2) && "Logic error: Triangle does not have an edges for bisection!");
      }

      //
      // Step 2: Add new cells to new mesh:
      //
      //0-3-2
      make_refinement_element<ElementType>( segment_out, vertex_handles, (offset + 0) % num_vertices, 3, (offset + 2) % num_vertices );

      //3-1-2:
      make_refinement_element<ElementType>( segment_out, vertex_handles, 3, (offset + 1) % num_vertices, (offset + 2) % num_vertices );
    } //apply1()


    /** @brief Refinement for one edge to be bisected.
     *
     * Orientation of vertices:  (established by rotating the triangle appropriately)
     *
     *            2
     *          /   \
     *         /     4
     *        /       \
     *       0 -- 3 -- 1
    */
    template <typename ElementType, typename MeshTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply2(ElementType const & element_in, MeshTypeOut & segment_out,
                       EdgeRefinementFlagAccessor const edge_refinement_flag_accessor, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const edge_to_vertex_handle_accessor)
    {
      typedef typename viennagrid::result_of::const_element_range<ElementType, viennagrid::vertex_tag>::type            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
      typedef typename viennagrid::result_of::const_element_range<ElementType, viennagrid::line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;

      typedef typename viennagrid::result_of::handle<MeshTypeOut, viennagrid::vertex_tag>::type             VertexHandleType;
      typedef typename viennagrid::result_of::element<MeshTypeOut, viennagrid::line_tag>::type             EdgeType;

      const unsigned int num_vertices = boundary_elements<triangle_tag, vertex_tag>::num;
      storage::static_array<VertexHandleType, num_vertices+2> vertex_handles;


      //
      // Step 1: Get vertices on the new mesh
      //

      //grab existing vertices:
      VertexOnCellRange vertices_on_cell = viennagrid::elements<viennagrid::vertex_tag>(element_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertex_handles[0] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertex_handles[1] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertex_handles[2] = vertex_to_vertex_handle_accessor(*vocit);

      //Find rotation offset such that first two edges are to be refined
      EdgeOnCellRange edges_on_cell = viennagrid::elements<viennagrid::line_tag>(element_in);
      std::size_t offset = 0;

      EdgeOnCellIterator eocit = edges_on_cell.begin();
      EdgeType const & e0 = *eocit; ++eocit;
      EdgeType const & e1 = *eocit; ++eocit;
      EdgeType const & e2 = *eocit;

      if ( edge_refinement_flag_accessor(e0) && edge_refinement_flag_accessor(e1) )
      {
        vertex_handles[3] = edge_to_vertex_handle_accessor(e1);
        vertex_handles[4] = edge_to_vertex_handle_accessor(e0);
        offset = 2;
      }
      else if ( edge_refinement_flag_accessor(e0) && edge_refinement_flag_accessor(e2) )
      {
        vertex_handles[3] = edge_to_vertex_handle_accessor(e0);
        vertex_handles[4] = edge_to_vertex_handle_accessor(e2);
        offset = 0;
      }
      else if ( edge_refinement_flag_accessor(e1) && edge_refinement_flag_accessor(e2) )
      {
        vertex_handles[3] = edge_to_vertex_handle_accessor(e2);
        vertex_handles[4] = edge_to_vertex_handle_accessor(e1);
        offset = 1;
      }
      else
      {
        assert( (2 < 2) && "Logic error: Triangle does not have two edges for bisection!");
      }

      //
      // Step 2: Add new cells to new mesh:
      //

      //3-1-4:
      make_refinement_element<ElementType>( segment_out, vertex_handles, 3, (offset + 1) % num_vertices, 4 );

      //split second-longest edge
      VertexHandleType vh0 = vertex_handles[(offset + 0) % num_vertices];
      VertexHandleType vh1 = vertex_handles[(offset + 1) % num_vertices];
      VertexHandleType vh2 = vertex_handles[(offset + 2) % num_vertices];
      double len_edge1 = viennagrid::norm( viennagrid::point(segment_out, vh1) - viennagrid::point(segment_out, vh0) );
      double len_edge2 = viennagrid::norm( viennagrid::point(segment_out, vh2) - viennagrid::point(segment_out, vh1) );

      if (len_edge1 > len_edge2) //split edge [v0, v1] again
      {
        //0-3-2:
        make_refinement_element<ElementType>( segment_out, vertex_handles, (offset + 0) % num_vertices, 3, (offset + 2) % num_vertices );

        //2-3-4:
        make_refinement_element<ElementType>( segment_out, vertex_handles, (offset + 2) % num_vertices, 3, 4 );
      }
      else //split edge [v1, v2]
      {
        //0-3-4:
        make_refinement_element<ElementType>( segment_out, vertex_handles, (offset + 0) % num_vertices, 3, 4 );

        //0-4-2:
        make_refinement_element<ElementType>( segment_out, vertex_handles, (offset + 0) % num_vertices, 4, (offset + 2) % num_vertices );
      }


    } //apply2()




    /** @brief Refinement of a triangle with three edges to be refined (uniform refinement) */
    template <typename ElementType, typename MeshTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply3(ElementType const & element_in, MeshTypeOut & segment_out,
                       EdgeRefinementFlagAccessor const, VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor, EdgeToVertexHandleAccessor const edge_to_vertex_handle_accessor)
    {
      typedef typename viennagrid::result_of::const_element_range<ElementType, viennagrid::vertex_tag>::type            VertexOnCellRange;
      typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
      typedef typename viennagrid::result_of::const_element_range<ElementType, viennagrid::line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;

      typedef typename viennagrid::result_of::handle<MeshTypeOut, viennagrid::vertex_tag>::type             VertexHandleType;

      const unsigned int num_vertices = boundary_elements<triangle_tag, vertex_tag>::num;
      const unsigned int num_lines = boundary_elements<triangle_tag, line_tag>::num;

      storage::static_array<VertexHandleType, num_vertices+num_lines> vertex_handles;

      //
      // Step 1: Get vertices on the new mesh
      //

      //grab existing vertices:
      VertexOnCellRange vertices_on_cell = viennagrid::elements<viennagrid::vertex_tag>(element_in);
      VertexOnCellIterator vocit = vertices_on_cell.begin();
      vertex_handles[0] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertex_handles[1] = vertex_to_vertex_handle_accessor(*vocit); ++vocit;
      vertex_handles[2] = vertex_to_vertex_handle_accessor(*vocit);

      //add vertices from edge
      EdgeOnCellRange edges_on_cell = viennagrid::elements<viennagrid::line_tag>(element_in);
      EdgeOnCellIterator eocit = edges_on_cell.begin();
      vertex_handles[3] = edge_to_vertex_handle_accessor(*eocit); ++eocit;
      vertex_handles[4] = edge_to_vertex_handle_accessor(*eocit); ++eocit;
      vertex_handles[5] = edge_to_vertex_handle_accessor(*eocit);

      //
      // Step 2: Add new cells to new mesh:
      //

      //0-3-4:
      make_refinement_element<ElementType>( segment_out, vertex_handles, 0, 3, 4 );

      //3-1-5:
      make_refinement_element<ElementType>( segment_out, vertex_handles, 3, 1, 5 );

      //4-5-2:
      make_refinement_element<ElementType>( segment_out, vertex_handles, 4, 5, 2 );

      //4-3-5:
      make_refinement_element<ElementType>( segment_out, vertex_handles, 4, 3, 5 );

    } //apply3()


    /** @brief Public entry function for the refinement of a triangle.
     *
     * @param element_in       The triangle to be refined
     * @param segment_out   The mesh or segment the refined triangles are written to
     */
    template <typename ElementType, typename MeshTypeOut, typename EdgeRefinementFlagAccessor, typename VertexToVertexHandleAccessor, typename EdgeToVertexHandleAccessor>
    static void apply(ElementType const & element_in, MeshTypeOut & segment_out,
                      EdgeRefinementFlagAccessor const edge_refinement_flag_accessor,
                      VertexToVertexHandleAccessor const vertex_to_vertex_handle_accessor,
                      EdgeToVertexHandleAccessor edge_to_vertex_handle_accessor)
    {
      typedef typename viennagrid::result_of::const_element_range<ElementType, viennagrid::line_tag>::type            EdgeOnCellRange;
      typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                 EdgeOnCellIterator;

      std::size_t edges_to_refine = 0;
      EdgeOnCellRange edges_on_cell = viennagrid::elements<viennagrid::line_tag>(element_in);
      for (EdgeOnCellIterator eocit = edges_on_cell.begin();
                              eocit != edges_on_cell.end();
                            ++eocit)
      {
        if ( edge_refinement_flag_accessor(*eocit) )
          ++edges_to_refine;
      }

      switch (edges_to_refine)
      {
        case 0: apply0(element_in, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor); break;
        case 1: apply1(element_in, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor); break;
        case 2: apply2(element_in, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor); break;
        case 3: apply3(element_in, segment_out, edge_refinement_flag_accessor, vertex_to_vertex_handle_accessor, edge_to_vertex_handle_accessor); break;
        default: //nothing to do...
                break;
      }
    } //apply()
  };


}

#endif

