#include <algorithm>
#include <cstring>
#include <cassert>
#include <string>
#include <vector>
#include <map>

#include "viennagrid/viennagrid.h"
#include "common.hpp"



// quantities
struct viennagrid_quantity_field_t
{
  viennagrid_quantity_field_t() : topologic_dimension(-1), size_of_value(-1), storage_layout(-1), reference_counter(1) {}

  std::string name;
  std::string unit;
  viennagrid_dimension topologic_dimension;


  viennagrid_int size_of_value;
  viennagrid_int storage_layout;


  std::vector<char> dense_values;
  std::vector<bool> dense_valid_flags;

  std::map< viennagrid_int, char * > sparse_values;



  void clear()
  {
    for (std::map< viennagrid_int, char * >::iterator it = sparse_values.begin(); it != sparse_values.end(); ++it)
    {
      delete[] (*it).second;
    }
  }

public:
  viennagrid_int reference_counter;
};




viennagrid_error viennagrid_quantity_field_create(viennagrid_quantity_field * quantity_field)
{
  *quantity_field = new viennagrid_quantity_field_t;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_retain(viennagrid_quantity_field quantity_field)
{
  if (quantity_field)
    retain(quantity_field);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_release(viennagrid_quantity_field quantity_field)
{
  if (quantity_field)
    release(quantity_field);

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_init(viennagrid_quantity_field quantity_field,
                                                viennagrid_dimension topologic_dimension,
                                                viennagrid_int size_of_value,
                                                viennagrid_int storage_layout)
{
  if ( (topologic_dimension == quantity_field->topologic_dimension) &&
       (storage_layout == quantity_field->storage_layout) &&
       (size_of_value == quantity_field->size_of_value) )
    return VIENNAGRID_SUCCESS;

  if ( (storage_layout != VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE) && (storage_layout != VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE) )
    return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  quantity_field->topologic_dimension = topologic_dimension;

  quantity_field->size_of_value = size_of_value;
  quantity_field->storage_layout = storage_layout;

  quantity_field->dense_values.clear();
  quantity_field->dense_valid_flags.clear();
  quantity_field->sparse_values.clear();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_topologic_dimension_get(viennagrid_quantity_field quantity_field,
                                                                   viennagrid_dimension * topologic_dimension)
{
  *topologic_dimension = quantity_field->topologic_dimension;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_storage_layout_get(viennagrid_quantity_field quantity_field,
                                                              viennagrid_int * storage_layout)
{
  *storage_layout  = quantity_field->storage_layout;
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_size_of_value_get(viennagrid_quantity_field quantity_field,
                                                             viennagrid_int * size_of_value)
{
  *size_of_value = quantity_field->size_of_value;
  return VIENNAGRID_SUCCESS;
}








viennagrid_error viennagrid_quantity_field_name_set(viennagrid_quantity_field quantity_field,
                                                    const char * name)
{
  if (!name)
    quantity_field->name.clear();
  else
    quantity_field->name = name;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_name_get(viennagrid_quantity_field quantity_field,
                                                    const char ** name)
{
  if (name)
    *name = quantity_field->name.c_str();
  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_unit_set(viennagrid_quantity_field quantity_field,
                                                    const char * unit)
{
  if (!unit)
    quantity_field->unit.clear();
  else
    quantity_field->unit = unit;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_unit_get(viennagrid_quantity_field quantity_field,
                                                    const char ** unit)
{
  if (unit)
    *unit = quantity_field->unit.c_str();
  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_resize(viennagrid_quantity_field quantity_field,
                                              viennagrid_int value_count)
{
  switch ( quantity_field->storage_layout )
  {
    case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
    {
      quantity_field->dense_values.resize( quantity_field->size_of_value / value_count );
      quantity_field->dense_valid_flags.resize( value_count, false );
      break;
    }
    case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
      break;
    default:
      assert(false);
  }

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_size(viennagrid_quantity_field quantity_field,
                                            viennagrid_int * value_count)
{
  if (value_count)
  {
    switch ( quantity_field->storage_layout )
    {
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
        *value_count = quantity_field->dense_values.size() / quantity_field->size_of_value;
        break;
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
        *value_count = quantity_field->sparse_values.size();
        break;
      default:
        assert(false);
    }
  }

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_value_set(viennagrid_quantity_field quantity_field,
                                                     viennagrid_int element_id,
                                                     void * value)
{
  assert( 0 <= element_id );

  void * dst = NULL;

  switch ( quantity_field->storage_layout )
  {
    case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
    {
      if ( element_id >= static_cast<viennagrid_int>(quantity_field->dense_valid_flags.size()) )
        viennagrid_quantity_field_resize(quantity_field, element_id+1);

      dst = &quantity_field->dense_values[0] + element_id*quantity_field->size_of_value;
      quantity_field->dense_valid_flags[element_id] = true;

      break;
    }

    case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
    {
      std::map< viennagrid_int, char* >::iterator it = quantity_field->sparse_values.find(element_id);
      if (it == quantity_field->sparse_values.end())
        it = quantity_field->sparse_values.insert( std::make_pair(element_id, new char[quantity_field->size_of_value]) ).first;

      dst = (*it).second;

      break;
    }

    default:
      assert(false);
  }

  memcpy(dst, value, quantity_field->size_of_value);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_value_get(viennagrid_quantity_field quantity_field,
                                                     viennagrid_int element_id,
                                                     void ** values)
{
  assert( 0 <= element_id );

  switch ( quantity_field->storage_layout )
  {
    case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
    {
      if ( (element_id < static_cast<viennagrid_int>(quantity_field->dense_valid_flags.size())) &&
           (quantity_field->dense_valid_flags[element_id]) )
      {
        *values = &quantity_field->dense_values[0] + element_id*quantity_field->size_of_value;
      }
      else
      {
        *values = NULL;
      }
      break;
    }

    case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
    {
      std::map< viennagrid_int, char* >::iterator it = quantity_field->sparse_values.find(element_id);
      if (it != quantity_field->sparse_values.end())
      {
        *values = &(*it).second[0];
      }
      else
      {
        *values = NULL;
      }
      break;
    }

    default:
      assert(false);
  }

  return VIENNAGRID_SUCCESS;
}

