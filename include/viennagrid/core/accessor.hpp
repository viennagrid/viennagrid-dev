#ifndef VIENNAGRID_ACCESSOR_HPP
#define VIENNAGRID_ACCESSOR_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include <stdexcept>
#include <assert.h>

#include <vector>
#include <deque>
#include <map>

#include "viennagrid/core/forwards.hpp"

/** @file viennagrid/accessor.hpp
    @brief Defines various accessors for storing data for the different element types.
*/


namespace viennagrid
{
  struct std_deque_tag;
  struct std_map_tag;
  struct std_vector_tag;



  namespace result_of
  {
    /** @brief Returns the underlying container type for storing data of type 'ValueT' on elements of type 'ElementT'.
      *
      * @tparam ElementT       The element type, e.g. a cell
      * @tparam ValueT         Type of the data to be stored
      * @tparam ContainerTagT  A tag selecting the appropriate container, e.g. std_vector_tag or std_map_tag
      */
    template<typename ElementT, typename ValueT, typename ContainerTagT = std_vector_tag>
    struct accessor_container {};

    /** \cond */
    template<typename ElementT, typename ValueT>
    struct accessor_container<ElementT, ValueT, std_vector_tag>
    {
      typedef std::vector<ValueT> type;
    };

    template<typename ElementT, typename ValueT>
    struct accessor_container<ElementT, ValueT, std_deque_tag>
    {
      typedef std::deque<ValueT> type;
    };

    template<typename ElementT, typename ValueT>
    struct accessor_container<ElementT, ValueT, std_map_tag>
    {
      typedef std::map< typename result_of::id<ElementT>::type, ValueT > type;
    };
    /** \endcond */
  }





  struct id_unpack
  {
    template<typename ElementT>
    typename viennagrid::result_of::id<ElementT>::type::index_type operator()(ElementT const & element) const
    { return element.id().index(); }
  };


  namespace result_of
  {
    template<typename ContainerT>
    struct unpack;

    template<typename T, typename Alloc>
    struct unpack< std::vector<T, Alloc> >
    {
      typedef id_unpack type;
    };

    template<typename T, typename Alloc>
    struct unpack< const std::vector<T, Alloc> >
    {
      typedef id_unpack type;
    };

    template<typename T, typename Alloc>
    struct unpack< std::deque<T, Alloc> >
    {
      typedef id_unpack type;
    };

    template<typename T, typename Alloc>
    struct unpack< const std::deque<T, Alloc> >
    {
      typedef id_unpack type;
    };

    template<typename Key, typename T, typename Compare, typename Alloc>
    struct unpack< std::map<Key, T, Compare, Alloc> >
    {
      typedef id_unpack type;
    };

    template<typename Key, typename T, typename Compare, typename Alloc>
    struct unpack< const std::map<Key, T, Compare, Alloc> >
    {
      typedef id_unpack type;
    };


    template<typename UnpackT, typename AccessT>
    struct unpack_id_type;

    template<typename AccessT>
    struct unpack_id_type<id_unpack, AccessT>
    {
      typedef typename AccessT::id_type::index_type type;
    };

  }




  /** @brief Implementation of an accessor for dense containers (most importantly std::vector, std::deque) which fulfills the accessor concept.
    *
    * @tparam   ContainerType    The underlying container type (e.g. std::vector<>)
    * @tparam   AccessType       The element from which data should be accessed
    */
  template<typename ContainerType, typename AccessType, typename UnpackT = id_unpack>
  class dense_container_accessor
  {
  public:

    typedef ContainerType                                           container_type;
    typedef typename ContainerType::value_type                      value_type;
    typedef AccessType                                              access_type;

    typedef typename ContainerType::reference       reference;
    typedef typename ContainerType::const_reference const_reference;

    typedef typename ContainerType::pointer         pointer;
    typedef typename ContainerType::const_pointer   const_pointer;

    typedef typename result_of::unpack_id_type<UnpackT, AccessType>::type offset_type;

    dense_container_accessor() : container(0) {}
    dense_container_accessor( ContainerType & container_ ) : container(&container_) {}

    bool valid() const { return container != NULL; }
    bool valid(AccessType const &) const {return true;}

    value_type get(AccessType const & element) const
    {
      offset_type offset = unpack(element);
      assert( static_cast<offset_type>((*container).size()) > offset );
      return (*container)[static_cast<std::size_t>(offset)];
    }

    void set(AccessType const & element, value_type const & value)
    {
      offset_type offset = unpack(element);
      if ( static_cast<offset_type>((*container).size()) <= offset)
        (*container).resize(offset+1);
      (*container)[static_cast<std::size_t>(offset)] = value;
    }

  protected:
    UnpackT unpack;
    ContainerType * container;
  };






  /** @brief  An accessor (fulfilling the accessor concept) for a container of interface similar to std::map<> */
  template<typename ContainerType, typename AccessType, typename UnpackT = id_unpack>
  class std_map_accessor
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

    std_map_accessor() : container(0) {}
    std_map_accessor( ContainerType & container_ ) : container(&container_) {}

    bool valid() const { return container != NULL; }
    bool valid(AccessType const & element) const {return container->find(unpack(element)) != container->end();}

    value_type get(AccessType const & element) const
    {
      return (*container)[ unpack(element) ];
    }

    void set(AccessType const & element, value_type const & value)
    {
      (*container)[ unpack(element) ] = value;
    }

  protected:
    UnpackT unpack;
    ContainerType * container;
  };


  namespace result_of
  {
    /** @brief Metafunction for retrieving the accessor type for a certain container and a certain object type from which to access data
      *
      * @tparam ContainerType   The container type for which the accessor should be obtained
      * @tparam AccessType      Type of the element which should be accessed, e.g. a vertex
      */
    template<typename ContainerType, typename AccessType, typename UnpackT = typename viennagrid::result_of::unpack<ContainerType>::type>
    struct accessor;

    /** \cond */
    template<typename T, typename Alloc, typename AccessType, typename UnpackT>
    struct accessor< std::vector<T, Alloc>, AccessType, UnpackT >
    {
      typedef viennagrid::dense_container_accessor<std::vector<T, Alloc>, AccessType, UnpackT> type;
    };

    template<typename T, typename Alloc, typename AccessType, typename UnpackT>
    struct accessor< const std::vector<T, Alloc>, AccessType, UnpackT >
    {
      typedef viennagrid::dense_container_accessor<const std::vector<T, Alloc>, AccessType, UnpackT> type;
    };

    template<typename T, typename Alloc, typename AccessType, typename UnpackT>
    struct accessor< std::deque<T, Alloc>, AccessType, UnpackT >
    {
      typedef viennagrid::dense_container_accessor<std::deque<T, Alloc>, AccessType, UnpackT> type;
    };

    template<typename T, typename Alloc, typename AccessType, typename UnpackT>
    struct accessor< const std::deque<T, Alloc>, AccessType, UnpackT >
    {
      typedef viennagrid::dense_container_accessor<const std::deque<T, Alloc>, AccessType, UnpackT> type;
    };

    template<typename Key, typename T, typename Compare, typename Alloc, typename AccessType, typename UnpackT>
    struct accessor< std::map<Key, T, Compare, Alloc>, AccessType, UnpackT >
    {
      typedef viennagrid::std_map_accessor<std::map<Key, T, Compare, Alloc>, AccessType, UnpackT> type;
    };

    template<typename Key, typename T, typename Compare, typename Alloc, typename AccessType, typename UnpackT>
    struct accessor< const std::map<Key, T, Compare, Alloc>, AccessType, UnpackT >
    {
      typedef viennagrid::std_map_accessor<const std::map<Key, T, Compare, Alloc>, AccessType, UnpackT> type;
    };
    /** \endcond */
  }


  /** @brief Convenience function for returning an accessor for the supplied container. Non-const version.
    *
    * @tparam AccessType      The element type for which data should be accessed (for example a vertex type)
    * @tparam ContainerType   The container type. Usually not supplied explicitly by the user.
    */
  template<typename AccessType, typename ContainerType>
  typename result_of::accessor<ContainerType, AccessType>::type make_accessor( ContainerType & container )
  {
    return typename result_of::accessor<ContainerType, AccessType>::type(container);
  }

  /** @brief Convenience function for returning an accessor for the supplied container. Const-version.
    *
    * @tparam AccessType      The element type for which data should be accessed (for example a vertex type)
    * @tparam ContainerType   The container type. Usually not supplied explicitly by the user.
    */
  template<typename AccessType, typename ContainerType>
  typename result_of::accessor<const ContainerType, AccessType>::type make_accessor( ContainerType const & container )
  {
    return typename result_of::accessor<const ContainerType, AccessType>::type(container);
  }







  /** @brief Base class for all dynamic accessor.
   *
   *  @tparam ValueType     The data type, e.g. double
   *  @tparam AccessType    The element for which data is accessed, e.g. a vertex
   */
  template<typename ValueType, typename AccessType>
  class base_dynamic_accessor
  {
  public:
    typedef ValueType  value_type;
    typedef AccessType access_type;

    typedef value_type       &       reference;
    typedef value_type const & const_reference;

    typedef value_type       *       pointer;
    typedef value_type const * const_pointer;

    virtual ~base_dynamic_accessor() {}

    virtual bool valid() const = 0;
    virtual bool valid(access_type const & element) const = 0;

    virtual value_type get(access_type const & element) const = 0;
    virtual void set(access_type const & element, value_type const & value) = 0;
  };




  /** @brief A dynamic accessor class which wraps any user accessor type fulfilling the accessor concept.
    *
    * This can be used for storing different accessors to the same element type for data with the same value type in a common container.
    */
  template<typename BaseAccessorT, typename AccessorType>
  class dynamic_accessor_wrapper : public BaseAccessorT
  {
  public:
    typedef typename BaseAccessorT::value_type value_type;
    typedef typename BaseAccessorT::access_type access_type;

    typedef typename BaseAccessorT::reference reference;
    typedef typename BaseAccessorT::const_reference const_reference;

    typedef typename BaseAccessorT::pointer pointer;
    typedef typename BaseAccessorT::const_pointer const_pointer;


    dynamic_accessor_wrapper(AccessorType accessor_) : accessor(accessor_) {}

    bool valid() const { return accessor.valid(); }
    bool valid(access_type const & element) const {return accessor.valid(element);}

    value_type get(access_type const & element) const { return accessor.get(element); }
    void set(access_type const & element, value_type const & value) { accessor.set(element, value); }

  private:
    AccessorType accessor;
  };

}


#endif
