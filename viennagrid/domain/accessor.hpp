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

#include <assert.h>

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
              assert(false && bool("WARNING! calling erase on apppendix accessor: not supported"));
            }            

            void clear()
            {
              assert(false && bool("WARNING! calling clear on apppendix accessor: not supported"));
            }
            
            void resize( std::size_t size )
            {
              assert(false && bool("WARNING! calling clear on apppendix accessor: not supported"));
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
//         template<typename key_type, typename value_type_, typename element_type_>
//         class viennadata_accessor_t
//         {
//         public:
//             
//             typedef value_type_ value_type;
//             typedef element_type_ access_type;
//             
//             viennadata_accessor_t( key_type const & key_ ) : key(key_) {}
//             
//             
//             value_type       * find( access_type const & element )        { return viennadata::find<key_type, value_type>(key)(element); }
//             value_type const * find( access_type const & element )  const { return viennadata::find<key_type, value_type>(key)(element); }
//             
//             value_type       & access( access_type const & element )       { return viennadata::access<key_type, value_type>(key)(element); }
//             value_type const & access( access_type const & element ) const { return viennadata::access<key_type, value_type>(key)(element); }
// 
//             value_type       & access_unchecked( access_type const & element )       { return access(element); }
//             value_type const & access_unchecked( access_type const & element ) const { return access(element); }
//             
//             value_type       & operator()( access_type const & element )       { return access(element); }
//             value_type const & operator()( access_type const & element ) const { return access(element); }            
//             
//             void erase( access_type const & element )
//             {
//                 viennadata::erase<key_type, value_type>(key)(element);
//             }
//             
//             void clear()
//             {
// #ifdef _DEBUG_
//                 std::cout << "WARNING! calling clear on viennadata accessor: not supported" << std::endl;
// #endif
//             }
//             
//             void resize( std::size_t size ) {}
//             
//         private:
//             key_type key;
//         };
        
        
        
        template<typename container_type_, typename element_type_>
        class dense_container_accessor_t
        {
        public:
            
            typedef container_type_ container_type;
            typedef typename container_type::value_type value_type;
            typedef typename container_type::size_type  size_type;
            typedef element_type_ access_type;
            
            dense_container_accessor_t() : container(0) {}
            dense_container_accessor_t( container_type & container_ ) : container(&container_) {}
            
            typename container_type::pointer find( access_type const & element )        { return (element.id().get() >= (*container).size()) ? NULL : &(*container)[ element.id().get() ]; }
            typename container_type::const_pointer find( access_type const & element )  const { return (element.id().get() >= (*container).size()) ? NULL : &(*container)[ element.id().get() ]; }

            typename container_type::reference access_unchecked( access_type const & element )       { return (*container)[ element.id().get() ]; }
            typename container_type::const_reference access_unchecked( access_type const & element ) const { return (*container)[ element.id().get() ]; }
            
            typename container_type::reference access( access_type const & element )
            {
                if ((*container).size() <= static_cast<size_type>(element.id().get())) (*container).resize(element.id().get()+1);
//                 container.resize(element.id().get());
                return (*container)[ element.id().get() ];
            }
            typename container_type::const_reference access( access_type const & element ) const
            {
                assert((*container).size() > static_cast<size_type>(element.id().get()));
                return (*container)[ element.id().get() ];
            }

            typename container_type::reference operator()( access_type const & element )       { return access(element); }
            typename container_type::const_reference operator()( access_type const & element ) const { return access(element); }            
            
            void erase( access_type const & element )
            {
              if ((*container).size() > 0)
                if (static_cast<size_type>(element.id().get()) == (*container).size() - 1)
                    (*container).erase( --(*container).end() );
            }
            
            void clear() { (*container).clear(); }
            void resize( std::size_t size ) { (*container).resize(size); }
            
        private:
            container_type * container;
        };
        
        
        
        template<typename container_type_, typename element_type_>
        class dense_container_accessor_t<const container_type_, element_type_>
        {
        public:
            
            typedef container_type_ container_type;
            typedef typename container_type::value_type value_type;
            typedef typename container_type::size_type  size_type;
            typedef element_type_ access_type;
            
            dense_container_accessor_t() : container(0) {}
            dense_container_accessor_t( container_type const & container_ ) : container(&container_) {}
            
            typename container_type::const_pointer find( access_type const & element )  const { return (element.id().get() >= (*container).size()) ? NULL : &(*container)[ element.id().get() ]; }

            typename container_type::const_reference access_unchecked( access_type const & element ) const { return (*container)[ element.id().get() ]; }

            typename container_type::const_reference access( access_type const & element ) const
            {
                assert((*container).size() > static_cast<size_type>(element.id().get()));
                return (*container)[ element.id().get() ];
            }

            typename container_type::const_reference operator()( access_type const & element ) const { return access(element); }            
            
        private:
            container_type const * container;
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
        

        
        
        
        
        
    namespace accessor
    {
        
        template<typename ValueType, typename AccessType>
        class base_dynamic_accessor_t
        {
        public:
            typedef ValueType value_type;
            typedef AccessType access_type;
            
            typedef value_type & reference;
            typedef value_type const & const_reference;
            
            typedef value_type * pointer;
            typedef value_type const * const_pointer;
            
            virtual ~base_dynamic_accessor_t() {}
            
            virtual pointer find( access_type const & element ) { return 0; }
            virtual const_pointer find( access_type const & element ) const { return 0; }

            virtual reference access_unchecked( access_type const & element ) = 0;
            virtual const_reference access_unchecked( access_type const & element ) const = 0;
            
            virtual reference access( access_type const & element ) = 0;
            virtual const_reference access( access_type const & element ) const = 0;

            reference operator()( access_type const & element )       { return access(element); }
            const_reference operator()( access_type const & element ) const { return access(element); }            
            
            virtual void erase( access_type const & element ) {}
            
            virtual void clear() {}
            virtual void resize( std::size_t size ) {}
        };
        
        
        
        
        template<typename AccessorType>
        class dynamic_accessor_t : public base_dynamic_accessor_t< typename AccessorType::value_type, typename AccessorType::access_type >
        {
        public:
            typedef typename AccessorType::value_type value_type;
            typedef typename AccessorType::access_type access_type;
            
            typedef value_type & reference;
            typedef value_type const & const_reference;
            
            typedef value_type * pointer;
            typedef value_type const * const_pointer;
            
            
            dynamic_accessor_t(AccessorType accessor_) : accessor(accessor_) {}
            
            virtual pointer find( access_type const & element ) { return accessor.find(element); }
            virtual const_pointer find( access_type const & element ) const { return accessor.find(element); }

            virtual reference access_unchecked( access_type const & element ) { return accessor.access_unchecked(element); }
            virtual const_reference access_unchecked( access_type const & element ) const { return accessor.access_unchecked(element); }
            
            virtual reference access( access_type const & element ) { return accessor.access(element); }
            virtual const_reference access( access_type const & element ) const { return accessor.access(element); }

            reference operator()( access_type const & element )       { return access(element); }
            const_reference operator()( access_type const & element ) const { return access(element); }            
            
            virtual void erase( access_type const & element ) { accessor.erase(element); }
            
            virtual void clear() { accessor.clear(); }
            virtual void resize( std::size_t size ) { accessor.resize(size); }
            
        private:
          AccessorType accessor;
        };
        
    }
        
}



#endif
