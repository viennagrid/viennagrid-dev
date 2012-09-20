#ifndef VIENNAGRID_CONTAINER_COLLECTION_HPP
#define VIENNAGRID_CONTAINER_COLLECTION_HPP

#include "viennagrid/meta/typelist.hpp"
#include "viennagrid/meta/typemap.hpp"

#include "viennagrid/storage/container.hpp"
#include "viennagrid/storage/collection.hpp"


namespace viennagrid
{
    namespace storage
    {
        namespace result_of
        {
            template<typename type>
            struct container_collection_element_required_typelist
            {
                typedef typename type::required_typelist typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<bool>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<char>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<unsigned char>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<short>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<unsigned short>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<int>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<unsigned int>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<float>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<double>
            {
                typedef viennameta::null_type typelist;                
            };
        }
        
        namespace container_collection_element
        {
            template<typename container_collection_element, typename container_collection>
            void insert_callback( container_collection_element & element, container_collection & collection)
            {
                element.insert_callback( collection );
            }
        }
        
        
        namespace container_collection
        {
            
            namespace result_of
            {
                template<typename element_type, typename container_config>
                struct container
                {
                    typedef typename viennameta::typemap::result_of::find<container_config, element_type>::type search_result;
                    typedef typename viennameta::typemap::result_of::find<container_config, viennagrid::storage::default_tag>::type default_container;
                    
                    typedef typename viennameta::_if<
                        !viennameta::_equal<search_result, viennameta::not_found>::value,
                        search_result,
                        default_container
                    >::type container_tag_pair;
                    
                    typedef typename viennagrid::storage::result_of::container_from_tag<element_type, typename container_tag_pair::second>::type type;
                };
                
                
                template<typename element_list, typename container_config>
                struct container_list {};
                
                template<typename container_config>
                struct container_list<viennameta::null_type, container_config>
                {
                    typedef viennameta::null_type type;
                };
                
                template<typename head, typename tail, typename container_config>
                struct container_list<viennameta::typelist_t<head, tail>, container_config>
                {
                    typedef viennameta::typelist_t<
                        typename container<head, container_config>::type,
                        typename container_list<tail, container_config>::type
                    > type;
                };
                
            } // namespace result_of
            
        } // namespace container_collection
        
        
        namespace result_of
        {
            
            template<typename element_list, typename container_config>
            struct container_collection
            {
                typedef typename viennagrid::storage::result_of::collection<
                    typename viennagrid::storage::container_collection::result_of::container_list<element_list, container_config>::type
                >::type type;
            };
            

            
            template<typename type, typename element_type>
            struct container_of
            {};
            
            template<typename element_type>
            struct container_of<viennameta::null_type, element_type>
            {
                typedef viennameta::null_type type;
            };
            
            template<typename head, typename tail, typename element_type>
            struct container_of< viennameta::typelist_t<head, tail>, element_type >
            {
                typedef typename viennameta::_if<
                    viennameta::_equal< typename head::value_type, element_type>::value,
                    head,
                    typename container_of<tail, element_type>::type
                >::type type;
            };
            
            template<typename typelist, typename element_type>
            struct container_of< viennagrid::storage::collection_t< typelist >, element_type >
            {
                typedef typename container_of<typelist, element_type>::type type;
            };
            
        } // namespace result_of
        
        
        
        namespace container_collection
        {
            
            template<typename element_type, typename container_collection>
            typename viennagrid::storage::result_of::container_of< container_collection, element_type>::type & get( container_collection & collection )
            {
                typedef typename viennagrid::storage::result_of::container_of< container_collection, element_type>::type container_type;
                return collection.get( viennameta::tag<container_type>() );                
            }
            
            template<typename element_type, typename container_collection>
            const typename viennagrid::storage::result_of::container_of< container_collection, element_type>::type & get( const container_collection & collection )
            {
                typedef typename viennagrid::storage::result_of::container_of< container_collection, element_type>::type container_type;
                return collection.get( viennameta::tag<container_type>() );                
            }
            
            template<typename element_type, typename container_collection>
            element_type & insert( container_collection & collection, const element_type & element)
            {
                typedef typename viennagrid::storage::result_of::container_of< container_collection, element_type>::type container_type;
                typename container_type::iterator inserted_element = viennagrid::storage::container::insert(
                    collection.get( viennameta::tag<container_type>() ),
                    element);
                
                viennagrid::storage::container_collection_element::insert_callback( *inserted_element, collection );
                return inserted_element;
            }
            
            template<typename container_collection, typename iterator_type>
            iterator_type erase( container_collection & collection, iterator_type it)
            {
                return get<typename iterator_type::value_type>(collection).erase(it);
            }
            
        } // namespace container_collection
        
    } // namespace storage

} // namespace viennagrid

#endif