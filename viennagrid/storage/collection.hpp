#ifndef VIENNAGRID_COLLECTION_HPP
#define VIENNAGRID_COLLECTION_HPP

#include "viennagrid/meta/utils.hpp"
#include "viennagrid/meta/typelist.hpp"

namespace viennagrid
{
    namespace storage
    {
        
        template<typename typelist>
        class collection_layer;

        template<typename _typelist>
        class collection_t : public collection_layer< typename viennameta::typelist::result_of::no_duplicates<_typelist>::type >
        {
        public:
            typedef typename viennameta::typelist::result_of::no_duplicates<_typelist>::type typelist;
            typedef collection_layer< typelist > base;
            
            using base::get;
        private:
        };
        

        template<typename head, typename tail>
        class collection_layer< viennameta::typelist_t<head, tail> > : public collection_layer<tail>
        {
        public:
            typedef collection_layer< tail > base;
            typedef head type;
            
            using base::get;
            head & get( viennameta::tag<type> ) { return element; }
            const head & get( viennameta::tag<type> ) const { return element; }
            
        private:
            type element;
        };

        template<>
        class collection_layer< viennameta::null_type >
        {
        public:            
            void get();
            void push_back();
        };
        
        
        namespace collection
        {
            template<typename type, typename collection>
            type & get( collection & c )
            {
                return c.get( viennameta::tag<type>() );
            }
            
            template<typename type, typename collection>
            const type & get( const collection & c )
            {
                return c.get( viennameta::tag<type>() );
            }
        } // namespace collection
        
        
        namespace result_of
        {
            template<typename typelist>
            struct collection
            {
                typedef collection_t<typelist> type;
            };
        } // namespace result_of
        
    } // namespace storge

} // namespace viennagrid

#endif