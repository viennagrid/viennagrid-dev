#ifndef VIENNAGRID_STORAGE_INSERTER_HPP
#define VIENNAGRID_STORAGE_INSERTER_HPP


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
            
//             template<typename value_type, typename inserter_type>
//             typename viennagrid::storage::result_of::container_of<physical_container_collection_type, value_type>::type::iterator physical_insert( const value_type & element,
//                                                                                                                                      inserter_type & inserter );
//             
//             template<typename value_type>
//             typename viennagrid::storage::result_of::container_of<physical_container_collection_type, value_type>::type::iterator insert( const value_type & element )
//             {
//                 return physical_insert( element, *this );
//             }
            
        protected:
            container_collection_type & collection;
        };
        


        template<typename container_collection_type>
        class physical_inserter_t : public inserter_base_t<container_collection_type>
        {
        public:
            typedef inserter_base_t<container_collection_type> base_inserter;
            typedef container_collection_type physical_container_collection_type;
            
            physical_inserter_t(container_collection_type & _collection) : base_inserter(_collection) {}
            
            template<typename value_type, typename inserter_type>
            typename viennagrid::storage::result_of::container_of<physical_container_collection_type, value_type>::type::iterator physical_insert( const value_type & element,
                                                                                                                                     inserter_type & inserter )
            {
                typedef typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type::iterator iterator;
                
                iterator it = viennagrid::storage::container::insert(viennagrid::storage::container_collection::get< value_type >( base_inserter::collection ), element);
                
                viennagrid::storage::container_collection_element::insert_callback(*it, inserter);
                
                inserter.reference_insert( it );
                
                return it;
            }
            
            template<typename iterator_type>
            void reference_insert( iterator_type it )
            {}
            
            template<typename value_type>
            typename viennagrid::storage::result_of::container_of<physical_container_collection_type, value_type>::type::iterator insert( const value_type & element )
            {
                return physical_insert( element, *this );
            }
        };       
        
        template<typename container_collection_type, typename dependend_inserter_type>
        class recursive_inserter_t : public inserter_base_t<container_collection_type>, dependend_inserter_type
        {
        public:
            typedef inserter_base_t<container_collection_type> base_inserter;
            typedef dependend_inserter_type base;
            
            recursive_inserter_t(container_collection_type & _collection, const dependend_inserter_type & dependend_inserter) :
                base_inserter(_collection), dependend_inserter_type(dependend_inserter) {}
            
            
            template<typename iterator_type>
            void reference_insert( iterator_type it )
            {
                typedef typename iterator_type::value_type value_type;
                
                if (!viennameta::_equal<
                        typename viennagrid::storage::result_of::container_of<container_collection_type, value_type>::type,
                        viennameta::null_type
                    >::value)
                {
                    viennagrid::storage::container_collection::get< value_type >( base_inserter::collection ).insert( *it );
                }
                
                base::reference_insert( it );
            }
            
            
            typedef typename base::physical_container_collection_type physical_container_collection_type;
            
            template<typename value_type, typename inserter_type>
            typename viennagrid::storage::result_of::container_of<physical_container_collection_type, value_type>::type::iterator physical_insert( const value_type & element, inserter_type & inserter )
            {
                return base::physical_insert( element, inserter );
            }
            
            template<typename value_type>
            typename viennagrid::storage::result_of::container_of<physical_container_collection_type, value_type>::type::iterator insert( const value_type & element )
            {
                return physical_insert( element, *this );
            }
        };
        
        
        
        namespace inserter
        {
            template<typename parent_inserter, typename container_collection_type>
            recursive_inserter_t<container_collection_type, parent_inserter> get_recursive( const parent_inserter & inserter, container_collection_type & collection )
            {
                return recursive_inserter_t<container_collection_type, parent_inserter>(inserter, collection);
            }
        }
        
        
        namespace result_of
        {
            template<typename container_collection_type, typename parent_inserter = viennameta::null_type>
            struct recursive_inserter
            {
                typedef recursive_inserter_t<container_collection_type, parent_inserter> type;
            };
            
            template<typename container_collection_type>
            struct recursive_inserter<container_collection_type, viennameta::null_type>
            {
                typedef physical_inserter_t<container_collection_type> type;
            };
        }
        
    }
}

#endif