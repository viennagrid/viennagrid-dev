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
                
                template<typename type>
                void operator()( viennameta::tag<type> )
                { f( viennagrid::storage::collection::get<type>(collection) ); }
                
                collection_type & collection;
                functor f;
            };
                        
            template<typename collection_type, typename functor>
            void for_each(collection_type & collection, functor f)
            {
                for_each_functor<collection_type, functor> ff(collection, f);
                viennameta::typelist::for_each< typename viennagrid::storage::container_collection::result_of::container_typelist<collection_type>::type >(ff);
            }
            
            template<typename typelist, typename collection_type, typename functor>
            void for_each_typelist(collection_type & collection, functor & f)
            {
                for_each_functor<collection_type, functor> ff(collection, f);
                viennameta::typelist::for_each<typelist>(ff);
            }
        }
        
        
        
        namespace container_collection
        {
            template<typename collection_type, typename functor>
            class for_each_functor
            {
            public:
                for_each_functor(collection_type & _collection, functor _f) : collection(_collection), f(_f) {}
                
                template<typename type>
                void operator()( viennameta::tag<type> )
                { f( viennagrid::storage::container_collection::get<type>(collection) ); }
                
            private:
                collection_type & collection;
                functor f;
            };
            

            
            template<typename typelist, typename collection_type, typename functor>
            void for_each_typelist(collection_type & collection, functor f)
            {
                for_each_functor<collection_type, functor> ff(collection, f);
                viennameta::typelist::for_each<typelist>(ff);
            }
            
            
            
            
            
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
                        viennagrid::storage::container_collection::get<type>(container_collection_1),
                        viennagrid::storage::container_collection::get<type>(container_collection_2)
                    );                    
                }
                
            private:
                collection_type_1 & container_collection_1;
                collection_type_2 & container_collection_2;
                functor f;
            };
            
            
            
            template<typename predicate>
            class copy_functor
            {
            public:
                copy_functor(predicate _pred) : pred(_pred) {}
                
                template<typename src_container_type, typename dst_container_type>
                void operator() (src_container_type & src_container, dst_container_type & dst_container)
                {
                    for (typename src_container_type::iterator it = src_container.begin(); it != src_container.end(); ++it)
                        if (pred(*it))
                            viennagrid::storage::container::insert(dst_container, *it);
                }
                
                template<typename src_container_type, typename dst_container_type>
                void operator() (const src_container_type & src_container, dst_container_type & dst_container)
                {
                    for (typename src_container_type::const_iterator it = src_container.begin(); it != src_container.end(); ++it)
                        if (pred(*it))
                            viennagrid::storage::container::insert(dst_container, *it);
                }
                
            private:
                predicate pred;
            };
            
            
            template<typename src_container_typelist, typename dst_container_typelist>
            void copy(const collection_t<src_container_typelist> & src, collection_t<dst_container_typelist> & dst)
            {
                dual_for_each_functor<
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
                dual_for_each_functor<
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

            
            
            template<typename src_container_typelist, typename dst_container_typelist>
            void reference(collection_t<src_container_typelist> & src, collection_t<dst_container_typelist> & dst)
            {
                dual_for_each_functor<
                    collection_t<src_container_typelist>,
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
            void reference_if(collection_t<src_container_typelist> & src, collection_t<dst_container_typelist> & dst, predicate pred)
            {
                dual_for_each_functor<
                    collection_t<src_container_typelist>,
                    collection_t<dst_container_typelist>,
                    copy_functor<predicate>
                    > functor(src, dst, copy_functor<predicate>(pred));

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
