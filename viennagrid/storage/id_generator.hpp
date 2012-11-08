#ifndef VIENNAGRID_STORAGE_ID_GENERATOR_HPP
#define VIENNAGRID_STORAGE_ID_GENERATOR_HPP

#include "viennagrid/meta/typelist.hpp"
#include "viennagrid/meta/typemap.hpp"
#include "id.hpp"


namespace viennagrid
{

    namespace storage
    {
     
        
        
        template<typename typemap>
        class continuous_id_generator_layer_t;
        
        template<typename value_type, typename id_type, typename tail>
        class continuous_id_generator_layer_t<viennameta::typelist_t< viennameta::static_pair<value_type, id_type>, tail> > : public continuous_id_generator_layer_t<tail>
        {
            typedef continuous_id_generator_layer_t<tail> base;
        public:
            continuous_id_generator_layer_t() : base(), last_id(0) {}
            
            using base::operator();
            id_type operator()( viennameta::tag<value_type> )
            {
                return last_id++;
            }
            
        private:
            id_type last_id;
        };
        
        template<>
        class continuous_id_generator_layer_t<viennameta::null_type>
        {
        public:
            void operator()();
        };
        
        
        namespace result_of
        {
            template<typename typelist, typename id_type>
            struct continuous_id_generator_config_helper;
            
            template<typename head, typename tail, typename id_type>
            struct continuous_id_generator_config_helper< viennameta::typelist_t<head, tail>, id_type >
            {
                typedef typename viennameta::typemap::result_of::insert<
                    typename continuous_id_generator_config_helper<tail, id_type>::type,
                    viennameta::static_pair<
                        head,
                        id_type
                    >
                >::type type;
            };
            
            template<typename id_type>
            struct continuous_id_generator_config_helper<viennameta::null_type, id_type>
            {
                typedef viennameta::null_type type;
            };
            
            template<typename typelist, typename id_type>
            struct continuous_id_generator_config
            {
                typedef typename continuous_id_generator_config_helper<
                    typename viennameta::typelist::result_of::no_duplicates<typelist>::type,
                    id_type
                >::type type;
            };
            
            
            template<typename typemap>
            struct continuous_id_generator
            {
                typedef viennagrid::storage::continuous_id_generator_layer_t< typemap > type;
            };
            
            
        }
        
    }
    
}

#endif