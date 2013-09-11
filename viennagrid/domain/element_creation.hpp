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

#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/segmentation.hpp"
#include "viennagrid/topology/plc.hpp"
#include "viennagrid/algorithm/norm.hpp"

/** @file element_creation.hpp
    @brief Contains functions for creating elements within a domain or segment
*/

namespace viennagrid
{
  // doxygen doku in forwards.hpp
  template<typename ElementTypeOrTagT, typename DomainOrSegmentTypeT, typename VertexHandleIteratorT>
  typename result_of::handle<DomainOrSegmentTypeT, ElementTypeOrTagT>::type make_element(
        DomainOrSegmentTypeT & domain,
        VertexHandleIteratorT vertices_begin,
        VertexHandleIteratorT const & vertices_end)
  {
      typedef typename viennagrid::result_of::element<DomainOrSegmentTypeT, ElementTypeOrTagT>::type ElementType;
      ElementType element = ElementType( inserter(domain).get_physical_container_collection() );

      size_t element_index = 0;
      for ( ; vertices_begin != vertices_end; ++vertices_begin, ++element_index )
          viennagrid::set_vertex( element, *vertices_begin, element_index );

      return push_element<true, true>(domain, element).first;
  }

  // doxygen doku in forwards.hpp
  template<typename ElementTypeOrTagT, typename DomainOrSegmentTypeT, typename VertexHandleIteratorT>
  typename result_of::handle<DomainOrSegmentTypeT, ElementTypeOrTagT>::type make_element_with_id(
        DomainOrSegmentTypeT & domain,
        VertexHandleIteratorT vertices_begin,
        VertexHandleIteratorT const & vertices_end,
        typename result_of::id< typename result_of::element<DomainOrSegmentTypeT, ElementTypeOrTagT>::type >::type id)
  {
      typedef typename viennagrid::result_of::element<DomainOrSegmentTypeT, ElementTypeOrTagT>::type ElementType;
      ElementType element = ElementType( inserter(domain).get_physical_container_collection() );

      element.id( id );

      size_t element_index = 0;
      for ( ; vertices_begin != vertices_end; ++vertices_begin, ++element_index )
          viennagrid::set_vertex( element, *vertices_begin, element_index );

      return push_element<false, true>(domain, element ).first;
  }
  
  
  
  /** @brief Function for creating a cell within a domain or a segment. Function might fail if there is more than 1 cell type in the domain/segment
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the element is created
    * @tparam VertexHandleIteratorT   A vertex handle iterator type
    * @param  domain_segment          The domain or segment object where the element should be created
    * @param  vertices_begin          An iterator pointing to the first vertex handle of the element
    * @param  vertices_end            An iterator defining the end of the vertices
    * @return                         A handle to the created cell
    */
  template<typename DomainOrSegmentTypeT, typename VertexHandleIteratorT>
  typename result_of::cell_handle<DomainOrSegmentTypeT>::type make_cell(
        DomainOrSegmentTypeT & domain_segment,
        VertexHandleIteratorT vertices_begin,
        VertexHandleIteratorT const & vertices_end)
  {
    typedef typename viennagrid::result_of::cell_tag<DomainOrSegmentTypeT>::type CellTagType;
    return make_element<CellTagType>( domain_segment, vertices_begin, vertices_end );
  }
  
  
  /** @brief Function for creating a cell within a domain or a segment with a specific ID. Function might fail if there is more than 1 cell type in the domain/segment
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the element is created
    * @tparam VertexHandleIteratorT   A vertex handle iterator type
    * @param  domain_segment          The domain or segment object where the element should be created
    * @param  vertices_begin          An iterator pointing to the first vertex handle of the element
    * @param  vertices_end            An iterator defining the end of the vertices
    * @param  id                      The id of the new element
    * @return                         A handle to the created cell
    */
  template<typename DomainOrSegmentTypeT, typename VertexHandleIteratorT>
  typename result_of::cell_handle<DomainOrSegmentTypeT>::type make_cell_with_id(
        DomainOrSegmentTypeT & domain_segment,
        VertexHandleIteratorT vertices_begin,
        VertexHandleIteratorT const & vertices_end,
        typename viennagrid::result_of::cell<DomainOrSegmentTypeT>::type::id_type id)
  {
    typedef typename viennagrid::result_of::cell_tag<DomainOrSegmentTypeT>::type CellTagType;
    return make_element_with_id<CellTagType>( domain_segment, vertices_begin, vertices_end, id );
  }






  /** @brief Function for creating a vertex within a domain or a segment
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the vertex is created
    * @param  domain_segment          The domain or segment object where the vertex should be created
    * @return                         A handle to the created vertex
    */
  template<typename DomainOrSegmentTypeT>
  typename result_of::vertex_handle<DomainOrSegmentTypeT>::type make_vertex(DomainOrSegmentTypeT & domain_segment)
  {
      typedef typename result_of::element<DomainOrSegmentTypeT, vertex_tag>::type element_type;
      return push_element<true, true>(domain_segment, element_type() ).first;
  }

  /** @brief Function for creating a vertex within a domain or a segment with a specific point
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the vertex is created
    * @param  domain_segment          The domain or segment object where the vertex should be created
    * @param  point                   The point which corresponds to the created vertex
    * @return                         A handle to the created vertex
    */
  template<typename DomainOrSegmentTypeT>
  typename result_of::vertex_handle<DomainOrSegmentTypeT>::type make_vertex(
        DomainOrSegmentTypeT & domain,
        typename result_of::point<DomainOrSegmentTypeT>::type const & point)
  {
      typename viennagrid::result_of::vertex_handle<DomainOrSegmentTypeT>::type vtx_handle = make_vertex(domain);
      viennagrid::point(domain, vtx_handle) = point;
      return vtx_handle;
  }

  /** @brief Function for creating a vertex within a domain or a segment with a specific point and specific ID
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the vertex is created
    * @param  domain_segment          The domain or segment object where the vertex should be created
    * @param  id                      The id of the new vertex
    * @param  point                   The point which corresponds to the created vertex
    * @return                         A handle to the created vertex
    */
  template<typename DomainOrSegmentTypeT>
  typename result_of::vertex_handle<DomainOrSegmentTypeT>::type make_vertex_with_id(
        DomainOrSegmentTypeT & domain,
        typename viennagrid::result_of::element<DomainOrSegmentTypeT, vertex_tag>::type::id_type id,
        typename result_of::point<DomainOrSegmentTypeT>::type const & point)
  {
      typedef typename result_of::vertex<DomainOrSegmentTypeT>::type VertexType;
      VertexType element;
      element.id( id );

      typename result_of::vertex_handle<DomainOrSegmentTypeT>::type ret = push_element<false, true>(domain, element ).first;
      viennagrid::point(domain, ret) = point;

      return ret;
  }

  /** @brief Function for creating a unique vertex. The uniqueness is checked by using the point of the vertex: if there is any vertex whose point is close to the point currently inserted, this handle is returned. A new vertex is created otherwise. A classical 2-norm and the tolerance is used for distance checking. The runtime of this function is linear in the vertices within the domain.
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the vertex is created
    * @param  domain_segment          The domain or segment object where the vertex should be created
    * @param  point                   The point which corresponds to the created vertex
    * @param  tolerance               The tolerance of the 2-norm for checking if two points are equal
    * @return                         A handle to a vertex which is close enough to point or a handle to a newly created vertex
    */
  template<typename DomainOrSegmentTypeT>
  typename result_of::vertex_handle<DomainOrSegmentTypeT>::type make_unique_vertex(
        DomainOrSegmentTypeT & domain,
        typename result_of::point<DomainOrSegmentTypeT>::type const & point,
        typename result_of::coord<DomainOrSegmentTypeT>::type tolerance)
  {
      typedef typename result_of::element_range<DomainOrSegmentTypeT, vertex_tag>::type vertex_range_type;
      typedef typename result_of::iterator<vertex_range_type>::type vertex_range_iterator;

      vertex_range_type vertices = viennagrid::elements(domain);
      for (vertex_range_iterator hit = vertices.begin(); hit != vertices.end(); ++hit)
      {
          if (viennagrid::norm_2( point - viennagrid::point(domain, *hit) ) < tolerance )
              return hit.handle();
      }

      return make_vertex(domain, point);
  }

  /** @brief Function for creating a unique vertex. The uniqueness is checked by using the point of the vertex: if there is any vertex whose point is close to the point currently inserted, this handle is returned. A new vertex is created otherwise. A classical 2-norm and the 2-norm of points as tolerance is used for distance checking. The runtime of this function is linear in the vertices within the domain.
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the vertex is created
    * @param  domain_segment          The domain or segment object where the vertex should be created
    * @param  point                   The point which corresponds to the created vertex
    * @return                         A handle to a vertex which is close enough to point or a handle to a newly created vertex
    */
  template<typename DomainOrSegmentTypeT>
  typename result_of::vertex_handle<DomainOrSegmentTypeT>::type make_unique_vertex(
        DomainOrSegmentTypeT & domain,
        typename result_of::point<DomainOrSegmentTypeT>::type const & p)
  {
      return make_unique_vertex( domain, p, viennagrid::norm_2(p) / 1e6 );
  }


  /** @brief Function for creating a line within a domain or a segment (same as make_edge)
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the line is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  domain_segment          The domain or segment object where the line should be created
    * @param  v0                      A handle to the first vertex of the line
    * @param  v1                      A handle to the second vertex of the line
    * @return                         A handle to the created line
    */
  template<typename DomainOrSegmentTypeT, typename VertexHandleT>
  typename result_of::line_handle<DomainOrSegmentTypeT>::type make_line(
        DomainOrSegmentTypeT & domain,
        VertexHandleT v0, VertexHandleT v1)
  {
      viennagrid::storage::static_array<VertexHandleT, 2> handles;
      handles[0] = v0;
      handles[1] = v1;

      return make_element<viennagrid::line_tag>( domain, handles.begin(), handles.end() );
  }
  
  /** @brief Function for creating an edge within a domain or a segment (same as make_edge)
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the edge is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  domain_segment          The domain or segment object where the edge should be created
    * @param  v0                      A handle to the first vertex of the edge
    * @param  v1                      A handle to the second vertex of the edge
    * @return                         A handle to the created edge
    */
  template<typename DomainOrSegmentTypeT, typename VertexHandleT>
  typename result_of::edge_handle<DomainOrSegmentTypeT>::type make_edge(
        DomainOrSegmentTypeT & domain,
        VertexHandleT v0, VertexHandleT v1)
  {
      viennagrid::storage::static_array<VertexHandleT, 2> handles;
      handles[0] = v0;
      handles[1] = v1;

      return make_element<viennagrid::edge_tag>( domain, handles.begin(), handles.end() );
  }

  /** @brief Function for creating a triangle within a domain or a segment
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the triangle is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  domain_segment          The domain or segment object where the triangle should be created
    * @param  v0                      A handle to the first vertex of the triangle
    * @param  v1                      A handle to the second vertex of the triangle
    * @param  v2                      A handle to the third vertex of the triangle
    * @return                         A handle to the created triangle
    */
  template<typename DomainOrSegmentTypeT, typename VertexHandleT>
  typename result_of::triangle_handle<DomainOrSegmentTypeT>::type make_triangle(
        DomainOrSegmentTypeT & domain,
        VertexHandleT v0, VertexHandleT v1, VertexHandleT v2)
  {
      viennagrid::storage::static_array<VertexHandleT, 3> handles;
      handles[0] = v0;
      handles[1] = v1;
      handles[2] = v2;

      return make_element<viennagrid::triangle_tag>( domain, handles.begin(), handles.end() );
  }
  
  /** @brief Function for creating a quadrilateral within a domain or a segment
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the quadrilateral is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  domain_segment          The domain or segment object where the quadrilateral should be created
    * @param  v0                      A handle to the first vertex of the quadrilateral
    * @param  v1                      A handle to the second vertex of the quadrilateral
    * @param  v2                      A handle to the third vertex of the quadrilateral
    * @param  v3                      A handle to the forth vertex of the quadrilateral
    * @return                         A handle to the created quadrilateral
    */
  template<typename DomainOrSegmentTypeT, typename VertexHandleT>
  typename result_of::quadrilateral_handle<DomainOrSegmentTypeT>::type make_quadrilateral(
        DomainOrSegmentTypeT & domain,
        VertexHandleT v0, VertexHandleT v1, VertexHandleT v2, VertexHandleT v3)
  {
      viennagrid::storage::static_array<VertexHandleT, 4> handles;
      handles[0] = v0;
      handles[1] = v1;
      handles[2] = v2;
      handles[3] = v3;

      return make_element<viennagrid::quadrilateral_tag>( domain, handles.begin(), handles.end() );
  }


  /** @brief Function for creating a PLC within a domain or a segment
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the PLC is created
    * @tparam LineHandleIteratorT     A line handle iterator type
    * @tparam VertexHandleIteratorT   A vertex handle iterator type
    * @tparam PointIteratorT          A point handle iterator type
    * @param  domain_segment          The domain or segment object where the PLC should be created
    * @param  lines_begin             An iterator pointing to the first line handle of the PLC
    * @param  lines_end               An iterator defining the end of the lines
    * @param  loose_vertices_begin    An iterator pointing to the first loose vertex handle of the PLC
    * @param  loose_vertices_end      An iterator defining the end of the loose vertices
    * @param  hole_points_begin       An iterator pointing to the first hole point of the PLC
    * @param  hole_points_end         An iterator defining the end of the hole points
    * @return                         A handle to the created PLC
    */
  template<typename DomainOrSegmentTypeT, typename LineHandleIteratorT, typename VertexHandleIteratorT, typename PointIteratorT>
  typename result_of::plc_handle<DomainOrSegmentTypeT>::type make_plc(
        DomainOrSegmentTypeT & domain,
        LineHandleIteratorT    lines_begin,           LineHandleIteratorT     lines_end,
        VertexHandleIteratorT  loose_vertices_begin,  VertexHandleIteratorT   loose_vertices_end,
        PointIteratorT         hole_points_begin,     PointIteratorT          hole_points_end)
  {
      typedef typename viennagrid::result_of::element<DomainOrSegmentTypeT, plc_tag>::type PLCType;;
      typedef typename result_of::handle<DomainOrSegmentTypeT, plc_tag>::type PLCHandleType;
      PLCType plc( inserter(domain).get_physical_container_collection() );

      for ( ; lines_begin != lines_end; ++lines_begin)
        plc.container( viennagrid::line_tag() ).insert_unique_handle( *lines_begin );

      for ( ; loose_vertices_begin != loose_vertices_end; ++loose_vertices_begin)
        plc.container( viennagrid::vertex_tag() ).insert_unique_handle( *loose_vertices_begin );

      PLCHandleType handle = viennagrid::push_element<true, true>(domain, plc ).first;

      PLCType & inserted_plc = viennagrid::dereference_handle(domain, handle);

      std::copy(hole_points_begin, hole_points_end, std::back_inserter( inserted_plc.appendix() ) );
      return handle;
  }






  /** @brief Function for creating a tetrahedron within a domain or a segment
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the tetrahedron is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  domain_segment          The domain or segment object where the tetrahedron should be created
    * @param  v0                      A handle to the first vertex of the tetrahedron
    * @param  v1                      A handle to the second vertex of the tetrahedron
    * @param  v2                      A handle to the third vertex of the tetrahedron
    * @param  v3                      A handle to the forth vertex of the tetrahedron
    * @return                         A handle to the created tetrahedron
    */
  template<typename DomainOrSegmentTypeT, typename VertexHandleT>
  typename result_of::tetrahedron_handle<DomainOrSegmentTypeT>::type make_tetrahedron(
        DomainOrSegmentTypeT & domain,
        VertexHandleT v0, VertexHandleT v1, VertexHandleT v2, VertexHandleT v3)
  {
      viennagrid::storage::static_array<VertexHandleT, 4> handles;
      handles[0] = v0;
      handles[1] = v1;
      handles[2] = v2;
      handles[3] = v3;

      return make_element<viennagrid::tetrahedron_tag>( domain, handles.begin(), handles.end() );
  }


  /** @brief Function for creating a hexahedron within a domain or a segment
    *
    * @tparam DomainOrSegmentT        The domain or segment type where the hexahedron is created
    * @tparam VertexHandleT           The vertex handle type
    * @param  domain_segment          The domain or segment object where the hexahedron should be created
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
  template<typename DomainOrSegmentTypeT, typename VertexHandleT>
  typename result_of::hexahedron_handle<DomainOrSegmentTypeT>::type make_hexahedron(
        DomainOrSegmentTypeT & domain_segment,
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

      return make_element<viennagrid::hexahedron_tag>( domain_segment, handles.begin(), handles.end() );
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
