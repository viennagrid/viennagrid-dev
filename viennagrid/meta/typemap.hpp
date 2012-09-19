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
                       
            // size
            template<typename typemap>
            struct size
            {
                enum { value = typelist::result_of::size<typemap>::value };
            };
            
            // at
            template<typename typemap, unsigned int index>
            struct at
            {
                typedef typename typelist::result_of::at<typemap, index>::type type;
            };
            
            
            // returns the index of the first key in the typelist
            template <typename typemap, typename to_find> struct index_of;
            
            template <typename to_find>
            struct index_of<null_type, to_find>
            {
                enum { value = -1 };
            };
            
            template <typename to_find, typename value_type, typename tail>
            struct index_of<typelist_t< static_pair<to_find, value_type>, tail>, to_find>
            {
                enum { value = 0 };
            };
            
            template <typename key_type, typename value_type, typename tail, typename to_find>
            struct index_of<typelist_t< static_pair<key_type, value_type>, tail>, to_find>
            {
            private:
                enum { temp = index_of<tail, to_find>::value };
            public:
                enum { value = (temp == -1 ? -1 : 1 + temp) };
            };
            
            
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
            
            
            // insert
            template <typename typemap, typename to_insert> struct insert;
            
            template <>
            struct insert<null_type, null_type>
            {
                typedef null_type type;
            };
            
            template <typename map_key, typename map_value, typename tail>
            struct insert<typelist_t<static_pair<map_key, map_value>, tail>, null_type>
            {
                typedef typelist_t<static_pair<map_key, map_value>, tail> type;
            };
            
            template<typename key, typename value>
            struct insert< null_type, static_pair<key, value> >
            {
                typedef typelist_t<static_pair<key, value>, null_type> type;
            };

            template <typename map_key, typename map_value, typename tail, typename to_insert_key, typename to_insert_value>
            struct insert<typelist_t<static_pair<map_key, map_value>, tail>, static_pair<to_insert_key, to_insert_value> >
            {
                typedef typelist_t<static_pair<map_key, map_value>, tail> typemap;
                
                // !!!!! INFO !!!!!
                // if a compiler error points to the next lines, you might want to insert a key which is already present
                typedef typename _identity_errcheck<
                    typename typelist::result_of::push_back<typemap, static_pair<to_insert_key, to_insert_value> >::type,
                    _equal<
                        not_found,
                        typename find<typemap,to_insert_key>::type
                    >::value
                >::type type;
            };
            
            
            
            // modifies an element
            template<typename typemap, typename to_modify>
            struct modify
            {};
            
            template <typename map_key, typename map_value, typename tail>
            struct modify<typelist_t<static_pair<map_key, map_value>, tail>, null_type>
            {
                typedef typelist_t<static_pair<map_key, map_value>, tail> type;
            };
            
            template<typename key_to_find, typename modified_value>
            struct modify< null_type, static_pair<key_to_find, modified_value> >
            {
                // !!!!! INFO !!!!!
                // if a compiler error points here, your might want to insert an element in an empty map
            };            
            
            template<typename map_key, typename map_value, typename tail, typename key_to_find, typename modified_value>
            struct modify< typelist_t<static_pair<map_key, map_value>, tail>, static_pair<key_to_find, modified_value> >
            {
                typedef typelist_t<static_pair<map_key, map_value>, tail> typemap;
                enum { index = index_of<typemap, key_to_find>::value };
                
                // !!!!! INFO !!!!!
                // if a compiler error points to the next lines, you might want to modify an element which is not present
                typedef typename _identity_errcheck<
                    typename typelist::result_of::replace_at<
                        typemap,
                        index,
                        static_pair<key_to_find, modified_value>
                    >::type,
                index != -1
                >::type type;
            };
            
            
            // modifies or inserts an element
            template<typename typemap, typename to_modify>
            struct insert_or_modify
            {};
            
            template <typename map_key, typename map_value, typename tail>
            struct insert_or_modify<typelist_t<static_pair<map_key, map_value>, tail>, null_type>
            {
                typedef typelist_t<static_pair<map_key, map_value>, tail> type;
            };
            
            template<typename key_to_find, typename modified_value>
            struct insert_or_modify< null_type, static_pair<key_to_find, modified_value> >
            {
                typedef typelist_t<static_pair<key_to_find, modified_value>, null_type> type;
            };
            
            template<typename map_key, typename map_value, typename tail, typename key_to_find, typename modified_value>
            struct insert_or_modify< typelist_t<static_pair<map_key, map_value>, tail>, static_pair<key_to_find, modified_value> >
            {
                typedef typelist_t<static_pair<map_key, map_value>, tail> typemap;
                enum { index = index_of<typemap, key_to_find>::value };
                
                // !!!!! INFO !!!!!
                // if a compiler error points to the next lines, you might want to modify an element which is not present
                typedef typename _if<
                    index == -1,
                    typename typelist::result_of::push_back<typemap, static_pair<key_to_find, modified_value> >::type,
                    typename typelist::result_of::replace_at<typemap, index, static_pair<key_to_find, modified_value> >::type
                >::type type;
            };
            
            
            
            // erases the element with key
            template<typename typemap, typename to_erase>
            struct erase
            {
                enum { index = index_of<typemap, to_erase>::value };
                
                // !!!!! INFO !!!!!
                // if a compiler error points to the next lines, you might want to erase a key which does not exist
                typedef typename _identity_errcheck<
                    typename typelist::result_of::erase_at<
                        typemap,
                        index
                    >::type,
                    index != -1
                >::type type;                
            };
            
            
            // merge two lists
            template<typename to_insert, typename search_result>
            struct merge_helper_error
            {};
            
            template<typename to_insert>
            struct merge_helper_error<to_insert, not_found>
            {
                typedef to_insert type;
            };
            
            template<typename to_insert, typename search_result>
            struct merge_helper_ignore
            {
                typedef search_result type;
            };
            
            template<typename to_insert>
            struct merge_helper_ignore<to_insert, not_found>
            {
                typedef to_insert type;
            };
            
            template<typename to_insert, typename search_result>
            struct merge_helper_overwrite
            {
                typedef to_insert type;
            };
            
            
            template <typename typemap, typename typemap_to_merge, template<typename,typename> class merge_helper> struct merge_impl;
            
            template <template<typename,typename> class merge_helper>
            struct merge_impl<null_type, null_type, merge_helper>
            {
                typedef null_type type;
            };
            
            template <typename key1, typename value1, typename tail1, template<typename,typename> class merge_helper>
            struct merge_impl<typelist_t<static_pair<key1, value1>, tail1>, null_type, merge_helper>
            {
                typedef typelist_t<static_pair<key1, value1>, tail1> type;
            };
            
            template <typename key2, typename value2, typename tail2, template<typename,typename> class merge_helper>
            struct merge_impl<null_type, typelist_t<static_pair<key2, value2>, tail2>, merge_helper>
            {
                typedef typelist_t<static_pair<key2, value2>, tail2> type;
            };

            template <typename key1, typename value1, typename tail1, typename key2, typename value2, typename tail2, template<typename,typename> class merge_helper>
            struct merge_impl<typelist_t<static_pair<key1, value1>, tail1>, typelist_t<static_pair<key2, value2>, tail2>, merge_helper>
            {
                typedef typelist_t<static_pair<key1, value1>, tail1> typemap1;
                typedef typelist_t<static_pair<key2, value2>, tail1> typemap2;
                
                typedef typename find<typemap1, key2>::type search_result;
                
                // !!!!! INFO !!!!!
                // if a compiler error points to the next lines, you might want to merge a two lists with both contain the same key
                typedef typename merge_impl<
                    typename insert_or_modify<
                        typemap1,
                        typename merge_helper<
                            static_pair<key2, value2>,
                            search_result
                        >::type
                    >::type,
                    tail2,
                    merge_helper
                >::type type;
            };
            
            
            
            template <typename typemap, typename typemap_to_merge>
            struct merge
            {
                typedef typename merge_impl<typemap, typemap_to_merge, merge_helper_error>::type type;
            };
            
            template <typename typemap, typename typemap_to_merge>
            struct merge_ignore
            {
                typedef typename merge_impl<typemap, typemap_to_merge, merge_helper_ignore>::type type;
            };
            
            template <typename typemap, typename typemap_to_merge>
            struct merge_overwrite
            {
                typedef typename merge_impl<typemap, typemap_to_merge, merge_helper_overwrite>::type type;
            };
            
            
            // check consistency           
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
                // if a compiler error points to the next lines, your viennamta::map is currupted (e.g. dublicate keys, ...)
                typedef typelist_t<
                    typename _identity_errcheck<
                        static_pair<key,value>,
                        _equal<
                            not_found,
                            typename find<tail, key>::type
                        >::value
                    >::type,
                    typename consistency<tail>::type
                > type;
            };
            
            
        }
        
    }
    
}

#endif