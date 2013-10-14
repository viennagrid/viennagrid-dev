#ifndef VIENNAGRID_ACCESSOR_HPP
#define VIENNAGRID_ACCESSOR_HPP

/* =======================================================================
   Copyright (c) 2011-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <stdexcept>
#include <assert.h>

#include "viennagrid/forwards.hpp"
#include "viennagrid/storage/id.hpp"
#include "viennagrid/storage/container_collection.hpp"

/** @file viennagrid/mesh/accessor.hpp
    @brief Defines various accessors for storing data for the different element types.
*/

#ifdef VIENNAGRID_WITH_VIENNADATA
#include "viennadata/api.hpp"


namespace viennadata
{
  namespace result_of
  {
    template<typename value_type, typename base_id_type>
    struct offset< viennagrid::storage::smart_id<value_type, base_id_type> >
    {
      typedef viennagrid::storage::smart_id<value_type, base_id_type> id_type;
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


  namespace result_of
  {

    template<typename mesh_or_element_type>
    struct default_point_accessor
    {
      typedef appendix_accessor_t<
        typename viennagrid::result_of::point<mesh_or_element_type>::type,
        typename viennagrid::result_of::vertex<mesh_or_element_type>::type
      > type;
    };

  }

  template<typename mesh_or_element_type>
  typename result_of::default_point_accessor<mesh_or_element_type>::type default_point_accessor( mesh_or_element_type const & )
  {
    return appendix_accessor_t<
      typename viennagrid::result_of::point<mesh_or_element_type>::type,
      typename viennagrid::result_of::vertex<mesh_or_element_type>::type
    >();
  }


  namespace result_of
  {
    template<typename ElementT, typename ValueT, typename ContainerTagT = storage::std_vector_tag>
    struct accessor_container;

    template<typename ElementT, typename ValueT>
    struct accessor_container<ElementT, ValueT, storage::std_vector_tag>
    {
      typedef std::vector<ValueT> type;
    };

    template<typename ElementT, typename ValueT>
    struct accessor_container<ElementT, ValueT, storage::std_deque_tag>
    {
      typedef std::deque<ValueT> type;
    };

    template<typename ElementT, typename ValueT>
    struct accessor_container<ElementT, ValueT, storage::std_map_tag>
    {
      typedef std::map< typename result_of::id<ElementT>::type, ValueT > type;
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

    reference operator()(AccessType const & element)
    {
      offset_type offset = element.id().get();
      if ( static_cast<offset_type>((*container).size()) <= offset) (*container).resize(offset+1);
      return (*container)[offset];
    }

    const_reference operator()(AccessType const & element) const
    {
      offset_type offset = element.id().get();
      assert( static_cast<offset_type>((*container).size()) > offset );
      return (*container)[offset];
    }

    reference at(AccessType const & element)
    {
      offset_type offset = element.id().get();
      if ( static_cast<offset_type>((*container).size()) <= offset) throw std::out_of_range("dense_container_accessor_t::at() failed");
      return (*container)[offset];
    }

    const_reference at(AccessType const & element) const
    {
      offset_type offset = element.id().get();
      if ( static_cast<offset_type>((*container).size()) <= offset) throw std::out_of_range("dense_container_accessor_t::at() const failed");
      return (*container)[offset];
    }

  protected:
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

    const_reference operator()(AccessType const & element) const
    {
      offset_type offset = element.id().get();
      assert( static_cast<offset_type>((*container).size()) > offset );
      return (*container)[offset];
    }

    const_reference at(AccessType const & element) const
    {
      offset_type offset = element.id().get();
      if ( static_cast<offset_type>((*container).size()) <= offset) throw std::out_of_range("dense_container_accessor_t::at() const failed");
      return (*container)[offset];
    }

    void erase(AccessType const & element);
    void clear();
    void resize( std::size_t size );


  protected:
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

    reference operator()(AccessType const & element)
    {
      return (*container)[ element.id() ];
    }

    const_reference operator()(AccessType const & element) const
    {
      typename container_type::const_iterator it = (*container).find( element.id() );
      assert(it != (*container).end()); // no release-runtime check for accessing elements outside (*container)
      return it->second;
    }

    reference at(AccessType const & element)
    {
      return (*this)(element);
    }

    const_reference at(AccessType const & element) const
    {
      typename container_type::const_iterator it = (*container).find( element.id() );
      if (it == (*container).end()) throw std::out_of_range("std_map_accessor_t::at() const failed");
      return it->second;
    }

  protected:
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

    const_reference operator()(AccessType const & element) const
    {
      typename container_type::const_iterator it = (*container).find( element.id() );
      assert(it != (*container).end()); // no release-runtime check for accessing elements outside (*container)
      return it->second;
    }

    const_reference at(AccessType const & element) const
    {
      typename container_type::const_iterator it = (*container).find( element.id() );
      if (it == (*container).end()) throw std::out_of_range("std_map_accessor_t::at() const failed");
      return it->second;
    }

  protected:
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
      element_type>::type make_accessor( storage::collection<container_collection_typemap> & collection )
  {
    return make_accessor<element_type>( storage::detail::get<element_type>(collection) );
  }

  template<typename element_type, typename container_collection_typemap>
  typename result_of::accessor<
      const typename storage::result_of::container_of<
          container_collection_typemap,
          element_type
      >::type,
      element_type>::type make_accessor( storage::collection<container_collection_typemap> const & collection )
  {
    return make_accessor<element_type>( storage::detail::get<element_type>(collection) );
  }




  template<typename ValueType, typename AccessType>
  class base_dynamic_accessor_t
  {
  public:
    typedef ValueType  value_type;
    typedef AccessType access_type;

    typedef value_type       &       reference;
    typedef value_type const & const_reference;

    typedef value_type       *       pointer;
    typedef value_type const * const_pointer;

    virtual ~base_dynamic_accessor_t() {}

    virtual       pointer find( access_type const & ) { return 0; }
    virtual const_pointer find( access_type const & ) const { return 0; }

    virtual       reference operator()( access_type const & element ) = 0;
    virtual const_reference operator()( access_type const & element ) const = 0;

    virtual       reference access( access_type const & element ) = 0;
    virtual const_reference access( access_type const & element ) const = 0;
  };

  template<typename ValueType, typename AccessType>
  class base_dynamic_accessor_t<const ValueType, AccessType>
  {
  public:
    typedef ValueType value_type;
    typedef AccessType access_type;

    typedef value_type const & reference;
    typedef value_type const & const_reference;

    typedef value_type const * pointer;
    typedef value_type const * const_pointer;

    virtual ~base_dynamic_accessor_t() {}

    virtual const_pointer find( access_type const & ) const { return 0; }
    virtual const_reference operator()( access_type const & element ) const = 0;
    virtual const_reference access( access_type const & element ) const = 0;
  };




  template<typename AccessorType>
  class dynamic_accessor_t : public base_dynamic_accessor_t< typename AccessorType::value_type, typename AccessorType::access_type >
  {
  public:
    typedef base_dynamic_accessor_t< typename AccessorType::value_type, typename AccessorType::access_type > BaseAccessorType;

    typedef typename BaseAccessorType::value_type value_type;
    typedef typename BaseAccessorType::access_type access_type;

    typedef typename BaseAccessorType::reference reference;
    typedef typename BaseAccessorType::const_reference const_reference;

    typedef typename BaseAccessorType::pointer pointer;
    typedef typename BaseAccessorType::const_pointer const_pointer;


    dynamic_accessor_t(AccessorType accessor_) : accessor(accessor_) {}

    virtual pointer find( access_type const & element ) { return accessor.find(element); }
    virtual const_pointer find( access_type const & element ) const { return accessor.find(element); }

    virtual reference operator()( access_type const & element )       { return access(element); }
    virtual const_reference operator()( access_type const & element ) const { return access(element); }

    virtual reference access( access_type const & element ) { return accessor.access(element); }
    virtual const_reference access( access_type const & element ) const { return accessor.access(element); }

  private:
    AccessorType accessor;
  };


  template<typename AccessorType>
  class dynamic_accessor_t<const AccessorType> : public base_dynamic_accessor_t< const typename AccessorType::value_type, typename AccessorType::access_type >
  {
  public:
    typedef base_dynamic_accessor_t< const typename AccessorType::value_type, typename AccessorType::access_type > BaseAccessorType;

    typedef typename BaseAccessorType::value_type value_type;
    typedef typename BaseAccessorType::access_type access_type;

    typedef typename BaseAccessorType::const_reference reference;
    typedef typename BaseAccessorType::const_reference const_reference;

    typedef typename BaseAccessorType::const_pointer pointer;
    typedef typename BaseAccessorType::const_pointer const_pointer;


    dynamic_accessor_t(AccessorType accessor_) : accessor(accessor_) {}

    virtual const_pointer find( access_type const & element ) const { return accessor.find(element); }
    virtual const_reference operator()( access_type const & element ) const { return access(element); }
    virtual const_reference access( access_type const & element ) const { return accessor.access(element); }

  private:
    AccessorType accessor;
  };














  template<typename ContainerType, typename AccessType>
  class dense_container_field_t
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

    dense_container_field_t() : default_value() {}
    dense_container_field_t( ContainerType & container_ ) : container(&container_), default_value() {}
    dense_container_field_t( ContainerType & container_, value_type const & value_type_ ) : container(&container_), default_value(value_type_) {}

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

    reference operator()(AccessType const & element)
    {
      offset_type offset = element.id().get();
      if ( static_cast<offset_type>((*container).size()) <= offset) (*container).resize(offset+1);
      return (*container)[offset];
    }

    value_type operator()(AccessType const & element) const
    {
      offset_type offset = element.id().get();

      if ( static_cast<offset_type>((*(this->container)).size()) <= offset)
        return default_value;

      return (*(this->container))[offset];
    }

    reference at(AccessType const & element)
    {
      offset_type offset = element.id().get();
      if ( static_cast<offset_type>((*container).size()) <= offset) throw std::out_of_range("dense_container_accessor_t::at() failed");
      return (*container)[offset];
    }

    const_reference at(AccessType const & element) const
    {
      offset_type offset = element.id().get();
      if ( static_cast<offset_type>((*container).size()) <= offset) throw std::out_of_range("dense_container_accessor_t::at() failed");
      return (*container)[offset];
    }



  protected:
    ContainerType * container;
    value_type default_value;
  };


  template<typename ContainerType, typename AccessType>
  class dense_container_field_t<const ContainerType, AccessType>
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

    dense_container_field_t() : default_value() {}
    dense_container_field_t( ContainerType const & container_ ) : container(&container_), default_value() {}
    dense_container_field_t( ContainerType const & container_, value_type const & value_type_ ) : container(&container_), default_value(value_type_) {}

    bool is_valid() const { return container != NULL; }

    const_pointer find(AccessType const & element) const
    {
      offset_type offset = element.id().get();
      return (static_cast<offset_type>((*container).size()) > element.id().get()) ? (&(*container)[offset]) : NULL;
    }

    value_type operator()(AccessType const & element) const
    {
      offset_type offset = element.id().get();

      if ( static_cast<offset_type>((*(this->container)).size()) <= offset)
        return default_value;

      return (*(this->container))[offset];
    }

    const_reference at(AccessType const & element) const
    {
      offset_type offset = element.id().get();
      if ( static_cast<offset_type>((*container).size()) <= offset) throw std::out_of_range("dense_container_accessor_t::at() failed");
      return (*container)[offset];
    }

    void erase(AccessType const & element);
    void clear();
    void resize( std::size_t size );


  protected:
    container_type * container;
    value_type default_value;
  };




  template<typename ContainerType, typename AccessType>
  class std_map_field_t
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

    std_map_field_t() : default_value() {}
    std_map_field_t( ContainerType & container_ ) : container(&container_), default_value() {}
    std_map_field_t( ContainerType & container_, value_type const & value_type_ ) : container(&container_), default_value(value_type_) {}

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

    reference operator()(AccessType const & element)
    {
      return (*container)[ element.id() ];
    }

    value_type operator()(AccessType const & element) const
    {
      typename container_type::const_iterator it = (*(this->container)).find( element.id() );

      if (it == (*(this->container)).end())
        return default_value;

      return it->second;
    }

    reference at(AccessType const & element)
    {
      return (*this)(element);
    }

    const_reference at(AccessType const & element) const
    {
      typename container_type::const_iterator it = (*container).find( element.id() );
      if (it == (*container).end()) throw std::out_of_range("std_map_accessor_t::at() const failed");
      return it->second;
    }

  protected:
    ContainerType * container;
    value_type default_value;
  };


  template<typename ContainerType, typename AccessType>
  class std_map_field_t<const ContainerType, AccessType>
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

    std_map_field_t() : default_value() {}
    std_map_field_t( ContainerType const & container_ ) : container(&container_), default_value() {}
    std_map_field_t( ContainerType const & container_, value_type const & value_type_ ) : container(&container_), default_value(value_type_) {}

    bool is_valid() const { return container != NULL; }

    const_pointer find(AccessType const & element) const
    {
      typename container_type::const_iterator it = (*container).find( element.id() );
      return (it != (*container).end()) ? &it->second : NULL; // return NULL if not found
    }

    value_type operator()(AccessType const & element) const
    {
      typename container_type::const_iterator it = (*(this->container)).find( element.id() );

      if (it == (*(this->container)).end())
        return default_value;

      return it->second;
    }

    const_reference at(AccessType const & element) const
    {
      typename container_type::const_iterator it = (*container).find( element.id() );
      if (it == (*container).end()) throw std::out_of_range("std_map_accessor_t::at() const failed");
      return it->second;
    }

  protected:
    container_type * container;
    value_type default_value;
  };





  namespace result_of
  {
    template<typename ContainerType, typename AccessType>
    struct field;

    template<typename T, typename Alloc, typename AccessType>
    struct field< std::vector<T, Alloc>, AccessType >
    {
      typedef dense_container_field_t<std::vector<T, Alloc>, AccessType> type;
    };

    template<typename T, typename Alloc, typename AccessType>
    struct field< const std::vector<T, Alloc>, AccessType >
    {
      typedef dense_container_field_t<const std::vector<T, Alloc>, AccessType> type;
    };

    template<typename T, typename Alloc, typename AccessType>
    struct field< std::deque<T, Alloc>, AccessType >
    {
      typedef dense_container_field_t<std::deque<T, Alloc>, AccessType> type;
    };

    template<typename T, typename Alloc, typename AccessType>
    struct field< const std::deque<T, Alloc>, AccessType >
    {
      typedef dense_container_field_t<const std::deque<T, Alloc>, AccessType> type;
    };

    template<typename Key, typename T, typename Compare, typename Alloc, typename AccessType>
    struct field< std::map<Key, T, Compare, Alloc>, AccessType >
    {
      typedef std_map_field_t<std::map<Key, T, Compare, Alloc>, AccessType> type;
    };

    template<typename Key, typename T, typename Compare, typename Alloc, typename AccessType>
    struct field< const std::map<Key, T, Compare, Alloc>, AccessType >
    {
      typedef std_map_field_t<const std::map<Key, T, Compare, Alloc>, AccessType> type;
    };
  }



  template<typename AccessType, typename ContainerType>
  typename result_of::field<ContainerType, AccessType>::type make_field( ContainerType & container )
  {
    return typename result_of::field<ContainerType, AccessType>::type(container);
  }

  template<typename AccessType, typename ContainerType>
  typename result_of::field<const ContainerType, AccessType>::type make_field( ContainerType const & container )
  {
    return typename result_of::field<const ContainerType, AccessType>::type(container);
  }



  template<typename element_type, typename container_collection_typemap>
  typename result_of::field<
      typename storage::result_of::container_of<
          container_collection_typemap,
          element_type
      >::type,
      element_type>::type make_field( storage::collection<container_collection_typemap> & collection )
  {
    return make_field<element_type>( storage::detail::get<element_type>(collection) );
  }

  template<typename element_type, typename container_collection_typemap>
  typename result_of::field<
      const typename storage::result_of::container_of<
          container_collection_typemap,
          element_type
      >::type,
      element_type>::type make_field( storage::collection<container_collection_typemap> const & collection )
  {
    return make_field<element_type>( storage::detail::get<element_type>(collection) );
  }







  template<typename ValueType, typename AccessType>
  class base_dynamic_field_t
  {
  public:
    typedef ValueType value_type;
    typedef AccessType access_type;

    typedef value_type & reference;
    typedef value_type const & const_reference;

    typedef value_type * pointer;
    typedef value_type const * const_pointer;

    virtual ~base_dynamic_field_t() {}

    virtual pointer find( access_type const & ) { return 0; }
    virtual const_pointer find( access_type const & ) const { return 0; }

    virtual reference operator()( access_type const & element ) = 0;
    virtual value_type operator()( access_type const & element ) const = 0;

    virtual reference at( access_type const & element ) = 0;
    virtual const_reference at( access_type const & element ) const = 0;
  };

  template<typename ValueType, typename AccessType>
  class base_dynamic_field_t<const ValueType, AccessType>
  {
  public:
    typedef ValueType value_type;
    typedef AccessType access_type;

    typedef value_type const & reference;
    typedef value_type const & const_reference;

    typedef value_type const * pointer;
    typedef value_type const * const_pointer;

    virtual ~base_dynamic_field_t() {}

    virtual const_pointer find( access_type const & ) const { return 0; }
    virtual value_type operator()( access_type const & element ) const = 0;
    virtual const_reference at( access_type const & element ) const = 0;
  };




  template<typename FieldType>
  class dynamic_field_t : public base_dynamic_field_t< typename FieldType::value_type, typename FieldType::access_type >
  {
  public:
    typedef base_dynamic_field_t< typename FieldType::value_type, typename FieldType::access_type > BaseFieldType;

    typedef typename BaseFieldType::value_type value_type;
    typedef typename BaseFieldType::access_type access_type;

    typedef typename BaseFieldType::reference reference;
    typedef typename BaseFieldType::const_reference const_reference;

    typedef typename BaseFieldType::pointer pointer;
    typedef typename BaseFieldType::const_pointer const_pointer;


    dynamic_field_t(FieldType field_) : field(field_) {}

    virtual pointer find( access_type const & element ) { return field.find(element); }
    virtual const_pointer find( access_type const & element ) const { return field.find(element); }

    virtual reference operator()( access_type const & element )       { return field(element); }
    virtual value_type operator()( access_type const & element ) const { return field(element); }

    virtual reference at( access_type const & element ) { return field.at(element); }
    virtual const_reference at( access_type const & element ) const { return field.at(element); }

  private:
    FieldType field;
  };


  template<typename FieldType>
  class dynamic_field_t<const FieldType> : public base_dynamic_field_t< const typename FieldType::value_type, typename FieldType::access_type >
  {
  public:
    typedef base_dynamic_field_t< const typename FieldType::value_type, typename FieldType::access_type > BaseFieldType;

    typedef typename BaseFieldType::value_type value_type;
    typedef typename BaseFieldType::access_type access_type;

    typedef typename BaseFieldType::const_reference reference;
    typedef typename BaseFieldType::const_reference const_reference;

    typedef typename BaseFieldType::const_pointer pointer;
    typedef typename BaseFieldType::const_pointer const_pointer;


    dynamic_field_t(FieldType field_) : field(field_) {}

    virtual const_pointer find( access_type const & element ) const { return field.find(element); }
    virtual value_type operator()( access_type const & element ) const { return field(element); }
    virtual const_reference at( access_type const & element ) const { return field.at(element); }

  private:
    FieldType field;
  };



#ifdef VIENNAGRID_WITH_VIENNADATA
  namespace result_of
  {
    template<typename ContainerType, typename AccessType, typename AccessTag>
    struct point< viennadata::container_accessor<ContainerType, AccessType, AccessTag> >
    {
      typedef typename viennadata::container_accessor<ContainerType, AccessType, AccessTag>::value_type type;
    };

    template<typename ContainerType, typename AccessType, typename AccessTag>
    struct point< const viennadata::container_accessor<ContainerType, AccessType, AccessTag> >
    {
      typedef typename viennadata::container_accessor<ContainerType, AccessType, AccessTag>::value_type type;
    };
  }
#endif


}


#endif
