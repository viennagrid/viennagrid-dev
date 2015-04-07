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

#include "viennagridpp/forwards.hpp"
#include "viennagridpp/element/element.hpp"

/** @file viennagrid/accessor.hpp
    @brief Defines various accessors for storing data for the different element types.
*/


namespace viennagrid
{
  class quantity_value : public point_t
  {
  public:

    quantity_value(viennagrid_numeric * values_, viennagrid_dimension dimension_)
    {
      resize(dimension_);
      std::copy( values_, values_+dimension_, begin() );
    }

    operator value_type() const
    {
      assert(size() == 1);
      return (*this)[0];
    }

  private:
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



    quantity_value get(viennagrid_index id) const
    {
      viennagrid_numeric * tmp;
      viennagrid_quantities_get_value(internal(), id, &tmp);

      return quantity_value(tmp, values_dimension());
    }

    template<bool element_is_const>
    quantity_value get(base_element<element_is_const> const & element) const
    {
      assert( viennagrid::topologic_dimension(element) == internal()->topologic_dimension );
      return get(element.id());
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


    std::string name() const
    {
      const char * tmp;
      viennagrid_quantities_get_name(internal(), &tmp);
      if (tmp)
        return tmp;
      else
        return "";
    }

    void set_name(std::string const & name_)
    {
      if (name_.empty())
        viennagrid_quantities_set_name(internal(), NULL);

      viennagrid_quantities_set_name(internal(), name_.c_str());
    }


    std::string unit() const
    {
      const char * tmp;
      viennagrid_quantities_get_unit(internal(), &tmp);
      if (tmp)
        return tmp;
      else
        return "";
    }

    void set_unit(std::string const & unit_)
    {
      if (unit_.empty())
        viennagrid_quantities_set_unit(internal(), NULL);

      viennagrid_quantities_set_unit(internal(), unit_.c_str());
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
