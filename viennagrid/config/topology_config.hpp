#ifndef VIENNAGRID_CONFIG_TOPOLOGY_CONFIG_HPP
#define VIENNAGRID_CONFIG_TOPOLOGY_CONFIG_HPP

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

#include "viennagrid/config/config.hpp"
#include "viennagrid/config/id_generator_config.hpp"


#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"
#include "viennagrid/element/element_key.hpp"
#include "viennagrid/config/element_config.hpp"

#include "viennagrid/domain/topology.hpp"

/** @file config/simplex.hpp
    @brief Provides default configuration classes for simplex domains
*/

namespace viennagrid
{
    namespace config
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
            
            
            template<typename element_tag, typename boundary_cell_tag, typename handle_tag>
            struct full_element_config
            {
                typedef typename viennameta::make_typemap<
                
                    element_id_tag,
                    viennagrid::storage::smart_id_tag<int>,
                    
                    element_container_tag,
                    viennagrid::storage::handled_container_tag<           
                        //viennagrid::storage::std_deque_tag,
                        typename default_container_tag<element_tag, boundary_cell_tag>::type,
                        handle_tag
                    >,
                    
                    element_boundary_storage_layout_tag,
                    typename storage_layout_config<element_tag, typename boundary_cell_tag::facet_tag>::type
                
                >::type type;
            };
            
            template<typename element_tag, typename boundary_cell_tag, typename handle_tag>
            struct full_topology_config_helper
            {
                typedef typename viennameta::typemap::result_of::insert<
                    typename full_topology_config_helper<element_tag, typename boundary_cell_tag::facet_tag, handle_tag>::type,
                    viennameta::static_pair<
                        boundary_cell_tag,
                        typename full_element_config<element_tag, boundary_cell_tag, handle_tag>::type
                    >
                >::type type;
            };
            
            template<typename element_tag, typename handle_tag>
            struct full_topology_config_helper<element_tag, viennagrid::vertex_tag, handle_tag>
            {
                typedef typename viennameta::make_typemap<
                    viennagrid::vertex_tag,
                    typename full_element_config<element_tag, viennagrid::vertex_tag, handle_tag>::type
                >::type type;
            };
            
            
            template<typename element_tag, typename handle_tag>
            struct full_topology_config
            {
                typedef typename viennameta::make_typemap<
                    topology_config_tag,
                    typename full_topology_config_helper<element_tag, element_tag, handle_tag>::type
                    >::type type;
            };
            
            
            
            
            
            
            //
            // generates a container for a specified element_tag for the domain container collection
            //
            template<typename something, typename element_tag>
            struct element_container
            {
                typedef something domain_config;
                typedef typename query_config< domain_config, topology_config_tag >::type topology_config;
                
                typedef typename viennagrid::result_of::element<domain_config, element_tag>::type element_type;
                typedef typename viennagrid::storage::result_of::container<element_type, typename query_element_container_tag<domain_config, element_tag>::type >::type type;
            };
            
            template<typename container_collection_typemap, typename element_tag>
            struct element_container< storage::collection_t<container_collection_typemap>, element_tag >
            {
                typedef typename viennagrid::result_of::element<storage::collection_t<container_collection_typemap>, element_tag>::type element_type;
                typedef typename viennameta::typemap::result_of::find< container_collection_typemap, element_type >::type::second type;
                //typedef typename viennagrid::storage::result_of::container<element_type, typename element_container_tag<config, element_tag>::type >::type type;
            };
            
            
            //
            // generates the container typelist for the domain container collection
            //
            template<typename domain_config, typename cur_config = typename config::result_of::query_config<domain_config, config::topology_config_tag>::type >
            struct element_container_typemap;
            
            template<typename domain_config, typename value_tag, typename value_config, typename tail>
            struct element_container_typemap< domain_config, viennameta::typelist_t< viennameta::static_pair<value_tag, value_config>, tail > >
            {
                typedef viennameta::typelist_t<
                    viennameta::static_pair<
                        typename viennagrid::result_of::element<domain_config, value_tag>::type,
                        typename element_container<domain_config, value_tag>::type
                    >,
                    typename element_container_typemap<domain_config, tail>::type
                > type;
            };
            
            template<typename domain_config>
            struct element_container_typemap<domain_config, viennameta::null_type>
            {
                typedef viennameta::null_type type;
            };
            
            
            
            
            
            

        }
    }
    
    
    
    
    
    namespace result_of
    {
        template<typename domain_config>
        struct element_container_collection
        {
//             typedef typename config::result_of::query_config<domain_config, config::topology_config_tag>::type topology_config;
            typedef typename config::result_of::element_container_typemap<domain_config>::type element_container_typelist;
            typedef typename viennagrid::storage::result_of::collection< element_container_typelist >::type type;
        };
        
        
        // topologic domain type from config
        template<typename key_type, typename value_type, typename tail>
        struct topology< viennameta::typelist_t< viennameta::static_pair<key_type, value_type>, tail > >
        {
            typedef viennameta::typelist_t< viennameta::static_pair<key_type, value_type>, tail > domain_config;

            typedef typename element_container_collection< domain_config >::type domain_container_collection_type;
            typedef typename id_generator<domain_config>::type id_generator_type;
            
            typedef typename viennagrid::storage::result_of::physical_inserter<domain_container_collection_type, id_generator_type>::type inserter_type; 
            
            typedef topology_t<domain_container_collection_type, inserter_type> type;
        };
        
        // identity meta functor
        
        template<typename domain_container_collection_type_, typename inserter_type_>
        struct topology< topology_t<domain_container_collection_type_, inserter_type_> >
        {
            typedef topology_t<domain_container_collection_type_, inserter_type_> type;
        };
    }

}


#endif
