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
        
        //for (typename handle_array_type::const_iterator it = array.begin(); it != array.end(); ++it, ++element_index)
            //viennagrid::set_vertex( element, *it, element_index );
                
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
    
    
    template<typename plc_type, typename domain_type, // typename polygon_handle_array_iterator_type,
//                 typename polygon_handle_type,
                typename line_handle_array_iterator_type, typename vertex_handle_array_iterator_type,
                typename point_iterator_type>
    typename result_of::handle<domain_type, plc_type>::type create_element( domain_type & domain,
//                              polygon_handle_array_iterator_type bounding_polygon_it, polygon_handle_array_iterator_type const & bounding_polygon_end,
//                              polygon_handle_type bounding_polygon,
                             line_handle_array_iterator_type line_it, line_handle_array_iterator_type const & lines_end,
                             vertex_handle_array_iterator_type vertex_it, vertex_handle_array_iterator_type const & vertices_end,
                             point_iterator_type hole_point_it, point_iterator_type const & hole_point_end
                                                                                )
    {
        std::pair<typename result_of::handle<domain_type, plc_type>::type, bool> ret = viennagrid::push_element<true, false>(domain, plc_type( inserter(domain).get_physical_container_collection() ) );
        typename result_of::handle<domain_type, plc_type>::type plc_handle = ret.first;
        plc_type & plc = viennagrid::dereference_handle(domain, plc_handle);
                
//         typename tagging::result_of::element_tag<plc_type, bounding_tag>::type plc_bounding_tag(plc);
//         typename tagging::result_of::element_tag<plc_type, loose_tag>::type plc_loose_tag(plc);
        
        for ( ; line_it != lines_end; ++line_it)
        {
            plc.container( viennagrid::line_tag() ).insert_handle( *line_it );
//             tagging::tag(viennagrid::dereference_handle(domain, *line_it), plc_loose_tag);
        }
        
//         std::cout << "In PLC_CREATE: " << plc.container( viennagrid::line_tag() ).size() << std::endl;
        
        for ( ; vertex_it != vertices_end; ++vertex_it)
        {
            plc.container( viennagrid::vertex_tag() ).insert_handle( *vertex_it );
//             tagging::tag(viennagrid::dereference_handle(domain, *vertex_it), plc_loose_tag);
        }
        
        
        typedef typename std::iterator_traits<point_iterator_type>::value_type point_type;
        std::vector<point_type> hole_points;
        std::copy(hole_point_it, hole_point_end, std::back_inserter(hole_points) );
        viennadata::access< plc_hole_point_tag, std::vector<point_type> >()(plc) = hole_points;

        plc.insert_callback( inserter(domain), ret.second );
        return plc_handle;
    }
    
    
    
//     template<typename plc_type, typename domain_type, typename polygon_handle_array_iterator_type,
// //                 typename polygon_handle_type,
//                 typename line_handle_array_iterator_type, typename vertex_handle_array_iterator_type,
//                 typename point_iterator_type>
//     typename result_of::handle<domain_type, plc_type>::type create_element( domain_type & domain,
//                              polygon_handle_array_iterator_type bounding_polygon_it, polygon_handle_array_iterator_type const & bounding_polygon_end,
// //                              polygon_handle_type bounding_polygon,
//                              line_handle_array_iterator_type line_it, line_handle_array_iterator_type const & lines_end,
//                              vertex_handle_array_iterator_type vertex_it, vertex_handle_array_iterator_type const & vertices_end,
//                              point_iterator_type hole_point_it, point_iterator_type const & hole_point_end
//                                                                                 )
//     {
//         typedef typename std::iterator_traits<line_handle_array_iterator_type>::value_type line_handle_type;
//         std::vector<line_handle_type> lines;
//         
//         std::copy( line_it, lines_end, std::back_inserter(lines) );
//         
//         for ( ; bounding_polygon_it != bounding_polygon_end; ++bounding_polygon_it  )
//         {
// //             typedef typename viennagrid::result_of::element<domain_type, viennagrid::polygon_tag>::type polygon_type;
//             typedef typename std::iterator_traits<bounding_polygon_it>::value_type polygon_type;
//             
//             polygon_type & polygon = viennagrid::dereference_handle( domain, *bounding_polygon_it );
//             
//             typedef typename viennagrid::result_of::element_range<polygon_type, viennagrid::line_tag>::type line_on_polygon_range;
//             line_on_polygon_range lines_on_polygon = viennagrid::elements<viennagrid::line_tag>(polygon);
//             
//             std::copy( lines_on_polygon.handle_begin(), lines_on_polygon.handle_end(), std::back_inserter(lines) );
//         }
//         
//         return create_element<plc_type>(  );
//     }
    
    
    
//     template<typename plc_type, typename domain_type,
//                 typename polygon_handle_array_iterator_type,
// //                 typename polygon_handle_type,
// //                 typename polygon_handle_array_iterator_type,
//                 typename line_handle_array_iterator_type, typename vertex_handle_array_iterator_type,
//                 typename point_iterator_type>
//     typename result_of::handle<domain_type, plc_type>::type create_element( domain_type & domain,
// //                              polygon_handle_type bounding_polygon,
//                              polygon_handle_array_iterator_type bounding_polygon_it, polygon_handle_array_iterator_type const & bounding_polygon_end,
//                              polygon_handle_array_iterator_type polygon_it, polygon_handle_array_iterator_type const & polygons_end,
//                              line_handle_array_iterator_type line_it, line_handle_array_iterator_type const & lines_end,
//                              vertex_handle_array_iterator_type vertex_it, vertex_handle_array_iterator_type const & vertices_end,
//                              point_iterator_type hole_point_it, point_iterator_type const & hole_point_end
//                                                                                 )
//     {
//         typedef typename std::iterator_traits<line_handle_array_iterator_type>::value_type line_handle_type;
//         std::vector<line_handle_type> lines;
//         
//         std::copy( line_it, lines_end, std::back_inserter(lines) );
//         
//         for ( ; polygon_it != polygons_end; ++polygon_it  )
//         {
//             typedef typename viennagrid::result_of::element<domain_type, viennagrid::polygon_tag>::type polygon_type;
//             
//             polygon_type & polygon = viennagrid::dereference_handle( domain, *polygon_it );
//             
//             typedef typename viennagrid::result_of::element_range<polygon_type, viennagrid::line_tag>::type line_on_polygon_range;
//             line_on_polygon_range lines_on_polygon = viennagrid::elements<viennagrid::line_tag>(polygon);
//             
//             std::copy( lines_on_polygon.handle_begin(), lines_on_polygon.handle_end(), std::back_inserter(lines) );
//         }
//         
//         return create_element<plc_type>(domain, bounding_polygon_it, bounding_polygon_end, lines.begin(), lines.end(), vertex_it, vertices_end, hole_point_it, hole_point_end);
//     }
//     

    
    template<typename domain_type>
    typename result_of::handle<domain_type, vertex_tag>::type create_vertex( domain_type & domain )
    {
        typedef typename result_of::element<domain_type, vertex_tag>::type element_type;
        return create_element<element_type>(domain);
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
