#ifndef VIENNAGRID_ACCESSOR_HPP
#define VIENNAGRID_ACCESSOR_HPP

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



// #include "viennagrid/domain/metainfo.hpp"
// #include "../forwards.hpp"

#include "viennagrid/forwards.hpp"
#include "viennagrid/storage/id.hpp"
#include "viennagrid/storage/container_collection.hpp"


#include "viennadata/meta/result_of.hpp"

namespace viennadata
{
    namespace result_of
    {
        template<typename value_type, typename base_id_type>
        struct offset< viennagrid::storage::smart_id_t<value_type, base_id_type> >
        {
            typedef viennagrid::storage::smart_id_t<value_type, base_id_type> id_type;
            typedef base_id_type type;

            static type get(id_type const & id) { return id.get(); }
        };
    }
}

namespace viennagrid
{

    namespace accessor
    {
        
        template<typename value_type_, typename element_type>
        class appendix_accessor_t
        {
        public:
            typedef value_type_ value_type;
            typedef element_type access_type;
            
            value_type       * find( access_type & element )              { return &element.appendix(); }
            value_type const * find( access_type const & element )  const { return &element.appendix(); }

            value_type       & access_unchecked( access_type & element )       { return access(element); }
            value_type const & access_unchecked( access_type const & element ) const { return access(element); }
            
            value_type       & access( access_type & element )             { return element.appendix(); }
            value_type const & access( access_type const & element ) const { return element.appendix(); }

            value_type       & operator()( access_type & element )             { return access(element); }
            value_type const & operator()( access_type const & element ) const { return access(element); }


            void erase( access_type const & element )
            {
#ifdef _DEBUG_
                std::cout << "WARNING! calling erase on apppendix accessor: not supported" << std::endl;
#endif
            }            

            void clear()
            {
#ifdef _DEBUG_
                std::cout << "WARNING! calling clear on apppendix accessor: not supported" << std::endl;
#endif
            }
            
            void resize( std::size_t size )
            {
#ifdef _DEBUG_
                std::cout << "WARNING! calling clear on apppendix accessor: not supported" << std::endl;
#endif
            }
            
        };
        
        
        
        template<typename domain_or_element_type>
        appendix_accessor_t<
          typename viennagrid::result_of::point_type<domain_or_element_type>::type,
          typename viennagrid::result_of::vertex<domain_or_element_type>::type
          > default_point_accessor( domain_or_element_type const & domain_or_element )
        {
          return appendix_accessor_t<
            typename viennagrid::result_of::point_type<domain_or_element_type>::type,
            typename viennagrid::result_of::vertex<domain_or_element_type>::type
          >();
        }
    }
    
    
    namespace result_of
    {
        template<typename value_type, typename WrappedConfigType>
        struct point_type< accessor::appendix_accessor_t<value_type, element_t<vertex_tag, WrappedConfigType> > >
        {
            typedef value_type type;
        };
        
        template<typename value_type, typename WrappedConfigType>
        struct point_type< const accessor::appendix_accessor_t<value_type, element_t<vertex_tag, WrappedConfigType> > >
        {
            typedef value_type type;
        };
    }
    
        
    namespace accessor
    {
        template<typename key_type, typename value_type_, typename element_type_>
        class viennadata_accessor_t
        {
        public:
            
            typedef value_type_ value_type;
            typedef element_type_ access_type;
            
            viennadata_accessor_t( key_type const & key_ ) : key(key_) {}
            
            
            value_type       * find( access_type const & element )        { return viennadata::find<key_type, value_type>(key)(element); }
            value_type const * find( access_type const & element )  const { return viennadata::find<key_type, value_type>(key)(element); }
            
            value_type       & access( access_type const & element )       { return viennadata::access<key_type, value_type>(key)(element); }
            value_type const & access( access_type const & element ) const { return viennadata::access<key_type, value_type>(key)(element); }

            value_type       & access_unchecked( access_type const & element )       { return access(element); }
            value_type const & access_unchecked( access_type const & element ) const { return access(element); }
            
            value_type       & operator()( access_type const & element )       { return access(element); }
            value_type const & operator()( access_type const & element ) const { return access(element); }            
            
            void erase( access_type const & element )
            {
                viennadata::erase<key_type, value_type>(key)(element);
            }
            
            void clear()
            {
#ifdef _DEBUG_
                std::cout << "WARNING! calling clear on viennadata accessor: not supported" << std::endl;
#endif
            }
            
            void resize( std::size_t size ) {}
            
        private:
            key_type key;
        };
        
        
        
        template<typename container_type_, typename element_type_>
        class dense_container_accessor_t
        {
        public:
            
            typedef container_type_ container_type;
            typedef typename container_type::value_type value_type;
            typedef element_type_ access_type;
            
            dense_container_accessor_t( container_type & container_ ) : container(container_) {}
            
            typename container_type::pointer find( access_type const & element )        { return (element.id().get() >= container.size()) ? NULL : &container[ element.id().get() ]; }
            typename container_type::const_pointer find( access_type const & element )  const { return (element.id().get() >= container.size()) ? NULL : &container[ element.id().get() ]; }

            typename container_type::reference access_unchecked( access_type const & element )       { return container[ element.id().get() ]; }
            typename container_type::const_reference access_unchecked( access_type const & element ) const { return container[ element.id().get() ]; }
            
            typename container_type::reference access( access_type const & element )
            {
                if (container.size() <= element.id().get()) container.resize(element.id().get()+1);
//                 container.resize(element.id().get());
                return container[ element.id().get() ];
            }
            typename container_type::const_reference access( access_type const & element ) const
            {
                assert(container.size() > element.id().get());
                return container[ element.id().get() ];
            }

            typename container_type::reference operator()( access_type const & element )       { return access(element); }
            typename container_type::const_reference operator()( access_type const & element ) const { return access(element); }            
            
            void erase( access_type const & element )
            {
                if (element.id().get()-1 == container.size())
                    container.erase( container.size()-1 );
            }
            
            void clear() { container.clear(); }
            void resize( std::size_t size ) { container.resize(size); }
            
        private:
            container_type & container;
        };
        
        
        
        template<typename container_type_, typename element_type_>
        class dense_container_accessor_t<const container_type_, element_type_>
        {
        public:
            
            typedef container_type_ container_type;
            typedef typename container_type::value_type value_type;
            typedef element_type_ access_type;
            
            dense_container_accessor_t( container_type const & container_ ) : container(container_) {}
            
            typename container_type::const_pointer find( access_type const & element )  const { return (element.id().get() >= container.size()) ? NULL : &container[ element.id().get() ]; }

            typename container_type::const_reference access_unchecked( access_type const & element ) const { return container[ element.id().get() ]; }

            typename container_type::const_reference access( access_type const & element ) const
            {
                assert(container.size() > element.id().get());
                return container[ element.id().get() ];
            }

            typename container_type::const_reference operator()( access_type const & element ) const { return access(element); }            
            
        private:
            container_type const & container;
        };
        
        
        
        
        template<typename element_type, typename container_type>
        dense_container_accessor_t< container_type, element_type > dense_container_accessor( container_type & container )
        { return dense_container_accessor_t< container_type, element_type >(container); }
        
        template<typename element_type, typename container_type>
        dense_container_accessor_t< const container_type, element_type > dense_container_accessor( container_type const & container )
        { return dense_container_accessor_t< const container_type, element_type >(container); }
        
        
        
        template<typename element_type, typename container_collection_typemap>
        dense_container_accessor_t<
            typename storage::result_of::container_of<
                container_collection_typemap,
                element_type
            >::type,
            element_type
        > dense_container_accessor( storage::collection_t<container_collection_typemap> & collection )
        {
            return dense_container_accessor<element_type>( storage::collection::get<element_type>(collection) );
        }
        
        template<typename element_type, typename container_collection_typemap>
        dense_container_accessor_t<
            const typename storage::result_of::container_of<
                container_collection_typemap,
                element_type
            >::type,
            element_type
        > dense_container_accessor( storage::collection_t<container_collection_typemap> const & collection )
        {
            return dense_container_accessor<element_type>( storage::collection::get<element_type>(collection) );
        }
        
    }
    
    namespace result_of
    {
        template<typename value_type, typename WrappedConfigType>
        struct point_type< accessor::dense_container_accessor_t<value_type, element_t<vertex_tag, WrappedConfigType> > >
        {
            typedef typename accessor::dense_container_accessor_t<value_type, element_t<vertex_tag, WrappedConfigType> >::value_type type;
        };
        
        template<typename value_type, typename WrappedConfigType>
        struct point_type< const accessor::dense_container_accessor_t<value_type, element_t<vertex_tag, WrappedConfigType> > >
        {
            typedef typename accessor::dense_container_accessor_t<value_type, element_t<vertex_tag, WrappedConfigType> >::value_type type;
        };
    }
        

        
        
        
    
//         template<typename key_type>
//         class viennadata_accessor
//         {
//         public:
//             viennadata_accessor( key_type const & key_ ) : key(key_) {}
//             
//             template<typename value_type, typename element_type>
//             value_type & get(element_type const & element)
//             { return viennadata::access<key_type, value_type>(key)(element); }
// 
//             template<typename value_type, typename element_type>
//             value_type const & get(element_type const & element) const
//             { return viennadata::access<key_type, value_type>(key)(element); }
// 
//             
//         private:
//             key_type key;
//         };
//         
//         
//         template<typename container_type>
//         class container_accessor
//         {
//         public:
//             typedef typename container_type::value_type value_type;
//             
//             container_accessor( container_type & container_ ) : container(container_) {}
//             
//             template<typename value_type, typename element_type>
//             value_type & get(element_type const & element)
//             { return viennagrid::metainfo::look_up(container, element); }
// 
//             template<typename value_type, typename element_type>
//             value_type const & get(element_type const & element) const
//             { return viennagrid::metainfo::look_up(container, element); }
// 
//             
//         private:
//             container_type & container;
//         };
//         
//         
//         template<typename metainfo_collection_type>
//         class metainfo_collection_accessor
//         {
//         public:
//             metainfo_collection_accessor( metainfo_collection_type & metainfo_collection_ ) : metainfo_collection(metainfo_collection_) {}
//             
//             template<typename metainfo_type, typename element_type>
//             metainfo_type & get(element_type const & element)
//             // explicit function template name needed because compiler (clang 3.0 and 4.7.2) cannot resolve correct template function. Might need to take a look at C++ standard function template function overloading resolution
//             { return viennagrid::look_up<metainfo_type, typename metainfo_collection_type::typemap, element_type>(metainfo_collection, element); }
// 
//             template<typename metainfo_type, typename element_type>
//             metainfo_type const & get(element_type const & element) const
//             // explicit function template name needed because compiler (clang 3.0 and 4.7.2) cannot resolve correct template function. Might need to take a look at C++ standard function template function overloading resolution
//             { return viennagrid::look_up<metainfo_type, typename metainfo_collection_type::typemap, element_type>(metainfo_collection, element); }
// 
//             
//         private:
//             metainfo_collection_type & metainfo_collection;
//         };
        
        
        
        
        
        
        
        
        
//         template<typename metainfo_type, typename accessor_type, typename element_type>
//         metainfo_type & look_up( accessor_type & accessor, element_type const & element)
//         {
//             return accessor.template get<metainfo_type>(element);
//         }
//         
//         template<typename metainfo_type, typename accessor_type, typename element_type>
//         metainfo_type const & look_up( accessor_type const & accessor, element_type const & element)
//         {
//             return accessor.template get<metainfo_type>(element);
//         }
        
        
        

        
    
//     }
    
}



#endif
