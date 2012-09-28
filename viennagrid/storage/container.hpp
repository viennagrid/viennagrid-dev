#ifndef VIENNAGRID_STORAGE_CONTAINER_HPP
#define VIENNAGRID_STORAGE_CONTAINER_HPP

#include <vector>
#include <deque>
#include <list>
#include <set>

#include "viennagrid/meta/utils.hpp"

namespace viennagrid
{
    namespace storage
    {
        struct default_tag;
        
        struct std_vector_tag;
        struct std_deque_tag;
        struct std_list_tag;
        struct std_set_tag;
        
        
        
        namespace result_of
        {
            template<typename element_type, typename container_tag>
            struct container_from_tag;
            
            template<typename element_type>
            struct container_from_tag<element_type, std_vector_tag>
            {
                typedef std::vector<element_type> type;
            };
            
            template<typename element_type>
            struct container_from_tag<element_type, std_deque_tag>
            {
                typedef std::deque<element_type> type;
            };
            
            template<typename element_type>
            struct container_from_tag<element_type, std_list_tag>
            {
                typedef std::list<element_type> type;
            };
            
            template<typename element_type>
            struct container_from_tag<element_type, std_set_tag>
            {
                typedef std::set<element_type> type;
            };
        }
        
        namespace container
        {
            template<typename container_type>
            typename container_type::iterator insert( container_type & container, const typename container_type::value_type & element  )
            {
                container.push_back( element );
                return --container.end();
            }
            
            template<typename key, typename compare, typename allocator>
            typename std::set<key, compare, allocator>::iterator insert( std::set<key, compare, allocator> & container, const key & element )
            {
                return container.insert( element ).first;
            }
        }
    
    }
}
        
#endif