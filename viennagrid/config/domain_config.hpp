#ifndef VIENNAGRID_CONFIG_DOMAIN_CONFIG_HPP
#define VIENNAGRID_CONFIG_DOMAIN_CONFIG_HPP

#include "viennagrid/config/config.hpp"
#include "viennagrid/config/topology_config.hpp"

namespace viennagrid
{
    namespace config
    {    
        namespace result_of
        {
            template<typename domain_config>
            struct vector_type
            {
                typedef typename viennameta::typemap::result_of::find<domain_config, vector_type_tag>::type::second type;
            };
            
            template<typename domain_config>
            struct metainfo_typelist
            {
                typedef typename viennameta::typemap::result_of::find<domain_config, metainfo_typelist_tag>::type::second type;
            };
            
            template<typename domain_config>
            struct metainfo_container_config
            {
                typedef typename viennameta::typemap::result_of::find<domain_config, metainfo_container_config_tag>::type::second type;
            };
            
            
            template<typename element_tag, typename vector_type, typename handle_tag = viennagrid::storage::pointer_handle_tag,
                typename metainfo_typelist = viennameta::null_type, typename metainfo_container_config = viennagrid::storage::default_container_config>
            struct full_domain_config
            {
                typedef typename viennagrid::config::result_of::full_topology_config<element_tag, handle_tag>::type topology_config;
                
                typedef typename viennameta::typemap::result_of::merge<
                    topology_config,
                    typename viennameta::make_typemap<
                        vector_type_tag, vector_type,
                        metainfo_typelist_tag, metainfo_typelist,
                        metainfo_container_config_tag, metainfo_container_config
                    >::type
                >::type type;
            };
        }
    }
    
}

#endif