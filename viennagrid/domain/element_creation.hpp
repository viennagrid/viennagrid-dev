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

namespace viennagrid
{
    template<typename ElementTypeOrTag, typename DomainType, typename VertexHandleIteratorT>
    typename result_of::handle<DomainType, ElementTypeOrTag>::type make_element(
          DomainType & domain,
          VertexHandleIteratorT vertices_begin,
          VertexHandleIteratorT const & vertices_end )
    {
        typedef typename viennagrid::result_of::element<DomainType, ElementTypeOrTag>::type ElementType;
        ElementType element = ElementType( inserter(domain).get_physical_container_collection() );

        size_t element_index = 0;
        for ( ; vertices_begin != vertices_end; ++vertices_begin, ++element_index )
            viennagrid::set_vertex( element, *vertices_begin, element_index );

        return push_element<true, true>(domain, element).first;
    }


    template<typename ElementTypeOrTag, typename DomainType, typename VertexHandleIteratorT>
    typename result_of::handle<DomainType, ElementTypeOrTag>::type make_element_with_id(
          DomainType & domain,
          VertexHandleIteratorT vertices_begin,
          VertexHandleIteratorT const & vertices_end,
          typename result_of::id< typename result_of::element<DomainType, ElementTypeOrTag>::type >::type id )
    {
        typedef typename viennagrid::result_of::element<DomainType, ElementTypeOrTag>::type ElementType;
        ElementType element = ElementType( inserter(domain).get_physical_container_collection() );

        element.id( id );

        size_t element_index = 0;
        for ( ; vertices_begin != vertices_end; ++vertices_begin, ++element_index )
            viennagrid::set_vertex( element, *vertices_begin, element_index );

        return push_element<false, true>(domain, element ).first;
    }
    
    
    
    template<typename DomainType, typename VertexHandleIteratorT>
    typename result_of::cell_handle<DomainType>::type make_cell(
          DomainType & domain,
          VertexHandleIteratorT vertices_begin,
          VertexHandleIteratorT const & vertices_end )
    {
      typedef typename viennagrid::result_of::cell_tag<DomainType>::type CellTagType;
      return make_element<CellTagType>( domain, vertices_begin, vertices_end );
    }
    
    
    template<typename DomainType, typename VertexHandleIteratorT>
    typename result_of::cell_handle<DomainType>::type make_cell_with_id(
          DomainType & domain,
          VertexHandleIteratorT vertices_begin,
          VertexHandleIteratorT const & vertices_end,
          typename viennagrid::result_of::cell<DomainType>::type::id_type id )
    {
      typedef typename viennagrid::result_of::cell_tag<DomainType>::type CellTagType;
      return make_element_with_id<CellTagType>( domain, vertices_begin, vertices_end, id );
    }







    template<typename DomainType>
    typename result_of::handle<DomainType, vertex_tag>::type make_vertex( DomainType & domain )
    {
        typedef typename result_of::element<DomainType, vertex_tag>::type element_type;
        return push_element<true, true>(domain, element_type() ).first;
    }

    template<typename ConfigType>
    typename viennagrid::result_of::vertex_handle< domain_t<ConfigType> >::type
        make_vertex( domain_t<ConfigType> & domain,
                       typename result_of::point< domain_t<ConfigType> >::type const & point )
    {
        typename viennagrid::result_of::vertex_handle< domain_t<ConfigType> >::type vtx_handle = make_vertex(domain);
        viennagrid::point(domain, vtx_handle) = point;
        return vtx_handle;
    }

    template<typename ConfigType>
    typename viennagrid::result_of::vertex_handle< domain_t<ConfigType> >::type
        make_vertex( domain_t<ConfigType> & domain,
                       typename viennagrid::result_of::element< domain_t<ConfigType>, vertex_tag>::type::id_type id,
                       typename result_of::point< domain_t<ConfigType> >::type const & point )
    {
        typedef typename result_of::element< domain_t<ConfigType>, vertex_tag>::type element_type;
        element_type element;
        element.id( id );

        typename result_of::handle< domain_t<ConfigType>, element_type>::type ret = push_element<false, true>(domain, element ).first;
        viennagrid::point(domain, ret) = point;

        return ret;
    }

    template<typename ConfigType>
    typename result_of::handle<domain_t<ConfigType>, vertex_tag>::type
        make_unique_vertex( domain_t<ConfigType> & domain,
                              typename result_of::point< domain_t<ConfigType> >::type const & p,
                              typename result_of::coord< domain_t<ConfigType> >::type tolerance )
    {
        typedef domain_t<ConfigType> DomainType;
        typedef typename result_of::element_range<DomainType, vertex_tag>::type vertex_range_type;
        typedef typename result_of::iterator<vertex_range_type>::type vertex_range_iterator;

        vertex_range_type vertices = viennagrid::elements<vertex_tag>(domain);
        for (vertex_range_iterator hit = vertices.begin(); hit != vertices.end(); ++hit)
        {
            if (viennagrid::norm_2( p - point(domain, *hit) ) < tolerance )
                return hit.handle();
        }

        return make_vertex(domain, p);
    }

    template<typename ConfigType>
    typename result_of::handle<domain_t<ConfigType>, vertex_tag>::type
        make_unique_vertex( domain_t<ConfigType> & domain,
                              typename result_of::point< domain_t<ConfigType> >::type const & p)
    {
        return make_unique_vertex( domain, p, viennagrid::norm_2(p) / 1e6 );
    }



    template<typename DomainType, typename vertex_handle_type>
    typename result_of::handle<DomainType, line_tag>::type make_line( DomainType & domain, vertex_handle_type v0, vertex_handle_type v1 )
    {
        viennagrid::storage::static_array<vertex_handle_type, 2> handles;
        handles[0] = v0;
        handles[1] = v1;

        return make_element<viennagrid::line_tag>( domain, handles.begin(), handles.end() );
    }

    template<typename DomainType, typename vertex_handle_type>
    typename result_of::handle<DomainType, triangle_tag>::type make_triangle( DomainType & domain, vertex_handle_type v0, vertex_handle_type v1, vertex_handle_type v2 )
    {
        viennagrid::storage::static_array<vertex_handle_type, 3> handles;
        handles[0] = v0;
        handles[1] = v1;
        handles[2] = v2;

        return make_element<viennagrid::triangle_tag>( domain, handles.begin(), handles.end() );
    }


    template<typename DomainType, typename LineHandleIteratorT, typename VertexHandleIteratorT, typename PointIteraorType>
    typename result_of::handle<DomainType, plc_tag>::type make_plc(DomainType & domain,
                                                                   LineHandleIteratorT    lines_begin,        LineHandleIteratorT     lines_end,
                                                                   VertexHandleIteratorT  vertices_begin,     VertexHandleIteratorT   vertices_end,
                                                                   PointIteraorType       hole_points_begin,  PointIteraorType        hole_points_end)
    {
        typedef typename viennagrid::result_of::element<DomainType, plc_tag>::type PLCType;;
        typedef typename result_of::handle<DomainType, plc_tag>::type PLCHandleType;
        PLCType plc( inserter(domain).get_physical_container_collection() );

        for ( ; lines_begin != lines_end; ++lines_begin)
          plc.container( viennagrid::line_tag() ).insert_unique_handle( *lines_begin );

        for ( ; vertices_begin != vertices_end; ++vertices_begin)
          plc.container( viennagrid::vertex_tag() ).insert_unique_handle( *vertices_begin );

        PLCHandleType handle = viennagrid::push_element<true, true>(domain, plc ).first;

        PLCType & inserted_plc = viennagrid::dereference_handle(domain, handle);

        std::copy(hole_points_begin, hole_points_end, std::back_inserter( inserted_plc.appendix() ) );
        return handle;
    }






    
    template<typename DomainType, typename vertex_handle_type>
    typename result_of::handle<DomainType, tetrahedron_tag>::type make_tetrahedron( DomainType & domain, vertex_handle_type v0, vertex_handle_type v1, vertex_handle_type v2, vertex_handle_type v3 )
    {
        viennagrid::storage::static_array<vertex_handle_type, 4> handles;
        handles[0] = v0;
        handles[1] = v1;
        handles[2] = v2;
        handles[3] = v3;

        return make_element<viennagrid::tetrahedron_tag>( domain, handles.begin(), handles.end() );
    }



    template<typename DomainType, typename vertex_handle_type>
    typename result_of::handle<DomainType, quadrilateral_tag>::type make_quadrilateral( DomainType & domain, vertex_handle_type v0, vertex_handle_type v1, vertex_handle_type v2, vertex_handle_type v3 )
    {
        viennagrid::storage::static_array<vertex_handle_type, 4> handles;
        handles[0] = v0;
        handles[1] = v1;
        handles[2] = v2;
        handles[3] = v3;

        return make_element<viennagrid::quadrilateral_tag>( domain, handles.begin(), handles.end() );
    }

    template<typename DomainType, typename vertex_handle_type>
    typename result_of::handle<DomainType, hexahedron_tag>::type make_hexahedron( DomainType & domain, vertex_handle_type v0, vertex_handle_type v1, vertex_handle_type v2, vertex_handle_type v3,
                                                                                                       vertex_handle_type v4, vertex_handle_type v5, vertex_handle_type v6, vertex_handle_type v7 )
    {
        viennagrid::storage::static_array<vertex_handle_type, 8> handles;
        handles[0] = v0;
        handles[1] = v1;
        handles[2] = v2;
        handles[3] = v3;
        handles[4] = v4;
        handles[5] = v5;
        handles[6] = v6;
        handles[7] = v7;

        return make_element<viennagrid::hexahedron_tag>( domain, handles.begin(), handles.end() );
    }

}



#endif
