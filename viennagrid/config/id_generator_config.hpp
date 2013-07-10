#ifndef VIENNAGRID_CONFIG_ID_GENERATOR_CONFIG_HPP
#define VIENNAGRID_CONFIG_ID_GENERATOR_CONFIG_HPP

#include "viennagrid/storage/id_generator.hpp"

namespace viennagrid
{
    namespace result_of
    {
        template<typename domain_config, typename id_generator_tag>
        struct id_generator_impl;

        template<typename domain_config>
        struct id_generator_impl<domain_config, config::continuous_id_generator_tag>
        {
            typedef typename viennagrid::storage::result_of::continuous_id_generator_config_from_domain_config<domain_config>::type typemap;
            typedef storage::continuous_id_generator<typemap> type;
        };

        template<typename WrappedConfig>
        struct id_generator
        {
            typedef typename config::result_of::query_config<typename WrappedConfig::type, config::id_generator_tag>::type id_generator_tag;
            typedef typename id_generator_impl<WrappedConfig, id_generator_tag>::type type;
        };
    }
}

#endif
