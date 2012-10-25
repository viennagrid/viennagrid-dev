#ifndef VIENNAGRID_STORAGE_INSERTER_HPP
#define VIENNAGRID_STORAGE_INSERTER_HPP


//#include "viennagrid/storage/reference.hpp"
#include "viennagrid/storage/container_collection.hpp"
#include "viennagrid/storage/container_collection_element.hpp"

namespace viennagrid
{
    namespace storage
    {
        
        template<typename container_collection_type>
        class inserter_base_t
        {
        public:
            inserter_base_t(container_collection_type & _collection) : collection(_collection) {}
            
        protected:
            container_collection_type & collection;
        };

        

        template<typename container_collection_type, typename id_generator_type__>
        class physical_inserter_t
        {
        public:
            typedef container_collection_type physical_container_collection_type;
            typedef id_generator_type__ id_generator_type;
            
            physical_inserter_t(container_collection_type & _collection, id_generator_type id_generator__) : collection(_collection), id_generator(id_generator__) {}
            
            template<typename value_type, typename inserter_type>
            std::pair<
                typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type::hook_type,
                bool
            >
                    physical_insert( value_type element, inserter_type & inserter )
            {
                typedef typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type::iterator iterator;
                typedef typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type container_type;
                typedef typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type::hook_tag hook_tag;
                typedef typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type::hook_type hook_type;
                
                
                container_type & container = viennagrid::storage::container_collection::get< value_type >( collection );
                
                if ( !container.is_present( element ) )
                    viennagrid::storage::id::set_id(element, id_generator( viennameta::tag<value_type>() ) );
                
                std::pair<hook_type, bool> ret = viennagrid::storage::container_collection::get< value_type >( collection ).insert( element );
                
                //container.dereference_hook(ret.first).set_container(collection);

                viennagrid::storage::container_collection_element::insert_callback(
                    container.dereference_hook(ret.first),
                    ret.second,
                    inserter,
                    collection
                                                                                  );
                
                //viennagrid::storage::container_collection_element::insert_callback(*ret.first, ret.second, inserter);
                
                inserter.hook_insert( ret.first, viennameta::tag<value_type>() );
                
                return ret;
            }
            
            template<typename hook_type, typename value_type>
            void hook_insert( hook_type ref, viennameta::tag<value_type> )
            {}
            
            template<typename value_type>
            std::pair<
                typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type::hook_type,
                bool
            >
                operator()( const value_type & element )
            {
                return physical_insert( element, *this );
            }
            
        private:
            container_collection_type & collection;
            id_generator_type id_generator;
        };
        
        
        
        
        
        
        
        
        template<typename view_collection_type, typename dependend_inserter_type>
        class recursive_inserter_t : public dependend_inserter_type
        {
        public:
            typedef dependend_inserter_type base;
            
            recursive_inserter_t(view_collection_type & _collection, const dependend_inserter_type & dependend_inserter) :
                dependend_inserter_type(dependend_inserter), view_collection(_collection) {}
            
            
            template<typename hook_type, typename value_type>
            void hook_insert( hook_type ref, viennameta::tag<value_type> )
            {
                viennagrid::storage::container_collection::hook_or_ignore( view_collection, ref, viennameta::tag<value_type>() );

                base::hook_insert( ref, viennameta::tag<value_type>() );
            }
            
            
            typedef typename base::physical_container_collection_type physical_container_collection_type;
            
            template<typename value_type, typename inserter_type>
            std::pair<
                typename viennagrid::storage::result_of::container_of<physical_container_collection_type, value_type>::type::hook_type,
                bool
            >
                physical_insert( const value_type & element, inserter_type & inserter )
            {
                return base::physical_insert( element, inserter );
            }
            
            template<typename value_type>
            std::pair<
                typename viennagrid::storage::result_of::container_of<physical_container_collection_type, value_type>::type::hook_type,
                bool
            >
                operator()( const value_type & element )
            {
                return physical_insert( element, *this );
            }
            
        private:
            view_collection_type & view_collection;
        };
        
        
        
        namespace inserter
        {
            //typedef VIENNAMETA_MAKE_TYPEMAP_1( viennagrid::storage::default_tag, viennagrid::storage::pointer_reference_tag ) pointer_reference_config;
            //typedef VIENNAMETA_MAKE_TYPEMAP_1( viennagrid::storage::default_tag, viennagrid::storage::iterator_reference_tag ) iterator_reference_config;
            
            template<typename dependend_inserter_type, typename container_collection_type>
            recursive_inserter_t<container_collection_type, dependend_inserter_type> get_recursive( const dependend_inserter_type & inserter, container_collection_type & collection )
            {
                return recursive_inserter_t<container_collection_type, dependend_inserter_type>(inserter, collection);
            }
        }
        
        
        namespace result_of
        {
            template<typename container_collection_type, typename dependend_inserter_type>
            struct recursive_inserter
            {
                typedef recursive_inserter_t<container_collection_type, dependend_inserter_type> type;
            };
            
            template<typename container_collection_type, typename id_generator_type>
            struct physical_inserter
            {
                typedef physical_inserter_t<container_collection_type, id_generator_type> type;
            };
        }
        
    }
}

#endif