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
                
                //
                // generates a typemap with value type and container type from a typelist and a container config
                //
                
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
                    
                    //typedef typename viennagrid::storage::result_of::container_from_tag<value_type, typename container_tag_pair::second>::type type;
                    typedef typename viennagrid::storage::result_of::container<value_type, typename container_tag_pair::second>::type type;
                };
                
                
                template<typename element_list, typename container_config>
                struct container_list_from_value_typelist_using_container_config;
                
                template<typename container_config>
                struct container_list_from_value_typelist_using_container_config<viennameta::null_type, container_config>
                {
                    typedef viennameta::null_type type;
                };
                
                template<typename value_type, typename tail, typename container_config>
                struct container_list_from_value_typelist_using_container_config<viennameta::typelist_t<value_type, tail>, container_config>
                {
                    typedef viennameta::typelist_t<
                        typename viennameta::static_pair<
                                value_type,
                                typename container_from_value_using_container_config<value_type, container_config>::type
                            >,
                        typename container_list_from_value_typelist_using_container_config<tail, container_config>::type
                    > type;
                };
                
                
                
                //
                // Generates a typelist of the container types
                //               
                template<typename container_collection_type>
                struct container_typelist {};
                
                template<typename typemap_>
                struct container_typelist< collection_t<typemap_> >
                {
                    typedef typename viennagrid::storage::result_of::value_typelist<typemap_>::type type;
                };
                
                template<typename typemap_>
                struct container_typelist< const collection_t<typemap_> >
                {
                    typedef typename viennagrid::storage::result_of::value_typelist<typemap_>::type type;
                };
                
                
                //
                // Generates a typelist of the value types
                //               

                template<typename container_collection_type>
                struct value_typelist {};
                
                template<typename typemap_>
                struct value_typelist< collection_t<typemap_> >
                {
                    typedef typename viennagrid::storage::result_of::key_typelist<typemap_>::type type;
                };
                
                template<typename typemap_>
                struct value_typelist< const collection_t<typemap_> >
                {
                    typedef typename viennagrid::storage::result_of::key_typelist<typemap_>::type type;
                };
                                
            } // namespace result_of
            
        } // namespace container_collection
        
        
        namespace result_of
        {

            
            template<typename typemap_, typename element_type>
            struct container_of
            {
                typedef typename viennameta::result_of::second<
                    typename viennameta::typemap::result_of::find< typemap_, element_type >::type
                >::type type;
            };
            
            template<typename typemap_, typename element_type>
            struct container_of< collection_t< typemap_ >, element_type >
            {
                typedef typename container_of<typemap_, element_type>::type type;
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
            typedef viennameta::make_typemap<
                        viennagrid::storage::default_tag,   viennagrid::storage::hooked_container_tag<viennagrid::storage::std_deque_tag, viennagrid::storage::pointer_hook_tag>
                    >::type default_container_config;
            
//             template<typename element_type, typename container_collection>
//             typename viennagrid::storage::result_of::container_of< container_collection, element_type>::type & get( container_collection & collection )
//             {
//                 typedef typename viennagrid::storage::result_of::container_of< container_collection, element_type>::type container_type;
//                 return collection.get( viennameta::tag<container_type>() );                
//             }
//             
//             template<typename element_type, typename container_collection>
//             const typename viennagrid::storage::result_of::container_of< container_collection, element_type>::type & get( const container_collection & collection )
//             {
//                 typedef typename viennagrid::storage::result_of::container_of< container_collection, element_type>::type container_type;
//                 return collection.get( viennameta::tag<container_type>() );                
//             }
            
            
            
            
            
            
            
            
            
            template<typename container_collection_type, typename element_type, typename search_result>
            struct insert_or_ignore_helper
            {
                static void insert_or_ignore( container_collection_type & collection, const element_type & element )
                {
                    collection.get( viennameta::tag<element_type>() ).insert(element);
                }
                
                static void insert_or_ignore( container_collection_type & collection, element_type & element )
                {
                    collection.get( viennameta::tag<element_type>() ).insert(element);
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
            
            
            
            
            
            
            
            

            
            
            
            template<typename container_collection_type, typename hook_type, typename container_type>
            struct hook_or_ignore_helper
            {
                typedef typename viennagrid::storage::hook::value_type<hook_type>::type value_type;
                
                static void hook_or_ignore( container_collection_type & collection, const hook_type & hook )
                {
                    collection.get( viennameta::tag<value_type>() ).insert_hook(hook);
                }
                
                static void hook_or_ignore( container_collection_type & collection, hook_type & hook )
                {
                    collection.get( viennameta::tag<value_type>() ).insert_hook(hook);
                }
            };
            
            template<typename container_collection_type, typename hook_type>
            struct hook_or_ignore_helper<container_collection_type, hook_type, viennameta::not_found>
            {
                static void hook_or_ignore( container_collection_type & collection, const hook_type & hook ) {}
                
                static void hook_or_ignore( container_collection_type & collection, hook_type & hook ) {}
            };

            
            template<typename container_collection_type, typename hook_type, typename element_type>
            void hook_or_ignore( container_collection_type & collection, const hook_type & hook, viennameta::tag<element_type> )
            {
                typedef typename viennagrid::storage::result_of::container_of< container_collection_type, element_type>::type container_type;
                hook_or_ignore_helper<container_collection_type, hook_type, container_type>::hook_or_ignore(collection, hook);
            }
            
            template<typename container_collection_type, typename hook_type, typename element_type>
            void hook_or_ignore( container_collection_type & collection, hook_type & hook, viennameta::tag<element_type> )
            {
                typedef typename viennagrid::storage::result_of::container_of< container_collection_type, element_type>::type container_type;
                hook_or_ignore_helper<container_collection_type, hook_type, container_type>::hook_or_ignore(collection, hook);
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
