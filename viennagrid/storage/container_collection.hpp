#ifndef VIENNAGRID_STORAGE_CONTAINER_COLLECTION_HPP
#define VIENNAGRID_STORAGE_CONTAINER_COLLECTION_HPP

#include "viennagrid/meta/typelist.hpp"
#include "viennagrid/meta/typemap.hpp"

#include "viennagrid/storage/container.hpp"
#include "viennagrid/storage/collection.hpp"
#include "viennagrid/storage/view.hpp"
#include "viennagrid/storage/range.hpp"

namespace viennagrid
{
    namespace storage
    {
        
        namespace container_collection
        {
            
            namespace result_of
            {
                template<typename value_type, typename container_config>
                struct container_from_value_using_container_config
                {
                    typedef typename viennameta::typemap::result_of::find<container_config, value_type>::type search_result;
                    typedef typename viennameta::typemap::result_of::find<container_config, viennagrid::storage::default_tag>::type default_container;
                    
                    typedef typename viennameta::_if<
                        !viennameta::_equal<search_result, viennameta::not_found>::value,
                        search_result,
                        default_container
                    >::type container_tag_pair;
                    
                    typedef typename viennagrid::storage::result_of::container_from_tag<value_type, typename container_tag_pair::second>::type type;
                };
                
                
                template<typename element_list, typename container_config>
                struct container_list_from_value_typelist_using_container_config {};
                
                template<typename container_config>
                struct container_list_from_value_typelist_using_container_config<viennameta::null_type, container_config>
                {
                    typedef viennameta::null_type type;
                };
                
                template<typename head, typename tail, typename container_config>
                struct container_list_from_value_typelist_using_container_config<viennameta::typelist_t<head, tail>, container_config>
                {
                    typedef viennameta::typelist_t<
                        typename container_from_value_using_container_config<head, container_config>::type,
                        typename container_list_from_value_typelist_using_container_config<tail, container_config>::type
                    > type;
                };
                

                template<typename container_typelist>
                struct value_typelist_from_container_typelist {};
                
                template<>
                struct value_typelist_from_container_typelist<viennameta::null_type>
                {
                    typedef viennameta::null_type type;
                };
                
                template<typename head, typename tail>
                struct value_typelist_from_container_typelist< viennameta::typelist_t<head, tail> >
                {
                    typedef viennameta::typelist_t< typename head::value_type, typename value_typelist_from_container_typelist<tail>::type > type;
                };
                
                
                
                template<typename container_collection_type>
                struct container_typelist {};
                
                template<typename _container_typelist>
                struct container_typelist< collection_t<_container_typelist> >
                { typedef _container_typelist type; };
                
                template<typename _container_typelist>
                struct container_typelist< const collection_t<_container_typelist> >
                { typedef _container_typelist type; };
                
                
                template<typename container_collection_type>
                struct value_typelist {};
                
                template<typename _container_typelist>
                struct value_typelist< collection_t<_container_typelist> >
                { typedef typename value_typelist_from_container_typelist<_container_typelist>::type type; };
                
                template<typename _container_typelist>
                struct value_typelist< const collection_t<_container_typelist> >
                { typedef typename value_typelist_from_container_typelist<_container_typelist>::type type; };
                                
            } // namespace result_of
            
        } // namespace container_collection
        
        
        namespace result_of
        {            
            template<typename typelist, typename element_type>
            struct container_of
            {};
            
            template<typename element_type>
            struct container_of<viennameta::null_type, element_type>
            {
                typedef viennameta::not_found type;
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
            
            template<typename container_typelist, typename element_type>
            struct container_of< collection_t< container_typelist >, element_type >
            {
                typedef typename container_of<container_typelist, element_type>::type type;
            };
            
            
            template<typename container_collection_1, typename container_collection_2>
            struct common_values;
            
            template<typename container_typelist_1, typename container_typelist_2>
            struct common_values< collection_t<container_typelist_1>, collection_t<container_typelist_2> >
            {
                typedef collection_t<container_typelist_1> from_container_collection_type; 
                typedef collection_t<container_typelist_2> to_container_collection_type;
                                
                typedef typename viennagrid::storage::container_collection::result_of::value_typelist<from_container_collection_type>::type from_container_collection_value_typelist;
                typedef typename viennagrid::storage::container_collection::result_of::value_typelist<to_container_collection_type>::type to_container_collection_value_typelist;
                
                typedef typename viennameta::typelist::result_of::intersection<
                    from_container_collection_value_typelist,
                    to_container_collection_value_typelist
                >::type type;

            };
            
        } // namespace result_of
        
        
        
        namespace container_collection
        {
            typedef VIENNAMETA_MAKE_TYPEMAP_1( viennagrid::storage::default_tag, viennagrid::storage::std_deque_tag) default_container_config;
            
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
            
            
            
            template<typename container_collection_type, typename element_type, typename search_result>
            struct insert_or_ignore_helper
            {
                typedef typename viennagrid::storage::result_of::container_of< container_collection_type, element_type>::type container_type;
                
                static void insert_or_ignore( container_collection_type & collection, const element_type & element )
                {
                    viennagrid::storage::container::insert(collection.get( viennameta::tag<container_type>() ), element);
                }
                
                static void insert_or_ignore( container_collection_type & collection, element_type & element )
                {
                    viennagrid::storage::container::insert(collection.get( viennameta::tag<container_type>() ), element);
                }
            };
            
            template<typename container_collection_type, typename element_type>
            struct insert_or_ignore_helper<container_collection_type, element_type, viennameta::not_found>
            {
                static void insert_or_ignore( container_collection_type & collection, const element_type & element ) {}
                
                static void insert_or_ignore( container_collection_type & collection, element_type & element ) {}
            };

            
            template<typename container_collection_type, typename element_type>
            void insert_or_ignore( container_collection_type & collection, const element_type & element)
            {
                typedef typename viennagrid::storage::result_of::container_of< container_collection_type, element_type>::type container_type;
                insert_or_ignore_helper<container_collection_type, element_type, container_type>::insert_or_ignore(collection, element);
            }
            
            template<typename container_collection_type, typename element_type>
            void insert_or_ignore( container_collection_type & collection, element_type & element)
            {
                typedef typename viennagrid::storage::result_of::container_of< container_collection_type, element_type>::type container_type;
                insert_or_ignore_helper<container_collection_type, element_type, container_type>::insert_or_ignore(collection, element);
            }
            

        } // namespace container_collection
        

        
        
        namespace result_of
        {
            template<typename value_typelist, typename container_config>
            struct container_collection
            {
                typedef collection_t<
                    typename viennagrid::storage::container_collection::result_of::container_list_from_value_typelist_using_container_config<
                        value_typelist,
                        container_config
                    >::type
                > type;
            };
        }
        
        
        
    } // namespace storage

} // namespace viennagrid

#endif