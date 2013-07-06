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
        
        typedef viennagrid::meta::make_typemap<
            default_tag,
            std_set_tag
        >::type default_view_container_config;
        
        
        template<typename container_tag_, typename handle_tag_>
        struct handled_container_tag
        {
            typedef container_tag_   container_tag;
            typedef handle_tag_      handle_tag;
        };

        // vector
        struct std_vector_with_iterator_handle_tag
        {
            typedef std_vector_tag         container_tag;
            typedef iterator_handle_tag    handle_tag;
        };
        struct std_vector_with_pointer_handle_tag
        {
            typedef std_vector_tag         container_tag;
            typedef pointer_handle_tag     handle_tag;
        };
        struct std_vector_with_id_handle_tag
        {
            typedef std_vector_tag         container_tag;
            typedef id_handle_tag          handle_tag;
        };

        // deque
        struct std_deque_with_iterator_handle_tag
        {
            typedef std_deque_tag          container_tag;
            typedef iterator_handle_tag    handle_tag;
        };
        struct std_deque_with_pointer_handle_tag
        {
            typedef std_deque_tag          container_tag;
            typedef pointer_handle_tag     handle_tag;
        };
        struct std_deque_with_id_handle_tag
        {
            typedef std_deque_tag          container_tag;
            typedef id_handle_tag          handle_tag;
        };

        // list
        struct std_list_with_iterator_handle_tag
        {
            typedef std_list_tag           container_tag;
            typedef iterator_handle_tag    handle_tag;
        };
        struct std_list_with_pointer_handle_tag
        {
            typedef std_list_tag           container_tag;
            typedef pointer_handle_tag     handle_tag;
        };
        struct std_list_with_id_handle_tag
        {
            typedef std_list_tag           container_tag;
            typedef id_handle_tag          handle_tag;
        };

        // set
        struct std_set_with_iterator_handle_tag
        {
            typedef std_set_tag            container_tag;
            typedef iterator_handle_tag    handle_tag;
        };
        struct std_set_with_pointer_handle_tag
        {
            typedef std_set_tag            container_tag;
            typedef pointer_handle_tag     handle_tag;
        };
        struct std_set_with_id_handle_tag
        {
            typedef std_set_tag            container_tag;
            typedef id_handle_tag          handle_tag;
        };

        namespace result_of
        {
          template <typename ContainerTag, typename HandleTag>
          struct handled_container
          {
              typedef handled_container_tag<ContainerTag, HandleTag>   tag;
          };

          // vector
          template <>
          struct handled_container<std_vector_tag, iterator_handle_tag>
          {
              typedef std_vector_with_iterator_handle_tag   tag;
          };

          template <>
          struct handled_container<std_vector_tag, pointer_handle_tag>
          {
              typedef std_vector_with_pointer_handle_tag   tag;
          };

          template <>
          struct handled_container<std_vector_tag, id_handle_tag>
          {
              typedef std_vector_with_id_handle_tag   tag;
          };


          // deque
          template <>
          struct handled_container<std_deque_tag, iterator_handle_tag>
          {
              typedef std_deque_with_iterator_handle_tag   tag;
          };

          template <>
          struct handled_container<std_deque_tag, pointer_handle_tag>
          {
              typedef std_deque_with_pointer_handle_tag   tag;
          };

          template <>
          struct handled_container<std_deque_tag, id_handle_tag>
          {
              typedef std_deque_with_id_handle_tag   tag;
          };


          // list
          template <>
          struct handled_container<std_list_tag, iterator_handle_tag>
          {
              typedef std_list_with_iterator_handle_tag   tag;
          };

          template <>
          struct handled_container<std_list_tag, pointer_handle_tag>
          {
              typedef std_list_with_pointer_handle_tag   tag;
          };

          template <>
          struct handled_container<std_list_tag, id_handle_tag>
          {
              typedef std_list_with_id_handle_tag   tag;
          };


          // set
          template <>
          struct handled_container<std_set_tag, iterator_handle_tag>
          {
              typedef std_set_with_iterator_handle_tag   tag;
          };

          template <>
          struct handled_container<std_set_tag, pointer_handle_tag>
          {
              typedef std_set_with_pointer_handle_tag   tag;
          };

          template <>
          struct handled_container<std_set_tag, id_handle_tag>
          {
              typedef std_set_with_id_handle_tag   tag;
          };


        }
        
    }
    
}

#endif
