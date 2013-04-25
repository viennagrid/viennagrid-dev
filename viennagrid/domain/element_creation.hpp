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

#include "viennagrid/element/tagging.hpp"
#include "viennagrid/domain/topology.hpp"
#include "viennagrid/domain/domain.hpp"
#include "viennagrid/topology/plc.hpp"

namespace viennagrid
{
       
    template<typename element_type, typename domain_type>
    typename result_of::handle<domain_type, element_type>::type create_element( domain_type & domain )
    {
        return push_element(domain, element_type() ).first;
    }
    
    template<typename element_type, typename domain_type>
    typename result_of::handle<domain_type, element_type>::type create_element( domain_type & domain, typename element_type::id_type id )
    {
        //typedef typename result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
        
        element_type element;
        element.id( id );
        
        return push_element_noid(domain, element).first;
    }
    
    
    template<typename element_type, typename domain_type, typename handle_array_iterator_type>
    typename result_of::handle<domain_type, typename element_type::tag>::type create_element( domain_type & domain,
                                                                                                    handle_array_iterator_type array_start, const handle_array_iterator_type & array_end )
    {
        element_type element = element_type( inserter(domain).get_physical_container_collection() );
        
        size_t element_index = 0;
        for ( ; array_start != array_end; ++array_start, ++element_index )
            viennagrid::set_vertex( element, *array_start, element_index );
                
        return push_element(domain, element).first;
    }
    
    template<typename element_type, typename domain_type, typename handle_array_type>
    typename result_of::handle<domain_type, typename element_type::tag>::type create_element( domain_type & domain, const handle_array_type & array )
    {
        return create_element<element_type>(domain, array.begin(), array.end() );
    }
    

    
    template<typename element_type, typename domain_type, typename handle_array_iterator_type>
    typename result_of::handle<domain_type, typename element_type::tag>::type create_element( domain_type & domain,
                                                                                                    handle_array_iterator_type array_start, const handle_array_iterator_type & array_end,
                                                                                                    typename element_type::id_type id )
    {
        element_type element = element_type( inserter(domain).get_physical_container_collection() );
        
        element.id( id );
        
        size_t element_index = 0;
        for ( ; array_start != array_end; ++array_start, ++element_index )
            viennagrid::set_vertex( element, *array_start, element_index );

        return push_element_noid(domain, element ).first;
    }
    
    
    template<typename element_type, typename domain_type, typename handle_array_type>
    typename result_of::handle<domain_type, typename element_type::tag>::type create_element( domain_type & domain, const handle_array_type & array, typename element_type::id_type id )
    {
        return create_element<element_type>(domain, array.begin(), array.end(), id );
    }
    
    
    template<typename plc_type, typename domain_type, typename line_handle_array_iterator_type, typename vertex_handle_array_iterator_type, typename point_iterator_type>
    typename result_of::handle<domain_type, plc_type>::type create_element( domain_type & domain,
                             line_handle_array_iterator_type line_it, line_handle_array_iterator_type const & lines_end,
                             vertex_handle_array_iterator_type vertex_it, vertex_handle_array_iterator_type const & vertices_end,
                             point_iterator_type hole_point_it, point_iterator_type const & hole_point_end
                                                                                )
    {
        std::pair<typename result_of::handle<domain_type, plc_type>::type, bool> ret = viennagrid::push_element<true, false>(domain, plc_type( inserter(domain).get_physical_container_collection() ) );
        typename result_of::handle<domain_type, plc_type>::type plc_handle = ret.first;
        plc_type & plc = viennagrid::dereference_handle(domain, plc_handle);
        
        for ( ; line_it != lines_end; ++line_it)
        {
            plc.container( viennagrid::line_tag() ).insert_handle( *line_it );
        }
        
        for ( ; vertex_it != vertices_end; ++vertex_it)
        {
            plc.container( viennagrid::vertex_tag() ).insert_handle( *vertex_it );
        }
        
        
        typedef typename std::iterator_traits<point_iterator_type>::value_type point_type;
        std::vector<point_type> hole_points;
        std::copy(hole_point_it, hole_point_end, std::back_inserter(hole_points) );
        viennadata::access< plc_hole_point_tag, std::vector<point_type> >()(plc) = hole_points;

        plc.insert_callback( inserter(domain), ret.second );
        return plc_handle;
    }
    
    
    
    template<typename element_type, typename vector_type, typename topology_type, typename metainfo_collection_type>
    typename result_of::handle<domain_t<vector_type, topology_type, metainfo_collection_type>, element_type>::type
        create_element( domain_t<vector_type, topology_type, metainfo_collection_type> & domain, const vector_type & point )
    {
        typename result_of::handle<domain_t<vector_type, topology_type, metainfo_collection_type>, element_type>::type ret = push_element(domain, element_type() ).first;
        viennagrid::point(domain, ret) = point;
        
        return ret;
    }
    
    template<typename element_type, typename vector_type, typename topology_type, typename metainfo_collection_type>
    typename result_of::handle<domain_t<vector_type, topology_type, metainfo_collection_type>, element_type>::type
        create_element( domain_t<vector_type, topology_type, metainfo_collection_type> & domain, typename element_type::id_type id, const vector_type & point )
    {
        element_type element;
        element.id( id );
        
        typename result_of::handle<domain_t<vector_type, topology_type, metainfo_collection_type>, element_type>::type ret = push_element_noid(domain, element_type() ).first;
        viennagrid::point(domain, ret) = point;
        
        return ret;
    }

    
    
    
    template<typename domain_type>
    typename result_of::handle<domain_type, vertex_tag>::type create_vertex( domain_type & domain )
    {
        typedef typename result_of::element<domain_type, vertex_tag>::type element_type;
        return create_element<element_type>(domain);
    }
    
    template<typename vector_type, typename topology_type, typename metainfo_collection_type>
    typename result_of::handle<domain_t<vector_type, topology_type, metainfo_collection_type>, vertex_tag>::type
        create_vertex( domain_t<vector_type, topology_type, metainfo_collection_type> & domain, const vector_type & point )
    {
        typedef typename result_of::element< domain_t<vector_type, topology_type, metainfo_collection_type>, vertex_tag>::type element_type;
        typename result_of::handle< domain_t<vector_type, topology_type, metainfo_collection_type>, element_type>::type ret = push_element(domain, element_type() ).first;
        viennagrid::point(domain, ret) = point;
        
        return ret;
    }
    
    template<typename vector_type, typename topology_type, typename metainfo_collection_type>
    typename result_of::handle<domain_t<vector_type, topology_type, metainfo_collection_type>, vertex_tag>::type
        create_vertex( domain_t<vector_type, topology_type, metainfo_collection_type> & domain,
                       typename result_of::element<domain_t<vector_type, topology_type, metainfo_collection_type>, vertex_tag>::type::id_type id,
                       const vector_type & point )
    {
        typedef typename result_of::element< domain_t<vector_type, topology_type, metainfo_collection_type>, vertex_tag>::type element_type;
        element_type element;
        element.id( id );
        
        typename result_of::handle< domain_t<vector_type, topology_type, metainfo_collection_type>, element_type>::type ret = push_element_noid(domain, element ).first;
        viennagrid::point(domain, ret) = point;
        
        return ret;
    }
    
    template<typename vector_type, typename topology_type, typename metainfo_collection_type>
    typename result_of::handle<domain_t<vector_type, topology_type, metainfo_collection_type>, vertex_tag>::type
        create_unique_vertex( domain_t<vector_type, topology_type, metainfo_collection_type> & domain, const vector_type & p, typename result_of::coord_type<vector_type>::type tolerance = 1e-6 )
    {
        typedef domain_t<vector_type, topology_type, metainfo_collection_type> domain_type;
        typedef typename result_of::element_range<domain_type, vertex_tag>::type vertex_range_type;
        typedef typename result_of::handle_iterator<vertex_range_type>::type vertex_range_handle_iterator;
        
        vertex_range_type vertices = viennagrid::elements<vertex_tag>(domain);
        for (vertex_range_handle_iterator hit = vertices.begin(); hit != vertices.end(); ++hit)
        {
            if (viennagrid::norm_2( p - point(domain, *hit) ) < tolerance )
                return *hit;
        }
        
        typedef typename result_of::element< domain_type, vertex_tag>::type element_type;
        typename result_of::handle< domain_type, element_type>::type ret = push_element(domain, element_type() ).first;
        viennagrid::point(domain, ret) = p;
        
        return ret;
    }
    

    
    template<typename domain_type, typename vertex_handle_type>
    typename result_of::handle<domain_type, line_tag>::type create_line( domain_type & domain, vertex_handle_type v0, vertex_handle_type v1 )
    {
        typedef typename result_of::element<domain_type, line_tag>::type element_type;
        element_type element = element_type( inserter(domain).get_physical_container_collection() );
        
        viennagrid::set_vertex( element, v0, 0 );
        viennagrid::set_vertex( element, v1, 1 );
                
        return push_element(domain, element).first;
    }
    
    template<typename domain_type, typename vertex_handle_type>
    typename result_of::handle<domain_type, triangle_tag>::type create_triangle( domain_type & domain, vertex_handle_type v0, vertex_handle_type v1, vertex_handle_type v2 )
    {
        typedef typename result_of::element<domain_type, triangle_tag>::type element_type;
        element_type element = element_type( inserter(domain).get_physical_container_collection() );
        
        viennagrid::set_vertex( element, v0, 0 );
        viennagrid::set_vertex( element, v1, 1 );
        viennagrid::set_vertex( element, v2, 2 );
                
        return push_element(domain, element).first;
    }
    
    template<typename domain_type, typename vertex_handle_type>
    typename result_of::handle<domain_type, tetrahedron_tag>::type create_tetrahedron( domain_type & domain, vertex_handle_type v0, vertex_handle_type v1, vertex_handle_type v2, vertex_handle_type v3 )
    {
        typedef typename result_of::element<domain_type, tetrahedron_tag>::type element_type;
        element_type element = element_type( inserter(domain).get_physical_container_collection() );
        
        viennagrid::set_vertex( element, v0, 0 );
        viennagrid::set_vertex( element, v1, 1 );
        viennagrid::set_vertex( element, v2, 2 );
        viennagrid::set_vertex( element, v3, 3 );
                
        return push_element(domain, element).first;
    }
    
}



#endif
