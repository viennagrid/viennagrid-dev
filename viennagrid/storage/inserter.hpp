#ifndef VIENNAGRID_STORAGE_INSERTER_HPP
#define VIENNAGRID_STORAGE_INSERTER_HPP


#include "viennagrid/storage/reference.hpp"
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

        

        template<typename container_collection_type, typename _container_collection_reference_config, typename _id_generator_type>
        class physical_inserter_t : public inserter_base_t<container_collection_type>
        {
        public:
            typedef inserter_base_t<container_collection_type> base_inserter;
            typedef container_collection_type physical_container_collection_type;
            typedef _id_generator_type id_generator_type;
            typedef _container_collection_reference_config container_collection_reference_config;
            
            physical_inserter_t(container_collection_type & _collection, id_generator_type & _id_generator) : base_inserter(_collection), id_generator(_id_generator) {}
            
            template<typename value_type, typename inserter_type>
            std::pair<
                typename viennagrid::storage::reference::reference_type_from_config<
                    typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type,
                    container_collection_reference_config
                >::type,
                bool
            >
                    physical_insert( const value_type & element, inserter_type & inserter )
            {
                typedef typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type::iterator iterator;
                typedef typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type container_type;
                typedef typename viennagrid::storage::reference::reference_tag_from_config<container_type, container_collection_reference_config>::type reference_tag;
                typedef typename viennagrid::storage::reference::reference_type_from_config<container_type, container_collection_reference_config>::type reference_type;
                
                std::pair<iterator, bool> ret = viennagrid::storage::container::insert(viennagrid::storage::container_collection::get< value_type >( base_inserter::collection ), element);
                
                if (ret.second) viennagrid::storage::container_collection_element::set_id(*ret.first, id_generator( viennameta::tag<value_type>() ) );
                
                viennagrid::storage::container_collection_element::insert_callback(*ret.first, ret.second, inserter);
                
                
                reference_type ref = viennagrid::storage::reference::iterator_to_reference(ret.first, reference_tag());
                inserter.reference_insert( ref );
                
                return std::make_pair(ref, ret.second);
            }
            
            template<typename reference_type>
            void reference_insert( reference_type ref )
            {}
            
            template<typename value_type>
            std::pair<
                typename viennagrid::storage::reference::reference_type_from_config<
                    //value_type,
                    typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type,
                    container_collection_reference_config
                >::type,
                bool
            >
                operator()( const value_type & element )
            {
                return physical_insert( element, *this );
            }
            
        private:
            id_generator_type & id_generator;
        };       
        
        template<typename container_collection_type, typename dependend_inserter_type>
        class recursive_inserter_t : public inserter_base_t<container_collection_type>, dependend_inserter_type
        {
        public:
            typedef inserter_base_t<container_collection_type> base_inserter;
            typedef dependend_inserter_type base;
            
            recursive_inserter_t(container_collection_type & _collection, const dependend_inserter_type & dependend_inserter) :
                base_inserter(_collection), dependend_inserter_type(dependend_inserter) {}
            
            
            template<typename reference_type>
            void reference_insert( reference_type ref )
            {
                typedef typename viennagrid::storage::reference::value_type_from_reference_type<reference_type>::type value_type;
                
                if (!viennameta::_equal<
                        typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type,
                        viennameta::null_type
                    >::value)
                {
                    viennagrid::storage::container_collection::insert_or_ignore( base_inserter::collection, *ref );
                }
                
                base::reference_insert( ref );
            }
            
            
            typedef typename base::physical_container_collection_type physical_container_collection_type;
            typedef typename base::container_collection_reference_config container_collection_reference_config;
            
            template<typename value_type, typename inserter_type>
            std::pair<
                typename viennagrid::storage::reference::reference_type_from_config<
                    //value_type,
                    typename viennagrid::storage::result_of::container_of<physical_container_collection_type, value_type>::type,
                    container_collection_reference_config
                >::type,
                bool
            >
                physical_insert( const value_type & element, inserter_type & inserter )
            {
                return base::physical_insert( element, inserter );
            }
            
            template<typename value_type>
            std::pair<
                typename viennagrid::storage::reference::reference_type_from_config<
                    //value_type,
                    typename viennagrid::storage::result_of::container_of<physical_container_collection_type, value_type>::type,
                    container_collection_reference_config
                >::type,
                bool
            >
                operator()( const value_type & element )
            {
                return physical_insert( element, *this );
            }
        };
        
        
        
        namespace inserter
        {
            typedef VIENNAMETA_MAKE_TYPEMAP_1( viennagrid::storage::default_tag, viennagrid::storage::pointer_reference_tag ) pointer_reference_config;
            typedef VIENNAMETA_MAKE_TYPEMAP_1( viennagrid::storage::default_tag, viennagrid::storage::iterator_reference_tag ) iterator_reference_config;
            
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
            
            template<typename container_collection_type, typename container_collection_reference_config, typename id_generator_type>
            struct physical_inserter
            {
                typedef physical_inserter_t<container_collection_type, container_collection_reference_config, id_generator_type> type;
            };
        }
        
    }
}

#endif