#ifndef VIENNAGRID_ELEMENT_CREATION_HPP
#define VIENNAGRID_ELEMENT_CREATION_HPP

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

#include "viennagrid/domain/topology.hpp"
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/topology/plc.hpp"
#include "viennagrid/algorithm/norm.hpp"

namespace viennagrid
{
    template<bool generate_id, bool call_callback, typename domain_type, typename ElementTag, typename WrappedConfigType>
    std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename result_of::element_collection<domain_type>::type,
                    viennagrid::element_t<ElementTag, WrappedConfigType>
                >::type::handle_type,
                bool
            >
        push_element( domain_type & domain, viennagrid::element_t<ElementTag, WrappedConfigType> const & element)
    {
//         increment_change_counter(domain);
        return inserter(domain).template insert<generate_id, call_callback>(element);
    }
    
    
    
    template<bool generate_id, bool call_callback, typename SegmentationType, typename ElementTag, typename WrappedConfigType>
    std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename result_of::element_collection< segment_t<SegmentationType> >::type,
                    viennagrid::element_t<ElementTag, WrappedConfigType>
                >::type::handle_type,
                bool
            >
        push_element( segment_t<SegmentationType> & segment, viennagrid::element_t<ElementTag, WrappedConfigType> const & element)
    {
        std::pair<
                typename viennagrid::storage::result_of::container_of<
                    typename result_of::element_collection< segment_t<SegmentationType> >::type,
                    viennagrid::element_t<ElementTag, WrappedConfigType>
                >::type::handle_type,
                bool
            > result = push_element<generate_id, call_callback>( segment.view(), element );

        add( segment, viennagrid::dereference_handle(segment, result.first) );
        return result;
    }
    
    

  
  

    
    
    template<typename ElementTypeOrTag, typename DomainType, typename HandleIteratorType>
    typename result_of::handle<DomainType, ElementTypeOrTag>::type make_element(
          DomainType & domain,
          HandleIteratorType array_start,
          HandleIteratorType const & array_end )
    {
        typedef typename viennagrid::result_of::element<DomainType, ElementTypeOrTag>::type ElementType;
        ElementType element = ElementType( inserter(domain).get_physical_container_collection() );
        
        size_t element_index = 0;
        for ( ; array_start != array_end; ++array_start, ++element_index )
            viennagrid::set_vertex( element, *array_start, element_index );
                
        return push_element<true, true>(domain, element).first;
    }
    
    
    template<typename ElementTypeOrTag, typename DomainType, typename HandleIteratorType>
    typename result_of::handle<DomainType, ElementTypeOrTag>::type make_element_with_id(
          DomainType & domain,
          HandleIteratorType array_start,
          HandleIteratorType const & array_end,
          typename viennagrid::result_of::element<DomainType, ElementTypeOrTag>::type::id_type id )
    {
        typedef typename viennagrid::result_of::element<DomainType, ElementTypeOrTag>::type ElementType;
        ElementType element = ElementType( inserter(domain).get_physical_container_collection() );
        
        element.id( id );
        
        size_t element_index = 0;
        for ( ; array_start != array_end; ++array_start, ++element_index )
            viennagrid::set_vertex( element, *array_start, element_index );

        return push_element<false, true>(domain, element ).first;
    }

    
    

    
    
    template<typename domain_type>
    typename result_of::handle<domain_type, vertex_tag>::type make_vertex( domain_type & domain )
    {
        typedef typename result_of::element<domain_type, vertex_tag>::type element_type;
        return push_element<true, true>(domain, element_type() ).first;
    }
    
    template<typename ConfigType>
    typename viennagrid::result_of::vertex_handle< domain_t<ConfigType> >::type
        make_vertex( domain_t<ConfigType> & domain,
                       typename result_of::point_type< domain_t<ConfigType> >::type const & point )
    {
        typename viennagrid::result_of::vertex_handle< domain_t<ConfigType> >::type vtx_handle = make_vertex(domain);
        viennagrid::point(domain, vtx_handle) = point;
        return vtx_handle;
    }
    
    template<typename ConfigType>
    typename viennagrid::result_of::vertex_handle< domain_t<ConfigType> >::type
        make_vertex( domain_t<ConfigType> & domain,
                       typename viennagrid::result_of::element< domain_t<ConfigType>, vertex_tag>::type::id_type id,
                       typename result_of::point_type< domain_t<ConfigType> >::type const & point )
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
                              typename result_of::point_type< domain_t<ConfigType> >::type const & p,
                              typename viennagrid::result_of::coord_type< domain_t<ConfigType> >::type tolerance )
    {
        typedef domain_t<ConfigType> domain_type;
        typedef typename result_of::element_range<domain_type, vertex_tag>::type vertex_range_type;
        typedef typename result_of::handle_iterator<vertex_range_type>::type vertex_range_handle_iterator;
        
        vertex_range_type vertices = viennagrid::elements<vertex_tag>(domain);
        for (vertex_range_handle_iterator hit = vertices.begin(); hit != vertices.end(); ++hit)
        {
            if (viennagrid::norm_2( p - point(domain, *hit) ) < tolerance )
                return *hit;
        }
        
        return make_vertex(domain, p);
    }
    

    
    template<typename domain_type, typename vertex_handle_type>
    typename result_of::handle<domain_type, line_tag>::type make_line( domain_type & domain, vertex_handle_type v0, vertex_handle_type v1 )
    {
        viennagrid::storage::static_array<vertex_handle_type, 2> handles;
        handles[0] = v0;
        handles[1] = v1;
        
        return make_element<viennagrid::line_tag>( domain, handles.begin(), handles.end() );
    }
    
    template<typename domain_type, typename vertex_handle_type>
    typename result_of::handle<domain_type, triangle_tag>::type make_triangle( domain_type & domain, vertex_handle_type v0, vertex_handle_type v1, vertex_handle_type v2 )
    {
        viennagrid::storage::static_array<vertex_handle_type, 3> handles;
        handles[0] = v0;
        handles[1] = v1;
        handles[2] = v2;
        
        return make_element<viennagrid::triangle_tag>( domain, handles.begin(), handles.end() );
    }
    
    template<typename domain_type, typename vertex_handle_type>
    typename result_of::handle<domain_type, tetrahedron_tag>::type make_tetrahedron( domain_type & domain, vertex_handle_type v0, vertex_handle_type v1, vertex_handle_type v2, vertex_handle_type v3 )
    {
        viennagrid::storage::static_array<vertex_handle_type, 4> handles;
        handles[0] = v0;
        handles[1] = v1;
        handles[2] = v2;
        handles[3] = v3;
        
        return make_element<viennagrid::tetrahedron_tag>( domain, handles.begin(), handles.end() );
    }
    
}



#endif
