#ifndef VIENNAMETA_ALGORITHM_HPP
#define VIENNAMETA_ALGORITHM_HPP

#include "collection.hpp"
#include "container_collection.hpp"
#include "view.hpp"

#include "viennagrid/meta/algorithm.hpp"



namespace viennagrid
{
    namespace storage
    {
        namespace collection
        {           
            template<typename collection_type, typename functor>
            struct for_each_functor
            {
                for_each_functor(collection_type & _collection, functor _f) : collection(_collection), f(_f) {}
                
                template<typename key_type, typename value_type>
                void operator()( viennameta::tag< viennameta::static_pair<key_type, value_type> > )
                { f( viennagrid::storage::collection::get<key_type>(collection) ); }
                
                collection_type & collection;
                functor f;
            };
                        
            template<typename collection_type, typename functor>
            void for_each( collection_type & collection, functor f)
            {
                for_each_functor<collection_type, functor> ff(collection, f);
                viennameta::typelist::for_each< typename collection_type::typemap >(ff);
            }
            
            template<typename typelist, typename collection_type, typename functor>
            void for_each_typelist(collection_type & collection, functor & f)
            {
                for_each_functor<collection_type, functor> ff(collection, f);
                viennameta::typelist::for_each<typelist>(ff);
            }
//         }
//         
//         
//         
//         namespace container_collection
//         {
//             template<typename collection_type, typename functor>
//             class for_each_functor
//             {
//             public:
//                 for_each_functor(collection_type & _collection, functor _f) : collection(_collection), f(_f) {}
//                 
//                 template<typename value_type, typename container_type>
//                 void operator()( viennameta::tag< viennameta::static_pair<value_type, container_type> > )
//                 { f( viennagrid::storage::collection::get<value_type>(collection) ); }
//                 
//             private:
//                 collection_type & collection;
//                 functor f;
//             };
            

            
//             template<typename typelist, typename collection_type, typename functor>
//             void for_each_typelist(collection_type & collection, functor f)
//             {
//                 for_each_functor<collection_type, functor> ff(collection, f);
//                 viennameta::typelist::for_each<typelist>(ff);
//             }
            
            
            
            
            
            template<typename collection_type_1, typename collection_type_2, typename functor>
            class dual_for_each_functor
            {
            public:
                
                dual_for_each_functor(
                    collection_type_1 & _container_collection_1,
                    collection_type_2 & _container_collection_2,
                    functor _f) :
                        container_collection_1(_container_collection_1),
                        container_collection_2(_container_collection_2),
                        f(_f) {}
                             
                template<typename type>
                void operator() ( viennameta::tag<type> )
                {
                    f(
                        viennagrid::storage::collection::get<type>(container_collection_1),
                        viennagrid::storage::collection::get<type>(container_collection_2)
                    );                    
                }
                
            private:
                collection_type_1 & container_collection_1;
                collection_type_2 & container_collection_2;
                functor f;
            };
            
            
        }
        
        
        namespace container_collection
        {   
            template<typename predicate>
            class copy_functor
            {
            public:
                copy_functor(predicate _pred) : pred(_pred) {}
                
                template<typename src_container_type, typename dst_container_type>
                void operator() (const src_container_type & src_container, dst_container_type & dst_container)
                {
                    for (typename src_container_type::const_iterator it = src_container.begin(); it != src_container.end(); ++it)
                        if (pred(*it))
                            dst_container.insert( *it );
                            //viennagrid::storage::container::insert(dst_container, *it);
                        //viennagrid::storage::container::insert_helper<dst_container_type>::insert_noreturn(dst_container, *it);
                }
                
            private:
                predicate pred;
            };
            

            
            
            template<typename src_container_typelist, typename dst_container_typelist>
            void copy(const collection_t<src_container_typelist> & src, collection_t<dst_container_typelist> & dst)
            {
                collection::dual_for_each_functor<
                    const collection_t<src_container_typelist>,
                    collection_t<dst_container_typelist>,
                    copy_functor<viennameta::true_predicate>
                    > functor(src, dst, copy_functor<viennameta::true_predicate>(viennameta::true_predicate()));

                typedef typename viennagrid::storage::result_of::common_values<
                    collection_t<src_container_typelist>,
                    collection_t<dst_container_typelist>
                >::type typelist;

                viennameta::typelist::for_each<typelist>(functor);
            }
            
            template<typename src_container_typelist, typename dst_container_typelist, typename predicate>
            void copy_if(const collection_t<src_container_typelist> & src, collection_t<dst_container_typelist> & dst, predicate pred)
            {
                collection::dual_for_each_functor<
                    const collection_t<src_container_typelist>,
                    collection_t<dst_container_typelist>,
                    copy_functor<predicate>
                    > functor(src, dst, copy_functor<predicate>(pred));

                typedef typename viennagrid::storage::result_of::common_values<
                    collection_t<src_container_typelist>,
                    collection_t<dst_container_typelist>
                >::type typelist;

                viennameta::typelist::for_each<typelist>(functor);
            }

            
            
            
            
            template<typename predicate>
            class hook_functor
            {
            public:
                hook_functor(predicate _pred) : pred(_pred) {}
                
                template<typename container_type, typename base_container_type, typename hook_tag, typename hook_container_tag>
                void operator() (container_type & src_container, view_t<base_container_type, hook_tag, hook_container_tag> & dst_view)
                {
                    for (typename container_type::hook_iterator it = src_container.hook_begin(); it != src_container.hook_end(); ++it)
                        if (pred( src_container.dereference_hook(*it) ))
                            dst_view.insert_hook( *it );
                }
                
                
            private:
                predicate pred;
            };
        
        
            template<typename src_container_typelist, typename dst_container_typelist>
            void hook(collection_t<src_container_typelist> & src, collection_t<dst_container_typelist> & dst)
            {
                collection::dual_for_each_functor<
                    collection_t<src_container_typelist>,
                    collection_t<dst_container_typelist>,
                    hook_functor<viennameta::true_predicate>
                    > functor(src, dst, hook_functor<viennameta::true_predicate>(viennameta::true_predicate()));

                typedef typename viennagrid::storage::result_of::common_values<
                    collection_t<src_container_typelist>,
                    collection_t<dst_container_typelist>
                >::type typelist;

                viennameta::typelist::for_each<typelist>(functor);
            }
            
            template<typename src_container_typelist, typename dst_container_typelist, typename predicate>
            void hook_if(collection_t<src_container_typelist> & src, collection_t<dst_container_typelist> & dst, predicate pred)
            {
                collection::dual_for_each_functor<
                    collection_t<src_container_typelist>,
                    collection_t<dst_container_typelist>,
                    hook_functor<predicate>
                    > functor(src, dst, hook_functor<predicate>(pred));

                typedef typename viennagrid::storage::result_of::common_values<
                    collection_t<src_container_typelist>,
                    collection_t<dst_container_typelist>
                >::type typelist;

                viennameta::typelist::for_each<typelist>(functor);
            }
            
        }
    }
}

#endif
