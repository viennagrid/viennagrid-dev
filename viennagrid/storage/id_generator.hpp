#ifndef VIENNAGRID_STORAGE_ID_GENERATOR_HPP
#define VIENNAGRID_STORAGE_ID_GENERATOR_HPP

#include "viennagrid/meta/typelist.hpp"


namespace viennagrid
{

    namespace storage
    {
     
        
        
        template<typename typelist, typename id_type>
        class continuous_id_generator_layer_t;
        
        template<typename head, typename tail, typename id_type>
        class continuous_id_generator_layer_t<viennameta::typelist_t<head, tail>, id_type> : public continuous_id_generator_layer_t<tail, id_type>
        {
            typedef continuous_id_generator_layer_t<tail, id_type> base;
        public:
            continuous_id_generator_layer_t() : base(), last_id(0) {}
            
            using base::operator();
            id_type operator()( viennameta::tag<head> )
            {
                return last_id++;
            }
            
        private:
            id_type last_id;
        };
        
        template<typename id_type>
        class continuous_id_generator_layer_t<viennameta::null_type, id_type>
        {
        public:
            id_type operator()();
        };
        
        template<typename typelist, typename id_type>
        class continuous_id_generator_t : public continuous_id_generator_layer_t<typelist, id_type>
        {
            typedef continuous_id_generator_layer_t<typelist, id_type> base;
            
        public:
            using base::operator();
        };
        
        
        
        
        namespace result_of
        {
            
            template<typename typelist, typename id_type = int>
            struct continuous_id_generator_layer
            {
                typedef continuous_id_generator_t<
                    typename viennameta::typelist::result_of::no_duplicates< typelist >::type,
                    id_type
                > type;
            };
            
            
        }
        
    }
    
}

#endif