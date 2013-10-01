#ifndef VIENNAGRID_ELEMENT_CREATION_HPP
#define VIENNAGRID_ELEMENT_CREATION_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/mesh/mesh.hpp"
#include "viennagrid/mesh/segmentation.hpp"
#include "viennagrid/topology/plc.hpp"
#include "viennagrid/algorithm/norm.hpp"

/** @file element_creation.hpp
    @brief Contains functions for creating elements within a mesh or segment
*/

namespace viennagrid
{
  // doxygen doku in forwards.hpp
  template<typename ElementTypeOrTagT, typename MeshOrSegmentTypeT, typename VertexHandleIteratorT>
  typename result_of::handle<MeshOrSegmentTypeT, ElementTypeOrTagT>::type make_element(
        MeshOrSegmentTypeT & mesh,
        VertexHandleIteratorT vertices_begin,
        VertexHandleIteratorT const & vertices_end)
  {
      typedef typename viennagrid::result_of::element<MeshOrSegmentTypeT, ElementTypeOrTagT>::type ElementType;
      ElementType element = ElementType( inserter(mesh).get_physical_container_collection() );

      size_t element_index = 0;
      for ( ; vertices_begin != vertices_end; ++vertices_begin, ++element_index )
          viennagrid::set_vertex( element, *vertices_begin, element_index );

      return push_element<true, true>(mesh, element).first;
  }

  // doxygen doku in forwards.hpp
  template<typename ElementTypeOrTagT, typename MeshOrSegmentTypeT, typename VertexHandleIteratorT>
  typename result_of::handle<MeshOrSegmentTypeT, ElementTypeOrTagT>::type make_element_with_id(
        MeshOrSegmentTypeT & mesh,
        VertexHandleIteratorT vertices_begin,
        VertexHandleIteratorT const & vertices_end,
        typename result_of::id< typename result_of::element<MeshOrSegmentTypeT, ElementTypeOrTagT>::type >::type id)
  {
      typedef typename viennagrid::result_of::element<MeshOrSegmentTypeT, ElementTypeOrTagT>::type ElementType;
      ElementType element = ElementType( inserter(mesh).get_physical_container_collection() );

      element.id( id );

      size_t element_index = 0;
      for ( ; vertices_begin != vertices_end; ++vertices_begin, ++element_index )
          viennagrid::set_vertex( element, *vertices_begin, element_index );

      return push_element<false, true>(mesh, element ).first;
  }
  
  
  
  /** @brief Function for creating a cell within a mesh or a segment. Function might fail if there is more than 1 cell type in the mesh/segment
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the element is created
    * @tparam VertexHandleIteratorT   A vertex handle iterator type
    * @param  mesh_segment          The mesh or segment object where the element should be created
    * @param  vertices_begin          An iterator pointing to the first vertex handle of the element
    * @param  vertices_end            An iterator defining the end of the vertices
    * @return                         A handle to the created cell
    */
  template<typename MeshOrSegmentTypeT, typename VertexHandleIteratorT>
  typename result_of::cell_handle<MeshOrSegmentTypeT>::type make_cell(
        MeshOrSegmentTypeT & mesh_segment,
        VertexHandleIteratorT vertices_begin,
        VertexHandleIteratorT const & vertices_end)
  {
    typedef typename viennagrid::result_of::cell_tag<MeshOrSegmentTypeT>::type CellTagType;
    return make_element<CellTagType>( mesh_segment, vertices_begin, vertices_end );
  }
  
  
  /** @brief Function for creating a cell within a mesh or a segment with a specific ID. Function might fail if there is more than 1 cell type in the mesh/segment
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the element is created
    * @tparam VertexHandleIteratorT   A vertex handle iterator type
    * @param  mesh_segment          The mesh or segment object where the element should be created
    * @param  vertices_begin          An iterator pointing to the first vertex handle of the element
    * @param  vertices_end            An iterator defining the end of the vertices
    * @param  id                      The id of the new element
    * @return                         A handle to the created cell
    */
  template<typename MeshOrSegmentTypeT, typename VertexHandleIteratorT>
  typename result_of::cell_handle<MeshOrSegmentTypeT>::type make_cell_with_id(
        MeshOrSegmentTypeT & mesh_segment,
        VertexHandleIteratorT vertices_begin,
        VertexHandleIteratorT const & vertices_end,
        typename viennagrid::result_of::cell<MeshOrSegmentTypeT>::type::id_type id)
  {
    typedef typename viennagrid::result_of::cell_tag<MeshOrSegmentTypeT>::type CellTagType;
    return make_element_with_id<CellTagType>( mesh_segment, vertices_begin, vertices_end, id );
  }






  /** @brief Function for creating a vertex within a mesh or a segment
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the vertex is created
    * @param  mesh_segment          The mesh or segment object where the vertex should be created
    * @return                         A handle to the created vertex
    */
  template<typename MeshOrSegmentTypeT>
  typename result_of::vertex_handle<MeshOrSegmentTypeT>::type make_vertex(MeshOrSegmentTypeT & mesh_segment)
  {
      typedef typename result_of::element<MeshOrSegmentTypeT, vertex_tag>::type element_type;
      return push_element<true, true>(mesh_segment, element_type() ).first;
  }

  /** @brief Function for creating a vertex within a mesh or a segment with a specific point
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the vertex is created
    * @param  mesh_segment          The mesh or segment object where the vertex should be created
    * @param  point                   The point which corresponds to the created vertex
    * @return                         A handle to the created vertex
    */
  template<typename MeshOrSegmentTypeT>
  typename result_of::vertex_handle<MeshOrSegmentTypeT>::type make_vertex(
        MeshOrSegmentTypeT & mesh,
        typename result_of::point<MeshOrSegmentTypeT>::type const & point)
  {
      typename viennagrid::result_of::vertex_handle<MeshOrSegmentTypeT>::type vtx_handle = make_vertex(mesh);
      viennagrid::point(mesh, vtx_handle) = point;
      return vtx_handle;
  }

  /** @brief Function for creating a vertex within a mesh or a segment with a specific point and specific ID
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the vertex is created
    * @param  mesh_segment          The mesh or segment object where the vertex should be created
    * @param  id                      The id of the new vertex
    * @param  point                   The point which corresponds to the created vertex
    * @return                         A handle to the created vertex
    */
  template<typename MeshOrSegmentTypeT>
  typename result_of::vertex_handle<MeshOrSegmentTypeT>::type make_vertex_with_id(
        MeshOrSegmentTypeT & mesh,
        typename viennagrid::result_of::element<MeshOrSegmentTypeT, vertex_tag>::type::id_type id,
        typename result_of::point<MeshOrSegmentTypeT>::type const & point)
  {
      typedef typename result_of::vertex<MeshOrSegmentTypeT>::type VertexType;
      VertexType element;
      element.id( id );

      typename result_of::vertex_handle<MeshOrSegmentTypeT>::type ret = push_element<false, true>(mesh, element ).first;
      viennagrid::point(mesh, ret) = point;

      return ret;
  }

  /** @brief Function for creating a unique vertex. The uniqueness is checked by using the point of the vertex: if there is any vertex whose point is close to the point currently inserted, this handle is returned. A new vertex is created otherwise. A classical 2-norm and the tolerance is used for distance checking. The runtime of this function is linear in the vertices within the mesh.
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the vertex is created
    * @param  mesh_segment          The mesh or segment object where the vertex should be created
    * @param  point                   The point which corresponds to the created vertex
    * @param  tolerance               The tolerance of the 2-norm for checking if two points are equal
    * @return                         A handle to a vertex which is close enough to point or a handle to a newly created vertex
    */
  template<typename MeshOrSegmentTypeT>
  typename result_of::vertex_handle<MeshOrSegmentTypeT>::type make_unique_vertex(
        MeshOrSegmentTypeT & mesh,
        typename result_of::point<MeshOrSegmentTypeT>::type const & point,
        typename result_of::coord<MeshOrSegmentTypeT>::type tolerance)
  {
      typedef typename result_of::element_range<MeshOrSegmentTypeT, vertex_tag>::type vertex_range_type;
      typedef typename result_of::iterator<vertex_range_type>::type vertex_range_iterator;

      vertex_range_type vertices = viennagrid::elements(mesh);
      for (vertex_range_iterator hit = vertices.begin(); hit != vertices.end(); ++hit)
      {
          if (viennagrid::norm_2( point - viennagrid::point(mesh, *hit) ) < tolerance )
              return hit.handle();
      }

      return make_vertex(mesh, point);
  }

  /** @brief Function for creating a unique vertex. The uniqueness is checked by using the point of the vertex: if there is any vertex whose point is close to the point currently inserted, this handle is returned. A new vertex is created otherwise. A classical 2-norm and the 2-norm of points as tolerance is used for distance checking. The runtime of this function is linear in the vertices within the mesh.
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the vertex is created
    * @param  mesh_segment          The mesh or segment object where the vertex should be created
    * @param  point                   The point which corresponds to the created vertex
    * @return                         A handle to a vertex which is close enough to point or a handle to a newly created vertex
    */
  template<typename MeshOrSegmentTypeT>
  typename result_of::vertex_handle<MeshOrSegmentTypeT>::type make_unique_vertex(
        MeshOrSegmentTypeT & mesh,
        typename result_of::point<MeshOrSegmentTypeT>::type const & p)
  {
      return make_unique_vertex( mesh, p, viennagrid::norm_2(p) / 1e6 );
  }


  /** @brief Function for creating a line within a mesh or a segment (same as make_edge)
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the line is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  mesh_segment          The mesh or segment object where the line should be created
    * @param  v0                      A handle to the first vertex of the line
    * @param  v1                      A handle to the second vertex of the line
    * @return                         A handle to the created line
    */
  template<typename MeshOrSegmentTypeT, typename VertexHandleT>
  typename result_of::line_handle<MeshOrSegmentTypeT>::type make_line(
        MeshOrSegmentTypeT & mesh,
        VertexHandleT v0, VertexHandleT v1)
  {
      viennagrid::storage::static_array<VertexHandleT, 2> handles;
      handles[0] = v0;
      handles[1] = v1;

      return make_element<viennagrid::line_tag>( mesh, handles.begin(), handles.end() );
  }
  
  /** @brief Function for creating an edge within a mesh or a segment (same as make_edge)
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the edge is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  mesh_segment          The mesh or segment object where the edge should be created
    * @param  v0                      A handle to the first vertex of the edge
    * @param  v1                      A handle to the second vertex of the edge
    * @return                         A handle to the created edge
    */
  template<typename MeshOrSegmentTypeT, typename VertexHandleT>
  typename result_of::edge_handle<MeshOrSegmentTypeT>::type make_edge(
        MeshOrSegmentTypeT & mesh,
        VertexHandleT v0, VertexHandleT v1)
  {
      viennagrid::storage::static_array<VertexHandleT, 2> handles;
      handles[0] = v0;
      handles[1] = v1;

      return make_element<viennagrid::edge_tag>( mesh, handles.begin(), handles.end() );
  }

  /** @brief Function for creating a triangle within a mesh or a segment
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the triangle is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  mesh_segment          The mesh or segment object where the triangle should be created
    * @param  v0                      A handle to the first vertex of the triangle
    * @param  v1                      A handle to the second vertex of the triangle
    * @param  v2                      A handle to the third vertex of the triangle
    * @return                         A handle to the created triangle
    */
  template<typename MeshOrSegmentTypeT, typename VertexHandleT>
  typename result_of::triangle_handle<MeshOrSegmentTypeT>::type make_triangle(
        MeshOrSegmentTypeT & mesh,
        VertexHandleT v0, VertexHandleT v1, VertexHandleT v2)
  {
      viennagrid::storage::static_array<VertexHandleT, 3> handles;
      handles[0] = v0;
      handles[1] = v1;
      handles[2] = v2;

      return make_element<viennagrid::triangle_tag>( mesh, handles.begin(), handles.end() );
  }
  
  /** @brief Function for creating a quadrilateral within a mesh or a segment
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the quadrilateral is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  mesh_segment          The mesh or segment object where the quadrilateral should be created
    * @param  v0                      A handle to the first vertex of the quadrilateral
    * @param  v1                      A handle to the second vertex of the quadrilateral
    * @param  v2                      A handle to the third vertex of the quadrilateral
    * @param  v3                      A handle to the forth vertex of the quadrilateral
    * @return                         A handle to the created quadrilateral
    */
  template<typename MeshOrSegmentTypeT, typename VertexHandleT>
  typename result_of::quadrilateral_handle<MeshOrSegmentTypeT>::type make_quadrilateral(
        MeshOrSegmentTypeT & mesh,
        VertexHandleT v0, VertexHandleT v1, VertexHandleT v2, VertexHandleT v3)
  {
      viennagrid::storage::static_array<VertexHandleT, 4> handles;
      handles[0] = v0;
      handles[1] = v1;
      handles[2] = v2;
      handles[3] = v3;

      return make_element<viennagrid::quadrilateral_tag>( mesh, handles.begin(), handles.end() );
  }


  /** @brief Function for creating a PLC within a mesh or a segment
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the PLC is created
    * @tparam LineHandleIteratorT     A line handle iterator type
    * @tparam VertexHandleIteratorT   A vertex handle iterator type
    * @tparam PointIteratorT          A point handle iterator type
    * @param  mesh_segment          The mesh or segment object where the PLC should be created
    * @param  lines_begin             An iterator pointing to the first line handle of the PLC
    * @param  lines_end               An iterator defining the end of the lines
    * @param  loose_vertices_begin    An iterator pointing to the first loose vertex handle of the PLC
    * @param  loose_vertices_end      An iterator defining the end of the loose vertices
    * @param  hole_points_begin       An iterator pointing to the first hole point of the PLC
    * @param  hole_points_end         An iterator defining the end of the hole points
    * @return                         A handle to the created PLC
    */
  template<typename MeshOrSegmentTypeT, typename LineHandleIteratorT, typename VertexHandleIteratorT, typename PointIteratorT>
  typename result_of::plc_handle<MeshOrSegmentTypeT>::type make_plc(
        MeshOrSegmentTypeT & mesh,
        LineHandleIteratorT    lines_begin,           LineHandleIteratorT     lines_end,
        VertexHandleIteratorT  loose_vertices_begin,  VertexHandleIteratorT   loose_vertices_end,
        PointIteratorT         hole_points_begin,     PointIteratorT          hole_points_end)
  {
      typedef typename viennagrid::result_of::element<MeshOrSegmentTypeT, plc_tag>::type PLCType;;
      typedef typename result_of::handle<MeshOrSegmentTypeT, plc_tag>::type PLCHandleType;
      PLCType plc( inserter(mesh).get_physical_container_collection() );

      for ( ; lines_begin != lines_end; ++lines_begin)
        plc.container( viennagrid::line_tag() ).insert_unique_handle( *lines_begin );

      for ( ; loose_vertices_begin != loose_vertices_end; ++loose_vertices_begin)
        plc.container( viennagrid::vertex_tag() ).insert_unique_handle( *loose_vertices_begin );

      PLCHandleType handle = viennagrid::push_element<true, true>(mesh, plc ).first;

      PLCType & inserted_plc = viennagrid::dereference_handle(mesh, handle);

      std::copy(hole_points_begin, hole_points_end, std::back_inserter( inserted_plc.appendix() ) );
      return handle;
  }






  /** @brief Function for creating a tetrahedron within a mesh or a segment
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the tetrahedron is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  mesh_segment          The mesh or segment object where the tetrahedron should be created
    * @param  v0                      A handle to the first vertex of the tetrahedron
    * @param  v1                      A handle to the second vertex of the tetrahedron
    * @param  v2                      A handle to the third vertex of the tetrahedron
    * @param  v3                      A handle to the forth vertex of the tetrahedron
    * @return                         A handle to the created tetrahedron
    */
  template<typename MeshOrSegmentTypeT, typename VertexHandleT>
  typename result_of::tetrahedron_handle<MeshOrSegmentTypeT>::type make_tetrahedron(
        MeshOrSegmentTypeT & mesh,
        VertexHandleT v0, VertexHandleT v1, VertexHandleT v2, VertexHandleT v3)
  {
      viennagrid::storage::static_array<VertexHandleT, 4> handles;
      handles[0] = v0;
      handles[1] = v1;
      handles[2] = v2;
      handles[3] = v3;

      return make_element<viennagrid::tetrahedron_tag>( mesh, handles.begin(), handles.end() );
  }


  /** @brief Function for creating a hexahedron within a mesh or a segment
    *
    * @tparam MeshOrSegmentT        The mesh or segment type where the hexahedron is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  mesh_segment          The mesh or segment object where the hexahedron should be created
    * @param  v0                      A handle to the first vertex of the hexahedron
    * @param  v1                      A handle to the second vertex of the hexahedron
    * @param  v2                      A handle to the third vertex of the hexahedron
    * @param  v3                      A handle to the forth vertex of the hexahedron
    * @param  v4                      A handle to the fifth vertex of the hexahedron
    * @param  v5                      A handle to the sixth vertex of the hexahedron
    * @param  v6                      A handle to the seventh vertex of the hexahedron
    * @param  v7                      A handle to the eighth vertex of the hexahedron
    * @return                         A handle to the created hexahedron
    */
  template<typename MeshOrSegmentTypeT, typename VertexHandleT>
  typename result_of::hexahedron_handle<MeshOrSegmentTypeT>::type make_hexahedron(
        MeshOrSegmentTypeT & mesh,
        VertexHandleT v0, VertexHandleT v1, VertexHandleT v2, VertexHandleT v3,
        VertexHandleT v4, VertexHandleT v5, VertexHandleT v6, VertexHandleT v7)
  {
      viennagrid::storage::static_array<VertexHandleT, 8> handles;
      handles[0] = v0;
      handles[1] = v1;
      handles[2] = v2;
      handles[3] = v3;
      handles[4] = v4;
      handles[5] = v5;
      handles[6] = v6;
      handles[7] = v7;

      return make_element<viennagrid::hexahedron_tag>( mesh, handles.begin(), handles.end() );
  }

  
  /** @brief Function for copying an element to a domain or segment
    *
    * @tparam ElementT                The element type which is copied
    * @tparam DomainOrSegmentT        The domain or segment type where the hexahedron is created
    * @param  element                 The element which is copied
    * @param  domain_segment          The domain or segment object where the element is copied to
    * @return                         A handle to the created hexahedron
    */
  template<typename ElementT, typename DomainOrSegmentT>
  typename viennagrid::result_of::handle<
      DomainOrSegmentT,
      typename viennagrid::result_of::element_tag<ElementT>::type
    >::type copy_element( ElementT const & element, DomainOrSegmentT & domain_segment )
  {
    typedef typename viennagrid::result_of::element_tag<ElementT>::type             ElementTag;
    typedef typename viennagrid::result_of::vertex_handle<DomainOrSegmentT>::type   VertexHandleType;
    std::vector<VertexHandleType> vertex_handles;
    
    typedef typename viennagrid::result_of::const_vertex_range<ElementT>::type      VertexRangeType;
    typedef typename viennagrid::result_of::iterator<VertexRangeType>::type         VertexRangeIterator;
    
    VertexRangeType vertices = viennagrid::elements(element);
    for (VertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
      vertex_handles.push_back( make_unique_vertex(domain_segment, viennagrid::point(*vit)) );
    
    return make_element<ElementTag>( domain_segment, vertex_handles.begin(), vertex_handles.end() );
  }
  
}



#endif
