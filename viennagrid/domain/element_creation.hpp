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
#include "viennagrid/domain/topologic_domain.hpp"
#include "viennagrid/domain/geometric_domain.hpp"

namespace viennagrid
{
       
    template<typename element_type, typename domain_type>
    typename result_of::element_hook<domain_type, element_type>::type create_element( domain_type & domain )
    {
        return push_element(domain, element_type() ).first;
    }
    
    template<typename element_type, typename domain_type>
    typename result_of::element_hook<domain_type, element_type>::type create_element( domain_type & domain, typename element_type::id_type id )
    {
        //typedef typename result_of::element<domain_type, viennagrid::vertex_tag>::type vertex_type;
        
        element_type element;
        element.id( id );
        
        return push_element_noid(domain, element).first;
    }
    
    
    template<typename element_type, typename domain_type, typename hook_array_iterator_type>
    typename result_of::element_hook<domain_type, typename element_type::tag>::type create_element( domain_type & domain,
                                                                                                    hook_array_iterator_type array_start, const hook_array_iterator_type & array_end )
    {
        element_type element = element_type( inserter(domain).get_physical_container_collection() );
        
        size_t element_index = 0;
        for ( ; array_start != array_end; ++array_start, ++element_index )
            viennagrid::set_vertex( element, *array_start, element_index );
        
        //for (typename hook_array_type::const_iterator it = array.begin(); it != array.end(); ++it, ++element_index)
            //viennagrid::set_vertex( element, *it, element_index );
                
        return push_element(domain, element ).first;
    }
    
    template<typename element_type, typename domain_type, typename hook_array_type>
    typename result_of::element_hook<domain_type, typename element_type::tag>::type create_element( domain_type & domain, const hook_array_type & array )
    {
        return create_element<element_type>(domain, array.begin(), array.end() );
    }
    

    
    template<typename element_type, typename domain_type, typename hook_array_iterator_type>
    typename result_of::element_hook<domain_type, typename element_type::tag>::type create_element( domain_type & domain,
                                                                                                    hook_array_iterator_type array_start, const hook_array_iterator_type & array_end,
                                                                                                    typename element_type::id_type id )
    {
        element_type element = element_type( inserter(domain).get_physical_container_collection() );
        
        element.id( id );
        
        size_t element_index = 0;
        for ( ; array_start != array_end; ++array_start, ++element_index )
            viennagrid::set_vertex( element, *array_start, element_index );

        return push_element_noid(domain, element ).first;
    }
    
    
    template<typename element_type, typename domain_type, typename hook_array_type>
    typename result_of::element_hook<domain_type, typename element_type::tag>::type create_element( domain_type & domain, const hook_array_type & array, typename element_type::id_type id )
    {
        return create_element<element_type>(domain, array.begin(), array.end(), id );
    }
    
    
    template<typename plc_type, typename domain_type, typename polygon_hook_array_iterator_type,
                typename line_hook_array_iterator_type, typename vertex_hook_array_iterator_type>
    typename result_of::element_hook<domain_type, plc_type>::type create_element( domain_type & domain,
                             polygon_hook_array_iterator_type bounding_polygon_it, const polygon_hook_array_iterator_type & bounding_polygons_end,
                             polygon_hook_array_iterator_type hole_polygon_it, const polygon_hook_array_iterator_type & hole_polygons_end,
                             line_hook_array_iterator_type line_it, const line_hook_array_iterator_type & lines_end,
                             vertex_hook_array_iterator_type vertex_it, const vertex_hook_array_iterator_type & vertices_end)
    {
        std::pair<typename result_of::element_hook<domain_type, plc_type>::type, bool> ret = viennagrid::push_element(domain, plc_type( inserter(domain).get_physical_container_collection() ) );
        typename result_of::element_hook<domain_type, plc_type>::type plc_hook = ret.first;
        plc_type & plc = viennagrid::dereference_hook(domain, plc_hook);
        
        
        typename tagging::result_of::element_tag<plc_type, hole_tag>::type plc_hole_tag(plc);
        typename tagging::result_of::element_tag<plc_type, bounding_tag>::type plc_bounding_tag(plc);
        typename tagging::result_of::element_tag<plc_type, loose_tag>::type plc_loose_tag(plc);
        
        for ( ; bounding_polygon_it != bounding_polygons_end; ++bounding_polygon_it)
        {
            plc.container( viennagrid::polygon_tag() ).insert_hook( *bounding_polygon_it );
            tagging::tag(viennagrid::dereference_hook(domain, *bounding_polygon_it), plc_bounding_tag);
        }
        
        for ( ; hole_polygon_it != hole_polygons_end; ++hole_polygon_it)
        {
            plc.container( viennagrid::polygon_tag() ).insert_hook( *hole_polygon_it );
            tagging::tag(viennagrid::dereference_hook(domain, *hole_polygon_it), plc_hole_tag);
            tagging::tag(viennagrid::dereference_hook(domain, *hole_polygon_it), plc_loose_tag);
        }
            
            
        for ( ; line_it != lines_end; ++line_it)
        {
            plc.container( viennagrid::line_tag() ).insert_hook( *line_it );
            tagging::tag(viennagrid::dereference_hook(domain, *line_it), plc_loose_tag);
            //viennadata::access<tag_key<loose_tag, plc_type>, bool>( tag_key<loose_tag, plc_type>(plc) )( viennagrid::dereference_hook(domain, *line_it) ) = true;
        }
        
        for ( ; vertex_it != vertices_end; ++vertex_it)
        {
            plc.container( viennagrid::vertex_tag() ).insert_hook( *vertex_it );
            tagging::tag(viennagrid::dereference_hook(domain, *vertex_it), plc_loose_tag);
//             viennadata::access<tag_key<loose_tag, plc_type>, bool>( tag_key<loose_tag, plc_type>(plc) )( viennagrid::dereference_hook(domain, *vertex_it) ) = true;
        }

        plc.insert_callback( inserter(domain), ret.second );
        return plc_hook;
    }
    

}



#endif
