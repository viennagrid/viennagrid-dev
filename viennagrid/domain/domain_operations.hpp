#ifndef VIENNAGRID_DOMAIN_OPERATIONS_HPP
#define VIENNAGRID_DOMAIN_OPERATIONS_HPP

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

#include "viennagrid/domain/domain.hpp"
#include "viennagrid/domain/element_creation.hpp"

namespace viennagrid
{
    
    template<typename src_domain_type, typename src_segment_container_type, typename dst_domain_type, typename dst_segment_container_type>
    void copy_domain(
            src_domain_type const & src_domain, src_segment_container_type const & src_segments,
            dst_domain_type & dst_domain, dst_segment_container_type & dst_segments )
    {
        typedef typename src_segment_container_type::value_type src_segment_type;
        typedef typename dst_segment_container_type::value_type dst_segment_type;
        
        typedef typename viennagrid::result_of::cell_tag<src_domain_type>::type src_cell_tag;
        typedef typename viennagrid::result_of::cell_type<src_domain_type>::type src_cell_type;
        
        typedef typename viennagrid::result_of::const_vertex_range<src_domain_type>::type src_vertex_range_type;
        typedef typename viennagrid::result_of::iterator<src_vertex_range_type>::type src_vertex_range_iterator;
        
        typedef typename viennagrid::result_of::const_cell_range<src_segment_type>::type src_cell_range_type;
        typedef typename viennagrid::result_of::iterator<src_cell_range_type>::type src_cell_range_itertor;
        
        typedef typename viennagrid::result_of::const_vertex_handle<src_domain_type>::type src_const_vertex_handle;
        typedef typename viennagrid::result_of::vertex_handle<dst_domain_type>::type dst_vertex_handle;
        
        if (&src_domain == &dst_domain)
            return;
        
        viennagrid::clear_domain(dst_domain);
        dst_segments.clear();
        
        std::map<src_const_vertex_handle, dst_vertex_handle> vertex_handle_map;
        
        src_vertex_range_type vertices = viennagrid::elements( src_domain );
        for (src_vertex_range_iterator it = vertices.begin(); it != vertices.end(); ++it)
            vertex_handle_map[it.handle()] = viennagrid::create_vertex( dst_domain, viennagrid::point(src_domain, *it) );
        

        
        for (typename src_segment_container_type::const_iterator seg_it = src_segments.begin(); seg_it != src_segments.end(); ++seg_it)
        {
            dst_segments.push_back( viennagrid::create_view<dst_segment_type>(dst_domain) );
            dst_segment_type & dst_segment = dst_segments.back();
            
            src_cell_range_type cells = viennagrid::elements( *seg_it );
            for (src_cell_range_itertor it = cells.begin(); it != cells.end(); ++it)
            {
                src_cell_type const & cell = *it;
                
                std::deque<dst_vertex_handle> vertex_handles;
                
                typedef typename viennagrid::result_of::const_vertex_range<src_cell_type>::type src_vertex_on_src_cell_range_type;
                typedef typename viennagrid::result_of::iterator<src_vertex_on_src_cell_range_type>::type src_vertex_on_src_cell_range_iterator;
                
                src_vertex_on_src_cell_range_type cell_vertices = viennagrid::elements(cell);
                for (src_vertex_on_src_cell_range_iterator jt = cell_vertices.begin(); jt != cell_vertices.end(); ++jt)
                    vertex_handles.push_back( vertex_handle_map[jt.handle()] );
                
                typedef typename viennagrid::result_of::cell_type<dst_domain_type>::type dst_cell_type;
                viennagrid::create_element<dst_cell_type>( dst_segment, vertex_handles.begin(), vertex_handles.end() );
            }
        }
    }
    
}

#endif
