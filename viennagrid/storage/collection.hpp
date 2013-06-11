#ifndef VIENNAGRID_STORAGE_COLLECTION_HPP
#define VIENNAGRID_STORAGE_COLLECTION_HPP

#include "viennagrid/meta/utils.hpp"
#include "viennagrid/meta/typemap.hpp"

namespace viennagrid
{
    namespace storage
    {
        
        template<typename typemap>
        class collection_layer;
        
        template<typename typemap_>
        class collection_t : public collection_layer< typemap_ >
        {
        public:
            typedef typemap_ typemap;
            typedef collection_layer< typemap > base;
            
            using base::get;
        private:
        };
        
        template<typename key_, typename value_, typename tail>
        class collection_layer< viennameta::typelist_t< viennameta::static_pair<key_, value_> , tail> > : public collection_layer<tail>
        {
        public:
            typedef collection_layer< tail > base;
            typedef key_ key_type;
            typedef value_ value_type;
            
            using base::get;
            value_type & get( viennameta::tag<key_type> ) { return element; }
            const value_type & get( viennameta::tag<key_type> ) const { return element; }
            
        private:
            value_type element;
        };
        
        template<>
        class collection_layer< viennameta::null_type >
        {
        public:            
            void get();
        };
        
        
        namespace result_of
        {
            template<typename typemap, typename key_type>
            struct value_type
            {
                typedef typename viennameta::typemap::result_of::find<typemap, key_type>::type::second type;
            };
            
            template<typename typemap, typename key_type>
            struct value_type< collection_t<typemap>, key_type >
            {
                typedef typename viennameta::typemap::result_of::find<typemap, key_type>::type::second type;
            };
        }
        
        
        
//         template<typename typelist>
//         class collection_layer;
// 
//         template<typename _typelist>
//         class collection_t : public collection_layer< typename viennameta::typelist::result_of::no_duplicates<_typelist>::type >
//         {
//         public:
//             typedef typename viennameta::typelist::result_of::no_duplicates<_typelist>::type typelist;
//             typedef collection_layer< typelist > base;
//             
//             using base::get;
//         private:
//         };
//         
// 
//         template<typename head, typename tail>
//         class collection_layer< viennameta::typelist_t<head, tail> > : public collection_layer<tail>
//         {
//         public:
//             typedef collection_layer< tail > base;
//             typedef head type;
//             
//             using base::get;
//             head & get( viennameta::tag<type> ) { return element; }
//             const head & get( viennameta::tag<type> ) const { return element; }
//             
//         private:
//             type element;
//         };
// 
//         template<>
//         class collection_layer< viennameta::null_type >
//         {
//         public:            
//             void get();
//         };
        
        
        namespace collection
        {
            template<typename type, typename typemap>
            typename result_of::value_type<typemap, typename viennameta::remove_const<type>::type >::type & get( collection_t<typemap> & c )
            {
                return c.get( viennameta::tag< typename viennameta::remove_const<type>::type >() );
            }
            
            template<typename type, typename typemap>
            const typename result_of::value_type<typemap, typename viennameta::remove_const<type>::type >::type & get( const collection_t<typemap> & c )
            {
                return c.get( viennameta::tag< typename viennameta::remove_const<type>::type >() );
            }
        } // namespace collection
        
        
        namespace result_of
        {
            template<typename typemap>
            struct collection
            {
                typedef collection_t<typemap> type;
            };
        } // namespace result_of
        
    } // namespace storge

} // namespace viennagrid

#endif
