#ifndef VIENNAGRID_ALGORITHM_DETAIL_REFINE_TRI_HPP
#define VIENNAGRID_ALGORITHM_DETAIL_REFINE_TRI_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/viennagrid.hpp"

/** @file viennagrid/algorithm/detail/refine_tri.hpp
    @brief Provides refinement routines for a triangle
*/

namespace viennagrid
{
  namespace detail
  {

    template<typename ElementsVerticesHandleContainerT, typename VertexHandleContainer>
    void add_refinement_element(ElementsVerticesHandleContainerT & elements_vertices,
                                VertexHandleContainer vertex_handle_container,
                                unsigned int i0, unsigned int i1, unsigned int i2)
    {
      elements_vertices.resize( elements_vertices.size()+1 );

      elements_vertices.back().resize(3);
      elements_vertices.back()[0] = *viennagrid::advance(vertex_handle_container.begin(), i0);
      elements_vertices.back()[1] = *viennagrid::advance(vertex_handle_container.begin(), i1);
      elements_vertices.back()[2] = *viennagrid::advance(vertex_handle_container.begin(), i2);
    }


    /** @brief Specialization of the refinement class for a triangle */
    template<>
    struct element_refinement<triangle_tag>
    {

      /** @brief No refinement. Just put same cell into new mesh. */
      template<typename ElementType, typename MeshT,
               typename ElementsVerticesHandleContainerT,
               typename VertexCopyMapT,
               typename EdgeRefinementFlagAccessor, typename EdgeToVertexHandleAccessor>
      static void apply0(ElementType const & element_in, MeshT const &,
                         ElementsVerticesHandleContainerT & elements_vertices,
                         VertexCopyMapT & vertex_copy_map_,
                         EdgeRefinementFlagAccessor const &, EdgeToVertexHandleAccessor const &)
      {
        typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type            VertexOnCellRange;
        typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
        typedef typename viennagrid::result_of::element<MeshT>::type             VertexType;

        std::vector<VertexType> vertices( triangle_tag().vertex_count() );

        //
        // Step 1: Get vertices on the new mesh
        //

        //grab existing vertices:
        VertexOnCellRange vertices_on_cell(element_in);
        VertexOnCellIterator vocit = vertices_on_cell.begin();
        vertices[0] = vertex_copy_map_(*vocit); ++vocit;
        vertices[1] = vertex_copy_map_(*vocit); ++vocit;
        vertices[2] = vertex_copy_map_(*vocit);

        //
        // Step 2: Add new cells to new mesh:
        //

        add_refinement_element( elements_vertices, vertices, 0, 1, 2);

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
      template<typename ElementType, typename MeshT,
               typename ElementsVerticesHandleContainerT,
               typename VertexCopyMapT,
               typename EdgeRefinementFlagAccessor, typename EdgeToVertexHandleAccessor>
      static void apply1(ElementType const & element_in, MeshT const &,
                         ElementsVerticesHandleContainerT & elements_vertices,
                         VertexCopyMapT & vertex_copy_map_,
                         EdgeRefinementFlagAccessor const & edge_refinement_flag_accessor, EdgeToVertexHandleAccessor const & edge_to_vertex_handle_accessor)
      {
        typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type            VertexOnCellRange;
        typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
        typedef typename viennagrid::result_of::const_element_range<ElementType, 1>::type            EdgeOnCellRange;
        typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;

        typedef typename viennagrid::result_of::element<MeshT>::type             VertexType;
        typedef typename viennagrid::result_of::element<MeshT>::type             EdgeType;

        const unsigned int num_vertices = triangle_tag().vertex_count();
        std::vector<VertexType> vertices( num_vertices+1 );


        //
        // Step 1: Get vertices on the new mesh
        //

        //grab existing vertices:
        VertexOnCellRange vertices_on_cell(element_in);
        VertexOnCellIterator vocit = vertices_on_cell.begin();
        vertices[0] = vertex_copy_map_(*vocit); ++vocit;
        vertices[1] = vertex_copy_map_(*vocit); ++vocit;
        vertices[2] = vertex_copy_map_(*vocit);

        //add vertices from edge
        EdgeOnCellRange edges_on_cell(element_in);
        std::size_t offset = 0;
        EdgeOnCellIterator eocit = edges_on_cell.begin();
        EdgeType const & e0 = *eocit; ++eocit;
        EdgeType const & e1 = *eocit; ++eocit;
        EdgeType const & e2 = *eocit;

        if ( edge_refinement_flag_accessor.get(e0) )
        {
          vertices[3] = edge_to_vertex_handle_accessor.get(e0);
          offset = 0;
        }
        else if ( edge_refinement_flag_accessor.get(e1) )
        {
          vertices[3] = edge_to_vertex_handle_accessor.get(e1);
          offset = 2;
        }
        else if ( edge_refinement_flag_accessor.get(e2) )
        {
          vertices[3] = edge_to_vertex_handle_accessor.get(e2);
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
        add_refinement_element( elements_vertices, vertices, (offset + 0) % num_vertices, 3, (offset + 2) % num_vertices );

        //3-1-2:
        add_refinement_element( elements_vertices, vertices, 3, (offset + 1) % num_vertices, (offset + 2) % num_vertices );
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
      template<typename ElementType, typename MeshT,
               typename ElementsVerticesHandleContainerT,
               typename VertexCopyMapT,
               typename EdgeRefinementFlagAccessor, typename EdgeToVertexHandleAccessor>
      static void apply2(ElementType const & element_in, MeshT const & mesh,
                         ElementsVerticesHandleContainerT & elements_vertices,
                         VertexCopyMapT & vertex_copy_map_,
                         EdgeRefinementFlagAccessor const & edge_refinement_flag_accessor, EdgeToVertexHandleAccessor const & edge_to_vertex_handle_accessor)
      {
        typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type            VertexOnCellRange;
        typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
        typedef typename viennagrid::result_of::const_element_range<ElementType, 1>::type            EdgeOnCellRange;
        typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;

        typedef typename viennagrid::result_of::element<MeshT>::type             VertexType;
        typedef typename viennagrid::result_of::element<MeshT>::type             EdgeType;

        const unsigned int num_vertices = triangle_tag().vertex_count();
        std::vector<VertexType> vertices(num_vertices+2);


        //
        // Step 1: Get vertices on the new mesh
        //

        //grab existing vertices:
        VertexOnCellRange vertices_on_cell(element_in);
        VertexOnCellIterator vocit = vertices_on_cell.begin();
        vertices[0] = vertex_copy_map_(*vocit); ++vocit;
        vertices[1] = vertex_copy_map_(*vocit); ++vocit;
        vertices[2] = vertex_copy_map_(*vocit);

        //Find rotation offset such that first two edges are to be refined
        EdgeOnCellRange edges_on_cell(element_in);
        std::size_t offset = 0;

        EdgeOnCellIterator eocit = edges_on_cell.begin();
        EdgeType const & e0 = *eocit; ++eocit;
        EdgeType const & e1 = *eocit; ++eocit;
        EdgeType const & e2 = *eocit;

        if ( edge_refinement_flag_accessor.get(e0) && edge_refinement_flag_accessor.get(e1) )
        {
          vertices[3] = edge_to_vertex_handle_accessor.get(e1);
          vertices[4] = edge_to_vertex_handle_accessor.get(e0);
          offset = 2;
        }
        else if ( edge_refinement_flag_accessor.get(e0) && edge_refinement_flag_accessor.get(e2) )
        {
          vertices[3] = edge_to_vertex_handle_accessor.get(e0);
          vertices[4] = edge_to_vertex_handle_accessor.get(e2);
          offset = 0;
        }
        else if ( edge_refinement_flag_accessor.get(e1) && edge_refinement_flag_accessor.get(e2) )
        {
          vertices[3] = edge_to_vertex_handle_accessor.get(e2);
          vertices[4] = edge_to_vertex_handle_accessor.get(e1);
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
        add_refinement_element( elements_vertices, vertices, 3, (offset + 1) % num_vertices, 4 );

        //split second-longest edge
        VertexType v0 = vertices[(offset + 0) % num_vertices];
        VertexType v1 = vertices[(offset + 1) % num_vertices];
        VertexType v2 = vertices[(offset + 2) % num_vertices];
        double len_edge1 = viennagrid::norm( viennagrid::get_point(mesh, v1) - viennagrid::get_point(mesh, v0) );
        double len_edge2 = viennagrid::norm( viennagrid::get_point(mesh, v2) - viennagrid::get_point(mesh, v1) );

        if (len_edge1 > len_edge2) //split edge [v0, v1] again
        {
          //0-3-2:
          add_refinement_element( elements_vertices, vertices, (offset + 0) % num_vertices, 3, (offset + 2) % num_vertices );

          //2-3-4:
          add_refinement_element( elements_vertices, vertices, (offset + 2) % num_vertices, 3, 4 );
        }
        else //split edge [v1, v2]
        {
          //0-3-4:
          add_refinement_element( elements_vertices, vertices, (offset + 0) % num_vertices, 3, 4 );

          //0-4-2:
          add_refinement_element( elements_vertices, vertices, (offset + 0) % num_vertices, 4, (offset + 2) % num_vertices );
        }


      } //apply2()




      /** @brief Refinement of a triangle with three edges to be refined (uniform refinement) */
      template<typename ElementType, typename MeshT,
               typename ElementsVerticesHandleContainerT,
               typename VertexCopyMapT,
               typename EdgeRefinementFlagAccessor, typename EdgeToVertexHandleAccessor>
      static void apply3(ElementType const & element_in, MeshT const &,
                         ElementsVerticesHandleContainerT & elements_vertices,
                         VertexCopyMapT & vertex_copy_map_,
                         EdgeRefinementFlagAccessor const &, EdgeToVertexHandleAccessor const & edge_to_vertex_handle_accessor)
      {
        typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type            VertexOnCellRange;
        typedef typename viennagrid::result_of::iterator<VertexOnCellRange>::type         VertexOnCellIterator;
        typedef typename viennagrid::result_of::const_element_range<ElementType, 1>::type            EdgeOnCellRange;
        typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type           EdgeOnCellIterator;

        typedef typename viennagrid::result_of::element<MeshT>::type             VertexType;

        const unsigned int num_vertices = triangle_tag().vertex_count();
        const unsigned int num_lines = triangle_tag().boundary_element_count( line_tag() );
        std::vector<VertexType> vertices( num_vertices+num_lines );

        //
        // Step 1: Get vertices on the new mesh
        //

        //grab existing vertices:
        VertexOnCellRange vertices_on_cell(element_in);
        VertexOnCellIterator vocit = vertices_on_cell.begin();
        vertices[0] = vertex_copy_map_(*vocit); ++vocit;
        vertices[1] = vertex_copy_map_(*vocit); ++vocit;
        vertices[2] = vertex_copy_map_(*vocit);

        //add vertices from edge
        EdgeOnCellRange edges_on_cell(element_in);
        EdgeOnCellIterator eocit = edges_on_cell.begin();
        vertices[3] = edge_to_vertex_handle_accessor.get(*eocit); ++eocit;
        vertices[4] = edge_to_vertex_handle_accessor.get(*eocit); ++eocit;
        vertices[5] = edge_to_vertex_handle_accessor.get(*eocit);

        //
        // Step 2: Add new cells to new mesh:
        //

        //0-3-4:
        add_refinement_element( elements_vertices, vertices, 0, 3, 4 );

        //3-1-5:
        add_refinement_element( elements_vertices, vertices, 3, 1, 5 );

        //4-5-2:
        add_refinement_element( elements_vertices, vertices, 4, 5, 2 );

        //4-3-5:
        add_refinement_element( elements_vertices, vertices, 4, 3, 5 );

      } //apply3()


      /** @brief Public entry function for the refinement of a triangle.
       *
       * @param element_in                        The triangle to be refined
       * @param mesh                              The mesh of which element_in is part of
       * @param elements_vertices                 The mesh or segment the refined triangles are written to
       * @param edge_refinement_flag_accessor     Accessor storing flags if an edge is marked for refinement
       * @param vertex_copy_map_                  Temporary accessor for vertex to vertex mapping
       * @param edge_to_vertex_handle_accessor    Temporary accessor for refined edge to vertex mapping
       */
      template<typename ElementT, typename MeshT,
               typename ElementsVerticesHandleContainerT, typename VertexCopyMapT,
               typename EdgeRefinementFlagAccessor, typename EdgeToVertexHandleAccessor>
      static void apply(ElementT const & element_in, MeshT const & mesh,
                        ElementsVerticesHandleContainerT & elements_vertices,
                        VertexCopyMapT & vertex_copy_map_,
                        EdgeRefinementFlagAccessor const & edge_refinement_flag_accessor,
                        EdgeToVertexHandleAccessor const & edge_to_vertex_handle_accessor)
      {
        typedef typename viennagrid::result_of::const_element_range<ElementT, 1>::type            EdgeOnCellRange;
        typedef typename viennagrid::result_of::iterator<EdgeOnCellRange>::type                 EdgeOnCellIterator;

        std::size_t edges_to_refine = 0;
        EdgeOnCellRange edges_on_cell(element_in);
        for (EdgeOnCellIterator eocit = edges_on_cell.begin();
                                eocit != edges_on_cell.end();
                              ++eocit)
        {
          if ( edge_refinement_flag_accessor.get(*eocit) )
            ++edges_to_refine;
        }

        switch (edges_to_refine)
        {
          case 0: apply0(element_in, mesh, elements_vertices, vertex_copy_map_, edge_refinement_flag_accessor,  edge_to_vertex_handle_accessor); break;
          case 1: apply1(element_in, mesh, elements_vertices, vertex_copy_map_, edge_refinement_flag_accessor, edge_to_vertex_handle_accessor); break;
          case 2: apply2(element_in, mesh, elements_vertices, vertex_copy_map_, edge_refinement_flag_accessor, edge_to_vertex_handle_accessor); break;
          case 3: apply3(element_in, mesh, elements_vertices, vertex_copy_map_, edge_refinement_flag_accessor, edge_to_vertex_handle_accessor); break;
          default: //nothing to do...
                  break;
        }
      } //apply()
    };

  } // namespace detail
}

#endif

