#ifndef VIENNAGRID_CONFIG_GENERIC_CONFIG_HPP
#define VIENNAGRID_CONFIG_GENERIC_CONFIG_HPP

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


#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"
#include "viennagrid/element/element_key.hpp"

/** @file config/simplex.hpp
    @brief Provides default configuration classes for simplex domains
*/

namespace viennagrid
{
    namespace result_of
    {
        //
        // Meta Functions for creating a default config
        //
        
        template<typename element_tag, typename boundary_cell_tag>
        struct storage_layout_config
        {
            
            typedef typename viennameta::typemap::result_of::insert<
                typename storage_layout_config<element_tag, typename boundary_cell_tag::facet_tag>::type,
                viennameta::static_pair<
                    boundary_cell_tag,
                    viennagrid::full_handling_tag
                    //viennameta::static_pair< viennagrid::full_handling_tag, viennagrid::full_handling_tag >
                >
            >::type type;
        };
        
        template<typename element_tag>
        struct storage_layout_config<element_tag, viennagrid::vertex_tag>
        {
            //typedef viennameta::static_pair< viennagrid::full_handling_tag, viennagrid::no_handling_tag > type;
            typedef typename viennameta::make_typemap<
                viennagrid::vertex_tag,
                viennagrid::no_orientation_handling_tag
                //viennameta::static_pair< viennagrid::full_handling_tag, viennagrid::no_handling_tag >
            >::type type;
        };
        
        template<typename element_tag>
        struct storage_layout_config<element_tag, viennameta::null_type>
        {
            //typedef viennameta::static_pair< viennagrid::full_handling_tag, viennagrid::no_handling_tag > type;
            typedef viennameta::null_type type;
        };
        
        
        template<typename element_tag, typename boundary_cell_tag>
        struct default_container_tag
        {
            typedef viennagrid::storage::hidden_key_map_tag< viennagrid::storage::element_key_tag > type;
        };
        
        template<typename element_tag>
        struct default_container_tag<element_tag, element_tag>
        {
            typedef viennagrid::storage::std_deque_tag type;
        };
        
        template<typename element_tag>
        struct default_container_tag<element_tag, viennagrid::vertex_tag>
        {
            typedef viennagrid::storage::std_deque_tag type;
        };
        
        
        template<typename element_tag, typename boundary_cell_tag, typename hook_tag>
        struct default_element_config
        {
            typedef typename viennameta::make_typemap<
            
                viennagrid::element_id_tag,
                viennagrid::storage::smart_id_tag<int>,
                
                viennagrid::element_container_tag,
                viennagrid::storage::hooked_container_tag<           
                    //viennagrid::storage::std_deque_tag,
                    typename default_container_tag<element_tag, boundary_cell_tag>::type,
                    hook_tag
                >,
                
                viennagrid::element_boundary_storage_layout_tag,
                typename storage_layout_config<element_tag, typename boundary_cell_tag::facet_tag>::type
            
            >::type type;
        };
        
        template<typename element_tag, typename boundary_cell_tag, typename hook_tag>
        struct default_topologic_config_helper
        {
            typedef typename viennameta::typemap::result_of::insert<
                typename default_topologic_config_helper<element_tag, typename boundary_cell_tag::facet_tag, hook_tag>::type,
                viennameta::static_pair<
                    boundary_cell_tag,
                    typename default_element_config<element_tag, boundary_cell_tag, hook_tag>::type
                >
            >::type type;
        };
        
        template<typename element_tag, typename hook_tag>
        struct default_topologic_config_helper<element_tag, viennagrid::vertex_tag, hook_tag>
        {
            typedef typename viennameta::make_typemap<
                viennagrid::vertex_tag,
                typename default_element_config<element_tag, viennagrid::vertex_tag, hook_tag>::type
            >::type type;
        };
        
        
        template<typename element_tag, typename hook_tag>
        struct default_topologic_config
        {
            typedef typename default_topologic_config_helper<element_tag, element_tag, hook_tag>::type type;
        };
        
        template<typename hook_tag>
        struct default_topologic_config<plc_tag, hook_tag>
        {
            typedef viennameta::static_pair<
                plc_tag,
                typename viennameta::make_typemap<
            
                    viennagrid::element_id_tag,
                    viennagrid::storage::smart_id_tag<int>,
                
                    viennagrid::element_container_tag,
                    viennagrid::storage::hooked_container_tag<           
                        typename default_container_tag<plc_tag, plc_tag>::type,
                        hook_tag
                    >,
                
                    viennagrid::element_boundary_storage_layout_tag,
                    viennameta::null_type
            
                >::type
            > plc_config;
            
            
            
            typedef viennameta::static_pair<
                hole_polygon_tag,
                typename viennameta::make_typemap<
            
                    viennagrid::element_id_tag,
                    viennagrid::storage::smart_id_tag<int>,
                
                    viennagrid::element_container_tag,
                    viennagrid::storage::hooked_container_tag<
                        typename default_container_tag<plc_tag, hole_polygon_tag>::type,
                        hook_tag
                    >,
                
                    viennagrid::element_boundary_storage_layout_tag,
                    viennameta::make_typemap<
                        viennagrid::vertex_tag, viennagrid::no_orientation_handling_tag,
                        viennagrid::line_tag, viennagrid::full_handling_tag
                    >::type
            
                >::type
            > hole_polygon_config;            
            
            
            
            typedef typename viennameta::typemap::result_of::insert<
                typename viennameta::typemap::result_of::insert<
                    typename default_topologic_config_helper<plc_tag, polygon_tag, hook_tag>::type,
                    hole_polygon_config
                >::type,
                plc_config
            >::type type;
        };
    }
}


#endif
