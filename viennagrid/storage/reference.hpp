#ifndef VIENNAGRID_STORAGE_REFERENCE_HPP
#define VIENNAGRID_STORAGE_REFERENCE_HPP

#include "iterator"
#include "map"
#include "viennagrid/meta/typemap.hpp"


namespace viennagrid
{
    namespace storage
    {
        
        struct pointer_reference_tag {};
        struct iterator_reference_tag {};
        struct id_reference_tag {};
        

        namespace reference
        {
            template<typename base_container_type, typename view_reference_tag>
            struct reference_type
            {};
            
            template<typename base_container_type>
            struct reference_type<base_container_type, pointer_reference_tag>
            {
                typedef typename base_container_type::pointer type;
            };
            
            template<typename base_container_type>
            struct reference_type<base_container_type, iterator_reference_tag>
            {
                typedef typename base_container_type::iterator type;
            };
            
            
            
            
            
            template<typename reference_type>
            struct value_type_from_reference_type;
            
            template<typename value_type>
            struct value_type_from_reference_type< value_type * >
            {
                typedef value_type type;
            };
            
            template<typename category, typename value_type, typename distance, typename pointer, typename reference>
            struct value_type_from_reference_type< std::iterator<category, value_type, distance, pointer, reference> >
            {
                typedef value_type type;
            };
            
                
            template<typename value_type, typename reference_tag_config>
            struct reference_tag_from_config
            {
                typedef typename viennameta::typemap::result_of::find<reference_tag_config, value_type>::type search_result;
                typedef typename viennameta::typemap::result_of::find<reference_tag_config, viennagrid::storage::default_tag>::type default_reference_tag;
                
                typedef typename viennameta::_if<
                    !viennameta::_equal<search_result, viennameta::not_found>::value,
                    search_result,
                    default_reference_tag
                >::type::second type;
            };
            
            template<typename container_type, typename reference_tag_config>
            struct reference_type_from_config
            {
                typedef typename reference_type<
                    container_type,
                    typename reference_tag_from_config<typename container_type::iterator, reference_tag_config>::type
                >::type type;
            };
  
            
            
            
            template<typename iterator_type>
            typename iterator_type::value_type * iterator_to_reference(iterator_type it, pointer_reference_tag)
            { return &* it; }
            
            template<typename iterator_type>
            iterator_type iterator_to_reference(iterator_type it, iterator_reference_tag)
            { return it; }
            
            template<typename iterator_type>
            typename iterator_type::value_type::id_type iterator_to_reference(iterator_type it, id_reference_tag)
            { return it->id(); }

        }
    }
}

#endif