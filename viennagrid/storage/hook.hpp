#ifndef VIENNAGRID_STORAGE_HOOK_HPP
#define VIENNAGRID_STORAGE_HOOK_HPP

#include <iterator>
#include <vector>
#include <map>
#include "viennagrid/meta/typemap.hpp"
#include "viennagrid/storage/id.hpp"
#include "viennagrid/storage/forwards.hpp"


namespace viennagrid
{
    namespace storage
    {
        

        namespace hook
        {
            
            template<typename base_container_type, typename view_reference_tag>
            struct hook_type
            {};
            
            template<typename base_container_type>
            struct hook_type<base_container_type, no_hook_tag>
            {
                typedef viennameta::null_type type;
            };
            
            template<typename base_container_type>
            struct hook_type<base_container_type, pointer_hook_tag>
            {
                typedef typename base_container_type::pointer type;
            };
            
            template<typename base_container_type>
            struct hook_type<base_container_type, iterator_hook_tag>
            {
                typedef typename base_container_type::iterator type;
            };
            
            template<typename base_container_type>
            struct hook_type<base_container_type, id_hook_tag>
            {
                typedef typename base_container_type::value_type::id_type type;
            };
            
            
            
            
            template<typename base_container_type, typename view_reference_tag>
            struct const_hook_type
            {};
            
            template<typename base_container_type>
            struct const_hook_type<base_container_type, no_hook_tag>
            {
                typedef const viennameta::null_type type;
            };
            
            template<typename base_container_type>
            struct const_hook_type<base_container_type, pointer_hook_tag>
            {
                typedef typename base_container_type::const_pointer type;
            };
            
            template<typename base_container_type>
            struct const_hook_type<base_container_type, iterator_hook_tag>
            {
                typedef typename base_container_type::const_iterator type;
            };
            
            template<typename base_container_type>
            struct const_hook_type<base_container_type, id_hook_tag>
            {
                typedef const typename base_container_type::value_type::id_type type;
            };
            
            
            
            
           
            
            
            
            template<typename container_type, typename hook_tag>
            struct iterator_to_hook;
            
            template<typename container_type>
            struct iterator_to_hook<container_type, no_hook_tag>
            {
                typedef typename viennagrid::storage::hook::hook_type<container_type, no_hook_tag>::type hook_type;
                
                template<typename iterator>
                static hook_type convert( iterator it ) { return hook_type(); }
            };
            
            template<typename container_type>
            struct iterator_to_hook<container_type, iterator_hook_tag>
            {
                typedef typename viennagrid::storage::hook::hook_type<container_type, iterator_hook_tag>::type hook_type;
                
                static hook_type convert( hook_type it ) { return it; }
            };
            
            template<typename container_type>
            struct iterator_to_hook<container_type, pointer_hook_tag>
            {
                typedef typename viennagrid::storage::hook::hook_type<container_type, pointer_hook_tag>::type hook_type;
                
                template<typename iterator>
                static hook_type convert( iterator it ) { return &* it; }
            };
            
            template<typename container_type>
            struct iterator_to_hook<container_type, id_hook_tag>
            {
                typedef typename viennagrid::storage::hook::hook_type<container_type, id_hook_tag>::type hook_type;
                
                template<typename iterator>
                static hook_type convert( iterator it ) { return it->id(); }
            };
            
            
            
            
          
            template<typename hook_type>
            struct value_type
            {
                typedef typename hook_type::value_type type;
            };
            
            template<typename value_type_>
            struct value_type< value_type_ * >
            {
                typedef value_type_ type;
            };
            
            template<typename value_type_>
            struct value_type< const value_type_ * >
            {
                typedef const value_type_ type;
            };
            
//             template<typename category, typename value_type, typename distance, typename pointer, typename reference>
//             struct value_type_from_reference_type< std::iterator<category, value_type, distance, pointer, reference> >
//             {
//                 typedef value_type type;
//             };
//             
                
//             template<typename value_type, typename reference_tag_config>
//             struct reference_tag_from_config
//             {
//                 typedef typename viennameta::typemap::result_of::find<reference_tag_config, value_type>::type search_result;
//                 typedef typename viennameta::typemap::result_of::find<reference_tag_config, viennagrid::storage::default_tag>::type default_reference_tag;
//                 
//                 typedef typename viennameta::_if<
//                     !viennameta::_equal<search_result, viennameta::not_found>::value,
//                     search_result,
//                     default_reference_tag
//                 >::type::second type;
//             };
//             
//             template<typename container_type, typename reference_tag_config>
//             struct reference_type_from_config
//             {
//                 typedef typename reference_type<
//                     container_type,
//                     typename reference_tag_from_config<typename container_type::iterator, reference_tag_config>::type
//                 >::type type;
//             };
  
            
            
            
//             template<typename iterator_type>
//             typename iterator_type::value_type * iterator_to_reference(iterator_type it, pointer_reference_tag)
//             { return &* it; }
//             
//             template<typename iterator_type>
//             iterator_type iterator_to_reference(iterator_type it, iterator_reference_tag)
//             { return it; }
//             
//             template<typename iterator_type>
//             typename iterator_type::value_type::id_type iterator_to_reference(iterator_type it, id_reference_tag)
//             { return it->id(); }
            
            
            
//             template<typename dst_reference_type>
//             struct reference_converter
//             {
//                 static dst_reference_type convert( dst_reference_type ref )
//                 {
//                     return ref;
//                 }
//             };
//             
//             template<typename value_type>
//             struct reference_converter<value_type *>
//             {
//                 template<typename iterator_type>
//                 static value_type * convert( iterator_type it )
//                 {
//                     typedef typename iterator_type::iterator_category tmp;
//                     return &* it;
//                 }
//             };

        }
    }
}

#endif
