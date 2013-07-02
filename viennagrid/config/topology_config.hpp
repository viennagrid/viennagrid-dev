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

//#include "viennagrid/domain/topology.hpp"

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
            
            
            
            template<typename cell_tag, typename vector_type, typename handle_tag = viennagrid::storage::pointer_handle_tag>
            struct full_domain_config
            {
                typedef typename viennagrid::config::result_of::full_topology_config<cell_tag, handle_tag>::type config;
                
                typedef typename viennameta::typemap::result_of::find<config, topology_config_tag>::type::second topology_config;
                typedef typename viennameta::typemap::result_of::find<topology_config, vertex_tag>::type::second vertex_config;
                
                typedef typename viennameta::typemap::result_of::insert_or_modify<
                        vertex_config,
                        viennameta::static_pair< element_appendix_type_tag, vector_type >
                    >::type new_vertex_config;
                    
                typedef typename viennameta::typemap::result_of::insert_or_modify<
                        topology_config,
                        viennameta::static_pair< vertex_tag, new_vertex_config >
                    >::type new_topology_config;
                    
                typedef typename viennameta::make_typemap<
                    topology_config_tag,
                    new_topology_config
                >::type type;
            };
            
            template<typename cell_tag, typename handle_tag>
            struct full_domain_config<cell_tag, void, handle_tag>
            {
                typedef typename viennagrid::config::result_of::full_topology_config<cell_tag, handle_tag>::type type;
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
        template<typename config_element_tag, typename config_element_config, typename config_tail, typename element_tag_>
        struct element< viennameta::typelist_t< viennameta::static_pair<config_element_tag, config_element_config>, config_tail >, element_tag_ >
        {
            typedef viennameta::typelist_t< viennameta::static_pair<config_element_tag, config_element_config>, config_tail > domain_config;
//             typedef typename config::result_of::query_config<domain_config, config::topology_config_tag>::type topology_config;
            typedef typename viennagrid::config::result_of::element_from_config_impl< domain_config, element_tag_ >::type type;
        };
        
        template<typename config_element_tag, typename config_element_config, typename config_tail, typename element_tag_>
        struct handle< viennameta::typelist_t< viennameta::static_pair<config_element_tag, config_element_config>, config_tail >, element_tag_ >
        {
            typedef viennameta::typelist_t< viennameta::static_pair<config_element_tag, config_element_config>, config_tail > domain_config;
            typedef typename config::result_of::element_container<domain_config, element_tag_>::type::handle_type type;
        }; 
        
        template<typename config_element_tag, typename config_element_config, typename config_tail, typename element_tag_>
        struct const_handle< viennameta::typelist_t< viennameta::static_pair<config_element_tag, config_element_config>, config_tail >, element_tag_ >
        {
            typedef viennameta::typelist_t< viennameta::static_pair<config_element_tag, config_element_config>, config_tail > domain_config;
            typedef typename config::result_of::element_container<domain_config, element_tag_>::type::const_handle_type type;
        }; 
    }
    
    
    
    
    namespace result_of
    {
        
        template<typename domain_config, typename element_tag, typename boundary_element_taglist =
          typename boundary_element_taglist< typename element<domain_config, element_tag>::type >::type >
        struct coboundary_container_collection_per_element_typemap;
        
        template<typename domain_config, typename element_tag>
        struct coboundary_container_collection_per_element_typemap<domain_config, element_tag, viennameta::null_type>
        {
            typedef viennameta::null_type type;
        };
        
        template<typename domain_config, typename element_tag, typename boundary_element_tag, typename tail>
        struct coboundary_container_collection_per_element_typemap<domain_config, element_tag, viennameta::typelist_t<boundary_element_tag, tail> >
        {
          // TODO richtig aus der config rauslesen!
            typedef typename config::result_of::query_config<domain_config, config::coboundary_container_tag>::type coboundary_container_tag;
            typedef typename config::result_of::query_config<domain_config, config::coboundary_view_container_tag>::type coboundary_view_container_tag;
            
            
            typedef typename config::result_of::element_container< domain_config, element_tag>::type base_element_container;
            typedef typename viennagrid::storage::result_of::view<base_element_container, coboundary_view_container_tag>::type element_view_type;
            typedef typename storage::result_of::container<element_view_type, coboundary_container_tag >::type base_coboundary_container;
            
            typedef viennameta::typelist_t<
                viennameta::static_pair<
                    viennameta::static_pair<
                        boundary_element_tag,
                        element_tag
                    >,
                    coboundary_container_wrapper<base_coboundary_container>
                >,
                typename coboundary_container_collection_per_element_typemap<domain_config, element_tag, tail>::type
            > type;
        };
        
        
        template<typename domain_config, typename element_taglist =
                typename viennameta::typemap::result_of::key_typelist<
                    typename config::result_of::query_config<domain_config, config::topology_config_tag>::type
                >::type
            >
        struct coboundary_container_collection_typemap;
        
        template<typename domain_config>
        struct coboundary_container_collection_typemap<domain_config, viennameta::null_type>
        {
          typedef viennameta::null_type type;
        };
        
        template<typename domain_config, typename element_tag, typename tail>
        struct coboundary_container_collection_typemap<domain_config, viennameta::typelist_t<element_tag, tail> >
        {
            typedef typename viennameta::typemap::result_of::merge<
              typename coboundary_container_collection_per_element_typemap<domain_config, element_tag>::type,
              typename coboundary_container_collection_typemap<domain_config, tail>::type
            >::type type;
        };
    }
    
    
    
    
    namespace result_of
    {
        template<typename domain_config, typename element_tag, typename boundary_element_taglist =
          typename boundary_element_taglist< typename element<domain_config, element_tag>::type >::type >
        struct neighbour_container_collection_per_element_typemap;
        
        template<typename domain_config, typename element_tag>
        struct neighbour_container_collection_per_element_typemap<domain_config, element_tag, viennameta::null_type>
        {
            typedef viennameta::null_type type;
        };
        
        template<typename domain_config, typename element_tag, typename connector_element_tag, typename tail>
        struct neighbour_container_collection_per_element_typemap<domain_config, element_tag, viennameta::typelist_t<connector_element_tag, tail> >
        {
          // TODO richtig aus der config rauslesen!
            typedef typename config::result_of::query_config<domain_config, config::coboundary_container_tag>::type coboundary_container_tag;
            typedef typename config::result_of::query_config<domain_config, config::coboundary_view_container_tag>::type coboundary_view_container_tag;
            
            
            typedef typename config::result_of::element_container< domain_config, element_tag>::type base_element_container;
            typedef typename viennagrid::storage::result_of::view<base_element_container, coboundary_view_container_tag>::type element_view_type;
            typedef typename storage::result_of::container<element_view_type, coboundary_container_tag >::type base_container;
            
            typedef viennameta::typelist_t<
                viennameta::static_pair<
                    viennameta::static_pair<
                        element_tag,
                        connector_element_tag
                    >,
                    neighbour_container_wrapper<base_container>
                >,
                typename neighbour_container_collection_per_element_typemap<domain_config, element_tag, tail>::type
            > type;
        };
        
        
        template<typename domain_config, typename element_taglist =
                typename viennameta::typemap::result_of::key_typelist<
                    typename config::result_of::query_config<domain_config, config::topology_config_tag>::type
                >::type
            >
        struct neighbour_container_collection_typemap;
        
        template<typename domain_config>
        struct neighbour_container_collection_typemap<domain_config, viennameta::null_type>
        {
          typedef viennameta::null_type type;
        };
        
        template<typename domain_config, typename element_tag, typename tail>
        struct neighbour_container_collection_typemap<domain_config, viennameta::typelist_t<element_tag, tail> >
        {
            typedef typename viennameta::typemap::result_of::merge<
              typename neighbour_container_collection_per_element_typemap<domain_config, element_tag>::type,
              typename neighbour_container_collection_typemap<domain_config, tail>::type
            >::type type;
        };
    }
    
    
    namespace result_of
    {
        template<typename typelist>
        struct topologic_cell_dimension;
        
        template<>
        struct topologic_cell_dimension<viennameta::null_type>
        {
            static const int value = -1;
        };
        
        template<typename element_tag, typename tail>
        struct topologic_cell_dimension< viennameta::typelist_t<element_tag, tail> >
        {
            static const int tail_cell_dimension = topologic_cell_dimension<tail>::value;
            static const int current_element_dimension = element_tag::dim;
            
            static const int value = (tail_cell_dimension > current_element_dimension) ? tail_cell_dimension : current_element_dimension;
        };
        

        
        
        template<typename typelist, int topologic_dimension>
        struct elements_of_topologic_dim;
        
        template<int topologic_dimension>
        struct elements_of_topologic_dim< viennameta::null_type, topologic_dimension >
        {
            typedef viennameta::null_type type;
        };
        
        template<typename element_tag, typename tail, int topologic_dimension>
        struct elements_of_topologic_dim< viennameta::typelist_t<element_tag, tail>, topologic_dimension >
        {
            typedef typename elements_of_topologic_dim<tail, topologic_dimension>::type tail_typelist;
            
            typedef typename viennameta::IF<
                element_tag::dim == topologic_dimension,
                typename viennameta::typelist::result_of::push_back<tail_typelist, element_tag>::type,
                tail_typelist
            >::type type;
        };
        
        
        template<typename typelist>
        struct cell_tag_from_typelist
        {
            static const int dim = topologic_cell_dimension<typelist>::value;
            typedef typename elements_of_topologic_dim<typelist, dim>::type cell_types;
            typedef typename viennameta::typelist::result_of::at<cell_types,0>::type type;
        };

        
      
      
      
        
        template<typename domain_config, typename element_taglist>
        struct boundary_information_collection_typemap_impl;
        
        template<typename domain_config>
        struct boundary_information_collection_typemap_impl<domain_config, viennameta::null_type>
        {
            typedef viennameta::null_type type;
        };
        
        template<typename domain_config, typename element_tag, typename tail>
        struct boundary_information_collection_typemap_impl<domain_config, viennameta::typelist_t<element_tag, tail> >
        {
          // TODO richtig aus der config rauslesen!
            typedef typename config::result_of::query_config<domain_config, config::coboundary_container_tag>::type boundary_container_tag;
            
//             typedef typename config::result_of::element_container< domain_config, element_tag>::type base_element_container;
//             typedef typename viennagrid::storage::result_of::view<base_element_container, coboundary_view_container_tag>::type element_view_type;
            typedef typename storage::result_of::container<bool, boundary_container_tag >::type base_container;
            
            typedef viennameta::typelist_t<
                viennameta::static_pair<
                    element_tag,
                    boundary_information_wrapper<base_container>
                >,
                typename boundary_information_collection_typemap_impl<domain_config, tail>::type
            > type;
        };
        
        
        template<typename domain_config>
        struct boundary_information_collection_typemap
        {
            typedef typename viennameta::typemap::result_of::key_typelist<
                    typename config::result_of::query_config<domain_config, config::topology_config_tag>::type
                >::type element_taglist;
                
            typedef typename cell_tag_from_typelist<element_taglist>::type cell_tag;
            typedef typename viennameta::typelist::result_of::erase< element_taglist, cell_tag>::type element_typelist_without_cell_tag;
          
          
            typedef typename boundary_information_collection_typemap_impl<domain_config, element_typelist_without_cell_tag>::type type;
        };
    }
    
    
    
    
    namespace result_of
    {
        template<typename key_type, typename value_type, typename tail>
        struct element_collection< viennameta::typelist_t< viennameta::static_pair<key_type, value_type>, tail> >
        {
            typedef viennameta::typelist_t< viennameta::static_pair<key_type, value_type>, tail> domain_config;
            typedef typename config::result_of::element_container_typemap<domain_config>::type element_container_typelist;
            typedef typename viennagrid::storage::result_of::collection< element_container_typelist >::type type;
        };
        
        // topologic domain type from config
        /*
        template<typename key_type, typename value_type, typename tail>
        struct domain< viennameta::typelist_t< viennameta::static_pair<key_type, value_type>, tail > >
        {
            typedef viennameta::typelist_t< viennameta::static_pair<key_type, value_type>, tail > domain_config;
//             typedef typename config::result_of::query_config< domain_config, config::topology_config_tag>::type topology_config;

            typedef typename element_collection< domain_config >::type element_collection_type;
            typedef typename viennagrid::storage::result_of::collection< typename coboundary_container_collection_typemap< domain_config >::type >::type coboundary_collection_type;
            
            typedef typename viennagrid::storage::result_of::collection< typename neighbour_container_collection_typemap< domain_config >::type >::type neighbour_collection_type;
            
            typedef typename viennagrid::storage::result_of::collection< typename boundary_information_collection_typemap<domain_config>::type >::type boundary_information_type;
            
            typedef typename id_generator<domain_config>::type id_generator_type;
            typedef typename viennagrid::storage::result_of::physical_inserter<element_collection_type, id_generator_type>::type inserter_type;
            
            
            typedef typename viennagrid::storage::collection_t<
                typename viennameta::make_typemap<
                
                    coboundary_collection_tag,
                    coboundary_collection_type,
                    
                    neighbour_collection_tag,
                    neighbour_collection_type,
                    
                    boundary_information_collection_tag,
                    boundary_information_type
                    
                >::type
            > appendix_type;

            
            
            typedef domain_t<element_collection_type, appendix_type, inserter_type> type;
        };
        
        // identity meta functor
        template<typename A, typename B, typename C>
        struct domain< domain_t<A, B, C> >
        {
            typedef domain_t<A, B, C> type;
        }; */
    }

}


#endif
