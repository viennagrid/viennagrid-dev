#ifndef VIENNAGRID_QUANTITY_FIELD_HPP
#define VIENNAGRID_QUANTITY_FIELD_HPP

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

// #include "viennagridpp/forwards.hpp"
#include "viennagridpp/element/element.hpp"
#include "viennagrid/quantity_field.h"

/** @file viennagrid/accessor.hpp
    @brief Defines various accessors for storing data for the different element types.
*/


namespace viennagrid
{


  class quantity_value : public point_t
  {
  public:

    quantity_value() {}
    quantity_value(viennagrid_numeric * values_in, viennagrid_dimension dimension_in)
    {
      if (values_in)
      {
        resize(dimension_in);
        std::copy(values_in, values_in+dimension_in, begin());
      }
    }

    quantity_value(point_t const & pt) : point_t(pt) {}

    operator value_type() const
    {
      assert(valid());
      assert(size() == 1);
      return (*this)[0];
    }

    bool valid() const { return !empty(); }

  private:
  };


  class quantity_field
  {
  public:

    typedef quantity_value value_type;

    quantity_field()
    {
      viennagrid_quantity_field_make( &internal_quantity_field );
    }

    quantity_field(viennagrid_dimension topologic_dimension,
                   viennagrid_dimension values_dimension,
                   viennagrid_int storage_type = QUANTITY_FIELD_STORAGE_DENSE)
    {
      viennagrid_quantity_field_make( &internal_quantity_field );
      init(topologic_dimension, values_dimension, storage_type);
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


    void init(viennagrid_dimension topologic_dimension,
              viennagrid_dimension values_dimension,
              viennagrid_int storage_type = QUANTITY_FIELD_STORAGE_DENSE)
    {
      viennagrid_quantity_field_init(internal(), topologic_dimension, values_dimension, storage_type);
    }


    void set_to_null()
    {
      release();
      internal_quantity_field = NULL;
    }

    bool valid() const
    {
      return internal() != NULL;
    }

    bool is_initialized() const
    {
      return valid() && (topologic_dimension() >= 0) && (values_dimension() >= 1);
    }



    value_type get(viennagrid_int id) const
    {
      viennagrid_numeric * tmp;
      viennagrid_quantity_field_get_value(internal(), id, &tmp);

      return value_type(tmp, values_dimension());
    }

    template<bool element_is_const>
    value_type get(base_element<element_is_const> const & element) const
    {
      assert( viennagrid::topologic_dimension(element) == topologic_dimension() );
      return get(element.id());
    }

    bool valid(viennagrid_int id) const
    {
      return get(id).valid();
    }

    template<bool element_is_const>
    bool valid(base_element<element_is_const> const & element) const
    {
      return valid(element.id());
    }


    void set(viennagrid_int id, viennagrid_numeric const * value)
    {
      viennagrid_quantity_field_set_value(internal(), id, const_cast<viennagrid_numeric*>(value));
    }

    void set(viennagrid_int id, std::vector<viennagrid_numeric> const & value)
    {
      viennagrid_quantity_field_set_value(internal(), id, const_cast<viennagrid_numeric*>(&value[0]));
    }

//     void set(viennagrid_int id, value_type const & value)
//     {
//       viennagrid_quantity_field_set_value(internal(), id, value.values());
//     }

    void set(viennagrid_int id, viennagrid_numeric value)
    {
      viennagrid_quantity_field_set_value(internal(), id, &value);
    }

    template<bool element_is_const>
    void set(base_element<element_is_const> const & element, viennagrid_numeric const * value)
    {
      check_init(element, values_dimension());
      set(element.id(), value);
    }

    template<bool element_is_const>
    void set(base_element<element_is_const> const & element, std::vector<viennagrid_numeric> const & value)
    {
      check_init(element, value.size());
      set(element.id(), value);
    }

//     template<bool element_is_const>
//     void set(base_element<element_is_const> const & element, value_type const & value)
//     {
//       check_init(element, value.size());
//       set(element.id(), value);
//     }

    template<bool element_is_const>
    void set(base_element<element_is_const> const & element, viennagrid_numeric value)
    {
      check_init(element, 1);
      set(element.id(), value);
    }

    viennagrid_int size() const
    {
      viennagrid_int size_;
      viennagrid_quantity_field_size(internal(), &size_);
      return size_;
    }

    void resize(viennagrid_int size)
    {
      viennagrid_quantity_field_resize(internal(), size);
    }

    viennagrid_dimension topologic_dimension() const
    {
      viennagrid_dimension topologic_dimension_;
      viennagrid_quantity_field_get_topologic_dimension(internal(), &topologic_dimension_);
      return topologic_dimension_;
    }

    viennagrid_dimension values_dimension() const
    {
      viennagrid_dimension values_dimension_;
      viennagrid_quantity_field_get_values_dimension(internal(), &values_dimension_);
      return values_dimension_;
    }

    viennagrid_int storage_layout() const
    {
      viennagrid_int storage_layout_;
      viennagrid_quantity_field_get_storage_layout(internal(), &storage_layout_);
      return storage_layout_;
    }

    std::string name() const
    {
      assert(valid());

      const char * tmp;
      viennagrid_quantity_field_get_name(internal(), &tmp);
      if (tmp)
        return tmp;
      else
        return "";
    }

    void set_name(std::string const & name_)
    {
      assert(valid());

      if (name_.empty())
        viennagrid_quantity_field_set_name(internal(), NULL);

      viennagrid_quantity_field_set_name(internal(), name_.c_str());
    }


    std::string unit() const
    {
      assert(valid());

      const char * tmp;
      viennagrid_quantity_field_get_unit(internal(), &tmp);
      if (tmp)
        return tmp;
      else
        return "";
    }

    void set_unit(std::string const & unit_)
    {
      assert(valid());

      if (unit_.empty())
        viennagrid_quantity_field_set_unit(internal(), NULL);

      viennagrid_quantity_field_set_unit(internal(), unit_.c_str());
    }

    viennagrid_quantity_field internal() const { return internal_quantity_field; }

  private:

    template<bool element_is_const>
    void check_init(base_element<element_is_const> const & element, viennagrid_dimension size)
    {
      if (!is_initialized())
      {
        init( viennagrid::topologic_dimension(element), size, QUANTITY_FIELD_STORAGE_DENSE );
      }

      assert( topologic_dimension() == viennagrid::topologic_dimension(element) );
      assert( values_dimension() == size );
    }

    void retain()
    {
      if (internal())
        viennagrid_quantity_field_retain(internal());
    }

    void release()
    {
      if (internal())
        viennagrid_quantity_field_release(internal());
    }

    viennagrid_quantity_field internal_quantity_field;
  };

}


#endif
