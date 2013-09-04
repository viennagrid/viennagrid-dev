#ifndef VIENNAGRID_CONFIG_TOPOLOGY_CONFIG_HPP
#define VIENNAGRID_CONFIG_TOPOLOGY_CONFIG_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/config/config.hpp"
#include "viennagrid/config/id_generator_config.hpp"


#include "viennagrid/topology/simplex.hpp"
#include "viennagrid/storage/hidden_key_map.hpp"
#include "viennagrid/element/element_key.hpp"
#include "viennagrid/config/element_config.hpp"

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

                typedef typename viennagrid::meta::typemap::result_of::insert<
                    typename storage_layout_config<element_tag, typename boundary_cell_tag::facet_tag>::type,
                    viennagrid::meta::static_pair<
                        boundary_cell_tag,
                        viennagrid::full_handling_tag
                    >
                >::type type;
            };

            template<typename element_tag>
            struct storage_layout_config<element_tag, viennagrid::vertex_tag>
            {
                typedef typename viennagrid::meta::make_typemap<
                    viennagrid::vertex_tag,
                    viennagrid::no_orientation_handling_tag
                >::type type;
            };

            template<typename element_tag>
            struct storage_layout_config<element_tag, viennagrid::meta::null_type>
            {
                typedef viennagrid::meta::null_type type;
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
            
            template<>
            struct default_container_tag<viennagrid::vertex_tag, viennagrid::vertex_tag>
            {
                typedef viennagrid::storage::std_deque_tag type;
            };


            template<typename element_tag, typename boundary_cell_tag, typename handle_tag>
            struct full_element_config
            {
                typedef typename viennagrid::storage::result_of::handled_container<typename default_container_tag<element_tag, boundary_cell_tag>::type,
                                                                                   handle_tag>::tag                     container_tag;

                typedef typename storage_layout_config<element_tag,
                                                       typename boundary_cell_tag::facet_tag>::type   boundary_storage_layout;

                typedef typename viennagrid::meta::make_typemap<
                    viennagrid::config::element_id_tag,
                    viennagrid::storage::smart_id_tag<int>,

                    viennagrid::config::element_container_tag,
                    container_tag,

                    viennagrid::config::element_boundary_storage_layout_tag,
                    boundary_storage_layout,

                    viennagrid::config::element_appendix_type_tag,
                    viennagrid::meta::null_type
                >::type type;
            };

            template<typename element_tag, typename handle_tag>
            struct full_element_config<element_tag, viennagrid::vertex_tag, handle_tag>
            {
                typedef typename viennagrid::storage::result_of::handled_container<typename default_container_tag<element_tag, viennagrid::vertex_tag>::type,
                                                                                   handle_tag>::tag                     container_tag;

                typedef typename viennagrid::meta::make_typemap<
                    viennagrid::config::element_id_tag,
                    viennagrid::storage::smart_id_tag<int>,

                    viennagrid::config::element_container_tag,
                    container_tag,

                    viennagrid::config::element_boundary_storage_layout_tag,
                    viennagrid::meta::null_type,

                    viennagrid::config::element_appendix_type_tag,
                    viennagrid::meta::null_type
                >::type type;
            };




            template<typename element_tag, typename boundary_cell_tag, typename handle_tag>
            struct full_topology_config_helper
            {
                typedef typename viennagrid::meta::typemap::result_of::insert<
                    typename full_topology_config_helper<element_tag, typename boundary_cell_tag::facet_tag, handle_tag>::type,
                    viennagrid::meta::static_pair<
                        boundary_cell_tag,
                        typename full_element_config<element_tag, boundary_cell_tag, handle_tag>::type
                    >
                >::type type;
            };

            template<typename element_tag, typename handle_tag>
            struct full_topology_config_helper<element_tag, viennagrid::vertex_tag, handle_tag>
            {
                typedef typename viennagrid::meta::make_typemap<
                    viennagrid::vertex_tag,
                    typename full_element_config<element_tag, viennagrid::vertex_tag, handle_tag>::type
                >::type type;
            };


            template<typename element_tag, typename handle_tag>
            struct full_topology_config
            {
                typedef typename viennagrid::meta::make_typemap<
                  topology_config_tag,
                  typename full_topology_config_helper<element_tag, element_tag, handle_tag>::type
                >::type type;
            };



            template<typename cell_tag, typename vector_type, typename handle_tag = viennagrid::storage::pointer_handle_tag>
            struct full_domain_config
            {
                typedef typename full_topology_config<cell_tag, handle_tag>::type DomainConfig;
                typedef typename query_config<DomainConfig, topology_config_tag>::type TopologyConfig;
                typedef typename query_config<TopologyConfig, vertex_tag>::type VertexConfig;

                typedef typename viennagrid::meta::typemap::result_of::insert_or_modify<

                  DomainConfig,
                  viennagrid::meta::static_pair<
                      topology_config_tag,
                      typename viennagrid::meta::typemap::result_of::insert_or_modify<

                          TopologyConfig,
                          viennagrid::meta::static_pair<
                              vertex_tag,
                              typename viennagrid::meta::typemap::result_of::insert_or_modify<

                                  VertexConfig,
                                  viennagrid::meta::static_pair<
                                      element_appendix_type_tag,
                                      vector_type
                                  >

                              >::type
                          >

                      >::type
                  >

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
            template<typename WrappedConfigType, typename element_tag>
            struct element_container
            {
                typedef typename result_of::query_config<typename WrappedConfigType::type, topology_config_tag>::type TopologyConfig;

                typedef element_t<element_tag, WrappedConfigType> element_type;
                typedef typename viennagrid::storage::result_of::container<element_type, typename query_element_container_tag<WrappedConfigType, element_tag>::type >::type type;
            };

            template<typename container_collection_typemap, typename element_tag>
            struct element_container< storage::collection_t<container_collection_typemap>, element_tag >
            {
                typedef typename viennagrid::result_of::element<storage::collection_t<container_collection_typemap>, element_tag>::type element_type;
                typedef typename viennagrid::meta::typemap::result_of::find< container_collection_typemap, element_type >::type::second type;
            };


            //
            // generates the container typelist for the domain container collection
            //
            template<typename WrappedConfigType, typename cur_config = typename config::result_of::query_config<typename WrappedConfigType::type, topology_config_tag>::type >
            struct element_container_typemap;

            template<typename domain_config, typename element_tag, typename value_config, typename tail>
            struct element_container_typemap< domain_config, viennagrid::meta::typelist_t< viennagrid::meta::static_pair<element_tag, value_config>, tail > >
            {
                typedef viennagrid::meta::typelist_t<
                    viennagrid::meta::static_pair<
                        element_t<element_tag, domain_config>,
                        typename element_container<domain_config, element_tag>::type
                    >,
                    typename element_container_typemap<domain_config, tail>::type
                > type;
            };

            template<typename domain_config>
            struct element_container_typemap<domain_config, viennagrid::meta::null_type>
            {
                typedef viennagrid::meta::null_type type;
            };

        }

    }

    namespace result_of
    {

        template<typename WrappedConfigType, typename element_tag, typename boundary_element_taglist =
          typename boundary_element_taglist< element_t<element_tag, WrappedConfigType> >::type >
        struct coboundary_container_collection_per_element_typemap;

        template<typename WrappedConfigType, typename element_tag>
        struct coboundary_container_collection_per_element_typemap<WrappedConfigType, element_tag, viennagrid::meta::null_type>
        {
            typedef viennagrid::meta::null_type type;
        };

        template<typename WrappedConfigType, typename element_tag, typename boundary_element_tag, typename tail>
        struct coboundary_container_collection_per_element_typemap<WrappedConfigType, element_tag, viennagrid::meta::typelist_t<boundary_element_tag, tail> >
        {
            typedef typename config::result_of::query_config<typename WrappedConfigType::type, config::domain_change_counter_tag>::type domain_change_counter_type;

            // TODO correctly extract from config
            typedef typename config::result_of::query_config<typename WrappedConfigType::type, config::coboundary_container_tag>::type coboundary_container_tag;
            typedef typename config::result_of::query_config<typename WrappedConfigType::type, config::coboundary_view_container_tag>::type coboundary_view_container_tag;


            typedef typename config::result_of::element_container< WrappedConfigType, element_tag>::type base_element_container;
            typedef typename viennagrid::storage::result_of::view<base_element_container, coboundary_view_container_tag>::type element_view_type;
            typedef typename storage::result_of::container<element_view_type, coboundary_container_tag >::type base_coboundary_container;

            typedef viennagrid::meta::typelist_t<
                viennagrid::meta::static_pair<
                    viennagrid::meta::static_pair<
                        boundary_element_tag,
                        element_tag
                    >,
                    coboundary_container_wrapper<base_coboundary_container, domain_change_counter_type>
                >,
                typename coboundary_container_collection_per_element_typemap<WrappedConfigType, element_tag, tail>::type
            > type;
        };


        template<typename WrappedConfigType,
                 typename element_taglist = typename viennagrid::meta::typemap::result_of::key_typelist< typename config::result_of::query_config<typename WrappedConfigType::type, config::topology_config_tag>::type >::type
            >
        struct coboundary_container_collection_typemap;

        template<typename WrappedConfigType>
        struct coboundary_container_collection_typemap<WrappedConfigType, viennagrid::meta::null_type>
        {
          typedef viennagrid::meta::null_type type;
        };

        template<typename WrappedConfigType, typename element_tag, typename tail>
        struct coboundary_container_collection_typemap<WrappedConfigType, viennagrid::meta::typelist_t<element_tag, tail> >
        {
            typedef typename viennagrid::meta::typemap::result_of::merge<
              typename coboundary_container_collection_per_element_typemap<WrappedConfigType, element_tag>::type,
              typename coboundary_container_collection_typemap<WrappedConfigType, tail>::type
            >::type type;
        };
    }




    namespace result_of
    {
        template<typename WrappedConfigType, typename element_tag, typename boundary_element_taglist =
          typename boundary_element_taglist< element_t<element_tag, WrappedConfigType> >::type >
        struct neighbour_container_collection_per_element_typemap;

        template<typename WrappedConfigType, typename element_tag>
        struct neighbour_container_collection_per_element_typemap<WrappedConfigType, element_tag, viennagrid::meta::null_type>
        {
            typedef viennagrid::meta::null_type type;
        };

        template<typename WrappedConfigType, typename element_tag, typename connector_element_tag, typename tail>
        struct neighbour_container_collection_per_element_typemap<WrappedConfigType, element_tag, viennagrid::meta::typelist_t<connector_element_tag, tail> >
        {
            typedef typename config::result_of::query_config<typename WrappedConfigType::type, config::domain_change_counter_tag>::type domain_change_counter_type;

            // TODO correctly extract from config
            typedef typename config::result_of::query_config<typename WrappedConfigType::type, config::coboundary_container_tag>::type coboundary_container_tag;
            typedef typename config::result_of::query_config<typename WrappedConfigType::type, config::coboundary_view_container_tag>::type coboundary_view_container_tag;


            typedef typename config::result_of::element_container< WrappedConfigType, element_tag>::type base_element_container;
            typedef typename viennagrid::storage::result_of::view<base_element_container, coboundary_view_container_tag>::type element_view_type;
            typedef typename storage::result_of::container<element_view_type, coboundary_container_tag >::type base_container;

            typedef viennagrid::meta::typelist_t<
                viennagrid::meta::static_pair<
                    viennagrid::meta::static_pair<
                        element_tag,
                        connector_element_tag
                    >,
                    neighbour_container_wrapper<base_container, domain_change_counter_type>
                >,
                typename neighbour_container_collection_per_element_typemap<WrappedConfigType, element_tag, tail>::type
            > type;
        };


        template<typename WrappedConfigType,
                 typename element_taglist = typename viennagrid::meta::typemap::result_of::key_typelist< typename config::result_of::query_config<typename WrappedConfigType::type, config::topology_config_tag>::type >::type
            >
        struct neighbour_container_collection_typemap;

        template<typename WrappedConfigType>
        struct neighbour_container_collection_typemap<WrappedConfigType, viennagrid::meta::null_type>
        {
          typedef viennagrid::meta::null_type type;
        };

        template<typename WrappedConfigType, typename element_tag, typename tail>
        struct neighbour_container_collection_typemap<WrappedConfigType, viennagrid::meta::typelist_t<element_tag, tail> >
        {
            typedef typename viennagrid::meta::typemap::result_of::merge<
              typename neighbour_container_collection_per_element_typemap<WrappedConfigType, element_tag>::type,
              typename neighbour_container_collection_typemap<WrappedConfigType, tail>::type
            >::type type;
        };
    }


    namespace result_of
    {
        template<typename typelist>
        struct topologic_cell_dimension;

        template<>
        struct topologic_cell_dimension<viennagrid::meta::null_type>
        {
            static const int value = -1;
        };

        template<typename element_tag, typename tail>
        struct topologic_cell_dimension< viennagrid::meta::typelist_t<element_tag, tail> >
        {
            static const int tail_cell_dimension = topologic_cell_dimension<tail>::value;
            static const int current_element_dimension = element_tag::dim;

            static const int value = (tail_cell_dimension > current_element_dimension) ? tail_cell_dimension : current_element_dimension;
        };




        template<typename typelist, int topologic_dimension>
        struct elements_of_topologic_dim;

        template<int topologic_dimension>
        struct elements_of_topologic_dim< viennagrid::meta::null_type, topologic_dimension >
        {
            typedef viennagrid::meta::null_type type;
        };

        template<typename element_tag, typename tail, int topologic_dimension>
        struct elements_of_topologic_dim< viennagrid::meta::typelist_t<element_tag, tail>, topologic_dimension >
        {
            typedef typename elements_of_topologic_dim<tail, topologic_dimension>::type tail_typelist;

            typedef typename viennagrid::meta::IF<
                element_tag::dim == topologic_dimension,
                typename viennagrid::meta::typelist::result_of::push_back<tail_typelist, element_tag>::type,
                tail_typelist
            >::type type;
        };


        template<typename typelist>
        struct cell_tag_from_typelist
        {
            static const int dim = topologic_cell_dimension<typelist>::value;
            typedef typename elements_of_topologic_dim<typelist, dim>::type cell_types;
            typedef typename viennagrid::meta::typelist::result_of::at<cell_types,0>::type type;
        };






        template<typename WrappedConfigType, typename element_taglist>
        struct boundary_information_collection_typemap_impl;

        template<typename WrappedConfigType>
        struct boundary_information_collection_typemap_impl<WrappedConfigType, viennagrid::meta::null_type>
        {
            typedef viennagrid::meta::null_type type;
        };

        template<typename WrappedConfigType, typename element_tag, typename tail>
        struct boundary_information_collection_typemap_impl<WrappedConfigType, viennagrid::meta::typelist_t<element_tag, tail> >
        {
            typedef typename config::result_of::query_config<typename WrappedConfigType::type, config::domain_change_counter_tag>::type domain_change_counter_type;

          // TODO correctly extract from config
            typedef typename config::result_of::query_config<typename WrappedConfigType::type, config::boundary_information_container_tag>::type boundary_container_tag;

            typedef typename storage::result_of::container<bool, boundary_container_tag >::type base_container;

            typedef viennagrid::meta::typelist_t<
                viennagrid::meta::static_pair<
                    element_tag,
                    boundary_information_wrapper<base_container, domain_change_counter_type>
                >,
                typename boundary_information_collection_typemap_impl<WrappedConfigType, tail>::type
            > type;
        };


        template<typename WrappedConfigType>
        struct boundary_information_collection_typemap
        {
            typedef typename viennagrid::meta::typemap::result_of::key_typelist< typename config::result_of::query_config<typename WrappedConfigType::type, config::topology_config_tag>::type >::type element_taglist;

            typedef typename cell_tag_from_typelist<element_taglist>::type cell_tag;
            typedef typename viennagrid::meta::typelist::result_of::erase< element_taglist, cell_tag>::type element_typelist_without_cell_tag;


            typedef typename boundary_information_collection_typemap_impl<WrappedConfigType, element_typelist_without_cell_tag>::type type;
        };
    }



    namespace config
    {
        namespace result_of
        {
            template<typename WrappedConfigType>
            struct element_collection
            {
                typedef typename config::result_of::element_container_typemap<WrappedConfigType>::type element_container_typelist;
                typedef typename viennagrid::storage::result_of::collection< element_container_typelist >::type type;
            };
        }
    }

}


#endif
