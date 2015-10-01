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
  viennagrid_quantity_field_t() : topological_dimension(-1), values_type(-1), values_per_quantity(-1), storage_layout(-1), reference_counter(1) {}

  std::string name;
  std::string unit;
  viennagrid_dimension topological_dimension;


  viennagrid_int values_type;
  viennagrid_int values_per_quantity;
  viennagrid_int storage_layout;

  viennagrid_int size_per_value() const
  {
    switch (values_type)
    {
      case VIENNAGRID_QUANTITY_FIELD_TYPE_BYTE:
        return sizeof(viennagrid_byte);
      case VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC:
        return sizeof(viennagrid_numeric);
    }

    return -1;
  }

  viennagrid_int size_per_quantity() const
  {
    return size_per_value() * values_per_quantity;
  }


  std::vector<char> dense_values;
  std::vector<bool> dense_valid_flags;

  std::map< viennagrid_int, char * > sparse_values;


  viennagrid_int size() const
  {
    switch ( storage_layout )
    {
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
        return dense_values.size() / size_per_quantity();
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
        return sparse_values.size();
    }

    return -1;
  }

  void resize(viennagrid_int quantity_count)
  {
    switch ( storage_layout )
    {
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
      {
        dense_values.resize( size_per_quantity() * quantity_count );
        dense_valid_flags.resize( quantity_count, false );
        break;
      }
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
        break;
      default:
        assert(false);
    }
  }

  void * get_or_create(viennagrid_int element_index)
  {
    void * dst = NULL;

    switch ( storage_layout )
    {
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
      {
        if ( element_index >= static_cast<viennagrid_int>(dense_valid_flags.size()) )
          resize(element_index+1);

        dst = &dense_values[0] + element_index*size_per_quantity();
        dense_valid_flags[element_index] = true;

        break;
      }

      case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
      {
        std::map< viennagrid_int, char* >::iterator it = sparse_values.find(element_index);
        if (it == sparse_values.end())
          it = sparse_values.insert( std::make_pair(element_index, new char[size_per_quantity()]) ).first;

        dst = (*it).second;

        break;
      }
    }

    return dst;
  }

  void set(viennagrid_int element_index, void const * quantity)
  {
    void * dst = get_or_create(element_index);

    if (quantity && dst)
      memcpy(dst, quantity, size_per_quantity());
  }

  void * get(viennagrid_int element_index)
  {
    switch ( storage_layout )
    {
      case VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE:
      {
        if ( (element_index < static_cast<viennagrid_int>(dense_valid_flags.size())) &&
            (dense_valid_flags[element_index]) )
        {
          return &dense_values[0] + element_index*size_per_quantity();
        }

        break;
      }

      case VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE:
      {
        std::map< viennagrid_int, char* >::iterator it = sparse_values.find(element_index);
        if (it != sparse_values.end())
        {
          return &(*it).second[0];
        }

        break;
      }
    }

    return NULL;
  }


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
  if (quantity_field)
    *quantity_field = new viennagrid_quantity_field_t;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_retain(viennagrid_quantity_field quantity_field)
{
  retain(quantity_field);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_release(viennagrid_quantity_field quantity_field)
{
  release(quantity_field);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_init(viennagrid_quantity_field quantity_field,
                                                viennagrid_dimension topological_dimension,
                                                viennagrid_int values_type,
                                                viennagrid_int values_per_quantity,
                                                viennagrid_int storage_layout)
{
  if (!quantity_field)                                                return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;
  if ((storage_layout != VIENNAGRID_QUANTITY_FIELD_STORAGE_DENSE) &&
      (storage_layout != VIENNAGRID_QUANTITY_FIELD_STORAGE_SPARSE)  ) return VIENNAGRID_ERROR_INVALID_STORAGE_LAYOUT;
  if ((values_type != VIENNAGRID_QUANTITY_FIELD_TYPE_BYTE) &&
      (values_type != VIENNAGRID_QUANTITY_FIELD_TYPE_NUMERIC))         return VIENNAGRID_ERROR_INVALID_QUANTITLY_VALUE_TYPE;
  if (values_per_quantity <= 0)                                       return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  if ( (topological_dimension == quantity_field->topological_dimension) &&
       (values_type == quantity_field->values_type) &&
       (values_per_quantity == quantity_field->values_per_quantity) &&
       (storage_layout == quantity_field->storage_layout) )
    return VIENNAGRID_SUCCESS;

  quantity_field->topological_dimension = topological_dimension;

  quantity_field->values_type = values_type;
  quantity_field->values_per_quantity = values_per_quantity;
  quantity_field->storage_layout = storage_layout;

  quantity_field->dense_values.clear();
  quantity_field->dense_valid_flags.clear();
  quantity_field->sparse_values.clear();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_topological_dimension_get(viennagrid_quantity_field quantity_field,
                                                                     viennagrid_dimension * topological_dimension)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (topological_dimension)
    *topological_dimension = quantity_field->topological_dimension;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_storage_layout_get(viennagrid_quantity_field quantity_field,
                                                              viennagrid_int * storage_layout)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (!storage_layout)
    *storage_layout  = quantity_field->storage_layout;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_values_type_get(viennagrid_quantity_field quantity_field,
                                                           viennagrid_int * values_type)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (values_type)
    *values_type = quantity_field->values_type;

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_values_per_quantity_get(viennagrid_quantity_field quantity_field,
                                                                   viennagrid_int * values_per_quantity)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (values_per_quantity)
    *values_per_quantity = quantity_field->values_per_quantity;

  return VIENNAGRID_SUCCESS;
}








viennagrid_error viennagrid_quantity_field_name_set(viennagrid_quantity_field quantity_field,
                                                    const char * name)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (!name)
    quantity_field->name.clear();
  else
    quantity_field->name = name;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_name_get(viennagrid_quantity_field quantity_field,
                                                    const char ** name)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (name)
    *name = quantity_field->name.c_str();

  return VIENNAGRID_SUCCESS;
}


viennagrid_error viennagrid_quantity_field_unit_set(viennagrid_quantity_field quantity_field,
                                                    const char * unit)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (!unit)
    quantity_field->unit.clear();
  else
    quantity_field->unit = unit;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_unit_get(viennagrid_quantity_field quantity_field,
                                                    const char ** unit)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (unit)
    *unit = quantity_field->unit.c_str();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_resize(viennagrid_quantity_field quantity_field,
                                                  viennagrid_int value_count)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;
  if (value_count < 0) return VIENNAGRID_ERROR_INVALID_ARGUMENTS;

  quantity_field->resize(value_count);

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_size(viennagrid_quantity_field quantity_field,
                                                viennagrid_int * value_count)
{
  if (!quantity_field) return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;

  if (value_count)
    *value_count = quantity_field->size();

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_value_get(viennagrid_quantity_field quantity_field,
                                                     viennagrid_int element_id,
                                                     void ** values)
{
  if (!quantity_field)       return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;
  if (INDEX(element_id) < 0) return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;

  if (values)
    *values = quantity_field->get(INDEX(element_id));

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_value_get_or_create(viennagrid_quantity_field quantity_field,
                                                               viennagrid_element_id element_id,
                                                               void ** values)
{
  if (!quantity_field)       return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;
  if (INDEX(element_id) < 0) return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;

  void * tmp = quantity_field->get_or_create(INDEX(element_id));
  if (values)
    *values = tmp;

  return VIENNAGRID_SUCCESS;
}

viennagrid_error viennagrid_quantity_field_value_set(viennagrid_quantity_field quantity_field,
                                                     viennagrid_int element_id,
                                                     void const * value)
{
  if (!quantity_field)       return VIENNAGRID_ERROR_INVALID_QUANTITY_FIELD;
  if (INDEX(element_id) < 0) return VIENNAGRID_ERROR_INVALID_ELEMENT_ID;

  quantity_field->set(INDEX(element_id), value);

  return VIENNAGRID_SUCCESS;
}



