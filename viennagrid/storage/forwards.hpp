#ifndef VIENNAGRID_STORAGE_FORWARDS_HPP
#define VIENNAGRID_STORAGE_FORWARDS_HPP

#include "viennagrid/meta/typemap.hpp"

namespace viennagrid
{
    
    namespace storage
    {

        // handles
        struct no_handle_tag {};
        struct iterator_handle_tag {};
        struct pointer_handle_tag {};
        struct id_handle_tag {};
        
        // container
        struct default_tag;
        
        struct std_vector_tag;
        struct std_deque_tag;
        struct std_list_tag;
        struct std_set_tag;
        
        
        typedef viennagrid::meta::make_typemap<
            default_tag,
            std_deque_tag
        >::type default_container_config;
        
        
        template<typename container_tag_, typename handle_tag_>
        struct handled_container_tag
        {
            typedef container_tag_ container_tag;
            typedef handle_tag_ handle_tag;
        };

        
    }
    
}

#endif
