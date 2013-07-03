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
//             template<typename domain_config>
//             struct vector_type
//             {
//                 typedef typename viennagrid::meta::typemap::result_of::find<domain_config, vector_type_tag>::type::second type;
//             };
//             
//             template<typename domain_config>
//             struct metainfo_typelist
//             {
//                 typedef typename viennagrid::meta::typemap::result_of::find<domain_config, metainfo_typelist_tag>::type::second type;
//             };
//             
//             template<typename domain_config>
//             struct metainfo_container_config
//             {
//                 typedef typename viennagrid::meta::typemap::result_of::find<domain_config, metainfo_container_config_tag>::type::second type;
//             };
//             
//             
//             template<typename cell_tag, typename vector_type, typename handle_tag = viennagrid::storage::pointer_handle_tag,
//                 typename metainfo_typelist = viennagrid::meta::null_type, typename metainfo_container_config = viennagrid::storage::default_container_config>
//             struct full_domain_config
//             {
//                 typedef typename viennagrid::config::result_of::full_topology_config<cell_tag, handle_tag>::type topology_config;
//                 
//                 typedef typename viennagrid::meta::typelist::result_of::push_back<
//                     metainfo_typelist,
//                     viennagrid::meta::static_pair<
//                         viennagrid::vertex_tag,
//                         vector_type
//                     >
//                 >::type metainfo_typelist_with_vector_type;
//                 
//                 
// //                 typedef typename viennagrid::meta::make_typelist<
// //                     viennagrid::meta::static_pair<
// //                         viennagrid::vertex_tag,
// //                         vector_type
// //                     >
// //                 >::type metainfo_typelist_with_vector_type;
//                 
//                 typedef typename viennagrid::meta::typemap::result_of::merge<
//                     topology_config,
//                     typename viennagrid::meta::make_typemap<
//                         vector_type_tag, vector_type,
//                         metainfo_typelist_tag, metainfo_typelist_with_vector_type,
//                         metainfo_container_config_tag, metainfo_container_config
//                     >::type
//                 >::type type;
//             };
            
            
            
            

        }
    }
    
}

#endif
