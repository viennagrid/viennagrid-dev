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

#ifdef VIENNAGRID_WITH_VIENNADATA
#include "viennadata/api.hpp"


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
#endif


namespace viennagrid
{
    template<typename value_type_, typename element_type>
    class appendix_accessor_t
    {
    public:
        typedef value_type_ value_type;
        typedef element_type access_type;

        bool is_valid() const { return true; }

        value_type       * find( access_type & element )              { return &element.appendix(); }
        value_type const * find( access_type const & element )  const { return &element.appendix(); }

        value_type       & access_unchecked( access_type & element )       { return access(element); }
        value_type const & access_unchecked( access_type const & element ) const { return access(element); }

        value_type       & access( access_type & element )             { return element.appendix(); }
        value_type const & access( access_type const & element ) const { return element.appendix(); }

        value_type       & operator()( access_type & element )             { return access(element); }
        value_type const & operator()( access_type const & element ) const { return access(element); }


        void erase( access_type const & )
        {
          assert(false && bool("WARNING! calling erase on apppendix accessor: not supported"));
        }

        void clear()
        {
          assert(false && bool("WARNING! calling clear on apppendix accessor: not supported"));
        }

        void resize( std::size_t )
        {
          assert(false && bool("WARNING! calling clear on apppendix accessor: not supported"));
        }

    };



    template<typename domain_or_element_type>
    appendix_accessor_t<
      typename viennagrid::result_of::point_type<domain_or_element_type>::type,
      typename viennagrid::result_of::vertex<domain_or_element_type>::type
      > default_point_accessor( domain_or_element_type const & )
    {
      return appendix_accessor_t<
        typename viennagrid::result_of::point_type<domain_or_element_type>::type,
        typename viennagrid::result_of::vertex<domain_or_element_type>::type
      >();
    }


    namespace result_of
    {
        template<typename value_type, typename WrappedConfigType>
        struct point_type< appendix_accessor_t<value_type, element_t<vertex_tag, WrappedConfigType> > >
        {
            typedef value_type type;
        };

        template<typename value_type, typename WrappedConfigType>
        struct point_type< const appendix_accessor_t<value_type, element_t<vertex_tag, WrappedConfigType> > >
        {
            typedef value_type type;
        };
    }








    template<typename ContainerType, typename AccessType>
    class dense_container_accessor_t
    {
    public:

      typedef ContainerType                                           container_type;
      typedef typename ContainerType::value_type                      value_type;
      typedef AccessType                                              access_type;

      typedef typename ContainerType::reference       reference;
      typedef typename ContainerType::const_reference const_reference;

      typedef typename ContainerType::pointer         pointer;
      typedef typename ContainerType::const_pointer   const_pointer;

      typedef typename access_type::id_type::base_id_type offset_type;
      
      dense_container_accessor_t() : container(0) {}
      dense_container_accessor_t( ContainerType & container_ ) : container(&container_) {}

      bool is_valid() const { return container != NULL; }

      pointer find(AccessType const & element)
      {
        offset_type offset = element.id().get();
        return (static_cast<offset_type>((*container).size()) > element.id().get()) ? (&(*container)[offset]) : NULL;
      }

      const_pointer find(AccessType const & element) const
      {
        offset_type offset = element.id().get();
        return (static_cast<offset_type>((*container).size()) > element.id().get()) ? (&(*container)[offset]) : NULL;
      }

      reference access_unchecked(AccessType const & element)
      {
        offset_type offset = element.id().get();
        return (*container)[offset];
      }

      const_reference access_unchecked(AccessType const & element) const
      {
        offset_type offset = element.id().get();

        assert( static_cast<offset_type>((*container).size()) > offset );
        return (*container)[offset];
      }

      reference access(AccessType const & element)
      {
        offset_type offset = element.id().get();

        if ( static_cast<offset_type>((*container).size()) <= offset) (*container).resize(offset+1);
        return (*container)[offset];
      }

      const_reference access(AccessType const & element) const
      { return access_unchecked(element); }

      reference       operator()(AccessType const & element)       { return access(element); }
      const_reference operator()(AccessType const & element) const { return access(element); }


      void erase(AccessType const & element)
      {
        offset_type offset = element.id().get();

        if (offset+1 == static_cast<offset_type>((*container).size()))
            (*container).resize((*container).size()-1);
      }

      void clear()
      { (*container).clear(); }

      void resize( std::size_t size )
      { (*container).resize( size ); }
      
      
    private:
      ContainerType * container;
    };


    template<typename ContainerType, typename AccessType>
    class dense_container_accessor_t<const ContainerType, AccessType>
    {
    public:

      typedef const ContainerType                                     container_type;
      typedef typename ContainerType::value_type                      value_type;
      typedef AccessType                                              access_type;

      typedef typename ContainerType::const_reference       reference;
      typedef typename ContainerType::const_reference const_reference;

      typedef typename ContainerType::const_pointer         pointer;
      typedef typename ContainerType::const_pointer   const_pointer;

      typedef typename access_type::id_type::base_id_type offset_type;

      dense_container_accessor_t() : container(0) {}
      dense_container_accessor_t( container_type & container_ ) : container(&container_) {}

      bool is_valid() const { return container != NULL; }

      const_pointer find(AccessType const & element) const
      {
        offset_type offset = element.id().get();
        return (static_cast<offset_type>((*container).size()) > element.id().get()) ? (&(*container)[offset]) : NULL;
      }

      const_reference access_unchecked(AccessType const & element) const
      {
        offset_type offset = element.id().get();

        assert( static_cast<offset_type>((*container).size()) > offset );
        return (*container)[offset];
      }

      const_reference access(AccessType const & element) const
      { return access_unchecked(element); }

      const_reference operator()(AccessType const & element) const { return access(element); }

      void erase(AccessType const & element);
      void clear();
      void resize( std::size_t size );


    private:
      container_type * container;
    };




    template<typename ContainerType, typename AccessType>
    class std_map_accessor_t
    {
    public:

      typedef ContainerType                                           container_type;
      typedef typename ContainerType::value_type::second_type         value_type;
      typedef typename ContainerType::value_type::first_type          key_type;
      typedef AccessType                                              access_type;

      typedef value_type &       reference;
      typedef value_type const & const_reference;

      typedef value_type *         pointer;
      typedef value_type const *   const_pointer;

      std_map_accessor_t() : container(0) {}
      std_map_accessor_t( ContainerType & container_ ) : container(&container_) {}

      bool is_valid() const { return container != NULL; }

      pointer find(AccessType const & element)
      {
        typename container_type::iterator it = (*container).find( element.id() );
        return (it != (*container).end()) ? &it->second : NULL; // return NULL if not found
      }

      const_pointer find(AccessType const & element) const
      {
        typename container_type::const_iterator it = (*container).find( element.id() );
        return (it != (*container).end()) ? &it->second : NULL; // return NULL if not found
      }

      reference access_unchecked(AccessType const & element)
      {
        return (*container)[ element.id() ];
      }

      const_reference access_unchecked(AccessType const & element) const
      {
        typename container_type::const_iterator it = (*container).find( element.id() );
        assert(it != (*container).end()); // no release-runtime check for accessing elements outside (*container)
        return it->second;
      }

      reference access(AccessType const & element)
      { return access_unchecked(element); }

      const_reference access(AccessType const & element) const
      { return access_unchecked(element); }

      reference       operator()(AccessType const & element)       { return access(element); }
      const_reference operator()(AccessType const & element) const { return access(element); }


      void erase(AccessType const & element)
      {
        typename container_type::iterator it = (*container).find( element.id() );
        if (it != (*container).end())
          (*container).erase(it);
      }
      void clear()
      { (*container).clear(); }
      void resize( std::size_t size ) {}


    private:
      ContainerType * container;
    };


    template<typename ContainerType, typename AccessType>
    class std_map_accessor_t<const ContainerType, AccessType>
    {
    public:

      typedef const ContainerType                                     container_type;
      typedef typename ContainerType::value_type::second_type         value_type;
      typedef typename ContainerType::value_type::first_type          key_type;
      typedef AccessType                                              access_type;

      typedef value_type const &       reference;
      typedef value_type const & const_reference;

      typedef value_type const *         pointer;
      typedef value_type const *   const_pointer;

      std_map_accessor_t() : container(0) {}
      std_map_accessor_t( container_type & container_ ) : container(&container_) {}

      bool is_valid() const { return container != NULL; }

      const_pointer find(AccessType const & element) const
      {
        typename container_type::const_iterator it = (*container).find( element.id() );
        return (it != (*container).end()) ? &it->second : NULL; // return NULL if not found
      }

      const_reference access_unchecked(AccessType const & element) const
      {
        typename container_type::const_iterator it = (*container).find( element.id() );
        assert(it != (*container).end()); // no release-runtime check for accessing elements outside (*container)
        return it->second;
      }

      const_reference access(AccessType const & element) const
      { return access_unchecked(element); }

      const_reference operator()(AccessType const & element) const { return access(element); }


      void erase(AccessType const & element);
      void clear();
      void resize( std::size_t size );


    private:
      container_type * container;
    };







    
    namespace result_of
    {
        template<typename ContainerType, typename AccessType>
        struct accessor;

        template<typename T, typename Alloc, typename AccessType>
        struct accessor< std::vector<T, Alloc>, AccessType >
        {
            typedef dense_container_accessor_t<std::vector<T, Alloc>, AccessType> type;
        };

        template<typename T, typename Alloc, typename AccessType>
        struct accessor< const std::vector<T, Alloc>, AccessType >
        {
            typedef dense_container_accessor_t<const std::vector<T, Alloc>, AccessType> type;
        };

        template<typename T, typename Alloc, typename AccessType>
        struct accessor< std::deque<T, Alloc>, AccessType >
        {
            typedef dense_container_accessor_t<std::deque<T, Alloc>, AccessType> type;
        };

        template<typename T, typename Alloc, typename AccessType>
        struct accessor< const std::deque<T, Alloc>, AccessType >
        {
            typedef dense_container_accessor_t<const std::deque<T, Alloc>, AccessType> type;
        };

        template<typename Key, typename T, typename Compare, typename Alloc, typename AccessType>
        struct accessor< std::map<Key, T, Compare, Alloc>, AccessType >
        {
            typedef std_map_accessor_t<std::map<Key, T, Compare, Alloc>, AccessType> type;
        };

        template<typename Key, typename T, typename Compare, typename Alloc, typename AccessType>
        struct accessor< const std::map<Key, T, Compare, Alloc>, AccessType >
        {
            typedef std_map_accessor_t<const std::map<Key, T, Compare, Alloc>, AccessType> type;
        };
    }



    template<typename AccessType, typename ContainerType>
    typename result_of::accessor<ContainerType, AccessType>::type make_accessor( ContainerType & container )
    {
        return typename result_of::accessor<ContainerType, AccessType>::type(container);
    }

    template<typename AccessType, typename ContainerType>
    typename result_of::accessor<const ContainerType, AccessType>::type make_accessor( ContainerType const & container )
    {
        return typename result_of::accessor<const ContainerType, AccessType>::type(container);
    }



    template<typename element_type, typename container_collection_typemap>
    typename result_of::accessor<
        typename storage::result_of::container_of<
            container_collection_typemap,
            element_type
        >::type,
        element_type>::type make_accessor( storage::collection_t<container_collection_typemap> & collection )
    {
        return make_accessor<element_type>( storage::collection::get<element_type>(collection) );
    }

    template<typename element_type, typename container_collection_typemap>
    typename result_of::accessor<
        const typename storage::result_of::container_of<
            container_collection_typemap,
            element_type
        >::type,
        element_type>::type make_accessor( storage::collection_t<container_collection_typemap> const & collection )
    {
        return make_accessor<element_type>( storage::collection::get<element_type>(collection) );
    }




#ifdef VIENNAGRID_WITH_VIENNADATA
    namespace result_of
    {
        template<typename ContainerType, typename AccessType, typename AccessTag>
        struct point_type< viennadata::container_accessor<ContainerType, AccessType, AccessTag> >
        {
            typedef typename viennadata::container_accessor<ContainerType, AccessType, AccessTag>::value_type type;
        };

        template<typename ContainerType, typename AccessType, typename AccessTag>
        struct point_type< const viennadata::container_accessor<ContainerType, AccessType, AccessTag> >
        {
            typedef typename viennadata::container_accessor<ContainerType, AccessType, AccessTag>::value_type type;
        };
    }
#endif





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

        virtual pointer find( access_type const & ) { return 0; }
        virtual const_pointer find( access_type const & ) const { return 0; }

        virtual reference access_unchecked( access_type const & element ) = 0;
        virtual const_reference access_unchecked( access_type const & element ) const = 0;

        virtual reference access( access_type const & element ) = 0;
        virtual const_reference access( access_type const & element ) const = 0;

        reference operator()( access_type const & element )       { return access(element); }
        const_reference operator()( access_type const & element ) const { return access(element); }

        virtual void erase( access_type const & ) {}

        virtual void clear() {}
        virtual void resize( std::size_t ) {}
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



#endif
