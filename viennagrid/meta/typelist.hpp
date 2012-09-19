#ifndef VIENNAMETA_TYPELIST_HPP
#define VIENNAMETA_TYPELIST_HPP


#include "utils.hpp"
#include "typelist_macros.hpp"


namespace viennameta
{

    template <class head_, class tail_>
    struct typelist_t
    {
       typedef head_ head;
       typedef tail_ tail;
    };

      
    
    namespace typelist
    {
        namespace result_of
        {
            
            // size of a typelist
            template <typename typelist> struct size;
            
            template <> struct size<null_type>
            {
                enum { value = 0 };
            };
            
            template <typename head, typename tail>
            struct size< typelist_t<head, tail> >
            {
                enum { value = 1 + size<tail>::value };
            };
            
            
            // index access
            template <typename typelist, unsigned int index> struct at
            {
                typedef out_of_range type;
            };
            
            template <typename head, typename tail>
            struct at<typelist_t<head, tail>, 0>
            {
                typedef head type;
            };

            template <typename head, typename tail, unsigned int i>
            struct at<typelist_t<head, tail>, i>
            {
                typedef typename at<tail, i - 1>::type type;
            };
            
            
            
            // find first type
            template <typename typelist, typename to_find> struct find;
            
            template <typename to_find>
            struct find<null_type, to_find>
            {
                enum { value = -1 };
            };
            
            template <typename to_find, typename tail>
            struct find<typelist_t<to_find, tail>, to_find>
            {
                enum { value = 0 };
            };
            
            template <typename head, typename tail, typename to_find>
            struct find<typelist_t<head, tail>, to_find>
            {
            private:
                enum { temp = find<tail, to_find>::value };
            public:
                enum { value = (temp == -1 ? -1 : 1 + temp) };
            };
            
            
            
            // push back a single type
            template <typename typelist, typename to_add> struct push_back;
            
            template <>
            struct push_back<null_type, null_type>
            {
                typedef null_type type;
            };
            
            template <typename to_add>
            struct push_back<null_type, to_add>
            {
                typedef typelist_t<to_add, null_type> type;
            };

            template <typename head, typename tail, typename to_add>
            struct push_back<typelist_t<head, tail>, to_add>
            {
                typedef typelist_t<head, typename push_back<tail, to_add>::type> type;
            };
            
            
            
            // push back list
            template <typename typelist, typename typelist_to_add> struct push_back_list;
            
            template <>
            struct push_back_list<null_type, null_type>
            {
                typedef null_type type;
            };
            
            template <typename head, typename tail>
            struct push_back_list<null_type, typelist_t<head, tail> >
            {
                typedef typelist_t<head, tail> type;
            };

            template <typename head, typename tail>
            struct push_back_list<typelist_t<head, tail>, null_type >
            {
                typedef typelist_t<head, tail> type;
            };
            
            template <typename head1, typename tail1, typename head2, typename tail2>
            struct push_back_list<typelist_t<head1, tail1>, typelist_t<head2, tail2> >
            {
                typedef typename push_back_list< typename push_back< typelist_t<head1, tail1>, head2>::type, tail2 >::type type;
            };

            
            
            // erase_at the type at index
            template <typename typelist, unsigned int index_to_erase> struct erase_at;
            
            template <unsigned int index_to_erase>
            struct erase_at<null_type, index_to_erase>
            {
                typedef null_type type;
            };

            template <typename head, typename tail>
            struct erase_at< typelist_t<head, tail>, 0>
            {
                typedef tail type;
            };
            
            template <typename head, typename tail, unsigned int index_to_erase>
            struct erase_at< typelist_t<head, tail>, index_to_erase>
            {
                typedef typelist_t<head, typename erase_at<tail, index_to_erase-1>::type> type;
            };
            
            
            
            // erase the first type
            template <typename typelist, typename to_erase> struct erase;
            
            template <typename to_erase>
            struct erase<null_type, to_erase>
            {
                typedef null_type type;
            };

            template <typename to_erase, typename tail>
            struct erase< typelist_t<to_erase, tail>, to_erase>
            {
                typedef tail type;
            };

            template <typename head, typename tail, typename to_erase>
            struct erase< typelist_t<head, tail>, to_erase>
            {
                typedef typelist_t<head, typename erase<tail, to_erase>::type> type;
            };
            
            
            // erase all types
            template <typename typelist, typename to_erase>
            struct erase_all;
            
            template <typename to_erase>
            struct erase_all<null_type, to_erase>
            {
                typedef null_type type;
            };
            
            template <typename to_erase, typename tail>
            struct erase_all< typelist_t<to_erase, tail>, to_erase>
            {
                // Go all the way down the list removing the type
                typedef typename erase_all<tail, to_erase>::type type;
            };
            template <typename head, typename tail, typename to_erase>
            
            struct erase_all< typelist_t<head, tail>, to_erase>
            {
                // Go all the way down the list removing the type
                typedef typelist_t<head, typename erase_all<tail, to_erase>::type> type;
            };
            
            
            // no duplicate types
            template <typename typelist> struct no_duplicates;
            
            template <> struct
            no_duplicates<null_type>
            {
                typedef null_type type;
            };

            template <typename head, typename tail>
            struct no_duplicates< typelist_t<head, tail> >
            {
            private:
                typedef typename no_duplicates<tail>::type L1;
                typedef typename erase<L1, head>::type L2;
            public:
                typedef typelist_t<head, L2> type;
            };
            

            // replace the type at index by another type
            template <typename typelist, unsigned int index_to_replace, typename replaced> struct replace_at;
            
            template <unsigned int index_to_replace, typename replaced>
            struct replace_at<null_type, index_to_replace, replaced>
            {
                typedef null_type type;
            };

            template <typename head, typename tail, typename replaced>
            struct replace_at<typelist_t<head, tail>, 0, replaced>
            {
                typedef typelist_t<replaced, tail> type;
            };

            template <typename head, typename tail, unsigned int index_to_replace, typename replaced>
            struct replace_at<typelist_t<head, tail>, index_to_replace, replaced>
            {
                typedef typelist_t<head, typename replace_at<tail, index_to_replace-1, replaced>::type> type;
            };
            
            // replace the first type by another type
            template <typename typelist, typename to_replace, typename replaced> struct replace;
            
            template <typename to_replace, typename replaced>
            struct replace<null_type, to_replace, replaced>
            {
                typedef null_type type;
            };

            template <typename to_replace, typename tail, typename replaced>
            struct replace<typelist_t<to_replace, tail>, to_replace, replaced>
            {
                typedef typelist_t<replaced, tail> type;
            };

            template <typename head, typename tail, typename to_replace, typename replaced>
            struct replace<typelist_t<head, tail>, to_replace, replaced>
            {
                typedef typelist_t<head, typename replace<tail, to_replace, replaced>::type> type;
            };
            
            
            // replace all types by another type
            template <typename typelist, typename to_replace, typename replaced> struct replace_all;
            
            template <typename to_replace, typename replaced>
            struct replace_all<null_type, to_replace, replaced>
            {
                typedef null_type type;
            };
            
            template <typename to_replace, typename tail, typename replaced>
            struct replace_all<typelist_t<to_replace, tail>, to_replace, replaced>
            {
                typedef typelist_t<replaced, typename replace_all<tail, to_replace, replaced>::type> type;
            };
            
            template <typename head, typename tail, typename to_replace, typename replaced>
            struct replace_all<typelist_t<head, tail>, to_replace, replaced>
            {
                typedef typelist_t<head, typename replace_all<tail, to_replace, replaced>::type> type;
            };
        }
    }   
}


#endif // end file guardian

