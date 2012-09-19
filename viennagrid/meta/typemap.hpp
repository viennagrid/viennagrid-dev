#ifndef VIENNAMETA_TYPEMAP_HPP
#define VIENNAMETA_TYPEMAP_HPP


#include "utils.hpp"
#include "typelist.hpp"
#include "typemap_macros.hpp"

namespace viennameta
{
    
    namespace typemap
    {
        
        namespace result_of
        {
            
            // find a key
            template<typename typemap, typename to_find>
            struct find
            {};
            
            template<typename to_find>
            struct find< null_type, to_find >
            {
                typedef not_found type;                
            };
            
            template<typename value, typename tail, typename to_find>
            struct find< typelist_t<static_pair<to_find, value>, tail>, to_find>
            {
                typedef static_pair<to_find, value> type;
            };
            
            template<typename key, typename value, typename tail, typename to_find>
            struct find< typelist_t<static_pair<key, value>, tail>, to_find>
            {
                typedef typename find<tail, to_find>::type type;
            };
            
            
            // check consistency
            
            template<typename key, typename value, typename search_result>
            struct consistency_helper {};
            
            template<typename key, typename value>
            struct consistency_helper< key, value, not_found >
            {
                typedef static_pair<key, value> type;
            };
            
            template<typename key, typename value1, typename value2>
            struct consistency_helper< key, value1, static_pair<key, value2> >
            {};
            
            
            template<typename typemap>
            struct consistency {};

            template<>
            struct consistency<null_type>
            {
                typedef null_type type;                
            };

            
            template<typename key, typename value, typename tail>
            struct consistency< typelist_t<static_pair<key, value>, tail> >
            {
                // !!!!! INFO !!!!!
                // if a compiler error points to the next line, your viennamta::map is currupted (e.g. dublicate keys, ...)
                typedef typelist_t< typename consistency_helper< key, value, typename find<tail, key>::type>::type, typename consistency<tail>::type > type;
            };
            
            
        }
        
    }
    
}

#endif