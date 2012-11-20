#ifndef VIENNAGRID_METAINFO_HPP
#define VIENNAGRID_METAINFO_HPP

/* =======================================================================
   Copyright (c) 2011-2012, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   Authors:      Karl Rupp                           rupp@iue.tuwien.ac.at
                 Josef Weinbub                    weinbub@iue.tuwien.ac.at
               
   (A list of additional contributors can be found in the PDF manual)

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */


namespace viennagrid
{

    namespace metainfo
    {
    
        struct random_access_tag {};
        struct associative_access_tag {};
        
        namespace result_of
        {
            
            template<typename container_type>
            struct associative_container_value_type
            {
                typedef typename container_type::value_type type;
            };
            
            template<typename key_type, typename value_type, typename compare, typename allocator>
            struct associative_container_value_type< std::map<key_type, value_type, compare, allocator> >
            {
                typedef value_type type;
            };
            
            
            template<typename container_type>
            struct associative_container_access_tag
            {
                typedef random_access_tag type;
            };
            
            template<typename key_type, typename value_type, typename compare, typename allocator>
            struct associative_container_access_tag< std::map<key_type, value_type, compare, allocator> >
            {
                typedef associative_access_tag type;
            };
            
        }
        
        

        
        
        template<typename geometric_container_type, typename element_type>
        typename result_of::associative_container_value_type<geometric_container_type>::type & look_up( geometric_container_type & container, const element_type & element, random_access_tag )
        {
            return container[ element.id().get() ];
        }
        
        template<typename geometric_container_type, typename element_type>
        typename result_of::associative_container_value_type<geometric_container_type>::type & look_up( geometric_container_type & container, const element_type & element, associative_access_tag )
        {
            typename geometric_container_type::iterator it = container.find( element.id() );
            return it->second;
        }
        
        template<typename geometric_container_type, typename element_type>
        typename result_of::associative_container_value_type<geometric_container_type>::type & look_up( geometric_container_type & container, const element_type & element )
        {
            return look_up(container, element, typename result_of::associative_container_access_tag<geometric_container_type>::type() );
        }
        
        
        
        
        template<typename geometric_container_type, typename element_type>
        void set( geometric_container_type & container, const element_type & element, const typename result_of::associative_container_value_type<geometric_container_type>::type & info, random_access_tag )
        {
            if (container.size() >= element.id().get() )
                container.resize( element.id().get()+1 );
            
            container[ element.id().get() ] = info;
        }
        
        template<typename geometric_container_type, typename element_type>
        void set( geometric_container_type & container, const element_type & element, const typename result_of::associative_container_value_type<geometric_container_type>::type & info, associative_access_tag )
        {
            container.insert(
                std::make_pair( element.id(), info )
            );
        }
        
        template<typename geometric_container_type, typename element_type>
        void set( geometric_container_type & container, const element_type & element, const typename result_of::associative_container_value_type<geometric_container_type>::type & info )
        {
            set(container, element, info, typename result_of::associative_container_access_tag<geometric_container_type>::type() );
        }
    }
}


namespace viennagrid
{
    namespace result_of
    {
        template<typename container_collection_type, typename element_type, typename metainfo_type>
        struct info_container;
        
        template<typename container_typemap, typename element_type, typename metainfo_type>
        struct info_container< viennagrid::storage::collection_t<container_typemap>, element_type, metainfo_type >
        {
            typedef typename viennagrid::storage::result_of::container_of< viennagrid::storage::collection_t<container_typemap>, viennameta::static_pair<element_type, metainfo_type> >::type type;
        };
    }
    
    
    template< typename element_type, typename metainfo_type, typename container_typemap >
    typename result_of::info_container< viennagrid::storage::collection_t<container_typemap>, element_type, metainfo_type>::type & get_info( viennagrid::storage::collection_t<container_typemap> & container_collection )
    {
        return viennagrid::storage::collection::get< viennameta::static_pair<element_type, metainfo_type> >(container_collection);
    }
    
    template< typename element_type, typename metainfo_type, typename container_typemap >
    const typename result_of::info_container<viennagrid::storage::collection_t<container_typemap>, element_type, metainfo_type>::type & get_info( const viennagrid::storage::collection_t<container_typemap> & container_collection )
    {
        return viennagrid::storage::collection::get< viennameta::static_pair<element_type, metainfo_type> >(container_collection);
    }
    
    
    
    
    template<typename metainfo_type, typename metainfo_container_typemap, typename element_type>
    typename metainfo::result_of::associative_container_value_type<
        typename result_of::info_container<
            viennagrid::storage::collection_t<metainfo_container_typemap>,
            element_type,
            metainfo_type
        >::type
    >::type & look_up(
            viennagrid::storage::collection_t<metainfo_container_typemap> & metainfo_collection,
            const element_type & element
        )
    {
        return metainfo::look_up( get_info<element_type, metainfo_type>(metainfo_collection), element );
    }
    
    template<typename metainfo_type, typename metainfo_container_typemap, typename element_type>
    void set(
            viennagrid::storage::collection_t<metainfo_container_typemap> & metainfo_collection,
             const element_type & element,
             const typename metainfo::result_of::associative_container_value_type<
                typename result_of::info_container<
                    viennagrid::storage::collection_t<metainfo_container_typemap>,
                    element_type,
                    metainfo_type
                >::type
            >::type & meta_info )
    {
        metainfo::set( get_info<element_type, metainfo_type>(metainfo_collection), element, meta_info );
    }
    
    
    
    
    namespace metainfo
    {
        
        template<typename element_type, typename cur_typemap>
        struct for_each_element_helper;
        
        template<typename element_type, typename cur_element_type, typename cur_metainfo_type, typename container_type, typename tail>
        struct for_each_element_helper<
            element_type,
            viennameta::typelist_t<
                viennameta::static_pair<
                    viennameta::static_pair<
                        cur_element_type,
                        cur_metainfo_type
                    >,
                    container_type
                >,
            tail>
        >
        {
            template<typename metainfo_container_typemap, typename functor_type>
            static void exec( storage::collection_t<metainfo_container_typemap> & collection, functor_type functor)
            {
                for_each_element_helper<element_type, tail>::exec(collection, functor);
            }
        };
        
        template<typename element_type, typename cur_metainfo_type, typename container_type, typename tail>
        struct for_each_element_helper<
            element_type,
            viennameta::typelist_t<
                viennameta::static_pair<
                    viennameta::static_pair<
                        element_type,
                        cur_metainfo_type
                    >,
                    container_type
                >,
            tail>
        >
        {
            template<typename metainfo_container_typemap, typename functor_type>
            static void exec( storage::collection_t<metainfo_container_typemap> & collection, functor_type functor)
            {
                functor( get_info<element_type, cur_metainfo_type>(collection) );
                //get_info<element_type, cur_metainfo_type>(collection).resize( size );
                for_each_element_helper<element_type, tail>::exec(collection, functor);
            }
        };
        
        template<typename element_type>
        struct for_each_element_helper<
            element_type,
            viennameta::null_type
        >
        {
            template<typename metainfo_container_typemap, typename functor_type>
            static void exec( storage::collection_t<metainfo_container_typemap> & collection, functor_type functor)
            {}
        };
        
        
        
        class resize_functor
        {
        public:
            resize_functor(std::size_t size_) : size(size_) {}
            
            template<typename container_type>
            void operator() ( container_type & container )
            {
                container.resize(size);
            }
            
        private:
            std::size_t size;
        };
        
        
        
        template<typename element_type, typename metainfo_container_typemap>
        void resize_container( storage::collection_t<metainfo_container_typemap> & collection, std::size_t size )
        {
            for_each_element_helper<element_type, metainfo_container_typemap>::exec(collection, resize_functor(size) );
        }
        
        
        
        class increment_size_functor
        {
        public:
            increment_size_functor() {}
            
            template<typename container_type>
            void operator() ( container_type & container )
            {
                container.resize( container.size()+1 );
            }
        };
        
        template<typename element_type, typename metainfo_container_typemap>
        void increment_size( storage::collection_t<metainfo_container_typemap> & collection )
        {
            for_each_element_helper<element_type, metainfo_container_typemap>::exec(collection, increment_size_functor() );
        }
        
    }
    
    
    

    
    
    
    namespace result_of
    {
        template<typename pair_type, typename container_config, typename topologic_domain_config>
        struct metainfo_container;

        template<typename element_type, typename metainfo_type, typename container_config, typename topologic_domain_config>
        struct metainfo_container< viennameta::static_pair<element_type, metainfo_type>, container_config, topologic_domain_config >
        {
            typedef typename viennameta::typemap::result_of::find<container_config, viennameta::static_pair<element_type, metainfo_type> >::type search_result;
            typedef typename viennameta::typemap::result_of::find<container_config, viennagrid::storage::default_tag>::type default_container;
            
            typedef typename viennameta::_if<
                !viennameta::_equal<search_result, viennameta::not_found>::value,
                search_result,
                default_container
            >::type container_tag_pair;
            
            //typedef typename viennagrid::storage::result_of::container_from_tag<value_type, typename container_tag_pair::second>::type type;
            typedef typename viennagrid::storage::result_of::container<metainfo_type, typename container_tag_pair::second>::type type;
        };
        
        
        template<typename element_list, typename container_config, typename topologic_domain_config>
        struct metainfo_container_typemap;
        
        template<typename container_config, typename topologic_domain_config>
        struct metainfo_container_typemap<viennameta::null_type, container_config, topologic_domain_config>
        {
            typedef viennameta::null_type type;
        };
        
        template<typename element_tag, typename metainfo_type, typename tail, typename container_config, typename topologic_domain_config>
        struct metainfo_container_typemap<viennameta::typelist_t< viennameta::static_pair<element_tag, metainfo_type>, tail>, container_config, topologic_domain_config>
        {
            typedef typename viennagrid::result_of::element<topologic_domain_config, element_tag>::type element_type;
            
            typedef viennameta::typelist_t<
                typename viennameta::static_pair<
                        viennameta::static_pair<element_type, metainfo_type>,
                        typename metainfo_container< viennameta::static_pair<element_type, metainfo_type>, container_config, topologic_domain_config>::type
                    >,
                typename metainfo_container_typemap<tail, container_config, topologic_domain_config>::type
            > type;
        };
    }
    
}



#endif
