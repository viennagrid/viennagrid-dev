#ifndef VIENNAGRID_STORAGE_FORWARDS_HPP
#define VIENNAGRID_STORAGE_FORWARDS_HPP

#include "viennagrid/meta/typemap.hpp"

namespace viennagrid
{
    
    namespace storage
    {
        
        // reference
//         struct pointer_reference_tag {};
//         
//         template<typename container_type>
//         struct iterator_reference_tag {};
//         
//         template<typename container_type>
//         struct id_reference_tag {};


        // handles
        struct no_handle_tag;
        struct iterator_handle_tag;
        struct pointer_handle_tag;
        struct id_handle_tag;

        
        
        
        
        // container
        struct default_tag;
        
        struct std_vector_tag;
        struct std_deque_tag;
        struct std_list_tag;
        struct std_set_tag;
        
        
        typedef viennameta::make_typemap<
            default_tag,
            std_deque_tag
        >::type default_container_config;
        
        
        template<typename container_tag__, typename handle_tag__>
        struct handleed_container_tag
        {
            typedef container_tag__ container_tag;
            typedef handle_tag__ handle_tag;
        };

        
    }
    
}

#endif
