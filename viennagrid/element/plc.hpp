#ifndef VIENNAGRID_PLC_HPP
#define VIENNAGRID_PLC_HPP

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


/** @file element.hpp
    @brief Provides the main n-cell type
*/

namespace viennagrid
{
    
    struct plc_tag
    {
        typedef viennameta::null_type facet_type;
        
        enum{ dim = 2 };
        static std::string name() { return "PLC"; }
    };
    
    
    template<typename bounding_polygon_view_type_, typename hole_polygon_view_type_, typename line_view_type_, typename vertex_view_type_, typename id_tag_>
    class plc_t : public viennagrid::storage::id_handler<
                    typename viennagrid::storage::result_of::id< plc_t<bounding_polygon_view_type_, hole_polygon_view_type_, line_view_type_, vertex_view_type_, id_tag_>, id_tag_>::type
                >
    {
    public:
        
        typedef plc_tag tag;

        typedef bounding_polygon_view_type_ bounding_polygon_view_type;
        typedef typename bounding_polygon_view_type::value_type bounding_polygon_type;
        typedef typename bounding_polygon_view_type::hook_type bounding_polygon_hook_type;
        
        typedef hole_polygon_view_type_ hole_polygon_view_type;
        typedef typename hole_polygon_view_type::value_type hole_polygon_type;
        typedef typename hole_polygon_view_type::hook_type hole_polygon_hook_type;
        
        typedef line_view_type_ line_view_type;
        typedef typename line_view_type::value_type line_type;
        typedef typename line_view_type::hook_type line_hook_type;
        
        typedef vertex_view_type_ vertex_view_type;
        typedef typename vertex_view_type::value_type vertex_type;
        typedef typename vertex_view_type::hook_type vertex_hook_type;
        
        
        
        void set_bounding_polygon( bounding_polygon_hook_type bounding_polygon_hook )
        {
            bounding_polygon.set_hook( bounding_polygon_hook, 0 );
            
            
            typedef typename viennagrid::result_of::element_range<bounding_polygon_type, line_type>::type line_range_type;
            typedef typename viennagrid::result_of::hook_iterator<line_range_type>::type line_hook_iterator;
            
            line_range_type line_range = viennagrid::elements<line_type>( get_bounding_polygon() );
            for (line_hook_iterator it = line_range.hook_begin(); it != line_range.hook_end(); ++it)
                lines.insert_hook( *it );
                

            typedef typename viennagrid::result_of::element_range<bounding_polygon_type, vertex_type>::type vertex_range_type;
            typedef typename viennagrid::result_of::hook_iterator<vertex_range_type>::type vertex_hook_iterator;
            
            vertex_range_type vertex_range = viennagrid::elements<vertex_type>( get_bounding_polygon() );
            for (vertex_hook_iterator it = vertex_range.hook_begin(); it != vertex_range.hook_end(); ++it)
                vertices.insert_hook( *it );
        }
        
        bounding_polygon_type & get_bounding_polygon() { return bounding_polygon[0]; }
        const bounding_polygon_type & get_bounding_polygon() const { return bounding_polygon[0]; }
        
        template<typename hook_array_iterator_type>
        void add_hole_polygons(hook_array_iterator_type array_start, const hook_array_iterator_type & array_end)
        {
            for ( ; array_start != array_end; ++array_start )
            {
                hole_polygons.insert_hook( *array_start );
                hole_polygon_type & poly = hole_polygons.dereference_hook( *array_start );
                
                
                typedef typename viennagrid::result_of::element_range<hole_polygon_type, line_type>::type line_range_type;
                typedef typename viennagrid::result_of::hook_iterator<line_range_type>::type line_hook_iterator;
                
                line_range_type line_range = viennagrid::elements<line_type>( poly );
                for (line_hook_iterator it = line_range.hook_begin(); it != line_range.hook_end(); ++it)
                    lines.insert_hook( *it );
                    

                typedef typename viennagrid::result_of::element_range<hole_polygon_type, vertex_type>::type vertex_range_type;
                typedef typename viennagrid::result_of::hook_iterator<vertex_range_type>::type vertex_hook_iterator;
                
                vertex_range_type vertex_range = viennagrid::elements<vertex_type>( poly );
                for (vertex_hook_iterator it = vertex_range.hook_begin(); it != vertex_range.hook_end(); ++it)
                    vertices.insert_hook( *it );
            }
        }
        
        hole_polygon_view_type & get_hole_polygons() { return hole_polygons; }
        const hole_polygon_view_type & get_hole_polygons() const { return hole_polygons; }
        
        template<typename hook_array_iterator_type>
        void add_lines(hook_array_iterator_type array_start, const hook_array_iterator_type & array_end)
        {
            for ( ; array_start != array_end; ++array_start )
            {
                lines.insert_hook( *array_start );
                line_type & line = lines.dereference_hook( *array_start );
                
                typedef typename viennagrid::result_of::element_range<line_type, vertex_type>::type vertex_range_type;
                typedef typename viennagrid::result_of::hook_iterator<vertex_range_type>::type vertex_hook_iterator;
                
                vertex_range_type vertex_range = viennagrid::elements<vertex_type>( line );
                for (vertex_hook_iterator it = vertex_range.hook_begin(); it != vertex_range.hook_end(); ++it)
                    vertices.insert_hook( *it );
            }
        }
        
        line_view_type & get_lines() { return lines; }
        const line_view_type & get_lines() const { return lines; }
        
        template<typename hook_array_iterator_type>
        void add_vertices(hook_array_iterator_type array_start, const hook_array_iterator_type & array_end)
        {
            for ( ; array_start != array_end; ++array_start )
            {
                vertices.insert_hook( *array_start );
            }
        }
        
        vertex_view_type & get_vertices() { return vertices; }
        const vertex_view_type & get_vertices() const { return vertices; }
        
        
        template<typename container_collection_type>
        void set_container_collection(container_collection_type & container_collection)
        {
            bounding_polygon.set_base_container( viennagrid::storage::collection::get<bounding_polygon_type>(container_collection) );
            
            hole_polygons.set_base_container( viennagrid::storage::collection::get<hole_polygon_type>(container_collection) );
            lines.set_base_container( viennagrid::storage::collection::get<line_type>(container_collection) );
            vertices.set_base_container( viennagrid::storage::collection::get<vertex_type>(container_collection) );
        }
        
        
        
        template<typename inserter_type>
        void insert_callback( inserter_type & inserter, bool inserted )
        {}

        
        
    private:
        bounding_polygon_view_type bounding_polygon;

        hole_polygon_view_type hole_polygons;
        line_view_type lines;
        vertex_view_type vertices;
    };
    
    
    
    
    /** @brief Overload for the output streaming operator */
    template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
    std::ostream & operator<<(std::ostream & os, plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag> & plc)
    {
        typedef plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag> element_type;
        typedef typename viennagrid::result_of::const_element_range< element_type, vertex_tag >::type vertex_range;
        typedef typename viennagrid::result_of::const_iterator< vertex_range >::type const_vertex_iterator;
        
        
        os << "-- " << plc_tag::name() << ", ID: " << plc.id() << " --";
        const vertex_range & vertices = elements<vertex_tag>(plc);
        for (const_vertex_iterator vit  = vertices.begin();
                            vit != vertices.end();
                        ++vit)
            os << std::endl << "  " << *vit;
        
        return os;
    }
    
    
    
    
    namespace result_of
    {
        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
        struct element_tag< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag> >
        {
            typedef plc_tag type;
        };
    }
    
    
    
    
    namespace result_of
    {
        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
        struct element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, vertex_tag >
        {
            typedef typename viennagrid::storage::container_range_wrapper<vertex_view_type> type;
        };

        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
        struct const_element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, vertex_tag >
        {
            typedef typename viennagrid::storage::container_range_wrapper<const vertex_view_type> type;
        };
        
        
        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
        struct element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, line_tag >
        {
            typedef typename viennagrid::storage::container_range_wrapper<line_view_type> type;
        };

        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
        struct const_element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, line_tag >
        {
            typedef typename viennagrid::storage::container_range_wrapper<const line_view_type> type;
        };
        
        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
        struct element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, hole_polygon_tag >
        {
            typedef typename viennagrid::storage::container_range_wrapper<hole_polygon_view_type> type;
        };

        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
        struct const_element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, hole_polygon_tag >
        {
            typedef typename viennagrid::storage::container_range_wrapper<const hole_polygon_view_type> type;
        };
        
        
        
        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag, typename element_type_or_tag>
        struct element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, element_type_or_tag >
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag_;
            typedef typename element_range<plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, element_tag_>::type type;
        };
        
        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag, typename element_type_or_tag>
        struct const_element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, element_type_or_tag >
        {
            typedef typename element_tag<element_type_or_tag>::type element_tag_;
            typedef typename const_element_range<plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, element_tag_>::type type;
        };
    }
    
    
    template<typename element_tag>
    struct plc_elements_helper;
    
    template<>
    struct plc_elements_helper<vertex_tag>
    {
        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
        static typename result_of::element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, vertex_tag >::type
            elements(  plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag> & plc )
        {
            return plc.get_vertices();
        }
    };
    
    template<>
    struct plc_elements_helper<line_tag>
    {
        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
        static typename result_of::element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, line_tag >::type
            elements(  plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag> & plc )
        {
            return plc.get_lines();
        }
    };
    
    template<>
    struct plc_elements_helper<hole_polygon_tag>
    {
        template<typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
        static typename result_of::element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, hole_polygon_tag >::type
            elements(  plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag> & plc )
        {
            return plc.get_hole_polygons();
        }
    };
    
    
    template<typename element_type_or_tag, typename bounding_polygon_view_type, typename hole_polygon_view_type, typename line_view_type, typename vertex_view_type, typename id_tag>
    typename result_of::element_range< plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag>, element_type_or_tag >::type
        elements( plc_t<bounding_polygon_view_type, hole_polygon_view_type, line_view_type, vertex_view_type, id_tag> & plc )
    {
        typedef typename viennagrid::result_of::element_tag<element_type_or_tag>::type element_tag;
        return plc_elements_helper<element_tag>::elements(plc);
    }
    
    
    

    
  
}


#endif
