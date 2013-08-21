#ifndef VIENNAGRID_CONFIG_HPP
#define VIENNAGRID_CONFIG_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennagrid/meta/typemap.hpp"
#include "viennagrid/storage/forwards.hpp"

namespace viennagrid
{

    namespace config
    {
        struct topology_config_tag;

        struct element_id_tag;
        struct element_container_tag;
        struct element_boundary_storage_layout_tag;
        struct element_appendix_type_tag;

        struct id_generator_tag;
        struct continuous_id_generator_tag;

        struct coboundary_container_tag;
        struct coboundary_view_container_tag;

        struct boundary_information_container_tag;

        struct interface_information_container_tag;

        struct vector_type_tag;

        struct domain_change_counter_tag;


        namespace result_of
        {
            template<typename config_tag>
            struct default_config
            {
              typedef viennagrid::meta::null_type type;
            };


            template<>
            struct default_config<element_id_tag>
            {
                typedef viennagrid::storage::smart_id_tag<int> type;
            };

            template<>
            struct default_config<element_container_tag>
            {
                typedef viennagrid::storage::handled_container_tag<viennagrid::storage::std_deque_tag, viennagrid::storage::pointer_handle_tag> type;
            };

            template<>
            struct default_config<id_generator_tag>
            {
                typedef continuous_id_generator_tag type;
            };

            template<>
            struct default_config<element_appendix_type_tag>
            {
                typedef viennagrid::meta::null_type type;
            };

            template<>
            struct default_config<coboundary_container_tag>
            {
                typedef storage::std_vector_tag type;
            };

            template<>
            struct default_config<coboundary_view_container_tag>
            {
                typedef storage::std_vector_tag type;
            };

            template<>
            struct default_config<boundary_information_container_tag>
            {
                typedef storage::std_vector_tag type;
            };

            template<>
            struct default_config<interface_information_container_tag>
            {
                typedef storage::std_vector_tag type;
            };

            template<>
            struct default_config<domain_change_counter_tag>
            {
                typedef long type;
            };

        }

        namespace result_of
        {
            template<typename config_entry, typename config_tag>
            struct query_config_impl
            {
                typedef typename config_entry::second type;
            };

            template<typename config_tag>
            struct query_config_impl< viennagrid::meta::not_found, config_tag >
            {
                typedef typename default_config<config_tag>::type type;
            };


            template<typename ConfigType, typename ConfigTag>
            struct query_config
            {
                typedef typename viennagrid::meta::typemap::result_of::find<ConfigType, ConfigTag>::type found;
                typedef typename query_config_impl<found, ConfigTag>::type type;
            };
        }
    }

}


#endif
