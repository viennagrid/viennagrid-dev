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

#include "viennagrid/forwards.hpp"

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
    typename viennagrid::result_of::id<ElementT>::type operator()(ElementT const & element) const
    { return element.id(); }
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
      typedef typename AccessT::id_type type;
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

    bool is_valid() const { return container != NULL; }

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

    bool is_valid() const { return container != NULL; }


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

    virtual bool is_valid() const = 0;

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

    bool is_valid() const { return accessor.is_valid(); }

    value_type get(access_type const & element) const { return accessor.get(element); }
    void set(access_type const & element, value_type const & value) { accessor.set(element, value); }

  private:
    AccessorType accessor;
  };







  class quantity_field;

  class quantity_value
  {
    friend class quantity_field;

    quantity_value(viennagrid_numeric * values_, viennagrid_dimension dimension_) :
        values(values_), dimension(dimension_) {}

  public:

    operator viennagrid_numeric()
    {
      assert(dimension == 1);
      return *values;
    }

    operator std::vector<viennagrid_numeric>()
    {
      std::vector<viennagrid_numeric> tmp(dimension);
      std::copy( values, values + dimension, &tmp[0] );
      return tmp;
    }

  private:
    viennagrid_numeric * values;
    viennagrid_dimension dimension;
  };


  class quantity_field
  {
  public:

    quantity_field()
    {
      viennagrid_quantity_field_make(&internal_quantity_field);
    }

    quantity_field(viennagrid_quantity_field internal_quantity_field_) :
        internal_quantity_field(internal_quantity_field_) { retain(); }
    quantity_field(quantity_field const & rhs) : internal_quantity_field(rhs.internal()) { retain(); }

    ~quantity_field() { release(); }

    quantity_field & operator=(quantity_field const & rhs)
    {
      release();
      internal_quantity_field = rhs.internal();
      retain();
      return *this;
    }


    void set_to_null()
    {
      release();
      internal_quantity_field = NULL;
    }

    bool is_valid() const { return internal() != NULL; }


    typedef std::vector<viennagrid_numeric> value_type;

    template<typename ElementT>
    quantity_value get(ElementT const & element) const
    {
      assert( viennagrid::topologic_dimension(element) == internal()->topologic_dimension );

      viennagrid_numeric * tmp;
      viennagrid_quantities_get_value(internal(), element.id(), &tmp);

      return quantity_value(tmp, values_dimension());
    }

    template<bool element_is_const>
    void set(base_element<element_is_const> const & element, value_type const & value)
    {
      if ( topologic_dimension() < 0 )
        set_topologic_dimension( viennagrid::topologic_dimension(element) );
      assert( topologic_dimension() == viennagrid::topologic_dimension(element) );

      if (values_dimension() < 0)
        set_values_dimension( value.size() );

      assert( values_dimension() == static_cast<viennagrid_int>(value.size()) );

      set(element.id(), value);
    }

    template<bool element_is_const>
    void set(base_element<element_is_const> const & element, viennagrid_numeric value)
    {
      if ( topologic_dimension() < 0 )
        set_topologic_dimension( viennagrid::topologic_dimension(element) );
      assert( topologic_dimension() == viennagrid::topologic_dimension(element) );

      if (values_dimension() < 0)
        set_values_dimension(1);
      assert( values_dimension() == 1 );

      set(element.id(), value);
    }

    void set(viennagrid_index id, value_type const & value)
    {
      if (size() <= id)
        resize(id+1);
      viennagrid_quantities_set_value(internal(), id, const_cast<viennagrid_numeric*>(&value[0]));
    }

    void set(viennagrid_index id, viennagrid_numeric value)
    {
      if (size() <= id)
        resize(id+1);
      viennagrid_quantities_set_value(internal(), id, &value);
    }

    viennagrid_int size() const
    {
      viennagrid_int size_;
      viennagrid_quantities_size(internal(), &size_);
      return size_;
    }

    void resize(viennagrid_int size)
    {
      viennagrid_quantities_resize(internal(), size);
    }

    void set_topologic_dimension(viennagrid_dimension topologic_dimension_)
    {
      viennagrid_quantities_set_topologic_dimension(internal(), topologic_dimension_);
    }

    viennagrid_dimension topologic_dimension() const
    {
      viennagrid_dimension topologic_dimension_;
      viennagrid_quantities_get_topologic_dimension(internal(), &topologic_dimension_);
      return topologic_dimension_;
    }

    viennagrid_dimension values_dimension() const
    {
      viennagrid_dimension values_dimension_;
      viennagrid_quantities_get_values_dimension(internal(), &values_dimension_);
      return values_dimension_;
    }

    void set_values_dimension(std::size_t values_dimension_)
    {
      viennagrid_quantities_set_values_dimension(internal(), values_dimension_);
    }

    viennagrid_quantity_field internal() const { return internal_quantity_field; }

  private:

    void retain()
    {
      viennagrid_quantity_field_retain(internal());
    }

    void release()
    {
      viennagrid_quantity_field_release(internal());
    }

    viennagrid_quantity_field internal_quantity_field;
  };









}


#endif
